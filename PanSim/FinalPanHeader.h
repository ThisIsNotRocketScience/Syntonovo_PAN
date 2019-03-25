

#ifndef LEDENCODER
	#define LEDENCODER(name, x,y, desc)
	#define LEDENCODERDEFINED
#endif

#ifndef LEDBUTTON
	#define LEDBUTTON(name, x,y,id,desc)
	#define LEDBUTTONDEFINED
#endif

#ifndef LED
	#define LED(name, x,y,desc)
	#define LEDDEFINED
#endif

LED(test,0,0,"led")

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

LEDENCODER(SketchLeft, 0, 142, "SketchLeft")
LEDENCODER(SketchRight, 0, 142, "SketchRight")


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

LEDBUTTON(L1, 197,  31,0, "L1")
LEDBUTTON(L2, 197,  43,1, "L2")
LEDBUTTON(L3, 197,  56,2, "L3")
LEDBUTTON(L4, 197,  68,3, "L4")
LEDBUTTON(L5, 197,  81,4, "L5")
LEDBUTTON(L6, 197,  94,5, "L6")
LEDBUTTON(L7, 197, 106,6, "L7")

LEDBUTTON(R1, 382,  31,7, "R1")
LEDBUTTON(R2, 382,  43,8, "R2")
LEDBUTTON(R3, 382,  56,9, "R3")
LEDBUTTON(R4, 382,  68,10, "R4")
LEDBUTTON(R5, 382,  81,11, "R5")
LEDBUTTON(R6, 382,  94,12, "R6")
LEDBUTTON(R7, 382, 106,13, "R7")

LEDBUTTON(BX, 235, 186,14, "X")
LEDBUTTON(BY, 250, 186,15, "Y")
LEDBUTTON(BZ, 266, 186,16, "Z")

LEDBUTTON(BCV, 313, 186,17, "CV")
LEDBUTTON(BEnv, 344, 186,18, "Env")
LEDBUTTON(BLFO, 329, 186,19, "LFO")

LEDBUTTON(BVelocity, 282, 186,20, "Velocity")
LEDBUTTON(BTouch, 298, 186,21, "Toucher")


LEDBUTTON(B1, 79, 181, 22,"1")
LEDBUTTON(B2, 94, 181, 23, "2")
LEDBUTTON(B3, 110, 181,24, "3")
LEDBUTTON(B4, 125, 181,25, "4")
LEDBUTTON(B5, 141, 181,26, "5")
LEDBUTTON(B6, 157, 181,27, "6")
LEDBUTTON(B7, 172, 181,28, "7")
LEDBUTTON(B8, 188, 181,29, "8")

LEDBUTTON( B9, 391, 181,30, "9")
LEDBUTTON(B10, 406,181,31, "10")
LEDBUTTON(B11, 422,181,32, "11")
LEDBUTTON(B12, 438,181,33, "12")
LEDBUTTON(B13, 453,181,34, "13")
LEDBUTTON(B14, 469,181,35, "14")
LEDBUTTON(B15, 484,181,36, "15")
LEDBUTTON(B16, 500,181,37, "16")

LEDBUTTON(BankLeft, 0, 0,38, "Bank")
LEDBUTTON(BankRight, 0, 0,39, "Bank")

LEDBUTTON(OctUpLeft, 0, 0,40, "OctUp")
LEDBUTTON(OctDownLeft, 0, 0,41, "OctDown")
LEDBUTTON(PortamentoLeft, 0, 0,42, "Portamento")

LEDBUTTON(OctUpRight, 0, 0,43, "OctUp")
LEDBUTTON(OctDownRight, 0, 0,44, "OctDown")
LEDBUTTON(PortamentoRight, 0, 0,45, "Portamento")

LEDBUTTON(ArpEnable, 0, 0,46, "ArpEnable")
LEDBUTTON(ArpEdit, 0, 0, 47,"ArpEdit")
LEDBUTTON(ArpFreeze, 0, 0,48, "ArpFreeze")

LEDBUTTON(SeqPlay, 0, 0, 49,"SeqPlay")
LEDBUTTON(SeqEdit, 0, 0, 50, "SeqEdit")
LEDBUTTON(SeqStop, 0, 0, 51, "SeqStop")

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