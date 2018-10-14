/*
 * LTC2712.c
 *
 *  Created on: Oct 6, 2018
 *      Author: Lauri
 */

#include "spi_sched.h"

static uint8_t ltc2712_init_data[4] = {0x3e, 0x00, 0x00};

static volatile int ready = 0;

void transmit_callback(uint32_t data, void* user)
{
	ready = 1;
}

void ltc2712_init()
{
	ready = 0;
	spi_sched_write(4, 1, ltc2712_init_data, 3, transmit_callback, (void*)1);

	while (!ready) ;
}

void ltc2712_write(int ic, int ch, uint16_t value, read_callback_t read_callback)
{
	unsigned char out[3];
	out[0] = 0x70 | ((ch&1) << 1);
	out[1] = (value & 0xFF00) >> 8;
	out[2] = value & 0xFF;

	spi_sched_write(4, 1, out, 3, read_callback, 0);
}
