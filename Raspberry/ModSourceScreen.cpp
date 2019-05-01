#include "../libs/imgui-master/imgui.h"
#include <stdio.h>
#include "ModSourceScreen.h"


uint16_t ModSourceScreen::GetParameterValue(int param)
{
	return gCurrentPreset.GetModParameterValue((ModParameters)param, ActiveInstance);
};

void ModSourceScreen::TweakParameterValue(int param, int delta)
{
	gCurrentPreset.TweakModulation((ModParameters)param, ActiveInstance, delta);
}

ModSource_t ModSourceScreen::ModTypeFromScreen(Screens_t screen)
{
	switch (screen)
	{
	case SCREEN_LFO: return Source_LFO;
	case SCREEN_ENVELOPE: return Source_Envelope;

	case SCREEN_X: return Source_x;
	case SCREEN_Y: return Source_y;
	case SCREEN_Z: return Source_y;

	case SCREEN_TOUCH: return Source_zprime;
	case SCREEN_KEYBOARD: return Source_note;
	

	case SCREEN_VELOCITY: return Source_vel;


	}

	return Source_none;
}

ModSourceScreen::ModSourceScreen(Screens_t screen)
{
	MaxInstances = 16;
	ActiveInstance = 0;
	myScreen = screen;
	modType = ModTypeFromScreen(screen);
	switch (myScreen)
	{
	case SCREEN_ENVELOPE:
		HasActiveInstanceDisplay = true;

		EnableAvailableEncoder("Attack", MenuEntry_EnvelopeValue, Envelope_Attack);
		EnableAvailableEncoder("Decay", MenuEntry_EnvelopeValue, Envelope_Decay);
		EnableAvailableEncoder("Sustain", MenuEntry_EnvelopeValue, Envelope_Sustain);
		EnableAvailableEncoder("Release", MenuEntry_EnvelopeValue, Envelope_Release);
		EnableAvailableEncoder("Curve", MenuEntry_EnvelopeValue, Envelope_Curve);

		break;
	case SCREEN_LFO:

		EnableAvailableEncoder("Speed", MenuEntry_EnvelopeValue, LFO_Speed);
		EnableAvailableEncoder("Shape", MenuEntry_EnvelopeValue, LFO_Shape);
		EnableAvailableEncoder("Reset Phase", MenuEntry_EnvelopeValue, LFO_ResetPhase);
		EnableAvailableEncoder("Depth", MenuEntry_EnvelopeValue, LFO_Depth);
		HasActiveInstanceDisplay = true;
		break;
	}
	if (HasActiveInstanceDisplay)
	{
		EnableButton(1, "Previous", MenuEntry_Action, MenuAction_Prev);
		EnableButton(8, "Next", MenuEntry_Action, MenuAction_Next);
	}
	EnableButton(9, "Parameters", MenuEntry_EncoderSet, 0);
	EnableButton(10, "Targets", MenuEntry_EncoderSet, 1);

	for (int i = 0; i < 11; i++)
	{
		encoders[1][i].enabled = true;
		encoders[1][i].style = MenuEntry_ModMatrixValue;
		char txt[30];
		snprintf(txt,30, "target %d", i);
		encoders[1][i].SetTitle(txt);
	}
}



void ModSourceScreen::Action(int a)
{
	switch (a)
	{
	case MenuAction_Next: ActiveInstance = (ActiveInstance + 1) % MaxInstances; break;
	case MenuAction_Prev: ActiveInstance = (ActiveInstance + MaxInstances - 1) % MaxInstances; break;
	}
}

void ModSourceScreen::Render(float DT)
{
	_screensetup_t::Render(DT);
	if (HasActiveInstanceDisplay)
	{
		for (int i = 0; i < MaxInstances; i++)
		{
			char txt[10];
			snprintf(txt,10, "%d", i);
			RenderLettersInABox(i * 40 + 200, ButtonHeight(1), i == ActiveInstance, txt, 35, 35);
		}
	}
	auto row = gCurrentPreset.GetModSourceRow(modType, ActiveInstance);
}

void ModSourceScreen::Activate()
{
	_control_t::Activate();
}

uint16_t ModSourceScreen::GetModValue(int target)
{
	auto row = gCurrentPreset.GetModSourceRow(modType, ActiveInstance);
	return row->targets[target].depth;
}