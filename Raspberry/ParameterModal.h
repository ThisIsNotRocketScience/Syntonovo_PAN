#pragma once

#include "gui.h"


#include "PanPreset.h"

class ParameterModal : public _screensetup_t
{
public:
	ParameterModal();

	void Setup(OutputEnum target);

	virtual	void Render(bool active, float DT);
	virtual void Action(int action);

	_textcontrol_t TitleText;
	_textcontrol_t DescText;

	int CurrentMod;
	OutputEnum CurrentTarget;

	virtual void SketchRightDelta(int delta);
	virtual void SketchLeftDelta(int delta);

	void SetupTargetButton();

	virtual void TweakParameterValue(int param, int delta);
	virtual uint16_t GetParameterValue(int param, int encoderset);
	virtual void SideButton(FinalLedButtonEnum b);
	ModSourcesForOutputStruct SourcesForCurrentTarget;
	
};


