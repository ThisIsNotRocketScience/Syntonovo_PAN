/*
 * hp.h
 *
 *  Created on: 28 Apr 2018
 *      Author: ljkop
 */

#ifndef HP_H_
#define HP_H_

#include <stdint.h>

struct hp_state_t
{
	int32_t value;
	int32_t last;
	int32_t a;
	int32_t b;
	uint32_t time;
};

void hp_init(struct hp_state_t* hp);
int32_t hp_update(struct hp_state_t* hp, int32_t value);
void hp_set_speed(struct hp_state_t* hp, uint16_t speed);

int32_t lp_update(struct hp_state_t* hp, int32_t value);

#endif /* HP_H_ */
