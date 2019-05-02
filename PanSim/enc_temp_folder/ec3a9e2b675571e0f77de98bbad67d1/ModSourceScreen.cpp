#include "../libs/imgui-master/imgui.h"
#include <stdio.h>
#include "ModSourceScreen.h"

uint16_t ModSourceScreen::GetParameterValue(int param, int encoderset)
{
	if (encoderset == 0)
	{
		return gCurrentPreset.GetModParameterValue((ModParameters)param, ActiveInstance);
	}
	else
	{
		auto row = gCurrentPreset.GetModSourceRow(modType, ActiveInstance);
		return row->targets[param].depth;
	}
};

void ModSourceScreen::TweakParameterValue(int param, int delta)
{
	if (currentencoderset == 0)
	{
		gCurrentPreset.TweakModulation((ModParameters)param, ActiveInstance, delta);
	}
	else
	{
		gCurrentPreset.TweakModMatrix(modType, ActiveInstance, param, delta);
	}
};

bool ModulationSourceHasInstances(ModSource_t modType)
{
	switch (modType)
	{
	case Source_Operator:
	case Source_Envelope:
	case Source_LFO:
		return true;
	}
	return false;
};

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
	theModTargetModal.Parent = this;
	myScreen = screen;
	modType = ModTypeFromScreen(screen);
	HasActiveInstanceDisplay = ModulationSourceHasInstances(modType);
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
		encoders[1][i].target = i;
		encoders[1][i].Set= 1;
		encoders[1][i].style = MenuEntry_ModMatrixValue;
		char txt[30];
		snprintf(txt,30, "target %d", i);
		encoders[1][i].SetTitle(txt);
	}
}

void ModSourceScreen::Deactivate()
{
	Modal = NULL;
}


void ModSourceScreen::Action(int a)
{
	switch (a)
	{
	case MenuAction_Next: ActiveInstance = (ActiveInstance + 1) % MaxInstances; break;
	case MenuAction_Prev: ActiveInstance = (ActiveInstance + MaxInstances - 1) % MaxInstances; break;
	}
}

void ModSourceScreen::Render(bool active, float DT)
{
	RenderContent(active, DT);
	
	if (HasActiveInstanceDisplay)
	{
		for (int i = 0; i < MaxInstances; i++)
		{
			char txt[10];
			snprintf(txt,10, "%d", i);
			RenderLettersInABox(i * 40 + 200, ButtonHeight(1), i == ActiveInstance, txt, 35, 35);
		}
	}
	RenderModalBox(active, DT);
	//auto row = gCurrentPreset.GetModSourceRow(modType, ActiveInstance);
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

void ModSourceScreen::OpenTargetModal(int n) 
{
	theModTargetModal.Instance = ActiveInstance;
	theModTargetModal.TargetID = n;
	theModTargetModal.modType = modType;

	auto row = gCurrentPreset.GetModSourceRow(modType, ActiveInstance);
	

	theModTargetModal.CurrentTarget = row->targets[target].outputid;

	Modal = &theModTargetModal;
}

void ModSourceScreen::EncoderPress(FinalEncoderEnum button)
{
	if (currentencoderset == 1)
	{
		switch (button)
		{
		case encoder_F1: OpenTargetModal(0); break;
		case encoder_F2: OpenTargetModal(1); break;
		case encoder_F3: OpenTargetModal(2); break;
		case encoder_F4: OpenTargetModal(3); break;
		case encoder_F5: OpenTargetModal(4); break;
		case encoder_F6: OpenTargetModal(5); break;
		case encoder_F7: OpenTargetModal(6); break;
		case encoder_F8: OpenTargetModal(7); break;
		case encoder_F9: OpenTargetModal(8); break;
		case encoder_F10: OpenTargetModal(9); break;
		case encoder_F11: OpenTargetModal(10); break;
		}
	}
}