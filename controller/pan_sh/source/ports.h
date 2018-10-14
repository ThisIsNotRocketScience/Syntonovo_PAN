/*
 * ports.h
 *
 *  Created on: Apr 1, 2018
 *      Author: ld0d
 */

#ifndef PORTS_H_
#define PORTS_H_

typedef void (*portfunc_write_t)(int ic, int ch, uint16_t value);

void ports_set_write(int portid, portfunc_write_t write_func, int ic, int ch);
void ports_value(int portid, uint16_t value);

typedef void (*portfunc_read_t)(int ic, int ch, uint16_t* result);

void ports_set_read(int portid, portfunc_read_t read_func, int ic, int ch);
void ports_input(int portid, uint16_t* value);

#endif /* PORTS_H_ */
