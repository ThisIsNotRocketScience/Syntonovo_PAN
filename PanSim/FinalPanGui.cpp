#include <stdint.h>
#include "../interface/paramdef.h"
#include <stdio.h>

enum ParamEnum
{
#define OUTPUT(name,codecport,codecpin, type,id, style,defaultvalue) Param_##name = id,
#define OUTPUT_VIRT(name,codecport,codecpin, type,id, style,defaultvalue) Param_##name = id,
#include "../interface/paramdef.h"
#undef OUTPUT
#undef OUTPUT_VIRT
	__OUTPUT_COUNT
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

typedef struct _sidebutton_t
{
	bool enabled;
	char textlabel[40];
	bool activestate;
	uint8_t ledmode;
} sidebutton_t;

enum
{
	bottomencoder_bipolar,
	bottomencoder_unipolar
};

typedef struct _bottomencoder_t
{
	bool enabled;
	char textlabel[40];
	uint8_t displaymode;	
} bottomencoder_t;

typedef struct _screensetup_t
{
	char title[40];
	sidebutton_t buttons[14];
	bottomencoder_t encoders[11];
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
#include "../libs/imgui-master/imgui.h"

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

void RenderScreen(_screensetup_t *s)
{
	if (strlen(s->title)>0)
	{
		ImGui::SetCursorPos(ImVec2(1024/2, 0));
		ImGui::Text(s->title);
	}

	for (int i = 0; i < 14; i++)
	{
		if (s->buttons[i].enabled)
		{
			int y = (i % 7) * 40;
			int x = 0;
			if (i > 7) x = 1024 - 80;
			ImGui::SetCursorPos(ImVec2(x, y));
			ImGui::Text(s->buttons[i].textlabel);
		}
	}
}


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

void ResetScreen(_screensetup_t *s)
{
	s->title[0] = 0;
	for (int i = 0; i < 14; i++)
	{
		s->buttons[i].enabled = 0;
	}
	for (int i = 0; i < 11; i++)
	{
		s->encoders[i].enabled = 0;
	}
}

bool EnableButton(_screensetup_t *s, int i, char *text, bool active = false, ledmodes l = ledmode_solid )
{
	sprintf(s->buttons[i].textlabel, text);
	s->buttons[i].enabled = 1;
	s->buttons[i].ledmode = l;
	return false;
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
	
	_screensetup_t Screen;
	ResetScreen(&Screen);
	sprintf(Screen.title, "Test scherm");
	if (EnableButton(&Screen, 2, "Button1!"))
	{
		//button2 pressed :-)
	}
	
	if (EnableButton(&Screen, 5, "Button2!"))
	{
		//button2 pressed :-)
	}

	if (EnableButton(&Screen, 11, "Button3!"))
	{
		//button2 pressed :-)
	}


	RenderScreen(&Screen);

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
