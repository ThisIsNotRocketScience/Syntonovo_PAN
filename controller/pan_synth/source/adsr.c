/*
 * adsr.c
 *
 *  Created on: Mar 6, 2018
 *      Author: ld0d
 */

#include "adsr.h"
#include "intrinsic.h"
#include <math.h>

extern uint32_t timer_value_nonisr();

typedef struct _adsr_state
{
	uint32_t a;
	uint32_t d;
	uint32_t s;
	uint32_t r;
	uint32_t value;
	uint32_t a_const;
	uint32_t d_const;
	uint32_t r_const;
	uint8_t state;
	uint8_t gate;
	uint32_t time;
} adsr_state_t;

#define ADSR_COUNT (256)

static adsr_state_t adsr_state[ADSR_COUNT];

static void adsr_init_state(adsr_state_t* adsr)
{
	adsr->a = 100;
	adsr->d = 100;
	adsr->s = 0x7FFFFFFF;
	adsr->r = 100;
	adsr->value = 0;
	adsr->a_const = 0;
	adsr->d_const = 0;
	adsr->r_const = 0;
	adsr->state = 0;
	adsr->gate = 0;
}

void adsr_init()
{
	for (int i = 0; i < ADSR_COUNT; i++) {
		adsr_init_state(&adsr_state[i]);

		adsr_set_a(i, 500);
		adsr_set_d(i, 500);
		adsr_set_s(i, 0x3FFFFFFF);
		adsr_set_r(i, 1000);
	}
}

void adsr_set_a(int adsrid, uint32_t a)
{
	adsr_state[adsrid].a = a;
	adsr_state[adsrid].a_const = (uint32_t)((float)0xFFFFFFFF * (1.0f - expf(-10.0f / (float)a)));
}

void adsr_set_d(int adsrid, uint32_t d)
{
	adsr_state[adsrid].d = d;
	adsr_state[adsrid].d_const = (uint32_t)((float)0x7FFFFFFF * (1.0f - expf(-10.0f / (float)d)));
}

void adsr_set_s(int adsrid, uint32_t s)
{
	adsr_state[adsrid].s = s >> 1;
}

void adsr_set_r(int adsrid, uint32_t r)
{
	adsr_state[adsrid].r = r;
	adsr_state[adsrid].r_const = (uint32_t)((float)0xFFFFFFFF * expf(-4.0f / (float)r));
}

void adsr_set_gate(int adsrid, int gate, int reset)
{
	if (gate && !adsr_state[adsrid].gate) {
		adsr_state[adsrid].state = 1;
		if (reset) {
			adsr_state[adsrid].value = 0;
		}
	}
	else if (!gate) {
		adsr_state[adsrid].state = 0;
	}

	adsr_state[adsrid].gate = !!gate;
}

uint16_t adsr_update(int adsrid, int release_dampen)
{
	uint32_t timer_count = timer_value_nonisr();
	uint32_t timer_delta = timer_count - adsr_state[adsrid].time;

	if (timer_delta > 0x40000) timer_delta = 1;
	else timer_delta >>= 13;
	adsr_state_t* adsr = &adsr_state[adsrid];

	if (timer_delta == 0) {
		return adsr->value >> 15;
	}
	adsr->time = timer_count;

	switch (adsr->state) {
	case 1: // attack
		while (timer_delta--)
		{
			uint32_t v = umlal32_hi(adsr->value, (0xFFFFFFFF - adsr->value), adsr->a_const);
			if (v > 0x7FFFFFFF) {
				v = 0x7FFFFFFF;
				adsr->state = 2;
				adsr->value = v;
				break;
			}
			adsr->value = v;
		}
		break;
	case 2: // decay
		while (timer_delta--)
		{
			adsr->value = smlal32_hi(adsr->value, (int32_t)adsr->s - (int32_t)adsr->value, adsr->d_const);
		}
		break;
	default: // release
	{
		uint32_t r_const = adsr->r_const - release_dampen;
		if (r_const > adsr->r_const) r_const = 0;
		while (timer_delta--) {
			adsr->value = umull32_hi(adsr->value, r_const);
		}
		break;
	}
	}

	return adsr->value >> 15;
}
