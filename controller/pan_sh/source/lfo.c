/*
 * lfo.c
 *
 *  Created on: Feb 1, 2018
 *      Author: ld0d
 */

#include <math.h>
#include "lfo.h"

extern uint32_t timer_value_nonisr();

uint16_t waveform[2049];

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

typedef struct _lfo_state
{
	uint32_t phase;
	uint32_t add;
	uint32_t time;
} lfo_state_t;

#define LFO_COUNT (256)
static lfo_state_t lfo_state[LFO_COUNT];

static void lfo_init_state(lfo_state_t* state)
{
	//state->phase = 1 << 30;
	//state->add = 1 << 31;
	state->phase = 0;
	state->add = 4294967;
}

void lfo_init()
{
	waveform_init();

	for (int i = 0; i < LFO_COUNT; i++) {
		lfo_init_state(&lfo_state[i]);
	}
}

uint16_t lfo_update(int lfoid)
{
	uint32_t timercount = timer_value_nonisr();
	uint32_t timerdelta = timercount - lfo_state[lfoid].time;
	lfo_state[lfoid].time = timercount;
	uint32_t phase = lfo_state[lfoid].phase + (lfo_state[lfoid].add * (timerdelta >> 11));
	lfo_state[lfoid].phase = phase;
	return waveform_sample(phase);
}

void lfo_set_speed(int lfoid, uint16_t speed)
{
	lfo_state[lfoid].add = (uint32_t) (((float)(1 << 24L)) * expf(-(float)(65535 - speed) * 0.0001f));
}

void lfo_reset(int lfoid, uint16_t phase)
{
	lfo_state[lfoid].phase = phase << 16;
}
