#pragma once

#include "FinalPanEnums.h"
#include "PanPreset.h"
#include "gui.h"

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

	virtual void Render(float DT);
	virtual void Activate();
};
