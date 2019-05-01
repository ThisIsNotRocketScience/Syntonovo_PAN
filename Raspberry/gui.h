#pragma once
#include <vector>
#include "../libs/imgui-master/imgui.h"


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
extern void RenderLettersInABox(int x, int y, bool active, const char *text, int w, int h);

extern uint16_t lerp(uint16_t i, uint16_t f, uint16_t t);
extern void LedLerp(bool active, uint16_t value, uint16_t *r, uint16_t *g, uint16_t *b);
extern void VerticalText(char *text, alignment_t align = align_left, ImU32 text_color = 0xffffffff);

typedef struct FinalPan_GuiResources_t
{
	ImTextureID MainBG;
	ImTextureID RootBG;
	ImTextureID TestBG;
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
	ImU32 Normal;
	ImU32 BGColor;
	ImU32 FillColor;
	int PageTime;
	int encoderbarmargin;
	int encoderheight;

	ImTextureID BgImages[1];
} FinalPan_GuiResources_t;

extern FinalPan_GuiResources_t gGuiResources; 

class _control_t
{
public:

	virtual void SketchRightDelta(int delta);
	virtual uint16_t GetParameterValue(int param) { return 0; };
	virtual void TweakParameterValue(int param, int delta) { };

	int style;
	int target;

	_control_t *Parent;

	_control_t();

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
	int y;
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

	std::vector<_screensetup_t *> SubScreens;

	std::vector<_control_t *> ControlsInOrder;
	int ActiveControl;
	_screensetup_t *Modal;

	std::vector<int> EncodersThatOpenThisScreen;
	std::vector<int> LedButtonsThatOpenThisScreen;

	_screensetup_t(_screensetup_t *parent = NULL);
	virtual uint16_t GetParameterValue(int param);
	virtual void TweakParameterValue(int param, int delta) ;
	virtual void SetupEncoderSet(int n);



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
	virtual void Render(float DT);
};

class Gui
{
public:
	Gui();
	void Init();
	void SketchLeftPress();
	void SketchRightPress();

	void EncoderPress(FinalEncoderEnum button);

	void SketchLeft(int delta);

	void SketchRight(int delta);

	void Encoder(FinalEncoderEnum button, int delta);

	void ButtonPressed(FinalLedButtonEnum Button);

	_screensetup_t *CS();

	_screensetup_t *Screens[SCREENS_COUNT];

	void BuildScreens();

	virtual	void Render(float DT);

	void GotoPage(Screens_t s);

	void SetupLeds();

	Screens_t CurrentScreen;
};


