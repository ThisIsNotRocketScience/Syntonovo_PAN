#include <stdio.h>
#include "NewModulationModal.h"

NewModulationModal *TheNewModulationModal;

NewModulationModal::NewModulationModal()
{
	EnableButton(LB1, "Back", MenuEntry_Action, MenuAction_Ok);

	ControlsInOrder.push_back(&this->TitleText);

	AddText(200, 20, "New modulation to");
	TitleText.SetTitle("....");
	TitleText.x = 200;
	TitleText.y = 60;


	EnableButton(LB3, "Mod Left");
	EnableButton(LB4, "Sustain Left");
	EnableButton(LB5, "Una corda Left");
	
	EnableButton(LB6, "ZPrime");
	EnableButton(LB7, "LFO");
	EnableButton(LB8, "Envelope");

	EnableButton(LB9, "Velocity");



	EnableButton(RB3, "Mod left");

	EnableButton(RB4, "Mod Right");
	EnableButton(RB5, "Sustain Right");
	EnableButton(RB6, "Una corda Left");
		
	EnableButton(RB7, "X");
	EnableButton(RB8, "Y");
	//EnableButton(14, "Z");
}

void NewModulationModal::Setup(OutputEnum target)
{
	CurrentTarget = target;
	char txt[300];
	GetModulationTargetLongName(target, txt, 300);
	TitleText.SetTitle(txt);
}

void NewModulationModal::Render(bool active, float DT)
{
	_screensetup_t::Render(active, DT);
	/*
	gCurrentPreset.GetModulationValue(ModSource_t::Source_left_mod);
	ModSource_t::Source_left_unac;
	ModSource_t::Source_left_sus;

	ModSource_t::Source_right_mod;
	ModSource_t::Source_right_sus;
	ModSource_t::Source_right_unac;

	ModSource_t::Source_x;
	ModSource_t::Source_y;
	ModSource_t::Source_z;

	*/

}

void NewModulationModal::Action(int action)
{
	_screensetup_t::Action(action);
	switch (action)
	{
		case MenuAction_Ok:
		{
			Parent->Action(MenuAction_CloseModal);
		}break;
	}
}
