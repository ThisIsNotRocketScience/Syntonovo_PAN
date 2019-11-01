/*
 * lfo.h
 *
 *  Created on: Feb 1, 2018
 *      Author: ld0d
 */

#ifndef LFO_H_
#define LFO_H_

#include <stdint.h>

void lfo_init();
int16_t lfo_update(int lfoid);

void lfo_reset(int lfoid, uint16_t phase);
void lfo_set_speed(int lfoid, uint16_t speed);
void lfo_set_shape(int lfoid, uint16_t shape);

#endif /* LFO_H_ */
