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
		ActiveInstance = id % MaxInstances;
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
	int cur = 0;
	switch (myScreen)
	{
	case SCREEN_KEYZONES:
	case SCREEN_KEYBOARD:
		HasActiveInstanceDisplay = true;
		MaxInstances = 4;
		break;
	case SCREEN_X:
		cur = EnableAvailableEncoder("Deadzone", MenuEntry_EnvelopeValue, KeyboardParam_DeadzoneX,cur);
		cur = SkipAvailableEncoder(cur+1);
		cur = EnableAvailableEncoder("Scale", MenuEntry_EnvelopeValue, KeyboardParam_ScaleX,cur);

		break;
	case SCREEN_Y:
		cur = EnableAvailableEncoder("Deadzone", MenuEntry_EnvelopeValue, KeyboardParam_DeadzoneY,cur);
		cur = SkipAvailableEncoder(cur+1);
		cur = EnableAvailableEncoder("Scale", MenuEntry_EnvelopeValue, KeyboardParam_ScaleY,cur);

		break;
	case SCREEN_Z:
		cur = EnableAvailableEncoder("Deadzone", MenuEntry_EnvelopeValue, KeyboardParam_DeadzoneZ,cur);
		cur = SkipAvailableEncoder(cur+1);
		cur = EnableAvailableEncoder("Scale", MenuEntry_EnvelopeValue, KeyboardParam_ScaleZ,cur);

		break;
	case SCREEN_LMOD:
		cur = EnableAvailableEncoder("Deadzone", MenuEntry_EnvelopeValue, KeyboardParam_DeadzoneLMod,cur);
		cur = SkipAvailableEncoder(cur+1);
		cur = EnableAvailableEncoder("Scale", MenuEntry_EnvelopeValue, KeyboardParam_ScaleLMod,cur);
		break;
	case SCREEN_RMOD:
		cur = EnableAvailableEncoder("Deadzone", MenuEntry_EnvelopeValue, KeyboardParam_DeadzoneRMod,-1);
		cur = SkipAvailableEncoder(cur+1);
		cur = EnableAvailableEncoder("Scale", MenuEntry_EnvelopeValue, KeyboardParam_ScaleRMod,cur);
		break;
	case SCREEN_LSUS:
		cur = EnableAvailableEncoder("Deadzone", MenuEntry_EnvelopeValue, KeyboardParam_DeadzoneLSus,cur);
		cur = SkipAvailableEncoder(cur+1);
		cur = EnableAvailableEncoder("Scale", MenuEntry_EnvelopeValue, KeyboardParam_ScaleLSus,cur);
		break;
	case SCREEN_RSUS:
		cur = EnableAvailableEncoder("Deadzone", MenuEntry_EnvelopeValue, KeyboardParam_DeadzoneRSus,cur);
		cur = SkipAvailableEncoder(cur+1);
		cur = EnableAvailableEncoder("Scale", MenuEntry_EnvelopeValue, KeyboardParam_ScaleRSus,cur);
		break;
	case SCREEN_LUNA:
		cur = EnableAvailableEncoder("Deadzone", MenuEntry_EnvelopeValue, KeyboardParam_DeadzoneLUna,cur);
		cur = SkipAvailableEncoder(cur+1);
		cur = EnableAvailableEncoder("Scale", MenuEntry_EnvelopeValue, KeyboardParam_ScaleLUna,cur);
		break;
	case SCREEN_RUNA:
		cur = EnableAvailableEncoder("Deadzone", MenuEntry_EnvelopeValue, KeyboardParam_DeadzoneRUna,cur);
		cur = SkipAvailableEncoder(cur+1);
		cur = EnableAvailableEncoder("Scale", MenuEntry_EnvelopeValue, KeyboardParam_ScaleRUna,cur);
		break;

	case SCREEN_ENVELOPE:
		HasActiveInstanceDisplay = true;

		EnableButton(LB3, "Legato", MenuEntry_EnvelopeToggle, Envelope_Retrigger);

		cur = EnableAvailableEncoder("Attack", MenuEntry_EnvelopeValue, Envelope_Attack,cur);
		cur = SkipAvailableEncoder(cur+1);
		cur = EnableAvailableEncoder("Decay", MenuEntry_EnvelopeValue, Envelope_Decay,cur);
		cur = SkipAvailableEncoder(cur+1);
		cur = EnableAvailableEncoder("Sustain", MenuEntry_EnvelopeValue, Envelope_Sustain,cur);
		cur = SkipAvailableEncoder(cur+1);
		cur = EnableAvailableEncoder("Release", MenuEntry_EnvelopeValue, Envelope_Release,cur);
//		EnableAvailableEncoder("Curve", MenuEntry_EnvelopeValue, Envelope_Curve);

		break;
	case SCREEN_LFO:
		EnableButton(LB3, "Reset on key", MenuEntry_EnvelopeToggle, LFO_ResetOnKey);
		cur = EnableAvailableEncoder("Speed", MenuEntry_EnvelopeValue, LFO_Speed,cur);
		cur = SkipAvailableEncoder(cur+1);
		cur = EnableAvailableEncoder("Shape", MenuEntry_EnvelopeValue, LFO_Shape, cur);
		cur = SkipAvailableEncoder(cur+1);
		cur = EnableAvailableEncoder("Reset Phase", MenuEntry_EnvelopeValue, LFO_ResetPhase,cur );
		//EnableAvailableEncoder("Depth", MenuEntry_EnvelopeValue, LFO_Depth);
		HasActiveInstanceDisplay = true;
		break;
	}
	if (HasActiveInstanceDisplay)
	{
		EnableButton(LB1, "Previous", MenuEntry_Action, MenuAction_Prev);
		EnableButton(RB1, "Next", MenuEntry_Action, MenuAction_Next);
	}
	EnableButton(RB3, "Parameters", MenuEntry_EncoderSet, 0);
	EnableButton(RB4, "Targets", MenuEntry_EncoderSet, 1);
	
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

	float W = 500;
	float H = 100;

	switch (myScreen)
	{
	case SCREEN_KEYBOARD:
	case SCREEN_KEYZONES:
		break;
	case SCREEN_LFO:
	{
		ImVec2 Pts[100];
		float Speed  = 0.1f  + gCurrentPreset.GetModParameterValue(LFO_Speed, ActiveInstance) / 65535.0f;
		float Ps = 6.283f* (gCurrentPreset.GetModParameterValue(LFO_ResetPhase, ActiveInstance) / 65535.0f);
		x = 512 - W/2;
		for (int i = 0; i < 100; i++)
		{
			Pts[i].x = W * i * 0.01 + x;
			Pts[i].y = -sin(Ps + (float)i * Speed)  * H/2 + y + H/2;
		}

		ImGui::GetWindowDrawList()->AddLine(ImVec2(0, y + H/2), ImVec2(1024, y + H/2), IM_COL32(255, 255, 255, 20), 3);
		ImGui::GetWindowDrawList()->AddPolyline(Pts, 100, gGuiResources.Normal,false, 5);

		break;
	};
	case SCREEN_ENVELOPE:
	{
		float A = gCurrentPreset.GetModParameterValue(Envelope_Attack, ActiveInstance)/65535.0f;
		float D = gCurrentPreset.GetModParameterValue(Envelope_Decay, ActiveInstance) / 65535.0f;
		float S = gCurrentPreset.GetModParameterValue(Envelope_Sustain, ActiveInstance) / 65535.0f;
		float R = gCurrentPreset.GetModParameterValue(Envelope_Release, ActiveInstance) / 65535.0f;


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

		ImGui::GetWindowDrawList()->AddLine(ImVec2(0, y + H), ImVec2(1024, y + H), IM_COL32(255, 255, 255, 20), 3);

		ImGui::GetWindowDrawList()->AddBezierCurve(S1, S2, S3, S4, gGuiResources.Normal, 5);
		ImGui::GetWindowDrawList()->AddCircleFilled (S1, 2.5, gGuiResources.Normal);
		ImGui::GetWindowDrawList()->AddCircleFilled(S4, 2.5, gGuiResources.Normal);
		ImGui::GetWindowDrawList()->AddBezierCurve(S4, S5, S6, S7, gGuiResources.Normal, 5);
		ImGui::GetWindowDrawList()->AddCircleFilled(S7, 2.5, gGuiResources.Normal);
		ImGui::GetWindowDrawList()->AddLine(S7, S8, gGuiResources.Normal, 5);
		ImGui::GetWindowDrawList()->AddCircleFilled(S8, 2.5, gGuiResources.Normal);
		ImGui::GetWindowDrawList()->AddBezierCurve(S8, S9, S10, S11, gGuiResources.Normal, 5);
		ImGui::GetWindowDrawList()->AddCircleFilled(S11, 2.5, gGuiResources.Normal);

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
	SetActiveInstance( FindNextUsed(ActiveInstance));
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

			int x = (i-MaxInstances/2) * 40 + 512;
			int y = 60;
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
