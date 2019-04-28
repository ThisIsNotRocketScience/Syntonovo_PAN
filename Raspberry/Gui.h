#pragma once


enum alignment_t
{
	align_left,
	align_right,
	align_center,
	__align_count
};


class _control_t
{
public:

	virtual void SketchRightDelta(int delta);

	int style;
	int target;

	_control_t *Parent;

	_control_t();

	bool enabled;
	bool skipencodercycling;
	char title[255];
	bool activestate;
	void SetTitle(char *t);

	virtual void Render(bool active);
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
	_textcontrol_t()
	{
		Color = ImVec4(1, 1, 1, 1);
		skipencodercycling = true;
	}
	float x, y;
	float fontsize;
	alignment_t Align;
	ImVec4 Color;
	virtual void Render(bool active)
	{
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
	}

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

	virtual void Render(bool active);
};



class  bottomencoder_t :public _control_t
{
public:

	alignment_t Align;
	ledmodes ledmode;
	uint16_t r, g, b;

	int ParameterID;
	int x, y;

	void SetupPosition(int id);
	void Turn(int delta);
	
	void UpdateLed(bool active);
	
	virtual void Render(bool active);
};


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


	virtual void SetupLeds();
	virtual void Action(int action);

	void SetFirstEnabledControlActive();
	void DisableEncoder(int i);

	void DisableButton(int i);

	bool EnableButton(int i, char *text, int style = MenuEntry_NoStyle, int target = -1, bool active = true, ledmodes l = ledmode_solid);

	int EnableAvailableEncoder(char *text, int style, int target);

	int EnableAvailableButton(char *text, int style, int target);

	void ButtonStyle(int i, int style, int target);

	sidebutton_t buttons[14];
	bottomencoder_t encoders[11];
	void AddText(float x, float y, char *t, alignment_t align = align_left);

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
	virtual void Render();
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

	virtual	void Render();

	void GotoPage(Screens_t s);

	void SetupLeds();

	Screens_t CurrentScreen;
};


