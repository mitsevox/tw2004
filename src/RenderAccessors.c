// RenderAccessors.c: small accessors of the renderer: a viewport's rectangle, a render context's
// viewport, and the setters of the depth test, depth writes and alpha test in gRenderState, which
// RenderState_Flush hands to GX.

#include "engine.h"
#include "camera.h"

// A viewport's height, as a fraction of the frame buffer's (1: all of it).
f32 VM_fGetViewportHeight(VM_SViewport* pViewport) {
    return pViewport->fHeight;
}

// A viewport's width, as a fraction of the frame buffer's (1: all of it).
f32 VM_fGetViewportWidth(VM_SViewport* pViewport) {
    return pViewport->fWidth;
}

// A viewport's top edge, as a fraction of the frame buffer's height (0: the top).
f32 VM_fGetViewportTop(VM_SViewport* pViewport) {
    return pViewport->fTop;
}

// A viewport's left edge, as a fraction of the frame buffer's width (0: the left).
f32 VM_fGetViewportLeft(VM_SViewport* pViewport) {
    return pViewport->fLeft;
}

// The viewport a render context draws into.
VM_SViewport* RC_spGetRenderCtxViewport(RenderCamera* pRenderCtx) {
    return pRenderCtx->pRect;
}

// Hands GX every group of gRenderState that a setter changed since the last time, then the texture
// or picture set for the next draw.
void RenderState_Flush(void) {
    RenderState_Apply();
}

// Sets the depth test's compare function for the next draws (DS_COMPARE_ALWAYS turns the test off).
void DS_vSetZBufferMode(DS_ECompareFunctions eZBufferMode) {
    gRenderState.eDepthCompare = eZBufferMode;
    gRenderState.uChanged |= DS_CHANGED_ZBUFFER_MODE;
}

// Turns depth-buffer writes on or off for the next draws.
void DS_vEnableZBufferUpdate(int bZBufferUpdate) {
    gRenderState.bDepthWrite = bZBufferUpdate;
    gRenderState.uChanged |= DS_CHANGED_ZBUFFER_UPDATE;
}

// Sets the alpha test of the next draws: off when bAlphaTestEnable is 0; otherwise a pixel is
// drawn when its alpha passes eAlphaMode against the reference. nAlphaRefValue is on a 0..0x80
// alpha scale (0x80 opaque) and is stored as nAlphaRefValue * 2 + 1, kept to 0..255. While the
// test is on, the depth test runs after texturing.
void DS_vSetAlphaTestMode(int bAlphaTestEnable, DS_ECompareFunctions eAlphaMode, int nAlphaRefValue) {
    int nRef;

    nRef = nAlphaRefValue * 2 + 1;
    gRenderState.eAlphaCompare = eAlphaMode;
    gRenderState.nAlphaRef = (nRef < 0) ? 0 : ((nRef <= 0xFF) ? nRef : 0xFF);
    gRenderState.bAlphaTest = bAlphaTestEnable;
    gRenderState.uChanged |= DS_CHANGED_ALPHA_TEST_MODE;
}
