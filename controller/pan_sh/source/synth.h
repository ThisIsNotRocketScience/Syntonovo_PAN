#pragma once

#include <stdint.h>

void synth_init();
void synth_run();
void synth_reset();

// access for testmode
void sctimer_init();
void ports_value(int portid, uint16_t value);
void ports_refresh(void);
