#include <stdio.h>
#include "HomeScreen.h"

extern presetnames_t presetnames;



HomeScreen::HomeScreen()
{
	this->ControlsInOrder.push_back(new BankList(512-320, 100, &gPanState.BankLeft,"Left" ));
	this->ControlsInOrder.push_back(new BankList(512+20, 100, &gPanState.BankRight,"Right" ));
}

