/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Peripherals v4.1
processor: LPC54616J512
package_id: LPC54616J512BD100
mcu_data: ksdk2_0
processor_version: 4.0.0
functionalGroups:
- name: BOARD_InitPeripherals
  called_from_default_init: true
  selectedCore: core0
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
component:
- type: 'system'
- type_id: 'system_54b53072540eeeb8f8e9343e71f28176'
- global_system_definitions: []
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
#include "peripherals.h"

/***********************************************************************************************************************
 * BOARD_InitPeripherals functional group
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * USART_MIDI initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'USART_MIDI'
- type: 'flexcomm_usart'
- mode: 'interrupts'
- type_id: 'flexcomm_usart_fcc110cc6b16332e9dfd9e0df675e21f'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'FLEXCOMM1'
- config_sets:
  - interruptsCfg:
    - interrupts: 'kUSART_RxLevelInterruptEnable'
    - interrupt_vectors:
      - enable_rx_tx_irq: 'true'
      - interrupt_rx_tx:
        - IRQn: 'FLEXCOMM1_IRQn'
        - enable_priority: 'false'
        - enable_custom_name: 'false'
  - usartConfig_t:
    - usartConfig:
      - clockSource: 'FXCOMFunctionClock'
      - clockSourceFreq: 'BOARD_BootClockRUN'
      - baudRate_Bps: '31250'
      - parityMode: 'kUSART_ParityDisabled'
      - stopBitCount: 'kUSART_OneStopBit'
      - bitCountPerChar: 'kUSART_8BitsPerChar'
      - loopback: 'false'
      - txWatermark: 'kUSART_TxFifo0'
      - rxWatermark: 'kUSART_RxFifo1'
      - enableRx: 'true'
      - enableTx: 'false'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const usart_config_t USART_MIDI_config = {
  .baudRate_Bps = 31250,
  .parityMode = kUSART_ParityDisabled,
  .stopBitCount = kUSART_OneStopBit,
  .bitCountPerChar = kUSART_8BitsPerChar,
  .loopback = false,
  .txWatermark = kUSART_TxFifo0,
  .rxWatermark = kUSART_RxFifo1,
  .enableRx = true,
  .enableTx = false
};

void USART_MIDI_init(void) {
  /* Reset FLEXCOMM device */
  RESET_PeripheralReset(kFC1_RST_SHIFT_RSTn);
  USART_Init(USART_MIDI_PERIPHERAL, &USART_MIDI_config, USART_MIDI_CLOCK_SOURCE);
  USART_EnableInterrupts(USART_MIDI_PERIPHERAL, kUSART_RxLevelInterruptEnable);
  /* Enable interrupt FLEXCOMM1_IRQn request in the NVIC */
  EnableIRQ(USART_MIDI_FLEXCOMM_IRQN);
}

/***********************************************************************************************************************
 * USART_DSP initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'USART_DSP'
- type: 'flexcomm_usart'
- mode: 'interrupts'
- type_id: 'flexcomm_usart_fcc110cc6b16332e9dfd9e0df675e21f'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'FLEXCOMM7'
- config_sets:
  - interruptsCfg:
    - interrupts: 'kUSART_RxLevelInterruptEnable'
    - interrupt_vectors:
      - enable_rx_tx_irq: 'true'
      - interrupt_rx_tx:
        - IRQn: 'FLEXCOMM7_IRQn'
        - enable_priority: 'false'
        - enable_custom_name: 'false'
  - usartConfig_t:
    - usartConfig:
      - clockSource: 'FXCOMFunctionClock'
      - clockSourceFreq: 'BOARD_BootClockRUN'
      - baudRate_Bps: '1400000'
      - parityMode: 'kUSART_ParityDisabled'
      - stopBitCount: 'kUSART_OneStopBit'
      - bitCountPerChar: 'kUSART_8BitsPerChar'
      - loopback: 'false'
      - txWatermark: 'kUSART_TxFifo0'
      - rxWatermark: 'kUSART_RxFifo1'
      - enableRx: 'true'
      - enableTx: 'true'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const usart_config_t USART_DSP_config = {
  .baudRate_Bps = 1400000,
  .parityMode = kUSART_ParityDisabled,
  .stopBitCount = kUSART_OneStopBit,
  .bitCountPerChar = kUSART_8BitsPerChar,
  .loopback = false,
  .txWatermark = kUSART_TxFifo0,
  .rxWatermark = kUSART_RxFifo1,
  .enableRx = true,
  .enableTx = true
};

void USART_DSP_init(void) {
  /* Reset FLEXCOMM device */
  RESET_PeripheralReset(kFC7_RST_SHIFT_RSTn);
  USART_Init(USART_DSP_PERIPHERAL, &USART_DSP_config, USART_DSP_CLOCK_SOURCE);
  USART_EnableInterrupts(USART_DSP_PERIPHERAL, kUSART_RxLevelInterruptEnable);
  /* Enable interrupt FLEXCOMM7_IRQn request in the NVIC */
  EnableIRQ(USART_DSP_FLEXCOMM_IRQN);
}

/***********************************************************************************************************************
 * SCTimer_1 initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'SCTimer_1'
- type: 'sctimer'
- mode: 'basic'
- type_id: 'sctimer_11116c58643bd290f0e74db0acbc9466'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'SCT0'
- config_sets:
  - main:
    - config:
      - clockMode: 'kSCTIMER_System_ClockMode'
      - clockSource: 'SynchronousFunctionClock'
      - clockSourceFreq: 'BOARD_BootClockRUN'
      - SCTInputClockSourceFreq: 'custom'
      - clockSelect: 'kSCTIMER_Clock_On_Rise_Input_0'
      - enableCounterUnify: 'true'
      - enableBidirection_l: 'false'
      - enableBidirection_h: 'false'
      - prescale_l: '1'
      - prescale_h: '1'
      - outInitState: 'SCTIMER_OUTPUT_0_MASK'
    - enableIRQ: 'true'
    - interrupt:
      - IRQn: 'SCT0_IRQn'
      - enable_priority: 'false'
      - enable_custom_name: 'false'
    - enableLTimer: 'true'
    - enableHTimer: 'false'
    - pwms: []
    - pwmMode: 'kSCTIMER_EdgeAlignedPwm'
    - pwmFrequency: '0'
    - events:
      - 0:
        - type: 'kSCTIMER_MatchEventOnly'
        - matchValue: '65535'
        - counter: 'kSCTIMER_Counter_L'
        - actions:
          - 0:
            - action: 'CounterLimit'
            - counter: 'kSCTIMER_Counter_L'
          - 1:
            - action: 'NextState'
            - nextState: '0'
    - states:
      - 0:
        - events: 'event0'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const sctimer_config_t SCTimer_1_initConfig = {
  .enableCounterUnify = true,
  .clockMode = kSCTIMER_System_ClockMode,
  .clockSelect = kSCTIMER_Clock_On_Rise_Input_0,
  .enableBidirection_l = false,
  .enableBidirection_h = false,
  .prescale_l = 0,
  .prescale_h = 0,
  .outInitState = SCTIMER_OUTPUT_0_MASK
};
uint32_t SCTimer_1_event[1];

void SCTimer_1_init(void) {
  SCTIMER_Init(SCTIMER_1_PERIPHERAL, &SCTimer_1_initConfig);
  /* Initialization of state 0 */
  SCTIMER_CreateAndScheduleEvent(SCTIMER_1_PERIPHERAL, kSCTIMER_MatchEventOnly, 65535, kSCTIMER_Out_0, kSCTIMER_Counter_L, &SCTimer_1_event[0]);
  SCTIMER_SetupCounterLimitAction(SCTIMER_1_PERIPHERAL, kSCTIMER_Counter_L, SCTimer_1_event[0]);
  SCTIMER_SetupNextStateAction(SCTIMER_1_PERIPHERAL, 0, SCTimer_1_event[0]);
  /* Enable interrupt SCT0_IRQn request in the NVIC */
  EnableIRQ(SCTIMER_1_IRQN);
  SCTIMER_StartTimer(SCTIMER_1_PERIPHERAL, kSCTIMER_Counter_L);
}

/***********************************************************************************************************************
 * USART_RPI_TX initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'USART_RPI_TX'
- type: 'flexcomm_usart'
- mode: 'interrupts'
- type_id: 'flexcomm_usart_fcc110cc6b16332e9dfd9e0df675e21f'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'FLEXCOMM3'
- config_sets:
  - interruptsCfg:
    - interrupts: 'kUSART_RxLevelInterruptEnable'
    - interrupt_vectors:
      - enable_rx_tx_irq: 'false'
      - interrupt_rx_tx:
        - IRQn: 'FLEXCOMM3_IRQn'
        - enable_priority: 'false'
        - enable_custom_name: 'false'
  - usartConfig_t:
    - usartConfig:
      - clockSource: 'FXCOMFunctionClock'
      - clockSourceFreq: 'BOARD_BootClockRUN'
      - baudRate_Bps: '500000'
      - parityMode: 'kUSART_ParityDisabled'
      - stopBitCount: 'kUSART_OneStopBit'
      - bitCountPerChar: 'kUSART_8BitsPerChar'
      - loopback: 'false'
      - txWatermark: 'kUSART_TxFifo0'
      - rxWatermark: 'kUSART_RxFifo1'
      - enableRx: 'false'
      - enableTx: 'true'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const usart_config_t USART_RPI_TX_config = {
  .baudRate_Bps = 500000,
  .parityMode = kUSART_ParityDisabled,
  .stopBitCount = kUSART_OneStopBit,
  .bitCountPerChar = kUSART_8BitsPerChar,
  .loopback = false,
  .txWatermark = kUSART_TxFifo0,
  .rxWatermark = kUSART_RxFifo1,
  .enableRx = false,
  .enableTx = true
};

void USART_RPI_TX_init(void) {
  /* Reset FLEXCOMM device */
  RESET_PeripheralReset(kFC3_RST_SHIFT_RSTn);
  USART_Init(USART_RPI_TX_PERIPHERAL, &USART_RPI_TX_config, USART_RPI_TX_CLOCK_SOURCE);
  USART_EnableInterrupts(USART_RPI_TX_PERIPHERAL, kUSART_RxLevelInterruptEnable);
}

/***********************************************************************************************************************
 * USART_RPI_RX initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'USART_RPI_RX'
- type: 'flexcomm_usart'
- mode: 'interrupts'
- type_id: 'flexcomm_usart_fcc110cc6b16332e9dfd9e0df675e21f'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'FLEXCOMM0'
- config_sets:
  - interruptsCfg:
    - interrupts: 'kUSART_RxLevelInterruptEnable'
    - interrupt_vectors:
      - enable_rx_tx_irq: 'false'
      - interrupt_rx_tx:
        - IRQn: 'FLEXCOMM0_IRQn'
        - enable_priority: 'false'
        - enable_custom_name: 'false'
  - usartConfig_t:
    - usartConfig:
      - clockSource: 'FXCOMFunctionClock'
      - clockSourceFreq: 'BOARD_BootClockRUN'
      - baudRate_Bps: '500000'
      - parityMode: 'kUSART_ParityDisabled'
      - stopBitCount: 'kUSART_OneStopBit'
      - bitCountPerChar: 'kUSART_8BitsPerChar'
      - loopback: 'false'
      - txWatermark: 'kUSART_TxFifo0'
      - rxWatermark: 'kUSART_RxFifo1'
      - enableRx: 'true'
      - enableTx: 'false'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const usart_config_t USART_RPI_RX_config = {
  .baudRate_Bps = 500000,
  .parityMode = kUSART_ParityDisabled,
  .stopBitCount = kUSART_OneStopBit,
  .bitCountPerChar = kUSART_8BitsPerChar,
  .loopback = false,
  .txWatermark = kUSART_TxFifo0,
  .rxWatermark = kUSART_RxFifo1,
  .enableRx = true,
  .enableTx = false
};

void USART_RPI_RX_init(void) {
  /* Reset FLEXCOMM device */
  RESET_PeripheralReset(kFC0_RST_SHIFT_RSTn);
  USART_Init(USART_RPI_RX_PERIPHERAL, &USART_RPI_RX_config, USART_RPI_RX_CLOCK_SOURCE);
  USART_EnableInterrupts(USART_RPI_RX_PERIPHERAL, kUSART_RxLevelInterruptEnable);
}

/***********************************************************************************************************************
 * USART_RPI_OLD initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'USART_RPI_OLD'
- type: 'flexcomm_usart'
- mode: 'interrupts'
- type_id: 'flexcomm_usart_fcc110cc6b16332e9dfd9e0df675e21f'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'FLEXCOMM2'
- config_sets:
  - interruptsCfg:
    - interrupts: 'kUSART_RxLevelInterruptEnable'
    - interrupt_vectors:
      - enable_rx_tx_irq: 'false'
      - interrupt_rx_tx:
        - IRQn: 'FLEXCOMM2_IRQn'
        - enable_priority: 'false'
        - enable_custom_name: 'false'
  - usartConfig_t:
    - usartConfig:
      - clockSource: 'FXCOMFunctionClock'
      - clockSourceFreq: 'BOARD_BootClockRUN'
      - baudRate_Bps: '500000'
      - parityMode: 'kUSART_ParityDisabled'
      - stopBitCount: 'kUSART_OneStopBit'
      - bitCountPerChar: 'kUSART_8BitsPerChar'
      - loopback: 'false'
      - txWatermark: 'kUSART_TxFifo0'
      - rxWatermark: 'kUSART_RxFifo1'
      - enableRx: 'false'
      - enableTx: 'false'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const usart_config_t USART_RPI_OLD_config = {
  .baudRate_Bps = 500000,
  .parityMode = kUSART_ParityDisabled,
  .stopBitCount = kUSART_OneStopBit,
  .bitCountPerChar = kUSART_8BitsPerChar,
  .loopback = false,
  .txWatermark = kUSART_TxFifo0,
  .rxWatermark = kUSART_RxFifo1,
  .enableRx = false,
  .enableTx = false
};

void USART_RPI_OLD_init(void) {
  /* Reset FLEXCOMM device */
  RESET_PeripheralReset(kFC2_RST_SHIFT_RSTn);
  USART_Init(USART_RPI_OLD_PERIPHERAL, &USART_RPI_OLD_config, USART_RPI_OLD_CLOCK_SOURCE);
  USART_EnableInterrupts(USART_RPI_OLD_PERIPHERAL, kUSART_RxLevelInterruptEnable);
}

/***********************************************************************************************************************
 * Initialization functions
 **********************************************************************************************************************/
void BOARD_InitPeripherals(void)
{
  /* Initialize components */
  USART_MIDI_init();
  USART_DSP_init();
  SCTimer_1_init();
  USART_RPI_TX_init();
  USART_RPI_RX_init();
  USART_RPI_OLD_init();
}

/***********************************************************************************************************************
 * BOARD_InitBootPeripherals function
 **********************************************************************************************************************/
void BOARD_InitBootPeripherals(void)
{
  BOARD_InitPeripherals();
}
