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
 * @file    uart-test-1.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "LPC54616.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

void FLEXCOMM2_IRQHandler(void)
{
    if (USART_RPI_PERIPHERAL->FIFOSTAT & USART_FIFOSTAT_RXERR_MASK)
    {
        /* Clear rx error state. */
    	USART_RPI_PERIPHERAL->FIFOSTAT |= USART_FIFOSTAT_RXERR_MASK;
        /* clear rxFIFO */
    	USART_RPI_PERIPHERAL->FIFOCFG |= USART_FIFOCFG_EMPTYRX_MASK;
    	/* Clear tx interrupt */
    	//USART_1_PERIPHERAL->FIFOINTENCLR |= USART_FIFOINTENSET_TXLVL_MASK;
    }

    int repeat = 1;
    while (repeat)
    {
    	repeat = 0;

        if (USART_RPI_PERIPHERAL->FIFOSTAT & USART_FIFOSTAT_RXNOTEMPTY_MASK)
        {
            uint8_t b = USART_RPI_PERIPHERAL->FIFORD & 0xFF;
            USART_RPI_PERIPHERAL->FIFOWR = b;

            repeat = 1;
        }
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

    printf("Hello World\n");

	USART_RPI_PERIPHERAL->FIFOCFG &= ~(USART_FIFOCFG_EMPTYTX_MASK | USART_FIFOCFG_ENABLETX_MASK);
	USART_RPI_PERIPHERAL->FIFOCFG |= USART_FIFOCFG_EMPTYRX_MASK | USART_FIFOCFG_ENABLERX_MASK;
	USART_RPI_PERIPHERAL->FIFOCFG |= USART_FIFOCFG_EMPTYTX_MASK | USART_FIFOCFG_ENABLETX_MASK;
    /* setup trigger level */
    USART_RPI_PERIPHERAL->FIFOTRIG &= ~(USART_FIFOTRIG_RXLVL_MASK | USART_FIFOTRIG_TXLVL_MASK);
    USART_RPI_PERIPHERAL->FIFOTRIG |= USART_FIFOTRIG_RXLVL(0);

    /* enable trigger interrupt */
    USART_RPI_PERIPHERAL->FIFOTRIG |= USART_FIFOTRIG_RXLVLENA_MASK;

	USART_RPI_PERIPHERAL->FIFOINTENSET |= USART_FIFOINTENSET_RXLVL_MASK | USART_FIFOINTENSET_RXERR_MASK;

    NVIC_SetPriority(USART_RPI_FLEXCOMM_IRQN, 1);
	EnableIRQ(USART_RPI_FLEXCOMM_IRQN);

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++ ;

        /*if (i == 100000000) {
        	i = 0;
        	USART_RPI_PERIPHERAL->FIFOWR = 'a';
        }*/
    }
    return 0 ;
}
