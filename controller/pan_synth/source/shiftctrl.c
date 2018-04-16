/*
 * shiftctrl.c
 *
 *  Created on: Mar 17, 2018
 *      Author: ld0d
 */

#include "peripherals.h"
#include "fsl_spi.h"

#include "shiftctrl.h"

static uint32_t shiftctrl_flags;

void shiftctrl_init()
{
	shiftctrl_flags = 0;
	shiftctlr_update();
}

void shiftctrl_set(int flag)
{
	shiftctrl_flags |= (1 << flag);
	shiftctlr_update();
}

void shiftctrl_clear(int flag)
{
	shiftctrl_flags &= ~(1 << flag);
	shiftctlr_update();
}

void shiftctlr_update()
{
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
	xfer.txData = &shiftctrl_flags;
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

