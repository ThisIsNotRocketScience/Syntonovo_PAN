/*
 * control.c
 *
 *  Created on: 25 Mar 2018
 *      Author: koponenl
 */

#include <stdint.h>
#include "control.h"
#include "peripherals.h"
#include "fsl_flexcomm.h"

#define WEAK __attribute__ ((weak))

static uint8_t* out_ptr = 0;
static int out_count = 0;

void control_out_queue(uint8_t* data, int count)
{
	__disable_irq();
	out_ptr = data;
	out_count = count;
	__enable_irq();

	USART_DSP_PERIPHERAL->FIFOINTENSET = USART_FIFOINTENSET_TXLVL_MASK | USART_FIFOINTENSET_TXERR_MASK;
}

void control_out_queue_wait()
{
	while (out_count) {}
    while (!(USART_DSP_PERIPHERAL->STAT & USART_STAT_TXIDLE_MASK)) {}
}

#define RING_SIZE	16
static uint8_t fifo_rx_ring[RING_SIZE];
static int fifo_rx_write = 0;
static int fifo_rx_read = 0;
static int fifo_reset_state = 1;

static void fifo_rx_queue(uint8_t byte)
{
	fifo_rx_ring[fifo_rx_write] = byte;
	fifo_rx_write = (fifo_rx_write + 1) & (RING_SIZE-1);
}

static int fifo_rx_empty()
{
	return fifo_rx_write == fifo_rx_read;
}

static int fifo_rx_numbytes()
{
	return (int)((unsigned int)(fifo_rx_write - fifo_rx_read) & (RING_SIZE-1));
}

static uint8_t fifo_rx_peek()
{
	uint8_t b = fifo_rx_ring[fifo_rx_read];
	return b;
}

static uint8_t fifo_rx_pop()
{
	uint8_t b = fifo_rx_peek();
	fifo_rx_read = (fifo_rx_read + 1) & (RING_SIZE-1);
	return b;
}

static void fifo_reset_enable()
{
	fifo_reset_state = 1;

	synth_reset();
}

static void fifo_reset_disable()
{
	fifo_reset_state = 0;
}

void fifo_reset_process()
{
	if (!fifo_reset_state) {
		return;
	}

	while (!fifo_rx_empty()) {
		if (fifo_rx_peek() != 0xff) {
			fifo_reset_disable();
			return;
		}

		(void) fifo_rx_pop();
	}
}

void fifo_rx_process()
{
	fifo_reset_process();

	if (fifo_rx_numbytes() >= 4)
	{
		uint8_t data[4];
		data[0] = fifo_rx_pop();
		data[1] = fifo_rx_pop();
		data[2] = fifo_rx_pop();
		data[3] = fifo_rx_pop();

		if (data[0] == 0xff
			&& data[1] == 0xff
			&& data[2] == 0xff
			&& data[3] == 0xff)
		{
			fifo_reset_enable();
			return;
		}

		control_cb(data[0], data[1], ((uint16_t)data[2] << 8) | (uint16_t)data[3]);
	}
}

volatile int receiveEnabled = 0;

void control_set_receive(int enable)
{
	receiveEnabled = enable;
}

void control_init()
{
	receiveEnabled = 1;
    //FLEXCOMM_SetIRQHandler(USART_DSP_FLEXCOMM_IRQN, (flexcomm_irq_handler_t)(uintptr_t)USART_TransferHandleIRQ, handle);

	USART_DSP_PERIPHERAL->FIFOCFG |= USART_FIFOCFG_EMPTYRX_MASK | USART_FIFOCFG_ENABLERX_MASK | USART_FIFOCFG_EMPTYTX_MASK | USART_FIFOCFG_ENABLETX_MASK;
    /* setup trigger level */
    USART_DSP_PERIPHERAL->FIFOTRIG &= ~(USART_FIFOTRIG_RXLVL_MASK);
    USART_DSP_PERIPHERAL->FIFOTRIG |= USART_FIFOTRIG_RXLVL(0);
    USART_DSP_PERIPHERAL->FIFOTRIG &= ~(USART_FIFOTRIG_TXLVL_MASK);
    USART_DSP_PERIPHERAL->FIFOTRIG |= USART_FIFOTRIG_TXLVL(7);
    /* enable trigger interrupt */
    USART_DSP_PERIPHERAL->FIFOTRIG |= USART_FIFOTRIG_RXLVLENA_MASK;
    USART_DSP_PERIPHERAL->FIFOTRIG |= USART_FIFOTRIG_TXLVLENA_MASK;

	USART_DSP_PERIPHERAL->FIFOINTENCLR = USART_FIFOINTENSET_TXLVL_MASK | USART_FIFOINTENSET_TXERR_MASK;
	USART_DSP_PERIPHERAL->FIFOINTENSET = USART_FIFOINTENSET_RXLVL_MASK | USART_FIFOINTENSET_RXERR_MASK;

    NVIC_SetPriority(USART_DSP_FLEXCOMM_IRQN, 3);
	EnableIRQ(USART_DSP_FLEXCOMM_IRQN);
}

void USART_DSP_FLEXCOMM_IRQHANDLER(void)
{
    if (!receiveEnabled || (USART_DSP_PERIPHERAL->FIFOSTAT & USART_FIFOSTAT_RXERR_MASK))
    {
        /* Clear rx error state. */
    	USART_DSP_PERIPHERAL->FIFOSTAT = USART_FIFOSTAT_RXERR_MASK;
        /* clear rxFIFO */
    	USART_DSP_PERIPHERAL->FIFOCFG |= USART_FIFOCFG_EMPTYRX_MASK;
    	/* Clear tx interrupt */
    	//USART_DSP_PERIPHERAL->FIFOINTENCLR |= USART_FIFOINTENSET_TXLVL_MASK;
    }

    if (USART_DSP_PERIPHERAL->FIFOSTAT & USART_FIFOSTAT_TXERR_MASK)
    {
    	out_ptr = 0;
    	out_count = 0;
    	USART_DSP_PERIPHERAL->FIFOINTENCLR = USART_FIFOINTENSET_TXLVL_MASK | USART_FIFOINTENSET_TXERR_MASK;
    	USART_DSP_PERIPHERAL->FIFOSTAT = USART_FIFOSTAT_TXERR_MASK;
    	USART_DSP_PERIPHERAL->FIFOCFG |= USART_FIFOCFG_EMPTYTX_MASK;
    }

    //USART_DSP_PERIPHERAL->FIFOWR = 'q';

    int repeat = 1;
    while (repeat)
    {
    	repeat = 0;

        if (receiveEnabled && (USART_DSP_PERIPHERAL->FIFOSTAT & USART_FIFOSTAT_RXNOTEMPTY_MASK))
        {
        	repeat = 1;

            uint8_t b = USART_DSP_PERIPHERAL->FIFORD & 0xFF;
            fifo_rx_queue(b);

            fifo_rx_process();
        }

        if (out_count && USART_DSP_PERIPHERAL->FIFOSTAT & USART_FIFOSTAT_TXNOTFULL_MASK) {
			USART_DSP_PERIPHERAL->FIFOWR = *out_ptr;
			out_ptr++;
			out_count--;
        	if (!out_count) {
        		USART_DSP_PERIPHERAL->FIFOINTENCLR = USART_FIFOINTENSET_TXLVL_MASK | USART_FIFOINTENSET_TXERR_MASK;
        	}
        	else {
    			repeat = 1;
        	}
        }

        //if (sendEnabled && (base->FIFOSTAT & USART_FIFOSTAT_TXNOTFULL_MASK))
        //{
        //	repeat = 1;
        //
        //    base->FIFOWR = *handle->txData;
        //}
    }
}
