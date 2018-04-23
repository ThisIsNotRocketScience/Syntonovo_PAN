#include "imgui.h"

#include "PanHeader.h"

typedef struct
{
	GuiState_t GuiState;
	int ModSelect;

	LfoModulation_t* editLfo;
	AdsrModulation_t* editAdsr;
	AdModulation_t* editAd;
	ControlModulation_t* editCtrl;
	ModTarget_t* editTargets;
	int selectTarget;
} Raspberry_GuiData_t;

static Raspberry_GuiData_t Raspberry_guidata;

void Raspberry_ToState(GuiState_t state, int modselect)
{
	Raspberry_guidata.GuiState = state;
	Raspberry_guidata.ModSelect = modselect;
	Raspberry_guidata.editLfo = 0;
	Raspberry_guidata.editAdsr = 0;
	Raspberry_guidata.editAd = 0;
	Raspberry_guidata.editTargets = 0;
	Raspberry_guidata.selectTarget = -1;
}

void Raspberry_EditLfo(LfoModulation_t& mod)
{
	Raspberry_guidata.editLfo = &mod;
	Raspberry_guidata.editTargets = &mod.target[0];
}

void Raspberry_EditAdsr(AdsrModulation_t& mod)
{
	Raspberry_guidata.editAdsr = &mod;
	Raspberry_guidata.editTargets = &mod.target[0];
}

void Raspberry_EditAd(AdModulation_t& mod)
{
	Raspberry_guidata.editAd = &mod;
	Raspberry_guidata.editTargets = &mod.target[0];
}

void Raspberry_EditCtrl(ControlModulation_t& mod)
{
	Raspberry_guidata.editCtrl = &mod;
	Raspberry_guidata.editTargets = &mod.target[0];
}

void Raspberry_SelectTarget(int target)
{
	Raspberry_guidata.selectTarget = target;
}

const char* ParamLabel(int param)
{
	return ParamToName(param);
}

float DepthLabel(int depth)
{
	return (float)(int16_t)depth * (1.0 / 0x4000);
}

void TargetsList()
{
	if (!Raspberry_guidata.editTargets) return;

	ImGui::Columns(2);

	for (int i = 0; i < 16; i++) {
		if (i == Raspberry_guidata.selectTarget) {
			ImGui::PushStyleColor(ImGuiCol_Text, 0xFFFFFFFF);
		}
		if (Raspberry_guidata.editTargets[i].param != 0) {
			ImGui::Text(ParamLabel(Raspberry_guidata.editTargets[i].param)); ImGui::NextColumn();
			ImGui::Text("%1.3f", DepthLabel(Raspberry_guidata.editTargets[i].depth)); ImGui::NextColumn();
		}
		if (i == Raspberry_guidata.selectTarget) {
			ImGui::PopStyleColor();
		}
	}
}

void Raspberry_RenderScreen()
{
	if (Raspberry_guidata.GuiState == GuiState_LfoSelect)
	{
		ImGui::Text("LFO");
		TargetsList();
	}
	else if (Raspberry_guidata.GuiState == GuiState_AdsrSelect)
	{
		ImGui::Text("ADSR");
		TargetsList();
	}
	else if (Raspberry_guidata.GuiState == GuiState_AdSelect)
	{
		ImGui::Text("AD");
		TargetsList();
	}
	else if (Raspberry_guidata.GuiState == GuiState_CtrlSelect)
	{
		ImGui::Text("Controller");
		TargetsList();
	}
}

void Raspberry_EncoderTurn(int id, int delta)
{

}