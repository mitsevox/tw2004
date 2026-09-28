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

void Mtx_Identity(f32 (*m)[4]);          // identity matrix
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

    // fake match: m74, u110 and uFlags through the global, the rest through p (only this mix gives
    // the original's base registers)
    p->n0 = 3;
    p->b4 = 1;
    p->n8 = 6;
    p->bC = 100;
    p->bD = 0;
    p->n10 = 4;
    p->n14 = 5;
    p->n18 = 1;
    p->b1C = 0xFF;
    p->b1D = 0;
    p->u20 = 0x70;
    p->nFC = 0;
    p->n24 = 2;
    p->f28 = 100.0f;
    p->f2C = 2048.0f;
    *(u32*)&p->c30 = 0xFFFFFFFF; // port: all four GXColor bytes 0xFF, stored as one word
    Mtx_Identity(p->m34);
    Mtx_Identity(gRenderState.m74);
    p->p100 = NULL;
    p->p104 = NULL;
    gRenderState.u110 = 0;
    gRenderState.uFlags = 0;
    GXSetCurrentMtx(0);
    BufferPool_FreeAll();
}

// Display-state shutdown (gomainloop's fn_8006C854): nothing to do on this machine.
void DS_vCloseModule(void) {
}

// Hands GX every group of gRenderState whose bit is set in u110 (depth, blending, constant alpha,
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
    if (gRenderState.u110 != 0) {
        // depth: compare unless the test always passes (GX_ALWAYS)
        if ((gRenderState.u110 & 0x1) || (gRenderState.u110 & 0x2)) {
            GXSetZMode(gRenderState.n0 != 7, gRenderState.n0, gRenderState.b4);
        }
        if (gRenderState.u110 & 0x10) {
            if (gRenderState.n10 == 1) {
                gRenderState.n18 = 3;
            }
            GXSetBlendMode(gRenderState.n18, gRenderState.n10, gRenderState.n14, 0);
        }
        if (gRenderState.u110 & 0x80) {
            RenderState_SetConstantAlphaActive(gRenderState.b1D);
            if (gRenderState.b1D != 0) {
                RenderState_SetKColorAlpha(gRenderState.b1C);
            } else {
                RenderState_SetKColorAlpha(0xFF);
            }
        }
        // alpha test: off, or compare n8 against the reference bC with the depth test after texturing
        if (gRenderState.u110 & 0x4) {
            if (gRenderState.bD == 0) {
                GXSetZCompLoc(1);
                GXSetAlphaCompare(7, 0, 0, 7, 0);
            } else {
                GXSetZCompLoc(0);
                GXSetAlphaCompare(gRenderState.n8, gRenderState.bC, 0, 7, 0);
            }
        }
        if (gRenderState.u110 & 0x20) {
            // untextured: one stage of the vertex colour
            if (!(gRenderState.u20 & 0x10)) {
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
            gRenderState.u110 |= 0x8;
            if (!(gRenderState.u20 & 0x40)) {
                gRenderState.n18 = 0;
                GXSetBlendMode(0, gRenderState.n10, gRenderState.n14, 0);
            } else if (gRenderState.n18 == 0 ||
                       (gRenderState.n18 == 3 && gRenderState.n10 != 1)) {
                gRenderState.n18 = 1;
                GXSetBlendMode(1, gRenderState.n10, gRenderState.n14, 0);
            }
        }
        if (gRenderState.u110 & 0x400) {
            GXSetClipMode(gRenderState.nFC);
        }
        if (gRenderState.u110 & 0x8) {
            // fog only while bit 0x20 is set
            GXSetFog((gRenderState.u20 & 0x20) ? gRenderState.n24 : 0, gRenderState.f28,
                     gRenderState.f2C, gRenderState.fB4, gRenderState.fB8, gRenderState.c30);
        }
        if (gRenderState.u110 & 0x100) {
            pCamera = RC_spGetCurrentRenderCtx();
            GXLoadPosMtxImm(gRenderState.m34, 0);
            PSMTXInvXpose(gRenderState.m34, mNormal);
            GXLoadNrmMtxImm(mNormal, 0);
            if (fn_80008378(pCamera->unk10) == 0) {
                GXSetProjection(gRenderState.m74, 0);
            } else {
                GXSetProjection(gRenderState.m74, 1);
            }
        }
        if (gRenderState.u110 & 0x200) {
            GXSetScissor(gRenderState.nBC, gRenderState.nC4, gRenderState.nC0 - gRenderState.nBC + 1,
                         gRenderState.nC8 - gRenderState.nC4 + 1);
        }
        if (gRenderState.u110 & 0x800) {
            GXSetViewport(gRenderState.fCC, gRenderState.fD0, gRenderState.fD4, gRenderState.fD8,
                          gRenderState.fDC, gRenderState.fE0);
        }
        if (gRenderState.u110 & 0x1000) {
            fn_8002F38C(gRenderState.nE4, gRenderState.nE8, gRenderState.nEC, gRenderState.nF0,
                        gRenderState.nF4, gRenderState.nF8);
        }
        gRenderState.u110 = 0;
    }

    if (gRenderState.uFlags != 0) {
        if ((gRenderState.uFlags & 0x1) && (gRenderState.u20 & 0x10)) {
            fn_8000F0EC(gRenderState.p100, gRenderState.p104);
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

// Loads TEV constant colour 0 with the alpha nAlpha (RenderState_Apply: the constant alpha b1C
// while it is on, else 0xFF).
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
