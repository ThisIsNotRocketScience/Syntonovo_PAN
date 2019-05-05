#pragma once
#ifndef PANPRESET
#define PANPRESET

#include <stdint.h>
#include <vector>
#include "FinalPanEnums.h"
#pragma pack(1)

#ifdef WIN32
#define PACK
#else
#define PACK  __attribute__((packed))
#endif


typedef struct
{
	uint16_t depth;
	uint16_t outputid;
	uint8_t sourceid;
} PACK ModTargetSpec_t;

typedef struct
{
	ModTargetSpec_t targets[MODTARGET_COUNT];
} PACK ModMatrixRow_t;

typedef struct
{
	uint16_t flags; // including key range
	uint16_t speed;
	int16_t depth;
	int16_t shape;
	uint16_t reset_phase;
	uint16_t ___reserved[3];
} PACK LfoParam_t;

typedef struct
{
	uint16_t flags; // including env type, key range
	uint16_t a;
	uint16_t d;
	uint16_t s;
	uint16_t r;
	uint16_t curve;
	uint16_t ___reserved[2];
} PACK EnvParam_t;

typedef struct
{
	int16_t scale;
	int16_t deadzone;
	//int16_t shape;
} PACK ControllerParam_t;

typedef struct
{
	uint8_t modsource1;
	uint8_t modsource2;
	uint16_t op; // tbd plus, minus, mul, diff, sh, lag, etc.
	uint16_t parameter; // parameter for selected op
	uint16_t ___reserved;
} PACK OperatorParam_t;


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

enum
{
	Controller_X,
	Controller_Y,
	Controller_Z,
	Controller_ZPrime,
	Controller_PadL,
	Controller_PadR,
	Controller_SustainL,
	Controller_SustainR,
	Controller_UnaCordaL,
	Controller_UnaCordaR,
	Controller_Pedal,
	NUM_CONTROLLERS
};


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
	char Name[PRESET_NAME_LENGTH];

	void PutSwitch(SwitchEnum SwitchID, bool v)
	{
		if (v) SetSwitch(SwitchID); else ClearSwitch(SwitchID);
	}

	void SetName(char *name)
	{
		snprintf(Name, PRESET_NAME_LENGTH, "%s", name);
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

	FilterTypes_t Decode(bool A, bool B)
	{
		switch ((A ? 1 : 0) + (B ? 2 : 0))
		{
		case 0: return Filt_LP;
		case 1: return Filt_HP;
		case 2: return Filt_BP;
		case 3: return Filt_BR;
		}
		return Filt_LP;

	}
	void SetFilterType(VCF2Filter filt, FilterTypes_t newf)
	{
		SwitchEnum S1 = Switch_SELVCF21A;
		SwitchEnum S2 = Switch_SELVCF21B;

		switch (filt)
		{
		case FilterVCF2A:S1 = Switch_SELVCF22A; S2 = Switch_SELVCF21B; break;
		case FilterVCF2B:S1 = Switch_SELVCF22A; S2 = Switch_SELVCF22B; break;
		case FilterVCF2C:S1 = Switch_SELVCF23A; S2 = Switch_SELVCF23B; break;
		case FilterVCF2D:S1 = Switch_SELVCF24A; S2 = Switch_SELVCF24B; break;
		}
		bool B1;
		bool B2;
		switch (newf)
		{
		case Filt_LP: B1 = false; B2 = false; break;
		case Filt_HP: B1 = true;  B2 = false; break;
		case Filt_BP: B1 = false; B2 = true; break;
		case Filt_BR: B1 = true;  B2 = true; break;
		}
		PutSwitch(S1, B1);
		PutSwitch(S2, B2);
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
	uint16_t *GetModParamPointer(ModParameters param, int instance)
	{
		switch (param)
		{
		case Envelope_Attack: return &env[instance].a;
		case Envelope_Decay: return &env[instance].d;
		case Envelope_Sustain: return &env[instance].s;
		case Envelope_Release: return &env[instance].r;
		case Envelope_Curve: return &env[instance].curve;

		case LFO_Speed: return &lfo[instance].speed;
		case LFO_Depth: return (uint16_t*)&lfo[instance].depth;
		case LFO_Shape: return (uint16_t*)&lfo[instance].shape;
		case LFO_ResetPhase: return &lfo[instance].reset_phase;

		case KeyboardParam_DeadzoneX: return (uint16_t*)&controller[Controller_X].deadzone;
		case KeyboardParam_ScaleX:    return (uint16_t*)&controller[Controller_X].scale;
	 
		case KeyboardParam_DeadzoneY:return (uint16_t*)&controller[Controller_Y].deadzone;
		case KeyboardParam_ScaleY:	 return (uint16_t*)&controller[Controller_Y].scale;
	 
		case KeyboardParam_DeadzoneZ:return (uint16_t*)&controller[Controller_Z].deadzone;
		case KeyboardParam_ScaleZ:	 return (uint16_t*)&controller[Controller_Z].scale;








		}
		return 0;
	}

	void TweakModulation(ModParameters param, int instance, int delta)
	{
		uint16_t *p = GetModParamPointer(param, instance);
		if (p) {
			int OrigVal = *p;
			int32_t val = OrigVal + delta ;
			if (val < 0) val = 0;
			if (val > 0xffff) val = 0xffff;
			*p = val;
		}

	}

	uint16_t GetModParameterValue(ModParameters param, int instance)
	{
		uint16_t *p = GetModParamPointer(param, instance);
		if (p) return *p;


		return 0;

	}

	void TweakParameter(OutputEnum param, int delta, int deltamod = 1)
	{
		int OrigVal = paramvalue[param];
		int32_t val = OrigVal + delta * deltamod;
		if (val < 0) val = 0;else if (val > 0xffff) val = 0xffff;
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

#define SYNTH_MODSOURCE_COUNT (64)
	ModMatrixRow_t modmatrix[SYNTH_MODSOURCE_COUNT];

	void TweakModMatrix(ModSource_t mod, int instance, int id, int delta, int deltamod = 1)
	{
		auto row = GetModSourceRow(mod, instance);
		
		int OrigVal = row->targets[id].depth;
		int32_t val = OrigVal + delta * deltamod;
		if (val < 0) val = 0; else if (val > 0xffff) val = 0xffff;
		row->targets[id].depth = val;


		
	}

	ModMatrixRow_t *GetModSourceRow(ModSource_t mod, int instance)
	{
		switch (mod)
		{
		case Source_LFO: return &modmatrix[0 + instance];
		case Source_Envelope: return &modmatrix[0x10 + instance];
		case Source_x: return &modmatrix[0x20];
		case Source_y: return &modmatrix[0x21];
		case Source_z: return &modmatrix[0x21];	
		case Source_zprime: return &modmatrix[0x22];
		case Source_left_mod: return &modmatrix[0x23];
		case Source_right_mod: return &modmatrix[0x24];
		case Source_left_unac: return &modmatrix[0x25];
		case Source_right_unac: return &modmatrix[0x26];
		case Source_note: return &modmatrix[0x2E];
		case Source_vel:  return &modmatrix[0x2E];
			//		case Source_pedal: return &modmatrix[0x22];
		}
		return 0;
	}

#define NUM_LFOS (16)
	LfoParam_t lfo[NUM_LFOS];

#define NUM_ENVS (16)
	EnvParam_t env[NUM_ENVS];

//#define NUM_CONTROLLERS (11)

	ControllerParam_t controller[NUM_CONTROLLERS];

#define NUM_OPERATORS (16)
	OperatorParam_t op[NUM_OPERATORS];

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
		case Led_Active_Sat: return active.s;	
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
		case Led_Active_Sat: p = &active.s; break;
		case Led_Active_Hue: p = &active.h; break;

		case Led_BlinkSpeed: p = &ledblinkspeed; break;
		case Led_Brightness: p = &ledbrightness; break;
		}
		int origp = *p;
		int newp = origp + delta * 200;
		if (newp > 0xffff) *p = 0xffff; else if (newp < 0) *p = 0; else *p = newp;
		
	}
	hsv low;
	hsv high;
	hsv active;
} PACK;

extern PanPreset_t gCurrentPreset;

#endif
