/*
 * Copyright (c) 2017, NXP Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    LPC54616J512_Project.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "LPC54616.h"
/* TODO: insert other include files here. */
#include <math.h>
#include "MAX11311.h"
#include "MAX11300.h"
#include "MAX5134.h"
#include "spi_sched.h"
#include "ports.h"
#include "shiftctrl.h"
#include "control.h"
#include "synth.h"
/* TODO: insert other definitions and declarations here. */

void codecsetup_init()
{
    for (int i = 0; i < 3; i++) {
    	max11311_set_port(0, i, MODE_ADC_M5_P5V);
    }
    for (int i = 3; i < 6; i++) {
    	max11311_set_port(0, i, MODE_DAC_0_P10V);
    }
    for (int i = 6; i < 12; i++) {
    	max11311_set_port(0, i, MODE_ADC_M5_P5V);
    }

    for (int i = 0; i < 12; i++) {
    	max11311_set_port(1, i, MODE_DAC_0_P10V);
    	max11311_set_port(2, i, MODE_DAC_0_P10V);
    	max11311_set_port(3, i, MODE_DAC_0_P10V);
    }

    for (int i = 0; i < 20; i++) {
    	max11300_set_port(0, i, MODE_DAC_0_P10V);
    	max11300_set_port(1, i, MODE_DAC_0_P10V);
    }
}

void portssetup_init()
{
    for (int i = 0; i < 12; i++) {
    	ports_set_read(0 * 12 + i, max11311_read, 0, i);
    	//ports_set_write(0 * 12 + 6 + i, max11311_write, 0, 6 + i);
    }

    for (int i = 0; i < 12; i++) {
    	ports_set_write(1 * 12 + i, max11311_write, 1, i);
    	ports_set_write(2 * 12 + i, max11311_write, 2, i);
    	ports_set_write(3 * 12 + i, max11311_write, 3, i);
    }

    for (int i = 0; i < 20; i++) {
    	ports_set_write(4 * 12 + 0 * 20 + i, max11300_write, 0, i);
    	ports_set_write(4 * 12 + 1 * 20 + i, max11300_write, 1, i);
    }

    for (int i = 0; i < 4; i++) {
    	ports_set_write(4 * 12 + 2 * 20 + 0 * 4 + i, max5134_write, 0, i);
    	ports_set_write(4 * 12 + 2 * 20 + 1 * 4 + i, max5134_write, 1, i);
    }
}

/*
 * @brief   Application entry point.
 */
int main(void) {
  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    //synth_init();
    codecsetup_init();
    shiftctrl_init();
    spi_sched_init();
    max11311_init();
    max11300_init();
    max5134_init();
    portssetup_init();
    control_init();

    spi_sched_start();

    CLOCK_EnableClock(kCLOCK_Gpio0);
    CLOCK_EnableClock(kCLOCK_Gpio1);

    synth_init();
    synth_run();

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++ ;
    }
    return 0 ;
}
