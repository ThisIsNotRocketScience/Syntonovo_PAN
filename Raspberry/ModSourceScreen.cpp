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
	case SCREEN_Z: return Source_z;
	
	case SCREEN_LMOD: return Source_left_mod;
	case SCREEN_RMOD: return Source_right_mod;
	case SCREEN_LSUS: return Source_left_sus;
	case SCREEN_RSUS: return Source_right_sus;
	case SCREEN_LUNA: return Source_left_unac;
	case SCREEN_RUNA: return Source_right_unac;

	case SCREEN_TOUCH: return Source_zprime;
	case SCREEN_KEYBOARD: return Source_note;
	

	case SCREEN_VELOCITY: return Source_vel;


	}

	return Source_none;
}
void ModSourceScreen::SetActiveInstance(int id)
{
	if (HasActiveInstanceDisplay)
	{
		ActiveInstance = id;
	}
}

ModSourceScreen::ModSourceScreen(Screens_t screen, ModSource_t modsourcetype)
{
	ModSource = modsourcetype;
	MaxInstances = 16;
	ActiveInstance = 0;
	theModTargetModal.Parent = this;
	myScreen = screen;
	modType = ModTypeFromScreen(screen);
	HasActiveInstanceDisplay = ModulationSourceHasInstances(modType);
	switch (myScreen)
	{

	case SCREEN_X:
		EnableAvailableEncoder("Deadzone", MenuEntry_EnvelopeValue, KeyboardParam_DeadzoneX);
		EnableAvailableEncoder("Scale", MenuEntry_EnvelopeValue, KeyboardParam_ScaleX);

		break;
	case SCREEN_Y:
		EnableAvailableEncoder("Deadzone", MenuEntry_EnvelopeValue, KeyboardParam_DeadzoneY);
		EnableAvailableEncoder("Scale", MenuEntry_EnvelopeValue, KeyboardParam_ScaleY);

		break;
	case SCREEN_Z:
		EnableAvailableEncoder("Deadzone", MenuEntry_EnvelopeValue, KeyboardParam_DeadzoneZ);
		EnableAvailableEncoder("Scale", MenuEntry_EnvelopeValue, KeyboardParam_ScaleZ);

		break;
	case SCREEN_LMOD:
		EnableAvailableEncoder("Deadzone", MenuEntry_EnvelopeValue, KeyboardParam_DeadzoneLMod);
		EnableAvailableEncoder("Scale", MenuEntry_EnvelopeValue, KeyboardParam_ScaleLMod);
		break;
	case SCREEN_RMOD:
		EnableAvailableEncoder("Deadzone", MenuEntry_EnvelopeValue, KeyboardParam_DeadzoneRMod);
		EnableAvailableEncoder("Scale", MenuEntry_EnvelopeValue, KeyboardParam_ScaleRMod);
		break;
	case SCREEN_LSUS:
		EnableAvailableEncoder("Deadzone", MenuEntry_EnvelopeValue, KeyboardParam_DeadzoneLSus);
		EnableAvailableEncoder("Scale", MenuEntry_EnvelopeValue, KeyboardParam_ScaleLSus);
		break;
	case SCREEN_RSUS:
		EnableAvailableEncoder("Deadzone", MenuEntry_EnvelopeValue, KeyboardParam_DeadzoneRSus);
		EnableAvailableEncoder("Scale", MenuEntry_EnvelopeValue, KeyboardParam_ScaleRSus);
		break;
	case SCREEN_LUNA:
		EnableAvailableEncoder("Deadzone", MenuEntry_EnvelopeValue, KeyboardParam_DeadzoneLUna);
		EnableAvailableEncoder("Scale", MenuEntry_EnvelopeValue, KeyboardParam_ScaleLUna);
		break;
	case SCREEN_RUNA:
		EnableAvailableEncoder("Deadzone", MenuEntry_EnvelopeValue, KeyboardParam_DeadzoneRUna);
		EnableAvailableEncoder("Scale", MenuEntry_EnvelopeValue, KeyboardParam_ScaleRUna);
		break;

	case SCREEN_ENVELOPE:
		HasActiveInstanceDisplay = true;

		EnableButton(3, "Legato", MenuEntry_EnvelopeToggle, Envelope_Retrigger);

		EnableAvailableEncoder("Attack", MenuEntry_EnvelopeValue, Envelope_Attack);
		EnableAvailableEncoder("Decay", MenuEntry_EnvelopeValue, Envelope_Decay);
		EnableAvailableEncoder("Sustain", MenuEntry_EnvelopeValue, Envelope_Sustain);
		EnableAvailableEncoder("Release", MenuEntry_EnvelopeValue, Envelope_Release);
//		EnableAvailableEncoder("Curve", MenuEntry_EnvelopeValue, Envelope_Curve);

		break;
	case SCREEN_LFO:
		EnableButton(3, "Reset on key", MenuEntry_EnvelopeToggle, LFO_ResetOnKey);
		EnableAvailableEncoder("Speed", MenuEntry_EnvelopeValue, LFO_Speed);
	//	EnableAvailableEncoder("Shape", MenuEntry_EnvelopeValue, LFO_Shape);
		EnableAvailableEncoder("Reset Phase", MenuEntry_EnvelopeValue, LFO_ResetPhase);
		//EnableAvailableEncoder("Depth", MenuEntry_EnvelopeValue, LFO_Depth);
		HasActiveInstanceDisplay = true;
		break;
	}
	if (HasActiveInstanceDisplay)
	{
		EnableButton(0, "Previous", MenuEntry_Action, MenuAction_Prev);
		EnableButton(7, "Next", MenuEntry_Action, MenuAction_Next);
	}
	EnableButton(9, "Parameters", MenuEntry_EncoderSet, 0);
	EnableButton(10, "Targets", MenuEntry_EncoderSet, 1);
	
	auto row = gCurrentPreset.GetModSourceRow(modType, ActiveInstance);
	
	for (int i = 0; i < 11; i++)
	{
		encoders[1][i].enabled = true;
		encoders[1][i].target = i;
		encoders[1][i].Set= 1;
		encoders[1][i].style = MenuEntry_ModMatrixValue;
		
//		encoders[1][i].SetTitle(GetModulationTargetName(row->targets[i].outputid));		
	}
	SetEncoderNames();
}

void ModSourceScreen::RenderContent(bool active, float DT)
{
	_screensetup_t::RenderContent(active, DT);
	int x = 100;
	int y = 200;
	switch (myScreen)
	{
	case SCREEN_ENVELOPE:
	{
		float A = gCurrentPreset.GetModParameterValue(Envelope_Attack, ActiveInstance)/65535.0f;
		float D = gCurrentPreset.GetModParameterValue(Envelope_Decay, ActiveInstance) / 65535.0f;
		float S = gCurrentPreset.GetModParameterValue(Envelope_Sustain, ActiveInstance) / 65535.0f;
		float R = gCurrentPreset.GetModParameterValue(Envelope_Release, ActiveInstance) / 65535.0f;


		float W = 500;
		float H = 150;
		float CumW = 0;

		float AW = (W / 4.0) * A;
		float DW = (W / 4.0) * D;
		float SW = (W / 4.0);
		
		float RW = (W / 4.0) * R;

		float TotalW = AW + DW + SW + RW;
		x = 512 - TotalW / 2;

		ImVec2 S1(x, y + H);
		ImVec2 S2(x, y + H - H / 2);
		ImVec2 S3(x + AW / 2, y );
		ImVec2 S4(x + AW , y);
		CumW = x + AW;
		ImVec2 S5(CumW, y  + H - (((S + 1 ) /2) * H ));
		ImVec2 S6(CumW + DW/2, y + H - S * H);
		ImVec2 S7(CumW + DW, y + H - S * H);
		CumW += DW;
		CumW += SW; 
		ImVec2 S8(CumW , y + H - S * H);
		
		ImVec2 S9(CumW , y + H - (S/2) * H );
		ImVec2 S10(CumW + RW/2, y + H);
		ImVec2 S11(CumW + RW, y + H);

		ImGui::GetWindowDrawList()->AddBezierCurve(S1, S2, S3, S4, gGuiResources.Normal, 5);

		ImGui::GetWindowDrawList()->AddBezierCurve(S4, S5, S6, S7, gGuiResources.Normal, 5);
		ImGui::GetWindowDrawList()->AddLine(S7, S8, gGuiResources.Normal, 5);
		ImGui::GetWindowDrawList()->AddBezierCurve(S8, S9, S10, S11, gGuiResources.Normal, 5);

	}
		break;
	}
}

void ModSourceScreen::Deactivate()
{
	Modal = NULL;
}

void ModSourceScreen::SetEncoderNames()
{
	auto row = gCurrentPreset.GetModSourceRow(modType, ActiveInstance);

	for (int i = 0; i < 11; i++)
	{
		encoders[1][i].enabled = true;
		encoders[1][i].target = i;
		encoders[1][i].Set = 1;
		encoders[1][i].style = row->targets[i].outputid!= NOMODTARGET ? MenuEntry_ModMatrixValue: MenuEntry_Ghosted;
		
		encoders[1][i].SetTitle(GetModulationTargetName(row->targets[i].outputid));
	}
	for (int i = 0; i < 14; i++)
	{
		buttons[i].Instance = ActiveInstance;
	}
}

int ModSourceScreen::FindNextUsed(int start)
{
	for (int i = 0; i < 15; i++)
	{
		bool used = false;

		auto row = gCurrentPreset.GetModSourceRow(modType, (i + start + 1) % 16);
		for (int j = 0; j < MODTARGET_COUNT; j++)
		{
			if (row->targets[j].depth != 0 && row->targets[j].outputid != -1) used = true;
		};
		if (used) return (i + start + 1) % 16;
	}
	return (start + 1)%16;
}

void ModSourceScreen::RepeatGoto()
{
	ActiveInstance = FindNextUsed(ActiveInstance);
	SetEncoderNames();
}

void ModSourceScreen::Action(int a)
{
	switch (a)
	{
	case MenuAction_CloseModal: Modal = NULL; SetEncoderNames(); break;
	case MenuAction_Next:
	{
		ActiveInstance = (ActiveInstance + 1) % MaxInstances;
		SetEncoderNames();
	}break;
	case MenuAction_Prev: 
	{
		ActiveInstance = (ActiveInstance + MaxInstances - 1) % MaxInstances; 
		SetEncoderNames();
	} break;
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
			snprintf(txt,10, "%d", i+1);
			bool used = false;
			
			auto row = gCurrentPreset.GetModSourceRow(modType, i);
			for (int j = 0; j < MODTARGET_COUNT; j++)
			{
				if (row->targets[j].depth != 0 && row->targets[j].outputid !=-1) used = true;
			};

			int x = (i-8) * 40 + 512;
			int y = MButtonHeight(1);
			RenderLettersInABox(x,y, i == ActiveInstance, txt, 35, 35, used);
		}
	}
	RenderModalBox(active, DT);
	//auto row = gCurrentPreset.GetModSourceRow(modType, ActiveInstance);
}

void ModSourceScreen::Activate()
{
	SetEncoderNames();
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
	Modal->Activate();
}

void ModSourceScreen::EncoderPress(FinalEncoderEnum button)
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
