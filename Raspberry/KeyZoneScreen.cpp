#include <math.h>
#include "../libs/imgui-master/imgui.h"
#include <stdio.h>
#include "KeyZoneScreen.h"


int ScreenToKeymap(int screen, int activeinstance)
{
	switch (screen)
	{
	case SCREEN_VCO1: return key_map_target_vco1;
	case SCREEN_VCO2: return key_map_target_vco2;
	case SCREEN_VCO3: return key_map_target_vco3;
	case SCREEN_VCO4: return key_map_target_vco4;
	case SCREEN_VCO5: return key_map_target_vco5;
	case SCREEN_VCO6: return key_map_target_vco6;
	case SCREEN_VCO7: return key_map_target_vco7;
	case SCREEN_VCO8: return key_map_target_vco8;

	case SCREEN_LFO: return key_map_target_lfo1 + activeinstance;

	case SCREEN_ENVELOPE:return key_map_target_env1 + activeinstance;

	case SCREEN_VCF1: return key_map_target_vcf1;
	case SCREEN_VCF2a: return key_map_target_vcf2a;
	case SCREEN_VCF2b: return key_map_target_vcf2b;
	case SCREEN_VCF2c: return key_map_target_vcf2c;
	case SCREEN_VCF2d: return key_map_target_vcf2d;



	}

	return -1;
}

bool KeyZoneSelectorScreen::GetToggle(int id)
{
	if (currentmapping == NULL) return false;
	switch (id)
	{
	case 0:return (currentmapping->keyzone == 0);
	case 1:return (currentmapping->keyzone == 1);
	case 2:return (currentmapping->keyzone == 2);
	case 3:return (currentmapping->keyzone == 3);

	case 4:return (currentmapping->keyindex == 0);
	case 5:return (currentmapping->keyindex == 1);
	case 6:return (currentmapping->keyindex == 2);

	}
	return false;
}

void KeyZoneSelectorScreen::DoToggle(int id)
{
	if (currentmapping == NULL) return;
	switch (id)
	{
	case 0:if (currentmapping->keyzone != 0)SetZone(0); break;
	case 1:if (currentmapping->keyzone != 1)SetZone(1); break;
	case 2:if (currentmapping->keyzone != 2)SetZone(2); break;
	case 3:if (currentmapping->keyzone != 3)SetZone(3); break;

	case 4:if (currentmapping->keyindex != 0) currentmapping->keyindex = 0; break;
	case 5:if (currentmapping->keyindex != 1) currentmapping->keyindex = 1; break;
	case 6:if (currentmapping->keyindex != 2) currentmapping->keyindex = 2; break;
	}
}


uint16_t KeyZoneSelectorScreen::GetParameterValue(int param, int encoderset)
{
	switch (param)
	{

	}
	return 0;
};

void KeyZoneSelectorScreen::TweakParameterValue(int param, int delta)
{
};


KeyZoneSelectorScreen::KeyZoneSelectorScreen(): _screensetup_t(SCREEN_KEYZONESELECTOR)
{
	MaxInstances = __KeyZone_Count;

	SetTitle("Keyzone");

	int cur = 0;

	EnableButton(RB1, "Close", MenuEntry_Action, MenuAction_CloseParentModal);
	//EnableButton(RB3, "Cancel", MenuEntry_Action, MenuAction_Revert);
	EnableButton(RB3, "Go to zone settings", MenuEntry_Action, MenuAction_1);
	EnableButton(LB3, "1", MenuEntry_FeatureToggle, 0);
	EnableButton(LB4, "2", MenuEntry_FeatureToggle, 1);
	EnableButton(LB5, "3", MenuEntry_FeatureToggle, 2);
	EnableButton(LB6, "4", MenuEntry_FeatureToggle, 3);

	EnableButton(RB5, "Paraphonic index 1", MenuEntry_FeatureToggle, 4);
	EnableButton(RB6, "Paraphonic index 2", MenuEntry_FeatureToggle, 5);
	EnableButton(RB7, "Paraphonic index 3", MenuEntry_FeatureToggle, 6);


	for (int i = 0; i < 11; i++)
	{
		encoders[1][i].enabled = false;
		encoders[1][i].target = i;
		encoders[1][i].Set = 1;
		encoders[1][i].style = MenuEntry_ModMatrixValue;
		//		encoders[1][i].SetTitle(GetModulationTargetName(row->targets[i].outputid));		
	}
	
}

void KeyZoneSelectorScreen::RenderContent(bool active, float DT)
{
	_screensetup_t::RenderContent(active, DT);
	
}

void KeyZoneSelectorScreen::Deactivate()
{
	Modal = NULL;
}

void KeyZoneSelectorScreen::SetZone(int Z)
{
	if (currentmapping == NULL) return;
	currentmapping->keyzone = Z % __KeyZone_Count;
}

void KeyZoneSelectorScreen::Action(int a)
{
	switch (a)
	{
		case MenuAction_1:if (currentmapping)gGui.GotoPageForKeyZone(currentmapping->keyzone); break;
		case MenuAction_CloseParentModal: Modal = NULL; CloseParentModal();  break;
		case MenuAction_Revert: SetZone(RevertZone); CloseParentModal(); Modal = NULL; break;
	}
}

void KeyZoneSelectorScreen::Render(bool active, float DT)
{
	RenderContent(active, DT);
	RenderModalBox(active, DT);
	//auto row = gCurrentPreset.GetModSourceRow(modType, ActiveInstance);
}

void KeyZoneSelectorScreen::Activate()
{
	
	_control_t::Activate();
}

void KeyZoneSelectorScreen::EncoderPress(FinalEncoderEnum button)
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

void KeyZoneSelectorScreen::SetTarget(int screen, int instance)
{
	currentmapping = NULL;
	RevertZone = 0;
	int Map = ScreenToKeymap(screen, instance);
	if (Map > -1)
	{
		currentmapping = &gCurrentPreset.key_mapping[Map];
		RevertZone = currentmapping->keyzone;
	}
}

KeyZoneSelectorScreen KeyZoneModal;


unsigned short ParameterValueRange(int lower, int upper, int value)
{

	int R = (value - lower) * 0xffff;
	R /= upper;
	return R;
};

enum
{
	Zone_Single,
	Zone_Arp,
	Zone_Para,

	Zone_Lower,
	Zone_Upper,
	Zone_Transpose
};


uint16_t KeyZoneScreen::GetParameterValue(int param, int encoderset)
{
	switch (param)
	{
	case Zone_Lower: return ParameterValueRange(0, 127, gCurrentPreset.key_input[ActiveInstance].rangelo);
	case Zone_Upper:return ParameterValueRange(0, 127, gCurrentPreset.key_input[ActiveInstance].rangelo);
	case Zone_Transpose:return ParameterValueRange(-24, 24, gCurrentPreset.key_input[ActiveInstance].rangelo);
	}
	return 0;
};




bool KeyZoneScreen::GetToggle(int id)
{
	switch (id)
	{
	case Zone_Single:  return gCurrentPreset.keyzone[ActiveInstance].type == PresetKeyzoneType_Single;
	case Zone_Arp:  return gCurrentPreset.keyzone[ActiveInstance].type == PresetKeyzoneType_Arpeggiator;
	case Zone_Para:  return gCurrentPreset.keyzone[ActiveInstance].type == PresetKeyzoneType_Paraphonic;

	}
	return false;
}

void KeyZoneScreen::DoToggle(int id)
{
	switch (id)
	{
	case Zone_Single:
		if (GetToggle(Zone_Single) == false)
		{
			gCurrentPreset.keyzone[ActiveInstance].type = PresetKeyzoneType_Single;
		}
		break;
	case Zone_Para:
		if (GetToggle(Zone_Para) == false)
		{
			gCurrentPreset.keyzone[ActiveInstance].type = PresetKeyzoneType_Paraphonic;
		}
		break;
	case Zone_Arp:
		if (GetToggle(Zone_Arp) == false)gCurrentPreset.keyzone[ActiveInstance].type = PresetKeyzoneType_Arpeggiator;
		break;
	}

}


void KeyZoneScreen::TweakParameterValue(int param, int delta)
{
	switch (param)
	{
	case Zone_Lower: 
		gCurrentPreset.key_input[ActiveInstance].rangelo= __min(127, __max(0, (gCurrentPreset.key_input[ActiveInstance].rangelo  + (delta > 0 ? 1 : -1))));
		gCurrentPreset.key_input[ActiveInstance + 4].rangelo = gCurrentPreset.key_input[ActiveInstance].rangelo;
		gCurrentPreset.key_input[ActiveInstance + 8].rangelo = gCurrentPreset.key_input[ActiveInstance].rangelo;

		if (gCurrentPreset.key_input[ActiveInstance].rangelo > gCurrentPreset.key_input[ActiveInstance].rangehi)
		{
			gCurrentPreset.key_input[ActiveInstance].rangehi = gCurrentPreset.key_input[ActiveInstance].rangelo ;
			gCurrentPreset.key_input[ActiveInstance + 4].rangehi = gCurrentPreset.key_input[ActiveInstance].rangehi;
			gCurrentPreset.key_input[ActiveInstance + 8].rangehi = gCurrentPreset.key_input[ActiveInstance].rangehi;

		}
		break;
	case Zone_Upper:
		gCurrentPreset.key_input[ActiveInstance].rangehi = __min(127, __max(0, (gCurrentPreset.key_input[ActiveInstance].rangehi + (delta > 0 ? 1 : -1))));
		gCurrentPreset.key_input[ActiveInstance + 4].rangehi = gCurrentPreset.key_input[ActiveInstance].rangehi;
		gCurrentPreset.key_input[ActiveInstance + 8].rangehi = gCurrentPreset.key_input[ActiveInstance].rangehi;

		if (gCurrentPreset.key_input[ActiveInstance].rangelo > gCurrentPreset.key_input[ActiveInstance].rangehi)
		{
			gCurrentPreset.key_input[ActiveInstance].rangelo = gCurrentPreset.key_input[ActiveInstance].rangehi;
			gCurrentPreset.key_input[ActiveInstance + 4].rangelo = gCurrentPreset.key_input[ActiveInstance].rangelo;
			gCurrentPreset.key_input[ActiveInstance + 8].rangelo = gCurrentPreset.key_input[ActiveInstance].rangelo;
		}

		break;
	case Zone_Transpose:
		gCurrentPreset.key_input[ActiveInstance].transpose = __min(127, __max(0, (gCurrentPreset.key_input[ActiveInstance].transpose + (delta > 0 ? 1 : -1))));
		gCurrentPreset.key_input[ActiveInstance + 4].transpose = gCurrentPreset.key_input[ActiveInstance].transpose;
		gCurrentPreset.key_input[ActiveInstance + 8].transpose= gCurrentPreset.key_input[ActiveInstance].transpose;
		break;
	}
};

void KeyZoneScreen::SetActiveInstance(int id)
{
	ActiveInstance = id % MaxInstances;

	char newtitle[200];
	snprintf(newtitle, 200, "Zone %d", ActiveInstance + 1);
	SetTitle(newtitle);

	snprintf(newtitle, 200, "Goto arpeggiator %d", ActiveInstance + 1);
	EnableButton(RB6, newtitle, MenuEntry_Action, MenuAction_1);
}

KeyZoneScreen::KeyZoneScreen() : _screensetup_t(SCREEN_KEYZONES)
{
	int off = (uint8_t*)&gCurrentPreset.keyzone - (uint8_t*)&gCurrentPreset;
	int off2 = (uint8_t*)&gCurrentPreset.keyzone[0].arpsettings - (uint8_t*)&gCurrentPreset;
	int off3 = (uint8_t*)&gCurrentPreset.keyzone[0].arpsettings.Rhythm - (uint8_t*)&gCurrentPreset;
	//int off3 = (uint8_t*)&gCurrentPreset.key_mapping - (uint8_t*)&gCurrentPreset;
	printf("keyzone offs 0x%x (%d)\n", off, off);
	printf("arpsettings offs 0x%x (%d)\n", off2, off2);
	printf("arpsetting rhythm offs 0x%x (%d)\n", off3, off3);
	

	MaxInstances = __KeyZone_Count;

	int cur = 0;
	//EnableButton(LB1, "Previous", MenuEntry_Action, MenuAction_Prev);
	//EnableButton(RB1, "Next", MenuEntry_Action, MenuAction_Next);

	
	EnableButton(LB1, "1", MenuEntry_Action, MenuAction_5);
	EnableButton(LB3, "2", MenuEntry_Action, MenuAction_6);
	EnableButton(LB5, "3", MenuEntry_Action, MenuAction_7);
	EnableButton(LB7, "4", MenuEntry_Action, MenuAction_8);

	EnableButton(RB3, "Single Mode", MenuEntry_FeatureToggle, Zone_Single);
	EnableButton(RB4, "Paraphonic Mode", MenuEntry_FeatureToggle, Zone_Para);
	EnableButton(RB5, "Arpeggiator Mode", MenuEntry_FeatureToggle, Zone_Arp);

	
	
	int idx = EnableAvailableEncoder("Lower", MenuEntry_Value, Zone_Lower, -1);
	idx = SkipAvailableEncoder(idx);
	idx = SkipAvailableEncoder(idx);
	idx = SkipAvailableEncoder(idx);
	idx = SkipAvailableEncoder(idx);
	idx = SkipAvailableEncoder(idx);
	idx = EnableAvailableEncoder("Transpose", MenuEntry_Value, Zone_Transpose, idx);
	idx = SkipAvailableEncoder(idx);
	idx = SkipAvailableEncoder(idx);
	idx = SkipAvailableEncoder(idx);
	idx = SkipAvailableEncoder(idx);
	idx = SkipAvailableEncoder(idx);
	idx = EnableAvailableEncoder("Upper", MenuEntry_Value, Zone_Upper, idx);




	SetActiveInstance(0);
}

void KeyZoneScreen::RenderContent(bool active, float DT)
{
	_screensetup_t::RenderContent(active, DT);

}

void KeyZoneScreen::Deactivate()
{
	Modal = NULL;
}


void KeyZoneScreen::RepeatGoto()
{
	SetActiveInstance((ActiveInstance + 1) % MaxInstances);
}

void KeyZoneScreen::Action(int a)
{
	switch (a)
	{
	case MenuAction_5: SetActiveInstance(0); break;
	case MenuAction_6: SetActiveInstance(1); break;
	case MenuAction_7: SetActiveInstance(2); break;
	case MenuAction_8: SetActiveInstance(3); break;
	case MenuAction_1:
		gGui.GotoPageForArpeggiator(ActiveInstance);
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

#define KEYWIDTH 10
#define KEYHEIGHT 50
#define BLACKKEYHEIGHT 30
#define RANGEHEIGHT 15

#define OCTAVEWIDTH KEYWIDTH * 7
/*

| | ||| | |
| |_|||_| |
|  |   |  |
-----------

*/
void RenderOctave(int x, int y)
{
	ImVec2 wp = ImGui::GetWindowPos();
	x += wp.x;
	y += wp.y;
	for (int i = 0; i < 7; i++)
	{
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x + i * KEYWIDTH, y), ImVec2(x + i * KEYWIDTH + (KEYWIDTH-1), y + KEYHEIGHT), gGuiResources.WhiteKey);
	}
	int black[6] = { 1,1,0,1,1,1 };
	for (int i = 0; i < 6; i++)
	{
		if (black[i])
		{
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x + i * KEYWIDTH + KEYWIDTH / 2, y), ImVec2(x + i * KEYWIDTH + (KEYWIDTH - 1) + KEYWIDTH / 2, y + BLACKKEYHEIGHT + 2), gGuiResources.BGColor);
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x + i * KEYWIDTH + KEYWIDTH / 2 + 1, y), ImVec2(x + i * KEYWIDTH + (KEYWIDTH - 1) + KEYWIDTH / 2 - 1, y + BLACKKEYHEIGHT ), gGuiResources.BlackKey);
		}
	}
}

void RenderLastOctave(int x, int y)
{
	ImVec2 wp = ImGui::GetWindowPos();
	x += wp.x;
	y += wp.y;

	for (int i = 0; i < 5; i++)
	{
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x + i * KEYWIDTH, y), ImVec2(x + i * KEYWIDTH + (KEYWIDTH - 1), y + KEYHEIGHT), gGuiResources.WhiteKey);
	}
	int black[6] = { 1,1,0,1,1,1 };
	for (int i = 0; i < 4; i++)
	{
		if (black[i])
		{
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x + i * KEYWIDTH + KEYWIDTH / 2, y), ImVec2(x + i * KEYWIDTH + (KEYWIDTH - 1) + KEYWIDTH / 2, y + BLACKKEYHEIGHT + 2), gGuiResources.BGColor);
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x + i * KEYWIDTH + KEYWIDTH / 2 + 1, y), ImVec2(x + i * KEYWIDTH + (KEYWIDTH - 1) + KEYWIDTH / 2 - 1, y + BLACKKEYHEIGHT), gGuiResources.BlackKey);
		}
	}
}

int KeyX(int inp)
{
	int Res =  512 - OCTAVEWIDTH * 5;

	int offs[12] =
	{
		KEYWIDTH * 0,
		KEYWIDTH * 0 + KEYWIDTH / 2,
		KEYWIDTH * 1,
		KEYWIDTH * 1 + KEYWIDTH / 2,
		KEYWIDTH * 2,
		KEYWIDTH * 3,
		KEYWIDTH * 3 + KEYWIDTH / 2,
		KEYWIDTH * 4,
		KEYWIDTH * 4 + KEYWIDTH / 2,
		KEYWIDTH * 5,
		KEYWIDTH * 5 + KEYWIDTH / 2,
		KEYWIDTH * 6
	};

		
		int oct = inp / 12;
	Res += OCTAVEWIDTH * oct;
	Res += offs[inp - oct * 12];
	return Res;
}

void DrawKeyRange(int Lower, int Upper, int Transpose, int col, int y, bool active, int idx)
{
	int lx = KeyX(Lower);
	int ux = KeyX(Upper);

	int lxt = KeyX(Lower + Transpose);
	int uxt = KeyX(Upper + Transpose);

	ImGui::SetCursorPos(ImVec2(KeyX(0) - 20, y));
	char txt[15];
	sprintf(txt, "zone %d", idx);
	//ImGui::Text(txt);

	ImGui::SetCursorPos(ImVec2(ux + KEYWIDTH + ParamMasterMargin, y));
	ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(col), txt);

	ImVec2 wp = ImGui::GetWindowPos();
	lx += wp.x;
	ux += wp.x;
	y += wp.y;
	if (active)
	{
		
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(lx, y), ImVec2(ux + KEYWIDTH, y+ RANGEHEIGHT - 1), col);
	}
	else
	{
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(lx, y), ImVec2(ux + KEYWIDTH, y + RANGEHEIGHT - 1), Dimmed(3, col));

	}

}

void KeyZoneScreen::Render(bool active, float DT)
{
	RenderContent(active, DT);

	for (int i = 0; i < MaxInstances; i++)
	{
		char txt[10];
		snprintf(txt, 10, "%d", i + 1);
		bool used = false;


		for (int j = 0; j < NUM_KEY_MAP_TARGETS; j++)
		{
			if (gCurrentPreset.key_mapping[j].keyzone == i) used = true;
		};

		int x = (i - MaxInstances / 2) * 40 + 512;
		int y = 60;

		int value = 0;
		RenderLettersInABox(x, y, i == ActiveInstance, txt, 35, 35, used, value, false,i, MaxInstances);
	}


	for (int i = 0; i < 4; i++)
	{
		int Lower = gCurrentPreset.key_input[i].rangelo;
		int Upper = gCurrentPreset.key_input[i].rangehi;
		int Transpose = gCurrentPreset.key_input[i].transpose;

		bool active = i == ActiveInstance;
		DrawKeyRange(Lower, Upper, Transpose, gGui.GetNumberColor(i, 4) , 400 -((RANGEHEIGHT + ParamMasterMargin) * NUM_KEYZONES) - 4 + i * (RANGEHEIGHT+ParamMasterMargin), active, i+1);
	}

	int oct = gCurrentPreset.GetOctave();
	oct += 2;
	int basenote =   oct * 12;
	int KeyboardL = KeyX(basenote);
	int KeyboardU = KeyX(basenote+37) + KEYWIDTH;



	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(KeyboardL, 450+ ParamMasterMargin), ImVec2(KeyboardU, 450+RANGEHEIGHT + ParamMasterMargin), IM_COL32(200,200,200,255));

	ImGui::SetCursorPos(ImVec2(KeyboardL, 450 + RANGEHEIGHT + ParamMasterMargin * 2));
	ImGui::Text("Keyboard");


	//ImGui::GetWindowDrawList()->AddText()

	for (int i = 0; i < 10; i++)
	{
		RenderOctave(KeyX(i*12), 400);
	}
	RenderLastOctave(KeyX(10 * 12), 400);

	RenderModalBox(active, DT);
	//auto row = gCurrentPreset.GetModSourceRow(modType, ActiveInstance);
}

void KeyZoneScreen::Activate()
{
	_control_t::Activate();
}

void KeyZoneScreen::EncoderPress(FinalEncoderEnum button)
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