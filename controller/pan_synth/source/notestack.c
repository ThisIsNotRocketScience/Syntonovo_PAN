/*
 * notestack.c
 *
 *  Created on: Apr 3, 2018
 *      Author: ld0d
 */

#include <stdlib.h>
#include "notestack.h"

#define NOTESTACK_SIZE 32
static int notestack_top = 0;
static struct note_t notestack[NOTESTACK_SIZE];

static enum keyboard_mode_t notestack_mode = keyboard_mode_high;

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

int notestack_empty()
{
	return notestack_top == 0;
}

struct note_t notestack_first()
{
	if (notestack_top == 0) {
		return notestack[0];
	}
	return notestack[notestack_top - 1];
}

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

void notestack_push(int note, uint32_t velocity)
{
	//__DI();
	if (notestack_top == NOTESTACK_SIZE) {
		//__EI();
		return;
	}
	for (int i = 0; i < notestack_top; i++) {
		if (notestack[i].note == note) {
			//__EI();
			return;
		}
	}
	notestack[notestack_top++] = makenote(note, velocity);

	if (notestack_mode == keyboard_mode_low) {
		qsort(notestack, notestack_top, sizeof(notestack[0]), (qsort_cmp)notecmp_lowest);
	}
	else if (notestack_mode == keyboard_mode_high) {
		qsort(notestack, notestack_top, sizeof(notestack[0]), (qsort_cmp)notecmp_highest);
	}
	else if (notestack_mode == keyboard_mode_last) {
		qsort(notestack, notestack_top, sizeof(notestack[0]), (qsort_cmp)notecmp_latest);
	}
	if (notestack_top == 1) {
		notestack[0].first = 1;
	} else {
		notestack[0].first = 0;
	}
	//__EI();
}

void notestack_pop(int note)
{
	notestack[0].first = 0;
	//__DI();
	if (notestack_top == 0) {
		//__EI();
		return;
	}
	for (int i = 0; i < notestack_top; i++) {
		if (notestack[i].note == note) {
			notestack_top--;
			for (int j = i; j < notestack_top; j++) {
				notestack[j] = notestack[j + 1];
			}
		}
	}
	//__EI();
}

void notestack_init(enum keyboard_mode_t mode)
{
	notestack_top = 0;

	if (mode >= keyboard_mode_start && mode <= keyboard_mode_end) {
		notestack_mode = mode;
	} else {
		notestack_mode = keyboard_mode_high;
	}
}
