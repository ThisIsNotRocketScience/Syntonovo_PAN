/*
 * MAX5134.c
 *
 *  Created on: Feb 1, 2018
 *      Author: ld0d
 */

#include "peripherals.h"
#include "fsl_spi.h"

#include "MAX5134.h"
#include "spi_sched.h"

void *SPI_GetConfig(SPI_Type *base);

static void WAIT1_Waitus(uint32_t us)
{
	for (uint32_t k = (us * (SystemCoreClock / 1000)) / 1000; k > 0; k--) {
		__NOP();
	}
}

static void WAIT1_Waitms(uint32_t ms)
{
	for (int k = 0; k < ms; k++) {
		WAIT1_Waitus(1000);
	}
}

static void max5134_write_reg(SPI_Type *spi_base, uint8_t control, uint16_t data)
{
	unsigned char out[3];
	out[0] = control;
	out[1] = (data & 0xFF00) >> 8;
	out[2] = data & 0xFF;

	spi_transfer_t xfer;
	xfer.txData = out;
	xfer.rxData = 0;
	xfer.configFlags = kSPI_FrameAssert;
	xfer.dataSize = 3;
	(void) SPI_MasterTransferBlocking(spi_base, &xfer);
}

#define NUM_IC 2

static SPI_Type* spi_bus[NUM_IC] = {
		SPI_6_PERIPHERAL,
		SPI_7_PERIPHERAL,
};

static int spi_bus_id[NUM_IC] = {
		6,
		7
};

void max5134_init()
{
	for (int i = 0; i < NUM_IC; i++) {
		max5134_write_reg(spi_bus[i], 0x02, 0);
		max5134_write_reg(spi_bus[i], 0x03, 0);
		max5134_write_reg(spi_bus[i], 0x05, 0x200);
		WAIT1_Waitms(10);
		max5134_write_reg(spi_bus[i], 0x05, 0);
	}
}

static void dummy_callback(uint32_t data, void* user)
{

}

void max5134_write(int ic, int ch, uint16_t value)
{
	unsigned char out[3];
	out[0] = 0x30 | (1 << ch);
	out[1] = (value & 0xFF00) >> 8;
	out[2] = value & 0xFF;
	spi_sched_write(spi_bus_id[ic], 0, out, 3, dummy_callback, 0);

	//max5134_write_reg(spi_bus[ic], 0x30 | (1 << ch), value);
}
