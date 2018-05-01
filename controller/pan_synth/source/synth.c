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

static volatile int reset = 0;
int doing_reset = 0;

struct peak_state_t
{
	int32_t values[32];
	int index;
};

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

void pad_zero();

const int KEYBOARD_X = 0;
const int KEYBOARD_Y = 1;
const int KEYBOARD_Z = 2;
const int PAD_MODL = 3;
const int PAD_SUSL = 4;
const int PAD_UNACL = 5;
const int PAD_MODR = 6;
const int PAD_SUSR = 7;
const int PAD_UNACR = 8;
uint16_t pad_adc_value[9];
uint16_t pad_calibration[9] = {0};
int32_t pad_value[9];

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

int32_t abs(int32_t x)
{
	if (x < 0) return -x;
	// else
	return x;
}

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

int32_t chase(int i, uint16_t value)
{
	int32_t x = (int32_t)value - i * 0x4000;
	if (x < 0) return 0;
	if (x > 0x3FFF) return 0xFFFF;
	return 4 * x;
}

int32_t stash(int i, uint16_t value)
{
	int32_t x = 0x4000 - abs((i + 1) * 0x4000 - (int32_t)value);
	if (x < 0) return 0;
	if (x > 0x3FFF) return 0xFFFF;
	return 4 * x;
}

int32_t chase_vco(int vco)
{
	if (!shiftctrl_flag_state(SELCHASEOSC4567)) return 0;

	if (shiftctrl_flag_state(SELCHASE)) return chase(vco, synth_param[CHASE].last) - 0xffff;
	if (shiftctrl_flag_state(SELSTASH)) return stash(vco, synth_param[CHASE].last) - 0xffff;
	return 0;
}

int32_t chase_vcf(int vcf)
{
	if (!shiftctrl_flag_state(SELCHASEVCF2)) return 0;

	if (shiftctrl_flag_state(SELCHASE)) return chase(vcf, synth_param[CHASE].last) - 0xffff;
	if (shiftctrl_flag_state(SELSTASH)) return stash(vcf, synth_param[CHASE].last) - 0xffff;
	return 0;
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

	synth_param[VCO1_MIX1].adsr_depth = 0x7fff;
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

void ports_value(int portid, uint16_t value)
{
	if (portfunc_write_func[portid]) {
		portfunc_write_func[portid] (portfunc_write_ic[portid], portfunc_write_ch[portid], value);
	}
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

	if (note_change || retrigger) {
		for (int i = 0; i < SYNTH_PARAM_COUNT; i++) {
			if (synth_param[i].flags & SubParamFlags_LfoRetrigger) {
				lfo_reset(i, synth_param[i].lfo_reset_phase);
			}
		}
	}

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

	switch (subparam) {
	case 0:
		synth_param[param].value = value;
		break;
	case 1:
		synth_param[param].lfo_speed = value;
		lfo_set_speed(param, value);
		break;
	case 2:
		synth_param[param].lfo_depth = value;
		break;
	case 3:
		synth_param[param].lfo_shape = value;
		break;
	case 4:
		synth_param[param].adsr_a = value;
		adsr_set_a(param, value);
		break;
	case 5:
		synth_param[param].adsr_d = value;
		adsr_set_d(param, value);
		break;
	case 6:
		synth_param[param].adsr_s = value;
		adsr_set_s(param, value << 16);
		break;
	case 7:
		synth_param[param].adsr_r = value;
		adsr_set_r(param, value);
		break;
	case 8:
		synth_param[param].adsr_depth = value;
		break;
	case 9:
		synth_param[param].ad_a = value;
		ad_set_a(param, value);
		break;
	case 10:
		synth_param[param].ad_d = value;
		ad_set_d(param, value);
		break;
	case 11:
		synth_param[param].ad_depth = value;
		break;
	case 12:
		synth_param[param].x = value;
		break;
	case 13:
		synth_param[param].y = value;
		break;
	case 14:
		synth_param[param].z = value;
		break;
	case 15:
		synth_param[param].zprime = value;
		break;
	case 16:
		synth_param[param].note = value;
		break;
	case 17:
		synth_param[param].vel = value;
		break;
	case 18:
		synth_param[param].lfo_reset_phase = value;
		break;
	case 19:
		synth_param[param].pad_l_depth = value;
		break;
	case 20:
		synth_param[param].pad_r_depth = value;
		break;
	case 21:
		synth_param[param].sus_l_depth = value;
		break;
	case 22:
		synth_param[param].sus_r_depth = value;
		break;
	case 23:
		synth_param[param].unac_l_depth = value;
		break;
	case 24:
		synth_param[param].unac_r_depth = value;
		break;
	case 32:
		synth_param[param].flags = value;
		break;
	}
}

int process_param_lin(int ctrlid)
{
	int result = doing_reset;

	adsr_set_gate(ctrlid, synth_param[GATE].value, synth_param[ctrlid].flags & SubParamFlags_AdsrRetrigger);
	ad_set_gate(ctrlid, synth_param[GATE].value, synth_param[ctrlid].flags & SubParamFlags_AdRetrigger);

	int32_t value = synth_param[ctrlid].value;
	if (synth_param[ctrlid].note) {
		value += notetrack_scale(synth_param[NOTE].last, synth_param[ctrlid].note);
	}
	if (synth_param[ctrlid].lfo_depth) {
		int32_t lfo = (int32_t)lfo_update(ctrlid) - 0x8000;
		value += bipolar_signed_scale(lfo, synth_param[ctrlid].lfo_depth);
	}
	if (synth_param[ctrlid].adsr_depth) {
		uint16_t adsr = adsr_update(ctrlid, una_corda_release);
		value += signed_scale(adsr, synth_param[ctrlid].adsr_depth);
	}
	if (synth_param[ctrlid].ad_depth) {
		uint16_t ad = ad_update(ctrlid, una_corda_release);
		value += signed_scale(ad, synth_param[ctrlid].ad_depth);
	}
	if (synth_param[ctrlid].x) {
		value += bipolar_signed_scale(pad_value[KEYBOARD_X], synth_param[ctrlid].x);
	}
	if (synth_param[ctrlid].y) {
		value += bipolar_signed_scale(pad_value[KEYBOARD_Y], synth_param[ctrlid].y);
	}
	if (synth_param[ctrlid].z) {
		value += bipolar_signed_scale(pad_value[KEYBOARD_Z], synth_param[ctrlid].z);
	}
	if (synth_param[ctrlid].zprime) {
		value += bipolar_signed_scale(zprime_value, synth_param[ctrlid].zprime);
	}
	if (synth_param[ctrlid].pad_l_depth) {
		value += bipolar_signed_scale(pad_value[PAD_MODL], synth_param[ctrlid].pad_l_depth);
	}
	if (synth_param[ctrlid].pad_r_depth) {
		value += bipolar_signed_scale(pad_value[PAD_MODR], synth_param[ctrlid].pad_r_depth);
	}
	if (synth_param[ctrlid].sus_l_depth) {
		value += bipolar_signed_scale(pad_value[PAD_SUSL], synth_param[ctrlid].sus_l_depth);
	}
	if (synth_param[ctrlid].sus_r_depth) {
		value += bipolar_signed_scale(pad_value[PAD_SUSR], synth_param[ctrlid].sus_r_depth);
	}
	if (synth_param[ctrlid].unac_l_depth) {
		value += bipolar_signed_scale(pad_value[PAD_UNACL], synth_param[ctrlid].unac_l_depth);
	}
	if (synth_param[ctrlid].unac_r_depth) {
		value += bipolar_signed_scale(pad_value[PAD_UNACR], synth_param[ctrlid].unac_r_depth);
	}

	if (value < 0) value = 0;
	else if (value > 65535) value = 65535;

	result |= (synth_param[ctrlid].last != value);
	synth_param[ctrlid].last = value;

	return result;
}

int process_param_inv(int ctrlid)
{
	int result = doing_reset;

	adsr_set_gate(ctrlid, synth_param[GATE].value, synth_param[ctrlid].flags & SubParamFlags_AdsrRetrigger);
	ad_set_gate(ctrlid, synth_param[GATE].value, synth_param[ctrlid].flags & SubParamFlags_AdRetrigger);

	int32_t value = synth_param[ctrlid].value;
	if (synth_param[ctrlid].note) {
		value += notetrack_scale(synth_param[NOTE].last, synth_param[ctrlid].note);
	}
	if (synth_param[ctrlid].lfo_depth) {
		int32_t lfo = (int32_t)lfo_update(ctrlid) - 0x8000;
		value += bipolar_signed_scale(lfo, synth_param[ctrlid].lfo_depth);
	}
	if (synth_param[ctrlid].adsr_depth) {
		uint16_t adsr = adsr_update(ctrlid, una_corda_release);
		value += signed_scale(adsr, synth_param[ctrlid].adsr_depth);
	}
	if (synth_param[ctrlid].ad_depth) {
		uint16_t ad = ad_update(ctrlid, una_corda_release);
		value += signed_scale(ad, synth_param[ctrlid].ad_depth);
	}
	if (synth_param[ctrlid].x) {
		value += bipolar_signed_scale(pad_value[KEYBOARD_X], synth_param[ctrlid].x);
	}
	if (synth_param[ctrlid].y) {
		value += bipolar_signed_scale(pad_value[KEYBOARD_Y], synth_param[ctrlid].y);
	}
	if (synth_param[ctrlid].z) {
		value += bipolar_signed_scale(pad_value[KEYBOARD_Z], synth_param[ctrlid].z);
	}
	if (synth_param[ctrlid].zprime) {
		value += bipolar_signed_scale(zprime_value, synth_param[ctrlid].zprime);
	}
	if (synth_param[ctrlid].pad_l_depth) {
		value += bipolar_signed_scale(pad_value[PAD_MODL], synth_param[ctrlid].pad_l_depth);
	}
	if (synth_param[ctrlid].pad_r_depth) {
		value += bipolar_signed_scale(pad_value[PAD_MODR], synth_param[ctrlid].pad_r_depth);
	}
	if (synth_param[ctrlid].sus_l_depth) {
		value += bipolar_signed_scale(pad_value[PAD_SUSL], synth_param[ctrlid].sus_l_depth);
	}
	if (synth_param[ctrlid].sus_r_depth) {
		value += bipolar_signed_scale(pad_value[PAD_SUSR], synth_param[ctrlid].sus_r_depth);
	}
	if (synth_param[ctrlid].unac_l_depth) {
		value += bipolar_signed_scale(pad_value[PAD_UNACL], synth_param[ctrlid].unac_l_depth);
	}
	if (synth_param[ctrlid].unac_r_depth) {
		value += bipolar_signed_scale(pad_value[PAD_UNACR], synth_param[ctrlid].unac_r_depth);
	}

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

	adsr_set_gate(ctrlid, synth_param[GATE].value, synth_param[ctrlid].flags & SubParamFlags_AdsrRetrigger);
	ad_set_gate(ctrlid, synth_param[GATE].value, synth_param[ctrlid].flags & SubParamFlags_AdRetrigger);

	int32_t value = synth_param[ctrlid].value + add;
	if (value < 0) value = 0;
	else if (value > 65535) value = 65535;
	value += addchase;
	if (value < 0) value = 0;
	else if (value > 65535) value = 65535;
	if (synth_param[ctrlid].lfo_depth) {
		int32_t lfo = (int32_t)lfo_update(ctrlid) - 0x8000;
		value += bipolar_signed_scale(lfo, synth_param[ctrlid].lfo_depth);
	}
	if (synth_param[ctrlid].adsr_depth) {
		uint16_t adsr = adsr_update(ctrlid, una_corda_release);
		value += signed_scale(adsr, synth_param[ctrlid].adsr_depth);
	}
	if (synth_param[ctrlid].ad_depth) {
		uint16_t ad = ad_update(ctrlid, una_corda_release);
		value += signed_scale(ad, synth_param[ctrlid].ad_depth);
	}
	if (synth_param[ctrlid].x) {
		value += bipolar_signed_scale(pad_value[KEYBOARD_X], synth_param[ctrlid].x);
	}
	if (synth_param[ctrlid].y) {
		value += bipolar_signed_scale(pad_value[KEYBOARD_Y], synth_param[ctrlid].y);
	}
	if (synth_param[ctrlid].z) {
		value += bipolar_signed_scale(pad_value[KEYBOARD_Z], synth_param[ctrlid].z);
	}
	if (synth_param[ctrlid].zprime) {
		value += bipolar_signed_scale(zprime_value, synth_param[ctrlid].zprime);
	}
	if (synth_param[ctrlid].pad_l_depth) {
		value += bipolar_signed_scale(pad_value[PAD_MODL], synth_param[ctrlid].pad_l_depth);
	}
	if (synth_param[ctrlid].pad_r_depth) {
		value += bipolar_signed_scale(pad_value[PAD_MODR], synth_param[ctrlid].pad_r_depth);
	}
	if (synth_param[ctrlid].sus_l_depth) {
		value += bipolar_signed_scale(pad_value[PAD_SUSL], synth_param[ctrlid].sus_l_depth);
	}
	if (synth_param[ctrlid].sus_r_depth) {
		value += bipolar_signed_scale(pad_value[PAD_SUSR], synth_param[ctrlid].sus_r_depth);
	}
	if (synth_param[ctrlid].unac_l_depth) {
		value += bipolar_signed_scale(pad_value[PAD_UNACL], synth_param[ctrlid].unac_l_depth);
	}
	if (synth_param[ctrlid].unac_r_depth) {
		value += bipolar_signed_scale(pad_value[PAD_UNACR], synth_param[ctrlid].unac_r_depth);
	}

	if (value < 0) value = 65535;
	else if (value > 65535) value = 0;
	else value = 65535 - value;

	result |= (synth_param[ctrlid].last != value);
	synth_param[ctrlid].last = value;

	return result;
}

int process_param_log(int ctrlid)
{
	return process_param_log_add(ctrlid, 0, 0);
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
	if (process_param_log_add(ctrlid, (int32_t)synth_param[VCO4567_DRY_MIX].last, chase_vco(0))) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO5_DRY_MIX(int ctrlid, int port)
{
	if (process_param_log_add(ctrlid, synth_param[VCO4567_DRY_MIX].last, chase_vco(1))) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO6_DRY_MIX(int ctrlid, int port)
{
	if (process_param_log_add(ctrlid, synth_param[VCO4567_DRY_MIX].last, chase_vco(2))) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO7_DRY_MIX(int ctrlid, int port)
{
	if (process_param_log_add(ctrlid, synth_param[VCO4567_DRY_MIX].last, chase_vco(3))) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCF2_L_MIX(int ctrlid, int port)
{
	if (process_param_log_add(ctrlid, 0, chase_vcf(0))) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCF2_M1_MIX(int ctrlid, int port)
{
	if (process_param_log_add(ctrlid, 0, chase_vcf(1))) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCF2_M2_MIX(int ctrlid, int port)
{
	if (process_param_log_add(ctrlid, 0, chase_vcf(2))) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCF2_H_MIX(int ctrlid, int port)
{
	if (process_param_log_add(ctrlid, 0, chase_vcf(3))) {
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

void mixtwo_1(int ctrlid, int port, int levelctrlid, int mixctrlid)
{
	int32_t mix1 = 0x10000 - (int32_t)synth_param[mixctrlid].last;
	int value = synth_param[levelctrlid].last;
	if (mix1 < 0x8000) value = bipolar_signed_scale(value, mix1);
	value = 65535 - value;
	int result = (synth_param[ctrlid].last != value) || doing_reset;
	if (result) {
		synth_param[ctrlid].last = value;
		ports_value(port, synth_param[ctrlid].last);
	}
}

void mixtwo_2(int ctrlid, int port, int levelctrlid, int mixctrlid)
{
	int32_t mix2 = (int32_t)synth_param[mixctrlid].last;
	int value = synth_param[levelctrlid].last;
	if (mix2 < 0x8000) value = bipolar_signed_scale(value, mix2);
	value = 65535 - value;
	int result = (synth_param[ctrlid].last != value) || doing_reset;
	if (result) {
		synth_param[ctrlid].last = value;
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_VCO1_MIX1(int ctrlid, int port)
{
	mixtwo_1(ctrlid, port, VCO1_LEVEL, VCO1_VCFMIX);
}

void do_output_VCO1_MIX2(int ctrlid, int port)
{
	mixtwo_2(ctrlid, port, VCO1_LEVEL, VCO1_VCFMIX);
}

void do_output_VCO2_MIX1(int ctrlid, int port)
{
	mixtwo_1(ctrlid, port, VCO2_LEVEL, VCO2_VCFMIX);
}

void do_output_VCO2_MIX2(int ctrlid, int port)
{
	mixtwo_2(ctrlid, port, VCO2_LEVEL, VCO2_VCFMIX);
}

void do_output_VCO3_MIX1(int ctrlid, int port)
{
	mixtwo_1(ctrlid, port, VCO3_LEVEL, VCO3_VCFMIX);
}

void do_output_VCO3_MIX2(int ctrlid, int port)
{
	mixtwo_2(ctrlid, port, VCO3_LEVEL, VCO3_VCFMIX);
}

void do_output_VCO4567_MIX1(int ctrlid, int port)
{
	mixtwo_1(ctrlid, port, VCO4567_LEVEL, VCO4567_VCFMIX);
}

void do_output_VCO4567_MIX2(int ctrlid, int port)
{
	mixtwo_2(ctrlid, port, VCO4567_LEVEL, VCO4567_VCFMIX);
}

void do_output_RM1_MIX1(int ctrlid, int port)
{
	mixtwo_1(ctrlid, port, RM1_LEVEL, RM1_VCFMIX);
}

void do_output_RM1_MIX2(int ctrlid, int port)
{
	mixtwo_2(ctrlid, port, RM1_LEVEL, RM1_VCFMIX);
}

void do_output_WHITENS_MIX1(int ctrlid, int port)
{
	mixtwo_1(ctrlid, port, WHITENS_LEVEL, WHITENS_VCFMIX);
}

void do_output_WHITENS_MIX2(int ctrlid, int port)
{
	mixtwo_2(ctrlid, port, WHITENS_LEVEL, WHITENS_VCFMIX);
}

void do_output_DIGINS_MIX1(int ctrlid, int port)
{
	mixtwo_1(ctrlid, port, DIGINS_LEVEL, DIGINS_VCFMIX);
}

void do_output_DIGINS_MIX2(int ctrlid, int port)
{
	mixtwo_2(ctrlid, port, DIGINS_LEVEL, DIGINS_VCFMIX);
}

void do_output_EXT_MIX1(int ctrlid, int port)
{
	mixtwo_1(ctrlid, port, EXT_LEVEL, EXT_VCFMIX);
}

void do_output_EXT_MIX2(int ctrlid, int port)
{
	mixtwo_2(ctrlid, port, EXT_LEVEL, EXT_VCFMIX);
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
	int result = (synth_param[CLEANF_L_LOG].last != synth_param[VCF2_LEVEL].last) || doing_reset;
	synth_param[CLEANF_L_LOG].last = synth_param[CLEANF_LEVEL].last;
	if (result) {
		ports_value(port, synth_param[ctrlid].last);
	}
}

void do_output_CLEANF_R_LOG(int ctrlid, int port)
{
	int result = (synth_param[CLEANF_R_LOG].last != synth_param[VCF2_LEVEL].last) || doing_reset;
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

void virt_CLEANF_LIN()
{
	process_param_lin(CLEANF_LIN);
}

void virt_VCF1_LIN()
{
	process_param_lin(VCF1_LIN);
}

void virt_VCF2_LIN()
{
	process_param_lin(VCF2_LIN);
}

void virt_CLEANF_LEVEL()
{
	process_param_log_add(CLEANF_LEVEL, (int32_t)synth_param[MASTER_LEVEL].last - 0xFFFF, 0);
}

void virt_VCF1_LEVEL()
{
	process_param_log_add(VCF1_LEVEL, (int32_t)synth_param[MASTER_LEVEL].last - 0xFFFF, 0);
}

void virt_VCF2_LEVEL()
{
	process_param_log_add(VCF2_LEVEL, (int32_t)synth_param[MASTER_LEVEL].last - 0xFFFF, 0);
}

void virt_CLEANF_PAN()
{
	process_param_lin(CLEANF_PAN);
}

void virt_VCF1_PAN()
{
	process_param_lin(VCF1_PAN);
}

void virt_VCF2_PAN()
{
	process_param_lin(VCF2_PAN);
}

void virt_VCO1_VCFMIX()
{
	process_param_lin(VCO1_VCFMIX);
}

void virt_VCO2_VCFMIX()
{
	process_param_lin(VCO2_VCFMIX);
}

void virt_VCO3_VCFMIX()
{
	process_param_lin(VCO3_VCFMIX);
}

void virt_VCO4567_VCFMIX()
{
	process_param_lin(VCO4567_VCFMIX);
}

void virt_RM1_VCFMIX()
{
	process_param_lin(RM1_VCFMIX);
}

void virt_WHITENS_VCFMIX()
{
	process_param_lin(WHITENS_VCFMIX);
}

void virt_DIGINS_VCFMIX()
{
	process_param_lin(DIGINS_VCFMIX);
}

void virt_EXT_VCFMIX()
{
	process_param_lin(EXT_VCFMIX);
}

void virt_VCO4567_DRY_MIX()
{
	process_param_lin(VCO4567_DRY_MIX);
}

void virt_VCO1_LEVEL()
{
	process_param_lin(VCO1_LEVEL);
}

void virt_VCO2_LEVEL()
{
	process_param_lin(VCO2_LEVEL);
}

void virt_VCO3_LEVEL()
{
	process_param_lin(VCO3_LEVEL);
}

void virt_VCO4567_LEVEL()
{
	process_param_lin(VCO4567_LEVEL);
}

void virt_RM1_LEVEL()
{
	process_param_lin(RM1_LEVEL);
}

void virt_WHITENS_LEVEL()
{
	process_param_lin(WHITENS_LEVEL);
}

void virt_DIGINS_LEVEL()
{
	process_param_lin(DIGINS_LEVEL);
}

void virt_EXT_LEVEL()
{
	process_param_lin(EXT_LEVEL);
}

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
	if (synth_param[PORTAMENTO_TIME].value != synth_param[PORTAMENTO_TIME].last) {
		porta_divider = 256 + ((int32_t)synth_param[PORTAMENTO_TIME].value * 8);
		synth_param[PORTAMENTO_TIME].value = synth_param[PORTAMENTO_TIME].last;
	}
}

void virt_MASTER_LEVEL()
{
	synth_param[MASTER_LEVEL].last = synth_param[MASTER_LEVEL].value;
}

void virt_ZPRIME_SPEED()
{
	if (synth_param[ZPRIME_SPEED].value != synth_param[ZPRIME_SPEED].last) {
		hp_set_speed(&zprime_hp, synth_param[ZPRIME_SPEED].value);
		synth_param[ZPRIME_SPEED].last = synth_param[ZPRIME_SPEED].value;
	}
}

void virt_X_DEADZONE()
{
}

void virt_Y_DEADZONE()
{
}

void virt_Z_DEADZONE()
{
}

void virt_X_SCALE()
{
}

void virt_Y_SCALE()
{
}

void virt_Z_SCALE()
{
}

void virt_MODL_SCALE()
{
}

void virt_SUSL_SCALE()
{
}

void virt_UNACL_SCALE()
{
}

void virt_MODR_SCALE()
{
}

void virt_SUSR_SCALE()
{
}

void virt_UNACR_SCALE()
{
}

int process_param_note(int ctrlid, int32_t notevalue, int modrange)
{
	int result = doing_reset;

	adsr_set_gate(ctrlid, synth_param[GATE].value, synth_param[ctrlid].flags & SubParamFlags_AdsrRetrigger);
	ad_set_gate(ctrlid, synth_param[GATE].value, synth_param[ctrlid].flags & SubParamFlags_AdRetrigger);

	int32_t value = notevalue;

	value += (int32_t)synth_param[MASTER_PITCH].last - 0x8000;
	value += (int32_t)synth_param[MASTER_PITCH2].last - 0x8000;
	int modvalue = 0;
	if (synth_param[ctrlid].lfo_depth) {
		int32_t lfo = (int32_t)lfo_update(ctrlid) - 0x8000;
		modvalue += bipolar_signed_scale(lfo, synth_param[ctrlid].lfo_depth);
	}
	if (synth_param[ctrlid].adsr_depth) {
		uint16_t adsr = adsr_update(ctrlid, una_corda_release);
		modvalue += signed_scale(adsr, synth_param[ctrlid].adsr_depth);
	}
	if (synth_param[ctrlid].ad_depth) {
		uint16_t ad = ad_update(ctrlid, una_corda_release);
		modvalue += signed_scale(ad, synth_param[ctrlid].ad_depth);
	}
	if (synth_param[ctrlid].x) {
		modvalue += bipolar_signed_scale(pad_value[KEYBOARD_X], synth_param[ctrlid].x);
	}
	if (synth_param[ctrlid].y) {
		modvalue += bipolar_signed_scale(pad_value[KEYBOARD_Y], synth_param[ctrlid].y);
	}
	if (synth_param[ctrlid].z) {
		modvalue += bipolar_signed_scale(pad_value[KEYBOARD_Z], synth_param[ctrlid].z);
	}
	if (synth_param[ctrlid].zprime) {
		modvalue += bipolar_signed_scale(zprime_value, synth_param[ctrlid].zprime);
	}
	if (synth_param[ctrlid].pad_l_depth) {
		modvalue += bipolar_signed_scale(pad_value[PAD_MODL], synth_param[ctrlid].pad_l_depth);
	}
	if (synth_param[ctrlid].pad_r_depth) {
		modvalue += bipolar_signed_scale(pad_value[PAD_MODR], synth_param[ctrlid].pad_r_depth);
	}
	if (synth_param[ctrlid].sus_l_depth) {
		modvalue += bipolar_signed_scale(pad_value[PAD_SUSL], synth_param[ctrlid].sus_l_depth);
	}
	if (synth_param[ctrlid].sus_r_depth) {
		modvalue += bipolar_signed_scale(pad_value[PAD_SUSR], synth_param[ctrlid].sus_r_depth);
	}
	if (synth_param[ctrlid].unac_l_depth) {
		modvalue += bipolar_signed_scale(pad_value[PAD_UNACL], synth_param[ctrlid].unac_l_depth);
	}
	if (synth_param[ctrlid].unac_r_depth) {
		modvalue += bipolar_signed_scale(pad_value[PAD_UNACR], synth_param[ctrlid].unac_r_depth);
	}
	value += bipolar_signed_scale(modvalue, modrange * (0x8000 / 128));

	if (value < 0) value = 0;
	else if (value > 65535) value = 65535;

	result |= (synth_param[ctrlid].last != value);
	synth_param[ctrlid].last = value;

	return result;
}

void virt_VCO1_PITCH()
{
	int32_t value = note_add(signed_scale(synth_param[NOTE].last, synth_param[VCO1_PITCH].note), note_scale(synth_param[VCO1_PITCH].value, 24 * 0x4000 / 128));

	process_param_note(VCO1_PITCH, value, 24);
}

void virt_VCO2_PITCH()
{
	int32_t value = signed_scale(synth_param[NOTE].last, synth_param[VCO1_PITCH].note);
	value = note_add(value, note_scale(synth_param[VCO1_PITCH].value, 24 * 0x4000 / 128));
	value = note_add(value, note_scale(synth_param[VCO2_PITCH].value, 4 * 0x4000 / 128));

	process_param_note(VCO2_PITCH, value, 24);
}

void virt_VCO3_PITCH()
{
	int32_t value = signed_scale(synth_param[NOTE].last, synth_param[VCO1_PITCH].note);
	value = note_add(value, note_scale(synth_param[VCO1_PITCH].value, 24 * 0x4000 / 128));
	value = note_add(value, note_scale(synth_param[VCO3_PITCH].value, 4 * 0x4000 / 128));

	process_param_note(VCO3_PITCH, value, 24);
}

void virt_VCO4_PITCH()
{
	int32_t value = signed_scale(synth_param[NOTE].last, synth_param[VCO4_PITCH].note);
	value = note_add(value, note_scale(synth_param[VCO4_PITCH].value, 72 * 0x4000 / 128));

	process_param_note(VCO4_PITCH, value, 24);
}

void virt_VCO5_PITCH()
{
	int32_t value = signed_scale(synth_param[NOTE].last, synth_param[VCO4_PITCH].note);
	value = note_add(value, note_scale(synth_param[VCO4_PITCH].value, 72 * 0x4000 / 128));
	value += signed_scale(synth_param[VCO5_PITCH].value, 36 * 0x4000 / 256);

	process_param_note(VCO5_PITCH, value, 24);
}

void virt_VCO6_PITCH()
{
	int32_t value = signed_scale(synth_param[NOTE].last, synth_param[VCO4_PITCH].note);
	value = note_add(value, note_scale(synth_param[VCO4_PITCH].value, 72 * 0x4000 / 128));
	value += signed_scale(synth_param[VCO6_PITCH].value, 36 * 0x4000 / 256);

	process_param_note(VCO6_PITCH, value, 24);
}

void virt_VCO7_PITCH()
{
	int32_t value = signed_scale(synth_param[NOTE].last, synth_param[VCO4_PITCH].note);
	value = note_add(value, note_scale(synth_param[VCO4_PITCH].value, 72 * 0x4000 / 128));
	value += signed_scale(synth_param[VCO7_PITCH].value, 36 * 0x4000 / 256);

	process_param_note(VCO7_PITCH, value, 24);
}

void virt_GATE()
{
	synth_param[GATE].last = synth_param[GATE].value;
}

void virt_RETRIGGER()
{
	if (synth_param[RETRIGGER].value) {
		synth_param[RETRIGGER].value--;
	}
	synth_param[RETRIGGER].last = synth_param[RETRIGGER].value;
}

void virt_MASTER_PITCH()
{
	synth_param[MASTER_PITCH].last = (((int32_t)synth_param[MASTER_PITCH].value - 0x8000) * 12) / 128 + 0x8000;
}

void virt_MASTER_PITCH2()
{
	synth_param[MASTER_PITCH2].last = synth_param[MASTER_PITCH2].value;
}

void virt_CHASE()
{
	process_param_lin(CHASE);
}

void synth_mapping_virt()
{
	static uint16_t last[8] = { 0xfffe, 0xfffe, 0xfffe, 0xfffe, 0xfffe, 0xfffe, 0xfffe, 0xfffe };

	uint16_t val = autotune_note_to_dac(0, synth_param[VCO1_PITCH].last);
	if (val != last[0] || doing_reset) {
		ports_value(BASE_P4_1 + 0, val);
		last[0] = val;
	}

	val = autotune_note_to_dac(1, synth_param[VCO2_PITCH].last);
	if (val != last[1] || doing_reset) {
		ports_value(BASE_P4_1 + 1, val);
		last[1] = val;
	}

	val = autotune_note_to_dac(2, synth_param[VCO3_PITCH].last);
	if (val != last[2] || doing_reset) {
		ports_value(BASE_P4_1 + 2, val);
		last[2] = val;
	}

	val = autotune_note_to_dac(3, synth_param[VCO4_PITCH].last);
	if (val != last[3] || doing_reset) {
		ports_value(BASE_P4_1 + 3, val);
		last[3] = val;
	}

	val = autotune_note_to_dac(4, synth_param[VCO5_PITCH].last);
	if (val != last[4] || doing_reset) {
		ports_value(BASE_P4_0 + 0, val);
		last[4] = val;
	}

	val = autotune_note_to_dac(5, synth_param[VCO6_PITCH].last);
	if (val != last[5] || doing_reset) {
		ports_value(BASE_P4_0 + 1, val);
		last[5] = val;
	}

	val = autotune_note_to_dac(6, synth_param[VCO7_PITCH].last);
	if (val != last[6] || doing_reset) {
		ports_value(BASE_P4_0 + 2, val);
		last[6] = val;
	}
}

void pad_zero()
{
	for (int i = 0; i < 3; i++) {
		ports_input(i, &pad_calibration[i]);
		ports_input(i + 6, &pad_calibration[i + 3]);
		ports_input(i + 9, &pad_calibration[i + 6]);
	}
}

void pad_init()
{
	pad_zero();
}

void synth_init()
{
	synth_mapping_init();
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
    shiftctrl_set(SELMOST3);
    shiftctrl_set(SELVCF2L0);
    shiftctrl_set(SELVCF2L1);
    //shiftctrl_set(SELEFFECT1);

    //SCTIMER_SetCallback(SCTIMER_1_PERIPHERAL, SCTimer_Func, 0);
    SCTIMER_EnableInterrupts(SCTIMER_1_PERIPHERAL, 3);
    SCTIMER_StartTimer(SCTIMER_1_PERIPHERAL, kSCTIMER_Counter_L);

    // must be lower than autotune priority or overflow checking will break!
    NVIC_SetPriority(SCTIMER_1_IRQN, 1);

    autotune_init();
    pad_init();
}

const int negate[9] = { 1, 1, 0,  1, 0, 0,  1, 0, 0 };

int32_t pad_threshold(int32_t value, int i)
{
	int dz = 0;
	int scale = 0x100;

	if (negate[i]) value = -value;

	switch (i) {
	case 0:
		dz = synth_param[X_DEADZONE].value >> 3;
		scale = synth_param[X_SCALE].value >> 3;
		break;
	case 1:
		dz = synth_param[Y_DEADZONE].value >> 3;
		scale = synth_param[Y_SCALE].value >> 3;
		break;
	case 2:
		dz = synth_param[Z_DEADZONE].value >> 3;
		scale = synth_param[Z_SCALE].value >> 3;
		break;
	case 3:
		scale = synth_param[MODL_SCALE].value >> 4;
		break;
	case 4:
		scale = synth_param[SUSL_SCALE].value >> 4;
		break;
	case 5:
		scale = synth_param[UNACL_SCALE].value >> 4;
		break;
	case 6:
		scale = synth_param[MODR_SCALE].value >> 4;
		break;
	case 7:
		scale = synth_param[SUSR_SCALE].value >> 4;
		break;
	case 8:
		scale = synth_param[UNACR_SCALE].value >> 4;
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

void synth_run()
{
	for (;;) {

		doing_reset = reset;

	    synth_mapping_run();
	    synth_mapping_virt();

		for (int i = 0; i < 3; i++) {
			ports_input(i, &pad_adc_value[i]);
			ports_input(i + 6, &pad_adc_value[i + 3]);
			ports_input(i + 9, &pad_adc_value[i + 6]);
		}

		for (int i = 0; i < 9; i++) {
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
		if (shiftctrl_flag_state(SELUNACL) && pad_value[PAD_UNACL] > 500)
			una_corda_release += (pad_value[PAD_UNACL] - 500) << 8;
		if (shiftctrl_flag_state(SELUNACR) && pad_value[PAD_UNACR] > 500)
			una_corda_release += (pad_value[PAD_UNACR] - 500) << 8;

		shiftctrl_update();
	}
}

void synth_reset()
{
	reset = 1;
}
