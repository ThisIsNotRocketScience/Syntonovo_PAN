/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Pins v4.1
processor: LPC54616J512
package_id: LPC54616J512BD100
mcu_data: ksdk2_0
processor_version: 4.0.0
pin_labels:
- {pin_num: '3', pin_signal: PIO1_4/FC0_SCK/SD_D(0)/CTIMER2_MAT1/SCT0_OUT0/FREQME_GPIO_CLK_A/EMC_D(11), label: SELF, identifier: SELF}
- {pin_num: '4', pin_signal: PIO1_5/FC0_RXD_SDA_MOSI/SD_D(2)/CTIMER2_MAT0/SCT0_GPI0/EMC_A(4), label: SELE, identifier: SELE}
- {pin_num: '5', pin_signal: PIO1_18/FC8_TXD_SCL_MISO/SCT0_OUT5/CAN1_RD/EMC_BLSN(1), label: SELC, identifier: SELC}
- {pin_num: '16', pin_signal: PIO1_19/FC8_SCK/SCT0_OUT7/CTIMER3_MAT1/SCT0_GPI7/FC4_SCK/EMC_D(8), label: SELA, identifier: SELA}
- {pin_num: '28', pin_signal: PIO0_31/FC0_CTS_SDA_SSEL0/SD_D(2)/CTIMER0_MAT1/SCT0_OUT3/TRACEDATA(0)/ADC0_5, label: TUNE, identifier: TUNE}
- {pin_num: '29', pin_signal: PIO1_0/FC0_RTS_SCL_SSEL1/SD_D(3)/CTIMER0_CAP2/SCT0_GPI4/TRACECLK/ADC0_6, label: GATE, identifier: GATE}
- {pin_num: '36', pin_signal: PIO1_8/FC0_CTS_SDA_SSEL0/SD_CLK/SCT0_OUT1/FC4_SSEL2/EMC_A(7), label: SLATCH0, identifier: SLATCH0}
- {pin_num: '47', pin_signal: PIO1_17/ENET_MDIO/FC8_RXD_SDA_MOSI/SCT0_OUT4/CAN1_TD/EMC_BLSN(0), label: SELB, identifier: SELB}
- {pin_num: '57', pin_signal: PIO1_24/FC2_RXD_SDA_MOSI/SCT0_OUT1/FC3_SSEL3/EMC_A(12), label: SRESET2, identifier: SRESET2}
- {pin_num: '63', pin_signal: PIO1_26/FC2_CTS_SDA_SSEL0/SCT0_OUT3/CTIMER0_CAP3/UTICK_CAP1/EMC_A(8), label: SLATCH2, identifier: SLATCH2}
- {pin_num: '67', pin_signal: PIO0_13/FC1_CTS_SDA_SSEL0/UTICK_CAP0/CTIMER0_CAP0/SCT0_GPI0/ENET_RXD0, label: SLATCH1, identifier: SLATCH1}
- {pin_num: '72', pin_signal: PIO0_18/FC4_CTS_SDA_SSEL0/SD_WR_PRT/CTIMER1_MAT0/SCT0_OUT1/SCI1_SCLK/EMC_A(0), label: EIN_EXT, identifier: EIN_EXT}
- {pin_num: '82', pin_signal: PIO0_29/FC0_RXD_SDA_MOSI/CTIMER2_MAT3/SCT0_OUT8/TRACEDATA(2), label: CODEC_INT, identifier: FC3_INT}
- {pin_num: '88', pin_signal: PIO1_16/ENET_MDC/FC6_TXD_SCL_MISO_WS/CTIMER1_MAT3/SD_CMD/EMC_A(10), label: LED, identifier: LED}
- {pin_num: '92', pin_signal: PIO1_31/MCLK/CTIMER0_MAT2/SCT0_OUT6/FC8_CTS_SDA_SSEL0/EMC_D(15), label: SELD, identifier: SELD}
- {pin_num: '94', pin_signal: PIO1_11/ENET_TX_EN/FC1_TXD_SCL_MISO/CTIMER1_CAP1/USB0_VBUS/EMC_CLK(0), label: SELINHG, identifier: SELINHG}
- {pin_num: '43', pin_signal: PIO1_22/FC8_RTS_SCL_SSEL1/SD_CMD/CTIMER2_MAT3/SCT0_GPI5/FC4_SSEL3/EMC_CKE(1), label: FC2INT, identifier: FC2_INT}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

#include "fsl_common.h"
#include "fsl_gpio.h"
#include "fsl_inputmux.h"
#include "pin_mux.h"

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitBootPins
 * Description   : Calls initialization functions.
 *
 * END ****************************************************************************************************************/
void BOARD_InitBootPins(void)
{
    BOARD_InitPins();
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitPins:
- options: {callFromInitBoot: 'true', coreID: core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '100', peripheral: FLEXCOMM3, signal: CTS_SDA_SSEL0, pin_signal: PIO0_1/CAN1_TD/FC3_CTS_SDA_SSEL0/CTIMER0_CAP0/SCT0_GPI1/PDM0_DATA}
  - {pin_num: '85', peripheral: FLEXCOMM3, signal: RXD_SDA_MOSI, pin_signal: PIO0_3/FC3_RXD_SDA_MOSI/CTIMER0_MAT1/SCT0_OUT1/SCT0_GPI3/EMC_D(1)}
  - {pin_num: '93', peripheral: FLEXCOMM3, signal: SCK, pin_signal: PIO0_0/CAN1_RD/FC3_SCK/CTIMER0_MAT0/SCT0_GPI0/PDM0_CLK}
  - {pin_num: '83', peripheral: FLEXCOMM3, signal: TXD_SCL_MISO, pin_signal: PIO0_2/FC3_TXD_SCL_MISO/CTIMER0_CAP1/SCT0_OUT0/SCT0_GPI2/EMC_D(0)}
  - {pin_num: '24', peripheral: SWD, signal: SWCLK, pin_signal: PIO0_11/FC6_RXD_SDA_MOSI_DATA/CTIMER2_MAT2/FREQME_GPIO_CLK_A/SWCLK/ADC0_1}
  - {pin_num: '25', peripheral: SWD, signal: SWDIO, pin_signal: PIO0_12/FC3_TXD_SCL_MISO/FREQME_GPIO_CLK_B/SCT0_GPI7/SWDIO/ADC0_2}
  - {pin_num: '56', peripheral: FLEXCOMM2, signal: RXD_SDA_MOSI, pin_signal: PIO0_26/FC2_RXD_SDA_MOSI/CLKOUT/CTIMER3_CAP2/SCT0_OUT5/PDM0_CLK/SPIFI_CLK/USB0_IDVALUE}
  - {pin_num: '46', peripheral: FLEXCOMM2, signal: SCK, pin_signal: PIO1_23/FC2_SCK/SCT0_OUT0/ENET_MDIO/FC3_SSEL2/EMC_A(11)}
  - {pin_num: '42', peripheral: FLEXCOMM2, signal: TXD_SCL_MISO, pin_signal: PIO0_27/FC2_TXD_SCL_MISO/CTIMER3_MAT2/SCT0_OUT6/PDM0_DATA/SPIFI_IO(3)}
  - {pin_num: '64', peripheral: FLEXCOMM5, signal: RXD_SDA_MOSI, pin_signal: PIO0_8/FC3_SSEL3/SD_CMD/FC5_RXD_SDA_MOSI/SWO/PDM1_DATA/EMC_D(6)}
  - {pin_num: '65', peripheral: FLEXCOMM5, signal: TXD_SCL_MISO, pin_signal: PIO0_9/FC3_SSEL2/SD_POW_EN/FC5_TXD_SCL_MISO/SCI1_IO/EMC_D(7)}
  - {pin_num: '92', peripheral: GPIO, signal: 'PIO1, 31', pin_signal: PIO1_31/MCLK/CTIMER0_MAT2/SCT0_OUT6/FC8_CTS_SDA_SSEL0/EMC_D(15), direction: OUTPUT}
  - {pin_num: '3', peripheral: GPIO, signal: 'PIO1, 4', pin_signal: PIO1_4/FC0_SCK/SD_D(0)/CTIMER2_MAT1/SCT0_OUT0/FREQME_GPIO_CLK_A/EMC_D(11), direction: OUTPUT}
  - {pin_num: '4', peripheral: GPIO, signal: 'PIO1, 5', pin_signal: PIO1_5/FC0_RXD_SDA_MOSI/SD_D(2)/CTIMER2_MAT0/SCT0_GPI0/EMC_A(4), direction: OUTPUT}
  - {pin_num: '5', peripheral: GPIO, signal: 'PIO1, 18', pin_signal: PIO1_18/FC8_TXD_SCL_MISO/SCT0_OUT5/CAN1_RD/EMC_BLSN(1), direction: OUTPUT}
  - {pin_num: '16', peripheral: GPIO, signal: 'PIO1, 19', pin_signal: PIO1_19/FC8_SCK/SCT0_OUT7/CTIMER3_MAT1/SCT0_GPI7/FC4_SCK/EMC_D(8), direction: OUTPUT}
  - {pin_num: '29', peripheral: GPIO, signal: 'PIO1, 0', pin_signal: PIO1_0/FC0_RTS_SCL_SSEL1/SD_D(3)/CTIMER0_CAP2/SCT0_GPI4/TRACECLK/ADC0_6, direction: OUTPUT, gpio_init_state: 'false'}
  - {pin_num: '28', peripheral: GPIO, signal: 'PIO0, 31', pin_signal: PIO0_31/FC0_CTS_SDA_SSEL0/SD_D(2)/CTIMER0_MAT1/SCT0_OUT3/TRACEDATA(0)/ADC0_5, direction: INPUT,
    mode: inactive}
  - {pin_num: '36', peripheral: GPIO, signal: 'PIO1, 8', pin_signal: PIO1_8/FC0_CTS_SDA_SSEL0/SD_CLK/SCT0_OUT1/FC4_SSEL2/EMC_A(7), direction: OUTPUT, gpio_init_state: 'false'}
  - {pin_num: '38', peripheral: FLEXCOMM0, signal: RXD_SDA_MOSI, pin_signal: PIO0_24/FC0_RXD_SDA_MOSI/SD_D(0)/CTIMER2_CAP0/SCT0_GPI0/SPIFI_IO(0)}
  - {pin_num: '43', peripheral: GPIO, signal: 'PIO1, 22', pin_signal: PIO1_22/FC8_RTS_SCL_SSEL1/SD_CMD/CTIMER2_MAT3/SCT0_GPI5/FC4_SSEL3/EMC_CKE(1), direction: INPUT,
    mode: pullUp}
  - {pin_num: '44', peripheral: FLEXCOMM0, signal: SCK, pin_signal: PIO0_28/FC0_SCK/CTIMER2_CAP3/SCT0_OUT7/TRACEDATA(3)/SPIFI_IO(2)/USB0_OVERCURRENTN}
  - {pin_num: '47', peripheral: GPIO, signal: 'PIO1, 17', pin_signal: PIO1_17/ENET_MDIO/FC8_RXD_SDA_MOSI/SCT0_OUT4/CAN1_TD/EMC_BLSN(0), direction: OUTPUT}
  - {pin_num: '57', peripheral: GPIO, signal: 'PIO1, 24', pin_signal: PIO1_24/FC2_RXD_SDA_MOSI/SCT0_OUT1/FC3_SSEL3/EMC_A(12), direction: OUTPUT}
  - {pin_num: '63', peripheral: GPIO, signal: 'PIO1, 26', pin_signal: PIO1_26/FC2_CTS_SDA_SSEL0/SCT0_OUT3/CTIMER0_CAP3/UTICK_CAP1/EMC_A(8), direction: OUTPUT}
  - {pin_num: '67', peripheral: GPIO, signal: 'PIO0, 13', pin_signal: PIO0_13/FC1_CTS_SDA_SSEL0/UTICK_CAP0/CTIMER0_CAP0/SCT0_GPI0/ENET_RXD0, direction: OUTPUT}
  - {pin_num: '72', peripheral: GPIO, signal: 'PIO0, 18', pin_signal: PIO0_18/FC4_CTS_SDA_SSEL0/SD_WR_PRT/CTIMER1_MAT0/SCT0_OUT1/SCI1_SCLK/EMC_A(0), direction: OUTPUT,
    mode: inactive}
  - {pin_num: '82', peripheral: GPIO, signal: 'PIO0, 29', pin_signal: PIO0_29/FC0_RXD_SDA_MOSI/CTIMER2_MAT3/SCT0_OUT8/TRACEDATA(2), direction: INPUT, mode: pullUp}
  - {pin_num: '87', peripheral: FLEXCOMM4, signal: SCK, pin_signal: PIO0_4/CAN0_RD/FC4_SCK/CTIMER3_CAP0/SCT0_GPI4/EMC_D(2)/ENET_MDC}
  - {pin_num: '88', peripheral: GPIO, signal: 'PIO1, 16', pin_signal: PIO1_16/ENET_MDC/FC6_TXD_SCL_MISO_WS/CTIMER1_MAT3/SD_CMD/EMC_A(10), direction: OUTPUT, gpio_init_state: 'false'}
  - {pin_num: '89', peripheral: FLEXCOMM4, signal: RXD_SDA_MOSI, pin_signal: PIO0_5/CAN0_TD/FC4_RXD_SDA_MOSI/CTIMER3_MAT0/SCT0_GPI5/EMC_D(3)/ENET_MDIO}
  - {pin_num: '91', peripheral: FLEXCOMM4, signal: RTS_SCL_SSEL1, pin_signal: PIO0_19/FC4_RTS_SCL_SSEL1/UTICK_CAP0/CTIMER0_MAT2/SCT0_OUT2/EMC_A(1)/FC7_TXD_SCL_MISO_WS}
  - {pin_num: '94', peripheral: GPIO, signal: 'PIO1, 11', pin_signal: PIO1_11/ENET_TX_EN/FC1_TXD_SCL_MISO/CTIMER1_CAP1/USB0_VBUS/EMC_CLK(0), direction: OUTPUT, gpio_init_state: 'false'}
  - {pin_num: '28', peripheral: PINT, signal: 'PINT, 0', pin_signal: PIO0_31/FC0_CTS_SDA_SSEL0/SD_D(2)/CTIMER0_MAT1/SCT0_OUT3/TRACEDATA(0)/ADC0_5, identifier: ''}
  - {pin_num: '81', peripheral: FLEXCOMM7, signal: RXD_SDA_MOSI_DATA, pin_signal: PIO1_29/FC7_RXD_SDA_MOSI_DATA/SD_D(6)/SCT0_GPI6/USB1_PORTPWRN/USB1_FRAME/EMC_D(13)}
  - {pin_num: '86', peripheral: FLEXCOMM7, signal: TXD_SCL_MISO_WS, pin_signal: PIO1_30/FC7_TXD_SCL_MISO_WS/SD_D(7)/SCT0_GPI7/USB1_OVERCURRENTN/USB1_UP_LED/EMC_D(14)}
  - {pin_num: '26', peripheral: GPIO, signal: 'PIO0, 15', pin_signal: PIO0_15/FC6_CTS_SDA_SSEL0/UTICK_CAP2/CTIMER4_CAP0/SCT0_OUT2/EMC_WEN/ENET_TX_EN/ADC0_3}
  - {pin_num: '27', peripheral: CTIMER1, signal: 'CAPTURE, 0', pin_signal: PIO0_16/FC4_TXD_SCL_MISO/CLKOUT/CTIMER1_CAP0/EMC_CSN(0)/ENET_TXD0/ADC0_4}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
/* Function assigned for the Cortex-M4F */
void BOARD_InitPins(void)
{
    /* Enables the clock for the input muxes. 0 = Disable; 1 = Enable.: 0x01u */
    CLOCK_EnableClock(kCLOCK_InputMux);
    /* Enables the clock for the IOCON block. 0 = Disable; 1 = Enable.: 0x01u */
    CLOCK_EnableClock(kCLOCK_Iocon);
    /* Enables the clock for the GPIO0 module */
    CLOCK_EnableClock(kCLOCK_Gpio0);
    /* Enables the clock for the GPIO1 module */
    CLOCK_EnableClock(kCLOCK_Gpio1);

    gpio_pin_config_t SLATCH1_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO0_13 (pin 67)  */
    GPIO_PinInit(BOARD_INITPINS_SLATCH1_GPIO, BOARD_INITPINS_SLATCH1_PORT, BOARD_INITPINS_SLATCH1_PIN, &SLATCH1_config);

    gpio_pin_config_t EIN_EXT_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO0_18 (pin 72)  */
    GPIO_PinInit(BOARD_INITPINS_EIN_EXT_GPIO, BOARD_INITPINS_EIN_EXT_PORT, BOARD_INITPINS_EIN_EXT_PIN, &EIN_EXT_config);

    gpio_pin_config_t FC3_INT_config = {
        .pinDirection = kGPIO_DigitalInput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO0_29 (pin 82)  */
    GPIO_PinInit(BOARD_INITPINS_FC3_INT_GPIO, BOARD_INITPINS_FC3_INT_PORT, BOARD_INITPINS_FC3_INT_PIN, &FC3_INT_config);

    gpio_pin_config_t TUNE_config = {
        .pinDirection = kGPIO_DigitalInput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO0_31 (pin 28)  */
    GPIO_PinInit(BOARD_INITPINS_TUNE_GPIO, BOARD_INITPINS_TUNE_PORT, BOARD_INITPINS_TUNE_PIN, &TUNE_config);

    gpio_pin_config_t GATE_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO1_0 (pin 29)  */
    GPIO_PinInit(BOARD_INITPINS_GATE_GPIO, BOARD_INITPINS_GATE_PORT, BOARD_INITPINS_GATE_PIN, &GATE_config);

    gpio_pin_config_t SELF_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO1_4 (pin 3)  */
    GPIO_PinInit(BOARD_INITPINS_SELF_GPIO, BOARD_INITPINS_SELF_PORT, BOARD_INITPINS_SELF_PIN, &SELF_config);

    gpio_pin_config_t SELE_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO1_5 (pin 4)  */
    GPIO_PinInit(BOARD_INITPINS_SELE_GPIO, BOARD_INITPINS_SELE_PORT, BOARD_INITPINS_SELE_PIN, &SELE_config);

    gpio_pin_config_t SLATCH0_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO1_8 (pin 36)  */
    GPIO_PinInit(BOARD_INITPINS_SLATCH0_GPIO, BOARD_INITPINS_SLATCH0_PORT, BOARD_INITPINS_SLATCH0_PIN, &SLATCH0_config);

    gpio_pin_config_t SELINHG_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO1_11 (pin 94)  */
    GPIO_PinInit(BOARD_INITPINS_SELINHG_GPIO, BOARD_INITPINS_SELINHG_PORT, BOARD_INITPINS_SELINHG_PIN, &SELINHG_config);

    gpio_pin_config_t LED_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO1_16 (pin 88)  */
    GPIO_PinInit(BOARD_INITPINS_LED_GPIO, BOARD_INITPINS_LED_PORT, BOARD_INITPINS_LED_PIN, &LED_config);

    gpio_pin_config_t SELB_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO1_17 (pin 47)  */
    GPIO_PinInit(BOARD_INITPINS_SELB_GPIO, BOARD_INITPINS_SELB_PORT, BOARD_INITPINS_SELB_PIN, &SELB_config);

    gpio_pin_config_t SELC_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO1_18 (pin 5)  */
    GPIO_PinInit(BOARD_INITPINS_SELC_GPIO, BOARD_INITPINS_SELC_PORT, BOARD_INITPINS_SELC_PIN, &SELC_config);

    gpio_pin_config_t SELA_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO1_19 (pin 16)  */
    GPIO_PinInit(BOARD_INITPINS_SELA_GPIO, BOARD_INITPINS_SELA_PORT, BOARD_INITPINS_SELA_PIN, &SELA_config);

    gpio_pin_config_t FC2_INT_config = {
        .pinDirection = kGPIO_DigitalInput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO1_22 (pin 43)  */
    GPIO_PinInit(BOARD_INITPINS_FC2_INT_GPIO, BOARD_INITPINS_FC2_INT_PORT, BOARD_INITPINS_FC2_INT_PIN, &FC2_INT_config);

    gpio_pin_config_t SRESET2_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO1_24 (pin 57)  */
    GPIO_PinInit(BOARD_INITPINS_SRESET2_GPIO, BOARD_INITPINS_SRESET2_PORT, BOARD_INITPINS_SRESET2_PIN, &SRESET2_config);

    gpio_pin_config_t SLATCH2_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO1_26 (pin 63)  */
    GPIO_PinInit(BOARD_INITPINS_SLATCH2_GPIO, BOARD_INITPINS_SLATCH2_PORT, BOARD_INITPINS_SLATCH2_PIN, &SLATCH2_config);

    gpio_pin_config_t SELD_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO1_31 (pin 92)  */
    GPIO_PinInit(BOARD_INITPINS_SELD_GPIO, BOARD_INITPINS_SELD_PORT, BOARD_INITPINS_SELD_PIN, &SELD_config);
    /* PIO0_31 is selected for PINT input 0 */
    INPUTMUX_AttachSignal(INPUTMUX, 0U, kINPUTMUX_GpioPort0Pin31ToPintsel);

    IOCON->PIO[0][0] = ((IOCON->PIO[0][0] &
                         /* Mask bits to zero which are setting */
                         (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                        /* Selects pin function.
                         * : PORT00 (pin 93) is configured as FC3_SCK. */
                        | IOCON_PIO_FUNC(PIO00_FUNC_ALT2)

                        /* Select Analog/Digital mode.
                         * : Digital mode. */
                        | IOCON_PIO_DIGIMODE(PIO00_DIGIMODE_DIGITAL));

    IOCON->PIO[0][1] = ((IOCON->PIO[0][1] &
                         /* Mask bits to zero which are setting */
                         (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                        /* Selects pin function.
                         * : PORT01 (pin 100) is configured as FC3_CTS_SDA_SSEL0. */
                        | IOCON_PIO_FUNC(PIO01_FUNC_ALT2)

                        /* Select Analog/Digital mode.
                         * : Digital mode. */
                        | IOCON_PIO_DIGIMODE(PIO01_DIGIMODE_DIGITAL));

    IOCON->PIO[0][11] = ((IOCON->PIO[0][11] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT011 (pin 24) is configured as SWCLK. */
                         | IOCON_PIO_FUNC(PIO011_FUNC_ALT6)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO011_DIGIMODE_DIGITAL));

    IOCON->PIO[0][12] = ((IOCON->PIO[0][12] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT012 (pin 25) is configured as SWDIO. */
                         | IOCON_PIO_FUNC(PIO012_FUNC_ALT6)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO012_DIGIMODE_DIGITAL));

    IOCON->PIO[0][13] = ((IOCON->PIO[0][13] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT013 (pin 67) is configured as PIO0_13. */
                         | IOCON_PIO_FUNC(PIO013_FUNC_ALT0)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO013_DIGIMODE_DIGITAL));

    IOCON->PIO[0][15] = ((IOCON->PIO[0][15] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT015 (pin 26) is configured as PIO0_15. */
                         | IOCON_PIO_FUNC(PIO015_FUNC_ALT0)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO015_DIGIMODE_DIGITAL));

    IOCON->PIO[0][16] = ((IOCON->PIO[0][16] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT016 (pin 27) is configured as CTIMER1_CAP0. */
                         | IOCON_PIO_FUNC(PIO016_FUNC_ALT3)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO016_DIGIMODE_DIGITAL));

    IOCON->PIO[0][18] = ((IOCON->PIO[0][18] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_MODE_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT018 (pin 72) is configured as PIO0_18. */
                         | IOCON_PIO_FUNC(PIO018_FUNC_ALT0)

                         /* Selects function mode (on-chip pull-up/pull-down resistor control).
                          * : Inactive.
                          * Inactive (no pull-down/pull-up resistor enabled). */
                         | IOCON_PIO_MODE(PIO018_MODE_INACTIVE)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO018_DIGIMODE_DIGITAL));

    IOCON->PIO[0][19] = ((IOCON->PIO[0][19] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT019 (pin 91) is configured as FC4_RTS_SCL_SSEL1. */
                         | IOCON_PIO_FUNC(PIO019_FUNC_ALT1)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO019_DIGIMODE_DIGITAL));

    IOCON->PIO[0][2] = ((IOCON->PIO[0][2] &
                         /* Mask bits to zero which are setting */
                         (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                        /* Selects pin function.
                         * : PORT02 (pin 83) is configured as FC3_TXD_SCL_MISO. */
                        | IOCON_PIO_FUNC(PIO02_FUNC_ALT1)

                        /* Select Analog/Digital mode.
                         * : Digital mode. */
                        | IOCON_PIO_DIGIMODE(PIO02_DIGIMODE_DIGITAL));

    IOCON->PIO[0][24] = ((IOCON->PIO[0][24] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT024 (pin 38) is configured as FC0_RXD_SDA_MOSI. */
                         | IOCON_PIO_FUNC(PIO024_FUNC_ALT1)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO024_DIGIMODE_DIGITAL));

    IOCON->PIO[0][26] = ((IOCON->PIO[0][26] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT026 (pin 56) is configured as FC2_RXD_SDA_MOSI. */
                         | IOCON_PIO_FUNC(PIO026_FUNC_ALT1)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO026_DIGIMODE_DIGITAL));

    IOCON->PIO[0][27] = ((IOCON->PIO[0][27] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT027 (pin 42) is configured as FC2_TXD_SCL_MISO. */
                         | IOCON_PIO_FUNC(PIO027_FUNC_ALT1)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO027_DIGIMODE_DIGITAL));

    IOCON->PIO[0][28] = ((IOCON->PIO[0][28] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT028 (pin 44) is configured as FC0_SCK. */
                         | IOCON_PIO_FUNC(PIO028_FUNC_ALT1)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO028_DIGIMODE_DIGITAL));

    IOCON->PIO[0][29] = ((IOCON->PIO[0][29] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_MODE_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT029 (pin 82) is configured as PIO0_29. */
                         | IOCON_PIO_FUNC(PIO029_FUNC_ALT0)

                         /* Selects function mode (on-chip pull-up/pull-down resistor control).
                          * : Pull-up.
                          * Pull-up resistor enabled. */
                         | IOCON_PIO_MODE(PIO029_MODE_PULL_UP)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO029_DIGIMODE_DIGITAL));

    IOCON->PIO[0][3] = ((IOCON->PIO[0][3] &
                         /* Mask bits to zero which are setting */
                         (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                        /* Selects pin function.
                         * : PORT03 (pin 85) is configured as FC3_RXD_SDA_MOSI. */
                        | IOCON_PIO_FUNC(PIO03_FUNC_ALT1)

                        /* Select Analog/Digital mode.
                         * : Digital mode. */
                        | IOCON_PIO_DIGIMODE(PIO03_DIGIMODE_DIGITAL));

    IOCON->PIO[0][31] = ((IOCON->PIO[0][31] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_MODE_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT031 (pin 28) is configured as PIO0_31. */
                         | IOCON_PIO_FUNC(PIO031_FUNC_ALT0)

                         /* Selects function mode (on-chip pull-up/pull-down resistor control).
                          * : Inactive.
                          * Inactive (no pull-down/pull-up resistor enabled). */
                         | IOCON_PIO_MODE(PIO031_MODE_INACTIVE)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO031_DIGIMODE_DIGITAL));

    IOCON->PIO[0][4] = ((IOCON->PIO[0][4] &
                         /* Mask bits to zero which are setting */
                         (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                        /* Selects pin function.
                         * : PORT04 (pin 87) is configured as FC4_SCK. */
                        | IOCON_PIO_FUNC(PIO04_FUNC_ALT2)

                        /* Select Analog/Digital mode.
                         * : Digital mode. */
                        | IOCON_PIO_DIGIMODE(PIO04_DIGIMODE_DIGITAL));

    IOCON->PIO[0][5] = ((IOCON->PIO[0][5] &
                         /* Mask bits to zero which are setting */
                         (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                        /* Selects pin function.
                         * : PORT05 (pin 89) is configured as FC4_RXD_SDA_MOSI. */
                        | IOCON_PIO_FUNC(PIO05_FUNC_ALT2)

                        /* Select Analog/Digital mode.
                         * : Digital mode. */
                        | IOCON_PIO_DIGIMODE(PIO05_DIGIMODE_DIGITAL));

    IOCON->PIO[0][8] = ((IOCON->PIO[0][8] &
                         /* Mask bits to zero which are setting */
                         (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                        /* Selects pin function.
                         * : PORT08 (pin 64) is configured as FC5_RXD_SDA_MOSI. */
                        | IOCON_PIO_FUNC(PIO08_FUNC_ALT3)

                        /* Select Analog/Digital mode.
                         * : Digital mode. */
                        | IOCON_PIO_DIGIMODE(PIO08_DIGIMODE_DIGITAL));

    IOCON->PIO[0][9] = ((IOCON->PIO[0][9] &
                         /* Mask bits to zero which are setting */
                         (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                        /* Selects pin function.
                         * : PORT09 (pin 65) is configured as FC5_TXD_SCL_MISO. */
                        | IOCON_PIO_FUNC(PIO09_FUNC_ALT3)

                        /* Select Analog/Digital mode.
                         * : Digital mode. */
                        | IOCON_PIO_DIGIMODE(PIO09_DIGIMODE_DIGITAL));

    IOCON->PIO[1][0] = ((IOCON->PIO[1][0] &
                         /* Mask bits to zero which are setting */
                         (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                        /* Selects pin function.
                         * : PORT10 (pin 29) is configured as PIO1_0. */
                        | IOCON_PIO_FUNC(PIO10_FUNC_ALT0)

                        /* Select Analog/Digital mode.
                         * : Digital mode. */
                        | IOCON_PIO_DIGIMODE(PIO10_DIGIMODE_DIGITAL));

    IOCON->PIO[1][11] = ((IOCON->PIO[1][11] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT111 (pin 94) is configured as PIO1_11. */
                         | IOCON_PIO_FUNC(PIO111_FUNC_ALT0)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO111_DIGIMODE_DIGITAL));

    IOCON->PIO[1][16] = ((IOCON->PIO[1][16] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT116 (pin 88) is configured as PIO1_16. */
                         | IOCON_PIO_FUNC(PIO116_FUNC_ALT0)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO116_DIGIMODE_DIGITAL));

    IOCON->PIO[1][17] = ((IOCON->PIO[1][17] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT117 (pin 47) is configured as PIO1_17. */
                         | IOCON_PIO_FUNC(PIO117_FUNC_ALT0)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO117_DIGIMODE_DIGITAL));

    IOCON->PIO[1][18] = ((IOCON->PIO[1][18] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT118 (pin 5) is configured as PIO1_18. */
                         | IOCON_PIO_FUNC(PIO118_FUNC_ALT0)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO118_DIGIMODE_DIGITAL));

    IOCON->PIO[1][19] = ((IOCON->PIO[1][19] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT119 (pin 16) is configured as PIO1_19. */
                         | IOCON_PIO_FUNC(PIO119_FUNC_ALT0)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO119_DIGIMODE_DIGITAL));

    IOCON->PIO[1][22] = ((IOCON->PIO[1][22] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_MODE_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT122 (pin 43) is configured as PIO1_22. */
                         | IOCON_PIO_FUNC(PIO122_FUNC_ALT0)

                         /* Selects function mode (on-chip pull-up/pull-down resistor control).
                          * : Pull-up.
                          * Pull-up resistor enabled. */
                         | IOCON_PIO_MODE(PIO122_MODE_PULL_UP)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO122_DIGIMODE_DIGITAL));

    IOCON->PIO[1][23] = ((IOCON->PIO[1][23] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT123 (pin 46) is configured as FC2_SCK. */
                         | IOCON_PIO_FUNC(PIO123_FUNC_ALT1)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO123_DIGIMODE_DIGITAL));

    IOCON->PIO[1][24] = ((IOCON->PIO[1][24] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT124 (pin 57) is configured as PIO1_24. */
                         | IOCON_PIO_FUNC(PIO124_FUNC_ALT0)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO124_DIGIMODE_DIGITAL));

    IOCON->PIO[1][26] = ((IOCON->PIO[1][26] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT126 (pin 63) is configured as PIO1_26. */
                         | IOCON_PIO_FUNC(PIO126_FUNC_ALT0)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO126_DIGIMODE_DIGITAL));

    IOCON->PIO[1][29] = ((IOCON->PIO[1][29] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT129 (pin 81) is configured as FC7_RXD_SDA_MOSI_DATA. */
                         | IOCON_PIO_FUNC(PIO129_FUNC_ALT1)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO129_DIGIMODE_DIGITAL));

    IOCON->PIO[1][30] = ((IOCON->PIO[1][30] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT130 (pin 86) is configured as FC7_TXD_SCL_MISO_WS. */
                         | IOCON_PIO_FUNC(PIO130_FUNC_ALT1)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO130_DIGIMODE_DIGITAL));

    IOCON->PIO[1][31] = ((IOCON->PIO[1][31] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT131 (pin 92) is configured as PIO1_31. */
                         | IOCON_PIO_FUNC(PIO131_FUNC_ALT0)

                         /* Select Analog/Digital mode.
                          * : Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO131_DIGIMODE_DIGITAL));

    IOCON->PIO[1][4] = ((IOCON->PIO[1][4] &
                         /* Mask bits to zero which are setting */
                         (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                        /* Selects pin function.
                         * : PORT14 (pin 3) is configured as PIO1_4. */
                        | IOCON_PIO_FUNC(PIO14_FUNC_ALT0)

                        /* Select Analog/Digital mode.
                         * : Digital mode. */
                        | IOCON_PIO_DIGIMODE(PIO14_DIGIMODE_DIGITAL));

    IOCON->PIO[1][5] = ((IOCON->PIO[1][5] &
                         /* Mask bits to zero which are setting */
                         (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                        /* Selects pin function.
                         * : PORT15 (pin 4) is configured as PIO1_5. */
                        | IOCON_PIO_FUNC(PIO15_FUNC_ALT0)

                        /* Select Analog/Digital mode.
                         * : Digital mode. */
                        | IOCON_PIO_DIGIMODE(PIO15_DIGIMODE_DIGITAL));

    IOCON->PIO[1][8] = ((IOCON->PIO[1][8] &
                         /* Mask bits to zero which are setting */
                         (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                        /* Selects pin function.
                         * : PORT18 (pin 36) is configured as PIO1_8. */
                        | IOCON_PIO_FUNC(PIO18_FUNC_ALT0)

                        /* Select Analog/Digital mode.
                         * : Digital mode. */
                        | IOCON_PIO_DIGIMODE(PIO18_DIGIMODE_DIGITAL));
}
/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
