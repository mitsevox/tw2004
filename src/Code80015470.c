// Code80015470.c (our name): the GameCube display state, most likely EA's Legacy/LL LLDisSt_Gc.c
// (TW2003 Xbox's source tree has Xbox/LLDisSt_Xbox.c, and TW07's LLDisSt.c DS_vInitModule and
// DS_vCloseModule sit at the same places in the start-up and shutdown; not proven). It defines the
// render state gRenderState that the DS_ and RenderState_ setters fill, starts it up
// (DS_vInitModule), hands its changed groups to GX (RenderState_Apply), keeps the pool of 20
// display-list blocks UObject3D.c records into, and has small TEV helpers and the current render
// context's getter. Its .sdata is padded to 8 at 0x80280E04..0x80280E08 and its .sdata2 at
// 0x80282B9C..0x80282BA0.

#include "ustream.h"
#include "camera.h"
#include "game.h"
#include "golfer.h"
#include "character.h"
#include "frontend/fe.h"
#include "gx.h"
#include "llpict.h"
#include "unsorted/cull.h"

// Defined here, last address first (CodeWarrior lays out .bss in reverse).
RenderState gRenderState;
BufferPool  gBufferPool;

BufferPool* gpBufferPool = &gBufferPool;

// ---- sweep code (not yet cleaned up) ----

void LLMath_IdentifyMat(f32 (*m)[4]);          // identity matrix
void DS_vCloseModule(void);
void RenderState_SetTexCoordGen(s32 nCoord, s32 nFunc, s32 nSrc, s32 nMtx);
void RenderState_SetKColorAlpha(u8 nAlpha);

// ---- end of sweep code ----

// Marks all 20 blocks of the display-list pool (gpBufferPool) free and restarts its search and its
// count.
void BufferPool_FreeAll(void) {
    BufferPoolBlock* pBlock;
    s32 i;

    gpBufferPool->nNext = 0;
    gpBufferPool->n4 = 0;
    pBlock = gpBufferPool->aBlocks;
    for (i = 0; i < 20; i++) {
        pBlock->u1000 = 0;
        pBlock++;
    }
}

// The first free block of the display-list pool, searching from nNext (which moves past the blocks
// in use); counts the hand-out in n4. The block is not marked used here: UObject3D.c records a
// display list into it and stores its size in u1000. There is no end check: with all 20 blocks in
// use it walks past the pool.
BufferPoolBlock* BufferPool_GetFreeBlock(void) {
    BufferPoolBlock* pBlock;

    pBlock = &gpBufferPool->aBlocks[gpBufferPool->nNext];
    for (;;) {
        if (pBlock->u1000 == 0) {
            break;
        }
        pBlock++;
        gpBufferPool->nNext++;
    }
    gpBufferPool->n4++;
    return pBlock;
}

// Display-state start-up (gomainloop's fn_8006C7A8): sets gRenderState to its defaults (depth test
// GX_LEQUAL with writes on, alpha test off, blending source alpha over inverse source alpha, draw
// flags 0x70, fog range 100..2048 in white, identity matrices, no texture), marks no group changed,
// selects GX position matrix 0 and frees the display-list pool.
void DS_vInitModule(void) {
    RenderState* const p = &gRenderState;

    // fake match: mProjection, uChanged and uFlags through the global, the rest through p (only
    // this mix gives the original's base registers)
    p->nDepthCompare = 3;
    p->bDepthWrite = 1;
    p->nAlphaCompare = 6;
    p->nAlphaRef = 100;
    p->bAlphaTest = 0;
    p->nBlendSrc = 4;
    p->nBlendDst = 5;
    p->nBlendMode = 1;
    p->nConstantAlpha = 0xFF;
    p->bConstantAlpha = 0;
    p->uDrawFlags = 0x70;
    p->nClipMode = 0;
    p->nFogType = 2;
    p->fFogStart = 100.0f;
    p->fFogEnd = 2048.0f;
    *(u32*)&p->c30 = 0xFFFFFFFF; // port: all four GXColor bytes 0xFF, stored as one word
    LLMath_IdentifyMat(p->mView);
    LLMath_IdentifyMat(gRenderState.mProjection);
    p->pTexBank = NULL;
    p->pTexEntry = NULL;
    gRenderState.uChanged = 0;
    gRenderState.uFlags = 0;
    GXSetCurrentMtx(0);
    BufferPool_FreeAll();
}

// Display-state shutdown (gomainloop's fn_8006C854): nothing to do on this machine.
void DS_vCloseModule(void) {
}

// Hands GX every group of gRenderState whose bit is set in uChanged (depth, blending, constant alpha,
// alpha test, draw flags, clip mode, fog, matrices, scissor, viewport, render surface), then the
// texture or movie picture of the next draw (uFlags), and clears both. While the screen copy of
// GxUtil.c is on (fn_8002A3A4), TEV stage 0 blends the copied screen (fn_8002A3AC) and the draw's
// own stages start at 1. UObject3D.c also records a call into a display list.
void RenderState_Apply(void) {
    f32 mNormal[3][4];
    Camera* pCamera;
    int nStage;
    // the YUV to RGB conversion's constants
    static const GXColorS10 cYuv = {-90, 0, -114, 135};
    static const GXColor cK0 = {0x00, 0x00, 0xE2, 0x58};
    static const GXColor cK1 = {0xB3, 0x00, 0x00, 0xB6};
    static const GXColor cK2 = {0xFF, 0x00, 0xFF, 0x00};

    nStage = 0;
    if (gRenderState.uChanged != 0) {
        // depth: compare unless the test always passes (GX_ALWAYS)
        if ((gRenderState.uChanged & 0x1) || (gRenderState.uChanged & 0x2)) {
            GXSetZMode(gRenderState.nDepthCompare != 7, gRenderState.nDepthCompare,
                       gRenderState.bDepthWrite);
        }
        if (gRenderState.uChanged & 0x10) {
            if (gRenderState.nBlendSrc == 1) {
                gRenderState.nBlendMode = 3;
            }
            GXSetBlendMode(gRenderState.nBlendMode, gRenderState.nBlendSrc, gRenderState.nBlendDst, 0);
        }
        if (gRenderState.uChanged & 0x80) {
            RenderState_SetConstantAlphaActive(gRenderState.bConstantAlpha);
            if (gRenderState.bConstantAlpha != 0) {
                RenderState_SetKColorAlpha(gRenderState.nConstantAlpha);
            } else {
                RenderState_SetKColorAlpha(0xFF);
            }
        }
        // alpha test: off, or compare against nAlphaRef with the depth test after texturing
        if (gRenderState.uChanged & 0x4) {
            if (gRenderState.bAlphaTest == 0) {
                GXSetZCompLoc(1);
                GXSetAlphaCompare(7, 0, 0, 7, 0);
            } else {
                GXSetZCompLoc(0);
                GXSetAlphaCompare(gRenderState.nAlphaCompare, gRenderState.nAlphaRef, 0, 7, 0);
            }
        }
        if (gRenderState.uChanged & 0x20) {
            // untextured: one stage of the vertex colour
            if (!(gRenderState.uDrawFlags & 0x10)) {
                if (fn_8002A3A4()) {
                    GXSetNumTexGens(1);
                    GXSetNumTevStages(2);
                    fn_8002A3AC(*lbl_80280DC8);
                    GXSetTevOrder(1, 0xFF, 0xFF, 4);
                    GXSetTevColorIn(1, 15, 15, 15, 10);
                    GXSetTevColorOp(1, 0, 0, 1, 1, 0);
                    GXSetTevAlphaIn(1, 7, 0, 5, 7);
                    GXSetTevAlphaOp(1, 0, 0, 1, 1, 0);
                } else {
                    GXSetNumTexGens(0);
                    GXSetNumTevStages(1);
                    GXSetTevOrder(0, 0xFF, 0xFF, 4);
                    GXSetTevColorIn(0, 15, 15, 15, 10);
                    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
                    GXSetTevAlphaIn(0, 7, 7, 7, 5);
                    GXSetTevAlphaOp(0, 0, 0, 1, 1, 0);
                }
            }
            gRenderState.uChanged |= 0x8;
            if (!(gRenderState.uDrawFlags & 0x40)) {
                gRenderState.nBlendMode = 0;
                GXSetBlendMode(0, gRenderState.nBlendSrc, gRenderState.nBlendDst, 0);
            } else if (gRenderState.nBlendMode == 0 ||
                       (gRenderState.nBlendMode == 3 && gRenderState.nBlendSrc != 1)) {
                gRenderState.nBlendMode = 1;
                GXSetBlendMode(1, gRenderState.nBlendSrc, gRenderState.nBlendDst, 0);
            }
        }
        if (gRenderState.uChanged & 0x400) {
            GXSetClipMode(gRenderState.nClipMode);
        }
        if (gRenderState.uChanged & 0x8) {
            // fog only while bit 0x20 is set
            GXSetFog((gRenderState.uDrawFlags & 0x20) ? gRenderState.nFogType : 0,
                     gRenderState.fFogStart, gRenderState.fFogEnd, gRenderState.fNearZ,
                     gRenderState.fFarZ, gRenderState.c30);
        }
        if (gRenderState.uChanged & 0x100) {
            pCamera = RC_spGetCurrentRenderCtx();
            GXLoadPosMtxImm(gRenderState.mView, 0);
            PSMTXInvXpose(gRenderState.mView, mNormal);
            GXLoadNrmMtxImm(mNormal, 0);
            if (fn_80008378(pCamera->unk10) == 0) {
                GXSetProjection(gRenderState.mProjection, 0);
            } else {
                GXSetProjection(gRenderState.mProjection, 1);
            }
        }
        if (gRenderState.uChanged & 0x200) {
            GXSetScissor(gRenderState.nScissorLeft, gRenderState.nScissorTop,
                         gRenderState.nScissorRight - gRenderState.nScissorLeft + 1,
                         gRenderState.nScissorBottom - gRenderState.nScissorTop + 1);
        }
        if (gRenderState.uChanged & 0x800) {
            GXSetViewport(gRenderState.fViewportLeft, gRenderState.fViewportTop,
                          gRenderState.fViewportWidth, gRenderState.fViewportHeight,
                          gRenderState.fViewportNear, gRenderState.fViewportFar);
        }
        if (gRenderState.uChanged & 0x1000) {
            fn_8002F38C(gRenderState.nSurface, gRenderState.nSurfaceWidth, gRenderState.nSurfaceHeight,
                        gRenderState.nSurfaceField, gRenderState.nF4, gRenderState.nF8);
        }
        gRenderState.uChanged = 0;
    }

    if (gRenderState.uFlags != 0) {
        if ((gRenderState.uFlags & 0x1) && (gRenderState.uDrawFlags & 0x10)) {
            fn_8000F0EC(gRenderState.pTexBank, gRenderState.pTexEntry);
        }
        if (gRenderState.uFlags & 0x2) {
            if (fn_8002A3A4()) {
                nStage = 1;
                fn_8002A3AC(*lbl_80280DC8);
            }
            GXSetNumTexGens(1);
            GXSetTevOrder(nStage, 0, nStage, 4);
            if (fn_8002A3A4()) {
                // both cases set the same stage
                if (*lbl_80280DC8 != 0) {
                    GXSetTevColorIn(nStage, 15, 8, 10, 15);
                    GXSetTevColorOp(nStage, 0, 0, 1, 1, 0);
                    GXSetTevAlphaIn(nStage, 7, 4, 0, 7);
                    GXSetTevAlphaOp(nStage, 0, 0, 1, 1, 0);
                } else {
                    GXSetTevColorIn(nStage, 15, 8, 10, 15);
                    GXSetTevColorOp(nStage, 0, 0, 1, 1, 0);
                    GXSetTevAlphaIn(nStage, 7, 4, 0, 7);
                    GXSetTevAlphaOp(nStage, 0, 0, 1, 1, 0);
                }
            } else if (*lbl_80280DC8 != 0) {
                GXSetTevColorIn(nStage, 15, 8, 10, 15);
                GXSetTevColorOp(nStage, 0, 0, 1, 1, 0);
                GXSetTevAlphaIn(nStage, 7, 4, 6, 7);
                GXSetTevAlphaOp(nStage, 0, 0, 1, 1, 0);
            } else {
                GXSetTevColorIn(nStage, 15, 8, 10, 15);
                GXSetTevColorOp(nStage, 0, 0, 1, 1, 0);
                GXSetTevAlphaIn(nStage, 7, 4, 5, 7);
                GXSetTevAlphaOp(nStage, 0, 0, 1, 1, 0);
            }
            if (gRenderState.pTex108->bPalette) {
                GXLoadTlut(&gRenderState.pTex108->tlut, nStage);
            }
            GXLoadTexObj(&gRenderState.pTex108->tex, nStage);
            GXSetNumTevStages(nStage + 1);
        }
        // a movie picture: its Y, U and V planes, turned into RGB over four stages
        if ((gRenderState.uFlags & 0x4) && gRenderState.pPict10C != NULL) {
            if (fn_8002A3A4()) {
                nStage++;
                fn_8002A3AC(*lbl_80280DC8);
            }
            GXLoadTexObj(&gRenderState.pPict10C->aTex[0], 0);
            GXLoadTexObj(&gRenderState.pPict10C->aTex[2], 1);
            GXLoadTexObj(&gRenderState.pPict10C->aTex[1], 2);
            GXSetNumTexGens(nStage + 2);
            RenderState_SetTexCoordGen(0, 1, 4, 60);
            RenderState_SetTexCoordGen(1, 1, 4, 60);
            GXSetNumTevStages(nStage + 4);
            GXSetTevOrder(nStage, 1, 2, 0xFF);
            GXSetTevColorIn(nStage, 15, 8, 14, 2);
            GXSetTevColorOp(nStage, 0, 0, 0, 0, 0);
            GXSetTevAlphaIn(nStage, 7, 4, 6, 1);
            GXSetTevAlphaOp(nStage, 1, 0, 0, 0, 0);
            GXSetTevKColorSel(nStage, 12);
            GXSetTevKAlphaSel(nStage, 28);
            GXSetTevSwapMode(nStage, 0, 0);
            GXSetTevOrder(nStage + 1, 1, 1, 0xFF);
            GXSetTevColorIn(nStage + 1, 15, 8, 14, 0);
            GXSetTevColorOp(nStage + 1, 0, 0, 1, 0, 0);
            GXSetTevAlphaIn(nStage + 1, 7, 4, 6, 0);
            GXSetTevAlphaOp(nStage + 1, 1, 0, 0, 0, 0);
            GXSetTevKColorSel(nStage + 1, 13);
            GXSetTevKAlphaSel(nStage + 1, 29);
            GXSetTevSwapMode(nStage + 1, 0, 0);
            GXSetTevOrder(nStage + 2, 0, 0, 0xFF);
            GXSetTevColorIn(nStage + 2, 15, 8, 12, 0);
            GXSetTevColorOp(nStage + 2, 0, 0, 0, 1, 0);
            GXSetTevAlphaIn(nStage + 2, 4, 7, 7, 0);
            GXSetTevAlphaOp(nStage + 2, 0, 0, 0, 1, 0);
            GXSetTevSwapMode(nStage + 2, 0, 0);
            GXSetTevOrder(nStage + 3, 0xFF, 0xFF, 0xFF);
            GXSetTevColorIn(nStage + 3, 1, 0, 14, 15);
            GXSetTevColorOp(nStage + 3, 0, 0, 0, 1, 0);
            GXSetTevAlphaIn(nStage + 3, 7, 7, 7, 6);
            GXSetTevAlphaOp(nStage + 3, 0, 0, 0, 1, 0);
            GXSetTevSwapMode(nStage + 3, 0, 0);
            GXSetTevKColorSel(nStage + 3, 14);
            GXSetTevColorS10(1, cYuv);
            GXSetTevKColor(0, cK0);
            GXSetTevKColor(1, cK1);
            GXSetTevKColor(2, cK2);
            GXSetTevSwapModeTable(0, 0, 1, 2, 3);
        }
        gRenderState.uFlags = 0;
    }
}

// The SDK's GXSetTexCoordGen out of line: texture coordinate nCoord from source nSrc through
// function nFunc and matrix nMtx, not normalised, with the identity post-transform matrix (125).
void RenderState_SetTexCoordGen(s32 nCoord, s32 nFunc, s32 nSrc, s32 nMtx) {
    GXSetTexCoordGen2(nCoord, nFunc, nSrc, nMtx, 0, 125);
}

// The current render context: the render camera being drawn with, as RC_vSetCurrentRenderCtx set
// it.
void* RC_spGetCurrentRenderCtx(void) {
    return *lbl_80280DF0;
}

// Loads TEV constant colour 0 with the alpha nAlpha (RenderState_Apply: nConstantAlpha
// while bConstantAlpha is on, else 0xFF).
void RenderState_SetKColorAlpha(u8 nAlpha) {
    GXColor colour;

    // EA bug: only the alpha is set; r, g and b are whatever was on the stack
    colour.a = nAlpha;
    GXSetTevKColor(0, colour);
}

// Sets the flag byte *lbl_80280DC8 (LLTex.c) that the TEV setups read: nonzero while the constant
// alpha is in use, so untextured stages take their alpha from TEV constant colour 0 instead of the
// vertex colour.
void RenderState_SetConstantAlphaActive(u8 bActive) {
    *lbl_80280DC8 = bActive;
}
