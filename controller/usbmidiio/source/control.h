/*
 * control.h
 *
 *  Created on: 25 Mar 2018
 *      Author: koponenl
 */

#ifndef CONTROL_H_
#define CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

void control_init();
void control_write(uint8_t* data, int count);

void control_cb(int param, int subparam, uint16_t value);

#ifdef __cplusplus
};
#endif

#endif /* CONTROL_H_ */
