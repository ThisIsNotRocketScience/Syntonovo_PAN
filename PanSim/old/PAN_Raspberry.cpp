#include "../libs/imgui-master/imgui.h"
#ifdef WIN32
#include <GL/gl3w.h> 
#else

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#endif
#include "../libs/lodepng-master/lodepng.h"
#include "FinalPanEnums.h"


extern ImTextureID Raspberry_LoadTexture(const char *filename);

#include "PanHeader.h"

#define MENU(id,button,name) static int const MenuItemCount_##id = 0
#define ENTRY(name, type, param) + 1
#define CUSTOMENTRY(name, type, param) + 1
#define ENDMENU() ;
#include "PanUiMap.h"
#undef ENDMENU
#undef ENTRY
#undef MENU
#undef CUSTOMENTRY

#ifndef __min
#define __min(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef __max
#define __max(a,b) (((a)>(b))?(a):(b))
#endif
typedef struct Raspberry_GuiResources_t
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

	ImTextureID BgImages[__GuiState_COUNT];
} Raspberry_GuiResources_t;


extern Raspberry_GuiData_t Raspberry_guidata;
static Raspberry_GuiResources_t res;

#define SR() PushRowStyle(row);
#define ER() PopRowStyle(row);NextRow(row);


typedef struct guirow_state_t
{
	float left;
	float right;
	float top;
	float bottom;
	int currentitem;
	bool active;
} guirow_state_t;



void RenderBox(int val, guirow_state_t &rowstate, int mode = BOX_REGULAR);


void PushRowStyle(guirow_state_t &rowstate)
{
	if (rowstate.active)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, res.Highlight);
	}
}
void PopRowStyle(guirow_state_t &rowstate)
{
	if (rowstate.active)
	{
		ImGui::PopStyleColor();
	}
}

void RenderCursor(guirow_state_t &rowstate);

void NextRow(guirow_state_t &rowstate)
{
	RenderCursor(rowstate);
	rowstate.currentitem++;
	rowstate.active = rowstate.currentitem == Raspberry_guidata.LeftEncoderValue ? true : false;
	rowstate.top = ImGui::GetCursorPos().y;
}





const char* ParamLabel(int param)
{
	return ParamToName(param);
}

float DepthLabel(int depth)
{
	return (float)(int16_t)depth * (1.0f / 0x4000);
}

void MenuRightAlignLabel(const char *name, guirow_state_t &rowstate)
{
	ImVec2 cp = ImGui::GetCursorPos();
	ImVec2 sz = ImGui::CalcTextSize(name);
	float left = 240 - sz.x;
	ImGui::SetCursorPos(ImVec2(240 - sz.x - 10, cp.y));
	ImGui::Text(name);
	rowstate.left = left;
	rowstate.right = left + sz.x + 30;
}


void TargetsList(guirow_state_t &row)
{

	ModTarget_t *t = 0;
	switch (Raspberry_guidata.GuiState)
	{
	case GuiState_AdSelect: t = Raspberry_guidata.dataAd.target; break;
	case GuiState_AdsrSelect: t = Raspberry_guidata.dataAdsr.target; break;
	case GuiState_CtrlSelect: t = Raspberry_guidata.dataCtrl.target; break;
	case GuiState_LfoSelect: t = Raspberry_guidata.dataLfo.target; break;
	}

	if (!t) return;


	for (int i = 0; i < 16; i++) {


		if (t[i].param != 0)
		{
			SR();	MenuRightAlignLabel(ParamLabel(t[i].param), row); ImGui::SameLine(); ImGui::Text("%1.3f", DepthLabel(t[i].depth)); ER();
		}

		if (i == Raspberry_guidata.selectTarget) {}
	}
}

char EffectChipStrings[8][4][24] = {

{    "Chorus - reverb" ,"Reverb mix","Chorus rate","Chorus mix"},
{    "Flange - reverb","Reverb mix","Flange rate","Flange mix" },
{    "Tremolo - reverb","Reverb mix","Tremolo rate","Tremolo mix"},
{    "Pitch shift","Pitch +/- 4 semitones","-","-"},
{    "Pitch - echo","Pitch shift","Echo delay","Echo mix"},
{    "Test","-","-","-"},
{    "Reverb 1","Reverb time","HF filter","LF filter"},
{    "Reverb 2","Reverb time","HF filter","LF filter"}
};

void Render_MenuEntry_Percentage(const char* name, int param, guirow_state_t &rowstate)
{
	MenuRightAlignLabel(name, rowstate); ImGui::SameLine();
	RenderBox(Raspberry_guidata.outputValues[param], rowstate);
	ImGui::Text("%1.1f%%", (float)((int)Raspberry_guidata.outputValues[param]) * (100.0f / (float)0xFFFF));
}

void Render_MenuEntry_EffectType(const char* name, int param, guirow_state_t &rowstate)
{
	MenuRightAlignLabel("Effect:", rowstate); ImGui::SameLine();
	int currenteffect = DecodeCurrentEffect(Raspberry_guidata.switches[0]);

	ImGui::Text(EffectChipStrings[currenteffect][0]);
}

void Render_MenuEntry_EffectParam1(const char* name, int param, guirow_state_t &rowstate)
{
	int currenteffect = DecodeCurrentEffect(Raspberry_guidata.switches[0]);
	Render_MenuEntry_Percentage(EffectChipStrings[currenteffect][1], param, rowstate);
}

void Render_MenuEntry_EffectParam2(const char* name, int param, guirow_state_t &rowstate)
{
	int currenteffect = DecodeCurrentEffect(Raspberry_guidata.switches[0]);;
	Render_MenuEntry_Percentage(EffectChipStrings[currenteffect][2], param, rowstate);
}

void Render_MenuEntry_EffectParam3(const char* name, int param, guirow_state_t &rowstate)
{
	int currenteffect = DecodeCurrentEffect(Raspberry_guidata.switches[0]);;

	Render_MenuEntry_Percentage(EffectChipStrings[currenteffect][3], param, rowstate);
}

void Render_KeyValue(const char* name, int value, guirow_state_t &rowstate)
{
	MenuRightAlignLabel(name, rowstate); ImGui::SameLine();
	RenderBox(value, rowstate);
	ImGui::Text("%1.1f%%", (float)((int)value) * (100.0f / (float)0xFFFF));
}

void Render_KeySwitch(const char *name, int value, guirow_state_t &rowstate)
{
	ImVec2 p1 = ImGui::GetCursorPos();
	MenuRightAlignLabel(name, rowstate);

	ImVec2 w(0, 0);
	ImVec2 p = ImGui::GetCursorPos();
	w.x = rowstate.right;
	int id = value;
	if (rowstate.active) id += 2;
	ImGui::SetCursorPos(ImVec2(245, p1.y));
	ImGui::Image(res.OnOff[id], ImVec2(128, 48));
}


int RPPresetRemapKnob(int param)
{
	// some hacks

	// Level knobs are used for linear modulations but actual level output params are different
	if (param == Output_CLEANF_LIN) return Output_CLEANF_LEVEL;
	if (param == Output_VCF1_LIN) return Output_VCF1_LEVEL;
	if (param == Output_VCF2_LIN) return Output_VCF2_LEVEL;

	return param;
}

void Render_MenuEntry_RemapValue(const char* name, int param, guirow_state_t &rowstate)
{
	param = RPPresetRemapKnob(param);
	MenuRightAlignLabel(name, rowstate); ImGui::SameLine();
	RenderBox(Raspberry_guidata.outputValues[param], rowstate);
	ImGui::Text("%1.1f%%", (float)((int)Raspberry_guidata.outputValues[param]) * (100.0f / (float)0xFFFF));
}

void Render_MenuEntry_Value(const char* name, int param, guirow_state_t &rowstate)
{
	MenuRightAlignLabel(name, rowstate); ImGui::SameLine();
	RenderBox(Raspberry_guidata.outputValues[param], rowstate);
	ImGui::Text("%1.1f%%", (float)((int)Raspberry_guidata.outputValues[param]) * (100.0f / (float)0xFFFF));
}
void Render_MenuEntry_MidValue(const char* name, int param, guirow_state_t &rowstate)
{
	MenuRightAlignLabel(name, rowstate); ImGui::SameLine();
	RenderBox(Raspberry_guidata.outputValues[param], rowstate, BOX_MID);
	ImGui::Text("%1.1f%%", (float)((int)Raspberry_guidata.outputValues[param]) * (200.0f / (float)0xFFFF) - 100);
}

void RenderBox(int val, guirow_state_t &rowstate, int mode)
{
	ImVec2 p = ImGui::GetCursorScreenPos();
	p.x += 10;
	ImVec2 tl = p;
	ImVec2 br = tl;
	br.x += 220;
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
	ImGui::GetWindowDrawList()->AddRect(tl, br, rowstate.active ? res.Highlight : res.Normal, 0, 0, 2);
	p.x += 12;
	ImGui::SetCursorScreenPos(p);
}

void Render_MenuEntry_Pitch(const char* name, int param, guirow_state_t &rowstate)
{
	MenuRightAlignLabel(name, rowstate); ImGui::SameLine();
	RenderBox(Raspberry_guidata.outputValues[param], rowstate, BOX_MID);
	ImGui::Text("%1.3f", (float)((int)Raspberry_guidata.outputValues[param] - 0x8000) * (1.0f / (float)0x100));
}

void Render_MenuEntry_Waveform1(const char* name, int param, guirow_state_t &rowstate)
{
	bool saw = (Raspberry_guidata.switches[0] >> param) & 1;

	ImVec4 selected = ImVec4(1.0, 1.0, 1.0, 1.0);
	ImVec4 unselected = ImVec4(0.0, 0.0, 0.0, 1.0);

	ImVec4 sawcol = saw ? selected : unselected;

	MenuRightAlignLabel("Saw mix", rowstate);
	ImGui::SameLine();
	ImGui::Text("%s", saw ? "On" : "Off");
}

void Render_MenuEntry_Toggle(const char *name, int param, guirow_state_t &rowstate)
{
	ImVec2 p1 = ImGui::GetCursorPos();
	MenuRightAlignLabel(name, rowstate);

	ImVec2 w(0, 0);
	ImVec2 p = ImGui::GetCursorPos();
	w.x = rowstate.right;
	int switchid = param / 32;
	int adjustedparam = param % 32;
	int id = ((Raspberry_guidata.switches[switchid] & (1 << adjustedparam)) > 0) ? 1 : 0;
	if (rowstate.active) id += 2;
	ImGui::SetCursorPos(ImVec2(245, p1.y));
	ImGui::Image(res.OnOff[id], ImVec2(128, 48));
}

void Render_MenuEntry_ToggleStashChase(const char *name, int param, guirow_state_t &rowstate)
{
	Render_MenuEntry_Toggle(name, param, rowstate);
}

void Render_MenuEntry_FilterMix(const char* name, int param, guirow_state_t &rowstate)
{
	int mix1 = (0x10000) - (int)Raspberry_guidata.outputValues[param];
	int mix2 = (int)Raspberry_guidata.outputValues[param];
	if (mix1 < 0) mix1 = 0;
	if (mix1 > 0x8000) mix1 = 0x8000;
	if (mix2 < 0) mix2 = 0;
	if (mix2 > 0x8000) mix2 = 0x8000;

	MenuRightAlignLabel(name, rowstate); ImGui::SameLine();
	ImGui::Text("%1.2f / %1.2f", (float)mix1 * (1.0f / (float)0x8000), (float)mix2 * (1.0f / (float)0x8000));
}

void RenderMenuTitle(const char *name)
{
	ImVec2 curpos = ImGui::GetCursorPos();
	ImVec2 curspos = ImGui::GetCursorScreenPos();

	ImGui::PushFont(res.BigFont);
	ImVec2 S = ImGui::CalcTextSize(name);
	ImGui::SetCursorPos(ImVec2(curpos.x + 240 - S.x / 2, curpos.y));

	ImGui::Text(name);
	ImGui::PopFont();
	float w = 240 - S.x / 2 - 10;
	float pt = (float)res.PageTime;
	if (pt > 14) pt = 14;
	pt /= 14.0f;
	float distance[13] = { 0,10,30, 40,50, 70,80,90,110,120,140,150,160 };

	for (int i = 0; i < 13; i++)
	{
		float x = w - (distance[i] * 1);
		float L =(float)( (ImGui::GetTextLineHeight() * pow(0.6f, distance[i] / 10.0f) + 20)* pt);
		ImGui::GetWindowDrawList()->AddLine(ImVec2(curspos.x + x, curspos.y - 2), ImVec2(curspos.x + x, curspos.y + L), IM_COL32(255, 255, 255, 255), 5);
		ImGui::GetWindowDrawList()->AddLine(ImVec2(curspos.x + 480 - x, curspos.y - 2), ImVec2(curspos.x + 480 - x, curspos.y + L), IM_COL32(255, 255, 255, 255), 5);
	}
}
void RenderStartMenu(const char *name, guirow_state_t &row)
{
	RenderMenuTitle(name);
	ImGui::SetCursorPos(ImVec2(50, 80));
	row.active = Raspberry_guidata.LeftEncoderValue == 0;
	row.left = 0;
	row.right = 0;
	row.top = ImGui::GetCursorPos().y;
	row.currentitem = 0;
	//ImGui::BeginChild("mainitems", ImVec2(300, 600));

}

void RenderEndMenu()
{
	//ImGui::EndChild();

}



void RenderCursor(guirow_state_t &rowstate)
{
	if (!rowstate.active) return;

	ImVec2 A = ImGui::GetWindowPos();
	ImVec2 B = ImGui::GetCursorPos();
	ImVec2 cbase = ImGui::GetCursorScreenPos();
	float xx = A.x;
	float yy = rowstate.top + A.y;
	yy += ImGui::GetTextLineHeight() - 10;
	int LeftOffs = 10;
	ImGui::SetCursorScreenPos(ImVec2(xx + rowstate.left - 42 - LeftOffs, yy - 32));
	ImGui::Image(res.LeftIndicator, ImVec2(32, 32));
	ImGui::GetWindowDrawList()->AddLine(ImVec2(xx + res.encoderbarmargin, yy), ImVec2(xx + rowstate.left - 10 - LeftOffs, yy), res.Highlight, 4);
	ImGui::GetWindowDrawList()->AddLine(ImVec2(xx + res.encoderbarmargin, yy), ImVec2(xx + res.encoderbarmargin, res.encoderheight), res.Highlight, 4);
	ImGui::GetWindowDrawList()->AddLine(ImVec2(xx - 10, res.encoderheight), ImVec2(xx + res.encoderbarmargin, res.encoderheight), res.Highlight, 4);
	ImGui::SetCursorPos(B);
}
bool RenderDefaultItems(GuiState_t state)
{
#define MENU(id, button, name) if (state == GuiState_Menu_##id) { guirow_state_t rowstate;RenderStartMenu(name,rowstate);rowstate.top = ImGui::GetCursorPos().y;
#define ENDMENU() RenderEndMenu(); return true; }

#define ENTRY(name,type,param) \
	PushRowStyle(rowstate);Render_##type(name, param,rowstate);PopRowStyle(rowstate);NextRow(rowstate);
#define CUSTOMENTRY(name,type,param) \
	PushRowStyle(rowstate);Render_##type(name, param,rowstate);PopRowStyle(rowstate);NextRow(rowstate);
#include "PanUiMap.h"
#undef MENU
#undef ENDMENU
#undef ENTRY
#undef CUSTOMENTRY
	return false;
}

bool RenderMenu(GuiState_t state)
{


	return RenderDefaultItems(state);

}

void Raspberry_Init()
{
	memset(&Raspberry_guidata, 0, sizeof(Raspberry_GuiData_t));

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

	for (int i = 0; i < __GuiState_COUNT; i++)
	{
		res.BgImages[i] = res.MainBG;
	}
	res.BgImages[GuiState_Root] = res.RootBG;
	res.BgImages[GuiState_Boot] = res.RootBG;

}

void Raspberry_PushStyle()
{
	ImGui::PushFont(res.SmallFont);
	ImGui::PushStyleColor(ImGuiCol_Text, res.Normal);
}

void Raspberry_PopStyle()
{
	ImGui::PopStyleColor();
	ImGui::PopFont();
}

void RenderHome()
{
	ImGui::Text("Current patch:");
	ImGui::PushFont(res.BigFont);
	ImGui::Text(Raspberry_guidata.PresetName);
	ImGui::PopFont();
	// render patch name
}

int BootTime = 0;
void RenderBootScreen()
{
	ImGui::Text("BOOT SCREEN HERE");
	ImVec2 pos = ImGui::GetCursorPos();

	if (res.BgImages[GuiState_Boot])
	{
		ImGui::Image(res.BgImages[GuiState_Boot], ImVec2(1024, 600));
	}
	else
	{
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 1024, pos.y + 600), res.BGColor);
	}
}

GuiState_t LastGuiState;
void SelectAModText()
{
	ImVec2 p = ImGui::GetCursorPos();
	ImGui::Text("Please select a");
	p.y += ImGui::GetTextLineHeight();
	ImGui::SetCursorPos(p);
	ImGui::Text("modulation target");
}

void RenderControlMenu()
{

#define CTRLKV(name, param){ SR(); Render_KeyValue(name, Raspberry_guidata. param, row); ER();}
#define SWITCH2(name, sw){ SR(); Render_KeySwitch(name, (Raspberry_guidata.switches[sw/32] >> (sw%32)) &1, row); ER();}
#define PARA(name, output){ SR(); Render_KeyValue(name, Raspberry_guidata.outputValues[output], row); ER();}
#define CTRLMENU(id,title) if (Raspberry_guidata.dataCtrl.source  == id)\
{\
	guirow_state_t row; \
	RenderStartMenu(title, row); \


#define CTRLENDMENU()  TargetsList(row);  RenderEndMenu(); }

#include "ModMenus.h"

#undef CTRLMENU
#undef CTRLKV
#undef	PARA
#undef CTRLENDMENU
#undef SWITCH2

}

void RenderAssignMenu()
{


#define KV(name, param, field){ SR(); Render_KeyValue(name, Raspberry_guidata. field, row); ER();}

#define MENU(id,title,structname) if (Raspberry_guidata.GuiState  == id)\
{\
	guirow_state_t row; \
	RenderStartMenu(title, row); \
	if (Raspberry_guidata.ModSelect != -1) {

#define ENDMENU()  TargetsList(row);}  else {SelectAModText();}RenderEndMenu(); }

#include "ModMenus.h"

#undef MENU
#undef KV
#undef ENDMENU

}

void Raspberry_RenderScreen()
{
	if (BootTime < 10)
	{
		BootTime++;
		RenderBootScreen();
		return;
	}

	if (Raspberry_guidata.GuiState == LastGuiState)
	{
		res.PageTime++;
	}
	else
	{
		LastGuiState = Raspberry_guidata.GuiState;
		res.PageTime = 0;
	}

	Raspberry_PushStyle();


	ImVec2 pos = ImGui::GetCursorPos();
	if (res.BgImages[Raspberry_guidata.GuiState])
	{
		ImGui::Image(res.BgImages[Raspberry_guidata.GuiState], ImVec2(1024, 600));
	}
	else
	{
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 1024, pos.y + 600), res.BGColor);
	}
	ImGui::SetCursorPos(pos);


	if (Raspberry_guidata.GuiState == GuiState_LfoSelect)
	{
		RenderAssignMenu();
	}
	else if (Raspberry_guidata.GuiState == GuiState_AdsrSelect)
	{
		RenderAssignMenu();
	}
	else if (Raspberry_guidata.GuiState == GuiState_AdSelect)
	{
		RenderAssignMenu();
	}
	else if (Raspberry_guidata.GuiState == GuiState_SavePreset)
	{
		guirow_state_t row;
		RenderStartMenu("Save Preset?", row);
		ImVec2 p = ImGui::GetCursorPos();
		ImGui::Text("Please press target");
		p.y += ImGui::GetTextLineHeight();
		ImGui::SetCursorPos(p);
		ImGui::Text("button to save ");
		RenderEndMenu();
	}
	else if (Raspberry_guidata.GuiState == GuiState_InitPatch)
	{
		guirow_state_t row;
		RenderStartMenu("Reset to init patch?", row);
		ImGui::Text("Press final to reset");
		RenderEndMenu();
	}
	else if (Raspberry_guidata.GuiState == GuiState_SelectBanks)
	{
		guirow_state_t row;
		RenderStartMenu("Select bank?", row);
		ImGui::Text("Please select:");
		RenderEndMenu();
	}
	else if (Raspberry_guidata.GuiState == GuiState_CtrlSelect)
	{
		/*Source_none,

			Source_left_mod,
			Source_right_mod,
			Source_x,
			Source_y,
			Source_z,
			Source_zprime,
			Source_note,
			Source_vel*/

		RenderControlMenu();


	}
	else if (RenderMenu(Raspberry_guidata.GuiState))
	{
	}
	else
	{
		RenderHome();
	}

	Raspberry_PopStyle();
}




void Raspberry_WindowFrame()
{

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

	ImGui::Begin("screen", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);

	ImGui::SetWindowSize(ImVec2(1024, 600));


	Raspberry_RenderScreen();

	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}

uint32_t RaspberryOffset = sizeof(Raspberry_GuiData_t);
Raspberry_GuiData_t incoming;
unsigned char *RaspberryPointer;

void AddIncomingByte(unsigned char b)
{
	if (RaspberryOffset < sizeof(Raspberry_GuiData_t))
	{
		RaspberryPointer[RaspberryOffset] = b;
		RaspberryOffset++;
		if (RaspberryOffset == sizeof(Raspberry_GuiData_t))
		{

		}
	}
}

int Written = 0;
void DoDataCommands(unsigned char comm, uint32_t data)
{
	switch (comm)
	{
		//#ifdef SIMULATEINCOMINGSERIAL
	case 0xd0:
	{
		//printf("Incoming Guistate\n");
		RaspberryOffset = 0;
		RaspberryPointer = (unsigned char *)&incoming;
		unsigned char b1 = data & 255;
		unsigned char b2 = (data >> 8) & 255;
		unsigned char b3 = (data >> 16) & 255;
		AddIncomingByte(b1);
		AddIncomingByte(b2);
		AddIncomingByte(b3);

	}
	break;
	case 0xd1:
	{
		//printf("GuistatePacket\n");
		unsigned char b1 = data & 255;
		unsigned char b2 = (data >> 8) & 255;
		unsigned char b3 = (data >> 16) & 255;
		AddIncomingByte(b1);
		AddIncomingByte(b2);
		AddIncomingByte(b3);
	}
	break;
	case 0xd2:
	{
		if (0) {
			printf("%d %d\n", RaspberryOffset, sizeof(Raspberry_GuiData_t));
			unsigned char *a = (unsigned char*)&Raspberry_guidata;
			unsigned char *b = (unsigned char*)&incoming;
			unsigned char cb = 0;
			for (int i = 0; i < sizeof(Raspberry_GuiData_t); i++)
			{
				if (cb != b[i]) printf("hmm\n");
				cb++;

			}
		}
		memcpy(&Raspberry_guidata, &incoming, sizeof(Raspberry_GuiData_t));
	}break;
	case 0xd3:
	{
		RaspberryPointer = (unsigned char *)&incoming;
		unsigned char b1 = data & 255;
		unsigned char b2 = (data >> 8) & 255;
		unsigned short offs = (data >> 16) & 255;
		RaspberryPointer[offs] = b1;
		RaspberryPointer[offs + 1] = b2;
	}
	break;
	}
	//#endif

}
