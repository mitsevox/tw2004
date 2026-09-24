// GoFrameBuf.c (EA's name, from its asserts; also in EA's 2002 source tree): a frame buffer's
// size and scale (GoFrameBuf, camera.h): made, set, freed, and the derived sizes worked out. The
// main loop, the grass and the shadows use them.

#include "camera.h"

void FB_vInitModule(void);
void FB_vCloseModule(void);

void FB_vUpdateInternalFrameBufferData(GoFrameBuf* pBuf) {
    pBuf->f18 = pBuf->fWidth * pBuf->f10;
    pBuf->f1C = pBuf->fHeight * pBuf->f14;
    pBuf->f28 = 1.0f / pBuf->f10;
    pBuf->f2C = 1.0f / pBuf->f14;
    pBuf->f20 = pBuf->fWidth * pBuf->f28;
    pBuf->f24 = pBuf->fHeight * pBuf->f2C;
    pBuf->fAspect = pBuf->fHeight / pBuf->fWidth;
}

void FB_vInitModule(void) {
}

void FB_vCloseModule(void) {
}

GoFrameBuf* FB_spCreateFrameBuffer(void) {
    GoFrameBuf* pBuf = fn_80009B34(sizeof(GoFrameBuf), 2, 16, "GoFrameBuf.c", 137);

    FB_vSetDefaultFrameBuffer(pBuf);
    return pBuf;
}

void FB_vReleaseFrameBuffer(GoFrameBuf* pBuf) {
    fn_80009E70(pBuf);
}

void FB_vSetDefaultFrameBuffer(GoFrameBuf* pBuf) {
    FB_vSetFrameBuffer(pBuf, 0.0f, 0.0f, 512.0f, 448.0f, 1.0f, 1.0f);
}

void FB_vSetFrameBuffer(GoFrameBuf* pBuf, f32 f0, f32 f4, f32 fWidth, f32 fHeight, f32 f10, f32 f14) {
    pBuf->f0 = f0;
    pBuf->f4 = f4;
    pBuf->fWidth = fWidth;
    pBuf->fHeight = fHeight;
    pBuf->f10 = f10;
    pBuf->f14 = f14;
    FB_vUpdateInternalFrameBufferData(pBuf);
}
