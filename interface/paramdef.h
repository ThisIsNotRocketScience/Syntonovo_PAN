#ifndef OUTPUT 
#define REDEFINEDOUTPUT
#define OUTPUT(name,codecport,codecpin, type,id, style,defaultvalue)
#endif

#ifndef OUTPUT_VIRT
#define REDEFINEDOUTPUTVIRT
#define OUTPUT_VIRT(name,codecport,codecpin, type,id, style,defaultvalue)
#endif

#ifndef SWITCH
#define REDEFINEDSWITCH
#define SWITCH(name,id)
#endif


//OUTPUT(XPAD_OUT, 	    P12_0,	6,	    INT,	0x07,	LIN,	0)
//OUTPUT(YPAD_OUT, 	    P12_0,	7,	    INT,	0x08,  	LIN,	0)
//OUTPUT(ZPAD_OUT, 	    P12_0,	8,	    INT,	0x09,  	LIN,	0)
//OUTPUT(BP_OUT, 	        P12_0,	9,	    INT,	0x0A,  	LIN,	0)
//OUTPUT(MP_OUT, 	        P12_0,	10,	    INT,	0x0B,  	LIN,	0)
//OUTPUT(SP_OUT, 	        P12_0,	11,	    INT,	0x0C,	LIN,	0)

OUTPUT(VCF1_RES, 	    P12_2,	0,	    CTRL,	0x11,	LIN,	0)
OUTPUT(VCF1_CROSSMOD,	P12_2,	1,	    CTRL,	0x12, 	LIN,	0)
OUTPUT(VCF2_RES,        P12_2,  2,      CTRL,	0x13, 	INV,	0)
OUTPUT(VCF2_CROSSMOD,   P12_2,  3,      CTRL,	0x14, 	LIN,	0)
OUTPUT(NOISE_COLOR,     P12_2,  4,      CTRL,	0x15, 	LIN,	0)
OUTPUT(VCA_FXPOT1,      P12_2,  7,      CTRL,	0x16, 	LIN,	0)
OUTPUT(VCA_FXPOT2,      P12_2,  8,      CTRL,	0x17, 	LIN,	0)
OUTPUT(VCA_FXPOT3,      P12_2,  9,		CTRL,	0x18, 	LIN,	0)

OUTPUT(VCF1_CV, 	    P12_3,	0,	    CTRL,	0x21, 	LIN,	0)
OUTPUT(VCF2_L_CV,       P12_3,	1,	    CTRL,	0x22, 	INV,	0x7fff)
OUTPUT(VCF2_M1_CV,      P12_3,  2,      CTRL,	0x23, 	INV,	0x7fff)
OUTPUT(VCF2_M2_CV,      P12_3,  3,      CTRL,	0x24, 	INV,	0x7fff)
OUTPUT(VCF2_H_CV,       P12_3,  4,      CTRL,	0x25, 	INV,	0x7fff)
OUTPUT(VCF2_L_MIX,      P12_3,  5,      CTRL,	0x26, 	LOG,	0)
OUTPUT(VCF2_M1_MIX,     P12_3,  6,      CTRL,	0x27, 	LOG,	0)
OUTPUT(VCF2_M2_MIX,     P12_3,  7,      CTRL,	0x28, 	LOG,	0)
OUTPUT(VCF2_H_MIX,      P12_3,  8,      CTRL,	0x29, 	LOG,	0)
OUTPUT(VCF2_FB,         P12_3,  9,     	CTRL,	0x2A, 	LIN,	0)

OUTPUT(VCF1_L_LIN, 	    P12_1,	0,	    INT,	0x31, 	CUSTOM,	0)
OUTPUT(VCF2_L_LIN,	    P12_1,	1,	    INT,	0x32, 	CUSTOM,	0)
OUTPUT(CLEANF_L_LIN,    P12_1,  2,      INT,	0x33, 	CUSTOM,	0)
OUTPUT(VCF1_L_LOG,      P12_1,  3,      INT,	0x34, 	CUSTOM,	0)
OUTPUT(VCF2_L_LOG,      P12_1,  4,      INT,	0x35, 	CUSTOM,	0)
OUTPUT(CLEANF_L_LOG,    P12_1,  5,      INT,	0x36, 	CUSTOM,	0)
OUTPUT(VCF1_R_LIN,      P12_1,  6,      INT,	0x37, 	CUSTOM,	0)
OUTPUT(VCF2_R_LIN,      P12_1,  7,      INT,	0x38, 	CUSTOM,	0)
OUTPUT(CLEANF_R_LIN,    P12_1,  8,      INT,	0x39, 	CUSTOM,	0)
OUTPUT(VCF1_R_LOG,      P12_1,  9,     	INT,	0x3A, 	CUSTOM,	0)
OUTPUT(VCF2_R_LOG,      P12_1,  10,     INT,	0x3B, 	CUSTOM,	0)
OUTPUT(CLEANF_R_LOG,    P12_1,  11,     INT,	0x3C, 	CUSTOM,	0)

OUTPUT(VCO1_PW,         P20_1,  0,      CTRL,	0x41, 	LIN,	0x7fff)
OUTPUT(VCO2_PW,         P20_1,  1,      CTRL,	0x42, 	LIN,	0x7fff)
OUTPUT(VCO3_PW,         P20_1,  2,      CTRL,	0x43, 	LIN,	0x7fff)
OUTPUT(VCO123_FM1,      P20_1,  3,      CTRL,	0x44, 	LIN,	0)
OUTPUT(VCO123_FM2,      P20_1,  4,      CTRL,	0x45, 	LIN,	0)
OUTPUT(VCO1_MIX2,       P20_1,  5,      CTRL,	0x46, 	CUSTOM,	0)
OUTPUT(VCO2_MIX2,       P20_1,  6,      CTRL,	0x47, 	CUSTOM,	0)
OUTPUT(VCO3_MIX2,       P20_1,  7,      CTRL,	0x48, 	CUSTOM,	0)
OUTPUT(RM1_MIX2,        P20_1,  8,      CTRL,	0x49, 	CUSTOM,	0)
OUTPUT(VCO1_MIX1,		P20_1,  9,     	CTRL,	0x4A, 	CUSTOM,	0)
OUTPUT(VCO2_MIX1,       P20_1,  10,     CTRL,	0x4B, 	CUSTOM,	0)
OUTPUT(VCO3_MIX1,       P20_1,  11,     CTRL,	0x4C, 	CUSTOM,	0)
OUTPUT(RM1_MIX1,        P20_1,  12,     CTRL,	0x4D, 	CUSTOM,	0)

OUTPUT(PUN_MIX,      	P20_0,  0,      CTRL,	0x61, 	LOG,	0)
OUTPUT(VCO4_DRY_MIX,    P20_0,  1,      CTRL,	0x62, 	CUSTOM,	0)
OUTPUT(VCO6_DRY_MIX,    P20_0,  2,      CTRL,	0x63, 	CUSTOM,	0)
OUTPUT(VCO4567_MIX2,    P20_0,  3,      CTRL,	0x64, 	CUSTOM,	0)
OUTPUT(WHITENS_MIX1,    P20_0,  4,      CTRL,	0x65, 	CUSTOM,	0)
OUTPUT(DIGINS_MIX1,     P20_0,  5,      CTRL,	0x66, 	CUSTOM,	0)
OUTPUT(EXT_MIX1,        P20_0,  6,      CTRL,	0x67, 	CUSTOM,	0)
OUTPUT(RM2_MIX3,        P20_0,  7,      CTRL,	0x68, 	LOG,	0)
OUTPUT(BN_MIX3,         P20_0,  8,      CTRL,	0x69, 	LOG,	0)
OUTPUT(VCO5_DRY_MIX,	P20_0,  9,     	CTRL,	0x6A, 	CUSTOM,	0)
OUTPUT(VCO7_DRY_MIX,    P20_0,  10,     CTRL,	0x6B, 	CUSTOM,	0)
OUTPUT(VCO4567_MIX1,    P20_0,  11,     CTRL,	0x6C, 	CUSTOM,	0)
OUTPUT(WHITENS_MIX2,    P20_0,  12,     CTRL,	0x6D, 	CUSTOM,	0)
OUTPUT(DIGINS_MIX2,     P20_0,  13,     CTRL,	0x6E, 	CUSTOM,	0)
OUTPUT(EXT_MIX2,        P20_0,  14,     CTRL,	0x6F, 	CUSTOM,	0)


/*OUTPUT(VCO1_FREQ,		P4_1,	0,     	CTRL,	0x70, 	LIN,	0x8000)
OUTPUT(VCO2_FREQ,		P4_1,	1,     	CTRL,	0x71, 	LIN,	0x8000)
OUTPUT(VCO3_FREQ,		P4_1,	2,     	CTRL,	0x72, 	LIN,	0x8000)
OUTPUT(VCO4_FREQ,		P4_1,	3,     	CTRL,	0x73, 	LIN,	0x8000)
OUTPUT(VCO5_FREQ,		P4_0,	0,     	CTRL,	0x74, 	LIN,	0x8000)
OUTPUT(VCO6_FREQ,		P4_0,	1,     	CTRL,	0x75, 	LIN,	0x8000)
OUTPUT(VCO7_FREQ,		P4_0,	2,     	CTRL,	0x76, 	LIN,	0x8000)*/

OUTPUT_VIRT(VCF1_LIN, VIRT, 0, CTRL, 0x80, LIN, 0)
OUTPUT_VIRT(VCF2_LIN, VIRT, 0, CTRL, 0x81, LIN, 0)
OUTPUT_VIRT(CLEANF_LIN, VIRT, 0, CTRL, 0x82, LIN, 0)
OUTPUT_VIRT(VCF1_LEVEL, VIRT, 0, CTRL, 0x83, LOG, 0)
OUTPUT_VIRT(VCF2_LEVEL, VIRT, 0, CTRL, 0x84, LOG, 0)
OUTPUT_VIRT(CLEANF_LEVEL, VIRT, 0, CTRL, 0x85, LOG, 0)
OUTPUT_VIRT(VCF1_PAN, VIRT, 0, CTRL, 0x86, LIN, 0x8000)
OUTPUT_VIRT(VCF2_PAN, VIRT, 0, CTRL, 0x87, LIN, 0x8000)
OUTPUT_VIRT(CLEANF_PAN, VIRT, 0, CTRL, 0x88, LIN, 0x8000)

OUTPUT_VIRT(VCO4567_DRY_MIX, VIRT, 0, CTRL, 0xCF, LIN, 0)

OUTPUT_VIRT(VCO1_VCFMIX, VIRT, 0, CTRL, 0xD0, LIN, 0x8000)
OUTPUT_VIRT(VCO2_VCFMIX, VIRT, 0, CTRL, 0xD1, LIN, 0x8000)
OUTPUT_VIRT(VCO3_VCFMIX, VIRT, 0, CTRL, 0xD2, LIN, 0x8000)
OUTPUT_VIRT(RM1_VCFMIX, VIRT, 0, CTRL, 0xD3, LIN, 0x8000)
OUTPUT_VIRT(WHITENS_VCFMIX, VIRT, 0, CTRL, 0xD4, LIN, 0x8000)
OUTPUT_VIRT(DIGINS_VCFMIX, VIRT, 0, CTRL, 0xD5, LIN, 0x8000)
OUTPUT_VIRT(EXT_VCFMIX, VIRT, 0, CTRL, 0xD6, LIN, 0x8000)
OUTPUT_VIRT(VCO4567_VCFMIX, VIRT, 0, CTRL, 0xD7, LIN, 0x8000)
OUTPUT_VIRT(VCO1_LEVEL, VIRT, 0, CTRL, 0xD8, LIN, 0)
OUTPUT_VIRT(VCO2_LEVEL, VIRT, 0, CTRL, 0xD9, LIN, 0)
OUTPUT_VIRT(VCO3_LEVEL, VIRT, 0, CTRL, 0xDA, LIN, 0)
OUTPUT_VIRT(RM1_LEVEL, VIRT, 0, CTRL, 0xDB, LIN, 0)
OUTPUT_VIRT(WHITENS_LEVEL, VIRT, 0, CTRL, 0xDC, LIN, 0)
OUTPUT_VIRT(DIGINS_LEVEL, VIRT, 0, CTRL, 0xDD, LIN, 0)
OUTPUT_VIRT(EXT_LEVEL, VIRT, 0, CTRL, 0xDE, LIN, 0)
OUTPUT_VIRT(VCO4567_LEVEL, VIRT, 0, CTRL, 0xDF, LIN, 0)

OUTPUT_VIRT(NOTE, VIRT, 0, CTRL, 0xE0, LIN, 0)

OUTPUT_VIRT(VCO1_PITCH, VIRT, 0, CTRL, 0xE1, LIN, 0x8000)
OUTPUT_VIRT(VCO2_PITCH, VIRT, 0, CTRL, 0xE2, LIN, 0x8000)
OUTPUT_VIRT(VCO3_PITCH, VIRT, 0, CTRL, 0xE3, LIN, 0x8000)
OUTPUT_VIRT(VCO4_PITCH, VIRT, 0, CTRL, 0xE4, LIN, 0x8000)
OUTPUT_VIRT(VCO5_PITCH, VIRT, 0, CTRL, 0xE5, LIN, 0x8000)
OUTPUT_VIRT(VCO6_PITCH, VIRT, 0, CTRL, 0xE6, LIN, 0x8000)
OUTPUT_VIRT(VCO7_PITCH, VIRT, 0, CTRL, 0xE7, LIN, 0x8000)

OUTPUT_VIRT(GATE, VIRT, 0, CTRL, 0xE8, LIN, 0)
OUTPUT_VIRT(RETRIGGER, VIRT, 0, CTRL, 0xE9, LIN, 0)

OUTPUT_VIRT(MASTER_PITCH, VIRT, 0, CTRL, 0xEA, LIN, 0x8000)

SWITCH(SEL1TRI, 24)
SWITCH(SEL1SAW, 25)
SWITCH(SEL1SQR, 26)
SWITCH(SEL1SUB, 27)
SWITCH(SEL2TRI, 28)
SWITCH(SEL2SAW, 29)
SWITCH(SEL2SQR, 30)
SWITCH(SEL2SUB, 31)

SWITCH(SEL3TRI, 16)
SWITCH(SEL3SAW, 17)
SWITCH(SEL3SQR, 18)
SWITCH(SEL3SUB, 19)
SWITCH(SELVCOSYNC1, 20)
SWITCH(SELVCOSYNC2, 21)
SWITCH(SEL4SAW, 22)
SWITCH(SEL5SAW, 23)

SWITCH(SEL6SAW, 8)
SWITCH(SEL7SAW, 9)
SWITCH(SELVCF2POST, 10)
SWITCH(SELVCF2L0, 11)
SWITCH(SELVCF2L1, 12)
//#define SELVCF2HP (13)
SWITCH(SELEF1, 14)
SWITCH(SELEF2, 15)

SWITCH(SELEF3, 0)
SWITCH(SELEF4, 1)
SWITCH(SELMOST2, 2)
SWITCH(SELMOST3, 3)

SWITCH(SELEFFECT1, 5)
SWITCH(SELEFFECT2, 6)
SWITCH(SELEFFECT3, 7)

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
