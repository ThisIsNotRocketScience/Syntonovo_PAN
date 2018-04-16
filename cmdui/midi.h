#pragma once

#include <stdint.h>

void player_init(void);
void player_deinit(void);

void midi_learn(int enable);
void midi_assign_last(uint16_t paramid);
void midi_assign(uint8_t channel, uint8_t cc, uint16_t paramid, uint16_t min, uint16_t max);

void midi_setswitch(int switchid);
void midi_clearswitch(int switchid);
