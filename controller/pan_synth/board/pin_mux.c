/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Pins v4.0
processor: LPC54616J512
package_id: LPC54616J512BD100
mcu_data: ksdk2_0
processor_version: 3.0.1
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

#include "fsl_common.h"
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
  - {pin_num: '77', peripheral: FLEXCOMM3, signal: RTS_SCL_SSEL1, pin_signal: PIO0_21/FC3_RTS_SCL_SSEL1/UTICK_CAP3/CTIMER3_MAT3/SCT0_GPI3/SCI0_SCLK/EMC_A(3)/FC7_SCK}
  - {pin_num: '63', peripheral: FLEXCOMM2, signal: CTS_SDA_SSEL0, pin_signal: PIO1_26/FC2_CTS_SDA_SSEL0/SCT0_OUT3/CTIMER0_CAP3/UTICK_CAP1/EMC_A(8)}
  - {pin_num: '68', peripheral: FLEXCOMM2, signal: RTS_SCL_SSEL1, pin_signal: PIO1_27/FC2_RTS_SCL_SSEL1/SD_D(4)/CTIMER0_MAT3/CLKOUT/EMC_A(9)}
  - {pin_num: '56', peripheral: FLEXCOMM2, signal: RXD_SDA_MOSI, pin_signal: PIO0_26/FC2_RXD_SDA_MOSI/CLKOUT/CTIMER3_CAP2/SCT0_OUT5/PDM0_CLK/SPIFI_CLK/USB0_IDVALUE}
  - {pin_num: '46', peripheral: FLEXCOMM2, signal: SCK, pin_signal: PIO1_23/FC2_SCK/SCT0_OUT0/ENET_MDIO/FC3_SSEL2/EMC_A(11)}
  - {pin_num: '42', peripheral: FLEXCOMM2, signal: TXD_SCL_MISO, pin_signal: PIO0_27/FC2_TXD_SCL_MISO/CTIMER3_MAT2/SCT0_OUT6/PDM0_DATA/SPIFI_IO(3)}
  - {pin_num: '78', peripheral: FLEXCOMM5, signal: CTS_SDA_SSEL0, pin_signal: PIO1_14/ENET_RX_DV/UTICK_CAP2/CTIMER1_MAT2/FC5_CTS_SDA_SSEL0/USB0_UP_LED/EMC_DQM(1)}
  - {pin_num: '84', peripheral: FLEXCOMM5, signal: RTS_SCL_SSEL1, pin_signal: PIO1_15/ENET_RX_CLK/UTICK_CAP3/CTIMER1_CAP3/FC5_RTS_SCL_SSEL1/FC4_RTS_SCL_SSEL1/EMC_CKE(0)}
  - {pin_num: '64', peripheral: FLEXCOMM5, signal: RXD_SDA_MOSI, pin_signal: PIO0_8/FC3_SSEL3/SD_CMD/FC5_RXD_SDA_MOSI/SWO/PDM1_DATA/EMC_D(6)}
  - {pin_num: '61', peripheral: FLEXCOMM5, signal: SCK, pin_signal: PIO0_7/FC3_RTS_SCL_SSEL1/SD_CLK/FC5_SCK/FC1_SCK/PDM1_CLK/EMC_D(5)/ENET_RX_CLK}
  - {pin_num: '65', peripheral: FLEXCOMM5, signal: TXD_SCL_MISO, pin_signal: PIO0_9/FC3_SSEL2/SD_POW_EN/FC5_TXD_SCL_MISO/SCI1_IO/EMC_D(7)}
  - {pin_num: '26', peripheral: FLEXCOMM6, signal: CTS_SDA_SSEL0, pin_signal: PIO0_15/FC6_CTS_SDA_SSEL0/UTICK_CAP2/CTIMER4_CAP0/SCT0_OUT2/EMC_WEN/ENET_TX_EN/ADC0_3}
  - {pin_num: '66', peripheral: FLEXCOMM6, signal: RXD_SDA_MOSI_DATA, pin_signal: PIO1_13/ENET_RXD1/FC6_RXD_SDA_MOSI_DATA/CTIMER1_CAP2/USB0_OVERCURRENTN/USB0_FRAME/EMC_DQM(0)}
  - {pin_num: '62', peripheral: FLEXCOMM6, signal: SCK, pin_signal: PIO1_12/ENET_RXD0/FC6_SCK/CTIMER1_MAT1/USB0_PORTPWRN/EMC_DYCSN(0)}
  - {pin_num: '37', peripheral: FLEXCOMM7, signal: CTS_SDA_SSEL0, pin_signal: PIO1_21/FC7_CTS_SDA_SSEL0/CTIMER3_MAT2/FC4_RXD_SDA_MOSI/EMC_D(10)}
  - {pin_num: '74', peripheral: FLEXCOMM7, signal: RXD_SDA_MOSI_DATA, pin_signal: PIO0_20/FC3_CTS_SDA_SSEL0/CTIMER1_MAT1/CTIMER3_CAP3/SCT0_GPI2/SCI0_IO/EMC_A(2)/FC7_RXD_SDA_MOSI_DATA}
  - {pin_num: '73', peripheral: FLEXCOMM7, signal: SCK, pin_signal: PIO1_28/FC7_SCK/SD_D(5)/CTIMER0_CAP2/EMC_D(12)}
  - {pin_num: '47', peripheral: FLEXCOMM8, signal: RXD_SDA_MOSI, pin_signal: PIO1_17/ENET_MDIO/FC8_RXD_SDA_MOSI/SCT0_OUT4/CAN1_TD/EMC_BLSN(0)}
  - {pin_num: '16', peripheral: FLEXCOMM8, signal: SCK, pin_signal: PIO1_19/FC8_SCK/SCT0_OUT7/CTIMER3_MAT1/SCT0_GPI7/FC4_SCK/EMC_D(8)}
  - {pin_num: '92', peripheral: GPIO, signal: 'PIO1, 31', pin_signal: PIO1_31/MCLK/CTIMER0_MAT2/SCT0_OUT6/FC8_CTS_SDA_SSEL0/EMC_D(15)}
  - {pin_num: '3', peripheral: GPIO, signal: 'PIO1, 4', pin_signal: PIO1_4/FC0_SCK/SD_D(0)/CTIMER2_MAT1/SCT0_OUT0/FREQME_GPIO_CLK_A/EMC_D(11)}
  - {pin_num: '4', peripheral: GPIO, signal: 'PIO1, 5', pin_signal: PIO1_5/FC0_RXD_SDA_MOSI/SD_D(2)/CTIMER2_MAT0/SCT0_GPI0/EMC_A(4)}
  - {pin_num: '41', peripheral: FLEXCOMM1, signal: RXD_SDA_MOSI, pin_signal: PIO1_10/ENET_TXD1/FC1_RXD_SDA_MOSI/CTIMER1_MAT0/SCT0_OUT3/EMC_RASN}
  - {pin_num: '23', peripheral: FLEXCOMM1, signal: TXD_SCL_MISO, pin_signal: PIO0_10/FC6_SCK/CTIMER2_CAP2/CTIMER2_MAT0/FC1_TXD_SCL_MISO/SWO/ADC0_0}
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
    /* Enables the clock for the IOCON block. 0 = Disable; 1 = Enable.: 0x01u */
    CLOCK_EnableClock(kCLOCK_Iocon);

    IOCON->PIO[0][0] = ((IOCON->PIO[0][0] &
                         /* Mask bits to zero which are setting */
                         (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                        /* Selects pin function.: PORT00 (pin 93) is configured as FC3_SCK */
                        | IOCON_PIO_FUNC(PIO00_FUNC_ALT2)

                        /* Select Analog/Digital mode.: Digital mode. */
                        | IOCON_PIO_DIGIMODE(PIO00_DIGIMODE_DIGITAL));

    IOCON->PIO[0][1] = ((IOCON->PIO[0][1] &
                         /* Mask bits to zero which are setting */
                         (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                        /* Selects pin function.: PORT01 (pin 100) is configured as FC3_CTS_SDA_SSEL0 */
                        | IOCON_PIO_FUNC(PIO01_FUNC_ALT2)

                        /* Select Analog/Digital mode.: Digital mode. */
                        | IOCON_PIO_DIGIMODE(PIO01_DIGIMODE_DIGITAL));

    IOCON->PIO[0][10] = ((IOCON->PIO[0][10] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.: PORT010 (pin 23) is configured as FC1_TXD_SCL_MISO */
                         | IOCON_PIO_FUNC(PIO010_FUNC_ALT4)

                         /* Select Analog/Digital mode.: Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO010_DIGIMODE_DIGITAL));

    IOCON->PIO[0][11] = ((IOCON->PIO[0][11] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.: PORT011 (pin 24) is configured as SWCLK */
                         | IOCON_PIO_FUNC(PIO011_FUNC_ALT6)

                         /* Select Analog/Digital mode.: Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO011_DIGIMODE_DIGITAL));

    IOCON->PIO[0][12] = ((IOCON->PIO[0][12] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.: PORT012 (pin 25) is configured as SWDIO */
                         | IOCON_PIO_FUNC(PIO012_FUNC_ALT6)

                         /* Select Analog/Digital mode.: Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO012_DIGIMODE_DIGITAL));

    IOCON->PIO[0][15] = ((IOCON->PIO[0][15] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.: PORT015 (pin 26) is configured as FC6_CTS_SDA_SSEL0 */
                         | IOCON_PIO_FUNC(PIO015_FUNC_ALT1)

                         /* Select Analog/Digital mode.: Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO015_DIGIMODE_DIGITAL));

    IOCON->PIO[0][2] = ((IOCON->PIO[0][2] &
                         /* Mask bits to zero which are setting */
                         (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                        /* Selects pin function.: PORT02 (pin 83) is configured as FC3_TXD_SCL_MISO */
                        | IOCON_PIO_FUNC(PIO02_FUNC_ALT1)

                        /* Select Analog/Digital mode.: Digital mode. */
                        | IOCON_PIO_DIGIMODE(PIO02_DIGIMODE_DIGITAL));

    IOCON->PIO[0][20] = ((IOCON->PIO[0][20] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.: PORT020 (pin 74) is configured as FC7_RXD_SDA_MOSI_DATA */
                         | IOCON_PIO_FUNC(PIO020_FUNC_ALT7)

                         /* Select Analog/Digital mode.: Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO020_DIGIMODE_DIGITAL));

    IOCON->PIO[0][21] = ((IOCON->PIO[0][21] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.: PORT021 (pin 77) is configured as FC3_RTS_SCL_SSEL1 */
                         | IOCON_PIO_FUNC(PIO021_FUNC_ALT1)

                         /* Select Analog/Digital mode.: Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO021_DIGIMODE_DIGITAL));

    IOCON->PIO[0][26] = ((IOCON->PIO[0][26] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.: PORT026 (pin 56) is configured as FC2_RXD_SDA_MOSI */
                         | IOCON_PIO_FUNC(PIO026_FUNC_ALT1)

                         /* Select Analog/Digital mode.: Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO026_DIGIMODE_DIGITAL));

    IOCON->PIO[0][27] = ((IOCON->PIO[0][27] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.: PORT027 (pin 42) is configured as FC2_TXD_SCL_MISO */
                         | IOCON_PIO_FUNC(PIO027_FUNC_ALT1)

                         /* Select Analog/Digital mode.: Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO027_DIGIMODE_DIGITAL));

    IOCON->PIO[0][3] = ((IOCON->PIO[0][3] &
                         /* Mask bits to zero which are setting */
                         (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                        /* Selects pin function.: PORT03 (pin 85) is configured as FC3_RXD_SDA_MOSI */
                        | IOCON_PIO_FUNC(PIO03_FUNC_ALT1)

                        /* Select Analog/Digital mode.: Digital mode. */
                        | IOCON_PIO_DIGIMODE(PIO03_DIGIMODE_DIGITAL));

    IOCON->PIO[0][7] = ((IOCON->PIO[0][7] &
                         /* Mask bits to zero which are setting */
                         (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                        /* Selects pin function.: PORT07 (pin 61) is configured as FC5_SCK */
                        | IOCON_PIO_FUNC(PIO07_FUNC_ALT3)

                        /* Select Analog/Digital mode.: Digital mode. */
                        | IOCON_PIO_DIGIMODE(PIO07_DIGIMODE_DIGITAL));

    IOCON->PIO[0][8] = ((IOCON->PIO[0][8] &
                         /* Mask bits to zero which are setting */
                         (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                        /* Selects pin function.: PORT08 (pin 64) is configured as FC5_RXD_SDA_MOSI */
                        | IOCON_PIO_FUNC(PIO08_FUNC_ALT3)

                        /* Select Analog/Digital mode.: Digital mode. */
                        | IOCON_PIO_DIGIMODE(PIO08_DIGIMODE_DIGITAL));

    IOCON->PIO[0][9] = ((IOCON->PIO[0][9] &
                         /* Mask bits to zero which are setting */
                         (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                        /* Selects pin function.: PORT09 (pin 65) is configured as FC5_TXD_SCL_MISO */
                        | IOCON_PIO_FUNC(PIO09_FUNC_ALT3)

                        /* Select Analog/Digital mode.: Digital mode. */
                        | IOCON_PIO_DIGIMODE(PIO09_DIGIMODE_DIGITAL));

    IOCON->PIO[1][10] = ((IOCON->PIO[1][10] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.: PORT110 (pin 41) is configured as FC1_RXD_SDA_MOSI */
                         | IOCON_PIO_FUNC(PIO110_FUNC_ALT2)

                         /* Select Analog/Digital mode.: Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO110_DIGIMODE_DIGITAL));

    IOCON->PIO[1][12] = ((IOCON->PIO[1][12] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.: PORT112 (pin 62) is configured as FC6_SCK */
                         | IOCON_PIO_FUNC(PIO112_FUNC_ALT2)

                         /* Select Analog/Digital mode.: Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO112_DIGIMODE_DIGITAL));

    IOCON->PIO[1][13] = ((IOCON->PIO[1][13] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.: PORT113 (pin 66) is configured as FC6_RXD_SDA_MOSI_DATA */
                         | IOCON_PIO_FUNC(PIO113_FUNC_ALT2)

                         /* Select Analog/Digital mode.: Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO113_DIGIMODE_DIGITAL));

    IOCON->PIO[1][14] = ((IOCON->PIO[1][14] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.: PORT114 (pin 78) is configured as FC5_CTS_SDA_SSEL0 */
                         | IOCON_PIO_FUNC(PIO114_FUNC_ALT4)

                         /* Select Analog/Digital mode.: Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO114_DIGIMODE_DIGITAL));

    IOCON->PIO[1][15] = ((IOCON->PIO[1][15] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.: PORT115 (pin 84) is configured as FC5_RTS_SCL_SSEL1 */
                         | IOCON_PIO_FUNC(PIO115_FUNC_ALT4)

                         /* Select Analog/Digital mode.: Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO115_DIGIMODE_DIGITAL));

    IOCON->PIO[1][17] = ((IOCON->PIO[1][17] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.: PORT117 (pin 47) is configured as FC8_RXD_SDA_MOSI */
                         | IOCON_PIO_FUNC(PIO117_FUNC_ALT2)

                         /* Select Analog/Digital mode.: Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO117_DIGIMODE_DIGITAL));

    IOCON->PIO[1][19] = ((IOCON->PIO[1][19] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.: PORT119 (pin 16) is configured as FC8_SCK */
                         | IOCON_PIO_FUNC(PIO119_FUNC_ALT1)

                         /* Select Analog/Digital mode.: Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO119_DIGIMODE_DIGITAL));

    IOCON->PIO[1][21] = ((IOCON->PIO[1][21] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.: PORT121 (pin 37) is configured as FC7_CTS_SDA_SSEL0 */
                         | IOCON_PIO_FUNC(PIO121_FUNC_ALT1)

                         /* Select Analog/Digital mode.: Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO121_DIGIMODE_DIGITAL));

    IOCON->PIO[1][23] = ((IOCON->PIO[1][23] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.: PORT123 (pin 46) is configured as FC2_SCK */
                         | IOCON_PIO_FUNC(PIO123_FUNC_ALT1)

                         /* Select Analog/Digital mode.: Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO123_DIGIMODE_DIGITAL));

    IOCON->PIO[1][26] = ((IOCON->PIO[1][26] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.: PORT126 (pin 63) is configured as FC2_CTS_SDA_SSEL0 */
                         | IOCON_PIO_FUNC(PIO126_FUNC_ALT1)

                         /* Select Analog/Digital mode.: Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO126_DIGIMODE_DIGITAL));

    IOCON->PIO[1][27] = ((IOCON->PIO[1][27] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.: PORT127 (pin 68) is configured as FC2_RTS_SCL_SSEL1 */
                         | IOCON_PIO_FUNC(PIO127_FUNC_ALT1)

                         /* Select Analog/Digital mode.: Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO127_DIGIMODE_DIGITAL));

    IOCON->PIO[1][28] = ((IOCON->PIO[1][28] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.: PORT128 (pin 73) is configured as FC7_SCK */
                         | IOCON_PIO_FUNC(PIO128_FUNC_ALT1)

                         /* Select Analog/Digital mode.: Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO128_DIGIMODE_DIGITAL));

    IOCON->PIO[1][31] = ((IOCON->PIO[1][31] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.: PORT131 (pin 92) is configured as PIO1_31 */
                         | IOCON_PIO_FUNC(PIO131_FUNC_ALT0)

                         /* Select Analog/Digital mode.: Digital mode. */
                         | IOCON_PIO_DIGIMODE(PIO131_DIGIMODE_DIGITAL));

    IOCON->PIO[1][4] = ((IOCON->PIO[1][4] &
                         /* Mask bits to zero which are setting */
                         (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                        /* Selects pin function.: PORT14 (pin 3) is configured as PIO1_4 */
                        | IOCON_PIO_FUNC(PIO14_FUNC_ALT0)

                        /* Select Analog/Digital mode.: Digital mode. */
                        | IOCON_PIO_DIGIMODE(PIO14_DIGIMODE_DIGITAL));

    IOCON->PIO[1][5] = ((IOCON->PIO[1][5] &
                         /* Mask bits to zero which are setting */
                         (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                        /* Selects pin function.: PORT15 (pin 4) is configured as PIO1_5 */
                        | IOCON_PIO_FUNC(PIO15_FUNC_ALT0)

                        /* Select Analog/Digital mode.: Digital mode. */
                        | IOCON_PIO_DIGIMODE(PIO15_DIGIMODE_DIGITAL));
}
/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
