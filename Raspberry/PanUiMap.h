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
CUSTOMENTRY("Sync", MenuEntry_Toggle, Switch_SELVCOSYNC1)

ENTRY("PW", MenuEntry_Percentage, Output_VCO1_PW)
ENTRY("VCFMix", MenuEntry_FilterMix, Output_VCO1_VCFMIX)
ENTRY("RM Mix", MenuEntry_FilterMix, Output_RM1_VCFMIX)
ENDMENU()

MENU(VCO2, ledbutton_VCO2, "VCO2")
ENTRY("Pitch", MenuEntry_Pitch, Output_VCO2_PITCH)
CUSTOMENTRY("Triangle", MenuEntry_Toggle, Switch_SEL2TRI)
CUSTOMENTRY("Pulse", MenuEntry_Toggle, Switch_SEL2SQR)
CUSTOMENTRY("Saw", MenuEntry_Toggle, Switch_SEL2SAW)
CUSTOMENTRY("Subosc", MenuEntry_Toggle, Switch_SEL2SUB)
CUSTOMENTRY("Sync", MenuEntry_Toggle, Switch_SELVCOSYNC2)
ENTRY("PW", MenuEntry_Percentage, Output_VCO2_PW)
ENTRY("FM", MenuEntry_Value, Output_VCO123_FM2)
ENTRY("VCFMix", MenuEntry_FilterMix, Output_VCO2_VCFMIX)
ENDMENU()

MENU(VCO3, ledbutton_VCO3, "VCO3")
ENTRY("Pitch", MenuEntry_Pitch, Output_VCO3_PITCH)
CUSTOMENTRY("Triangle", MenuEntry_Toggle, Switch_SEL3TRI)
CUSTOMENTRY("Pulse", MenuEntry_Toggle, Switch_SEL3SQR)
CUSTOMENTRY("Saw", MenuEntry_Toggle, Switch_SEL3SAW)
CUSTOMENTRY("Subosc", MenuEntry_Toggle, Switch_SEL3SUB)
ENTRY("PW", MenuEntry_Percentage, Output_VCO3_PW)
ENTRY("FM", MenuEntry_Value, Output_VCO123_FM3)
ENTRY("VCFMix", MenuEntry_FilterMix, Output_VCO3_VCFMIX)
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
ENTRY("VCFMix", MenuEntry_FilterMix, Output_VCO4567_VCFMIX)
ENDMENU()

EXTRABUTTON(VCF1, ledbutton_VCF1_ResonanceBut)
EXTRABUTTON(VCF1, ledbutton_VCF1_Routing)
EXTRABUTTON(VCF1, ledbutton_VCF1_Spectrum_ModBut)
MENU(VCF1, ledbutton_VCF1_FrequencyBut, "VCF1")
ENTRY("Freq", MenuEntry_Value, Output_VCF1_CV)
ENTRY("Resonance", MenuEntry_Value, Output_VCF1_RES)
//CUSTOMENTRY("VCF parallel", MenuEntry_Toggle, Switch_SELVCF2POST)
ENDMENU()


MENU(VCF1MIX, ledbutton_VCF1_FrequencyBut, "VCF1 Mix")
ENTRY("Osc1", MenuEntry_Value, Output_VCO1_MIX1)
ENTRY("Osc2", MenuEntry_Value, Output_VCO2_MIX1)
ENTRY("Osc3", MenuEntry_Value, Output_VCO2_MIX1)
//CUSTOMENTRY("VCF parallel", MenuEntry_Toggle, Switch_SELVCF2POST)
ENDMENU()

MENU(VCF2MIX, ledbutton_VCF1_FrequencyBut, "VCF2 Mix")
ENTRY("Osc1", MenuEntry_Value, Output_VCO1_MIX2)
ENTRY("Osc2", MenuEntry_Value, Output_VCO2_MIX2)
ENTRY("Osc3", MenuEntry_Value, Output_VCO2_MIX2)
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

MENU(VCF2a, ledbutton_LowButton, "VCF2")
//ENTRY("Feedback", MenuEntry_Value, Output_VCF2_FB)
ENTRY("Res L", MenuEntry_Value, Output_VCF2_L_RES)
ENTRY("Res M1", MenuEntry_Value, Output_VCF2_M1_RES)
ENTRY("Res M2", MenuEntry_Value, Output_VCF2_M2_RES)
ENTRY("Res H", MenuEntry_Value, Output_VCF2_H_RES)
ENTRY("Spectrum Mod", MenuEntry_Value, Output_VCF2_CROSSMOD)
ENTRY("Freq L", MenuEntry_Value, Output_VCF2_L_CV)
ENTRY("Mix L", MenuEntry_Value, Output_VCF2_L_MIX)
ENTRY("Freq M1", MenuEntry_Value, Output_VCF2_M1_CV)
ENTRY("Mix M1", MenuEntry_Value, Output_VCF2_M1_MIX)
ENTRY("Freq M2", MenuEntry_Value, Output_VCF2_M2_CV)
ENTRY("Mix M2", MenuEntry_Value, Output_VCF2_M2_MIX)
ENTRY("Freq H", MenuEntry_Value, Output_VCF2_H_CV)
ENTRY("Mix H", MenuEntry_Value, Output_VCF2_H_MIX)
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
MENU(MIXER, ledbutton_Cleanfeed_M_S, "Mixer")
ENTRY("CLNFD pan", MenuEntry_MidValue, Output_CLEANF_PAN)
ENTRY("CLNFD lvl", MenuEntry_RemapValue, Output_CLEANF_LIN)
ENTRY("VCF 1 pan", MenuEntry_MidValue, Output_VCF1_PAN)
ENTRY("VCF 1 lvl", MenuEntry_RemapValue, Output_VCF1_LIN)
ENTRY("VCF 2 pan", MenuEntry_MidValue, Output_VCF2_PAN)
ENTRY("VCF 2 lvl", MenuEntry_RemapValue, Output_VCF2_LIN)
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
