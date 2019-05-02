#include "ModTargetModal.h"


void ModTargetModal::SetupLeds()
{
	_screensetup_t::SetupLeds();
}



ModTargetModal::ModTargetModal()
{
	EnableButton(11, "OK", MenuEntry_Action, MenuAction_Ok);
	EnableButton(10, "Cancel", MenuEntry_Action, MenuAction_Cancel);
	EnableButton(9, "Remove Modulation", MenuEntry_Action, MenuAction_Remove);



	ControlsInOrder.push_back(&TheTargetList);
}


void GetModulationName(char *t, int len, ModSource_t mod)
{
	switch (mod)
	{
	case Source_none:

	case Source_left_mod: snprintf(t, len, "Left pad"); break;
	case Source_right_mod:snprintf(t, len, "Right pad"); break;
	case Source_x: snprintf(t, len, "Keyboard X"); break;
	case Source_y: snprintf(t, len, "Keyboard Y"); break;
	case Source_z: snprintf(t, len, "Keyboard Z"); break;
	case Source_zprime: snprintf(t, len, "Keyboard Toucher"); break;
	case Source_note: snprintf(t, len, "Keyboard Note"); break;
	case Source_vel: snprintf(t, len, "Keyboard Velocity"); break;
	case Source_left_sus: snprintf(t, len, "Left sustain"); break;
	case Source_right_sus: snprintf(t, len, "Right sustain"); break;
	case Source_left_unac: snprintf(t, len, "Left Una Corda"); break;
	case Source_right_unac: snprintf(t, len, "Right Una Corda"); break;
	case Source_Envelope: snprintf(t, len, "Envelope"); break;
	case Source_LFO: snprintf(t, len, "LFO"); break;
	default:
		snprintf(t, len, "Unknown modulation source");
	}
}

void ModTargetModal::Action(int action)
{
	switch (action)
	{

	}
}

void ModTargetModal::Render(float dt)
{
	_screensetup_t::Render(dt);


	char name[100];
	char txt[100];
	GetModulationName(name, 100, modType);

	if (ModulationSourceHasInstances(modType))
	{
		snprintf(txt, 100, "%s %d slot %d", name, this->Instance, this->TargetID);
	}
	else
	{
		snprintf(txt, 100, "%s slot %d", name, this->TargetID);

	}
	ImGui::PushFont(gGuiResources.BigFont);
	auto R = ImGui::CalcTextSize(txt);
	ImGui::SetCursorPos(ImVec2(1024 / 2 - R.x / 2, ParamMasterMargin));
	ImGui::Text(txt);
	ImGui::PopFont();


}