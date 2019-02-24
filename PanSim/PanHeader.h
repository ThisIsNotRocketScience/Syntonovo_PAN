#pragma once

//#define SIMULATEINCOMINGSERIAL

#ifndef SYNTONPAN
#define SYNTONPAN
#include <string>

#define LED_BLINK 2
#define LED_ON 1
#define LED_OFF 0

#define KNOB(name,x,y,min,max,idx,lbl) +1
const int knobcount = 0
#include "PanControls.h"
;
#undef KNOB

enum KnobEnum
{
#define KNOB(name,x,y,min,max,idx,lbl)  knob_##name,
#define SLIDER(name,x,y,min,max,idx,lbl)  knob_##name,
#include "PanControls.h"
#undef KNOB
#undef SLIDER
	__KNOB_COUNT
};

enum LedButtonEnum
{
#define LEDBUTTON(name,x,y,idx)  ledbutton_##name,
#include "PanControls.h"
#undef LEDBUTTON
	__LEDBUTTON_COUNT
};

enum LedEnum
{
#define LED(name,x,y)  led_##name,
#include "PanControls.h"
#undef LED
	__LED_COUNT
};

enum EncoderEnum
{
#define ENCODER(name,x,y)  encoder_##name,
#include "PanControls.h"
#undef ENCODER
	__ENCODER_COUNT
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




typedef struct _param_t
{	
	uint16_t value;

	uint8_t lfo_group;
	uint16_t lfo_speed;
	int16_t lfo_depth;
	int16_t lfo_shape;

	uint8_t adsr_group;
	uint16_t adsr_a;
	uint16_t adsr_d;
	uint16_t adsr_s;
	uint16_t adsr_r;
	int16_t adsr_depth;

	uint8_t ad_group;
	uint16_t ad_a;
	uint16_t ad_d;
	int16_t ad_depth;

	uint8_t xgroup;
	int16_t x;
	uint8_t ygroup;
	int16_t y;
	uint8_t zgroup;
	int16_t z;

	uint8_t zprimegroup;
	int16_t zprime;
	
	uint8_t notegroup;
	int16_t note;
	
	uint8_t velgroup;
	int16_t vel;

	uint8_t aftertouchgroup;
	int16_t aftertouch;
} param_t;


enum paramorderenum{
	 value,

	lfo_group,
	lfo_speed,
	lfo_depth,
	lfo_shape,

	adsr_group,
	adsr_a,
	adsr_d,
	adsr_s,
	adsr_r,
	adsr_depth,

	ad_group,
	ad_a,
	ad_d,
	ad_depth,

	xgroup,
	x,
	ygroup,
	y,
	zgroup,
	z,

	zprimegroup,
	zprime,

	notegroup,
	note,

	velgroup,
	vel,

	aftertouchgroup,
	aftertouch
};

class aLedButton
{
public:

	char *name;
	float x;
	float y;
	LedButtonEnum id;
	int fpid;
	bool value;
	int ledmode;
};

extern aLedButton Buttons[];

extern const int ledbuttoncount;

class Led
{
public:

	char* name;
	float x;
	float y;
	int id;
	bool value;
};



class Knob
{
public:

	char *name;
	float x;
	float y;
	KnobEnum id;
	int isslider;
	int frontpanelidx;
	char *label;
	float value;
};


class Encoder
{
public:
	char *name;
	float x;
	float y;
	EncoderEnum id;
	float pos;
	int delta;
};


class Screen
{
public:
	float x;
	float y;
	float width;
	float height;
};

extern Knob Knobs[];
extern Led Leds[];
extern Encoder Encoders[];
extern Screen TheScreen;



typedef struct PanScanner_t
{
	
} PanScanner_t;
typedef enum
{
	Page_BOOT,
	Page_HOME,
	Page_MidiSettings,
	
	Page_Source_LFO,
	Page_Source_ADSR,
	Page_Source_AR,
	Page_Source_KEYBOARD_VELOCITY,
	Page_Source_KEYBOARD_GATE,
	Page_Source_KEYBOARD_TRIGGER,
	Page_Source_KEYBOARD_AFTERTOUCH,
	Page_Source_PADLEFT_A,
	Page_Source_PADRIGHT_A,

	Page_Source_XYZ_X,
	Page_Source_XYZ_Y,
	Page_Source_XYZ_Z,
} Pages;

extern void GUI_OpenPage(Pages Page);
extern void GUI_Home(Pages Page);

extern void GUI_Enc1(int Delta);
extern void GUI_Enc1Press();
extern void GUI_Enc2(int Delta);
extern void GUI_Enc2Press();

extern void GUI_Enter();
extern void GUI_Cancel();

enum SubParam_t
{
	Sub_value = 0,
	Sub_lfo_speed,
	Sub_lfo_depth,
	Sub_lfo_shape,
	Sub_adsr_a,
	Sub_adsr_d,
	Sub_adsr_s,
	Sub_adsr_r,
	Sub_adsr_depth,
	Sub_ad_a,
	Sub_ad_d,
	Sub_ad_depth,
	Sub_x,
	Sub_y,
	Sub_z,
	Sub_zprime,
	Sub_note,
	Sub_vel,
	Sub_lfo_reset_phase,
	Sub_modpad_l,
	Sub_modpad_r,
	Sub_suspad_l,
	Sub_suspad_r,
	Sub_unacpad_l,
	Sub_unacpad_r,

	Sub_flags = 32
};

enum SubParamFlags_t
{
	SubParamFlags_AdsrRetrigger = 1,
	SubParamFlags_AdRetrigger = 2,
	SubParamFlags_LfoRetrigger = 4,

	SubParamFlags_max = 0xffff
};

inline int ParamToButton(int paramid)
{
#define TARGET(param, button, knob, name) \
	if (paramid == param) return button;
#include "PanUiMap.h"
#undef TARGET
	return -1;
}

inline int ButtonToParam(int buttonid, int depth)
{
#define TARGET(param, button, knob, name) \
	if (buttonid == button && !(depth--)) return param;
#include "PanUiMap.h"
#undef TARGET
	return -1;
}

inline int KnobToParam(int knobid)
{
#define TARGET(param, button, knob, name) \
	if (knobid == knob) return param;
#include "PanUiMap.h"
#undef TARGET
	return -1;
}

inline KnobEnum ParamToKnob(int paramid)
{
#define TARGET(param, button, knob, name) \
	if (param == paramid) return knob;
#include "PanUiMap.h"
#undef TARGET
	return __KNOB_COUNT;
}

inline const char* ParamToName(int paramid)
{
#define TARGET(param, button, knob, name) \
	if (paramid == param) return name;
#include "PanUiMap.h"
#undef TARGET
	return "Unnamed";
}

#pragma pack(1)

#ifdef WIN32
#define PACK
#else
#define PACK  __attribute__((packed))
#endif
typedef struct PanGui_t
{
	
} PanGui_t;

typedef struct {
	uint8_t param;
	uint16_t depth;
} PACK ModTarget_t;

typedef struct {
	ModTarget_t target[16];
	uint16_t speed;
	uint16_t shape;
} PACK LfoModulation_t;

typedef struct {
	ModTarget_t target[16];
	uint16_t a;
	uint16_t d;
	uint16_t s;
	uint16_t r;
} PACK AdsrModulation_t;

typedef struct {
	ModTarget_t target[16];
	uint16_t a;
	uint16_t d;
} PACK  AdModulation_t;

enum ModSource_t: unsigned char {
	Source_none,

	Source_left_mod,
	Source_right_mod,
	Source_x,
	Source_y,
	Source_z,
	Source_zprime,
	Source_note,
	Source_vel,
	Source_left_sus,
	Source_right_sus,
	Source_left_unac,
	Source_right_unac,
	__ModSource_COUNT
};

typedef struct {
	ModTarget_t target[16];
	ModSource_t source;
} ControlModulation_t;

#define PRESET_NAME_LENGTH 16

typedef struct {
	uint32_t switches[2];
	uint16_t paramvalue[256];

	LfoModulation_t lfomod[16];
	AdsrModulation_t adsrmod[16];
	AdModulation_t admod[16];
	ControlModulation_t ctrlmod[16];
	char Name[PRESET_NAME_LENGTH];
} PACK PanPreset_t;

extern bool Teensy_KnobLastValue[__KNOB_COUNT];
extern bool Teensy_KnobTouched[__KNOB_COUNT];

typedef enum: unsigned char{
	GuiState_Root,
	GuiState_LfoSelect,
	GuiState_AdsrSelect,
	GuiState_AdSelect,
	GuiState_CtrlSelect,
	GuiState_SelectBanks,
	GuiState_SelectSaveSlot,
	GuiState_SavePreset,
	GuiState_InitPatch,
#define MENU(id, buttonid, name) \
	GuiState_Menu_##id,
#include "PanUiMap.h"
#undef MENU
	GuiState_Boot,
	__GuiState_COUNT
} GuiState_t;

typedef struct Raspberry_GuiData_t
{
	GuiState_t GuiState;
	
	int ModSelect;
	int selectTarget;
	char PresetName[PRESET_NAME_LENGTH];

	ControlModulation_t dataCtrl;
	LfoModulation_t dataLfo;
	AdsrModulation_t dataAdsr;
	AdModulation_t dataAd;

	uint32_t outputValues[256];
	uint32_t switches[2];

	uint32_t LeftEncoderValue;
	uint32_t RightEncoderValue;
	bool dirty;
	int banks[2];
	int activeslot;
	int activebank;
} PACK  Raspberry_GuiData_t;

#pragma pack()

inline GuiState_t ButtonToMenu(int buttonid)
{
#define EXTRABUTTON(id, button) if (buttonid == button) return GuiState_Menu_##id;
#define MENU(id, button, name) if (buttonid == button) return GuiState_Menu_##id;
#include "PanUiMap.h"
#undef MENU
#undef EXTRABUTTON
	return GuiState_Root;
}

inline bool IsMenuState(GuiState_t state)
{
#define MENU(id, button, name) \
	if (state == GuiState_Menu_##id) return true;
#include "PanUiMap.h"
#undef MENU
	return false;
}

extern void Teensy_Reset();
extern void Teensy_InitPreset();
extern void Teensy_KnobChanged(KnobEnum ID, uint32_t value);
extern void Teensy_ButtonPressed(LedButtonEnum ID, int value);
extern void Teensy_EncoderPress(int id);
extern void Teensy_EncoderRotate(int id, int delta);

extern int Teensy_FindKnobIDX(int in);
extern int Teensy_FindButtonIDX(int in);

extern void Raspberry_Reset();
extern void Raspberry_RenderScreen();
extern void Raspberry_Init();
extern void Raspberry_EncoderTurn(EncoderEnum id, int delta);

void Raspberry_ToState(GuiState_t state, int modselect);
void Raspberry_EditLfo(LfoModulation_t& mod);
void Raspberry_EditAdsr(AdsrModulation_t& mod);
void Raspberry_EditAd(AdModulation_t& mod);
void Raspberry_EditCtrl(ControlModulation_t& mod);
void Raspberry_SelectTarget(int target);
void Raspberry_UpdateTarget(int target, int param, int depth);
void Raspberry_SetLfoParam(SubParam_t subparam, uint32_t value);
void Raspberry_SetAdsrParam(SubParam_t subparam, uint32_t value);
void Raspberry_SetAdParam(SubParam_t subparam, uint32_t value);
void Raspberry_OutputChangeValue(int output, uint32_t value);
void Raspberry_SetSwitches(uint32_t* switches);
void Raspberry_SetName(char* newname);
void Raspberry_WindowFrame();

int DecodeCurrentEffect(uint32_t switches);
void Teensy_ReSendPreset();
extern Raspberry_GuiData_t Raspberry_guidata;
void LoadPreset(PanPreset_t& preset);
void SetLedButton(int id, int mode);

void DoDataCommands(unsigned char comm, uint32_t data);

#endif