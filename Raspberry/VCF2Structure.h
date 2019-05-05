#pragma once
#include "FinalPanEnums.h"
#include "PanPreset.h"
#include "gui.h"
#include "ModTargetModal.h"

class VCF2StructureScreen : public _screensetup_t
{
public: 
	VCF2StructureScreen();
	virtual void Action(int action);
	virtual void Render(bool active, float DT);
};