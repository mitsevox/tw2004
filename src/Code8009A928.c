// Code8009A928.c (our name): the frame hooks gomainloop.c calls for the glows: set-up and
// shut-down of the glow queue (GoShaderObject_Glows_Gc.c), Code8009AA28.c's lens state and
// Code8009B340.c's fading nodes, their per-frame update and their drawing.
// Its .sdata2 is the lone 1/60 at 0x80283E90 (8 bytes, between SunFlr_Gc.c's pool and
// Code8009AA28.c's, which starts at 0x80283E98), so GLW_vUpdateGlows is in an object of its own.
// Its start (GLW_vInitModule, or as early as 0x8009A844) and its end (ColGlow_ResetCurrentList,
// or Code8009AA28.c's first functions) are not proven.

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
void ColGlow_SetCurrentList(int n);
void ColGlow_ResetCurrentList(void);

// Set up for nViews views: the glow queue (queue 0), the fading nodes and the lens state.
void GLW_vInitModule(s32 nViews) {
    fn_800988D8();
    ColGlow_SetCurrentList(0);
    fn_8009B604();
    fn_8009AF30(nViews);
}

void GLW_vCloseModule(void) {
    fn_8009B0D0();
    fn_8009B610();
    fn_80098910();
}

// A frame for view nView: empty the glow queue, update the view's lens state (fn_8009B18C) and
// advance the fading nodes by one 60th of a second.
void GLW_vUpdateGlows(s32 nView) {
    ColGlow_ResetCurrentList();
    fn_8009B18C(nView);
    fn_8009B528(1.0f / 60.0f);
}

// Draws the fading nodes and the queued glows, then resets the render camera's matrix to the
// identity (fn_80013D9C with NULL). nView: gomainloop passes the view; unused here.
void GLW_vRenderGlows(int nView) {
    fn_8009B57C();
    fn_8001614C();
    fn_80098938();
    fn_80013D9C(fn_8001614C(), NULL);
}

// Makes queue n the one glows are queued on.
void ColGlow_SetCurrentList(int n) {
    lbl_80281F80 = &lbl_801D9A68[n];
}

void ColGlow_ResetCurrentList(void) {
    lbl_80281F80->nCount = 0;
}
