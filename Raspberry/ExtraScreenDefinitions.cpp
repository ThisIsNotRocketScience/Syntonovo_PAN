#include <stdlib.h>
#include <stdio.h>
#include "ExtraScreenDefinitions.h" 

#include "PanPreset.h"

extern PanPreset_t gCurrentPreset;

void LetterControl::SketchRightDelta(int delta)
{
	Current = GetDeltaChar(Current, delta);// ((Current + delta - 32 + 96) % 96) + 32;
}

char LetterControl::GetDeltaChar(char base, int delta)
{
	return ((base + delta - 32 + 96) % 96) + 32;
}

void LetterControl::SketchRightPressed()
{
	Current = ((Current + 32 - 64 + 64) % 64) + 64;
}

LetterControl::LetterControl(int _x, int _y, int _id)
{
	x = _x;
	y = _y;
	id = _id;
	Current = gCurrentPreset.Name[id];
	if (!(Current >= ' ' || Current <= '~'))
	{
		Current = ' ';
	}
}

void PresetScreen::Action(int action)
{
	switch (action)
	{
	case MenuAction_No:

		gGui.GotoPage(SCREEN_HOME);
		break;
	case MenuAction_Yes:

		for (int i = 0; i < PRESET_NAME_LENGTH - 1; i++)
		{
			gCurrentPreset.Name[i] = Letters[i]->Current;
		}
		gGui.GotoPage(SCREEN_HOME);
		break;
	case MenuAction_Backspace:
	{
		int N = GetActiveLetter();
		if (N < 0)
		{
			N = Letters.size() - 1;

		}
		SetActiveLetter(N - 1);
		Letters[N]->Current = ' ';

	}
	break;
	case MenuAction_SpaceBar:
	{
		int N = GetActiveLetter();
		if (N < 0)
		{
			N = 0;
		}
		SetActiveLetter(N + 1);
		Letters[N]->Current = ' ';

	}
	break;
	}
}
void PresetScreen::SetActiveLetter(int N)
{
	if (N >= (int)Letters.size()) N = Letters.size() - 1;
	_control_t* c = Letters[N];
	for (int i = 0; i < (int)ControlsInOrder.size(); i++)
	{
		if (ControlsInOrder[i] == c)
		{
			ActiveControl = i;
			return;
		}
	}
}
int PresetScreen::GetActiveLetter()
{
	return LastActiveLetter;
}

#define LetterBoxW 40
#define LetterBoxH 40


void RenderLettersInABox(int x, int y, bool active, const char* text, int w, int h, bool notghosted)
{
	ImVec2 tl(x, y);
	ImVec2 br(x + w, y + h);

	ImGui::GetWindowDrawList()->AddRect(tl, br, active ? gGuiResources.Highlight : Dimmed(notghosted ? 1 : 3, gGuiResources.Normal), 0, 0, 2);

	auto s = ImGui::CalcTextSize(text);

	ImGui::SetCursorPos(ImVec2(x + LetterBoxW / 2 - s.x / 2, y + LetterBoxH / 2 - s.y / 2));
	ImGui::TextColored((ImVec4)(ImColor)(Dimmed(notghosted ? 1 : 3, gGuiResources.Normal)), text);

}

PresetScreen::PresetScreen()
{

	for (int i = 0; i < PRESET_NAME_LENGTH - 1; i++)
	{
		AddLetterControl(1024 / 2 - (LetterBoxW + 5) * ((PRESET_NAME_LENGTH - 1.5) / 2) + i * (LetterBoxW + 5), 300 - LetterBoxH / 2, i);
	}

	
	EnableButton(LB5, "Space", MenuEntry_Action, MenuAction_SpaceBar);
	EnableButton(LB6, "Backspace", MenuEntry_Action, MenuAction_Backspace);

	EnableButton(RB2, "Cancel", MenuEntry_Action, MenuAction_No);

	EnableButton(RB3, "OK", MenuEntry_Action, MenuAction_Yes);
}

void PresetScreen::Activate()
{
	for (int i = 0; i < PRESET_NAME_LENGTH - 1; i++)
	{
		Letters[i]->Current = gCurrentPreset.Name[i];
	}

	SetActiveControl(Letters[0]);
}


void PresetScreen::AddLetterControl(int x, int y, int id)
{
	auto L = new LetterControl(x, y, id);
	L->Parent = this;
	Letters.push_back(L);
	ControlsInOrder.push_back(L);
}


void LetterControl::Render(bool active, float DT)
{

	if (active)
	{
		((PresetScreen*)Parent)->LastActiveLetter = id;
		for (int i = -2; i < 3; i++)
		{
			if (i != 0)
			{
				ImVec2 tl(x, y + i * (LetterBoxH + ParamMasterMargin));
				ImVec2 br(x + LetterBoxW, y + LetterBoxH + i * (LetterBoxH + ParamMasterMargin));
				ImGui::GetWindowDrawList()->AddRect(tl, br, Dimmed(abs(i), active ? gGuiResources.Highlight : gGuiResources.Normal), 0, 0, 2);
				auto s = ImGui::CalcTextSize(&Current, &Current + 1);
				ImGui::SetCursorPos(ImVec2(x + LetterBoxW / 2 - s.x / 2, y + LetterBoxH / 2 - s.y / 2 + i * (LetterBoxH + ParamMasterMargin)));
				ImGui::TextColored(ImVec4(ImColor(Dimmed(abs(i), active ? gGuiResources.Highlight : gGuiResources.Normal))), "%c", GetDeltaChar(Current, i));
			}
		}
	}
	ImVec2 tl(x, y);
	ImVec2 br(x + LetterBoxW, y + LetterBoxH);
	ImGui::GetWindowDrawList()->AddRect(tl, br, active ? gGuiResources.Highlight : gGuiResources.Normal, 0, 0, 2);
	auto s = ImGui::CalcTextSize(&Current, &Current + 1);
	ImGui::SetCursorPos(ImVec2(x + LetterBoxW / 2 - s.x / 2, y + LetterBoxH / 2 - s.y / 2));
	ImGui::Text("%c", Current);
}
void BankSelectScreen::Action(int action)
{

	if (Side == Left) gPanState.BankLeft = action; else gPanState.BankRight = action;
	//LoadSelectedPreset();
	Activate();
}

void BankSelectScreen::SetLeftRight(LeftRight lr)
{

}


BankSelectScreen::BankSelectScreen()
{
	df = 0;
	mybank = &df;
	list = new BankList(400, 150, mybank, "");
	ControlsInOrder.push_back(list);
}
void BankSelectScreen::Activate()
{
	int* CurrentBank = &gPanState.BankLeft;
	list->SetBankPointer(CurrentBank);
	if (Side == Left)
	{
		SetTitle("Select Left Bank");
	}
	else
	{
		CurrentBank = &gPanState.BankRight;
		SetTitle("Select Right Bank");
	}
	EnableButton(LB2, (*CurrentBank == 0) ? "A (current)" : "A", MenuEntry_Action, 0);
	EnableButton(LB3, (*CurrentBank == 1) ? "B (current)" : "B", MenuEntry_Action, 1);
	EnableButton(LB4, (*CurrentBank == 2) ? "C (current)" : "C", MenuEntry_Action, 2);
	EnableButton(LB5, (*CurrentBank == 3) ? "D (current)" : "D", MenuEntry_Action, 3);
	EnableButton(LB6, (*CurrentBank == 4) ? "E (current)" : "E", MenuEntry_Action, 4);
	EnableButton(LB7, (*CurrentBank == 5) ? "F (current)" : "F", MenuEntry_Action, 5);

	EnableButton(RB2, (*CurrentBank == 6) ? "G (current)" : "G", MenuEntry_Action, 6);
	EnableButton(RB3, (*CurrentBank == 7) ? "H (current)" : "H", MenuEntry_Action, 7);
	EnableButton(RB4, (*CurrentBank == 8) ? "I (current)" : "I", MenuEntry_Action, 8);
	EnableButton(RB5, (*CurrentBank == 9) ? "J (current)" : "J", MenuEntry_Action, 9);
	EnableButton(RB6, (*CurrentBank == 10) ? "K (current)" : "K", MenuEntry_Action, 10);
	EnableButton(RB7, (*CurrentBank == 11) ? "L (current)" : "L", MenuEntry_Action, 11);
}

void SavePresetScreen::PatchButton(FinalLedButtonEnum b)
{
	int idx = -1;
	switch (b)
	{
	case ledbutton_B1:idx = 0; break;
	case ledbutton_B2:idx = 1; break;
	case ledbutton_B3:idx = 2; break;
	case ledbutton_B4:idx = 3; break;
	case ledbutton_B5:idx = 4; break;
	case ledbutton_B6:idx = 5; break;
	case ledbutton_B7:idx = 6; break;
	case ledbutton_B8:idx = 7; break;
	case ledbutton_B9:idx = 0; break;
	case ledbutton_B10:idx = 1; break;
	case ledbutton_B11:idx = 2; break;
	case ledbutton_B12:idx = 3; break;
	case ledbutton_B13:idx = 4; break;
	case ledbutton_B14:idx = 5; break;
	case ledbutton_B15:idx = 6; break;
	case ledbutton_B16:idx = 7; break;
	}
	cmd_preset_save(idx + df * 16);
	gGui.GotoPage(SCREEN_HOME);
}

void SavePresetScreen::Action(int action)
{
	switch (action)
	{

	case MenuAction_BankA:df = 0; break;
	case MenuAction_BankB:df = 1; break;
	case MenuAction_BankC:df = 2; break;
	case MenuAction_BankD:df = 3; break;
	case MenuAction_BankE:df = 4; break;
	case MenuAction_BankF:df = 5; break;
	case MenuAction_BankG:df = 6; break;
	case MenuAction_BankH:df = 7; break;
	case MenuAction_BankI:df = 8; break;
	case MenuAction_BankJ:df = 9; break;
	case MenuAction_BankK:df = 10; break;
	case MenuAction_BankL:df = 11; break;

	default:
		_screensetup_t::Action(action);
	}
	SetNames();
}

void SavePresetScreen::SetLeftRight(LeftRight lr)
{

}

SavePresetScreen::SavePresetScreen()
{
	df = 0;
	list = new BankList(400, 150, &df, "");
	ControlsInOrder.push_back(list);
	EnableButton(RB1, "Cancel", MenuEntry_Action, MenuAction_Home);
	SetTitle("Save preset");
	AddText(140, 50, "select bank and press a preset button to save");
}

void SavePresetScreen::Activate()
{
	int* CurrentBank = &gPanState.BankLeft;
	list->SetBankPointer(CurrentBank);
	df = *CurrentBank;
	/*if (Side == Left)
	{
		SetTitle("Select Left Bank");
	}
	else
	{
		CurrentBank = &gPanState.BankRight;
		SetTitle("Select Right Bank");
	}*/
	SetNames();
};

void SavePresetScreen::SetNames()
{
	list->SetBankPointer(&df);
	EnableButton(LB2, (df == 0) ? "A (current)" : "A", MenuEntry_Action, MenuAction_BankA);
	EnableButton(LB3, (df == 1) ? "B (current)" : "B", MenuEntry_Action, MenuAction_BankB);
	EnableButton(LB4, (df == 2) ? "C (current)" : "C", MenuEntry_Action, MenuAction_BankC);
	EnableButton(LB5, (df == 3) ? "D (current)" : "D", MenuEntry_Action, MenuAction_BankD);
	EnableButton(LB6, (df == 4) ? "E (current)" : "E", MenuEntry_Action, MenuAction_BankE);
	EnableButton(LB7, (df == 5) ? "F (current)" : "F", MenuEntry_Action, MenuAction_BankF);
	EnableButton(RB2, (df == 6) ? "G (current)" : "G", MenuEntry_Action, MenuAction_BankG);
	EnableButton(RB3, (df == 7) ? "H (current)" : "H", MenuEntry_Action, MenuAction_BankH);
	EnableButton(RB4, (df == 8) ? "I (current)" : "I", MenuEntry_Action, MenuAction_BankI);
	EnableButton(RB5, (df == 9) ? "J (current)" : "J", MenuEntry_Action, MenuAction_BankJ);
	EnableButton(RB6, (df == 10) ? "K (current)" : "K", MenuEntry_Action, MenuAction_BankK);
	EnableButton(RB7, (df == 11) ? "L (current)" : "L", MenuEntry_Action, MenuAction_BankL);
}


ImageScreen::ImageScreen(ImTextureID img)
{
	theImg = img;
}

void ImageScreen::Render(bool active, float dt)
{
	_screensetup_t::Render(active, dt);
	ImGui::SetCursorPos(ImVec2(0, 0));
	ImGui::Image(theImg, ImVec2(1024, 600));
}

EffectList::EffectList() 
{
}

void EffectList::SketchRight(int delta)
{
	while (delta < 0) {
		delta++; Action(MenuAction_FX_Prev);
	};
	while (delta > 0) {
		delta--; Action(MenuAction_FX_Next);
	};
}

char EffectChipStrings[8][4][24] = 
{

	{ "Chorus & Reverb" ,"Reverb mix","Chorus rate","Chorus mix" },
	{ "Flanger & Reverb","Reverb mix","Flange rate","Flange mix" },
	{ "Tremolo & Reverb","Reverb mix","Tremolo rate","Tremolo mix" },
	{ "Pitch shift","Pitch +/- 4 semitones","","" },
	{ "Pitch & echo","Pitch shift","Echo delay","Echo mix" },
	{ "Test","-","-","-" },
	{ "Reverb 1","Reverb time","HF filter","LF filter" },
	{ "Reverb 2","Reverb time","HF filter","LF filter" }
};

void EffectList::Render(bool active, float dt)
{
	ImGui::PushFont(gGuiResources.MediumFont);

	char txt[200];
	float L = ImGui::GetTextLineHeight();
	snprintf(txt, 200, "Current effect: %s", EffectChipStrings[DecodeCurrentEffect()][0]);

	auto S = ImGui::CalcTextSize(txt);
	ImGui::SetCursorPos(ImVec2(512 - S.x / 2, MButtonHeight(1)));
	ImGui::Text(txt);
	ImGui::PopFont();
	ImGui::PushFont(gGuiResources.SmallFont);

	snprintf(txt, 200, "Parameter 1: %s", EffectChipStrings[DecodeCurrentEffect()][1]);
	ImGui::SetCursorPos(ImVec2(250, 240 + L));
	ImGui::Text(txt);

	snprintf(txt, 200, "Parameter 2: %s", EffectChipStrings[DecodeCurrentEffect()][2]);
	ImGui::SetCursorPos(ImVec2(250, 240 + L * 2));
	ImGui::Text(txt);

	snprintf(txt, 200, "Parameter 3: %s", EffectChipStrings[DecodeCurrentEffect()][3]);
	ImGui::SetCursorPos(ImVec2(250, 240 + L * 3));
	ImGui::Text(txt);

	ImGui::PopFont();

	for (int i = 0; i < 8; i++)
	{
		char txt[10];
		snprintf(txt, 10, "%d", i + 1);
		bool used = false;

		int x = (i - 4) * 40 + 512;
		int y = MButtonHeight(2);
		RenderLettersInABox(x, y, i == DecodeCurrentEffect(), txt, 35, 35, true);
	}
}
