#pragma once

//#define SIMULATEINCOMINGSERIAL

#ifndef SYNTONPAN
#define SYNTONPAN
#include <string>



#include "PanPreset.h"

#pragma pack()
/*
inline GuiState_t ButtonToMenu(int buttonid)
{
#define EXTRABUTTON(id, button) if (buttonid == button) return GuiState_Menu_##id;
#define MENU(id, button, name) if (buttonid == button) return GuiState_Menu_##id;
#include "PanUiMap.h"
#undef MENU
#undef EXTRABUTTON
	return GuiState_Root;
}

inline bool IsMenuState(GuiState_t state)
{
#define MENU(id, button, name) \
	if (state == GuiState_Menu_##id) return true;
#include "PanUiMap.h"
#undef MENU
	return false;
}*/


#endif