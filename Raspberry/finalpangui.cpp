#include <stdint.h>
#include "../interface/paramdef.h"
#include <stdio.h>
#include "../libs/imgui-master/imgui.h"
#include <vector>
#include "FinalPanEnums.h"
#include "PanPreset.h"

#include "gui.h"

#include "ModSourceScreen.h"
#include "HomeScreen.h"
#include "VCF2Structure.h"


PanPreset_t gCurrentPreset;
PanPreset_t gRevertPreset;
PanState_t gPanState;

Gui gGui;

void cmd_pad_zero();
void cmd_calibrate();
void cmd_preset_load(int presetid);
void cmd_preset_save(int presetid);



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

void Gui::NextVCO()
{
	switch (CurrentScreen)
	{

	case SCREEN_VCO1:	GotoPage(SCREEN_VCO2); break;
	case SCREEN_VCO2:	GotoPage(SCREEN_VCO3); break;
	case SCREEN_VCO3:	GotoPage(SCREEN_VCO4); break;
	case SCREEN_VCO4:	GotoPage(SCREEN_VCO5); break;
	case SCREEN_VCO5:	GotoPage(SCREEN_VCO6); break;
	case SCREEN_VCO6:	GotoPage(SCREEN_VCO7); break;
	case SCREEN_VCO7:	GotoPage(SCREEN_VCO8); break;
	case SCREEN_VCO8:	GotoPage(SCREEN_VCO1); break;

	default: GotoPage(SCREEN_VCO1); break;
	}

}
void Gui::PrevVCO()
{
	switch (CurrentScreen)
	{

	case SCREEN_VCO1:	GotoPage(SCREEN_VCO8); break;
	case SCREEN_VCO2:	GotoPage(SCREEN_VCO1); break;
	case SCREEN_VCO3:	GotoPage(SCREEN_VCO2); break;
	case SCREEN_VCO4:	GotoPage(SCREEN_VCO3); break;
	case SCREEN_VCO5:	GotoPage(SCREEN_VCO4); break;
	case SCREEN_VCO6:	GotoPage(SCREEN_VCO5); break;
	case SCREEN_VCO7:	GotoPage(SCREEN_VCO6); break;
	case SCREEN_VCO8:	GotoPage(SCREEN_VCO7); break;

	default: GotoPage(SCREEN_VCO1); break;
	}
}

void Gui::NextVCF2()
{
	switch (CurrentScreen)
	{

	case SCREEN_VCF2a:	GotoPage(SCREEN_VCF2b); break;
	case SCREEN_VCF2b:	GotoPage(SCREEN_VCF2c); break;
	case SCREEN_VCF2c:	GotoPage(SCREEN_VCF2d); break;
	case SCREEN_VCF2d:	GotoPage(SCREEN_VCF2a); break;

	default: GotoPage(SCREEN_VCF2a); break;
	}
}
void Gui::PrevVCF2()
{
	switch (CurrentScreen)
	{

	case SCREEN_VCF2a:	GotoPage(SCREEN_VCF2d); break;
	case SCREEN_VCF2b:	GotoPage(SCREEN_VCF2a); break;
	case SCREEN_VCF2c:	GotoPage(SCREEN_VCF2b); break;
	case SCREEN_VCF2d:	GotoPage(SCREEN_VCF2c); break;

	default: GotoPage(SCREEN_VCF2a); break;
	}
}

int GetAssociatedParameter(FinalEncoderEnum button)
{
	switch (button)
	{

	case encoder_Masterout: return Output_MASTER_LEVEL;
	case encoder_MasteroutHeadphone: return Output_MASTER_LEVEL; // TODO -> headphone level?

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
	case encoder_VCF2a: return Output_VCF2_A_CV;
	case encoder_VCF2b: return Output_VCF2_B_CV;
	case encoder_VCF2c: return Output_VCF2_C_CV;
	case encoder_VCF2d: return Output_VCF2_D_CV;
	case encoder_Cleanmix: return Output_CLEANF_LEVEL;
	case encoder_VCF2Mix: return Output_VCF2_LEVEL;
	case encoder_VCF1Mix: return Output_VCF1_LEVEL;



	}
	return -1;
}
int ButtonHeight(int idx)
{
	return  (int)((idx % 7 + 0.5f) * (600.0f / 7.0f));
}

int MButtonHeight(int idx)
{
	return  ButtonHeight(0) + (int)((idx % 7) * (600.0f / 10.0f));
}

extern presetnames_t presetnames;

void FinalPan_DefaultPresetNamesInBank()
{
	for (int i = 0; i < PRESET_COUNT; i++)
	{
		snprintf(presetnames.names[i], PRESET_NAME_LENGTH, "%c%d", 'A' + (i / 8), i % 8);
	}
}

void FinalPan_SetupDefaultPreset()
{

#define SWITCH(name,id,defaultvalue) 	gCurrentPreset.PutSwitch(Switch_##name , defaultvalue>0?true:false);gRevertPreset.PutSwitch(Switch_##name , defaultvalue>0?true:false);   
#define OUTPUT(name,codecport,codecpin, type,id, style,defaultvalue,Label, Category) 	gCurrentPreset.paramvalue[id] = gRevertPreset.paramvalue[id] = defaultvalue;
#define OUTPUT_VIRT(name,codecport,codecpin, type,id, style,defaultvalue,Label, Category) gCurrentPreset.paramvalue[id] = gRevertPreset.paramvalue[id] = defaultvalue;
#include "../interface/paramdef.h"
#undef OUTPUT
#undef OUTPUT_VIRT
#undef SWITCH

	gCurrentPreset.high.h = 0x2000;
	gCurrentPreset.low.h = 0x1000;
	gCurrentPreset.active.h = 0x2200;

	gCurrentPreset.low.v = 0x4000;
	gCurrentPreset.active.v = 0x8000;
	gCurrentPreset.high.v = 0xff00;

	gCurrentPreset.active.s = 0x8fff;
	gCurrentPreset.high.s = 0xffff;
	gCurrentPreset.low.s = 0xffff;

	gPanState.SetLed(Led_Low, gCurrentPreset.low);
	gPanState.SetLed(Led_High, gCurrentPreset.high);
	gPanState.SetLed(Led_Active, gCurrentPreset.active);
	gPanState.BankLeft = 0;
	gPanState.BankRight = 1;
	gRevertPreset.SetName("Fancy Beeping");
	gCurrentPreset.SetName("Fancy Beeping");


	for (int i = 0; i < SYNTH_MODSOURCE_COUNT; i++)
	{
		for (int j = 0; j < MODTARGET_COUNT; j++)
		{
			gCurrentPreset.modmatrix[i].targets[j].depth = 0;
			gCurrentPreset.modmatrix[i].targets[j].outputid = NOMODTARGET;
			gCurrentPreset.modmatrix[i].targets[j].sourceid = 0;
		}
	}
	gCurrentPreset.modmatrix[0x10].targets[0].depth = 0xffff;
	gCurrentPreset.modmatrix[0x10].targets[0].outputid = 0;
	gCurrentPreset.modmatrix[0x10].targets[0].sourceid = Output_VCF1_LEVEL;

	gCurrentPreset.modmatrix[0x20].targets[0].depth = 0xffff;
	gCurrentPreset.modmatrix[0x20].targets[0].outputid = 0;
	gCurrentPreset.modmatrix[0x20].targets[0].sourceid = Output_MASTER_PITCH;


	gCurrentPreset.modmatrix[modsource_ENV0].targets[0].depth = 0x3fff;
	gCurrentPreset.modmatrix[modsource_ENV0].targets[0].outputid = Output_VCF1_LIN;

	gCurrentPreset.modmatrix[modsource_ENV0].targets[1].depth = 0x3fff;
	gCurrentPreset.modmatrix[modsource_ENV0].targets[1].outputid = Output_VCF2_LIN;

	gCurrentPreset.modmatrix[modsource_ENV0].targets[2].depth = 0x3fff;
	gCurrentPreset.modmatrix[modsource_ENV0].targets[2].outputid = Output_CLEANF_LIN;

	gCurrentPreset.modmatrix[modsource_X].targets[0].depth = 0x3fff;
	gCurrentPreset.modmatrix[modsource_X].targets[0].outputid = Output_VCF1_CV;




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



FinalPan_GuiResources_t gGuiResources;

enum ParamEnum
{
#define OUTPUT(name,codecport,codecpin, type,id, style,defaultvalue,Label, Category) Param_##name = id,
#define OUTPUT_VIRT(name,codecport,codecpin, type,id, style,defaultvalue,Label, Category) Param_##name = id,
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

void VerticalText(char *text, alignment_t align, ImU32 text_color)
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
	Parent = NULL;
}

void _control_t::SetTitle(const char *t)
{
	snprintf(title, 255, "%s", t);
}


void _control_t::Render(bool active, float DT)
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

sidebutton_t::sidebutton_t()
{
	myIcon = Icon_NO;
}

void _control_t::Activate() {
	SetupEncoderSet(currentencoderset);
}
void _control_t::Deactivate() {}

ImU32 CalcFillColor(uint16_t value, bool active)
{
	uint16_t r, g, b;
	if (0)
	{
		r = gPanState.active_led_r;
		g = gPanState.active_led_g;
		b = gPanState.active_led_b;
	}
	else
	{
		if (value > 0)
		{
			r = gPanState.high_led_r;
			g = gPanState.high_led_g;
			b = gPanState.high_led_b;
		}
		else
		{
			r = gPanState.low_led_r;
			g = gPanState.low_led_g;
			b = gPanState.low_led_b;

		}
	}
	return ImColor(r >> 8, g >> 8, b >> 8);
}

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
		ImGui::GetWindowDrawList()->AddRectFilled(tl, br, CalcFillColor(0, active));
		ImGui::GetWindowDrawList()->AddRectFilled(tl, br2, CalcFillColor(1, active));
		ImGui::GetWindowDrawList()->AddRect(tl, br2, active ? gGuiResources.Highlight : gGuiResources.Normal, 0, 0, 2);
	}
	break;
	case BOX_INV:
	{
		ImVec2 br2 = br;
		ImVec2 tl2 = tl;
		tl2.x = tl.x + (val * 220) / 0xffff;
		br2.x = tl.x + 220;
		ImGui::GetWindowDrawList()->AddRectFilled(tl, br, CalcFillColor(0, active));
		ImGui::GetWindowDrawList()->AddRectFilled(tl, br2, CalcFillColor(1, active));
		ImGui::GetWindowDrawList()->AddRect(tl, br2, active ? gGuiResources.Highlight : gGuiResources.Normal, 0, 0, 2);
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
		ImGui::GetWindowDrawList()->AddRectFilled(tl, br, CalcFillColor(0, active));

		ImGui::GetWindowDrawList()->AddRectFilled(tl2, br2, CalcFillColor(1, active));
		ImGui::GetWindowDrawList()->AddRect(tl2, br2, active ? gGuiResources.Highlight : gGuiResources.Normal, 0, 0, 2);
	}
	break;
	}
	ImGui::GetWindowDrawList()->AddRect(tl, br, active ? gGuiResources.Highlight : gGuiResources.Normal, 0, 0, 2);
	p.x += 12;
	ImGui::SetCursorScreenPos(p);
}

void _control_t::RenderBoxVertical(int x, int y, int val, int mode, bool active)
{
	ImVec2 p = ImVec2(x, y);// +ImGui::GetTextLineHeight());
	p.x -= ParamBoxDim / 2;
	ImVec2 tl = p;
	ImVec2 br = tl;
	br.x += ParamBoxDim;
	br.y += ParamVerticalBoxHeight;
	switch (mode)
	{
	case BOX_REGULAR:
	{
		ImVec2 br2 = br;
		ImVec2 tl2 = tl;
		tl2.y = br.y - (val * ParamVerticalBoxHeight) / 0xffff;
		ImGui::GetWindowDrawList()->AddRectFilled(tl, br, CalcFillColor(0, active));
		ImGui::GetWindowDrawList()->AddRectFilled(tl2, br2, CalcFillColor(1, active));


		//ImGui::GetWindowDrawList()->AddRectFilled(tl2, br2, CalcFillColor(val, active));
		ImGui::GetWindowDrawList()->AddRect(tl2, br2, active ? gGuiResources.Highlight : gGuiResources.Normal, 0, 0, 2);
	}
	break;

	case BOX_GHOST:
	{
		ImGui::GetWindowDrawList()->AddRectFilled(tl, br, gGuiResources.GhostBG);
	}
	break;
	case BOX_INV:
	{
		ImVec2 br2 = br;
		ImVec2 tl2 = tl;
		tl2.y = tl.y + (val * ParamVerticalBoxHeight) / 0xffff;
		br2.y = tl.y + ParamVerticalBoxHeight / 2;
		ImGui::GetWindowDrawList()->AddRectFilled(tl, br, CalcFillColor(0, active));

		ImGui::GetWindowDrawList()->AddRectFilled(tl, br2, CalcFillColor(1, active));
		ImGui::GetWindowDrawList()->AddRect(tl, br2, active ? gGuiResources.Highlight : gGuiResources.Normal, 0, 0, 2);
	}
	break;
	case BOX_MOD:
	{
		ImVec2 br2 = br;
		float y1 = tl.y + ParamVerticalBoxHeight / 2;
		float y2 = y1 - ((val)* ParamVerticalBoxHeight / 2) / 0x8000;
		ImVec2 tl2 = tl;
		tl2.y = __min(y1, y2);
		br2.y = __max(y1, y2);
		ImGui::GetWindowDrawList()->AddRectFilled(tl, br, CalcFillColor(0, active));
		ImGui::GetWindowDrawList()->AddRectFilled(tl2, br2, CalcFillColor(1, active));
		ImGui::GetWindowDrawList()->AddRect(tl2, br2, active ? gGuiResources.Highlight : gGuiResources.Normal, 0, 0, 2);
	}
	break;
	case BOX_MID:
	{
		ImVec2 br2 = br;
		float y1 = tl.y + ParamVerticalBoxHeight / 2;
		float y2 = tl.y + ((0xffff - val) * ParamVerticalBoxHeight) / 0xffff;
		ImVec2 tl2 = tl;
		tl2.y = __min(y1, y2);
		br2.y = __max(y1, y2);
		ImGui::GetWindowDrawList()->AddRectFilled(tl, br, CalcFillColor(0, active));
		ImGui::GetWindowDrawList()->AddRectFilled(tl2, br2, CalcFillColor(1, active));
		ImGui::GetWindowDrawList()->AddRect(tl2, br2, active ? gGuiResources.Highlight : gGuiResources.Normal, 0, 0, 2);
	}
	break;
	}
	ImGui::GetWindowDrawList()->AddRect(tl, br, active ? gGuiResources.Highlight : gGuiResources.Normal, 0, 0, 2);
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
	case MenuEntry_EncoderSet: Parent->SetupEncoderSet(target); break;
	case MenuEntry_Toggle: gCurrentPreset.ToggleSwitch((SwitchEnum)target); break;

	case MenuEntry_EnvelopeToggle: gCurrentPreset.ToggleModulationSwitch(target, Instance);
	}
}

void sidebutton_t::SketchRightPressed()
{
	Pressed();
}

void sidebutton_t::SetupPosition(int id)
{
	y2 = ButtonHeight(id);
	y = MButtonHeight(id);
	x = 60;
	lx2 = 0;
	Align = align_left;
	if (id > 6)
	{
		lx2 = 1024;
		x = 1024 - 60;
		Align = align_right;
	}
}

void sidebutton_t::Render(bool active, float DT)
{
	if (enabled)
	{
		icons icon = myIcon;



		ImGui::PushFont(gGuiResources.SmallFont);
		int x2 = x;
		if (Align == align_right)
		{
			ImVec2 textsize = ImGui::CalcTextSize(title);
			x2 -= textsize.x;
			//			ImGui::GetWindowDrawList()->AddLine(ImVec2(x+ParamMasterMargin*2, y + ImGui::GetTextLineHeight() / 2), ImVec2(lx2, y2), gGuiResources.Normal, 3);
			ImVec2 A = ImVec2(x + ParamMasterMargin * 2, y + ImGui::GetTextLineHeight() / 2);
			ImVec2 B = A;
			B.x += ParamMasterMargin * 2;
			ImVec2 C = ImVec2(lx2 - ParamMasterMargin * 1, y2);
			ImVec2 D = ImVec2(lx2, y2);
			ImGui::GetWindowDrawList()->AddBezierCurve(A, B, C, D, Dimmed(3, gGuiResources.Normal), 3);
		}
		else
		{
			ImVec2 A = ImVec2(x - ParamMasterMargin * 2, y + ImGui::GetTextLineHeight() / 2);
			ImVec2 B = A;
			B.x -= ParamMasterMargin * 2;
			ImVec2 C = ImVec2(lx2 + ParamMasterMargin * 1, y2);
			ImVec2 D = ImVec2(lx2, y2);
			ImGui::GetWindowDrawList()->AddBezierCurve(A, B, C, D, Dimmed(3, gGuiResources.Normal), 3);
			//ImGui::GetWindowDrawList()->AddLine(ImVec2(x- +ParamMasterMargin * 2, y + ImGui::GetTextLineHeight() / 2), ImVec2(lx2, y2), gGuiResources.Normal, 3);

		}

		int xoff = 0;
		bool drawline = true;
		if (Align == align_right)
		{
			xoff = -15 - ParamMasterMargin;
		}
		else
		{
			xoff = 15 + ParamMasterMargin;
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

		case MenuEntry_Page:
			if (Align == align_right) icon = Icon_GOTOR; else icon = Icon_GOTOL;
			break;
		case MenuEntry_EnvelopeToggle:
			if (gCurrentPreset.GetModulationSwitch(target, Instance)) icon = Icon_ON; else icon = Icon_OFF;
			break;
		case MenuEntry_Toggle:

			int id = gCurrentPreset.GetSwitch((SwitchEnum)target) ? 1 : 0;
			if (id) icon = Icon_ON; else icon = Icon_OFF;
			break;
		}

		if (icon != Icon_NO)
		{
			drawline = false;
			if (Align == align_right)
			{
				ImGui::SetCursorPos(ImVec2(x - 15, y));
				xoff = -15 - ParamMasterMargin;
			}
			else
			{
				ImGui::SetCursorPos(ImVec2(x - 15, y));
				xoff = 15 + ParamMasterMargin;
			}
			ImTextureID icim;
			switch (icon)
			{
			case Icon_ON:icim = gGuiResources.OnOff[1]; break;
			case Icon_OFF:icim = gGuiResources.OnOff[0]; break;

			case Icon_GOTOL:icim = gGuiResources.GotoIconLeft; break;
			case Icon_GOTOR:icim = gGuiResources.GotoIconRight; break;
			}

			if (active)
			{
				ImColor ic(gGuiResources.Highlight);
				ImGui::Image(icim, ImVec2(30, 30), ImVec2(0, 0), ImVec2(1, 1), (ImVec4)ic);
			}
			else
			{
				ImGui::Image(icim, ImVec2(30, 30));

			}

		}

		ImGui::SetCursorPos(ImVec2(x2 + xoff, y));
		if (active)
		{
			ImGui::TextColored(ImColor(gGuiResources.Highlight), title);
		}
		else
		{
			ImGui::Text(title);
		}
		if (drawline)
		{
			if (Align == align_right)
			{
				ImVec2 A = ImVec2(x + ParamMasterMargin * 2, y + ImGui::GetTextLineHeight() / 2);
				ImVec2 B = A;
				B.x += xoff;
				ImGui::GetWindowDrawList()->AddLine(A, B, Dimmed(3, gGuiResources.Normal), 3);
			}
			else
			{
				ImVec2 A = ImVec2(x - ParamMasterMargin * 2, y + ImGui::GetTextLineHeight() / 2);
				ImVec2 B = A;
				B.x += xoff;
				ImGui::GetWindowDrawList()->AddLine(A, B, Dimmed(3, gGuiResources.Normal), 3);
			}
		}

		ImGui::PopFont();
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
	// there is space for 256 presets
	cmd_preset_load((bank << 4) + idx);
}

void LoadPatch(int n)
{
	gPanState.CurrentPatch = n;

	LoadSelectedPreset();
	// do the stuff needed to load bank/patch
	// Load(bank,idx); -> maybe loading anim?

}

extern ImTextureID Raspberry_LoadTexture(const char *filename);
static bool init = false;

void PrintFontSpec(ImFont*F, const char* name)
{
	ImGui::PushFont(F);
	float H = ImGui::GetTextLineHeight();
	auto S = ImGui::CalcTextSize("the quick brown fox jumps over the lazy dog THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG");
	printf("%s: height = %f, box = %f,%f\n", name, H, S.x, S.y);
	ImGui::PopFont();

}

void CheckFont(const char*name, ImFont*F)
{
	if (F) printf("%s loaded correctly\n", name); else
	{
		printf("failed to load %s!!!\n");
	}
}

void FinalPan_LoadResources()
{
	BuildModulationTargetList();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	gGuiResources.encoderbarmargin = 10;
	gGuiResources.encoderheight = 600;
	gGuiResources.PageTime = 0;
	gGuiResources.Highlight = IM_COL32(235, 200, 28, 255);
	gGuiResources.Normal = IM_COL32(200, 200, 200, 255);
	gGuiResources.BGColor = IM_COL32(0, 58, 66, 255);
	gGuiResources.ModalBGColor = IM_COL32(0, 58, 66, 140);
	gGuiResources.GhostBG = IM_COL32(0, 0, 0, 200);
	//res.BGColor = IM_COL32(0, 0, 0, 255);
	gGuiResources.FillColor = IM_COL32(0, 137, 127, 255);
	gGuiResources.OnOff[0] = Raspberry_LoadTexture("PAN__OFF.png");
	gGuiResources.OnOff[1] = Raspberry_LoadTexture("PAN__ON.png");
	gGuiResources.OnOff[2] = Raspberry_LoadTexture("UI_ONOFF_OFF_HI.png");
	gGuiResources.OnOff[3] = Raspberry_LoadTexture("UI_ONOFF_ON_HI.png");

	gGuiResources.LogoScreen = Raspberry_LoadTexture("PAN__LOGO.png");
	gGuiResources.RootBG = Raspberry_LoadTexture("PAN__MAINBG.png");
	gGuiResources.MainBG = Raspberry_LoadTexture("PAN__MAINBG.png");
	gGuiResources.TestBG = Raspberry_LoadTexture("PAN_TEST.png");

	gGuiResources.GotoIconLeft = Raspberry_LoadTexture("PAN__GOTOL.png");
	gGuiResources.GotoIconRight = Raspberry_LoadTexture("PAN__GOTOR.png");

	gGuiResources.VCF2 = Raspberry_LoadTexture("PAN__FILT_BG.png");
	gGuiResources.F[Filt_LP] = Raspberry_LoadTexture("PAN__FILT_LP.png");
	gGuiResources.F[Filt_HP] = Raspberry_LoadTexture("PAN__FILT_HP.png");
	gGuiResources.F[Filt_BP] = Raspberry_LoadTexture("PAN__FILT_BP.png");
	gGuiResources.F[Filt_BR] = Raspberry_LoadTexture("PAN__FILT_NOTCH.png");
	gGuiResources.Fs[Filt_Par] = Raspberry_LoadTexture("PAN__FILT_PAR.png");
	gGuiResources.Fs[Filt_Ser] = Raspberry_LoadTexture("PAN__FILT_SER.png");

	gGuiResources.LeftIndicator = Raspberry_LoadTexture("UI_LEFT.png");
	gGuiResources.RightIndicator = Raspberry_LoadTexture("UI_RIGHT.png");
	gGuiResources.TinyFont = io.Fonts->AddFontFromFileTTF("CORBEL.TTF", 20.0f);
	gGuiResources.SmallFont = io.Fonts->AddFontFromFileTTF("CORBEL.TTF", 30.0f);
	gGuiResources.MediumFont = io.Fonts->AddFontFromFileTTF("CORBEL.TTF", 38.0f);
	//gGuiResources.BigFont = io.Fonts->AddFontFromFileTTF("Fontfabric - Panton ExtraBold.otf", 44.0f);

	gGuiResources.referencelines = false;
	gGuiResources.testimage = false;
	gGuiResources.BigFont = io.Fonts->AddFontFromFileTTF("Petronius-Roman.ttf", 38.0f);
	init = true;

	CheckFont("BigFont - PetroniusRoman", gGuiResources.BigFont);
	CheckFont("TinyFont - Panton", gGuiResources.TinyFont);
	CheckFont("SmallFont - Panton", gGuiResources.MediumFont);
	CheckFont("MediunFont - Panton", gGuiResources.SmallFont);

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
	Waiting = false;
}

void Gui::Init()
{
	Waiting = false;
	BuildScreens();
	CurrentScreen = SCREEN_LOGO;
}

void Gui::SketchLeftPress()
{
	if (Waiting) return;

	CS()->SketchLeftPress();
}

void Gui::SketchRightPress()
{
	if (Waiting) return;

	CS()->SketchRightPress();
}

void Gui::EncoderPress(FinalEncoderEnum button)
{

	if (Waiting) return;
	CS()->EncoderPress(button);
}

void Gui::SketchLeft(int delta)
{
	if (Waiting) return;
	CS()->SketchLeft(delta);
}

void Gui::SketchRight(int delta)
{
	if (Waiting) return;
	CS()->SketchRight(delta);
}


void Gui::Encoder(FinalEncoderEnum button, int delta)
{
	if (Waiting) return;
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

void AddPitch(int add)
{
	//gCurrentPreset.TweakParameter(Output_MASTER_PITCH2, add,1);
	int oct = ((int)gCurrentPreset.paramvalue[Output_MASTER_PITCH2] - 0x8000) / (0x100 * 12);
	oct += add;
	oct *= 0x100 * 12;
	oct += 0x8000;
	gCurrentPreset.paramvalue[Output_MASTER_PITCH2] = oct;
}

void Gui::ButtonPressed(FinalLedButtonEnum Button)
{
	if (Waiting) return;
	// insert what to do if selecting modulation source/target here? 
	switch (Button)
	{

	case ledbutton_OctDownLeft:
	case ledbutton_OctDownRight:
		AddPitch(-1);
		break;

	case ledbutton_OctUpLeft:
	case ledbutton_OctUpRight:
		AddPitch(1);
		break;

	case ledbutton_PortamentoLeft:
	case ledbutton_PortamentoRight:
		GotoPage(SCREEN_PORTAMENTO); break;
	case ledbutton_BankLeft: GotoPage(SCREEN_SELECTBANKL); break;
	case ledbutton_BankRight: GotoPage(SCREEN_SELECTBANKR); break;
	case ledbutton_ArpEdit: GotoPage(SCREEN_ARP); break;
	case ledbutton_BX:  GotoPage(SCREEN_X); break;
	case ledbutton_BY:  GotoPage(SCREEN_Y); break;
	case ledbutton_BZ:  GotoPage(SCREEN_Z); break;
	case ledbutton_BVelocity: GotoPage(SCREEN_VELOCITY); break;
	case ledbutton_BEnv: GotoPage(SCREEN_ENVELOPE); break;
	case ledbutton_BLFO: GotoPage(SCREEN_LFO); break;
	case ledbutton_BCV: GotoPage(SCREEN_KEYBOARD); break;
	case ledbutton_BTouch: GotoPage(SCREEN_TOUCH); break;
	case ledbutton_BHome: GotoPage(SCREEN_HOME); break;
	case ledbutton_LMod: GotoPage(SCREEN_LMOD); break;
	case ledbutton_RMod: GotoPage(SCREEN_RMOD); break;
	case ledbutton_LSus: GotoPage(SCREEN_LSUS); break;
	case ledbutton_RSus: GotoPage(SCREEN_RSUS); break;
	case ledbutton_LUna: GotoPage(SCREEN_LUNA); break;
	case ledbutton_RUna: GotoPage(SCREEN_RUNA); break;

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


void RenderLettersInABox(int x, int y, bool active, const char *text, int w, int h, bool notghosted)
{
	ImVec2 tl(x, y);
	ImVec2 br(x + w, y + h);

	ImGui::GetWindowDrawList()->AddRect(tl, br, active ? gGuiResources.Highlight : Dimmed(notghosted ? 1 : 3, gGuiResources.Normal), 0, 0, 2);

	auto s = ImGui::CalcTextSize(text);

	ImGui::SetCursorPos(ImVec2(x + LetterBoxW / 2 - s.x / 2, y + LetterBoxH / 2 - s.y / 2));
	ImGui::TextColored((ImVec4)(ImColor)(Dimmed(notghosted ? 1 : 3, gGuiResources.Normal)), text);

}

ImU32 Dimmed(int dim, ImU32 col)
{
	ImColor C(col);
	ImVec4 cc = (ImVec4)C;
	cc.x /= (1.0 + dim * 0.5);
	cc.y /= (1.0 + dim * 0.5);
	cc.z /= (1.0 + dim * 0.5);


	return ImColor(cc);
};

class LetterControl : public _control_t
{
public:
	int id;
	int x;
	int y;
	virtual void SketchRightDelta(int delta)
	{
		Current = GetDeltaChar(Current, delta);// ((Current + delta - 32 + 96) % 96) + 32;
	}

	char GetDeltaChar(char base, int delta)
	{
		return ((base + delta - 32 + 96) % 96) + 32;
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
	
	void Render(bool active, float DT);
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
		case MenuAction_Backspace:
		{
			int N = GetActiveLetter();
			if (N < 0)
			{
				N = Letters.size() - 1;
				
			}
			SetActiveLetter(N - 1);
			Letters[N]->Current = ' ';
			gCurrentPreset.Name[N] = ' ';
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
			gCurrentPreset.Name[N] = ' ';
		}
		break;
		}
	}
	int LastActiveLetter;
	void SetActiveLetter(int N)
	{
		if (N >= Letters.size()) N = Letters.size() - 1;
		_control_t *c = Letters[N];
		for (int i = 0; i < ControlsInOrder.size(); i++)
		{
			if (ControlsInOrder[i] == c)
			{
				ActiveControl =  i;
				return;
			}
		}
	}
	int GetActiveLetter()
	{
		return LastActiveLetter;
	}

	PresetScreen()
	{

		for (int i = 0; i < PRESET_NAME_LENGTH - 1; i++)
		{
			AddLetterControl(1024 / 2 - (LetterBoxW + 5)*((PRESET_NAME_LENGTH - 1.5) / 2) + i * (LetterBoxW + 5), 300 - LetterBoxH / 2, i);
		}

		EnableButton(10, "Cancel", MenuEntry_Action, MenuAction_No);

		EnableButton(5, "Space", MenuEntry_Action, MenuAction_SpaceBar);
		EnableButton(6, "Backspace", MenuEntry_Action, MenuAction_Backspace);

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
		L->Parent = this;
		Letters.push_back(L);
		ControlsInOrder.push_back(L);
	}
};

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
class BankSelectScreen : public _screensetup_t
{
public:

	virtual void Action(int action)
	{

		if (Side == Left) gPanState.BankLeft = action; else gPanState.BankRight = action;
		//LoadSelectedPreset();
		Activate();
	}
	LeftRight Side;
	int *mybank;
	int df;
	void SetLeftRight(LeftRight lr)
	{

	}

	BankList* list;
	BankSelectScreen()
	{
		mybank = &df;
		list = new BankList(400, 150, mybank, "");
		ControlsInOrder.push_back(list);
	}
	virtual void Activate()
	{
		int *CurrentBank = &gPanState.BankLeft;
		list->bankid = CurrentBank;
		if (Side == Left)
		{
			SetTitle("Select Left Bank");
		}
		else
		{
			CurrentBank = &gPanState.BankRight;
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


class SavePresetScreen : public _screensetup_t
{
public:
	virtual void PatchButton(FinalLedButtonEnum b)
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
	virtual void Action(int action)
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

	LeftRight Side;
	int *mybank;
	int df;
	void SetLeftRight(LeftRight lr)
	{

	}

	BankList* list;
	SavePresetScreen()
	{
		
		list = new BankList(400, 150, &df, "");
		ControlsInOrder.push_back(list);
		EnableButton(7, "Cancel", MenuEntry_Action, MenuAction_Home);
		SetTitle("Save preset");
		AddText(140, 50, "select bank and press a preset button to save");




	}
	virtual void Activate()
	{
		int *CurrentBank = &gPanState.BankLeft;
		list->bankid = CurrentBank;
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

	void SetNames()
	{
		list->bankid = &df;
		EnableButton(1, (df == 0) ? "A (current)" : "A", MenuEntry_Action, MenuAction_BankA);
		EnableButton(2, (df == 1) ? "B (current)" : "B", MenuEntry_Action, MenuAction_BankB);
		EnableButton(3, (df == 2) ? "C (current)" : "C", MenuEntry_Action, MenuAction_BankC);
		EnableButton(4, (df == 3) ? "D (current)" : "D", MenuEntry_Action, MenuAction_BankD);
		EnableButton(5, (df == 4) ? "E (current)" : "E", MenuEntry_Action, MenuAction_BankE);
		EnableButton(6, (df == 5) ? "F (current)" : "F", MenuEntry_Action, MenuAction_BankF);
		EnableButton(8, (df == 6) ? "G (current)" : "G", MenuEntry_Action, MenuAction_BankG);
		EnableButton(9, (df == 7) ? "H (current)" : "H", MenuEntry_Action, MenuAction_BankH);
		EnableButton(10, (df == 8) ? "I (current)" : "I", MenuEntry_Action, MenuAction_BankI);
		EnableButton(11, (df == 9) ? "J (current)" : "J", MenuEntry_Action, MenuAction_BankJ);
		EnableButton(12, (df == 10) ? "K (current)" : "K", MenuEntry_Action, MenuAction_BankK);
		EnableButton(13, (df == 11) ? "L (current)" : "L", MenuEntry_Action, MenuAction_BankL);
	}
};

class ImageScreen : public _screensetup_t
{
public:
	ImTextureID theImg;
	ImageScreen(ImTextureID img)
	{
		theImg = img;
	}
	virtual void Render(bool active, float dt)
	{
		_screensetup_t::Render(active, dt);
		ImGui::SetCursorPos(ImVec2(0, 0));
		ImGui::Image(theImg, ImVec2(1024, 600));
	}
};

void EncoderLineDisplay::Render(bool active, float dt)
{
	float midy = (fromY + 500) / 2;



	//	ImGui::GetWindowDrawList()->AddLine(ImVec2(fromX, fromY), ImVec2(fromX, midy), gGuiResources.Normal, 3);
	float minx = 1024;
	float maxx = 0;
	for (int i = 0; i < this->toEncoders.size(); i++)
	{
		int x = GetEncoderX(toEncoders[i]);
		if (x > maxx) maxx = x;
		if (x < minx) minx = x;
		//int tox =
		ImGui::GetWindowDrawList()->AddBezierCurve(ImVec2(fromX, fromY), ImVec2(fromX, fromY + 70), ImVec2(x, 430), ImVec2(x, 600 - ParamVerticalBoxHeight - ParamMasterMargin * 2), gGuiResources.Normal, 3);


		//ImGui::GetWindowDrawList()->AddLine(ImVec2(x, midy), ImVec2(x, 500), gGuiResources.Normal, 3);

	}
	//ImGui::GetWindowDrawList()->AddLine(ImVec2(minx, midy), ImVec2(maxx, midy), gGuiResources.Normal, 3);

}

void _screensetup_t::AddVCONextPrev()
{
	EnableButton(6, "previous", MenuEntry_Action, MenuAction_PrevVCO);
	buttons[6].myIcon = Icon_GOTOL;
	EnableButton(13, "next", MenuEntry_Action, MenuAction_NextVCO);
	buttons[13].myIcon = Icon_GOTOR;
}

void _screensetup_t::AddVCF2NextPrev()
{
	EnableButton(6, "previous", MenuEntry_Action, MenuAction_PrevVCF2);
	buttons[6].myIcon = Icon_GOTOL;
	EnableButton(13, "next", MenuEntry_Action, MenuAction_NextVCF2);
	buttons[13].myIcon = Icon_GOTOR;
}


char EffectChipStrings[8][4][24] = {

	{ "Chorus & Reverb" ,"Reverb mix","Chorus rate","Chorus mix" },
{ "Flanger & Reverb","Reverb mix","Flange rate","Flange mix" },
{ "Tremolo & Reverb","Reverb mix","Tremolo rate","Tremolo mix" },
{ "Pitch shift","Pitch +/- 4 semitones","","" },
{ "Pitch & echo","Pitch shift","Echo delay","Echo mix" },
{ "Test","-","-","-" },
{ "Reverb 1","Reverb time","HF filter","LF filter" },
{ "Reverb 2","Reverb time","HF filter","LF filter" }
};


int DecodeCurrentEffect()
{
	int a = gCurrentPreset.GetSwitch(Switch_SELEF1) ? 0 : 1;
	int b = gCurrentPreset.GetSwitch(Switch_SELEF2) ? 0 : 2;
	int c = gCurrentPreset.GetSwitch(Switch_SELEF3) ? 0 : 4;

	return a + b + c;
}

void SetEffect(int effect)
{
	bool a = (effect & 1) == 0;
	bool b = (effect & 2) == 0;
	bool c = (effect & 4) == 0;
	gCurrentPreset.PutSwitch(Switch_SELEF1, a);
	gCurrentPreset.PutSwitch(Switch_SELEF2, b);
	gCurrentPreset.PutSwitch(Switch_SELEF3, c);
}



class EffectList : public _control_t
{
public:

	EffectList() {}

	virtual void SketchRight(int delta)
	{
		while (delta < 0) {
			delta++; Action(MenuAction_FX_Prev);
		};
		while (delta > 0) {
			delta--; Action(MenuAction_FX_Next);
		};
	}

	virtual void Render(bool active, float dt)
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

};


void Gui::BuildScreens()
{
	for (int i = 0; i < SCREENS_COUNT; i++) Screens[i] = 0;



	Screens[SCREEN_PRESETNAME] = new PresetScreen();
	auto BL = new BankSelectScreen();
	BL->Side = Left;
	auto BR = new BankSelectScreen();
	BR->Side = Right;
	Screens[SCREEN_SELECTBANKL] = BL;
	Screens[SCREEN_SELECTBANKR] = BR;

	auto SaveScreen = new SavePresetScreen();
	Screens[SCREEN_PRESETSAVE] = SaveScreen;

	Screens[SCREEN_TEST] = new ImageScreen(gGuiResources.TestBG);
	Screens[SCREEN_TEST]->AddText(10, 30, "10,30 - tadaa", align_left, font_small);
	Screens[SCREEN_TEST]->AddText(30, 60, "10,60 - tadaa", align_left, font_small);
	for (int i = 0; i < 100; i += 10)
	{
		Screens[SCREEN_TEST]->AddText(50 + 4 * i, 600 - i, "Hmm", align_left, font_small);
	}

	Screens[SCREEN_LOGO] = new ImageScreen(gGuiResources.LogoScreen);
	BL->LedButtonsThatOpenThisScreen.push_back(ledbutton_BankLeft);
	BR->LedButtonsThatOpenThisScreen.push_back(ledbutton_BankRight);

	Screens[SCREEN_X] = new ModSourceScreen(SCREEN_X);
	Screens[SCREEN_X]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BX);
	Screens[SCREEN_Y] = new ModSourceScreen(SCREEN_Y);
	Screens[SCREEN_Y]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BY);
	Screens[SCREEN_Z] = new ModSourceScreen(SCREEN_Z);
	Screens[SCREEN_Z]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BZ);
	Screens[SCREEN_TOUCH] = new ModSourceScreen(SCREEN_TOUCH);
	Screens[SCREEN_TOUCH]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BTouch);
	Screens[SCREEN_LMOD] = new ModSourceScreen(SCREEN_LMOD);
	Screens[SCREEN_LMOD]->LedButtonsThatOpenThisScreen.push_back(ledbutton_LMod);
	Screens[SCREEN_RMOD] = new ModSourceScreen(SCREEN_RMOD);
	Screens[SCREEN_RMOD]->LedButtonsThatOpenThisScreen.push_back(ledbutton_RMod);
	Screens[SCREEN_LSUS] = new ModSourceScreen(SCREEN_LSUS);
	Screens[SCREEN_LSUS]->LedButtonsThatOpenThisScreen.push_back(ledbutton_LSus);
	Screens[SCREEN_RSUS] = new ModSourceScreen(SCREEN_RSUS);
	Screens[SCREEN_RSUS]->LedButtonsThatOpenThisScreen.push_back(ledbutton_RSus);
	Screens[SCREEN_LUNA] = new ModSourceScreen(SCREEN_LUNA);
	Screens[SCREEN_LUNA]->LedButtonsThatOpenThisScreen.push_back(ledbutton_LUna);
	Screens[SCREEN_RUNA] = new ModSourceScreen(SCREEN_RUNA);
	Screens[SCREEN_RUNA]->LedButtonsThatOpenThisScreen.push_back(ledbutton_RUna);

	Screens[SCREEN_VELOCITY] = new ModSourceScreen(SCREEN_VELOCITY);
	Screens[SCREEN_VELOCITY]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BVelocity);
	Screens[SCREEN_KEYBOARD] = new ModSourceScreen(SCREEN_KEYBOARD);
	Screens[SCREEN_KEYBOARD]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BCV);
	Screens[SCREEN_LFO] = new ModSourceScreen(SCREEN_LFO);
	Screens[SCREEN_LFO]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BLFO);

	Screens[SCREEN_ENVELOPE] = new ModSourceScreen(SCREEN_ENVELOPE);
	Screens[SCREEN_ENVELOPE]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BEnv);
	Screens[SCREEN_VCF2_structure] = new VCF2StructureScreen();

	Screens[SCREEN_HOME] = new HomeScreen();

	for (int i = 0; i < SCREENS_COUNT; i++)
	{
		if (Screens[i] == 0) Screens[i] = new _screensetup_t();
	}

	Screens[SCREEN_HOME]->SetTitle("Syntonovo Pan");
	Screens[SCREEN_HOME]->AddText(512, 50, "Current preset: ", align_right);
	Screens[SCREEN_HOME]->AddDynamicText(512, 50, gCurrentPreset.Name, PRESET_NAME_LENGTH);
	Screens[SCREEN_HOME]->EncodersThatOpenThisScreen.push_back(encoder_SketchLeft);
	Screens[SCREEN_LFO]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BHome);


	Screens[SCREEN_LOADING]->SetTitle("Loading...");

	Screens[SCREEN_EFFECTS]->ControlsInOrder.push_back(new EffectList());

	Screens[SCREEN_EFFECTS]->EnableButton(2, "Previous", MenuEntry_Action, MenuAction_FX_Prev);
	Screens[SCREEN_EFFECTS]->EnableButton(9, "Next", MenuEntry_Action, MenuAction_FX_Next);

	Screens[SCREEN_VCF2a]->SetTitle("VCF2 A");
	Screens[SCREEN_VCF2a]->EnableButton(12, "to VCF2 Mix", MenuEntry_Page, SCREEN_VCF2MIX);
	Screens[SCREEN_VCF2b]->SetTitle("VCF2 B");
	Screens[SCREEN_VCF2b]->EnableButton(12, "to VCF2 Mix", MenuEntry_Page, SCREEN_VCF2MIX);
	Screens[SCREEN_VCF2c]->SetTitle("VCF2 C");
	Screens[SCREEN_VCF2c]->EnableButton(12, "to VCF2 Mix", MenuEntry_Page, SCREEN_VCF2MIX);
	Screens[SCREEN_VCF2d]->SetTitle("VCF2 D");
	Screens[SCREEN_VCF2d]->EnableButton(12, "to VCF2 Mix", MenuEntry_Page, SCREEN_VCF2MIX);

	Screens[SCREEN_VCF2_structure]->SetTitle("VCF2 routing");
	Screens[SCREEN_VCF1]->EnableButton(12, "to VCF1 Mix", MenuEntry_Page, SCREEN_VCF1MIX);


	Screens[SCREEN_VCO4]->SetTitle("VCO 5");
	Screens[SCREEN_VCO5]->SetTitle("VCO 6");
	Screens[SCREEN_VCO6]->SetTitle("VCO 7");
	Screens[SCREEN_VCO7]->SetTitle("VCO 8");
	Screens[SCREEN_VCO4]->EncodersThatOpenThisScreen.push_back(encoder_VCO4);
	Screens[SCREEN_VCO5]->EncodersThatOpenThisScreen.push_back(encoder_VCO5);
	Screens[SCREEN_VCO6]->EncodersThatOpenThisScreen.push_back(encoder_VCO6);
	Screens[SCREEN_VCO7]->EncodersThatOpenThisScreen.push_back(encoder_VCO7);


	Screens[SCREEN_SYSTEM]->SetTitle("System Settings");
	Screens[SCREEN_SYSTEM]->EnableAvailableButton("Recalibrate Oscillators", MenuEntry_Action, MenuAction_CalibrateOscillators);
	Screens[SCREEN_SYSTEM]->EnableAvailableButton("Recalibrate Pads", MenuEntry_Action, MenuAction_CalibratePads);
	Screens[SCREEN_SYSTEM]->EnableButton(7, "Done", MenuEntry_Action, MenuAction_Home);
	Screens[SCREEN_SYSTEM]->EnableAvailableButton("Polymode bit 0", MenuEntry_Toggle, Switch_POLYMODE1);
	Screens[SCREEN_SYSTEM]->EnableAvailableButton("Polymode bit 1", MenuEntry_Toggle, Switch_POLYMODE2);
	Screens[SCREEN_SYSTEM]->EnableAvailableButton("KeyPrio bit 0", MenuEntry_Toggle, Switch_KEYPRIO1);
	Screens[SCREEN_SYSTEM]->EnableAvailableButton("KeyPrio bit 1", MenuEntry_Toggle, Switch_KEYPRIO2);

	Screens[SCREEN_SYSTEM]->EnableAvailableButton("Headphone enable", MenuEntry_Toggle, Switch_SELVCF1MOST);

	Screens[SCREEN_HOME]->EnableButton(7, "Change name", MenuEntry_Page, SCREEN_PRESETNAME);//(512, 40, "Some Sound");
	Screens[SCREEN_HOME]->EnableButton(8, "Save preset", MenuEntry_Page, SCREEN_PRESETSAVE);//(512, 40, "Some Sound");
	Screens[SCREEN_HOME]->EnableButton(9, "Revert", MenuEntry_Action, MenuAction_Revert);
	Screens[SCREEN_HOME]->EnableButton(10, "System", MenuEntry_Page, SCREEN_SYSTEM);
	Screens[SCREEN_HOME]->EnableButton(12, "Colors", MenuEntry_Page, SCREEN_COLORS);
	Screens[SCREEN_HOME]->EnableButton(5, "Reference Lines", MenuEntry_Action, MenuAction_EnableReferenceLines);
	Screens[SCREEN_HOME]->EnableButton(6, "Test-image", MenuEntry_Action, MenuAction_EnableTestImage);

	Screens[SCREEN_PRESETNAME]->SetTitle("Edit Name/Category");

	Screens[SCREEN_COLORS]->SetTitle("Colors");

	Screens[SCREEN_PORTAMENTO]->SetTitle("Portamento");

	Screens[SCREEN_COLORS]->AddLedControl("Low", GetEncoderX(1) - 20, 300, Led_Low);
	Screens[SCREEN_COLORS]->AddLedControl("High", GetEncoderX(5) - 20, 300, Led_High);
	Screens[SCREEN_COLORS]->AddLedControl("Active", GetEncoderX(9) - 20, 300, Led_Active);
	Screens[SCREEN_COLORS]->EnableButton(7, "Done", MenuEntry_Action, MenuAction_Home);




	auto Lines1 = new EncoderLineDisplay();
	Lines1->fromX = GetEncoderX(1);
	Lines1->fromY = 350;
	Lines1->toEncoders.push_back(0);
	Lines1->toEncoders.push_back(1);
	Lines1->toEncoders.push_back(2);
	Lines1->AddTo(Screens[SCREEN_COLORS]);

	auto Lines2 = new EncoderLineDisplay();
	Lines2->fromX = GetEncoderX(5);
	Lines2->fromY = 350;
	Lines2->toEncoders.push_back(4);
	Lines2->toEncoders.push_back(5);
	Lines2->toEncoders.push_back(6);
	Lines2->AddTo(Screens[SCREEN_COLORS]);

	auto Lines3 = new EncoderLineDisplay();
	Lines3->fromX = GetEncoderX(9);
	Lines3->fromY = 350;
	Lines3->toEncoders.push_back(8);
	Lines3->toEncoders.push_back(9);
	Lines3->toEncoders.push_back(10);

	Lines3->AddTo(Screens[SCREEN_COLORS]);


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

	Screens[SCREEN_LMOD]->SetTitle("Left Mod");
	Screens[SCREEN_RMOD]->SetTitle("Right Mod");
	Screens[SCREEN_LSUS]->SetTitle("Left Sustain");
	Screens[SCREEN_RSUS]->SetTitle("Right Sustain");
	Screens[SCREEN_LUNA]->SetTitle("Left Una Corda");
	Screens[SCREEN_RUNA]->SetTitle("Right Una Corda");

	Screens[SCREEN_ENVELOPE]->SetTitle("Envelope");
	Screens[SCREEN_ENVELOPE]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BEnv);

	Screens[SCREEN_LFO]->SetTitle("LFO");
	Screens[SCREEN_LFO]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BLFO);

	Screens[SCREEN_ARP]->SetTitle("Arpeggiator");
	Screens[SCREEN_ARP]->LedButtonsThatOpenThisScreen.push_back(ledbutton_ArpEdit);

	Screens[SCREEN_VCO1]->AddVCONextPrev();
	Screens[SCREEN_VCO2]->AddVCONextPrev();
	Screens[SCREEN_VCO3]->AddVCONextPrev();
	Screens[SCREEN_VCO4]->AddVCONextPrev();
	Screens[SCREEN_VCO5]->AddVCONextPrev();
	Screens[SCREEN_VCO6]->AddVCONextPrev();
	Screens[SCREEN_VCO7]->AddVCONextPrev();
	Screens[SCREEN_VCO8]->AddVCONextPrev();

	Screens[SCREEN_VCF2a]->AddVCF2NextPrev();
	Screens[SCREEN_VCF2b]->AddVCF2NextPrev();
	Screens[SCREEN_VCF2c]->AddVCF2NextPrev();
	Screens[SCREEN_VCF2d]->AddVCF2NextPrev();


	Screens[SCREEN_VCO1]->SetTitle("VCO1");
	Screens[SCREEN_VCO1]->EncodersThatOpenThisScreen.push_back(encoder_VCO1);

	Screens[SCREEN_VCO2]->SetTitle("VCO2");
	Screens[SCREEN_VCO2]->EncodersThatOpenThisScreen.push_back(encoder_VCO2);

	Screens[SCREEN_VCO3]->SetTitle("VCO3");
	Screens[SCREEN_VCO3]->EncodersThatOpenThisScreen.push_back(encoder_VCO3);

	Screens[SCREEN_VCF1]->SetTitle("VCF1");
	Screens[SCREEN_VCF1]->EncodersThatOpenThisScreen.push_back(encoder_VCF1Freq);

	Screens[SCREEN_VCF2a]->SetTitle("VCF2: A");
	Screens[SCREEN_VCF2a]->EncodersThatOpenThisScreen.push_back(encoder_VCF2a);
	Screens[SCREEN_VCF2a]->EnableAvailableButton("Routing", MenuEntry_Action, MenuAction_OpenVCF2Structure);

	Screens[SCREEN_VCF2b]->SetTitle("VCF2: B");
	Screens[SCREEN_VCF2b]->EncodersThatOpenThisScreen.push_back(encoder_VCF2b);
	Screens[SCREEN_VCF2b]->EnableAvailableButton("Routing", MenuEntry_Action, MenuAction_OpenVCF2Structure);

	Screens[SCREEN_VCF2c]->SetTitle("VCF2: C");
	Screens[SCREEN_VCF2c]->EncodersThatOpenThisScreen.push_back(encoder_VCF2c);
	Screens[SCREEN_VCF2c]->EnableAvailableButton("Routing", MenuEntry_Action, MenuAction_OpenVCF2Structure);

	Screens[SCREEN_VCF2d]->SetTitle("VCF2: D");
	Screens[SCREEN_VCF2d]->EncodersThatOpenThisScreen.push_back(encoder_VCF2d);
	Screens[SCREEN_VCF2d]->EnableAvailableButton("Routing", MenuEntry_Action, MenuAction_OpenVCF2Structure);


	Screens[SCREEN_VCF1MIX]->EnableAvailableButton("Effects", MenuEntry_Action, MenuAction_OpenEffects);
	Screens[SCREEN_VCF2MIX]->EnableAvailableButton("Effects", MenuEntry_Action, MenuAction_OpenEffects);
	Screens[SCREEN_CLEANMIX]->EnableAvailableButton("Effects", MenuEntry_Action, MenuAction_OpenEffects);
	Screens[SCREEN_MASTER]->EnableAvailableButton("Effects", MenuEntry_Action, MenuAction_OpenEffects);

	Screens[SCREEN_EFFECTS]->EnableAvailableButton("Done", MenuEntry_Action, MenuAction_CloseParentModal);




	Screens[SCREEN_TEST]->SetTitle("Test scherm");
	//Screens[SCREEN_TEST]->EnableButton(2, "Button1!");
	//Screens[SCREEN_TEST]->EnableButton(5, "Button2!");
	//Screens[SCREEN_TEST]->EnableButton(11, "Button3!");



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
		Screens[i]->SetupEncoderSet(0);
	}
}


void Gui::Render(bool active, float dt)
{
	if (Waiting)
	{
		Screens[SCREEN_LOADING]->Render(active, dt);
		return;
	}
	Screens[CurrentScreen]->Render(active, dt);

	if (gGuiResources.referencelines)
	{
		for (int i = 0; i < 11; i++)
		{
			int X = GetEncoderX(i);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(X, 0), ImVec2(X, 1024), gGuiResources.Normal, 2);
		}
	}
}

void Gui::GotoPage(Screens_t s)
{
	if (CurrentScreen == s)
	{
		CS()->RepeatGoto();
	}
	else
	{
		CS()->Deactivate();
		CurrentScreen = s;
		CS()->Activate();
	}
}

void hsv2rgb(uint16_t h, uint16_t s, uint16_t v, uint16_t *r, uint16_t *g, uint16_t *b)
{
	float rr, gg, bb;
	ImGui::ColorConvertHSVtoRGB((h) / 65535.0, s / 65535.0f, v / 65535.0f, rr, gg, bb);
	*r = (int)(rr * 65535.0f);
	*g = (int)(gg * 65535.0f);
	*b = (int)(bb * 65535.0f);
}

void Gui::SetupLeds()
{
	if (Waiting) return;

	gPanState.SetLed(Led_Active, gCurrentPreset.active);
	gPanState.SetLed(Led_High, gCurrentPreset.high);
	gPanState.SetLed(Led_Low, gCurrentPreset.low);

	gGuiResources.Highlight = ImColor(gPanState.active_led_r >> 8, gPanState.active_led_g >> 8, gPanState.active_led_b >> 8);

	gPanState.ClearLeds();
	CS()->SetupLeds();
}

void FinalPan_PushStyle()
{
	ImGui::PushFont(gGuiResources.SmallFont);
	ImGui::PushStyleColor(ImGuiCol_Text, gGuiResources.Normal);
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
	case encoder_VCO4: return SCREEN_VCO4;
	case encoder_VCO5: return SCREEN_VCO5;
	case encoder_VCO6: return SCREEN_VCO6;
	case encoder_VCO7: return SCREEN_VCO7;
	case encoder_VCO8: return SCREEN_VCO8;

		// 8 on right side
	case encoder_VCF1Freq: return SCREEN_VCF1;
	case encoder_VCF2a: return SCREEN_VCF2a;
	case encoder_VCF2b: return SCREEN_VCF2b;
	case encoder_VCF2c: return SCREEN_VCF2c;
	case encoder_VCF2d: return SCREEN_VCF2d;
	case encoder_Cleanmix: return SCREEN_CLEANMIX;
	case encoder_VCF2Mix: return SCREEN_VCF2MIX;
	case encoder_VCF1Mix: return SCREEN_VCF1MIX;


	}
	return SCREEN_HOME;
}
bool dumpspec = true;

void RenderMain(float DT)
{
	if (!init)
	{

		FinalPan_LoadResources();
	}
	FinalPan_PushStyle();
	ImVec2 pos = ImGui::GetCursorPos();
	pos = ImGui::GetCursorScreenPos();
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(1024 + pos.x, 600 + pos.y), gGuiResources.BGColor);

	gGui.Render(true, DT);

	if (dumpspec)
	{
		dumpspec = false;
		PrintFontSpec(gGuiResources.TinyFont, "Tiny");
		PrintFontSpec(gGuiResources.SmallFont, "Small");
		PrintFontSpec(gGuiResources.MediumFont, "Medium");
		PrintFontSpec(gGuiResources.BigFont, "Big");


	}


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

	RenderMain(DT);

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

void LedEncoderButtonLeft(FinalEncoderEnum Button, int delta)
{
	switch (Button)
	{
	case encoder_SketchLeft: gGui.SketchLeft(-1); break;
	case encoder_SketchRight: gGui.SketchRight(-1); break;
	default: gGui.Encoder(Button, -delta); break;
	}
}

void LedEncoderButtonRight(FinalEncoderEnum Button, int delta)
{
	switch (Button)
	{
	case encoder_SketchLeft: gGui.SketchLeft(1); break;
	case encoder_SketchRight: gGui.SketchRight(1); break;
	default: gGui.Encoder(Button, delta); break;
	}
}

void FinalPan_Pause(bool pause)
{
	gGui.PauseAll(pause);
}
