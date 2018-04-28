/*
 * shiftctrl.h
 *
 *  Created on: Mar 17, 2018
 *      Author: ld0d
 */

#ifndef SHIFTCTRL_H_
#define SHIFTCTRL_H_

void shiftctrl_init();
void shiftctrl_set(int flag);
void shiftctrl_clear(int flag);
int shiftctrl_flag_state(int flag);
void shiftctrl_update();

#endif /* SHIFTCTRL_H_ */
