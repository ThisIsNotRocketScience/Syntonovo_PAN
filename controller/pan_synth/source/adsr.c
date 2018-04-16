/*
 * adsr.c
 *
 *  Created on: Mar 6, 2018
 *      Author: ld0d
 */

#include "adsr.h"
#include "intrinsic.h"
#include <math.h>

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
	adsr_state[adsrid].a_const = (uint32_t)((float)0xFFFFFFFF * (1.0f - expf(-1.0f / (float)a)));
}

void adsr_set_d(int adsrid, uint32_t d)
{
	adsr_state[adsrid].d = d;
	adsr_state[adsrid].d_const = (uint32_t)((float)0x7FFFFFFF * (1.0f - expf(-1.0f / (float)d)));
}

void adsr_set_s(int adsrid, uint32_t s)
{
	adsr_state[adsrid].s = s;
}

void adsr_set_r(int adsrid, uint32_t r)
{
	adsr_state[adsrid].r = r;
	adsr_state[adsrid].r_const = (uint32_t)((float)0xFFFFFFFF * expf(-1.0f / (float)r));
}

void adsr_set_gate(int adsrid, int gate)
{
	if (gate && !adsr_state[adsrid].gate) {
		adsr_state[adsrid].state = 1;
	}
	else if (!gate) {
		adsr_state[adsrid].state = 0;
	}

	adsr_state[adsrid].gate = !!gate;
}

uint16_t adsr_update(int adsrid)
{
	adsr_state_t* adsr = &adsr_state[adsrid];
	switch (adsr->state) {
	case 1: // attack
		{
			uint32_t v = umlal32_hi(adsr->value, (0xFFFFFFFF - adsr->value), adsr->a_const);
			if (v > 0x7FFFFFFF) {
				v = 0x7FFFFFFF;
				adsr->state = 2;
			}
			adsr->value = v;
		}
		break;
	case 2: // decay
		{
			adsr->value = smlal32_hi(adsr->value, (adsr->s - adsr->value), adsr->d_const);
		}
		break;
	default: // release
		adsr->value = umull32_hi(adsr->value, adsr->r_const);
		break;
	}

	return adsr->value >> 15;
}
