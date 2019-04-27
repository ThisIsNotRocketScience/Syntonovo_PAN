#pragma once
#include <stdint.h>
#include <vector>
#include "FinalPanEnums.h"
#pragma pack(1)

#ifdef WIN32
#define PACK
#else
#define PACK  __attribute__((packed))
#endif


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

enum ModSource_t : unsigned char
{
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

enum PresetCategoryEnum
{
	PresetCategory_Lead,
	PresetCategory_Bass,
	PresetCategory_Keys,
	PresetCategory_Effect,
	PresetCategory_Percussive,
	PresetCategory_Drone,
	PresetCategory_Brass,
	__PresetCategoryCount
};

typedef struct
{
	ModTarget_t target[16];
	ModSource_t source;
} ControlModulation_t;

#define PRESET_NAME_LENGTH 16

class PanPreset_Listener
{
public:
	static std::vector<PanPreset_Listener* > RegisteredPresetListeners;

	virtual void ParamChanged(OutputEnum paramID, uint16_t newvalue) {};
	virtual void SwitchChanged(SwitchEnum switchID, bool newvalue) {};
	virtual void NameChanged(char *newname) {}
	virtual void CategoryChanged(PresetCategoryEnum newcategory) {}
};

class PanPreset_t {
public:
	void PutSwitch(SwitchEnum SwitchID, bool v)
	{
		if (v) SetSwitch(SwitchID); else ClearSwitch(SwitchID);
	}

	void ClearSwitch(SwitchEnum SwitchID)
	{
		int switchset = ((int)SwitchID) / 32;
		int adjustedswitchid = SwitchID % 32;
		switches[switchset] &= ~(1 << adjustedswitchid);
	}

	void SetSwitch(SwitchEnum SwitchID)
	{
		int switchset = ((int)SwitchID) / 32;
		int adjustedswitchid = SwitchID % 32;
		switches[switchset] &= ~(1 << (adjustedswitchid));
		switches[switchset] |= (1 << (adjustedswitchid));
	}

	bool GetSwitch(SwitchEnum SwitchID)
	{
		int switchset = ((int)SwitchID) / 32;
		int adjustedswitchid = SwitchID % 32;

		if (((switches[switchset] >> adjustedswitchid) & 1) == 1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void ToggleSwitch(SwitchEnum SwitchID)
	{
		if (GetSwitch(SwitchID))
		{
			ClearSwitch(SwitchID);
		}

		else
		{
			SetSwitch(SwitchID);
		}

	}

	void TweakParameter(OutputEnum param, int delta)
	{
		int OrigVal = paramvalue[param];
		int32_t val = OrigVal + delta * 1000;
		if (val < 0) val = 0;
		if (val > 0xffff) val = 0xffff;
		paramvalue[param] = val;
	}

	void DescribeParam(OutputEnum param, int style, char *targetbuffer, int bufferlength)
	{
		switch (style)
		{
		case MenuEntry_MidValue:  snprintf(targetbuffer, bufferlength, "%1.1f%%", (float)((int)paramvalue[param]) * (200.0f / (float)0xFFFF) - 100); break;
		case MenuEntry_Value:  snprintf(targetbuffer, bufferlength, "%1.1f%%", (float)((int)paramvalue[param]) * (100.0f / (float)0xFFFF)); break;
		case MenuEntry_Percentage: 	snprintf(targetbuffer, bufferlength, "%1.1f%%", (float)((int)paramvalue[param]) * (100.0f / (float)0xFFFF));


		default: snprintf(targetbuffer, bufferlength, ""); break;
		}
	}

	uint32_t switches[2];
	uint16_t paramvalue[256];

	uint16_t ledbrightness;
	uint16_t ledblinkspeed;
	uint16_t GetLedParameter(LedParameter par)
	{

		switch (par)
		{
		case Led_Low_Bright: return low.v;
		case Led_Low_Sat: return low.s;
		case Led_Low_Hue: return low.h;

		case Led_High_Bright: return high.v;
		case Led_High_Sat: return high.s;
		case Led_High_Hue: return high.h;

		case Led_Active_Bright: return active.v;
		case Led_Active_Sat: return active.v;
		case Led_Active_Hue: return active.h;

		case Led_BlinkSpeed: return ledblinkspeed;
		case Led_Brightness: return ledbrightness;
		}

		return 0;
	}
	void TweakLed(LedParameter par, int delta)
	{
		uint16_t *p = &ledbrightness;

		switch (par)
		{
		case Led_Low_Bright: p = &low.v; break;
		case Led_Low_Sat: p = &low.s; break;
		case Led_Low_Hue: p = &low.h; break;

		case Led_High_Bright: p = &high.v; break;
		case Led_High_Sat: p = &high.s; break;
		case Led_High_Hue: p = &high.h; break;

		case Led_Active_Bright: p = &active.v; break;
		case Led_Active_Sat: p = &active.v; break;
		case Led_Active_Hue: p = &active.h; break;

		case Led_BlinkSpeed: p = &ledblinkspeed; break;
		case Led_Brightness: p = &ledbrightness; break;
		}
		int origp = *p;
		int newp = origp + delta  * 1000;
		if (newp > 0xffff) newp = 0xffff; else if (newp < 0) newp = 0;
		*p = newp;
	}
	hsv low;
	hsv high;
	hsv active;

	LfoModulation_t lfomod[16];
	AdsrModulation_t adsrmod[16];
	AdModulation_t admod[16];
	ControlModulation_t ctrlmod[16];

	char Name[PRESET_NAME_LENGTH];
} PACK;