/*
 * hp.c
 *
 *  Created on: 28 Apr 2018
 *      Author: ljkop
 */

#include "hp.h"
#include "intrinsic.h"
#include <math.h>

extern uint32_t timer_value_nonisr();

void hp_init(struct hp_state_t* hp)
{
	hp->value = 0;
	hp->last = 0;
	hp->time = 0;
	hp_set_speed(hp, 0);
}

int32_t hp_update(struct hp_state_t* hp, int32_t value)
{
	uint32_t timer_count = timer_value_nonisr();
	uint32_t timer_delta = timer_count - hp->time;

	if (timer_delta > 0x80000) timer_delta = 1;
	else
		timer_delta >>= 15;

	if (timer_delta == 0) {
		return hp->last >> 16;
	}
	hp->time = timer_count;

	int32_t a = hp->a;
	int32_t b = hp->b;

	if (value < -32767) value = -32767;
	else if (value > 32767) value = 32767;
	value <<= 16;

	int32_t res;
	while (timer_delta--)
	{
		//newvalue = b * value - b * lastvalue + a * lastoutput;
		res = smlal32_hi(0, value, b);
		res = smlal32_hi(res, hp->value, -b);
		res = smlal32_hi(res, hp->last, a);
		hp->value = value;
		hp->last = res << 1;
	}

	return res >> 15;
}

void hp_set_speed(struct hp_state_t* hp, uint16_t speed)
{
	float f = 0.0001f * powf(0.4f / 0.0001f, (float)speed / 65535.f);
	int32_t a = (int32_t)((float)0x7FFFFFFF * expf(-2.0f * 3.141592654f * f));
	int32_t b = (a/2) + 0x3FFFFFFF;
	hp->a = a;
	hp->b = b;
}

int32_t lp_update(struct hp_state_t* hp, int32_t value)
{
	uint32_t timer_count = timer_value_nonisr();
	uint32_t timer_delta = timer_count - hp->time;

	if (timer_delta > 0x80000) timer_delta = 1;
	else
		timer_delta >>= 15;

	if (timer_delta == 0) {
		return hp->last >> 16;
	}
	hp->time = timer_count;

	int32_t a = hp->a;

	if (value < -32767) value = -32767;
	else if (value > 32767) value = 32767;
	value <<= 16;

	int32_t res;
	while (timer_delta--)
	{
		//newvalue = b * value - b * lastvalue + a * lastoutput;
		res = smlal32_hi(0, value, 0x7FFFFFFF - a);
		res = smlal32_hi(res, hp->last, a);
		hp->value = value;
		hp->last = res << 1;
	}

	return res >> 15;
}
