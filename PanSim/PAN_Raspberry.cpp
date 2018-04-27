#include "../libs/imgui-master/imgui.h"
#ifdef WIN32
#include <GL/gl3w.h> 
#else

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

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


extern Raspberry_GuiData_t Raspberry_guidata;
static Raspberry_GuiResources_t res;


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
	
	ModTarget_t *t = 0;
	switch (Raspberry_guidata.GuiState)
	{
	case GuiState_AdSelect: t = Raspberry_guidata.dataAd.target; break;
	case GuiState_AdsrSelect: t = Raspberry_guidata.dataAdsr.target; break;
	case GuiState_CtrlSelect: t = Raspberry_guidata.dataCtrl.target; break;
	case GuiState_LfoSelect: t = Raspberry_guidata.dataLfo.target; break;
	}
	
	if (!t) return;

	ImGui::Columns(2);

	for (int i = 0; i < 16; i++) {
		if (i == Raspberry_guidata.selectTarget) {
			ImGui::PushStyleColor(ImGuiCol_Text, 0xFFFFFFFF);
		}
		if (t[i].param != 0) {
			ImGui::Text(ParamLabel(t[i].param)); ImGui::NextColumn();
			ImGui::Text("%1.3f", DepthLabel(t[i].depth)); ImGui::NextColumn();
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
	ImGui::SetCursorPos(ImVec2(240 - sz.x-10 , cp.y));
	ImGui::Text(name);
	rowstate.left =  left ;
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
	ImVec2 p = ImGui::GetCursorPos();
	w.x = rowstate.right;
	int id = ((Raspberry_guidata.switches[0] & (1<<param) )> 0) ? 1 : 0;
	if (rowstate.active) id += 2;
	ImGui::SetCursorPos(ImVec2(245, p1.y));
	ImGui::Image(res.OnOff[id], ImVec2(128, 48));
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
	float distance[5] = { 0,10,30,40,50 };
	float len[5] = { 1.0,0.8,0.6,0.6,0.6 };

	for (int i = 0; i <5 ; i ++)
	{
		float M = ((float)(i) / (float)5);
		M *= M;
		float x = w - (distance[i] * 2)*M;
		float L = (ImGui::GetTextLineHeight() * len[i] + 20)* pt;
		ImGui::GetWindowDrawList()->AddLine(ImVec2(curspos.x + x, curspos.y-2), ImVec2(curspos.x + x, curspos.y+L), IM_COL32(255, 255, 255, 255), 1 + (float)(5-i)*3);
		ImGui::GetWindowDrawList()->AddLine(ImVec2(curspos.x + 480-x, curspos.y-2), ImVec2(curspos.x + 480-x, curspos.y+L), IM_COL32(255, 255, 255, 255), 1+(float)(5-i)*3);
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
	yy += ImGui::GetTextLineHeight() -10;
	ImGui::SetCursorScreenPos(ImVec2(xx + rowstate.left - 42, yy - 32));
	ImGui::Image(res.LeftIndicator, ImVec2(32, 32));
	ImGui::GetWindowDrawList()->AddLine(ImVec2(xx + res.encoderbarmargin, yy ), ImVec2(xx+rowstate.left-10, yy), res.Highlight, 4);
	ImGui::GetWindowDrawList()->AddLine(ImVec2(xx + res.encoderbarmargin,yy), ImVec2(xx+ res.encoderbarmargin,res.encoderheight), res.Highlight, 4);
	ImGui::GetWindowDrawList()->AddLine(ImVec2(xx-10, res.encoderheight), ImVec2(xx + res.encoderbarmargin, res.encoderheight), res.Highlight, 4);
	ImGui::SetCursorPos(B);
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
		ImGui::Image(res.BgImages[GuiState_Boot], ImVec2(480, 800));
	}
	else
	{
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 480, pos.y + 800), res.BGColor);
	}
}
void Raspberry_RenderScreen()
{
	if (BootTime < 10)
	{
		BootTime++;
		RenderBootScreen();
		return;
	}
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
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
		ImGui::LabelText("Speed", "%d", Raspberry_guidata.dataLfo.speed);
		ImGui::LabelText("Shape", "%d", Raspberry_guidata.dataLfo.shape);
		ImGui::PopStyleVar();
		TargetsList();
		
		RenderEndMenu();
	}
	else if (Raspberry_guidata.GuiState == GuiState_AdsrSelect)
	{
		RenderStartMenu("ADSR assign");
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
		ImGui::LabelText("A", "%d", Raspberry_guidata.dataAdsr.a);
		ImGui::LabelText("D", "%d", Raspberry_guidata.dataAdsr.d);
		ImGui::LabelText("S", "%d", Raspberry_guidata.dataAdsr.s);
		ImGui::LabelText("R", "%d", Raspberry_guidata.dataAdsr.r);
		ImGui::PopStyleVar();
		TargetsList();
		
		RenderEndMenu();
	}
	else if (Raspberry_guidata.GuiState == GuiState_AdSelect)
	{
		RenderStartMenu("AD assign");
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
			ImGui::LabelText("A", "%d", Raspberry_guidata.dataAd.a);
			ImGui::LabelText("D", "%d", Raspberry_guidata.dataAd.d);
			ImGui::PopStyleVar();
			TargetsList();
		
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
		const char ControllerNames[__ModSource_COUNT][20] = { "NONE", "Left mod","Right mod","X-pression","Y-pression","Z-pression","Z'-pression","Note"," Velocity" };
		RenderStartMenu(ControllerNames[Raspberry_guidata.dataCtrl.source] );
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




void Raspberry_WindowFrame()
{

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

	ImGui::Begin("screen", 0,   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar|  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);

	ImGui::SetWindowSize(ImVec2(480, 800));


	Raspberry_RenderScreen();

	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}
