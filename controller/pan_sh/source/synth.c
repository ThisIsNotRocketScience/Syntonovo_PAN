/*
 * synth.c
 *
 *  Created on: Apr 1, 2018
 *      Author: ld0d
 */

#include <stdint.h>
#include <math.h>

void ports_value(int portid, uint16_t value);

#include "peripherals.h"
#include "pin_mux.h"
#include "max11311.h"
#include "ltc2712.h"
#include "synth.h"
#include "synth_internal.h"
#include "shiftctrl.h"
#include "lfo.h"
#include "adsr.h"
#include "ad.h"
#include "hp.h"

#include "spi_sched.h"
#include "control.h"
#include "autotune.h"
#include "notestack.h"

__attribute__( ( section(".data") ) )
void virt_gate(int gate);

__attribute__( ( section(".data") ) )
void virt_retrigger(int retrigger);

void virt_note(int portaid, int note);

void pad_zero();


volatile int mod_data_done = 0;
volatile uint8_t mod_data[49] = {0};

const int KEYBOARD_X = 0;
const int KEYBOARD_Y = 1;
const int KEYBOARD_Z = 2;
const int PAD_MODL = 6;
const int PAD_SUSL = 7;
const int PAD_UNACL = 8;
const int PAD_MODR = 9;
const int PAD_SUSR = 10;
const int PAD_UNACR = 11;
volatile uint16_t pad_adc_value[12];
volatile uint16_t pad_calibration[12] = {0};
volatile int32_t pad_value[12];

//int32_t pad_adc_value_min[12] = { -0xffffff, -0xffffff, -0xffffff, -0xffffff, -0xffffff, -0xffffff, -0xffffff, -0xffffff, -0xffffff, -0xffffff, -0xffffff, -0xffffff };;
//int32_t pad_adc_value_max[12] = { -0xffffff, -0xffffff, -0xffffff, -0xffffff, -0xffffff, -0xffffff, -0xffffff, -0xffffff, -0xffffff, -0xffffff, -0xffffff, -0xffffff };;

//int32_t pad_adc_value_range[12];

static volatile int inputcycle_last_port = 12;

static void inputcycle_cb(uint32_t data, void* user)
{
	int port = (int)user;

	if (port >= 0 && port < 12) {
		pad_adc_value[port] = (data & 0xFFF) << 4;

		//if (pad_adc_value_min[port] == -0xffffff || pad_adc_value[port] < pad_adc_value_min[port]) pad_adc_value_min[port] = pad_adc_value[port];
		//if (pad_adc_value_max[port] == -0xffffff || pad_adc_value[port] > pad_adc_value_max[port]) pad_adc_value_max[port] = pad_adc_value[port];

		//pad_adc_value_range[port] = pad_adc_value_max[port] - pad_adc_value_min[port];

		inputcycle_last_port = port;

		if (port < 11) {
			max11311_read_nb(0, port + 1, inputcycle_cb, (void*)(port + 1));
		}
	}
}

static void inputcycle_start()
{
	inputcycle_last_port = 0;
	max11311_read_nb(0, 0, inputcycle_cb, (void*)0);
}

static void inputcycle_flush()
{
	while (inputcycle_last_port < 11) ;
}

static volatile int reset = 1;
int doing_reset = 0;

struct peak_state_t
{
	int32_t values[32];
	int index;
};

__attribute__( ( section(".data") ) )
int32_t peak_handle(struct peak_state_t* state, int32_t value)
{
	state->index++;
	state->index &= 31;
	state->values[state->index] = value;

	int32_t max = -0x80000000;
	for (int i = 0; i < 32; i++) {
		if (state->values[i] > max) max = state->values[i];
	}

	return max;
}

struct peak_state_t peak_state_z;

struct hp_state_t zprime_lp;
struct hp_state_t zprime_hp;
int32_t zprime_value;

int sustain_gate = 0;
int una_corda_release = 0;

typedef struct {
	uint32_t porta_timer_count;
	int32_t porta_time;
	int32_t porta_divider;
	int32_t porta_timer_shift;
} porta_state_t;

porta_state_t porta_state[64];

void init_porta_state()
{
	for (int i = 0; i < 64; i++) {
		porta_state[i].porta_timer_count = 0;
		porta_state[i].porta_time = 0;
		porta_state[i].porta_divider = 256;
		porta_state[i].porta_timer_shift = 14;
	}
}

uint16_t pan_law_table[2049];

static void pan_law_init()
{
	for (int i = 0; i < 2048; i++) {
		float x = cosf((float)i * (0.5f * 3.141592654f / 2048.0f));
		pan_law_table[i] = floorf((x * x) * 16384.f);
	}
	pan_law_table[2048] = 0;
}

static uint16_t linint(uint16_t* table, uint32_t phase)
{
	int index = phase >> 21;
	int fract = (phase >> 10) - (index << 11);

	int a = table[index];
	int b = table[index + 1];
	int diff = b - a;

	return (uint16_t) (a + ((diff * fract) >> 11));
}

uint16_t pan_law(uint16_t x)
{
	return linint(pan_law_table, ((uint32_t)x) << 16);
}

#if 0
uint16_t note_to_voltage(int osc, uint16_t value)
{
#define DAC_RANGE 65536.0f
#define NOTES_PER_OCT 12.0f
#define OCTAVES 10.0f
#define NOTE_SUBDIV 256.0f

	int note = value;
	float ctrl_v = (float)note * (DAC_RANGE / (NOTE_SUBDIV * NOTES_PER_OCT * OCTAVES));

	int ictrl = (int32_t)floorf(ctrl_v);
	if (ictrl < 0) return 0;
	else if (ictrl > 65535) return 65535;
	// else
	return (uint16_t) ictrl;
}
#endif

static int32_t note_quantize(int value)
{
	int note = ((int)value - 0x8000) / 0x100;
	return note * 0x100;
}

static int32_t octave_quantize(int value)
{
	int octave = ((int)value - 0x8000) / 0x1000;
	return octave * 0xC00;
}

// scale = -0x8000..0x7fff, 0x4000 ~ 1.0
static int32_t notetrack_scale(uint16_t note_v, int16_t scale)
{
	int32_t r = ((int32_t)note_v - 0x2400) * (int32_t)scale;
	r >>= 14;
	r += 0x2400;

	if (r < 0) return 0;
	else if (r > 65535) return 65535;
	// else
	return (uint16_t) r;
}

// note_v = around 0x8000
// scale = -0x8000..0x7fff, 0x4000 ~ 1.0
static int32_t note_scale(uint16_t note_v, int32_t scale)
{
	int32_t r = ((int32_t)note_v - 0x8000) * scale;
	r >>= 14;
	r += 0x8000;

	//if (r < 0) return 0;
	//else if (r > 65535) return 65535;
	// else
	return r;
}

// add = 0..0xffff, 0x8000 ~ 0.0
static int32_t note_add(int32_t note_v, uint32_t add)
{
	int32_t r = note_v + add - 0x8000;

	//if (r < 0) return 0;
	//else if (r > 65535) return 65535;
	// else
	return r;
}

static int32_t signed_scale(uint16_t value, int16_t scale)
{
	int32_t r = (int32_t)value * (int32_t)scale;
	r >>= 14;

	//if (r < -65536) return -65536;
	//else if (r > 65535) return 65535;
	// else
	return r;
}

static int32_t bipolar_signed_scale(int32_t value, int16_t scale)
{
	int32_t r = value * (int32_t)scale;
	r >>= 15;

	//if (r < -65536) return -65536;
	//else if (r > 65535) return 65535;
	// else
	return r;
}

static int32_t unipolar_unsigned_scale(int32_t value, uint16_t scale)
{
	int32_t r = value * (int32_t)(scale>>1);
	r >>= 15;

	//if (r < -65536) return -65536;
	//else if (r > 65535) return 65535;
	// else
	return r;
}

int32_t abs(int32_t x)
{
	if (x < 0) return -x;
	// else
	return x;
}

__attribute__( ( section(".data") ) )
void update_porta_time(int zone, int note, int retrigger)
{
	porta_state_t* porta = &porta_state[note - 0x100];
	if (!shiftctrl_flag_state(SELPORTAMENTO)
		|| (shiftctrl_flag_state(SELSTACCATO) && !retrigger))
	{
		// no portamento
		porta->porta_timer_shift = 13;
		porta->porta_time = abs(synth_param[note].value - synth_param[note].last);
		return;
	}

	porta->porta_time = (abs((int32_t)(synth_param[note].value - synth_param[note].last)) * 256) / porta->porta_divider;
	porta->porta_timer_shift = 13;
	int div = porta->porta_divider;
	while (porta->porta_time < 1 && div > 4) {
		porta->porta_timer_shift++;
		div >>= 1;
		porta->porta_time = (abs((int32_t)(synth_param[note].value - synth_param[note].last)) * 256) / div;
	}
}

void do_update_smooth(int ctrlid)
{
	const int steps = 100;
	int32_t value = synth_param[ctrlid].value;
	int32_t target = synth_param[ctrlid].target;
	int32_t add = 0;
	if (value < target) {
		add = ((target - value) + steps - 1) / steps;
	}
	else if (value > target) {
		add = (((value - target) + steps - 1) / steps);
	}
	// else add = 0
	synth_param[ctrlid].add = add;
}

void no_smooth(int ctrlid)
{
	synth_param[ctrlid].flags &= ~SubParamFlags_SmoothUpdate;
	synth_param[ctrlid].value = synth_param[ctrlid].target;
}

void do_smooth(int ctrlid)
{
	if (synth_param[ctrlid].flags & SubParamFlags_SmoothUpdate) {
		synth_param[ctrlid].flags &= ~SubParamFlags_SmoothUpdate;
		do_update_smooth(ctrlid);
	}
	int32_t value = synth_param[ctrlid].value;
	int32_t target = synth_param[ctrlid].target;
	if (value < target) {
		value += synth_param[ctrlid].add;
		if (value > target) value = target;
	}
	else if (value > target) {
		value -= synth_param[ctrlid].add;
		if (value < target) value = target;
	}
	synth_param[ctrlid].value = value;
}

int32_t chase(int i, uint16_t value)
{
	int32_t x = 0x4000 - abs((i + 1) * 0x4000 - (int32_t)value);
	if (x < 0) return 0;
	if (x > 0x3FFF) return 0xFFFF;
	return 4 * x;
}

int32_t stash(int i, uint16_t value)
{
	int32_t x = (int32_t)value - i * 0x4000;
	if (x < 0) return 0;
	if (x > 0x3FFF) return 0xFFFF;
	return 4 * x;
}

int32_t chase_vco(int vco)
{
	int32_t value = 0;

//	if (shiftctrl_flag_state(SELCHASEOSC4567)) value += chase(vco, synth_param[CHASE].last) - 0xffff;
//	if (shiftctrl_flag_state(SELSTASHOSC4567)) value += stash(vco, synth_param[STASH].last) - 0xffff;
	return value;
}

int32_t chase_vcf(int vcf)
{
	int32_t value = 0;

//	if (shiftctrl_flag_state(SELCHASEVCF2)) value += chase(vcf, synth_param[CHASE].last) - 0xffff;
//	if (shiftctrl_flag_state(SELSTASHVCF2)) value += stash(vcf, synth_param[STASH].last) - 0xffff;
	return value;
}

//uint16_t synth_mapping_note()
//{
//	return synth_param[NOTE].value >> 8;
//}

void synth_mapping_defaultpatch()
{
	synth_param[VCO1_PITCH].note = 0x4000;
	synth_param[VCO2_PITCH].note = 0x4000;
	synth_param[VCO3_PITCH].note = 0x4000;
	synth_param[VCO4_PITCH].note = 0x4000;
	synth_param[VCO5_PITCH].note = 0x4000;
	synth_param[VCO6_PITCH].note = 0x4000;
	synth_param[VCO7_PITCH].note = 0x4000;
	synth_param[NOISE_COLOR].note = 0x4000;

	shiftctrl_set(SEL1SQR);
	shiftctrl_set(SEL2SQR);
	shiftctrl_set(SEL3SQR);

    for (int i = 0; i < 16; i++) {
		adsr_set_a(i, 0x10);
		adsr_set_d(i, 0x2000);
		adsr_set_s(i, 0x2000 << 16);
		adsr_set_r(i, 0x5000);
    }

	modmatrix[0x10].targets[0].outputid = VCF1_LIN;
	modmatrix[0x10].targets[0].sourceid = 0;
	modmatrix[0x10].targets[0].depth = 0x3fff;
	modmatrix[0x10].targets[1].outputid = VCF1_LIN;
	modmatrix[0x10].targets[1].sourceid = 0;
	modmatrix[0x10].targets[1].depth = 0x3fff;
	modmatrix[0x10].targets[2].outputid = VCF1_LIN;
	modmatrix[0x10].targets[2].sourceid = 0;
	modmatrix[0x10].targets[2].depth = 0x3fff;

	for (int i = 0; i < 64; i++) {
		key_mapping[i].keyindex = 0;
		key_mapping[i].keyzone = 0;
	}
	key_mapping[key_map_target_vco1].keyindex = 0;
	key_mapping[key_map_target_vco2].keyindex = 1;
	key_mapping[key_map_target_vco3].keyindex = 2;

#if FIXME
	synth_param[VCO1_MIX1].adsr_depth = 0x7fff;
#endif
}

volatile int sctimer_state = 0;
volatile int sctimer_counter = 0;
//void SCTimer_Func(void)
//{
//}

__attribute__( ( section(".data") ) )
void SCT0_IRQHandler(void)
{
	__disable_irq();
	sctimer_state = SCT0->STATE;
	sctimer_counter++;

    /* Clear event interrupt flag */
    SCT0->EVFLAG = SCT0->EVFLAG;

	__enable_irq();

    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

__attribute__( ( section(".data") ) )
uint32_t timer_value_nonisr()
{
	__disable_irq();
	uint32_t highcount = sctimer_counter << 16;
	uint32_t timervalue = SCT0->COUNT;

	// but now the counter may have already overflowed! so highcount may be outdated.
	// must read COUNT first, because the overflow may happen also between reading COUNT and STATE.
	int overflow = SCT0->STATE != sctimer_state;
	__enable_irq();

	// if there was an overflow, and the timer was close to beginning of its range,
	// assume that it did indeed overflow since last interrupt.
	if (overflow && timervalue < 32768) {
			highcount += 65536;
	}

	return highcount + timervalue;
}

#if 0
typedef void (*portfunc_write_t)(int ic, int ch, uint16_t value);

portfunc_write_t portfunc_write_func[128] = {0};
int portfunc_write_ic[128] = {0};
int portfunc_write_ch[128] = {0};

void ports_set_write(int portid, portfunc_write_t write_func, int ic, int ch)
{
	portfunc_write_func[portid] = write_func;
	portfunc_write_ic[portid] = ic;
	portfunc_write_ch[portid] = ch;
}
#endif

void ports_dummy_callback(uint32_t data, void* user)
{
}

volatile int ports_strobe = -1;

void nops(int count)
{
    __DSB();
	while (count--) __NOP();
    __DSB();
}

void ports_write_callback(uint32_t data, void* user)
{
	__disable_irq();

	const int A = ((ports_strobe >> 1) & 1) == 0;
	const int B = ((ports_strobe >> 2) & 1) == 0;
	const int C = ((ports_strobe >> 3) & 1) == 0;
	const int D = (ports_strobe >> 4) & 1;
	const int E = (ports_strobe >> 5) & 1;
	const int F = (ports_strobe >> 6) & 1;

	GPIO->B[BOARD_INITPINS_SELA_PORT][BOARD_INITPINS_SELA_PIN] = A;
	GPIO->B[BOARD_INITPINS_SELB_PORT][BOARD_INITPINS_SELB_PIN] = B;
	GPIO->B[BOARD_INITPINS_SELC_PORT][BOARD_INITPINS_SELC_PIN] = C;
	GPIO->B[BOARD_INITPINS_SELD_PORT][BOARD_INITPINS_SELD_PIN] = D;
	GPIO->B[BOARD_INITPINS_SELE_PORT][BOARD_INITPINS_SELE_PIN] = E;
	GPIO->B[BOARD_INITPINS_SELF_PORT][BOARD_INITPINS_SELF_PIN] = F;

	nops(30);

	GPIO->B[BOARD_INITPINS_SELINHG_PORT][BOARD_INITPINS_SELINHG_PIN] = 1;

	// wait for charging
	nops(20);

	GPIO->B[BOARD_INITPINS_SELINHG_PORT][BOARD_INITPINS_SELINHG_PIN] = 0;

	ports_strobe = -1;

//	nops(100);

	__enable_irq();
}

__attribute__( ( section(".data") ) )
void ports_wait_write()
{
	// wait for write strobe
	while (ports_strobe != -1) ;
}

uint16_t ports_last_value[256] = {0};
int ports_last_updated = -1;

int ports_refresh_id = 0;

__attribute__( ( section(".data") ) )
void ports_refresh(void)
{
	// scan through all ports and make sure the s&h are not sagging

	ports_value(ports_refresh_id, ports_last_value[ports_refresh_id]);
	ports_value(ports_refresh_id+1, ports_last_value[ports_refresh_id+1]);
	ports_refresh_id += 0x20;

	if (ports_refresh_id >= 0x60) {
		ports_refresh_id -= 0x60;
	}

	ports_value(ports_refresh_id, ports_last_value[ports_refresh_id]);
	ports_value(ports_refresh_id+1, ports_last_value[ports_refresh_id+1]);
	ports_refresh_id += 0x20;

	if (ports_refresh_id >= 0x60) {
		ports_refresh_id -= 0x60;
	}

	ports_value(ports_refresh_id, ports_last_value[ports_refresh_id]);
	ports_value(ports_refresh_id+1, ports_last_value[ports_refresh_id+1]);
	ports_refresh_id += 0x22;

	if (ports_refresh_id >= 0x60) {
		ports_refresh_id -= 0x60;
	}
}

void ports_flush()
{
	if ((ports_last_updated & 1) == 0)
	{
		ports_value(ports_last_updated + 1, ports_last_value[ports_last_updated + 1]);
		ports_wait_write();
	}
}

__attribute__( ( section(".data") ) )
void ports_value(int portid, uint16_t value)
{
	/*if (portid > 0x10) value = portid&2 ? 0x300 : 0xffff;
	else
	value = 0x9000;*/
	//if (portfunc_write_func[portid]) {
	//	portfunc_write_func[portid] (portfunc_write_ic[portid], portfunc_write_ch[portid], value);
	//}

	ports_wait_write();

	if ((ports_last_updated & 1) == 0 && portid == (ports_last_updated + 1)) {
		ports_strobe = portid;
		ltc2712_write(0, 1, value, ports_write_callback);
	}
	else if ((portid & 1) == 0 && (ports_last_updated & 1) == 1) {
		ltc2712_write(0, 0, value, ports_dummy_callback);
	}
	else if ((ports_last_updated & 1) == 0) {
		ports_strobe = ports_last_updated;
		ltc2712_write(0, 1, ports_last_value[ports_last_updated + 1], ports_write_callback);

		ports_wait_write();

		if ((portid & 1) == 1) {
			ltc2712_write(0, 0, ports_last_value[portid ^ 1], ports_dummy_callback);
			ports_strobe = portid;
			ltc2712_write(0, 1, value, ports_write_callback);
		}
		else {
			ltc2712_write(0, 0, value, ports_dummy_callback);
		}
	}
	else {
		if ((portid & 1) == 1) {
			ltc2712_write(0, 0, ports_last_value[portid ^ 1], ports_dummy_callback);
			ports_strobe = portid;
			ltc2712_write(0, 1, value, ports_write_callback);
		}
		else {
			ltc2712_write(0, 0, value, ports_dummy_callback);
		}
	}
	ports_last_updated = portid;
	ports_last_value[portid] = value;
}

typedef void (*portfunc_read_t)(int ic, int ch, uint16_t* result);

portfunc_read_t portfunc_read_func[16] = {0};
int portfunc_read_ic[128] = {0};
int portfunc_read_ch[128] = {0};

void ports_set_read(int portid, portfunc_read_t read_func, int ic, int ch)
{
	portfunc_read_func[portid] = read_func;
	portfunc_read_ic[portid] = ic;
	portfunc_read_ch[portid] = ch;
}

void ports_input(int portid, uint16_t* value)
{
	if (portfunc_read_func[portid]) {
		portfunc_read_func[portid] (portfunc_read_ic[portid], portfunc_read_ch[portid], value);
	}
}

//__attribute__( ( section(".data") ) )
static void update_note(int zone)
{
	int gateid = GATE1 + zone;
	int retriggerid = RETRIGGER1 + zone;
	int noteid = ZONE1NOTE1 + (zone << 4);

	if (notestack_empty(zone)) {
		synth_param[gateid].value = sustain_gate ? 0xFFFF : 0;
		synth_param[retriggerid].value = 0;
		virt_gate(gateid);
		virt_retrigger(retriggerid);
		return;
	}

	int is_new_note[4];
	int note_value[4];
	int note_change = 0;

	//int polymode = shiftctrl_flag_state(POLYMODE1) | (shiftctrl_flag_state(POLYMODE2) << 1);
	//if (polymode & 1) {
		for (int i = 0; i < 4; i++) {
			int new_note_value = (notestack_n(zone, i).note << 8);
			is_new_note[i] = (new_note_value != synth_param[noteid + i].value);
			if (is_new_note[i]) {
				note_change = 1;
			}
			synth_param[noteid + i].value = new_note_value;
		}
	/*}
	else {
		int new_note_value = (notestack_n(zone, 0).note << 8);
		for (int i = 0; i < 4; i++) {
			is_new_note[i] = (new_note_value != synth_param[noteid + i].value);
			if (is_new_note[i]) {
				note_change = 1;
			}
			synth_param[noteid + i].value = new_note_value;
		}
	}*/

	int retrigger = 1;
	if (synth_param[gateid].value != 0xffff) {
		retrigger = 0;
		note_change = 1;
	}

	for (int i = 0; i < 4; i++) {
		if (is_new_note[i]) {
			update_porta_time(zone, noteid + i, retrigger);
			virt_note(i, noteid + i);
		}
	}

	if (synth_param[gateid].value != 0xffff) {
		synth_param[gateid].value = 0xffff;
		virt_gate(gateid);
	}

	if (!(shiftctrl_flag_state(SELPORTAMENTO) && retrigger))
	{
		if (note_change || retrigger) {
			synth_param[retriggerid].value = 2;
			virt_retrigger(retriggerid);

			for (int i = 0; i < 16; i++) {
				if (lfo_param[i].flags & LfoParamFlags_LfoRetrigger) {
					lfo_reset(i, lfo_param[i].reset_phase);
				}
			}
			for (int i = 0; i < 16; i++) {
				if (env_param[i].flags & SubParamFlags_AdsrRetrigger) {
					adsr_set_gate(i, 0, 0);
					adsr_set_gate(i, synth_param[GATE1 + key_mapping[key_map_target_env1 + i].keyzone].value, 0);
				}
			}
		}
	}
}

volatile int do_autotune = 0;
volatile int got_input = 0;

void control_cb(int param, int subparam, uint16_t value)
{
	got_input = 1;
	if (param == 0xfe) {
		if (subparam == 0xfe) {
			do_autotune = 1;
			return;
		}

		if (subparam == 0xfd) {
			if ((value & 0xFF00) == 0x200) {
				shiftctrl_set(value & 0x7f);
			}
			else if ((value & 0xFF00) == 0x100) {
				shiftctrl_clear(value & 0x7f);
			}

			/*
			if ((value & 0x7f) == KEYPRIO1 || (value & 0x7f) == KEYPRIO2) {
				notestack_init(0, shiftctrl_flag_state(KEYPRIO1) | (shiftctrl_flag_state(KEYPRIO2) << 1));
				notestack_init(1, shiftctrl_flag_state(KEYPRIO1) | (shiftctrl_flag_state(KEYPRIO2) << 1));
				notestack_init(2, shiftctrl_flag_state(KEYPRIO1) | (shiftctrl_flag_state(KEYPRIO2) << 1));
				notestack_init(3, shiftctrl_flag_state(KEYPRIO1) | (shiftctrl_flag_state(KEYPRIO2) << 1));
				update_note(0);
			}
			*/
		}

		if (subparam == 0xfc) {
			int zone = (value >> 3) & 3;
			notestack_init(zone, value & 7);
			update_note(zone);
		}

		if (subparam == 0xfb) {
			pad_zero();
		}

		if (subparam == 0xfa) {
			if (!mod_data_done) {
				mod_data_done = 1;
			}
		}
		return;
	}

	if (param == 0xfc) {
		int zone = (subparam >> 4) & 3;
		if ((subparam & 0xf) == 2) {
			notestack_push(zone, value & 0xff, value >> 8);
			update_note(zone);
		}
		else if ((subparam & 0xf) == 1) {
			notestack_pop(zone, value & 0xff);
			update_note(zone);
		}

		return;
	}
	if (param == 0xfb) {
		int tgt = subparam & 0x3f;
		int zone = value >> 4;
		int keyindex = value & 0x0f;
		key_mapping[tgt].keyzone = zone;
		key_mapping[tgt].keyindex = keyindex;
		return;
	}

	if (subparam & 0x80) {
		// modsource
		subparam &= 0x7f;

		if (param >= 0x00 && param < 0x00 + NUM_LFOS) {
			switch (subparam) {
			case 0:
				lfo_param[param - 0x00].flags = value;
				break;
			case 1:
				lfo_param[param - 0x00].speed = value;
				lfo_set_speed(param - 0x00, value);
				break;
			case 2:
				lfo_param[param - 0x00].depth = value;
				break;
			case 3:
				lfo_param[param - 0x00].shape = value;
				lfo_set_shape(param - 0x00, value);
				break;
			case 4:
				lfo_param[param - 0x00].reset_phase = value;
				break;
			}
		}
		else if (param >= 0x10 && param < 0x10 + NUM_ENVS) {
			switch (subparam) {
			case 0:
				env_param[param - 0x10].flags = value;
				break;
			case 1:
				env_param[param - 0x10].a = value;
				adsr_set_a(param - 0x10, value);
				break;
			case 2:
				env_param[param - 0x10].d = value;
				adsr_set_d(param - 0x10, value);
				break;
			case 3:
				env_param[param - 0x10].s = value;
				adsr_set_s(param - 0x10, value << 16);
				break;
			case 4:
				env_param[param - 0x10].r = value;
				adsr_set_r(param - 0x10, value);
				break;
			}
		}
		else if (param >= 0x20 && param < 0x20 + NUM_CONTROLLERS) {
			switch (subparam) {
			case 0:
				controller_param[param - 0x20].scale = value;
				break;
			case 1:
				controller_param[param - 0x20].deadzone = value;
				break;
			}
		}
		else if (param >= 0x30 && param < 0x30 + NUM_OPERATORS) {
			switch (subparam) {
			case 0:
				op_param[param - 0x30].modsource1 = value;
				break;
			case 1:
				op_param[param - 0x30].modsource2 = value;
				break;
			case 2:
				op_param[param - 0x30].op = value;
				break;
			case 3:
				op_param[param - 0x30].parameter = value;
				break;
			}
		}

		return;
	}
	else if (subparam & 0x40) {
		subparam &= 0x3f;

		int subsubparam = subparam & 3;
		subparam >>= 2;

		if (subparam >= MODTARGET_COUNT)
			return;

		if (subsubparam == 2)
			modmatrix[param].targets[subparam].sourceid = value;
		else if (subsubparam == 1)
			modmatrix[param].targets[subparam].outputid = value;
		else
			modmatrix[param].targets[subparam].depth = value;

		return;
	}

	switch (subparam) {
	case 0:
		synth_param[param].target = value;
		synth_param[param].value = value;
		synth_param[param].add = 0;
		break;
	case 1:
		synth_param[param].note = value;
		break;
	case 2:
		synth_param[param].target = value;
		synth_param[param].flags |= SubParamFlags_SmoothUpdate;
		break;
	case 32:
		synth_param[param].flags = value;
		break;
	}
}

int process_param_lin(int ctrlid)
{
	int result = doing_reset;

	int32_t value = synth_param[ctrlid].value + synth_param[ctrlid].sum;

	if (value < 0) value = 0;
	else if (value > 65535) value = 65535;

	result |= (synth_param[ctrlid].last != value);
	synth_param[ctrlid].last = value;

	return result;
}

int process_param_inv(int ctrlid)
{
	int result = doing_reset;

	int32_t value = synth_param[ctrlid].value + synth_param[ctrlid].sum;

	if (value < 0) value = 65535;
	else if (value > 65535) value = 0;
	else value = 65535 - value;

	result |= (synth_param[ctrlid].last != value);
	synth_param[ctrlid].last = value;

	return result;
}

int32_t log_curve(int32_t value)
{
	if (value < 0x1000) return value * 8;
	else if (value < 0x2000) return (value - 0x1000) * 4 + 0x8000;
	else if (value < 0xE000) return ((value - 0x2000) >> 2) + 0xC000;
	else return ((value - 0xE000) >> 1) + 0xF000;
}

int process_param_log_add(int ctrlid, int32_t add, int32_t addchase)
{
	int result = doing_reset;

	int32_t value = (int32_t)synth_param[ctrlid].value + add;
	//printf("%d = %d + %d\n", value, synth_param[ctrlid].value, add);
	if (value < 0) value = 0;
	else if (value > 65535) value = 65535;
	value += addchase;
	if (value < 0) value = 0;
	else if (value > 65535) value = 65535;
	else value = log_curve(value);
	//printf("+ sum\n", synth_param[ctrlid].sum),
	value += synth_param[ctrlid].sum;
	if (value < 0) value = 65535;
	else if (value > 65535) value = 0;
	else value = 65535 - value;
	//printf("= %d\n", value);

	result |= (synth_param[ctrlid].last != value);
	synth_param[ctrlid].last = value;

	return result;
}

int process_param_log(int ctrlid)
{
	return process_param_log_add(ctrlid, 0, 0);
}

int process_param_note(int ctrlid, int coarsectrlid, int octctrlid, int32_t notevalue, int modrange)
{
	int result = doing_reset;

	int32_t value = notevalue;

	value += note_quantize(synth_param[coarsectrlid].last);
	value += octave_quantize(synth_param[octctrlid].last);
	value += (int32_t)synth_param[MASTER_PITCH].last - 0x8000;
	value += (int32_t)synth_param[MASTER_PITCH2].last - 0x8000;
	int modvalue = synth_param[ctrlid].sum;
	value += bipolar_signed_scale(modvalue, modrange * (0x8000 / 128));

	if (value < 0) value = 0;
	else if (value > 65535) value = 65535;

	result |= (synth_param[ctrlid].last != value);
	synth_param[ctrlid].last = value;

	return result;
}

void do_output_lin(int ctrlid, int port)
{
	if (process_param_lin(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_inv(int ctrlid, int port)
{
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_log(int ctrlid, int port)
{
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO4_DRY_MIX(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log_add(ctrlid, (int32_t)synth_param[VCO4567_DRY_MIX].last, chase_vco(0))) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO5_DRY_MIX(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log_add(ctrlid, synth_param[VCO4567_DRY_MIX].last, chase_vco(1))) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO6_DRY_MIX(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log_add(ctrlid, synth_param[VCO4567_DRY_MIX].last, chase_vco(2))) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO7_DRY_MIX(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log_add(ctrlid, synth_param[VCO4567_DRY_MIX].last, chase_vco(3))) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCF2_A_MIX(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log_add(ctrlid, 0, chase_vcf(0))) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCF2_B_MIX(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log_add(ctrlid, 0, chase_vcf(1))) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCF2_C_MIX(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log_add(ctrlid, 0, chase_vcf(2))) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCF2_D_MIX(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log_add(ctrlid, 0, chase_vcf(3))) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO1_SUB1(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO1_SUB2(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCF1_FX_12(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCF1_FX_24(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCF2_FX_L(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCF2_FX_R(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_CLEANF_FX_L(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_CLEANF_FX_R(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_FX_L_RETURN(int ctrlid, int port)
{
	//do_smooth(ctrlid);
	//if (process_param_log(ctrlid)) {
	//	ports_value(port, synth_param[ctrlid].last);
	//}

	int result = (synth_param[FX_L_RETURN].last != synth_param[FX_RET_LEVEL].last) || doing_reset;
	synth_param[FX_L_RETURN].last = synth_param[FX_RET_LEVEL].last;
	if (result) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_FX_R_RETURN(int ctrlid, int port)
{
	//do_smooth(ctrlid);
	//if (process_param_log(ctrlid)) {
	//	ports_value(port, synth_param[ctrlid].last);
	//}

	int result = (synth_param[FX_R_RETURN].last != synth_param[FX_RET_LEVEL].last) || doing_reset;
	synth_param[FX_R_RETURN].last = synth_param[FX_RET_LEVEL].last;
	if (result) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void linpan_l(int ctrlid, int port, int linctrlid, int panctrlid)
{
	uint16_t value = signed_scale(synth_param[linctrlid].last, pan_law(65535 - synth_param[panctrlid].last));

	int result = (synth_param[ctrlid].last != value) || doing_reset;
	if (result) {
		synth_param[ctrlid].last = value;
		ports_value(port, synth_param[ctrlid].last);
	}
}

void linpan_r(int ctrlid, int port, int linctrlid, int panctrlid)
{
	uint16_t value = signed_scale(synth_param[linctrlid].last, pan_law(synth_param[panctrlid].last));

	int result = (synth_param[ctrlid].last != value) || doing_reset;
	if (result) {
		synth_param[ctrlid].last = value;
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_GATETRIG_MIX2(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO1_MIX1(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO1_MIX2(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO2_MIX1(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO2_MIX2(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO3_MIX1(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO3_MIX2(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO4567_MIX1(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO4567_MIX2(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_RM1_MIX1(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_RM1_MIX2(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_RM2_MIX3(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}

/*	int value = synth_param[RM1_LEVEL].last;
	int result = (synth_param[ctrlid].last != value) || doing_reset;
	if (result) {
		synth_param[ctrlid].last = value;
		ports_value(port, synth_param[ctrlid].last);
	}*/
}

void do_output_WHITENS_MIX1(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_WHITENS_MIX2(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_DIGINS_MIX1(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_DIGINS_MIX2(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_EXT_MIX1(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_EXT_MIX2(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_DNSSAW_MIX1(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_DNSSAW_MIX2(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_log(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_CLEANF_L_LIN(int ctrlid, int port)
{
	linpan_l(ctrlid, port, CLEANF_LIN, CLEANF_PAN);
}

void do_output_CLEANF_R_LIN(int ctrlid, int port)
{
	linpan_r(ctrlid, port, CLEANF_LIN, CLEANF_PAN);
}

void do_output_VCF1_L_LIN(int ctrlid, int port)
{
	linpan_l(ctrlid, port, VCF1_LIN, VCF1_PAN);
}

void do_output_VCF1_R_LIN(int ctrlid, int port)
{
	linpan_r(ctrlid, port, VCF1_LIN, VCF1_PAN);
}

void do_output_VCF2_L_LIN(int ctrlid, int port)
{
	// fix: VCF2 panning is reversed
	linpan_r(ctrlid, port, VCF2_LIN, VCF2_PAN);
}

void do_output_VCF2_R_LIN(int ctrlid, int port)
{
	// fix: VCF2 panning is reversed
	linpan_l(ctrlid, port, VCF2_LIN, VCF2_PAN);
}

void do_output_CLEANF_L_LOG(int ctrlid, int port)
{
	int result = (synth_param[CLEANF_L_LOG].last != synth_param[CLEANF_LEVEL].last) || doing_reset;
	synth_param[CLEANF_L_LOG].last = synth_param[CLEANF_LEVEL].last;
	if (result) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_CLEANF_R_LOG(int ctrlid, int port)
{
	int result = (synth_param[CLEANF_R_LOG].last != synth_param[CLEANF_LEVEL].last) || doing_reset;
	synth_param[CLEANF_R_LOG].last = synth_param[CLEANF_LEVEL].last;
	if (result) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCF1_L_LOG(int ctrlid, int port)
{
	int result = (synth_param[VCF1_L_LOG].last != synth_param[VCF1_LEVEL].last) || doing_reset;
	synth_param[VCF1_L_LOG].last = synth_param[VCF1_LEVEL].last;
	if (result) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCF1_R_LOG(int ctrlid, int port)
{
	int result = (synth_param[VCF1_R_LOG].last != synth_param[VCF1_LEVEL].last) || doing_reset;
	synth_param[VCF1_R_LOG].last = synth_param[VCF1_LEVEL].last;
	if (result) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCF2_L_LOG(int ctrlid, int port)
{
	int result = (synth_param[VCF2_L_LOG].last != synth_param[VCF2_LEVEL].last) || doing_reset;
	synth_param[VCF2_L_LOG].last = synth_param[VCF2_LEVEL].last;
	if (result) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCF2_R_LOG(int ctrlid, int port)
{
	int result = (synth_param[VCF2_R_LOG].last != synth_param[VCF2_LEVEL].last) || doing_reset;
	synth_param[VCF2_R_LOG].last = synth_param[VCF2_LEVEL].last;
	if (result) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_NOISE_COLOR(int ctrlid, int port)
{
	do_smooth(NOISE_COLOR);

	int noteparam = 0x100 + (key_mapping[key_map_target_vco8].keyzone << 4) + key_mapping[key_map_target_vco8].keyindex;

	int32_t value = signed_scale(synth_param[noteparam].last, synth_param[NOISE_COLOR].note);
	value = note_add(value, note_scale(synth_param[NOISE_COLOR].value, 72 * 0x4000 / 128));

	process_param_note(NOISE_COLOR, VCO8_COARSE, VCO8_OCTAVE, value, 24);
}

void do_output_VCO1_FREQ(int ctrlid, int port)
{
}

void do_output_VCO2_FREQ(int ctrlid, int port)
{
}

void do_output_VCO3_FREQ(int ctrlid, int port)
{
}

void do_output_VCO4_FREQ(int ctrlid, int port)
{
}

void do_output_VCO5_FREQ(int ctrlid, int port)
{
}

void do_output_VCO6_FREQ(int ctrlid, int port)
{
}

void do_output_VCO7_FREQ(int ctrlid, int port)
{
}

void virt_CLEANF_LIN()
{
	do_smooth(CLEANF_LIN);
	process_param_lin(CLEANF_LIN);
}

void virt_VCF1_LIN()
{
	do_smooth(VCF1_LIN);
	process_param_lin(VCF1_LIN);
}

void virt_VCF2_LIN()
{
	do_smooth(VCF2_LIN);
	process_param_lin(VCF2_LIN);
}

void virt_CLEANF_LEVEL()
{
	do_smooth(CLEANF_LEVEL);
	process_param_log_add(CLEANF_LEVEL, 0/*(int32_t)synth_param[MASTER_LEVEL].last - 0xFFFF*/, 0);
}

void virt_VCF1_LEVEL()
{
	do_smooth(VCF1_LEVEL);
	//printf("VCF1\n");
	process_param_log_add(VCF1_LEVEL, 0/*(int32_t)synth_param[MASTER_LEVEL].last - 0xFFFF*/, 0);
	//printf("---\n");
}

void virt_VCF2_LEVEL()
{
	do_smooth(VCF2_LEVEL);
	process_param_log_add(VCF2_LEVEL, 0/*(int32_t)synth_param[MASTER_LEVEL].last - 0xFFFF*/, 0);
}

void virt_CLEANF_PAN()
{
	do_smooth(CLEANF_PAN);
	process_param_lin(CLEANF_PAN);
}

void virt_VCF1_PAN()
{
	do_smooth(VCF1_PAN);
	process_param_lin(VCF1_PAN);
}

void virt_VCF2_PAN()
{
	do_smooth(VCF2_PAN);
	process_param_lin(VCF2_PAN);
}

void virt_FX_RET_LEVEL()
{
	do_smooth(FX_RET_LEVEL);
	process_param_log_add(FX_RET_LEVEL, 0/*(int32_t)synth_param[MASTER_LEVEL].last - 0xFFFF*/, 0);
}

/*void virt_FX_RET_PAN()
{
	do_smooth(FX_RET_PAN);
	process_param_lin(FX_RET_PAN);
}*/

void virt_VCO4567_DRY_MIX()
{
	do_smooth(VCO4567_DRY_MIX);
	process_param_lin(VCO4567_DRY_MIX);
}

int update_porta(int id)
{
	uint32_t timer_count = timer_value_nonisr();
	uint32_t timer_delta = timer_count - porta_state[id].porta_timer_count;

	timer_delta >>= porta_state[id].porta_timer_shift;
	if (timer_delta == 0) return 0;
	porta_state[id].porta_timer_count = timer_count;
	if (timer_delta > 100) timer_delta = 100;

	return porta_state[0].porta_time * (int32_t)timer_delta;
}

void virt_note(int portaid, int note)
{
	if (synth_param[note].last == synth_param[note].value) return;

	int porta_time_scaled = update_porta(portaid);
	int32_t newvalue = synth_param[note].last;

	if (newvalue < synth_param[note].value) {
		newvalue += porta_time_scaled;
		if (newvalue > synth_param[note].value) {
			newvalue = synth_param[note].value;
		}
	}
	else if (newvalue > synth_param[note].value) {
		newvalue -= porta_time_scaled;
		if (newvalue < synth_param[note].value) {
			newvalue = synth_param[note].value;
		}
	}
	synth_param[note].last = newvalue;
}

#define _VIRT_NOTE(NAME) \
	__attribute__( ( section(".data") ) ) \
	void virt_ ## NAME() \
	{ \
		virt_note(NAME - ZONE1NOTE1, NAME); \
	}

_VIRT_NOTE(ZONE1NOTE1);
_VIRT_NOTE(ZONE1NOTE2);
_VIRT_NOTE(ZONE1NOTE3);
_VIRT_NOTE(ZONE1NOTE4);
_VIRT_NOTE(ZONE1NOTE5);
_VIRT_NOTE(ZONE1NOTE6);
_VIRT_NOTE(ZONE1NOTE7);
_VIRT_NOTE(ZONE1NOTE8);
_VIRT_NOTE(ZONE2NOTE1);
_VIRT_NOTE(ZONE2NOTE2);
_VIRT_NOTE(ZONE2NOTE3);
_VIRT_NOTE(ZONE2NOTE4);
_VIRT_NOTE(ZONE2NOTE5);
_VIRT_NOTE(ZONE2NOTE6);
_VIRT_NOTE(ZONE2NOTE7);
_VIRT_NOTE(ZONE2NOTE8);
_VIRT_NOTE(ZONE3NOTE1);
_VIRT_NOTE(ZONE3NOTE2);
_VIRT_NOTE(ZONE3NOTE3);
_VIRT_NOTE(ZONE3NOTE4);
_VIRT_NOTE(ZONE3NOTE5);
_VIRT_NOTE(ZONE3NOTE6);
_VIRT_NOTE(ZONE3NOTE7);
_VIRT_NOTE(ZONE3NOTE8);
_VIRT_NOTE(ZONE4NOTE1);
_VIRT_NOTE(ZONE4NOTE2);
_VIRT_NOTE(ZONE4NOTE3);
_VIRT_NOTE(ZONE4NOTE4);
_VIRT_NOTE(ZONE4NOTE5);
_VIRT_NOTE(ZONE4NOTE6);
_VIRT_NOTE(ZONE4NOTE7);
_VIRT_NOTE(ZONE4NOTE8);

void virt_PORTAMENTO_TIME()
{
	do_smooth(PORTAMENTO_TIME);
	if (synth_param[PORTAMENTO_TIME].value != synth_param[PORTAMENTO_TIME].last) {
		synth_param[PORTAMENTO_TIME].last = synth_param[PORTAMENTO_TIME].value;
		int value = (int32_t)synth_param[PORTAMENTO_TIME].value;
		if (value < 0x1000) {
			porta_state[0].porta_divider = 256 + ((int32_t)synth_param[PORTAMENTO_TIME].value);
		}
		else if (value < 0x2000) {
			porta_state[0].porta_divider = 256 + ((int32_t)synth_param[PORTAMENTO_TIME].value) + ((int32_t)synth_param[PORTAMENTO_TIME].value - 0x1000) * 2;
		}
		else if (value < 0x4000) {
			porta_state[0].porta_divider = 256 + ((int32_t)synth_param[PORTAMENTO_TIME].value) + ((int32_t)synth_param[PORTAMENTO_TIME].value - 0x1000) * 2 + ((int32_t)synth_param[PORTAMENTO_TIME].value - 0x2000) * 4;
		}
		else {
			porta_state[0].porta_divider = 256 + ((int32_t)synth_param[PORTAMENTO_TIME].value) + ((int32_t)synth_param[PORTAMENTO_TIME].value - 0x1000) * 2 + ((int32_t)synth_param[PORTAMENTO_TIME].value - 0x2000) * 4 + ((int32_t)synth_param[PORTAMENTO_TIME].value - 0x4000) * 16;
		}
	}
}

/*void virt_MASTER_LEVEL()
{
	do_smooth(MASTER_LEVEL);
	synth_param[MASTER_LEVEL].last = synth_param[MASTER_LEVEL].value;
}*/

void virt_ZPRIME_SPEED()
{
	if (synth_param[ZPRIME_SPEED].value != synth_param[ZPRIME_SPEED].last) {
		hp_set_speed(&zprime_hp, synth_param[ZPRIME_SPEED].value);
		synth_param[ZPRIME_SPEED].last = synth_param[ZPRIME_SPEED].value;
	}
}

void virt_VCO1_PITCH()
{
	do_smooth(VCO1_PITCH);

	int noteparam = 0x100 + (key_mapping[key_map_target_vco1].keyzone << 4) + key_mapping[key_map_target_vco1].keyindex;

	int32_t value = note_add(signed_scale(synth_param[noteparam].last, synth_param[VCO1_PITCH].note),
			                 note_scale(synth_param[VCO1_PITCH].value, 24 * 0x4000 / 128));

	process_param_note(VCO1_PITCH, VCO1_COARSE, VCO1_OCTAVE, value, 24);
}
/*
int32_t note_distance(int notectrl, int polymode)
{
	int curmode = shiftctrl_flag_state(POLYMODE1) | (shiftctrl_flag_state(POLYMODE2) << 1);
	if (curmode != polymode) return 0;
	return (int32_t)synth_param[notectrl].value - (int32_t)synth_param[ZONE1NOTE1].value;
}
*/
uint16_t add_clamp_signed(uint16_t a, int16_t b)
{
	int value = (int32_t)a + (int32_t)b;
	if (value < 0) return 0;
	else if (value > 65535) return 65535;
	return (uint16_t)value;
}

void virt_VCO2_PITCH()
{
	do_smooth(VCO2_PITCH);

	int noteparam = 0x100 + (key_mapping[key_map_target_vco2].keyzone << 4) + key_mapping[key_map_target_vco2].keyindex;
	int32_t value = note_add(signed_scale(synth_param[noteparam].last, synth_param[VCO2_PITCH].note),
			                 note_scale(synth_param[VCO2_PITCH].value, 24 * 0x4000 / 128));

	//int add = note_distance(noteparam, 1);
	//int32_t value = signed_scale(add_clamp_signed(synth_param[noteparam].last, add), synth_param[VCO1_PITCH].note);
	//value = note_add(value, note_scale(synth_param[VCO1_PITCH].value, 24 * 0x4000 / 128));
	//value = note_add(value, note_scale(synth_param[VCO2_PITCH].value, 24 * 0x4000 / 128));

	process_param_note(VCO2_PITCH, VCO2_COARSE, VCO2_OCTAVE, value, 24);
}

void virt_VCO3_PITCH()
{
	do_smooth(VCO3_PITCH);

	int noteparam = 0x100 + (key_mapping[key_map_target_vco3].keyzone << 4) + key_mapping[key_map_target_vco3].keyindex;
	int32_t value = note_add(signed_scale(synth_param[noteparam].last, synth_param[VCO3_PITCH].note),
			                 note_scale(synth_param[VCO3_PITCH].value, 24 * 0x4000 / 128));

	//int add = note_distance(NOTE3, 1);
	//int32_t value = signed_scale(add_clamp_signed(synth_param[NOTE].last, add), synth_param[VCO1_PITCH].note);
	//value = note_add(value, note_scale(synth_param[VCO1_PITCH].value, 24 * 0x4000 / 128));
	//value = note_add(value, note_scale(synth_param[VCO3_PITCH].value, 24 * 0x4000 / 128));

	process_param_note(VCO3_PITCH, VCO3_COARSE, VCO3_OCTAVE, value, 24);
}

void virt_VCO4_PITCH()
{
	do_smooth(VCO4_PITCH);
	//int32_t value = signed_scale(synth_param[NOTE].last, synth_param[VCO4_PITCH].note);
	//value = note_add(value, note_scale(synth_param[VCO4_PITCH].value, 24 * 0x4000 / 128));

	int noteparam = 0x100 + (key_mapping[key_map_target_vco4].keyzone << 4) + key_mapping[key_map_target_vco4].keyindex;
	int32_t value = note_add(signed_scale(synth_param[noteparam].last, synth_param[VCO4_PITCH].note),
			                 note_scale(synth_param[VCO4_PITCH].value, 24 * 0x4000 / 128));

	process_param_note(VCO4_PITCH, VCO4_COARSE, VCO4_OCTAVE, value, 24);
}

void virt_VCO5_PITCH()
{
	do_smooth(VCO5_PITCH);
	//int32_t value = signed_scale(synth_param[NOTE].last, synth_param[VCO4_PITCH].note);
	//value = note_add(value, note_scale(synth_param[VCO4_PITCH].value, 72 * 0x4000 / 128));
	//value += signed_scale(synth_param[VCO5_PITCH].value, 36 * 0x4000 / 256);
	//value = note_add(value, note_scale(synth_param[VCO5_PITCH].value, 24 * 0x4000 / 128));

	int noteparam = 0x100 + (key_mapping[key_map_target_vco5].keyzone << 4) + key_mapping[key_map_target_vco5].keyindex;
	int32_t value = note_add(signed_scale(synth_param[noteparam].last, synth_param[VCO5_PITCH].note),
			                 note_scale(synth_param[VCO5_PITCH].value, 24 * 0x4000 / 128));

	process_param_note(VCO5_PITCH, VCO5_COARSE, VCO5_OCTAVE, value, 24);
}

void virt_VCO6_PITCH()
{
	do_smooth(VCO6_PITCH);
	//int32_t value = signed_scale(synth_param[NOTE].last, synth_param[VCO4_PITCH].note);
	//value = note_add(value, note_scale(synth_param[VCO4_PITCH].value, 72 * 0x4000 / 128));
	//value += signed_scale(synth_param[VCO6_PITCH].value, 36 * 0x4000 / 256);
	//value = note_add(value, note_scale(synth_param[VCO6_PITCH].value, 24 * 0x4000 / 128));

	int noteparam = 0x100 + (key_mapping[key_map_target_vco6].keyzone << 4) + key_mapping[key_map_target_vco6].keyindex;
	int32_t value = note_add(signed_scale(synth_param[noteparam].last, synth_param[VCO6_PITCH].note),
			                 note_scale(synth_param[VCO6_PITCH].value, 24 * 0x4000 / 128));

	process_param_note(VCO6_PITCH, VCO6_COARSE, VCO6_OCTAVE, value, 24);
}

void virt_VCO7_PITCH()
{
	do_smooth(VCO7_PITCH);
	//int32_t value = signed_scale(synth_param[NOTE].last, synth_param[VCO4_PITCH].note);
	//value = note_add(value, note_scale(synth_param[VCO4_PITCH].value, 72 * 0x4000 / 128));
	//value += signed_scale(synth_param[VCO7_PITCH].value, 36 * 0x4000 / 256);
	//value = note_add(value, note_scale(synth_param[VCO7_PITCH].value, 24 * 0x4000 / 128));

	int noteparam = 0x100 + (key_mapping[key_map_target_vco7].keyzone << 4) + key_mapping[key_map_target_vco7].keyindex;
	int32_t value = note_add(signed_scale(synth_param[noteparam].last, synth_param[VCO7_PITCH].note),
			                 note_scale(synth_param[VCO7_PITCH].value, 24 * 0x4000 / 128));

	process_param_note(VCO7_PITCH, VCO7_COARSE, VCO7_OCTAVE, value, 24);
}

/*
void virt_VCO8_PITCH()
{
	do_smooth(VCO8_PITCH);
	int32_t value = signed_scale(synth_param[NOTE].last, synth_param[VCO8_PITCH].note);
	value = note_add(value, note_scale(synth_param[VCO8_PITCH].value, 72 * 0x4000 / 128));

	process_param_note(VCO8_PITCH, value, 24);
}*/

void virt_VCO1_OCTAVE()
{
	no_smooth(VCO1_OCTAVE);
	synth_param[VCO1_OCTAVE].last = synth_param[VCO1_OCTAVE].value;
}

void virt_VCO2_OCTAVE()
{
	no_smooth(VCO2_OCTAVE);
	synth_param[VCO2_OCTAVE].last = synth_param[VCO2_OCTAVE].value;
}

void virt_VCO3_OCTAVE()
{
	no_smooth(VCO3_OCTAVE);
	synth_param[VCO3_OCTAVE].last = synth_param[VCO3_OCTAVE].value;
}

void virt_VCO4_OCTAVE()
{
	no_smooth(VCO4_OCTAVE);
	synth_param[VCO4_OCTAVE].last = synth_param[VCO4_OCTAVE].value;
}

void virt_VCO5_OCTAVE()
{
	no_smooth(VCO5_OCTAVE);
	synth_param[VCO5_OCTAVE].last = synth_param[VCO5_OCTAVE].value;
}

void virt_VCO6_OCTAVE()
{
	no_smooth(VCO6_OCTAVE);
	synth_param[VCO6_OCTAVE].last = synth_param[VCO6_OCTAVE].value;
}

void virt_VCO7_OCTAVE()
{
	no_smooth(VCO7_OCTAVE);
	synth_param[VCO7_OCTAVE].last = synth_param[VCO7_OCTAVE].value;
}

void virt_VCO8_OCTAVE()
{
	no_smooth(VCO8_OCTAVE);
	synth_param[VCO8_OCTAVE].last = synth_param[VCO8_OCTAVE].value;
}

void virt_VCO1_COARSE()
{
	no_smooth(VCO1_COARSE);
	synth_param[VCO1_COARSE].last = synth_param[VCO1_COARSE].value;
}

void virt_VCO2_COARSE()
{
	no_smooth(VCO2_COARSE);
	synth_param[VCO2_COARSE].last = synth_param[VCO2_COARSE].value;
}

void virt_VCO3_COARSE()
{
	no_smooth(VCO3_COARSE);
	synth_param[VCO3_COARSE].last = synth_param[VCO3_COARSE].value;
}

void virt_VCO4_COARSE()
{
	no_smooth(VCO4_COARSE);
	synth_param[VCO4_COARSE].last = synth_param[VCO4_COARSE].value;
}

void virt_VCO5_COARSE()
{
	no_smooth(VCO5_COARSE);
	synth_param[VCO5_COARSE].last = synth_param[VCO5_COARSE].value;
}

void virt_VCO6_COARSE()
{
	no_smooth(VCO6_COARSE);
	synth_param[VCO6_COARSE].last = synth_param[VCO6_COARSE].value;
}

void virt_VCO7_COARSE()
{
	no_smooth(VCO7_COARSE);
	synth_param[VCO7_COARSE].last = synth_param[VCO7_COARSE].value;
}

void virt_VCO8_COARSE()
{
	no_smooth(VCO8_COARSE);
	synth_param[VCO8_COARSE].last = synth_param[VCO8_COARSE].value;
}

void update_gateled()
{
	if (synth_param[GATE1].last | synth_param[GATE2].last | synth_param[GATE3].last | synth_param[GATE4].last) {
		GPIO->B[BOARD_INITPINS_LED_PORT][BOARD_INITPINS_LED_PIN] = 0;
	}
	else {
		GPIO->B[BOARD_INITPINS_LED_PORT][BOARD_INITPINS_LED_PIN] = 1;
	}
}

__attribute__( ( section(".data") ) )
void virt_gate(int gate)
{
	synth_param[gate].last = synth_param[gate].value;
}

__attribute__( ( section(".data") ) )
void virt_GATE1()
{
	virt_gate(GATE1);
	update_gateled();
}

__attribute__( ( section(".data") ) )
void virt_GATE2()
{
	virt_gate(GATE2);
}

__attribute__( ( section(".data") ) )
void virt_GATE3()
{
	virt_gate(GATE3);
}

__attribute__( ( section(".data") ) )
void virt_GATE4()
{
	virt_gate(GATE4);
}

void virt_retrigger(int retrigger)
{
	if (synth_param[retrigger].value) {
		synth_param[retrigger].value--;
	}
	synth_param[retrigger].last = synth_param[retrigger].value;
}

__attribute__( ( section(".data") ) )
void virt_RETRIGGER1()
{
	virt_retrigger(RETRIGGER1);
}

__attribute__( ( section(".data") ) )
void virt_RETRIGGER2()
{
	virt_retrigger(RETRIGGER2);
}

__attribute__( ( section(".data") ) )
void virt_RETRIGGER3()
{
	virt_retrigger(RETRIGGER3);
}

__attribute__( ( section(".data") ) )
void virt_RETRIGGER4()
{
	virt_retrigger(RETRIGGER4);
}

void virt_MASTER_PITCH()
{
	do_smooth(MASTER_PITCH);

	int32_t value = (((int32_t)synth_param[MASTER_PITCH].value - 0x8000) * 12) / 128 + 0x8000;
	int modvalue = synth_param[MASTER_PITCH].sum;
	value += modvalue;
	if (value < 0) value = 0;
	else if (value > 65535) value = 65535;

	synth_param[MASTER_PITCH].last = value;
}

void virt_MASTER_PITCH2()
{
	synth_param[MASTER_PITCH2].value = synth_param[MASTER_PITCH2].target;
	synth_param[MASTER_PITCH2].last = synth_param[MASTER_PITCH2].value;
}

void virt_CHASE()
{
	do_smooth(CHASE);
	process_param_lin(CHASE);
}

void virt_STASH()
{
	do_smooth(STASH);
	process_param_lin(STASH);
}

void synth_mapping_virt()
{
	static uint16_t last[8] = { 0xfffe, 0xfffe, 0xfffe, 0xfffe, 0xfffe, 0xfffe, 0xfffe, 0xfffe };

	uint16_t val = autotune_note_to_dac(0, synth_param[VCO1_PITCH].last);
	if (val != last[0] || doing_reset) {
		ports_value(synth_param[VCO1_FREQ].port, val);
		last[0] = val;
	}

	val = autotune_note_to_dac(1, synth_param[VCO2_PITCH].last);
	if (val != last[1] || doing_reset) {
		ports_value(synth_param[VCO2_FREQ].port, val);
		last[1] = val;
	}

	val = autotune_note_to_dac(2, synth_param[VCO3_PITCH].last);
	if (val != last[2] || doing_reset) {
		ports_value(synth_param[VCO3_FREQ].port, val);
		last[2] = val;
	}

	val = autotune_note_to_dac(3, synth_param[VCO4_PITCH].last);
	if (val != last[3] || doing_reset) {
		ports_value(synth_param[VCO4_FREQ].port, val);
		last[3] = val;
	}

	val = autotune_note_to_dac(4, synth_param[VCO5_PITCH].last);
	if (val != last[4] || doing_reset) {
		ports_value(synth_param[VCO5_FREQ].port, val);
		last[4] = val;
	}

	val = autotune_note_to_dac(5, synth_param[VCO6_PITCH].last);
	if (val != last[5] || doing_reset) {
		ports_value(synth_param[VCO6_FREQ].port, val);
		last[5] = val;
	}

	val = autotune_note_to_dac(6, synth_param[VCO7_PITCH].last);
	if (val != last[6] || doing_reset) {
		ports_value(synth_param[VCO7_FREQ].port, val);
		last[6] = val;
	}

	val = autotune_note_to_dac(7, synth_param[NOISE_COLOR].last);
	if (val != last[7] || doing_reset) {
		ports_value(synth_param[NOISE_COLOR].port, val);
		last[7] = val;
	}
}

void pad_zero()
{
	inputcycle_start();
	inputcycle_flush();

	for (int i = 0; i < 12; i++) {
		pad_calibration[i] = pad_adc_value[i];
	}
}

void pad_init()
{
	pad_zero();
}

void synth_init()
{
    shiftctrl_clear(SELOUTPUT);

	notestack_init(0, keyboard_mode_last);
	notestack_init(1, keyboard_mode_last);
	notestack_init(2, keyboard_mode_last);
	notestack_init(3, keyboard_mode_last);

	for (int i = 0; i < SYNTH_MODSOURCE_COUNT; i++) {
		for(int k = 0; k < MODTARGET_COUNT; k++) {
			modmatrix[i].targets[k].outputid = 0xffff;
			modmatrix[i].targets[k].sourceid = 0;
			modmatrix[i].targets[k].depth = 0;
		}
	}

	synth_mapping_init();

	// start with master level OFF
	synth_param[MASTER_LEVEL].last = 0;
	synth_param[MASTER_LEVEL].value = 0;

	pan_law_init();
    lfo_init();
    adsr_init();
    ad_init();
    hp_init(&zprime_lp);
    hp_init(&zprime_hp);

	hp_set_speed(&zprime_lp, 0xF000);

    //shiftctrl_set(SEL1TRI);
    //shiftctrl_set(SEL1SAW);
    shiftctrl_set(SEL1SQR);
    shiftctrl_set(SEL2SQR);
    //shiftctrl_set(SEL1SUB);
    //shiftctrl_set(SEL4SAW);
    //shiftctrl_set(SELMOST3);
    //shiftctrl_set(SELVCF2L0);
    //shiftctrl_set(SELVCF2L1);
    //shiftctrl_set(SELEFFECT1);

	//synth_param[VCF2_H_CV].lfo_depth = 0x8000;
	//synth_param[VCF2_H_CV].lfo_speed = 0xffff;
	//lfo_set_speed(VCF2_H_CV, synth_param[VCF2_H_CV].lfo_speed);

    sctimer_init();

    autotune_init();
    pad_init();
}

void sctimer_init()
{
    //SCTIMER_SetCallback(SCTIMER_1_PERIPHERAL, SCTimer_Func, 0);
    SCTIMER_EnableInterrupts(SCTIMER_1_PERIPHERAL, 3);
    SCTIMER_StartTimer(SCTIMER_1_PERIPHERAL, kSCTIMER_Counter_L);

    // must be lower than autotune priority or overflow checking will break!
    NVIC_SetPriority(SCTIMER_1_IRQN, 1);
}

const int negate[12] = { 1, 0, 1,  0, 0, 0,  1, 0, 0,  1, 0, 0 };

//__attribute__( ( section(".data") ) )
int32_t pad_threshold(int32_t value, int i)
{
	int dz = 0;
	int scale = 0x100;

	if (negate[i]) value = -value;

	switch (i) {
	case 0:
		dz = controller_param[0].deadzone >> 3;
		scale = controller_param[0].scale >> 3;
		break;
	case 1:
		dz = controller_param[1].deadzone >> 3;
		scale = controller_param[1].scale >> 3;
		break;
	case 2:
		dz = controller_param[2].deadzone >> 3;
		scale = controller_param[2].scale >> 3;
		break;
	case 3:
		dz = controller_param[3].deadzone >> 3;
		scale = controller_param[3].scale >> 4;
		break;
	case 4:
		dz = controller_param[6].deadzone >> 3;
		scale = controller_param[6].scale >> 4;
		break;
	case 5:
		dz = controller_param[8].deadzone >> 3;
		scale = controller_param[8].scale >> 4;
		break;
	case 6: // modl
		dz = controller_param[4].deadzone >> 3;
		scale = controller_param[4].scale >> 4;
		break;
	case 7: // susl
		dz = controller_param[6].deadzone >> 3;
		scale = controller_param[6].scale >> 4;
		break;
	case 8: // unal
		dz = controller_param[8].deadzone >> 3;
		scale = controller_param[8].scale >> 4;
		break;
	case 9: // modr
		dz = controller_param[5].deadzone >> 3;
		scale = controller_param[5].scale >> 4;
		break;
	case 10: // susr
		dz = controller_param[7].deadzone >> 3;
		scale = controller_param[7].scale >> 4;
		break;
	case 11: // unar
		dz = controller_param[9].deadzone >> 3;
		scale = controller_param[9].scale >> 4;
		break;
	}

	if (value < -dz) value += dz;
	else if (value > dz) value -= dz;
	else return 0;

	value = (value * scale) / 0x100;
	if (value < -32767) return -32767;
	else if (value > 32767) value = 32767;
	return value;
}

//__attribute__( ( section(".data") ) )
static void process_inputs()
{
	for (int i = 0; i < 12; i++) {
		pad_value[i] = pad_threshold(((int32_t)pad_adc_value[i] - (int32_t)pad_calibration[i]), i);
	}

	pad_value[KEYBOARD_Z] = peak_handle(&peak_state_z, pad_value[KEYBOARD_Z]);

	int32_t zprime_tmp = lp_update(&zprime_lp, pad_value[KEYBOARD_Z]);
	zprime_value = hp_update(&zprime_hp, zprime_tmp);

	int any_gate_open = 0;
	for (int i = 0; i < 4; i++) {
		if (synth_param[GATE1 + i].value == 0xFFFF) {
			any_gate_open = 1;
			break;
		}
	}

	if (shiftctrl_flag_state(SELSUSTAINL) && pad_value[PAD_SUSL] > 500 && any_gate_open)
		sustain_gate = 1;
	else if (shiftctrl_flag_state(SELSUSTAINR) && pad_value[PAD_SUSR] > 500 && any_gate_open)
		sustain_gate = 1;
	else {
		sustain_gate = 0;

		for (int i = 0; i < 4; i++) {
			if (notestack_empty(i) && synth_param[GATE1 + i].value == 0xFFFF) {
				update_note(i);
			}
		}
	}

	una_corda_release = 0;
	if (shiftctrl_flag_state(SELUNACL) && pad_value[PAD_UNACL] > 500) {
		una_corda_release += (pad_value[PAD_UNACL] - 500) << 8;
	}
	if (shiftctrl_flag_state(SELUNACR) && pad_value[PAD_UNACR] > 500) {
		una_corda_release += (pad_value[PAD_UNACR] - 500) << 8;
	}
}

void add_mod_targets(int modid, int32_t value)
{
	for (int k = 0; k < NUM_CONTROLLERS; k++) {
		int outputid = modmatrix[modid].targets[k].outputid;
		if (outputid >= 0 && outputid < 0xf0) {
			int depth = modmatrix[modid].targets[k].depth;
			synth_param[outputid].sum += bipolar_signed_scale(value, depth);
		}
		else if (outputid >= 0x100 && outputid < 0x120) {
			int depth = modmatrix[modid].targets[k].depth;
			lfo_mod[outputid - 0x100].sum += bipolar_signed_scale(value, depth);
		}
	}
}

int32_t controller_update(int ctrlid)
{
	int32_t value = 0;

	switch (ctrlid) {
	case 0:
		return pad_value[KEYBOARD_X];
	case 1:
		return pad_value[KEYBOARD_Y];
	case 2:
		return pad_value[KEYBOARD_Z];
	case 3:
		return zprime_value;
	case 4:
		return pad_value[PAD_MODL];
	case 5:
		return pad_value[PAD_MODR];
	case 6:
		return pad_value[PAD_SUSL];
	case 7:
		return pad_value[PAD_SUSR];
	case 8:
		return pad_value[PAD_UNACL];
	case 9:
		return pad_value[PAD_UNACR];
	case 10:
		return 0;//pedalxxxx;
	}

	return value;
}

void synth_modulation_run()
{
	int read_mods = mod_data_done;

	for (int i = 0; i < NUM_LFOS; i++) {
		if (lfo_mod[i].add != lfo_mod[i].sum) {
			lfo_mod[i].add = lfo_mod[i].sum;
			lfo_set_speed(i, add_clamp_signed(lfo_param[i].speed, lfo_mod[i].sum));
			lfo_set_shape(i, add_clamp_signed(lfo_param[i].shape, lfo_mod[i + NUM_LFOS].sum));
		}
		lfo_mod[i].sum = 0;
	}
	for (int i = 0; i < NUM_LFOS; i++) {
		int modid = 0x00 + i;
		int32_t lfo = (int32_t)lfo_update(i);
		if (read_mods) {
			mod_data[modid+1] = (uint8_t)(lfo >> 8);
		}
		add_mod_targets(modid, lfo);
	}
	for (int i = 0; i < NUM_ENVS; i++) {
		int modid = 0x10 + i;

		adsr_set_gate(i, synth_param[GATE1 + key_mapping[key_map_target_env1 + i].keyzone].value, env_param[i].flags & SubParamFlags_AdsrRetrigger);
		//ad_set_gate(ctrlid, synth_param[GATE].value, synth_param[ctrlid].flags & SubParamFlags_AdRetrigger);

		int32_t adsr = adsr_update(i, una_corda_release);
		if (read_mods) {
			mod_data[modid+1] = (uint8_t)(adsr >> 9);
		}
		add_mod_targets(modid, adsr);
		//uint16_t ad = ad_update(ctrlid, una_corda_release);
	}
	for (int i = 0; i < NUM_CONTROLLERS; i++) {
		int modid = 0x20 + i;
		int32_t value = controller_update(i);
		if (read_mods) {
			int ctrlval = value;
			if (i >= 6 && i <= 9) {
				ctrlval = ((int)value) >> 6;
				if (ctrlval < 0) ctrlval = 0;
				else if (ctrlval > 255) ctrlval = 255;
			}
			else {
				ctrlval = value >> 8;
			}
			mod_data[modid+1] = (uint8_t)(ctrlval);
		}
		add_mod_targets(modid, value);
	}
	//for (int i = 0; i < NUM_OPERATORS; i++) {
		//int modid = 0x30 + i;
	//}

	if (read_mods) {
		mod_data_done = 0;
		mod_data[0] = control_state_mod_fb;
		control_out_queue(mod_data, sizeof(mod_data));
	}
}

void synth_run()
{
	for (;;) {
		if (do_autotune) {
			control_set_receive(0);
			autotune_start();
			do_autotune = 0;
			control_set_receive(1);
		}

		doing_reset = reset;
		reset = 0;

		if (got_input) {
			shiftctrl_set(SELOUTPUT);
		}

		inputcycle_start();

		synth_mapping_reset();

		synth_modulation_run();

	    synth_mapping_run();
	    synth_mapping_virt();

	    inputcycle_flush();

	    process_inputs();

		ports_refresh();
		shiftctrl_update();
	}
}

void synth_reset()
{
	//reset = 1;
}
