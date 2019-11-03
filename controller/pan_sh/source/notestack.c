/*
 * notestack.c
 *
 *  Created on: Apr 3, 2018
 *      Author: ld0d
 */

#include <stdlib.h>
#include "notestack.h"

#define NUM_STACKS 4
#define NOTESTACK_SIZE 32

typedef struct {
	int notestack_top;
	struct note_t notestack[NOTESTACK_SIZE];
	enum keyboard_mode_t notestack_mode;
} notestack_t;

static notestack_t stacks[NUM_STACKS];

typedef int (*qsort_cmp)(const void *, const void *);

int notecmp_lowest(struct note_t* left, struct note_t* right)
{
	if (left->note > right->note) return -1;
	else if (left->note < right->note) return 1;
	return 0;
}

int notecmp_highest(struct note_t* left, struct note_t* right)
{
	if (left->note < right->note) return -1;
	else if (left->note > right->note) return 1;
	return 0;
}

int notecmp_latest(struct note_t* left, struct note_t* right)
{
	if (left->index < right->index) return -1;
	else if (left->index > right->index) return 1;
	return 0;
}

int notestack_empty(int stack)
{
	return stacks[stack].notestack_top == 0;
}

//__attribute__( ( section(".data") ) )
struct note_t notestack_first(int stack)
{
	if (stacks[stack].notestack_top == 0) {
		return stacks[stack].notestack[0];
	}
	return stacks[stack].notestack[stacks[stack].notestack_top - 1];
}

//__attribute__( ( section(".data") ) )
struct note_t notestack_n(int stack, int n)
{
	if (stacks[stack].notestack_top <= n) {
		return stacks[stack].notestack[stacks[stack].notestack_top - 1];
	}
	return stacks[stack].notestack[stacks[stack].notestack_top - n - 1];
}

#if 0
//__attribute__( ( section(".data") ) )
struct note_t notestack_second()
{
	if (notestack_top == 0) {
		return notestack[0];
	}
	else if (notestack_top == 1) {
		return notestack[0];
	}
	return notestack[notestack_top - 2];
}

struct note_t notestack_third()
{
	if (notestack_top == 0) {
		return notestack[0];
	}
	else if (notestack_top == 1) {
		return notestack[0];
	}
	else if (notestack_top == 2) {
		return notestack[0];
	}
	return notestack[notestack_top - 3];
}
#endif

static struct note_t makenote(int note, uint32_t velocity)
{
	static uint32_t noteindex = 0;
	struct note_t n;
	n.note = note;
	n.vel = velocity;
	n.first = 0;
	n.index = noteindex;
	noteindex++;
	return n;
}

void notestack_push(int stack, int note, uint32_t velocity)
{
	//__DI();
	if (stacks[stack].notestack_top == NOTESTACK_SIZE) {
		//__EI();
		return;
	}
	for (int i = 0; i < stacks[stack].notestack_top; i++) {
		if (stacks[stack].notestack[i].note == note) {
			//__EI();
			return;
		}
	}
	stacks[stack].notestack[stacks[stack].notestack_top++] = makenote(note, velocity);

	if (stacks[stack].notestack_mode == keyboard_mode_low) {
		qsort(stacks[stack].notestack, stacks[stack].notestack_top, sizeof(stacks[0].notestack[0]), (qsort_cmp)notecmp_lowest);
	}
	else if (stacks[stack].notestack_mode == keyboard_mode_high) {
		qsort(stacks[stack].notestack, stacks[stack].notestack_top, sizeof(stacks[0].notestack[0]), (qsort_cmp)notecmp_highest);
	}
	else if (stacks[stack].notestack_mode == keyboard_mode_last) {
		qsort(stacks[stack].notestack, stacks[stack].notestack_top, sizeof(stacks[0].notestack[0]), (qsort_cmp)notecmp_latest);
	}
	if (stacks[stack].notestack_top == 1) {
		stacks[stack].notestack[0].first = 1;
	} else {
		stacks[stack].notestack[0].first = 0;
	}
	//__EI();
}

void notestack_pop(int stack, int note)
{
	stacks[stack].notestack[0].first = 0;
	//__DI();
	if (stacks[stack].notestack_top == 0) {
		//__EI();
		return;
	}
	for (int i = 0; i < stacks[stack].notestack_top; i++) {
		if (stacks[stack].notestack[i].note == note) {
			stacks[stack].notestack_top--;
			for (int j = i; j < stacks[stack].notestack_top; j++) {
				stacks[stack].notestack[j] = stacks[stack].notestack[j + 1];
			}
		}
	}
	//__EI();
}

void notestack_init(int stack, enum keyboard_mode_t mode)
{
	stacks[stack].notestack_top = 0;

	if (mode >= keyboard_mode_start && mode <= keyboard_mode_end) {
		stacks[stack].notestack_mode = mode;
	} else {
		stacks[stack].notestack_mode = keyboard_mode_high;
	}
}
