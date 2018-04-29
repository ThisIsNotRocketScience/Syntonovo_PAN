/*
 * ad.h
 *
 *  Created on: Mar 6, 2018
 *      Author: ld0d
 */

#ifndef AD_H_
#define AD_H_

#include <stdint.h>

void ad_init();
uint16_t ad_update(int lfoid, int release_dampen);

void ad_set_a(int adid, uint32_t a);
void ad_set_d(int adid, uint32_t d);
void ad_set_gate(int adid, int gate, int reset);

#endif /* ad_H_ */
