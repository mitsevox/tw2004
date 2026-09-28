// Code80012ED0.c (our name; EA's file is not known): small render helpers. First out-of-line
// copies of header getters TW07 has as inlines: a viewport's left, top, width and height
// (GoViewport.h VM_fGetViewport*) and a render context's viewport (GoRenderCtx.h). Then the call
// that hands GX the changed render state, and three of the display-state setters (TW07's
// Legacy/LL LLDisSt: DS_v...) that each write one group of the render state gRenderState and set
// that group's bit in u110 for RenderState_Apply. Last an empty start-up step. Its extent is the
// space between UFont.c and Controller_Gc.c; whether it is one original file is not proven.

#include "engine.h"
#include "camera.h"

void fn_80012FA0(void);

// A viewport's height, as a fraction of the frame buffer's (1: all of it).
f32 VM_fGetViewportHeight(f32* pRect) {
    return pRect[3];
}

// A viewport's width, as a fraction of the frame buffer's (1: all of it).
f32 VM_fGetViewportWidth(f32* pRect) {
    return pRect[2];
}

// A viewport's top edge, as a fraction of the frame buffer's height (0: the top).
f32 VM_fGetViewportTop(f32* pRect) {
    return pRect[1];
}

// A viewport's left edge, as a fraction of the frame buffer's width (0: the left).
f32 VM_fGetViewportLeft(f32* pRect) {
    return pRect[0];
}

// The viewport (screen rectangle) a render context draws into.
f32* RC_spGetRenderCtxViewport(void* pCamera) {
    return ((RenderCamera*)pCamera)->pRect;
}

// Hands GX the render state that changed (RenderState_Apply).
void RenderState_Flush(void) {
    RenderState_Apply();
}

// Sets the depth compare function (GX_ALWAYS turns the depth test off), applied with the next
// RenderState_Apply.
void DS_vSetZBufferMode(int nCompare) {
    gRenderState.n0 = nCompare;
    gRenderState.u110 |= 0x1;
}

// Turns depth-buffer writes on or off, applied with the next RenderState_Apply.
void DS_vEnableZBufferUpdate(int bEnable) {
    gRenderState.b4 = bEnable;
    gRenderState.u110 |= 0x2;
}

// Sets the alpha test of the next draws: bEnable 0 turns it off; otherwise a pixel is drawn when
// its alpha passes the compare function nCompare (GX_NEVER..GX_ALWAYS) against nRef. nRef is on a
// 0..0x80 scale (callers pass 0x80 for opaque) and is stored as nRef * 2 + 1, kept to 0..255.
// Applied with the next RenderState_Apply, which also moves the depth test after texturing while
// the alpha test is on.
void DS_vSetAlphaTestMode(int bEnable, int nCompare, int nRef) {
    int n;

    n = nRef * 2 + 1;
    gRenderState.n8 = nCompare;
    gRenderState.bC = (n < 0) ? 0 : ((n <= 0xFF) ? n : 0xFF);
    gRenderState.bD = bEnable;
    gRenderState.u110 |= 0x4;
}

// Empty; called once by the boot sequence fn_80005520, as some module's start-up step. Nothing in
// it or its caller says which module, so it keeps its address name.
void fn_80012FA0(void) {
}
