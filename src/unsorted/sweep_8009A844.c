// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"
#include "unsorted/cull.h"

void fn_8009A844(void* pCamera, u8* pIn, u8* pOut);

// Moves a point from the camera's 0..1 screen rectangle units (pIn: x, y) into frame buffer units
// (pOut), as fn_80013EEC does for the rectangle itself.
void fn_8009A844(void* pCamera, u8* pIn, u8* pOut) {
    f32* pRect;
    f32* pSrc = (f32*)pIn;
    f32* pDst = (f32*)pOut;
    f32 fLeft;
    f32 fWidth;
    f32 fTop;
    f32 fHeight;
    GoFrameBuf* pBuf;
    f32 f0;
    f32 fBufWidth;
    f32 f4;
    f32 fBufHeight;

    pRect = fn_80012EF0(pCamera);
    fLeft = fn_80012EE8(pRect);
    fWidth = fn_80012ED8(pRect);
    pDst[0] = pSrc[0] * fWidth + fLeft;
    fTop = fn_80012EE0(pRect);
    fHeight = fn_80012ED0(pRect);
    pDst[1] = pSrc[1] * fHeight + fTop;
    pBuf = fn_80013E40(pCamera);
    f0 = fn_80014174(pBuf);
    fBufWidth = fn_8001416C(pBuf);
    pDst[0] = pDst[0] * fBufWidth + f0;
    f4 = fn_80014164(pBuf);
    fBufHeight = fn_8001415C(pBuf);
    pDst[1] = pDst[1] * fBufHeight + f4;
}
