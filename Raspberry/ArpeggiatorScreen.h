#pragma once

#include "FinalPanEnums.h"
#include "PanPreset.h"
#include "gui.h"
#include "ModTargetModal.h"

class ArpeggiatorScreen: public _screensetup_t
{
public:
	int ActiveInstance;
	int MaxInstances;
	
	virtual uint16_t GetParameterValue(int param, int encoderset);
	virtual void TweakParameterValue(int param, int delta);
	
	ArpeggiatorScreen();
	virtual void Action(int a);

	virtual void EncoderPress(FinalEncoderEnum button);
	
	void SetActiveInstance(int id);

	virtual void Deactivate();
	virtual void RenderContent(bool active, float DT);
	virtual void RepeatGoto();

	virtual void Render(bool active, float DT);
	virtual void Activate();

	virtual bool GetToggle(int id);
	virtual void DoToggle(int id);

	ArpSettings_t* settings;
};
