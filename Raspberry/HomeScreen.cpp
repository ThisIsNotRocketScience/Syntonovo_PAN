#include <stdio.h>
#include "HomeScreen.h"

extern presetnames_t presetnames;



HomeScreen::HomeScreen()
{
	this->ControlsInOrder.push_back(new BankList(512-20 - ParamBankWidth, 150, &gPanState.BankLeft,"Left" ));
	this->ControlsInOrder.push_back(new BankList(512+20, 150, &gPanState.BankRight,"Right" ));
}

