/*
 * autotune.c
 *
 *  Created on: Apr 1, 2018
 *      Author: ld0d
 */

#include "autotune.h"
#include <math.h>
#include <stdlib.h>

#include "peripherals.h"

#include "fsl_eeprom.h"
#include "fsl_inputmux.h"
//#include "fsl_pint.h"
#include "clock_config.h"
#include "pin_mux.h"

#include "ports.h"
#include "synth_internal_ctrlmap.h"
#include "shiftctrl.h"
#include "synth_param.h"
#include "control.h"

#define AVERAGECOUNT 32

volatile int32_t doaveragecount = 4;

volatile uint32_t comparetimes[AVERAGECOUNT];
volatile uint32_t elapsed[AVERAGECOUNT];
volatile int32_t compareindex = 0;

volatile uint32_t elapsedavg[32];
volatile int32_t avgindex = 0;
volatile uint32_t elapsedavgsum = 0;

#define WEAK __attribute__ ((weak))

extern volatile int sctimer_state;
extern volatile int sctimer_counter;

uint32_t timer_value_isr()
{
	uint32_t highcount = sctimer_counter << 16;
	uint32_t timervalue = SCT0->COUNT;

	// but now the counter may have already overflowed! so highcount may be outdated.
	// must read COUNT first, because the overflow may happen also between reading COUNT and STATE.
	int overflow = SCT0->STATE != sctimer_state;

	// if there was an overflow, and the timer was close to beginning of its range,
	// assume that it did indeed overflow since last interrupt.
	if (overflow && timervalue < 32768) {
			highcount += 65536;
	}

	return highcount + timervalue;
}

__attribute__( ( section(".data") ) )
WEAK void PIN_INT0_IRQHandler(void)
{
	uint32_t timer = timer_value_isr();

	int32_t index = compareindex;
	uint32_t elapsedtime = timer - comparetimes[index];

	elapsed[index] = elapsedtime;

	if (index != 0) {
		elapsedavgsum += elapsedtime;
	}

	index = (index + 1) & (doaveragecount-1);
	comparetimes[index] = timer;
	compareindex = index;

	if (index == 0) {
		elapsedavg[avgindex] = elapsedavgsum / (doaveragecount - 1);
		elapsedavgsum = 0;
		avgindex = (avgindex + 1) & 31;
	}

	PINT->IST = PINT_IST_PSTAT_MASK;
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
	__DSB();
#endif
}

static void WAIT1_Waitus(uint32_t us)
{
	for (uint32_t k = (us * (SystemCoreClock / 1000)) / 1000; k > 0; k--) {
		__NOP();
	}
}

static void WAIT1_Waitms(uint32_t ms)
{
	for (int k = 0; k < ms; k++) {
		WAIT1_Waitus(1000);
	}
}

void Voutraw(int channel, uint16_t ictrl)
{
#if 0
#define DAC_RANGE 65536.0f

	float ctrl_v = voltage * (DAC_RANGE / 10.0f);

	int ictrl = (int32_t)floorf(ctrl_v);
	if (ictrl < 0) ictrl = 0;
	else if (ictrl > 65535) ictrl = 65535;
#endif
	// output = Vdac * (3.9+12.7)/3.9 - 3.3
	// Vdac = (output + 3.3) * 3.9 / (3.9+12.7)
	// dacvalue = Vdac / 2.5 * 65536

	//float Vdac = (voltage + (2.5f*100.f/71.5f)) * (20.0f / 100.f);
	//float dacvalue = Vdac * (65536.f/2.5f);
	//if (dacvalue < 0.0f) dacvalue = 0.0f;
	//else if (dacvalue > 65535.0f) dacvalue = 65535.0f;

	// Bert swapped the channels by accident
	//if (channel == 0) channel = 1; else if(channel == 1) channel = 0;

	//uint32_t intdacvalue = (uint32_t)dacvalue;
	//dac8564_write(channel, intdacvalue /*& ~15*/);

	switch (channel) {
	case 0:
		ports_value(synth_param[VCO1_FREQ].port, ictrl);
		break;
	case 1:
		ports_value(synth_param[VCO2_FREQ].port, ictrl);
		break;
	case 2:
		ports_value(synth_param[VCO3_FREQ].port, ictrl);
		break;
	case 3:
		ports_value(synth_param[VCO4_FREQ].port, ictrl);
		break;
	case 4:
		ports_value(synth_param[VCO5_FREQ].port, ictrl);
		break;
	case 5:
		ports_value(synth_param[VCO6_FREQ].port, ictrl);
		break;
	case 6:
		ports_value(synth_param[VCO7_FREQ].port, ictrl);
		break;
	case 7:
		ports_value(synth_param[NOISE_COLOR].port, ictrl);
		break;
	}
	ports_flush();
}

#define BINCOUNT (10)
#define v0_bin (3)
// 1 octave down because our control range goes only from -3V up to 10V
#define v0_freq (0.5f*261.6255653f)
#define binwidth (1.0f)

struct caldata_t {
	uint32_t magic;
	//float inv_scale_error[BINCOUNT];

	uint16_t dacvalue[BINCOUNT];
	int16_t dacerror[BINCOUNT];

	float inv_offset_error[BINCOUNT];
	float calpitch[BINCOUNT];
	int minbin, maxbin;
};

static struct caldata_t caldata[8] = {0};

struct flashcontent_t
{
	struct caldata_t caldata[8];
	//POPSettings settings;
};

const float minbinvoltage = 0.0f;

uint16_t autotune_note_to_dac(int osc, uint16_t note)
{
	int lobin = (note >> 8) / 12;

	if (lobin < caldata[osc].minbin) {
		return caldata[osc].dacvalue[caldata[osc].minbin];
	}
	else if (lobin >= caldata[osc].maxbin) {
		return caldata[osc].dacvalue[caldata[osc].maxbin];
	}

	// else
	int interp = note - ((lobin * 12) << 8);
	uint32_t v = (uint32_t)caldata[osc].dacvalue[lobin] +
			((uint32_t)(caldata[osc].dacvalue[lobin + 1] - caldata[osc].dacvalue[lobin]) * interp / 3072);
	return (uint16_t)v;
}


void Vout(int channel, uint16_t note)
{
	Voutraw(channel, autotune_note_to_dac(channel, note));
}

void resetcal()
{
	for (int c = 0; c < 8; c++) {
		for (int i = 0; i < BINCOUNT; i++) {
			//caldata.inv_scale_error[i] = 1.0f;
			caldata[c].inv_offset_error[i] = 0.0f;
			caldata[c].calpitch[i] = 0.0f;
		}
		caldata[c].minbin = 0;
		caldata[c].maxbin = BINCOUNT-1;
	}
}

struct flashcontent_t flashdata;

int loadcal()
{
	//FLASH1_Read(FLASH1_DeviceData, FLASH1_USER_AREA0_DATA_ADDRESS, &caldata[0], sizeof(caldata));
	memcpy(&flashdata, (void*)FSL_FEATURE_EEPROM_BASE_ADDRESS, sizeof(flashdata));

	if (flashdata.caldata[0].magic == 0x55612348) {
		memcpy(&caldata[0], &flashdata.caldata[0], 8*sizeof(struct caldata_t));
		return 0;
	}
	else {
		resetcal();
		return 1;
	}
}

#define EEPROM_PAGE_SIZE (128)

void writecal()
{
	memcpy(&flashdata.caldata[0], &caldata[0], 8*sizeof(struct caldata_t));
	flashdata.caldata[0].magic = 0x55612348;

	int pagecount = (sizeof(struct flashcontent_t) + (EEPROM_PAGE_SIZE-1)) / EEPROM_PAGE_SIZE;

	for (int i = 0; i < pagecount; i++) {
		unsigned char* flashdataptr = (unsigned char*)&flashdata;
		EEPROM_WritePage(EEPROM, i, (uint32_t *)&flashdataptr[i * EEPROM_PAGE_SIZE]);
	}
}

void autotune_init()
{
	if (FSL_FEATURE_EEPROM_SIZE < sizeof(struct flashcontent_t)) { for (;;) ; }

	CLOCK_EnableClock(kCLOCK_InputMux);
	INPUTMUX_AttachSignal(INPUTMUX, 0, kINPUTMUX_GpioPort0Pin31ToPintsel);

	CLOCK_EnableClock(kCLOCK_Pint);
	RESET_PeripheralReset(kPINT_RST_SHIFT_RSTn);

	// edge sensitive: ISEL=0
	// rising edge: SIENR=1
	PINT->SIENR = 1U << 0;

	eeprom_config_t config;
	EEPROM_GetDefaultConfig(&config);
	EEPROM_Init(EEPROM, &config, BOARD_BOOTCLOCKRUN_CORE_CLOCK);

	if (loadcal())
	{
		GPIO->B[BOARD_INITPINS_LED_PORT][BOARD_INITPINS_LED_PIN] = 0;
		autotune_start();
		GPIO->B[BOARD_INITPINS_LED_PORT][BOARD_INITPINS_LED_PIN] = 1;
	}
}

void autotune_set_routing(int osc)
{
    shiftctrl_clear(SEL1SQR);
    shiftctrl_clear(SEL1SAW);
    shiftctrl_clear(SEL1TRI);
    shiftctrl_clear(SEL1SUB);
    shiftctrl_clear(SEL2SQR);
    shiftctrl_clear(SEL2SAW);
    shiftctrl_clear(SEL2TRI);
    shiftctrl_clear(SEL2SUB);
    shiftctrl_clear(SEL3SQR);
    shiftctrl_clear(SEL3SAW);
    shiftctrl_clear(SEL3TRI);
    shiftctrl_clear(SEL3SUB);
    shiftctrl_clear(SEL4SQR);
    shiftctrl_clear(SEL5SQR);
    shiftctrl_clear(SEL6SQR);
    shiftctrl_clear(SEL7SQR);

    shiftctrl_clear(SELVCF21A);
    shiftctrl_clear(SELVCF21B);
    shiftctrl_clear(SELVCF22A);
    shiftctrl_clear(SELVCF22B);
    shiftctrl_clear(SELVCF23A);
    shiftctrl_clear(SELVCF23B);
    shiftctrl_clear(SELVCF24A);
    shiftctrl_clear(SELVCF24B);

    shiftctrl_clear(SELVCOSYNC1);
    shiftctrl_clear(SELVCOSYNC2);
    shiftctrl_clear(SELVCOSYNC3);

    shiftctrl_set(SELMUTEDNSAW);
    shiftctrl_set(SELMUTEVCOTUNE);
    shiftctrl_clear(SELTUNEVCF);
    shiftctrl_update();

    // disable all outputs
	ports_value(PORT_VCF1_L_LIN, 0);
	ports_value(PORT_VCF2_L_LIN, 0);
	ports_value(PORT_CLEANF_L_LIN, 0);
	ports_value(PORT_VCF1_L_LOG, 0xffff);
	ports_value(PORT_VCF2_L_LOG, 0xffff);
	ports_value(PORT_CLEANF_L_LOG, 0xffff);
	ports_value(PORT_VCF1_R_LIN, 0);
	ports_value(PORT_VCF2_R_LIN, 0);
	ports_value(PORT_CLEANF_R_LIN, 0);
	ports_value(PORT_VCF1_R_LOG, 0xffff);
	ports_value(PORT_VCF2_R_LOG, 0xffff);
	ports_value(PORT_CLEANF_R_LOG, 0xffff);

	ports_value(PORT_VCF1_RES, 0xffff);
	ports_value(PORT_VCF2_A_CV, 0xffff);
	ports_value(PORT_VCF2_B_CV, 0xffff);
	ports_value(PORT_VCF2_C_CV, 0xffff);
	ports_value(PORT_VCF2_D_CV, 0xffff);
	ports_value(PORT_VCF2_A_RES, 0xffff);
	ports_value(PORT_VCF2_B_RES, 0xffff);
	ports_value(PORT_VCF2_C_RES, 0xffff);
	ports_value(PORT_VCF2_D_RES, 0xffff);

	ports_value(PORT_VCO1_PW, 0x8000);
	ports_value(PORT_VCO2_PW, 0x8000);
	ports_value(PORT_VCO3_PW, 0x8000);
	ports_value(PORT_VCO4_PW, 0xA000);
	ports_value(PORT_VCO5_PW, 0xA000);
	ports_value(PORT_VCO6_PW, 0xA000);
	ports_value(PORT_VCO7_PW, 0xA000);
	ports_value(PORT_VCO123_FM2, 0);
	ports_value(PORT_VCO123_FM3, 0);

	ports_flush();

    switch (osc) {
	case 0:
	    shiftctrl_set(SEL1SQR);
	    break;
	case 1:
	    shiftctrl_set(SEL2SQR);
	    break;
	case 2:
	    shiftctrl_set(SEL3SQR);
	    break;
	case 3:
	    shiftctrl_set(SEL4SQR);
	    break;
	case 4:
	    shiftctrl_set(SEL5SQR);
	    break;
	case 5:
	    shiftctrl_set(SEL6SQR);
	    break;
	case 6:
	    shiftctrl_set(SEL7SQR);
	    break;
	case 7:
	    shiftctrl_clear(SELMUTEDNSAW);
	    break;
	}
    shiftctrl_update();
}

extern volatile int sctimer_counter;

//#define AVERAGECOUNT 32
//extern volatile int32_t doaveragecount;
//extern volatile int32_t compareindex;
//extern volatile uint32_t comparetimes[AVERAGECOUNT];
//extern volatile uint32_t elapsedavg[AVERAGECOUNT];
//extern volatile int32_t avgindex;
//extern volatile uint32_t elapsedavgsum;

void enable_pin_int()
{
	NVIC_ClearPendingIRQ(PIN_INT0_IRQn);
    PINT->IST = (1U << 0);
    NVIC_SetPriority(PIN_INT0_IRQn, 0);
	EnableIRQ(PIN_INT0_IRQn);
}

void disable_pin_int()
{
	DisableIRQ(PIN_INT0_IRQn);

}

void resetcmp(int averagecount)
{
	//__disable_irq();
	compareindex = 0;
	comparetimes[0] = 0;
	avgindex = 0;
	elapsedavgsum = 0;
	memset((void*)elapsedavg, 0, AVERAGECOUNT*4);
	doaveragecount = averagecount;
	//__enable_irq();

	enable_pin_int();
}

uint32_t timer_value() { return 0; }

float readhz(int averagecount)
{
	//uint32_t starttime = timer_value();

	resetcmp(averagecount);

	while (avgindex < 2 /*&& (timer_value() - starttime) < 12000000*/) {
		ports_refresh();
	}
	__disable_irq();
	uint32_t avg = elapsedavg[1];

	disable_pin_int();
	__enable_irq();

	if (avgindex < 2)
	{
		return 40000.0f;
	}

	return (180000000.0f * 0.125f) / (float)avg;
}

float binfreq(int bin)
{
	return (float)(1 << bin) * 16.35159798313f;
}

uint16_t bindacvalue(int bin)
{
	return (uint16_t) ((65535 * bin) / 10);
}

uint16_t bindacvalue_e(int bin, int32_t error)
{
	int32_t value = ((65535 * bin) / 10);
	value += error;
	if (value < 0) return 0;
	else if (value > 65535) return 65535;
	return (uint16_t) value;
}

float binvalue(int bin)
{
	return (float)bin;
}

int binsamples(int bin)
{
	if (bin < 4) return 4;
	if (bin < 7) return 8;
	return 16;
}

typedef struct _meas_entry_t {
	int osc;
	int bin;
	int i;
	uint16_t targetvalue;
	int binsamples;
	float target_freq;
	float pitch;
	uint16_t dacerror;
	float low_pitch;
	uint16_t low;
	float high_pitch;
	uint16_t high;
	int status;
} meas_entry_t;

int meas_logpos = 0;
meas_entry_t meas_log[1024];

void log_meas(int osc, int bin, int i, uint16_t targetvalue, int binsamples, float target_freq, float pitch, uint16_t dacerror, float low_pitch, uint16_t low, float high_pitch, uint16_t high, int status)
{
	if (meas_logpos == 1024) {
		return;
	}

	meas_entry_t* e = &meas_log[meas_logpos];
	e->osc = osc;
	e->bin = bin;
	e->i = i;
	e->targetvalue = targetvalue;
	e->binsamples = binsamples;
	e->target_freq = target_freq;
	e->pitch = pitch;
	e->dacerror = dacerror;
	e->low_pitch = low_pitch;
	e->low = low;
	e->high_pitch = high_pitch;
	e->high = high;
	e->status = status;
	meas_logpos++;
}

uint16_t uint16_add_clamp(uint16_t value, uint16_t add)
{
	int32_t x = (int32_t)value + (int32_t)add;
	if (x > 65535) return 65535;
	return x;
}

uint16_t uint16_sub_clamp(uint16_t value, uint16_t sub)
{
	int32_t x = (int32_t)value - (int32_t)sub;
	if (x < 0) return 0;
	return x;
}

volatile float startf;

void autotune_control_out(int osc, int state)
{
	uint8_t data[2];
	data[0] = control_state_autotune_status;
	data[1] = (osc << 4) | state;
	control_out_queue(data, 2);
	control_out_queue_wait();
}

int autotune(int osc)
{
	control_out_queue_wait();
	autotune_control_out(osc, 0);
	DisableIRQ(USART_DSP_FLEXCOMM_IRQN);

	__disable_irq();
	sctimer_counter = 0;
	__enable_irq();

	//while(1)
		autotune_set_routing(osc);

	int osc_offset = 0;

	if (osc == 7)
		osc_offset = 0x6000;

	Voutraw(osc, osc_offset);
	WAIT1_Waitms(1);
	startf = readhz(16);

	if (startf >= 15.0) {
		//printf("low freq out of range: %f >= 15.0\n", startf);
		EnableIRQ(USART_DSP_FLEXCOMM_IRQN);
		autotune_control_out(osc, 0x0f);
		return 1;
	}

	for (int bin = 0; bin < BINCOUNT; bin++) {

		float target_freq = binfreq(bin);
		//printf("target: %f\n", target_freq);
		caldata[osc].calpitch[bin] = target_freq;
		if (bin == 0) {
			caldata[osc].inv_offset_error[bin] = 0.0f;
			caldata[osc].dacerror[bin] = 0;
			caldata[osc].dacvalue[bin] = bindacvalue_e(bin, osc_offset);
		}
		else {
			caldata[osc].inv_offset_error[bin] = caldata[osc].inv_offset_error[bin - 1];
			caldata[osc].dacerror[bin] = caldata[osc].dacerror[bin - 1];
			caldata[osc].dacvalue[bin] = bindacvalue_e(bin, (int32_t)caldata[osc].dacerror[bin] + osc_offset);
		}
		//float last_real_scale_error = 10000.0f;

		Voutraw(osc, caldata[osc].dacvalue[bin]);
		float pitch = readhz(binsamples(bin));
		float high_pitch;
		uint16_t high;
		uint16_t low;
		float low_pitch;
		if (pitch > target_freq) {
			high_pitch = pitch;
			high = caldata[osc].dacvalue[bin];
			low = uint16_sub_clamp(high, 3000);
			Voutraw(osc, low);
			low_pitch = readhz(binsamples(bin));
		}
		else {
			low_pitch = pitch;
			low = caldata[osc].dacvalue[bin];
			high = uint16_add_clamp(low, 3000);
			Voutraw(osc, high);
			high_pitch = readhz(binsamples(bin));
		}

		for (int i = 0; i < 8; i++) {
			// pick best possible value with linear interpolation

			EnableIRQ(USART_DSP_FLEXCOMM_IRQN);
			autotune_control_out(osc, bin+1);
			DisableIRQ(USART_DSP_FLEXCOMM_IRQN);

			float targetvalue = ((target_freq - low_pitch) / (float)(high_pitch - low_pitch)) * (float)(high - low) + (float)low;
			uint16_t ivalue = (uint16_t) targetvalue;
			if (targetvalue < 0) ivalue = 0;
			else if (targetvalue > 65535) ivalue = 65535;

			//if (targetvalue <= 1 || targetvalue >= 65535) {
			//	log_meas(osc, bin, i, targetvalue, binsamples(bin), target_freq, pitch, caldata[osc].dacerror[bin], low_pitch, low, high_pitch, high, 1);
			//	break;
			//}

			Voutraw(osc, ivalue);
			float pitch = readhz(binsamples(bin));
			//printf("pitch: %f\n", pitch);

			if (abs((int32_t)ivalue - (int32_t)low) < 4) {
				log_meas(osc, bin, i, ivalue, binsamples(bin), target_freq, pitch, caldata[osc].dacerror[bin], low_pitch, low, high_pitch, high, 11);
				break;
			}
			if (abs((int32_t)ivalue - (int32_t)high) < 4) {
				log_meas(osc, bin, i, ivalue, binsamples(bin), target_freq, pitch, caldata[osc].dacerror[bin], low_pitch, low, high_pitch, high, 12);
				break;
			}

			log_meas(osc, bin, i, ivalue, binsamples(bin), target_freq, pitch, caldata[osc].dacerror[bin], low_pitch, low, high_pitch, high, 0);

			if (pitch > target_freq) {
				//if (pitch > high_pitch) {
				//	// fail
				//	log_meas(osc, bin, i, ivalue, binsamples(bin), target_freq, pitch, caldata[osc].dacerror[bin], low_pitch, low, high_pitch, high, 2);
				//	return 1;
				//}

				high_pitch = pitch;
				high = ivalue;
			}
			else {
				//if (pitch < low_pitch) {
				//	// fail
				//	log_meas(osc, bin, i, ivalue, binsamples(bin), target_freq, pitch, caldata[osc].dacerror[bin], low_pitch, low, high_pitch, high, 3);
				//	return 1;
				//}

				low_pitch = pitch;
				low = ivalue;
			}

			caldata[osc].dacvalue[bin] = ivalue;
			caldata[osc].dacerror[bin] = (int32_t)caldata[osc].dacvalue[bin] - (int32_t)bindacvalue_e(bin, osc_offset);

			if (high <= low + 4) {
				log_meas(osc, bin, i, ivalue, binsamples(bin), target_freq, pitch, caldata[osc].dacerror[bin], low_pitch, low, high_pitch, high, 10);

				break;
			}

/*			float new_offset_error = caldata[osc].inv_offset_error[bin] + -logf(pitch / target_freq) / logf(2.0f);
			float real_scale_error = pitch / target_freq;

			if (i > 2 && fabs(logf(real_scale_error)) > fabs(logf(last_real_scale_error))) {
				log_meas(osc, bin, i, binvalue(bin), binsamples(bin), target_freq, pitch, caldata[osc].inv_offset_error[bin], last_real_scale_error, new_offset_error, real_scale_error, 0);

				continue;
			}
			else if (i > 2) {
				i--;
			}*/

			//log_meas(osc, bin, i, binvalue(bin), binsamples(bin), target_freq, pitch, caldata[osc].inv_offset_error[bin], last_real_scale_error, new_offset_error, real_scale_error, 1);

//			last_real_scale_error = real_scale_error;
//			caldata[osc].inv_offset_error[bin] = new_offset_error;
		}
	}

	EnableIRQ(USART_DSP_FLEXCOMM_IRQN);

	autotune_control_out(osc, 0x0e);

	return 0;
}

volatile int autotune_phase = 0;

void autotune_start()
{
	int r;

	autotune_phase = 1;

	resetcal();

	autotune_phase = 2;

	r = autotune(0);
	if (r) {
		autotune_phase |= 0x100;
		//return;
	}

	autotune_phase = 3;

	r = autotune(1);
	if (r) {
		autotune_phase |= 0x100;
		//return;
	}

	autotune_phase = 4;

	r = autotune(2);
	if (r) {
		autotune_phase |= 0x100;
		//return;
	}

	autotune_phase = 5;

	r = autotune(3);
	if (r) {
		autotune_phase |= 0x100;
		//return;
	}

	autotune_phase = 6;

	r = autotune(4);
	if (r) {
		autotune_phase |= 0x100;
		//return;
	}

	autotune_phase = 7;

	r = autotune(5);
	if (r) {
		autotune_phase |= 0x100;
		//return;
	}

	autotune_phase = 8;

	r = autotune(6);
	if (r) {
		autotune_phase |= 0x100;
		//return;
	}

	autotune_phase = 9;

	r = autotune(7);
	if (r) {
		autotune_phase |= 0x100;
		//return;
	}

	autotune_phase = 10;
	autotune_control_out(0xf, 0x00);

	//printf("writecal\n");

	writecal();

	//printf("done\n");

	autotune_phase = 11;
	autotune_control_out(0xf, 0x01);

	//Vout(6, 2*256);

	autotune_phase = 12;
	autotune_control_out(0xf, 0x02);
}

