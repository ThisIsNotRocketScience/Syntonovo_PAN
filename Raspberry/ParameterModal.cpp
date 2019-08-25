
#include "ParameterModal.h"


ParameterModal::ParameterModal()
{
	EnableButton(1, "Back", MenuEntry_Action, MenuAction_Ok);

	EnableButton(7, "New Modulation", MenuEntry_Action, MenuAction_NewModulation);
	EnableButton(9, "Remove Modulation", MenuEntry_Action, MenuAction_RemoveModulation);
	EnableButton(11, "To Modulation Source", MenuEntry_Action, MenuAction_GotoModulation);

	ControlsInOrder.push_back(&this->TitleText);

	AddText(100, 20, "Parameter Details");
	TitleText.SetTitle("hah!");
	TitleText.x = 100;
	TitleText.y = 60;
}

void ParameterModal::Action(int action)
{
	_screensetup_t::Action(action);

	switch (action)
	{
	case MenuAction_GotoModulation:
		ModSource_t mod;
		int instance;
		gCurrentPreset.RowIntToModSourceAndInstance(SourcesForCurrentTarget.SourceDetails[CurrentMod].sourceid, &mod, &instance);
		gGui.GotoPageForMod(mod, instance);
		break;
	case MenuAction_Ok:
	{
		Parent->Action(MenuAction_CloseModal);
	}break;

	}
}


void ParameterModal::Setup(OutputEnum target)
{
	CurrentMod = 0;

	char txt[300];
	GetModulationTargetLongName(target, txt, 300);
	TitleText.SetTitle(txt);

	//ModSourcesForOutputStruct MSFOS;
	OutputEnum targetparam = gCurrentPreset.GetModulationOutput(target);
	gCurrentPreset.FillModSourcesForOutput(targetparam, &this->SourcesForCurrentTarget);


}

void ParameterModal::Render(bool active, float DT)
{
	_screensetup_t::Render(active, DT);
	int ModSource = 0;
	for (; ModSource < SourcesForCurrentTarget.Sources; ModSource++)
	{
		char txt[400];
		gCurrentPreset.GetModSourceName(SourcesForCurrentTarget.SourceDetails[ModSource].sourceid, txt, 400);
		if (ModSource == CurrentMod)
		{
			ImGui::Text(txt);

		}
		else
		{
			ImGui::Text(txt);
		}
	}
	ImGui::Text("new");

}
