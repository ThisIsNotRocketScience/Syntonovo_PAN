#include "FinalPanEnums.h"




#undef SCREEN

int DecodeCurrentEffect(uint32_t switches)
{
	int a = ((switches >> Switch_SELEF1) & 1)?0:1;
	int b = ((switches >> Switch_SELEF2) & 1)?0:1;
	int c = ((switches >> Switch_SELEF3) & 1)?0:1;
	
	return (a << 0)+(b << 1) + (c << 2) ;
}






