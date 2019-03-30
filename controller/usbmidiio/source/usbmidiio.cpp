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
 * @file    usbmidiio2.cpp
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "LPC54616.h"
/* TODO: insert other include files here. */
#include "control.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "paramlist.h"
/* TODO: insert other definitions and declarations here. */

void preset_init();

#define NOTEON(note, velocity) dsp_cmd(0xfc, 2, note | (velocity << 8))
#define NOTEOFF(note) dsp_cmd(0xfc, 1, note)

void dsp_reset()
{
	uint8_t data[4];
	data[0] = 0xff;
	data[1] = 0xff;
	data[2] = 0xff;
	data[3] = 0xff;

	control_write(data, 4);
	control_write(data, 4);
	control_write(data, 4);
	control_write(data, 4);
}

void dsp_cmd(int param, int subparam, uint16_t value)
{
	uint8_t data[4];

	data[0] = param;
	data[1] = subparam;
	data[2] = value >> 8;
	data[3] = value & 0xFF;

	control_write(data, 4);
}

BaseType_t keys_press(int keyindex, uint32_t timediff)
{
	NOTEON(keyindex, 0xff);
	return pdFALSE;
}

BaseType_t keys_release(int keyindex)
{
	NOTEOFF(keyindex);
	return pdFALSE;
}

uint32_t ReadT()
{
	return (BOARD_INITPINS_T0_GPIO->B[BOARD_INITPINS_T0_PORT][BOARD_INITPINS_T0_PIN] << 0)
			| (BOARD_INITPINS_T1_GPIO->B[BOARD_INITPINS_T1_PORT][BOARD_INITPINS_T1_PIN] << 1)
			| (BOARD_INITPINS_T2_GPIO->B[BOARD_INITPINS_T2_PORT][BOARD_INITPINS_T2_PIN] << 2)
			| (BOARD_INITPINS_T3_GPIO->B[BOARD_INITPINS_T3_PORT][BOARD_INITPINS_T3_PIN] << 3)
			| (BOARD_INITPINS_T4_GPIO->B[BOARD_INITPINS_T4_PORT][BOARD_INITPINS_T4_PIN] << 4)
			//| (BOARD_INITPINS_T5_GPIO->B[BOARD_INITPINS_T5_PORT][BOARD_INITPINS_T5_PIN] << 5)
			| (BOARD_INITPINS_T6_GPIO->B[BOARD_INITPINS_T6_PORT][BOARD_INITPINS_T6_PIN] << 6)
			| (BOARD_INITPINS_T7_GPIO->B[BOARD_INITPINS_T7_PORT][BOARD_INITPINS_T7_PIN] << 7);
}

void SetMkBr(uint16_t mask)
{
	// from LSB: BR0, MK0, BR1, MK1, ...

	BOARD_INITPINS_BR0_GPIO->W[BOARD_INITPINS_BR0_PORT][BOARD_INITPINS_BR0_PIN] = mask & (1 << 1);
	BOARD_INITPINS_MK0_GPIO->W[BOARD_INITPINS_MK0_PORT][BOARD_INITPINS_MK0_PIN] = mask & (1 << 0);
	BOARD_INITPINS_BR1_GPIO->W[BOARD_INITPINS_BR1_PORT][BOARD_INITPINS_BR1_PIN] = mask & (1 << 3);
	BOARD_INITPINS_MK1_GPIO->W[BOARD_INITPINS_MK1_PORT][BOARD_INITPINS_MK1_PIN] = mask & (1 << 2);
	//BOARD_INITPINS_BR2_GPIO->W[BOARD_INITPINS_BR2_PORT][BOARD_INITPINS_BR2_PIN] = mask & (1 << 5);
	//BOARD_INITPINS_MK2_GPIO->W[BOARD_INITPINS_MK2_PORT][BOARD_INITPINS_MK2_PIN] = mask & (1 << 4);
	BOARD_INITPINS_BR3_GPIO->W[BOARD_INITPINS_BR3_PORT][BOARD_INITPINS_BR3_PIN] = mask & (1 << 7);
	BOARD_INITPINS_MK3_GPIO->W[BOARD_INITPINS_MK3_PORT][BOARD_INITPINS_MK3_PIN] = mask & (1 << 6);
	BOARD_INITPINS_BR4_GPIO->W[BOARD_INITPINS_BR4_PORT][BOARD_INITPINS_BR4_PIN] = mask & (1 << 9);
	BOARD_INITPINS_MK4_GPIO->W[BOARD_INITPINS_MK4_PORT][BOARD_INITPINS_MK4_PIN] = mask & (1 << 8);
}

SemaphoreHandle_t xKeyTimerSemaphore = NULL;

uint32_t timer_value()
{
	static uint32_t value = 0;

	value++;
	return value;
}

extern "C" void SCT0_IRQHandler(void)
{
	BaseType_t higherPriorityTaskWoken;
	xSemaphoreGiveFromISR(xKeyTimerSemaphore, &higherPriorityTaskWoken);

	SCTIMER_ClearStatusFlags(SCTIMER_1_PERIPHERAL, 1);

	portEND_SWITCHING_ISR(higherPriorityTaskWoken);
}

#define KEYSCAN_NUMKEYSETS (5)
#define KEYSCAN_MAXINDEX (2*KEYSCAN_NUMKEYSETS)
uint32_t keyscanindex = 0;
uint32_t keyscanstate[(KEYSCAN_MAXINDEX+3)/4] = {0};
uint32_t keyscantimer[KEYSCAN_NUMKEYSETS*8] = {0};

void KeyScan()
{
	BaseType_t higherPriorityTaskWoken = pdFALSE;
	uint32_t timervalue = timer_value();
	uint32_t T = ReadT();

	int stateindex = keyscanindex / 4;
	uint32_t prevT = (keyscanstate[stateindex] >> ((keyscanindex & 3)*8)) & 0xFF;
	keyscanstate[stateindex] &= ~(0xFF << ((keyscanindex & 3)*8));
	keyscanstate[stateindex] |= T << ((keyscanindex & 3)*8);

	if ((keyscanindex & 1) == 0) {
	        // make
	        for (int i = 0; i < 8; i++) {
	                if ((T & (1 << i)) == 0
	                        && (prevT & (1 << i)) != 0) {
	                        // key is newly pressed
	                        int keyindex = (keyscanindex >> 1) * 8 + i;
		                    printf("%d make\n", keyindex);
	                        uint32_t timediff = timervalue - keyscantimer[keyindex];
	                        higherPriorityTaskWoken |= keys_press(keyindex, timediff);
	                        //notestack_push(keyindex + 36);
	                        //midikeypress = 1;
	                }
	                else if ((T & (1 << i)) != 0
	                                && (prevT & (1 << i)) == 0) {
	                        // released
	                        int keyindex = (keyscanindex >> 1) * 8 + i;
		                    printf("%d release\n", keyindex);
	                        higherPriorityTaskWoken |= keys_release(keyindex);
	                        //notestack_pop(keyindex + 36);
	                        //midikeypress = 1;
	                }
	        }
	}
	else {
	        // break
	        for (int i = 0; i < 8; i++) {
	                if ((T & (1 << i)) == 0
	                        && (prevT & (1 << i)) != 0) {
	                    printf("%d break\n", (keyscanindex >> 1) * 8 + i);
	                        // key is newly pressed
	                        keyscantimer[(keyscanindex >> 1) * 8 + i] = timervalue;
	                }
	        }
	}

	// prepare for next cycle
	keyscanindex++;
	if (keyscanindex >= KEYSCAN_MAXINDEX) {
	        keyscanindex = 0;
	}
	SetMkBr(~(1 << keyscanindex));

	/*if (midiin_timeout_counter < 10000) {
	        midiin_timeout_counter++;
	        if (midiin_timeout_counter >= 10000) {
	                higherPriorityTaskWoken |= midiin_timeout();
	                midiin_timeout_counter = 0;
	        }
	}*/

	//synth_tick();
	portEND_SWITCHING_ISR(higherPriorityTaskWoken);
}

void KeyboardTask(void* pvParameters)
{
	dsp_reset();
	preset_init();

	NVIC_SetPriority(SCTIMER_1_IRQN, 2);
    SCTIMER_EnableInterrupts(SCTIMER_1_PERIPHERAL, 1);

	while (1) {
        if( xSemaphoreTake( xKeyTimerSemaphore, ( TickType_t ) 10 ) == pdTRUE )
        {
        	KeyScan();
        }
	}
}

///////////////////////////////////////////////////////////////////////

#define FULLVALUE(param, subparam, value) dsp_cmd(param, subparam, value)
#define MODSOURCE(source, subparam, value) dsp_cmd(source, subparam | 0x80, value)
#define MODMATRIX(param, target, subparam, value) dsp_cmd(param, ((target << 1) + (subparam & 1)) | 0x40, value)

#define SWITCH_ON(sw) FULLVALUE(0xfe, 0xfd, 0x200 | sw);
#define SWITCH_OFF(sw) FULLVALUE(0xfe, 0xfd, 0x100 | sw);

void preset_init()
{
	// lfos
	for (int param = 0; param < 16; param++) {
		for (int i = 0; i < 5; i++) {
			MODSOURCE(param + 0x10, i, 0);
		}
	}
	// envelopes
	for (int param = 0; param < 16; param++) {
		for (int i = 0; i < 5; i++) {
			MODSOURCE(param + 0x30, i, 0);
		}
	}
	// controllers
	for (int param = 0; param < 11; param++) {
		MODSOURCE(param + 0x60, 0, 0x4000);
		MODSOURCE(param + 0x60, 1, 0x0100);
	}
	// operators
	for (int param = 0; param < 16; param++) {
		MODSOURCE(param + 0x80, 0, 0xff);
		MODSOURCE(param + 0x80, 1, 0xff);
		MODSOURCE(param + 0x80, 2, 0);
		MODSOURCE(param + 0x80, 3, 0);
	}

	// modmatrix
	for (int param = 0; param < 0x100; param++) {
		for (int target = 0; target < 11; target++) {
				MODMATRIX(param, target, 0, 0);
				MODMATRIX(param, target, 1, 0);
			}
	}

	// parameters
	for (int param = 0; param < 0xf0; param++) {
		FULLVALUE(param, 0, 0);
		FULLVALUE(param, 1, 0);
		FULLVALUE(param, 2, 0);
	}

	FULLVALUE(output_VCO1_PITCH, 1, 0x4000);
	FULLVALUE(output_VCO2_PITCH, 1, 0x4000);
	FULLVALUE(output_VCO3_PITCH, 1, 0x4000);
	FULLVALUE(output_VCO4_PITCH, 1, 0x4000);
	FULLVALUE(output_VCO5_PITCH, 1, 0x4000);
	FULLVALUE(output_VCO6_PITCH, 1, 0x4000);
	FULLVALUE(output_VCO7_PITCH, 1, 0x4000);

	FULLVALUE(output_VCO1_PW, 0, 0x7fff);
	FULLVALUE(output_VCO2_PW, 0, 0x7fff);
	FULLVALUE(output_VCO3_PW, 0, 0x7fff);
	FULLVALUE(output_VCO4_PW, 0, 0x7fff);
	FULLVALUE(output_VCO5_PW, 0, 0x7fff);
	FULLVALUE(output_VCO6_PW, 0, 0x7fff);
	FULLVALUE(output_VCO7_PW, 0, 0x7fff);

	FULLVALUE(output_VCF1_CV, 0, 0xffff);
	FULLVALUE(output_VCF1_RES, 0, 0);

	FULLVALUE(output_VCA_FINAL, 0, 0xffff);

	FULLVALUE(output_VCF2_L_CV, 0, 0x7fff);
	FULLVALUE(output_VCF2_M1_CV, 0, 0x7fff);
	FULLVALUE(output_VCF2_M2_CV, 0, 0x7fff);
	FULLVALUE(output_VCF2_H_CV, 0, 0x7fff);

	FULLVALUE(output_VCF1_LIN, 0, 0);
	FULLVALUE(output_VCF1_LEVEL, 0, 0xffff);
	FULLVALUE(output_VCF1_PAN, 0, 0x7fff);
	FULLVALUE(output_VCF2_PAN, 0, 0x7fff);
	FULLVALUE(output_CLEANF_PAN, 0, 0x7fff);

	FULLVALUE(output_MASTER_LEVEL, 0, 0xffff);
	FULLVALUE(output_ZPRIME_SPEED, 0, 0x2000);

	//FULLVALUE(output_VCO1_MIX1, 0, 0x7fff);
	//FULLVALUE(output_VCO2_MIX1, 0, 0x7fff);
	//FULLVALUE(output_VCO3_MIX1, 0, 0x7fff);
	FULLVALUE(output_VCO1_LEVEL, 0, 0xffff);
	FULLVALUE(output_VCO2_LEVEL, 0, 0xffff);
	FULLVALUE(output_VCO3_LEVEL, 0, 0xffff);

	FULLVALUE(output_VCO1_VCFMIX, 0, 0x8000);
	FULLVALUE(output_VCO2_VCFMIX, 0, 0x8000);
	FULLVALUE(output_VCO3_VCFMIX, 0, 0x8000);

	FULLVALUE(output_VCO1_PITCH, 0, 0x8000);
	FULLVALUE(output_VCO2_PITCH, 0, 0x8000);
	FULLVALUE(output_VCO3_PITCH, 0, 0x8000);
	FULLVALUE(output_VCO4_PITCH, 0, 0x8000);
	FULLVALUE(output_VCO5_PITCH, 0, 0x8000);
	FULLVALUE(output_VCO6_PITCH, 0, 0x8000);
	FULLVALUE(output_VCO7_PITCH, 0, 0x8000);

	FULLVALUE(output_MASTER_PITCH, 0, 0x8000);
	FULLVALUE(output_MASTER_PITCH2, 0, 0x8000);

	for (int i = 0; i < 64; i++) {
	    SWITCH_OFF(i);
	}
    SWITCH_ON(switch_SEL1SQR);
    SWITCH_ON(switch_SEL2SQR);
    SWITCH_ON(switch_SEL3SQR);

    MODSOURCE(modsource_ENV0, 1, 0x10);
    MODSOURCE(modsource_ENV0, 2, 0x2000);
    MODSOURCE(modsource_ENV0, 3, 0x2000);
    MODSOURCE(modsource_ENV0, 4, 0x5000);

    MODMATRIX(modsource_ENV0, 0, 0, 0x3fff);
    MODMATRIX(modsource_ENV0, 0, 1, output_VCF1_LIN);

//    MODSOURCE(modsource_LFO0, 1, 0x200);

//    MODMATRIX(modsource_LFO0, 0, 0, 0x7fff);
//    MODMATRIX(modsource_LFO0, 0, 1, output_VCF1_LIN);
}
///////////////////////////////////////////////////////////////////////

/*
 * @brief   Application entry point.
 */
int main(void) {
  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    printf("Board init done\n");

    CLOCK_EnableClock(kCLOCK_Gpio0);
    CLOCK_EnableClock(kCLOCK_Gpio1);
    CLOCK_EnableClock(kCLOCK_Gpio2);
    CLOCK_EnableClock(kCLOCK_Gpio3);

    control_init();

    xKeyTimerSemaphore = xSemaphoreCreateBinary();

    xTaskCreate(KeyboardTask, "main", 256, NULL, 4, NULL);

    printf("Everything running\n");

    vTaskStartScheduler();

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++ ;
    }
    return 0 ;
}
