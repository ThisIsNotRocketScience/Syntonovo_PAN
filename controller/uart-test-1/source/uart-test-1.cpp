/*
 * Copyright 2016-2018 NXP Semiconductor, Inc.
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
 * @file    uart-test-1.cpp
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "LPC54616.h"
/* TODO: insert other include files here. */
#include "rpi.h"

/* TODO: insert other definitions and declarations here. */

uart_t rpi_uart;
sync_state_t rpi_sync;

uint8_t block[150000];

void sync_data_func(int addr, uint8_t* data)
{
	//if ((addr & 0xfff) == 0) { printf("data(%x)\n", addr); }
}

int sync_oobdata_func(uint8_t cmd, uint32_t data)
{
	//printf("oobdata(%d, %x)\n", cmd, data);
	return 0;
}

void sync_complete(int status)
{
	printf("sync_complete(%d)\n", status);
	if (status != 0) {
	    //sync_block(&rpi_sync, block, 0, 150000, sync_complete);
		return;
	}
	block[0]++;
}


/*
 * @brief   Application entry point.
 */
int main(void) {
  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    printf("Hello World\n");

    rpi_init(&rpi_uart);
    sync_init(&rpi_sync, &rpi_uart, sync_data_func, sync_oobdata_func);
    rpi_start();

    sync_block(&rpi_sync, block, 0, 8, sync_complete);
    //int sync_oob_word(sync_state_t* state, uint8_t cmd, uint32_t data, oob_transfer_complete_cb complete_cb);

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++ ;

        if (i == 0x2000000) {
        	//printf("sync_block\n");
            sync_block(&rpi_sync, block, 0, 150000, sync_complete);
            i = 0;
        }

        if ((i & 0x1FFFFF) == 0) {
        	rpi_uart.config.timer_tick(rpi_uart.config.timer_tick_data);
        }

        if ((i & 0xFFFFF) == 0) {
        	//printf("sync_oob_word\n");
            sync_oob_word(&rpi_sync, 0x42, 0x11223344, 0);
        }
    }
    return 0 ;
}
