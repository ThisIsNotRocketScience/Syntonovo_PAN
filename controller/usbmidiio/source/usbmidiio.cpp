#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "LPC54616.h"
/* TODO: insert other include files here. */
#include "fsl_power.h"
#include "control.h"
#include "rpi.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "paramlist.h"
#include <math.h>
#include <stdlib.h>
#include "rpi.h"
#include "../../Raspberry/PanPreset.h"
#include "../../Raspberry/FinalPanEnums.h"
#include "Arpeggiator.h"
#include "midicmd.h"

/* TODO: insert other definitions and declarations here. */

#define CMD_PAD_ZERO		(0x11)
#define CMD_CALIBRATE		(0x12)
#define CMD_PRESET_LOAD		(0x21)
#define CMD_PRESET_STORE	(0x22)

#define OOB_UI_PAUSE		(0x41)
#define OOB_UI_CONTINUE		(0x42)
#define OOB_BUTTON_DOWN		(0x50)
#define OOB_BUTTON_UP		(0x51)
#define OOB_ENCODER_CCW		(0x56)
#define OOB_ENCODER_CW		(0x57)
#define OOB_ENCODER_DOWN	(0x58)
#define OOB_ENCODER_UP		(0x59)
#define OOB_SWITCH_CHANGE	(0x5F)
#define OOB_NOTEON			(0x5C)
#define OOB_NOTEOFF			(0x5D)

#define OOB_AUTOTUNE_STATE	(0x60)

void arpclock_run();
void arpclock_reset();
void arpclock_set_speed(float bpm);

uint32_t timer_value();

void preset_init();
void flash_savepreset(int presetid, PanPreset_t* preset);
int flash_loadpreset(int presetid, PanPreset_t* preset);
void sync_preset_full();

PanPreset_t preset;
PanLedState_t ledstate;

uart_t rpi_uart;
sync_state_t rpi_sync;

volatile int loading = 0;

#define NOTEON(zone, note, velocity) dsp_cmd(0xfc, 2 | ((zone&3) << 4), note | (velocity << 8))
#define NOTEOFF(zone, note) dsp_cmd(0xfc, 1 | ((zone&3) << 4), note)

#define SET_KEYPRIORITY(zone, priority) dsp_cmd(0xfe, 0xfc, (((zone)&3) << 3) | (priority&7));

void dsp_reset()
{
	uint8_t data[4];
	data[0] = 0xff;
	data[1] = 0xff;
	data[2] = 0xff;
	data[3] = 0xff;

	control_write(data, 4);
	control_write(data, 4);
	control_write(data, 4);
	control_write(data, 4);
}

void dsp_calibrate()
{
	uint8_t data[4];

	data[0] = 0xfe;
	data[1] = 0xfe;
	data[2] = 0;
	data[3] = 0;

	control_write(data, 4);
}

void dsp_pad_zero()
{
	uint8_t data[4];

	data[0] = 0xfe;
	data[1] = 0xfb;
	data[2] = 0;
	data[3] = 0;

	control_write(data, 4);
}

void dsp_cmd(int param, int subparam, uint16_t value)
{
	uint8_t data[4];

	data[0] = param;
	data[1] = subparam;
	data[2] = value >> 8;
	data[3] = value & 0xFF;

	control_write(data, 4);
	//rpi_write(data, 4);
}

class DirectSynth : public Synth {
public:
	void Init(int zone) {
		zone_ = zone;
	}

	virtual void NoteOn(int note, int vel) {
		if (vel == 0) {
			NOTEOFF(zone_, note);
			return;
		}
		NOTEON(zone_, note, vel);
	}
	virtual void NoteOff(int note, int vel) {
		NOTEOFF(zone_, note);
	}
	virtual void Panic() {
		// setting key priority resets playing notes
		SET_KEYPRIORITY(zone_, preset.keyzone[zone_].keypriority);
	};
	virtual void Start() {};
	virtual void Stop() { Panic(); };

	int zone_;
};

DirectSynth direct[4];
ArpeggioState arp[4];

typedef enum {
	key_source_midi,
	key_source_usb,
	key_source_kb
} key_source_t;

void key_up(key_source_t source, int channel, int key);

void key_down(key_source_t source, int channel, int key, int vel)
{
	if (vel == 0) {
		key_up(source, channel, key);
		return;
	}

	uint8_t data[4] = {0};
	data[0] = source;
	data[1] = channel;
	data[2] = key;
	data[3] = vel;
	sync_oob_word(&rpi_sync, OOB_NOTEON, *(uint32_t*)data, 0);

	int fieldbase = 0;
	if (source == key_source_midi) fieldbase = 0;
	else if (source == key_source_usb) fieldbase = 4;
	else if (source == key_source_kb) fieldbase = 8;

	for (int zone = 0; zone < 4; zone++) {
		int field = fieldbase + zone;
		if (key >= preset.key_input[field].rangelo && key <= preset.key_input[field].rangehi) {
			int playkey = key + preset.key_input[field].transpose;
			if (playkey < 0) playkey = 0;
			else if (playkey > 0x7f) playkey = 0x7f;

			switch (preset.keyzone[zone].type) {
			case PresetKeyzoneType_Single:
			case PresetKeyzoneType_Paraphonic:
				direct[zone].NoteOn(playkey, vel);
				break;
			case PresetKeyzoneType_Arpeggiator:
				{
					if (preset.keyzone[zone].arpsettings.ResetTimeOnFirstKey) {
						if (!arp[0].NotesOn && !arp[1].NotesOn && !arp[2].NotesOn && !arp[3].NotesOn) {
							arpclock_reset();
						}
					}
					arp[zone].NoteOn(playkey, vel);
					break;
				}
			}
		}
	}
}

void key_up(key_source_t source, int channel, int key)
{
	uint8_t data[4] = {0};
	data[0] = source;
	data[1] = channel;
	data[2] = key;
	data[3] = 0;
	sync_oob_word(&rpi_sync, OOB_NOTEOFF, *(uint32_t*)data, 0);

	int fieldbase = 0;
	if (source == key_source_midi) fieldbase = 0;
	else if (source == key_source_usb) fieldbase = 4;
	else if (source == key_source_kb) fieldbase = 8;

	for (int zone = 0; zone < 4; zone++) {
		int field = fieldbase + zone;
		if (key >= preset.key_input[field].rangelo && key <= preset.key_input[field].rangehi) {
			int playkey = key + preset.key_input[field].transpose;
			if (playkey < 0) playkey = 0;
			else if (playkey > 0x7f) playkey = 0x7f;

			switch (preset.keyzone[zone].type) {
			case PresetKeyzoneType_Single:
			case PresetKeyzoneType_Paraphonic:
				direct[zone].NoteOff(playkey, 0);
				break;
			case PresetKeyzoneType_Arpeggiator:
				arp[zone].NoteOff(playkey, 0);
				break;
			}
		}
	}
}

BaseType_t keys_press(int keyindex, uint32_t timediff)
{
	key_down(key_source_kb, 0, keyindex, 0x7f);
	return pdFALSE;
}

BaseType_t keys_release(int keyindex)
{
	key_up(key_source_kb, 0, keyindex);
	return pdFALSE;
}

void L2ClkOn()
{
	BOARD_INITPINS_LD2CLK_GPIO->W[BOARD_INITPINS_LD2CLK_PORT][BOARD_INITPINS_LD2CLK_PIN] = 1;
}

void L2ClkOff()
{
	BOARD_INITPINS_LD2CLK_GPIO->W[BOARD_INITPINS_LD2CLK_PORT][BOARD_INITPINS_LD2CLK_PIN] = 0;
}

void L2DatOn()
{
	BOARD_INITPINS_LD2DATA_GPIO->W[BOARD_INITPINS_LD2DATA_PORT][BOARD_INITPINS_LD2DATA_PIN] = 1;
}

void L2DatOff()
{
	BOARD_INITPINS_LD2DATA_GPIO->W[BOARD_INITPINS_LD2DATA_PORT][BOARD_INITPINS_LD2DATA_PIN] = 0;
}


void L2LatchOn()
{
	BOARD_INITPINS_LD2XLAT_GPIO->W[BOARD_INITPINS_LD2XLAT_PORT][BOARD_INITPINS_LD2XLAT_PIN] = 1;
}

void L2LatchOff()
{
	BOARD_INITPINS_LD2XLAT_GPIO->W[BOARD_INITPINS_LD2XLAT_PORT][BOARD_INITPINS_LD2XLAT_PIN] = 0;
}



void L1ClkOn()
{
	BOARD_INITPINS_LD1CLK_GPIO->W[BOARD_INITPINS_LD1CLK_PORT][BOARD_INITPINS_LD1CLK_PIN] = 1;
}

void L1ClkOff()
{
	BOARD_INITPINS_LD1CLK_GPIO->W[BOARD_INITPINS_LD1CLK_PORT][BOARD_INITPINS_LD1CLK_PIN] = 0;
}

void L1DatOn()
{
	BOARD_INITPINS_LD1DATA_GPIO->W[BOARD_INITPINS_LD1DATA_PORT][BOARD_INITPINS_LD1DATA_PIN] = 1;
}

void L1DatOff()
{
	BOARD_INITPINS_LD1DATA_GPIO->W[BOARD_INITPINS_LD1DATA_PORT][BOARD_INITPINS_LD1DATA_PIN] = 0;
}


void L1LatchOn()
{
	BOARD_INITPINS_LD1XLAT_GPIO->W[BOARD_INITPINS_LD1XLAT_PORT][BOARD_INITPINS_LD1XLAT_PIN] = 1;
}

void L1LatchOff()
{
	BOARD_INITPINS_LD1XLAT_GPIO->W[BOARD_INITPINS_LD1XLAT_PORT][BOARD_INITPINS_LD1XLAT_PIN] = 0;
}

void L1BlankOff()
{
	BOARD_INITPINS_LD1BLANK_GPIO->W[BOARD_INITPINS_LD1BLANK_PORT][BOARD_INITPINS_LD1BLANK_PIN] = 0;
}
void L1BlankOn()
{
	BOARD_INITPINS_LD1BLANK_GPIO->W[BOARD_INITPINS_LD1BLANK_PORT][BOARD_INITPINS_LD1BLANK_PIN] = 1;
}

void L2BlankOff()
{
	BOARD_INITPINS_LD2BLANK_GPIO->W[BOARD_INITPINS_LD2BLANK_PORT][BOARD_INITPINS_LD2BLANK_PIN] = 0;
}
void L2BlankOn()
{
	BOARD_INITPINS_LD2BLANK_GPIO->W[BOARD_INITPINS_LD2BLANK_PORT][BOARD_INITPINS_LD2BLANK_PIN] = 1;
}

uint8_t leddata1[(24*12 + 16 + 24*12 + 16) / 8] = {0};
uint8_t leddata2[(24*12 + 16 + 24*12 + 16) / 8] = {0};

int encoderindex[__FINALENCODER_COUNT] = {0};
int buttonindex[__FINALLEDBUTTON_COUNT] = {0};

void Set12b(int bitindex, int value)
{
	uint8_t* ptr;
	if (bitindex >= 608) {
		bitindex -= 608;
		int arrayindex = 75 - bitindex / 8 - 1;
		ptr = &leddata2[arrayindex];
	}
	else {
		int arrayindex = 75 - bitindex / 8 - 1;
		ptr = &leddata1[arrayindex];
	}
	if (bitindex & 4) {
		*ptr = (value >> 4) & 0xFF;
		ptr++;
		*ptr = (*ptr & 0x0F) | ((value << 4) & 0xF0);
	}
	else {
		*ptr = (*ptr & 0xF0) | ((value >> 8) & 0x0F);
		ptr++;
		*ptr = value & 0xFF;
	}
}

void Set1b(int bitindex, int value)
{
	uint8_t* ptr;
	if (bitindex >= 608) {
		bitindex -= 608;
		int arrayindex = 75 - bitindex / 8;
		ptr = &leddata2[arrayindex];
	}
	else {
		int arrayindex = 75 - bitindex / 8;
		ptr = &leddata1[arrayindex];
	}
	int off = bitindex & 7;
	if (value) {
		*ptr |= 1 << off;
	} else {
		*ptr &= ~(1 << off);
	}
}

#define ENCODER(BUS, NAME) if (BUS == 0) { encoderindex[encoder_##NAME] = index1; index1 += 12*3; } else { encoderindex[encoder_##NAME] = index2; index2 += 12*3; }
#define LEDBUTTON(BUS, NAME) if (BUS == 0) { buttonindex[ledbutton_##NAME] = index1; index1 += 1; } else { buttonindex[ledbutton_##NAME] = index2; index2 += 1; }
#define UNUSEDENCODER(BUS) if (BUS == 0) { index1 += 12*3; } else { index2 += 12*3; }
#define UNUSED(BUS) if (BUS == 0) { index1++; } else { index2++; }
void setupleds()
{
	int index1 = 608;
	int index2 = 0;

#ifdef OLDSYNTH
	ENCODER(0, VCO4);
	ENCODER(0, VCO5);
	ENCODER(0, VCO6);
	ENCODER(0, VCO7);
	ENCODER(0, VCO8); // dn
	ENCODER(0, VCO3);
	ENCODER(0, VCO2);
	ENCODER(0, VCO1);

	UNUSED(0);
	LEDBUTTON(0, L1);
	LEDBUTTON(0, L2);
	LEDBUTTON(0, L3);
	LEDBUTTON(0, L4);
	LEDBUTTON(0, L5);
	LEDBUTTON(0, L6);
	LEDBUTTON(0, L7);
	LEDBUTTON(0, R7);
	LEDBUTTON(0, R6);
	LEDBUTTON(0, R5);
	LEDBUTTON(0, R4);
	LEDBUTTON(0, R3);
	LEDBUTTON(0, R2);
	LEDBUTTON(0, R1);
	UNUSED(0);
	LEDBUTTON(0, B11);
	LEDBUTTON(0, B12);
	LEDBUTTON(0, B13);
	LEDBUTTON(0, B14);
	LEDBUTTON(0, B15);
	LEDBUTTON(0, B16);
	LEDBUTTON(0, OctDownRight);
	LEDBUTTON(0, OctUpRight);
	LEDBUTTON(0, BankRight);
	LEDBUTTON(0, PortamentoRight);
	LEDBUTTON(0, RSus);
	LEDBUTTON(0, RUna);
	LEDBUTTON(0, RMod);
	LEDBUTTON(0, ArpEnable);
	LEDBUTTON(0, ArpFreeze);
	LEDBUTTON(0, ArpEdit);

	ENCODER(0, VCF1Freq);
	ENCODER(0, VCF1Mix);
	ENCODER(0, VCF2Mix);
	ENCODER(0, Cleanmix);
	ENCODER(0, VCF2d);
	ENCODER(0, VCF2c);
	ENCODER(0, VCF2b);
	ENCODER(0, VCF2a);

	LEDBUTTON(1, BHome);
	UNUSED(1);//LEDBUTTON(1, Lseq2);
	UNUSED(1);//LEDBUTTON(1, Lseq1);
	LEDBUTTON(1, LSus);
	LEDBUTTON(1, LUna);
	LEDBUTTON(1, LMod);
	LEDBUTTON(1, PortamentoLeft);
	LEDBUTTON(1, OctDownLeft);
	LEDBUTTON(1, B6);
	LEDBUTTON(1, B5);
	LEDBUTTON(1, B4);
	LEDBUTTON(1, B3);
	LEDBUTTON(1, B2);
	LEDBUTTON(1, B1);
	LEDBUTTON(1, BankLeft);
	LEDBUTTON(1, OctUpLeft);

	ENCODER(1, SketchLeft);
	ENCODER(1, MasteroutHeadphone);
	ENCODER(1, F2);
	ENCODER(1, F4);
	ENCODER(1, F6);
	ENCODER(1, F5);
	ENCODER(1, F3);
	ENCODER(1, F1);

	LEDBUTTON(1, B7);
	LEDBUTTON(1, B8);
	LEDBUTTON(1, BX);
	LEDBUTTON(1, BY);
	LEDBUTTON(1, BZ);
	LEDBUTTON(1, BVelocity);
	UNUSED(1);
	UNUSED(1);
	UNUSED(1);
	UNUSED(1);
	LEDBUTTON(1, B9);
	LEDBUTTON(1, B10);
	LEDBUTTON(1, BEnv);
	LEDBUTTON(1, BLFO);
	LEDBUTTON(1, BCV);
	LEDBUTTON(1, BTouch);

	ENCODER(1, F9);
	ENCODER(1, F7);
	ENCODER(1, F8);
	ENCODER(1, F10);
	UNUSEDENCODER(1); //ENCODER(1, FExt);
	ENCODER(1, Masterout);
	ENCODER(1, SketchRight);
	ENCODER(1, F11);
#else
	ENCODER(0, VCO4);
	ENCODER(0, VCO5);
	ENCODER(0, VCO6);
	ENCODER(0, VCO7);
	ENCODER(0, VCO8); // dn
	ENCODER(0, VCO3);
	ENCODER(0, VCO2);
	ENCODER(0, VCO1);

	LEDBUTTON(0, L1);
	LEDBUTTON(0, L2);
	LEDBUTTON(0, L3);
	LEDBUTTON(0, L4);
	LEDBUTTON(0, L5);
	LEDBUTTON(0, L6);
	LEDBUTTON(0, L7);
	LEDBUTTON(0, L8);
	LEDBUTTON(0, R8);
	LEDBUTTON(0, R7);
	LEDBUTTON(0, R6);
	LEDBUTTON(0, R5);
	LEDBUTTON(0, R4);
	LEDBUTTON(0, R3);
	LEDBUTTON(0, R2);
	LEDBUTTON(0, R1);
	LEDBUTTON(0, B11);
	LEDBUTTON(0, B12);
	LEDBUTTON(0, B13);
	LEDBUTTON(0, B14);
	LEDBUTTON(0, B15);
	LEDBUTTON(0, B16);
	LEDBUTTON(0, OctUpRight);
	LEDBUTTON(0, OctDownRight);
	LEDBUTTON(0, BankRight);
	LEDBUTTON(0, PortamentoRight);
	LEDBUTTON(0, RSus);
	LEDBUTTON(0, RUna);
	LEDBUTTON(0, RMod);
	LEDBUTTON(0, ArpEnable);
	LEDBUTTON(0, ArpFreeze);
	LEDBUTTON(0, ArpEdit);

	ENCODER(0, VCF1Freq);
	ENCODER(0, VCF1Mix);
	ENCODER(0, VCF2Mix);
	ENCODER(0, Cleanmix);
	ENCODER(0, VCF2d);
	ENCODER(0, VCF2c);
	ENCODER(0, VCF2b);
	ENCODER(0, VCF2a);

	LEDBUTTON(1, BHome);
	UNUSED(1);//LEDBUTTON(1, Lseq2);
	UNUSED(1);//LEDBUTTON(1, Lseq1);
	LEDBUTTON(1, LSus);
	LEDBUTTON(1, LUna);
	LEDBUTTON(1, LMod);
	LEDBUTTON(1, PortamentoLeft);
	LEDBUTTON(1, OctDownLeft);
	LEDBUTTON(1, B6);
	LEDBUTTON(1, B5);
	LEDBUTTON(1, B4);
	LEDBUTTON(1, B3);
	LEDBUTTON(1, B2);
	LEDBUTTON(1, B1);
	LEDBUTTON(1, BankLeft);
	LEDBUTTON(1, OctUpLeft);

//	ENCODER(1, LedSwitch); //???? SW65
	UNUSEDENCODER(1); //ENCODER(1, FExt);

//	ENCODER(1, SketchLeft);
	ENCODER(1, MasteroutHeadphone);
	ENCODER(1, F2);
	ENCODER(1, F4);
	ENCODER(1, F6);
	ENCODER(1, F5);
	ENCODER(1, F3);
	ENCODER(1, F1);

	LEDBUTTON(1, B7);
	LEDBUTTON(1, B8);
	LEDBUTTON(1, BX);
	LEDBUTTON(1, BY);
	LEDBUTTON(1, BZ);
	LEDBUTTON(1, BVelocity);
	UNUSED(1); // Toucher
	LEDBUTTON(1, BCV);
	LEDBUTTON(1, R9);
	LEDBUTTON(1, R10);
	LEDBUTTON(1, L10);
	LEDBUTTON(1, L9);
	LEDBUTTON(1, B9);
	LEDBUTTON(1, B10);
	LEDBUTTON(1, BEnv);
	LEDBUTTON(1, BLFO);

	ENCODER(1, F9);
	ENCODER(1, F7);
	ENCODER(1, F8);
	ENCODER(1, F10);
	UNUSEDENCODER(1); //ENCODER(1, FExt);
	ENCODER(1, Masterout);
	ENCODER(1, SketchRight);
	ENCODER(1, F11);
#endif
}

#undef ENCODER
#undef LEDBUTTON

void SendLeds()
{
	for (int i = 0; i < __FINALENCODER_COUNT; i++) {
		Set12b(encoderindex[i], ledstate.encoders[i].b >> 4);
		Set12b(encoderindex[i] + 12, ledstate.encoders[i].g >> 4);
		Set12b(encoderindex[i] + 24, ledstate.encoders[i].r >> 4);
	}

	for (int i = 0; i < __FINALLEDBUTTON_COUNT; i++) {
		Set1b(buttonindex[i], ledstate.ledbuttons[i].mode != ledmode_off);
	}

	for (int i = 0; i < sizeof(leddata1); i++) {
		for (int b = 7; b >= 0; b--) {
			L1ClkOff();
			L2ClkOff();
			__NOP();
			__NOP();
			__NOP();
			__NOP();
			__NOP();
			if (leddata1[i] & (1 << b)) {
				L1DatOn();
			}
			else {
				L1DatOff();
			}
			if (leddata2[i] & (1 << b)) {
				L2DatOn();
			}
			else {
				L2DatOff();
			}
			__NOP();
			__NOP();
			__NOP();
			__NOP();
			__NOP();
			L1ClkOn();
			L2ClkOn();
			__NOP();
			__NOP();
			__NOP();
			__NOP();
			__NOP();
		}
	}

	L1LatchOn();
	L2LatchOn();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	L1LatchOff();
	L2LatchOff();
}


void SW1ClkOn()
{
	BOARD_INITPINS_SW1CLK_GPIO->W[BOARD_INITPINS_SW1CLK_PORT][BOARD_INITPINS_SW1CLK_PIN] = 1;
}

void SW1ClkOff()
{
	BOARD_INITPINS_SW1CLK_GPIO->W[BOARD_INITPINS_SW1CLK_PORT][BOARD_INITPINS_SW1CLK_PIN] = 0;
}

void SW2ClkOn()
{
	BOARD_INITPINS_SW2CLK_GPIO->W[BOARD_INITPINS_SW2CLK_PORT][BOARD_INITPINS_SW2CLK_PIN] = 1;
}

void SW2ClkOff()
{
	BOARD_INITPINS_SW2CLK_GPIO->W[BOARD_INITPINS_SW2CLK_PORT][BOARD_INITPINS_SW2CLK_PIN] = 0;
}


void SW1LatchOn()
{
	BOARD_INITPINS_SW1LATCH_GPIO->W[BOARD_INITPINS_SW1LATCH_PORT][BOARD_INITPINS_SW1LATCH_PIN] = 1;
}

void SW1LatchOff()
{
	BOARD_INITPINS_SW1LATCH_GPIO->W[BOARD_INITPINS_SW1LATCH_PORT][BOARD_INITPINS_SW1LATCH_PIN] = 0;
}

void SW2LatchOn()
{
	BOARD_INITPINS_SW2LATCH_GPIO->W[BOARD_INITPINS_SW2LATCH_PORT][BOARD_INITPINS_SW2LATCH_PIN] = 1;
}

void SW2LatchOff()
{
	BOARD_INITPINS_SW2LATCH_GPIO->W[BOARD_INITPINS_SW2LATCH_PORT][BOARD_INITPINS_SW2LATCH_PIN] = 0;
}

int SW1Data()
{
	return BOARD_INITPINS_SW1DATA_GPIO->B[BOARD_INITPINS_SW1DATA_PORT][BOARD_INITPINS_SW1DATA_PIN];
}

int SW2Data()
{
	return BOARD_INITPINS_SW2DATA_GPIO->B[BOARD_INITPINS_SW2DATA_PORT][BOARD_INITPINS_SW2DATA_PIN];
}

enum LedButton_t
{
#define LEDBUTTON(name,x,y,id,str,r,g,b)  _ledbutton_##name,
#include "../../../Raspberry/FinalPanHeader.h"
#undef LEDBUTTON
	__ledbutton_Count
};

enum Encoder_t
{
#define LEDENCODER(name,x,y,str)  _encoder_##name,
#include "../../../Raspberry/FinalPanHeader.h"
#undef LEDENCODER
	__encoder_Count
};

int switch_sw[__ledbutton_Count];
int encoder_sw[__encoder_Count];
int encoder_a[__encoder_Count];
int encoder_c[__encoder_Count];

#define PINMANGLE(PINID) (((PINID)/80)*80+(79-((PINID)%80)))

void buttonmap_init()
{
#define DEFINESWITCH(PINID, dummy, NAME) switch_sw[_ledbutton_##NAME] = PINMANGLE(PINID);
#include "switches.h"
#undef DEFINESWITCH

#define ENCODER_sw(PINID, NAME) encoder_sw[_encoder_##NAME] = PINMANGLE(PINID);
#define ENCODER_a(PINID, NAME) encoder_a[_encoder_##NAME] = PINMANGLE(PINID);
#define ENCODER_c(PINID, NAME) encoder_c[_encoder_##NAME] = PINMANGLE(PINID);
#define DEFINEENCODER(PINID, PIN, NAME) ENCODER_##PIN(PINID, NAME)
#include "switches.h"
#undef ENCODER_sw
#undef ENCODER_a
#undef ENCODER_c
#undef DEFINEENCODER
}

#define ENCODER_DEBOUNCE_COUNT 2

uint8_t switch_state[__ledbutton_Count] = {0};
uint8_t encoder_sw_state[__encoder_Count] = {0};
uint8_t encoder_state[__encoder_Count] = {0};
uint8_t encoder_debounce_state[__encoder_Count * ENCODER_DEBOUNCE_COUNT] = {0};

int switch_process(uint8_t* state, int swstate)
{
	int r = 0;

	if (swstate == 0 && *state) r = -1;
	if (swstate == 1 && !*state) r = 1;

	*state = swstate;
	return r;
}

int encoder_debounce(int encid, int input)
{
	int ok = 1;
	uint8_t* state = &encoder_debounce_state[encid * ENCODER_DEBOUNCE_COUNT];
	for (int i = 0; i < ENCODER_DEBOUNCE_COUNT - 1; i++) {
			if (input != state[i]) {
					ok = 0;
			}
			state[i] = state[i + 1];
	}
	if (input != state[ENCODER_DEBOUNCE_COUNT - 1]) {
			ok = 0;
	}
	state[ENCODER_DEBOUNCE_COUNT - 1] = input;

	return ok;
}

int encoder_time[__encoder_Count] = {0};

uint32_t cur_encoder_time = 0;

int speedup(int encid, int dir)
{
	uint32_t diff = (cur_encoder_time - encoder_time[encid]) >> 11;

	encoder_time[encid] = cur_encoder_time;

	if (diff < 1) diff = 1;
	int mul = (1<<19) / diff;
	if (mul < 150) mul = 1;

	mul = (mul * mul) >> 11;
	if (mul < 1) mul = 1;

	int value = dir * mul;
	if (value < -0x800) value = -0x800;
	if (value > 0x800) value = 0x800;

	return value;
}

int encoder_process(int encid, int astate, int cstate)
{
	int encoder_shift[] = {
			 0,  1, -1,  0,
			-1,  0,  0,  1,
			 1,  0,  0, -1,
			 0, -1,  1,  0
	};
	int prev_encoder_state = encoder_state[encid] & 0x0f;
	int prev_encoder_value = (encoder_state[encid] >> 4) & 3;
	int cur_encoder_value = (encoder_state[encid] >> 6) & 3;
	int new_encoder_state = (astate & 1) | ((cstate & 1) << 1);
	if (!encoder_debounce(encid, new_encoder_state)) {
		return 0;
	}

	if (new_encoder_state != prev_encoder_state) {
			int dir = encoder_shift[(prev_encoder_state << 2) | new_encoder_state];

			cur_encoder_value = (cur_encoder_value + dir) & 3;

			if (((cur_encoder_value - prev_encoder_value) & 3) == 2) {
				prev_encoder_value = cur_encoder_value;

				if (astate == 1) {
					dir = 0;
				}
			}
			else {
				dir = 0;
			}

			encoder_state[encid] = new_encoder_state | (prev_encoder_value << 4) | (cur_encoder_value << 6);

			//if (dir) printf("%d -> %d\n", encid, dir);

			return speedup(encid, dir);
	}

	return 0;
}

uint8_t sw[80 * 2];
uint8_t last_sw[80 * 2];

void ScanButtonsAndEncoders()
{
	SW1LatchOff();
	SW2LatchOff();

	memset(sw, 0, sizeof(sw));

	SW1LatchOn();
	SW2LatchOn();

	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();

	for (int i = 0; i < 80; i++) {
		sw[i] = SW1Data();
		sw[i + 80] = SW2Data();
		SW1ClkOff();
		SW2ClkOff();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		SW1ClkOn();
		SW2ClkOn();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
	}

	uint8_t data[4] = {0};
/*
	for (int i = 0; i < 160; i++) {
		if (sw[i] != last_sw[i]) {
			printf("sw %d\n", PINMANGLE(i));
			data[0] = PINMANGLE(i);
			sync_oob_word(&rpi_sync, OOB_SWITCH_CHANGE, *(uint32_t*)data, 0);
		}
	}
	memcpy(last_sw, sw, 160);
*/
	for (int i = 0; i < __ledbutton_Count; i++) {
		if (switch_sw[i] == 0) {
			// sw[0] is bound to an encoder so we can easily skip unbound buttons
			continue;
		}

		int move = switch_process(&switch_state[i], sw[switch_sw[i]]);

		if (move > 0) {
			//printf("sw %d (%d) up\n", i, switch_sw[i]);
			data[0] = i;
			sync_oob_word(&rpi_sync, OOB_BUTTON_UP, *(uint32_t*)data, 0);
		}
		else if (move < 0) {
			//printf("sw %d (%d) down\n", i, switch_sw[i]);
			data[0] = i;
			sync_oob_word(&rpi_sync, OOB_BUTTON_DOWN, *(uint32_t*)data, 0);
		}
	}

	cur_encoder_time = timer_value();

	for (int i = 0; i < __encoder_Count; i++) {
#ifndef OLDSYNTH
		if (i == 18 || i == 19) continue;
#endif

		int move = encoder_process(i, sw[encoder_a[i]], sw[encoder_c[i]]);

		if (move < 0) {
			data[0] = i;
			move = -move;
			data[1] = move & 0xFF;
			data[2] = (move >> 8) & 0xFF;
			sync_oob_word(&rpi_sync, OOB_ENCODER_CCW, *(uint32_t*)data, 0);
		}
		else if (move > 0) {
			data[0] = i;
			data[1] = move & 0xFF;
			data[2] = (move >> 8) & 0xFF;
			sync_oob_word(&rpi_sync, OOB_ENCODER_CW, *(uint32_t*)data, 0);
		}

		move = switch_process(&encoder_sw_state[i], sw[encoder_sw[i]]);
		if (move > 0) {
			//printf("enc %d up\n", i);
			data[0] = i;
			sync_oob_word(&rpi_sync, OOB_ENCODER_UP, *(uint32_t*)data, 0);
		}
		else if (move < 0) {
			//printf("enc %d down\n", i);
			data[0] = i;
			sync_oob_word(&rpi_sync, OOB_ENCODER_DOWN, *(uint32_t*)data, 0);
		}
	}
}

void LedsOff()
{
	L1BlankOn();
	L2BlankOn();
}


void LedsOn()
{
	L1BlankOff();
	L2BlankOff();
}
uint32_t ReadT()
{
	return (BOARD_INITPINS_T0_GPIO->B[BOARD_INITPINS_T0_PORT][BOARD_INITPINS_T0_PIN] << 0)
			| (BOARD_INITPINS_T1_GPIO->B[BOARD_INITPINS_T1_PORT][BOARD_INITPINS_T1_PIN] << 1)
			| (BOARD_INITPINS_T2_GPIO->B[BOARD_INITPINS_T2_PORT][BOARD_INITPINS_T2_PIN] << 2)
			| (BOARD_INITPINS_T3_GPIO->B[BOARD_INITPINS_T3_PORT][BOARD_INITPINS_T3_PIN] << 3)
			| (BOARD_INITPINS_T4_GPIO->B[BOARD_INITPINS_T4_PORT][BOARD_INITPINS_T4_PIN] << 4)
			| (BOARD_INITPINS_T5_GPIO->B[BOARD_INITPINS_T5_PORT][BOARD_INITPINS_T5_PIN] << 5)
			| (BOARD_INITPINS_T6_GPIO->B[BOARD_INITPINS_T6_PORT][BOARD_INITPINS_T6_PIN] << 6)
			| (BOARD_INITPINS_T7_GPIO->B[BOARD_INITPINS_T7_PORT][BOARD_INITPINS_T7_PIN] << 7);
}

void SetMkBr(uint16_t mask)
{
	// from LSB: BR0, MK0, BR1, MK1, ...

	BOARD_INITPINS_BR0_GPIO->W[BOARD_INITPINS_BR0_PORT][BOARD_INITPINS_BR0_PIN] = mask & (1 << 1);
	BOARD_INITPINS_MK0_GPIO->W[BOARD_INITPINS_MK0_PORT][BOARD_INITPINS_MK0_PIN] = mask & (1 << 0);
	BOARD_INITPINS_BR1_GPIO->W[BOARD_INITPINS_BR1_PORT][BOARD_INITPINS_BR1_PIN] = mask & (1 << 3);
	BOARD_INITPINS_MK1_GPIO->W[BOARD_INITPINS_MK1_PORT][BOARD_INITPINS_MK1_PIN] = mask & (1 << 2);
	BOARD_INITPINS_BR2_GPIO->W[BOARD_INITPINS_BR2_PORT][BOARD_INITPINS_BR2_PIN] = mask & (1 << 5);
	BOARD_INITPINS_MK2_GPIO->W[BOARD_INITPINS_MK2_PORT][BOARD_INITPINS_MK2_PIN] = mask & (1 << 4);
	BOARD_INITPINS_BR3_GPIO->W[BOARD_INITPINS_BR3_PORT][BOARD_INITPINS_BR3_PIN] = mask & (1 << 7);
	BOARD_INITPINS_MK3_GPIO->W[BOARD_INITPINS_MK3_PORT][BOARD_INITPINS_MK3_PIN] = mask & (1 << 6);
	BOARD_INITPINS_BR4_GPIO->W[BOARD_INITPINS_BR4_PORT][BOARD_INITPINS_BR4_PIN] = mask & (1 << 9);
	BOARD_INITPINS_MK4_GPIO->W[BOARD_INITPINS_MK4_PORT][BOARD_INITPINS_MK4_PIN] = mask & (1 << 8);
}

SemaphoreHandle_t xKeyTimerSemaphore = NULL;

volatile uint32_t sctimer_state = 0;
volatile uint32_t sctimer_counter = 0;

uint32_t timer_value()
{
	taskENTER_CRITICAL();
	uint32_t highcount = sctimer_counter << 16;
	uint32_t timervalue = SCT0->COUNT;

	// but now the counter may have already overflowed! so highcount may be outdated.
	// must read COUNT first, because the overflow may happen also between reading COUNT and STATE.
	int overflow = SCT0->STATE != sctimer_state;
	taskEXIT_CRITICAL();

	// if there was an overflow, and the timer was close to beginning of its range,
	// assume that it did indeed overflow since last interrupt.
	if (overflow && timervalue < 32768) {
			highcount += 65536;
	}

	return highcount + timervalue;
}

extern "C" void SCT0_IRQHandler(void)
{
	sctimer_state = SCT0->STATE;
	sctimer_counter++;

	BaseType_t higherPriorityTaskWoken;
	xSemaphoreGiveFromISR(xKeyTimerSemaphore, &higherPriorityTaskWoken);

	SCTIMER_1_PERIPHERAL->EVFLAG = SCTIMER_1_PERIPHERAL->EVFLAG;
	//SCTIMER_ClearStatusFlags(SCTIMER_1_PERIPHERAL, 1);

	portEND_SWITCHING_ISR(higherPriorityTaskWoken);
}

//struct presetnames_t {
//	unsigned char names[PRESET_COUNT * PRESET_NAME_LENGTH];
//};

presetnames_t presetnames = {0};

void sync_complete(int status);

template <typename T>
class DiffSyncer
{
public:
	DiffSyncer(T* data, uint32_t address) : _data(data), _address(address), _resend_full(true)
	{
		_dummy = 0;
	}

	void reset()
	{
		_resend_full = true;
	}

	int run()
	{
		if (_resend_full) {
		//printf("%x - %x @ %x\n", 0, sizeof(T), _address);
			_resend_full = false;
			memcpy(&_prev, _data, sizeof(T));
			sync_block(&rpi_sync, (uint8_t*)&_prev, _address, sizeof(T), sync_complete);
			return 0;
		}

		for (int i = 0; i < sizeof(T); i += 4) {
			if ( *(uint32_t*)&((uint8_t*)_data)[i] != *(uint32_t*)&((uint8_t*)&_prev)[i] )
			{
				int j = i + 4;
				for (; j < sizeof(T); j += 4) {
					if ( *(uint32_t*)&((uint8_t*)_data)[j] == *(uint32_t*)&((uint8_t*)&_prev)[j] ) {
						break;
					}
				}
		//printf("%x - %x (max %x) @ %x\n", i, j, sizeof(T), _address);
				memcpy(&((uint8_t*)&_prev)[i], &((uint8_t*)_data)[i], j - i);
				sync_block(&rpi_sync, &((uint8_t*)&_prev)[i], _address + i, j - i, sync_complete);
				return 0;
			}
		}

		return 1;
	}
private:
	T _prev;
	uint32_t _dummy; // don't move it, it's for padding end of _prev
	T* _data;
	uint32_t _address;
	bool _resend_full;
};

//volatile int sync_running = 1;
DiffSyncer<presetnames_t> namesync(&presetnames, 0x1000000);

uint8_t mod_fb[48] = {0};
volatile int mod_fb_count = sizeof(mod_fb);
volatile int mod_sync_done = 1;
volatile int mod_data_ready = 0;

//void sync_complete_presetnames(int status);

void sync_mod_data_complete(int status)
{
	mod_data_ready = 0;
	mod_sync_done = 1;
	sync_complete(status);
}

void sync_mod_data_start()
{
	if (loading) {
		return;
	}
    sync_block(&rpi_sync, mod_fb, 0x2000000, sizeof(mod_fb), sync_mod_data_complete);
}

int preset_load_start()
{
	if (loading) return 1;

	loading = 1;
	sync_oob_word(&rpi_sync, OOB_UI_PAUSE, 0, 0);
    sync_block(&rpi_sync, (uint8_t*)&preset, 0, sizeof(preset), sync_complete);

	return 0;
}

void sync_complete(int status)
{
	if (status != 0) {
		loading = 0;
		namesync.reset();
		preset_load_start();
		return;
	}

	if (status == 0) {
		if (loading) {
			if (namesync.run()) {
				sync_oob_word(&rpi_sync, OOB_UI_CONTINUE, 0, 0);
		        LedsOn();
				loading = 0;
			}
		}
		if (!loading && mod_data_ready) {
			sync_mod_data_start();
		}
	}
}

#define KEYSCAN_NUMKEYSETS (5)
#define KEYSCAN_MAXINDEX (2*KEYSCAN_NUMKEYSETS)
uint32_t keyscanindex = 0;
uint32_t keyscanstate[(KEYSCAN_MAXINDEX+3)/4] = {0};
uint32_t keyscantimer[KEYSCAN_NUMKEYSETS*8] = {0};

void KeyScan()
{
	BaseType_t higherPriorityTaskWoken = pdFALSE;
	uint32_t timervalue = timer_value();
	uint32_t T = ReadT();

	int stateindex = keyscanindex / 4;
	uint32_t prevT = (keyscanstate[stateindex] >> ((keyscanindex & 3)*8)) & 0xFF;
	keyscanstate[stateindex] &= ~(0xFF << ((keyscanindex & 3)*8));
	keyscanstate[stateindex] |= T << ((keyscanindex & 3)*8);

	if ((keyscanindex & 1) == 0) {
	        // make
	        for (int i = 0; i < 8; i++) {
	                if ((T & (1 << i)) == 0
	                        && (prevT & (1 << i)) != 0) {
	                        // key is newly pressed
	                        int keyindex = (keyscanindex >> 1) * 8 + i;
		                    //printf("%d make\n", keyindex);
	                        uint32_t timediff = timervalue - keyscantimer[keyindex];
	                        higherPriorityTaskWoken |= keys_press(keyindex, timediff);
	                        //notestack_push(keyindex + 36);
	                        //midikeypress = 1;
	                }
	                else if ((T & (1 << i)) != 0
	                                && (prevT & (1 << i)) == 0) {
	                        // released
	                        int keyindex = (keyscanindex >> 1) * 8 + i;
		                    //printf("%d release\n", keyindex);
	                        higherPriorityTaskWoken |= keys_release(keyindex);
	                        //notestack_pop(keyindex + 36);
	                        //midikeypress = 1;
	                }
	        }
	}
	else {
	        // break
	        for (int i = 0; i < 8; i++) {
	                if ((T & (1 << i)) == 0
	                        && (prevT & (1 << i)) != 0) {
	                    //printf("%d break\n", (keyscanindex >> 1) * 8 + i);
	                        // key is newly pressed
	                        keyscantimer[(keyscanindex >> 1) * 8 + i] = timervalue;
	                }
	        }
	}

	// prepare for next cycle
	keyscanindex++;
	if (keyscanindex >= KEYSCAN_MAXINDEX) {
	        keyscanindex = 0;
	}
	SetMkBr(~(1 << keyscanindex));

	/*if (midiin_timeout_counter < 10000) {
	        midiin_timeout_counter++;
	        if (midiin_timeout_counter >= 10000) {
	                higherPriorityTaskWoken |= midiin_timeout();
	                midiin_timeout_counter = 0;
	        }
	}*/

	//synth_tick();
	//portEND_SWITCHING_ISR(higherPriorityTaskWoken);
}

void midi_process();

void KeyboardTask(void* pvParameters)
{
	dsp_reset();
	if (!flash_loadpreset(0, &preset)) {
		preset_init();
		flash_savepreset(0, &preset);
	}
	else {
		sync_preset_full();
	}

	rpi_start();

    sync_block(&rpi_sync, (uint8_t*)&preset, 0, sizeof(preset), sync_complete);

	NVIC_SetPriority(SCTIMER_1_IRQN, 2);
    SCTIMER_EnableInterrupts(SCTIMER_1_PERIPHERAL, 1);

    arpclock_run();

	while (1) {
        if (xSemaphoreTake(xKeyTimerSemaphore, (TickType_t)10) == pdTRUE) {
        	KeyScan();
            ScanButtonsAndEncoders();
            midi_process();
        }
		//vTaskDelay(1);
	}
}

SemaphoreHandle_t xBpmTimerSemaphore = NULL;

void ArpTriggerTask(void* pvParameters)
{
	while (1) {
        if (xSemaphoreTake(xBpmTimerSemaphore, (TickType_t)10) == pdTRUE) {
        	for (int i = 0; i < 4; i++) {
        		arp[i].TimerTick();
        	}
        }
	}
}

#define FULLVALUE(param, subparam, value) dsp_cmd(param, subparam, value);
#define SETMODSOURCE(source, subparam, value) dsp_cmd(source, (subparam) | 0x80, value)
#define MODMATRIX(param, target, subparam, value) dsp_cmd(param, ((((target & 0xF) << 2)) + ((subparam) & 3)) | 0x40, value)
#define SETKEYMAP(target, zone, keyindex) dsp_cmd(0xfb, target&0x3f, ((zone&0x3) << 4) | (keyindex & 0x3))

#define SWITCH_ON(sw) FULLVALUE(0xfe, 0xfd, 0x200 | (sw));
#define SWITCH_OFF(sw) FULLVALUE(0xfe, 0xfd, 0x100 | (sw));

#define REQUEST_MODVALUES() dsp_cmd(0xfe, 0xfa, 0);

void preset_init()
{
	// lfos
	for (int param = 0; param < 16; param++) {
		for (int i = 0; i < 5; i++) {
			SETMODSOURCE(param + 0x00, i, 0);
		}
	}
	// envelopes
	for (int param = 0; param < 16; param++) {
		for (int i = 0; i < 5; i++) {
			SETMODSOURCE(param + 0x10, i, 0);
		}
	}
	// controllers
	for (int param = 0; param < 11; param++) {
		SETMODSOURCE(param + 0x20, 0, 0x4000);
		SETMODSOURCE(param + 0x20, 1, 0x0100);
		preset.controller[param].scale = 0x4000;
		preset.controller[param].deadzone = 0x0100;
	}
	// operators
	for (int param = 0; param < 16; param++) {
		SETMODSOURCE(param + 0x30, 0, 0xff);
		preset.op[param].modsource1 = 0xff;
		preset.op[param].modsource2 = 0xff;
		SETMODSOURCE(param + 0x30, 1, 0xff);
		SETMODSOURCE(param + 0x30, 2, 0);
		SETMODSOURCE(param + 0x30, 3, 0);
	}

	// modmatrix
	for (int param = 0; param < 0x40; param++) {
		for (int target = 0; target < 11; target++) {
				preset.modmatrix[param].targets[target].outputid = 0xffff;
				MODMATRIX(param, target, 0, 0);
				MODMATRIX(param, target, 1, 0xffff);
				MODMATRIX(param, target, 2, 0);
			}
	}

	// keymap
	for (int tgt = 0; tgt < 0x40; tgt++) {
		preset.key_mapping[tgt].keyzone = 0;
		preset.key_mapping[tgt].keyindex = 0;
		SETKEYMAP(tgt, 0, 0);
	}

	// parameters
	for (int param = 0; param < 0xf0; param++) {
		FULLVALUE(param, 0, 0);
		FULLVALUE(param, 1, 0);
		FULLVALUE(param, 2, 0);
	}

#define OUTPUT(NAME, x, y, z, w, k, VALUE, dummy1, dummy2) FULLVALUE(output_##NAME, 0, VALUE); preset.paramvalue[output_##NAME] = VALUE;
#define OUTPUT_VIRT(NAME, x, y, z, w, k, VALUE, dummy1, dummy2) do { if (output_##NAME < 0xff) { FULLVALUE(output_##NAME, 0, VALUE); preset.paramvalue[output_##NAME] = VALUE; } } while(0);

#include "../../../interface/paramdef.h"

#undef OUTPUT
#undef OUTPUT_VIRT

	FULLVALUE(output_VCO1_PITCH, 1, 0x4000);
	FULLVALUE(output_VCO2_PITCH, 1, 0x4000);
	FULLVALUE(output_VCO3_PITCH, 1, 0x4000);
	FULLVALUE(output_VCO4_PITCH, 1, 0x4000);
	FULLVALUE(output_VCO5_PITCH, 1, 0x4000);
	FULLVALUE(output_VCO6_PITCH, 1, 0x4000);
	FULLVALUE(output_VCO7_PITCH, 1, 0x4000);

	preset.switches[0] = 0;
	preset.switches[1] = 0;
	for (int i = 0; i < 64; i++) {
	    SWITCH_OFF(i);
	}
    SWITCH_ON(switch_SEL1SQR);
	preset.switches[1] |= 1 << (switch_SEL1SQR - 32);
    SWITCH_ON(switch_SEL2SQR);
	preset.switches[1] |= 1 << (switch_SEL2SQR - 32);
    SWITCH_ON(switch_SEL3SQR);
	preset.switches[1] |= 1 << (switch_SEL3SQR - 32);

    for (int i = 0; i < 16; i++) {
		SETMODSOURCE(i + modsource_ENV0, 0,1);
    	preset.env[i].flags = 1; // SubParamFlags_AdsrRetrigger
		SETMODSOURCE(i + modsource_ENV0, 1, 0x10);
		preset.env[i].a = 0x10;
		SETMODSOURCE(i + modsource_ENV0, 2, 0x2000);
		preset.env[i].d = 0x2000;
		SETMODSOURCE(i + modsource_ENV0, 3, 0xE000);
		preset.env[i].s = 0xE000;
		SETMODSOURCE(i + modsource_ENV0, 4, 0x5000);
		preset.env[i].r = 0x5000;
    }

    MODMATRIX(modsource_ENV0, 0, 0, 0x3fff);
    MODMATRIX(modsource_ENV0, 0, 1, output_VCF1_LIN);
	preset.modmatrix[modsource_ENV0].targets[0].depth = 0x3fff;
	preset.modmatrix[modsource_ENV0].targets[0].outputid = output_VCF1_LIN;

    MODMATRIX(modsource_ENV0, 1, 0, 0x3fff);
    MODMATRIX(modsource_ENV0, 1, 1, output_VCF2_LIN);
	preset.modmatrix[modsource_ENV0].targets[1].depth = 0x3fff;
	preset.modmatrix[modsource_ENV0].targets[1].outputid = output_VCF2_LIN;

    MODMATRIX(modsource_ENV0, 2, 0, 0x3fff);
    MODMATRIX(modsource_ENV0, 2, 1, output_CLEANF_LIN);
	preset.modmatrix[modsource_ENV0].targets[2].depth = 0x3fff;
	preset.modmatrix[modsource_ENV0].targets[2].outputid = output_CLEANF_LIN;

    MODMATRIX(modsource_X, 0, 0, 0x3fff);
    MODMATRIX(modsource_X, 0, 1, output_VCF1_CV);
	preset.modmatrix[modsource_X].targets[0].depth = 0x3fff;
	preset.modmatrix[modsource_X].targets[0].outputid = output_VCF1_CV;

    MODMATRIX(modsource_LFO0, 0, 0, 0x3fff);
    MODMATRIX(modsource_LFO0, 0, 1, output_VCF1_CV);
	preset.modmatrix[modsource_LFO0].targets[0].depth = 0x3fff;
	preset.modmatrix[modsource_LFO0].targets[0].outputid = output_VCF1_CV;
	SETMODSOURCE(0, 1, 0x4000);
	preset.lfo[0].speed = 0x4000;

	// vco2
	preset.key_mapping[key_map_target_vco2].keyindex = 1;
	SETKEYMAP(key_map_target_vco2, 0, 1);
	preset.key_mapping[key_map_target_vco3].keyindex = 2;
	SETKEYMAP(key_map_target_vco3, 0, 2);

	for (int i = 0; i < 16; i++) {
		preset.key_input[i].channel = 0xff;
		preset.key_input[i].rangelo = 0;
		preset.key_input[i].rangehi = 0x7f;
		preset.key_input[i].transpose = 0;
	}
	for (int i = 0; i < 4; i++) {
		preset.keyzone[i].type = PresetKeyzoneType_Single;
		preset.keyzone[i].keypriority = PresetKeyzoneKeyPriority_high;

		SET_KEYPRIORITY(i, preset.keyzone[i].keypriority);
	}

	preset.clock.source = ClockSourceType_Internal;
	preset.clock.internal_bpm = 14400;
	arpclock_set_speed((float)preset.clock.internal_bpm * 0.01f);

	preset.keyzone[0].type = PresetKeyzoneType_Paraphonic;
	preset.keyzone[0].arpsettings = ArpSettings_t();
	preset.keyzone[0].arpsettings.Mode = Arp_Up;
	preset.keyzone[0].arpsettings.Octaves = Oct_One;
	preset.keyzone[0].arpsettings.ResetTimeOnFirstKey = true;
	preset.keyzone[0].arpsettings.Rhythm = 0xff;
	preset.keyzone[0].arpsettings.TimingSource = TimeSource_Internal;
	preset.keyzone[0].arpsettings.Interleave = Interleave_Pattern;
	preset.keyzone[0].arpsettings.Speed.Top = 1;
	preset.keyzone[0].arpsettings.Speed.Bottom = 16;
	preset.keyzone[0].arpsettings.KillNotesAfterLastNoteOff = true;

	preset.low.h = 0x1000;
	preset.high.h = 0x1000;
	preset.active.h = 0xC000;

	preset.low.v = 0x1fff;
	preset.high.v = 0x7fff;
	preset.active.v = 0xffff;

	preset.low.s = 0xffff;
	preset.high.s = 0xffff;
	preset.active.s = 0xffff;

	preset.SetName("Bleepy");
}
///////////////////////////////////////////////////////////////////////


extern "C" void APPInit(void);
extern "C" void APPTask(void);

void USBTask(void* pvParameters)
{
	APPInit();

	while(1) {
		APPTask();
		vTaskDelay(1);
	}
}

typedef enum {
	control_state_idle = 0,
	control_state_mod_fb = 1,
	control_state_autotune_status = 2
} control_cb_state_t;

control_cb_state_t control_cb_state = control_state_idle;

volatile int autotune_status = 0;

void control_cb(uint8_t data)
{
	if (control_cb_state == control_state_idle) {
		if (data == control_state_mod_fb) {
			control_cb_state = control_state_mod_fb;
		}
		else if (data == control_state_autotune_status) {
			control_cb_state = control_state_autotune_status;
		}
		else {
		}
	}
	else if (control_cb_state == control_state_mod_fb) {
		if (mod_fb_count < sizeof(mod_fb)) {
			mod_fb[mod_fb_count] = data;
			mod_fb_count++;
		}
		if (mod_fb_count == sizeof(mod_fb)) {
			control_cb_state = control_state_idle;
		}
	}
	else if (control_cb_state == control_state_autotune_status) {
		autotune_status = data;
        sync_oob_word(&rpi_sync, OOB_AUTOTUNE_STATE, autotune_status, 0);
        printf("autotune: %02x\n", (int)(uint16_t)autotune_status);
		control_cb_state = control_state_idle;
	}
}

void mod_data_loop()
{
	static int no_mod_reply_count = 0;

	if (mod_data_ready) {
		return;
	}

	if (mod_sync_done) {
		mod_fb_count = 0;
		mod_sync_done = 0;
		__disable_irq();
		if (control_cb_state == control_state_mod_fb) {
			control_state_idle;
		}
		__enable_irq();
		REQUEST_MODVALUES();
		no_mod_reply_count = 0;
	}
	else {
		control_process();

		if (mod_fb_count == sizeof(mod_fb)) {
			mod_sync_done = 0;
			mod_data_ready = 1;
			sync_mod_data_start();
		}
		else {
			no_mod_reply_count++;
			if (no_mod_reply_count > 5) {
				mod_fb_count = sizeof(mod_fb);
				mod_data_ready = 0;
				mod_sync_done = 1;
				__disable_irq();
				if (control_cb_state == control_state_mod_fb) {
					control_state_idle;
				}
				__enable_irq();
			}
		}
	}
}

void IdleTask(void* pvParameters)
{
	int count = 0;
	while(1) {
		mod_data_loop();

		count++;
		if (count > 10) {
			rpi_uart.config.timer_tick(rpi_uart.config.timer_tick_data);
			count = 0;
		}
		vTaskDelay(5);
	}
}

//#define offsetof(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))

void sync_preset(uint32_t addr)
{
	if (addr >= offsetof(PanPreset_t, clock)) {
		addr -= offsetof(PanPreset_t, clock);
		switch (addr) {
		case 0:
			break;
		case 1:
			arpclock_set_speed((float)preset.clock.internal_bpm * 0.01f);
			break;
		}
	}
	else if (addr >= offsetof(PanPreset_t, keyzone)) {
		addr -= offsetof(PanPreset_t, keyzone);
		int zone = addr / sizeof(keyzone_settings_t);
		int suboffset = addr - zone * sizeof(keyzone_settings_t);
		if (suboffset >= offsetof(keyzone_settings_t, type) && suboffset < offsetof(keyzone_settings_t, type)+4) {
			arp[zone].Panic();

			if (preset.keyzone[zone].type == PresetKeyzoneType_Single) {
				for (int mapid = 0; mapid < NUM_KEY_MAP_TARGETS; mapid++) {
					if (preset.key_mapping[mapid].keyzone == zone) {
						SETKEYMAP(mapid, zone, 0);
					}
				}
			}
			else {
				for (int mapid = 0; mapid < NUM_KEY_MAP_TARGETS; mapid++) {
					if (preset.key_mapping[mapid].keyzone == zone) {
						SETKEYMAP(mapid, zone, preset.key_mapping[mapid].keyindex);
					}
				}
			}
		}
		else if (suboffset >= offsetof(keyzone_settings_t, keypriority) && suboffset < offsetof(keyzone_settings_t, keypriority)+4) {
			SET_KEYPRIORITY(zone, preset.keyzone[zone].keypriority);
		}
	}
	else if (addr >= offsetof(PanPreset_t, key_input)) {
		// skip
//#define NUM_KEY_INPUTS (16)
//	key_input_t key_input[NUM_KEY_INPUTS];
	}
	else if (addr >= offsetof(PanPreset_t, key_mapping)) {
		addr -= offsetof(PanPreset_t, key_mapping);

		int mapid = addr / sizeof(key_mapping_t);
		int subaddr = addr - mapid * sizeof(key_mapping_t);

		switch (subaddr) {
		case 0:
			if (preset.keyzone[preset.key_mapping[mapid].keyzone].type == PresetKeyzoneType_Single) {
				SETKEYMAP(mapid, preset.key_mapping[mapid].keyzone, 0);
			} else {
				SETKEYMAP(mapid, preset.key_mapping[mapid].keyzone, preset.key_mapping[mapid].keyindex);
			}
			break;
		}
	}
	else if (addr >= offsetof(PanPreset_t, ledbrightness)) {
		// skip
	}
	else if (addr >= offsetof(PanPreset_t, op)) {
		addr -= offsetof(PanPreset_t, op);
	}
	else if (addr >= offsetof(PanPreset_t, controller)) {
		addr -= offsetof(PanPreset_t, controller);

		int ctrlid = addr / sizeof(ControllerParam_t);
		int subaddr = addr - ctrlid * sizeof(ControllerParam_t);

		switch (subaddr) {
		case 0:
			SETMODSOURCE(ctrlid + modsource_X, 0, preset.controller[ctrlid].scale);
			SETMODSOURCE(ctrlid + modsource_X, 1, preset.controller[ctrlid].deadzone);
			break;
		}
	}
	else if (addr >= offsetof(PanPreset_t, env)) {
		addr -= offsetof(PanPreset_t, env);

		int envid = addr / sizeof(EnvParam_t);
		int subaddr = addr - envid * sizeof(EnvParam_t);
		switch (subaddr) {
		case 0:
			//printf("env %d a=%x\n", envid, preset.env[envid].a);
			SETMODSOURCE(envid + modsource_ENV0, 0, preset.env[envid].flags);
			SETMODSOURCE(envid + modsource_ENV0, 1, preset.env[envid].a);
			break;
		case 4:
			//printf("env %d d=%x\n", envid, preset.env[envid].d);
			//printf("env %d s=%x\n", envid, preset.env[envid].s);
			SETMODSOURCE(envid + modsource_ENV0, 2, preset.env[envid].d);
			SETMODSOURCE(envid + modsource_ENV0, 3, preset.env[envid].s);
			break;
		case 8:
			//printf("env %d r=%x\n", envid, preset.env[envid].r);
			SETMODSOURCE(envid + modsource_ENV0, 4, preset.env[envid].r);
			break;
		case 12:
			break;
		}
	}
	else if (addr >= offsetof(PanPreset_t, lfo)) {
		addr -= offsetof(PanPreset_t, lfo);

		int lfoid = addr / sizeof(LfoParam_t);
		int subaddr = addr - lfoid * sizeof(LfoParam_t);
		switch (subaddr) {
		case 0:
			SETMODSOURCE(lfoid + modsource_LFO0, 0, preset.lfo[lfoid].flags);
			SETMODSOURCE(lfoid + modsource_LFO0, 1, preset.lfo[lfoid].speed);
			break;
		case 4:
			SETMODSOURCE(lfoid + modsource_LFO0, 2, preset.lfo[lfoid].depth);
			SETMODSOURCE(lfoid + modsource_LFO0, 3, preset.lfo[lfoid].shape);
			break;
		case 8:
			SETMODSOURCE(lfoid + modsource_LFO0, 4, preset.lfo[lfoid].reset_phase);
			break;
		}
	}
	else if (addr >= offsetof(PanPreset_t, modmatrix)) {
		addr -= offsetof(PanPreset_t, modmatrix);

		int matrixrow = addr / sizeof(ModMatrixRow_t);
		int entry = (addr - matrixrow*sizeof(ModMatrixRow_t)) / sizeof(ModTargetSpec_t);
		int subaddr = addr - matrixrow*sizeof(ModMatrixRow_t) - entry*sizeof(ModTargetSpec_t);

		switch (subaddr) {
		case 0:
			//if (preset.modmatrix[matrixrow].targets[entry].depth)
			//	printf("mod %d/%d d=%x oid=%d at %x+%x\n", matrixrow, entry, preset.modmatrix[matrixrow].targets[entry].depth, preset.modmatrix[matrixrow].targets[entry].outputid, addr, offsetof(PanPreset_t, modmatrix));
		    MODMATRIX(matrixrow, entry, 0, preset.modmatrix[matrixrow].targets[entry].depth);
		    MODMATRIX(matrixrow, entry, 1, preset.modmatrix[matrixrow].targets[entry].outputid);
			break;
		case 4:
			//if (preset.modmatrix[matrixrow].targets[entry].depth)
			//	printf("mod %d/%d d=%x oid=%d sid=%d at %x\n", matrixrow, entry, preset.modmatrix[matrixrow].targets[entry].depth, preset.modmatrix[matrixrow].targets[entry].outputid, preset.modmatrix[matrixrow].targets[entry].sourceid, addr);
		    MODMATRIX(matrixrow, entry, 2, preset.modmatrix[matrixrow].targets[entry].sourceid);
			break;
		}
	}
	else if (addr >= offsetof(PanPreset_t, paramvalue)) {
		addr -= offsetof(PanPreset_t, paramvalue);

		int paramid = addr / 2;
		if (paramid < 0xf0) {
			FULLVALUE(paramid, 2, preset.paramvalue[paramid]);
			FULLVALUE(paramid+1, 2, preset.paramvalue[paramid+1]);
		}
	}
	else if (addr >= offsetof(PanPreset_t, switches)) {
		addr -= offsetof(PanPreset_t, switches);
		addr /= 4;

		for (int i = 0; i < 32; i++) {
			if (preset.switches[addr] & (1 << i)) {
				SWITCH_ON(addr * 32 + i);
			}
			else {
				SWITCH_OFF(addr * 32 + i);
			}
		}
	}
	else if (addr >= offsetof(PanPreset_t, Name)) {
		addr -= offsetof(PanPreset_t, Name);
		// name
	}
}

void sync_preset_full()
{
	// lfos
	for (int param = 0; param < 16; param++) {
		SETMODSOURCE(param + 0x00, 0, preset.lfo[param].flags);
		SETMODSOURCE(param + 0x00, 1, preset.lfo[param].speed);
		SETMODSOURCE(param + 0x00, 2, preset.lfo[param].depth);
		SETMODSOURCE(param + 0x00, 3, preset.lfo[param].shape);
		SETMODSOURCE(param + 0x00, 4, preset.lfo[param].reset_phase);
	}
	// envelopes
	for (int param = 0; param < 16; param++) {
		SETMODSOURCE(param + 0x10, 0, preset.env[param].flags);
		SETMODSOURCE(param + 0x10, 1, preset.env[param].a);
		SETMODSOURCE(param + 0x10, 2, preset.env[param].d);
		SETMODSOURCE(param + 0x10, 3, preset.env[param].s);
		SETMODSOURCE(param + 0x10, 4, preset.env[param].r);
	}
	// controllers
	for (int param = 0; param < 11; param++) {
		SETMODSOURCE(param + 0x20, 0, preset.controller[param].scale);
		SETMODSOURCE(param + 0x20, 1, preset.controller[param].deadzone);
	}
	// operators
	for (int param = 0; param < 16; param++) {
		SETMODSOURCE(param + 0x30, 0, preset.op[param].modsource1);
		SETMODSOURCE(param + 0x30, 1, preset.op[param].modsource2);
		SETMODSOURCE(param + 0x30, 2, preset.op[param].op);
		SETMODSOURCE(param + 0x30, 3, preset.op[param].parameter);
	}

	// modmatrix
	for (int param = 0; param < 0x40; param++) {
		for (int target = 0; target < 11; target++) {
				MODMATRIX(param, target, 0, preset.modmatrix[param].targets[target].depth);
				MODMATRIX(param, target, 1, preset.modmatrix[param].targets[target].outputid);
				MODMATRIX(param, target, 2, preset.modmatrix[param].targets[target].sourceid);
			}
	}

	// parameters
	for (int param = 0; param < 0xf0; param++) {
		FULLVALUE(param, 0, preset.paramvalue[param]);
		FULLVALUE(param, 1, 0);
	}

	FULLVALUE(output_VCO1_PITCH, 1, 0x4000);
	FULLVALUE(output_VCO2_PITCH, 1, 0x4000);
	FULLVALUE(output_VCO3_PITCH, 1, 0x4000);
	FULLVALUE(output_VCO4_PITCH, 1, 0x4000);
	FULLVALUE(output_VCO5_PITCH, 1, 0x4000);
	FULLVALUE(output_VCO6_PITCH, 1, 0x4000);
	FULLVALUE(output_VCO7_PITCH, 1, 0x4000);
	FULLVALUE(output_NOISE_COLOR, 1, 0x4000);

	for (int i = 0; i < 64; i++) {
		if (i < 32) {
			if (preset.switches[0] & (1 << i)) {
			    SWITCH_ON(i);
			}
			else {
			    SWITCH_OFF(i);
			}
		}
		else {
			if (preset.switches[1] & (1 << (i - 32))) {
			    SWITCH_ON(i);
			}
			else {
			    SWITCH_OFF(i);
			}
		}
	}

	for (int tgt = 0; tgt < 0x40; tgt++) {
		SETKEYMAP(tgt, preset.key_mapping[tgt].keyzone, preset.key_mapping[tgt].keyindex);
	}

	for (int zone = 0; zone < 4; zone++) {
		SET_KEYPRIORITY(zone, preset.keyzone[zone].keypriority);
	}

	arpclock_set_speed((float)preset.clock.internal_bpm * 0.01f);
}

void sync_ledstate_encoder(int index)
{
}

void sync_ledstate_button(int index)
{

}

void sync_ledstate(int addr)
{
	if (addr >= offsetof(PanLedState_t, encoders)) {
		addr -= offsetof(PanLedState_t, encoders);

		int index = addr / sizeof(LedState_t);
		sync_ledstate_encoder(index);
	}
	else {
		int index = addr / sizeof(LedState_t);

		sync_ledstate_button(index);
	}
}

void sync_data_func(int addr, uint8_t* data)
{
	if (addr & 3) return;

	if (addr >= 0 && addr < (int)sizeof(preset)) {
		if (loading) return;
		*(uint32_t*)&((uint8_t*)&preset)[addr] = *(uint32_t*)data;
		sync_preset(addr);
	}

	if (addr >= 0x1000000 && addr < ((int)sizeof(ledstate) | 0x1000000)) {
		addr -= 0x1000000;
		*(uint32_t*)&((uint8_t*)&ledstate)[addr] = *(uint32_t*)data;
		sync_ledstate(addr);
	}
}

int sync_oobdata_func(uint8_t cmd, uint32_t data)
{
	switch (cmd) {
	case CMD_PAD_ZERO:
		dsp_pad_zero();
		return 0;
	case CMD_CALIBRATE:
		dsp_calibrate();
		return 0;
	case CMD_PRESET_LOAD: {
		if (loading) return 0;
		uint16_t mastervol = preset.paramvalue[Output_MASTER_LEVEL];
        if (!flash_loadpreset(data, &preset)) preset_init();
		preset.paramvalue[Output_MASTER_LEVEL] = mastervol;
        sync_preset_full();
		preset_load_start();
		return 0;
	}
	case CMD_PRESET_STORE:
		if (loading) return 0;
        sync_oob_word(&rpi_sync, OOB_UI_PAUSE, 0, 0);
        loading = 1;
        flash_savepreset(data, &preset);
        if (namesync.run()) {
        	loading = 0;
        	sync_oob_word(&rpi_sync, OOB_UI_CONTINUE, 0, 0);
        }
		return 0;
	}
	return 1;
}

/*
 * @brief   Application entry point.
 */

/* Task to be created. */
void LedTask( void * pvParameters )
{
	const TickType_t xDelay = 15 / portTICK_PERIOD_MS;
	for( ;; )
    {
        SendLeds();
        vTaskDelay( xDelay );
    }

}

#include "fsl_nor_flash.h"
#include "fsl_spifi_nor_flash.h"

#define FLASH_PAGE_SIZE (256)
#define SECTOR_SIZE (4096)
//#define EXAMPLE_SPI_BAUDRATE (96000000)
#define SPIFI_CLOCK_FREQ_MIN (1000000)
#define SPIFI_CLOCK_FREQ_MAX (1000000)
#define NOR_FLASH_START_ADDRESS  FSL_FEATURE_SPIFI_START_ADDR

void SPIFI_Clock(spifi_nor_clock_init_t clock)
{
	uint32_t sourceClockFreq;

	switch(clock)
	{
		case kSpifiNorClockInit_Max:
	    sourceClockFreq = CLOCK_GetCoreSysClkFreq();
	    CLOCK_SetClkDiv(kCLOCK_DivSpifiClk, sourceClockFreq / SPIFI_CLOCK_FREQ_MAX, false);
	    break;
		case kSpifiNorClockInit_Sdfp:
        default:
	    sourceClockFreq = CLOCK_GetCoreSysClkFreq();
	    /* Set the clock divider. */
	    CLOCK_SetClkDiv(kCLOCK_DivSpifiClk, sourceClockFreq / SPIFI_CLOCK_FREQ_MIN, false);
	    break;
	}
}

uint8_t mem_writeBuffer[FLASH_PAGE_SIZE];
uint8_t mem_readBuffer[FLASH_PAGE_SIZE] = {0};

extern nor_config_t norConfig;
nor_handle_t norHandle = {NULL};

spifi_mem_nor_config_t spifiMemConfig = {
    .clockInit = SPIFI_Clock,
    .cmd_format = kSPIFI_CommandDataQuad,
    .quad_mode_setting = kSerialNorQuadMode_StatusReg2_Bit1_0x31,
};

nor_config_t norConfig = {
    .memControlConfig = (void *)&spifiMemConfig,
    .driverBaseAddr = (void *)SPIFI0,
};

void spifi_init()
{
    memset(mem_writeBuffer, 0xaa, sizeof(mem_writeBuffer));

    //printf("init\n");
    int status = Nor_Flash_Init(&norConfig, &norHandle);
    if (status != kStatus_Success)
    {
        printf("\r\n***NOR Flash Initialization Failed!***\r\n");
        return;
    }

//    uint32_t address = NOR_FLASH_START_ADDRESS + norHandle.bytesInPageSize * 10;//pageIndex;

/*    printf("erase\n");
    status = Nor_Flash_Erase_Chip(&norHandle);
    if (status != kStatus_Success)
    {
        printf("\r\n***NOR Flash Erase Chip Failed!***\r\n");
        return;
    }*/
#if 0
    printf("program\n");
     status = Nor_Flash_Page_Program(&norHandle, address, mem_writeBuffer);
     if (status != kStatus_Success)
     {
         printf("\r\n***NOR Flash Page %d Program Failed!***\r\n", 5);
         return;
     }

     status = Nor_Flash_Read(&norHandle, address, mem_readBuffer, norHandle.bytesInPageSize);
      if (status != kStatus_Success)
      {
    	  printf("\r\n***NOR Flash Page %d Read Failed!***\r\n", 5);
          return;
      }

      if (memcmp(mem_writeBuffer, mem_readBuffer, norHandle.bytesInPageSize) != 0)
      {
    	  printf("\r\n***NOR Flash Page %d Read/Write Failed!***\r\n", 5);
          return;
      }
#endif
}

void flash_init();

extern "C" void CTIMER0_IRQHandler(void)
{
	// clear all interrupt flags
	CTIMER0->IR = 0xf;

	BaseType_t higherPriorityTaskWoken;
	xSemaphoreGiveFromISR(xBpmTimerSemaphore, &higherPriorityTaskWoken);
	portEND_SWITCHING_ISR(higherPriorityTaskWoken);
}

void arpclock_init()
{
	CLOCK_EnableClock(kCLOCK_Ct32b0);
	RESET_ClearPeripheralReset(kCT32B0_RST_SHIFT_RSTn);

	CTIMER0->TCR = CTIMER_TCR_CRST_MASK;

	CTIMER0->PR = 59; // 180 MHz / 60 = 3 MHz
	CTIMER0->MCR = CTIMER_MCR_MR0I_MASK | CTIMER_MCR_MR0R_MASK | CTIMER_MCR_MR0RL_MASK;
	CTIMER0->CTCR = 0;
	CTIMER0->PWMC = 0;

	// initial value, 1 Hz
	CTIMER0->MR[0] = 180000000/60;
}

void arpclock_run()
{
	// reset and run
	CTIMER0->TCR = CTIMER_TCR_CEN_MASK | CTIMER_TCR_CRST_MASK;

	NVIC_SetPriority(CTIMER0_IRQn, 2);
	NVIC_EnableIRQ(CTIMER0_IRQn);

	CTIMER0->TCR = CTIMER_TCR_CEN_MASK;
}

void arpclock_reset()
{
	CTIMER0->TCR = CTIMER_TCR_CEN_MASK | CTIMER_TCR_CRST_MASK;
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	CTIMER0->TCR = CTIMER_TCR_CEN_MASK;
}

void arpclock_set_speed(float bpm)
{
	// 180 MHz / 24 = 7.5 MHz
	CTIMER0->MSR[0] = (int)((7500000.f /* * 60.f */ ) / bpm);
}

extern "C" void DoMidiCommand(midicmd_t cmd)
{
	key_source_t src = (cmd.header == 0x10) ? key_source_midi : key_source_usb;
	int channel = cmd.b1&0xF;

	if ((cmd.b1 & 0xF0) == 0x90)
	{
		key_down(src, channel, cmd.b2, cmd.b3);
	}
	else if ((cmd.b1 & 0xF0) == 0x80) {
		key_up(src, channel, cmd.b2);
	}
#if 0
	else if (cmd.b1 == 0xF2) {
		int beat = cmd.b2 + cmd.b3 * 0x80;
		for (int i = 0; i < 4; i++) {
			midi_clock_seek(&clock[i], beat);
		}
	}
	else if (cmd.b1 == 0xF8) {
		for (int i = 0; i < 4; i++) {
			midi_clock_step(&clock[i]);
			if (Settings.CVGate.Channels[i].Arp.TimingSource== TimeSource) arpeggiator[i].TimerTick();
		}
	}
	else if (cmd.b1 == 0xFA) {
		// start
		for (int i = 0; i < 4; i++) {
			midi_clock_seek(&clock[i], 0);
			if (Settings.CVGate.Channels[i].Arp.TimingSource== TimeSource) arpeggiator[i].Start();
		}
		play = 1;
	}
	else if (cmd.b1 == 0xFB) {
		// continue
		play = 1;
	}
	else if (cmd.b1 == 0xFC) {
		// stop
		for (int i = 0; i < 4; i++) {
			if (Settings.CVGate.Channels[i].Arp.TimingSource== TimeSource) arpeggiator[i].Stop();
		}
		play = 0;
	}
#endif
}

int main(void) {
  	/* Init board hardware. */
	//POWER_EnablePD(kPDRUNCFG_PD_VD2_ANA);
	//POWER_EnablePD(kPDRUNCFG_PD_VD3);
	//POWER_EnablePD(kPDRUNCFG_PD_VD5);
	//POWER_EnablePD(kPDRUNCFG_PD_USB1_PHY);

	BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    buttonmap_init();
    setupleds();

    LedsOff();
    printf("Board init done\n");

	//POWER_DisablePD(kPDRUNCFG_PD_VD5);
	POWER_DisablePD(kPDRUNCFG_PD_USB1_PHY);
	//CLOCK_EnableClock(kCLOCK_Usbd1);
	//CLOCK_EnableClock(kCLOCK_UsbRam1);
	/* enable usb1 host clock */
	CLOCK_EnableClock(kCLOCK_Usbh1);

	for (volatile int i = 0; i < 10000; i++) ;

	/* According to reference mannual, device mode setting has to be set by access usb host register */
	*((uint32_t *)(USBHSH_BASE + 0x50)) |= USBHSH_PORTMODE_DEV_ENABLE_MASK;

    printf("USB init done\n");

    CLOCK_EnableClock(kCLOCK_Gpio0);
    CLOCK_EnableClock(kCLOCK_Gpio1);
    CLOCK_EnableClock(kCLOCK_Gpio2);
    CLOCK_EnableClock(kCLOCK_Gpio3);

    spifi_init();
    flash_init();

    control_init();

    arpclock_init();

    rpi_init(&rpi_uart);
    sync_init(&rpi_sync, &rpi_uart, sync_data_func, sync_oobdata_func);

    for (int i = 0; i < 4; i++) {
    	direct[i].Init(i);
    	arp[i].Init(&preset.keyzone[i].arpsettings, &direct[i]);
    }

   // while(1) SendLeds();

    xBpmTimerSemaphore = xSemaphoreCreateBinary();
    xKeyTimerSemaphore = xSemaphoreCreateBinary();

    volatile BaseType_t r = 0;
    r = xTaskCreate(KeyboardTask, "keys", 256, NULL, 2, NULL);
    r = xTaskCreate(LedTask, "Leds", 1024, NULL, 3, NULL);
    r = xTaskCreate(ArpTriggerTask, "Arp", 256, NULL, 2, NULL);
    //xTaskCreate(RpiTask, "rpi", 256, NULL, 4, NULL);
    r = xTaskCreate(USBTask, "usb", 2048, NULL, 3, NULL);
    r = xTaskCreate(IdleTask, "idle", 512, NULL, 3, NULL);

    printf("Everything running\n");

    vTaskStartScheduler();

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++ ;
        __NOP();
    }
    return 0 ;
}
