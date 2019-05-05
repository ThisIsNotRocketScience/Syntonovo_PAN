#ifndef OUTPUT 
#define REDEFINEDOUTPUT
#define OUTPUT(name,codecport,codecpin, type,id, style,defaultvalue, label, category)
#endif

#ifndef OUTPUT_VIRT
#define REDEFINEDOUTPUTVIRT
#define OUTPUT_VIRT(name,codecport,codecpin, type,id, style,defaultvalue, label, category)
#endif

#ifndef SWITCH
#define REDEFINEDSWITCH
#define SWITCH(name,id,def)
#endif

#ifndef MODSOURCE
#define REDEFINEDMODSOURCE
#define MODSOURCE(name,type,id)
#endif

#ifndef CATEGORY
#define REDEFINEDCATEGORY
#define CATEGORY(name,label)
#endif

CATEGORY(EMPTY, "")

CATEGORY(VCO1, "Oscillator 1")
CATEGORY(VCO2, "Oscillator 2")
CATEGORY(VCO3, "Oscillator 3")
CATEGORY(VCO4, "Oscillator 4")
CATEGORY(VCO5, "Oscillator 5")
CATEGORY(VCO6, "Oscillator 6")
CATEGORY(VCO7, "Oscillator 7")
CATEGORY(VCO8, "Oscillator 8")

CATEGORY(VCF1, "VCF 1")
CATEGORY(VCF2, "VCF 2")
CATEGORY(VCF2A, "VCF 2A")
CATEGORY(VCF2B, "VCF 2B")
CATEGORY(VCF2C, "VCF 2C")
CATEGORY(VCF2D, "VCF 2D")
CATEGORY(Cleanfeed, "Cleanfeed")

CATEGORY(VCF1MIX, "VCF1 input mix")
CATEGORY(VCF2MIX, "VCF2 input mix")
CATEGORY(CLEANMIX, "Cleanfeed input mix")

CATEGORY(MASTERMIX, "Master mix")
CATEGORY(FX, "Output Effects")
CATEGORY(EXTERNAL, "External outputs")
CATEGORY(MODULATION, "Modulation Parameters")

CATEGORY(IGNORE, "to be ignored - for internal use only")



OUTPUT(VCO1_MIX1,		0,	0,	CTRL,	0x10,	CUSTOM,	0xffff, "VCO1 -> VCF1", VCF1MIX)
OUTPUT(WHITENS_MIX1,	1,	0,	CTRL,	0x18,	CUSTOM,	0, "Whitenoise -> VCF1", VCF1MIX)
OUTPUT(VCO1_MIX2,		0,	1,	CTRL,	0x11,	CUSTOM,	0xffff, "VCO1 -> VCF2", VCF2MIX)
OUTPUT(VCO4567_MIX2,	1,	1,	CTRL,	0x19,	CUSTOM,	0, "VCO4567 -> VCF2", VCF2MIX)
OUTPUT(VCO1_SUB2,		0,	2,	CTRL,	0x12,	CUSTOM,	0,"VCO1 Sub -> VCF2", VCF2MIX)
OUTPUT(VCO4567_MIX1,	1,	2,	CTRL,	0x1A,	CUSTOM,	0,"VCO4567 -> VCF1", VCF1MIX)
OUTPUT(VCO1_SUB1,		0,	3,	CTRL,	0x13,	CUSTOM,	0, "VCO1 Sub -> VCF1", VCF1MIX)
OUTPUT(WHITENS_MIX2,	1,	3,	CTRL,	0x1B,	CUSTOM,	0, "Whitenoise -> VCF2", VCF2MIX)
OUTPUT(VCO1_FREQ,		0,	4,	CTRL,	0x14,	CUSTOM,	0, "", IGNORE)
OUTPUT(VCO3_MIX2,		1,	4,	CTRL,	0x1C,	CUSTOM,	0, "VCO3 -> VCF2", VCF2MIX)
OUTPUT(VCO3_MIX1,		1,	5,	CTRL,	0x1D,	CUSTOM,	0, "VCO3 -> VCF1", VCF1MIX)
OUTPUT(VCO1_PW,			0,	6,	CTRL,	0x16,	LIN,	0x7fff, "Pulsewidth", VCO1 )
OUTPUT(VCO2_MIX1,		1,	6,	CTRL,	0x1E,	CUSTOM,	0, "VCO2 -> VCF1", VCF1MIX)
OUTPUT(VCO2_MIX2,		1,	7,	CTRL,	0x1F,	CUSTOM,	0, "VCO2 -> VCF2", VCF2MIX)
OUTPUT(VCO2_FREQ,		2,	0,	CTRL,	0x20,	CUSTOM,	0, "", IGNORE)
OUTPUT(VCO4_FREQ,		3,	0,	CTRL,	0x28,	CUSTOM,	0, "", IGNORE)
OUTPUT(VCO3_FREQ,		2,	1,	CTRL,	0x21,	CUSTOM,	0, "", IGNORE)
OUTPUT(VCO5_PW,			3,	1,	CTRL,	0x29,	LIN,	0x7fff, "Pulsewidth", VCO5)
OUTPUT(VCO3_PW,			2,	2,	CTRL,	0x22,	LIN,	0x7fff, "Pulsewidth", VCO3)
OUTPUT(VCO5_FREQ,		3,	2,	CTRL,	0x2A,	CUSTOM,	0, "", IGNORE)
OUTPUT(VCO2_PW,			2,	3,	CTRL,	0x23,	LIN,	0x7fff, "Pulsewidth", VCO2)
OUTPUT(VCO4_PW,			3,	3,	CTRL,	0x2B,	LIN,	0x7fff, "Pulsewidth", VCO4)
OUTPUT(RM1_MIX1,		2,	4,	CTRL,	0x24,	CUSTOM,	0, "Ringmod -> VCF1", VCF1MIX)
OUTPUT(VCO6_DRY_MIX,	3,	4,	CTRL,	0x2C,	CUSTOM,	0, "VCO6 -> Cleanfeed", CLEANMIX)
OUTPUT(DIGINS_MIX2,		2,	5,	CTRL,	0x25,	CUSTOM,	0, "Digital Noise -> VCF2", VCF2MIX)
OUTPUT(VCO5_DRY_MIX,	3,	5,	CTRL,	0x2D,	CUSTOM,	0, "VCO5 -> Cleanfeed", CLEANMIX)
OUTPUT(RM1_MIX2,		2,	6,	CTRL,	0x26,	LOG,	0, "Ringmod -> VCF2", VCF2MIX)
OUTPUT(VCO7_DRY_MIX,	3,	6,	CTRL,	0x2E,	CUSTOM,	0, "VCO7 -> Cleanfeed", CLEANMIX)
OUTPUT(DIGINS_MIX1,		2,	7,	CTRL,	0x27,	CUSTOM,	0, "Digital Noise -> VCF1", VCF1MIX)
OUTPUT(VCO4_DRY_MIX,	3,	7,	CTRL,	0x2F,	CUSTOM,	0, "VCO4 -> Cleanfeed", CLEANMIX)
OUTPUT(VCF2_R_LOG,		4,	0,	INT,	0x30,	CUSTOM,	0, "", IGNORE)
OUTPUT(BN_MIX3,			5,	0,	CTRL,	0x3F,	LOG,	0, "Brownnoise -> Cleanfeed", CLEANMIX)
OUTPUT(MASTER_LEVEL,	4,	1,	CTRL,	0x89,	LOG,	0xC000,"",IGNORE)
OUTPUT(VCF1_R_LOG,		5,	1,	INT,	0x3E,	CUSTOM,	0, "", IGNORE)
OUTPUT(VCO1SIN_MIX3,	4,	2,	CTRL,	0x70,	LOG,	0, "VCO1 sine -> Cleanfeed", CLEANMIX)
OUTPUT(VCF1_L_LOG,		5,	2,	INT,	0x3C,	CUSTOM,	0, "", IGNORE)
OUTPUT(VCF2_FX_R,		4,	3,	CTRL,	0x33,	CUSTOM,	0, "", IGNORE)
OUTPUT(PUN_MIX,			5,	3,	CTRL,	0x3D,	LOG,	0,"Purplenoise -> Cleanfeed", CLEANMIX)
OUTPUT(VCF2_L_LOG,		4,	4,	INT,	0x34,	CUSTOM,	0, "", IGNORE)
OUTPUT(VCO7_PW,			5,	4,	CTRL,	0x32,	LIN,	0x7fff, "Pulsewidth", VCO7)
OUTPUT(VCO6_PW,			4,	5,	CTRL,	0x35,	LIN,	0x7fff, "Pulsewidth", VCO7)
OUTPUT(VCO2SIN_MIX3,	5,	5,	CTRL,	0x71,	LOG,	0, "VCO2 sine -> Cleanfeed", CLEANMIX)
OUTPUT(VCF2_FX_L,		4,	6,	CTRL,	0x36,	CUSTOM,	0, "", IGNORE)
OUTPUT(VCO7_FREQ,		5,	6,	CTRL,	0x31,	CUSTOM,	0, "", IGNORE)
OUTPUT(VCO6_FREQ,		4,	7,	CTRL,	0x37,	CUSTOM,	0, "", IGNORE)
OUTPUT(VCO3SIN_MIX3,	5,	7,	CTRL,	0x72,	LOG,	0, "VCO3 sine -> Cleanfeed", CLEANMIX)
OUTPUT(VCF1_R_LIN,		6,	0,	INT,	0x47,	CUSTOM,	0, "", IGNORE)
OUTPUT(VCO123_FM2,		7,	0,	CTRL,	0x17,	INV,	0,"FM level", VCO2)
OUTPUT(CLEANF_L_LOG,	6,	1,	INT,	0x46,	CUSTOM,	0, "", IGNORE)
OUTPUT(VCF1_CROSSMOD,	7,	1,	CTRL,	0x4E,	INV,	0, "Cross modulation", VCF1)
OUTPUT(VCF1_L_LIN,		6,	2,	INT,	0x44,	CUSTOM,	0, "", IGNORE)
OUTPUT(VCF2_CROSSMOD,	7,	2,	CTRL,	0x4C,	INV,	0, "Cross modulation", VCF2)
OUTPUT(DNSSAW_MIX1,		6,	3,	CTRL,	0x73,	CUSTOM,	0, "Digital noise sawtooth -> VCF1", VCF1MIX)
OUTPUT(VCO123_FM3,		7,	3,	CTRL,	0x15,	INV,	0, "FM level", VCO3)
OUTPUT(RM2_MIX3,		6,	4,	CTRL,	0x39,	CUSTOM,	0, "Ringmod 2 -> Cleanfeed", CLEANMIX)
OUTPUT(CLEANF_FX_R,		7,	4,	CTRL,	0x43,	CUSTOM,	0, "", IGNORE)
OUTPUT(VCF1_FX_24,		6,	5,	CTRL,	0x3B,	CUSTOM,	0, "FX 24", VCF1)
OUTPUT(CLEANF_R_LOG,	7,	5,	INT,	0x42,	CUSTOM,	0, "", IGNORE)
OUTPUT(DNSSAW_MIX2,		6,	6,	CTRL,	0x74,	CUSTOM,	0, "Digital noise sawtooth -> VCF2", VCF2MIX)
OUTPUT(NOISE_COLOR,		7,	6,	CTRL,	0x41,	CUSTOM,	0, "Noise color", VCO8)
OUTPUT(VCF1_FX_12,		6,	7,	CTRL,	0x38,	CUSTOM,	0, "FX 24", VCF1)
OUTPUT(CLEANF_FX_L,		7,	7,	CTRL,	0x40,	CUSTOM,	0, "", IGNORE)

OUTPUT(VCF2_A_RES,		8,	0,	CTRL,	0x49,	INV,	0, "VCF2 A Resonance", VCF2)
OUTPUT(VCF2_D_RES,		9,	0,	CTRL,	0x5D,	INV,	0, "VCF2 D Resonance", VCF2)
OUTPUT(VCF2_C_RES,		8,	1,	CTRL,	0x56,	INV,	0, "VCF2 C Resonance", VCF2)
OUTPUT(VCF2_D_CV,		9,	1,	CTRL,	0x5F,	INV,	0x7fff, "VCF2 D Frequency", VCF2)
OUTPUT(VCF2_C_CV,		8,	2,	CTRL,	0x54,	INV,	0x7fff, "VCF2 C Frequency", VCF2)
OUTPUT(CLEANF_R_LIN,	9,	2,	INT,	0x5E,	CUSTOM,	0, "", IGNORE)
OUTPUT(VCF2_A_CV,		8,	3,	CTRL,	0x4A,	INV,	0x7fff, "VCF2 A Frequency", VCF2)
OUTPUT(CLEANF_L_LIN,	9,	3,	INT,	0x5C,	CUSTOM,	0, "", IGNORE)
OUTPUT(VCF1_RES,		8,	4,	CTRL,	0x4B,	LIN,	0,"Resonance", VCF1)
OUTPUT(VCF2_B_CV,		9,	4,	CTRL,	0x52,	INV,	0x7fff, "VCF2 B Frequency", VCF2)
OUTPUT(FX_L_RETURN,		8,	5,	CTRL,	0x55,	CUSTOM,	0, "", IGNORE)
OUTPUT(VCF2_B_RES,		9,	5,	CTRL,	0x51,	INV,	0, "VCF2 B Resonance", VCF2)
OUTPUT(VCF1_CV,			8,	6,	CTRL,	0x48,	INV,	0xffff,"Frequency", VCF1)
OUTPUT(VCF2_M_FB,		9,	6,	CTRL,	0x53,	LIN,	0, "M Feedback" ,VCF2)
OUTPUT(FX_R_RETURN,		8,	7,	CTRL,	0x57,	CUSTOM,	0, "", IGNORE)
OUTPUT(VCF2_P_FB,		9,	7,	CTRL,	0x50,	LIN,	0, "P Feedback", VCF2)

OUTPUT(VCF2_A_MIX,		10,	0,	CTRL,	0x67,	CUSTOM,	0, "VCF2 A Mix", VCF2)
OUTPUT(CV_EXT_3,		11,	0,	CTRL,	0x68,	LIN,	0, "Output 3", EXTERNAL)
OUTPUT(VCF2_C_MIX,		10,	1,	CTRL,	0x64,	CUSTOM,	0, "VCF2 C Mix", VCF2)
OUTPUT(CV_EXT_6,		11,	1,	CTRL,	0x69,	LIN,	0, "Output 6", EXTERNAL)
OUTPUT(VCF2_L_LIN,		10,	2,	INT,	0x66,	CUSTOM,	0, "", IGNORE)
OUTPUT(CV_EXT_5,		11,	2,	CTRL,	0x6A,	LIN,	0, "Output 5", EXTERNAL)
OUTPUT(VCF2_D_MIX,		10,	3,	CTRL,	0x65,	CUSTOM,	0, "VCF2 D Mix", VCF2)
OUTPUT(CV_EXT_4,		11,	3,	CTRL,	0x6B,	LIN,	0, "Output 4", EXTERNAL)
OUTPUT(VCA_FXPOT3,		10,	4,	CTRL,	0x5A,	LIN,	0, "Parameter 3", FX)
OUTPUT(CV_EXT_1,		11,	4,	CTRL,	0x6C,	LIN,	0, "Output 1", EXTERNAL)
OUTPUT(VCF2_B_MIX,		10,	5,	CTRL,	0x59,	CUSTOM,	0, "VCF2 B Mix", VCF2)
OUTPUT(CV_EXT_KB,		11,	5,	CTRL,	0x6D,	LIN,	0, "KB", EXTERNAL)
OUTPUT(VCA_FXPOT2,		10,	6,	CTRL,	0x5B,	LIN,	0, "Parameter 2", FX)
OUTPUT(CV_EXT_2,		11,	6,	CTRL,	0x6E,	LIN,	0, "Output 2", EXTERNAL)
OUTPUT(VCA_FXPOT1,		10,	7,	CTRL,	0x58,	LIN,	0, "Parameter 1", FX)
OUTPUT(VCF2_R_LIN,		11,	7,	INT,	0x6F,	CUSTOM,	0, "", IGNORE)

OUTPUT_VIRT(VCF1_LIN, VIRT, 0, CTRL, 0x80, LIN, 0,"VCF1 Level", MASTERMIX)
OUTPUT_VIRT(VCF2_LIN, VIRT, 0, CTRL, 0x81, LIN, 0, "VCF2 Level", MASTERMIX)
OUTPUT_VIRT(CLEANF_LIN, VIRT, 0, CTRL, 0x82, LIN, 0, "Cleanfeed Level", MASTERMIX)
OUTPUT_VIRT(VCF1_LEVEL, VIRT, 0, CTRL, 0x83, LOG, 0xffff,"",IGNORE)
OUTPUT_VIRT(VCF2_LEVEL, VIRT, 0, CTRL, 0x84, LOG, 0,"",IGNORE)
OUTPUT_VIRT(CLEANF_LEVEL, VIRT, 0, CTRL, 0x85, LOG, 0,"",IGNORE)
OUTPUT_VIRT(VCF1_PAN, VIRT, 0, CTRL, 0x86, LIN, 0x8000,"VCF1 Pan", MASTERMIX)
OUTPUT_VIRT(VCF2_PAN, VIRT, 0, CTRL, 0x87, LIN, 0x8000,"VCF2 Pan", MASTERMIX)
OUTPUT_VIRT(CLEANF_PAN, VIRT, 0, CTRL, 0x88, LIN, 0x8000, "Cleanfeed", MASTERMIX)

OUTPUT_VIRT(ZPRIME_SPEED, VIRT, 0, CTRL, 0xA0, LIN, 0x2000, "Toucher speed", MODULATION)

OUTPUT_VIRT(CHASE, VIRT, 0, CTRL, 0xB0, LIN, 0,"", IGNORE)
OUTPUT_VIRT(STASH, VIRT, 0, CTRL, 0xB1, LIN, 0,"", IGNORE)

OUTPUT_VIRT(VCO4567_DRY_MIX, VIRT, 0, CTRL, 0xCF, LIN, 0,"", IGNORE)

OUTPUT_VIRT(NOTE, VIRT, 0, CTRL, 0xE0, LIN, 0,"",IGNORE)

OUTPUT_VIRT(VCO1_PITCH, VIRT, 0, CTRL, 0xE1, LIN, 0x8000,"Frequency",VCO1)
OUTPUT_VIRT(VCO2_PITCH, VIRT, 0, CTRL, 0xE2, LIN, 0x8000,"Frequency",VCO2)
OUTPUT_VIRT(VCO3_PITCH, VIRT, 0, CTRL, 0xE3, LIN, 0x8000,"Frequency",VCO3)
OUTPUT_VIRT(VCO4_PITCH, VIRT, 0, CTRL, 0xE4, LIN, 0x8000,"Frequency",VCO4)
OUTPUT_VIRT(VCO5_PITCH, VIRT, 0, CTRL, 0xE5, LIN, 0x8000,"Frequency",VCO5)
OUTPUT_VIRT(VCO6_PITCH, VIRT, 0, CTRL, 0xE6, LIN, 0x8000,"Frequency",VCO6)
OUTPUT_VIRT(VCO7_PITCH, VIRT, 0, CTRL, 0xE7, LIN, 0x8000,"Frequency",VCO7)
//OUTPUT_VIRT(VCO8_PITCH, VIRT, 0, CTRL, 0xED, LIN, 0x8000,"Frequency",VCO8)

OUTPUT_VIRT(GATE, VIRT, 0, CTRL, 0xE8, LIN, 0, "", IGNORE)
OUTPUT_VIRT(RETRIGGER, VIRT, 0, CTRL, 0xE9, LIN, 0, "", IGNORE)

OUTPUT_VIRT(MASTER_PITCH, VIRT, 0, CTRL, 0xEA, LIN, 0x8000, "", IGNORE)
OUTPUT_VIRT(PORTAMENTO_TIME, VIRT, 0, CTRL, 0xEB, LIN, 0,"Portamento time", MODULATION)
OUTPUT_VIRT(MASTER_PITCH2, VIRT, 0, CTRL, 0xEC, LIN, 0x8000, "", IGNORE)

OUTPUT_VIRT(VCO1_OCTAVE, VIRT, 0, CTRL, 0xD0, LIN, 0x8000, "", IGNORE)
OUTPUT_VIRT(VCO2_OCTAVE, VIRT, 0, CTRL, 0xD1, LIN, 0x8000, "", IGNORE)
OUTPUT_VIRT(VCO3_OCTAVE, VIRT, 0, CTRL, 0xD2, LIN, 0x8000, "", IGNORE)
OUTPUT_VIRT(VCO4_OCTAVE, VIRT, 0, CTRL, 0xD3, LIN, 0x8000, "", IGNORE)
OUTPUT_VIRT(VCO5_OCTAVE, VIRT, 0, CTRL, 0xD4, LIN, 0x8000, "", IGNORE)
OUTPUT_VIRT(VCO6_OCTAVE, VIRT, 0, CTRL, 0xD5, LIN, 0x8000, "", IGNORE)
OUTPUT_VIRT(VCO7_OCTAVE, VIRT, 0, CTRL, 0xD6, LIN, 0x8000, "", IGNORE)
OUTPUT_VIRT(VCO8_OCTAVE, VIRT, 0, CTRL, 0xD7, LIN, 0x8000, "", IGNORE)

SWITCH(SEL1TRI, 47, 0)
SWITCH(SEL1SAW, 46, 0)
SWITCH(SEL1SQR, 45, 1)
SWITCH(SEL1SUB, 44, 0)
SWITCH(SEL2TRI, 43, 0)
SWITCH(SEL2SAW, 42, 0)
SWITCH(SEL2SQR, 41, 1)
SWITCH(SEL2SUB, 40, 0)

SWITCH(SEL3TRI, 39, 0)
SWITCH(SEL3SAW, 38, 0)
SWITCH(SEL3SQR, 37, 1)
SWITCH(SEL3SUB, 36, 0)

// sync vco2 with vco1
SWITCH(SELVCOSYNC1, 35, 0)
// sync vco3 with vco2
SWITCH(SELVCOSYNC2, 34, 0)
// sync vco1 with vco3
SWITCH(SELVCOSYNC3, 33, 0)
SWITCH(SELGTRVCF2, 32, 0)

SWITCH(SEL4SQR, 31, 0)
SWITCH(SEL5SQR, 30, 0)
SWITCH(SEL6SQR, 29, 0)
SWITCH(SEL7SQR, 28, 0)
SWITCH(SELRMVCF2, 27, 0)
SWITCH(SELVCF21A, 26, 0)
SWITCH(SELVCF21B, 25, 0)
SWITCH(SELVCF22A, 24, 0)

SWITCH(SELVCF22B, 23, 0)
SWITCH(SELVCF23A, 22, 0)
SWITCH(SELVCF23B, 21, 0)
SWITCH(SELVCF24A, 20, 0)
SWITCH(SELVCF24B, 19, 0)
SWITCH(SELVCF21SER, 18, 0)
SWITCH(SELVCF23SER, 17, 0)
SWITCH(SELVCF23FSM, 16, 0)

SWITCH(SELVCF1MOST, 15, 0)
SWITCH(SELVCF2MOST, 14, 0)
SWITCH(SELVCLEANFMOST, 13, 0)
SWITCH(SELEF0, 12, 0)
SWITCH(SELEF1, 11, 0)
SWITCH(SELEF2, 10, 0)
SWITCH(SELEF3, 9,  0)
SWITCH(SELMUTEDNSAW, 8, 0)

SWITCH(SELVCF45FSM, 7, 0)
SWITCH(SELTUNEVCF, 6, 0)
SWITCH(SELVCF1AP, 5, 0)
SWITCH(SELVCF1BP, 4, 0)
SWITCH(SEQCLICK, 3, 0)
SWITCH(SELFSM1SRC, 2, 0)
SWITCH(SELFSM2SRC, 1, 0)
SWITCH(SELMUTEVCOTUNE, 0, 0)


SWITCH(SELPORTAMENTO, 48, 0)
SWITCH(SELSTACCATO, 49, 0)

SWITCH(SELSTASHOSC4567, 50, 0)
SWITCH(SELSTASHVCF2, 51, 0)

SWITCH(SELCHASEOSC4567, 52, 0)
SWITCH(SELCHASEVCF2, 53, 0)

// global sustain -> keep gate open
SWITCH(SELSUSTAINL, 54, 0)
SWITCH(SELSUSTAINR, 55, 0)

// global una corda -> shorten adsr release
SWITCH(SELUNACL, 56, 0)
SWITCH(SELUNACR, 57, 0)

//struct lfo {
//	uint16_t flags; // including key range
//	uint16_t speed;
//	int16_t depth;
//	int16_t shape;
//	uint16_t reset_phase;
//};
MODSOURCE(LFO0, 		LFO,			0x00)
MODSOURCE(LFO1, 		LFO, 			0x01)
MODSOURCE(LFO2, 		LFO, 			0x02)
MODSOURCE(LFO3, 		LFO, 			0x03)
MODSOURCE(LFO4, 		LFO, 			0x04)
MODSOURCE(LFO5, 		LFO, 			0x05)
MODSOURCE(LFO6, 		LFO, 			0x06)
MODSOURCE(LFO7, 		LFO, 			0x07)
MODSOURCE(LFO8, 		LFO, 			0x08)
MODSOURCE(LFO9, 		LFO, 			0x09)
MODSOURCE(LFO10, 		LFO, 			0x0A)
MODSOURCE(LFO11, 		LFO, 			0x0B)
MODSOURCE(LFO12, 		LFO, 			0x0C)
MODSOURCE(LFO13, 		LFO, 			0x0D)
MODSOURCE(LFO14, 		LFO, 			0x0E)
MODSOURCE(LFO15, 		LFO, 			0x0F)

//struct env {
//	uint16_t flags; // including env type, key range
//	uint16_t a;
//	uint16_t d;
//	uint16_t s;
//	uint16_t r;
//	//uint16_t curve;
//};
MODSOURCE(ENV0, 		ENVELOPE, 		0x10)
MODSOURCE(ENV1, 		ENVELOPE, 		0x11)
MODSOURCE(ENV2, 		ENVELOPE, 		0x12)
MODSOURCE(ENV3, 		ENVELOPE, 		0x13)
MODSOURCE(ENV4, 		ENVELOPE, 		0x14)
MODSOURCE(ENV5, 		ENVELOPE, 		0x15)
MODSOURCE(ENV6, 		ENVELOPE, 		0x16)
MODSOURCE(ENV7, 		ENVELOPE, 		0x17)
MODSOURCE(ENV8, 		ENVELOPE, 		0x18)
MODSOURCE(ENV9, 		ENVELOPE, 		0x19)
MODSOURCE(ENV10, 		ENVELOPE, 		0x1A)
MODSOURCE(ENV11, 		ENVELOPE, 		0x1B)
MODSOURCE(ENV12, 		ENVELOPE, 		0x1C)
MODSOURCE(ENV13, 		ENVELOPE, 		0x1D)
MODSOURCE(ENV14, 		ENVELOPE, 		0x1E)
MODSOURCE(ENV15, 		ENVELOPE, 		0x1F)

//struct controller {
//	int16_t scale;
//	int16_t deadzone;
//	//int16_t shape;
//};
MODSOURCE(X, 			CONTROLLER, 	0x20)
MODSOURCE(Y,			CONTROLLER, 	0x21)
MODSOURCE(Z, 			CONTROLLER, 	0x22)
MODSOURCE(ZPRIME, 		CONTROLLER, 	0x23)
MODSOURCE(PADL, 		CONTROLLER, 	0x24)
MODSOURCE(PADR, 		CONTROLLER, 	0x25)
MODSOURCE(SUSTAINL, 	CONTROLLER, 	0x26)
MODSOURCE(SUSTAINR, 	CONTROLLER, 	0x27)
MODSOURCE(UNACORDAL, 	CONTROLLER, 	0x28)
MODSOURCE(UNACORDAR, 	CONTROLLER, 	0x29)
MODSOURCE(PEDAL, 		CONTROLLER, 	0x2A)

//struct operator {
//	uint8_t modsource1;
//	uint8_t modsource2;
//	uint16_t op; // tbd plus, minus, mul, diff, sh, lag, etc.
//	uint16_t parameter; // parameter for selected op
//};
MODSOURCE(OPERATOR0, 	OPERATOR, 		0x30)
MODSOURCE(OPERATOR1, 	OPERATOR, 		0x31)
MODSOURCE(OPERATOR2, 	OPERATOR, 		0x32)
MODSOURCE(OPERATOR3, 	OPERATOR, 		0x33)
MODSOURCE(OPERATOR4, 	OPERATOR, 		0x34)
MODSOURCE(OPERATOR5, 	OPERATOR, 		0x35)
MODSOURCE(OPERATOR6, 	OPERATOR, 		0x36)
MODSOURCE(OPERATOR7, 	OPERATOR, 		0x37)
MODSOURCE(OPERATOR8, 	OPERATOR, 		0x38)
MODSOURCE(OPERATOR9, 	OPERATOR, 		0x39)
MODSOURCE(OPERATOR10, 	OPERATOR, 		0x3A)
MODSOURCE(OPERATOR11, 	OPERATOR, 		0x3B)
MODSOURCE(OPERATOR12, 	OPERATOR, 		0x3C)
MODSOURCE(OPERATOR13, 	OPERATOR, 		0x3D)
MODSOURCE(OPERATOR14, 	OPERATOR, 		0x3E)
MODSOURCE(OPERATOR15, 	OPERATOR, 		0x3F)

// keychannel is special: its value depends on selected key range of the target
MODSOURCE(NOTE, 		KEYRANGE, 		0x2E)
MODSOURCE(VELOCITY, 	KEYRANGE, 		0x2F)


#ifdef REDEFINEDOUTPUT
#undef OUTPUT
#undef REDEFINEDOUTPUT
#endif

#ifdef REDEFINEDOUTPUTVIRT
#undef OUTPUT_VIRT
#undef REDEFINEDOUTPUTVIRT
#endif

#ifdef REDEFINEDSWITCH
#undef SWITCH
#undef REDEFINEDSWITCH
#endif

#ifdef REDEFINEDCATEGORY
#undef CATEGORY
#undef REDEFINEDCATEGORY
#endif

#ifdef REDEFINEDMODSOURCE
#undef MODSOURCE
#undef REDEFINEDMODSOURCE
#endif
