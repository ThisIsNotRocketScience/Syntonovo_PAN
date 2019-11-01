/*
 * lfo.c
 *
 *  Created on: Feb 1, 2018
 *      Author: ld0d
 */

#include <math.h>
#include "lfo.h"
#include "intrinsic.h"

extern uint32_t timer_value_nonisr();

#define DATA_SECTION
#define TEXT_SECTION

DATA_SECTION
int64_t m64(int32_t a, int32_t b)
{
	return (((int64_t)a * (int64_t)b) >> 32);
}

#ifdef __arm__
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE
#endif
TEXT_SECTION
int32_t NOINLINE div64(int32_t a, int32_t b)
{
#if 0
	uint32_t a_hi = a;
	a = 0;
	uint32_t carry = 0;
	for (int i = 0; i < 32; i++) {
	                if ((int32_t)(a_hi^b) < 0) {
	                        uint32_t as_hi = (a_hi<<1) | (a>>31);
	                        uint32_t as = (a<<1) | (carry?1:0);
	                        uint32_t ass_hi = as_hi + b;
	                        carry = ass_hi < b || ass_hi < as_hi;
	                        a_hi = ass_hi;
	                        a = as;
	                }
	                else {
	                        uint32_t as_hi = (a_hi<<1) | (a>>31);
	                        uint32_t as = (a<<1) | (carry?1:0);
	                        uint32_t ass_hi = as_hi + (-b);
	                        carry = ass_hi < (-b) || ass_hi < as_hi;
	                        a_hi = ass_hi;
	                        a = as;
	                }
	}
	return a;
#endif

#if 1
	                //__arm__
	register int32_t lo = 0;
	register int32_t negden;
	asm("lo .req %[lo]\n"
		"hi .req %[hi]\n"
		"den .req %[den]\n"
		"negden .req %[negden]\n"
		"adds lo, lo\n"
		"rsb negden, den, 0\n"
		".rept 32\n"
		"    adcs lo, lo\n"
		"    adcs hi, hi\n"
		"    ite cs\n"
		"	 movcs negden, den\n"
		"    rsbcc negden, den, 0\n"
		"    adds hi, hi, negden\n"
		".endr\n" : [lo] "+&l" (lo), [hi] "+l" (a), [negden] "=&l" (negden) : [den] "l" (b) : "cc");
	return lo;
#else
	return (int32_t)(((int64_t)a << 31) / b);
#endif
}


static uint16_t waveform[2049];
static uint32_t speed_tab[2049];

static void waveform_init()
{
	for (int i = 0; i < 512; i++) {
		waveform[i] = 32768 + i * 64;
	}
	for (int i = 0; i < 1024; i++) {
		waveform[i + 512] = 65535 - i * 64;
	}
	for (int i = 0; i < 512; i++) {
		waveform[i + 1536] = i * 64;
	}
	waveform[2048] = waveform[0];

	for (int i = 0; i < 2049; i++) {
		speed_tab[i] = (uint32_t) (((float)(1 << 19L)) * expf(-(float)(65536 - (i << 5)) * 0.0001f));
	}
}

static uint16_t waveform_sample(uint32_t phase)
{
	int index = phase >> 21;
	int fract = (phase >> 10) - (index << 11);

	int a = waveform[index];
	int b = waveform[index + 1];
	int diff = b - a;

	return (uint16_t) (a + ((diff * fract) >> 11));
}

static uint32_t speed_sample(uint16_t speed)
{
	int index = speed >> 5;
	int fract = speed & 0x1f;

	int a = speed_tab[index];
	int b = speed_tab[index + 1];
	int diff = b - a;

	return (uint32_t) ((a << 5) + (diff * fract));
}

typedef struct _lfo_state
{
	uint32_t phase;
	uint32_t add;
	uint32_t time;
	int32_t shape;
	int32_t ooshape;
	int32_t ooomshape;
} lfo_state_t;

#define LFO_COUNT (256)
static lfo_state_t lfo_state[LFO_COUNT];

static void update_shape(lfo_state_t* state)
{
	state->ooshape = div64(1<<16, state->shape);
	state->ooomshape = div64(1<<16, 0x7FFFFFFF - state->shape);
}

static void lfo_init_state(lfo_state_t* state)
{
	//state->phase = 1 << 30;
	//state->add = 1 << 31;
	state->phase = 0;
	state->add = 4294967;
	state->shape = 1 << 30;
	update_shape(state);
}

void lfo_init()
{
	waveform_init();

	for (int i = 0; i < LFO_COUNT; i++) {
		lfo_init_state(&lfo_state[i]);
	}
}

int16_t lfo_update(int lfoid)
{
	uint32_t timercount = timer_value_nonisr();
	uint32_t timerdelta = timercount - lfo_state[lfoid].time;
	lfo_state[lfoid].time = timercount;
	uint32_t phase = lfo_state[lfoid].phase + (lfo_state[lfoid].add * (timerdelta >> 11));
	lfo_state[lfoid].phase = phase;
	//return waveform_sample(phase);
#if 1
	int32_t shape = lfo_state[lfoid].shape;
	int32_t sphase = (int32_t)phase;
	if (sphase < -shape) {
		return m64(-(sphase) - 0x7fffffff, lfo_state[lfoid].ooomshape);
	} else if (sphase < shape) {
		return m64(sphase, lfo_state[lfoid].ooshape);
	} else { // phase > 1-shape
		return m64(-(sphase) + 0x7fffffff, lfo_state[lfoid].ooomshape);
	}
#endif
}

void lfo_set_speed(int lfoid, uint16_t speed)
{
//	lfo_state[lfoid].add = (uint32_t) (((float)(1 << 24L)) * expf(-(float)(65535 - speed) * 0.0001f));
	lfo_state[lfoid].add = speed_sample(speed);
}

void lfo_set_shape(int lfoid, uint16_t shape)
{
	lfo_state[lfoid].shape = (int32_t)shape << 15;
	update_shape(&lfo_state[lfoid]);
}

void lfo_reset(int lfoid, uint16_t phase)
{
	lfo_state[lfoid].phase = phase << 16;
}
