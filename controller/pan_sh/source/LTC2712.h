/*
 * LTC2712.h
 *
 *  Created on: Oct 6, 2018
 *      Author: Lauri
 */

#ifndef LTC2712_H_
#define LTC2712_H_

#include "spi_sched.h"

void ltc2712_init();
void ltc2712_write(int ic, int ch, uint16_t value, read_callback_t read_callback);

#endif /* LTC2712_H_ */
