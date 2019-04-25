#include <stdint.h>
#include "../interface/paramdef.h"
#include <stdio.h>
#include "../libs/imgui-master/imgui.h"
#include <vector>
#include "FinalPanEnums.h"
#include "PanPreset.h"

PanPreset_t CurrentPreset;
PanPreset_t RevertPreset;

enum ParamEnum
{
#define OUTPUT(name,codecport,codecpin, type,id, style,defaultvalue) Param_##name = id,
#define OUTPUT_VIRT(name,codecport,codecpin, type,id, style,defaultvalue) Param_##name = id,
#include "../interface/paramdef.h"
#undef OUTPUT
#undef OUTPUT_VIRT
	__PARAMOUTPUT_COUNT
};

enum
{
	ENV_1,
	ENV_2,
	ENV_3,
	ENV_4,
	ENV_5,
	ENV_6,
	ENV_7,
	ENV_8,
	ENV_9,
	ENV_10,
	ENV_11,
	ENV_12,
	ENV_13,
	ENV_14,
	ENV_15,
	ENV_16,
	LFO_1,
	LFO_2,
	LFO_3,
	LFO_4,
	LFO_5,
	LFO_6,
	LFO_7,
	LFO_8,
	LFO_9,
	LFO_10,
	LFO_11,
	LFO_12,
	LFO_13,
	LFO_14,
	LFO_15,
	LFO_16,
	Keyboard_X,
	Keyboard_Y,
	Keyboard_Z,
	Keyboard_ZPrime,
	Keyboard_CV,
	Keyboard_Velocity,
	Pad_Left,
	Pad_Right,
	__modulation_source_count
};

typedef struct _modmatrix_target_t
{
	int16_t Amount;
	uint16_t ParameterID;
} modmatrix_target_t;

typedef struct _modmatrix_source_t
{
	int16_t MinValue;
	int16_t MaxValue;
	int16_t CurrentValue;
	int16_t ModulatorState;
	modmatrix_target_t targets[11];
} modmatrix_source_t;

typedef struct _modmatrix_t
{
	_modmatrix_source_t sources[__modulation_source_count];
} modmatrix_t;

enum ledmodes
{
	ledmode_solid,
	ledmode_blinkslow,
	ledmode_blinkfast,
	ledmode_off
};

typedef struct _led_t
{
	uint8_t mode;
	uint8_t r;
	uint8_t g;
	uint8_t b;
} led_t;

class _control_t
{
public:
	bool enabled;
	char title[40];
	bool activestate;

	virtual void SketchRight(int delta)
	{

	}

	virtual void Render(bool active)
	{

	}


};

enum alignment_t
{
	align_left,
	align_right,
	align_center,
	__align_count
};

class sidebutton_t : public _control_t
{
public:
	uint8_t ledmode;
	int x;
	int y;
	alignment_t Align;
	void SetupPosition(int id)
	{
		y = (id % 7) * 40;
		x = 0;
		Align = align_left;
		if (id > 7)
		{
			x = 1024;
			Align = align_right;
		}

	}
	virtual void Render(bool active)
	{
		if (enabled)
		{
			int x2 = x;
			if (Align == align_right)
			{
				ImVec2 textsize = ImGui::CalcTextSize(title);
				x2 -= textsize.x;
			}
			ImGui::SetCursorPos(ImVec2(x2, y));
			if (active)
			{
				ImGui::TextColored(ImVec4(1, 1, 0, 1), title);
			}
			else
			{
				ImGui::Text(title);
			}

		}

	}
};

enum
{
	bottomencoder_bipolar,
	bottomencoder_unipolar
};

class  bottomencoder_t :public _control_t
{
public:
	uint8_t displaymode;
};

class _screensetup_t : public _control_t
{
public:

	std::vector<_screensetup_t *> SubScreens;
	_screensetup_t *Parent;

	std::vector<_control_t *> ControlsInOrder;
	int ActiveControl;
	_screensetup_t *Modal;

	_screensetup_t(_screensetup_t *parent = NULL)
	{
		Parent = parent;
		Modal = NULL;
		SetTitle("");
		for (int i = 0; i < 14; i++)
		{
			EnableButton(i, "", false, ledmode_off);
			DisableButton(i);
			buttons[i].SetupPosition(i);
		}

		for (int i = 0; i < 11; i++)
		{
			DisableEncoder(i);
		}

		ControlsInOrder.push_back(&buttons[0]);
		ControlsInOrder.push_back(&buttons[1]);
		ControlsInOrder.push_back(&buttons[2]);
		ControlsInOrder.push_back(&buttons[3]);
		ControlsInOrder.push_back(&buttons[4]);
		ControlsInOrder.push_back(&buttons[5]);
		ControlsInOrder.push_back(&buttons[6]);

		ControlsInOrder.push_back(&encoders[0]);
		ControlsInOrder.push_back(&encoders[1]);
		ControlsInOrder.push_back(&encoders[2]);
		ControlsInOrder.push_back(&encoders[3]);
		ControlsInOrder.push_back(&encoders[4]);
		ControlsInOrder.push_back(&encoders[5]);
		ControlsInOrder.push_back(&encoders[6]);
		ControlsInOrder.push_back(&encoders[7]);
		ControlsInOrder.push_back(&encoders[8]);
		ControlsInOrder.push_back(&encoders[9]);
		ControlsInOrder.push_back(&encoders[10]);

		ControlsInOrder.push_back(&buttons[13]);
		ControlsInOrder.push_back(&buttons[12]);
		ControlsInOrder.push_back(&buttons[11]);
		ControlsInOrder.push_back(&buttons[10]);
		ControlsInOrder.push_back(&buttons[9]);
		ControlsInOrder.push_back(&buttons[8]);
		ControlsInOrder.push_back(&buttons[7]);


		SetFirstEnabledControlActive();
	}

	void SetFirstEnabledControlActive()
	{
		for (int i = 0; i < ControlsInOrder.size(); i++)
		{
			if (ControlsInOrder[i]->enabled)
			{
				ActiveControl = i;
				return;
			}
		}
		ActiveControl = 0;
	}
	void DisableEncoder(int i)
	{
		encoders[i].enabled = 0;
	}

	void DisableButton(int i)
	{
		buttons[i].enabled = false;
	}

	bool EnableButton(int i, char *text, bool active = true, ledmodes l = ledmode_solid)
	{
		sprintf(buttons[i].title, text);
		buttons[i].enabled = active;
		buttons[i].ledmode = l;
		return false;
	}

	int EnableAvailableButton(char *text, int style, int target)
	{
		for (int i = 0; i < 14; i++)
		{
			if (buttons[i].enabled == false)
			{
				sprintf(buttons[i].title, text);
				buttons[i].enabled = true;
				buttons[i].ledmode = ledmode_solid;
				return i;				
			}
		}
	}

	void ButtonStyle(int i, int style, int target)
	{

	}

	sidebutton_t buttons[14];
	bottomencoder_t encoders[11];

	void SetTitle(char *t)
	{
		sprintf_s(title, 40, "%s", t);
	}
	void ChangeActiveControl(int delta)
	{
		int Start = (ActiveControl + delta + ControlsInOrder.size()) % ControlsInOrder.size();
		for (int i = 0; i < ControlsInOrder.size(); i++)
		{
			if (ControlsInOrder[Start]->enabled)
			{
				ActiveControl = Start;
				return;
			}
			Start = (Start + delta + ControlsInOrder.size())% ControlsInOrder.size();
		}
	}

	virtual void SketchLeft(int delta)
	{
		if (Modal)
		{
			Modal->SketchLeft(delta);
			return;
		}
		ChangeActiveControl(delta);

	}

	virtual void SketchRight(int delta)
	{
		ControlsInOrder[ActiveControl]->SketchRight(delta);
	}

	virtual void SketchLeftPress()
	{
	}

	virtual void SketchRightPress()
	{
	}

	virtual void EncoderPress(FinalEncoderEnum button)
	{
	}


	void Encoder(FinalEncoderEnum button, int delta)
	{
	}

	virtual void Render()
	{
		if (strlen(title) > 0)
		{
			ImGui::SetCursorPos(ImVec2(1024 / 2, 0));
			ImGui::Text(title);
		}

		for (int i = 0; i < ControlsInOrder.size(); i++)
		{
			ControlsInOrder[i]->Render((i == ActiveControl) && (Modal == NULL));
		}
		if (Modal)
		{
			ImVec2 pos = ImGui::GetCursorPos();
			pos.x = 0;
			pos.y = 0;
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 1024, pos.y + 600), IM_COL32(0, 0, 0, 200));

			Modal->Render();
		}

	}
};


void RenderModulationAssign(int id, _modmatrix_source_t *source, _modmatrix_target_t *target)
{

}

void RenderModulationAssignBar(_modmatrix_source_t *source)
{
	for (int i = 0; i < 11; i++)
	{
		if (source->targets[i].ParameterID != 0xff)
		{
			RenderModulationAssign(i, source, &source->targets[i]);
		}
	}
}

typedef struct FinalPan_GuiResources_t
{
	ImTextureID MainBG;
	ImTextureID RootBG;
	ImTextureID LeftIndicator;
	ImTextureID RightIndicator;
	ImTextureID SpriteSheet;
	ImTextureID OnOff[4];
	ImFont *BigFont;
	ImFont *SmallFont;
	ImU32 Highlight;
	ImU32 Normal;
	ImU32 BGColor;
	ImU32 FillColor;
	int PageTime;
	int encoderbarmargin;
	int encoderheight;

	ImTextureID BgImages[1];
} FinalPan_GuiResources_t;

static FinalPan_GuiResources_t res;
extern ImTextureID Raspberry_LoadTexture(const char *filename);
static bool init = false;
void FinalPan_LoadResources()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	res.encoderbarmargin = 10;
	res.encoderheight = 600;
	res.PageTime = 0;
	res.Highlight = IM_COL32(235, 200, 28, 255);
	res.Normal = IM_COL32(255, 255, 255, 255);
	res.BGColor = IM_COL32(0, 58, 66, 255);
	res.FillColor = IM_COL32(0, 137, 127, 255);
	res.OnOff[0] = Raspberry_LoadTexture("UI_ONOFF_OFF.png");
	res.OnOff[1] = Raspberry_LoadTexture("UI_ONOFF_ON.png");
	res.OnOff[2] = Raspberry_LoadTexture("UI_ONOFF_OFF_HI.png");
	res.OnOff[3] = Raspberry_LoadTexture("UI_ONOFF_ON_HI.png");
	res.MainBG = Raspberry_LoadTexture("UI_MAINBG.png");
	res.RootBG = Raspberry_LoadTexture("UI_ROOTBG.png");
	res.LeftIndicator = Raspberry_LoadTexture("UI_LEFT.png");
	res.RightIndicator = Raspberry_LoadTexture("UI_RIGHT.png");
	res.SmallFont = io.Fonts->AddFontFromFileTTF("Fontfabric - Panton.otf", 40.0f);
	res.BigFont = io.Fonts->AddFontFromFileTTF("Fontfabric - Panton ExtraBold.otf", 54.0f);
	init = true;

}

class Gui
{
public:
	Gui()
	{
		BuildScreens();
		CurrentScreen = SCREEN_HOME;
	}

	void SketchLeftPress()
	{
		CS()->SketchLeftPress();
	}

	void SketchRightPress()
	{
		CS()->SketchRightPress();
	}

	void EncoderPress(FinalEncoderEnum button)
	{

		CS()->EncoderPress(button);
	}

	void SketchLeft(int delta)
	{
		CS()->SketchLeft(delta);
	}

	void SketchRight(int delta)
	{
		CS()->SketchRight(delta);
	}

	void Encoder(FinalEncoderEnum button, int delta)
	{

		CS()->Encoder(button, delta);
	}

	_screensetup_t *CS()

	{
		return Screens[CurrentScreen];
	}

	_screensetup_t *Screens[SCREENS_COUNT];
	void BuildScreens()
	{
		for (int i = 0; i < SCREENS_COUNT; i++) Screens[i] = 0;


		for (int i = 0; i < SCREENS_COUNT; i++)
		{
			if (Screens[i] == 0) Screens[i] = new _screensetup_t();
		}
		Screens[SCREEN_HOME]->SetTitle("Syntonovo Pan");


		Screens[SCREEN_VCO1]->SetTitle("Oscillator 1");
		Screens[SCREEN_VCO2]->SetTitle("Oscillator 2");
		Screens[SCREEN_VCO3]->SetTitle("Oscillator 3");
		Screens[SCREEN_VCF1]->SetTitle("Filter 1");

		Screens[SCREEN_VCF2a]->SetTitle("Filter 2: A");
		Screens[SCREEN_VCF2b]->SetTitle("Filter 2: B");
		Screens[SCREEN_VCF2c]->SetTitle("Filter 2: C");
		Screens[SCREEN_VCF2d]->SetTitle("Filter 2: D");


		Screens[SCREEN_TEST]->SetTitle("Test scherm");
		Screens[SCREEN_TEST]->EnableButton(2, "Button1!");
		Screens[SCREEN_TEST]->EnableButton(5, "Button2!");
		Screens[SCREEN_TEST]->EnableButton(11, "Button3!");

		_screensetup_t *current = Screens[SCREEN_TEST];
		int lastbutton = 0;
#define MENU(a,b,c) current = Screens[SCREEN_##a];current->SetTitle(c);lastbutton = 0;
#define ENTRY(a,b,c)  lastbutton = current->EnableAvailableButton( a,b,c);
		
		//ENTRY("Spectrum Mod", MenuEntry_Value, Output_VCF2_CROSSMOD) 
#include "PanUiMap.h"
#undef MENU
#undef ENTRY

		for (int i = 0; i < SCREENS_COUNT; i++)
		{
			Screens[i]->SetFirstEnabledControlActive();
		}
	}

	virtual	void Render()
	{
		Screens[CurrentScreen]->Render();

	}

	void GotoPage(Screens_t s)
	{
		if (CurrentScreen == s)
		{
			// maybe rotate through pages if they exist??
		}
		else
		{
			CurrentScreen = s;
		}

	}

	Screens_t CurrentScreen;
};

Gui gGui;




void FinalPan_PushStyle()
{
	ImGui::PushFont(res.SmallFont);
	ImGui::PushStyleColor(ImGuiCol_Text, res.Normal);
}

void FinalPan_PopStyle()
{
	ImGui::PopStyleColor();
	ImGui::PopFont();
}



bool isPageEncoder(FinalEncoderEnum Button)
{
	switch (Button)
	{
	case encoder_SketchLeft:
	case encoder_SketchRight:
	case encoder_F1:
	case encoder_F2:
	case encoder_F3:
	case encoder_F4:
	case encoder_F5:
	case encoder_F6:
	case encoder_F7:
	case encoder_F8:
	case encoder_F9:
	case encoder_F10:
	case encoder_F11: return false;
	}

	return true;
}

Screens_t GetPage(FinalEncoderEnum Button)
{
	switch (Button)
	{
		case encoder_VCF1Freq: return SCREEN_VCF1;
		case encoder_VCF1Mix: return SCREEN_VCF1;
		case encoder_VCF2a: return SCREEN_VCF2a;
		case encoder_VCF2b: return SCREEN_VCF2a;
		case encoder_VCF2c: return SCREEN_VCF2a;
		case encoder_VCF2d: return SCREEN_VCF2a;
		case encoder_Masterout: return SCREEN_MASTER;
		case encoder_VCO1: return SCREEN_VCO1;
		case encoder_VCO2: return SCREEN_VCO2;
		case encoder_VCO3: return SCREEN_VCO3;
		case encoder_VCO4: return SCREEN_VCO4567;
		case encoder_VCO5: return SCREEN_VCO4567;
		case encoder_VCO6: return SCREEN_VCO4567;
		case encoder_VCO7: return SCREEN_VCO4567;
		case encoder_VCO8: return SCREEN_VCO8;
	}
	return SCREEN_HOME;
}


void LedEncoderButtonPress(FinalEncoderEnum Button)
{
	switch (Button)
	{
	case encoder_SketchLeft:
		gGui.SketchLeftPress();
		return;

	case encoder_SketchRight:
		gGui.SketchRightPress();
		return;
	default:
	{
		if (isPageEncoder(Button))
		{
			Screens_t Page = GetPage(Button);
			gGui.GotoPage(Page);
		}
		else
		{
			gGui.EncoderPress(Button);
		}
	}
	break;
	}
}

void LedEncoderButtonLeft(FinalEncoderEnum Button)
{
	switch (Button)
	{
	case encoder_SketchLeft: gGui.SketchLeft(-1); break;
	case encoder_SketchRight: gGui.SketchRight(-1); break;
	}
}

void LedEncoderButtonRight(FinalEncoderEnum Button)
{
	switch (Button)
	{
	case encoder_SketchLeft: gGui.SketchLeft(1); break;
	case encoder_SketchRight: gGui.SketchRight(1); break;
	}

}

void RenderMain()
{
	if (!init)
	{
		FinalPan_LoadResources();
	}
	FinalPan_PushStyle();
	ImVec2 pos = ImGui::GetCursorPos();

	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 1024, pos.y + 600), res.BGColor);

	gGui.Render();

	FinalPan_PopStyle();
}

void FinalPan_WindowFrame()
{

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

	ImGui::Begin("screen", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);

	ImGui::SetWindowSize(ImVec2(1024, 600));

	RenderMain();

	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}
