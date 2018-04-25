
#ifndef KNOB
	#define KNOB(name,x,y,mintext,maxtext,fpid,label)
#endif
#ifndef SLIDER
#define SLIDER(name,x,y,mintext,maxtext, fpid, label)
#endif
#ifndef LEDBUTTON
	#define LEDBUTTON(name,x,y, fpid)
#endif
#ifndef LED
	#define LED(name,x,y)
#endif

#ifndef ENCODER
#define ENCODER(name,x,y)
#endif

#ifndef SCREEN
#define SCREEN(name,x,y,w,h)
#endif

#ifndef FADER
#define FADER(name,x,y)
#endif

KNOB(MASTER_TUNE, 1,1,"","",0,"Master Tune")
LEDBUTTON(VCO1, 2.3,1,103)
KNOB(VCO1_Pitch, 3,1, "-2_oct","+2_oct",32,"VCO1 Pitch")
KNOB(VCO1_PW, 4, 1, "", "",16,"VCO1 PW")
LEDBUTTON(VCO4, 5.2, 1,99)
KNOB(VCO4_Pitch,6,1,"min","max",48,"VCO3 Pitch" )

KNOB(FM_1_to_2,1,2,"0","",8, "FM 1 to 2")
LEDBUTTON(VCO2,2.3,2,102)
KNOB(VCO2_Pitch,3,2,"-1/3d","+1/3d",40, "VCO2 Pitch")
KNOB(VCO2_PW,4,2,"","",24,"VCO2 PW")
LEDBUTTON(VCO5,5.2,2,98)
KNOB(VCO5_Pitch,6,2,"0","+3_oct",56, "VCO5 Pitch")

KNOB(FM_2_to_3,1,3,"0","",4,"FM 2 to 3")
LEDBUTTON(VCO3,2.3,3,101)
KNOB(VCO3_Pitch,3,3,"-1/3d","+1/3d",36,"VCO3 Pitch")
KNOB(VCO3_PW,4,3,"","",20, "VCO3 PW")
LEDBUTTON(VCO6,5.2,3,97)
KNOB(VCO6_Pitch,6,3,"0","+3_oct",52, "VCO6 Pitch")

LEDBUTTON(VCO7,5.2,4,96)
KNOB(VCO7_Pitch,6,4,"0","+3_oct",12, "VCO7 Pitch")


LEDBUTTON(Interval_spread, 2.3, 4,100)
LEDBUTTON(Spread, 7, 4,55)

#define VCF1X 7.7
#define VCF1Y 1

KNOB(VCF1_Resonance,VCF1X,VCF1Y,"min","max",1, "VCF1 Res")
LEDBUTTON(VCF1_ResonanceBut,VCF1X+0.2,VCF1Y+1,54)
LED(VCF1_O_L,VCF1X+0.5,VCF1Y-0.2)
KNOB(VCF1_Spectrum_Mod,VCF1X+1.5,VCF1Y,"0","",33,"VCF1 Spectrum")
LEDBUTTON(VCF1_Spectrum_ModBut,VCF1X+1.7,VCF1Y+1,53)

KNOB(VCF1_Frequency,VCF1X+0.75,VCF1Y + 1.5,"min","max",17,"VCF1 Freq")
LEDBUTTON(VCF1_FrequencyBut,VCF1X+0.2,VCF1Y+2.5,52)
LEDBUTTON(VCF1_Routing, VCF1X+1.7, VCF1Y + 2.5,51)

#define VCF2X 11
#define VCF2Y 1.1


KNOB(VCF2_Spectrum_mod,VCF2X,VCF2Y,"0","",5,"VCF2 Spectrum")
KNOB(VCF2_Feed_Back,VCF2X+1,VCF2Y,"0","",37, "VCF2 Feedback")
KNOB(VCF2_Resonance,VCF2X+2,VCF2Y,"0","",21, "VCF2 Resonance")
LED(VCF2_O_L,VCF2X+2.5,VCF2Y-0.2)

LEDBUTTON(VCF2_SPM,VCF2X +0.2,VCF2Y+1,49)
LEDBUTTON(VCF2_F_B,VCF2X + 0.2,VCF2Y+1.7,50)
LEDBUTTON(VCF2_RES,VCF2X + 0.2,VCF2Y+2.5,48)


#define BANKX 14.3
#define BANKY 1
KNOB(Bank_Low,BANKX,BANKY,"min","max",53,"VCF2 low")
KNOB(Bank_Mid_1,BANKX+1,BANKY,"min","max",13, "VCF2 mid1")
KNOB(Bank_Mid_2,BANKX+2,BANKY,"min","max",45, "VCF2 mid2")
KNOB(Bank_High,BANKX+3,BANKY,"min","max",29, "VCF2 high")

LEDBUTTON(LowButton,BANKX+0.8,BANKY+1,47)
LEDBUTTON(Mid1Button,BANKX+1.8,BANKY+1,46)
LEDBUTTON(Mid2Button,BANKX+2.8,BANKY+1,45)
LEDBUTTON(HighButton,BANKX+3.8,BANKY+1,44)


KNOB(Bank_Low_Level,BANKX,BANKY+1.5,"min","max",61, "VCF2 low lvl")
KNOB(Bank_Mid_1_Level,BANKX+1,BANKY+1.5,"min","max",3, "VCF2 mid1 lvl")
KNOB(Bank_Mid_2_Level,BANKX+2,BANKY+1.5,"min","max",35, "VCF2 mid2 lvl")
KNOB(Bank_High_Level,BANKX+3,BANKY+1.5,"min","max",19, "VCF2 high lvl")

LEDBUTTON(PB1but,BANKX+0.8,BANKY+2.5,43)
LEDBUTTON(PB2but,BANKX+1.8,BANKY+2.5,41)
LEDBUTTON(PB3but,BANKX+2.8,BANKY+2.5,42)
LEDBUTTON(PB4but,BANKX+3.8,BANKY+2.5,40)

#define AMPX 19
#define AMPY 1.5

LEDBUTTON(Cleanfeed_M_S,AMPX+0.5,AMPY+0.5,79)
KNOB(PAN_Cleanfeed,AMPX+1,AMPY,"L","R",2, "Clean Pan")
LEDBUTTON(VCF1_M_S,AMPX+2.5,AMPY+0.5,78)
KNOB(PAN_VCF1,AMPX+3,AMPY,"L","R",34, "VCF1 Pan")
LEDBUTTON(VCF2_M_S,AMPX+4.5,AMPY+0.5,77)
KNOB(PAN_VCF2,AMPX+5,AMPY,"L","R",18, "VCF2 Pan")

LEDBUTTON(Cleanfeed_VCA,AMPX+.5,AMPY+1.5,76)
KNOB(LEVEL_Cleanfeed,AMPX+1,AMPY+1,"L","R",50, "Clean lvl")
LEDBUTTON(VCF1_VCA,AMPX+2.5,AMPY+1.5,75)
KNOB(LEVEL_VCF1,AMPX+3,AMPY+1,"L","R",10, "VCF1 lvl")
LEDBUTTON(VCF2_VCA,AMPX+4.5,AMPY+1.5,74)
KNOB(LEVEL_VCF2,AMPX+5,AMPY+1,"L","R",42, "VCF2 lvl")

LEDBUTTON(Cleanfeed_FX,AMPX+1.2,AMPY+2.2,71)
LEDBUTTON(VCF1_FX,AMPX+3.2,AMPY+2.2,73)
LEDBUTTON(VCF2_FX,AMPX+5.2,AMPY+2.2,72)

KNOB(TOTAL_OUT, AMPX + 5, AMPY + 3, "min", "max",26, "Master lvl")

#define ADSRX VCF2X
#define ADSRY 6

LEDBUTTON(ADSR,ADSRX-0.5,ADSRY+0.5,37)
KNOB(ADSR_A,ADSRX,ADSRY,"","",7, "Attack")
KNOB(ADSR_D,ADSRX+1,ADSRY,"","",39, "Decay")
KNOB(ADSR_S,ADSRX+2,ADSRY,"","",23, "Sustain")
KNOB(ADSR_R,ADSRX+3,ADSRY,"","",55, "Release")


#define ADX VCF2X-0.25
#define ADY 7

LEDBUTTON(AD,ADX-0.5,ADY+0.5, 39)
KNOB(AD_A,ADX,ADY,"","",15, "Attack")
KNOB(AD_D,ADX+1,ADY,"","",47, "Decay")

#define LFOX ADX+2.5
#define LFOY ADY

LEDBUTTON(LFO, LFOX + 1.6, LFOY + 0.5, 17)
KNOB(LFO_SPEED, LFOX, LFOY, "", "",31, "LFO Rate")
KNOB(LFO_SHAPE, LFOX + 1, LFOY, "", "",63, "LFO Shape")

LEDBUTTON(CHASE, 16, 4.6,4)
LEDBUTTON(STASH, 17, 4.6,3)

LEDBUTTON(CANCEL_LEFT, ADSRX-0.4, ADSRY-0.7, 38)
LEDBUTTON(PARTIAL, ADSRX + 0.4, ADSRY - 0.7, 36)
LEDBUTTON(FINAL, ADSRX+3 - 0.1, ADSRY - 0.7, 6)
LEDBUTTON(CANCEL_RIGHT, ADSRX+3 + 0.7, ADSRY - 0.7, 5)

LEDBUTTON(ZERO, 1, 5.8, 95)
LEDBUTTON(PORTAMENTO_LEFT, 2.7, 6, 93)
LEDBUTTON(OCTUP_LEFT, 2.2, 6.5, 92)
LEDBUTTON(OCTDOWN_LEFT, 1.7, 7, 91)
LEDBUTTON(PORTAMENTO_RIGHT, 23, 6, 69)
LEDBUTTON(OCTUP_RIGHT, 23.5, 6.5, 62)
LEDBUTTON(OCTDOWN_RIGHT, 24, 7, 58)

#define BROWX 3.5
#define BROWY 8
#define W 0.7
LEDBUTTON(LEFT_MOD, BROWX, BROWY-1, 87)
LEDBUTTON(LEFT_UNAC, BROWX+1*W, BROWY - 1, 86)
LEDBUTTON(LEFT_SUS, BROWX + 2 * W, BROWY - 1, 85)
LEDBUTTON(LEFT_1, BROWX, BROWY, 84)
LEDBUTTON(LEFT_2, BROWX+1*W, BROWY, 83)
LEDBUTTON(LEFT_3, BROWX + 2 * W, BROWY, 82)
LEDBUTTON(LEFT_4, BROWX + 3 * W, BROWY, 81)
LEDBUTTON(LEFT_5, BROWX + 4 * W, BROWY, 31)
LEDBUTTON(LEFT_6, BROWX + 5 * W, BROWY, 30)
LEDBUTTON(LEFT_7, BROWX + 6 * W, BROWY, 29)
LEDBUTTON(LEFT_8, BROWX + 7 * W, BROWY, 28)
LEDBUTTON(LEFT_MORE, BROWX + 8 * W, BROWY, 27)
#undef BROWX
#define BROWX 16
LEDBUTTON(SOURCE_LFO, BROWX+1, BROWY-1, 2)
LEDBUTTON(SOURCE_AD, BROWX + 1 + 1*W, BROWY - 1, 10)
LEDBUTTON(SOURCE_ADSR, BROWX + 1 + 2*W, BROWY - 1, 9)
LEDBUTTON(RIGHT_VLFDIRT, BROWX + 5 * W, BROWY-1, 8)
LEDBUTTON(RIGHT_MOD, BROWX + 6 * W, BROWY-1, 68)
LEDBUTTON(RIGHT_UNAC, BROWX + 7 * W, BROWY-1, 67)
LEDBUTTON(RIGHT_SUS, BROWX + 8 * W, BROWY-1, 63)
LEDBUTTON(RIGHT_MORE, BROWX, BROWY, 16)
LEDBUTTON(RIGHT_1, BROWX + 1 * W, BROWY, 15)
LEDBUTTON(RIGHT_2, BROWX + 2 * W, BROWY, 14)
LEDBUTTON(RIGHT_3, BROWX + 3 * W, BROWY, 13)
LEDBUTTON(RIGHT_4, BROWX + 4 * W, BROWY, 12)
LEDBUTTON(RIGHT_5, BROWX + 5 * W, BROWY, 11)
LEDBUTTON(RIGHT_6, BROWX + 6 * W, BROWY, 61)
LEDBUTTON(RIGHT_7, BROWX + 7 * W, BROWY, 60)
LEDBUTTON(RIGHT_8, BROWX + 8 * W, BROWY, 59)
#undef BROWX

#define BROWX VCF2X+1.7
LEDBUTTON(VELOCITY, BROWX, BROWY-1, 23)
LEDBUTTON(AT, BROWX, BROWY - 0.5, 22)
LEDBUTTON(ZPRIME, BROWX, BROWY, 21)
LEDBUTTON(X, BROWX - 3 * W, BROWY, 26)
LEDBUTTON(Y, BROWX - 2 * W, BROWY, 25)
LEDBUTTON(Z, BROWX - 1 * W, BROWY, 24)
LEDBUTTON(KBCV, BROWX+1*W, BROWY, 20)
LEDBUTTON(KBGATE, BROWX+2*W, BROWY, 19)
LEDBUTTON(KBTRIG, BROWX+3*W, BROWY, 18)
#undef BROWX
#undef W

#define SCREENX (VCF2X+0.8)
#define SCREENY (VCF2Y+1)

#define SCREENSIZE 2.0f

SCREEN(mainscreen,SCREENX,SCREENY, SCREENSIZE,(SCREENSIZE*(800.0f/480.0f)))
ENCODER(leftencoder,SCREENX-1,SCREENY+2)
ENCODER(rightencoder,SCREENX+2.4,SCREENY+2)


#define MIXER1X 4
#define MIXER1Y 5.5
#define W 0.7

SLIDER(MIXER_VCO1, MIXER1X, MIXER1Y, "", "",44, "VCO1 mix")
SLIDER(MIXER_VCO2, MIXER1X+1*W, MIXER1Y, "", "",28, "VCO2 mix")
SLIDER(MIXER_VCO3, MIXER1X+2*W, MIXER1Y, "", "",60, "VCO3 mix")

SLIDER(MIXER_VCO4_7,      MIXER1X + 3 * W, MIXER1Y, "", "",49, "VCO4567 mix")
SLIDER(MIXER_NOISE_DIG,   MIXER1X + 4 * W, MIXER1Y, "", "", 25, "DigNoise mix")
SLIDER(MIXER_NOISECOLOR,  MIXER1X + 5 * W, MIXER1Y, "", "", 9, "ColNoise mix")
SLIDER(MIXER_RM1_2,       MIXER1X + 6 * W, MIXER1Y, "", "",9, "RM12 mix")
SLIDER(MIXER_NOISE_ANAWT, MIXER1X + 7 * W, MIXER1Y, "", "",41, "AnaNoise mix")


#define MIXER2X 16
#define MIXER2Y 5.5


SLIDER(MIXER_SUMQUAD, MIXER2X + 4 * W, MIXER2Y, "", "",27, "SumQuad mix")
SLIDER(MIXER_VCO4, MIXER2X, MIXER2Y, "", "",59, "VCO4 mix")
SLIDER(MIXER_VCO5, MIXER2X + 1*W, MIXER2Y, "", "", -1,"VCO5 mix")
SLIDER(MIXER_VCO6, MIXER2X + 2*W, MIXER2Y, "", "",22, "VCO6 mix")
SLIDER(MIXER_VCO7, MIXER2X + 3*W, MIXER2Y, "", "",54, "VCO7 mix")
SLIDER(MIXER_RM2_3, MIXER2X + 5*W, MIXER2Y, "", "", -1, "RM23 mix")
SLIDER(MIXER_NOISE_BROWN, MIXER2X + 6*W, MIXER2Y, "", "",30, "Brown mix")
SLIDER(MIXER_NOISE_VIOLET, MIXER2X + 7*W, MIXER2Y, "", "",62, "Violet mix")

#undef W

#undef SCREEN
#undef FADER
#undef ENCODER
#undef LEDBUTTON
#undef KNOB
#undef LED
#undef SLIDER