#include <math.h>
#include "../libs/imgui-master/imgui.h"
#include <stdio.h>
#include "ArpeggiatorScreen.h"

uint16_t ArpeggiatorScreen::GetParameterValue(int param, int encoderset)
{
	switch (param)
	{

	}
	return 0;
};
enum
{
	
	Arp_Enable = __ArpeggioMode_Count,
	Arp_Reset,
	Arp_KillAll,
	Arp_Hold,
	Arp_Mode,
	Arp_Octaves,
	Arp_speedlower,
	Arp_speedupper,
	Arp_Transpose,


};

bool ArpeggiatorScreen::GetToggle(int id)
{
	switch (id)
	{
	case Arp_Enable:  return gCurrentPreset.keyzone[ActiveInstance].type == PresetKeyzoneType_Arpeggiator;
	case Arp_Reset:  return settings->ResetTimeOnFirstKey;
	case Arp_KillAll:  return settings->KillNotesAfterLastNoteOff;
	case Arp_Hold:  return settings->Hold;


	case Arp_Up: return settings->Mode == Arp_Up;
	case Arp_Down: return settings->Mode == Arp_Down;
	case Arp_UpDown: return settings->Mode == Arp_UpDown;
	case Arp_DownUp: return settings->Mode == Arp_DownUp;
	case Arp_InOrder: return settings->Mode == Arp_InOrder;
	case Arp_ReverseOrder: return settings->Mode == Arp_ReverseOrder;
	case Arp_Random: return settings->Mode == Arp_Random;

	}
	return false;
}

void ArpeggiatorScreen::DoToggle(int id)
{
	switch (id)
	{
	case Arp_Enable:
	{
		bool current = gCurrentPreset.keyzone[ActiveInstance].type == PresetKeyzoneType_Arpeggiator;
		if (current)
		{
			gCurrentPreset.keyzone[ActiveInstance].type = PresetKeyzoneType_Single;
		}
		else
		{
			gCurrentPreset.keyzone[ActiveInstance].type = PresetKeyzoneType_Arpeggiator;
		}
	}
	break;

	case Arp_Reset: settings->ResetTimeOnFirstKey = ~settings->ResetTimeOnFirstKey; break;
	case Arp_KillAll: settings->KillNotesAfterLastNoteOff = ~settings->KillNotesAfterLastNoteOff; break;
	case Arp_Hold: settings->Hold = ~settings->Hold; break;


	case Arp_Up:  settings->Mode = Arp_Up; break;
	case Arp_Down: settings->Mode = Arp_Down; break;
	case Arp_UpDown: settings->Mode = Arp_UpDown; break;
	case Arp_DownUp: settings->Mode = Arp_DownUp; break;
	case Arp_InOrder: settings->Mode = Arp_InOrder; break;
	case Arp_ReverseOrder: settings->Mode = Arp_ReverseOrder; break;
	case Arp_Random: settings->Mode = Arp_Random; break;
	}

}


void ArpeggiatorScreen::TweakParameterValue(int param, int delta)
{
	switch (param)
	{	
	case Arp_Octaves:break;
	case Arp_Transpose:break;
	case Arp_speedupper:break;
	case Arp_speedlower :break;
	
	}
};

void ArpeggiatorScreen::SetActiveInstance(int id)
{
	ActiveInstance = id % MaxInstances;

	char newtitle[200];
	snprintf(newtitle, 200, "Arpeggiator for zone %d", ActiveInstance + 1);
	SetTitle(newtitle);
	snprintf(newtitle, 200, "Goto zone %d", ActiveInstance + 1);
	EnableButton(LB2, newtitle, MenuEntry_Action, MenuAction_1);

	settings = &gCurrentPreset.keyzone[ActiveInstance].arpsettings;
}

ArpeggiatorScreen::ArpeggiatorScreen() : _screensetup_t(SCREEN_ARP)
{
	MaxInstances = __KeyZone_Count;

	int cur = 0;
	EnableButton(LB1, "Previous", MenuEntry_Action, MenuAction_Prev);
	EnableButton(RB1, "Next", MenuEntry_Action, MenuAction_Next);

	EnableButton(LB4, "Enabled", MenuEntry_FeatureToggle, Arp_Enable);
	EnableButton(LB5, "Stop gates after last note", MenuEntry_FeatureToggle, Arp_KillAll);
	EnableButton(LB6, "Hold", MenuEntry_FeatureToggle, Arp_Hold);
	EnableButton(LB7, "Reset Timing", MenuEntry_FeatureToggle, Arp_Reset);

	EnableButton(RB3, "Up", MenuEntry_FeatureToggle, Arp_Up);
	EnableButton(RB4, "Down", MenuEntry_FeatureToggle, Arp_Down);
	EnableButton(RB5, "UpDown", MenuEntry_FeatureToggle, Arp_UpDown);
	EnableButton(RB6, "DownUp", MenuEntry_FeatureToggle, Arp_DownUp);
	EnableButton(RB7, "Random", MenuEntry_FeatureToggle, Arp_Random);
	EnableButton(RB8, "InOrder", MenuEntry_FeatureToggle, Arp_InOrder);
	EnableButton(RB9, "ReverseOrder", MenuEntry_FeatureToggle, Arp_ReverseOrder);

	//EnableAvailableEncoder("mode", MenuEntry_Value, Arp_Mode,-1);
	EnableAvailableEncoder("octaves", MenuEntry_Value, Arp_Octaves, -1);
	EnableAvailableEncoder("transpose", MenuEntry_Value, Arp_Transpose, -1);
	EnableAvailableEncoder("time upper", MenuEntry_Value, Arp_speedupper, -1);
	EnableAvailableEncoder("time lower", MenuEntry_Value, Arp_speedlower, -1);

	
	

	SetActiveInstance(0);
}

void ArpeggiatorScreen::RenderContent(bool active, float DT)
{
	_screensetup_t::RenderContent(active, DT);

}

void ArpeggiatorScreen::Deactivate()
{
	Modal = NULL;
}


void ArpeggiatorScreen::RepeatGoto()
{
	SetActiveInstance((ActiveInstance + 1) % MaxInstances);
}

void ArpeggiatorScreen::Action(int a)
{
	switch (a)
	{
	case MenuAction_1:
		gGui.GotoPageForKeyZone(ActiveInstance);
		break;
	case MenuAction_CloseModal: Modal = NULL; break;
	case MenuAction_Next:
	{
		SetActiveInstance((ActiveInstance + 1) % MaxInstances);
	}break;
	case MenuAction_Prev:
	{
		SetActiveInstance((ActiveInstance + MaxInstances - 1) % MaxInstances);
	} break;
	}
}

void ArpeggiatorScreen::Render(bool active, float DT)
{
	RenderContent(active, DT);

	for (int i = 0; i < MaxInstances; i++)
	{
		char txt[10];
		snprintf(txt, 10, "%d", i + 1);
		bool used = false;


		for (int j = 0; j < NUM_KEY_MAP_TARGETS; j++)
		{
			if (gCurrentPreset.key_mapping[j].keyzone == ActiveInstance) used = true;
		};

		int x = (i - MaxInstances / 2) * 40 + 512;
		int y = 60;

		int value = 0;
		RenderLettersInABox(x, y, i == ActiveInstance, txt, 35, 35, used, value, false);
	}



	RenderModalBox(active, DT);
	//auto row = gCurrentPreset.GetModSourceRow(modType, ActiveInstance);
}

void ArpeggiatorScreen::Activate()
{

	_control_t::Activate();
}

void ArpeggiatorScreen::EncoderPress(FinalEncoderEnum button)
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
