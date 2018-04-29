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

TARGET(Output_VCO1_PITCH, ledbutton_VCO1, knob_VCO1_Pitch, "VCO1 Pitch")
TARGET(Output_VCO2_PITCH, ledbutton_VCO2, knob_VCO2_Pitch, "VCO2 Pitch")
TARGET(Output_VCO3_PITCH, ledbutton_VCO3, knob_VCO3_Pitch, "VCO3 Pitch")
TARGET(Output_VCO4_PITCH, ledbutton_VCO4, knob_VCO4_Pitch, "VCO4 Pitch")
TARGET(Output_VCO5_PITCH, ledbutton_VCO5, knob_VCO5_Pitch, "VCO5 Pitch")
TARGET(Output_VCO6_PITCH, ledbutton_VCO6, knob_VCO6_Pitch, "VCO6 Pitch")
TARGET(Output_VCO7_PITCH, ledbutton_VCO7, knob_VCO7_Pitch, "VCO7 Pitch")

TARGET(Output_VCO1_PW, ledbutton_VCO1, knob_VCO1_PW, "VCO1 PW")
TARGET(Output_VCO2_PW, ledbutton_VCO2, knob_VCO2_PW, "VCO2 PW")
TARGET(Output_VCO3_PW, ledbutton_VCO3, knob_VCO3_PW, "VCO3 PW")
TARGET(Output_VCO123_FM1, ledbutton_VCO1, knob_FM_1_to_2, "FM1")
TARGET(Output_VCO123_FM2, ledbutton_VCO2, knob_FM_2_to_3, "FM2")

TARGET(Output_VCF1_RES, ledbutton_VCF1_ResonanceBut, knob_VCF1_Resonance, "VCF1 res")
TARGET(Output_VCF1_CROSSMOD, ledbutton_VCF1_Spectrum_ModBut, knob_VCF1_Spectrum_Mod, "VCF1 SPM")
TARGET(Output_VCF2_RES, ledbutton_VCF2_RES, knob_VCF2_Resonance, "VCF2 Res")
TARGET(Output_VCF2_CROSSMOD, ledbutton_VCF2_SPM, knob_VCF2_Spectrum_mod, "VCF2 SPM")

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

TARGET(Output_NOISE_COLOR, __LEDBUTTON_COUNT, knob_MIXER_NOISECOLOR, "")

//TARGET(VCA_FXPOT1, ledbutton_, knob_)
//TARGET(VCA_FXPOT2, ledbutton_, knob_)
//TARGET(VCA_FXPOT3, ledbutton_, knob_)

TARGET(Output_VCO1_VCFMIX, __LEDBUTTON_COUNT, __KNOB_COUNT, "")
TARGET(Output_VCO2_VCFMIX, __LEDBUTTON_COUNT, __KNOB_COUNT, "")
TARGET(Output_VCO3_VCFMIX, __LEDBUTTON_COUNT, __KNOB_COUNT, "")
TARGET(Output_RM1_VCFMIX, __LEDBUTTON_COUNT, __KNOB_COUNT, "")
TARGET(Output_WHITENS_VCFMIX, __LEDBUTTON_COUNT, __KNOB_COUNT, "")
TARGET(Output_DIGINS_VCFMIX, __LEDBUTTON_COUNT, __KNOB_COUNT, "")
TARGET(Output_EXT_VCFMIX, __LEDBUTTON_COUNT, __KNOB_COUNT, "")
TARGET(Output_VCO4567_VCFMIX, __LEDBUTTON_COUNT, __KNOB_COUNT, "")
TARGET(Output_VCO1_LEVEL, __LEDBUTTON_COUNT, knob_MIXER_VCO1, "")
TARGET(Output_VCO2_LEVEL, __LEDBUTTON_COUNT, knob_MIXER_VCO2, "")
TARGET(Output_VCO3_LEVEL, __LEDBUTTON_COUNT, knob_MIXER_VCO3, "")
TARGET(Output_RM1_LEVEL, __LEDBUTTON_COUNT, knob_MIXER_RM1_2, "")
TARGET(Output_WHITENS_LEVEL, __LEDBUTTON_COUNT, knob_MIXER_NOISE_ANAWT, "")
TARGET(Output_DIGINS_LEVEL, __LEDBUTTON_COUNT, knob_MIXER_NOISE_DIG, "")
TARGET(Output_EXT_LEVEL, __LEDBUTTON_COUNT, __KNOB_COUNT, "")
TARGET(Output_VCO4567_LEVEL, __LEDBUTTON_COUNT, knob_MIXER_VCO4_7, "")

TARGET(Output_VCO4567_DRY_MIX, __LEDBUTTON_COUNT, knob_MIXER_SUMQUAD, "")


TARGET(Output_PUN_MIX, __LEDBUTTON_COUNT, knob_MIXER_NOISE_VIOLET, "")
TARGET(Output_VCO4_DRY_MIX, __LEDBUTTON_COUNT, knob_MIXER_VCO4, "")
TARGET(Output_VCO5_DRY_MIX, __LEDBUTTON_COUNT, knob_MIXER_VCO5, "")
TARGET(Output_VCO6_DRY_MIX, __LEDBUTTON_COUNT, knob_MIXER_VCO6, "")
TARGET(Output_VCO7_DRY_MIX, __LEDBUTTON_COUNT, knob_MIXER_VCO7, "")
TARGET(Output_RM2_MIX3, __LEDBUTTON_COUNT, knob_MIXER_RM2_3, "")
TARGET(Output_BN_MIX3, __LEDBUTTON_COUNT, knob_MIXER_NOISE_BROWN, "")

TARGET(Output_CLEANF_LIN, ledbutton_Cleanfeed_VCA, knob_LEVEL_Cleanfeed, "CF Mix")
TARGET(Output_VCF1_LIN, ledbutton_VCF1_VCA, knob_LEVEL_VCF1, "VCF1 Mix")
TARGET(Output_VCF2_LIN, ledbutton_VCF2_VCA, knob_LEVEL_VCF2, "VCF2 Mix")
TARGET(Output_CLEANF_LEVEL, __LEDBUTTON_COUNT, __KNOB_COUNT, "")
TARGET(Output_VCF1_LEVEL, __LEDBUTTON_COUNT, __KNOB_COUNT, "")
TARGET(Output_VCF2_LEVEL, __LEDBUTTON_COUNT, __KNOB_COUNT, "")
TARGET(Output_CLEANF_PAN, ledbutton_Cleanfeed_M_S, knob_PAN_Cleanfeed, "CF Pan")
TARGET(Output_VCF1_PAN, ledbutton_VCF1_M_S, knob_PAN_VCF1, "VCF1 Pan")
TARGET(Output_VCF2_PAN, ledbutton_VCF2_M_S, knob_PAN_VCF2, "VCF2 Pan")

TARGET(Output_MASTER_PITCH, __LEDBUTTON_COUNT, knob_MASTER_TUNE, "")
TARGET(Output_MASTER_LEVEL, __LEDBUTTON_COUNT, knob_TOTAL_OUT, "Master level")

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
ENTRY("FM", MenuEntry_Value, Output_VCO123_FM1)
ENTRY("VCFMix", MenuEntry_FilterMix, Output_VCO2_VCFMIX)
ENDMENU()

MENU(VCO3, ledbutton_VCO3, "VCO3")
ENTRY("Pitch", MenuEntry_Pitch, Output_VCO3_PITCH)
CUSTOMENTRY("Triangle", MenuEntry_Toggle, Switch_SEL3TRI)
CUSTOMENTRY("Pulse", MenuEntry_Toggle, Switch_SEL3SQR)
CUSTOMENTRY("Saw", MenuEntry_Toggle, Switch_SEL3SAW)
CUSTOMENTRY("Subosc", MenuEntry_Toggle, Switch_SEL3SUB)
ENTRY("PW", MenuEntry_Percentage, Output_VCO3_PW)
ENTRY("FM", MenuEntry_Value, Output_VCO123_FM2)
ENTRY("VCFMix", MenuEntry_FilterMix, Output_VCO3_VCFMIX)
ENDMENU()

MENU(VCO4, ledbutton_VCO4, "VCO4")
ENTRY("Pitch", MenuEntry_Pitch, Output_VCO4_PITCH)
CUSTOMENTRY("Saw", MenuEntry_Toggle, Switch_SEL4SAW)
ENTRY("VCFMix", MenuEntry_FilterMix, Output_VCO4567_VCFMIX)
ENDMENU()

MENU(VCO5, ledbutton_VCO5, "VCO5")
ENTRY("Pitch", MenuEntry_Pitch, Output_VCO5_PITCH)
CUSTOMENTRY("Saw", MenuEntry_Toggle, Switch_SEL5SAW)
ENTRY("VCFMix", MenuEntry_FilterMix, Output_VCO4567_VCFMIX)
ENDMENU()

MENU(VCO6, ledbutton_VCO6, "VCO6")
ENTRY("Pitch", MenuEntry_Pitch, Output_VCO6_PITCH)
CUSTOMENTRY("Saw", MenuEntry_Toggle, Switch_SEL6SAW)
ENTRY("VCFMix", MenuEntry_FilterMix, Output_VCO4567_VCFMIX)
ENDMENU()

MENU(VCO7, ledbutton_VCO7, "VCO7")
ENTRY("Pitch", MenuEntry_Pitch, Output_VCO7_PITCH)
CUSTOMENTRY("Saw", MenuEntry_Toggle, Switch_SEL7SAW)
ENTRY("VCFMix", MenuEntry_FilterMix, Output_VCO4567_VCFMIX)
ENDMENU()

EXTRABUTTON(VCF1, ledbutton_VCF1_ResonanceBut)
EXTRABUTTON(VCF1, ledbutton_VCF1_Routing)
EXTRABUTTON(VCF1, ledbutton_VCF1_Spectrum_ModBut)
MENU(VCF1, ledbutton_VCF1_FrequencyBut, "VCF1")
ENTRY("Freq", MenuEntry_Value, Output_VCF1_CV)
ENTRY("Resonance", MenuEntry_Value, Output_VCF1_RES)
CUSTOMENTRY("VCF parallel", MenuEntry_Toggle, Switch_SELVCF2POST)
ENDMENU()

EXTRABUTTON(VCF2, ledbutton_Mid1Button)
EXTRABUTTON(VCF2, ledbutton_Mid2Button)
EXTRABUTTON(VCF2, ledbutton_HighButton)
EXTRABUTTON(VCF2, ledbutton_VCF2_F_B)
EXTRABUTTON(VCF2, ledbutton_VCF2_SPM)
EXTRABUTTON(VCF2, ledbutton_PB1but)
EXTRABUTTON(VCF2, ledbutton_PB2but)
EXTRABUTTON(VCF2, ledbutton_PB3but)
EXTRABUTTON(VCF2, ledbutton_PB4but)
EXTRABUTTON(VCF2, ledbutton_VCF2_RES)

MENU(VCF2, ledbutton_LowButton, "VCF2")
ENTRY("Feedback", MenuEntry_Value, Output_VCF2_FB)
ENTRY("Resonance", MenuEntry_Value, Output_VCF2_RES)
ENTRY("Spectrum Mod", MenuEntry_Value, Output_VCF2_CROSSMOD)
ENTRY("Freq L", MenuEntry_Value, Output_VCF2_L_CV)
ENTRY("Mix L", MenuEntry_Value, Output_VCF2_L_MIX)
ENTRY("Freq M1", MenuEntry_Value, Output_VCF2_M1_CV)
ENTRY("Mix M1", MenuEntry_Value, Output_VCF2_M1_MIX)
ENTRY("Freq M2", MenuEntry_Value, Output_VCF2_M2_CV)
ENTRY("Mix M2", MenuEntry_Value, Output_VCF2_M2_MIX)
ENTRY("Freq H", MenuEntry_Value, Output_VCF2_H_CV)
ENTRY("Mix H", MenuEntry_Value, Output_VCF2_H_MIX)
CUSTOMENTRY("L0", MenuEntry_Toggle, Switch_SELVCF2L0)
CUSTOMENTRY("L1", MenuEntry_Toggle, Switch_SELVCF2L1)
ENDMENU()


EXTRABUTTON(EFFECTS1, ledbutton_VCF1_FX)
EXTRABUTTON(EFFECTS1, ledbutton_VCF2_FX)
MENU(EFFECTS1, ledbutton_Cleanfeed_FX, "Global FX")
CUSTOMENTRY("Type", MenuEntry_EffectType, Output_VCA_FXPOT1)
CUSTOMENTRY("Parameter 1", MenuEntry_EffectParam1, Output_VCA_FXPOT3)
CUSTOMENTRY("Parameter 2", MenuEntry_EffectParam2, Output_VCA_FXPOT2)
CUSTOMENTRY("Parameter 3", MenuEntry_EffectParam3, Output_VCA_FXPOT1)

CUSTOMENTRY("CLNFD", MenuEntry_Toggle, Switch_SELEFFECT3)
CUSTOMENTRY("VCF 1", MenuEntry_Toggle, Switch_SELEFFECT1)
CUSTOMENTRY("VCF 2", MenuEntry_Toggle, Switch_SELEFFECT2)
ENDMENU()

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
