

#ifndef LEDENCODER
	#define LEDENCODER(name, x,y, desc)
	#define LEDENCODERDEFINED
#endif

#ifndef LEDBUTTON
	#define LEDBUTTON(name, x,y,id,desc,r,g,b)
	#define LEDBUTTONDEFINED
#endif

#ifndef LED
	#define LED(name, x,y,desc)
	#define LEDDEFINED
#endif

/*LED(test,0,0,"led")*/
LEDENCODER(VCO1, 42, 25, "VCO 1")
LEDENCODER(VCO2, 77, 25, "VCO 2")
LEDENCODER(VCO3, 112, 25, "VCO 3")
LEDENCODER(VCO4, 147, 25, "VCO 4")

LEDENCODER(VCO5, 60, 65, "VCO 5")
LEDENCODER(VCO6, 95, 65, "VCO 6")
LEDENCODER(VCO7, 130, 65, "VCO 7")
LEDENCODER(VCO8, 165, 65, "VCO 8")

LEDENCODER(VCF2a, 432, 25, "VCF2a")
LEDENCODER(VCF2b, 467, 25, "VCF2b")
LEDENCODER(VCF2c, 502, 25, "VCF2c")
LEDENCODER(VCF2d, 537, 25, "VCF2d")

LEDENCODER(VCF1Freq, 414, 65, "VCF1Freq")
LEDENCODER(VCF1Mix, 449, 65, "VCF1Mix")
LEDENCODER(VCF2Mix, 484, 65, "VCF2Mix")
LEDENCODER(Cleanmix, 520, 65, "Cleanmix")

LEDENCODER(Masterout, 553, 94, "Masterout")
LEDENCODER(MasteroutHeadphone, 26, 94, "MasteroutHeadphone")

LEDENCODER(SketchLeft, 150, 142, "SketchLeft")
LEDENCODER(SketchRight, 400, 142, "SketchRight")

LEDENCODER( F1, 221, 131, "F1")
LEDENCODER( F2, 234, 158, "F2")
LEDENCODER( F3, 248, 131, "F3")
LEDENCODER( F4, 262, 158, "F4")
LEDENCODER( F5, 276, 131, "F5")
LEDENCODER( F6, 289, 158, "F6")
LEDENCODER( F7, 303, 131, "F7")
LEDENCODER( F8, 317, 158, "F8")
LEDENCODER( F9, 330, 131, "F9")
LEDENCODER(F10, 344, 158, "F10")
LEDENCODER(F11, 358, 131, "F11")

LEDBUTTON(L1, 197-10,  31 + 14 *(0), 0, "L1", 1, 0.9f, 0)
LEDBUTTON(L3, 197-10,  31 + 14 *(1), 2, "L3", 1, 0.9f, 0)
LEDBUTTON(L5, 197-10,  31 + 14 *(2), 4, "L5", 1, 0.9f, 0)
LEDBUTTON(L7, 197-10,  31 + 14 *(3), 6, "L7", 1, 0.9f, 0)
LEDBUTTON(L9, 197-10,  31 + 14 *(4), 8, "L9", 1, 0.9f, 0)

LEDBUTTON(L2, 197, 31 + 7 + 14 * (0), 1, "L2", 1, 0.9f, 0)
LEDBUTTON(L4, 197, 31 + 7 + 14 * (1), 3, "L4", 1, 0.9f, 0)
LEDBUTTON(L6, 197, 31 + 7 + 14 * (2), 5, "L6", 1, 0.9f, 0)
LEDBUTTON(L8, 197, 31 + 7 + 14 * (3), 7, "L8", 1, 0.9f, 0)

LEDBUTTON(L10,197-10, 31 + 14 *(6), 9, "L10", 1, 0.9f, 0)



LEDBUTTON(R1, 382 + 10, 31 + 14 * (0), 10, "R1", 1, 0.9f, 0)
LEDBUTTON(R3, 382 + 10, 31 + 14 * (1), 12, "R3", 1, 0.9f, 0)
LEDBUTTON(R5, 382 + 10, 31 + 14 * (2), 14, "R5", 1, 0.9f, 0)
LEDBUTTON(R7, 382 + 10, 31 + 14 * (3), 16, "R7", 1, 0.9f, 0)
LEDBUTTON(R9, 382 + 10, 31 + 14 * (4), 18, "R9", 1, 0.9f, 0)
			  
LEDBUTTON(R2, 382, 31 + 7 + 14 * (0), 11, "R2", 1, 0.9f, 0)
LEDBUTTON(R4, 382, 31 + 7 + 14 * (1), 13, "R4", 1, 0.9f, 0)
LEDBUTTON(R6, 382, 31 + 7 + 14 * (2), 15, "R6", 1, 0.9f, 0)
LEDBUTTON(R8, 382, 31 + 7 + 14 * (3), 17, "R8", 1, 0.9f, 0)
			  
LEDBUTTON(R10,382 + 10, 31 + 14 * (6), 19, "R10", 1, 0.9f, 0)




LEDBUTTON(BX, 235, 186,20, "X", 1, 0.9f, 0)
LEDBUTTON(BY, 250, 186,21, "Y", 1, 0.9f, 0)
LEDBUTTON(BZ, 266, 186,22, "Z", 1, 0.9f, 0)

LEDBUTTON(BCV, 313, 186,23, "CV", 1, 0.9f, 0)
LEDBUTTON(BEnv, 344, 186,24, "Env", 1, 0.9f, 0)
LEDBUTTON(BLFO, 329, 186,25, "LFO", 1, 0.9f, 0)

LEDBUTTON(BVelocity, 282, 186,26, "Velocity", 1, 0.9f, 0)
LEDBUTTON(BTouch, 298, 186,27, "Toucher", 1, 0.9f, 0)

LEDBUTTON(B1, 79, 181, 28,"1",1,0,0)
LEDBUTTON(B2, 94, 181, 29, "2", 1, 0, 0)
LEDBUTTON(B3, 110, 181,30, "3", 1, 0, 0)
LEDBUTTON(B4, 125, 181,31, "4", 1, 0, 0)
LEDBUTTON(B5, 141, 181,32, "5", 1, 0, 0)
LEDBUTTON(B6, 157, 181,33, "6", 1, 0, 0)
LEDBUTTON(B7, 172, 181,34, "7", 1, 0, 0)
LEDBUTTON(B8, 188, 181,35, "8", 1, 0, 0)

LEDBUTTON( B9, 391, 181,36, "9", 1, 0, 0)
LEDBUTTON(B10, 406,181,37, "10", 1, 0, 0)
LEDBUTTON(B11, 422,181,38, "11", 1, 0, 0)
LEDBUTTON(B12, 438,181,39, "12", 1, 0, 0)
LEDBUTTON(B13, 453,181,40, "13", 1, 0, 0)
LEDBUTTON(B14, 469,181,41, "14", 1, 0, 0)
LEDBUTTON(B15, 484,181,42, "15", 1, 0, 0)
LEDBUTTON(B16, 500,181,43, "16", 1, 0, 0)

LEDBUTTON(BankLeft, 150, 170,44, "Left Hand Bank", 1, 0.9f, 0)
LEDBUTTON(BankRight, 400, 170,45, "Right Hand Bank", 1, 0.9f, 0)

LEDBUTTON(OctUpLeft, 40, 161,46, "OctUp L", 1, 0.9f, 0)
LEDBUTTON(OctDownLeft, 40, 181,47, "OctDown L", 1, 0.9f, 0)
LEDBUTTON(PortamentoLeft, 50, 170,48, "Portamento L", 1, 0.9f, 0)

LEDBUTTON(OctUpRight, 520, 161,49, "OctUp R", 1, 0.9f, 0)
LEDBUTTON(OctDownRight, 520, 181,50, "OctDown R", 1, 0.9f, 0)
LEDBUTTON(PortamentoRight, 500, 170,51, "Portamento R", 1, 0.9f, 0)

LEDBUTTON(ArpEnable, 120, 0,52, "ArpEnable", 1, 0.9f, 0)
LEDBUTTON(ArpEdit, 140, 0, 53,"ArpEdit", 1, 0.9f, 0)
LEDBUTTON(ArpFreeze, 160, 0,54, "ArpFreeze", 1, 0.9f, 0)

LEDBUTTON(SeqPlay, 180, 0, 55,"SeqPlay", 1, 0.9f, 0)
LEDBUTTON(SeqEdit, 200, 0, 56, "SeqEdit", 1, 0.9f, 0)
LEDBUTTON(SeqStop, 220, 0, 57, "SeqStop", 1, 0.9f, 0)

LEDBUTTON(LMod, 40, 140, 58, "PadLeft", 1, 0.9f, 0)
LEDBUTTON(RMod, 520, 140, 59, "PadRight", 1, 0.9f, 0)
LEDBUTTON(BHome, 60, 140, 60, "Home", 1, 0.9f, 0)
LEDBUTTON(LUna, 40, 140, 61, "L Una", 1, 0.9f, 0)
LEDBUTTON(LSus, 40, 140, 62, "L Sus", 1, 0.9f, 0)

LEDBUTTON(RUna, 520, 140, 63, "R Una", 1, 0.9f, 0)
LEDBUTTON(RSus, 520, 140, 64, "R Sus", 1, 0.9f, 0)


LEDBUTTON(Zone1, 10, 120, 65, "Zone 1", 1, 0.9f, 0) // SW65??
LEDBUTTON(Zone2, 40, 120, 66, "Zone 2", 1, 0.9f, 0) // 
LEDBUTTON(Zone3, 70, 120, 67, "Zone 3", 1, 0.9f, 0) // 
LEDBUTTON(Zone4, 100, 120, 68, "Zone 4", 1, 0.9f, 0) // 

//LEDBUTTON(SeqStop, 220, 0, 51, "SeqStop", 1, 0.9f, 0)


#ifdef LEDENCODERDEFINED
	#undef LEDENCODERDEFINED
	#undef LEDENCODER
#endif

#ifdef LEDBUTTONDEFINED
	#undef LEDBUTTONDEFINED
	#undef LEDBUTTON
#endif

#ifdef LEDDEFINED
	#undef LEDDEFINED
	#undef LED
#endif
