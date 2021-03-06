#pragma once
#include <vector>
#include "../libs/imgui-master/imgui.h"
#include "FinalPanEnums.h"
#include "FinalPanHeader.h"

#pragma warning( disable : 4244)

#ifndef PANGUIH
#define PANGUIH
#define HEADERHEIGHT 30

#include "PanPreset.h"

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

enum
{
	LB1,
	LB2,
	LB3,
	LB4,
	LB5,
	LB6,
	LB7,
	LB8,
	LB9,
	LB10,
	RB1,
	RB2,
	RB3,
	RB4,
	RB5,
	RB6,
	RB7,
	RB8,
	RB9,
	RB10,
	SIDEBUTTON_COUNT
};


#define SIDEBUTTON_PER_COLUMN 10

extern void cmd_pad_zero();
extern void cmd_calibrate();
extern void cmd_AddCalibrationByte(unsigned char cmd);
extern void cmd_preset_load(int presetid);
extern void cmd_preset_save(int presetid);


extern int ButtonHeight(int idx);
extern int MButtonHeight(int idx);
extern int GetEncoderX(int id);
extern void RenderLettersInABox(int x, int y, bool active, const char *text, int w, int h, bool notghosted = true, int value = 0, bool unipolar = false, int idx = 0, int total = 1);
ImU32 Dimmed(int dim, ImU32 col);

extern uint16_t lerp(uint16_t i, uint16_t f, uint16_t t);
extern void LedLerp(bool active, uint16_t value, uint16_t *r, uint16_t *g, uint16_t *b);
extern void VerticalText(char *text, alignment_t align = align_left, ImU32 text_color = 0xffffffff);

extern void BuildModulationTargetList();
extern const char *GetModulationTargetName(int Output);
extern void GetModulationTargetLongName(int Output, char *txt, int len);

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
extern int DecodeCurrentEffect();
extern void SetEffect(int effect);
extern void RenderModulationAmountV(int x, int y, bool active, int w, int h, int value, bool unipolardisplay, bool notghosted);
extern void RenderModulationAmountH(int x, int y, bool active, int w, int h, int value, bool unipolardisplay, bool notghosted);

typedef struct FinalPan_GuiResources_t
{
	ImTextureID MainBG;
	ImTextureID RootBG;
	ImTextureID GotoIconLeft;
	ImTextureID GotoIconRight;

	ImTextureID TestBG;
	ImTextureID VCF2;
	ImTextureID GraphBG;
	ImTextureID F[__FILT_COUNT];
	ImTextureID Fs[__FILT_STRUCT_COUNT];
	ImTextureID Plugs[__PLUGS_COUNT];
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
	
	ImU32 WhiteKey;
	ImU32 BlackKey;
	ImU32 WhiteKeyActive;
	ImU32 BlackKeyActive;

	ImU32 KeyRangeColor[4];


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
	virtual void SetupEncoderSet(int n);

	virtual void Render(bool active, float DT);
	virtual void Action(int action);
	virtual void SketchRightPressed();

	virtual void Activate();
	virtual void Deactivate();

	void RenderBox(int x, int y, int val, int mode, bool active);
	void RenderBoxVertical(int x, int y, int val, int mode, bool active);
	void RenderBoxHorizontal(int x, int y, int val, int mode, bool active);
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
		x = y = 0;
		dynamic = dyn;
		src = dynpointer;
		Color = ImVec4(1, 1, 1, 1);
		skipencodercycling = true;
		Align = alignment_t::align_left;
		fontsize = font_size::font_small;
	}
	float x, y;
		alignment_t Align;
	ImVec4 Color;
	virtual void Render(bool active, float DT);

};

enum icons
{
	Icon_NO,
	Icon_ON,
	Icon_OFF,
	Icon_GOTOL,
	Icon_GOTOR,
	Icon___ICONCOUNT
};

class sidebutton_t : public _control_t
{
public:
	void UpdateLed(bool active);
	icons myIcon;
	sidebutton_t();
	int x;
	int lx2;
	int y;
	int y2;
	int style;
	int target;
	int Instance;

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
		IsDirectOutput = false;
		ledmode = ledmodes::ledmode_off;
		r = g = b = 255;
		ModSourceCount = 0;
		EncoderID = 0;
		x = y = 0;
		Align = alignment_t::align_left;
	}
	alignment_t Align;
	ledmodes ledmode;
	int EncoderID;
	uint16_t r, g, b;
	
	int Set;
	
	//int ParameterID;
	int x, y;
	
	ModSourcesForOutputStruct MSFOS;

	bool IsDirectOutput;
	int ModSourceCount;
	
	void SetupPosition(int id);
	void Turn(int delta);
	
	void UpdateLed(bool active);
	
	virtual void Render(bool active, float DT);
};
#define MAXENCODERSETS 2

class _screensetup_t : public _control_t
{
public:

	int ScreenID;
	ImTextureID BG;
	//std::vector<_screensetup_t *> SubScreens;
	virtual void Deactivate()
	{
		if (Modal)
		{
			Modal->Deactivate();
			Modal = NULL;
		}
	}
	std::vector<_control_t *> ControlsInOrder;
	virtual void RepeatGoto() 
	{
		if (Modal)
		{
			Modal->Deactivate();
			Modal = NULL;
		}
	};

	virtual void RegisterNewChild(_control_t *newcontrol) { ControlsInOrder.push_back(newcontrol); }
	int ActiveControl;
	_screensetup_t *Modal;

	std::vector<int> EncodersThatOpenThisScreen;
	std::vector<int> LedButtonsThatOpenThisScreen;

	_screensetup_t(int ScreenID, _screensetup_t *parent = NULL);
	virtual uint16_t GetParameterValue(int param, int encoderset);
	virtual void TweakParameterValue(int param, int delta) ;
	virtual void SetupEncoderSet(int n);

	void RenderPatchBox();

	void AddVCONextPrev();
	void AddVCF2NextPrev();
	void SetupMainUILeds();
	void SetupEncodersAndButtonsLeds();
	void SetupKeyboardStateLeds();

	virtual void SetupLeds();
	virtual void Action(int action);

	virtual bool GetToggle(int id) { return false; };
	virtual void DoToggle(int id) {};

	void SetFirstEnabledControlActive();
	void DisableEncoder(int i);

	void DisableButton(int i);

	bool EnableButton(int i, const char *text, int style = MenuEntry_NoStyle, int target = -1, bool active = true, ledmodes l = ledmode_solid);

	int EnableAvailableEncoder(const char *text, int style, int target, int startidx, bool isdirectoutput = false);
	int SkipAvailableEncoder(int startidx);

	int EnableAvailableButton(const char *text, int style, int target);

	void ButtonStyle(int i, int style, int target);

	sidebutton_t buttons[SIDEBUTTON_COUNT];

	bottomencoder_t encoders[MAXENCODERSETS][11];

	_textcontrol_t *AddText(float x, float y, const char *t, alignment_t align = align_left, font_size fontsize = font_small);
	void AddDynamicText(float x, float y, char *t, int len, alignment_t align = align_left, font_size fontsize = font_small);
	
	void AddLedControl(const char *name, int x, int y, LedTheme whichled);

	void ChangeActiveControl(int delta);
	void OpenKeyZoneModal();
	void CloseParentModal();

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
	virtual int GetActiveInstance() { return 0; }
};


class BankList : public _control_t
{
	int* bankid;

public:
	int x, y;
	void SetBankPointer(int* p)
	{
		bankid = p;
	}
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


class ModSourceScreen;
class Gui
{
public:
	Gui();
	ModSourceScreen *AddModSourceScreen(Screens_t screen, ModSource_t mod, const char *basetitle);
	uint32_t GetNumberColor(int idx, int total);
	void PrintSwitchName(char *txt, int strlen, SwitchEnum idx);
	void PrintSwitchDesc(char* txt, int strlen, SwitchEnum idx);
	void GotoPageForMod(ModSource_t mod, int instance);
	void GotoPageForKeyZone(int zone);
	void GotoPageForArpeggiator(int arp);
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

	_screensetup_t *Screens[__SCREENS_COUNT];

	
	void BuildScreens();

	virtual	void Render(bool active, float DT);

	void GotoPage(Screens_t s);

	void SetupLeds();

	void NextVCO();
	void PrevVCO();
	void NextVCF2();
	void PrevVCF2();

	std::vector< ModSourceScreen *> ModSourceScreens;

	Screens_t CurrentScreen;
};

extern Gui gGui;
class ParameterModal;
extern ParameterModal  *theParameterBindingModal;
extern int8_t mod_values[128];
extern bool isPageEncoder(FinalEncoderEnum Button);
extern Screens_t GetPage(FinalEncoderEnum Button);


#endif
