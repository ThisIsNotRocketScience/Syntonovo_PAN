#include "peripherals.h"
#include "pin_mux.h"
#include "synth.h"
#include "lfo.h"
#include "shiftctrl.h"

void testmode_run()
{
	lfo_init();
	shiftctrl_init();
	sctimer_init();

	for (int i = 0; i < 128; i++) {
		lfo_set_speed(i, 0xD000);
		lfo_set_shape(i, 0x8000);
	}

	int cur_shift_bit = 0;
	int slow_shift_counter = 0;
	while (1) {

		slow_shift_counter++;
		if (slow_shift_counter >= 40) {
			slow_shift_counter = 0;
			for (int i = 0; i < 64; i++) {
				shiftctrl_clear(i);
			}
			shiftctrl_set(cur_shift_bit);
			cur_shift_bit = (cur_shift_bit + 1) & 63;
			//shiftctrl_set(22);
			shiftctrl_update();
		}

		for (int i = 0; i < 128; i++) {
			int32_t lfo = (int32_t)lfo_update(i);
			ports_value(i, lfo + 0x8000);
		}
		ports_refresh();

		BOARD_INITPINS_GATE_GPIO->B[BOARD_INITPINS_GATE_PORT][BOARD_INITPINS_GATE_PIN] = slow_shift_counter < 20 ? 0 : 1;
	}
}
