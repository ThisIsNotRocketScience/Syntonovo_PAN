/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/
/*
 * How to set up clock using clock driver functions:
 *
 * 1. Setup clock sources.
 *
 * 2. Setup voltage for the fastest of the clock outputs
 *
 * 3. Set up wait states of the flash.
 *
 * 4. Set up all dividers.
 *
 * 5. Set up all selectors to provide selected clocks.
 */

/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Clocks v4.1
processor: LPC54616J512
package_id: LPC54616J512BD100
mcu_data: ksdk2_0
processor_version: 4.0.0
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

#include "fsl_power.h"
#include "fsl_clock.h"
#include "clock_config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* System clock frequency. */
extern uint32_t SystemCoreClock;

/*******************************************************************************
 ************************ BOARD_InitBootClocks function ************************
 ******************************************************************************/
void BOARD_InitBootClocks(void)
{
    BOARD_BootClockRUN();
}

/*******************************************************************************
 ********************** Configuration BOARD_BootClockRUN ***********************
 ******************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!Configuration
name: BOARD_BootClockRUN
called_from_default_init: true
outputs:
- {id: AUDPLL_clock.outFreq, value: 96 MHz}
- {id: FRO12M_clock.outFreq, value: 12 MHz}
- {id: FROHF_clock.outFreq, value: 48 MHz}
- {id: FXCOM0_clock.outFreq, value: 96 MHz}
- {id: FXCOM1_clock.outFreq, value: 96 MHz}
- {id: FXCOM2_clock.outFreq, value: 96 MHz}
- {id: FXCOM3_clock.outFreq, value: 96 MHz}
- {id: FXCOM4_clock.outFreq, value: 96 MHz}
- {id: FXCOM5_clock.outFreq, value: 96 MHz}
- {id: FXCOM6_clock.outFreq, value: 96 MHz}
- {id: FXCOM7_clock.outFreq, value: 96 MHz}
- {id: FXCOM8_clock.outFreq, value: 96 MHz}
- {id: MAIN_clock.outFreq, value: 180 MHz}
- {id: SCT_clock.outFreq, value: 180 MHz}
- {id: SYSPLL_clock.outFreq, value: 180 MHz}
- {id: SYSTICK_clock.outFreq, value: 11.25/2 MHz}
- {id: System_clock.outFreq, value: 180 MHz, locked: true, accuracy: '0.001'}
settings:
- {id: SYSCON.AHBCLKDIV.scale, value: '1', locked: true}
- {id: SYSCON.AUDPDEC.scale, value: '4', locked: true}
- {id: SYSCON.AUDPLLCLKSEL.sel, value: SYSCON._clk_in}
- {id: SYSCON.AUD_M_MULT.scale, value: '192', locked: true}
- {id: SYSCON.AUD_N_DIV.scale, value: '6', locked: true}
- {id: SYSCON.FRGCTRL_DIV.scale, value: '384'}
- {id: SYSCON.FXCLKSEL0.sel, value: SYSCON.AUDPLL_BYPASS}
- {id: SYSCON.FXCLKSEL1.sel, value: SYSCON.AUDPLL_BYPASS}
- {id: SYSCON.FXCLKSEL2.sel, value: SYSCON.AUDPLL_BYPASS}
- {id: SYSCON.FXCLKSEL3.sel, value: SYSCON.AUDPLL_BYPASS}
- {id: SYSCON.FXCLKSEL4.sel, value: SYSCON.AUDPLL_BYPASS}
- {id: SYSCON.FXCLKSEL5.sel, value: SYSCON.AUDPLL_BYPASS}
- {id: SYSCON.FXCLKSEL6.sel, value: SYSCON.AUDPLL_BYPASS}
- {id: SYSCON.FXCLKSEL7.sel, value: SYSCON.AUDPLL_BYPASS}
- {id: SYSCON.FXCLKSEL8.sel, value: SYSCON.AUDPLL_BYPASS}
- {id: SYSCON.MAINCLKSELB.sel, value: SYSCON.PLL_BYPASS}
- {id: SYSCON.M_MULT.scale, value: '30'}
- {id: SYSCON.N_DIV.scale, value: '1'}
- {id: SYSCON.PDEC.scale, value: '2'}
- {id: SYSCON.SC0CLKDIV.scale, value: '1', locked: true}
- {id: SYSCON.SCTCLKDIV.scale, value: '1', locked: true}
- {id: SYSCON.SCTCLKSEL.sel, value: SYSCON.MAINCLKSELB}
- {id: SYSCON.SYSPLLCLKSEL.sel, value: SYSCON._clk_in}
- {id: SYSCON.SYSTICKCLKDIV.scale, value: '32', locked: true}
- {id: SYSCON_PDRUNCFG0_PDEN_SYS_PLL_CFG, value: Power_up}
- {id: SYSCON_PDRUNCFG1_PDEN_AUD_PLL_CFG, value: Power_up}
- {id: SYSTICKCLK_OUTPUT_ENDI, value: Enabled}
sources:
- {id: SYSCON._clk_in.outFreq, value: 12 MHz, enabled: true}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/*******************************************************************************
 * Variables for BOARD_BootClockRUN configuration
 ******************************************************************************/
/*******************************************************************************
 * Code for BOARD_BootClockRUN configuration
 ******************************************************************************/
void BOARD_BootClockRUN(void)
{
    /*!< Set up the clock sources */
    /*!< Set up FRO */
    POWER_DisablePD(kPDRUNCFG_PD_FRO_EN);                   /*!< Ensure FRO is on  */
    CLOCK_AttachClk(kFRO12M_to_MAIN_CLK);                  /*!< Switch to FRO 12MHz first to ensure we can change voltage without accidentally
                                                                being below the voltage for current speed */
    POWER_DisablePD(kPDRUNCFG_PD_SYS_OSC);          /*!< Enable System Oscillator Power */
    SYSCON->SYSOSCCTRL = ((SYSCON->SYSOSCCTRL & ~SYSCON_SYSOSCCTRL_FREQRANGE_MASK) | SYSCON_SYSOSCCTRL_FREQRANGE(0U)); /*!< Set system oscillator range */
    POWER_SetVoltageForFreq(180000000U);             /*!< Set voltage for the one of the fastest clock outputs: System clock output */
    CLOCK_SetFLASHAccessCyclesForFreq(180000000U);    /*!< Set FLASH wait states for core */

    /*!< Set up SYS PLL */
    const pll_setup_t pllSetup = {
        .pllctrl =  SYSCON_SYSPLLCTRL_SELI(32U) | SYSCON_SYSPLLCTRL_SELP(16U) | SYSCON_SYSPLLCTRL_SELR(0U),
        .pllmdec = (SYSCON_SYSPLLMDEC_MDEC(8191U)),
        .pllndec = (SYSCON_SYSPLLNDEC_NDEC(770U)),
        .pllpdec = (SYSCON_SYSPLLPDEC_PDEC(98U)),
        .pllRate = 180000000U,
        .flags =  PLL_SETUPFLAG_WAITLOCK | PLL_SETUPFLAG_POWERUP
    };
    CLOCK_AttachClk(kEXT_CLK_to_SYS_PLL);        /*!< Set sys pll clock source*/
    CLOCK_SetPLLFreq(&pllSetup);                 /*!< Configure PLL to the desired value */
    /*!< Set up AUDIO PLL */
    const pll_setup_t audio_pllSetup = {
        .pllctrl = SYSCON_AUDPLLCTRL_SELI(20U) | SYSCON_AUDPLLCTRL_SELP(31U) | SYSCON_AUDPLLCTRL_SELR(0U),
        .pllmdec = (SYSCON_AUDPLLMDEC_MDEC(12733U)),
        .pllndec = (SYSCON_AUDPLLNDEC_NDEC(11U)),
        .pllpdec = (SYSCON_AUDPLLPDEC_PDEC(66U)),
        .pllRate = 96000000U,
        .flags =  PLL_SETUPFLAG_WAITLOCK | PLL_SETUPFLAG_POWERUP
    };
    CLOCK_AttachClk(kEXT_CLK_to_AUDIO_PLL);                         /*!< Set audio pll clock source*/
    CLOCK_SetAudioPLLFreq(&audio_pllSetup);                        /*!< Configure PLL to the desired value */


    /*!< Set up dividers */
    CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U, false);                  /*!< Reset divider counter and set divider to value 1 */
    CLOCK_SetClkDiv(kCLOCK_DivSystickClk, 0U, true);                  /*!< Reset SYSTICKCLKDIV divider counter and halt it */
    CLOCK_SetClkDiv(kCLOCK_DivSystickClk, 32U, false);                  /*!< Set SYSTICKCLKDIV divider to value 32 */
    CLOCK_SetClkDiv(kCLOCK_DivSctClk, 0U, true);                  /*!< Reset SCTCLKDIV divider counter and halt it */
    CLOCK_SetClkDiv(kCLOCK_DivSctClk, 1U, false);                  /*!< Set SCTCLKDIV divider to value 1 */

    /*!< Set up clock selectors - Attach clocks to the peripheries */
    CLOCK_AttachClk(kSYS_PLL_to_MAIN_CLK);                  /*!< Switch MAIN_CLK to SYS_PLL */
    CLOCK_AttachClk(kAUDIO_PLL_to_FLEXCOMM0);                  /*!< Switch FLEXCOMM0 to AUDIO_PLL */
    CLOCK_AttachClk(kAUDIO_PLL_to_FLEXCOMM1);                  /*!< Switch FLEXCOMM1 to AUDIO_PLL */
    CLOCK_AttachClk(kAUDIO_PLL_to_FLEXCOMM2);                  /*!< Switch FLEXCOMM2 to AUDIO_PLL */
    CLOCK_AttachClk(kAUDIO_PLL_to_FLEXCOMM3);                  /*!< Switch FLEXCOMM3 to AUDIO_PLL */
    CLOCK_AttachClk(kAUDIO_PLL_to_FLEXCOMM4);                  /*!< Switch FLEXCOMM4 to AUDIO_PLL */
    CLOCK_AttachClk(kAUDIO_PLL_to_FLEXCOMM5);                  /*!< Switch FLEXCOMM5 to AUDIO_PLL */
    CLOCK_AttachClk(kAUDIO_PLL_to_FLEXCOMM6);                  /*!< Switch FLEXCOMM6 to AUDIO_PLL */
    CLOCK_AttachClk(kAUDIO_PLL_to_FLEXCOMM7);                  /*!< Switch FLEXCOMM7 to AUDIO_PLL */
    CLOCK_AttachClk(kAUDIO_PLL_to_FLEXCOMM8);                  /*!< Switch FLEXCOMM8 to AUDIO_PLL */
    CLOCK_AttachClk(kMCLK_to_SCT_CLK);                  /*!< Switch SCT_CLK to MAIN_CLK */
    SYSCON->MAINCLKSELA = ((SYSCON->MAINCLKSELA & ~SYSCON_MAINCLKSELA_SEL_MASK) | SYSCON_MAINCLKSELA_SEL(0U)); /*!< Switch MAINCLKSELA to FRO12M even it is not used for MAINCLKSELB */
    /* Set SystemCoreClock variable. */
    SystemCoreClock = BOARD_BOOTCLOCKRUN_CORE_CLOCK;
}

