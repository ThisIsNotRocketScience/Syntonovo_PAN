#pragma once
#include <stdint.h>

void control_init();
//void control_write();

void control_set_receive(int enable);

void control_cb(int param, int subparam, uint16_t value);

void control_out_queue(uint8_t* data, int count);
void control_out_queue_wait();

typedef enum {
	control_state_idle = 0,
	control_state_mod_fb = 1,
	control_state_autotune_status = 2
} control_cb_state_t;

