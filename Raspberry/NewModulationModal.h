
#pragma once

#include "gui.h"


#include "PanPreset.h"

class NewModulationModal : public _screensetup_t
{
public:
	NewModulationModal();

	void Setup(OutputEnum target);

	virtual	void Render(bool active, float DT);

	virtual void Action(int action);

	_textcontrol_t TitleText;
	OutputEnum CurrentTarget;

	int Current;
};

extern NewModulationModal *TheNewModulationModal;