#include <math.h>
#include "../libs/imgui-master/imgui.h"
#include <stdio.h>
#include "SwitchDebug.h"


uint16_t SwitchScreen::GetParameterValue(int param, int encoderset)
{
	switch (param)
	{

	}
	return 0;
};

bool SwitchScreen::GetToggle(int id)
{
	return gCurrentPreset.GetSwitch((SwitchEnum) id);
}

void SwitchScreen::DoToggle(int id)
{
	gCurrentPreset.ToggleSwitch((SwitchEnum)id);
	

}


void SwitchScreen::TweakParameterValue(int param, int delta)
{
	delta = delta > 0 ? 1 : -1;
	
};

SwitchScreen::SwitchScreen() : _screensetup_t(SCREEN_ARP)
{
	MaxInstances = __KeyZone_Count;
	Current = 0;
	int cur = 0;
	EnableButton(RB1, "Previous", MenuEntry_Action, MenuAction_Prev);
	EnableButton(RB2, "Next", MenuEntry_Action, MenuAction_Next);

	EnableButton(RB4, "Toggle", MenuEntry_Action, MenuAction_1);

}

void SwitchScreen::RenderContent(bool active, float DT)
{
	_screensetup_t::RenderContent(active, DT);

}

void SwitchScreen::Deactivate()
{
	Modal = NULL;
}


void SwitchScreen::RepeatGoto()
{
	
}

void SwitchScreen::Action(int a)
{
	switch (a)
	{
	case MenuAction_1:
		DoToggle(Current);
		break;
	case MenuAction_CloseModal: Modal = NULL; break;
	case MenuAction_Next:
	{
		Current = (Current + 1) % __SWITCH_COUNT;

	
	}break;
	case MenuAction_Prev:
	{
		Current = (Current -1 + __SWITCH_COUNT) % __SWITCH_COUNT;
		
	} break;
	}
}

void SwitchScreen::Render(bool active, float DT)
{
	RenderContent(active, DT);

	for (int i = 0; i < __SWITCH_COUNT; i++)
	{
		char txt[100];
		//snprintf(txt, 100, "%d", i + 1);
		gGui.PrintSwitchName(txt, 99, (SwitchEnum)i);

		ImGui::SetCursorPos(ImVec2(100, 20 * (i - Current) + 100));
		if (gCurrentPreset.GetSwitch((SwitchEnum)i))
		{
			ImGui::TextColored(ImVec4(1, 1, 0, 1),  txt);
		}
		else
		{
			ImGui::TextColored(ImVec4(0.8, 0.8, 0.8, 1), txt);
		
		}
		if (i == Current)
		{
			ImGui::SetCursorPos(ImVec2(10, 20 * (i - Current) + 100));
			ImGui::TextColored(ImVec4(1, 0, 1, 1), "---> ");

		}
	}

	char txt2[100];
	//snprintf(txt, 100, "%d", i + 1);
	gGui.PrintSwitchName(txt2, 99, (SwitchEnum)Current);

	ImGui::SetCursorPos(ImVec2(300, 300));
	if (gCurrentPreset.GetSwitch((SwitchEnum)Current))
	{
		ImGui::TextColored(ImVec4(1, 1, 0, 1), txt2);
	}
	else
	{
		ImGui::TextColored(ImVec4(0.8, 0.8, 0.8, 1), txt2);

	}


	RenderModalBox(active, DT);
	//auto row = gCurrentPreset.GetModSourceRow(modType, ActiveInstance);
}

void SwitchScreen::Activate()
{

	_control_t::Activate();
}

void SwitchScreen::EncoderPress(FinalEncoderEnum button)
{
	if (Modal)
	{
		Modal->EncoderPress(button);
		return;
	}

	if (currentencoderset == 1)
	{
		switch (button)
		{
		}
	}

	_screensetup_t::EncoderPress(button);
}
