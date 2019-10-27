#pragma once
#ifndef PANPRESET
#define PANPRESET

#include <stdint.h>
#include <vector>
#include "FinalPanEnums.h"

#pragma pack(push)
#pragma pack(1)

#ifdef WIN32
#define PACKED
#else
#define PACKED  __attribute__((packed))
#endif


typedef struct
{
	int16_t depth;
	uint16_t outputid;
	uint8_t sourceid;
	uint8_t __reserved1;
	uint16_t __reserved2;
} PACKED ModTargetSpec_t;

typedef struct
{
	ModTargetSpec_t targets[MODTARGET_COUNT];
} PACKED ModMatrixRow_t;

typedef struct
{
	uint16_t flags; // including key range
	uint16_t speed;
	int16_t depth;
	int16_t shape;
	uint16_t reset_phase;
	uint16_t ___reserved[3];
} PACKED LfoParam_t;

typedef struct
{
	uint16_t flags; // including env type, key range
	uint16_t a;
	uint16_t d;
	uint16_t s;
	uint16_t r;
	uint16_t curve;
	uint16_t ___reserved[2];
} PACKED EnvParam_t;

typedef struct
{
	int16_t scale;
	int16_t deadzone;
	//int16_t shape;
} PACKED ControllerParam_t;

typedef struct
{
	uint8_t modsource1;
	uint8_t modsource2;
	uint16_t op; // tbd plus, minus, mul, diff, sh, lag, etc.
	uint16_t parameter; // parameter for selected op
	uint16_t ___reserved;
} PACKED OperatorParam_t;


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

#define MAX_MODULATION_SOURCE (6)
#define NUM_LFOS (16)
#define NUM_ENVS (16)
#define NUM_OPERATORS (16)
#define SYNTH_MODSOURCE_COUNT (64)

typedef struct
{
	int16_t depth;
	uint16_t sourceid;
	uint8_t subsourceid;
} PACKED ModSourceSpec_t;

class ModSourcesForOutputStruct
{
public:
	int Sources;
	ModSourceSpec_t SourceDetails[MODTARGET_COUNT * SYNTH_MODSOURCE_COUNT];
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
	bool GetModulationSwitch(int switchid, int instance)
	{
		switch (switchid)
		{
		case Envelope_Retrigger:
		{
			uint16_t *s = GetModParamPointer(Envelope_Switches, instance);
			return ((*s) & SubParamFlags_AdsrRetrigger) > 0;

		}break;
		case LFO_ResetOnKey:
		{
			uint16_t *s = GetModParamPointer(LFO_Switches, instance);
			return ((*s) & SubParamFlags_AdsrRetrigger) > 0;
		}break;
		}
		return false;

	}
	void ToggleModulationSwitch(int switchid, int instance)
	{
		switch (switchid)
		{
		case Envelope_Retrigger:
		{	
			uint16_t *s = GetModParamPointer(Envelope_Switches, instance);
			*s = ((*s) & ~SubParamFlags_AdsrRetrigger) | (~(*s) & SubParamFlags_AdsrRetrigger);

		}break;
		case LFO_ResetOnKey:
		{
			uint16_t *s = GetModParamPointer(LFO_Switches, instance);
			*s = ((*s) & ~LfoParamFlags_LfoRetrigger) | (~(*s) & LfoParamFlags_LfoRetrigger);
		}break;
		}
	}

	void PutSwitch(SwitchEnum SwitchID, bool v)
	{
		if (v) SetSwitch(SwitchID); else ClearSwitch(SwitchID);
	}

	void SetName(const char *name)
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
		bool B1 = false;
		bool B2 = false;
		switch (newf)
		{
		default: B1 = B2 = false; break;
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
		case KeyboardParam_DeadzoneLMod: return (uint16_t*)&controller[Controller_PadL].deadzone;
		case KeyboardParam_DeadzoneRMod: return (uint16_t*)&controller[Controller_PadR].deadzone;
		case KeyboardParam_DeadzoneLSus: return (uint16_t*)&controller[Controller_SustainL].deadzone;
		case KeyboardParam_DeadzoneRSus: return (uint16_t*)&controller[Controller_SustainR].deadzone;
		case KeyboardParam_DeadzoneLUna: return (uint16_t*)&controller[Controller_UnaCordaL].deadzone;
		case KeyboardParam_DeadzoneRUna: return (uint16_t*)&controller[Controller_UnaCordaR].deadzone;
		case KeyboardParam_ScaleLMod: return (uint16_t*)&controller[Controller_PadL].scale;
		case KeyboardParam_ScaleRMod: return (uint16_t*)&controller[Controller_PadR].scale;
		case KeyboardParam_ScaleLSus: return (uint16_t*)&controller[Controller_SustainL].scale;
		case KeyboardParam_ScaleRSus: return (uint16_t*)&controller[Controller_SustainR].scale;
		case KeyboardParam_ScaleLUna: return (uint16_t*)&controller[Controller_UnaCordaL].scale;
		case KeyboardParam_ScaleRUna: return (uint16_t*)&controller[Controller_UnaCordaR].scale;

		case Envelope_Switches:  return (uint16_t*)&env[instance].flags;
		case LFO_Switches:  return (uint16_t*)&lfo[instance].flags;





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
	OutputEnum GetModulationOutput(OutputEnum inparam)
	{
		switch (inparam)
		{
		case Output_VCF1_LEVEL: return Output_VCF1_LIN;
		case Output_VCF2_LEVEL: return Output_VCF2_LIN;
		case Output_CLEANF_LEVEL: return Output_CLEANF_LIN;
		}

		return inparam;
	}
	void FillModSourcesForOutput(OutputEnum param, ModSourcesForOutputStruct *ModSources)
	{
		ModSources->Sources = 0;
		for (int i = 0; i < SYNTH_MODSOURCE_COUNT; i++)
		{
			ModMatrixRow_t *mm = &modmatrix[i];
			for (int j = 0; j < 11; j++)
			{
				ModTargetSpec_t *tt = &mm->targets[j];
				if (tt->outputid == param)
				{
					ModSourceSpec_t *mss = &ModSources->SourceDetails[ModSources->Sources];
					mss->depth = tt->depth;
					mss->sourceid = i;
					mss->subsourceid = tt->sourceid;
					ModSources->Sources++;
				}
			}
		}
	}

	int GetModSourceCountForOutput(OutputEnum param)
	{
		ModSourcesForOutputStruct S;
		FillModSourcesForOutput(param, &S);
		return S.Sources;
	}

	void RemoveModulation(int sourceid, int subsourceid, OutputEnum target)
	{
		for (int i = 0; i < 11; i++)
		{
			if (modmatrix[sourceid].targets[i].outputid == target && modmatrix[sourceid].targets[i].sourceid == subsourceid)
			{
				// remove it! 
				modmatrix[sourceid].targets[i].outputid = NOMODTARGET;
				modmatrix[sourceid].targets[i].depth = 0;
				modmatrix[sourceid].targets[i].sourceid = 0;
			}
		}	
	}

	uint32_t switches[2];
	uint16_t paramvalue[256];

	ModMatrixRow_t modmatrix[SYNTH_MODSOURCE_COUNT];

	void TweakModMatrix(ModSource_t mod, int instance, int id, int delta, int deltamod = 1)
	{
		auto row = GetModSourceRow(mod, instance);
		
		int OrigVal = (int16_t)row->targets[id].depth;
		int32_t val = OrigVal + delta * deltamod;
		if (val < -0x8000) val = -0x8000; else if (val > 0x7fff) val = 0x7fff;
		row->targets[id].depth = val;


		
	}
	
	void RowIntToModSourceAndInstance(int id, ModSource_t *mod, int *instance)
	{
		if (id < 0x10)
		{
			*mod = Source_LFO;
			*instance = id;
			return;
		}
		if (id < 0x20)
		{
			*mod = Source_Envelope;
			*instance = id - 0x10;
			return;
		}
		*instance = 0;
		switch (id)
		{
		case 0x20:  *mod = Source_x; return;
		case 0x21:  *mod = Source_y; return;
		case 0x22:  *mod = Source_z; return;
		case 0x23:  *mod = Source_zprime; return;
		case 0x24:  *mod = Source_left_mod; return;
		case 0x25:  *mod = Source_right_mod; return;
		case 0x26:  *mod = Source_left_sus; return;
		case 0x27:  *mod = Source_right_sus; return;
		case 0x28:  *mod = Source_left_unac; return;
		case 0x29:  *mod = Source_right_unac; return;
		case 0x2a:  *mod = Source_note; return;
		case 0x2b:  *mod = Source_vel; return;
		}
		*mod = Source_none;
	}

	bool isModUnipolar(ModSource_t modsourcetype)
	{
		switch (modsourcetype)
		{
			case ModSource_t::Source_Envelope: return true;
		}
		return false;
	}

	void GetModSourceName(int id, char *txt, int len)
	{
		int instance = 0;
		ModSource_t mod = Source_none;

		RowIntToModSourceAndInstance(id, &mod, &instance);

		switch (mod)
		{
			case Source_LFO:snprintf(txt, len, "LFO %d", instance + 1); return;
			case Source_Envelope:snprintf(txt, len, "Envelope %d", instance + 1); return;
			case Source_x:snprintf(txt, len, "X"); return;
			case Source_y:snprintf(txt, len, "Y"); return;
			case Source_z:snprintf(txt, len, "Z"); return;
			case Source_zprime:snprintf(txt, len, "Z'"); return;
			case Source_left_mod:snprintf(txt, len, "Left pad"); return;
			case Source_right_mod:snprintf(txt, len, "Right pad"); return;
			case Source_left_sus:snprintf(txt, len, "Left sustain"); return;
			case Source_right_sus:snprintf(txt, len, "Right sustain"); return;
			case Source_left_unac:snprintf(txt, len, "Left una corda"); return;
			case Source_right_unac:snprintf(txt, len, "Right una corda"); return;
			case Source_note:snprintf(txt, len, "Keyboard"); return;
			case Source_vel:snprintf(txt, len, "Velocity"); return;			
		}
		snprintf(txt, len, "unknown?");
	}

	int GetModMatrixIndex(ModSource_t mod, int instance)
	{
		switch (mod)
		{
		case Source_LFO: return instance;
		case Source_Envelope: return 0x10 + instance;
		case Source_x: return 0x20;
		case Source_y: return 0x21;
		case Source_z: return 0x22;
		case Source_zprime: return 0x23;
		case Source_left_mod: return 0x24;
		case Source_right_mod: return 0x25;
		case Source_left_sus: return 0x26;
		case Source_right_sus: return 0x27;
		case Source_left_unac: return 0x28;
		case Source_right_unac: return 0x29;
		case Source_note: return 0x2a;
		case Source_vel:  return 0x2b;
			//		case Source_pedal: return &modmatrix[0x22];
		}
		return -1;

	}

	ModMatrixRow_t *GetModSourceRow(ModSource_t mod, int instance)
	{
		int idx = GetModMatrixIndex(mod, instance);
		if (idx > -1) return &modmatrix[idx];
		return 0;
	}

	LfoParam_t lfo[NUM_LFOS];

	EnvParam_t env[NUM_ENVS];

//#define NUM_CONTROLLERS (11)

	ControllerParam_t controller[NUM_CONTROLLERS];

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
		int newp = origp + delta ;
		if (newp > 0xffff) *p = 0xffff; else if (newp < 0) *p = 0; else *p = newp;
		
	}
	hsv low;
	hsv high;
	hsv active;
} PACKED;

extern PanPreset_t gCurrentPreset;
#pragma pack(pop)
#endif
