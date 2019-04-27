#include <stdint.h>
#include "../interface/paramdef.h"
#include <stdio.h>
#include "../libs/imgui-master/imgui.h"
#include <vector>
#include "FinalPanEnums.h"
#include "PanPreset.h"

#include "Gui.h"

PanPreset_t gCurrentPreset;
PanPreset_t gRevertPreset;
PanState_t gPanState;
Gui gGui;

void FinalPan_SetupDefaultPreset()
{

#define OUTPUT(name,codecport,codecpin, type,id, style,defaultvalue) 	gCurrentPreset.paramvalue[id] = gRevertPreset.paramvalue[id] = defaultvalue;
#define OUTPUT_VIRT(name,codecport,codecpin, type,id, style,defaultvalue) gCurrentPreset.paramvalue[id] = gRevertPreset.paramvalue[id] = defaultvalue;
#include "../interface/paramdef.h"
#undef OUTPUT
#undef OUTPUT_VIRT

};

int GetSideButtonID(FinalLedButtonEnum B);
bool IsSideButton(FinalLedButtonEnum B);
bool IsPatchButton(FinalLedButtonEnum B);


#ifndef __min
#define __min(a,b) ((a<b)?(a):(b))
#endif

#ifndef __max
#define __max(a,b) ((a>b)?(a):(b))
#endif
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

#include "../libs/imgui-master/imgui.h"
#ifdef WIN32
#include <GL/gl3w.h> 
#else

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#endif
#include "../libs/lodepng-master/lodepng.h"


ImTextureID Raspberry_LoadTexture(const char *filename)
{
	std::vector<unsigned char> buffer, image;
	lodepng::load_file(buffer, filename); //load the image file with given filename
	unsigned w, h;
	unsigned error = lodepng::decode(image, w, h, buffer); //decode the png

	if (error)
	{
		printf("error reading %s\n", filename);
		return 0;
	}
	GLuint tex;

#ifdef WIN32
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_NEAREST = no smoothing
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

#else 
	GLint last_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
	glBindTexture(GL_TEXTURE_2D, last_texture);
#endif

	return (ImTextureID)tex;
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



void VerticalText(char *text, alignment_t align = align_left, ImU32 text_color = 0xffffffff)
{
	ImFont *font = ImGui::GetFont();
	char c;
	const ImFont::Glyph *glyph;

	const ImGuiStyle& style = ImGui::GetStyle();
	float pad = style.FramePadding.x;
	ImVec4 color;
	ImVec2 text_size = ImGui::CalcTextSize(text);

	ImVec2 pos = ImGui::GetCursorPos();

	pos.x += pad;
	pos.y += pad;// (text_size.x) + pad;
	if (align == align_right)
	{
		pos.y += text_size.x;
			
	}
	

	while ((c = *text++))
	{
		glyph = font->FindGlyph(c);
		if (!glyph) continue;

		ImGui::GetWindowDrawList()->PrimReserve(6, 4);
		ImGui::GetWindowDrawList()->PrimQuadUV(
			ImVec2(glyph->Y0 + pos.x, -glyph->X0 + pos.y),
			ImVec2(glyph->Y0 + pos.x, -glyph->X1 + pos.y),
			ImVec2(glyph->Y1 + pos.x, -glyph->X1 + pos.y),
			ImVec2(glyph->Y1 + pos.x, -glyph->X0 + pos.y),

			ImVec2(glyph->U0, glyph->V0),
			ImVec2(glyph->U1, glyph->V0),
			ImVec2(glyph->U1, glyph->V1),
			ImVec2(glyph->U0, glyph->V1),
			text_color);
		pos.y -= glyph->AdvanceX;
	}
}





_control_t::_control_t()
{
	style = MenuEntry_NoStyle;
	target = -1;
	enabled = true;
	skipencodercycling = false;
}

void _control_t::SetTitle(char *t)
{
	snprintf(title, 255, "%s", t);
}


void _control_t::Render(bool active)
{

}
void _control_t::Action(int action) {}
void _control_t::SketchRightPressed() {}

void _control_t::SketchRightDelta(int delta)
{
	switch (style)
	{
	case MenuEntry_Toggle: gCurrentPreset.PutSwitch((SwitchEnum)target, delta > 0); break;

	case MenuEntry_MidValue:
	case MenuEntry_Percentage:
	case MenuEntry_Pitch:
	case MenuEntry_EffectParam1:
	case MenuEntry_EffectParam2:
	case MenuEntry_EffectParam3:
	case MenuEntry_FilterMix:
	case MenuEntry_RemapValue:
	case MenuEntry_Value: gCurrentPreset.TweakParameter((OutputEnum)target, delta); break;

	}
}

void bottomencoder_t::Turn(int delta)
{
	switch (style)
	{
	case MenuEntry_Toggle: gCurrentPreset.PutSwitch((SwitchEnum)target, delta > 0); break;

	case MenuEntry_MidValue:
	case MenuEntry_Percentage:
	case MenuEntry_Pitch:
	case MenuEntry_EffectParam1:
	case MenuEntry_EffectParam2:
	case MenuEntry_EffectParam3:
	case MenuEntry_FilterMix:
	case MenuEntry_RemapValue:
	case MenuEntry_Value: gCurrentPreset.TweakParameter((OutputEnum)target, delta); break;

	}
}
void _control_t::Activate() {}
void _control_t::Deactivate() {}

void _control_t::RenderBox(int x, int y, int val, int mode, bool active)
{
	ImVec2 p = ImVec2(x, y + ImGui::GetTextLineHeight());
	p.x += 10;
	ImVec2 tl = p;
	ImVec2 br = tl;
	br.x += ParamBoxWidth;
	br.y += ImGui::GetTextLineHeight() - 2;
	switch (mode)
	{
	case BOX_REGULAR:
	{
		ImVec2 br2 = br;
		br2.x = tl.x + (val * 220) / 0xffff;
		ImGui::GetWindowDrawList()->AddRectFilled(tl, br2, res.FillColor);
	}
	break;
	case BOX_INV:
	{
		ImVec2 br2 = br;
		ImVec2 tl2 = tl;
		tl2.x = tl.x + (val * 220) / 0xffff;
		br2.x = tl.x + 220;
		ImGui::GetWindowDrawList()->AddRectFilled(tl, br2, res.FillColor);
	}
	break;
	case BOX_MID:
	{
		ImVec2 br2 = br;
		float x1 = tl.x + 110;
		float x2 = tl.x + (val * 220) / 0xffff;
		ImVec2 tl2 = tl;
		tl2.x = __min(x1, x2);
		br2.x = __max(x1, x2);
		ImGui::GetWindowDrawList()->AddRectFilled(tl2, br2, res.FillColor);
	}
	break;
	}
	ImGui::GetWindowDrawList()->AddRect(tl, br, active ? res.Highlight : res.Normal, 0, 0, 2);
	p.x += 12;
	ImGui::SetCursorScreenPos(p);
}

void _control_t::RenderBoxVertical(int x, int y, int val, int mode, bool active)
{
	ImVec2 p = ImVec2(x, y + ImGui::GetTextLineHeight());
	p.x += 10;
	ImVec2 tl = p;
	ImVec2 br = tl;
	br.x += 0;// ImGui::GetTextLineHeight() - 2;
	br.y += ParamVerticalBoxHeight;
	switch (mode)
	{
	case BOX_REGULAR:
	{
		ImVec2 br2 = br;
		br2.y = tl.y + (val * ParamVerticalBoxHeight) / 0xffff;
		ImGui::GetWindowDrawList()->AddRectFilled(tl, br2, res.FillColor);
	}
	break;
	case BOX_INV:
	{
		ImVec2 br2 = br;
		ImVec2 tl2 = tl;
		tl2.y = tl.y + (val * ParamVerticalBoxHeight) / 0xffff;
		br2.y = tl.y + 220;
		ImGui::GetWindowDrawList()->AddRectFilled(tl, br2, res.FillColor);
	}
	break;
	case BOX_MID:
	{
		ImVec2 br2 = br;
		float y1 = tl.y + 110;
		float y2 = tl.y + (val * ParamVerticalBoxHeight) / 0xffff;
		ImVec2 tl2 = tl;
		tl2.y = __min(y1, y2);
		br2.y = __max(y1, y2);
		ImGui::GetWindowDrawList()->AddRectFilled(tl2, br2, res.FillColor);
	}
	break;
	}
	ImGui::GetWindowDrawList()->AddRect(tl, br, active ? res.Highlight : res.Normal, 0, 0, 2);
	p.x += 12;
	ImGui::SetCursorScreenPos(p);
}


void sidebutton_t::UpdateLed(bool active)
{
	if (enabled)
	{
		ledmode = active ? ledmode_blinkslow : ledmode_solid;
	}
	else
	{
		ledmode = ledmode_off;
	}
}


void sidebutton_t::Pressed()
{
	switch (style)
	{
	case MenuEntry_Page: gGui.GotoPage((Screens_t)target); break;
	case MenuEntry_Action: Parent->Action(target); break;
	case MenuEntry_Toggle: gCurrentPreset.ToggleSwitch((SwitchEnum)target); break;
	}
}

void sidebutton_t::SketchRightPressed()
{
	Pressed();
}

void sidebutton_t::SetupPosition(int id)
{
	y = (id % 7) * (600 / 8) + 600 / 16;
	x = 10;
	Align = align_left;
	if (id > 6)
	{
		x = 1024 - 10;
		Align = align_right;
	}
}

void sidebutton_t::Render(bool active)
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


		switch (style)
		{

		case MenuEntry_MidValue:
		case MenuEntry_Percentage:
		case MenuEntry_Pitch:
		case MenuEntry_EffectParam1:
		case MenuEntry_EffectParam2:
		case MenuEntry_EffectParam3:
		case MenuEntry_FilterMix:
		case MenuEntry_RemapValue:
		case MenuEntry_Value:
		{

			if (Align == align_right)
			{
				RenderBox(x - 200 - ParamBoxWidth, y, gCurrentPreset.paramvalue[target], style == MenuEntry_MidValue ? BOX_MID : BOX_REGULAR, active);
			}
			else
			{
				RenderBox(x2 + 200, y, gCurrentPreset.paramvalue[target], style == MenuEntry_MidValue ? BOX_MID : BOX_REGULAR, active);
			}
			char txt[400];

			gCurrentPreset.DescribeParam((OutputEnum)target, style, txt, 400);
			ImGui::Text(txt);
		}

		break;

		case MenuEntry_Toggle:

			int id = gCurrentPreset.GetSwitch((SwitchEnum)target) ? 1 : 0;

			if (Align == align_right)
			{
				ImGui::SetCursorPos(ImVec2(x2 - 128 - 200, y));
			}
			else
			{
				ImGui::SetCursorPos(ImVec2(x2 + 200, y));

			}
			ImGui::Image(res.OnOff[id + (active ? 2 : 0)], ImVec2(128, 48));

			break;
		}
	}

}




void bottomencoder_t::SetupPosition(int id)
{
	y = 600 - 100;
	x = ((1024 - 200) / 11.0f) * id;
	Align = align_left;

}

void bottomencoder_t::Render(bool active)
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
			VerticalText(title, align_left, res.Highlight);
		}
		else
		{
			VerticalText(title, align_left);
		}


		switch (style)
		{

		case MenuEntry_MidValue:
		case MenuEntry_Percentage:
		case MenuEntry_Pitch:
		case MenuEntry_EffectParam1:
		case MenuEntry_EffectParam2:
		case MenuEntry_EffectParam3:
		case MenuEntry_FilterMix:
		case MenuEntry_RemapValue:
		case MenuEntry_Value:
		{

			RenderBoxVertical(x , y, gCurrentPreset.paramvalue[target], style == MenuEntry_MidValue ? BOX_MID : BOX_REGULAR, active);
			
			char txt[400];

			gCurrentPreset.DescribeParam((OutputEnum)target, style, txt, 400);
			VerticalText(txt, align_right);
		}

		break;

		case MenuEntry_Toggle:

			int id = gCurrentPreset.GetSwitch((SwitchEnum)target) ? 1 : 0;

			if (Align == align_right)
			{
				ImGui::SetCursorPos(ImVec2(x2 - 128 - 200, y));
			}
			else
			{
				ImGui::SetCursorPos(ImVec2(x2 + 200, y));

			}
			ImGui::Image(res.OnOff[id + (active ? 2 : 0)], ImVec2(128, 48));

			break;
		}
	}

}

void bottomencoder_t::UpdateLed(bool active)
{
	if (enabled)
	{
		ledmode = active ? ledmode_blinkslow : ledmode_solid;
		r = gCurrentPreset.paramvalue[Output_LEDR];
		g = gCurrentPreset.paramvalue[Output_LEDG];
		b = gCurrentPreset.paramvalue[Output_LEDB];
	}
	else
	{
		ledmode = ledmode_off;
	}
}



_screensetup_t::_screensetup_t(_screensetup_t *parent )
{
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

	for (int i = 0; i < 11; i++)
	{
		encoders[i].Parent = this;
		encoders[i].SetupPosition(i);
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

void _screensetup_t::SetupLeds()
{
	for (int i = 0; i < 14; i++)
	{
		buttons[i].UpdateLed(ControlsInOrder[ActiveControl] == &buttons[i]);
	}

	for (int i = 0; i < 11; i++)
	{
		encoders[i].UpdateLed(ControlsInOrder[ActiveControl] == &encoders[i]);
	}

	gPanState.SetEncoderLed(encoder_F1, encoders[0].ledmode,   encoders[ 0].r, encoders[0].g, encoders[0].b);
	gPanState.SetEncoderLed(encoder_F2, encoders[1].ledmode, encoders[1].r, encoders[1].g, encoders[1].b);
	gPanState.SetEncoderLed(encoder_F3, encoders[2].ledmode, encoders[2].r, encoders[2].g, encoders[2].b);
	gPanState.SetEncoderLed(encoder_F4, encoders[3].ledmode, encoders[3].r, encoders[3].g, encoders[3].b);
	gPanState.SetEncoderLed(encoder_F5, encoders[4].ledmode, encoders[4].r, encoders[4].g, encoders[4].b);
	gPanState.SetEncoderLed(encoder_F6, encoders[5].ledmode, encoders[5].r, encoders[5].g, encoders[5].b);
	gPanState.SetEncoderLed(encoder_F7, encoders[6].ledmode, encoders[6].r, encoders[6].g, encoders[6].b);
	gPanState.SetEncoderLed(encoder_F8, encoders[7].ledmode, encoders[7].r, encoders[7].g, encoders[7].b);
	gPanState.SetEncoderLed(encoder_F9, encoders[8].ledmode, encoders[8].r, encoders[8].g, encoders[8].b);
	gPanState.SetEncoderLed(encoder_F10, encoders[9].ledmode, encoders[9].r, encoders[9].g, encoders[9].b);
	gPanState.SetEncoderLed(encoder_F11, encoders[10].ledmode, encoders[10].r, encoders[10].g, encoders[10].b);

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


}

void _screensetup_t::Action(int action)
{

}

void _screensetup_t::SetFirstEnabledControlActive()
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

void _screensetup_t::DisableEncoder(int i)
{
	encoders[i].enabled = 0;
}

void _screensetup_t::DisableButton(int i)
{
	buttons[i].enabled = false;
}

bool _screensetup_t::EnableButton(int i, char *text, int style , int target, bool active , ledmodes l )
{
	buttons[i].SetTitle(text);
	buttons[i].style = style;
	buttons[i].target = target;

	buttons[i].enabled = active;
	buttons[i].ledmode = l;
	return false;
}

int _screensetup_t::EnableAvailableEncoder(char *text, int style, int target)
{
	for (int i = 0; i < 11; i++)
	{
		if (encoders[i].enabled == false)
		{
			encoders[i].SetTitle(text);
			encoders[i].enabled = true;
			encoders[i].style = style;
			encoders[i].target = target;
			encoders[i].ledmode = ledmode_solid;
			return i;
		}
	}
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
}

void _screensetup_t::ButtonStyle(int i, int style, int target)
{

}

void _screensetup_t::AddText(float x, float y, char *t, alignment_t align)
{
	_textcontrol_t *T = new _textcontrol_t();
	T->Align = align;
	T->SetTitle(t);
	T->x = x;
	T->y = y;
	ControlsInOrder.push_back(T);
}

void _screensetup_t::ChangeActiveControl(int delta)
{
	int Start = (ActiveControl + delta + ControlsInOrder.size()) % ControlsInOrder.size();
	for (int i = 0; i < ControlsInOrder.size(); i++)
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
}
int _screensetup_t::GetControlIndex(_control_t *c)
{
	for (int i = 0; i < ControlsInOrder.size(); i++)
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

void _screensetup_t::PatchButton(FinalLedButtonEnum b)
{
}

void _screensetup_t::Encoder(FinalEncoderEnum button, int delta)
{
	switch (button)
	{
	case encoder_F1: encoders[0].Turn(delta); break;
	case encoder_F2: encoders[1].Turn(delta); break;
	case encoder_F3: encoders[2].Turn(delta); break;
	case encoder_F4: encoders[3].Turn(delta); break;
	case encoder_F5: encoders[4].Turn(delta); break;
	case encoder_F6: encoders[5].Turn(delta); break;
	case encoder_F7: encoders[6].Turn(delta); break;
	case encoder_F8: encoders[7].Turn(delta); break;
	case encoder_F9: encoders[8].Turn(delta); break;
	case encoder_F10: encoders[9].Turn(delta); break;
	case encoder_F11: encoders[10].Turn(delta); break;
	}
}

void _screensetup_t::Render()
{
	if (strlen(title) > 0)
	{
		auto R = ImGui::CalcTextSize(title);
		ImGui::SetCursorPos(ImVec2(1024 / 2 - R.x / 2, 0));
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
	res.BGColor = IM_COL32(0, 0, 0, 255);
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




int GetSideButtonID(FinalLedButtonEnum B)
{
	switch (B)
	{
	case ledbutton_L1: return 0;
	case ledbutton_L2:return 1;
	case ledbutton_L3:return 2;
	case ledbutton_L4:return 3;
	case ledbutton_L5:return 4;
	case ledbutton_L6:return 5;
	case ledbutton_L7:return 6;

	case ledbutton_R1:return 7;
	case ledbutton_R2:return 8;
	case ledbutton_R3:return 9;
	case ledbutton_R4:return 10;
	case ledbutton_R5:return 11;
	case ledbutton_R6:return 12;
	case ledbutton_R7:return 13;

	}

	return -1;
}
bool IsSideButton(FinalLedButtonEnum B)
{
	if (GetSideButtonID(B) > -1) return true;
	return false;
}


bool IsPatchButton(FinalLedButtonEnum B)
{
	switch (B)
	{
	case ledbutton_B1:
	case ledbutton_B2:
	case ledbutton_B3:
	case ledbutton_B4:
	case ledbutton_B5:
	case ledbutton_B6:
	case ledbutton_B7:
	case ledbutton_B8:
	case ledbutton_B9:
	case ledbutton_B10:
	case ledbutton_B11:
	case ledbutton_B12:
	case ledbutton_B13:
	case ledbutton_B14:
	case ledbutton_B15:
	case ledbutton_B16:
		return true;
	}

	return false;
}

Gui::Gui()
{
	BuildScreens();
	CurrentScreen = SCREEN_HOME;
}

void Gui::SketchLeftPress()
{
	CS()->SketchLeftPress();
}

void Gui::SketchRightPress()
{
	CS()->SketchRightPress();
}

void Gui::EncoderPress(FinalEncoderEnum button)
{

	CS()->EncoderPress(button);
}

void Gui::SketchLeft(int delta)
{
	CS()->SketchLeft(delta);
}

void Gui::SketchRight(int delta)
{
	CS()->SketchRight(delta);
}

void Gui::Encoder(FinalEncoderEnum button, int delta)
{

	CS()->Encoder(button, delta);
}

void Gui::ButtonPressed(FinalLedButtonEnum Button)
{

	// insert what to do if selecting modulation source/target here? 
	switch (Button)
	{
	case ledbutton_ArpEdit: GotoPage(SCREEN_ARP); break;
	case ledbutton_BX:  GotoPage(SCREEN_X); break;
	case ledbutton_BY:  GotoPage(SCREEN_Y); break;
	case ledbutton_BZ:  GotoPage(SCREEN_Z); break;

	case ledbutton_BEnv: GotoPage(SCREEN_ENVELOPE); break;
	case ledbutton_BLFO: GotoPage(SCREEN_LFO); break;
	case ledbutton_BCV: GotoPage(SCREEN_KEYBOARD); break;
	case ledbutton_BTouch: GotoPage(SCREEN_TOUCH); break;

	}

	if (IsSideButton(Button))
	{
		CS()->SideButton(Button);
	}
	if (IsPatchButton(Button))
	{
		CS()->PatchButton(Button);
	}

}

_screensetup_t *Gui::CS()

{
	return Screens[CurrentScreen];
}

void Gui::BuildScreens()
{
	for (int i = 0; i < SCREENS_COUNT; i++) Screens[i] = 0;


	for (int i = 0; i < SCREENS_COUNT; i++)
	{
		if (Screens[i] == 0) Screens[i] = new _screensetup_t();
	}

	Screens[SCREEN_HOME]->SetTitle("Syntonovo Pan");
	Screens[SCREEN_HOME]->AddText(512, 40, "Current preset: ", align_right);
	Screens[SCREEN_HOME]->AddText(512, 40, "Some Sound");

	Screens[SCREEN_HOME]->EnableButton(8, "Store", MenuEntry_Action, MenuAction_Store);//(512, 40, "Some Sound");
	Screens[SCREEN_HOME]->EnableButton(9, "Revert", MenuEntry_Action, MenuAction_Revert);
	Screens[SCREEN_HOME]->EnableButton(10, "System", MenuEntry_Page, SCREEN_SYSTEM);
	Screens[SCREEN_HOME]->EnableButton(12, "Lights", MenuEntry_Page, SCREEN_LIGHTS);


	Screens[SCREEN_LIGHTS]->SetTitle("Lights");

	Screens[SCREEN_LIGHTS]->EnableAvailableEncoder("R", MenuEntry_Value, Output_LEDR);
	Screens[SCREEN_LIGHTS]->EnableAvailableEncoder("G", MenuEntry_Value, Output_LEDG);
	Screens[SCREEN_LIGHTS]->EnableAvailableEncoder("B", MenuEntry_Value, Output_LEDB);

	Screens[SCREEN_LIGHTS]->EnableAvailableEncoder("R", MenuEntry_Value, Output_HLEDR);
	Screens[SCREEN_LIGHTS]->EnableAvailableEncoder("G", MenuEntry_Value, Output_HLEDG);
	Screens[SCREEN_LIGHTS]->EnableAvailableEncoder("B", MenuEntry_Value, Output_HLEDB);

	Screens[SCREEN_LIGHTS]->EnableAvailableEncoder("Brightness", MenuEntry_Value, Output_LEDBRIGHT);


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
	int lastencoder = 0;
#define MENU(a,b,c) current = Screens[SCREEN_##a];current->SetTitle(c);lastbutton = 0;lastencoder = 0;
#define ENTRY(a,b,c)  lastencoder = current->EnableAvailableEncoder( a,b,c);
#define CUSTOMENTRY(name, style, target) lastbutton = current->EnableAvailableButton(name ,style, target);
	//ENTRY("Spectrum Mod", MenuEntry_Value, Output_VCF2_CROSSMOD) 
#include "PanUiMap.h"
#undef MENU
#undef ENTRY

	for (int i = 0; i < SCREENS_COUNT; i++)
	{
		Screens[i]->SetFirstEnabledControlActive();
	}
}

void Gui::Render()
{
	Screens[CurrentScreen]->Render();

}

void Gui::GotoPage(Screens_t s)
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

void Gui::SetupLeds()
{
	CS()->SetupLeds();
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

void RenderMain()
{
	if (!init)
	{
		FinalPan_LoadResources();
	}
	FinalPan_PushStyle();
	ImVec2 pos = ImGui::GetCursorPos();
	pos = ImGui::GetCursorScreenPos();
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(1024 + pos.x, 600 + pos.y), res.BGColor);

	gGui.Render();

	FinalPan_PopStyle();
}


void FinalPan_SetupLeds()
{
	gGui.SetupLeds();
}


void FinalPan_WindowFrame()
{

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	

	ImGui::Begin("screen", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);

	ImGui::SetWindowSize(ImVec2(1024, 600));

	RenderMain();

	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}


// INTERFACE FUNCTIONS

void LedButtonPressed(FinalLedButtonEnum Button)
{
	gGui.ButtonPressed(Button);
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
	default: gGui.Encoder(Button, -1); break;
	}
}

void LedEncoderButtonRight(FinalEncoderEnum Button)
{
	switch (Button)
	{
	case encoder_SketchLeft: gGui.SketchLeft(1); break;
	case encoder_SketchRight: gGui.SketchRight(1); break;
	default: gGui.Encoder(Button, 1); break;

	}

}

