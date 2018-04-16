/*
 * spi_sched.c
 *
 *  Created on: Feb 1, 2018
 *      Author: ld0d
 */

#include "peripherals.h"
#include "fsl_spi.h"

#include "spi_sched.h"

#define WEAK __attribute__ ((weak))

#define NUM_BUS 10
static SPI_Type* spi_bus[NUM_BUS] = {
		0,
		0,
		SPI_2_PERIPHERAL,
		SPI_3_PERIPHERAL,
		0,
		SPI_5_PERIPHERAL,
		SPI_6_PERIPHERAL,
		SPI_7_PERIPHERAL,
		0,
		0
};

static spi_master_handle_t spi_handle[NUM_BUS] = {0};

#define DATA_COUNT_MAX 4

typedef struct _write_entry {
	uint8_t data[DATA_COUNT_MAX];
	int count;
	uint32_t flags;
	read_callback_t callback;
	void* read_user_data;
} write_entry_t;

typedef struct _tx_entry {
	SPI_Type* bus;
	//uint8_t* data;
	//int busid;
	//uint8_t size;
	//uint8_t ssel;
} tx_entry_t;

#define QUEUE_DEPTH 64

typedef struct _bus_queue {
	write_entry_t queue[QUEUE_DEPTH];
	int queue_first;
	int queue_last;
	int busy;

	tx_entry_t tx;
} bus_queue_t;

static bus_queue_t spi_bus_queue[NUM_BUS];

__attribute__( ( section(".data") ) )
inline void spi_sched_transfer(bus_queue_t* queue, write_entry_t* entry)
{
	SPI_Type* bus = queue->tx.bus;
	uint8_t* data = &entry->data[0];
	//int size = entry->count;
	uint32_t flags = entry->flags;

    /* clear FIFOs when transfer starts */
	bus->FIFOCFG |= SPI_FIFOCFG_EMPTYTX_MASK | SPI_FIFOCFG_EMPTYRX_MASK;
	bus->FIFOSTAT |= SPI_FIFOSTAT_TXERR_MASK | SPI_FIFOSTAT_RXERR_MASK;
	bus->FIFOTRIG = (bus->FIFOTRIG & (~(SPI_FIFOTRIG_RXLVL_MASK | SPI_FIFOTRIG_RXLVL_MASK))) |
					 SPI_FIFOTRIG_RXLVL(2) | SPI_FIFOTRIG_TXLVL(0);

	/* enable generating rxIRQ, empty rxFIFO triggers end of transfer handler */
	bus->FIFOINTENSET |= SPI_FIFOINTENSET_RXLVL_MASK;

	// data is less than fifo size (8 bytes), just write it all
	//size--; // handle last byte separately
	//while (size--) {
	    queue->tx.bus->FIFOWR = *(data++) | flags;
	    queue->tx.bus->FIFOWR = *(data++) | flags;
	//}
    bus->FIFOWR = *(data++) | flags | SPI_FIFOWR_EOT_MASK;
}

__attribute__( ( section(".data") ) )
static void spi_sched_bus_process_noirq(bus_queue_t* queue)
{
	//spi_transfer_t xfer;

	if (queue->busy) {
		return;
	}
	if (queue->queue_first == queue->queue_last) {
	    return;
	}

	write_entry_t* entry = &queue->queue[queue->queue_first];

	/*spi_handle[spi_bus_id].sselNum = entry->ssel;

	xfer.txData = &entry->data[0];
	xfer.rxData = 0;
	xfer.configFlags = kSPI_FrameAssert;
	xfer.dataSize = entry->count;*/

	queue->busy = 1;

	//SPI_MasterTransferNonBlocking(spi_bus[spi_bus_id], &spi_handle[spi_bus_id], &xfer);
	spi_sched_transfer(queue, entry);
}

__attribute__( ( section(".data") ) )
static void spi_callback(SPI_Type *base, spi_master_handle_t *handle, status_t status, void *userData)
{
	// called from IRQ handler, interrupts are disabled

	int spi_bus_id = (int)userData;
	bus_queue_t* queue = &spi_bus_queue[spi_bus_id];
	if (queue->queue_first != queue->queue_last) {
		queue->queue_first = (queue->queue_first + 1) & (QUEUE_DEPTH-1);
	}
	queue->busy = 0;
	spi_sched_bus_process_noirq(queue);
}

__attribute__( ( section(".data") ) )
void spi_sched_write(int spi_bus_id, int spi_ssel, uint8_t* data, int count, read_callback_t read_callback, void* read_user_data)
{
	bus_queue_t* queue = &spi_bus_queue[spi_bus_id];

	__disable_irq();

	int next_last_index = (queue->queue_last + 1) & (QUEUE_DEPTH-1);
	if (next_last_index == queue->queue_first) {
		__enable_irq();

		while (1) {
			__disable_irq();
			if (next_last_index != queue->queue_first) {
				break;
			}
			__enable_irq();
			__NOP();
			__NOP();
			__NOP();
			__NOP();
			__NOP();
		}

		//return;
	}

	write_entry_t* entry = &queue->queue[queue->queue_last];
	queue->queue_last = next_last_index;

	//if (count >= DATA_COUNT_MAX) {
	//	count = DATA_COUNT_MAX;
	//}

	*(uint32_t*)entry->data = *(uint32_t*)data;
	//for (int i = 0; i < count; i++)
	//	entry->data[i] = data[i];

	//entry->count = count;
	entry->flags = SPI_ASSERTNUM_SSEL(spi_ssel) | SPI_FIFOWR_LEN(7);
	entry->callback = read_callback;
	entry->read_user_data = read_user_data;

	spi_sched_bus_process_noirq(queue);

	__enable_irq();
}

void spi_sched_init()
{
	for (int i = 0; i < NUM_BUS; i++) {
		spi_bus_queue[i].tx.bus = spi_bus[i];
		//spi_bus_queue[i].tx.busid = i;
		spi_bus_queue[i].queue_first = 0;
		spi_bus_queue[i].queue_last = 0;
		spi_bus_queue[i].busy = 0;

		if (spi_bus[i]) {
			SPI_MasterTransferCreateHandle(
					spi_bus[i],
					&spi_handle[i],
					spi_callback,
					(void*)i);
		}
	}

    NVIC_SetPriority(SPI_2_FLEXCOMM_IRQN, 2);
    NVIC_SetPriority(SPI_3_FLEXCOMM_IRQN, 2);
    NVIC_SetPriority(SPI_5_FLEXCOMM_IRQN, 2);
    NVIC_SetPriority(SPI_6_FLEXCOMM_IRQN, 2);
    NVIC_SetPriority(SPI_7_FLEXCOMM_IRQN, 2);
}

__attribute__( ( section(".data") ) )
void spi_sched_irq(SPI_Type *base, bus_queue_t *queue)
{
	// end of xmit

	write_entry_t* entry = &queue->queue[queue->queue_first];

	//while (((queue->tx.bus->FIFOSTAT >> 16) & 0x1f) < 3);

	uint32_t data = 0;
	data |= queue->tx.bus->FIFORD & 0xFF;
	data <<= 8;
	data |= queue->tx.bus->FIFORD & 0xFF;
	data <<= 8;
	data |= queue->tx.bus->FIFORD & 0xFF;

	base->FIFOINTENCLR = SPI_FIFOINTENCLR_RXLVL_MASK | SPI_FIFOINTENCLR_TXLVL_MASK;

	read_callback_t callback = entry->callback;
	void* user = entry->read_user_data;

	//if (queue->queue_first != queue->queue_last) {
		queue->queue_first = (queue->queue_first + 1) & (QUEUE_DEPTH-1);
	//}
	queue->busy = 0;
	spi_sched_bus_process_noirq(queue);

	callback(data, user);
}

void spi_sched_start()
{
	/*for (int i = 0; i < NUM_BUS; i++) {
		if (spi_bus[i]) {
		    FLEXCOMM_SetIRQHandler(spi_bus[i], (flexcomm_irq_handler_t)(uintptr_t)spi_sched_irq, &spi_bus_queue[i]);
		}
	}*/
}

__attribute__( ( section(".data") ) )
WEAK void FLEXCOMM2_IRQHandler(void)
{
	spi_sched_irq(spi_bus[2], &spi_bus_queue[2]);
}

__attribute__( ( section(".data") ) )
WEAK void FLEXCOMM3_IRQHandler(void)
{
	spi_sched_irq(spi_bus[3], &spi_bus_queue[3]);
}

__attribute__( ( section(".data") ) )
WEAK void FLEXCOMM5_IRQHandler(void)
{
	spi_sched_irq(spi_bus[5], &spi_bus_queue[5]);
}

__attribute__( ( section(".data") ) )
WEAK void FLEXCOMM6_IRQHandler(void)
{
	spi_sched_irq(spi_bus[6], &spi_bus_queue[6]);
}

__attribute__( ( section(".data") ) )
WEAK void FLEXCOMM7_IRQHandler(void)
{
	spi_sched_irq(spi_bus[7], &spi_bus_queue[7]);
}

