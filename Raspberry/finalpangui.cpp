#include <stdint.h>
#include "../interface/paramdef.h"
#include <stdio.h>
#include "../libs/imgui-master/imgui.h"
#include <vector>
#include "FinalPanEnums.h"
#include "PanPreset.h"

#include "gui.h"

PanPreset_t gCurrentPreset;
PanPreset_t gRevertPreset;
PanState_t gPanState;

Gui gGui;

bool IsCenterEncoder(FinalEncoderEnum button)
{
	switch (button)
	{
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
	case encoder_F11:
		return true;
	}
	return false;
}
int GetAssociatedParameter(FinalEncoderEnum button)
{
	switch (button)
	{

	case encoder_Masterout: return Output_MASTER_LEVEL;
	case encoder_MasteroutHeadphone: return -1; // TODO -> headphone level?

												// 8 on left side
	case encoder_VCO1: return Output_VCO1_PITCH;
	case encoder_VCO2: return Output_VCO2_PITCH;
	case encoder_VCO3: return Output_VCO3_PITCH;
	case encoder_VCO4: return Output_VCO4_PITCH;
	case encoder_VCO5: return Output_VCO5_PITCH;
	case encoder_VCO6: return Output_VCO6_PITCH;
	case encoder_VCO7: return Output_VCO7_PITCH;
	case encoder_VCO8: return Output_NOISE_COLOR; // TODO -> correct? 

												  // 8 on right side
	case encoder_VCF1Freq: return Output_VCF1_CV;
	case encoder_VCF2a: return Output_VCF2_L_CV;
	case encoder_VCF2b: return Output_VCF2_M1_CV;
	case encoder_VCF2c: return Output_VCF2_M2_CV;
	case encoder_VCF2d: return Output_VCF2_H_CV;
	case encoder_Cleanmix: return Output_CLEANF_LEVEL;
	case encoder_VCF2Mix: return Output_VCF2_LEVEL;
	case encoder_VCF1Mix: return Output_VCF1_LEVEL;



	}
	return -1;
}


void FinalPan_SetupDefaultPreset()
{

#define SWITCH(name,id,defaultvalue) 	gCurrentPreset.PutSwitch(Switch_##name , defaultvalue>0?true:false);gRevertPreset.PutSwitch(Switch_##name , defaultvalue>0?true:false);   
#define OUTPUT(name,codecport,codecpin, type,id, style,defaultvalue) 	gCurrentPreset.paramvalue[id] = gRevertPreset.paramvalue[id] = defaultvalue;
#define OUTPUT_VIRT(name,codecport,codecpin, type,id, style,defaultvalue) gCurrentPreset.paramvalue[id] = gRevertPreset.paramvalue[id] = defaultvalue;
#include "../interface/paramdef.h"
#undef OUTPUT
#undef OUTPUT_VIRT
#undef SWITCH

	gCurrentPreset.high.h = 0x1000;
	gCurrentPreset.low.h = 0x4000;
	gCurrentPreset.active.h = 0x3000;

	gCurrentPreset.low.v = 0xffff;
	gCurrentPreset.active.v = 0xffff;
	gCurrentPreset.high.v = 0xffff;

	gCurrentPreset.active.s = 0x1000;
	gCurrentPreset.high.s = 0xffff;
	gCurrentPreset.low.s = 0xffff;

	gPanState.SetLed(Led_Low, gCurrentPreset.low);
	gPanState.SetLed(Led_High, gCurrentPreset.high);
	gPanState.SetLed(Led_Active, gCurrentPreset.active);
	gPanState.BankLeft = 0;
	gPanState.BankRight = 1;
	gRevertPreset.SetName("Fancy Beeping");
	gCurrentPreset.SetName("Fancy Beeping");

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
	ImFont *MediumFont;
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

void _control_t::SetTitle(const char *t)
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

	case MenuEntry_LedValue:
	{
		gCurrentPreset.TweakLed((LedParameter)target, delta); break;
		
	}

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
	case MenuEntry_Value: Parent->TweakParameterValue((OutputEnum)target, delta); break;
	default: Parent->TweakParameterValue((OutputEnum)target, delta); break;

	case MenuEntry_LedValue:
	{
		gCurrentPreset.TweakLed((LedParameter)target, delta); break;

	}
	
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
	br.x += ImGui::GetTextLineHeight() - 2;
	br.y += ParamVerticalBoxHeight;
	switch (mode)
	{
	case BOX_REGULAR:
	{
		ImVec2 br2 = br;
		ImVec2 tl2 = tl;
		tl2.y = br.y - (val * ParamVerticalBoxHeight) / 0xffff;
		ImGui::GetWindowDrawList()->AddRectFilled(tl2, br2, res.FillColor);
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

int ButtonHeight(int idx)
{
	return  (idx % 7) * (600 / 8) + 600 / 16;
}

void sidebutton_t::SetupPosition(int id)
{
	y = ButtonHeight(id);
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
	x = ((1024) / 12.0f) * id + 1024.0f/24.0f;
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

		case MenuEntry_EnvelopeValue: 
		case MenuEntry_LFOValue:
		{
			RenderBoxVertical(x, y, Parent->GetParameterValue(target), style == BOX_REGULAR, active);
		//	char txt[400];
	//		gCurrentPreset.DescribeParam((OutputEnum)target, style, txt, 400);
	//		VerticalText(txt, align_right);
		}

		break;

		case MenuEntry_LedValue:
		{

			RenderBoxVertical(x, y, gCurrentPreset.GetLedParameter((LedParameter) target), style == BOX_REGULAR, active);
			char txt[400];
			//gCurrentPreset.DescribeParam((OutputEnum)target, style, txt, 400);
			//VerticalText(txt, align_right);
		}

		break;

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

uint16_t lerp(uint16_t i, uint16_t f, uint16_t t)
{
	int diff = t - f;
	uint32_t R = (f << 16) + (diff * i);
	return R >> 16;
}

void LedLerp(bool active, uint16_t value, uint16_t *r, uint16_t *g, uint16_t *b)
{
	if (active)
	{
		*r = lerp(value, gPanState.low_led_r, gPanState.high_led_r);
		*g = lerp(value, gPanState.low_led_g, gPanState.high_led_g);
		*b = lerp(value, gPanState.low_led_b, gPanState.high_led_b);

		*r = lerp(0x8000, *r, gPanState.active_led_r);
		*g = lerp(0x8000, *g, gPanState.active_led_g);
		*b = lerp(0x8000, *b, gPanState.active_led_b);
	
	}
	else
	{
		*r = lerp(value, gPanState.low_led_r, gPanState.high_led_r);
		*g = lerp(value, gPanState.low_led_g, gPanState.high_led_g);
		*b = lerp(value, gPanState.low_led_b, gPanState.high_led_b);
	}
}

void bottomencoder_t::UpdateLed(bool active)
{
	if (enabled)
	{
		ledmode = active ? ledmode_blinkslow : ledmode_solid;
		if (active)
		{

		}
		uint16_t value = 0;
		switch (style)
		{
		case MenuEntry_LedValue:
			value = gCurrentPreset.GetLedParameter((LedParameter)target);
			break;
		default:
			value = gCurrentPreset.paramvalue[target];
			break;
		}
		LedLerp(active, value, &r, &g, &b);


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



uint16_t _screensetup_t::GetParameterValue(int param)
{
	return gCurrentPreset.paramvalue[param];
}

void _screensetup_t::TweakParameterValue(int param, int delta)
{
	gCurrentPreset.TweakParameter((OutputEnum)param, delta);
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



	for (int i = 0; i < LedButtonsThatOpenThisScreen.size(); i++)
	{
		gPanState.SetButtonLed((FinalLedButtonEnum)LedButtonsThatOpenThisScreen[i], ledmode_solid);
	}
	for (int i = 0; i < __FINALENCODER_COUNT; i++)
	{
		FinalEncoderEnum enc = (FinalEncoderEnum)i;
		if (IsCenterEncoder(enc) == false)
		{
			uint16_t V,r,g,b;
			V = 0;
			int idx = GetAssociatedParameter(enc);
			bool active = false;
			for (int i = 0; i < EncodersThatOpenThisScreen.size(); i++) if (EncodersThatOpenThisScreen[i] == i) active = true;
			
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
					gPanState.SetEncoderLed(enc, ledmode_off, 0,0,0);

				}
			}



		}
	}

	for (int i = 0; i < EncodersThatOpenThisScreen.size(); i++)
	{
		gPanState.SetEncoderLed((FinalEncoderEnum)EncodersThatOpenThisScreen[i], ledmode_solid, gPanState.active_led_r, gPanState.active_led_g, gPanState.active_led_r);
	}
//	gPanState.SetEncoderLed()


}

void _screensetup_t::Action(int action)
{

};

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

bool _screensetup_t::EnableButton(int i, const char *text, int style , int target, bool active , ledmodes l )
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

void _textcontrol_t::Render(bool active)
{
	switch (fontsize)
	{
	case font_medium: ImGui::PushFont(res.MediumFont); break;
	case font_large: ImGui::PushFont(res.BigFont); break;
	default:
	case font_small: ImGui::PushFont(res.SmallFont); break;
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

void _screensetup_t::AddDynamicText(float x, float y, char *t, int len, alignment_t align, font_size fontsize )
{
	_textcontrol_t *T = new _textcontrol_t(true, t);
	T->Align = align;
	T->fontsize = fontsize;
	T->SetTitle(t);
	T->x = x;
	T->y = y;
	ControlsInOrder.push_back(T);

}
void _screensetup_t::AddText(float x, float y, char *t, alignment_t align, font_size fontsize)
{
	
	_textcontrol_t *T = new _textcontrol_t();
	T->fontsize = fontsize;
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


void LoadSelectedPreset()
{
	int n = gPanState.CurrentPatch;
	int bank = gPanState.BankLeft;
	int idx = n;
	if (n > 7)
	{
		bank = gPanState.BankRight;
		idx -= 8;
	}

	printf("Pan should load preset %d from bank %c.\n", idx, bank + 'A');

}


void LoadPatch(int n)
{
	gPanState.CurrentPatch = n;

	LoadSelectedPreset();
	// do the stuff needed to load bank/patch
	// Load(bank,idx); -> maybe loading anim?

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
	case ledbutton_B10:LoadPatch( 9); break;
	case ledbutton_B11:LoadPatch( 10); break;
	case ledbutton_B12:LoadPatch( 11); break;
	case ledbutton_B13:LoadPatch( 12); break;
	case ledbutton_B14:LoadPatch( 13); break;
	case ledbutton_B15:LoadPatch( 14); break;
	case ledbutton_B16:LoadPatch( 15); break;

	}
	
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
		ImGui::PushFont(res.BigFont);
		auto R = ImGui::CalcTextSize(title);
		ImGui::SetCursorPos(ImVec2(1024 / 2 - R.x / 2, 0));
		ImGui::Text(title);
		ImGui::PopFont();
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



class LedControl : public _control_t
{
public:
	LedTheme myLed;
	int x, y;
	LedControl(LedTheme w,int _x, int _y, const char* name)
	{
		SetTitle(name);
		x = _x;
		y = _y;

		myLed = w;
		skipencodercycling = true;
	}

	virtual void Render(bool active)
	{
		ImGui::SetCursorPos(ImVec2(x, y - ImGui::GetTextLineHeight()));
		uint16_t r, g, b;
		gPanState.GetThemeLed(myLed, &r, &g, &b);
		ImGui::Text(title);		
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x,y), ImVec2(x + 40, y + 40), IM_COL32(r>>8, g >>8, b >>8, 255));
	}
};

void _screensetup_t::AddLedControl(const char *name, int x, int y, LedTheme whichled)
{
	ControlsInOrder.push_back(new LedControl(whichled, x, y, name));
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
	//res.BGColor = IM_COL32(0, 0, 0, 255);
	res.FillColor = IM_COL32(0, 137, 127, 255);
	res.OnOff[0] = Raspberry_LoadTexture("UI_ONOFF_OFF.png");
	res.OnOff[1] = Raspberry_LoadTexture("UI_ONOFF_ON.png");
	res.OnOff[2] = Raspberry_LoadTexture("UI_ONOFF_OFF_HI.png");
	res.OnOff[3] = Raspberry_LoadTexture("UI_ONOFF_ON_HI.png");
	res.MainBG = Raspberry_LoadTexture("UI_MAINBG.png");
	res.RootBG = Raspberry_LoadTexture("UI_ROOTBG.png");
	res.LeftIndicator = Raspberry_LoadTexture("UI_LEFT.png");
	res.RightIndicator = Raspberry_LoadTexture("UI_RIGHT.png");
	res.SmallFont = io.Fonts->AddFontFromFileTTF("Fontfabric - Panton.otf", 30.0f);
	res.MediumFont = io.Fonts->AddFontFromFileTTF("Fontfabric - Panton.otf", 38.0f);
	res.BigFont = io.Fonts->AddFontFromFileTTF("Fontfabric - Panton ExtraBold.otf", 44.0f);
	init = true;


	int X = sizeof(PanPreset_t);
	printf("Pan preset size: %d (%x) bytes\n", X, X);
	gGui.Init();
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

}

void Gui::Init()
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
	if (IsCenterEncoder(button))
	{
		CS()->Encoder(button, delta);
	}
	else
	{
		int param = GetAssociatedParameter(button);
		if (param > -1)
		{
			gCurrentPreset.TweakParameter((OutputEnum)param, delta);
		}
	}
}

void Gui::ButtonPressed(FinalLedButtonEnum Button)
{

	// insert what to do if selecting modulation source/target here? 
	switch (Button)
	{
	case ledbutton_BankLeft: GotoPage(SCREEN_SELECTBANKL); break;
	case ledbutton_BankRight: GotoPage(SCREEN_SELECTBANKR); break;
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
#define LetterBoxW 40
#define LetterBoxH 40


void RenderLettersInABox(int x, int y, bool active, const char *text, int w, int h)
{
	ImVec2 tl(x, y);
	ImVec2 br(x + w, y + h);
	ImGui::GetWindowDrawList()->AddRect(tl, br, active ? res.Highlight : res.Normal, 0, 0, 2);
	auto s = ImGui::CalcTextSize(text);
	ImGui::SetCursorPos(ImVec2(x + w / 2 - s.x, y + h / 2 - s.y / 2 - ImGui::GetTextLineHeight()));
	ImGui::Text(text);

}
class LetterControl : public _control_t
{
	public:
		int id;
		int x;
		int y;
		virtual void SketchRightDelta(int delta)
		{
			Current = ((Current + delta - 32 + 96) % 96) + 32;						
		}
		
		virtual void SketchRightPressed()
		{
			Current = ((Current + 32 - 64 + 64) % 64) + 64;
			
		}

		LetterControl(int _x, int _y, int _id)
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
		char Current;

		virtual void Render(bool active)
		{
			ImVec2 tl(x, y);
			ImVec2 br(x + LetterBoxW, y + LetterBoxH);
			ImGui::GetWindowDrawList()->AddRect(tl, br, active ? res.Highlight : res.Normal, 0, 0, 2);
			auto s = ImGui::CalcTextSize(&Current, &Current + 1);
			ImGui::SetCursorPos(ImVec2(x+LetterBoxW/2 - s.x, y + LetterBoxH/2 - s.y/2- ImGui::GetTextLineHeight()));
			ImGui::Text("%c", Current);
		}
};

class PresetScreen : public _screensetup_t
{
public:
	std::vector<LetterControl*> Letters;
	virtual void Action(int action)
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
		}
	}
	PresetScreen()
	{

		for (int i = 0; i < PRESET_NAME_LENGTH-1; i++)
		{
			AddLetterControl(1024/2 - (LetterBoxW+5)*((PRESET_NAME_LENGTH-1.5)/2) + i * (LetterBoxW+5), 300-LetterBoxH/2, i);
		}

		EnableButton(10, "Cancel", MenuEntry_Action, MenuAction_No);

		EnableButton(11, "OK", MenuEntry_Action, MenuAction_Yes);	
	}

	virtual void Activate()
	{
		for (int i = 0; i < PRESET_NAME_LENGTH - 1; i++)
		{
			Letters[i]->Current = gCurrentPreset.Name[i];
		}

		SetActiveControl(Letters[0]);
	}
	
	
	void AddLetterControl(int x, int y, int id)
	{
		auto L = new LetterControl(x, y, id);
		Letters.push_back(L);
		ControlsInOrder.push_back(L);
	}
};

class ModSourceScreen : public _screensetup_t
{
public:
	Screens_t myScreen;
	int ActiveInstance ;
	int MaxInstances;
	bool HasActiveInstanceDisplay;
	virtual uint16_t GetParameterValue(int param) 
	{
		return gCurrentPreset.GetModParameterValue((ModParameters)param, ActiveInstance);
	};
	virtual void TweakParameterValue(int param, int delta)
	{
		gCurrentPreset.TweakModulation((ModParameters)param, ActiveInstance, delta);
	}

	ModSourceScreen(Screens_t screen)
	{
		MaxInstances = 16;
		ActiveInstance = 0;
		myScreen = screen;
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
			EnableButton(1, "Previous", MenuEntry_Action,MenuAction_Prev);
			EnableButton(8, "Next", MenuEntry_Action, MenuAction_Next);
		}
		
	}



	virtual void Action(int a)
	{
		switch (a)
		{
		case MenuAction_Next: ActiveInstance = (ActiveInstance + 1) % MaxInstances; break;
		case MenuAction_Prev: ActiveInstance = (ActiveInstance + MaxInstances - 1) % MaxInstances; break;
		}
		
	}
	virtual void Render()
	{
		_screensetup_t::Render();
		if (HasActiveInstanceDisplay)
		{
			for (int i = 0; i < MaxInstances; i++)
			{
				char txt[10];
				sprintf(txt, "%d", i);
				RenderLettersInABox(i * 40 + 200, ButtonHeight(1), i == ActiveInstance, txt, 35, 35);
			}
		}

		//auto row = gCurrentPreset.modmatrix[gCurrentGetModSourceRow(myScreen, ActiveInstance)];

	}
	virtual void Activate()
	{

	}
};

class BankSelectScreen : public _screensetup_t
{
public:

	virtual void Action(int action)
	{
		
		if (Side == Left) gPanState.BankLeft = action; else gPanState.BankRight = action;
		LoadSelectedPreset();
		Activate();
	}
	LeftRight Side;
	void SetLeftRight(LeftRight lr)
	{

	}
	BankSelectScreen()
	{

	
	}
	virtual void Activate()
	{
		int *CurrentBank = &gPanState.BankLeft;
		if (Side == Left)
		{
			SetTitle("Select Left Bank");
		}
		else
		{
			CurrentBank= &gPanState.BankRight;
			SetTitle("Select Right Bank");
		}
		EnableButton(1, (*CurrentBank == 0) ? "A (current)" : "A", MenuEntry_Action, 0);
		EnableButton(2, (*CurrentBank == 1) ? "B (current)" : "B", MenuEntry_Action, 1);
		EnableButton(3, (*CurrentBank == 2) ? "C (current)" : "C", MenuEntry_Action, 2);
		EnableButton(4, (*CurrentBank == 3) ? "D (current)" : "D", MenuEntry_Action, 3);
		EnableButton(5, (*CurrentBank == 4) ? "E (current)" : "E", MenuEntry_Action, 4);
		EnableButton(6, (*CurrentBank == 5) ? "F (current)" : "F", MenuEntry_Action, 5);
		
		EnableButton(8, (*CurrentBank == 6) ? "G (current)" : "G", MenuEntry_Action, 6);
		EnableButton(9, (*CurrentBank == 7) ? "H (current)" : "H", MenuEntry_Action, 7);
		EnableButton(10, (*CurrentBank == 8) ? "I (current)" : "I", MenuEntry_Action, 8);
		EnableButton(11, (*CurrentBank == 9) ? "J (current)" : "J", MenuEntry_Action, 9);
		EnableButton(12, (*CurrentBank == 10) ? "K (current)" : "K", MenuEntry_Action, 10);
		EnableButton(13, (*CurrentBank == 11) ? "L (current)" : "L", MenuEntry_Action, 11);
	}
};

void Gui::BuildScreens()
{
	for (int i = 0; i < SCREENS_COUNT; i++) Screens[i] = 0;

	Screens[SCREEN_PRESET] = new PresetScreen();
	auto BL = new BankSelectScreen();
	BL->Side = Left;
	auto BR = new BankSelectScreen();
	BR->Side = Right;
	Screens[SCREEN_SELECTBANKL] = BL;
	Screens[SCREEN_SELECTBANKR] = BR;

	BL->LedButtonsThatOpenThisScreen.push_back(ledbutton_BankLeft);
	BR->LedButtonsThatOpenThisScreen.push_back(ledbutton_BankRight);

	Screens[SCREEN_X] = new ModSourceScreen(SCREEN_X);
	Screens[SCREEN_Y] = new ModSourceScreen(SCREEN_Y);
	Screens[SCREEN_Z] = new ModSourceScreen(SCREEN_Z);
	Screens[SCREEN_TOUCH] = new ModSourceScreen(SCREEN_TOUCH);
	Screens[SCREEN_VELOCITY] = new ModSourceScreen(SCREEN_VELOCITY);
	Screens[SCREEN_KEYBOARD] = new ModSourceScreen(SCREEN_KEYBOARD);

	Screens[SCREEN_LFO] = new ModSourceScreen(SCREEN_LFO);
	Screens[SCREEN_ENVELOPE] = new ModSourceScreen(SCREEN_ENVELOPE);

	for (int i = 0; i < SCREENS_COUNT; i++)
	{
		if (Screens[i] == 0) Screens[i] = new _screensetup_t();
	}

	Screens[SCREEN_HOME]->SetTitle("Syntonovo Pan");
	Screens[SCREEN_HOME]->AddText(512, 40, "Current preset: ", align_right);
	Screens[SCREEN_HOME]->AddDynamicText(512, 40, gCurrentPreset.Name, PRESET_NAME_LENGTH);

	Screens[SCREEN_HOME]->EncodersThatOpenThisScreen.push_back(encoder_SketchLeft);


	Screens[SCREEN_HOME]->EnableButton(8, "Store", MenuEntry_Page, SCREEN_PRESET);//(512, 40, "Some Sound");
	Screens[SCREEN_HOME]->EnableButton(9, "Revert", MenuEntry_Action, MenuAction_Revert);
	Screens[SCREEN_HOME]->EnableButton(10, "System", MenuEntry_Page, SCREEN_SYSTEM);
	Screens[SCREEN_HOME]->EnableButton(12, "Colors", MenuEntry_Page, SCREEN_COLORS);

	Screens[SCREEN_PRESET]->SetTitle("Edit Name/Category");

	Screens[SCREEN_COLORS]->SetTitle("Colors");


	Screens[SCREEN_COLORS]->AddLedControl("Low",100,200,  Led_Low);
	Screens[SCREEN_COLORS]->AddLedControl("High", 200, 200, Led_High);
	Screens[SCREEN_COLORS]->AddLedControl("Active", 300, 200, Led_Active);

	Screens[SCREEN_COLORS]->EnableAvailableEncoder("Low: Hue", MenuEntry_LedValue, Led_Low_Hue);
	Screens[SCREEN_COLORS]->EnableAvailableEncoder("Low: Sat", MenuEntry_LedValue, Led_Low_Sat);
	Screens[SCREEN_COLORS]->EnableAvailableEncoder("Low: Bright", MenuEntry_LedValue, Led_Low_Bright);

	Screens[SCREEN_COLORS]->EnableAvailableEncoder("BlinkSpeed", MenuEntry_LedValue, Led_BlinkSpeed);

	Screens[SCREEN_COLORS]->EnableAvailableEncoder("High: Hue", MenuEntry_LedValue, Led_High_Hue);
	Screens[SCREEN_COLORS]->EnableAvailableEncoder("High: Sat", MenuEntry_LedValue, Led_High_Sat);
	Screens[SCREEN_COLORS]->EnableAvailableEncoder("High: Bright", MenuEntry_LedValue, Led_High_Bright);

	Screens[SCREEN_COLORS]->EnableAvailableEncoder("Brightness", MenuEntry_LedValue, Led_Brightness);

	Screens[SCREEN_COLORS]->EnableAvailableEncoder("Active: Hue", MenuEntry_LedValue, Led_Active_Hue);
	Screens[SCREEN_COLORS]->EnableAvailableEncoder("Active: Sat", MenuEntry_LedValue, Led_Active_Sat);
	Screens[SCREEN_COLORS]->EnableAvailableEncoder("Active: Bright", MenuEntry_LedValue, Led_Active_Bright);

	Screens[SCREEN_X]->SetTitle("Keyboard X");
	Screens[SCREEN_Y]->SetTitle("Keyboard Y");
	Screens[SCREEN_Z]->SetTitle("Keyboard Z");
	Screens[SCREEN_TOUCH]->SetTitle("Keyboard Touch");
	Screens[SCREEN_VELOCITY]->SetTitle("Keyboard Velocity");
	Screens[SCREEN_KEYBOARD]->SetTitle("CV Keytrack");


	Screens[SCREEN_ENVELOPE]->SetTitle("Envelopes");	
	Screens[SCREEN_ENVELOPE]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BEnv);

	Screens[SCREEN_LFO]->SetTitle("LFO's");
	Screens[SCREEN_LFO]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BLFO);

	Screens[SCREEN_ARP]->SetTitle("Arpeggiator");
	Screens[SCREEN_ARP]->LedButtonsThatOpenThisScreen.push_back(ledbutton_ArpEdit);


	Screens[SCREEN_VCO1]->SetTitle("Oscillator 1");
	Screens[SCREEN_VCO1]->EncodersThatOpenThisScreen.push_back(encoder_VCO1);

	Screens[SCREEN_VCO2]->SetTitle("Oscillator 2");
	Screens[SCREEN_VCO2]->EncodersThatOpenThisScreen.push_back(encoder_VCO2);

	Screens[SCREEN_VCO3]->SetTitle("Oscillator 3");
	Screens[SCREEN_VCO3]->EncodersThatOpenThisScreen.push_back(encoder_VCO3);

	Screens[SCREEN_VCF1]->SetTitle("Filter 1");
	Screens[SCREEN_VCF1]->EncodersThatOpenThisScreen.push_back(encoder_VCF1Freq);


	Screens[SCREEN_VCF2a]->SetTitle("Filter 2: A");
	Screens[SCREEN_VCF2a]->EncodersThatOpenThisScreen.push_back(encoder_VCF2a);
	Screens[SCREEN_VCF2b]->SetTitle("Filter 2: B");
	Screens[SCREEN_VCF2b]->EncodersThatOpenThisScreen.push_back(encoder_VCF2b);
	Screens[SCREEN_VCF2c]->SetTitle("Filter 2: C");
	Screens[SCREEN_VCF2c]->EncodersThatOpenThisScreen.push_back(encoder_VCF2c);
	Screens[SCREEN_VCF2d]->SetTitle("Filter 2: D");
	Screens[SCREEN_VCF2d]->EncodersThatOpenThisScreen.push_back(encoder_VCF2d);


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
		CS()->Deactivate();
		CurrentScreen = s;
		CS()->Activate();
	}

}

void Gui::SetupLeds()
{
	gPanState.SetLed(Led_Active, gCurrentPreset.active);
	gPanState.SetLed(Led_High, gCurrentPreset.high);
	gPanState.SetLed(Led_Low, gCurrentPreset.low);
	gPanState.ClearLeds();
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
		// 2 sticking out 
	case encoder_Masterout: return SCREEN_MASTER;
	case encoder_MasteroutHeadphone: return SCREEN_MASTER;

		// 8 on left side
	case encoder_VCO1: return SCREEN_VCO1;
	case encoder_VCO2: return SCREEN_VCO2;
	case encoder_VCO3: return SCREEN_VCO3;
	case encoder_VCO4: return SCREEN_VCO4567;
	case encoder_VCO5: return SCREEN_VCO4567;
	case encoder_VCO6: return SCREEN_VCO4567;
	case encoder_VCO7: return SCREEN_VCO4567;
	case encoder_VCO8: return SCREEN_VCO8;

		// 8 on right side
	case encoder_VCF1Freq: return SCREEN_VCF1;
	case encoder_VCF2a: return SCREEN_VCF2a;
	case encoder_VCF2b: return SCREEN_VCF2a;
	case encoder_VCF2c: return SCREEN_VCF2a;
	case encoder_VCF2d: return SCREEN_VCF2a;
	case encoder_Cleanmix: return SCREEN_CLEANMIX;
	case encoder_VCF2Mix: return SCREEN_VCF2MIX;
	case encoder_VCF1Mix: return SCREEN_VCF1MIX;

	
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


void FinalPan_WindowFrame(float DT)
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

