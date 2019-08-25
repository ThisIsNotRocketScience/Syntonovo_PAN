#include "NewModulationModal.h"

NewModulationModal *TheNewModulationModal;

NewModulationModal::NewModulationModal()
{
	EnableButton(0, "Back", MenuEntry_Action, MenuAction_Ok);

	ControlsInOrder.push_back(&this->TitleText);

	AddText(200, 20, "New modulation to");
	TitleText.SetTitle("....");
	TitleText.x = 200;
	TitleText.y = 60;


	EnableButton(1, "Mod Left");
	EnableButton(2, "Sustain Left");
	EnableButton(3, "Una corda Left");
	
	EnableButton(4, "ZPrime");
	EnableButton(5, "LFO");
	EnableButton(6, "Envelope");

	EnableButton(7, "Velocity");
	EnableButton(8, "Mod left");

	EnableButton(7, "Mod Right");
	EnableButton(8, "Sustain Rihgt");
	EnableButton(9, "Una corda Left");
		
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