
#include "PanHeader.h"


#ifdef SIMULATEINCOMINGSERIAL


void Raspberry_ToState(GuiState_t state, int modselect)
{
	
}


void Raspberry_EditLfo(LfoModulation_t& mod)
{
	
}

void Raspberry_EditAdsr(AdsrModulation_t& mod)
{
	
}

void Raspberry_EditAd(AdModulation_t& mod)
{
	
}

void Raspberry_EditCtrl(ControlModulation_t& mod)
{
	
}

void Raspberry_SelectTarget(int target)
{
	
}

void Raspberry_UpdateTarget(int target, int param, int depth)
{
	
}

void Raspberry_SetLfoParam(SubParam_t subparam, uint32_t value)
{
}

void Raspberry_SetAdsrParam(SubParam_t subparam, uint32_t value)
{
}

void Raspberry_SetAdParam(SubParam_t subparam, uint32_t value)
{
}

void Raspberry_OutputChangeValue(int output, uint32_t value)
{
}

void Raspberry_SetSwitches(uint32_t* switches)
{
}

void Raspberry_SetName(char *newname)
{
}

void Raspberry_EncoderTurn(EncoderEnum id, int delta)
{
}

void Raspberry_Reset()
{
}
#else

void Raspberry_ToState(GuiState_t state, int modselect)
{
	Raspberry_guidata.GuiState = state;
	Raspberry_guidata.ModSelect = modselect;
	Raspberry_guidata.editLfo = 0;
	Raspberry_guidata.editAdsr = 0;
	Raspberry_guidata.editAd = 0;
	Raspberry_guidata.editTargets = 0;
	Raspberry_guidata.selectTarget = -1;
	Raspberry_guidata.dirty = true;
}


void Raspberry_EditLfo(LfoModulation_t& mod)
{
	memcpy(&Raspberry_guidata.dataLfo, &mod, sizeof(LfoModulation_t));
	Raspberry_guidata.editLfo = &Raspberry_guidata.dataLfo;
	Raspberry_guidata.editTargets = &Raspberry_guidata.dataLfo.target[0];

	Raspberry_guidata.dirty = true;
}

void Raspberry_EditAdsr(AdsrModulation_t& mod)
{
	memcpy(&Raspberry_guidata.dataAdsr, &mod, sizeof(AdsrModulation_t));
	Raspberry_guidata.editAdsr = &Raspberry_guidata.dataAdsr;
	Raspberry_guidata.editTargets = &Raspberry_guidata.dataAdsr.target[0];
	Raspberry_guidata.dirty = true;
}

void Raspberry_EditAd(AdModulation_t& mod)
{
	memcpy(&Raspberry_guidata.dataAd, &mod, sizeof(AdModulation_t));
	Raspberry_guidata.editAd = &Raspberry_guidata.dataAd;
	Raspberry_guidata.editTargets = &Raspberry_guidata.dataAd.target[0];
	Raspberry_guidata.dirty = true;
}

void Raspberry_EditCtrl(ControlModulation_t& mod)
{
	memcpy(&Raspberry_guidata.dataCtrl, &mod, sizeof(ControlModulation_t));
	Raspberry_guidata.editCtrl = &Raspberry_guidata.dataCtrl;
	Raspberry_guidata.editTargets = &Raspberry_guidata.dataCtrl.target[0];
	Raspberry_guidata.dirty = true;
}

void Raspberry_SelectTarget(int target)
{
	Raspberry_guidata.selectTarget = target;
	Raspberry_guidata.dirty = true;
}

void Raspberry_UpdateTarget(int target, int param, int depth)
{
	Raspberry_guidata.editTargets[target].param = param;
	Raspberry_guidata.editTargets[target].depth = depth;
	Raspberry_guidata.dirty = true;
}

void Raspberry_SetLfoParam(SubParam_t subparam, uint32_t value)
{
	switch (subparam) {
	case Sub_lfo_shape:
		Raspberry_guidata.editLfo->shape = value;
		break;
	case Sub_lfo_speed:
		Raspberry_guidata.editLfo->speed = value;
		break;
	}
	Raspberry_guidata.dirty = true;
}

void Raspberry_SetAdsrParam(SubParam_t subparam, uint32_t value)
{
	switch (subparam) {
	case Sub_adsr_a:
		Raspberry_guidata.editAdsr->a = value;
		break;
	case Sub_adsr_d:
		Raspberry_guidata.editAdsr->d = value;
		break;
	case Sub_adsr_s:
		Raspberry_guidata.editAdsr->s = value;
		break;
	case Sub_adsr_r:
		Raspberry_guidata.editAdsr->r = value;
		break;
	}
	Raspberry_guidata.dirty = true;
}

void Raspberry_SetAdParam(SubParam_t subparam, uint32_t value)
{
	switch (subparam) {
	case Sub_ad_a:
		Raspberry_guidata.editAd->a = value;
		break;
	case Sub_ad_d:
		Raspberry_guidata.editAd->d = value;
		break;
	}
	Raspberry_guidata.dirty = true;
}

void Raspberry_OutputChangeValue(int output, uint32_t value)
{
	Raspberry_guidata.outputValues[output] = value;
	Raspberry_guidata.dirty = true;
}

void Raspberry_SetSwitches(uint32_t* switches)
{
	Raspberry_guidata.switches[0] = switches[0];
	Raspberry_guidata.dirty = true;
}

void Raspberry_SetName(char *newname)
{
	for (int i = 0; i < PRESET_NAME_LENGTH; i++) Raspberry_guidata.PresetName[i] = newname[i];
	Raspberry_guidata.dirty = true;
}

void Raspberry_EncoderTurn(EncoderEnum id, int delta)
{
	printf("encoder %s: %d\n", Encoders[id].name, delta);
	Raspberry_guidata.dirty = true;
}

void Raspberry_Reset()
{
	Raspberry_guidata.GuiState = GuiState_Root;
	Raspberry_guidata.dirty = true;
}
#endif