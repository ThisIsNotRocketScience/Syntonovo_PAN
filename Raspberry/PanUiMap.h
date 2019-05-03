#ifndef TARGET 
	#define REDEFINEDTARGET
	#define TARGET(param,buttonid,knobid,name)
#endif

#ifndef EXTRABUTTON
#define REDEFINEEXTRABUTTON
#define EXTRABUTTON(id, buttonid)
#endif

#ifndef MENU
#define REDEFINEDMENU
#define MENU(id,buttonid,name)
#endif

#ifndef ENTRY
#define REDEFINEDENTRY
#define ENTRY(name,entryid,knob)
#endif

#ifndef CUSTOMENTRY
#define REDEFINEDCUSTOMENTRY
#define CUSTOMENTRY(name,entryid,ctrl)
#endif

#ifndef ENDMENU
#define REDEFINEDENDMENU
#define ENDMENU()
#endif


MENU(VCO1, ledbutton_VCO1, "VCO1")
ENTRY("Pitch", MenuEntry_Pitch, Output_VCO1_PITCH)
CUSTOMENTRY("Triangle", MenuEntry_Toggle, Switch_SEL1TRI)
CUSTOMENTRY("Pulse", MenuEntry_Toggle, Switch_SEL1SQR)
CUSTOMENTRY("Saw", MenuEntry_Toggle, Switch_SEL1SAW)
CUSTOMENTRY("Subosc", MenuEntry_Toggle, Switch_SEL1SUB)
CUSTOMENTRY("Sync", MenuEntry_Toggle, Switch_SELVCOSYNC3)
ENTRY("PW", MenuEntry_Percentage, Output_VCO1_PW)
ENDMENU()

MENU(VCO2, ledbutton_VCO2, "VCO2")
ENTRY("Pitch", MenuEntry_Pitch, Output_VCO2_PITCH)
CUSTOMENTRY("Triangle", MenuEntry_Toggle, Switch_SEL2TRI)
CUSTOMENTRY("Pulse", MenuEntry_Toggle, Switch_SEL2SQR)
CUSTOMENTRY("Saw", MenuEntry_Toggle, Switch_SEL2SAW)
CUSTOMENTRY("Subosc", MenuEntry_Toggle, Switch_SEL2SUB)
CUSTOMENTRY("Sync", MenuEntry_Toggle, Switch_SELVCOSYNC1)
ENTRY("PW", MenuEntry_Percentage, Output_VCO2_PW)
ENTRY("FM (from VCO1)", MenuEntry_Value, Output_VCO123_FM2)

ENDMENU()

MENU(VCO3, ledbutton_VCO3, "VCO3")
ENTRY("Pitch", MenuEntry_Pitch, Output_VCO3_PITCH)
CUSTOMENTRY("Triangle", MenuEntry_Toggle, Switch_SEL3TRI)
CUSTOMENTRY("Pulse", MenuEntry_Toggle, Switch_SEL3SQR)
CUSTOMENTRY("Saw", MenuEntry_Toggle, Switch_SEL3SAW)
CUSTOMENTRY("Subosc", MenuEntry_Toggle, Switch_SEL3SUB)
CUSTOMENTRY("Sync", MenuEntry_Toggle, Switch_SELVCOSYNC2)

ENTRY("PW", MenuEntry_Percentage, Output_VCO3_PW)
ENTRY("FM  (from VCO1)", MenuEntry_Value, Output_VCO123_FM3)
ENDMENU()

MENU(VCO8, ledbutton_VCO3, "Noise VCO")

ENTRY("Color", MenuEntry_Pitch, Output_NOISE_COLOR)

ENDMENU()

MENU(VCO4567, ledbutton_VCO4, "VCO 4 5 6 7")

ENTRY("4 Pitch", MenuEntry_Pitch, Output_VCO4_PITCH)
CUSTOMENTRY("4 Sqr", MenuEntry_Toggle, Switch_SEL4SQR)

ENTRY("5 Pitch", MenuEntry_Pitch, Output_VCO5_PITCH)
CUSTOMENTRY("5 Sqr", MenuEntry_Toggle, Switch_SEL5SQR)

ENTRY("6 Pitch", MenuEntry_Pitch, Output_VCO6_PITCH)
CUSTOMENTRY("6 Sqr", MenuEntry_Toggle, Switch_SEL6SQR)

ENTRY("7 Pitch", MenuEntry_Pitch, Output_VCO7_PITCH)
CUSTOMENTRY("7 Sqr", MenuEntry_Toggle, Switch_SEL7SQR)

ENDMENU()

EXTRABUTTON(VCF1, ledbutton_VCF1_ResonanceBut)
EXTRABUTTON(VCF1, ledbutton_VCF1_Routing)
EXTRABUTTON(VCF1, ledbutton_VCF1_Spectrum_ModBut)
MENU(VCF1, ledbutton_VCF1_FrequencyBut, "VCF1")
ENTRY("Freq", MenuEntry_Value, Output_VCF1_CV)
ENTRY("Resonance", MenuEntry_Value, Output_VCF1_RES)
ENTRY("Spectrum Mod", MenuEntry_Value, Output_VCF1_CROSSMOD)


CUSTOMENTRY("SELVCF1AP", MenuEntry_Toggle, Switch_SELVCF1AP)
CUSTOMENTRY("SELVCF1BP", MenuEntry_Toggle, Switch_SELVCF1BP)
CUSTOMENTRY("SELFSM1SRC", MenuEntry_Toggle, Switch_SELFSM1SRC)

//CUSTOMENTRY("VCF parallel", MenuEntry_Toggle, Switch_SELVCF2POST)
ENDMENU()


MENU(VCF1MIX, ledbutton_VCF1_FrequencyBut, "VCF1 Mix")
ENTRY("Level", MenuEntry_Value, Output_VCF1_LEVEL)
ENTRY("VCO1", MenuEntry_Value, Output_VCO1_MIX1)
ENTRY("VCO2", MenuEntry_Value, Output_VCO2_MIX1)
ENTRY("VCO3", MenuEntry_Value, Output_VCO3_MIX1)
ENTRY("VCO4567",  MenuEntry_Value, Output_VCO4567_MIX1)
ENTRY("Whitenoise", MenuEntry_Value, Output_WHITENS_MIX1)
ENTRY("Diginoise", MenuEntry_Value, Output_DIGINS_MIX1)
ENTRY("DNSSaw", MenuEntry_Value, Output_DNSSAW_MIX1)
ENTRY("RingMod", MenuEntry_Value, Output_RM1_MIX1)
ENTRY("Sub", MenuEntry_Value, Output_VCO1_SUB1)

//ENTRY("FX left", MenuEntry_Value, Output_VCF1_FX_12)
//ENTRY("FX right", MenuEntry_Value, Output_VCF1_FX_24)

//CUSTOMENTRY("VCF parallel", MenuEntry_Toggle, Switch_SELVCF2POST)
ENDMENU()

MENU(PORTAMENTO, ledbutton_PortamentoLeft,"Portamento" )
ENTRY("Time", MenuEntry_Value, Output_PORTAMENTO_TIME)
CUSTOMENTRY("Portamento", MenuEntry_Toggle, Switch_SELPORTAMENTO)
CUSTOMENTRY("Staccato", MenuEntry_Toggle, Switch_SELSTACCATO)
ENDMENU()

MENU(VCF2MIX, ledbutton_VCF1_FrequencyBut, "VCF2 Mix")
ENTRY("Level", MenuEntry_Value, Output_VCF2_LEVEL)
ENTRY("VCO1", MenuEntry_Value, Output_VCO1_MIX2)
ENTRY("VCO2", MenuEntry_Value, Output_VCO2_MIX2)
ENTRY("VCO3", MenuEntry_Value, Output_VCO3_MIX2)
ENTRY("VCO4567", MenuEntry_Value, Output_VCO4567_MIX2)
ENTRY("Whitenoise", MenuEntry_Value, Output_WHITENS_MIX2)
ENTRY("Diginoise", MenuEntry_Value, Output_DIGINS_MIX2)
ENTRY("DNSSaw", MenuEntry_Value, Output_DNSSAW_MIX2)
ENTRY("RingMod", MenuEntry_Value, Output_RM1_MIX2)
ENTRY("Sub", MenuEntry_Value, Output_VCO1_SUB2)


//ENTRY("FX left", MenuEntry_Value, Output_VCF2_FX_L)
//ENTRY("FX right", MenuEntry_Value, Output_VCF2_FX_L)

//CUSTOMENTRY("VCF parallel", MenuEntry_Toggle, Switch_SELVCF2POST)
ENDMENU()

MENU(CLEANMIX, ledbutton_VCF1_FrequencyBut, "Cleanfeed Mix")
ENTRY("Level", MenuEntry_Value, Output_CLEANF_LEVEL)
ENTRY("VCO1 Sine", MenuEntry_Value, Output_VCO1SIN_MIX3)
ENTRY("VCO2 Sine", MenuEntry_Value, Output_VCO2SIN_MIX3)
ENTRY("VCO3 Sine", MenuEntry_Value, Output_VCO3SIN_MIX3)
ENTRY("VCO4", MenuEntry_Value, Output_VCO4_DRY_MIX)
ENTRY("VCO5", MenuEntry_Value, Output_VCO5_DRY_MIX)
ENTRY("VCO6", MenuEntry_Value, Output_VCO6_DRY_MIX)
ENTRY("VCO7", MenuEntry_Value, Output_VCO7_DRY_MIX)
ENTRY("Purple Noise", MenuEntry_Value, Output_PUN_MIX)
ENTRY("Brown Noise", MenuEntry_Value, Output_BN_MIX3)

//ENTRY("FX left", MenuEntry_Value, Output_CLEANF_FX_L)
//ENTRY("FX right", MenuEntry_Value, Output_CLEANF_FX_L)
//CUSTOMENTRY("VCF parallel", MenuEntry_Toggle, Switch_SELVCF2POST)
ENDMENU()

EXTRABUTTON(VCF2a, ledbutton_Mid1Button)
EXTRABUTTON(VCF2a, ledbutton_Mid2Button)
EXTRABUTTON(VCF2a, ledbutton_HighButton)
EXTRABUTTON(VCF2a, ledbutton_VCF2_F_B)
EXTRABUTTON(VCF2a, ledbutton_VCF2_SPM)
EXTRABUTTON(VCF2a, ledbutton_PB1but)
EXTRABUTTON(VCF2a, ledbutton_PB2but)
EXTRABUTTON(VCF2a, ledbutton_PB3but)
EXTRABUTTON(VCF2a, ledbutton_PB4but)
EXTRABUTTON(VCF2a, ledbutton_VCF2_RES)

MENU(VCF2a, ledbutton_LowButton, "VCF2 A")

CUSTOMENTRY("Ringmod", MenuEntry_Toggle, Switch_SELRMVCF2)
CUSTOMENTRY("Mod to AB", MenuEntry_Toggle, Switch_SELVCF23FSM)
CUSTOMENTRY("Mod to CD", MenuEntry_Toggle, Switch_SELVCF45FSM)
CUSTOMENTRY("SELFSM2SRC", MenuEntry_Toggle, Switch_SELFSM2SRC)

//ENTRY("Feedback", MenuEntry_Value, Output_VCF2_FB)
ENTRY("Frequency A", MenuEntry_Value, Output_VCF2_A_CV)
ENTRY("Resonance A", MenuEntry_Value, Output_VCF2_A_RES)
ENTRY("Mix A", MenuEntry_Value, Output_VCF2_A_MIX)
ENTRY("Spectrum Mod", MenuEntry_Value, Output_VCF2_CROSSMOD)
//CUSTOMENTRY("L0", MenuEntry_Toggle, Switch_SELVCF2L0)
//CUSTOMENTRY("L1", MenuEntry_Toggle, Switch_SELVCF2L1)
ENDMENU()


MENU(VCF2b, ledbutton_LowButton, "VCF2 B")
//ENTRY("Feedback", MenuEntry_Value, Output_VCF2_FB)
ENTRY("Frequency B", MenuEntry_Value, Output_VCF2_B_CV)
ENTRY("Resonance B", MenuEntry_Value, Output_VCF2_B_RES)
ENTRY("Mix B", MenuEntry_Value, Output_VCF2_B_MIX)
ENTRY("Spectrum Mod", MenuEntry_Value, Output_VCF2_CROSSMOD)
//CUSTOMENTRY("L0", MenuEntry_Toggle, Switch_SELVCF2L0)
//CUSTOMENTRY("L1", MenuEntry_Toggle, Switch_SELVCF2L1)
ENDMENU()

MENU(VCF2c, ledbutton_LowButton, "VCF2 C")
//ENTRY("Feedback", MenuEntry_Value, Output_VCF2_FB)
ENTRY("Frequency C", MenuEntry_Value, Output_VCF2_C_CV)
ENTRY("Resonance C", MenuEntry_Value, Output_VCF2_C_RES)
ENTRY("Mix C", MenuEntry_Value, Output_VCF2_C_MIX)
ENTRY("Spectrum Mod", MenuEntry_Value, Output_VCF2_CROSSMOD)
//CUSTOMENTRY("L0", MenuEntry_Toggle, Switch_SELVCF2L0)
//CUSTOMENTRY("L1", MenuEntry_Toggle, Switch_SELVCF2L1)
ENDMENU()

MENU(VCF2d, ledbutton_LowButton, "VCF2 D")
//ENTRY("Feedback", MenuEntry_Value, Output_VCF2_FB)
ENTRY("Frequency D", MenuEntry_Value, Output_VCF2_D_CV)
ENTRY("Resonancy D", MenuEntry_Value, Output_VCF2_D_RES)
ENTRY("Mix D", MenuEntry_Value, Output_VCF2_D_MIX)
ENTRY("Spectrum Mod", MenuEntry_Value, Output_VCF2_CROSSMOD)
//CUSTOMENTRY("L0", MenuEntry_Toggle, Switch_SELVCF2L0)
//CUSTOMENTRY("L1", MenuEntry_Toggle, Switch_SELVCF2L1)
ENDMENU()

EXTRABUTTON(EFFECTS, ledbutton_VCF1_FX)
EXTRABUTTON(EFFECTS, ledbutton_VCF2_FX)

MENU(EFFECTS, ledbutton_Cleanfeed_FX, "Global FX")
CUSTOMENTRY("Type", MenuEntry_EffectType, Output_VCA_FXPOT1)
CUSTOMENTRY("Parameter 1", MenuEntry_EffectParam1, Output_VCA_FXPOT3)
CUSTOMENTRY("Parameter 2", MenuEntry_EffectParam2, Output_VCA_FXPOT2)
CUSTOMENTRY("Parameter 3", MenuEntry_EffectParam3, Output_VCA_FXPOT1)

//CUSTOMENTRY("CLNFD", MenuEntry_Toggle, Switch_SELEFFECT3)
//CUSTOMENTRY("VCF 1", MenuEntry_Toggle, Switch_SELEFFECT1)
//CUSTOMENTRY("VCF 2", MenuEntry_Toggle, Switch_SELEFFECT2)
ENDMENU()

/*
MENU(CHASE, ledbutton_CHASE, "Chase")
ENTRY("Chase", MenuEntry_Value, Output_CHASE)
CUSTOMENTRY("Chase 4567", MenuEntry_ToggleStashChase, Switch_SELCHASEOSC4567)
CUSTOMENTRY("Chase VCF2", MenuEntry_ToggleStashChase, Switch_SELCHASEVCF2)
ENDMENU()

MENU(STASH, ledbutton_STASH, "Stash")
ENTRY("Stash", MenuEntry_Value, Output_STASH)
CUSTOMENTRY("Stash 4567", MenuEntry_ToggleStashChase, Switch_SELSTASHOSC4567)
CUSTOMENTRY("Stash VCF2", MenuEntry_ToggleStashChase, Switch_SELSTASHVCF2)
ENDMENU()

EXTRABUTTON(CHASE, ledbutton_PORTAMENTO_RIGHT)
MENU(PORTAMENTO,ledbutton_PORTAMENTO_LEFT, "Portamento")
CUSTOMENTRY("Portamento", MenuEntry_Toggle, Switch_SELPORTAMENTO)
ENTRY("PortaTime", MenuEntry_Value, Output_PORTAMENTO_TIME)
ENDMENU()*/

EXTRABUTTON(MIXER, ledbutton_Cleanfeed_VCA)
EXTRABUTTON(MIXER, ledbutton_VCF1_M_S)
EXTRABUTTON(MIXER, ledbutton_VCF1_VCA)
EXTRABUTTON(MIXER, ledbutton_VCF2_M_S)
EXTRABUTTON(MIXER, ledbutton_VCF2_VCA)
MENU(MASTER, ledbutton_Cleanfeed_M_S, "Mixer")
ENTRY("CLNFD pan", MenuEntry_MidValue, Output_CLEANF_PAN)
ENTRY("CLNFD lvl", MenuEntry_RemapValue, Output_CLEANF_LEVEL)
ENTRY("VCF 1 pan", MenuEntry_MidValue, Output_VCF1_PAN)
ENTRY("VCF 1 lvl", MenuEntry_RemapValue, Output_VCF1_LEVEL)
ENTRY("VCF 2 pan", MenuEntry_MidValue, Output_VCF2_PAN)
ENTRY("VCF 2 lvl", MenuEntry_RemapValue, Output_VCF2_LEVEL)
ENTRY("Master Level", MenuEntry_RemapValue, Output_MASTER_LEVEL)
ENTRY("Fx 1", MenuEntry_Value, Output_VCA_FXPOT1)
ENTRY("Fx 2", MenuEntry_Value, Output_VCA_FXPOT2)
ENTRY("Fx 3", MenuEntry_Value, Output_VCA_FXPOT3)

ENDMENU()


#ifdef REDEFINEDTARGET
#undef TARGET
#undef REDEFINEDTARGET
#endif

#ifdef REDEFINEDMENU
#undef MENU
#undef REDEFINEDMENU
#endif

#ifdef REDEFINEDENTRY
#undef ENTRY
#undef REDEFINEDENTRY
#endif

#ifdef REDEFINEDCUSTOMENTRY
#undef CUSTOMENTRY
#undef REDEFINEDCUSTOMENTRY
#endif

#ifdef REDEFINEDENDMENU
#undef ENDMENU
#undef REDEFINEDENDMENU
#endif

#ifdef REDEFINEEXTRABUTTON
#undef EXTRABUTTON
#undef REDEFINEEXTRABUTTON
#endif
