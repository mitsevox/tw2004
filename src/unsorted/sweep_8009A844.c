// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"
#include "unsorted/cull.h"

void fn_8009A844(void* pCamera, u8* pIn, u8* pOut);

// Moves a point from the camera's 0..1 screen rectangle units (pIn: x, y) into frame buffer units
// (pOut), as RenderState_SetViewport does for the rectangle itself.
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

    pRect = RC_spGetRenderCtxViewport(pCamera);
    fLeft = VM_fGetViewportLeft(pRect);
    fWidth = VM_fGetViewportWidth(pRect);
    pDst[0] = pSrc[0] * fWidth + fLeft;
    fTop = VM_fGetViewportTop(pRect);
    fHeight = VM_fGetViewportHeight(pRect);
    pDst[1] = pSrc[1] * fHeight + fTop;
    pBuf = RC_spGetRenderCtxFrameBuffer(pCamera);
    f0 = FB_fGetFrameBufferOffsetX(pBuf);
    fBufWidth = FB_fGetFrameBufferWidth(pBuf);
    pDst[0] = pDst[0] * fBufWidth + f0;
    f4 = FB_fGetFrameBufferOffsetY(pBuf);
    fBufHeight = FB_fGetFrameBufferHeight(pBuf);
    pDst[1] = pDst[1] * fBufHeight + f4;
}
