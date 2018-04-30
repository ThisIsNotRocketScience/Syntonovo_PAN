/*
 * adsr.h
 *
 *  Created on: Mar 6, 2018
 *      Author: ld0d
 */

#ifndef ADSR_H_
#define ADSR_H_

#include <stdint.h>

void adsr_init();
uint16_t adsr_update(int lfoid, int release_dampen);

void adsr_set_a(int adsrid, uint32_t a);
void adsr_set_d(int adsrid, uint32_t d);
void adsr_set_s(int adsrid, uint32_t s);
void adsr_set_r(int adsrid, uint32_t r);
void adsr_set_gate(int adsrid, int gate, int reset);

#endif /* ADSR_H_ */
