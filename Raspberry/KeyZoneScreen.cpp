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

	EnableButton(RB2, "Close", MenuEntry_Action, MenuAction_CloseParentModal);
	//EnableButton(RB3, "Cancel", MenuEntry_Action, MenuAction_Revert);
	EnableButton(RB3, "Go to zone settings", MenuEntry_Action, MenuAction_1);
	EnableButton(LB3, "1", MenuEntry_FeatureToggle, 0);
	EnableButton(LB4, "2", MenuEntry_FeatureToggle, 1);
	EnableButton(LB5, "3", MenuEntry_FeatureToggle, 2);
	EnableButton(LB6, "4", MenuEntry_FeatureToggle, 3);

	EnableButton(RB5, "Para 1", MenuEntry_FeatureToggle, 4);
	EnableButton(RB6, "Para 2", MenuEntry_FeatureToggle, 5);
	EnableButton(RB7, "Para 3", MenuEntry_FeatureToggle, 6);


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



uint16_t KeyZoneScreen::GetParameterValue(int param, int encoderset)
{
	switch (param)
	{

	}
	return 0;
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
	EnableButton(LB1, "Previous", MenuEntry_Action, MenuAction_Prev);
	EnableButton(RB1, "Next", MenuEntry_Action, MenuAction_Next);

	EnableButton(LB3, "Single Mode", MenuEntry_FeatureToggle, Zone_Single);
	EnableButton(LB4, "Paraphonic Mode", MenuEntry_FeatureToggle, Zone_Para);
	EnableButton(LB5, "Arpeggiator Mode", MenuEntry_FeatureToggle, Zone_Arp);

	

	EnableAvailableEncoder("Lower", MenuEntry_Value, Zone_Lower, -1);
	EnableAvailableEncoder("Upper", MenuEntry_Value, Zone_Upper, -1);
	EnableAvailableEncoder("Transpose", MenuEntry_Value, Zone_Transpose, -1);




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
		RenderLettersInABox(x, y, i == ActiveInstance, txt, 35, 35, used, value, false);
	}

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