
#ifndef DEFINESWITCH
	#define DEFINESWITCH(id,dummy,name)
	#define DEFINESWITCHDEFINED
#endif

#ifndef DEFINEENCODER
	#define DEFINEENCODER(id,pin,name)
	#define DEFINEENCODERDEFINED
#endif

DEFINESWITCH(0,-,BTouch)
DEFINESWITCH(1,-,BCV)
DEFINESWITCH(2,-,BLFO)
DEFINESWITCH(3,-,BEnv)
DEFINESWITCH(4,-,B10)
DEFINESWITCH(5,-,B9)
DEFINEENCODER(8,a,SketchRight)
DEFINEENCODER(9,c,SketchRight)
DEFINEENCODER(10,sw,SketchRight)
DEFINEENCODER(12,a,Masterout)
DEFINEENCODER(13,c,Masterout)
DEFINEENCODER(14,sw,Masterout)
DEFINEENCODER(16,sw,F9)
DEFINEENCODER(17,a,F10)
DEFINEENCODER(18,c,F10)
DEFINEENCODER(19,sw,F10)
DEFINEENCODER(20,a,F11)
DEFINEENCODER(21,c,F11)
DEFINEENCODER(22,sw,F11)
DEFINEENCODER(24,a,F7)
DEFINEENCODER(25,c,F7)
DEFINEENCODER(26,sw,F7)
DEFINEENCODER(27,a,F8)
DEFINEENCODER(28,c,F8)
DEFINEENCODER(29,sw,F8)
DEFINEENCODER(30,a,F9)
DEFINEENCODER(31,c,F9)
DEFINEENCODER(32,c,F4)
DEFINEENCODER(33,sw,F4)
DEFINEENCODER(34,a,F5)
DEFINEENCODER(35,c,F5)
DEFINEENCODER(36,sw,F5)
DEFINEENCODER(37,a,F6)
DEFINEENCODER(38,c,F6)
DEFINEENCODER(39,sw,F6)
DEFINEENCODER(40,sw,F1)
DEFINEENCODER(41,a,F2)
DEFINEENCODER(42,c,F2)
DEFINEENCODER(43,sw,F2)
DEFINEENCODER(44,a,F3)
DEFINEENCODER(45,c,F3)
DEFINEENCODER(46,sw,F3)
DEFINEENCODER(47,a,F4)
DEFINESWITCH(48,-,B7)
DEFINESWITCH(49,-,B8)
DEFINESWITCH(50,-,BX)
DEFINESWITCH(51,-,BY)
DEFINESWITCH(52,-,BVelocity)
DEFINESWITCH(56,-,OctUpLeft)
DEFINESWITCH(58,-,B1)
DEFINESWITCH(59,-,B2)
DEFINESWITCH(60,-,B3)
DEFINESWITCH(61,-,B4)
DEFINESWITCH(62,-,B5)
DEFINESWITCH(63,-,B6)
DEFINESWITCH(70,-,PortamentoLeft)
DEFINESWITCH(71,-,OctDownLeft)
DEFINEENCODER(72,a,SketchLeft)
DEFINEENCODER(73,c,MasteroutHeadphone)
DEFINEENCODER(74,sw,MasteroutHeadphone)
DEFINEENCODER(75,a,MasteroutHeadphone)
DEFINEENCODER(76,c,SketchLeft)
DEFINEENCODER(77,sw,SketchLeft)
DEFINEENCODER(78,a,F1)
DEFINEENCODER(79,c,F1)
DEFINESWITCH(80,-,OctUpRight)
DEFINESWITCH(81,-,OctDownRight)
DEFINESWITCH(82,-,B16)
DEFINESWITCH(83,-,B15)
DEFINESWITCH(84,-,B14)
DEFINESWITCH(85,-,B13)
DEFINESWITCH(86,-,B12)
DEFINESWITCH(87,-,B11)
DEFINESWITCH(88,-,ArpEdit)
DEFINESWITCH(89,-,ArpFreeze)
DEFINESWITCH(90,-,ArpEnable)
DEFINESWITCH(94,-,PortamentoRight)
DEFINESWITCH(95,-,BankRight)
DEFINEENCODER(96,c,VCF2d)
DEFINEENCODER(97,a,VCF2d)
DEFINEENCODER(98,sw,VCF2c)
DEFINEENCODER(99,sw,VCF2d)
DEFINEENCODER(100,sw,VCF1Mix)
DEFINEENCODER(101,a,Cleanmix)
DEFINEENCODER(102,c,Cleanmix)
DEFINEENCODER(103,sw,Cleanmix)
DEFINEENCODER(104,a,VCF1Freq)
DEFINEENCODER(105,c,VCF1Freq)
DEFINEENCODER(106,sw,VCF1Mix)
DEFINEENCODER(107,c,VCF1Mix)
DEFINEENCODER(108,a,VCF2Mix)
DEFINEENCODER(109,c,VCF2Mix)
DEFINEENCODER(110,sw,VCF2Mix)
DEFINEENCODER(111,sw,VCF1Freq)
DEFINEENCODER(112,a,VCF2a)
DEFINEENCODER(113,c,VCF2a)
DEFINEENCODER(114,sw,VCF2a)
DEFINEENCODER(115,a,VCF2b)
DEFINEENCODER(116,c,VCF2b)
DEFINEENCODER(117,sw,VCF2b)
DEFINEENCODER(118,a,VCF2c)
DEFINEENCODER(119,c,VCF2c)
DEFINESWITCH(121,-,R1)
DEFINESWITCH(122,-,R2)
DEFINESWITCH(123,-,R3)
DEFINESWITCH(124,-,R4)
DEFINESWITCH(125,-,R5)
DEFINESWITCH(126,-,R6)
DEFINESWITCH(127,-,R7)
DEFINESWITCH(129,-,L1)
DEFINESWITCH(130,-,L2)
DEFINESWITCH(131,-,L3)
DEFINESWITCH(132,-,L4)
DEFINESWITCH(133,-,L5)
DEFINESWITCH(134,-,L6)
DEFINESWITCH(135,-,L7)
DEFINEENCODER(136,c,VCO5)
DEFINEENCODER(137,sw,VCO5)
DEFINEENCODER(138,a,VCO6)
DEFINEENCODER(139,c,VCO6)
DEFINEENCODER(140,sw,VCO5)
DEFINEENCODER(141,a,VCO7)
DEFINEENCODER(142,c,VCO7)
DEFINEENCODER(143,sw,VCO7)
DEFINEENCODER(144,sw,VCO8)
DEFINEENCODER(145,a,VCO8)
DEFINEENCODER(146,c,VCO8)
DEFINEENCODER(147,sw,VCO3)
DEFINEENCODER(148,a,VCO4)
DEFINEENCODER(149,c,VCO4)
DEFINEENCODER(150,sw,VCO4)
DEFINEENCODER(151,a,VCO5)
DEFINEENCODER(152,a,VCO1)
DEFINEENCODER(153,c,VCO1)
DEFINEENCODER(154,sw,VCO1)
DEFINEENCODER(155,a,VCO2)
DEFINEENCODER(156,c,VCO2)
DEFINEENCODER(157,sw,VCO2)
DEFINEENCODER(158,a,VCO3)
DEFINEENCODER(159,c,VCO3)

#ifdef DEFINESWITCHDEFINED
#undef DEFINESWITCH
#undef DEFINESWITCHDEFINED
#endif

#ifdef DEFINEENCODERDEFINED
#undef DEFINEENCODER
#undef DEFINEENCODERDEFINED
#endif
