#include "PanHeader.h"


extern void WriteKnob(int id, uint32_t value);
extern void WriteWithSubKnob(int id, int subid, uint32_t value);

void Teensy_KnobChanged(int ID, uint32_t value)
{
	switch (ID)
	{
		//case knob_MASTER_TUNE: WriteKnob()




	case knob_FM_1_to_2: WriteKnob(Output_VCO123_FM1, value); break;
	case knob_FM_2_to_3: WriteKnob(Output_VCO123_FM2, value); break;
	case knob_VCO1_Pitch: WriteKnob(Output_VCO1_PITCH, value); break;
	case knob_VCO2_Pitch: WriteKnob(Output_VCO2_PITCH, value); break;
	case knob_VCO3_Pitch: WriteKnob(Output_VCO3_PITCH, value); break;
	
	case knob_VCO1_PW: WriteKnob(Output_VCO1_PW, value); break;
	case knob_VCO2_PW: WriteKnob(Output_VCO2_PW, value); break;
	case knob_VCO3_PW: WriteKnob(Output_VCO3_PW, value); break;

	case knob_VCO4_Pitch: WriteKnob(Output_VCO4_PITCH, value); break;
	case knob_VCO5_Pitch: WriteKnob(Output_VCO5_PITCH, value); break;
	case knob_VCO6_Pitch: WriteKnob(Output_VCO6_PITCH, value); break;
	case knob_VCO7_Pitch: WriteKnob(Output_VCO7_PITCH, value); break;

	case knob_VCF1_Resonance: WriteKnob(Output_VCF1_RES, value); break;
	case knob_VCF1_Frequency: WriteKnob(Output_VCF1_CV, value); break;
	case knob_VCF1_Spectrum_Mod: WriteKnob(Output_VCF1_CROSSMOD, value); break;

	case knob_VCF2_Resonance: WriteKnob(Output_VCF2_RES, value); break;
	case knob_VCF2_Feed_Back: WriteKnob(Output_VCF2_FB, value); break;
	case knob_VCF2_Spectrum_mod: WriteKnob(Output_VCF2_CROSSMOD, value); break;

	case knob_Bank_Low: WriteKnob(Output_VCF2_L_CV, value); break;
	case knob_Bank_Low_Level: WriteKnob(Output_VCF2_L_MIX, value); break;
	
	case knob_Bank_High: WriteKnob(Output_VCF2_H_CV, value); break;
	case knob_Bank_High_Level: WriteKnob(Output_VCF2_H_MIX, value); break;

	case knob_Bank_Mid_1: WriteKnob(Output_VCF2_M1_CV, value); break;
	case knob_Bank_Mid_1_Level: WriteKnob(Output_VCF2_M1_MIX, value); break;

	case knob_Bank_Mid_2: WriteKnob(Output_VCF2_M2_CV, value); break;
	case knob_Bank_Mid_2_Level: WriteKnob(Output_VCF2_M2_MIX, value); break;

	case knob_PAN_Cleanfeed: WriteKnob(Output_CLEANF_PAN, value); break;
	case knob_PAN_VCF1: WriteKnob(Output_VCF1_PAN, value); break;
	case knob_PAN_VCF2: WriteKnob(Output_VCF2_PAN, value); break;

	case knob_LEVEL_Cleanfeed: WriteKnob(Output_CLEANF_LEVEL, value); break;
	case knob_LEVEL_VCF1: WriteKnob(Output_VCF1_LEVEL, value); break;
	case knob_LEVEL_VCF2: WriteKnob(Output_VCF2_LEVEL, value); break;








	case knob_MIXER_VCO1:WriteKnob(Output_VCO1_MIX1, value); break;
	case knob_MIXER_VCO2:WriteKnob(Output_VCO2_MIX1, value); break;
	case knob_MIXER_VCO3:WriteKnob(Output_VCO3_MIX1, value); break;
	case knob_MIXER_VCO4_7:WriteKnob(Output_VCO4567_MIX1, value); break;
	case knob_MIXER_RM1_2:WriteKnob(Output_RM1_MIX1, value); break;
	case knob_MIXER_NOISE_ANAWT:WriteKnob(Output_WHITENS_MIX1, value); break;
	case knob_MIXER_NOISE_DIG:WriteKnob(Output_DIGINS_MIX1, value); break;
	case knob_MIXER_NOISECOLOR:WriteKnob(Output_NOISE_COLOR, value); break;


	case knob_MIXER_VCO4:WriteKnob(Output_VCO4_DRY_MIX, value); break;
	case knob_MIXER_VCO5:WriteKnob(Output_VCO5_DRY_MIX, value); break;
	case knob_MIXER_VCO6:WriteKnob(Output_VCO6_DRY_MIX, value); break;
	case knob_MIXER_VCO7:WriteKnob(Output_VCO7_DRY_MIX, value); break;
	case knob_MIXER_RM2_3:WriteKnob(Output_RM2_MIX3, value); break;
	case knob_MIXER_SUMQUAD: printf("TODODODOTODODODOTODODO!!\n"); break;
	case knob_MIXER_NOISE_BROWN:WriteKnob(Output_BN_MIX3, value); break;
	case knob_MIXER_NOISE_VIOLET:WriteKnob(Output_PUN_MIX, value); break;
		//case knob_TOTAL_OUT: WriteKnob(Output_, value); break;
	}
	printf("knob %s: %d\n", Knobs[ID].name, value);

}

void Teensy_ButtonPressed(int ID, int value)
{
	printf("button %s: %d\n", Buttons[ID].name, Buttons[ID].value ? 1 : 0);



	/*
modes:

	press a target, get detail menu for just target

	
	press a source once
		- all bound targets light up
		- screen menu 
			- source details + current modulation?
			- target amounts	
		- select all sources that apply (with submenu)
	
	

	*/
}

