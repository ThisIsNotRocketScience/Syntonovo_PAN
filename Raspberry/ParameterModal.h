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

	ModSourcesForOutputStruct SourcesForCurrentTarget;
	
};


