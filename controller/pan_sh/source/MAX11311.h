/*
 * MAX11311.h
 *
 *  Created on: Jan 29, 2018
 *      Author: ld0d
 */

#ifndef MAX11311_H_
#define MAX11311_H_

//#include "fsl_spi.h"

#include "MAX11300_const.h"

void max11311_set_port(int ic, int ch, int mode);

void max11311_init();
void max11311_read(int ic, int ch, uint16_t* result);
void max11311_write(int ic, int ch, uint16_t value);

void max11311_write_done();

#endif /* MAX11311_H_ */
