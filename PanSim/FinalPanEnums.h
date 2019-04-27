#pragma once
#ifndef FINALPANENUMS
#define FINALPANENUMS

#include <stdint.h>

#define ParamBoxWidth 220
#define ParamVerticalBoxHeight 90


class hsv
{
public:
	hsv()
	{
		h = 0;
		s = 0xffff;
		v = 0xffff;
	}
	uint16_t h, s, v;
};
enum
{
	Led_Low,
	Led_High,
	Led_Active
};

enum Screens_t
{
	SCREEN_HOME,
	
	SCREEN_ALLPARAMS,
	SCREEN_ALLSWITCHES,

	SCREEN_COLORS,
	SCREEN_SYSTEM,

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
	MenuAction_Store,
	MenuAction_Yes,
	MenuAction_No,
	MenuAction_Cancel,
	__MenuAction_Count
};

enum LedParameter
{
	Led_Low_Bright,
	Led_Low_Sat,
	Led_Low_Hue,

	Led_High_Bright,
	Led_High_Sat,
	Led_High_Hue,

	Led_Active_Bright,
	Led_Active_Sat,
	Led_Active_Hue,

	Led_BlinkSpeed,
	Led_Brightness

};


enum UIDisplayModes
{
	MenuEntry_NoStyle,
	MenuEntry_Page,
	MenuEntry_LedValue,
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
#define SWITCH(name,id,defaultvalue) Switch_##name = id,
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

	int r;
	int g;
	int b;
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

	void SetEncoderLed(FinalEncoderEnum encoder, ledmodes mode, int r, int g, int b)
	{
		encoders[encoder].led.mode = mode;
		encoders[encoder].led.r = r;
		encoders[encoder].led.g = g;
		encoders[encoder].led.b = b;
	}

	uint16_t ledr, ledg, ledb, bledr, bledg, bledb, hledr, hledg, hledb;

	void SetLed(int led, hsv &H)
	{
		uint16_t *r = &ledr;
		uint16_t *g = &ledg;
		uint16_t *b = &ledb;

		switch (led)
		{
		case 0:
			break;
		case 1:
			r = &bledr; g = &bledg; b = &bledb;
			break;
		case 2:
			r = &hledr; g = &hledg; b = &hledb;
			break;
		}

		hsv2rgb(H.h, H.s, H.v, r, g, b);
		
	}

	void hsv2rgb(uint16_t h, uint16_t s, uint16_t v, uint16_t *r, uint16_t *g, uint16_t *b)
	{
		uint16_t region, remainder, p, q, t;

		if (s == 0)
		{
			*r = v;
			*g = v;
			*b = v;
			return;
		}

		region = h / (43<<8);
		remainder = (h - (region * (43<<8))) * 6;

		p = (v * (0xffff - s)) >> 16;
		q = (v * (0xffff - ((s * remainder) >> 16))) >> 16;
		t = (v * (0xffff - ((s * (0xffff- remainder)) >> 16))) >> 16;

		switch (region)
		{
		case 0:
			*r = v; *g = t; *b = p;
			break;
		case 1:
			*r = q; *g = v; *b = p;
			break;
		case 2:
			*r = p; *g = v; *b = t;
			break;
		case 3:
			*r = p; *g = q; *b = v;
			break;
		case 4:
			*r = t; *g = p; *b = v;
			break;
		default:
			*r = v; *g = p; *b = q;
			break;
		}
		
	}


	ButtonState_t ledbuttons[__FINALLEDBUTTON_COUNT];
	ButtonState_t encoders[__FINALENCODER_COUNT];
};

#endif