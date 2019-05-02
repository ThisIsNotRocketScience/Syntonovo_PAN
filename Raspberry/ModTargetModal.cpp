#include "ModTargetModal.h"


typedef struct ModulationTargetOutputEntries_t
{
	const char *name;
	int targetid;
	int categoryid;
} ModulationTargetOutputEntries_t;

const char *CategoryNames[]=
{
	"VCO1",
	"VCO2",
	"VCO3",
	"VCO4",
	"VCO5",
	"VCO6",
	"VCO7",
	"VCO8",
	"VCF1",
	"VCF2",
	"VCF1 Mixer",
	"VCF2 Mixer",
	"Cleanfeed Mixer",
	"Effect",
};


ModulationTargetOutputEntries_t Entries[] = {

//#include "PanUiMap.h"
	{ "Pitch"     , Output_VCO1_FREQ,0},
	{ "Pulsewidth", Output_VCO1_PW,0 },
{ "Pitch"     , Output_VCO2_FREQ,1 },
{ "Pulsewidth", Output_VCO2_PW,1 },
{ "Pitch"     , Output_VCO3_FREQ,2 },
{ "Pulsewidth", Output_VCO3_PW,2 },
};

void TargetList::Render(bool active, float dt)
{
	for (int i = 0; i < PageLength; i++)
	{

	}
}


void ModTargetModal::SetupLeds()
{
	_screensetup_t::SetupLeds();
}

void ModTargetModal::Action(int action)
{
	switch (action)
	{
	case MenuAction_Ok:
	{
		Parent->Action(MenuAction_CloseModal);
	}
	break;
	case MenuAction_Cancel:
	{
		auto row = gCurrentPreset.GetModSourceRow(modType, Instance);
		row->targets[TargetID].outputid = OriginalOutputID;
		row->targets[TargetID].depth = OriginalModulation;
		row->targets[TargetID].sourceid = OriginalSourceID;
		Parent->Action(MenuAction_CloseModal);
	}
	break;
	case MenuAction_Remove:
	{
		auto row = gCurrentPreset.GetModSourceRow(modType, Instance);
		row->targets[TargetID].outputid = -1;
		row->targets[TargetID].depth = 0;
		row->targets[TargetID].sourceid = 0;
		Parent->Action(MenuAction_CloseModal);
	}
	break;

	}
}



uint16_t ModTargetModal::GetParameterValue(int param, int encoderset)
{
	auto row = gCurrentPreset.GetModSourceRow(modType, Instance);
	return row->targets[param].depth;
}

void ModTargetModal::Activate()
{
	auto row = gCurrentPreset.GetModSourceRow(modType, Instance);	
	OriginalOutputID = row->targets[TargetID].outputid;
	OriginalSourceID = row->targets[TargetID].sourceid;
	OriginalModulation = row->targets[TargetID].depth;
}

ModTargetModal::ModTargetModal()
{
	EnableButton(11, "OK", MenuEntry_Action, MenuAction_Ok);
	EnableButton(10, "Cancel", MenuEntry_Action, MenuAction_Cancel);
	EnableButton(9, "Remove Modulation", MenuEntry_Action, MenuAction_Remove);

	EnableAvailableEncoder("Modulation", MenuEntry_MidValue, 0);

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


void ModTargetModal::Render(bool active, float dt)
{
	_screensetup_t::Render(active, dt);


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