/*
 * MAX5134.h
 *
 *  Created on: Feb 1, 2018
 *      Author: ld0d
 */

#ifndef MAX5134_H_
#define MAX5134_H_

void max5134_init();
void max5134_write(int ic, int ch, uint16_t value);

#endif /* MAX5134_H_ */
