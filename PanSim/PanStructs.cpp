#include "FinalPanEnums.h"
#include "PanHeader.h"


aLedButton Buttons[__LEDBUTTON_COUNT] = {
#define LEDBUTTON(iname,ix,iy,fpid) {#iname,ix, iy,ledbutton_##iname, fpid},
#include "PanControls.h"
#undef LEDBUTTON

};

Led Leds[__LED_COUNT] = {
#define LED(iname,ix,iy) {#iname,ix, iy, led_##iname},
#include "PanControls.h"
#undef LED

};

Knob Knobs[__KNOB_COUNT] = {
#define KNOB(iname,ix,iy,imin,imax,fpidx,lbl) {#iname,ix, iy,knob_##iname,0,fpidx,lbl},
#define SLIDER(iname,ix,iy,imin,imax,fpidx,lbl) {#iname,ix, iy,knob_##iname,1,fpidx,lbl},
#include "PanControls.h"
#undef KNOB
#undef SLIDER
};

Encoder Encoders[__ENCODER_COUNT] = {
#define ENCODER(iname,ix,iy) {(char*)#iname,ix, iy, encoder_##iname},
#include "PanControls.h"
#undef ENCODER
};

#define SCREEN(iname,ix,iy,iw,ih) {ix, iy,iw,ih}
Screen TheScreen =
#include "PanControls.h"
;


#undef SCREEN

int DecodeCurrentEffect(uint32_t switches)
{
	int a = ((switches >> Switch_SELEF1) & 1)?0:1;
	int b = ((switches >> Switch_SELEF2) & 1)?0:1;
	int c = ((switches >> Switch_SELEF3) & 1)?0:1;
	
	return (a << 0)+(b << 1) + (c << 2) ;
}



Raspberry_GuiData_t Raspberry_guidata;



