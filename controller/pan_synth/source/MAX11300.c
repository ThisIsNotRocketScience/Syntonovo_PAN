/*
 * MAX11300.c
 *
 *  Created on: Jan 29, 2018
 *      Author: ld0d
 */

#include "peripherals.h"
#include "fsl_spi.h"

#include "MAX11300.h"
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


static void max11300_read_reg(SPI_Type *spi_base, int ssel, int address, uint16_t* value)
{
	spi_config_t* config = SPI_GetConfig(spi_base);
	config->sselNum = ssel;

	unsigned char out[3];
	unsigned char in[3];
	out[0] = (address << 1) | 1;
	out[1] = 0;
	out[2] = 0;

	spi_transfer_t xfer;
	xfer.txData = out;
	xfer.rxData = in;
	xfer.configFlags = kSPI_FrameAssert;
	xfer.dataSize = 3;
	(void) SPI_MasterTransferBlocking(spi_base, &xfer);

	*value = (in[1] << 8) | in[2];
}

static void max11300_write_reg(SPI_Type *spi_base, int ssel, int address, uint16_t value)
{
	spi_config_t* config = SPI_GetConfig(spi_base);
	config->sselNum = ssel;

	unsigned char out[3];
	out[0] = (address << 1) | 0;
	out[1] = (value & 0xFF00) >> 8;
	out[2] = value & 0xFF;

	spi_transfer_t xfer;
	xfer.txData = out;
	xfer.rxData = 0;
	xfer.configFlags = kSPI_FrameAssert;
	xfer.dataSize = 3;
	(void) SPI_MasterTransferBlocking(spi_base, &xfer);
}

#define NUM_IC 2
#define NUM_PORTS_PER_IC 20

static SPI_Type* spi_bus[NUM_IC] = {
		SPI_5_PERIPHERAL,
		SPI_5_PERIPHERAL,
};

static int spi_bus_id[NUM_IC] = {
		5,
		5
};

static int spi_ssel[NUM_IC] = {
		0,
		1,
};

static volatile int max11300_ready = 1;
static uint16_t portconfig[NUM_IC * NUM_PORTS_PER_IC] = { 0 };

void max11300_set_port(int ic, int ch, int mode)
{
	portconfig[ic * NUM_PORTS_PER_IC + ch] = mode;
}

#define BRST (14)
#define THSDHN (7)
#define ADCCONV (4)
#define DACREF (6)
#define DACCTL (2)
#define ADCCTL (0)

static const uint8_t portconfig_reg[20] = { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33 };
static const uint8_t dacdat_reg[20] = { 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73 };
static const uint8_t adcdat_reg[20] = { 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53 };

void max11300_init()
{
	for (int k = 0; k < NUM_IC; k++) {
		// reset
		//max11300_write_reg(0x10, 0x8000);
		//WAIT1_Waitms(10);
		volatile uint16_t value;
		max11300_read_reg(spi_bus[k], spi_ssel[k], 0x00, (uint16_t*)&value);
		max11300_read_reg(spi_bus[k], spi_ssel[k], 0x10, (uint16_t*)&value);
		max11300_write_reg(spi_bus[k], spi_ssel[k], 0x10, (0 << BRST) | (0 << THSDHN) | (0 << ADCCONV) | (1 << DACREF) | (1 << DACCTL));
		WAIT1_Waitus(200);
		max11300_read_reg(spi_bus[k], spi_ssel[k], 0x10, (uint16_t*)&value);

		for (int i = 0; i < 20; i++) {
			if ((portconfig[k * NUM_PORTS_PER_IC + i] & 0xF000) == 0x5000) {
				max11300_write_reg(spi_bus[k], spi_ssel[k], dacdat_reg[i], 0);
				max11300_write_reg(spi_bus[k], spi_ssel[k], portconfig_reg[i], portconfig[k * NUM_PORTS_PER_IC + i]);
				WAIT1_Waitms(1);
			}
		}

		for (int i = 0; i < 20; i++) {
			if ((portconfig[k * NUM_PORTS_PER_IC + i] & 0xF000) == 0x7000) {
				max11300_write_reg(spi_bus[k], spi_ssel[k], portconfig_reg[i], portconfig[k * NUM_PORTS_PER_IC + i]);
				WAIT1_Waitus(100);
			}
		}

		max11300_write_reg(spi_bus[k], spi_ssel[k], 0x10, (0 << BRST) | (0 << THSDHN) | (0 << ADCCONV) | (1 << DACREF) | (1 << DACCTL) | (3 << ADCCTL));
		WAIT1_Waitus(200);
	}
}

uint16_t max11300_read(int ic, int ch)
{
	uint16_t value;
	max11300_read_reg(spi_bus[ic], spi_ssel[ic], adcdat_reg[ch], &value);
	return value;
}

static void dummy_callback(uint32_t data, void* user)
{

}

void max11300_write(int ic, int ch, uint16_t value)
{
	value >>= 4;

	unsigned char out[3];
	out[0] = (dacdat_reg[ch] << 1) | 0;
	out[1] = (value & 0xFF00) >> 8;
	out[2] = value & 0xFF;

	spi_sched_write(spi_bus_id[ic], spi_ssel[ic], out, 3, dummy_callback, 0);

	//max11300_write_reg(spi_bus[ic], spi_ssel[ic], dacdat_reg[ch], value);
}

void max11300_write_done()
{
	max11300_ready = 1;
}
