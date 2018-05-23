#ifndef MENU
#define MENU(sourceid, title, structname)
#define MENUREDEFINED
#endif

#ifndef ENDMENU
#define ENDMENU()
#define ENDMENUREDEFINED
#endif

#ifndef KV
#define KV(name, subparam, variable)
#define KVREDEFINED
#endif

#ifndef PARA
#define PARA(name, variable)
#define PARAREDEFINED
#endif

#ifndef SWITCH2
#define SWITCH2(name, variable)
#define SWITCH2REDEFINED
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
KV("Speed", Sub_lfo_speed, dataLfo.speed)
KV("Shape", Sub_lfo_shape, dataLfo.shape)
ENDMENU()

MENU(GuiState_AdsrSelect, "ADSR Assign", dataAdsr)
KV("Attack", Sub_adsr_a, dataAdsr.a)
KV("Decay", Sub_adsr_d, dataAdsr.d)
KV("Sustain", Sub_adsr_s, dataAdsr.s)
KV("Release", Sub_adsr_r, dataAdsr.r)
//SWITCH2("Retrigger", dataAdsr.r)
ENDMENU()

MENU(GuiState_AdSelect, "AD Assign", dataAd)
KV("Attack", Sub_ad_a, dataAd.a)
KV("Decay", Sub_ad_d, dataAd.d)
ENDMENU()

MENU(GuiState_CtrlSelect, "Control Assign", dataCtrl)

CTRLMENU(Source_left_mod,"Left Mod")
PARA("Scale", Output_MODL_SCALE)
CTRLENDMENU()

CTRLMENU(Source_right_mod, "Right Mod")
PARA("Scale", Output_MODR_SCALE)
CTRLENDMENU()

CTRLMENU(Source_left_sus, "Left Sus")
PARA("Scale", Output_SUSL_SCALE)
SWITCH2("Sustain", Switch_SELSUSTAINL)
CTRLENDMENU()

CTRLMENU(Source_right_sus, "Right Sus")
PARA("Scale", Output_SUSR_SCALE)
SWITCH2("Sustain", Switch_SELSUSTAINR)
CTRLENDMENU()

CTRLMENU(Source_left_unac, "Left UnaC")
PARA("Scale", Output_UNACL_SCALE)
SWITCH2("Una Corda", Switch_SELUNACL)
CTRLENDMENU()

CTRLMENU(Source_right_unac, "Right UnaC")
PARA("Scale", Output_UNACR_SCALE)
SWITCH2("Una Corda", Switch_SELUNACR)
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

#ifdef SWITCH2REDEFINED
#undef SWITCH2REDEFINED
#undef SWITCH2
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

