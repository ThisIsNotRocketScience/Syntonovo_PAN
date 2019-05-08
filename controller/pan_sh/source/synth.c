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

void pad_zero();

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

uint32_t porta_timer_count = 0;
int32_t porta_time;
int32_t porta_divider = 256;
int32_t porta_timer_shift = 14;

uint16_t pan_law_table[2049];

static void pan_law_init()
{
	for (int i = 0; i < 2048; i++) {
		float x = sinf((float)i * (0.5f * 3.141592654f / 2048.0f));
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
void update_porta_time(int retrigger)
{
	if (!shiftctrl_flag_state(SELPORTAMENTO)
		|| (shiftctrl_flag_state(SELSTACCATO) && !retrigger))
	{
		// no portamento
		porta_timer_shift = 14;
		porta_time = abs(synth_param[NOTE].value - synth_param[NOTE].last);
		return;
	}

	porta_time = (abs((int32_t)(synth_param[NOTE].value - synth_param[NOTE].last)) * 256) / porta_divider;
	porta_timer_shift = 14;
	int div = porta_divider;
	while (porta_time < 1 && div > 4) {
		porta_timer_shift++;
		div >>= 1;
		porta_time = (abs((int32_t)(synth_param[NOTE].value - synth_param[NOTE].last)) * 256) / div;
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

	if (shiftctrl_flag_state(SELCHASEOSC4567)) value += chase(vco, synth_param[CHASE].last) - 0xffff;
	if (shiftctrl_flag_state(SELSTASHOSC4567)) value += stash(vco, synth_param[STASH].last) - 0xffff;
	return value;
}

int32_t chase_vcf(int vcf)
{
	int32_t value = 0;

	if (shiftctrl_flag_state(SELCHASEVCF2)) value += chase(vcf, synth_param[CHASE].last) - 0xffff;
	if (shiftctrl_flag_state(SELSTASHVCF2)) value += stash(vcf, synth_param[STASH].last) - 0xffff;
	return value;
}

uint16_t synth_mapping_note()
{
	return synth_param[NOTE].value >> 8;
}

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
	while (count--) __NOP();
}

void ports_write_callback(uint32_t data, void* user)
{
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

	nops(5);

	GPIO->B[BOARD_INITPINS_SELINHG_PORT][BOARD_INITPINS_SELINHG_PIN] = 1;

	// wait for charging
	nops(10);

	GPIO->B[BOARD_INITPINS_SELINHG_PORT][BOARD_INITPINS_SELINHG_PIN] = 0;

	ports_strobe = -1;

//	nops(100);
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
static void update_note()
{
	if (notestack_empty()) {
		synth_param[GATE].value = sustain_gate ? 0xFFFF : 0;
		synth_param[RETRIGGER].value = 0;
		virt_GATE();
		virt_RETRIGGER();
		return;
	}

	int note_change = (notestack_first().note << 8) != synth_param[NOTE].value;

	int retrigger = 1;
	if (synth_param[GATE].value != 0xffff) {
		retrigger = 0;
		note_change = 1;
	}

	synth_param[NOTE].value = notestack_first().note << 8;
	update_porta_time(retrigger);
	virt_NOTE();

#if FIXME
	if (note_change || retrigger) {
		for (int i = 0; i < SYNTH_PARAM_COUNT; i++) {
			if (synth_param[i].flags & SubParamFlags_LfoRetrigger) {
				lfo_reset(i, synth_param[i].lfo_reset_phase);
			}
		}
	}
#endif

	if (synth_param[GATE].value != 0xffff) {
		synth_param[GATE].value = 0xffff;
		virt_GATE();
	}

	if (note_change) {
		synth_param[RETRIGGER].value = 2;
		virt_RETRIGGER();
	}
}

void control_cb(int param, int subparam, uint16_t value)
{
	if (param == 0xfe) {
		if (subparam == 0xfe) {
			autotune_start();
			return;
		}

		if (subparam == 0xfd) {
			if ((value & 0xFF00) == 0x200) {
				shiftctrl_set(value & 0x7f);
			}
			else if ((value & 0xFF00) == 0x100) {
				shiftctrl_clear(value & 0x7f);
			}
		}

		if (subparam == 0xfc) {
			notestack_init(value);
			update_note();
		}

		if (subparam == 0xfb) {
			pad_zero();
		}
		return;
	}

	if (param == 0xfc) {
		if (subparam == 2) {
			notestack_push(value & 0xff, value >> 8);

			update_note();
		}
		else if (subparam == 1) {
			notestack_pop(value & 0xff);

			update_note();
		}

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

int process_param_note(int ctrlid, int octctrlid, int32_t notevalue, int modrange)
{
	int result = doing_reset;

	int32_t value = notevalue;

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
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO1_SUB2(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCF1_FX_12(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCF1_FX_24(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCF2_FX_L(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCF2_FX_R(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_CLEANF_FX_L(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_CLEANF_FX_R(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_FX_L_RETURN(int ctrlid, int port)
{
	//do_smooth(ctrlid);
	//if (process_param_inv(ctrlid)) {
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
	//if (process_param_inv(ctrlid)) {
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
	uint16_t value = signed_scale(synth_param[linctrlid].last, pan_law(synth_param[panctrlid].last));

	int result = (synth_param[ctrlid].last != value) || doing_reset;
	if (result) {
		synth_param[ctrlid].last = value;
		ports_value(port, synth_param[ctrlid].last);
	}
}

void linpan_r(int ctrlid, int port, int linctrlid, int panctrlid)
{
	uint16_t value = signed_scale(synth_param[linctrlid].last, pan_law(65535 - synth_param[panctrlid].last));

	int result = (synth_param[ctrlid].last != value) || doing_reset;
	if (result) {
		synth_param[ctrlid].last = value;
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO1_MIX1(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO1_MIX2(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO2_MIX1(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO2_MIX2(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO3_MIX1(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO3_MIX2(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO4567_MIX1(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO4567_MIX2(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_RM1_MIX1(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_RM1_MIX2(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_RM2_MIX3(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
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
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_WHITENS_MIX2(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_DIGINS_MIX1(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_DIGINS_MIX2(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_EXT_MIX1(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_EXT_MIX2(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_DNSSAW_MIX1(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_DNSSAW_MIX2(int ctrlid, int port)
{
	do_smooth(ctrlid);
	if (process_param_inv(ctrlid)) {
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
	int32_t value = signed_scale(synth_param[NOTE].last, synth_param[NOISE_COLOR].note);
	value = note_add(value, note_scale(synth_param[NOISE_COLOR].value, 72 * 0x4000 / 128));

	process_param_note(NOISE_COLOR, VCO8_OCTAVE, value, 24);
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

__attribute__( ( section(".data") ) )
void virt_NOTE()
{
	//synth_param[NOTE].last = note_to_voltage(synth_param[NOTE].value);
	//synth_param[NOTE].last = synth_param[NOTE].value;

	uint32_t timer_count = timer_value_nonisr();
	uint32_t timer_delta = timer_count - porta_timer_count;

	timer_delta >>= porta_timer_shift;
	if (timer_delta == 0) return;
	porta_timer_count = timer_count;
	if (timer_delta > 100) timer_delta = 100;

	int32_t porta_time_scaled = porta_time * (int32_t)timer_delta;

	int32_t newvalue = synth_param[NOTE].last;

	if (newvalue < synth_param[NOTE].value) {
		newvalue += porta_time_scaled;
		if (newvalue > synth_param[NOTE].value) {
			newvalue = synth_param[NOTE].value;
		}
	}
	else if (newvalue > synth_param[NOTE].value) {
		newvalue -= porta_time_scaled;
		if (newvalue < synth_param[NOTE].value) {
			newvalue = synth_param[NOTE].value;
		}
	}
	synth_param[NOTE].last = newvalue;
}

void virt_PORTAMENTO_TIME()
{
	do_smooth(PORTAMENTO_TIME);
	if (synth_param[PORTAMENTO_TIME].value != synth_param[PORTAMENTO_TIME].last) {
		synth_param[PORTAMENTO_TIME].last = synth_param[PORTAMENTO_TIME].value;
		porta_divider = 256 + ((int32_t)synth_param[PORTAMENTO_TIME].value * 8);
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
	int32_t value = note_add(signed_scale(synth_param[NOTE].last, synth_param[VCO1_PITCH].note),
			                 note_scale(synth_param[VCO1_PITCH].value, 24 * 0x4000 / 128));

	process_param_note(VCO1_PITCH, VCO1_OCTAVE, value, 24);
}

void virt_VCO2_PITCH()
{
	do_smooth(VCO2_PITCH);
	int32_t value = signed_scale(synth_param[NOTE].last, synth_param[VCO1_PITCH].note);
	//value = note_add(value, note_scale(synth_param[VCO1_PITCH].value, 24 * 0x4000 / 128));
	value = note_add(value, note_scale(synth_param[VCO2_PITCH].value, 24 * 0x4000 / 128));

	process_param_note(VCO2_PITCH, VCO2_OCTAVE, value, 24);
}

void virt_VCO3_PITCH()
{
	do_smooth(VCO3_PITCH);
	int32_t value = signed_scale(synth_param[NOTE].last, synth_param[VCO1_PITCH].note);
	//value = note_add(value, note_scale(synth_param[VCO1_PITCH].value, 24 * 0x4000 / 128));
	value = note_add(value, note_scale(synth_param[VCO3_PITCH].value, 24 * 0x4000 / 128));

	process_param_note(VCO3_PITCH, VCO3_OCTAVE, value, 24);
}

void virt_VCO4_PITCH()
{
	do_smooth(VCO4_PITCH);
	int32_t value = signed_scale(synth_param[NOTE].last, synth_param[VCO4_PITCH].note);
	value = note_add(value, note_scale(synth_param[VCO4_PITCH].value, 24 * 0x4000 / 128));

	process_param_note(VCO4_PITCH, VCO4_OCTAVE, value, 24);
}

void virt_VCO5_PITCH()
{
	do_smooth(VCO5_PITCH);
	int32_t value = signed_scale(synth_param[NOTE].last, synth_param[VCO4_PITCH].note);
	//value = note_add(value, note_scale(synth_param[VCO4_PITCH].value, 72 * 0x4000 / 128));
	//value += signed_scale(synth_param[VCO5_PITCH].value, 36 * 0x4000 / 256);
	value = note_add(value, note_scale(synth_param[VCO5_PITCH].value, 24 * 0x4000 / 128));

	process_param_note(VCO5_PITCH, VCO5_OCTAVE, value, 24);
}

void virt_VCO6_PITCH()
{
	do_smooth(VCO6_PITCH);
	int32_t value = signed_scale(synth_param[NOTE].last, synth_param[VCO4_PITCH].note);
	//value = note_add(value, note_scale(synth_param[VCO4_PITCH].value, 72 * 0x4000 / 128));
	//value += signed_scale(synth_param[VCO6_PITCH].value, 36 * 0x4000 / 256);
	value = note_add(value, note_scale(synth_param[VCO6_PITCH].value, 24 * 0x4000 / 128));

	process_param_note(VCO6_PITCH, VCO6_OCTAVE, value, 24);
}

void virt_VCO7_PITCH()
{
	do_smooth(VCO7_PITCH);
	int32_t value = signed_scale(synth_param[NOTE].last, synth_param[VCO4_PITCH].note);
	//value = note_add(value, note_scale(synth_param[VCO4_PITCH].value, 72 * 0x4000 / 128));
	//value += signed_scale(synth_param[VCO7_PITCH].value, 36 * 0x4000 / 256);
	value = note_add(value, note_scale(synth_param[VCO7_PITCH].value, 24 * 0x4000 / 128));

	process_param_note(VCO7_PITCH, VCO7_OCTAVE, value, 24);
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

__attribute__( ( section(".data") ) )
void virt_GATE()
{
	synth_param[GATE].last = synth_param[GATE].value;
	if (synth_param[GATE].last) {
		GPIO->B[BOARD_INITPINS_LED_PORT][BOARD_INITPINS_LED_PIN] = 0;
	}
	else {
		GPIO->B[BOARD_INITPINS_LED_PORT][BOARD_INITPINS_LED_PIN] = 1;
	}
}

__attribute__( ( section(".data") ) )
void virt_RETRIGGER()
{
	if (synth_param[RETRIGGER].value) {
		synth_param[RETRIGGER].value--;
	}
	synth_param[RETRIGGER].last = synth_param[RETRIGGER].value;
}

void virt_MASTER_PITCH()
{
	do_smooth(MASTER_PITCH);
	synth_param[MASTER_PITCH].last = (((int32_t)synth_param[MASTER_PITCH].value - 0x8000) * 12) / 128 + 0x8000;
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
	notestack_init(keyboard_mode_last);

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

    //SCTIMER_SetCallback(SCTIMER_1_PERIPHERAL, SCTimer_Func, 0);
    SCTIMER_EnableInterrupts(SCTIMER_1_PERIPHERAL, 3);
    SCTIMER_StartTimer(SCTIMER_1_PERIPHERAL, kSCTIMER_Counter_L);

    // must be lower than autotune priority or overflow checking will break!
    NVIC_SetPriority(SCTIMER_1_IRQN, 1);

    autotune_init();
    pad_init();
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
		scale = controller_param[4].scale >> 4;
		break;
	case 4:
		scale = controller_param[6].scale >> 4;
		break;
	case 5:
		scale = controller_param[8].scale >> 4;
		break;
	case 6:
		scale = controller_param[5].scale >> 4;
		break;
	case 7:
		scale = controller_param[7].scale >> 4;
		break;
	case 8:
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

	if (shiftctrl_flag_state(SELSUSTAINL) && pad_value[PAD_SUSL] > 500 && synth_param[GATE].value == 0xFFFF)
		sustain_gate = 1;
	else if (shiftctrl_flag_state(SELSUSTAINR) && pad_value[PAD_SUSR] > 500 && synth_param[GATE].value == 0xFFFF)
		sustain_gate = 1;
	else {
		sustain_gate = 0;

		if (notestack_empty() && synth_param[GATE].value == 0xFFFF) {
			update_note();
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
		//else if (outputid >= 0x100 && outputid < 0x110) {

		//}
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
	for (int i = 0; i < NUM_LFOS; i++) {
		int modid = 0x00 + i;
		int32_t lfo = (int32_t)lfo_update(i) - 0x8000;
		add_mod_targets(modid, lfo);
	}
	for (int i = 0; i < NUM_ENVS; i++) {
		int modid = 0x10 + i;

		adsr_set_gate(i, synth_param[GATE].value, env_param[i].flags & SubParamFlags_AdsrRetrigger);
		//ad_set_gate(ctrlid, synth_param[GATE].value, synth_param[ctrlid].flags & SubParamFlags_AdRetrigger);

		int32_t adsr = adsr_update(i, una_corda_release);
		add_mod_targets(modid, adsr);
		//uint16_t ad = ad_update(ctrlid, una_corda_release);
	}
	for (int i = 0; i < NUM_CONTROLLERS; i++) {
		int modid = 0x20 + i;
		int32_t value = controller_update(i);
		add_mod_targets(modid, value);
	}
	//for (int i = 0; i < NUM_OPERATORS; i++) {
		//int modid = 0x30 + i;
	//}
}

void synth_run()
{
	for (;;) {
		doing_reset = reset;
		reset = 0;

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
