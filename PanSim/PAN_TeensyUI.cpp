#include "PanHeader.h"

extern void WriteKnob(int id, uint32_t value);
extern void WriteWithSubKnob(int id, int subid, uint32_t value);
extern void WriteSwitch(int id, int state);
extern void WriteSyncLfo(uint8_t* paramids);

PanPreset_t gPreset;

void InitPreset(PanPreset_t& preset)
{
	memset(&preset, 0, sizeof(PanPreset_t));

	preset.ctrlmod[0].source = ControlModulation_t::Source_note;
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
	preset.adsrmod[0].target[0].param = Output_VCF1_LIN;
	preset.adsrmod[0].target[0].depth = 0x4000;
	preset.adsrmod[0].target[1].param = Output_VCF2_LIN;
	preset.adsrmod[0].target[0].depth = 0x4000;
	preset.adsrmod[0].target[2].param = Output_CLEANF_LIN;
	preset.adsrmod[0].target[0].depth = 0x4000;

	preset.paramvalue[Output_VCO1_MIX1] = 0xffff;

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

	preset.switches[0] |= (1 << Switch_SEL1SAW);
	preset.switches[0] |= (1 << Switch_SELMOST2);
	preset.switches[0] |= (1 << Switch_SELMOST3);
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
	WriteWithSubKnob(id, Sub_vel, 0);
#define OUTPUT_VIRT(name,codecport,codecpin, type,id, style,defaultvalue) \
	OUTPUT(name,codecport,codecpin,type,id,style,defaultvalue);
#define SWITCH(name,id) \
	WriteSwitch(id, (preset.switches[0]>>id) & 1);
#include "../interface/paramdef.h"
#undef OUTPUT
#undef OUTPUT_VIRT
#undef SWITCH

	for (int i = 0; i < 16; i++) {
		uint8_t synclfo[16] = { 0 };
		for (int j = 0; j < 16; j++) {
			int param = preset.lfomod[i].target[j].param;
			if (param == 0) continue;
			synclfo[j] = param;
			WriteWithSubKnob(param, Sub_lfo_depth, preset.lfomod[i].target[i].depth);
			WriteWithSubKnob(param, Sub_lfo_speed, preset.lfomod[i].speed);
			WriteWithSubKnob(param, Sub_lfo_shape, preset.lfomod[i].shape);
		}
		WriteSyncLfo(synclfo);
	}

	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			int param = preset.adsrmod[i].target[j].param;
			if (param == 0) continue;
			WriteWithSubKnob(param, Sub_adsr_depth, preset.adsrmod[i].target[i].depth);
			WriteWithSubKnob(param, Sub_adsr_a, preset.adsrmod[i].a);
			WriteWithSubKnob(param, Sub_adsr_d, preset.adsrmod[i].d);
			WriteWithSubKnob(param, Sub_adsr_s, preset.adsrmod[i].s);
			WriteWithSubKnob(param, Sub_adsr_r, preset.adsrmod[i].r);
		}
	}

	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			int param = preset.admod[i].target[j].param;
			if (param == 0) continue;
			WriteWithSubKnob(param, Sub_ad_depth, preset.admod[i].target[i].depth);
			WriteWithSubKnob(param, Sub_ad_a, preset.admod[i].a);
			WriteWithSubKnob(param, Sub_ad_d, preset.admod[i].d);
		}
	}

	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			int param = preset.ctrlmod[i].target[j].param;
			if (param == 0) continue;

			switch (preset.ctrlmod[i].source) {
			case ControlModulation_t::Source_left_mod:
				//todo
				//WriteWithSubKnob(param, Sub)
				break;
			case ControlModulation_t::Source_right_mod:
				//todo
				break;
			case ControlModulation_t::Source_x:
				WriteWithSubKnob(param, Sub_x, preset.ctrlmod[i].target[j].depth);
				break;
			case ControlModulation_t::Source_y:
				WriteWithSubKnob(param, Sub_y, preset.ctrlmod[i].target[j].depth);
				break;
			case ControlModulation_t::Source_z:
				WriteWithSubKnob(param, Sub_z, preset.ctrlmod[i].target[j].depth);
				break;
			case ControlModulation_t::Source_zprime:
				WriteWithSubKnob(param, Sub_zprime, preset.ctrlmod[i].target[j].depth);
				break;
			case ControlModulation_t::Source_note:
				WriteWithSubKnob(param, Sub_note, preset.ctrlmod[i].target[j].depth);
				break;
			case ControlModulation_t::Source_vel:
				WriteWithSubKnob(param, Sub_vel, preset.ctrlmod[i].target[j].depth);
				break;
			}
		}
	}
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
	return emptyindex;
}

void PresetChangeAdsrRemoveParam(PanPreset_t& preset, int mod, int param)
{
	for (int i = 0; i < 16; i++)
	{
		if (preset.adsrmod[mod].target[i].param == param) {
			preset.adsrmod[mod].target[i].param = 0;
			WriteWithSubKnob(param, Sub_adsr_depth, 0);
			return;
		}
	}
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
}

void PresetChangeAdDepth(PanPreset_t& preset, int mod, int index, uint16_t value)
{
	preset.admod[mod].target[index].depth = value;
	WriteWithSubKnob(preset.admod[mod].target[index].param, Sub_ad_depth, value);
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
	return emptyindex;
}

void PresetChangeAdRemoveParam(PanPreset_t& preset, int mod, int param)
{
	for (int i = 0; i < 16; i++)
	{
		if (preset.admod[mod].target[i].param == param) {
			preset.admod[mod].target[i].param = 0;
			WriteWithSubKnob(param, Sub_ad_depth, 0);
			return;
		}
	}
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
}

void PresetChangeLfoDepth(PanPreset_t& preset, int mod, int index, uint16_t value)
{
	preset.lfomod[mod].target[index].depth = value;
	WriteWithSubKnob(preset.lfomod[mod].target[index].param, Sub_lfo_depth, value);
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
	return emptyindex;
}

void PresetChangeLfoRemoveParam(PanPreset_t& preset, int mod, int param)
{
	for (int i = 0; i < 16; i++)
	{
		if (preset.lfomod[mod].target[i].param == param) {
			preset.lfomod[mod].target[i].param = 0;
			WriteWithSubKnob(param, Sub_lfo_depth, 0);
			return;
		}
	}
}

void WriteCtrlDepth(int param, int source, uint16_t depth)
{
	switch (source)
	{
	case ControlModulation_t::Source_left_mod:
		//WriteWithSubKnob(param, Sub_lfo_, 0);
		// todo
		break;
	case ControlModulation_t::Source_right_mod:
		// todo
		break;
	case ControlModulation_t::Source_x:
		WriteWithSubKnob(param, Sub_x, depth);
		break;
	case ControlModulation_t::Source_y:
		WriteWithSubKnob(param, Sub_y, depth);
		break;
	case ControlModulation_t::Source_z:
		WriteWithSubKnob(param, Sub_z, depth);
		break;
	case ControlModulation_t::Source_zprime:
		WriteWithSubKnob(param, Sub_zprime, depth);
		break;
	case ControlModulation_t::Source_note:
		WriteWithSubKnob(param, Sub_note, depth);
		break;
	case ControlModulation_t::Source_vel:
		WriteWithSubKnob(param, Sub_vel, depth);
		break;
	}
}

void PresetChangeCtrlSource(PanPreset_t& preset, int mod, ControlModulation_t::ModSource_t value)
{
	int oldsource = preset.ctrlmod[mod].source;
	preset.ctrlmod[mod].source = value;
	for (int i = 0; i < 16; i++)
	{
		int param = preset.ctrlmod[mod].target[i].param;
		WriteCtrlDepth(param, oldsource, 0);
		WriteCtrlDepth(param, value, preset.ctrlmod[mod].target[i].depth);
	}
}

void PresetChangeCtrlDepth(PanPreset_t& preset, int mod, int index, uint16_t value)
{
	preset.ctrlmod[mod].target[index].depth = value;
	WriteCtrlDepth(preset.ctrlmod[mod].target[index].param, preset.ctrlmod[mod].source, value);
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
	return emptyindex;
}

void PresetChangeCtrlRemoveParam(PanPreset_t& preset, int mod, int param)
{
	for (int i = 0; i < 16; i++)
	{
		if (preset.ctrlmod[mod].target[i].param == param) {
			preset.ctrlmod[mod].target[i].param = 0;
			WriteCtrlDepth(param, preset.ctrlmod[mod].source, 0);
			return;
		}
	}
}

void Teensy_InitPreset()
{
	InitPreset(gPreset);
	LoadPreset(gPreset);
}

typedef struct
{
	GuiState_t GuiState;
	int ModSelect;
	int TargetSelect;

	char cancel[128];
} Teensy_GuiData_t;

static Teensy_GuiData_t Teensy_guidata;

ControlModulation_t::ModSource_t LedButtonToSource(int ledbuttonid)
{
	switch (ledbuttonid)
	{
	case ledbutton_LEFT_MOD: return ControlModulation_t::Source_left_mod;
	case ledbutton_RIGHT_MOD: return ControlModulation_t::Source_right_mod;
	case ledbutton_X: return ControlModulation_t::Source_x;
	case ledbutton_Y: return ControlModulation_t::Source_y;
	case ledbutton_Z: return ControlModulation_t::Source_z;
	case ledbutton_ZPRIME: return ControlModulation_t::Source_zprime;
	case ledbutton_KBCV: return ControlModulation_t::Source_note;
	case ledbutton_VELOCITY: return ControlModulation_t::Source_vel;
	}

	return ControlModulation_t::Source_none;
}

int SourceToLedButton(ControlModulation_t::ModSource_t source)
{
	switch (source)
	{
	case ControlModulation_t::Source_left_mod: return ledbutton_LEFT_MOD;
	case ControlModulation_t::Source_right_mod: return ledbutton_RIGHT_MOD;
	case ControlModulation_t::Source_x: return ledbutton_X;
	case ControlModulation_t::Source_y: return ledbutton_Y;
	case ControlModulation_t::Source_z: return ledbutton_Z;
	case ControlModulation_t::Source_zprime: return ledbutton_ZPRIME;
	case ControlModulation_t::Source_note: return ledbutton_KBCV;
	case ControlModulation_t::Source_vel: return ledbutton_VELOCITY;
	}

	return -1;
}

void UpdateTargets()
{
	for (int i = 0; i < __LEDBUTTON_COUNT; i++)
	{
		Buttons[i].value = false;
	}

	switch (Teensy_guidata.GuiState) {
	case GuiState_LfoSelect:
		Buttons[ledbutton_LFO].value = true;
		break;
	case GuiState_AdsrSelect:
		Buttons[ledbutton_ADSR].value = true;
		break;
	case GuiState_AdSelect:
		Buttons[ledbutton_AD].value = true;
		break;
	case GuiState_CtrlSelect:
		Buttons[SourceToLedButton(gPreset.ctrlmod[Teensy_guidata.ModSelect].source)].value = true;
		break;
	}

	switch (Teensy_guidata.GuiState)
	{
	case GuiState_LfoSelect:
		{
			if (Teensy_guidata.ModSelect >= 0) {
				for (int j = 0; j < 16; j++)
				{
					int but = ParamToButton(gPreset.lfomod[Teensy_guidata.ModSelect].target[j].param);
					if (but > 0) {
						Buttons[but].value = true;
					}
				}
			}
			else {
				for (int i = 0; i < 16; i++)
				{
					for (int j = 0; j < 16; j++)
					{
						int but = ParamToButton(gPreset.lfomod[i].target[j].param);
						if (but > 0) {
							Buttons[but].value = true;
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
				if (but > 0) {
					Buttons[but].value = true;
				}
			}
		}
		else {
			for (int i = 0; i < 16; i++)
			{
				for (int j = 0; j < 16; j++)
				{
					int but = ParamToButton(gPreset.adsrmod[i].target[j].param);
					if (but > 0) {
						Buttons[but].value = true;
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
				if (but > 0) {
					Buttons[but].value = true;
				}
			}
		}
		else {
			for (int i = 0; i < 16; i++)
			{
				for (int j = 0; j < 16; j++)
				{
					int but = ParamToButton(gPreset.admod[i].target[j].param);
					if (but > 0) {
						Buttons[but].value = true;
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
				Buttons[but].value = true;
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

int CtrlMod(PanPreset_t& preset, ControlModulation_t::ModSource_t source)
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

bool IsModEdit()
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
	if (Teensy_guidata.GuiState == GuiState_LfoSelect && Teensy_guidata.ModSelect >= 0) {
		memcpy(&gPreset.lfomod[Teensy_guidata.ModSelect], &Teensy_guidata.cancel, sizeof(gPreset.lfomod[0]));
	}
	else if (Teensy_guidata.GuiState == GuiState_AdsrSelect && Teensy_guidata.ModSelect >= 0) {
		memcpy(&gPreset.adsrmod[Teensy_guidata.ModSelect], &Teensy_guidata.cancel, sizeof(gPreset.adsrmod[0]));
	}
	else if (Teensy_guidata.GuiState == GuiState_AdSelect && Teensy_guidata.ModSelect >= 0) {
		memcpy(&gPreset.admod[Teensy_guidata.ModSelect], &Teensy_guidata.cancel, sizeof(gPreset.admod[0]));
	}
	else if (Teensy_guidata.GuiState == GuiState_CtrlSelect && Teensy_guidata.ModSelect >= 0) {
		memcpy(&gPreset.ctrlmod[Teensy_guidata.ModSelect], &Teensy_guidata.cancel, sizeof(gPreset.ctrlmod[0]));
	}
}

void SelectMod()
{
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

	if (state == GuiState_CtrlSelect && modselect != -1) {
		modselect = CtrlMod(gPreset, LedButtonToSource(modselect));
	}

	Teensy_guidata.GuiState = state;
	Teensy_guidata.ModSelect = modselect;
	Teensy_guidata.TargetSelect = -1;

	SelectMod();
	Raspberry_ToState(state, modselect);

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

	UpdateTargets();
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
			for (int j = 0; j < 16; j++) {
				if (firstEmpty == -1 && gPreset.lfomod[i].target[j].param == 0) firstEmpty = i;
				if (ParamToButton(gPreset.lfomod[i].target[j].param) == ledbutton) {
					Teensy_guidata.ModSelect = i;
					Teensy_guidata.TargetSelect = j;
					SelectMod();
					Raspberry_EditLfo(gPreset.lfomod[Teensy_guidata.ModSelect]);
					Raspberry_SelectTarget(Teensy_guidata.TargetSelect);
					return true;
				}
			}
		}
		Teensy_guidata.ModSelect = firstEmpty;
		Teensy_guidata.TargetSelect = -1;
		SelectMod();
		Raspberry_EditLfo(gPreset.lfomod[Teensy_guidata.ModSelect]);
	}
	else if (Teensy_guidata.GuiState == GuiState_AdsrSelect) {
		int firstEmpty = -1;
		for (int i = 0; i < 16; i++) {
			for (int j = 0; j < 16; j++) {
				if (firstEmpty == -1 && gPreset.adsrmod[i].target[j].param == 0) firstEmpty = i;
				if (ParamToButton(gPreset.adsrmod[i].target[j].param) == ledbutton) {
					Teensy_guidata.ModSelect = i;
					Teensy_guidata.TargetSelect = j;
					SelectMod();
					Raspberry_EditAdsr(gPreset.adsrmod[Teensy_guidata.ModSelect]);
					Raspberry_SelectTarget(Teensy_guidata.TargetSelect);
					return true;
				}
			}
		}
		Teensy_guidata.ModSelect = firstEmpty;
		Teensy_guidata.TargetSelect = -1;
		SelectMod();
		Raspberry_EditAdsr(gPreset.adsrmod[Teensy_guidata.ModSelect]);
	}
	else if (Teensy_guidata.GuiState == GuiState_AdSelect) {
		int firstEmpty = -1;
		for (int i = 0; i < 16; i++) {
			for (int j = 0; j < 16; j++) {
				if (firstEmpty == -1 && gPreset.admod[i].target[j].param == 0) firstEmpty = i;
				if (ParamToButton(gPreset.admod[i].target[j].param) == ledbutton) {
					Teensy_guidata.ModSelect = i;
					Teensy_guidata.TargetSelect = j;
					SelectMod();
					Raspberry_EditAd(gPreset.admod[Teensy_guidata.ModSelect]);
					Raspberry_SelectTarget(Teensy_guidata.TargetSelect);
					return true;
				}
			}
		}
		Teensy_guidata.ModSelect = firstEmpty;
		Teensy_guidata.TargetSelect = -1;
		SelectMod();
		Raspberry_EditAd(gPreset.admod[Teensy_guidata.ModSelect]);
	}

	return false;
}

void Teensy_SelectTarget(int ledbutton)
{
	if (Teensy_guidata.ModSelect == -1) {
		if (Teensy_FindModulation(ledbutton)) {
			return;
		}
	}
	if (Teensy_guidata.GuiState == GuiState_LfoSelect) {
		int target = NextTarget(&gPreset.lfomod[Teensy_guidata.ModSelect].target[0], ledbutton, Teensy_guidata.TargetSelect);
		if (target == -1) {
			target = PresetChangeLfoAddParam(gPreset, Teensy_guidata.ModSelect, ButtonToParam(ledbutton), 0);
			if (target != -1) Buttons[ledbutton].value = true;
		}
		Teensy_guidata.TargetSelect = target;
		Raspberry_SelectTarget(target);
	}
	else if (Teensy_guidata.GuiState == GuiState_AdsrSelect) {
		int target = NextTarget(&gPreset.adsrmod[Teensy_guidata.ModSelect].target[0], ledbutton, Teensy_guidata.TargetSelect);
		if (target == -1) {
			target = PresetChangeAdsrAddParam(gPreset, Teensy_guidata.ModSelect, ButtonToParam(ledbutton), 0);
			if (target != -1) Buttons[ledbutton].value = true;
		}
		Teensy_guidata.TargetSelect = target;
		Raspberry_SelectTarget(target);
	}
	else if (Teensy_guidata.GuiState == GuiState_AdSelect) {
		int target = NextTarget(&gPreset.admod[Teensy_guidata.ModSelect].target[0], ledbutton, Teensy_guidata.TargetSelect);
		if (target == -1) {
			target = PresetChangeAdAddParam(gPreset, Teensy_guidata.ModSelect, ButtonToParam(ledbutton), 0);
			if (target != -1) Buttons[ledbutton].value = true;
		}
		Teensy_guidata.TargetSelect = target;
		Raspberry_SelectTarget(target);
	}
	else if (Teensy_guidata.GuiState == GuiState_CtrlSelect) {
		int target = NextTarget(&gPreset.ctrlmod[Teensy_guidata.ModSelect].target[0], ledbutton, Teensy_guidata.TargetSelect);
		if (target == -1) {
			target = PresetChangeCtrlAddParam(gPreset, Teensy_guidata.ModSelect, ButtonToParam(ledbutton), 0);
			if (target != -1) Buttons[ledbutton].value = true;
		}
		Teensy_guidata.TargetSelect = target;
		Raspberry_SelectTarget(target);
	}
}

void Teensy_Cancel(int side)
{
	Teensy_ToState(GuiState_Root, 0);
}

void Teensy_Final()
{
	Teensy_guidata.GuiState = GuiState_Root;
	Teensy_guidata.ModSelect = -1;
	Teensy_guidata.TargetSelect = -1;

	Raspberry_ToState(Teensy_guidata.GuiState, Teensy_guidata.ModSelect);
	UpdateTargets();
}

void Teensy_KnobChanged(int ID, uint32_t value)
{
	switch (ID)
	{
		//case knob_MASTER_TUNE: WriteKnob()



	case knob_FM_1_to_2: PresetChangeValue(gPreset, Output_VCO123_FM1, value); break;
	case knob_FM_2_to_3: PresetChangeValue(gPreset, Output_VCO123_FM2, value); break;
	case knob_VCO1_Pitch: PresetChangeValue(gPreset, Output_VCO1_PITCH, value); break;
	case knob_VCO2_Pitch: PresetChangeValue(gPreset, Output_VCO2_PITCH, value); break;
	case knob_VCO3_Pitch: PresetChangeValue(gPreset, Output_VCO3_PITCH, value); break;
	
	case knob_VCO1_PW: PresetChangeValue(gPreset, Output_VCO1_PW, value); break;
	case knob_VCO2_PW: PresetChangeValue(gPreset, Output_VCO2_PW, value); break;
	case knob_VCO3_PW: PresetChangeValue(gPreset, Output_VCO3_PW, value); break;

	case knob_VCO4_Pitch: PresetChangeValue(gPreset, Output_VCO4_PITCH, value); break;
	case knob_VCO5_Pitch: PresetChangeValue(gPreset, Output_VCO5_PITCH, value); break;
	case knob_VCO6_Pitch: PresetChangeValue(gPreset, Output_VCO6_PITCH, value); break;
	case knob_VCO7_Pitch: PresetChangeValue(gPreset, Output_VCO7_PITCH, value); break;

	case knob_VCF1_Resonance: PresetChangeValue(gPreset, Output_VCF1_RES, value); break;
	case knob_VCF1_Frequency: PresetChangeValue(gPreset, Output_VCF1_CV, value); break;
	case knob_VCF1_Spectrum_Mod: PresetChangeValue(gPreset, Output_VCF1_CROSSMOD, value); break;

	case knob_VCF2_Resonance: PresetChangeValue(gPreset, Output_VCF2_RES, value); break;
	case knob_VCF2_Feed_Back: PresetChangeValue(gPreset, Output_VCF2_FB, value); break;
	case knob_VCF2_Spectrum_mod: PresetChangeValue(gPreset, Output_VCF2_CROSSMOD, value); break;

	case knob_Bank_Low: PresetChangeValue(gPreset, Output_VCF2_L_CV, value); break;
	case knob_Bank_Low_Level: PresetChangeValue(gPreset, Output_VCF2_L_MIX, value); break;
	
	case knob_Bank_High: PresetChangeValue(gPreset, Output_VCF2_H_CV, value); break;
	case knob_Bank_High_Level: PresetChangeValue(gPreset, Output_VCF2_H_MIX, value); break;

	case knob_Bank_Mid_1: PresetChangeValue(gPreset, Output_VCF2_M1_CV, value); break;
	case knob_Bank_Mid_1_Level: PresetChangeValue(gPreset, Output_VCF2_M1_MIX, value); break;

	case knob_Bank_Mid_2: PresetChangeValue(gPreset, Output_VCF2_M2_CV, value); break;
	case knob_Bank_Mid_2_Level: PresetChangeValue(gPreset, Output_VCF2_M2_MIX, value); break;

	case knob_PAN_Cleanfeed: PresetChangeValue(gPreset, Output_CLEANF_PAN, value); break;
	case knob_PAN_VCF1: PresetChangeValue(gPreset, Output_VCF1_PAN, value); break;
	case knob_PAN_VCF2: PresetChangeValue(gPreset, Output_VCF2_PAN, value); break;

	case knob_LEVEL_Cleanfeed: PresetChangeValue(gPreset, Output_CLEANF_LEVEL, value); break;
	case knob_LEVEL_VCF1: PresetChangeValue(gPreset, Output_VCF1_LEVEL, value); break;
	case knob_LEVEL_VCF2: PresetChangeValue(gPreset, Output_VCF2_LEVEL, value); break;








	case knob_MIXER_VCO1:PresetChangeValue(gPreset, Output_VCO1_MIX1, value); break;
	case knob_MIXER_VCO2:PresetChangeValue(gPreset, Output_VCO2_MIX1, value); break;
	case knob_MIXER_VCO3:PresetChangeValue(gPreset, Output_VCO3_MIX1, value); break;
	case knob_MIXER_VCO4_7:PresetChangeValue(gPreset, Output_VCO4567_MIX1, value); break;
	case knob_MIXER_RM1_2:PresetChangeValue(gPreset, Output_RM1_MIX1, value); break;
	case knob_MIXER_NOISE_ANAWT:PresetChangeValue(gPreset, Output_WHITENS_MIX1, value); break;
	case knob_MIXER_NOISE_DIG:PresetChangeValue(gPreset, Output_DIGINS_MIX1, value); break;
	case knob_MIXER_NOISECOLOR:PresetChangeValue(gPreset, Output_NOISE_COLOR, value); break;


	case knob_MIXER_VCO4:PresetChangeValue(gPreset, Output_VCO4_DRY_MIX, value); break;
	case knob_MIXER_VCO5:PresetChangeValue(gPreset, Output_VCO5_DRY_MIX, value); break;
	case knob_MIXER_VCO6:PresetChangeValue(gPreset, Output_VCO6_DRY_MIX, value); break;
	case knob_MIXER_VCO7:PresetChangeValue(gPreset, Output_VCO7_DRY_MIX, value); break;
	case knob_MIXER_RM2_3:PresetChangeValue(gPreset, Output_RM2_MIX3, value); break;
	case knob_MIXER_SUMQUAD: printf("TODODODOTODODODOTODODO!!\n"); break;
	case knob_MIXER_NOISE_BROWN:PresetChangeValue(gPreset, Output_BN_MIX3, value); break;
	case knob_MIXER_NOISE_VIOLET:PresetChangeValue(gPreset, Output_PUN_MIX, value); break;
		//case knob_TOTAL_OUT: PresetChangeValue(gPreset, Output_, value); break;
	}
	printf("knob %s: %d\n", Knobs[ID].name, value);

}

void Teensy_ButtonPressed(int ID, int value)
{
	printf("button %s: %d\n", Buttons[ID].name, Buttons[ID].value ? 1 : 0);



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
	case ledbutton_CANCEL_LEFT: Teensy_Cancel(0); break;
	case ledbutton_CANCEL_RIGHT: Teensy_Cancel(1); break;
	case ledbutton_FINAL: Teensy_Final(); break;
	case ledbutton_LFO: Teensy_ToState(GuiState_LfoSelect); break;
	case ledbutton_ADSR: Teensy_ToState(GuiState_AdsrSelect); break;
	case ledbutton_AD: Teensy_ToState(GuiState_AdSelect); break;
	case ledbutton_LEFT_MOD: Teensy_ToState(GuiState_CtrlSelect, ledbutton_LEFT_MOD); break;
	case ledbutton_RIGHT_MOD: Teensy_ToState(GuiState_CtrlSelect, ledbutton_RIGHT_MOD); break;
	case ledbutton_X: Teensy_ToState(GuiState_CtrlSelect, ledbutton_X); break;
	case ledbutton_Y: Teensy_ToState(GuiState_CtrlSelect, ledbutton_Y); break;
	case ledbutton_Z: Teensy_ToState(GuiState_CtrlSelect, ledbutton_Z); break;
	case ledbutton_ZPRIME: Teensy_ToState(GuiState_CtrlSelect, ledbutton_ZPRIME); break;
	case ledbutton_KBCV: Teensy_ToState(GuiState_CtrlSelect, ledbutton_KBCV); break;
	case ledbutton_VELOCITY: Teensy_ToState(GuiState_CtrlSelect, ledbutton_VELOCITY); break;

	default:
		if (IsModEdit()) {
			Teensy_SelectTarget(ID);
		}
		break;
	}
}

