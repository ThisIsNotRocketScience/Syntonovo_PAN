#pragma once
#include <vector>
#include "../libs/imgui-master/imgui.h"
#include "FinalPanEnums.h"
#include "FinalPanHeader.h"

#ifndef PANGUIH
#define PANGUIH

enum alignment_t
{
	align_left,
	align_right,
	align_center,
	__align_count
};

enum font_size
{
	font_tiny,
	font_small,
	font_medium,
	font_large
};

extern int ButtonHeight(int idx);
extern int MButtonHeight(int idx);
extern int GetEncoderX(int id);
extern void RenderLettersInABox(int x, int y, bool active, const char *text, int w, int h, bool notghosted = true);
ImU32 Dimmed(int dim, ImU32 col);

extern uint16_t lerp(uint16_t i, uint16_t f, uint16_t t);
extern void LedLerp(bool active, uint16_t value, uint16_t *r, uint16_t *g, uint16_t *b);
extern void VerticalText(char *text, alignment_t align = align_left, ImU32 text_color = 0xffffffff);

extern void BuildModulationTargetList();
const char *GetModulationTargetName(int Output);

extern bool IsCenterEncoder(FinalEncoderEnum button);
extern int GetAssociatedParameter(FinalEncoderEnum button);
extern int ButtonHeight(int idx);

extern void FinalPan_SetupDefaultPreset();
extern int GetSideButtonID(FinalLedButtonEnum B);
extern bool IsSideButton(FinalLedButtonEnum B);
extern bool IsPatchButton(FinalLedButtonEnum B);


extern void LoadSelectedPreset();
extern void LoadPatch(int n);
extern bool ModulationSourceHasInstances(ModSource_t modType);


typedef struct FinalPan_GuiResources_t
{
	ImTextureID MainBG;
	ImTextureID RootBG;
	ImTextureID TestBG;
	ImTextureID VCF2;
	ImTextureID F[__FILT_COUNT];
	ImTextureID Fs[__FILT_STRUCT_COUNT];
	ImTextureID LogoScreen;
	ImTextureID LeftIndicator;
	ImTextureID RightIndicator;
	ImTextureID SpriteSheet;
	ImTextureID OnOff[4];
	ImFont *BigFont;
	ImFont *TinyFont;
	ImFont *SmallFont;
	ImFont *MediumFont;
	ImU32 Highlight;

	ImU32 GhostBG;
	ImU32 Normal;
	ImU32 BGColor;
	ImU32 ModalBGColor;
	ImU32 FillColor;
	int PageTime;
	int encoderbarmargin;
	int encoderheight;

	ImTextureID BgImages[1];

	bool referencelines;
	bool testimage;
} FinalPan_GuiResources_t;

extern FinalPan_GuiResources_t gGuiResources; 

class _control_t
{
public:

	virtual void SketchRightDelta(int delta);
	virtual uint16_t GetParameterValue(int param, int encodersset) { return 0; };
	virtual void TweakParameterValue(int param, int delta) { };

	int style;
	int target;

	_control_t *Parent;
	virtual void AddTo(_control_t *newp)
	{
		if (Parent)
		{
			printf("need to remove parent first! ");
			*(int32_t*)0 = 1;
		}
		Parent = newp;
		if (Parent) Parent->RegisterNewChild(this);
	};
	_control_t();
	virtual void RegisterNewChild(_control_t *newcontrol) {}
	bool enabled;
	bool skipencodercycling;
	char title[255];
	bool activestate;
	void SetTitle(const char *t);

	int encodersets;
	int currentencoderset;
	virtual void SetupEncoderSet(int n) { currentencoderset = n; };

	virtual void Render(bool active, float DT);
	virtual void Action(int action);
	virtual void SketchRightPressed();

	virtual void Activate();
	virtual void Deactivate();

	void RenderBox(int x, int y, int val, int mode, bool active);
	void RenderBoxVertical(int x, int y, int val, int mode, bool active);
};

class EncoderLineDisplay : public _control_t
{
public:
	EncoderLineDisplay()
	{
		toSet = 0;
		HideIfSetIsInactive = false;
		fromX = 100;
		fromY = 400;
		skipencodercycling = true;
		enabled = true;
	}
	virtual void Render(bool active, float dt);
	
	int fromX;
	int fromY;
	std::vector<int> toEncoders;
	int toSet;
	bool HideIfSetIsInactive;
};

class _textcontrol_t : public _control_t
{
public:
	bool dynamic;
	char *src;
	font_size fontsize;
	_textcontrol_t(bool dyn = false, char *dynpointer = 0)
	{
		dynamic = dyn;
		src = dynpointer;
		Color = ImVec4(1, 1, 1, 1);
		skipencodercycling = true;
	}
	float x, y;
		alignment_t Align;
	ImVec4 Color;
	virtual void Render(bool active, float DT);

};

class sidebutton_t : public _control_t
{
public:
	void UpdateLed(bool active);

	int x;
	int lx2;
	int y;
	int y2;
	int style;
	int target;

	ledmodes ledmode;

	alignment_t Align;

	virtual void Pressed();

	virtual void SketchRightPressed();

	void SetupPosition(int id);

	virtual void Render(bool active, float DT);
};



class  bottomencoder_t :public _control_t
{
public:
	bottomencoder_t()
	{
		Set = 0;
	}
	alignment_t Align;
	ledmodes ledmode;
	uint16_t r, g, b;
	
	int Set;
	
	int ParameterID;
	int x, y;

	void SetupPosition(int id);
	void Turn(int delta);
	
	void UpdateLed(bool active);
	
	virtual void Render(bool active, float DT);
};
#define MAXENCODERSETS 2

class _screensetup_t : public _control_t
{
public:

	ImTextureID BG;
	//std::vector<_screensetup_t *> SubScreens;

	std::vector<_control_t *> ControlsInOrder;
	virtual void RepeatGoto() {};
	virtual void RegisterNewChild(_control_t *newcontrol) { ControlsInOrder.push_back(newcontrol); }
	int ActiveControl;
	_screensetup_t *Modal;

	std::vector<int> EncodersThatOpenThisScreen;
	std::vector<int> LedButtonsThatOpenThisScreen;

	_screensetup_t(_screensetup_t *parent = NULL);
	virtual uint16_t GetParameterValue(int param, int encoderset);
	virtual void TweakParameterValue(int param, int delta) ;
	virtual void SetupEncoderSet(int n);

	void AddVCONextPrev();
	void AddVCF2NextPrev();
	void SetupMainUILeds();
	void SetupEncodersAndButtonsLeds();
	void SetupKeyboardStateLeds();

	virtual void SetupLeds();
	virtual void Action(int action);

	void SetFirstEnabledControlActive();
	void DisableEncoder(int i);

	void DisableButton(int i);

	bool EnableButton(int i, const char *text, int style = MenuEntry_NoStyle, int target = -1, bool active = true, ledmodes l = ledmode_solid);

	int EnableAvailableEncoder(char *text, int style, int target);

	int EnableAvailableButton(char *text, int style, int target);

	void ButtonStyle(int i, int style, int target);

	sidebutton_t buttons[14];

	bottomencoder_t encoders[MAXENCODERSETS][11];

	void AddText(float x, float y, char *t, alignment_t align = align_left, font_size fontsize = font_small);
	void AddDynamicText(float x, float y, char *t, int len, alignment_t align = align_left, font_size fontsize = font_small);
	
	void AddLedControl(const char *name, int x, int y, LedTheme whichled);

	void ChangeActiveControl(int delta);

	virtual void SketchLeft(int delta);

	virtual void SketchRight(int delta);

	virtual void SketchLeftPress();

	virtual void SketchRightPress();

	virtual void EncoderPress(FinalEncoderEnum button);

	int GetControlIndex(_control_t *c);
	void SetActiveControl(_control_t *c);
	virtual void SideButton(FinalLedButtonEnum b);
	virtual void PatchButton(FinalLedButtonEnum b);
	void Encoder(FinalEncoderEnum button, int delta);
	
	virtual void RenderContent(bool active, float DT);
	void RenderModalBox(bool active, float DT);
	virtual void Render(bool active, float DT);

};


class BankList : public _control_t
{
public:
	int x, y;
	int *bankid;
	char Title[100];
	BankList(int _x, int _y, int* _bank, const char *title)
	{
		bankid = _bank;
		x = _x;
		y = _y;
		snprintf(Title, 100, title);
		skipencodercycling = true;
	}

	virtual void Render(bool active, float dt)
	{
		ImGui::PushFont(gGuiResources.TinyFont);
		for (int i = 0; i < 8; i++)
		{
			int presetidx = (*bankid) * 16 + i;
			int Y = (ImGui::GetTextLineHeight() + ParamMasterMargin*3)  * i + y;
			ImGui::SetCursorPos(ImVec2(x+ ParamMasterMargin, Y + ParamMasterMargin));
			ImGui::Text(presetnames.names[presetidx]);
			ImGui::GetWindowDrawList()->AddRect(ImVec2(x, Y), ImVec2(x + ParamBankWidth, Y + ImGui::GetTextLineHeight() + ParamMasterMargin*2), gGuiResources.Normal);
		}
		ImGui::PopFont();
	}
};



class Gui
{
public:
	Gui();
	void Init();
	void SketchLeftPress();
	void SketchRightPress();
	bool Waiting;
	void PauseAll(bool paused)
	{
		Waiting = paused;
	}
	void EncoderPress(FinalEncoderEnum button);

	void SketchLeft(int delta);

	void SketchRight(int delta);

	void Encoder(FinalEncoderEnum button, int delta);

	void ButtonPressed(FinalLedButtonEnum Button);

	_screensetup_t *CS();

	_screensetup_t *Screens[SCREENS_COUNT];

	void BuildScreens();

	virtual	void Render(bool active, float DT);

	void GotoPage(Screens_t s);

	void SetupLeds();

	void NextVCO();
	void PrevVCO();
	void NextVCF2();
	void PrevVCF2();

	Screens_t CurrentScreen;
};

extern Gui gGui;
#endif