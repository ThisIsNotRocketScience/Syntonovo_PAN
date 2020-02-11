#pragma once

#include "gui.h"

class LetterControl : public _control_t
{
public:
	int id;
	int x;
	int y;
	virtual void SketchRightDelta(int delta);
	char GetDeltaChar(char base, int delta);
	virtual void SketchRightPressed();
	LetterControl(int _x, int _y, int _id);
	char Current;
	virtual void Render(bool active, float DT);
};

class PresetScreen : public _screensetup_t
{
public:
	std::vector<LetterControl*> Letters;

	virtual void Action(int action);
	int LastActiveLetter;
	void SetActiveLetter(int N);
	int GetActiveLetter();

	PresetScreen();

	virtual void Activate();

	void AddLetterControl(int x, int y, int id);
};

class SystemScreen : public _screensetup_t
{
public:
	SystemScreen();
	int OscillatorOctave[8];
	bool OscillatorError[8];
	int calibrationcount;

	bool OscillatorReady[8];
	bool CalibrationReady;

	virtual void Render(bool active, float DT);

};

class BankSelectScreen : public _screensetup_t
{
public:

	virtual void Action(int action);

	LeftRight Side;
	int* mybank;
	int df;

	void SetLeftRight(LeftRight lr);

	BankList* list;

	BankSelectScreen();

	virtual void Activate();
};


class SavePresetScreen : public _screensetup_t
{
public:
	virtual void PatchButton(FinalLedButtonEnum b);

	virtual void Action(int action);

	LeftRight Side;
	int* mybank;
	int df;

	void SetLeftRight(LeftRight lr);

	BankList* list;
	SavePresetScreen();
	virtual void Activate();

	void SetNames();
};

class ImageScreen : public _screensetup_t
{
public:
	ImTextureID theImg;
	ImageScreen(ImTextureID img);
	virtual void Render(bool active, float dt);
};

class EffectList : public _control_t
{
public:

	EffectList();
	virtual void SketchRight(int delta);
	virtual void Render(bool active, float dt);
};

