/*
 * control.c
 *
 *  Created on: 25 Mar 2018
 *      Author: koponenl
 */

#include <stdint.h>
#include "rpi.h"
#include "control.h"
#include "peripherals.h"
#include "fsl_flexcomm.h"

#define WEAK __attribute__ ((weak))

#define RING_SIZE	16
static uint8_t fifo_rx_ring[RING_SIZE];
static int fifo_rx_write = 0;
static int fifo_rx_read = 0;

static int fifo_rx_level = 1;

static uint8_t fifo_tx_ring[RING_SIZE];
static int fifo_tx_write = 0;
static int fifo_tx_read = 0;

static uart_t* rpi_uart;

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

static void fifo_tx_queue(uint8_t byte)
{
	fifo_tx_ring[fifo_tx_write] = byte;
	fifo_tx_write = (fifo_tx_write + 1) & (RING_SIZE-1);
    USART_RPI_PERIPHERAL->FIFOINTENSET |= USART_FIFOINTENSET_TXLVL_MASK;
}

static int fifo_tx_empty()
{
	return fifo_tx_write == fifo_tx_read;
}

static int fifo_tx_full()
{
	return ((fifo_tx_write + 1) & (RING_SIZE - 1)) == fifo_tx_read;
}

static int fifo_tx_numbytes()
{
	return (int)((unsigned int)(fifo_tx_write - fifo_tx_read) & (RING_SIZE-1));
}

static uint8_t fifo_tx_peek()
{
	uint8_t b = fifo_tx_ring[fifo_tx_read];
	return b;
}

static uint8_t fifo_tx_pop()
{
	uint8_t b = fifo_tx_peek();
	fifo_tx_read = (fifo_tx_read + 1) & (RING_SIZE-1);
	return b;
}

static void fifo_rx_process()
{
	if (fifo_rx_numbytes() >= fifo_rx_level) {
		rpi_uart->config.read_complete(0, rpi_uart->config.read_complete_data);
	}
}

static int receiveEnabled = 0;
static int sendEnabled = 0;

void FLEXCOMM2_IRQHandler(void)
{
    if (!receiveEnabled || (USART_RPI_PERIPHERAL->FIFOSTAT & USART_FIFOSTAT_RXERR_MASK))
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

        if (receiveEnabled && (USART_RPI_PERIPHERAL->FIFOSTAT & USART_FIFOSTAT_RXNOTEMPTY_MASK))
        {
        	repeat = 1;

            uint8_t b = USART_RPI_PERIPHERAL->FIFORD & 0xFF;
            fifo_rx_queue(b);

            fifo_rx_process();
        }

        if (sendEnabled)
		{
			if (fifo_tx_empty())
			{
				if (USART_RPI_PERIPHERAL->FIFOSTAT & USART_FIFOSTAT_TXEMPTY_MASK) {
				    USART_RPI_PERIPHERAL->FIFOINTENCLR = USART_FIFOINTENSET_TXLVL_MASK;
				}
			}
			else
			{
				if (USART_RPI_PERIPHERAL->FIFOSTAT & USART_FIFOSTAT_TXNOTFULL_MASK) {
					repeat = 1;

					volatile uint8_t b = fifo_tx_pop();
					USART_RPI_PERIPHERAL->FIFOWR = b;
				}
			}
        }
    }
}

static void rpi_waitfor(int level)
{
	fifo_rx_level = level;
}

static void rpi_read(uint8_t* data, int count)
{
	for (int i = 0; i < count; i++) {
		data[i] = fifo_rx_pop();
	}
}

static int rpi_write(uint8_t* data, int count)
{
	while (fifo_tx_numbytes() >= RING_SIZE - count) {}

	for (int i = 0; i < count; i++) {
		fifo_tx_queue(data[i]);
	}

	rpi_uart->config.write_complete(0, rpi_uart->config.write_complete_data);
	return count;
}

void rpi_init(uart_t* uart)
{
	rpi_uart = uart;
	uart->rx.waitfor = rpi_waitfor;
	uart->rx.peek = fifo_rx_peek;
	uart->rx.read = rpi_read;
	uart->tx.write = rpi_write;

	receiveEnabled = 1;
	sendEnabled = 1;
    //FLEXCOMM_SetIRQHandler(USART_1_FLEXCOMM_IRQN, (flexcomm_irq_handler_t)(uintptr_t)USART_TransferHandleIRQ, handle);

	USART_RPI_PERIPHERAL->FIFOCFG &= ~(USART_FIFOCFG_EMPTYTX_MASK | USART_FIFOCFG_ENABLETX_MASK);
	USART_RPI_PERIPHERAL->FIFOCFG |= USART_FIFOCFG_EMPTYRX_MASK | USART_FIFOCFG_ENABLERX_MASK | USART_FIFOCFG_ENABLETX_MASK;
    /* setup trigger level */
    USART_RPI_PERIPHERAL->FIFOTRIG &= ~(USART_FIFOTRIG_RXLVL_MASK | USART_FIFOTRIG_TXLVL_MASK);
    USART_RPI_PERIPHERAL->FIFOTRIG |= USART_FIFOTRIG_RXLVL(0);
    USART_RPI_PERIPHERAL->FIFOTRIG |= USART_FIFOTRIG_TXLVL(0);

    /* enable trigger interrupt */
    USART_RPI_PERIPHERAL->FIFOTRIG |= USART_FIFOTRIG_RXLVLENA_MASK | USART_FIFOTRIG_TXLVLENA_MASK;

	USART_RPI_PERIPHERAL->FIFOINTENSET |= USART_FIFOINTENSET_RXLVL_MASK | USART_FIFOINTENSET_RXERR_MASK | USART_FIFOINTENSET_TXLVL_MASK;

    NVIC_SetPriority(USART_RPI_FLEXCOMM_IRQN, 3);
}

void rpi_start()
{
	EnableIRQ(USART_RPI_FLEXCOMM_IRQN);
}
