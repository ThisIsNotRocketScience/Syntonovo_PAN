/*
 * shiftctrl.c
 *
 *  Created on: Mar 17, 2018
 *      Author: ld0d
 */

#include "peripherals.h"
#include "fsl_spi.h"

#include "shiftctrl.h"

static volatile int shiftctrl_update_needed = 1;
static volatile uint32_t shiftctrl_flags = 0;
static volatile uint32_t shiftctrl_extra_flags = 0;

void shiftctrl_init()
{
	shiftctrl_update_needed = 1;
	shiftctrl_flags = 0;
	shiftctrl_extra_flags = 0;
	shiftctrl_update();
}

void shiftctrl_set(int flag)
{
	if (flag < 32) {
		shiftctrl_flags |= (1 << flag);
		shiftctrl_update_needed = 1;
	}
	else {
		shiftctrl_extra_flags |= (1 << (flag - 32));
	}
	//shiftctrl_update();
}

void shiftctrl_clear(int flag)
{
	if (flag < 32) {
		shiftctrl_flags &= ~(1 << flag);
		shiftctrl_update_needed = 1;
	}
	else {
		shiftctrl_extra_flags &= ~(1 << (flag - 32));
	}
	//shiftctrl_update();
}

int shiftctrl_flag_state(int flag)
{
	if (flag < 32) {
		return (shiftctrl_flags >> flag) & 1;
	}
	else {
		return (shiftctrl_extra_flags >> (flag - 32)) & 1;
	}
}

void shiftctrl_update()
{
	if (shiftctrl_update_needed) {
		shiftctrl_update_needed = 0;
		uint32_t flags = shiftctrl_flags;

		GPIO->DIR[1] |= 1U << 31;
		GPIO->CLR[1] |= 1U << 31;

		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();

		spi_transfer_t xfer;
		xfer.txData = &flags;
		xfer.rxData = 0;
		xfer.configFlags = kSPI_FrameAssert;
		xfer.dataSize = 4;
		volatile status_t s = SPI_MasterTransferBlocking(SPI_8_PERIPHERAL, &xfer);

		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();

		GPIO->SET[1] |= 1U << 31;

		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
	}
}

