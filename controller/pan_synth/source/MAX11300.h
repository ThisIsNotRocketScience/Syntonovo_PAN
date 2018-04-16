/*
 * MAX11300.h
 *
 *  Created on: Jan 29, 2018
 *      Author: ld0d
 */

#ifndef MAX11300_H_
#define MAX11300_H_

//#include "fsl_spi.h"

#include "MAX11300_const.h"

void max11300_set_port(int ic, int ch, int mode);

void max11300_init();
uint16_t max11300_read(int ic, int ch);
void max11300_write(int ic, int ch, uint16_t value);

void max11300_write_done();

#endif /* MAX11300_H_ */
