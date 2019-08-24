#pragma once
#include "FinalPanEnums.h"
#include "PanPreset.h"
#include "gui.h"
#include "ModTargetModal.h"

class ModSourceSelectorModalScreen : public _screensetup_t
{
public: 
	ModSourceSelectorModalScreen();
	virtual void Action(int action);
	virtual void Render(bool active, float DT);
};