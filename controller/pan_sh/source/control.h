#pragma once
#include <stdint.h>

void control_init();
//void control_write();

void control_cb(int param, int subparam, uint16_t value);

void control_out_queue(uint8_t* data, int count);
