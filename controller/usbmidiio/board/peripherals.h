/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PERIPHERALS_H_
#define _PERIPHERALS_H_

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
#include "fsl_common.h"
#include "fsl_reset.h"
#include "fsl_usart.h"
#include "fsl_clock.h"
#include "fsl_sctimer.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/
/* Definitions for BOARD_InitPeripherals functional group */
/* Definition of peripheral ID */
#define USART_MIDI_PERIPHERAL ((USART_Type *)FLEXCOMM1)
/* Definition of the clock source frequency */
#define USART_MIDI_CLOCK_SOURCE 48000000UL
/* USART_MIDI interrupt vector ID (number). */
#define USART_MIDI_FLEXCOMM_IRQN FLEXCOMM1_IRQn
/* USART_MIDI interrupt handler identifier. */
#define USART_MIDI_FLEXCOMM_IRQHANDLER FLEXCOMM1_IRQHandler
/* Definition of peripheral ID */
#define USART_DSP_PERIPHERAL ((USART_Type *)FLEXCOMM7)
/* Definition of the clock source frequency */
#define USART_DSP_CLOCK_SOURCE 48000000UL
/* USART_DSP interrupt vector ID (number). */
#define USART_DSP_FLEXCOMM_IRQN FLEXCOMM7_IRQn
/* USART_DSP interrupt handler identifier. */
#define USART_DSP_FLEXCOMM_IRQHANDLER FLEXCOMM7_IRQHandler
/* BOARD_InitPeripherals defines for SCT0 */
/* Definition of peripheral ID */
#define SCTIMER_1_PERIPHERAL SCT0
/* Definition of clock source frequency */
#define SCTIMER_1_CLOCK_FREQ 180000000UL
/* SCTimer_1 interrupt vector ID (number). */
#define SCTIMER_1_IRQN SCT0_IRQn
/* SCTimer_1 interrupt handler identifier. */
#define SCTIMER_1_IRQHANDLER SCT0_IRQHandler
/* SCTimer output 0 mask */
#define SCTIMER_OUTPUT_0_MASK (1U << kSCTIMER_Out_0)
/* SCTimer output 1 mask */
#define SCTIMER_OUTPUT_1_MASK (1U << kSCTIMER_Out_1)
/* SCTimer output 2 mask */
#define SCTIMER_OUTPUT_2_MASK (1U << kSCTIMER_Out_2)
/* SCTimer output 3 mask */
#define SCTIMER_OUTPUT_3_MASK (1U << kSCTIMER_Out_3)
/* SCTimer output 4 mask */
#define SCTIMER_OUTPUT_4_MASK (1U << kSCTIMER_Out_4)
/* SCTimer output 5 mask */
#define SCTIMER_OUTPUT_5_MASK (1U << kSCTIMER_Out_5)
/* SCTimer output 6 mask */
#define SCTIMER_OUTPUT_6_MASK (1U << kSCTIMER_Out_6)
/* SCTimer output 7 mask */
#define SCTIMER_OUTPUT_7_MASK (1U << kSCTIMER_Out_7)
/* Definition of peripheral ID */
#define USART_RPI_TX_PERIPHERAL ((USART_Type *)FLEXCOMM3)
/* Definition of the clock source frequency */
#define USART_RPI_TX_CLOCK_SOURCE 48000000UL
/* USART_RPI_TX interrupt vector ID (number). */
#define USART_RPI_TX_FLEXCOMM_IRQN FLEXCOMM3_IRQn
/* USART_RPI_TX interrupt handler identifier. */
#define USART_RPI_TX_FLEXCOMM_IRQHANDLER FLEXCOMM3_IRQHandler
/* Definition of peripheral ID */
#define USART_RPI_RX_PERIPHERAL ((USART_Type *)FLEXCOMM0)
/* Definition of the clock source frequency */
#define USART_RPI_RX_CLOCK_SOURCE 48000000UL
/* USART_RPI_RX interrupt vector ID (number). */
#define USART_RPI_RX_FLEXCOMM_IRQN FLEXCOMM0_IRQn
/* USART_RPI_RX interrupt handler identifier. */
#define USART_RPI_RX_FLEXCOMM_IRQHANDLER FLEXCOMM0_IRQHandler
/* Definition of peripheral ID */
#define USART_RPI_OLD_PERIPHERAL ((USART_Type *)FLEXCOMM2)
/* Definition of the clock source frequency */
#define USART_RPI_OLD_CLOCK_SOURCE 48000000UL
/* USART_RPI_OLD interrupt vector ID (number). */
#define USART_RPI_OLD_FLEXCOMM_IRQN FLEXCOMM2_IRQn
/* USART_RPI_OLD interrupt handler identifier. */
#define USART_RPI_OLD_FLEXCOMM_IRQHANDLER FLEXCOMM2_IRQHandler

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
extern const usart_config_t USART_MIDI_config;
extern const usart_config_t USART_DSP_config;
extern const sctimer_config_t SCTimer_1_initConfig;
extern uint32_t SCTimer_1_event[1];
extern const usart_config_t USART_RPI_TX_config;
extern const usart_config_t USART_RPI_RX_config;
extern const usart_config_t USART_RPI_OLD_config;

/***********************************************************************************************************************
 * Initialization functions
 **********************************************************************************************************************/
void BOARD_InitPeripherals(void);

/***********************************************************************************************************************
 * BOARD_InitBootPeripherals function
 **********************************************************************************************************************/
void BOARD_InitBootPeripherals(void);

#if defined(__cplusplus)
}
#endif

#endif /* _PERIPHERALS_H_ */
