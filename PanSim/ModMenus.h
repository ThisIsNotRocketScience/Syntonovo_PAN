#ifndef MENU
#define MENU(sourceid, title, structname)
#define MENUREDEFINED
#endif

#ifndef ENDMENU
#define ENDMENU()
#define ENDMENUREDEFINED
#endif

#ifndef KV
#define KV(name, variable)
#define KVREDEFINED
#endif

#ifndef PARA
#define PARA(name, variable)
#define PARAREDEFINED
#endif

#ifndef TOGGLE
#define TOGGLE(name, variable)
#define TOGGLEREDEFINED
#endif

#ifndef CTRLMENU
#define CTRLMENU(sourceid, title)
#define CTRLMENUREDEFINED
#endif

#ifndef CTRLENDMENU
#define CTRLENDMENU()
#define CTRLENDMENUREDEFINED
#endif

MENU(GuiState_LfoSelect, "LFO Assign", dataLfo)
KV("Speed", dataLfo.speed)
KV("Shape", dataLfo.shape)
ENDMENU()

MENU(GuiState_AdsrSelect, "ADSR Assign", ctrlAdsr)
KV("Attack", dataAdsr.a)
KV("Decay", dataAdsr.d)
KV("Sustain", dataAdsr.s)
KV("Release", dataAdsr.r)
TOGGLE("Retrigger", dataAdsr.r)
ENDMENU()

MENU(GuiState_AdSelect, "AD Assign", dataAd)
KV("Attack", dataAd.a)
KV("Decay", dataAd.d)
ENDMENU()

MENU(GuiState_CtrlSelect, "Control Assign", dataCtrl)

CTRLMENU(Source_left_mod,"Left Mod")
CTRLENDMENU()

CTRLMENU(Source_right_mod, "Right Mod")
CTRLENDMENU()

CTRLMENU(Source_x,"X-pression")
PARA("Scale", Output_X_SCALE)
PARA("Deadzone", Output_X_DEADZONE)
CTRLENDMENU()

CTRLMENU(Source_y, "Y-pression")
PARA("Scale", Output_Y_SCALE)
PARA("Deadzone", Output_Y_DEADZONE)
CTRLENDMENU()

CTRLMENU(Source_z, "Z-pression")
PARA("Scale", Output_Z_SCALE)
PARA("Deadzone", Output_Z_DEADZONE)
CTRLENDMENU()

CTRLMENU(Source_zprime, "Z'-pression")
PARA("Speed", Output_ZPRIME_SPEED)
CTRLENDMENU()

CTRLMENU(Source_note, "Note")
PARA("Portamento", Output_PORTAMENTO_TIME)
CTRLENDMENU()

CTRLMENU(Source_vel,"Velocity" )
CTRLENDMENU()

ENDMENU()



#ifdef MENUREDEFINED
#undef MENUREDEFINED
#undef MENU
#endif

#ifdef KVREDEFINED
#undef KVREDEFINED
#undef KV
#endif

#ifdef PARAREDEFINED
#undef PARAREDEFINED
#undef PARA
#endif

#ifdef ENDMENUREDEFINED
#undef ENDMENUREDEFINED
#undef ENDMENU
#endif

#ifdef CTRLMENUREDEFINED
#undef CTRLMENUREDEFINED
#undef CTRLMENU
#endif 

#ifdef CTRLENDMENUREDEFINED
#undef CTRLENDMENUREDEFINED
#undef CTRLENDMENU
#endif

#ifdef TOGGLEREDEFINED
#undef TOGGLEREDEFINED
#undef TOGGLE
#endif
