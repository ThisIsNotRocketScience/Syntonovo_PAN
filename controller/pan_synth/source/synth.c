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

#include "spi_sched.h"
#include "control.h"
#include "autotune.h"
#include "notestack.h"

static volatile int reset = 0;
int doing_reset = 0;

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

// scale = -0x8000..0x7fff, 0x4000 ~ 1.0
static uint16_t note_scale(uint16_t note_v, int16_t scale)
{
	int32_t r = (int32_t)note_v * (int32_t)scale;
	r >>= 14;

	if (r < 0) return 0;
	else if (r > 65535) return 65535;
	// else
	return (uint16_t) r;
}

// add = 0..0xffff, 0x8000 ~ 0.0
static uint16_t note_add(uint16_t note_v, uint16_t add)
{
	int32_t r = (int32_t)note_v + (int32_t)add - 0x8000;

	if (r < 0) return 0;
	else if (r > 65535) return 65535;
	// else
	return (uint16_t) r;
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
		synth_param[GATE].value = 0;
		synth_param[RETRIGGER].value = 0;
		return;
	}

	int note_change = (notestack_first().note << 8) != synth_param[NOTE].value;
	synth_param[NOTE].value = notestack_first().note << 8;
	virt_NOTE();

	if (synth_param[GATE].value != 0xffff) {
		note_change = 1;
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
		adsr_set_s(param, value);
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
	}
}

int process_param_lin(int ctrlid)
{
	adsr_set_gate(ctrlid, synth_param[GATE].value);
	ad_set_gate(ctrlid, synth_param[GATE].value);

	int32_t value = synth_param[ctrlid].value;
	if (synth_param[ctrlid].note) {
		value += notetrack_scale(synth_param[NOTE].last, synth_param[ctrlid].note);
	}
	if (synth_param[ctrlid].lfo_depth) {
		int32_t lfo = (int32_t)lfo_update(ctrlid) - 0x8000;
		value += bipolar_signed_scale(lfo, synth_param[ctrlid].lfo_depth);
	}
	if (synth_param[ctrlid].adsr_depth) {
		uint16_t adsr = adsr_update(ctrlid);
		value += signed_scale(adsr, synth_param[ctrlid].adsr_depth);
	}
	if (synth_param[ctrlid].ad_depth) {
		uint16_t ad = ad_update(ctrlid);
		value += signed_scale(ad, synth_param[ctrlid].ad_depth);
	}

	if (value < 0) value = 0;
	else if (value > 65535) value = 65535;

	int result = (synth_param[ctrlid].last != value);
	synth_param[ctrlid].last = value;

	return result;
}

int process_param_inv(int ctrlid)
{
	adsr_set_gate(ctrlid, synth_param[GATE].value);
	ad_set_gate(ctrlid, synth_param[GATE].value);

	int32_t value = synth_param[ctrlid].value;
	if (synth_param[ctrlid].note) {
		value += notetrack_scale(synth_param[NOTE].last, synth_param[ctrlid].note);
	}
	if (synth_param[ctrlid].lfo_depth) {
		int32_t lfo = (int32_t)lfo_update(ctrlid) - 0x8000;
		value += bipolar_signed_scale(lfo, synth_param[ctrlid].lfo_depth);
	}
	if (synth_param[ctrlid].adsr_depth) {
		uint16_t adsr = adsr_update(ctrlid);
		value += signed_scale(adsr, synth_param[ctrlid].adsr_depth);
	}
	if (synth_param[ctrlid].ad_depth) {
		uint16_t ad = ad_update(ctrlid);
		value += signed_scale(ad, synth_param[ctrlid].ad_depth);
	}

	if (value < 0) value = 65535;
	else if (value > 65535) value = 0;
	else value = 65535 - value;

	int result = (synth_param[ctrlid].last != value);
	synth_param[ctrlid].last = value;

	return result;
}

int process_param_log(int ctrlid)
{
	adsr_set_gate(ctrlid, synth_param[GATE].value);
	ad_set_gate(ctrlid, synth_param[GATE].value);

	int32_t value = synth_param[ctrlid].value;
	if (synth_param[ctrlid].lfo_depth) {
		uint16_t lfo = (lfo_update(ctrlid) >> 2) + 0x8000;
		value += signed_scale(lfo, synth_param[ctrlid].lfo_depth);
	}
	if (synth_param[ctrlid].adsr_depth) {
		uint16_t adsr = adsr_update(ctrlid);
		value += signed_scale(adsr, synth_param[ctrlid].adsr_depth);
	}
	if (synth_param[ctrlid].ad_depth) {
		uint16_t ad = ad_update(ctrlid);
		value += signed_scale(ad, synth_param[ctrlid].ad_depth);
	}

	if (value < 0) value = 65535;
	else if (value > 65535) value = 0;
	else value = 65535 - value;

	int result = (synth_param[ctrlid].last != value) || doing_reset;
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

void virt_VCF1_LEVEL()
{

}

void virt_VCF2_LEVEL()
{

}

void virt_CLEANF_LEVEL()
{

}

void virt_VCF1_PAN()
{

}

void virt_VCF2_PAN()
{

}

void virt_CLEANF_PAN()
{

}

void virt_NOTE()
{
	//synth_param[NOTE].last = note_to_voltage(synth_param[NOTE].value);
	synth_param[NOTE].last = synth_param[NOTE].value;
}

void virt_VCO1_PITCH()
{
	const int ctrlid = VCO1_PITCH;

	adsr_set_gate(ctrlid, synth_param[GATE].value);
	ad_set_gate(ctrlid, synth_param[GATE].value);

	int32_t value = note_add(note_scale(synth_param[NOTE].last, synth_param[ctrlid].note), synth_param[ctrlid].value);
	if (synth_param[ctrlid].lfo_depth) {
		uint16_t lfo = (lfo_update(ctrlid) >> 2) + 0x8000;
		value += signed_scale(lfo, synth_param[ctrlid].lfo_depth);
	}
	if (synth_param[ctrlid].adsr_depth) {
		uint16_t adsr = adsr_update(ctrlid);
		value += signed_scale(adsr, synth_param[ctrlid].adsr_depth);
	}
	if (synth_param[ctrlid].ad_depth) {
		uint16_t ad = ad_update(ctrlid);
		value += signed_scale(ad, synth_param[ctrlid].ad_depth);
	}

	if (value < 0) value = 0;
	else if (value > 65535) value = 65535;
	synth_param[ctrlid].last = value;
}

void virt_VCO2_PITCH()
{
	const int ctrlid = VCO2_PITCH;

	adsr_set_gate(ctrlid, synth_param[GATE].value);
	ad_set_gate(ctrlid, synth_param[GATE].value);

	int32_t value = note_add(note_scale(synth_param[NOTE].last, synth_param[ctrlid].note), synth_param[ctrlid].value);
	if (synth_param[ctrlid].lfo_depth) {
		uint16_t lfo = (lfo_update(ctrlid) >> 2) + 0x8000;
		value += signed_scale(lfo, synth_param[ctrlid].lfo_depth);
	}
	if (synth_param[ctrlid].adsr_depth) {
		uint16_t adsr = adsr_update(ctrlid);
		value += signed_scale(adsr, synth_param[ctrlid].adsr_depth);
	}
	if (synth_param[ctrlid].ad_depth) {
		uint16_t ad = ad_update(ctrlid);
		value += signed_scale(ad, synth_param[ctrlid].ad_depth);
	}

	if (value < 0) value = 0;
	else if (value > 65535) value = 65535;
	synth_param[ctrlid].last = value;
}

void virt_VCO3_PITCH()
{
	const int ctrlid = VCO3_PITCH;

	adsr_set_gate(ctrlid, synth_param[GATE].value);
	ad_set_gate(ctrlid, synth_param[GATE].value);

	int32_t value = note_add(note_scale(synth_param[NOTE].last, synth_param[ctrlid].note), synth_param[ctrlid].value);
	if (synth_param[ctrlid].lfo_depth) {
		uint16_t lfo = (lfo_update(ctrlid) >> 2) + 0x8000;
		value += signed_scale(lfo, synth_param[ctrlid].lfo_depth);
	}
	if (synth_param[ctrlid].adsr_depth) {
		uint16_t adsr = adsr_update(ctrlid);
		value += signed_scale(adsr, synth_param[ctrlid].adsr_depth);
	}
	if (synth_param[ctrlid].ad_depth) {
		uint16_t ad = ad_update(ctrlid);
		value += signed_scale(ad, synth_param[ctrlid].ad_depth);
	}

	if (value < 0) value = 0;
	else if (value > 65535) value = 65535;
	synth_param[ctrlid].last = value;
}

void virt_VCO4_PITCH()
{
	const int ctrlid = VCO4_PITCH;

	adsr_set_gate(ctrlid, synth_param[GATE].value);
	ad_set_gate(ctrlid, synth_param[GATE].value);

	int32_t value = note_add(note_scale(synth_param[NOTE].last, synth_param[ctrlid].note), synth_param[ctrlid].value);
	if (synth_param[ctrlid].lfo_depth) {
		uint16_t lfo = (lfo_update(ctrlid) >> 2) + 0x8000;
		value += signed_scale(lfo, synth_param[ctrlid].lfo_depth);
	}
	if (synth_param[ctrlid].adsr_depth) {
		uint16_t adsr = adsr_update(ctrlid);
		value += signed_scale(adsr, synth_param[ctrlid].adsr_depth);
	}
	if (synth_param[ctrlid].ad_depth) {
		uint16_t ad = ad_update(ctrlid);
		value += signed_scale(ad, synth_param[ctrlid].ad_depth);
	}

	if (value < 0) value = 0;
	else if (value > 65535) value = 65535;
	synth_param[ctrlid].last = value;
}

void virt_VCO5_PITCH()
{
	const int ctrlid = VCO5_PITCH;

	adsr_set_gate(ctrlid, synth_param[GATE].value);
	ad_set_gate(ctrlid, synth_param[GATE].value);

	int32_t value = note_add(note_scale(synth_param[NOTE].last, synth_param[ctrlid].note), synth_param[ctrlid].value);
	if (synth_param[ctrlid].lfo_depth) {
		uint16_t lfo = (lfo_update(ctrlid) >> 2) + 0x8000;
		value += signed_scale(lfo, synth_param[ctrlid].lfo_depth);
	}
	if (synth_param[ctrlid].adsr_depth) {
		uint16_t adsr = adsr_update(ctrlid);
		value += signed_scale(adsr, synth_param[ctrlid].adsr_depth);
	}
	if (synth_param[ctrlid].ad_depth) {
		uint16_t ad = ad_update(ctrlid);
		value += signed_scale(ad, synth_param[ctrlid].ad_depth);
	}

	if (value < 0) value = 0;
	else if (value > 65535) value = 65535;
	synth_param[ctrlid].last = value;
}

void virt_VCO6_PITCH()
{
	const int ctrlid = VCO6_PITCH;

	adsr_set_gate(ctrlid, synth_param[GATE].value);
	ad_set_gate(ctrlid, synth_param[GATE].value);

	int32_t value = note_add(note_scale(synth_param[NOTE].last, synth_param[ctrlid].note), synth_param[ctrlid].value);
	if (synth_param[ctrlid].lfo_depth) {
		uint16_t lfo = (lfo_update(ctrlid) >> 2) + 0x8000;
		value += signed_scale(lfo, synth_param[ctrlid].lfo_depth);
	}
	if (synth_param[ctrlid].adsr_depth) {
		uint16_t adsr = adsr_update(ctrlid);
		value += signed_scale(adsr, synth_param[ctrlid].adsr_depth);
	}
	if (synth_param[ctrlid].ad_depth) {
		uint16_t ad = ad_update(ctrlid);
		value += signed_scale(ad, synth_param[ctrlid].ad_depth);
	}

	if (value < 0) value = 0;
	else if (value > 65535) value = 65535;
	synth_param[ctrlid].last = value;
}

void virt_VCO7_PITCH()
{
	const int ctrlid = VCO7_PITCH;

	adsr_set_gate(ctrlid, synth_param[GATE].value);
	ad_set_gate(ctrlid, synth_param[GATE].value);

	int32_t value = note_add(note_scale(synth_param[NOTE].last, synth_param[ctrlid].note), synth_param[ctrlid].value);
	if (synth_param[ctrlid].lfo_depth) {
		uint16_t lfo = (lfo_update(ctrlid) >> 2) + 0x8000;
		value += signed_scale(lfo, synth_param[ctrlid].lfo_depth);
	}
	if (synth_param[ctrlid].adsr_depth) {
		uint16_t adsr = adsr_update(ctrlid);
		value += signed_scale(adsr, synth_param[ctrlid].adsr_depth);
	}
	if (synth_param[ctrlid].ad_depth) {
		uint16_t ad = ad_update(ctrlid);
		value += signed_scale(ad, synth_param[ctrlid].ad_depth);
	}

	if (value < 0) value = 0;
	else if (value > 65535) value = 65535;
	synth_param[ctrlid].last = value;
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

void synth_init()
{
	synth_mapping_init();
    lfo_init();
    adsr_init();
    ad_init();

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
}

void synth_run()
{
	uint16_t adc_value[6];

	int last_trigger = 0;
	int last_release = 0;
	int last_timer_value = 0;
	volatile int overflow = 0;
	for (;;) {

		doing_reset = reset;

	    synth_mapping_run();
	    synth_mapping_virt();

#if 0
		for (int i = 0; i < 6; i++) {
			ports_value(i+6, adc_value[i]);
		}

		for (int i = 12; i < 96; i++) {
			uint16_t lfo = (lfo_update(i) >> 2) + 0x8000;
			uint16_t adsr = adsr_update(i);
			ports_value(i, ((uint32_t)lfo * (uint32_t)adsr) >> 16);
		}

		for (int i = 0; i < 6; i++) {
			ports_input(i, &adc_value[i]);
		}
#endif
	}
}

void synth_reset()
{
	reset = 1;
}
