#include "imgui.h"
#include <GL/gl3w.h> 
#include "../libs/lodepng-master/lodepng.h"
#include "PanHeader.h"

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


	uint32_t LeftEncoderValue;
} Raspberry_GuiData_t;

typedef struct Raspberry_GuiResources_t
{
	ImTextureID MainBG;
	ImTextureID MenuBG;
	ImTextureID SpriteSheet;
	ImFont *BigFont;
	ImFont *SmallFont;
	ImU32 Highlight;
	ImU32 Normal;
	ImU32 BGColor;
	int PageTime;
} Raspberry_GuiResources_t;


ImTextureID Raspberry_LoadTexture(const char *filename)
{
	std::vector<unsigned char> buffer, image;
	lodepng::load_file(buffer, filename); //load the image file with given filename
	unsigned w, h;
	unsigned error = lodepng::decode(image, w, h, buffer); //decode the png

	if (error)
	{
		return 0;
	}
	GLuint tex;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_NEAREST = no smoothing
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

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
	return (float)(int16_t)depth * (1.0 / 0x4000);
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

void Render_MenuEntry_Value(const char* name, int param, bool active)
{
	ImGui::LabelText(name, "%1.3f", (float)((int)Raspberry_guidata.outputValues[param]) * (1.0f / (float)0xFFFF));
}

void Render_MenuEntry_Pitch(const char* name, int param, bool active)
{
	ImGui::LabelText(name, "%1.3f", (float)((int)Raspberry_guidata.outputValues[param] - 0x8000) * (1.0f / (float)0x100));
}

void Render_MenuEntry_Waveform1(const char* name, int param, bool active)
{
	bool saw = (Raspberry_guidata.switches[0] >> param) & 1;

	ImVec4 selected = ImVec4(1.0, 1.0, 1.0, 1.0);
	ImVec4 unselected = ImVec4(0.0, 0.0, 0.0, 1.0);

	ImVec4 sawcol = saw ? selected : unselected;

	ImGui::TextColored(sawcol, "SAW MIX"); ImGui::SameLine();
	ImGui::Text("%s", saw ? "On" : "Off");
}

void Render_MenuEntry_Toggle(const char *name, int param, bool active)
{
	ImGui::Text(name);
}

void Render_MenuEntry_Waveform3(const char* name, int param, bool active)
{
	bool tri = (Raspberry_guidata.switches[0] >> param) & 1;
	bool saw = (Raspberry_guidata.switches[0] >> (param + 1)) & 1;
	bool sqr = (Raspberry_guidata.switches[0] >> (param + 2)) & 1;
	bool sub = (Raspberry_guidata.switches[0] >> (param + 3)) & 1;

	ImVec4 selected = ImVec4(1.0, 1.0, 1.0, 1.0);
	ImVec4 unselected = ImVec4(0.0, 0.0, 0.0, 1.0);

	ImVec4 tricol = tri ? selected : unselected;
	ImVec4 sawcol = saw ? selected : unselected;
	ImVec4 sqrcol = sqr ? selected : unselected;
	ImVec4 subcol = sub ? selected : unselected;

	ImGui::TextColored(tricol, "T"); ImGui::SameLine();
	ImGui::TextColored(sawcol, "S"); ImGui::SameLine();
	ImGui::TextColored(sqrcol, "P"); ImGui::SameLine();
	ImGui::TextColored(subcol, "U");
}

void Render_MenuEntry_Pulsewidth(const char* name, int param, bool active)
{
	ImGui::Text(name); ImGui::SameLine(); ImGui::Text( "%1.2f %%", (float)((int)Raspberry_guidata.outputValues[param]) * (1.0f / (float)0xFFFF));
}

void Render_MenuEntry_FilterMix(const char* name, int param, bool active)
{
	int mix1 = (0x10000) - (int)Raspberry_guidata.outputValues[param];
	int mix2 = (int)Raspberry_guidata.outputValues[param];
	if (mix1 < 0) mix1 = 0;
	if (mix1 > 0x8000) mix1 = 0x8000;
	if (mix2 < 0) mix2 = 0;
	if (mix2 > 0x8000) mix2 = 0x8000;
	ImGui::Text(name); ImGui::SameLine();ImGui::Text("%1.2f / %1.2f", (float)mix1 * (1.0f / (float)0x8000), (float)mix2 * (1.0f / (float)0x8000));
}

void RenderMenuTitle(const char *name)
{
	ImVec2 curpos = ImGui::GetCursorPos();
	ImVec2 winpos = ImGui::GetWindowPos();
	ImGui::PushFont(res.BigFont);
	ImVec2 S = ImGui::CalcTextSize(name);
	ImGui::SetCursorPos(ImVec2(curpos.x + 240 - S.x / 2, curpos.y));

	ImGui::Text(name);
	ImGui::PopFont();
	int w = 240 - S.x / 2 - 10;
	float pt = res.PageTime;
	if (pt > 14) pt = 14;
	pt /= 14.0f;
	for (int x = 0; x <w ; x += 20)
	{
		float M = ((float)x / (float)w);
		M *= M;
		float L = (ImGui::GetTextLineHeight() * M + 10)* pt;
		ImGui::GetWindowDrawList()->AddLine(ImVec2(winpos.x + x, winpos.y-2), ImVec2(winpos.x + x, winpos.y+L), IM_COL32(255, 255, 255, 255), 1 + x/20);
		ImGui::GetWindowDrawList()->AddLine(ImVec2(winpos.x + 480-x, winpos.y-2), ImVec2(winpos.x + 480-x, winpos.y+L), IM_COL32(255, 255, 255, 255), 1+x/20);
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
void RenderCursor(bool active)
{
	if (!active) return;
	ImVec2 A = ImGui::GetWindowPos();
	ImVec2 B = ImGui::GetCursorPos();
	float xx = A.x;
	float yy = A.y + B.y;
	yy += ImGui::GetTextLineHeight() / 2.0f;
	
	ImGui::GetWindowDrawList()->AddLine(ImVec2(xx +3, yy ), ImVec2(xx+20, yy), res.Highlight, 4);
	ImGui::GetWindowDrawList()->AddLine(ImVec2(xx + 3,yy), ImVec2(xx+3,yy+ 600), res.Highlight, 4);
}
bool RenderDefaultItems(GuiState_t state)
{
#define MENU(id, button, name) if (state == GuiState_Menu_##id) { RenderStartMenu(name);int currentitem = 0;
#define ENDMENU() RenderEndMenu(); return true; }

#define ENTRY(name,type,param) \
	RenderCursor(currentitem == Raspberry_guidata.LeftEncoderValue?true:false);Render_##type(name, param,currentitem == Raspberry_guidata.LeftEncoderValue?true:false);currentitem++;
#define CUSTOMENTRY(name,type,param) \
	RenderCursor(currentitem == Raspberry_guidata.LeftEncoderValue?true:false);Render_##type(name, param,currentitem == Raspberry_guidata.LeftEncoderValue?true:false);currentitem++;
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

	res.PageTime = 0;
	res.Highlight = IM_COL32(235, 200, 28,255);
	res.Normal  = IM_COL32(255, 255, 255, 255);
	res.BGColor = IM_COL32(0, 58, 66, 255);
	res.MainBG = Raspberry_LoadTexture("UI_MAINBG.png");

	res.SmallFont = io.Fonts->AddFontFromFileTTF("Fontfabric - Panton.otf", 40.0f);
	res.BigFont = io.Fonts->AddFontFromFileTTF("Fontfabric - Panton ExtraBold.otf", 54.0f);

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
	ImVec2 pos = ImGui::GetCursorScreenPos();
	if (res.MainBG)
	{
		ImGui::Image(res.MainBG, ImVec2(480, 800));
	}
	else
	{
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 480, pos.y + 800), res.BGColor);
	}
	ImGui::SetCursorScreenPos(pos);
	

	if (Raspberry_guidata.GuiState == GuiState_LfoSelect)
	{
		RenderStartMenu("LFO assign");
		if (Raspberry_guidata.editLfo != 0) {
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

void Raspberry_EncoderTurn(EncoderEnum id, int delta)
{
	printf("encoder %s: %d\n", Encoders[id].name, delta);
}

void Raspberry_Reset()
{
	Raspberry_guidata.GuiState = GuiState_Root;
}