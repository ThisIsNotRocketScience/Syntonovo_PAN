#pragma once
#ifndef FINALPANENUMS
#define FINALPANENUMS

#include <stdint.h>

#define ParamBoxWidth 220

enum Screens_t
{
	SCREEN_HOME,
	
	SCREEN_ALLPARAMS,
	SCREEN_ALLSWITCHES,

	SCREEN_EDIT_PRESETNAME,
	SCREEN_OVERWRITE_YES_NO,
	SCREEN_PRESET_LIBRARY,

	SCREEN_BINDING_FROM_MOD_TO_TARGET,
	SCREEN_BINDING_FROM_TARGET_TO_MOD,

	SCREEN_KEYBOARD,
	SCREEN_TOUCH,
	SCREEN_X,
	SCREEN_Y,
	SCREEN_Z,
	SCREEN_LFO,
	SCREEN_ENVELOPE,

	SCREEN_VCO1,
	SCREEN_VCO2,
	SCREEN_VCO3,
	SCREEN_VCO4567,
	SCREEN_VCO8,
	SCREEN_MASTER,
	SCREEN_PRESET,
	SCREEN_LOGO,
	SCREEN_VCF1,
	SCREEN_VCF2a,
	SCREEN_VCF2b,
	SCREEN_VCF2c,
	SCREEN_VCF2d,

	SCREEN_ARP,
	SCREEN_MIXER,
	SCREEN_EFFECTS,

	SCREEN_TEST,
	SCREENS_COUNT
};

enum FinalLedEnum
{
#define LED(name,x,y,str)  led_##name,
#include "FinalPanHeader.h"

#undef LED
	__FINALLED_COUNT
};

enum FinalLedButtonEnum
{
#define LEDBUTTON(name,x,y,id,str,r,g,b)  ledbutton_##name,
#include "FinalPanHeader.h"

#undef LEDBUTTON
	__FINALLEDBUTTON_COUNT
};

enum FinalEncoderEnum
{
#define LEDENCODER(name,x,y,str)  encoder_##name,
#include "FinalPanHeader.h"
#undef LEDENCODER
	__FINALENCODER_COUNT
};

void LedEncoderButtonPress(FinalEncoderEnum Button);
void LedEncoderButtonLeft(FinalEncoderEnum Button);
void LedEncoderButtonRight(FinalEncoderEnum Button);
void LedButtonPressed(FinalLedButtonEnum button);

enum
{
	MenuAction_Save,
	MenuAction_Revert,
	MenuAction_Yes,
	MenuAction_No,
	MenuAction_Cancel,
	__MenuAction_Count
};

enum UIDisplayModes
{
	MenuEntry_Text,
	MenuEntry_Toggle,
	MenuEntry_Action,
	MenuEntry_Pitch,
	MenuEntry_Percentage,
	MenuEntry_FilterMix,
	MenuEntry_Value,
	MenuEntry_RemapValue,
	MenuEntry_MidValue,
	MenuEntry_EffectType,
	MenuEntry_EffectParam1,
	MenuEntry_EffectParam2,
	MenuEntry_EffectParam3,
};

enum
{
	BOX_REGULAR,
	BOX_MID,
	BOX_INV
};


enum OutputEnum
{
#define OUTPUT(name,codecport,codecpin, type,id, style,defaultvalue) Output_##name = id,
#define OUTPUT_VIRT(name,codecport,codecpin, type,id, style,defaultvalue) Output_##name = id,
#include "../interface/paramdef.h"
#undef OUTPUT
#undef OUTPUT_VIRT
	__OUTPUT_COUNT
};

enum SwitchEnum
{
#define SWITCH(name,id) Switch_##name = id,
#include "../interface/paramdef.h"
#undef SWITCH
	__SWITCH_COUNT
};

enum ledmodes
{
	ledmode_solid,
	ledmode_blinkslow,
	ledmode_blinkfast,
	ledmode_off
};

typedef struct LedState_t
{

	uint8_t r;
	uint8_t g;
	uint8_t b;
	ledmodes mode;
} LedState_t;


typedef struct ButtonState_t
{
	LedState_t led;
	bool down;
} ButtonState_t;


class PanState_t
{
public:
	void SetButtonLed(FinalLedButtonEnum led, ledmodes mode)
	{
		ledbuttons[led].led.mode = mode;
	}
	ButtonState_t ledbuttons[__FINALLEDBUTTON_COUNT];
	ButtonState_t encoders[__FINALENCODER_COUNT];
};

#endif