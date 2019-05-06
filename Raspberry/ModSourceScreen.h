#pragma once

#include "FinalPanEnums.h"
#include "PanPreset.h"
#include "gui.h"
#include "ModTargetModal.h"

class ModSourceScreen : public _screensetup_t
{
public:
	Screens_t myScreen;
	int ActiveInstance;
	int MaxInstances;

	ModSource_t modType;
	bool HasActiveInstanceDisplay;
	virtual uint16_t GetParameterValue(int param, int encoderset);
	virtual void TweakParameterValue(int param, int delta);
	ModSource_t ModTypeFromScreen(Screens_t screen);
	ModSourceScreen(Screens_t screen);
	virtual void Action(int a);
	uint16_t GetModValue(int v);
	
	virtual void EncoderPress(FinalEncoderEnum button);
	
	ModTargetModal theModTargetModal;
	virtual void Deactivate();
	virtual void RenderContent(bool active, float DT);
	virtual void RepeatGoto();
	int FindNextUsed(int start);

	void OpenTargetModal(int modtarget);
	virtual void Render(bool active, float DT);
	virtual void Activate();
	void SetEncoderNames();
};
