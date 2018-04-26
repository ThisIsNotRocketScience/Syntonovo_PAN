#include "../libs/imgui-master/imgui.h"
#ifdef WIN32
#include <GL/gl3w.h> 
#endif
#include "../libs/lodepng-master/lodepng.h"
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

typedef struct guirow_state_t
{
	float left;
	float right;
	float top;
	float bottom;
	bool active;
} guirow_state_t;

typedef struct Raspberry_GuiData_t
{
	GuiState_t GuiState;
	GuiState_t LastGuiState;
	int ModSelect;

	LfoModulation_t* editLfo;
	AdsrModulation_t* editAdsr;
	AdModulation_t* editAd;
	ControlModulation_t* editCtrl;
	ModTarget_t* editTargets;
	int selectTarget;

	LfoModulation_t dataLfo;
	AdsrModulation_t dataAdsr;
	AdModulation_t dataAd;
	ControlModulation_t dataCtrl;

	uint32_t outputValues[256];
	uint32_t switches[1];

	char PresetName[PRESET_NAME_LENGTH];
	uint32_t LeftEncoderValue;
} Raspberry_GuiData_t;

typedef struct Raspberry_GuiResources_t
{
	ImTextureID MainBG;
	ImTextureID RootBG;
	ImTextureID SpriteSheet;
	ImFont *BigFont;
	ImFont *SmallFont;
	ImU32 Highlight;
	ImU32 Normal;
	ImU32 BGColor;
	int PageTime;
	int encoderbarmargin;
	int encoderheight;

	ImTextureID BgImages[__GuiState_COUNT];
} Raspberry_GuiResources_t;


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
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_NEAREST = no smoothing
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

#else 
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
	glBindTexture(GL_TEXTURE_2D, last_texture);
#endif

	return (ImTextureID)tex;
};


static Raspberry_GuiData_t Raspberry_guidata;
static Raspberry_GuiResources_t res;

void Raspberry_ToState(GuiState_t state, int modselect)
{
	Raspberry_guidata.GuiState = state;
	Raspberry_guidata.ModSelect = modselect;
	Raspberry_guidata.editLfo = 0;
	Raspberry_guidata.editAdsr = 0;
	Raspberry_guidata.editAd = 0;
	Raspberry_guidata.editTargets = 0;
	Raspberry_guidata.selectTarget = -1;
}

void Raspberry_EditLfo(LfoModulation_t& mod)
{
	memcpy(&Raspberry_guidata.dataLfo, &mod, sizeof(LfoModulation_t));
	Raspberry_guidata.editLfo = &Raspberry_guidata.dataLfo;
	Raspberry_guidata.editTargets = &Raspberry_guidata.dataLfo.target[0];
}

void Raspberry_EditAdsr(AdsrModulation_t& mod)
{
	memcpy(&Raspberry_guidata.dataAdsr, &mod, sizeof(AdsrModulation_t));
	Raspberry_guidata.editAdsr = &Raspberry_guidata.dataAdsr;
	Raspberry_guidata.editTargets = &Raspberry_guidata.dataAdsr.target[0];
}

void Raspberry_EditAd(AdModulation_t& mod)
{
	memcpy(&Raspberry_guidata.dataAd, &mod, sizeof(AdModulation_t));
	Raspberry_guidata.editAd = &Raspberry_guidata.dataAd;
	Raspberry_guidata.editTargets = &Raspberry_guidata.dataAd.target[0];
}

void Raspberry_EditCtrl(ControlModulation_t& mod)
{
	memcpy(&Raspberry_guidata.dataCtrl, &mod, sizeof(ControlModulation_t));
	Raspberry_guidata.editCtrl = &Raspberry_guidata.dataCtrl;
	Raspberry_guidata.editTargets = &Raspberry_guidata.dataCtrl.target[0];
}

void Raspberry_SelectTarget(int target)
{
	Raspberry_guidata.selectTarget = target;
}

void Raspberry_UpdateTarget(int target, int param, int depth)
{
	Raspberry_guidata.editTargets[target].param = param;
	Raspberry_guidata.editTargets[target].depth = depth;
}

void Raspberry_SetLfoParam(SubParam_t subparam, uint32_t value)
{
	switch (subparam) {
	case Sub_lfo_shape:
		Raspberry_guidata.editLfo->shape = value;
		break;
	case Sub_lfo_speed:
		Raspberry_guidata.editLfo->speed = value;
		break;
	}
}

void Raspberry_SetAdsrParam(SubParam_t subparam, uint32_t value)
{
	switch (subparam) {
	case Sub_adsr_a:
		Raspberry_guidata.editAdsr->a = value;
		break;
	case Sub_adsr_d:
		Raspberry_guidata.editAdsr->d = value;
		break;
	case Sub_adsr_s:
		Raspberry_guidata.editAdsr->s = value;
		break;
	case Sub_adsr_r:
		Raspberry_guidata.editAdsr->r = value;
		break;
	}
}

void Raspberry_SetAdParam(SubParam_t subparam, uint32_t value)
{
	switch (subparam) {
	case Sub_ad_a:
		Raspberry_guidata.editAd->a = value;
		break;
	case Sub_ad_d:
		Raspberry_guidata.editAd->d = value;
		break;
	}
}

const char* ParamLabel(int param)
{
	return ParamToName(param);
}

float DepthLabel(int depth)
{
	return (float)(int16_t)depth * (1.0f / 0x4000);
}

void TargetsList()
{
	if (!Raspberry_guidata.editTargets) return;

	ImGui::Columns(2);

	for (int i = 0; i < 16; i++) {
		if (i == Raspberry_guidata.selectTarget) {
			ImGui::PushStyleColor(ImGuiCol_Text, 0xFFFFFFFF);
		}
		if (Raspberry_guidata.editTargets[i].param != 0) {
			ImGui::Text(ParamLabel(Raspberry_guidata.editTargets[i].param)); ImGui::NextColumn();
			ImGui::Text("%1.3f", DepthLabel(Raspberry_guidata.editTargets[i].depth)); ImGui::NextColumn();
		}
		if (i == Raspberry_guidata.selectTarget) {
			ImGui::PopStyleColor();
		}
	}
}

void MenuRightAlignLabel(const char *name, guirow_state_t &rowstate)
{
	ImVec2 cp = ImGui::GetCursorPos();
	ImVec2 sz = ImGui::CalcTextSize(name);
	float left = 240 - sz.x;
	ImGui::SetCursorPos(ImVec2(240 - sz.x , cp.y));
	ImGui::Text(name);
	rowstate.left =  left - 10 - cp.x;
	rowstate.right = left + sz.x + 10;
}
char EffectChipStrings[8][4][24] = {

	{"Chorus - reverb","Reverb mix","Chorus rate","Chorus mix"},
{"Flange - reverb","Reverb mix","Flange rate","Flange mix" },
{"Tremolo - reverb","Reverb mix","Tremolo rate","Tremolo mix"},
{"Pitch shift","Pitch +/- 4 semitones","-","-"},
{"Pitch - echo","Pitch shift","Echo delay","Echo mix"},
{"Test","-","-","-"},
{"Reverb 1","Reverb time","HF filter","LF filter"},
{"Reverb 2","Reverb time","HF filter","LF filter"}
};
void Render_MenuEntry_Percentage(const char* name, int param, guirow_state_t &rowstate)
{
	 MenuRightAlignLabel(name,rowstate); ImGui::SameLine();

	ImGui::Text("%1.f %%", (float)((int)Raspberry_guidata.outputValues[param]) * (100.0f / (float)0xFFFF));
}

void Render_MenuEntry_EffectType(const char* name, int param, guirow_state_t &rowstate)
{
	MenuRightAlignLabel("Effect:", rowstate); ImGui::SameLine();
	int currenteffect = 0;
	ImGui::Text(EffectChipStrings[currenteffect][0]);
}

void Render_MenuEntry_EffectParam1(const char* name, int param, guirow_state_t &rowstate)
{
	int currenteffect = 0;
	Render_MenuEntry_Percentage(EffectChipStrings[currenteffect][1], param, rowstate);
}

void Render_MenuEntry_EffectParam2(const char* name, int param, guirow_state_t &rowstate)
{
	int currenteffect = 0;
	Render_MenuEntry_Percentage(EffectChipStrings[currenteffect][2], param, rowstate);
}

void Render_MenuEntry_EffectParam3(const char* name, int param, guirow_state_t &rowstate)
{
	int currenteffect = 0;
	Render_MenuEntry_Percentage(EffectChipStrings[currenteffect][3], param, rowstate);
}

void Render_MenuEntry_Value(const char* name, int param, guirow_state_t &rowstate)
{
	 MenuRightAlignLabel(name, rowstate); ImGui::SameLine();
	ImGui::Text("%1.3f", (float)((int)Raspberry_guidata.outputValues[param]) * (1.0f / (float)0xFFFF));
}

void Render_MenuEntry_Pitch(const char* name, int param, guirow_state_t &rowstate)
{
	MenuRightAlignLabel(name, rowstate); ImGui::SameLine();
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
	ImVec2 p = ImGui::GetCursorScreenPos();
	w.x = rowstate.right;
	ImVec2 onsize = ImGui::CalcTextSize("ON");
	ImVec2 offsize = ImGui::CalcTextSize("OFF");

	
	if (Raspberry_guidata.switches[param]>0)ImGui::GetWindowDrawList()->AddRect(ImVec2(p.x + w.x, p.y + w.y - 48), ImVec2(w.x + p.x + 128, w.y + p.y), res.Highlight, 3, 14, 2);
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
	for (int x = 0; x <w ; x += 20)
	{
		float M = ((float)x / (float)w);
		M *= M;
		float L = (ImGui::GetTextLineHeight() * M + 10)* pt;
		ImGui::GetWindowDrawList()->AddLine(ImVec2(curspos.x + x, curspos.y-2), ImVec2(curspos.x + x, curspos.y+L), IM_COL32(255, 255, 255, 255), 1 + (float)x/20.0f);
		ImGui::GetWindowDrawList()->AddLine(ImVec2(curspos.x + 480-x, curspos.y-2), ImVec2(curspos.x + 480-x, curspos.y+L), IM_COL32(255, 255, 255, 255), 1+(float)x/20.0f);
	}
}
void RenderStartMenu(const char *name)
{	
	RenderMenuTitle(name);
	ImGui::SetCursorPos(ImVec2(50, 80));
	//ImGui::BeginChild("mainitems", ImVec2(300, 600));

}

void RenderEndMenu()
{
	//ImGui::EndChild();

}


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
void RenderCursor( guirow_state_t &rowstate)
{
	if (!rowstate.active) return;

	ImVec2 A = ImGui::GetWindowPos();
	ImVec2 B = ImGui::GetCursorPos();
	ImVec2 cbase = ImGui::GetCursorScreenPos();
	float xx = A.x ;
	float yy = rowstate.top +A.y;
	yy += ImGui::GetTextLineHeight() / 2.0f;
	
	ImGui::GetWindowDrawList()->AddLine(ImVec2(xx + res.encoderbarmargin, yy ), ImVec2(xx+rowstate.left, yy), res.Highlight, 4);
	ImGui::GetWindowDrawList()->AddLine(ImVec2(xx + res.encoderbarmargin,yy), ImVec2(xx+ res.encoderbarmargin,yy+ res.encoderheight), res.Highlight, 4);
}
bool RenderDefaultItems(GuiState_t state)
{
#define MENU(id, button, name) if (state == GuiState_Menu_##id) { RenderStartMenu(name);int currentitem = 0;guirow_state_t rowstate={0,0,0,0,currentitem == Raspberry_guidata.LeftEncoderValue?true:false};rowstate.top = ImGui::GetCursorPos().y;
#define ENDMENU() RenderEndMenu(); return true; }

#define ENTRY(name,type,param) \
	PushRowStyle(rowstate);Render_##type(name, param,rowstate);RenderCursor(rowstate);PopRowStyle(rowstate);currentitem++;rowstate.active = currentitem == Raspberry_guidata.LeftEncoderValue?true:false;rowstate.top = ImGui::GetCursorPos().y;
#define CUSTOMENTRY(name,type,param) \
	PushRowStyle(rowstate);Render_##type(name, param,rowstate);RenderCursor(rowstate);PopRowStyle(rowstate);currentitem++;rowstate.active = currentitem == Raspberry_guidata.LeftEncoderValue?true:false;rowstate.top = ImGui::GetCursorPos().y;
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
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	res.encoderbarmargin = 10;
	res.encoderheight = 600;
	res.PageTime = 0;
	res.Highlight = IM_COL32(235, 200, 28,255);
	res.Normal  = IM_COL32(255, 255, 255, 255);
	res.BGColor = IM_COL32(0, 58, 66, 255);
	res.MainBG = Raspberry_LoadTexture("UI_MAINBG.png");
	res.RootBG = Raspberry_LoadTexture("UI_ROOTBG.png");

	res.SmallFont = io.Fonts->AddFontFromFileTTF("Fontfabric - Panton.otf", 40.0f);
	res.BigFont = io.Fonts->AddFontFromFileTTF("Fontfabric - Panton ExtraBold.otf", 54.0f);
	
	for (int i = 0; i < __GuiState_COUNT; i++)
	{
		res.BgImages[i] = res.MainBG;
	}
	res.BgImages[GuiState_Root] = res.RootBG;

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

void Raspberry_RenderScreen()
{
	if (Raspberry_guidata.GuiState == Raspberry_guidata.LastGuiState)
	{
		res.PageTime++;
	}
	else
	{
		Raspberry_guidata.LastGuiState = Raspberry_guidata.GuiState;
		res.PageTime=0;
	}

	Raspberry_PushStyle();

	ImVec2 pos = ImGui::GetCursorPos();
	if (res.BgImages[Raspberry_guidata.GuiState])
	{
		ImGui::Image(res.BgImages[Raspberry_guidata.GuiState], ImVec2(480, 800));
	}
	else
	{
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 480, pos.y + 800), res.BGColor);
	}
	ImGui::SetCursorPos(pos);
	
	if (Raspberry_guidata.GuiState == GuiState_LfoSelect)
	{
		RenderStartMenu("LFO assign");
		if (Raspberry_guidata.editLfo != 0)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
			ImGui::LabelText("Speed", "%d", Raspberry_guidata.editLfo->speed);
			ImGui::LabelText("Shape", "%d", Raspberry_guidata.editLfo->shape);
			ImGui::PopStyleVar();
			TargetsList();
		}
		RenderEndMenu();
	}
	else if (Raspberry_guidata.GuiState == GuiState_AdsrSelect)
	{
		RenderStartMenu("ADSR assign");
		if (Raspberry_guidata.editAdsr != 0) {
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
			ImGui::LabelText("A", "%d", Raspberry_guidata.editAdsr->a);
			ImGui::LabelText("D", "%d", Raspberry_guidata.editAdsr->d);
			ImGui::LabelText("S", "%d", Raspberry_guidata.editAdsr->s);
			ImGui::LabelText("R", "%d", Raspberry_guidata.editAdsr->r);
			ImGui::PopStyleVar();
			TargetsList();
		}
		RenderEndMenu();
	}
	else if (Raspberry_guidata.GuiState == GuiState_AdSelect)
	{
		RenderStartMenu("AD assign");
		if (Raspberry_guidata.editAd != 0) {
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
			ImGui::LabelText("A", "%d", Raspberry_guidata.editAd->a);
			ImGui::LabelText("D", "%d", Raspberry_guidata.editAd->d);
			ImGui::PopStyleVar();
			TargetsList();
		}
		RenderEndMenu();
	}
	else if (Raspberry_guidata.GuiState == GuiState_CtrlSelect)
	{
		RenderStartMenu("Controller assign");
		TargetsList();
		RenderEndMenu();
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

void Raspberry_OutputChangeValue(int output, uint32_t value)
{
	Raspberry_guidata.outputValues[output] = value;
}

void Raspberry_SetSwitches(uint32_t* switches)
{
	Raspberry_guidata.switches[0] = switches[0];
}
void Raspberry_SetName(char *newname)
{
	for (int i = 0; i < PRESET_NAME_LENGTH; i++) Raspberry_guidata.PresetName[i] = newname[i];
}
void Raspberry_EncoderTurn(EncoderEnum id, int delta)
{
	printf("encoder %s: %d\n", Encoders[id].name, delta);
}

void Raspberry_Reset()
{
	Raspberry_guidata.GuiState = GuiState_Root;
}