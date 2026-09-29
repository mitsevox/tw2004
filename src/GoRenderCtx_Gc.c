// GoRenderCtx_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the render
// camera (lens, frame buffer, screen rectangle and the matrices made from them), the renderer's
// screen state, and the pads' button masks.

#include "game_types.h"
#include "engine.h"
#include "unsorted/cull.h"

void RC_vOnRenderCtxScreenUpdated(Camera* pCamera);
void RC_vUpdateRenderCtxScreen(Camera* pCamera);
void RC_vSetRenderCtxViewport(Camera* pCamera, f32* pRect);
void RC_vSetRenderCtxFrameBuffer(Camera* pCamera, GoFrameBuf* pBuf);
void RC_vSetRenderCtxCamera(Camera* pCamera, CamLens* pLens);
void RC_vSetDefaultRenderCtx(Camera* pCamera);
void RenderState_SetClipZFromRenderCtx(Camera* pCamera);
void RenderState_SetRenderSurface(int a, int nWidth, int nHeight, int nField, int b, int c);
void Mtx_OrthoScale(f32 (*pMtx)[4], f32 f1, f32 f2);   // matrix builders, not decompiled yet
void Mtx_Perspective(f32 (*pMtx)[4], f32 f1, f32 f2, f32 f3, f32 f4, f32 f5);
void Mtx_PerspectiveDepthOverNear(f32 (*pMtx)[4], f32 f1, f32 f2, f32 f3, f32 f4);
f32 CA_fGetCameraFarZ(u8* p);
f32 CA_fGetCameraNearZ(u8* p);
f32 Math_Tan(f32 x0);

// This file's .sbss (engine.h), in reverse address order as the compiler lays it out.
s8    gnInputControlSet;
void* gapCurrentRenderCtx[2];   // 8 bytes in the DOL (gnInputControlSet follows at +8); only [0] is used

// This file's .sdata (camera.h).
void** gppCurrentRenderCtx = gapCurrentRenderCtx;

// Makes a render context (0x234 bytes, TW07's RC_SRenderCtx) from a lens (TW07's CA_SCamera), a
// frame buffer and a screen rectangle: the identity model matrix, the default clear values, and the
// screen matrices worked out.
void* RC_spCreateRenderCtx(CamLens* pLens, GoFrameBuf* pBuf, f32* pRect) {
    Camera* pCamera;

    pCamera = StaticMem_Alloc(0x234, 2, 16, "GoRenderCtx_Gc.c", 96);
    RC_vSetRenderCtxCamera(pCamera, pLens);
    RC_vSetRenderCtxFrameBuffer(pCamera, pBuf);
    RC_vSetRenderCtxViewport(pCamera, pRect);
    RC_vSetDefaultRenderCtx(pCamera);
    RC_vUpdateRenderCtxScreen(pCamera);
    return pCamera;
}

// fake match: stands in for a function the original linker stripped. The file's pool has 0.0,
// 0.5, 1.0, 16773216, 512, 448 in that order, before the functions below use them (they would
// put 1.0 before 0.5, and 1.442695 first of the rest); its body is unknown, this one only
// reproduces the order.
static f32 GoRenderCtx_Gc_StrippedFn(f32 x) {
    if (x < 0.5f) {
        x = 0.0f;
    }
    if (x < 16773216.0f) {
        x = 1.0f;
    }
    if (x < 448.0f) {
        x = 512.0f;
    }
    return x;
}

// ---- sweep code (not yet cleaned up) ----

s32 LLMath_CopyMat44();
s32 LLMath_Transpose44();
s32 LLMath_mat44fltMultiplyList();
void RC_vSetCurrentRenderCtx(s32 v);
void RC_vUpdateRenderCtxScreenMatricesAndInfo(Camera* pCamera);   // not decompiled yet
void RC_vStoreRenderCtxTransformationMatrix(u8* arg0, f32 (*arg1)[4]);
s32 LLMath_IdentifyMat();
f32 VM_fGetViewportHeightRatio(u8* p);
f32 VM_fGetViewportWidthRatio(u8* p);
f32 VM_fGetViewportOneOverHeightRatio(u8* p);
f32 VM_fGetViewportHeightOverWidth(u8* p);
f32 VM_fGetViewportOneOverWidthRatio(u8* p);
f32 FB_fGetFrameBufferOneOverHeightRatio(u8* p);
f32 VM_fGetFrameBufferHeightOverWidth(u8* p);
f32 FB_fGetFrameBufferOneOverWidthRatio(u8* p);

void RC_vReleaseRenderCtx(void* pCamera) {
    StaticMem_Free(pCamera);
}

// Hands the render context's viewport and scissor rectangle to the renderer
// (RenderState_SetViewport, flushed at once), then its near and far clip distances
// (RenderState_SetClipZFromRenderCtx).
void RC_vApplyRenderCtxToRenderState(Camera* pCamera) {
    RenderState_SetViewport(pCamera);
    RenderState_Flush();
    RenderState_SetClipZFromRenderCtx(pCamera);
}

// Clears the screen: draws a quad over all of it in the colour at the start of the render context
// (its a0, r g b a; 0, 0, 0.5, 0 from RC_vSetDefaultRenderCtx), depth test off. uFlags bit 0: keep
// DS_vEnableZBufferUpdate's setting (else depth writes off for the quad); bit 1: pass 1 instead of
// 2 to the first RenderState_SetRenderSurface (colour and alpha written instead of neither). Depth
// writes, z mode 3 and colour-only writes (8) are set afterwards.
void RC_vClearRenderCtxScreen(f32* pColour, u32 uFlags) {
    f32 aXY[8];

    RenderView_SetUseCurrentMatrices(0);
    RenderState_SetDrawFlags(0);
    if (!(uFlags & 1)) {
        DS_vEnableZBufferUpdate(0);
    }
    if (!(uFlags & 2)) {
        RenderState_SetRenderSurface(0, 512, 448, lbl_80281B88 & 1, 2, 1);
    } else {
        RenderState_SetRenderSurface(0, 512, 448, lbl_80281B88 & 1, 1, 1);
    }
    DS_vSetAlphaTestMode(0, 6, 0x80);
    DS_vSetZBufferMode(7);
    RenderState_Flush();
    RenderView_MakeQuad(aXY, NULL, 0.0f, 0.0f, 1.0f, 1.0f);
    aXY[2] = 0.0f;
    aXY[6] = 0.0f;
    RenderView_SetColor(pColour);
    RenderView_DrawPrimitive(0xA1, aXY, NULL, NULL, 2);
    DS_vSetAlphaTestMode(0, 6, 0x80);
    DS_vSetZBufferMode(3);
    DS_vEnableZBufferUpdate(1);
    RenderState_SetRenderSurface(0, 512, 448, lbl_80281B88 & 1, 8, 1);
    RenderState_Flush();
}

// Works out the render context's screen values from its lens, screen rectangle and frame buffer:
// tan of half the field of view (f224) and its inverse, the focal length in frame buffer pixels
// (f1E0), the rectangle's centre, the near and far clip (unk1F4 scaled by f1E0 / 554.256, unk1F8),
// and the sine and cosine of the half field of view across and down, and of double its tangent
// (unk204..unk220; TW07's RC_fGetRenderCtxHalfFieldOfViewSinX and kin read them). Then the
// projection m5C (perspective, or flat from the lens's fFlatWidth x fFlatHeight when fn_80008378
// says the lens is not perspective), its transpose m9C, and world to screen mDC.
void RC_vUpdateRenderCtxScreenMatricesAndInfo(Camera* pCamera) {
    CamLens* pLens;
    f32* pRect;
    GoFrameBuf* pBuf;
    f32 f;
    f32 mFlat[4][4];
    f32 mProj[4][4];
    f32 aSrc[4];    // fake match: three are used; [4] gives the original's stack layout
    f32 aDst[4];    // fake match: as aSrc

    pLens = pCamera->unk10;
    pRect = pCamera->pRect;
    pBuf = pCamera->pBuf;
    pCamera->f224 = Math_Tan(CA_fGetCameraFieldOfView(pLens) * 0.5f);
    pCamera->f228 = 1.0f / pCamera->f224;
    FB_fGetFrameBufferHeight(pBuf);
    FB_fGetFrameBufferWidth(pBuf);
    pCamera->f1E0 = pCamera->f228 * (VM_fGetViewportWidth(pRect) * FB_fGetFrameBufferWidth(pBuf) * 0.5f);
    pCamera->n22C = 0;
    pCamera->f230 = -(logf(pCamera->f1E0 * (1.0f / 554.256f)) * 1.442695f);
    pCamera->f1E4 = VM_fGetViewportLeft(pRect) + VM_fGetViewportWidth(pRect) * 0.5f;
    pCamera->f1E8 = 1.0f - (VM_fGetViewportTop(pRect) + VM_fGetViewportHeight(pRect) * 0.5f);
    pCamera->unk1F4 = pCamera->f1E0 * (CA_fGetCameraNearZ((u8*)pLens) / 554.256f);
    pCamera->unk1F8 = CA_fGetCameraFarZ((u8*)pLens);
    pCamera->f1FC = pCamera->f224 * VM_fGetViewportOneOverWidthRatio((u8*)pRect) * FB_fGetFrameBufferOneOverWidthRatio((u8*)pBuf);
    pCamera->f200 = VM_fGetFrameBufferHeightOverWidth((u8*)pBuf) * (VM_fGetViewportHeightOverWidth((u8*)pRect) *
                    (pCamera->f224 * VM_fGetViewportOneOverHeightRatio((u8*)pRect) * FB_fGetFrameBufferOneOverHeightRatio((u8*)pBuf)));

    aSrc[0] = 1.0f;
    aSrc[1] = pCamera->f1FC;
    aSrc[2] = 0.0f;
    LLMath_Normalize3(aSrc, aDst);
    pCamera->unk204 = aDst[1];
    pCamera->unk20C = aDst[0];
    aSrc[0] = 1.0f;
    aSrc[1] = pCamera->f200;
    aSrc[2] = 0.0f;
    LLMath_Normalize3(aSrc, aDst);
    pCamera->unk208 = aDst[1];
    pCamera->unk210 = aDst[0];
    aSrc[0] = 1.0f;
    aSrc[1] = pCamera->f1FC * 2.0f;
    aSrc[2] = 0.0f;
    LLMath_Normalize3(aSrc, aDst);
    pCamera->unk214 = aDst[1];
    pCamera->unk218 = aDst[0];
    aSrc[0] = 1.0f;
    aSrc[1] = pCamera->f200 * 2.0f;
    aSrc[2] = 0.0f;
    LLMath_Normalize3(aSrc, aDst);
    pCamera->unk21C = aDst[1];
    pCamera->unk220 = aDst[0];

    if (fn_80008378(pLens) == 0) {
        f = pRect[2] * (1.0f / VM_fGetViewportHeightRatio((u8*)pRect)) / pRect[3];
        Mtx_Perspective(pCamera->m5C, pCamera->f228, 1.0f / VM_fGetViewportWidthRatio((u8*)pRect), f,
                    pCamera->unk1F4, pCamera->unk1F8);
    } else {
        Mtx_OrthoScale(mFlat, pLens->fFlatWidth, pLens->fFlatHeight);
        f = pRect[2] * (1.0f / VM_fGetViewportHeightRatio((u8*)pRect)) / pRect[3];
        Mtx_PerspectiveDepthOverNear(mProj, 1.0f / VM_fGetViewportWidthRatio((u8*)pRect), f, pCamera->unk1F4, pCamera->unk1F8);
        LLMath_mat44fltMultiplyList(mProj, mFlat, pCamera->m5C, 4);
    }
    LLMath_Transpose44(pCamera->m5C, pCamera->m9C);
    LLMath_mat44fltMultiplyList(pCamera->m5C, pLens->m44, pCamera->mDC, 4);
}

// Rebuilds the matrices made from the model matrix at +0x1C: the lens's world-to-camera matrix with
// it (viewMtx) and the world-to-screen matrix mDC with it (+0x19C), then viewMtx transposed (m15C).
// With the identity model matrix (the byte at +0x1DC set) it copies the two instead of multiplying.
void RC_vUpdateRenderCtxTransformationMatrices(void* pCamera) {
    u8* arg0 = pCamera;

    if ((u8) (*(u8*)((u8*)(arg0) + 0x1DC)) != 0) {
        LLMath_CopyMat44((*(s32*)((u8*)(arg0) + 0x10)) + 0x44, arg0 + 0x11C);
        LLMath_CopyMat44(arg0 + 0xDC, arg0 + 0x19C);
    } else {
        LLMath_mat44fltMultiplyList((*(s32*)((u8*)(arg0) + 0x10)) + 0x44, arg0 + 0x1C, arg0 + 0x11C, 4);
        LLMath_mat44fltMultiplyList(arg0 + 0xDC, arg0 + 0x1C, arg0 + 0x19C, 4);
    }
    LLMath_Transpose44(arg0 + 0x11C, arg0 + 0x15C);
}

// Empty in this build: RC_vUpdateRenderCtxScreen calls it right after the screen matrices are
// rebuilt.
void RC_vOnRenderCtxScreenUpdated(Camera* pCamera) {
}

// Makes pCamera the current render camera (the one RC_spGetCurrentRenderCtx returns).
void RC_vSetCurrentRenderCtx(s32 v) {
    *(s32*)(gppCurrentRenderCtx + 0x0) = v;
}

// Rebuilds the render context's screen values and matrices
// (RC_vUpdateRenderCtxScreenMatricesAndInfo) after its lens, rectangle or frame buffer changed.
void RC_vUpdateRenderCtxScreen(Camera* pCamera) {
    RC_vUpdateRenderCtxScreenMatricesAndInfo(pCamera);
    RC_vOnRenderCtxScreenUpdated(pCamera);
}

// Gives the camera the model matrix pMtx (NULL: the identity).
void RC_vSetRenderCtxTransformationMatrix(void* pCamera, f32 (*pMtx)[4]) {
    RC_vStoreRenderCtxTransformationMatrix(pCamera, pMtx);
    RC_vUpdateRenderCtxTransformationMatrices(pCamera);
}

// Stores the model matrix pMtx at +0x1C (NULL: the identity, and the flag at +0x1DC set so
// RC_vUpdateRenderCtxTransformationMatrices copies instead of multiplying). Nothing is rebuilt
// here.
void RC_vStoreRenderCtxTransformationMatrix(u8* arg0, f32 (*arg1)[4]) {
    if (arg1 == NULL) {
        LLMath_IdentifyMat(arg0 + 0x1C);
        (*(s8*)((u8*)(arg0) + 0x1DC)) = 1;
        return;
    }
    LLMath_CopyMat44(arg1, arg0 + 0x1C);
    (*(s8*)((u8*)(arg0) + 0x1DC)) = 0;
}

void RC_vSetRenderCtxViewport(Camera* pCamera, f32* pRect) {
    pCamera->pRect = pRect;
}

void RC_vSetRenderCtxFrameBuffer(Camera* pCamera, GoFrameBuf* pBuf) {
    pCamera->pBuf = pBuf;
}

void RC_vSetRenderCtxCamera(Camera* pCamera, CamLens* pLens) {
    pCamera->unk10 = pLens;
}

GoFrameBuf* RC_spGetRenderCtxFrameBuffer(Camera* pCamera) {
    return pCamera->pBuf;
}

// A new render context's defaults: the identity model matrix, the clear colour a0 (0, 0, 0.5, 0;
// RC_vClearRenderCtxScreen draws with it), f1EC 1.0 and f1F0 16773216.
void RC_vSetDefaultRenderCtx(Camera* pCamera) {
    RC_vSetRenderCtxTransformationMatrix(pCamera, NULL);
    pCamera->a0[0] = 0.0f;
    pCamera->a0[1] = 0.0f;
    pCamera->a0[2] = 0.5f;
    pCamera->a0[3] = 0.0f;
    pCamera->f1EC = 1.0f;
    pCamera->f1F0 = 16773216.0f;
}

// Hands the render context's near and far clip distances (fn_80008360, fn_80008368) to the renderer
// (gRenderState.fNearZ, fFarZ).
void RenderState_SetClipZFromRenderCtx(Camera* pCamera) {
    gRenderState.fNearZ = fn_80008360(pCamera);
    gRenderState.fFarZ = fn_80008368(pCamera);
}

// Hands the renderer the camera's screen rectangle: in frame buffer units (bit 0x800), and in
// 512 x 448 screen pixels, left, right, top, bottom (bit 0x200).
void RenderState_SetViewport(void* pCamera) {
    f32* pRect;
    GoFrameBuf* pBuf;

    pRect = RC_spGetRenderCtxViewport(pCamera);
    pBuf = RC_spGetRenderCtxFrameBuffer(pCamera);
    gRenderState.fViewportLeft = FB_fGetFrameBufferOffsetX(pBuf) + VM_fGetViewportLeft(pRect) * FB_fGetFrameBufferWidth(pBuf);
    gRenderState.fViewportTop = FB_fGetFrameBufferOffsetY(pBuf) + VM_fGetViewportTop(pRect) * FB_fGetFrameBufferHeight(pBuf);
    gRenderState.fViewportWidth = VM_fGetViewportWidth(pRect) * FB_fGetFrameBufferWidth(pBuf);
    gRenderState.fViewportHeight = VM_fGetViewportHeight(pRect) * FB_fGetFrameBufferHeight(pBuf);
    gRenderState.fViewportNear = 0.0f;
    gRenderState.fViewportFar = 1.0f;
    gRenderState.uChanged |= 0x800;
    gRenderState.nScissorLeft = VM_fGetViewportLeft(pRect) * 512.0f;
    gRenderState.nScissorRight = (int)((VM_fGetViewportLeft(pRect) + VM_fGetViewportWidth(pRect)) * 512.0f) - 1;
    gRenderState.nScissorTop = VM_fGetViewportTop(pRect) * 448.0f;
    gRenderState.nScissorBottom = (int)((VM_fGetViewportTop(pRect) + VM_fGetViewportHeight(pRect)) * 448.0f) - 1;
    gRenderState.uChanged |= 0x200;
}

// Selects render surface a (GoRenderSurface.c) with the next RenderState_Apply: its width, height
// and field, and in b the channels drawing writes (1 both, 2 neither, 4 alpha only, 8 colour only).
void RenderState_SetRenderSurface(int a, int nWidth, int nHeight, int nField, int b, int c) {
    gRenderState.nSurface = a;
    gRenderState.nSurfaceWidth = nWidth;
    gRenderState.nSurfaceHeight = nHeight;
    gRenderState.nSurfaceField = nField;
    gRenderState.nF4 = b;
    gRenderState.nF8 = c;
    gRenderState.uChanged |= 0x1000;
}

// Sets the draw flags (bit 0x10 textured, 0x40 blended); RenderState_Apply sets up the TEV stages
// and blending from them.
void RenderState_SetDrawFlags(int a) {
    gRenderState.uDrawFlags = a;
    gRenderState.uChanged |= 0x20;
}

f32 VM_fGetViewportHeightRatio(u8* p) {
    return *(f32*)(p + 0x14);
}

f32 VM_fGetViewportWidthRatio(u8* p) {
    return *(f32*)(p + 0x10);
}

f32 VM_fGetViewportOneOverHeightRatio(u8* p) {
    return *(f32*)(p + 0x2C);
}

f32 VM_fGetViewportHeightOverWidth(u8* p) {
    return *(f32*)(p + 0x30);
}

f32 VM_fGetViewportOneOverWidthRatio(u8* p) {
    return *(f32*)(p + 0x28);
}

f32 FB_fGetFrameBufferHeight(GoFrameBuf* pBuf) {
    return pBuf->fHeight;
}

f32 FB_fGetFrameBufferOffsetY(GoFrameBuf* pBuf) {
    return pBuf->f4;
}

f32 FB_fGetFrameBufferWidth(GoFrameBuf* pBuf) {
    return pBuf->fWidth;
}

f32 FB_fGetFrameBufferOffsetX(GoFrameBuf* pBuf) {
    return pBuf->f0;
}

f32 FB_fGetFrameBufferOneOverHeightRatio(u8* p) {
    return *(f32*)(p + 0x2C);
}

f32 VM_fGetFrameBufferHeightOverWidth(u8* p) {
    return *(f32*)(p + 0x30);
}

f32 FB_fGetFrameBufferOneOverWidthRatio(u8* p) {
    return *(f32*)(p + 0x28);
}

// ---- end of sweep code ----

// Set the colour of the view's vertices (r, g, b, a); NULL: the default grey.
void RenderView_SetColor(f32* pColour) {
    if (pColour == NULL) {
        RenderView_SetDefaultColor();
        return;
    }
    LLMath_CopyVec(pColour, lbl_80280E08->aColour);
}

// The default vertex colour: half grey, opaque.
void RenderView_SetDefaultColor(void) {
    lbl_80280E08->aColour[0] = 0.5f;
    lbl_80280E08->aColour[1] = 0.5f;
    lbl_80280E08->aColour[2] = 0.5f;
    lbl_80280E08->aColour[3] = 1.0f;
}

// Fill a screen quad's two corners (x0, y0)-(x1, y1) and its texture coordinates (0,0)-(1,1),
// four floats per vertex.
void RenderView_MakeQuad(f32* pXY, f32* pUV, f32 x0, f32 y0, f32 x1, f32 y1) {
    if (pXY != NULL) {
        pXY[0] = x0;
        pXY[1] = y0;
        pXY[2] = 0.0f;
        pXY[3] = 1.0f;
        pXY[4] = x1;
        pXY[5] = y1;
        pXY[6] = 0.0f;
        pXY[7] = 1.0f;
    }
    if (pUV != NULL) {
        pUV[0] = 0.0f;
        pUV[1] = 0.0f;
        pUV[2] = 0.0f;
        pUV[3] = 1.0f;
        pUV[4] = 1.0f;
        pUV[5] = 1.0f;
        pUV[6] = 0.0f;
        pUV[7] = 1.0f;
    }
}

// ---- sweep code (not yet cleaned up) ----

double tan();
void Input_vSelectControlSet(s8 v);
void Input_vStopVibration(int nController);
void Input_vStopAllVibration(void);

// 1: RenderView_DrawPrimitive draws with the viewport and matrices already set (the camera's); 0:
// with the view's own screen projection and viewport, put back after the draw.
void RenderView_SetUseCurrentMatrices(int a) {
    lbl_80280E08->nD0 = a;
}

f32 CA_fGetCameraFarZ(u8* p) {
    return *(f32*)(p + 0xAC);
}

f32 CA_fGetCameraNearZ(u8* p) {
    return *(f32*)(p + 0xA8);
}

f32 CA_fGetCameraFieldOfView(CamLens* pLens) {
    return pLens->fFov;
}

f32 Math_Tan(f32 x0) {
    f32 t0;
    t0 = tan(x0);
    return t0;
}

// fake match: EA's table starts 8-aligned after the 17-byte "GoRenderCtx_Gc.c" (as TibExt.c's
// lbl_80194758 after "TibExt.c"); plain u32 data is only 4-aligned. The functions from
// Input_vSelectControlSet on came from sweeps and may be another file, which would explain it.
u32 gauInputButtonMap[][0xE8 / 4] __attribute__((aligned(8))) = {
    {
        0xFFFF, 0x800, 0x800, 0x100, 0x400, 0x100, 0x800, 0x100, 0x400, 0x20,
        0x40, 0x1, 0x2, 0x8, 0x4, 0x1, 0x2, 0x8, 0x4, 0x40,
        0x1, 0x2, 0x100, 0x800, 0x200, 0x400, 0x1, 0x2, 0x8, 0x4,
        0x200, 0x10, 0x10, 0, 0x800, 0x100, 0x40, 0x400, 0x100, 0x1,
        0x2, 0x8, 0x4, 0, 0x40, 0x400, 0x40, 0x100, 0x20, 0x800,
        0x400, 0x40, 0x20, 0x200, 0, 0, 0, 0
    },
};

// Selects the control set: the row of gauInputButtonMap that Input_uiMap reads (front-end message
// 120, GM_vSetButtonConfig; the table has only row 0 in this build).
void Input_vSelectControlSet(s8 v) {
    gnInputControlSet = v;
}

// A button's pad mask in the selected control set (gauInputButtonMap[gnInputControlSet][nButton]);
// bShift (TW07's parameter name: held) moves it up 16 bits.
u32 Input_uiMap(int nButton, u8 bShift) {
    if (bShift) {
        return gauInputButtonMap[gnInputControlSet][nButton] << 16;
    }
    return gauInputButtonMap[gnInputControlSet][nButton];
}

// Whether any of the four pads has any of the buttons in uMask (0: any button at all).
u8 Input_AnyPadPressed(u32 uMask) {
    u8 bPressed = 0;
    int nController = 0;

    do {
        if ((uMask == 0 && Input_ReadControlPad(nController) != 0) || (uMask & Input_ReadControlPad(nController))) {
            bPressed = 1;
        }
        nController++;
    } while (nController < 4);
    return bPressed;
}

// Stops the rumble on all four pads (Input_vStopVibration).
void Input_vStopAllVibration(void) {
    s32 var_r31;

    var_r31 = 0;
    do {
        Input_vStopVibration(var_r31);
        var_r31 += 1;
    } while (var_r31 < 4);
}

// ---- end of sweep code ----
