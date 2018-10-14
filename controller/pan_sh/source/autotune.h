/*
 * autotune.h
 *
 *  Created on: Apr 1, 2018
 *      Author: ld0d
 */

#ifndef AUTOTUNE_H_
#define AUTOTUNE_H_

#include <stdint.h>

void autotune_init();
void autotune_start();

uint16_t autotune_note_to_dac(int osc, uint16_t note);

#endif /* AUTOTUNE_H_ */
