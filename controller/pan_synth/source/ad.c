/*
 * ad.c
 *
 *  Created on: Mar 6, 2018
 *      Author: ld0d
 */

#include "ad.h"
#include "intrinsic.h"
#include <math.h>

extern uint32_t timer_value_nonisr();

typedef struct _ad_state
{
	uint32_t a;
	uint32_t d;
	uint32_t value;
	uint32_t a_const;
	uint32_t d_const;
	uint8_t state;
	uint8_t gate;
	uint32_t time;
} ad_state_t;

#define AD_COUNT (256)

static ad_state_t ad_state[AD_COUNT];

static void ad_init_state(ad_state_t* ad)
{
	ad->a = 100;
	ad->d = 100;
	ad->value = 0;
	ad->a_const = 0;
	ad->d_const = 0;
	ad->state = 0;
	ad->gate = 0;
}

void ad_init()
{
	for (int i = 0; i < AD_COUNT; i++) {
		ad_init_state(&ad_state[i]);

		ad_set_a(i, 500);
		ad_set_d(i, 500);
	}
}

void ad_set_a(int adid, uint32_t a)
{
	ad_state[adid].a = a;
	ad_state[adid].a_const = (uint32_t)((float)0xFFFFFFFF * (1.0f - expf(-10.0f / (float)a)));
}

void ad_set_d(int adid, uint32_t d)
{
	ad_state[adid].d = d;
	ad_state[adid].d_const = (uint32_t)((float)0xFFFFFFFF * expf(-10.0f / (float)d));
}

void ad_set_gate(int adid, int gate)
{
	if (gate && !ad_state[adid].gate) {
		ad_state[adid].state = 1;
	}
	else if (!gate) {
		ad_state[adid].state = 0;
	}

	ad_state[adid].gate = !!gate;
}

uint16_t ad_update(int adid)
{
	uint32_t timer_count = timer_value_nonisr();
	uint32_t timer_delta = timer_count - ad_state[adid].time;

	if (timer_delta > 0x40000) timer_delta = 1;
	else
		timer_delta >>= 13;
	ad_state_t* ad = &ad_state[adid];

	if (timer_delta == 0) {
		return ad->value >> 15;
	}
	ad_state[adid].time = timer_count;

	switch (ad->state) {
	case 1: // attack
		while (timer_delta--)
		{
			uint32_t v = umlal32_hi(ad->value, (0xFFFFFFFF - ad->value), ad->a_const);
			if (v > 0x7FFFFFFF) {
				v = 0x7FFFFFFF;
				ad->state = 2;
				ad->value = v;
				break;
			}
			ad->value = v;
		}
		break;
	case 2:
		break;
	default: // decay
		while (timer_delta--) {
			ad->value = umull32_hi(ad->value, ad->d_const);
		}
		break;
	}

	return ad->value >> 15;
}
