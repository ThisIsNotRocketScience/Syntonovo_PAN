#include "imgui.h"
#include <GL/gl3w.h> 
#include "../libs/lodepng-master/lodepng.h"
#include "PanHeader.h"

typedef struct Raspberry_GuiData_t
{
	GuiState_t GuiState;
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
static Raspberry_GuiResources_t Raspberry_resources;

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

void Render_MenuEntry_Value(const char* name, int param)
{
	ImGui::LabelText(name, "%1.3f", (float)((int)Raspberry_guidata.outputValues[param]) * (1.0f / (float)0xFFFF));
}

void Render_MenuEntry_Pitch(const char* name, int param)
{
	ImGui::LabelText(name, "%1.3f", (float)((int)Raspberry_guidata.outputValues[param] - 0x8000) * (1.0f / (float)0x100));
}

void Render_MenuEntry_Waveform1(const char* name, int param)
{
	bool saw = (Raspberry_guidata.switches[0] >> param) & 1;

	ImVec4 selected = ImVec4(1.0, 1.0, 1.0, 1.0);
	ImVec4 unselected = ImVec4(0.0, 0.0, 0.0, 1.0);

	ImVec4 sawcol = saw ? selected : unselected;

	ImGui::TextColored(sawcol, "SAW MIX"); ImGui::SameLine();
	ImGui::Text("%s", saw ? "On" : "Off");
}

void Render_MenuEntry_Waveform3(const char* name, int param)
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

void Render_MenuEntry_Pulsewidth(const char* name, int param)
{
	ImGui::LabelText(name, "%1.2f %%", (float)((int)Raspberry_guidata.outputValues[param]) * (1.0f / (float)0xFFFF));
}

void Render_MenuEntry_FilterMix(const char* name, int param)
{
	int mix1 = (0x10000) - (int)Raspberry_guidata.outputValues[param];
	int mix2 = (int)Raspberry_guidata.outputValues[param];
	if (mix1 < 0) mix1 = 0;
	if (mix1 > 0x8000) mix1 = 0x8000;
	if (mix2 < 0) mix2 = 0;
	if (mix2 > 0x8000) mix2 = 0x8000;
	ImGui::LabelText(name, "%1.2f / %1.2f", (float)mix1 * (1.0f / (float)0x8000), (float)mix2 * (1.0f / (float)0x8000));
}


bool RenderDefaultItems(GuiState_t state)
{
#define MENU(id, button, name) \
	if (state == GuiState_Menu_##id) {
#define ENDMENU() \
		return true; \
	}
#define ENTRY(name,type,param) \
	Render_##type(name, param);
#define CUSTOMENTRY(name,type,param) \
	Render_##type(name, param);
#include "PanUiMap.h"
#undef MENU
#undef ENDMENU
#undef ENTRY
#undef CUSTOMENTRY

}

bool RenderMenu(GuiState_t state)
{
	return RenderDefaultItems(state);
	
}

void Raspberry_Init()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;


	Raspberry_resources.MainBG = Raspberry_LoadTexture("UI_MAINBG.png");
	Raspberry_resources.SmallFont = io.Fonts->AddFontFromFileTTF("ProggyTiny.ttf", 25.0f);

}

void Raspberry_PushStyle()
{
	ImGui::PushFont(Raspberry_resources.SmallFont);
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 255, 255, 255));

}

void Raspberry_PopStyle()
{
	ImGui::PopStyleColor();
	ImGui::PopFont();
}

void Raspberry_RenderScreen()
{
	Raspberry_PushStyle();
	ImVec2 pos = ImGui::GetCursorScreenPos();
	if (Raspberry_resources.MainBG)
	{
		ImGui::Image(Raspberry_resources.MainBG, ImVec2(480, 800));
	}
	else
	{
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 480, pos.y + 800), IM_COL32(0, 58, 66, 255));
	}
	ImGui::SetCursorScreenPos(pos);
	

	if (Raspberry_guidata.GuiState == GuiState_LfoSelect)
	{
		ImGui::Text("LFO assign");
		if (Raspberry_guidata.editLfo != 0) {
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
			ImGui::LabelText("Speed", "%d", Raspberry_guidata.editLfo->speed);
			ImGui::LabelText("Shape", "%d", Raspberry_guidata.editLfo->shape);
			ImGui::PopStyleVar();
			TargetsList();
		}
	}
	else if (Raspberry_guidata.GuiState == GuiState_AdsrSelect)
	{
		ImGui::Text("ADSR assign");
		if (Raspberry_guidata.editAdsr != 0) {
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
			ImGui::LabelText("A", "%d", Raspberry_guidata.editAdsr->a);
			ImGui::LabelText("D", "%d", Raspberry_guidata.editAdsr->d);
			ImGui::LabelText("S", "%d", Raspberry_guidata.editAdsr->s);
			ImGui::LabelText("R", "%d", Raspberry_guidata.editAdsr->r);
			ImGui::PopStyleVar();
			TargetsList();
		}
	}
	else if (Raspberry_guidata.GuiState == GuiState_AdSelect)
	{
		ImGui::Text("AD assign");
		if (Raspberry_guidata.editAd != 0) {
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
			ImGui::LabelText("A", "%d", Raspberry_guidata.editAd->a);
			ImGui::LabelText("D", "%d", Raspberry_guidata.editAd->d);
			ImGui::PopStyleVar();
			TargetsList();
		}
	}
	else if (Raspberry_guidata.GuiState == GuiState_CtrlSelect)
	{
		ImGui::Text("Controller assign");
		TargetsList();
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