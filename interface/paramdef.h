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

#ifndef MODSOURCE
#define REDEFINEDMODSOURCE
#define MODSOURCE(name,type,id)
#endif


//OUTPUT(XPAD_OUT, 	    P12_0,	6,	    INT,	0x07,	LIN,	0)
//OUTPUT(YPAD_OUT, 	    P12_0,	7,	    INT,	0x08,  	LIN,	0)
//OUTPUT(ZPAD_OUT, 	    P12_0,	8,	    INT,	0x09,  	LIN,	0)
//OUTPUT(BP_OUT, 	        P12_0,	9,	    INT,	0x0A,  	LIN,	0)
//OUTPUT(MP_OUT, 	        P12_0,	10,	    INT,	0x0B,  	LIN,	0)
//OUTPUT(SP_OUT, 	        P12_0,	11,	    INT,	0x0C,	LIN,	0)

OUTPUT(VCO1_MIX1,		0,	0,	CTRL,	0x10,	CUSTOM,	0)
OUTPUT(WHITENS_MIX1,	1,	0,	CTRL,	0x18,	CUSTOM,	0)
OUTPUT(VCO1_MIX2,		0,	1,	CTRL,	0x11,	CUSTOM,	0)
OUTPUT(VCO4567_MIX2,	1,	1,	CTRL,	0x19,	CUSTOM,	0)
OUTPUT(VCO1_SUB2,		0,	2,	CTRL,	0x12,	CUSTOM,	0)
OUTPUT(VCO4567_MIX1,	1,	2,	CTRL,	0x1A,	CUSTOM,	0)
OUTPUT(VCO1_SUB1,		0,	3,	CTRL,	0x13,	CUSTOM,	0)
OUTPUT(WHITENS_MIX2,	1,	3,	CTRL,	0x1B,	CUSTOM,	0)
OUTPUT(VCO1_FREQ,		0,	4,	CTRL,	0x14,	CUSTOM,	0)
OUTPUT(VCO3_MIX2,		1,	4,	CTRL,	0x1C,	CUSTOM,	0)
//OUTPUT(VCO123_FM3,		0,	5,	CTRL,	0x15,	LIN,	0)
OUTPUT(VCO3_MIX1,		1,	5,	CTRL,	0x1D,	CUSTOM,	0)
OUTPUT(VCO1_PW,			0,	6,	CTRL,	0x16,	LIN,	0x7fff)
OUTPUT(VCO2_MIX1,		1,	6,	CTRL,	0x1E,	CUSTOM,	0)
//OUTPUT(VCO123_FM2,		0,	7,	CTRL,	0x17,	LIN,	0)
OUTPUT(VCO2_MIX2,		1,	7,	CTRL,	0x1F,	CUSTOM,	0)
OUTPUT(VCO2_FREQ,		2,	0,	CTRL,	0x20,	CUSTOM,	0)
OUTPUT(VCO4_FREQ,		3,	0,	CTRL,	0x28,	CUSTOM,	0)
OUTPUT(VCO3_FREQ,		2,	1,	CTRL,	0x21,	CUSTOM,	0)
OUTPUT(VCO5_PW,			3,	1,	CTRL,	0x29,	LIN,	0x3fff)
OUTPUT(VCO3_PW,			2,	2,	CTRL,	0x22,	LIN,	0x3fff)
OUTPUT(VCO5_FREQ,		3,	2,	CTRL,	0x2A,	CUSTOM,	0)
OUTPUT(VCO2_PW,			2,	3,	CTRL,	0x23,	LIN,	0x7fff)
OUTPUT(VCO4_PW,			3,	3,	CTRL,	0x2B,	LIN,	0x3fff)
OUTPUT(RM1_MIX1,		2,	4,	CTRL,	0x24,	CUSTOM,	0)
OUTPUT(VCO6_DRY_MIX,	3,	4,	CTRL,	0x2C,	CUSTOM,	0)
OUTPUT(DIGINS_MIX2,		2,	5,	CTRL,	0x25,	CUSTOM,	0)
OUTPUT(VCO5_DRY_MIX,	3,	5,	CTRL,	0x2D,	CUSTOM,	0)
OUTPUT(RM1_MIX2,		2,	6,	CTRL,	0x26,	LOG,	0)
OUTPUT(VCO7_DRY_MIX,	3,	6,	CTRL,	0x2E,	CUSTOM,	0)
OUTPUT(DIGINS_MIX1,		2,	7,	CTRL,	0x27,	CUSTOM,	0)
OUTPUT(VCO4_DRY_MIX,	3,	7,	CTRL,	0x2F,	CUSTOM,	0)
OUTPUT(VCF2_R_LOG,		4,	0,	INT,	0x30,	CUSTOM,	0)
//OUTPUT(VCF1_FX_12,		5,	0,	CTRL,	0x38,	CUSTOM,	0)
OUTPUT(BN_MIX3,			5,	0,	CTRL,	0x3F,	LOG,	0)
//OUTPUT(VCO7_FREQ,		4,	1,	CTRL,	0x31,	CUSTOM,	0)
OUTPUT(VCA_FINAL,		4,	1,	CTRL,	0x8A,	LOG,	0xffff)
//OUTPUT(RM2_MIX3,		5,	1,	CTRL,	0x39,	LOG,	0)
OUTPUT(VCF1_R_LOG,		5,	1,	INT,	0x3E,	CUSTOM,	0)
//OUTPUT(VCO7_PW,			4,	2,	CTRL,	0x32,	LIN,	0x3fff)
OUTPUT(VCO1SIN_MIX3,	4,	2,	CTRL,	0x70,	LOG,	0)
OUTPUT(VCF1_L_LOG,		5,	2,	INT,	0x3C,	CUSTOM,	0)
OUTPUT(VCF2_FX_R,		4,	3,	CTRL,	0x33,	CUSTOM,	0)
//OUTPUT(VCF1_FX_24,		5,	3,	CTRL,	0x3B,	CUSTOM,	0)
OUTPUT(PUN_MIX,			5,	3,	CTRL,	0x3D,	LOG,	0)
OUTPUT(VCF2_L_LOG,		4,	4,	INT,	0x34,	CUSTOM,	0)
//OUTPUT(VCF1_L_LOG,		5,	4,	INT,	0x3C,	CUSTOM,	0)
OUTPUT(VCO2SIN_MIX3,	5,	4,	CTRL,	0x71,	LOG,	0)
OUTPUT(VCO6_PW,			4,	5,	CTRL,	0x35,	LIN,	0x7fff)
//OUTPUT(PUN_MIX,			5,	5,	CTRL,	0x3D,	LOG,	0)
OUTPUT(VCO7_PW,			5,	5,	CTRL,	0x32,	LIN,	0x3fff)
OUTPUT(VCF2_FX_L,		4,	6,	CTRL,	0x36,	CUSTOM,	0)
//OUTPUT(VCF1_R_LOG,		5,	6,	INT,	0x3E,	CUSTOM,	0)
OUTPUT(VCO7_FREQ,		5,	6,	CTRL,	0x31,	CUSTOM,	0)
OUTPUT(VCO6_FREQ,		4,	7,	CTRL,	0x37,	CUSTOM,	0)
//OUTPUT(BN_MIX3,			5,	7,	CTRL,	0x3F,	LOG,	0)
OUTPUT(VCO3SIN_MIX3,	5,	7,	CTRL,	0x72,	LOG,	0)

//OUTPUT(CLEANF_FX_L,		6,	0,	CTRL,	0x40,	CUSTOM,	0)
OUTPUT(VCF1_R_LIN,		6,	0,	INT,	0x47,	CUSTOM,	0)
//OUTPUT(VCF1_CV,			7,	0,	CTRL,	0x48,	LIN,	0)
//OUTPUT(VCOFM2,			7,	0,	CTRL,	0x75,	LIN,	0)
OUTPUT(VCO123_FM2,		7,	0,	CTRL,	0x17,	INV,	0)
//OUTPUT(NOISE_COLOR,		6,	1,	CTRL,	0x41,	LIN,	0)
OUTPUT(CLEANF_L_LOG,	6,	1,	INT,	0x46,	CUSTOM,	0)
//OUTPUT(VCF2_L_RES,		7,	1,	CTRL,	0x49,	INV,	0)
OUTPUT(VCF1_CROSSMOD,	7,	1,	CTRL,	0x4E,	INV,	0)
//OUTPUT(CLEANF_R_LOG,	6,	2,	INT,	0x42,	CUSTOM,	0)
OUTPUT(VCF1_L_LIN,		6,	2,	INT,	0x44,	CUSTOM,	0)
//OUTPUT(VCF2_L_CV,		7,	2,	CTRL,	0x4A,	INV,	0x7fff)
OUTPUT(VCF2_CROSSMOD,	7,	2,	CTRL,	0x4C,	INV,	0)
//OUTPUT(CLEANF_FX_R,		6,	3,	CTRL,	0x43,	CUSTOM,	0)
OUTPUT(DNSSAW_MIX1,		6,	3,	CTRL,	0x73,	CUSTOM,	0)
//OUTPUT(VCF1_RES,		7,	3,	CTRL,	0x4B,	LIN,	0)
//OUTPUT(VCOFM3,			7,	3,	CTRL,	0x76,	LIN,	0)
OUTPUT(VCO123_FM3,		7,	3,	CTRL,	0x15,	INV,	0)
//OUTPUT(VCF1_L_LIN,		6,	4,	INT,	0x44,	CUSTOM,	0)
OUTPUT(VCF1_FX_24,		6,	4,	CTRL,	0x3B,	CUSTOM,	0)
//OUTPUT(VCF2_CROSSMOD,	7,	4,	CTRL,	0x4C,	LIN,	0)
OUTPUT(CLEANF_R_LOG,	7,	4,	INT,	0x42,	CUSTOM,	0)
//OUTPUT(NOISE_SAW,		6,	5,	CTRL,	0x45,	LIN,	0)
OUTPUT(RM2_MIX3,		6,	5,	CTRL,	0x39,	CUSTOM,	0)
OUTPUT(CLEANF_FX_R,		7,	5,	CTRL,	0x43,	CUSTOM,	0)
//OUTPUT(CLEANF_L_LOG,	6,	6,	INT,	0x46,	CUSTOM,	0)
OUTPUT(DNSSAW_MIX2,		6,	6,	CTRL,	0x74,	CUSTOM,	0)
//OUTPUT(VCF1_CROSSMOD,	7,	6,	CTRL,	0x4E,	LIN,	0)
OUTPUT(NOISE_COLOR,		7,	6,	CTRL,	0x41,	LIN,	0)
//OUTPUT(VCF1_R_LIN,		6,	7,	INT,	0x47,	CUSTOM,	0)
OUTPUT(VCF1_FX_12,		6,	7,	CTRL,	0x38,	CUSTOM,	0)
OUTPUT(CLEANF_FX_L,		7,	7,	CTRL,	0x40,	CUSTOM,	0)

//OUTPUT(VCF2_P_FB,		8,	0,	CTRL,	0x50,	LIN,	0)
OUTPUT(VCF2_L_RES,		8,	0,	CTRL,	0x49,	INV,	0)
//OUTPUT(VCA_FXPOT1,		9,	0,	CTRL,	0x58,	LIN,	0)
OUTPUT(VCF2_H_RES,		9,	0,	CTRL,	0x5D,	INV,	0)
//OUTPUT(VCF2_M1_RES,		8,	1,	CTRL,	0x51,	INV,	0)
OUTPUT(VCF2_M2_RES,		8,	1,	CTRL,	0x56,	INV,	0)
//OUTPUT(VCF2_M1_MIX,		9,	1,	CTRL,	0x59,	CUSTOM,	0)
OUTPUT(VCF2_H_CV,		9,	1,	CTRL,	0x5F,	INV,	0x7fff)
//OUTPUT(VCF2_M1_CV,		8,	2,	CTRL,	0x52,	INV,	0x7fff)
OUTPUT(VCF2_M2_CV,		8,	2,	CTRL,	0x54,	INV,	0x7fff)
//OUTPUT(VCA_FXPOT3,		9,	2,	CTRL,	0x5A,	LIN,	0)
OUTPUT(CLEANF_R_LIN,	9,	2,	INT,	0x5E,	CUSTOM,	0)
//OUTPUT(VCF2_M_FB,		8,	3,	CTRL,	0x53,	LIN,	0)
OUTPUT(VCF2_L_CV,		8,	3,	CTRL,	0x4A,	INV,	0x7fff)
//OUTPUT(VCA_FXPOT2,		9,	3,	CTRL,	0x5B,	LIN,	0)
OUTPUT(CLEANF_L_LIN,	9,	3,	INT,	0x5C,	CUSTOM,	0)
//OUTPUT(VCF2_M2_CV,		8,	4,	CTRL,	0x54,	INV,	0x7fff)
OUTPUT(VCF1_RES,		8,	4,	CTRL,	0x4B,	LIN,	0)
//OUTPUT(CLEANF_L_LIN,	9,	4,	INT,	0x5C,	CUSTOM,	0)
OUTPUT(VCF2_M1_CV,		9,	4,	CTRL,	0x52,	INV,	0x7fff)
//OUTPUT(FX_L_RETURN,		8,	5,	CTRL,	0x55,	CUSTOM,	0)
OUTPUT(FX_L_RETURN,		8,	5,	CTRL,	0x55,	CUSTOM,	0)
//OUTPUT(VCF2_H_RES,		9,	5,	CTRL,	0x5D,	INV,	0)
OUTPUT(VCF2_M1_RES,		9,	5,	CTRL,	0x51,	INV,	0)
//OUTPUT(VCF2_M2_RES,		8,	6,	CTRL,	0x56,	INV,	0)
OUTPUT(VCF1_CV,			8,	6,	CTRL,	0x48,	INV,	0)
//OUTPUT(CLEANF_R_LIN,	9,	6,	INT,	0x5E,	CUSTOM,	0)
OUTPUT(VCF2_M_FB,		9,	6,	CTRL,	0x53,	LIN,	0)
OUTPUT(FX_R_RETURN,		8,	7,	CTRL,	0x57,	CUSTOM,	0)
//OUTPUT(VCF2_H_CV,		9,	7,	CTRL,	0x5F,	INV,	0x7fff)
OUTPUT(VCF2_P_FB,		9,	7,	CTRL,	0x50,	LIN,	0)

OUTPUT(VCF2_L_MIX,		10,	0,	CTRL,	0x67,	CUSTOM,	0)
OUTPUT(CV_EXT_3,		11,	0,	CTRL,	0x68,	LIN,	0)
OUTPUT(VCF2_M2_MIX,		10,	1,	CTRL,	0x64,	CUSTOM,	0)
OUTPUT(CV_EXT_6,		11,	1,	CTRL,	0x69,	LIN,	0)
OUTPUT(VCF2_L_LIN,		10,	2,	INT,	0x66,	CUSTOM,	0)
OUTPUT(CV_EXT_5,		11,	2,	CTRL,	0x6A,	LIN,	0)
OUTPUT(VCF2_H_MIX,		10,	3,	CTRL,	0x65,	CUSTOM,	0)
OUTPUT(CV_EXT_4,		11,	3,	CTRL,	0x6B,	LIN,	0)
//OUTPUT(VCF2_M2_MIX,		10,	4,	CTRL,	0x64,	CUSTOM,	0)
OUTPUT(VCA_FXPOT3,		10,	4,	CTRL,	0x5A,	LIN,	0)
OUTPUT(CV_EXT_1,		11,	4,	CTRL,	0x6C,	LIN,	0)
//OUTPUT(VCF2_H_MIX,		10,	5,	CTRL,	0x65,	CUSTOM,	0)
OUTPUT(VCF2_M1_MIX,		10,	5,	CTRL,	0x59,	CUSTOM,	0)
OUTPUT(CV_EXT_KB,		11,	5,	CTRL,	0x6D,	LIN,	0)
//OUTPUT(VCF2_L_LIN,		10,	6,	INT,	0x66,	CUSTOM,	0)
OUTPUT(VCA_FXPOT2,		10,	6,	CTRL,	0x5B,	LIN,	0)
OUTPUT(CV_EXT_2,		11,	6,	CTRL,	0x6E,	LIN,	0)
//OUTPUT(VCF2_L_MIX,		10,	7,	CTRL,	0x67,	CUSTOM,	0)
OUTPUT(VCA_FXPOT1,		10,	7,	CTRL,	0x58,	LIN,	0)
OUTPUT(VCF2_R_LIN,		11,	7,	INT,	0x6F,	CUSTOM,	0)

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

OUTPUT_VIRT(MASTER_LEVEL, VIRT, 0, CTRL, 0x89, LOG, 0xFFFF)

OUTPUT_VIRT(ZPRIME_SPEED, VIRT, 0, CTRL, 0xA0, LIN, 0x2000)
/*
OUTPUT_VIRT(X_DEADZONE, VIRT, 0, CTRL, 0xA1, LIN, 0x2000)
OUTPUT_VIRT(Y_DEADZONE, VIRT, 0, CTRL, 0xA2, LIN, 0x2000)
OUTPUT_VIRT(Z_DEADZONE, VIRT, 0, CTRL, 0xA3, LIN, 0x2000)
OUTPUT_VIRT(X_SCALE, VIRT, 0, CTRL, 0xA4, LIN, 0x2000)
OUTPUT_VIRT(Y_SCALE, VIRT, 0, CTRL, 0xA5, LIN, 0x2000)
OUTPUT_VIRT(Z_SCALE, VIRT, 0, CTRL, 0xA6, LIN, 0x2000)
OUTPUT_VIRT(MODL_SCALE, VIRT, 0, CTRL, 0xA7, LIN, 0x2000)
OUTPUT_VIRT(SUSL_SCALE, VIRT, 0, CTRL, 0xA8, LIN, 0x2000)
OUTPUT_VIRT(UNACL_SCALE, VIRT, 0, CTRL, 0xA9, LIN, 0x2000)
OUTPUT_VIRT(MODR_SCALE, VIRT, 0, CTRL, 0xAA, LIN, 0x2000)
OUTPUT_VIRT(SUSR_SCALE, VIRT, 0, CTRL, 0xAB, LIN, 0x2000)
OUTPUT_VIRT(UNACR_SCALE, VIRT, 0, CTRL, 0xAC, LIN, 0x2000)
*/
OUTPUT_VIRT(CHASE, VIRT, 0, CTRL, 0xB0, LIN, 0)
OUTPUT_VIRT(STASH, VIRT, 0, CTRL, 0xB1, LIN, 0)

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

OUTPUT_VIRT(DNSSAW_VCFMIX, VIRT, 0, CTRL, 0xF0, LIN, 0x8000)
OUTPUT_VIRT(DNSSAW_LEVEL, VIRT, 0, CTRL, 0xF1, LIN, 0)

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
OUTPUT_VIRT(PORTAMENTO_TIME, VIRT, 0, CTRL, 0xEB, LIN, 0)
OUTPUT_VIRT(MASTER_PITCH2, VIRT, 0, CTRL, 0xEC, LIN, 0x8000)

OUTPUT_VIRT(LEDR, VIRT, 0, CTRL, 0xED, LIN, 0x8000)
OUTPUT_VIRT(LEDG, VIRT, 0, CTRL, 0xEE, LIN, 0x8000)
OUTPUT_VIRT(LEDB, VIRT, 0, CTRL, 0xEF, LIN, 0x8000)
OUTPUT_VIRT(HLEDR, VIRT, 0, CTRL, 0xF0, LIN, 0x8000)
OUTPUT_VIRT(HLEDG, VIRT, 0, CTRL, 0xF1, LIN, 0x8000)
OUTPUT_VIRT(HLEDB, VIRT, 0, CTRL, 0xF2, LIN, 0x8000)
OUTPUT_VIRT(LEDBRIGHT, VIRT, 0, CTRL, 0xF3, LIN, 0x8000)


SWITCH(SEL1TRI, 47)
SWITCH(SEL1SAW, 46)
SWITCH(SEL1SQR, 45)
SWITCH(SEL1SUB, 44)
SWITCH(SEL2TRI, 43)
SWITCH(SEL2SAW, 42)
SWITCH(SEL2SQR, 41)
SWITCH(SEL2SUB, 40)

SWITCH(SEL3TRI, 39)
SWITCH(SEL3SAW, 38)
SWITCH(SEL3SQR, 37)
SWITCH(SEL3SUB, 36)
// sync vco2 with vco1
SWITCH(SELVCOSYNC1, 35)
// sync vco3 with vco2
SWITCH(SELVCOSYNC2, 34)
// sync vco1 with vco3
SWITCH(SELVCOSYNC3, 33)
SWITCH(SELGTRVCF2, 32)

SWITCH(SEL4SQR, 31)
SWITCH(SEL5SQR, 30)
SWITCH(SEL6SQR, 29)
SWITCH(SEL7SQR, 28)
SWITCH(SELRMVCF2, 27)
SWITCH(SELVCF21A, 26)
SWITCH(SELVCF21B, 25)
SWITCH(SELVCF22A, 24)

SWITCH(SELVCF22B, 23)
SWITCH(SELVCF23A, 22)
SWITCH(SELVCF23B, 21)
SWITCH(SELVCF24A, 20)
SWITCH(SELVCF24B, 19)
SWITCH(SELVCF21SER, 18)
SWITCH(SELVCF23SER, 17)
SWITCH(SELVCF23FSM, 16)

SWITCH(SELVCF1MOST, 15)
SWITCH(SELVCF2MOST, 14)
SWITCH(SELVCLEANFMOST, 13)
SWITCH(SELEF0, 12)
SWITCH(SELEF1, 11)
SWITCH(SELEF2, 10)
SWITCH(SELEF3, 9)
SWITCH(SELMUTEDNSAW, 8)

SWITCH(SELVCF45FSM, 7)
SWITCH(SELTUNEVCF, 6)
SWITCH(SELVCF1AP, 5)
SWITCH(SELVCF1BP, 4)
SWITCH(SEQCLICK, 3)
SWITCH(SELFSM1SRC, 2)
SWITCH(SELFSM2SRC, 1)
SWITCH(SELMUTEVCOTUNE, 0)


SWITCH(SELPORTAMENTO, 48)
SWITCH(SELSTACCATO, 49)

SWITCH(SELSTASHOSC4567, 50)
SWITCH(SELSTASHVCF2, 51)

SWITCH(SELCHASEOSC4567, 52)
SWITCH(SELCHASEVCF2, 53)

// global sustain -> keep gate open
SWITCH(SELSUSTAINL, 54)
SWITCH(SELSUSTAINR, 55)

// global una corda -> shorten adsr release
SWITCH(SELUNACL, 56)
SWITCH(SELUNACR, 57)

//struct lfo {
//	uint16_t flags; // including key range
//	uint16_t speed;
//	int16_t depth;
//	int16_t shape;
//	uint16_t reset_phase;
//};
MODSOURCE(LFO0, 		LFO,			0x10)
MODSOURCE(LFO1, 		LFO, 			0x11)
MODSOURCE(LFO2, 		LFO, 			0x12)
MODSOURCE(LFO3, 		LFO, 			0x13)
MODSOURCE(LFO4, 		LFO, 			0x14)
MODSOURCE(LFO5, 		LFO, 			0x15)
MODSOURCE(LFO6, 		LFO, 			0x16)
MODSOURCE(LFO7, 		LFO, 			0x17)
MODSOURCE(LFO8, 		LFO, 			0x18)
MODSOURCE(LFO9, 		LFO, 			0x19)
MODSOURCE(LFO10, 		LFO, 			0x1A)
MODSOURCE(LFO11, 		LFO, 			0x1B)
MODSOURCE(LFO12, 		LFO, 			0x1C)
MODSOURCE(LFO13, 		LFO, 			0x1D)
MODSOURCE(LFO14, 		LFO, 			0x1E)
MODSOURCE(LFO15, 		LFO, 			0x1F)

//struct env {
//	uint16_t flags; // including env type, key range
//	uint16_t a;
//	uint16_t d;
//	uint16_t s;
//	uint16_t r;
//	//uint16_t curve;
//};
MODSOURCE(ENV0, 		ENVELOPE, 		0x30)
MODSOURCE(ENV1, 		ENVELOPE, 		0x31)
MODSOURCE(ENV2, 		ENVELOPE, 		0x32)
MODSOURCE(ENV3, 		ENVELOPE, 		0x33)
MODSOURCE(ENV4, 		ENVELOPE, 		0x34)
MODSOURCE(ENV5, 		ENVELOPE, 		0x35)
MODSOURCE(ENV6, 		ENVELOPE, 		0x36)
MODSOURCE(ENV7, 		ENVELOPE, 		0x37)
MODSOURCE(ENV8, 		ENVELOPE, 		0x38)
MODSOURCE(ENV9, 		ENVELOPE, 		0x39)
MODSOURCE(ENV10, 		ENVELOPE, 		0x3A)
MODSOURCE(ENV11, 		ENVELOPE, 		0x3B)
MODSOURCE(ENV12, 		ENVELOPE, 		0x3C)
MODSOURCE(ENV13, 		ENVELOPE, 		0x3D)
MODSOURCE(ENV14, 		ENVELOPE, 		0x3E)
MODSOURCE(ENV15, 		ENVELOPE, 		0x3F)

//struct controller {
//	int16_t scale;
//	int16_t deadzone;
//	//int16_t shape;
//};
MODSOURCE(X, 			CONTROLLER, 	0x60)
MODSOURCE(Y,			CONTROLLER, 	0x61)
MODSOURCE(Z, 			CONTROLLER, 	0x62)
MODSOURCE(ZPRIME, 		CONTROLLER, 	0x63)
MODSOURCE(PADL, 		CONTROLLER, 	0x64)
MODSOURCE(PADR, 		CONTROLLER, 	0x65)
MODSOURCE(SUSTAINL, 	CONTROLLER, 	0x66)
MODSOURCE(SUSTAINR, 	CONTROLLER, 	0x67)
MODSOURCE(UNACORDAL, 	CONTROLLER, 	0x68)
MODSOURCE(UNACORDAR, 	CONTROLLER, 	0x69)
MODSOURCE(PEDAL, 		CONTROLLER, 	0x6A)

//struct operator {
//	uint8_t modsource1;
//	uint8_t modsource2;
//	uint16_t op; // tbd plus, minus, mul, diff, sh, lag, etc.
//	uint16_t parameter; // parameter for selected op
//};
MODSOURCE(OPERATOR0, 	OPERATOR, 		0x80)
MODSOURCE(OPERATOR1, 	OPERATOR, 		0x81)
MODSOURCE(OPERATOR2, 	OPERATOR, 		0x82)
MODSOURCE(OPERATOR3, 	OPERATOR, 		0x83)
MODSOURCE(OPERATOR4, 	OPERATOR, 		0x84)
MODSOURCE(OPERATOR5, 	OPERATOR, 		0x85)
MODSOURCE(OPERATOR6, 	OPERATOR, 		0x86)
MODSOURCE(OPERATOR7, 	OPERATOR, 		0x87)
MODSOURCE(OPERATOR8, 	OPERATOR, 		0x88)
MODSOURCE(OPERATOR9, 	OPERATOR, 		0x89)
MODSOURCE(OPERATOR10, 	OPERATOR, 		0x8A)
MODSOURCE(OPERATOR11, 	OPERATOR, 		0x8B)
MODSOURCE(OPERATOR12, 	OPERATOR, 		0x8C)
MODSOURCE(OPERATOR13, 	OPERATOR, 		0x8D)
MODSOURCE(OPERATOR14, 	OPERATOR, 		0x8E)
MODSOURCE(OPERATOR15, 	OPERATOR, 		0x8F)

// keychannel is special: its value depends on selected key range of the target
MODSOURCE(NOTE, 		KEYRANGE, 		0xC0)
MODSOURCE(VELOCITY, 	KEYRANGE, 		0xC1)


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

#ifdef REDEFINEDMODSOURCE
#undef MODSOURCE
#undef REDEFINEDMODSOURCE
#endif
