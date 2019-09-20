#include <stdio.h>
#include "gui.h"
#include "PanPreset.h"
#include "ParameterModal.h"


extern int DecodeCurrentEffect();
extern void SetEffect(int effect);



_screensetup_t::_screensetup_t(_screensetup_t *parent)
{
	BG = NULL;
	currentencoderset = 0;
	encodersets = 1;
	Parent = parent;
	Modal = NULL;
	SetTitle("");
	for (int i = 0; i < 14; i++)
	{
		EnableButton(i, "", false, ledmode_off);
		DisableButton(i);
		buttons[i].Parent = this;
		buttons[i].SetupPosition(i);
	}

	for (int j = 0; j < MAXENCODERSETS; j++)
	{
		for (int i = 0; i < 11; i++)
		{
			encoders[j][i].SetTitle("");
			encoders[j][i].Set = j;
			encoders[j][i].Parent = this;
			encoders[j][i].SetupPosition(i);
			DisableEncoder(i);
		}
	}

	ControlsInOrder.push_back(&buttons[0]);
	ControlsInOrder.push_back(&buttons[1]);
	ControlsInOrder.push_back(&buttons[2]);
	ControlsInOrder.push_back(&buttons[3]);
	ControlsInOrder.push_back(&buttons[4]);
	ControlsInOrder.push_back(&buttons[5]);
	ControlsInOrder.push_back(&buttons[6]);

	for (int i = 0; i < MAXENCODERSETS; i++)
	{
		for (int j = 0; j < 11; j++)
		{
			ControlsInOrder.push_back(&encoders[i][j]);
		}
	}

	ControlsInOrder.push_back(&buttons[13]);
	ControlsInOrder.push_back(&buttons[12]);
	ControlsInOrder.push_back(&buttons[11]);
	ControlsInOrder.push_back(&buttons[10]);
	ControlsInOrder.push_back(&buttons[9]);
	ControlsInOrder.push_back(&buttons[8]);
	ControlsInOrder.push_back(&buttons[7]);


	SetFirstEnabledControlActive();
}



uint16_t _screensetup_t::GetParameterValue(int param, int encoderset)
{
	return gCurrentPreset.paramvalue[param];
}

void _screensetup_t::TweakParameterValue(int param, int delta)
{
	gCurrentPreset.TweakParameter((OutputEnum)param, delta);
}

void _screensetup_t::SetupEncodersAndButtonsLeds()
{
	for (int i = 0; i < 14; i++)
	{
		buttons[i].UpdateLed(ControlsInOrder[ActiveControl] == &buttons[i]);
	}

	for (int i = 0; i < 11; i++)
	{
		encoders[currentencoderset][i].UpdateLed(ControlsInOrder[ActiveControl] == &encoders[currentencoderset][i]);
	}

}

void _screensetup_t::SetupMainUILeds()
{

	gPanState.SetEncoderLed(encoder_F1, encoders[currentencoderset][0].ledmode, encoders[currentencoderset][0].r, encoders[currentencoderset][0].g, encoders[currentencoderset][0].b);
	gPanState.SetEncoderLed(encoder_F2, encoders[currentencoderset][1].ledmode, encoders[currentencoderset][1].r, encoders[currentencoderset][1].g, encoders[currentencoderset][1].b);
	gPanState.SetEncoderLed(encoder_F3, encoders[currentencoderset][2].ledmode, encoders[currentencoderset][2].r, encoders[currentencoderset][2].g, encoders[currentencoderset][2].b);
	gPanState.SetEncoderLed(encoder_F4, encoders[currentencoderset][3].ledmode, encoders[currentencoderset][3].r, encoders[currentencoderset][3].g, encoders[currentencoderset][3].b);
	gPanState.SetEncoderLed(encoder_F5, encoders[currentencoderset][4].ledmode, encoders[currentencoderset][4].r, encoders[currentencoderset][4].g, encoders[currentencoderset][4].b);
	gPanState.SetEncoderLed(encoder_F6, encoders[currentencoderset][5].ledmode, encoders[currentencoderset][5].r, encoders[currentencoderset][5].g, encoders[currentencoderset][5].b);
	gPanState.SetEncoderLed(encoder_F7, encoders[currentencoderset][6].ledmode, encoders[currentencoderset][6].r, encoders[currentencoderset][6].g, encoders[currentencoderset][6].b);
	gPanState.SetEncoderLed(encoder_F8, encoders[currentencoderset][7].ledmode, encoders[currentencoderset][7].r, encoders[currentencoderset][7].g, encoders[currentencoderset][7].b);
	gPanState.SetEncoderLed(encoder_F9, encoders[currentencoderset][8].ledmode, encoders[currentencoderset][8].r, encoders[currentencoderset][8].g, encoders[currentencoderset][8].b);
	gPanState.SetEncoderLed(encoder_F10, encoders[currentencoderset][9].ledmode, encoders[currentencoderset][9].r, encoders[currentencoderset][9].g, encoders[currentencoderset][9].b);
	gPanState.SetEncoderLed(encoder_F11, encoders[currentencoderset][10].ledmode, encoders[currentencoderset][10].r, encoders[currentencoderset][10].g, encoders[currentencoderset][10].b);

	gPanState.SetButtonLed(ledbutton_L1, buttons[0].ledmode);
	gPanState.SetButtonLed(ledbutton_L2, buttons[1].ledmode);
	gPanState.SetButtonLed(ledbutton_L3, buttons[2].ledmode);
	gPanState.SetButtonLed(ledbutton_L4, buttons[3].ledmode);
	gPanState.SetButtonLed(ledbutton_L5, buttons[4].ledmode);
	gPanState.SetButtonLed(ledbutton_L6, buttons[5].ledmode);
	gPanState.SetButtonLed(ledbutton_L7, buttons[6].ledmode);
	gPanState.SetButtonLed(ledbutton_R1, buttons[7].ledmode);
	gPanState.SetButtonLed(ledbutton_R2, buttons[8].ledmode);
	gPanState.SetButtonLed(ledbutton_R3, buttons[9].ledmode);
	gPanState.SetButtonLed(ledbutton_R4, buttons[10].ledmode);
	gPanState.SetButtonLed(ledbutton_R5, buttons[11].ledmode);
	gPanState.SetButtonLed(ledbutton_R6, buttons[12].ledmode);
	gPanState.SetButtonLed(ledbutton_R7, buttons[13].ledmode);


	gPanState.SetButtonLed(ledbutton_B1, gPanState.CurrentPatch == 0 ? ledmode_solid : ledmode_off);
	gPanState.SetButtonLed(ledbutton_B2, gPanState.CurrentPatch == 1 ? ledmode_solid : ledmode_off);
	gPanState.SetButtonLed(ledbutton_B3, gPanState.CurrentPatch == 2 ? ledmode_solid : ledmode_off);
	gPanState.SetButtonLed(ledbutton_B4, gPanState.CurrentPatch == 3 ? ledmode_solid : ledmode_off);
	gPanState.SetButtonLed(ledbutton_B5, gPanState.CurrentPatch == 4 ? ledmode_solid : ledmode_off);
	gPanState.SetButtonLed(ledbutton_B6, gPanState.CurrentPatch == 5 ? ledmode_solid : ledmode_off);
	gPanState.SetButtonLed(ledbutton_B7, gPanState.CurrentPatch == 6 ? ledmode_solid : ledmode_off);
	gPanState.SetButtonLed(ledbutton_B8, gPanState.CurrentPatch == 7 ? ledmode_solid : ledmode_off);
	gPanState.SetButtonLed(ledbutton_B9, gPanState.CurrentPatch == 8 ? ledmode_solid : ledmode_off);
	gPanState.SetButtonLed(ledbutton_B10, gPanState.CurrentPatch == 9 ? ledmode_solid : ledmode_off);
	gPanState.SetButtonLed(ledbutton_B11, gPanState.CurrentPatch == 10 ? ledmode_solid : ledmode_off);
	gPanState.SetButtonLed(ledbutton_B12, gPanState.CurrentPatch == 11 ? ledmode_solid : ledmode_off);
	gPanState.SetButtonLed(ledbutton_B13, gPanState.CurrentPatch == 12 ? ledmode_solid : ledmode_off);
	gPanState.SetButtonLed(ledbutton_B14, gPanState.CurrentPatch == 13 ? ledmode_solid : ledmode_off);
	gPanState.SetButtonLed(ledbutton_B15, gPanState.CurrentPatch == 14 ? ledmode_solid : ledmode_off);
	gPanState.SetButtonLed(ledbutton_B16, gPanState.CurrentPatch == 15 ? ledmode_solid : ledmode_off);



	for (int i = 0; i < (int)LedButtonsThatOpenThisScreen.size(); i++)
	{
		gPanState.SetButtonLed((FinalLedButtonEnum)LedButtonsThatOpenThisScreen[i], ledmode_solid);
	}
	for (int i = 0; i < __FINALENCODER_COUNT; i++)
	{
		FinalEncoderEnum enc = (FinalEncoderEnum)i;
		if (IsCenterEncoder(enc) == false)
		{
			uint16_t V, r, g, b;
			V = 0;
			int idx = GetAssociatedParameter(enc);
			bool active = false;
			for (int k = 0; k < (int)EncodersThatOpenThisScreen.size(); k++) if (EncodersThatOpenThisScreen[k] == i) active = true;

			if (idx > -1)
			{
				V = gCurrentPreset.paramvalue[idx];
				LedLerp(active, V, &r, &g, &b);
				gPanState.SetEncoderLed(enc, ledmode_solid, r, g, b);
			}
			else
			{
				if (active)
				{
					gPanState.SetEncoderLed(enc, ledmode_solid, gPanState.active_led_r, gPanState.active_led_g, gPanState.active_led_r);
				}
				else
				{
					gPanState.SetEncoderLed(enc, ledmode_off, 0, 0, 0);

				}
			}



		}
	}

	for (int i = 0; i < (int)EncodersThatOpenThisScreen.size(); i++)
	{
		gPanState.SetEncoderLed((FinalEncoderEnum)EncodersThatOpenThisScreen[i], ledmode_solid, gPanState.active_led_r, gPanState.active_led_g, gPanState.active_led_r);
	}
	//	gPanState.SetEncoderLed()

}

void _screensetup_t::SetupKeyboardStateLeds()
{
	int oct = ((int)gCurrentPreset.paramvalue[Output_MASTER_PITCH2] - 0x8000) / (0x100 * 12);

	gPanState.SetButtonLed(ledbutton_OctDownLeft, oct < 0 ? ledmode_solid : ledmode_off);
	gPanState.SetButtonLed(ledbutton_OctDownRight, oct < 0 ? ledmode_solid : ledmode_off);
	gPanState.SetButtonLed(ledbutton_OctUpLeft, oct > 0 ? ledmode_solid : ledmode_off);
	gPanState.SetButtonLed(ledbutton_OctUpRight, oct > 0 ? ledmode_solid : ledmode_off);

}

void _screensetup_t::SetupLeds()
{
	if (Modal) 
	{
		Modal->SetupLeds();
		return;
	}
	SetupMainUILeds();
	SetupKeyboardStateLeds();
	SetupEncodersAndButtonsLeds();
	
}
extern void cmd_pad_zero();
extern void cmd_calibrate();

void _screensetup_t::Action(int action)
{
	switch (action)
	{
	case MenuAction_PrevVCF2: gGui.PrevVCF2(); break;
	case MenuAction_NextVCF2: gGui.NextVCF2(); break;
	case MenuAction_PrevVCO: gGui.PrevVCO(); break;
	case MenuAction_NextVCO: gGui.NextVCO(); break;

	case MenuAction_OpenVCF2Structure:
	{
		_screensetup_t * eff = gGui.Screens[SCREEN_VCF2_structure];;
		if (eff->Parent) ((_screensetup_t*)eff->Parent)->Modal = NULL;		
		Modal = eff;		
		Modal->Parent = this;
	}
	break;
	case MenuAction_OpenEffects: 
	{	
		_screensetup_t * eff = gGui.Screens[SCREEN_EFFECTS];;
		if (eff->Parent) ((_screensetup_t*)eff->Parent)->Modal = NULL;
		Modal = eff;
		Modal->Parent = this; break;
	}
	case MenuAction_Home: gGui.GotoPage(SCREEN_HOME); break;
	case MenuAction_CloseParentModal: if (Parent) ((_screensetup_t*)Parent)->Modal = NULL; break;	
	case MenuAction_CalibratePads: cmd_pad_zero(); break;
	case MenuAction_CalibrateOscillators: cmd_calibrate(); break;
	case MenuAction_CloseModal: Modal = NULL; break;
	case MenuAction_EnableReferenceLines: gGuiResources.referencelines = !gGuiResources.referencelines; break;
	case MenuAction_EnableTestImage: gGuiResources.testimage = !gGuiResources.testimage; break;

	case MenuAction_FX_Next:
	{
		int ef = (DecodeCurrentEffect());
		int newef = (ef + 1) % 8;
//		printf("effect %d -> %d -> ", ef, newef);
		SetEffect(newef);
		ef = (DecodeCurrentEffect());
	//	printf("%d\n", ef);
	}break;

	case MenuAction_FX_Prev: 
	{
		int ef = (DecodeCurrentEffect());
		int newef = (ef + 7) % 8;
//		printf("effect %d -> %d -> ", ef, newef);
		SetEffect(newef);
		ef = (DecodeCurrentEffect());
	//	printf("%d\n", ef);
	}break;

	
	}
};

void _screensetup_t::SetFirstEnabledControlActive()
{
	for (int i = 0; i < (int)ControlsInOrder.size(); i++)
	{
		if (ControlsInOrder[i]->enabled)
		{
			ActiveControl = i;
			return;
		}
	}
	ActiveControl = 0;
}

void _screensetup_t::DisableEncoder(int i)
{
	encoders[currentencoderset][i].enabled = 0;
}

void _screensetup_t::DisableButton(int i)
{
	buttons[i].enabled = false;
}

bool _screensetup_t::EnableButton(int i, const char *text, int style, int target, bool active, ledmodes l)
{
	buttons[i].SetTitle(text);
	buttons[i].style = style;
	buttons[i].target = target;

	buttons[i].enabled = active;
	buttons[i].ledmode = l;
	return false;
}

int _screensetup_t::EnableAvailableEncoder(char *text, int style, int target, bool isdirectoutput)
{
	for (int i = 0; i < 11; i++)
	{
		if (encoders[currentencoderset][i].enabled == false)
		{
			encoders[currentencoderset][i].SetTitle(text);
			encoders[currentencoderset][i].enabled = true;
			encoders[currentencoderset][i].style = style;
			encoders[currentencoderset][i].IsDirectOutput = isdirectoutput;

			encoders[currentencoderset][i].target = target;
			encoders[currentencoderset][i].ledmode = ledmode_solid;
			return i;
		}
	}

	return -1;
}

int _screensetup_t::EnableAvailableButton(char *text, int style, int target)
{
	for (int i = 0; i < 14; i++)
	{
		if (buttons[i].enabled == false)
		{
			buttons[i].SetTitle(text);
			buttons[i].enabled = true;
			buttons[i].style = style;
			buttons[i].target = target;
			buttons[i].ledmode = ledmode_solid;
			return i;
		}
	}

	return -1;
}

void _screensetup_t::ButtonStyle(int i, int style, int target)
{

}

void _textcontrol_t::Render(bool active, float DT)
{
	switch (fontsize)
	{
	case font_medium: ImGui::PushFont(gGuiResources.MediumFont); break;
	case font_large: ImGui::PushFont(gGuiResources.BigFont); break;
	default:
	case font_small: ImGui::PushFont(gGuiResources.SmallFont); break;
	}
	if (dynamic) SetTitle(src);
	float x2 = x;
	switch (Align)
	{
	case align_right:
	{
		ImVec2 textsize = ImGui::CalcTextSize(title);
		x2 -= textsize.x;
	}
	break;
	case align_center:
	{
		ImVec2 textsize = ImGui::CalcTextSize(title);
		x2 -= textsize.x / 2;
	}
	break;
	}

	ImGui::SetCursorPos(ImVec2(x2, y));
	ImGui::TextColored(Color, title);
	ImGui::PopFont();
}

void _screensetup_t::AddDynamicText(float x, float y, char *t, int len, alignment_t align, font_size fontsize)
{
	_textcontrol_t *T = new _textcontrol_t(true, t);
	T->Align = align;
	T->fontsize = fontsize;
	T->SetTitle(t);
	T->x = x;
	T->y = y;
	ControlsInOrder.push_back(T);

}

_textcontrol_t * _screensetup_t::AddText(float x, float y, char *t, alignment_t align, font_size fontsize)
{

	_textcontrol_t *T = new _textcontrol_t();
	T->fontsize = fontsize;
	T->Align = align;
	T->SetTitle(t);
	T->x = x;
	T->y = y;
	ControlsInOrder.push_back(T);
	return T;
}

void _screensetup_t::ChangeActiveControl(int delta)
{
	int Start = (ActiveControl + delta + ControlsInOrder.size()) % ControlsInOrder.size();
	for (int i = 0; i < (int)ControlsInOrder.size(); i++)
	{
		if (ControlsInOrder[Start]->enabled == true && ControlsInOrder[Start]->skipencodercycling == false)
		{
			ActiveControl = Start;
			return;
		}
		Start = (Start + delta + ControlsInOrder.size()) % ControlsInOrder.size();
	}
}

void _screensetup_t::SketchLeft(int delta)
{
	if (Modal)
	{
		Modal->SketchLeft(delta);
		return;
	}
	ChangeActiveControl(delta);

}

void _screensetup_t::SketchRight(int delta)
{
	if (Modal)
	{
		Modal->SketchRight(delta);
		return;
	}
	ControlsInOrder[ActiveControl]->SketchRightDelta(delta);
}

void _screensetup_t::SketchRightPress()
{
	ControlsInOrder[ActiveControl]->SketchRightPressed();
}

void _screensetup_t::EncoderPress(FinalEncoderEnum button)
{
	if (Modal)
	{
		Modal->EncoderPress(button);
		return;
	}

	int bottomencoderid = -1;

	switch (button)
	{
	case encoder_F1: bottomencoderid = 0; break;
	case encoder_F2: bottomencoderid = 1; break;
	case encoder_F3: bottomencoderid = 2; break;
	case encoder_F4: bottomencoderid = 3; break;
	case encoder_F5: bottomencoderid = 4; break;
	case encoder_F6: bottomencoderid = 5; break;
	case encoder_F7: bottomencoderid = 6; break;
	case encoder_F8: bottomencoderid = 7; break;
	case encoder_F9: bottomencoderid = 8; break;
	case encoder_F10: bottomencoderid = 9; break;
	case encoder_F11: bottomencoderid = 10; break;
	}

	if (bottomencoderid > -1)
	{
		if (encoders[currentencoderset][bottomencoderid].IsDirectOutput)
		{
			
			Modal = theParameterBindingModal;
			Modal->Parent = this;
			theParameterBindingModal->Setup( (OutputEnum) encoders[currentencoderset][bottomencoderid].target);			
			Modal->Activate(); 
		
		}
	}
}

int _screensetup_t::GetControlIndex(_control_t *c)
{
	for (int i = 0; i < (int)ControlsInOrder.size(); i++)
	{
		if (c == ControlsInOrder[i]) return i;
	}
	return -1;
}
void _screensetup_t::SetActiveControl(_control_t *c)
{
	int id = GetControlIndex(c);
	if (id > -1)
	{
		if (ActiveControl != id) ControlsInOrder[ActiveControl]->Deactivate();
		ActiveControl = id;
		ControlsInOrder[ActiveControl]->Activate();
	}

}
void _screensetup_t::SideButton(FinalLedButtonEnum b)
{
	if (Modal)
	{
		Modal->SideButton(b);
		return;
	}
	int SideButtonID = GetSideButtonID(b);
	if (SideButtonID > -1)
	{
		if (buttons[SideButtonID].enabled)
		{
			SetActiveControl(&buttons[SideButtonID]);
			buttons[SideButtonID].Pressed();
		}
	}
}

void _screensetup_t::SetupEncoderSet(int n)
{
	_control_t::SetupEncoderSet(n);
	for (int i = 0; i < MAXENCODERSETS; i++)
	{
		bool doskip = i != currentencoderset;
		for (int j = 0; j < 11; j++)
		{
			encoders[i][j].skipencodercycling = doskip;
		}
	}

	ModSourcesForOutputStruct MSFOS;
	for (int i = 0; i < 11; i++)
	{
		if (encoders[n][i].enabled && encoders[n][i].IsDirectOutput)
		{
			OutputEnum targetparam = gCurrentPreset.GetModulationOutput((OutputEnum)encoders[n][i].target);
			gCurrentPreset.FillModSourcesForOutput(targetparam, &MSFOS);
			encoders[n][i].ModSourceCount = MSFOS.Sources;
		}

	}
}

void _screensetup_t::SketchLeftPress()
{
	if (Modal)
	{
		Modal->SketchLeftPress();
		return;
	}
	gGui.GotoPage(SCREEN_HOME);
}

void _screensetup_t::PatchButton(FinalLedButtonEnum b)
{
	switch (b)
	{
	case ledbutton_B1:LoadPatch(0); break;
	case ledbutton_B2:LoadPatch(1); break;
	case ledbutton_B3:LoadPatch(2); break;
	case ledbutton_B4:LoadPatch(3); break;
	case ledbutton_B5:LoadPatch(4); break;
	case ledbutton_B6:LoadPatch(5); break;
	case ledbutton_B7:LoadPatch(6); break;
	case ledbutton_B8:LoadPatch(7); break;
	case ledbutton_B9:LoadPatch(8); break;
	case ledbutton_B10:LoadPatch(9); break;
	case ledbutton_B11:LoadPatch(10); break;
	case ledbutton_B12:LoadPatch(11); break;
	case ledbutton_B13:LoadPatch(12); break;
	case ledbutton_B14:LoadPatch(13); break;
	case ledbutton_B15:LoadPatch(14); break;
	case ledbutton_B16:LoadPatch(15); break;

	}

}

void _screensetup_t::Encoder(FinalEncoderEnum button, int delta)
{
	if (Modal)
	{
		Modal->Encoder(button, delta);
		return;
	}
	switch (button)
	{
	case encoder_F1: encoders[currentencoderset][0].Turn(delta); break;
	case encoder_F2: encoders[currentencoderset][1].Turn(delta); break;
	case encoder_F3: encoders[currentencoderset][2].Turn(delta); break;
	case encoder_F4: encoders[currentencoderset][3].Turn(delta); break;
	case encoder_F5: encoders[currentencoderset][4].Turn(delta); break;
	case encoder_F6: encoders[currentencoderset][5].Turn(delta); break;
	case encoder_F7: encoders[currentencoderset][6].Turn(delta); break;
	case encoder_F8: encoders[currentencoderset][7].Turn(delta); break;
	case encoder_F9: encoders[currentencoderset][8].Turn(delta); break;
	case encoder_F10: encoders[currentencoderset][9].Turn(delta); break;
	case encoder_F11: encoders[currentencoderset][10].Turn(delta); break;
	}
}

void _screensetup_t::RenderContent(bool active, float DT)
{
	if (Parent && ((_screensetup_t*)Parent)->Modal == this)
	{
		if (BG)			
		{
			ImGui::SetCursorPos(ImVec2(0, 0));
			ImGui::Image(BG, ImVec2(1024, 600));
		}
		else
		{
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(30, 30), ImVec2(1024 - 30, 600 - 30), gGuiResources.ModalBGColor);
		}
		
		
	}
	else
	{
		ImGui::SetCursorPos(ImVec2(0, 0));
		if (gGuiResources.testimage)
		{
			ImGui::Image(gGuiResources.TestBG, ImVec2(1024, 600));
		}
		else
		{
			ImGui::Image(BG?BG:gGuiResources.MainBG, ImVec2(1024, 600));
		}
	}
	if (strlen(title) > 0)
	{
		ImGui::PushFont(gGuiResources.BigFont);
		auto R = ImGui::CalcTextSize(title);
		ImGui::SetCursorPos(ImVec2(1024 / 2 - R.x / 2, ParamMasterMargin));
		ImGui::Text(title);
		ImGui::PopFont();
	}

	for (int i = 0; i < (int)ControlsInOrder.size(); i++)
	{
		ControlsInOrder[i]->Render((i == ActiveControl) && (Modal == NULL), DT);
	}

}

void _screensetup_t::RenderModalBox(bool active, float DT)
{
	if (Modal)
	{
		ImVec2 pos = ImGui::GetCursorPos();
		pos.x = 0;
		pos.y = 0;
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 1024, pos.y + 600), IM_COL32(0, 0, 0, 200));

		Modal->Render(active, DT);
	}
}

void _screensetup_t::Render(bool active, float DT)
{
	RenderContent(active, DT);
	RenderModalBox(active, DT);
}

class LedControl : public _control_t
{
public:
	LedTheme myLed;
	int x, y;
	LedControl(LedTheme w, int _x, int _y, const char* name)
	{
		SetTitle(name);
		x = _x;
		y = _y;

		myLed = w;
		skipencodercycling = true;
	}

	virtual void Render(bool active, float DT)
	{
		auto R = ImGui::CalcTextSize(title);
		ImGui::SetCursorPos(ImVec2(x-R.x/2 +20, y - ImGui::GetTextLineHeight()));
		uint16_t r, g, b;
		gPanState.GetThemeLed(myLed, &r, &g, &b);
		ImGui::Text(title);
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + 40, y + 40), IM_COL32(r >> 8, g >> 8, b >> 8, 255));
	}
};


void _screensetup_t::AddLedControl(const char *name, int x, int y, LedTheme whichled)
{
	ControlsInOrder.push_back(new LedControl(whichled, x, y, name));
}
