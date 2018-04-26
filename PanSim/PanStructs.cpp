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


