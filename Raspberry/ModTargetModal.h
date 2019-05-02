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
	virtual void Render(float dt)
	{
		for (int i = 0; i < PageLength; i++)
		{

		}
	}
};

class ModTargetModal : public _screensetup_t
{
public:
	ModTargetModal();

	TargetList TheTargetList;

	virtual void Render(float dt);

	virtual void SketchLeft(int delta) {}

	virtual void SketchRight(int delta) {}
	virtual void SketchLeftPress() {}

	virtual void SketchRightPress() {}

	virtual void EncoderPress(FinalEncoderEnum button) {}

	virtual void SetupLeds();
		
	ModSource_t modType;
	virtual void Action(int action);

	int Instance;
	int TargetID;
	int CurrentTarget;
	int CurrentCategory;
};