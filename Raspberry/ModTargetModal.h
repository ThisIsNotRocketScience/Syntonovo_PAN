#pragma once

#include "FinalPanEnums.h"
#include "PanPreset.h"
#include "gui.h"

class TargetList : public _control_t
{
public:
	int Current;
	int PageStart;
	int PageEnd;
	int PageLength;
	TargetList()
	{
		Current = 0;
		PageLength = 20;
		SetPage(0);
	}
	void SetPage(int idx)
	{
		PageStart = idx;
		PageEnd = PageStart + PageLength;
	}
	virtual void Render(bool active, float dt);
};

class ModTargetModal : public _screensetup_t
{
public:
	ModTargetModal();

	TargetList TheTargetList;

	virtual void Render(bool active, float dt);

	virtual void SketchLeft(int delta) {}

	virtual void SketchRight(int delta) {}
	virtual void SketchLeftPress() {}

	virtual void SketchRightPress() {}

	virtual void EncoderPress(FinalEncoderEnum button) {}

	virtual void Activate();
	virtual void SetupLeds();
		
	ModSource_t modType;
	virtual void Action(int action);
	
	int OriginalModulation;
	int OriginalOutputID;
	int OriginalSourceID;

	virtual uint16_t GetParameterValue(int param, int encoderset);

	int Instance;
	int TargetID;
	int CurrentTarget;
	int CurrentCategory;
};