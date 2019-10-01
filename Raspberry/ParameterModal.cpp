
#include "ParameterModal.h"
#include "NewModulationModal.h"

ParameterModal::ParameterModal()
{
	EnableButton(0, "Back", MenuEntry_Action, MenuAction_Ok);

	ControlsInOrder.push_back(&this->TitleText);
	SetTitle("...");
	//AddText(200, 50, "Parameter Details");
	TitleText.SetTitle("");
	TitleText.x = 200;
	TitleText.y = 80;

	AddText(200, 500, "This is a small text to explain ALL the\ngood stuff about this parameter.", align_left, font_tiny);
}

void ParameterModal::Action(int action)
{
	_screensetup_t::Action(action);



	switch (action)
	{

	case MenuAction_NewModulation:
		Modal = TheNewModulationModal;
		Modal->Parent = this;
		TheNewModulationModal->Setup(CurrentTarget);

		break;

	case MenuAction_RemoveModulation1:
	case MenuAction_RemoveModulation2:
	case MenuAction_RemoveModulation3:
	case MenuAction_RemoveModulation4:
	case MenuAction_RemoveModulation5:
	case MenuAction_RemoveModulation6:
	{
		int RM = action - MenuAction_RemoveModulation1;
		gCurrentPreset.RemoveModulation(SourcesForCurrentTarget.SourceDetails[RM].sourceid, SourcesForCurrentTarget.SourceDetails[CurrentMod].subsourceid, CurrentTarget);
		Setup(CurrentTarget);
		SketchRightDelta(-1);
	}
	break;

	case MenuAction_GotoModulation1:
	case MenuAction_GotoModulation2:
	case MenuAction_GotoModulation3:
	case MenuAction_GotoModulation4:
	case MenuAction_GotoModulation5:
	case MenuAction_GotoModulation6:
	{	int MM = action - MenuAction_GotoModulation1;
	ModSource_t mod;
	int instance;
	gCurrentPreset.RowIntToModSourceAndInstance(SourcesForCurrentTarget.SourceDetails[MM].sourceid, &mod, &instance);
	gGui.GotoPageForMod(mod, instance);
	}
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
	CurrentTarget = target;
	char txt[300];
	GetModulationTargetLongName(target, txt, 300);
	//TitleText.SetTitle(txt);
	SetTitle(txt);
	//ModSourcesForOutputStruct MSFOS;
	OutputEnum targetparam = gCurrentPreset.GetModulationOutput(target);
	gCurrentPreset.FillModSourcesForOutput(targetparam, &this->SourcesForCurrentTarget);

	if (SourcesForCurrentTarget.Sources == 0)
	{
		DisableButton(11);
	}
	else
	{
	}


	SetupTargetButton();

	int i = 0;
	for (i=0; i < 11; i++)
	{
		encoders[0][i].enabled = false;
	}

	for (i=0; i < SourcesForCurrentTarget.Sources; i++)
	{

		char txt[400];
		char ftxt[400];
		gCurrentPreset.GetModSourceName(SourcesForCurrentTarget.SourceDetails[CurrentMod].sourceid, txt, 400);
		snprintf(ftxt, 400, "Remove %s", txt);


		EnableButton(i + 1, ftxt, MenuEntry_Action, MenuAction_RemoveModulation1 + i);

		encoders[0][i*2].enabled = true;
		encoders[0][i*2].target = i;
		encoders[0][i*2].Set = 0;
		encoders[0][i*2].style = MenuEntry_ModMatrixValue;

		encoders[0][i*2].SetTitle(txt);
	}

}


uint16_t ParameterModal::GetParameterValue(int param, int encoderset)
{
	ModSource_t mod;
	int instance;
	gCurrentPreset.RowIntToModSourceAndInstance(SourcesForCurrentTarget.SourceDetails[param].sourceid, &mod, &instance);
	auto row = gCurrentPreset.GetModSourceRow(mod, instance);
	return row->targets[param].depth;
};


void ParameterModal::TweakParameterValue(int param, int delta)
{

	;
	ModSource_t mod;
	int instance;
	gCurrentPreset.RowIntToModSourceAndInstance(SourcesForCurrentTarget.SourceDetails[param].sourceid, &mod, &instance);
	gCurrentPreset.TweakModMatrix(mod, instance, param, delta);
};


void ParameterModal::SetupTargetButton()
{
	for (int i = 0; i < MAX_MODULATION_SOURCE; i++)
	{
		DisableButton(8 + i);
	}
	for (int i = 0; i < SourcesForCurrentTarget.Sources; i++)

	{
		char txt[400];
		char buttontxt[400];
		gCurrentPreset.GetModSourceName(SourcesForCurrentTarget.SourceDetails[i].sourceid, txt, 400);
		snprintf(buttontxt, 400, "Go to %s", txt);
		EnableButton(8 + i, buttontxt, MenuEntry_Action, MenuAction_GotoModulation1 + i);

	}

	if (SourcesForCurrentTarget.Sources < MAX_MODULATION_SOURCE)
	{
		EnableButton(8+SourcesForCurrentTarget.Sources, "New Modulation", MenuEntry_Action, MenuAction_NewModulation);

	}
	else
	{
		DisableButton(7);
	}
}

void ParameterModal::Render(bool active, float DT)
{
	_screensetup_t::Render(active, DT);
	/*	int ModSource = 0;
		for (; ModSource < SourcesForCurrentTarget.Sources; ModSource++)
		{
			char txt[400];
			gCurrentPreset.GetModSourceName(SourcesForCurrentTarget.SourceDetails[ModSource].sourceid, txt, 400);


			ImGui::SetCursorPos(ImVec2(240, ModSource * 30 + 160));
			ImGui::TextColored((ImVec4)(ImColor)(ModSource == CurrentMod ? gGuiResources.Highlight : gGuiResources.Normal), txt);
		}*/
}

void ParameterModal::SketchRightDelta(int delta)
{
	if (SourcesForCurrentTarget.Sources == 0)
	{
		CurrentMod = 0;
		return;
	}
	CurrentMod = (CurrentMod + SourcesForCurrentTarget.Sources * 10 + delta) % SourcesForCurrentTarget.Sources;
	SetupTargetButton();

}

void ParameterModal::SideButton(FinalLedButtonEnum b)
{
	_screensetup_t::SideButton(b);
	int deletemod = -1;
	int gotomod = -1;
	switch (b)
	{
	case ledbutton_B1: deletemod = 0; break;
	case ledbutton_B2: deletemod = 1; break;
	case ledbutton_B3: deletemod = 2; break;
	case ledbutton_B4: deletemod = 3; break;
	case ledbutton_B5: deletemod = 4; break;
	case ledbutton_B6: deletemod = 5; break;


	case ledbutton_B9: gotomod = 0; break;
	case ledbutton_B10: gotomod = 1; break;
	case ledbutton_B11: gotomod = 2; break;
	case ledbutton_B12: gotomod = 3; break;
	case ledbutton_B13: gotomod = 4; break;
	case ledbutton_B14: gotomod = 5; break;

	}

	if (deletemod > -1 && deletemod < SourcesForCurrentTarget.Sources)
	{
		//CurrentMod = targetmod;
	}
}
void ParameterModal::SketchLeftDelta(int delta)
{

}
