// Code80015470.c (our name; TW07 gives no file): own unit, its .sdata is padded to 8 at
// 0x80280E04..0x80280E08 and its .sdata2 at 0x80282B9C..0x80282BA0

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
RenderState lbl_801B8980;
BufferPool  lbl_801A4900;

BufferPool* lbl_80280E00 = &lbl_801A4900;

// ---- sweep code (not yet cleaned up) ----

void fn_8000ADC0(f32 (*m)[4]);          // identity matrix
void fn_80015620(void);
void fn_80016124(s32 p0, s32 p1, s32 p2, s32 p3);
void fn_80016158(u8 nAlpha);

// ---- end of sweep code ----

// Free every block of the pool.
void fn_80015470(void) {
    BufferPoolBlock* pBlock;
    s32 i;

    lbl_80280E00->nNext = 0;
    lbl_80280E00->n4 = 0;
    pBlock = lbl_80280E00->aBlocks;
    for (i = 0; i < 20; i++) {
        pBlock->u1000 = 0;
        pBlock++;
    }
}

// Move the pool's nNext past the blocks in use and return the first free block.
BufferPoolBlock* fn_800154F4(void) {
    BufferPoolBlock* pBlock;

    pBlock = &lbl_80280E00->aBlocks[lbl_80280E00->nNext];
    for (;;) {
        if (pBlock->u1000 == 0) {
            break;
        }
        pBlock++;
        lbl_80280E00->nNext++;
    }
    lbl_80280E00->n4++;
    return pBlock;
}

// Reset the renderer's state and free the buffer pool.
void fn_80015540(void) {
    RenderState* const p = &lbl_801B8980;

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
    fn_8000ADC0(p->m34);
    fn_8000ADC0(lbl_801B8980.m74);
    p->p100 = NULL;
    p->p104 = NULL;
    lbl_801B8980.u110 = 0;
    lbl_801B8980.uFlags = 0;
    GXSetCurrentMtx(0);
    fn_80015470();
}

// ---- sweep code (not yet cleaned up) ----

void fn_80015620(void) {
}

// ---- end of sweep code ----

// Hand GX every group of the renderer's state that changed (u110), then the texture of the next
// draw (uFlags). While fn_8002A164's screen copy is drawn with, it takes TEV stage 0 and the
// draw's stages start at 1.
void fn_80015624(void) {
    f32 mNormal[3][4];
    Camera* pCamera;
    int nStage;
    // the YUV to RGB conversion's constants
    static const GXColorS10 cYuv = {-90, 0, -114, 135};
    static const GXColor cK0 = {0x00, 0x00, 0xE2, 0x58};
    static const GXColor cK1 = {0xB3, 0x00, 0x00, 0xB6};
    static const GXColor cK2 = {0xFF, 0x00, 0xFF, 0x00};

    nStage = 0;
    if (lbl_801B8980.u110 != 0) {
        // depth: compare unless the test always passes (GX_ALWAYS)
        if ((lbl_801B8980.u110 & 0x1) || (lbl_801B8980.u110 & 0x2)) {
            GXSetZMode(lbl_801B8980.n0 != 7, lbl_801B8980.n0, lbl_801B8980.b4);
        }
        if (lbl_801B8980.u110 & 0x10) {
            if (lbl_801B8980.n10 == 1) {
                lbl_801B8980.n18 = 3;
            }
            GXSetBlendMode(lbl_801B8980.n18, lbl_801B8980.n10, lbl_801B8980.n14, 0);
        }
        if (lbl_801B8980.u110 & 0x80) {
            fn_8001618C(lbl_801B8980.b1D);
            if (lbl_801B8980.b1D != 0) {
                fn_80016158(lbl_801B8980.b1C);
            } else {
                fn_80016158(0xFF);
            }
        }
        // alpha test: off, or compare n8 against the reference bC with the depth test after texturing
        if (lbl_801B8980.u110 & 0x4) {
            if (lbl_801B8980.bD == 0) {
                GXSetZCompLoc(1);
                GXSetAlphaCompare(7, 0, 0, 7, 0);
            } else {
                GXSetZCompLoc(0);
                GXSetAlphaCompare(lbl_801B8980.n8, lbl_801B8980.bC, 0, 7, 0);
            }
        }
        if (lbl_801B8980.u110 & 0x20) {
            // untextured: one stage of the vertex colour
            if (!(lbl_801B8980.u20 & 0x10)) {
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
            lbl_801B8980.u110 |= 0x8;
            if (!(lbl_801B8980.u20 & 0x40)) {
                lbl_801B8980.n18 = 0;
                GXSetBlendMode(0, lbl_801B8980.n10, lbl_801B8980.n14, 0);
            } else if (lbl_801B8980.n18 == 0 ||
                       (lbl_801B8980.n18 == 3 && lbl_801B8980.n10 != 1)) {
                lbl_801B8980.n18 = 1;
                GXSetBlendMode(1, lbl_801B8980.n10, lbl_801B8980.n14, 0);
            }
        }
        if (lbl_801B8980.u110 & 0x400) {
            GXSetClipMode(lbl_801B8980.nFC);
        }
        if (lbl_801B8980.u110 & 0x8) {
            // fog only while bit 0x20 is set
            GXSetFog((lbl_801B8980.u20 & 0x20) ? lbl_801B8980.n24 : 0, lbl_801B8980.f28,
                     lbl_801B8980.f2C, lbl_801B8980.fB4, lbl_801B8980.fB8, lbl_801B8980.c30);
        }
        if (lbl_801B8980.u110 & 0x100) {
            pCamera = fn_8001614C();
            GXLoadPosMtxImm(lbl_801B8980.m34, 0);
            PSMTXInvXpose(lbl_801B8980.m34, mNormal);
            GXLoadNrmMtxImm(mNormal, 0);
            if (fn_80008378(pCamera->unk10) == 0) {
                GXSetProjection(lbl_801B8980.m74, 0);
            } else {
                GXSetProjection(lbl_801B8980.m74, 1);
            }
        }
        if (lbl_801B8980.u110 & 0x200) {
            GXSetScissor(lbl_801B8980.nBC, lbl_801B8980.nC4, lbl_801B8980.nC0 - lbl_801B8980.nBC + 1,
                         lbl_801B8980.nC8 - lbl_801B8980.nC4 + 1);
        }
        if (lbl_801B8980.u110 & 0x800) {
            GXSetViewport(lbl_801B8980.fCC, lbl_801B8980.fD0, lbl_801B8980.fD4, lbl_801B8980.fD8,
                          lbl_801B8980.fDC, lbl_801B8980.fE0);
        }
        if (lbl_801B8980.u110 & 0x1000) {
            fn_8002F38C(lbl_801B8980.nE4, lbl_801B8980.nE8, lbl_801B8980.nEC, lbl_801B8980.nF0,
                        lbl_801B8980.nF4, lbl_801B8980.nF8);
        }
        lbl_801B8980.u110 = 0;
    }

    if (lbl_801B8980.uFlags != 0) {
        if ((lbl_801B8980.uFlags & 0x1) && (lbl_801B8980.u20 & 0x10)) {
            fn_8000F0EC(lbl_801B8980.p100, lbl_801B8980.p104);
        }
        if (lbl_801B8980.uFlags & 0x2) {
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
            if (lbl_801B8980.pTex108->bPalette) {
                GXLoadTlut(&lbl_801B8980.pTex108->tlut, nStage);
            }
            GXLoadTexObj(&lbl_801B8980.pTex108->tex, nStage);
            GXSetNumTevStages(nStage + 1);
        }
        // a movie picture: its Y, U and V planes, turned into RGB over four stages
        if ((lbl_801B8980.uFlags & 0x4) && lbl_801B8980.pPict10C != NULL) {
            if (fn_8002A3A4()) {
                nStage++;
                fn_8002A3AC(*lbl_80280DC8);
            }
            GXLoadTexObj(&lbl_801B8980.pPict10C->aTex[0], 0);
            GXLoadTexObj(&lbl_801B8980.pPict10C->aTex[2], 1);
            GXLoadTexObj(&lbl_801B8980.pPict10C->aTex[1], 2);
            GXSetNumTexGens(nStage + 2);
            fn_80016124(0, 1, 4, 60);
            fn_80016124(1, 1, 4, 60);
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
        lbl_801B8980.uFlags = 0;
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_80016124(s32 p0, s32 p1, s32 p2, s32 p3) {
    GXSetTexCoordGen2(p0, p1, p2, p3, 0, 125);
}

void* fn_8001614C(void) {
    return *lbl_80280DF0;
}

// ---- end of sweep code ----

// Set the alpha of TEV constant colour 0.
void fn_80016158(u8 nAlpha) {
    GXColor colour;

    // EA bug: only the alpha is set; r, g and b are whatever was on the stack
    colour.a = nAlpha;
    GXSetTevKColor(0, colour);
}

// ---- sweep code (not yet cleaned up) ----

void fn_8001618C(u8 v) {
    *(u8*)(lbl_80280DC8 + 0x0) = v;
}

// ---- end of sweep code ----
