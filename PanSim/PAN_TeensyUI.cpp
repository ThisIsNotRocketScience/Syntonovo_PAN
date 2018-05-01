#pragma GCC diagnostic ignored "-Wswitch"
#pragma GCC diagnostic ignored "-Wwrite-strings"

#include "PanHeader.h"

extern void WriteKnob(int id, uint32_t value);
extern void WriteWithSubKnob(int id, int subid, uint32_t value);
extern void WriteSwitch(int id, int state);
extern void WriteSyncLfo(uint8_t* paramids);
extern void WritePadZero();

void WriteCtrlDepth(int param, int source, uint16_t depth);

PanPreset_t gPresetBank[16];
PanPreset_t gPreset;

bool Teensy_KnobLastValue[__KNOB_COUNT] = { 0 };
bool Teensy_KnobTouched[__KNOB_COUNT] = { 0 };

#define MENU(id,button,name) static int const MenuItemCount_##id = 0
#define ENTRY(name, type, param) + 1
#define CUSTOMENTRY(name, type, param) + 1
#define ENDMENU() ;
#include "PanUiMap.h"
#undef ENDMENU
#undef ENTRY
#undef MENU
#undef CUSTOMENTRY

void SetSwitch(SwitchEnum SwitchID);
bool GetSwitch(SwitchEnum SwitchID);
void ToggleSwitch(SwitchEnum SwitchID);
void ClearSwitch(SwitchEnum SwitchID);


void InitPreset(PanPreset_t& preset)
{
	memset(&preset, 0, sizeof(PanPreset_t));
	preset.Name[0] = 'I';
	preset.Name[1] = 'n';
	preset.Name[2] = 'i';
	preset.Name[3] = 't';
	preset.Name[4] = '\0';

	preset.ctrlmod[0].source = ModSource_t::Source_note;
	preset.ctrlmod[0].target[0].param = Output_VCO1_PITCH;
	preset.ctrlmod[0].target[0].depth = 0x4000;
	preset.ctrlmod[0].target[1].param = Output_VCO2_PITCH;
	preset.ctrlmod[0].target[1].depth = 0x4000;
	preset.ctrlmod[0].target[2].param = Output_VCO3_PITCH;
	preset.ctrlmod[0].target[2].depth = 0x4000;
	preset.ctrlmod[0].target[3].param = Output_VCO4_PITCH;
	preset.ctrlmod[0].target[3].depth = 0x4000;
	preset.ctrlmod[0].target[4].param = Output_VCO5_PITCH;
	preset.ctrlmod[0].target[4].depth = 0x4000;
	preset.ctrlmod[0].target[5].param = Output_VCO6_PITCH;
	preset.ctrlmod[0].target[5].depth = 0x4000;
	preset.ctrlmod[0].target[6].param = Output_VCO7_PITCH;
	preset.ctrlmod[0].target[6].depth = 0x4000;

	preset.adsrmod[0].a = 0x5;
	preset.adsrmod[0].d = 0x200;
	preset.adsrmod[0].s = 0x4000;
	preset.adsrmod[0].r = 0x20;

#define REGULARENV
//#define ZDEPTHMOD 

#ifdef REGULARENV
	preset.adsrmod[0].target[0].param = Output_VCF1_LIN;
	preset.adsrmod[0].target[0].depth = 0x4000;
	preset.adsrmod[0].target[1].param = Output_VCF2_LIN;
	preset.adsrmod[0].target[1].depth = 0x4000;
	preset.adsrmod[0].target[2].param = Output_CLEANF_LIN;
	preset.adsrmod[0].target[2].depth = 0x4000;
#endif

#ifdef ZDEPTHMOD
	preset.ctrlmod[1].source = ModSource_t::Source_z;
	preset.ctrlmod[1].target[0].param = Output_VCF1_LIN;
	preset.ctrlmod[1].target[0].depth = 0x7fff;
	preset.ctrlmod[1].target[1].param = Output_VCF2_LIN;
	preset.ctrlmod[1].target[1].depth = 0x7fff;
	preset.ctrlmod[1].target[2].param = Output_CLEANF_LIN;
	preset.ctrlmod[1].target[2].depth = 0x7fff;
#endif

	preset.paramvalue[Output_VCO1_LEVEL] = 0xffff;

	preset.paramvalue[Output_VCO1_VCFMIX] = 0x8000;
	preset.paramvalue[Output_VCO2_VCFMIX] = 0x8000;
	preset.paramvalue[Output_VCO3_VCFMIX] = 0x8000;
	preset.paramvalue[Output_RM1_VCFMIX] = 0x8000;
	preset.paramvalue[Output_WHITENS_VCFMIX] = 0x8000;
	preset.paramvalue[Output_DIGINS_VCFMIX] = 0x8000;
	preset.paramvalue[Output_EXT_VCFMIX] = 0x8000;
	preset.paramvalue[Output_VCO4567_VCFMIX] = 0x8000;

	preset.paramvalue[Output_VCF1_LEVEL] = 0xffff;
	preset.paramvalue[Output_VCF1_CV] = 0x6fff;

	preset.paramvalue[Output_VCF1_PAN] = 0x8000;
	preset.paramvalue[Output_VCF2_PAN] = 0x8000;
	preset.paramvalue[Output_CLEANF_PAN] = 0x8000;

	preset.paramvalue[Output_VCO1_PITCH] = 0x8000;
	preset.paramvalue[Output_VCO2_PITCH] = 0x8000;
	preset.paramvalue[Output_VCO3_PITCH] = 0x8000;
	preset.paramvalue[Output_VCO4_PITCH] = 0x8000;
	preset.paramvalue[Output_VCO5_PITCH] = 0x8000;
	preset.paramvalue[Output_VCO6_PITCH] = 0x8000;
	preset.paramvalue[Output_VCO7_PITCH] = 0x8000;
	
	preset.paramvalue[Output_MASTER_LEVEL] = 0xFFFF;

	preset.paramvalue[Output_X_DEADZONE] = 0x200;
	preset.paramvalue[Output_Y_DEADZONE] = 0x200;
	preset.paramvalue[Output_Z_DEADZONE] = 0x200;

	preset.paramvalue[Output_X_SCALE] = 0x2000;
	preset.paramvalue[Output_Y_SCALE] = 0x2000;
	preset.paramvalue[Output_Z_SCALE] = 0x6000;

	preset.paramvalue[Output_MODL_SCALE] = 0x2000;
	preset.paramvalue[Output_SUSL_SCALE] = 0x2000;
	preset.paramvalue[Output_UNACL_SCALE] = 0x2000;
	preset.paramvalue[Output_MODR_SCALE] = 0x2000;
	preset.paramvalue[Output_SUSR_SCALE] = 0x2000;
	preset.paramvalue[Output_UNACR_SCALE] = 0x2000;

	preset.paramvalue[Output_ZPRIME_SPEED] = 0xa000;

	preset.paramvalue[Output_MASTER_PITCH] = 0x8000;
	preset.paramvalue[Output_MASTER_PITCH2] = 0x8000;

	preset.switches[0] |= (1 << Switch_SELEF4);


	preset.switches[0] |= (1 << Switch_SEL1SAW);

	preset.switches[0] |= (1 << Switch_SEL4SAW);
	preset.switches[0] |= (1 << Switch_SEL5SAW);
	preset.switches[0] |= (1 << Switch_SEL6SAW);
	preset.switches[0] |= (1 << Switch_SEL7SAW);

	preset.switches[0] |= (1 << Switch_SEL2SAW);

	preset.switches[0] |= (1 << Switch_SEL3SAW);
	preset.switches[0] |= (1 << Switch_SEL3SQR);
	preset.switches[0] |= (1 << Switch_SEL1SQR);

	preset.switches[0] |= (1 << Switch_SELVCF2L0);
	preset.switches[0] |= (1 << Switch_SELVCF2L1);

	preset.switches[0] |= (1 << Switch_SELMOST2);
	preset.switches[0] |= (1 << Switch_SELMOST3);
}

void SyncLfo(PanPreset_t& preset, int i)
{
	uint8_t synclfo[16] = { 0 };
	for (int j = 0; j < 16; j++) {
		int param = preset.lfomod[i].target[j].param;
		if (param == 0) continue;
		synclfo[j] = param;
	}
	WriteSyncLfo(synclfo);
}

void WriteLfo(PanPreset_t& preset, int i)
{
	uint8_t synclfo[16] = { 0 };
	for (int j = 0; j < 16; j++) {
		int param = preset.lfomod[i].target[j].param;
		if (param == 0) continue;
		synclfo[j] = param;
		WriteWithSubKnob(param, Sub_lfo_depth, preset.lfomod[i].target[j].depth);
		WriteWithSubKnob(param, Sub_lfo_speed, preset.lfomod[i].speed);
		WriteWithSubKnob(param, Sub_lfo_shape, preset.lfomod[i].shape);
//		WriteWithSubKnob(param, Sub_flags, preset.lfomod[i].shape);
	}
	WriteSyncLfo(synclfo);
}

void WriteAdsr(PanPreset_t& preset, int i)
{
	for (int j = 0; j < 16; j++) {
		int param = preset.adsrmod[i].target[j].param;
		if (param == 0) continue;
		WriteWithSubKnob(param, Sub_adsr_depth, preset.adsrmod[i].target[j].depth);
		WriteWithSubKnob(param, Sub_adsr_a, preset.adsrmod[i].a);
		WriteWithSubKnob(param, Sub_adsr_d, preset.adsrmod[i].d);
		WriteWithSubKnob(param, Sub_adsr_s, preset.adsrmod[i].s);
		WriteWithSubKnob(param, Sub_adsr_r, preset.adsrmod[i].r);
	}
}

void WriteAd(PanPreset_t& preset, int i)
{
	for (int j = 0; j < 16; j++) {
		int param = preset.admod[i].target[j].param;
		if (param == 0) continue;
		WriteWithSubKnob(param, Sub_ad_depth, preset.admod[i].target[j].depth);
		WriteWithSubKnob(param, Sub_ad_a, preset.admod[i].a);
		WriteWithSubKnob(param, Sub_ad_d, preset.admod[i].d);
	}
}

void WriteCtrl(PanPreset_t& preset, int i)
{
	for (int j = 0; j < 16; j++) {
		int param = preset.ctrlmod[i].target[j].param;
		if (param == 0) continue;

		WriteCtrlDepth(param, preset.ctrlmod[i].source, preset.ctrlmod[i].target[j].depth);
	}
}

void LoadPreset(PanPreset_t& preset)
{
#define OUTPUT(name,codecport,codecpin, type,id, style,defaultvalue) \
	WriteKnob(id, preset.paramvalue[id]); \
	WriteWithSubKnob(id, Sub_lfo_depth, 0); \
	WriteWithSubKnob(id, Sub_adsr_depth, 0); \
	WriteWithSubKnob(id, Sub_ad_depth, 0); \
	WriteWithSubKnob(id, Sub_x, 0); \
	WriteWithSubKnob(id, Sub_y, 0); \
	WriteWithSubKnob(id, Sub_z, 0); \
	WriteWithSubKnob(id, Sub_zprime, 0); \
	WriteWithSubKnob(id, Sub_note, 0); \
	WriteWithSubKnob(id, Sub_vel, 0); \
	WriteWithSubKnob(id, Sub_flags, 0);
#define OUTPUT_VIRT(name,codecport,codecpin, type,id, style,defaultvalue) \
	OUTPUT(name,codecport,codecpin,type,id,style,defaultvalue);
#define SWITCH(name,id) \
	WriteSwitch(id, (preset.switches[id/32]>>(id%32)) & 1);
#include "../interface/paramdef.h"
#undef OUTPUT
#undef OUTPUT_VIRT
#undef SWITCH

	for (int i = 0; i < 16; i++) {
		WriteLfo(preset, i);
	}

	for (int i = 0; i < 16; i++) {
		WriteAdsr(preset, i);
	}

	for (int i = 0; i < 16; i++) {
		WriteAd(preset, i);
	}

	for (int i = 0; i < 16; i++) {
		WriteCtrl(preset, i);
	}

	for (int i = 0; i < __KNOB_COUNT; i++) {
		Teensy_KnobTouched[i] = 0;
	}

	Raspberry_SetSwitches(preset.switches);
	Raspberry_SetName(preset.Name);
}

void PresetChangeValue(PanPreset_t& preset, int param, uint16_t value)
{
	preset.paramvalue[param] = value;
	WriteKnob(param, value);
}

void PresetChangeAdsrParam(PanPreset_t& preset, int mod, SubParam_t subparam, uint16_t value)
{
	switch (subparam) {
	case Sub_adsr_a:
	{
		preset.adsrmod[mod].a = value;
		for (int i = 0; i < 16; i++) {
			int param = preset.adsrmod[mod].target[i].param;
			if (param == 0) continue;
			WriteWithSubKnob(param, Sub_adsr_a, value);
		}
		break;
	}
	case Sub_adsr_d:
	{
		preset.adsrmod[mod].d = value;
		for (int i = 0; i < 16; i++) {
			int param = preset.adsrmod[mod].target[i].param;
			if (param == 0) continue;
			WriteWithSubKnob(param, Sub_adsr_d, value);
		}
		break;
	}
	case Sub_adsr_s:
	{
		preset.adsrmod[mod].s = value;
		for (int i = 0; i < 16; i++) {
			int param = preset.adsrmod[mod].target[i].param;
			if (param == 0) continue;
			WriteWithSubKnob(param, Sub_adsr_s, value);
		}
		break;
	}
	case Sub_adsr_r:
	{
		preset.adsrmod[mod].r = value;
		for (int i = 0; i < 16; i++) {
			int param = preset.adsrmod[mod].target[i].param;
			if (param == 0) continue;
			WriteWithSubKnob(param, Sub_adsr_r, value);
		}
		break;
	}
	}
}

void PresetChangeAdsrDepth(PanPreset_t& preset, int mod, int index, uint16_t value)
{
	preset.adsrmod[mod].target[index].depth = value;
	WriteWithSubKnob(preset.adsrmod[mod].target[index].param, Sub_adsr_depth, value);
	Raspberry_EditAdsr(preset.adsrmod[mod]);
}

int PresetChangeAdsrAddParam(PanPreset_t& preset, int mod, int param, int depth)
{
	int emptyindex;
	for (emptyindex = 0; emptyindex < 16; emptyindex++) {
		if (preset.adsrmod[mod].target[emptyindex].param == 0) break;
	}
	if (emptyindex == 16) return -1;
	preset.adsrmod[mod].target[emptyindex].param = param;
	preset.adsrmod[mod].target[emptyindex].depth = depth;
	WriteWithSubKnob(param, Sub_adsr_a, preset.adsrmod[mod].a);
	WriteWithSubKnob(param, Sub_adsr_d, preset.adsrmod[mod].d);
	WriteWithSubKnob(param, Sub_adsr_s, preset.adsrmod[mod].s);
	WriteWithSubKnob(param, Sub_adsr_r, preset.adsrmod[mod].r);
	WriteWithSubKnob(param, Sub_adsr_depth, depth);
	Raspberry_EditAdsr(preset.adsrmod[mod]);
	return emptyindex;
}

int PresetChangeAdsrRemoveParam(PanPreset_t& preset, int mod, int param)
{
	for (int i = 0; i < 16; i++)
	{
		if (preset.adsrmod[mod].target[i].param == param) {
			preset.adsrmod[mod].target[i].param = 0;
			WriteWithSubKnob(param, Sub_adsr_depth, 0);
			Raspberry_EditAdsr(preset.adsrmod[mod]);

			return i;
		}
	}
	return -1;
}

void PresetChangeAdParam(PanPreset_t& preset, int mod, SubParam_t subparam, uint16_t value)
{
	switch (subparam) {
	case Sub_ad_a:
	{
		preset.admod[mod].a = value;
		for (int i = 0; i < 16; i++) {
			int param = preset.admod[mod].target[i].param;
			if (param == 0) continue;
			WriteWithSubKnob(param, Sub_ad_a, value);
		}
		break;
	}
	case Sub_ad_d:
	{
		preset.admod[mod].d = value;
		for (int i = 0; i < 16; i++) {
			int param = preset.admod[mod].target[i].param;
			if (param == 0) continue;
			WriteWithSubKnob(param, Sub_ad_d, value);
		}
		break;
	}
	}

	Raspberry_EditAd(preset.admod[mod]);
}

void PresetChangeAdDepth(PanPreset_t& preset, int mod, int index, uint16_t value)
{
	preset.admod[mod].target[index].depth = value;
	WriteWithSubKnob(preset.admod[mod].target[index].param, Sub_ad_depth, value);
	Raspberry_EditAd(preset.admod[mod]);
}

int PresetChangeAdAddParam(PanPreset_t& preset, int mod, int param, int depth)
{
	int emptyindex;
	for (emptyindex = 0; emptyindex < 16; emptyindex++) {
		if (preset.admod[mod].target[emptyindex].param == 0) break;
	}
	if (emptyindex == 16) return -1;
	preset.admod[mod].target[emptyindex].param = param;
	preset.admod[mod].target[emptyindex].depth = depth;
	WriteWithSubKnob(param, Sub_ad_a, preset.admod[mod].a);
	WriteWithSubKnob(param, Sub_ad_d, preset.admod[mod].d);
	WriteWithSubKnob(param, Sub_ad_depth, depth);
	Raspberry_EditAd(preset.admod[mod]);
	return emptyindex;
}

int PresetChangeAdRemoveParam(PanPreset_t& preset, int mod, int param)
{
	for (int i = 0; i < 16; i++)
	{
		if (preset.admod[mod].target[i].param == param) {
			preset.admod[mod].target[i].param = 0;
			WriteWithSubKnob(param, Sub_ad_depth, 0);
			Raspberry_EditAd(preset.admod[mod]);
			return i;
		}
	}

	return -1;
}

void PresetChangeLfoParam(PanPreset_t& preset, int mod, SubParam_t subparam, uint16_t value)
{
	switch (subparam) {
	case Sub_lfo_speed:
	{
		preset.lfomod[mod].speed = value;
		for (int i = 0; i < 16; i++) {
			int param = preset.lfomod[mod].target[i].param;
			if (param == 0) continue;
			WriteWithSubKnob(param, Sub_lfo_speed, value);
		}
		break;
	}
	case Sub_lfo_shape:
	{
		preset.lfomod[mod].shape = value;
		for (int i = 0; i < 16; i++) {
			int param = preset.lfomod[mod].target[i].param;
			if (param == 0) continue;
			WriteWithSubKnob(param, Sub_lfo_shape, value);
		}
		break;
	}
	}
	Raspberry_EditLfo(preset.lfomod[mod]);
}

void PresetChangeLfoDepth(PanPreset_t& preset, int mod, int index, uint16_t value)
{
	preset.lfomod[mod].target[index].depth = value;
	WriteWithSubKnob(preset.lfomod[mod].target[index].param, Sub_lfo_depth, value);
	Raspberry_EditLfo(preset.lfomod[mod]);

}

int PresetChangeLfoAddParam(PanPreset_t& preset, int mod, int param, int depth)
{
	int emptyindex;
	for (emptyindex = 0; emptyindex < 16; emptyindex++) {
		if (preset.lfomod[mod].target[emptyindex].param == 0) break;
	}
	if (emptyindex == 16) return -1;
	preset.lfomod[mod].target[emptyindex].param = param;
	preset.lfomod[mod].target[emptyindex].depth = depth;
	WriteWithSubKnob(param, Sub_lfo_speed, preset.lfomod[mod].speed);
	WriteWithSubKnob(param, Sub_lfo_shape, preset.lfomod[mod].shape);
	WriteWithSubKnob(param, Sub_lfo_depth, depth);
	Raspberry_EditLfo(preset.lfomod[mod]);

	return emptyindex;
}

int PresetChangeLfoRemoveParam(PanPreset_t& preset, int mod, int param)
{
	for (int i = 0; i < 16; i++)
	{
		if (preset.lfomod[mod].target[i].param == param) {
			preset.lfomod[mod].target[i].param = 0;
			WriteWithSubKnob(param, Sub_lfo_depth, 0);
			Raspberry_EditLfo(preset.lfomod[mod]);

			return i;
		}
	}
	return -1;
}

void WriteCtrlDepth(int param, int source, uint16_t depth)
{
	switch (source)
	{
	case ModSource_t::Source_left_mod:
		WriteWithSubKnob(param, Sub_modpad_l, depth);
		break;
	case ModSource_t::Source_right_mod:
		WriteWithSubKnob(param, Sub_modpad_r, depth);
		break;
	case ModSource_t::Source_x:
		WriteWithSubKnob(param, Sub_x, depth);
		break;
	case ModSource_t::Source_y:
		WriteWithSubKnob(param, Sub_y, depth);
		break;
	case ModSource_t::Source_z:
		WriteWithSubKnob(param, Sub_z, depth);
		break;
	case ModSource_t::Source_zprime:
		WriteWithSubKnob(param, Sub_zprime, depth);
		break;
	case ModSource_t::Source_note:
		WriteWithSubKnob(param, Sub_note, depth);
		break;
	case ModSource_t::Source_vel:
		WriteWithSubKnob(param, Sub_vel, depth);
		break;
	case ModSource_t::Source_left_sus:
		WriteWithSubKnob(param, Sub_suspad_l, depth);
		break;
	case ModSource_t::Source_right_sus:
		WriteWithSubKnob(param, Sub_suspad_r, depth);
		break;
	case ModSource_t::Source_left_unac:
		WriteWithSubKnob(param, Sub_unacpad_l, depth);
		break;
	case ModSource_t::Source_right_unac:
		WriteWithSubKnob(param, Sub_unacpad_r, depth);
		break;
	}


}

void PresetChangeCtrlSource(PanPreset_t& preset, int mod, ModSource_t value)
{
	int oldsource = preset.ctrlmod[mod].source;
	preset.ctrlmod[mod].source = value;
	for (int i = 0; i < 16; i++)
	{
		int param = preset.ctrlmod[mod].target[i].param;
		WriteCtrlDepth(param, oldsource, 0);
		WriteCtrlDepth(param, value, preset.ctrlmod[mod].target[i].depth);


	}
	Raspberry_EditCtrl(preset.ctrlmod[mod]);
}

void PresetChangeCtrlDepth(PanPreset_t& preset, int mod, int index, uint16_t value)
{
	preset.ctrlmod[mod].target[index].depth = value;
	WriteCtrlDepth(preset.ctrlmod[mod].target[index].param, preset.ctrlmod[mod].source, value);
	Raspberry_EditCtrl(preset.ctrlmod[mod]);

}

int PresetChangeCtrlAddParam(PanPreset_t& preset, int mod, int param, int depth)
{
	int emptyindex;
	for (emptyindex = 0; emptyindex < 16; emptyindex++) {
		if (preset.ctrlmod[mod].target[emptyindex].param == 0) break;
	}
	if (emptyindex == 16) return -1;
	preset.ctrlmod[mod].target[emptyindex].param = param;
	preset.ctrlmod[mod].target[emptyindex].depth = depth;
	WriteCtrlDepth(param, preset.ctrlmod[mod].source, depth);
	Raspberry_EditCtrl(preset.ctrlmod[mod]);

	return emptyindex;
}

int PresetChangeCtrlRemoveParam(PanPreset_t& preset, int mod, int param)
{
	for (int i = 0; i < 16; i++)
	{
		if (preset.ctrlmod[mod].target[i].param == param) {
			preset.ctrlmod[mod].target[i].param = 0;
			WriteCtrlDepth(param, preset.ctrlmod[mod].source, 0);
			Raspberry_EditCtrl(preset.ctrlmod[mod]);

			return i;
		}
	}
	return -1;
}

void Teensy_InitPreset()
{
	InitPreset(gPreset);
	LoadPreset(gPreset);
}

void Teensy_ReSendPreset()
{
	LoadPreset(gPreset);
}

typedef struct
{
	int modid;
	ModTarget_t target;
} Teensy_CancelStack_t;

typedef struct
{
	GuiState_t GuiState;
	int ModSelect;
	int TargetSelect;

	int lastAddedLedButton;
	int lastAddedTarget;
	int lastAddedFuncIndex;

	// Old data of selected modulation
	char cancel[128];
	int CancelStackSize;
	Teensy_CancelStack_t CancelStack[128];
} Teensy_GuiData_t;

static Teensy_GuiData_t Teensy_guidata;

ModSource_t LedButtonToSource(int ledbuttonid)
{
	switch (ledbuttonid)
	{
	case ledbutton_LEFT_MOD: return ModSource_t::Source_left_mod;
	case ledbutton_RIGHT_MOD: return ModSource_t::Source_right_mod;
	case ledbutton_X: return ModSource_t::Source_x;
	case ledbutton_Y: return ModSource_t::Source_y;
	case ledbutton_Z: return ModSource_t::Source_z;
	case ledbutton_ZPRIME: return ModSource_t::Source_zprime;
	case ledbutton_KBCV: return ModSource_t::Source_note;
	case ledbutton_VELOCITY: return ModSource_t::Source_vel;
	case ledbutton_LEFT_SUS: return ModSource_t::Source_left_sus;
	case ledbutton_RIGHT_SUS: return ModSource_t::Source_right_sus;
	case ledbutton_LEFT_UNAC: return ModSource_t::Source_left_unac;
	case ledbutton_RIGHT_UNAC: return ModSource_t::Source_right_unac;
	}

	return ModSource_t::Source_none;
}

int SourceToLedButton(ModSource_t source)
{
	switch (source)
	{
	case ModSource_t::Source_left_mod: return ledbutton_LEFT_MOD;
	case ModSource_t::Source_right_mod: return ledbutton_RIGHT_MOD;
	case ModSource_t::Source_x: return ledbutton_X;
	case ModSource_t::Source_y: return ledbutton_Y;
	case ModSource_t::Source_z: return ledbutton_Z;
	case ModSource_t::Source_zprime: return ledbutton_ZPRIME;
	case ModSource_t::Source_note: return ledbutton_KBCV;
	case ModSource_t::Source_vel: return ledbutton_VELOCITY;
	case ModSource_t::Source_left_sus: return ledbutton_LEFT_SUS;
	case ModSource_t::Source_right_sus: return ledbutton_RIGHT_SUS;
	case ModSource_t::Source_left_unac: return ledbutton_LEFT_UNAC;
	case ModSource_t::Source_right_unac: return ledbutton_RIGHT_UNAC;
	}

	return -1;
}

void UpdateMenuButtons()
{
#define EXTRABUTTON(id, button) if (Teensy_guidata.GuiState == GuiState_Menu_##id) SetLedButton(button, LED_ON);
#define MENU(id, button, name) \
	if (Teensy_guidata.GuiState == GuiState_Menu_##id) { \
		SetLedButton(button, LED_ON);SetLedButton(ledbutton_CANCEL_LEFT, LED_ON);SetLedButton(ledbutton_CANCEL_RIGHT, LED_ON);
#define ENDMENU() \
		return; \
	}
#include "PanUiMap.h"
#undef MENU
#undef ENDMENU
#undef EXTRABUTTON
}

void SetLedButton(int id, int mode)
{
	if (id < 0 || id >= __LEDBUTTON_COUNT)
	{
		return;
	}
	if (mode > 0)
	{
		Buttons[id].value = true;
		Buttons[id].ledmode = mode;
	}
	else
	{
		Buttons[id].ledmode = LED_OFF;
		Buttons[id].value = false;
	}
}

void UpdateTargets()
{
	for (int i = 0; i < __LEDBUTTON_COUNT; i++)
	{
		SetLedButton(i, LED_OFF);	
	}
	SetLedButton(ledbutton_VCF1_Routing, GetSwitch(Switch_SELVCF2POST)?LED_ON:LED_OFF);
	int ActivePresetButton = Raspberry_guidata.activebank * 8 + Raspberry_guidata.activeslot;

	SetLedButton(ledbutton_LEFT_1, ActivePresetButton == 0 ? LED_ON : LED_OFF);
	SetLedButton(ledbutton_LEFT_2, ActivePresetButton == 1 ? LED_ON : LED_OFF);
	SetLedButton(ledbutton_LEFT_3, ActivePresetButton == 2 ? LED_ON : LED_OFF);
	SetLedButton(ledbutton_LEFT_4, ActivePresetButton == 3 ? LED_ON : LED_OFF);
	SetLedButton(ledbutton_LEFT_5, ActivePresetButton == 4 ? LED_ON : LED_OFF);
	SetLedButton(ledbutton_LEFT_6, ActivePresetButton == 5 ? LED_ON : LED_OFF);
	SetLedButton(ledbutton_LEFT_7, ActivePresetButton == 6 ? LED_ON : LED_OFF);
	SetLedButton(ledbutton_LEFT_8, ActivePresetButton == 7 ? LED_ON : LED_OFF);
	SetLedButton(ledbutton_RIGHT_1, ActivePresetButton == 8 ? LED_ON : LED_OFF);
	SetLedButton(ledbutton_RIGHT_2, ActivePresetButton == 9 ? LED_ON : LED_OFF);
	SetLedButton(ledbutton_RIGHT_3, ActivePresetButton == 10 ? LED_ON : LED_OFF);
	SetLedButton(ledbutton_RIGHT_4, ActivePresetButton == 11 ? LED_ON : LED_OFF);
	SetLedButton(ledbutton_RIGHT_5, ActivePresetButton == 12 ? LED_ON : LED_OFF);
	SetLedButton(ledbutton_RIGHT_6, ActivePresetButton == 13 ? LED_ON : LED_OFF);
	SetLedButton(ledbutton_RIGHT_7, ActivePresetButton == 14 ? LED_ON : LED_OFF);
	SetLedButton(ledbutton_RIGHT_8, ActivePresetButton == 15 ? LED_ON : LED_OFF);


	switch (Teensy_guidata.GuiState) {
	case GuiState_Menu_EFFECTS1:
	
	SetLedButton(ledbutton_Cleanfeed_FX, LED_ON);
	SetLedButton(ledbutton_VCF1_FX, LED_ON);
	SetLedButton(ledbutton_VCF2_FX, LED_ON);
	break;
	case GuiState_LfoSelect:
		SetLedButton(ledbutton_CANCEL_RIGHT, LED_ON);
		SetLedButton(ledbutton_CANCEL_LEFT, LED_ON);
		SetLedButton(ledbutton_FINAL, LED_ON);
		SetLedButton(ledbutton_LFO, LED_ON);
		if (Teensy_guidata.ModSelect != -1) {			
			SetLedButton(ledbutton_SOURCE_LFO, LED_ON);
		}
		break;
	case GuiState_AdsrSelect:
		SetLedButton(ledbutton_CANCEL_RIGHT, LED_ON);
		SetLedButton(ledbutton_CANCEL_LEFT, LED_ON);
		SetLedButton(ledbutton_FINAL, LED_ON);
		SetLedButton(ledbutton_ADSR, LED_ON);
		if (Teensy_guidata.ModSelect != -1) {
			SetLedButton(ledbutton_SOURCE_ADSR, LED_ON);
			
		}
		break;
	case GuiState_AdSelect:
		SetLedButton(ledbutton_CANCEL_RIGHT, LED_ON);
		SetLedButton(ledbutton_CANCEL_LEFT, LED_ON);
		SetLedButton(ledbutton_FINAL, LED_ON);
		SetLedButton(ledbutton_AD, LED_ON);
		if (Teensy_guidata.ModSelect != -1) {
			SetLedButton(ledbutton_SOURCE_AD, LED_ON);
			
		}
		break;
	case GuiState_CtrlSelect:
		SetLedButton(ledbutton_CANCEL_RIGHT, LED_ON);
		SetLedButton(ledbutton_CANCEL_LEFT, LED_ON);
		SetLedButton(ledbutton_FINAL, LED_ON);

		SetLedButton(SourceToLedButton(gPreset.ctrlmod[Teensy_guidata.ModSelect].source), LED_ON);
		
		break;
	case GuiState_InitPatch:
		SetLedButton(ledbutton_CANCEL_RIGHT, LED_ON);
		SetLedButton(ledbutton_FINAL, LED_ON);
		break;

	case GuiState_SelectBanks:
		SetLedButton(ledbutton_LEFT_MORE, LED_ON);
		SetLedButton(ledbutton_RIGHT_MORE, LED_ON);

		SetLedButton(ledbutton_LEFT_1, Raspberry_guidata.banks[0] == 0 ? LED_ON : LED_BLINK);
		SetLedButton(ledbutton_LEFT_2, Raspberry_guidata.banks[0] == 1 ? LED_ON : LED_BLINK);
		SetLedButton(ledbutton_LEFT_3, Raspberry_guidata.banks[0] == 2 ? LED_ON : LED_BLINK);
		SetLedButton(ledbutton_LEFT_4, Raspberry_guidata.banks[0] == 3 ? LED_ON : LED_BLINK);
		SetLedButton(ledbutton_LEFT_5, Raspberry_guidata.banks[0] == 4 ? LED_ON : LED_BLINK);
		SetLedButton(ledbutton_LEFT_6, Raspberry_guidata.banks[0] == 5 ? LED_ON : LED_BLINK);
		SetLedButton(ledbutton_LEFT_7, Raspberry_guidata.banks[0] == 6 ? LED_ON : LED_BLINK);
		SetLedButton(ledbutton_LEFT_8, Raspberry_guidata.banks[0] == 7 ? LED_ON : LED_BLINK);
		SetLedButton(ledbutton_RIGHT_1, Raspberry_guidata.banks[1] == 0 ? LED_ON : LED_BLINK);
		SetLedButton(ledbutton_RIGHT_2, Raspberry_guidata.banks[1] == 1 ? LED_ON : LED_BLINK);
		SetLedButton(ledbutton_RIGHT_3, Raspberry_guidata.banks[1] == 2 ? LED_ON : LED_BLINK);
		SetLedButton(ledbutton_RIGHT_4, Raspberry_guidata.banks[1] == 3 ? LED_ON : LED_BLINK);
		SetLedButton(ledbutton_RIGHT_5, Raspberry_guidata.banks[1] == 4 ? LED_ON : LED_BLINK);
		SetLedButton(ledbutton_RIGHT_6, Raspberry_guidata.banks[1] == 5 ? LED_ON : LED_BLINK);
		SetLedButton(ledbutton_RIGHT_7, Raspberry_guidata.banks[1] == 6 ? LED_ON : LED_BLINK);
		SetLedButton(ledbutton_RIGHT_8, Raspberry_guidata.banks[1] == 7 ? LED_ON : LED_BLINK);

		break;
	case GuiState_SavePreset:
		SetLedButton(ledbutton_CANCEL_RIGHT, LED_ON);
		SetLedButton(ledbutton_CANCEL_LEFT, LED_ON);

		SetLedButton(ledbutton_LEFT_1,  LED_BLINK);
		SetLedButton(ledbutton_LEFT_2,  LED_BLINK);
		SetLedButton(ledbutton_LEFT_3,  LED_BLINK);
		SetLedButton(ledbutton_LEFT_4,  LED_BLINK);
		SetLedButton(ledbutton_LEFT_5,  LED_BLINK);
		SetLedButton(ledbutton_LEFT_6,  LED_BLINK);
		SetLedButton(ledbutton_LEFT_7,  LED_BLINK);
		SetLedButton(ledbutton_LEFT_8,  LED_BLINK);
		SetLedButton(ledbutton_RIGHT_1, LED_BLINK);
		SetLedButton(ledbutton_RIGHT_2, LED_BLINK);
		SetLedButton(ledbutton_RIGHT_3, LED_BLINK);
		SetLedButton(ledbutton_RIGHT_4, LED_BLINK);
		SetLedButton(ledbutton_RIGHT_5, LED_BLINK);
		SetLedButton(ledbutton_RIGHT_6, LED_BLINK);
		SetLedButton(ledbutton_RIGHT_7, LED_BLINK);
		SetLedButton(ledbutton_RIGHT_8, LED_BLINK);

		break;
	case GuiState_Root:
		SetLedButton(ledbutton_FINAL, LED_ON);
		UpdateMenuButtons();
	default:

		UpdateMenuButtons();
		return;
	}

	// Modulation buttons
	switch (Teensy_guidata.GuiState)
	{
	case GuiState_Root:
		SetLedButton(ledbutton_FINAL, LED_ON);
		break;
	case GuiState_LfoSelect:
	{
		if (Teensy_guidata.ModSelect >= 0) {
			for (int j = 0; j < 16; j++)
			{
				int but = ParamToButton(gPreset.lfomod[Teensy_guidata.ModSelect].target[j].param);
				if (but >= 0) {
					SetLedButton(but, LED_ON);
					
				}
			}
		}
		else {
			for (int i = 0; i < 16; i++)
			{
				for (int j = 0; j < 16; j++)
				{
					int but = ParamToButton(gPreset.lfomod[i].target[j].param);
					if (but >= 0) {
						SetLedButton(but, LED_ON);
						
					}
				}
			}
		}
	}
	break;
	case GuiState_AdsrSelect:
	{
		if (Teensy_guidata.ModSelect >= 0) {
			for (int j = 0; j < 16; j++)
			{
				int but = ParamToButton(gPreset.adsrmod[Teensy_guidata.ModSelect].target[j].param);
				if (but >= 0) {
					SetLedButton(but, LED_ON);
					
				}
			}
		}
		else {
			for (int i = 0; i < 16; i++)
			{
				for (int j = 0; j < 16; j++)
				{
					int but = ParamToButton(gPreset.adsrmod[i].target[j].param);
					if (but >= 0) {
						SetLedButton(but, LED_ON);
					}
				}
			}
		}
	}
	break;
	case GuiState_AdSelect:
	{
		if (Teensy_guidata.ModSelect >= 0) {
			for (int j = 0; j < 16; j++)
			{
				int but = ParamToButton(gPreset.admod[Teensy_guidata.ModSelect].target[j].param);
				if (but >= 0) {
					SetLedButton(but, LED_ON);
				}
			}
		}
		else {
			for (int i = 0; i < 16; i++)
			{
				for (int j = 0; j < 16; j++)
				{
					int but = ParamToButton(gPreset.admod[i].target[j].param);
					if (but >= 0) {
						SetLedButton(but, LED_ON);
					}
				}
			}
		}
	}
	break;
	case GuiState_CtrlSelect:
	{
		for (int j = 0; j < 16; j++)
		{
			int but = ParamToButton(gPreset.ctrlmod[Teensy_guidata.ModSelect].target[j].param);
			if (but >= 0) {
				SetLedButton(but, LED_ON);
			}
		}
	}
	break;
	}
}

bool IsLfoActive(PanPreset_t& preset, int mod)
{
	for (int i = 0; i < 16; i++)
	{
		if (preset.lfomod[mod].target[i].param != 0) {
			return true;
		}
	}

	return false;
}

int NextLfoMod(PanPreset_t& preset, int mod)
{
	int firstmod = mod;
	if (firstmod < 0) firstmod = 0;
	do
	{
		mod++;
		if (mod == 16) mod = 0;
	} while (mod != firstmod && !IsLfoActive(preset, mod));

	if (mod == firstmod) return -1;
	return mod;
}

bool IsAdsrActive(PanPreset_t& preset, int mod)
{
	for (int i = 0; i < 16; i++)
	{
		if (preset.adsrmod[mod].target[i].param != 0) {
			return true;
		}
	}

	return false;
}

int NextAdsrMod(PanPreset_t& preset, int mod)
{
	int firstmod = mod;
	if (firstmod < 0) firstmod = 0;
	do
	{
		mod++;
		if (mod == 16) mod = 0;
	} while (mod != firstmod && !IsAdsrActive(preset, mod));

	if (mod == firstmod) return -1;
	return mod;
}

bool IsAdActive(PanPreset_t& preset, int mod)
{
	for (int i = 0; i < 16; i++)
	{
		if (preset.admod[mod].target[i].param != 0) {
			return true;
		}
	}

	return false;
}

int NextAdMod(PanPreset_t& preset, int mod)
{
	int firstmod = mod;
	if (firstmod < 0) firstmod = 0;
	do
	{
		mod++;
		if (mod == 16) mod = 0;
	} while (mod != firstmod && !IsAdsrActive(preset, mod));

	if (mod == firstmod) return -1;
	return mod;
}

bool IsCtrlModActive(PanPreset_t& preset, int mod)
{
	for (int i = 0; i < 16; i++)
	{
		if (preset.ctrlmod[mod].target[i].param != 0) {
			return true;
		}
	}

	return false;
}

int CtrlMod(PanPreset_t& preset, ModSource_t source)
{
	int firstEmpty = -1;
	for (int i = 0; i < 16; i++)
	{
		if (firstEmpty == -1 && !IsCtrlModActive(preset, i))
		{
			firstEmpty = i;
		}
		if (gPreset.ctrlmod[i].source == source)
		{
			return i;
		}
	}
	PresetChangeCtrlSource(preset, firstEmpty, source);
	return firstEmpty;
}

bool Teensy_IsModEdit()
{
	if (Teensy_guidata.GuiState == GuiState_LfoSelect
		|| Teensy_guidata.GuiState == GuiState_AdsrSelect
		|| Teensy_guidata.GuiState == GuiState_AdSelect
		|| Teensy_guidata.GuiState == GuiState_CtrlSelect)
	{
		return true;
	}

	return false;
}

void CancelMod()
{
	Teensy_guidata.lastAddedLedButton = -1;
	if (Teensy_guidata.GuiState == GuiState_LfoSelect && Teensy_guidata.ModSelect >= 0) {
		for (int i = 0; i < 16; i++) {
			PresetChangeLfoRemoveParam(gPreset, Teensy_guidata.ModSelect, gPreset.lfomod[Teensy_guidata.ModSelect].target[i].param);
		}
		memcpy(&gPreset.lfomod[Teensy_guidata.ModSelect], &Teensy_guidata.cancel, sizeof(gPreset.lfomod[0]));
		WriteLfo(gPreset, Teensy_guidata.ModSelect);

		int changes[16] = { 0 };
		for (int i = 0; i < Teensy_guidata.CancelStackSize; i++) {
			PresetChangeLfoAddParam(gPreset, Teensy_guidata.CancelStack[i].modid, Teensy_guidata.CancelStack[i].target.param, Teensy_guidata.CancelStack[i].target.depth);
			changes[Teensy_guidata.CancelStack[i].modid] = 1;
		}
		for (int i = 0; i < 16; i++) {
			if (changes[i]) {
				WriteLfo(gPreset, i);
			}
		}
	}
	else if (Teensy_guidata.GuiState == GuiState_AdsrSelect && Teensy_guidata.ModSelect >= 0) {
		for (int i = 0; i < 16; i++) {
			PresetChangeAdsrRemoveParam(gPreset, Teensy_guidata.ModSelect, gPreset.adsrmod[Teensy_guidata.ModSelect].target[i].param);
		}
		memcpy(&gPreset.adsrmod[Teensy_guidata.ModSelect], &Teensy_guidata.cancel, sizeof(gPreset.adsrmod[0]));
		WriteAdsr(gPreset, Teensy_guidata.ModSelect);

		int changes[16] = { 0 };
		for (int i = 0; i < Teensy_guidata.CancelStackSize; i++) {
			PresetChangeAdsrAddParam(gPreset, Teensy_guidata.CancelStack[i].modid, Teensy_guidata.CancelStack[i].target.param, Teensy_guidata.CancelStack[i].target.depth);
			changes[Teensy_guidata.CancelStack[i].modid] = 1;
		}
		for (int i = 0; i < 16; i++) {
			if (changes[i]) {
				WriteAdsr(gPreset, i);
			}
		}
	}
	else if (Teensy_guidata.GuiState == GuiState_AdSelect && Teensy_guidata.ModSelect >= 0) {
		for (int i = 0; i < 16; i++) {
			PresetChangeAdRemoveParam(gPreset, Teensy_guidata.ModSelect, gPreset.admod[Teensy_guidata.ModSelect].target[i].param);
		}
		memcpy(&gPreset.admod[Teensy_guidata.ModSelect], &Teensy_guidata.cancel, sizeof(gPreset.admod[0]));
		WriteAd(gPreset, Teensy_guidata.ModSelect);

		int changes[16] = { 0 };
		for (int i = 0; i < Teensy_guidata.CancelStackSize; i++) {
			PresetChangeAdAddParam(gPreset, Teensy_guidata.CancelStack[i].modid, Teensy_guidata.CancelStack[i].target.param, Teensy_guidata.CancelStack[i].target.depth);
			changes[Teensy_guidata.CancelStack[i].modid] = 1;
		}
		for (int i = 0; i < 16; i++) {
			if (changes[i]) {
				WriteAd(gPreset, i);
			}
		}
	}
	else if (Teensy_guidata.GuiState == GuiState_CtrlSelect && Teensy_guidata.ModSelect >= 0) {
		for (int i = 0; i < 16; i++) {
			PresetChangeCtrlRemoveParam(gPreset, Teensy_guidata.ModSelect, gPreset.ctrlmod[Teensy_guidata.ModSelect].target[i].param);
		}
		memcpy(&gPreset.ctrlmod[Teensy_guidata.ModSelect], &Teensy_guidata.cancel, sizeof(gPreset.ctrlmod[0]));
		WriteCtrl(gPreset, Teensy_guidata.ModSelect);

		int changes[16] = { 0 };
		for (int i = 0; i < Teensy_guidata.CancelStackSize; i++) {
			PresetChangeCtrlAddParam(gPreset, Teensy_guidata.CancelStack[i].modid, Teensy_guidata.CancelStack[i].target.param, Teensy_guidata.CancelStack[i].target.depth);
			changes[Teensy_guidata.CancelStack[i].modid] = 1;
		}
		for (int i = 0; i < 16; i++) {
			if (changes[i]) {
				WriteCtrl(gPreset, i);
			}
		}
	}
}

void SelectMod()
{
	Teensy_guidata.CancelStackSize = 0;
	if (Teensy_guidata.GuiState == GuiState_LfoSelect && Teensy_guidata.ModSelect >= 0) {
		memcpy(&Teensy_guidata.cancel, &gPreset.lfomod[Teensy_guidata.ModSelect], sizeof(gPreset.lfomod[0]));
	}
	else if (Teensy_guidata.GuiState == GuiState_AdsrSelect && Teensy_guidata.ModSelect >= 0) {
		memcpy(&Teensy_guidata.cancel, &gPreset.adsrmod[Teensy_guidata.ModSelect], sizeof(gPreset.adsrmod[0]));
	}
	else if (Teensy_guidata.GuiState == GuiState_AdSelect && Teensy_guidata.ModSelect >= 0) {
		memcpy(&Teensy_guidata.cancel, &gPreset.admod[Teensy_guidata.ModSelect], sizeof(gPreset.admod[0]));
	}
	else if (Teensy_guidata.GuiState == GuiState_CtrlSelect && Teensy_guidata.ModSelect >= 0) {
		memcpy(&Teensy_guidata.cancel, &gPreset.ctrlmod[Teensy_guidata.ModSelect], sizeof(gPreset.ctrlmod[0]));
	}
}

void Teensy_ToState(GuiState_t state, int modselect = -1)
{
	CancelMod();
	if (modselect == -1) {
		bool sameState = Teensy_guidata.GuiState == state;
		if (state == GuiState_LfoSelect) {
			if (sameState) {
				modselect = NextLfoMod(gPreset, Teensy_guidata.ModSelect);
			}
			else {
				modselect = NextLfoMod(gPreset, -1);
			}
		}
		else if (state == GuiState_AdsrSelect) {
			if (sameState) {
				modselect = NextAdsrMod(gPreset, Teensy_guidata.ModSelect);
			}
			else {
				modselect = NextAdsrMod(gPreset, -1);
			}
		}
		else if (state == GuiState_AdSelect) {
			if (sameState) {
				modselect = NextAdMod(gPreset, Teensy_guidata.ModSelect);
			}
			else {
				modselect = NextAdMod(gPreset, -1);
			}
		}
	}

	if (state == GuiState_CtrlSelect && modselect != -1)
	{
		modselect = CtrlMod(gPreset, LedButtonToSource(modselect));
	}

	Teensy_guidata.GuiState = state;
	Teensy_guidata.ModSelect = modselect;
	Teensy_guidata.TargetSelect = -1;

	SelectMod();

	if (modselect != -1) {
		if (state == GuiState_LfoSelect) {
			Raspberry_EditLfo(gPreset.lfomod[modselect]);
		}
		else if (state == GuiState_AdsrSelect) {
			Raspberry_EditAdsr(gPreset.adsrmod[modselect]);
		}
		else if (state == GuiState_AdSelect) {
			Raspberry_EditAd(gPreset.admod[modselect]);
		}
		else if (state == GuiState_CtrlSelect) {
			Raspberry_EditCtrl(gPreset.ctrlmod[modselect]);
		}
	}
	Raspberry_ToState(state, modselect);

	UpdateTargets();
}

int Teensy_FirstEmptyLfoMod()
{
	for (int i = 0; i < 16; i++) {
		if (!IsLfoActive(gPreset, i)) return i;
	}

	return -1;
}

int Teensy_FirstEmptyAdsrMod()
{
	for (int i = 0; i < 16; i++) {
		if (!IsAdsrActive(gPreset, i)) return i;
	}

	return -1;
}

int Teensy_FirstEmptyAdMod()
{
	for (int i = 0; i < 16; i++) {
		if (!IsAdActive(gPreset, i)) return i;
	}

	return -1;
}


int NextTarget(ModTarget_t* targets, int button, int lastsel = -1)
{
	int sel = lastsel;
	for (int i = 0; i < 16; i++) {
		sel++;
		if (sel == 16) sel = 0;
		if (ParamToButton(targets[sel].param) == button)
		{
			return sel;
		}
	}

	return -1;
}

bool Teensy_FindModulation(int ledbutton)
{
	CancelMod();

	if (Teensy_guidata.GuiState == GuiState_LfoSelect) {
		int firstEmpty = -1;
		for (int i = 0; i < 16; i++) {
			bool empty = true;
			for (int j = 0; j < 16; j++) {
				if (gPreset.lfomod[i].target[j].param != 0) {
					empty = false;
				}
				if (ParamToButton(gPreset.lfomod[i].target[j].param) == ledbutton) {
					Teensy_guidata.ModSelect = i;
					Teensy_guidata.TargetSelect = j;
					SelectMod();
					UpdateTargets();
					Raspberry_ToState(Teensy_guidata.GuiState, Teensy_guidata.ModSelect);
					Raspberry_EditLfo(gPreset.lfomod[Teensy_guidata.ModSelect]);
					Raspberry_SelectTarget(Teensy_guidata.TargetSelect);
					return true;
				}
			}
			if (firstEmpty == -1 && empty) firstEmpty = i;
		}
		Teensy_guidata.ModSelect = firstEmpty;
		Teensy_guidata.TargetSelect = -1;
		SelectMod();
		Raspberry_ToState(Teensy_guidata.GuiState, Teensy_guidata.ModSelect);
		Raspberry_EditLfo(gPreset.lfomod[Teensy_guidata.ModSelect]);
		UpdateTargets();
	}
	else if (Teensy_guidata.GuiState == GuiState_AdsrSelect) {
		int firstEmpty = -1;
		for (int i = 0; i < 16; i++) {
			bool empty = true;
			for (int j = 0; j < 16; j++) {
				if (gPreset.adsrmod[i].target[j].param != 0) {
					empty = false;
				}
				if (ParamToButton(gPreset.adsrmod[i].target[j].param) == ledbutton) {
					Teensy_guidata.ModSelect = i;
					Teensy_guidata.TargetSelect = j;
					SelectMod();
					UpdateTargets();
					Raspberry_ToState(Teensy_guidata.GuiState, Teensy_guidata.ModSelect);
					Raspberry_EditAdsr(gPreset.adsrmod[Teensy_guidata.ModSelect]);
					Raspberry_SelectTarget(Teensy_guidata.TargetSelect);
					return true;
				}
			}
			if (firstEmpty == -1 && empty) firstEmpty = i;
		}
		Teensy_guidata.ModSelect = firstEmpty;
		Teensy_guidata.TargetSelect = -1;
		SelectMod();
		Raspberry_ToState(Teensy_guidata.GuiState, Teensy_guidata.ModSelect);
		Raspberry_EditAdsr(gPreset.adsrmod[Teensy_guidata.ModSelect]);
		UpdateTargets();
	}
	else if (Teensy_guidata.GuiState == GuiState_AdSelect) {
		int firstEmpty = -1;
		for (int i = 0; i < 16; i++) {
			bool empty = true;
			for (int j = 0; j < 16; j++) {
				if (gPreset.admod[i].target[j].param != 0) {
					empty = false;
				}
				if (ParamToButton(gPreset.admod[i].target[j].param) == ledbutton) {
					Teensy_guidata.ModSelect = i;
					Teensy_guidata.TargetSelect = j;
					SelectMod();
					UpdateTargets();
					Raspberry_ToState(Teensy_guidata.GuiState, Teensy_guidata.ModSelect);
					Raspberry_EditAd(gPreset.admod[Teensy_guidata.ModSelect]);
					Raspberry_SelectTarget(Teensy_guidata.TargetSelect);
					return true;
				}
			}
			if (firstEmpty == -1 && empty) firstEmpty = i;
		}
		Teensy_guidata.ModSelect = firstEmpty;
		Teensy_guidata.TargetSelect = -1;
		SelectMod();
		Raspberry_ToState(Teensy_guidata.GuiState, Teensy_guidata.ModSelect);
		Raspberry_EditAd(gPreset.admod[Teensy_guidata.ModSelect]);
		UpdateTargets();
	}

	return false;
}

void Teensy_RestorePrevious(int param)
{
	if (Teensy_guidata.GuiState == GuiState_LfoSelect) {
		PresetChangeLfoRemoveParam(gPreset, Teensy_guidata.ModSelect, param);

		for (int i = 0; i < Teensy_guidata.CancelStackSize; i++) {
			if (Teensy_guidata.CancelStack[i].target.param == param) {
				PresetChangeLfoAddParam(gPreset, Teensy_guidata.CancelStack[i].modid, Teensy_guidata.CancelStack[i].target.param, Teensy_guidata.CancelStack[i].target.depth);
				memmove(&Teensy_guidata.CancelStack[i], &Teensy_guidata.CancelStack[i + 1], (Teensy_guidata.CancelStackSize - i - 1) * sizeof(Teensy_guidata.CancelStack[0]));
				Teensy_guidata.CancelStackSize--;
				return;
			}
		}
	}
	else if (Teensy_guidata.GuiState == GuiState_AdsrSelect) {
		PresetChangeAdsrRemoveParam(gPreset, Teensy_guidata.ModSelect, param);

		for (int i = 0; i < Teensy_guidata.CancelStackSize; i++) {
			if (Teensy_guidata.CancelStack[i].target.param == param) {
				PresetChangeAdsrAddParam(gPreset, Teensy_guidata.CancelStack[i].modid, Teensy_guidata.CancelStack[i].target.param, Teensy_guidata.CancelStack[i].target.depth);
				memmove(&Teensy_guidata.CancelStack[i], &Teensy_guidata.CancelStack[i + 1], (Teensy_guidata.CancelStackSize - i - 1) * sizeof(Teensy_guidata.CancelStack[0]));
				Teensy_guidata.CancelStackSize--;
				return;
			}
		}
	}
	else if (Teensy_guidata.GuiState == GuiState_AdSelect) {
		PresetChangeAdRemoveParam(gPreset, Teensy_guidata.ModSelect, param);

		for (int i = 0; i < Teensy_guidata.CancelStackSize; i++) {
			if (Teensy_guidata.CancelStack[i].target.param == param) {
				PresetChangeAdAddParam(gPreset, Teensy_guidata.CancelStack[i].modid, Teensy_guidata.CancelStack[i].target.param, Teensy_guidata.CancelStack[i].target.depth);
				memmove(&Teensy_guidata.CancelStack[i], &Teensy_guidata.CancelStack[i + 1], (Teensy_guidata.CancelStackSize - i - 1) * sizeof(Teensy_guidata.CancelStack[0]));
				Teensy_guidata.CancelStackSize--;
				return;
			}
		}
	}
}

void Teensy_UnmapPrevious(int param)
{
	if (Teensy_guidata.GuiState == GuiState_LfoSelect) {
		for (int i = 0; i < 16; i++) {
			if (i == Teensy_guidata.ModSelect) continue;

			for (int j = 0; j < 16; j++) {
				if (gPreset.lfomod[i].target[j].param == param) {
					Teensy_guidata.CancelStack[Teensy_guidata.CancelStackSize].modid = i;
					Teensy_guidata.CancelStack[Teensy_guidata.CancelStackSize].target.param = param;
					Teensy_guidata.CancelStack[Teensy_guidata.CancelStackSize].target.depth = gPreset.lfomod[i].target[j].depth;
					Teensy_guidata.CancelStackSize++;
					PresetChangeLfoRemoveParam(gPreset, i, param);
					return;
				}
			}
		}
	}
	else if (Teensy_guidata.GuiState == GuiState_AdsrSelect) {
		for (int i = 0; i < 16; i++) {
			if (i == Teensy_guidata.ModSelect) continue;

			for (int j = 0; j < 16; j++) {
				if (gPreset.adsrmod[i].target[j].param == param) {
					Teensy_guidata.CancelStack[Teensy_guidata.CancelStackSize].modid = i;
					Teensy_guidata.CancelStack[Teensy_guidata.CancelStackSize].target.param = param;
					Teensy_guidata.CancelStack[Teensy_guidata.CancelStackSize].target.depth = gPreset.adsrmod[i].target[j].depth;
					Teensy_guidata.CancelStackSize++;
					PresetChangeAdsrRemoveParam(gPreset, i, param);
					return;
				}
			}
		}
	}
	else if (Teensy_guidata.GuiState == GuiState_AdSelect) {
		for (int i = 0; i < 16; i++) {
			if (i == Teensy_guidata.ModSelect) continue;

			for (int j = 0; j < 16; j++) {
				if (gPreset.admod[i].target[j].param == param) {
					Teensy_guidata.CancelStack[Teensy_guidata.CancelStackSize].modid = i;
					Teensy_guidata.CancelStack[Teensy_guidata.CancelStackSize].target.param = param;
					Teensy_guidata.CancelStack[Teensy_guidata.CancelStackSize].target.depth = gPreset.admod[i].target[j].depth;
					Teensy_guidata.CancelStackSize++;
					PresetChangeAdRemoveParam(gPreset, i, param);
					return;
				}
			}
		}
	}
}

void Teensy_RotateLastAdded()
{
	if (Teensy_guidata.GuiState == GuiState_LfoSelect) {
		Teensy_guidata.lastAddedFuncIndex++;
		int param = ButtonToParam(Teensy_guidata.lastAddedLedButton, Teensy_guidata.lastAddedFuncIndex);
		if (param == -1) {
			param = ButtonToParam(Teensy_guidata.lastAddedLedButton, 0);
			Teensy_guidata.lastAddedFuncIndex = 0;
		}

		int lastParam = gPreset.lfomod[Teensy_guidata.ModSelect].target[Teensy_guidata.lastAddedTarget].param;
		if (param != lastParam)
		{
			uint32_t depth = gPreset.lfomod[Teensy_guidata.ModSelect].target[Teensy_guidata.lastAddedTarget].depth;
			Teensy_RestorePrevious(lastParam);
			Teensy_UnmapPrevious(param);
			int target = PresetChangeLfoAddParam(gPreset, Teensy_guidata.ModSelect, param, depth);
			Teensy_guidata.lastAddedTarget = target;
			Raspberry_UpdateTarget(target, param, depth);
		}
	}
}

void Teensy_SelectTarget(int ledbutton)
{
	if (Teensy_guidata.ModSelect == -1) {
		if (Teensy_FindModulation(ledbutton)) {
			return;
		}
	}
	if (Teensy_guidata.GuiState == GuiState_LfoSelect) {
		if (Teensy_guidata.lastAddedLedButton == ledbutton) {
			Teensy_RotateLastAdded();
			return;
		}
		Teensy_guidata.lastAddedLedButton = -1;
		int target = NextTarget(&gPreset.lfomod[Teensy_guidata.ModSelect].target[0], ledbutton, Teensy_guidata.TargetSelect);
		if (target == -1) {
			int param = ButtonToParam(ledbutton, 0);
			if (param == -1) {
				return;
			}
			Teensy_UnmapPrevious(param);
			target = PresetChangeLfoAddParam(gPreset, Teensy_guidata.ModSelect, param, 0);
			if (target != -1) {
				SyncLfo(gPreset, Teensy_guidata.ModSelect);
				Raspberry_EditLfo(gPreset.lfomod[Teensy_guidata.ModSelect]);
				//Raspberry_UpdateTarget(target, param, 0);
				SetLedButton(ledbutton, LED_ON);
				Teensy_guidata.lastAddedLedButton = ledbutton;
				Teensy_guidata.lastAddedTarget = target;
				Teensy_guidata.lastAddedFuncIndex = 0;
			}
		}
		Teensy_guidata.TargetSelect = target;
		Raspberry_SelectTarget(target);
	}
	else if (Teensy_guidata.GuiState == GuiState_AdsrSelect) {
		int target = NextTarget(&gPreset.adsrmod[Teensy_guidata.ModSelect].target[0], ledbutton, Teensy_guidata.TargetSelect);
		if (target == -1) {
			int param = ButtonToParam(ledbutton, 0);
			if (param == -1) {
				return;
			}
			Teensy_UnmapPrevious(param);
			target = PresetChangeAdsrAddParam(gPreset, Teensy_guidata.ModSelect, param, 0);
			if (target != -1) {
				Raspberry_EditAdsr(gPreset.adsrmod[Teensy_guidata.ModSelect]);
				//Raspberry_UpdateTarget(target, param, 0);
				SetLedButton(ledbutton, LED_ON);
				Teensy_guidata.lastAddedLedButton = ledbutton;
				Teensy_guidata.lastAddedTarget = target;
				Teensy_guidata.lastAddedFuncIndex = 0;
			}
		}
		Teensy_guidata.TargetSelect = target;
		Raspberry_SelectTarget(target);
	}
	else if (Teensy_guidata.GuiState == GuiState_AdSelect) {
		int target = NextTarget(&gPreset.admod[Teensy_guidata.ModSelect].target[0], ledbutton, Teensy_guidata.TargetSelect);
		if (target == -1) {
			int param = ButtonToParam(ledbutton, 0);
			if (param == -1) {
				return;
			}
			Teensy_UnmapPrevious(param);
			target = PresetChangeAdAddParam(gPreset, Teensy_guidata.ModSelect, param, 0);
			if (target != -1) {
				Raspberry_EditAd(gPreset.admod[Teensy_guidata.ModSelect]);
				//Raspberry_UpdateTarget(target, param, 0);
				SetLedButton(ledbutton, LED_ON);
				Teensy_guidata.lastAddedLedButton = ledbutton;
				Teensy_guidata.lastAddedTarget = target;
				Teensy_guidata.lastAddedFuncIndex = 0;
			}
		}
		Teensy_guidata.TargetSelect = target;
		Raspberry_SelectTarget(target);
	}
	else if (Teensy_guidata.GuiState == GuiState_CtrlSelect) {
		int target = NextTarget(&gPreset.ctrlmod[Teensy_guidata.ModSelect].target[0], ledbutton, Teensy_guidata.TargetSelect);
		if (target == -1) {
			int param = ButtonToParam(ledbutton, 0);
			if (param == -1) {
				return;
			}
			Teensy_UnmapPrevious(param);
			target = PresetChangeCtrlAddParam(gPreset, Teensy_guidata.ModSelect, param, 0);
			if (target != -1) {
				Raspberry_EditCtrl(gPreset.ctrlmod[Teensy_guidata.ModSelect]);
				//Raspberry_UpdateTarget(target, param, 0);
				SetLedButton(ledbutton, LED_ON);
				Teensy_guidata.lastAddedLedButton = ledbutton;
				Teensy_guidata.lastAddedTarget = target;
				Teensy_guidata.lastAddedFuncIndex = 0;
			}
		}
		Teensy_guidata.TargetSelect = target;
		Raspberry_SelectTarget(target);
	}
}

void Teensy_Cancel(int side)
{
	if (Teensy_guidata.GuiState == GuiState_Root)
	{ 
		Teensy_ToState(GuiState_InitPatch);
	}
	else
	{
		Teensy_ToState(GuiState_Root, 0);
	}
	
}

void Teensy_Final()
{

	if (Teensy_guidata.GuiState == GuiState_Root)
	{
		Teensy_ToState(GuiState_SavePreset);
	}
	else if (Teensy_guidata.GuiState == GuiState_InitPatch)
	{
		InitPreset(gPreset);
		LoadPreset(gPreset);
		Teensy_ToState(GuiState_Root);
	}
	else
	{
		Teensy_guidata.GuiState = GuiState_Root;
		Teensy_guidata.ModSelect = -1;
		Teensy_guidata.TargetSelect = -1;

		Raspberry_ToState(Teensy_guidata.GuiState, Teensy_guidata.ModSelect);
	}
	UpdateTargets();
}

int Teensy_KnobInModulation(int knobid)
{
	int param = KnobToParam(knobid);

	switch (Teensy_guidata.GuiState)
	{
	case GuiState_LfoSelect:
	{
		if (Teensy_guidata.ModSelect >= 0) {
			for (int j = 0; j < 16; j++)
			{
				if (gPreset.lfomod[Teensy_guidata.ModSelect].target[j].param == param)
				{
					return j;
				}
			}
		}
		return -1;
	}
	break;
	case GuiState_AdsrSelect:
	{
		if (Teensy_guidata.ModSelect >= 0) {
			for (int j = 0; j < 16; j++)
			{
				if (gPreset.adsrmod[Teensy_guidata.ModSelect].target[j].param == param)
				{
					return j;
				}
			}
		}
		return -1;
	}
	break;
	case GuiState_AdSelect:
	{
		if (Teensy_guidata.ModSelect >= 0) {
			for (int j = 0; j < 16; j++)
			{
				if (gPreset.admod[Teensy_guidata.ModSelect].target[j].param == param)
				{
					return j;
				}
			}
		}
		return -1;
	}
	break;
	case GuiState_CtrlSelect:
	{
		for (int j = 0; j < 16; j++)
		{
			if (gPreset.ctrlmod[Teensy_guidata.ModSelect].target[j].param == param)
			{
				return j;
			}
		}
		return -1;
	}
	break;
	}
	return -1;
}

void Teensy_ModChangeDepth(int target, uint32_t value)
{
	// mangle uint16 value range to int16 properly
	value = (int16_t)(value - 0x8000);

	switch (Teensy_guidata.GuiState)
	{
	case GuiState_LfoSelect:
	{
		PresetChangeLfoDepth(gPreset, Teensy_guidata.ModSelect, target, value);
		Raspberry_UpdateTarget(target, gPreset.lfomod[Teensy_guidata.ModSelect].target[target].param, value);
	}
	break;
	case GuiState_AdsrSelect:
	{
		PresetChangeAdsrDepth(gPreset, Teensy_guidata.ModSelect, target, value);
		Raspberry_UpdateTarget(target, gPreset.adsrmod[Teensy_guidata.ModSelect].target[target].param, value);
	}
	break;
	case GuiState_AdSelect:
	{
		PresetChangeAdDepth(gPreset, Teensy_guidata.ModSelect, target, value);
		Raspberry_UpdateTarget(target, gPreset.admod[Teensy_guidata.ModSelect].target[target].param, value);
	}
	break;
	case GuiState_CtrlSelect:
	{
		PresetChangeCtrlDepth(gPreset, Teensy_guidata.ModSelect, target, value);
		Raspberry_UpdateTarget(target, gPreset.ctrlmod[Teensy_guidata.ModSelect].target[target].param, value);
	}
	break;
	}
}

void SetSwitch(SwitchEnum SwitchID)
{
	int switchset = ((int)SwitchID) / 32;

	int adjustedswitchid = SwitchID % 32;

	gPreset.switches[switchset] &= ~(1 << (adjustedswitchid));
	gPreset.switches[switchset] |=  (1 << (adjustedswitchid));
	Raspberry_guidata.switches[0] = gPreset.switches[0];
	Raspberry_guidata.switches[1] = gPreset.switches[1];
	Raspberry_guidata.dirty = 1;
	WriteSwitch(SwitchID, 1);
}

bool GetSwitch(SwitchEnum SwitchID)
{
	int switchset = ((int)SwitchID) / 32;
	int adjustedswitchid = SwitchID % 32;

	if ((gPreset.switches[switchset] >> adjustedswitchid)&1)
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

void ClearSwitch(SwitchEnum SwitchID)
{
	int switchset = ((int)SwitchID) / 32;
	int adjustedswitchid = SwitchID%32;
	gPreset.switches[switchset] &= ~(1 << adjustedswitchid);
	Raspberry_guidata.switches[0] = gPreset.switches[0];
	Raspberry_guidata.switches[1] = gPreset.switches[1];
	Raspberry_guidata.dirty = 1;
	WriteSwitch(SwitchID, 0);

}

bool Teensy_ActivateMenu(int buttonid)
{
	GuiState_t menu = ButtonToMenu(buttonid);

	if (menu != GuiState_Root) {
		
		bool waseffect = false;
		bool waschase = false;
		switch (Raspberry_guidata.GuiState)
		{
		case GuiState_Menu_CHASE:
			waschase = true;
			break;
		case GuiState_Menu_EFFECTS1:
			waseffect = true;
			break;
		}
		
			switch (menu)
			{
			case GuiState_Menu_CHASE:
			{
				if (waschase)
				{
					if (buttonid == ledbutton_CHASE)
					{
						ToggleSwitch(Switch_SELCHASE);
					}
					if (buttonid == ledbutton_STASH)
					{
						ToggleSwitch(Switch_SELSTASH);
					}
				}
			}break;
			case GuiState_Menu_EFFECTS1:
				if (waseffect)
				{
					if (buttonid == ledbutton_Cleanfeed_FX)
					{
						ToggleSwitch(Switch_SELEFFECT3);
					}
					if (buttonid == ledbutton_VCF1_FX)
					{
						ToggleSwitch(Switch_SELEFFECT1);
					}
					if (buttonid == ledbutton_VCF2_FX)
					{
						ToggleSwitch(Switch_SELEFFECT2);
					}
				}
			}
		
		Teensy_ToState(menu, 0);


		return true;
	}

	return false;
}

bool Teensy_KnobInMenu(KnobEnum knobid)
{
	int paramid = KnobToParam(knobid);
#define MENU(id, button, name) \
		if (Teensy_guidata.GuiState == GuiState_Menu_##id) {
#define ENDMENU() \
		}
#define ENTRY(name,type,param) \
		if (param == paramid) return true;
#include "PanUiMap.h"
#undef MENU
#undef ENDMENU
#undef ENTRY
	return false;
}

int PresetRemapKnob(int param)
{
	// some hacks

	// Level knobs are used for linear modulations but actual level output params are different
	if (param == Output_CLEANF_LIN) return Output_CLEANF_LEVEL;
	if (param == Output_VCF1_LIN) return Output_VCF1_LEVEL;
	if (param == Output_VCF2_LIN) return Output_VCF2_LEVEL;

	return param;
}

#ifndef abs
int32_t abs(int32_t x)
{
	if (x < 0) return -x;
	return x;
}
#endif

void Teensy_KnobChanged(KnobEnum ID, uint32_t value)
{
	if (!Teensy_KnobTouched[ID]) {
		if (abs((int32_t)value - (int32_t)Teensy_KnobLastValue[ID]) < 16384) {
			return;
		}
	}
	
	Teensy_KnobLastValue[ID] = value;
	Teensy_KnobTouched[ID] = 1;

	int target = Teensy_KnobInModulation(ID);
	if (target != -1) {
		Teensy_ModChangeDepth(target, value);
	}
	else {
		int param = KnobToParam(ID);
		if (param != -1) {
			param = PresetRemapKnob(param);

			if (Teensy_KnobInMenu(ID)) {
				Raspberry_OutputChangeValue(param, value);
			}

			PresetChangeValue(gPreset, param, value);
		}
		else {
			switch (ID)
			{
			case knob_LFO_SHAPE: {
				if (Teensy_guidata.GuiState == GuiState_LfoSelect && Teensy_guidata.ModSelect != -1) {
					PresetChangeLfoParam(gPreset, Teensy_guidata.ModSelect, Sub_lfo_shape, value);
					Raspberry_SetLfoParam(Sub_lfo_shape, value);
				}
				break;
			}
			case knob_LFO_SPEED: {
				if (Teensy_guidata.GuiState == GuiState_LfoSelect && Teensy_guidata.ModSelect != -1) {
					PresetChangeLfoParam(gPreset, Teensy_guidata.ModSelect, Sub_lfo_speed, value);
					Raspberry_SetLfoParam(Sub_lfo_speed, value);
				}
				break;
			}

			case knob_ADSR_A: {
				if (Teensy_guidata.GuiState == GuiState_AdsrSelect && Teensy_guidata.ModSelect != -1) {
					PresetChangeAdsrParam(gPreset, Teensy_guidata.ModSelect, Sub_adsr_a, value);
					Raspberry_SetAdsrParam(Sub_adsr_a, value);
				}
				break;
			}
			case knob_ADSR_D: {
				if (Teensy_guidata.GuiState == GuiState_AdsrSelect && Teensy_guidata.ModSelect != -1) {
					PresetChangeAdsrParam(gPreset, Teensy_guidata.ModSelect, Sub_adsr_d, value);
					Raspberry_SetAdsrParam(Sub_adsr_d, value);
				}
				break;
			}
			case knob_ADSR_S: {
				if (Teensy_guidata.GuiState == GuiState_AdsrSelect && Teensy_guidata.ModSelect != -1) {
					PresetChangeAdsrParam(gPreset, Teensy_guidata.ModSelect, Sub_adsr_s, value);
					Raspberry_SetAdsrParam(Sub_adsr_s, value);
				}
				break;
			}
			case knob_ADSR_R: {
				if (Teensy_guidata.GuiState == GuiState_AdsrSelect && Teensy_guidata.ModSelect != -1) {
					PresetChangeAdsrParam(gPreset, Teensy_guidata.ModSelect, Sub_adsr_r, value);
					Raspberry_SetAdsrParam(Sub_adsr_r, value);
				}
				break;
			}

			case knob_AD_A: {
				if (Teensy_guidata.GuiState == GuiState_AdSelect && Teensy_guidata.ModSelect != -1) {
					PresetChangeAdParam(gPreset, Teensy_guidata.ModSelect, Sub_ad_a, value);
					Raspberry_SetAdParam(Sub_ad_a, value);
				}
				break;
			}
			case knob_AD_D: {
				if (Teensy_guidata.GuiState == GuiState_AdSelect && Teensy_guidata.ModSelect != -1) {
					PresetChangeAdParam(gPreset, Teensy_guidata.ModSelect, Sub_ad_d, value);
					Raspberry_SetAdParam(Sub_ad_d, value);
				}
				break;
			}
			}
		}
	}
	//printf("knob %s: %d\n", Knobs[ID].name, value);

}

extern void Teensy_LoadSDPreset(int bank, int slot);
extern void Teensy_SaveSDPreset(int bank, int slot);

void Teensy_NumberHandler(int N)
{
	switch (Raspberry_guidata.GuiState)
	{
		case GuiState_SavePreset:
		{
			int bankside = N / 8;
			int bank = Raspberry_guidata.banks[bankside];
			int preset = N % 8;
			Teensy_SaveSDPreset(bank, preset);
			Teensy_ToState(GuiState_Root);
		}
		break;
	
	case GuiState_SelectBanks:
	{
		int newbank = N % 8;

		int bankside = N / 8;
		Raspberry_guidata.banks[bankside] = newbank;

		int bank = Raspberry_guidata.activebank;
		int preset = Raspberry_guidata.activeslot;

		Raspberry_guidata.dirty = true;
		Teensy_LoadSDPreset(Raspberry_guidata.banks[bank], Raspberry_guidata.activeslot);
	}
	break;
	default:
	{
		int bankside = N / 8;
		int bank = Raspberry_guidata.banks[bankside];
		int preset = N % 8;
		Raspberry_guidata.activeslot = preset;
		Raspberry_guidata.activebank = bankside;
		Raspberry_guidata.dirty = true;
		Teensy_LoadSDPreset(bank, preset);
	}
	break;
	}
	UpdateTargets();
}
void Teensy_ButtonPressed(LedButtonEnum ID, int value)
{
	//printf("button %s: %d\n", Buttons[ID].name, Buttons[ID].value ? 1 : 0);


	/*
modes:

	press a target, get detail menu for just target


	press a source once
		- all bound targets light up
		- screen menu
			- source details + current modulation?
			- target amounts
		- select all sources that apply (with submenu)



	*/

	switch (ID)
	{
	case ledbutton_LEFT_1: Teensy_NumberHandler(0); break;
	case ledbutton_LEFT_2: Teensy_NumberHandler(1); break;
	case ledbutton_LEFT_3: Teensy_NumberHandler(2); break;
	case ledbutton_LEFT_4: Teensy_NumberHandler(3); break;
	case ledbutton_LEFT_5: Teensy_NumberHandler(4); break;
	case ledbutton_LEFT_6: Teensy_NumberHandler(5); break;
	case ledbutton_LEFT_7: Teensy_NumberHandler(6); break;
	case ledbutton_LEFT_8: Teensy_NumberHandler(7); break;
	case ledbutton_RIGHT_1: Teensy_NumberHandler(8); break;
	case ledbutton_RIGHT_2: Teensy_NumberHandler(9); break;
	case ledbutton_RIGHT_3: Teensy_NumberHandler(10); break;
	case ledbutton_RIGHT_4: Teensy_NumberHandler(11); break;
	case ledbutton_RIGHT_5: Teensy_NumberHandler(12); break;
	case ledbutton_RIGHT_6: Teensy_NumberHandler(13); break;
	case ledbutton_RIGHT_7: Teensy_NumberHandler(14); break;
	case ledbutton_RIGHT_8: Teensy_NumberHandler(15); break;
	case ledbutton_VCF1_Routing: ToggleSwitch(Switch_SELVCF2POST); UpdateTargets(); break;
	case ledbutton_LEFT_MORE:
	case ledbutton_RIGHT_MORE:
		if (Raspberry_guidata.GuiState == GuiState_SelectBanks)
		{
			Teensy_ToState(GuiState_Root);
		}
		else
		{
			Teensy_ToState(GuiState_SelectBanks);
		}
		break;
	case ledbutton_CANCEL_LEFT: Teensy_Cancel(0); break;
	case ledbutton_CANCEL_RIGHT: Teensy_Cancel(1); break;
	case ledbutton_FINAL: Teensy_Final(); break;
	case ledbutton_LFO: Teensy_ToState(GuiState_LfoSelect); break;
	case ledbutton_ADSR: Teensy_ToState(GuiState_AdsrSelect); break;
	case ledbutton_AD: Teensy_ToState(GuiState_AdSelect); break;
	case ledbutton_SOURCE_LFO: Teensy_ToState(GuiState_LfoSelect, Teensy_FirstEmptyLfoMod()); break;
	case ledbutton_SOURCE_ADSR: Teensy_ToState(GuiState_AdsrSelect, Teensy_FirstEmptyAdsrMod()); break;
	case ledbutton_SOURCE_AD: Teensy_ToState(GuiState_AdSelect, Teensy_FirstEmptyAdMod()); break;
	case ledbutton_LEFT_MOD: Teensy_ToState(GuiState_CtrlSelect, ledbutton_LEFT_MOD); break;
	case ledbutton_RIGHT_MOD: Teensy_ToState(GuiState_CtrlSelect, ledbutton_RIGHT_MOD); break;
	case ledbutton_LEFT_SUS: Teensy_ToState(GuiState_CtrlSelect, ledbutton_LEFT_SUS); break;
	case ledbutton_RIGHT_SUS: Teensy_ToState(GuiState_CtrlSelect, ledbutton_RIGHT_SUS); break;
	case ledbutton_LEFT_UNAC: Teensy_ToState(GuiState_CtrlSelect, ledbutton_LEFT_UNAC); break;
	case ledbutton_RIGHT_UNAC: Teensy_ToState(GuiState_CtrlSelect, ledbutton_RIGHT_UNAC); break;
	case ledbutton_X: Teensy_ToState(GuiState_CtrlSelect, ledbutton_X); break;
	case ledbutton_Y: Teensy_ToState(GuiState_CtrlSelect, ledbutton_Y); break;
	case ledbutton_Z: Teensy_ToState(GuiState_CtrlSelect, ledbutton_Z); break;
	case ledbutton_ZPRIME: Teensy_ToState(GuiState_CtrlSelect, ledbutton_ZPRIME); break;
	case ledbutton_KBCV: Teensy_ToState(GuiState_CtrlSelect, ledbutton_KBCV); break;
	case ledbutton_VELOCITY: Teensy_ToState(GuiState_CtrlSelect, ledbutton_VELOCITY); break;

	case ledbutton_ZERO: WritePadZero(); break;

	default:

		if (Teensy_IsModEdit()) {
			Teensy_SelectTarget(ID);
		}
		else if (Teensy_ActivateMenu(ID)) {

		}
		break;
	}
}

void Teensy_Reset()
{
	Teensy_guidata.GuiState = GuiState_Root;
}



int Teensy_FindKnobIDX(int in)
{
	for (int i = 0; i < __KNOB_COUNT; i++)
	{
		if (Knobs[i].frontpanelidx == in)
		{
			return i;
		}
	}
	return -1;
}
int Teensy_FindButtonIDX(int in)
{
	for (int i = 0; i < __LEDBUTTON_COUNT; i++)
	{
		if (Buttons[i].fpid == in)
		{
			return i;
		}
	}
	return -1;
}

void Teensy_EncoderPress(int id)
{

}

#define MENU(id,button,name) static int const TeensyMenuItemCount_##id = 0
#define ENTRY(name, type, param) + 1
#define CUSTOMENTRY(name, type, param) + 1
#define ENDMENU() ;
#include "PanUiMap.h"
#undef ENDMENU
#undef ENTRY
#undef MENU
#undef CUSTOMENTRY


bool RightDelta_MenuEntry_Value(const char *name, int param, int delta)
{
	int OrigVal = gPreset.paramvalue[param];
	int32_t val = OrigVal+ delta * 1000;
	if (val < 0) val = 0;
	if (val > 0xffff) val = 0xffff;

	Raspberry_OutputChangeValue(param, val);
	PresetChangeValue(gPreset, param, val);

	return true;
}

bool RightDelta_MenuEntry_Switch(const char *name, int sw, int delta)
{
	if (delta < 0) {
		SetSwitch((SwitchEnum)sw);
	}
	else {
		ClearSwitch((SwitchEnum)sw);
	}

	return true;
}

bool RightDelta_MenuEntry_RemapValue(const char *name, int param, int delta)
{
	return RightDelta_MenuEntry_Value(name, PresetRemapKnob(param), delta);
}


bool RightDelta_MenuEntry_MidValue(const char *name, int param, int delta) 
{
	return RightDelta_MenuEntry_Value(name, param, delta);
}

bool RightDelta_MenuEntry_Percentage(const char *name, int param, int delta) { return RightDelta_MenuEntry_Value(name, param, delta); };
bool RightDelta_MenuEntry_Pitch(const char *name, int param, int delta) { return RightDelta_MenuEntry_Value(name, param, delta); };
bool RightDelta_MenuEntry_FilterMix(const char *name, int param, int delta) { return RightDelta_MenuEntry_Value(name, param, delta); };

void Teensy_Switch(SwitchEnum switchid, int newval)
{
	if (newval) SetSwitch(switchid); else ClearSwitch(switchid);
}

bool RightDelta_MenuEntry_EffectType(const char *name, int param, int delta)
{
	int current = DecodeCurrentEffect(Raspberry_guidata.switches[0]);
	int neweffect = (current + delta + 8) % 8;
	int a = ((neweffect >> 0) & 1) ? 0 : 1;
	int b = ((neweffect >> 1) & 1) ? 0 : 1;
	int c = ((neweffect >> 2) & 1) ? 0 : 1;
	int d = 1;
	Teensy_Switch(Switch_SELEF1, a);
	Teensy_Switch(Switch_SELEF2, b);
	Teensy_Switch(Switch_SELEF3, c);
	Teensy_Switch(Switch_SELEF4, d);

	return true;
};

bool RightDelta_MenuEntry_EffectParam1(const char *name, int param, int delta) { return RightDelta_MenuEntry_Value(name, param, delta); };
bool RightDelta_MenuEntry_EffectParam2(const char *name, int param, int delta) { return RightDelta_MenuEntry_Value(name, param, delta); };
bool RightDelta_MenuEntry_EffectParam3(const char *name, int param, int delta) { return RightDelta_MenuEntry_Value(name, param, delta); };

bool RightDelta_MenuEntry_Toggle(const char *name, SwitchEnum param, int delta)
{
	if (delta < 0)
	{
		Teensy_Switch(param, 1);
	}
	else
	{
		Teensy_Switch(param, 0);
	}
	return true;
}

int TargetCount()
{
	if (Raspberry_guidata.ModSelect == -1) return 0;
	int count = 0;
	switch (Raspberry_guidata.GuiState)
	{
	case GuiState_AdSelect: 
		for (int i = 0; i < 16; i++) { if (gPreset.admod[Raspberry_guidata.ModSelect].target[i].param != 0) { count++; } } break;
	case GuiState_AdsrSelect:
		for (int i = 0; i < 16; i++) { if (gPreset.adsrmod[Raspberry_guidata.ModSelect].target[i].param != 0) { count++; } } break;
	case GuiState_CtrlSelect:
		for (int i = 0; i < 16; i++) { if (gPreset.ctrlmod[Raspberry_guidata.ModSelect].target[i].param != 0) { count++; } } break;
	case GuiState_LfoSelect:
		for (int i = 0; i < 16; i++) { if (gPreset.lfomod[Raspberry_guidata.ModSelect].target[i].param != 0) { count++; } } break;
	}
	return count;

}

void LimitRange(int max)
{
	Raspberry_guidata.LeftEncoderValue = (Raspberry_guidata.LeftEncoderValue + max) % max;
	Raspberry_guidata.dirty = true;
}

int CtrlParamCount(ModSource_t M)
{
#define CTRLMENU(id, name) if (id == M) { return  0
#define PARA(id,id2)   +1
#define SWITCH2(id,id2)   +1
#define CTRLENDMENU() ;};

#include "ModMenus.h"

#undef CTRLMENU
#undef PARA
#undef SWITCH2
#undef CTRLENDMENU

	
	; return 0;
};

void DoAssignMenu(int state, int delta)
{
#define MENU(id, name, structname) if (Raspberry_guidata.GuiState == state) { int currentitem = 0;
#define ENDMENU()  return ; }

#include "ModMenus.h"
#undef MENU
#undef ENDMENU

	//DoAssignMenu(Raspberry_guidata.GuiState);
}

void DoCtrlMenu(int delta)
{

#define CTRLMENU(id, name) if (Raspberry_guidata.dataCtrl.source == id) { int currentitem = 0;
#define CTRLENDMENU()  return ; }
#define PARA(name,output) if (currentitem == Raspberry_guidata.LeftEncoderValue) \
		{\
			RightDelta_MenuEntry_Value(name, output, delta);\
		} currentitem++;
#define SWITCH2(name,sw) if (currentitem == Raspberry_guidata.LeftEncoderValue) \
		{\
			RightDelta_MenuEntry_Switch(name, sw, delta);\
		} currentitem++;
#include "ModMenus.h"
#undef CTRLMENU
#undef CTRLENDMENU
#undef PARA
#undef SWITCH2
}

void Teensy_EncoderRotate(int id, int delta)
{
	if (id == 0)
	{
		Raspberry_guidata.LeftEncoderValue += delta;
		
		switch (Raspberry_guidata.GuiState)
		{
		case GuiState_LfoSelect: LimitRange(2 + TargetCount()); break;
		case GuiState_AdsrSelect: LimitRange(4 + TargetCount()); break;
		case GuiState_CtrlSelect: LimitRange(CtrlParamCount(Raspberry_guidata.dataCtrl.source) + TargetCount()); break;
		case GuiState_AdSelect: LimitRange(2 + TargetCount()); break;
		default:
		{
#define MENU(id, button, name) if (Raspberry_guidata.GuiState == GuiState_Menu_##id) {int max = TeensyMenuItemCount_##id; if (max <1) return;Raspberry_guidata.LeftEncoderValue = (Raspberry_guidata.LeftEncoderValue + max)%max;
#define ENDMENU() Raspberry_guidata.dirty = true;return; };
#define KV(name,param) \
	if (currentitem == Raspberry_guidata.LeftEncoderValue) \
		{\
					Raspberry_guidata.dirty = true;\
			return;\
		};\
		currentitem++;
#define SWITCH(name,sw) \
	if (currentitem == Raspberry_guidata.LeftEncoderValue) \
		{\
					Raspberry_guidata.dirty = true;\
			return;\
		};\
		currentitem++;

#include "PanUiMap.h"
#undef MENU
#undef ENDMENU
#undef KV		
		}	
		break;
		}
	}
	else
	{
		if (Raspberry_guidata.GuiState == GuiState_LfoSelect)
		{
			DoAssignMenu(Raspberry_guidata.GuiState,  delta);
		}
		else if (Raspberry_guidata.GuiState == GuiState_AdsrSelect)
		{
			DoAssignMenu(Raspberry_guidata.GuiState,  delta);
		}
		else if (Raspberry_guidata.GuiState == GuiState_CtrlSelect)
		{
			DoCtrlMenu(delta);
		}
		else if (Raspberry_guidata.GuiState == GuiState_AdSelect)
		{
			DoAssignMenu(Raspberry_guidata.GuiState,  delta);
		}
		else
		{

#define MENU(id, button, name) if (Raspberry_guidata.GuiState == GuiState_Menu_##id) { int currentitem = 0;
#define ENDMENU()  return ; }

#define ENTRY(name,type,param) \
	if (currentitem == Raspberry_guidata.LeftEncoderValue) \
		{\
			if (RightDelta_##type(name, param,delta))\
			{\
					Raspberry_guidata.dirty = true;\
			};\
			return;\
		};\
		currentitem++;
#define CUSTOMENTRY(name,type,param) \
	if (currentitem == Raspberry_guidata.LeftEncoderValue) {if(RightDelta_##type(name, param,delta)){Raspberry_guidata.dirty = true;};return;};currentitem++;
#include "PanUiMap.h"
#undef MENU
#undef ENDMENU
#undef ENTRY
#undef CUSTOMENTRY

		}

	}


}
