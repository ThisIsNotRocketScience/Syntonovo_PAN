/*
 * notestack.h
 *
 *  Created on: Apr 3, 2018
 *      Author: ld0d
 */

#ifndef NOTESTACK_H_
#define NOTESTACK_H_

#include <stdint.h>

enum keyboard_mode_t {
	keyboard_mode_low = 1,
	keyboard_mode_high = 2,
	keyboard_mode_last = 3,

	keyboard_mode_start = 1,
	keyboard_mode_end = 3
};

void notestack_init(int stack, enum keyboard_mode_t mode);

void notestack_push(int stack, int note, uint32_t velocity);
void notestack_pop(int stack, int note);

struct note_t
{
	int32_t note;
	int32_t vel;
	int32_t first;
	uint32_t index;
};

int notestack_empty(int stack);
struct note_t notestack_n(int stack, int n);
//struct note_t notestack_first();
//struct note_t notestack_second();
//struct note_t notestack_third();

#endif /* NOTESTACK_H_ */
