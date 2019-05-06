/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

/*! @brief Direction type  */
typedef enum _pin_mux_direction
{
    kPIN_MUX_DirectionInput = 0U,        /* Input direction */
    kPIN_MUX_DirectionOutput = 1U,       /* Output direction */
    kPIN_MUX_DirectionInputOrOutput = 2U /* Input or output direction */
} pin_mux_direction_t;

/*!
 * @addtogroup pin_mux
 * @{
 */

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);

/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO00_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 2. */
#define PIO00_FUNC_ALT2 0x02u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO011_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 6. */
#define PIO011_FUNC_ALT6 0x06u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO012_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 6. */
#define PIO012_FUNC_ALT6 0x06u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO013_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO013_FUNC_ALT0 0x00u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO018_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO018_FUNC_ALT0 0x00u
/*!
 * @brief
 * Selects function mode (on-chip pull-up/pull-down resistor control).
 * : Inactive.
 * Inactive (no pull-down/pull-up resistor enabled).
 */
#define PIO018_MODE_INACTIVE 0x00u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO019_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 1. */
#define PIO019_FUNC_ALT1 0x01u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO01_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 2. */
#define PIO01_FUNC_ALT2 0x02u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO024_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 1. */
#define PIO024_FUNC_ALT1 0x01u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO026_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 1. */
#define PIO026_FUNC_ALT1 0x01u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO027_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 1. */
#define PIO027_FUNC_ALT1 0x01u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO028_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 1. */
#define PIO028_FUNC_ALT1 0x01u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO029_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO029_FUNC_ALT0 0x00u
/*!
 * @brief Selects function mode (on-chip pull-up/pull-down resistor control).: Pull-up. Pull-up resistor enabled. */
#define PIO029_MODE_PULL_UP 0x02u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO02_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 1. */
#define PIO02_FUNC_ALT1 0x01u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO031_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO031_FUNC_ALT0 0x00u
/*!
 * @brief
 * Selects function mode (on-chip pull-up/pull-down resistor control).
 * : Inactive.
 * Inactive (no pull-down/pull-up resistor enabled).
 */
#define PIO031_MODE_INACTIVE 0x00u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO03_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 1. */
#define PIO03_FUNC_ALT1 0x01u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO04_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 2. */
#define PIO04_FUNC_ALT2 0x02u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO05_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 2. */
#define PIO05_FUNC_ALT2 0x02u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO08_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 3. */
#define PIO08_FUNC_ALT3 0x03u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO09_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 3. */
#define PIO09_FUNC_ALT3 0x03u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO10_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO10_FUNC_ALT0 0x00u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO111_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO111_FUNC_ALT0 0x00u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO116_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO116_FUNC_ALT0 0x00u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO117_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO117_FUNC_ALT0 0x00u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO118_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO118_FUNC_ALT0 0x00u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO119_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO119_FUNC_ALT0 0x00u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO122_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO122_FUNC_ALT0 0x00u
/*!
 * @brief Selects function mode (on-chip pull-up/pull-down resistor control).: Pull-up. Pull-up resistor enabled. */
#define PIO122_MODE_PULL_UP 0x02u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO123_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 1. */
#define PIO123_FUNC_ALT1 0x01u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO124_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO124_FUNC_ALT0 0x00u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO126_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO126_FUNC_ALT0 0x00u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO129_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 1. */
#define PIO129_FUNC_ALT1 0x01u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO130_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 1. */
#define PIO130_FUNC_ALT1 0x01u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO131_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO131_FUNC_ALT0 0x00u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO14_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO14_FUNC_ALT0 0x00u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO15_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO15_FUNC_ALT0 0x00u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO18_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO18_FUNC_ALT0 0x00u

/*! @name PIO1_31 (number 92), SELD
  @{ */
#define BOARD_INITPINS_SELD_PERIPHERAL GPIO                    /*!<@brief Device name: GPIO */
#define BOARD_INITPINS_SELD_SIGNAL PIO1                        /*!<@brief GPIO signal: PIO1 */
#define BOARD_INITPINS_SELD_GPIO GPIO                          /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_SELD_GPIO_PIN 31U                       /*!<@brief PIO1 pin index: 31 */
#define BOARD_INITPINS_SELD_PORT 1U                            /*!<@brief PORT device name: 1U */
#define BOARD_INITPINS_SELD_PIN 31U                            /*!<@brief 1U pin index: 31 */
#define BOARD_INITPINS_SELD_CHANNEL 31                         /*!<@brief GPIO PIO1 channel: 31 */
#define BOARD_INITPINS_SELD_PIN_NAME PIO1_31                   /*!<@brief Pin name */
#define BOARD_INITPINS_SELD_LABEL "SELD"                       /*!<@brief Label */
#define BOARD_INITPINS_SELD_NAME "SELD"                        /*!<@brief Identifier name */
#define BOARD_INITPINS_SELD_DIRECTION kPIN_MUX_DirectionOutput /*!<@brief Direction */
                                                               /* @} */

/*! @name PIO1_4 (number 3), SELF
  @{ */
#define BOARD_INITPINS_SELF_PERIPHERAL GPIO                    /*!<@brief Device name: GPIO */
#define BOARD_INITPINS_SELF_SIGNAL PIO1                        /*!<@brief GPIO signal: PIO1 */
#define BOARD_INITPINS_SELF_GPIO GPIO                          /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_SELF_GPIO_PIN 4U                        /*!<@brief PIO1 pin index: 4 */
#define BOARD_INITPINS_SELF_PORT 1U                            /*!<@brief PORT device name: 1U */
#define BOARD_INITPINS_SELF_PIN 4U                             /*!<@brief 1U pin index: 4 */
#define BOARD_INITPINS_SELF_CHANNEL 4                          /*!<@brief GPIO PIO1 channel: 4 */
#define BOARD_INITPINS_SELF_PIN_NAME PIO1_4                    /*!<@brief Pin name */
#define BOARD_INITPINS_SELF_LABEL "SELF"                       /*!<@brief Label */
#define BOARD_INITPINS_SELF_NAME "SELF"                        /*!<@brief Identifier name */
#define BOARD_INITPINS_SELF_DIRECTION kPIN_MUX_DirectionOutput /*!<@brief Direction */
                                                               /* @} */

/*! @name PIO1_5 (number 4), SELE
  @{ */
#define BOARD_INITPINS_SELE_PERIPHERAL GPIO                    /*!<@brief Device name: GPIO */
#define BOARD_INITPINS_SELE_SIGNAL PIO1                        /*!<@brief GPIO signal: PIO1 */
#define BOARD_INITPINS_SELE_GPIO GPIO                          /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_SELE_GPIO_PIN 5U                        /*!<@brief PIO1 pin index: 5 */
#define BOARD_INITPINS_SELE_PORT 1U                            /*!<@brief PORT device name: 1U */
#define BOARD_INITPINS_SELE_PIN 5U                             /*!<@brief 1U pin index: 5 */
#define BOARD_INITPINS_SELE_CHANNEL 5                          /*!<@brief GPIO PIO1 channel: 5 */
#define BOARD_INITPINS_SELE_PIN_NAME PIO1_5                    /*!<@brief Pin name */
#define BOARD_INITPINS_SELE_LABEL "SELE"                       /*!<@brief Label */
#define BOARD_INITPINS_SELE_NAME "SELE"                        /*!<@brief Identifier name */
#define BOARD_INITPINS_SELE_DIRECTION kPIN_MUX_DirectionOutput /*!<@brief Direction */
                                                               /* @} */

/*! @name PIO1_18 (number 5), SELC
  @{ */
#define BOARD_INITPINS_SELC_PERIPHERAL GPIO                    /*!<@brief Device name: GPIO */
#define BOARD_INITPINS_SELC_SIGNAL PIO1                        /*!<@brief GPIO signal: PIO1 */
#define BOARD_INITPINS_SELC_GPIO GPIO                          /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_SELC_GPIO_PIN 18U                       /*!<@brief PIO1 pin index: 18 */
#define BOARD_INITPINS_SELC_PORT 1U                            /*!<@brief PORT device name: 1U */
#define BOARD_INITPINS_SELC_PIN 18U                            /*!<@brief 1U pin index: 18 */
#define BOARD_INITPINS_SELC_CHANNEL 18                         /*!<@brief GPIO PIO1 channel: 18 */
#define BOARD_INITPINS_SELC_PIN_NAME PIO1_18                   /*!<@brief Pin name */
#define BOARD_INITPINS_SELC_LABEL "SELC"                       /*!<@brief Label */
#define BOARD_INITPINS_SELC_NAME "SELC"                        /*!<@brief Identifier name */
#define BOARD_INITPINS_SELC_DIRECTION kPIN_MUX_DirectionOutput /*!<@brief Direction */
                                                               /* @} */

/*! @name PIO1_19 (number 16), SELA
  @{ */
#define BOARD_INITPINS_SELA_PERIPHERAL GPIO                    /*!<@brief Device name: GPIO */
#define BOARD_INITPINS_SELA_SIGNAL PIO1                        /*!<@brief GPIO signal: PIO1 */
#define BOARD_INITPINS_SELA_GPIO GPIO                          /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_SELA_GPIO_PIN 19U                       /*!<@brief PIO1 pin index: 19 */
#define BOARD_INITPINS_SELA_PORT 1U                            /*!<@brief PORT device name: 1U */
#define BOARD_INITPINS_SELA_PIN 19U                            /*!<@brief 1U pin index: 19 */
#define BOARD_INITPINS_SELA_CHANNEL 19                         /*!<@brief GPIO PIO1 channel: 19 */
#define BOARD_INITPINS_SELA_PIN_NAME PIO1_19                   /*!<@brief Pin name */
#define BOARD_INITPINS_SELA_LABEL "SELA"                       /*!<@brief Label */
#define BOARD_INITPINS_SELA_NAME "SELA"                        /*!<@brief Identifier name */
#define BOARD_INITPINS_SELA_DIRECTION kPIN_MUX_DirectionOutput /*!<@brief Direction */
                                                               /* @} */

/*! @name PIO1_0 (number 29), GATE
  @{ */
#define BOARD_INITPINS_GATE_PERIPHERAL GPIO                    /*!<@brief Device name: GPIO */
#define BOARD_INITPINS_GATE_SIGNAL PIO1                        /*!<@brief GPIO signal: PIO1 */
#define BOARD_INITPINS_GATE_GPIO GPIO                          /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_GATE_GPIO_PIN 0U                        /*!<@brief PIO1 pin index: 0 */
#define BOARD_INITPINS_GATE_PORT 1U                            /*!<@brief PORT device name: 1U */
#define BOARD_INITPINS_GATE_PIN 0U                             /*!<@brief 1U pin index: 0 */
#define BOARD_INITPINS_GATE_CHANNEL 0                          /*!<@brief GPIO PIO1 channel: 0 */
#define BOARD_INITPINS_GATE_PIN_NAME PIO1_0                    /*!<@brief Pin name */
#define BOARD_INITPINS_GATE_LABEL "GATE"                       /*!<@brief Label */
#define BOARD_INITPINS_GATE_NAME "GATE"                        /*!<@brief Identifier name */
#define BOARD_INITPINS_GATE_DIRECTION kPIN_MUX_DirectionOutput /*!<@brief Direction */
                                                               /* @} */

/*! @name PIO0_31 (number 28), TUNE
  @{ */
#define BOARD_INITPINS_TUNE_PERIPHERAL GPIO                   /*!<@brief Device name: GPIO */
#define BOARD_INITPINS_TUNE_SIGNAL PIO0                       /*!<@brief GPIO signal: PIO0 */
#define BOARD_INITPINS_TUNE_GPIO GPIO                         /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_TUNE_GPIO_PIN 31U                      /*!<@brief PIO0 pin index: 31 */
#define BOARD_INITPINS_TUNE_PORT 0U                           /*!<@brief PORT device name: 0U */
#define BOARD_INITPINS_TUNE_PIN 31U                           /*!<@brief 0U pin index: 31 */
#define BOARD_INITPINS_TUNE_CHANNEL 31                        /*!<@brief GPIO PIO0 channel: 31 */
#define BOARD_INITPINS_TUNE_PIN_NAME PIO0_31                  /*!<@brief Pin name */
#define BOARD_INITPINS_TUNE_LABEL "TUNE"                      /*!<@brief Label */
#define BOARD_INITPINS_TUNE_NAME "TUNE"                       /*!<@brief Identifier name */
#define BOARD_INITPINS_TUNE_DIRECTION kPIN_MUX_DirectionInput /*!<@brief Direction */
                                                              /* @} */

/*! @name PIO1_8 (number 36), SLATCH0
  @{ */
#define BOARD_INITPINS_SLATCH0_PERIPHERAL GPIO                    /*!<@brief Device name: GPIO */
#define BOARD_INITPINS_SLATCH0_SIGNAL PIO1                        /*!<@brief GPIO signal: PIO1 */
#define BOARD_INITPINS_SLATCH0_GPIO GPIO                          /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_SLATCH0_GPIO_PIN 8U                        /*!<@brief PIO1 pin index: 8 */
#define BOARD_INITPINS_SLATCH0_PORT 1U                            /*!<@brief PORT device name: 1U */
#define BOARD_INITPINS_SLATCH0_PIN 8U                             /*!<@brief 1U pin index: 8 */
#define BOARD_INITPINS_SLATCH0_CHANNEL 8                          /*!<@brief GPIO PIO1 channel: 8 */
#define BOARD_INITPINS_SLATCH0_PIN_NAME PIO1_8                    /*!<@brief Pin name */
#define BOARD_INITPINS_SLATCH0_LABEL "SLATCH0"                    /*!<@brief Label */
#define BOARD_INITPINS_SLATCH0_NAME "SLATCH0"                     /*!<@brief Identifier name */
#define BOARD_INITPINS_SLATCH0_DIRECTION kPIN_MUX_DirectionOutput /*!<@brief Direction */
                                                                  /* @} */

/*! @name PIO1_22 (number 43), FC2INT
  @{ */
#define BOARD_INITPINS_FC2_INT_PERIPHERAL GPIO                   /*!<@brief Device name: GPIO */
#define BOARD_INITPINS_FC2_INT_SIGNAL PIO1                       /*!<@brief GPIO signal: PIO1 */
#define BOARD_INITPINS_FC2_INT_GPIO GPIO                         /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_FC2_INT_GPIO_PIN 22U                      /*!<@brief PIO1 pin index: 22 */
#define BOARD_INITPINS_FC2_INT_PORT 1U                           /*!<@brief PORT device name: 1U */
#define BOARD_INITPINS_FC2_INT_PIN 22U                           /*!<@brief 1U pin index: 22 */
#define BOARD_INITPINS_FC2_INT_CHANNEL 22                        /*!<@brief GPIO PIO1 channel: 22 */
#define BOARD_INITPINS_FC2_INT_PIN_NAME PIO1_22                  /*!<@brief Pin name */
#define BOARD_INITPINS_FC2_INT_LABEL "FC2INT"                    /*!<@brief Label */
#define BOARD_INITPINS_FC2_INT_NAME "FC2_INT"                    /*!<@brief Identifier name */
#define BOARD_INITPINS_FC2_INT_DIRECTION kPIN_MUX_DirectionInput /*!<@brief Direction */
                                                                 /* @} */

/*! @name PIO1_17 (number 47), SELB
  @{ */
#define BOARD_INITPINS_SELB_PERIPHERAL GPIO                    /*!<@brief Device name: GPIO */
#define BOARD_INITPINS_SELB_SIGNAL PIO1                        /*!<@brief GPIO signal: PIO1 */
#define BOARD_INITPINS_SELB_GPIO GPIO                          /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_SELB_GPIO_PIN 17U                       /*!<@brief PIO1 pin index: 17 */
#define BOARD_INITPINS_SELB_PORT 1U                            /*!<@brief PORT device name: 1U */
#define BOARD_INITPINS_SELB_PIN 17U                            /*!<@brief 1U pin index: 17 */
#define BOARD_INITPINS_SELB_CHANNEL 17                         /*!<@brief GPIO PIO1 channel: 17 */
#define BOARD_INITPINS_SELB_PIN_NAME PIO1_17                   /*!<@brief Pin name */
#define BOARD_INITPINS_SELB_LABEL "SELB"                       /*!<@brief Label */
#define BOARD_INITPINS_SELB_NAME "SELB"                        /*!<@brief Identifier name */
#define BOARD_INITPINS_SELB_DIRECTION kPIN_MUX_DirectionOutput /*!<@brief Direction */
                                                               /* @} */

/*! @name PIO1_24 (number 57), SRESET2
  @{ */
#define BOARD_INITPINS_SRESET2_PERIPHERAL GPIO                    /*!<@brief Device name: GPIO */
#define BOARD_INITPINS_SRESET2_SIGNAL PIO1                        /*!<@brief GPIO signal: PIO1 */
#define BOARD_INITPINS_SRESET2_GPIO GPIO                          /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_SRESET2_GPIO_PIN 24U                       /*!<@brief PIO1 pin index: 24 */
#define BOARD_INITPINS_SRESET2_PORT 1U                            /*!<@brief PORT device name: 1U */
#define BOARD_INITPINS_SRESET2_PIN 24U                            /*!<@brief 1U pin index: 24 */
#define BOARD_INITPINS_SRESET2_CHANNEL 24                         /*!<@brief GPIO PIO1 channel: 24 */
#define BOARD_INITPINS_SRESET2_PIN_NAME PIO1_24                   /*!<@brief Pin name */
#define BOARD_INITPINS_SRESET2_LABEL "SRESET2"                    /*!<@brief Label */
#define BOARD_INITPINS_SRESET2_NAME "SRESET2"                     /*!<@brief Identifier name */
#define BOARD_INITPINS_SRESET2_DIRECTION kPIN_MUX_DirectionOutput /*!<@brief Direction */
                                                                  /* @} */

/*! @name PIO1_26 (number 63), SLATCH2
  @{ */
#define BOARD_INITPINS_SLATCH2_PERIPHERAL GPIO                    /*!<@brief Device name: GPIO */
#define BOARD_INITPINS_SLATCH2_SIGNAL PIO1                        /*!<@brief GPIO signal: PIO1 */
#define BOARD_INITPINS_SLATCH2_GPIO GPIO                          /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_SLATCH2_GPIO_PIN 26U                       /*!<@brief PIO1 pin index: 26 */
#define BOARD_INITPINS_SLATCH2_PORT 1U                            /*!<@brief PORT device name: 1U */
#define BOARD_INITPINS_SLATCH2_PIN 26U                            /*!<@brief 1U pin index: 26 */
#define BOARD_INITPINS_SLATCH2_CHANNEL 26                         /*!<@brief GPIO PIO1 channel: 26 */
#define BOARD_INITPINS_SLATCH2_PIN_NAME PIO1_26                   /*!<@brief Pin name */
#define BOARD_INITPINS_SLATCH2_LABEL "SLATCH2"                    /*!<@brief Label */
#define BOARD_INITPINS_SLATCH2_NAME "SLATCH2"                     /*!<@brief Identifier name */
#define BOARD_INITPINS_SLATCH2_DIRECTION kPIN_MUX_DirectionOutput /*!<@brief Direction */
                                                                  /* @} */

/*! @name PIO0_13 (number 67), SLATCH1
  @{ */
#define BOARD_INITPINS_SLATCH1_PERIPHERAL GPIO                    /*!<@brief Device name: GPIO */
#define BOARD_INITPINS_SLATCH1_SIGNAL PIO0                        /*!<@brief GPIO signal: PIO0 */
#define BOARD_INITPINS_SLATCH1_GPIO GPIO                          /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_SLATCH1_GPIO_PIN 13U                       /*!<@brief PIO0 pin index: 13 */
#define BOARD_INITPINS_SLATCH1_PORT 0U                            /*!<@brief PORT device name: 0U */
#define BOARD_INITPINS_SLATCH1_PIN 13U                            /*!<@brief 0U pin index: 13 */
#define BOARD_INITPINS_SLATCH1_CHANNEL 13                         /*!<@brief GPIO PIO0 channel: 13 */
#define BOARD_INITPINS_SLATCH1_PIN_NAME PIO0_13                   /*!<@brief Pin name */
#define BOARD_INITPINS_SLATCH1_LABEL "SLATCH1"                    /*!<@brief Label */
#define BOARD_INITPINS_SLATCH1_NAME "SLATCH1"                     /*!<@brief Identifier name */
#define BOARD_INITPINS_SLATCH1_DIRECTION kPIN_MUX_DirectionOutput /*!<@brief Direction */
                                                                  /* @} */

/*! @name PIO0_18 (number 72), EIN_EXT
  @{ */
#define BOARD_INITPINS_EIN_EXT_PERIPHERAL GPIO                    /*!<@brief Device name: GPIO */
#define BOARD_INITPINS_EIN_EXT_SIGNAL PIO0                        /*!<@brief GPIO signal: PIO0 */
#define BOARD_INITPINS_EIN_EXT_GPIO GPIO                          /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_EIN_EXT_GPIO_PIN 18U                       /*!<@brief PIO0 pin index: 18 */
#define BOARD_INITPINS_EIN_EXT_PORT 0U                            /*!<@brief PORT device name: 0U */
#define BOARD_INITPINS_EIN_EXT_PIN 18U                            /*!<@brief 0U pin index: 18 */
#define BOARD_INITPINS_EIN_EXT_CHANNEL 18                         /*!<@brief GPIO PIO0 channel: 18 */
#define BOARD_INITPINS_EIN_EXT_PIN_NAME PIO0_18                   /*!<@brief Pin name */
#define BOARD_INITPINS_EIN_EXT_LABEL "EIN_EXT"                    /*!<@brief Label */
#define BOARD_INITPINS_EIN_EXT_NAME "EIN_EXT"                     /*!<@brief Identifier name */
#define BOARD_INITPINS_EIN_EXT_DIRECTION kPIN_MUX_DirectionOutput /*!<@brief Direction */
                                                                  /* @} */

/*! @name PIO0_29 (number 82), CODEC_INT
  @{ */
#define BOARD_INITPINS_FC3_INT_PERIPHERAL GPIO                   /*!<@brief Device name: GPIO */
#define BOARD_INITPINS_FC3_INT_SIGNAL PIO0                       /*!<@brief GPIO signal: PIO0 */
#define BOARD_INITPINS_FC3_INT_GPIO GPIO                         /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_FC3_INT_GPIO_PIN 29U                      /*!<@brief PIO0 pin index: 29 */
#define BOARD_INITPINS_FC3_INT_PORT 0U                           /*!<@brief PORT device name: 0U */
#define BOARD_INITPINS_FC3_INT_PIN 29U                           /*!<@brief 0U pin index: 29 */
#define BOARD_INITPINS_FC3_INT_CHANNEL 29                        /*!<@brief GPIO PIO0 channel: 29 */
#define BOARD_INITPINS_FC3_INT_PIN_NAME PIO0_29                  /*!<@brief Pin name */
#define BOARD_INITPINS_FC3_INT_LABEL "CODEC_INT"                 /*!<@brief Label */
#define BOARD_INITPINS_FC3_INT_NAME "FC3_INT"                    /*!<@brief Identifier name */
#define BOARD_INITPINS_FC3_INT_DIRECTION kPIN_MUX_DirectionInput /*!<@brief Direction */
                                                                 /* @} */

/*! @name PIO1_16 (number 88), LED
  @{ */
#define BOARD_INITPINS_LED_PERIPHERAL GPIO                    /*!<@brief Device name: GPIO */
#define BOARD_INITPINS_LED_SIGNAL PIO1                        /*!<@brief GPIO signal: PIO1 */
#define BOARD_INITPINS_LED_GPIO GPIO                          /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_LED_GPIO_PIN 16U                       /*!<@brief PIO1 pin index: 16 */
#define BOARD_INITPINS_LED_PORT 1U                            /*!<@brief PORT device name: 1U */
#define BOARD_INITPINS_LED_PIN 16U                            /*!<@brief 1U pin index: 16 */
#define BOARD_INITPINS_LED_CHANNEL 16                         /*!<@brief GPIO PIO1 channel: 16 */
#define BOARD_INITPINS_LED_PIN_NAME PIO1_16                   /*!<@brief Pin name */
#define BOARD_INITPINS_LED_LABEL "LED"                        /*!<@brief Label */
#define BOARD_INITPINS_LED_NAME "LED"                         /*!<@brief Identifier name */
#define BOARD_INITPINS_LED_DIRECTION kPIN_MUX_DirectionOutput /*!<@brief Direction */
                                                              /* @} */

/*! @name PIO1_11 (number 94), SELINHG
  @{ */
#define BOARD_INITPINS_SELINHG_PERIPHERAL GPIO                    /*!<@brief Device name: GPIO */
#define BOARD_INITPINS_SELINHG_SIGNAL PIO1                        /*!<@brief GPIO signal: PIO1 */
#define BOARD_INITPINS_SELINHG_GPIO GPIO                          /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_SELINHG_GPIO_PIN 11U                       /*!<@brief PIO1 pin index: 11 */
#define BOARD_INITPINS_SELINHG_PORT 1U                            /*!<@brief PORT device name: 1U */
#define BOARD_INITPINS_SELINHG_PIN 11U                            /*!<@brief 1U pin index: 11 */
#define BOARD_INITPINS_SELINHG_CHANNEL 11                         /*!<@brief GPIO PIO1 channel: 11 */
#define BOARD_INITPINS_SELINHG_PIN_NAME PIO1_11                   /*!<@brief Pin name */
#define BOARD_INITPINS_SELINHG_LABEL "SELINHG"                    /*!<@brief Label */
#define BOARD_INITPINS_SELINHG_NAME "SELINHG"                     /*!<@brief Identifier name */
#define BOARD_INITPINS_SELINHG_DIRECTION kPIN_MUX_DirectionOutput /*!<@brief Direction */
                                                                  /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void); /* Function assigned for the Cortex-M4F */

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
