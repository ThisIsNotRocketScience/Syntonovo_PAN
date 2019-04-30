/*
 * Copyright 2016-2018 NXP Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    usbmidiio2.cpp
 * @brief   Application entry point.
 */
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

/* TODO: insert other definitions and declarations here. */

void preset_init();

PanPreset_t preset;
PanLedState_t ledstate;

uart_t rpi_uart;
sync_state_t rpi_sync;

#define NOTEON(note, velocity) dsp_cmd(0xfc, 2, note | (velocity << 8))
#define NOTEOFF(note) dsp_cmd(0xfc, 1, note)

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

BaseType_t keys_press(int keyindex, uint32_t timediff)
{
	NOTEON(keyindex, 0xff);
	return pdFALSE;
}

BaseType_t keys_release(int keyindex)
{
	NOTEOFF(keyindex);
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

int Cur = 0;



//int colorset1[24]= {255,255,0,  255,0,255,  0,255,255,  255,0,0,   0,255,0 ,  0,0,255,    255,255,255,  255,255,255};
//int colorset1[24]= {255,255,0,  255,0,255,  0,255,255,  255,0,0,   0,255,0 ,  0,0,255,    255,255,255,  255,255,255};

//                    brg      brg       brg
int colorset1[24*2]= {0,40,0,  0,60,0,  0,80,0,  0,100,0,   0,110,0,   0,255,0,   0,400,0,  0,500,0};

int colorset2[24*2]= {0,0,40,  0,0,60,  0,0,80,  0,0,100,   0,0,110 ,  0,0,255,    0,0,400,  0,0,500};

void SetColor(int Led, int r, int g, int b)
{
	int idx = (Led) % 16;
	int set = (Led) / 16;
	int *colorset = set>0?colorset2:colorset1;
	colorset[idx * 3 + 0] = b >> 4; // XXX TODO DIM LEDS
	colorset[idx * 3 + 1] = r >> 4;
	colorset[idx * 3 + 2] = g >> 4;


}

void PWM2Set(int off)
{
	for (int c = 0;c<24;c++)
		{
			// 24 channels per TLC5947
			// 12 bits per channel, send MSB first
			for (int8_t b = 11; b >= 0; b--) {
				L2ClkOff();

				if (colorset2[c + off] & (1 << b))
				{ L2DatOn();}
				else
				{L2DatOff();}

				L2ClkOn();
			}
		}
		L2ClkOff();

}

void Led2bits(int inp)
{
	for(int i =0 ;i<16;i++)
	{
		L2DatOn();
		L2ClkOn();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		L2ClkOff();
	}
}




void PWM1Set(int off)
{
	for (int c = 0;c<24;c++)
	{
		// 24 channels per TLC5974
		// 12 bits per channel, send MSB first
		for (int8_t b = 11; b >= 0; b--) {
			L1ClkOff();

			if (colorset1[c + off] & (1 << b))
			{ L1DatOn();}
			else
			{L1DatOff();}

			L1ClkOn();
		}
	}
	L1ClkOff();


}

void Led1bits(int inp)
{
	for(int i =0 ;i<16;i++)
	{
		L1ClkOff();
		L1DatOn();
		L1ClkOn();
	}
	L1ClkOff();
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
		//*ptr = (*ptr & 0xF0) | ((value >> 8) & 0x0F);
		//ptr++;
		//*ptr = value & 0xFF;
		*ptr = (value >> 4) & 0xFF;
		ptr++;
		*ptr = (*ptr & 0x0F) | ((value << 4) & 0xF0);
	}
	else {
		//*ptr = (value >> 4) & 0xFF;
		//ptr++;
		//*ptr = (*ptr & 0x0F) | ((value >> 4) & 0xF0);
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
	UNUSED(0);//LEDBUTTON(0, RSus);
	UNUSED(0);//LEDBUTTON(0, RUna);
	UNUSED(0);//LEDBUTTON(0, RMod);
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

	UNUSED(1);//LEDBUTTON(1, LHome);
	UNUSED(1);//LEDBUTTON(1, Lseq2);
	UNUSED(1);//LEDBUTTON(1, Lseq1);
	UNUSED(1);//LEDBUTTON(1, LSus);
	UNUSED(1);//LEDBUTTON(1, LUna);
	UNUSED(1);//LEDBUTTON(1, LMod);
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

	//printf("indexes %d  %d\n", index1, index2);
}

#undef ENCODER
#undef LEDBUTTON

void SendLeds()
{
	//memset(&leddata1, 0, sizeof(leddata1));
	//memset(&leddata2, 0, sizeof(leddata2));

	//Set12b(encoderindex[3] + 12, 0x800);
	//Set12b(encoderindex[4] + 12, 0x800);

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

	L1LatchOff();
	L2LatchOff();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	L1LatchOn();
	L2LatchOn();
}


#define OOB_UI_PAUSE		(0x41)
#define OOB_UI_CONTINUE		(0x42)
#define OOB_BUTTON_DOWN		(0x50)
#define OOB_BUTTON_UP		(0x51)
#define OOB_ENCODER_CCW		(0x56)
#define OOB_ENCODER_CW		(0x57)
#define OOB_ENCODER_DOWN	(0x58)
#define OOB_ENCODER_UP		(0x59)
#define OOB_SWITCH_CHANGE	(0x5F)

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

			return dir;
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

	for (int i = 0; i < 160; i++) {
		if (sw[i] != last_sw[i]) { //printf("sw %d\n", PINMANGLE(i));
			data[0] = PINMANGLE(i);
			sync_oob_word(&rpi_sync, OOB_SWITCH_CHANGE, *(uint32_t*)data, 0);
		}
	}
	memcpy(last_sw, sw, 160);

	for (int i = 0; i < __ledbutton_Count; i++) {
		if (switch_sw[i] == 0) {
			// sw[0] is bound to an encoder so we can easily skip unbound buttons
			continue;
		}
		int move = switch_process(&switch_state[i], sw[switch_sw[i]]);

		if (move > 0) {
			data[0] = i;
			sync_oob_word(&rpi_sync, OOB_BUTTON_UP, *(uint32_t*)data, 0);
		}
		else if (move < 0) {
			data[0] = i;
			sync_oob_word(&rpi_sync, OOB_BUTTON_DOWN, *(uint32_t*)data, 0);
		}
	}

	for (int i = 0; i < __encoder_Count; i++) {
		int move = encoder_process(i, sw[encoder_a[i]], sw[encoder_c[i]]);

		if (move < 0) {
			data[0] = i;
			sync_oob_word(&rpi_sync, OOB_ENCODER_CCW, *(uint32_t*)data, 0);
		}
		else if (move > 0) {
			data[0] = i;
			sync_oob_word(&rpi_sync, OOB_ENCODER_CW, *(uint32_t*)data, 0);
		}

		move = switch_process(&encoder_sw_state[i], sw[encoder_sw[i]]);
		if (move > 0) {
			data[0] = i;
			sync_oob_word(&rpi_sync, OOB_ENCODER_UP, *(uint32_t*)data, 0);
		}
		else if (move < 0) {
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

uint32_t timer_value()
{
	static uint32_t value = 0;

	value++;
	return value;
}

extern "C" void SCT0_IRQHandler(void)
{
	BaseType_t higherPriorityTaskWoken;
	xSemaphoreGiveFromISR(xKeyTimerSemaphore, &higherPriorityTaskWoken);

	SCTIMER_ClearStatusFlags(SCTIMER_1_PERIPHERAL, 1);

	portEND_SWITCHING_ISR(higherPriorityTaskWoken);
}

void sync_complete(int status)
{
	if (status != 0) {
        sync_oob_word(&rpi_sync, OOB_UI_PAUSE, 0, 0);
	    sync_block(&rpi_sync, (uint8_t*)&preset, 0, sizeof(preset), sync_complete);
		return;
	}

	if (status == 0) {
        sync_oob_word(&rpi_sync, OOB_UI_CONTINUE, 0, 0);
        return;
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
		                    printf("%d make\n", keyindex);
	                        uint32_t timediff = timervalue - keyscantimer[keyindex];
	                        higherPriorityTaskWoken |= keys_press(keyindex, timediff);
	                        //notestack_push(keyindex + 36);
	                        //midikeypress = 1;
	                }
	                else if ((T & (1 << i)) != 0
	                                && (prevT & (1 << i)) == 0) {
	                        // released
	                        int keyindex = (keyscanindex >> 1) * 8 + i;
		                    printf("%d release\n", keyindex);
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
	                    printf("%d break\n", (keyscanindex >> 1) * 8 + i);
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

void KeyboardTask(void* pvParameters)
{
	dsp_reset();
	preset_init();

	rpi_start();

    sync_block(&rpi_sync, (uint8_t*)&preset, 0, sizeof(preset), sync_complete);

	NVIC_SetPriority(SCTIMER_1_IRQN, 2);
    SCTIMER_EnableInterrupts(SCTIMER_1_PERIPHERAL, 1);

	while (1) {
        if( xSemaphoreTake( xKeyTimerSemaphore, ( TickType_t ) 10 ) == pdTRUE )
        {
        	KeyScan();
            ScanButtonsAndEncoders();
        }
		//vTaskDelay(1);
	}
}

#define FULLVALUE(param, subparam, value) dsp_cmd(param, subparam, value)
#define SETMODSOURCE(source, subparam, value) dsp_cmd(source, (subparam) | 0x80, value)
#define MODMATRIX(param, target, subparam, value) dsp_cmd(param, ((((target) << 2) & 0xF) + ((subparam) & 3)) | 0x40, value)

#define SWITCH_ON(sw) FULLVALUE(0xfe, 0xfd, 0x200 | (sw));
#define SWITCH_OFF(sw) FULLVALUE(0xfe, 0xfd, 0x100 | (sw));

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
	}
	// operators
	for (int param = 0; param < 16; param++) {
		SETMODSOURCE(param + 0x30, 0, 0xff);
		SETMODSOURCE(param + 0x30, 1, 0xff);
		SETMODSOURCE(param + 0x30, 2, 0);
		SETMODSOURCE(param + 0x30, 3, 0);
	}

	// modmatrix
	for (int param = 0; param < 0x40; param++) {
		for (int target = 0; target < 11; target++) {
				MODMATRIX(param, target, 0, 0);
				MODMATRIX(param, target, 1, 0);
				MODMATRIX(param, target, 2, 0);
			}
	}

	// parameters
	for (int param = 0; param < 0xf0; param++) {
		FULLVALUE(param, 0, 0);
		FULLVALUE(param, 1, 0);
		FULLVALUE(param, 2, 0);
	}

#define OUTPUT(NAME, x, y, z, w, k, VALUE) FULLVALUE(output_##NAME, 0, VALUE);
#define OUTPUT_VIRT(NAME, x, y, z, w, k, VALUE) FULLVALUE(output_##NAME, 0, VALUE);

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

	for (int i = 0; i < 64; i++) {
	    SWITCH_OFF(i);
	}
    SWITCH_ON(switch_SEL1SQR);
    SWITCH_ON(switch_SEL2SQR);
    SWITCH_ON(switch_SEL3SQR);

    for (int i = 0; i < 16; i++) {
		SETMODSOURCE(i + modsource_ENV0, 1, 0x10);
		SETMODSOURCE(i + modsource_ENV0, 2, 0x2000);
		SETMODSOURCE(i + modsource_ENV0, 3, 0x2000);
		SETMODSOURCE(i + modsource_ENV0, 4, 0x5000);
    }

    MODMATRIX(modsource_ENV0, 0, 0, 0x3fff);
    MODMATRIX(modsource_ENV0, 0, 1, output_VCF1_LIN);
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

void IdleTask(void* pvParameters)
{
	while(1) {
		rpi_uart.config.timer_tick(rpi_uart.config.timer_tick_data);
		vTaskDelay(50);
	}
}

//#define offsetof(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))

void sync_preset(uint32_t addr)
{
	if (addr >= offsetof(PanPreset_t, Name)) {
		addr -= offsetof(PanPreset_t, Name);
		// name
	}
	else if (addr >= offsetof(PanPreset_t, op)) {
		addr -= offsetof(PanPreset_t, op);
	}
	else if (addr >= offsetof(PanPreset_t, controller)) {
		addr -= offsetof(PanPreset_t, controller);
	}
	else if (addr >= offsetof(PanPreset_t, env)) {
		addr -= offsetof(PanPreset_t, env);
	}
	else if (addr >= offsetof(PanPreset_t, lfo)) {
		addr -= offsetof(PanPreset_t, lfo);
	}
	else if (addr >= offsetof(PanPreset_t, modmatrix)) {
		addr -= offsetof(PanPreset_t, modmatrix);
	}
	else if (addr >= offsetof(PanPreset_t, paramvalue)) {
		addr -= offsetof(PanPreset_t, paramvalue);

		int paramid = addr * 2;
		FULLVALUE(paramid, 0, preset.paramvalue[paramid]);
		FULLVALUE(paramid+1, 0, preset.paramvalue[paramid+1]);
	}
	else if (addr >= offsetof(PanPreset_t, switches)) {
		addr -= offsetof(PanPreset_t, switches);

		for (int i = 0; i < 32; i++) {
			if (preset.switches[addr] & (1 << i)) {
				SWITCH_ON(addr * 32 + i);
			}
			else {
				SWITCH_OFF(addr * 32 + i);
			}
		}
	}
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
	return 0;
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

int main(void) {
  	/* Init board hardware. */
	BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    buttonmap_init();
    setupleds();

    LedsOff();
    LedsOn();
    printf("Board init done\n");

    POWER_DisablePD(kPDRUNCFG_PD_USB1_PHY);
    /* enable usb1 host clock */
    CLOCK_EnableClock(kCLOCK_Usbh1);
    /* According to reference mannual, device mode setting has to be set by access usb host register */
    *((uint32_t *)(USBHSH_BASE + 0x50)) |= USBHSH_PORTMODE_DEV_ENABLE_MASK;
    /* disable usb1 host clock */
    CLOCK_DisableClock(kCLOCK_Usbh1);

    POWER_EnablePD(kPDRUNCFG_PD_VD5);
    POWER_EnablePD(kPDRUNCFG_PD_USB1_PHY);

    printf("USB init done\n");
    //POWER_EnablePD(kPDRUNCFG_PD_VD3);

    CLOCK_EnableClock(kCLOCK_Gpio0);
    CLOCK_EnableClock(kCLOCK_Gpio1);
    CLOCK_EnableClock(kCLOCK_Gpio2);
    CLOCK_EnableClock(kCLOCK_Gpio3);

    control_init();

    rpi_init(&rpi_uart);
    sync_init(&rpi_sync, &rpi_uart, sync_data_func, sync_oobdata_func);

   // while(1) SendLeds();


    xKeyTimerSemaphore = xSemaphoreCreateBinary();

    volatile BaseType_t r = 0;
    r = xTaskCreate(KeyboardTask, "keys", 256, NULL, 2, NULL);
    r = xTaskCreate(LedTask, "Leds", 1024, NULL, 3, NULL);
    //xTaskCreate(RpiTask, "rpi", 256, NULL, 4, NULL);
    //xTaskCreate(USBTask, "usb", 512, NULL, 3, NULL);
    r = xTaskCreate(IdleTask, "idle", 512, NULL, 3, NULL);

    printf("Everything running\n");

    vTaskStartScheduler();

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++ ;
    }
    return 0 ;
}
