#pragma once
#ifndef FINALPANENUMS
#define FINALPANENUMS

#include <stdint.h>

#define ParamBoxWidth 220
#define ParamVerticalBoxHeight 90

#pragma pack(1)

#ifdef WIN32
	#ifndef PACK 
		#define PACK
	#endif
#else
	#ifndef PACK 
		#define PACK  __attribute__((packed))
	#endif
#endif


class hsv
{
public:
	uint16_t h, s, v;
} PACK;

enum LedTheme
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
	SCREEN_VELOCITY,
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

	SCREEN_CLEANMIX,
	SCREEN_VCF1MIX,
	SCREEN_VCF2MIX,


	SCREEN_ARP,
	SCREEN_MIXER,
	SCREEN_EFFECTS,
	SCREEN_SELECTBANKL,
	SCREEN_SELECTBANKR,
	SCREEN_TEST,
	SCREENS_COUNT
};

enum LeftRight
{
	Left,
	Right
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
} PACK;

typedef struct LedState_t
{

	uint16_t r;
	uint16_t g;
	uint16_t b;
	ledmodes mode;
} PACK LedState_t;


typedef struct ButtonState_t
{
	bool down;
} ButtonState_t;

typedef struct
{
	LedState_t ledbuttons[__FINALLEDBUTTON_COUNT];
	LedState_t encoders[__FINALENCODER_COUNT];
} PACK PanLedState_t;

class PanState_t
{
public:
	void SetButtonLed(FinalLedButtonEnum led, ledmodes mode)
	{
		s.ledbuttons[led].mode = mode;
	}

	void SetEncoderLed(FinalEncoderEnum encoder, ledmodes mode, int r, int g, int b)
	{
		s.encoders[encoder].mode = mode;
		s.encoders[encoder].r = r;
		s.encoders[encoder].g = g;
		s.encoders[encoder].b = b;
	}

	uint16_t low_led_r, low_led_g, low_led_b, high_led_r, high_led_g, high_led_b, active_led_r, active_led_g, active_led_b;
	void GetThemeLed(LedTheme led, uint16_t *r, uint16_t *g, uint16_t *b)
	{
		switch (led)
		{
		case Led_Low:
			*r = low_led_r;
			*g = low_led_g;
			*b = low_led_b;
			break;
		case Led_High:
			*r = high_led_r;
			*g = high_led_g;
			*b = high_led_b;
			break;
		case Led_Active:
			*r = active_led_r;
			*g = active_led_g;
			*b = active_led_b;
			break;
		}
	}
	void SetLed(LedTheme led, hsv &H)
	{
		uint16_t *r = &low_led_r;
		uint16_t *g = &low_led_g;
		uint16_t *b = &low_led_b;

		switch (led)
		{
		case Led_Low:
			break;
		case Led_High:
			r = &high_led_r; g = &high_led_g; b = &high_led_b;
			break;
		case Led_Active:
			r = &active_led_r; g = &active_led_g; b = &active_led_b;
			break;
		}

		hsv2rgb(H.h, H.s, H.v, r, g, b);
		
	}


	void hsv2rgb(uint16_t h, uint16_t s, uint16_t v, uint16_t *r, uint16_t *g, uint16_t *b)
	{
#define HSV_SECTION_6 (0x20)
#define HSV_SECTION_3 (0x40)

		uint8_t hue = h >> 8;
		uint8_t value = v >> 8;
		uint8_t saturation = s >> 8;

		// The brightness floor is minimum number that all of
		// R, G, and B will be set to.
		uint8_t invsat = 255 - saturation;
		uint8_t brightness_floor = (value * invsat) / 256;

		uint8_t color_amplitude = value - brightness_floor;

		uint8_t section = hue / HSV_SECTION_3; // 0..2
		uint8_t offset = hue % HSV_SECTION_3;  // 0..63

		uint8_t rampup = offset; // 0..63
		uint8_t rampdown = (HSV_SECTION_3 - 1) - offset; // 63..0

														
		uint8_t rampup_amp_adj = (rampup   * color_amplitude) / (256 / 4);
		uint8_t rampdown_amp_adj = (rampdown * color_amplitude) / (256 / 4);

		// add brightness_floor offset to everything
		uint8_t rampup_adj_with_floor = rampup_amp_adj + brightness_floor;
		uint8_t rampdown_adj_with_floor = rampdown_amp_adj + brightness_floor;


		if (section) {
			if (section == 1) {
				// section 1: 0x40..0x7F
				*r = brightness_floor <<8 ;
				*g = rampdown_adj_with_floor << 8 ;
				*b = rampup_adj_with_floor <<8;
			}
			else {
				// section 2; 0x80..0xBF
				*r = rampup_adj_with_floor <<8;
				*g = brightness_floor <<8;
				*b = rampdown_adj_with_floor <<8 ;
			}
		}
		else {
			// section 0: 0x00..0x3F
			*r = rampdown_adj_with_floor << 8;
			*g = rampup_adj_with_floor << 8;
			*b = brightness_floor << 8;
		}
	}


	void brokenhsv2rgb(uint16_t h, uint16_t s, uint16_t v, uint16_t *r, uint16_t *g, uint16_t *b)
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


	void ClearLeds()
	{
		for (int i = 0; i < __FINALLEDBUTTON_COUNT; i++)
		{

			s.ledbuttons[i].mode = ledmode_off;

		}
		for (int i = 0; i < __FINALENCODER_COUNT; i++)
		{
			s.encoders[i].mode = ledmode_off;
		}
	}
	int BankLeft;
	int BankRight;
	int CurrentPatch;
	ButtonState_t ledbuttons[__FINALLEDBUTTON_COUNT];
	ButtonState_t encoders[__FINALENCODER_COUNT];

	PanLedState_t s;
};

#endif