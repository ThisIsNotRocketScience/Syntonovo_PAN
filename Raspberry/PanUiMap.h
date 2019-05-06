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
CUSTOMENTRY("Octave", MenuEntry_Octave, Output_VCO1_OCTAVE)


ENTRY("-> VCF1", MenuEntry_Value, Output_VCO1_MIX1)
ENTRY("-> VCF2", MenuEntry_Value, Output_VCO1_MIX2)
ENTRY("Sine -> Cleanfeed ", MenuEntry_Value, Output_VCO1SIN_MIX3)

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
ENTRY("-> VCF1", MenuEntry_Value, Output_VCO2_MIX1)
ENTRY("-> VCF2", MenuEntry_Value, Output_VCO2_MIX2)
ENTRY("Sine -> Cleanfeed ", MenuEntry_Value, Output_VCO2SIN_MIX3)
CUSTOMENTRY("Octave", MenuEntry_Octave, Output_VCO2_OCTAVE)

ENDMENU()

MENU(VCO3, ledbutton_VCO3, "VCO3")
ENTRY("Pitch", MenuEntry_Pitch, Output_VCO3_PITCH)
CUSTOMENTRY("Triangle", MenuEntry_Toggle, Switch_SEL3TRI)
CUSTOMENTRY("Pulse", MenuEntry_Toggle, Switch_SEL3SQR)
CUSTOMENTRY("Saw", MenuEntry_Toggle, Switch_SEL3SAW)
CUSTOMENTRY("Subosc", MenuEntry_Toggle, Switch_SEL3SUB)
CUSTOMENTRY("Sync", MenuEntry_Toggle, Switch_SELVCOSYNC2)

ENTRY("PW", MenuEntry_Percentage, Output_VCO3_PW)
ENTRY("FM (from VCO1)", MenuEntry_Value, Output_VCO123_FM3)

ENTRY("-> VCF1", MenuEntry_Value, Output_VCO3_MIX1)
ENTRY("-> VCF2", MenuEntry_Value, Output_VCO3_MIX2)
ENTRY("Sine -> Cleanfeed ", MenuEntry_Value, Output_VCO3SIN_MIX3)
CUSTOMENTRY("Octave", MenuEntry_Octave, Output_VCO3_OCTAVE)


ENDMENU()

MENU(VCO8, ledbutton_VCO3, "Noise VCO")

ENTRY("Color", MenuEntry_Pitch, Output_NOISE_COLOR)
ENTRY("Saw -> VCF1", MenuEntry_Value, Output_DNSSAW_MIX1)
ENTRY("Saw -> VCF2", MenuEntry_Value, Output_DNSSAW_MIX2)
ENTRY("Noise -> VCF1", MenuEntry_Value, Output_DIGINS_MIX1)
ENTRY("Noise -> VCF2", MenuEntry_Value, Output_DIGINS_MIX2)


ENDMENU()

MENU(VCO4, ledbutton_VCO4, "VCO 4")
ENTRY("Pitch", MenuEntry_Pitch, Output_VCO4_PITCH)
ENTRY("Pulsewidth", MenuEntry_Value, Output_VCO4_PW)
CUSTOMENTRY("Square Output", MenuEntry_Toggle, Switch_SEL4SQR)
CUSTOMENTRY("Octave", MenuEntry_Octave, Output_VCO4_OCTAVE)

ENTRY("-> Cleanfeed", MenuEntry_Value, Output_VCO4_DRY_MIX)
ENTRY("4567 Square -> VCF1", MenuEntry_Value, Output_VCO4567_MIX1)
ENTRY("4567 Square -> VCF2", MenuEntry_Value, Output_VCO4567_MIX2)

ENDMENU()

MENU(VCO5, ledbutton_VCO4, "VCO 5")
ENTRY("Pitch", MenuEntry_Pitch, Output_VCO5_PITCH)
ENTRY("Pulsewidth", MenuEntry_Value, Output_VCO5_PW)
CUSTOMENTRY("Square Output", MenuEntry_Toggle, Switch_SEL5SQR)
CUSTOMENTRY("Octave", MenuEntry_Octave, Output_VCO5_OCTAVE)
ENTRY("-> Cleanfeed", MenuEntry_Value, Output_VCO5_DRY_MIX)
ENTRY("4567 Square -> VCF1", MenuEntry_Value, Output_VCO4567_MIX1)
ENTRY("4567 Square -> VCF2", MenuEntry_Value, Output_VCO4567_MIX2)

ENDMENU()

MENU(VCO6, ledbutton_VCO4, "VCO 6")
ENTRY("Pitch", MenuEntry_Pitch, Output_VCO6_PITCH)
ENTRY("Pulsewidth", MenuEntry_Value, Output_VCO6_PW)
CUSTOMENTRY("Square Output", MenuEntry_Toggle, Switch_SEL6SQR)
CUSTOMENTRY("Octave", MenuEntry_Octave, Output_VCO6_OCTAVE)
ENTRY("-> Cleanfeed", MenuEntry_Value, Output_VCO6_DRY_MIX)
ENTRY("4567 Square -> VCF1", MenuEntry_Value, Output_VCO4567_MIX1)
ENTRY("4567 Square -> VCF2", MenuEntry_Value, Output_VCO4567_MIX2)

ENDMENU()

MENU(VCO7, ledbutton_VCO4, "VCO 7")
ENTRY("Pitch", MenuEntry_Pitch, Output_VCO7_PITCH)
ENTRY("Pulsewidth", MenuEntry_Value, Output_VCO7_PW)
CUSTOMENTRY("Square Output", MenuEntry_Toggle, Switch_SEL7SQR)
CUSTOMENTRY("Octave", MenuEntry_Octave, Output_VCO7_OCTAVE)
ENTRY("-> Cleanfeed", MenuEntry_Value, Output_VCO7_DRY_MIX)
ENTRY("4567 Square -> VCF1", MenuEntry_Value, Output_VCO4567_MIX1)
ENTRY("4567 Square -> VCF2", MenuEntry_Value, Output_VCO4567_MIX2)

ENDMENU()

EXTRABUTTON(VCF1, ledbutton_VCF1_ResonanceBut)
EXTRABUTTON(VCF1, ledbutton_VCF1_Routing)
EXTRABUTTON(VCF1, ledbutton_VCF1_Spectrum_ModBut)
MENU(VCF1, ledbutton_VCF1_FrequencyBut, "VCF1")
ENTRY("Frequency", MenuEntry_Value, Output_VCF1_CV)
ENTRY("Resonance", MenuEntry_Value, Output_VCF1_RES)
ENTRY("Spectrum Mod", MenuEntry_Value, Output_VCF1_CROSSMOD)


//CUSTOMENTRY("SELVCF1AP", MenuEntry_Toggle, Switch_SELVCF1AP)
//CUSTOMENTRY("SELVCF1BP", MenuEntry_Toggle, Switch_SELVCF1BP)
CUSTOMENTRY("FSM src -> Osc1 / Osc4", MenuEntry_Toggle, Switch_SELFSM1SRC)
CUSTOMENTRY("Stereo/Mono", MenuEntry_Toggle, Switch_SELVCF1MOST)

//CUSTOMENTRY("VCF parallel", MenuEntry_Toggle, Switch_SELVCF2POST)
ENDMENU()


MENU(VCF1MIX, ledbutton_VCF1_FrequencyBut, "VCF1 Mix")
ENTRY("Level", MenuEntry_Value, Output_VCF1_LEVEL)
ENTRY("VCO1", MenuEntry_Value, Output_VCO1_MIX1)
ENTRY("VCO2", MenuEntry_Value, Output_VCO2_MIX1)
ENTRY("VCO3", MenuEntry_Value, Output_VCO3_MIX1)
ENTRY("VCO4567 Square", MenuEntry_Value, Output_VCO4567_MIX1)
ENTRY("Whitenoise", MenuEntry_Value, Output_WHITENS_MIX1)
ENTRY("Diginoise", MenuEntry_Value, Output_DIGINS_MIX1)
ENTRY("DNSSaw", MenuEntry_Value, Output_DNSSAW_MIX1)
ENTRY("RingMod", MenuEntry_Value, Output_RM1_MIX1)
ENTRY("Sub", MenuEntry_Value, Output_VCO1_SUB1)

//ENTRY("FX left", MenuEntry_Value, Output_VCF1_FX_12)
//ENTRY("FX right", MenuEntry_Value, Output_VCF1_FX_24)

//CUSTOMENTRY("VCF parallel", MenuEntry_Toggle, Switch_SELVCF2POST)
ENDMENU()

MENU(PORTAMENTO, ledbutton_PortamentoLeft, "Portamento")
ENTRY("Time", MenuEntry_Value, Output_PORTAMENTO_TIME)
CUSTOMENTRY("Portamento", MenuEntry_Toggle, Switch_SELPORTAMENTO)
CUSTOMENTRY("Staccato", MenuEntry_Toggle, Switch_SELSTACCATO)
ENDMENU()

MENU(VCF2MIX, ledbutton_VCF1_FrequencyBut, "VCF2 Mix")
CUSTOMENTRY("Ringmod Enable", MenuEntry_Toggle, Switch_SELRMVCF2)
CUSTOMENTRY("Gatetrigger Enable", MenuEntry_Toggle, Switch_SELGTRVCF2)


ENTRY("Level", MenuEntry_Value, Output_VCF2_LEVEL)
ENTRY("VCO1", MenuEntry_Value, Output_VCO1_MIX2)
ENTRY("VCO2", MenuEntry_Value, Output_VCO2_MIX2)
ENTRY("VCO3", MenuEntry_Value, Output_VCO3_MIX2)
ENTRY("VCO4567 Square", MenuEntry_Value, Output_VCO4567_MIX2)
ENTRY("Whitenoise", MenuEntry_Value, Output_WHITENS_MIX2)
ENTRY("Diginoise", MenuEntry_Value, Output_DIGINS_MIX2)
ENTRY("DNSSaw", MenuEntry_Value, Output_DNSSAW_MIX2)
ENTRY("RingMod/Gatetrigger", MenuEntry_Value, Output_RM1_MIX2)
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
ENTRY("Ringmod", MenuEntry_Value, Output_RM2_MIX3)
CUSTOMENTRY("Stereo/Mono", MenuEntry_Toggle, Switch_SELVCLEANFMOST)
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


CUSTOMENTRY("FSM to A & B", MenuEntry_Toggle, Switch_SELVCF23FSM)

CUSTOMENTRY("FSM src -> Osc1 / Osc5", MenuEntry_Toggle, Switch_SELFSM2SRC)
CUSTOMENTRY("Stereo/Mono", MenuEntry_Toggle, Switch_SELVCF2MOST)

//ENTRY("Feedback", MenuEntry_Value, Output_VCF2_FB)
ENTRY("Frequency A", MenuEntry_Value, Output_VCF2_A_CV)
ENTRY("Resonance A", MenuEntry_Value, Output_VCF2_A_RES)
ENTRY("Mix A", MenuEntry_Value, Output_VCF2_A_MIX)
ENTRY("Spectrum Mod", MenuEntry_Value, Output_VCF2_CROSSMOD)
ENTRY("Negative FB", MenuEntry_Value, Output_VCF2_M_FB)
ENTRY("Positive FB", MenuEntry_Value, Output_VCF2_P_FB)
//CUSTOMENTRY("L0", MenuEntry_Toggle, Switch_SELVCF2L0)
//CUSTOMENTRY("L1", MenuEntry_Toggle, Switch_SELVCF2L1)
ENDMENU()


MENU(VCF2b, ledbutton_LowButton, "VCF2 B")
CUSTOMENTRY("FSM to A & B", MenuEntry_Toggle, Switch_SELVCF23FSM)
CUSTOMENTRY("FSM src -> Osc1 / Osc5", MenuEntry_Toggle, Switch_SELFSM2SRC)
CUSTOMENTRY("Stereo/Mono", MenuEntry_Toggle, Switch_SELVCF2MOST)

//ENTRY("Feedback", MenuEntry_Value, Output_VCF2_FB)
ENTRY("Frequency B", MenuEntry_Value, Output_VCF2_B_CV)
ENTRY("Resonance B", MenuEntry_Value, Output_VCF2_B_RES)
ENTRY("Mix B", MenuEntry_Value, Output_VCF2_B_MIX)
ENTRY("Spectrum Mod", MenuEntry_Value, Output_VCF2_CROSSMOD)
ENTRY("Negative FB", MenuEntry_Value, Output_VCF2_M_FB)
ENTRY("Positive FB", MenuEntry_Value, Output_VCF2_P_FB)

//CUSTOMENTRY("L0", MenuEntry_Toggle, Switch_SELVCF2L0)
//CUSTOMENTRY("L1", MenuEntry_Toggle, Switch_SELVCF2L1)
ENDMENU()

MENU(VCF2c, ledbutton_LowButton, "VCF2 C")

CUSTOMENTRY("FSM to C & D", MenuEntry_Toggle, Switch_SELVCF45FSM)
CUSTOMENTRY("FSM src -> Osc1 / Osc5", MenuEntry_Toggle, Switch_SELFSM2SRC)
CUSTOMENTRY("Stereo/Mono", MenuEntry_Toggle, Switch_SELVCF2MOST)

//ENTRY("Feedback", MenuEntry_Value, Output_VCF2_FB)
ENTRY("Frequency C", MenuEntry_Value, Output_VCF2_C_CV)
ENTRY("Resonance C", MenuEntry_Value, Output_VCF2_C_RES)
ENTRY("Mix C", MenuEntry_Value, Output_VCF2_C_MIX)
ENTRY("Spectrum Mod", MenuEntry_Value, Output_VCF2_CROSSMOD)
ENTRY("Negative FB", MenuEntry_Value, Output_VCF2_M_FB)
ENTRY("Positive FB", MenuEntry_Value, Output_VCF2_P_FB)

//CUSTOMENTRY("L0", MenuEntry_Toggle, Switch_SELVCF2L0)
//CUSTOMENTRY("L1", MenuEntry_Toggle, Switch_SELVCF2L1)
ENDMENU()

MENU(VCF2d, ledbutton_LowButton, "VCF2 D")
CUSTOMENTRY("FSM to C & D", MenuEntry_Toggle, Switch_SELVCF45FSM)

CUSTOMENTRY("FSM src -> Osc1 / Osc5", MenuEntry_Toggle, Switch_SELFSM2SRC)
CUSTOMENTRY("Stereo/Mono", MenuEntry_Toggle, Switch_SELVCF2MOST)

//ENTRY("Feedback", MenuEntry_Value, Output_VCF2_FB)
ENTRY("Frequency D", MenuEntry_Value, Output_VCF2_D_CV)
ENTRY("Resonancy D", MenuEntry_Value, Output_VCF2_D_RES)
ENTRY("Mix D", MenuEntry_Value, Output_VCF2_D_MIX)
ENTRY("Spectrum Mod", MenuEntry_Value, Output_VCF2_CROSSMOD)
ENTRY("Negative FB", MenuEntry_Value, Output_VCF2_M_FB)
ENTRY("Positive FB", MenuEntry_Value, Output_VCF2_P_FB)

//CUSTOMENTRY("L0", MenuEntry_Toggle, Switch_SELVCF2L0)
//CUSTOMENTRY("L1", MenuEntry_Toggle, Switch_SELVCF2L1)
ENDMENU()

EXTRABUTTON(EFFECTS, ledbutton_VCF1_FX)
EXTRABUTTON(EFFECTS, ledbutton_VCF2_FX)

MENU(EFFECTS, ledbutton_Cleanfeed_FX, "Global Effects")
//CUSTOMENTRY("Type", MenuEntry_EffectType, Output_VCA_FXPOT1)
CUSTOMENTRY("FXSel 1", MenuEntry_Toggle, Switch_SELEF0)
CUSTOMENTRY("FXSel 2", MenuEntry_Toggle, Switch_SELEF1)
CUSTOMENTRY("FXSel 3", MenuEntry_Toggle, Switch_SELEF2)
//CUSTOMENTRY("FXSel 4", MenuEntry_Toggle, Switch_SELEF3)


ENTRY("VCF1 12 -> Effect", MenuEntry_Value, Output_VCF1_FX_12)
ENTRY("Parameter 1", MenuEntry_Value, Output_VCA_FXPOT3)
ENTRY("VCF1 24 -> Effect", MenuEntry_Value, Output_VCF1_FX_24)
ENTRY("Parameter 2", MenuEntry_Value, Output_VCA_FXPOT2)
ENTRY("VCF2 L -> Effect", MenuEntry_Value, Output_VCF2_FX_L)
ENTRY("Parameter 3", MenuEntry_Value, Output_VCA_FXPOT1)
ENTRY("VCF2 R -> Effect", MenuEntry_Value, Output_VCF2_FX_R)
ENTRY("Cleanfeed L -> Effect", MenuEntry_Value, Output_CLEANF_FX_L)
ENTRY("Cleanfeed R -> Effect", MenuEntry_Value, Output_CLEANF_FX_R)
ENTRY("Effect Return", MenuEntry_Value, Output_FX_RET_LEVEL)



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
ENTRY("Effect Return", MenuEntry_Value, Output_FX_RET_LEVEL)

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
