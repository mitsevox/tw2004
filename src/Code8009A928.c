// Code8009A928.c (our name): the frame hooks gomainloop.c calls for the glows: set-up and
// shut-down of the glow queue (GoShaderObject_Glows_Gc.c), Code8009AA28.c's lens state and
// Code8009B340.c's fading nodes, their per-frame update and their drawing.
// Its .sdata2 is the lone 1/60 at 0x80283E90 (8 bytes, between SunFlr_Gc.c's pool and
// Code8009AA28.c's, which starts at 0x80283E98), so fn_8009A990 is in an object of its own. Its
// start (fn_8009A928, or as early as 0x8009A844) and its end (fn_8009AA18, or Code8009AA28.c's
// first functions) are not proven.

#include "glows.h"
#include "camera.h"

void fn_800988D8(void);                 // GoShaderObject_Glows_Gc.c
void fn_80098910(void);                 // GoShaderObject_Glows_Gc.c
void fn_80098938(void);                 // GoShaderObject_Glows_Gc.c: draw the queued glows
void fn_8009AF30(s32 nViews);           // Code8009AA28.c
void fn_8009B0D0(void);                 // Code8009AA28.c
void fn_8009B18C(s32 nView);            // Code8009AA28.c
void fn_8009B528(f32 fTime);            // Code8009B340.c
void fn_8009B57C(void);                 // Code8009B340.c
void fn_8009B604(void);                 // Code8009B340.c
void fn_8009B610(void);                 // Code8009B340.c
void fn_8009AA00(int n);
void fn_8009AA18(void);

// Set up for nViews views: the glow queue (queue 0), the fading nodes and the lens state.
void fn_8009A928(s32 nViews) {
    fn_800988D8();
    fn_8009AA00(0);
    fn_8009B604();
    fn_8009AF30(nViews);
}

void fn_8009A968(void) {
    fn_8009B0D0();
    fn_8009B610();
    fn_80098910();
}

// A frame for view nView: empty the glow queue, refill it from the lens state and advance the
// fading nodes by one 60th of a second.
void fn_8009A990(s32 nView) {
    fn_8009AA18();
    fn_8009B18C(nView);
    fn_8009B528(1.0f / 60.0f);
}

// Draws the fading nodes and the queued glows, then gives the render camera the identity view
// matrix. nView: gomainloop passes the view; unused here.
void fn_8009A9CC(int nView) {
    fn_8009B57C();
    fn_8001614C();
    fn_80098938();
    fn_80013D9C(fn_8001614C(), NULL);
}

// Makes queue n the one glows are queued on.
void fn_8009AA00(int n) {
    lbl_80281F80 = &lbl_801D9A68[n];
}

void fn_8009AA18(void) {
    lbl_80281F80->nCount = 0;
}
