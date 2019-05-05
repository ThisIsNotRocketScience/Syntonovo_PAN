#include "HomeScreen.h"
extern presetnames_t presetnames;



HomeScreen::HomeScreen()
{
	this->ControlsInOrder.push_back(new BankList(512-300, 100, &gPanState.BankLeft,"Left" ));
	this->ControlsInOrder.push_back(new BankList(512, 100, &gPanState.BankRight,"Right" ));
}