#ifndef TARGET 
	#define REDEFINEDTARGET
	#define TARGET(param,buttonid,knobid,name)
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


TARGET(Output_VCF1_RES, ledbutton_VCF1_ResonanceBut, knob_VCF1_Resonance, "VCF1 res")
TARGET(Output_VCF1_CROSSMOD, ledbutton_VCF1_Spectrum_ModBut, knob_VCF1_Spectrum_Mod, "VCF1 SPM")
TARGET(Output_VCF2_RES, ledbutton_VCF2_RES, knob_VCF2_Resonance, "VCF2 Res")
TARGET(Output_VCF2_CROSSMOD, ledbutton_VCF2_SPM, knob_VCF2_Spectrum_mod, "VCF2 SPM")
//TARGET(NOISE_COLOR, ledbutton_, knob_)
//TARGET(VCA_FXPOT1, ledbutton_, knob_)
//TARGET(VCA_FXPOT2, ledbutton_, knob_)
//TARGET(VCA_FXPOT3, ledbutton_, knob_)
//
TARGET(Output_VCF1_CV, ledbutton_VCF1_FrequencyBut, knob_VCF1_Frequency, "VCF1 Freq")
TARGET(Output_VCF2_L_CV, ledbutton_LowButton, knob_Bank_Low, "VCF2 Low")
TARGET(Output_VCF2_M1_CV, ledbutton_Mid1Button, knob_Bank_Mid_1, "VCF2 M1")
TARGET(Output_VCF2_M2_CV, ledbutton_Mid2Button, knob_Bank_Mid_2, "VCF2 M2")
TARGET(Output_VCF2_H_CV, ledbutton_HighButton, knob_Bank_High, "VCF2 Hi")
TARGET(Output_VCF2_L_MIX, ledbutton_PB1but, knob_Bank_Low_Level, "VCF2 Low Mix")
TARGET(Output_VCF2_M1_MIX, ledbutton_PB2but, knob_Bank_Mid_1_Level, "VCF2 M1 Mix")
TARGET(Output_VCF2_M2_MIX, ledbutton_PB3but, knob_Bank_Mid_2_Level, "VCF2 M2 Mix")
TARGET(Output_VCF2_H_MIX, ledbutton_PB4but, knob_Bank_High_Level, "VCF2 Hi Mix")
TARGET(Output_VCF2_FB, ledbutton_VCF2_F_B, knob_VCF2_Feed_Back, "VCF2 FB")

//
TARGET(Output_VCO1_PW, ledbutton_VCO1, knob_VCO1_PW, "VCO1 PW")
TARGET(Output_VCO2_PW, ledbutton_VCO2, knob_VCO2_PW, "VCO2 PW")
TARGET(Output_VCO3_PW, ledbutton_VCO3, knob_VCO3_PW, "VCO3 PW")
TARGET(Output_VCO123_FM1, ledbutton_VCO1, knob_FM_1_to_2, "FM1")
TARGET(Output_VCO123_FM2, ledbutton_VCO2, knob_FM_2_to_3, "FM2")
//TARGET(VCO1_MIX2, ledbutton_, knob_, "")
//TARGET(VCO2_MIX2, ledbutton_, knob_, "")
//TARGET(VCO3_MIX2, ledbutton_, knob_, "")
//TARGET(RM1_MIX2, ledbutton_, knob_, "")
//TARGET(VCO1_MIX1, ledbutton_, knob_, "")
//TARGET(VCO2_MIX1, ledbutton_, knob_, "")
//TARGET(VCO3_MIX1, ledbutton_, knob_, "")
//TARGET(RM1_MIX1, ledbutton_, knob_, "")
//
//TARGET(PUN_MIX, ledbutton_, knob_, "")
//TARGET(VCO4_DRY_MIX, ledbutton_, knob_, "")
//TARGET(VCO6_DRY_MIX, ledbutton_, knob_, "")
//TARGET(VCO4567_MIX2, ledbutton_, knob_, "")
//TARGET(WHITENS_MIX1, ledbutton_, knob_, "")
//TARGET(DIGINS_MIX1, ledbutton_, knob_, "")
//TARGET(EXT_MIX1, ledbutton_, knob_, "")
//TARGET(RM2_MIX3, ledbutton_, knob_, "")
//TARGET(BN_MIX3, ledbutton_, knob_, "")
//TARGET(VCO5_DRY_MIX, ledbutton_, knob_, "")
//TARGET(VCO7_DRY_MIX, ledbutton_, knob_, "")
//TARGET(VCO4567_MIX1, ledbutton_, knob_, "")
//TARGET(WHITENS_MIX2, ledbutton_, knob_, "")
//TARGET(DIGINS_MIX2, ledbutton_, knob_, "")
//TARGET(EXT_MIX2, ledbutton_, knob_, "")

//
TARGET(Output_CLEANF_LIN, ledbutton_Cleanfeed_VCA, __KNOB_COUNT, "CF Mix")
TARGET(Output_VCF1_LIN, ledbutton_VCF1_VCA, __KNOB_COUNT, "VCF1 Mix")
TARGET(Output_VCF2_LIN, ledbutton_VCF2_VCA, __KNOB_COUNT, "VCF2 Mix")
TARGET(Output_CLEANF_LEVEL, ledbutton_Cleanfeed_VCA, knob_LEVEL_Cleanfeed, "")
TARGET(Output_VCF1_LEVEL, ledbutton_VCF1_VCA, knob_LEVEL_VCF1, "")
TARGET(Output_VCF2_LEVEL, ledbutton_VCF2_VCA, knob_LEVEL_VCF2, "")
TARGET(Output_CLEANF_PAN, ledbutton_Cleanfeed_M_S, knob_PAN_Cleanfeed, "CF Pan")
TARGET(Output_VCF1_PAN, ledbutton_VCF1_M_S, knob_PAN_VCF1, "VCF1 Pan")
TARGET(Output_VCF2_PAN, ledbutton_VCF2_M_S, knob_PAN_VCF2, "VCF2 Pan")


TARGET(Output_VCO1_PITCH, ledbutton_VCO1, knob_VCO1_Pitch, "VCO1 Pitch")
TARGET(Output_VCO2_PITCH, ledbutton_VCO2, knob_VCO2_Pitch, "VCO2 Pitch")
TARGET(Output_VCO3_PITCH, ledbutton_VCO3, knob_VCO3_Pitch, "VCO3 Pitch")
TARGET(Output_VCO4_PITCH, ledbutton_VCO4, knob_VCO4_Pitch, "VCO4 Pitch")
TARGET(Output_VCO5_PITCH, ledbutton_VCO5, knob_VCO5_Pitch, "VCO5 Pitch")
TARGET(Output_VCO6_PITCH, ledbutton_VCO6, knob_VCO6_Pitch, "VCO6 Pitch")
TARGET(Output_VCO7_PITCH, ledbutton_VCO7, knob_VCO7_Pitch, "VCO7 Pitch")

MENU(VCO1, ledbutton_VCO1, "VCO1")
ENTRY("Pitch", MenuEntry_Pitch, Output_VCO1_PITCH)
CUSTOMENTRY("Wave", MenuEntry_Waveform3, SEL1TRI)
ENTRY("PW", MenuEntry_Pulsewidth, Output_VCO1_PW)
ENTRY("VCFMix", MenuEntry_FilterMix, Output_VCO1_MIX1)
ENTRY("RM Mix", MenuEntry_FilterMix, Output_RM1_MIX1)
ENDMENU()

MENU(VCO2, ledbutton_VCO2, "VCO2")
ENTRY("Pitch", MenuEntry_Pitch, Output_VCO2_PITCH)
CUSTOMENTRY("Wave", MenuEntry_Waveform3, SEL2TRI)
ENTRY("PW", MenuEntry_Pulsewidth, Output_VCO2_PW)
ENTRY("FM", MenuEntry_Value, Output_VCO123_FM1)
ENTRY("VCFMix", MenuEntry_FilterMix, Output_VCO2_MIX1)
ENTRY("RM Mix", MenuEntry_FilterMix, Output_RM1_MIX1)
ENDMENU()

MENU(VCO3, ledbutton_VCO3, "VCO3")
ENTRY("Pitch", MenuEntry_Pitch, Output_VCO3_PITCH)
CUSTOMENTRY("Wave", MenuEntry_Waveform3, SEL3TRI)
ENTRY("PW", MenuEntry_Pulsewidth, Output_VCO3_PW)
ENTRY("FM", MenuEntry_Value, Output_VCO123_FM2)
ENTRY("VCFMix", MenuEntry_FilterMix, Output_VCO3_MIX1)
ENTRY("RM Mix", MenuEntry_FilterMix, Output_RM1_MIX1)
ENDMENU()

MENU(VCO4, ledbutton_VCO4, "VCO4")
ENTRY("Pitch", MenuEntry_Pitch, Output_VCO4_PITCH)
CUSTOMENTRY("Wave", MenuEntry_Waveform1, SEL4SAW)
ENTRY("VCFMix", MenuEntry_FilterMix, Output_VCO4567_MIX1)
ENDMENU()

MENU(VCO5, ledbutton_VCO5, "VCO5")
ENTRY("Pitch", MenuEntry_Pitch, Output_VCO5_PITCH)
CUSTOMENTRY("Wave", MenuEntry_Waveform1, SEL5SAW)
ENTRY("VCFMix", MenuEntry_FilterMix, Output_VCO4567_MIX1)
ENDMENU()

MENU(VCO6, ledbutton_VCO6, "VCO6")
ENTRY("Pitch", MenuEntry_Pitch, Output_VCO6_PITCH)
CUSTOMENTRY("Wave", MenuEntry_Waveform1, SEL6SAW)
ENTRY("VCFMix", MenuEntry_FilterMix, Output_VCO4567_MIX1)
ENDMENU()

MENU(VCO7, ledbutton_VCO7, "VCO7")
ENTRY("Pitch", MenuEntry_Pitch, Output_VCO7_PITCH)
CUSTOMENTRY("Wave", MenuEntry_Waveform1, SEL7SAW)
ENTRY("VCFMix", MenuEntry_FilterMix, Output_VCO4567_MIX1)
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

