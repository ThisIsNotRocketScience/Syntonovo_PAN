/*
 * control.h
 *
 *  Created on: 25 Mar 2018
 *      Author: koponenl
 */

#ifndef CONTROL_H_
#define CONTROL_H_

void control_init();
//void control_write();

void control_cb(int param, int subparam, uint16_t value);

#endif /* CONTROL_H_ */
