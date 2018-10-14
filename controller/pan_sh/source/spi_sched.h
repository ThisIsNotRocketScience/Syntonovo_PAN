/*
 * spi_sched.h
 *
 *  Created on: Feb 1, 2018
 *      Author: ld0d
 */

#ifndef SPI_SCHED_H_
#define SPI_SCHED_H_

#include <stdint.h>

typedef void (*read_callback_t) (uint32_t data, void* user);

void spi_sched_init();
void spi_sched_start(); // hijack fsl_spi
void spi_sched_write(int spi_bus_id, int spi_ssel, uint8_t* data, int count, read_callback_t read_callback, void* read_user_data);

#endif /* SPI_SCHED_H_ */
