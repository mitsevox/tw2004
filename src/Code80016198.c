// Code80016198.c (our name; TW07 gives no file): own unit, its .sdata is padded to 8 at
// 0x80280E0C..0x80280E10

#include "ustream.h"
#include "camera.h"
#include "game.h"
#include "golfer.h"
#include "character.h"
#include "frontend/fe.h"
#include "gx.h"
#include "llpict.h"
#include "unsorted/cull.h"

// ---- sweep code (not yet cleaned up) ----

void fn_8000ADC0(f32 (*m)[4]);          // identity matrix
s32  fn_800072E0(void);
void fn_8000A0E8(f32 (*pSrc)[4], f32 (*pDst)[4]);  // UMemPool.c: copy a 4x4 matrix
void fn_80016C28(f32* pSrc, f32* pDst);             // negate four floats
void fn_80012520(u32 ePrim, u32 eFormat, u16 nVerts);  // LLFont.c: GXBegin
void fn_800124A8(void);                                // LLFont.c: end the primitive
void fn_800162AC(f32* pPos, f32* pColour, f32* pUV, int nVerts);
void fn_800169AC(void);     // apply lbl_80280E08's viewport
void fn_80016208(void);
void fn_80016978(f32 x0, f32 y0, f32 x1, f32 y1);
void fn_80016B54(int nWidth, int nHeight, f32 fX, f32 fY);
void fn_80016B6C(f32 fX, f32 fY);
void fn_800162A0(void);
void fn_800162A4(void);
void fn_800162A8(void);
void fn_80016C7C(s32 p0, s32 p1, s32 p2, s32 p3);
void fn_80016C94(f32 farg0, f32 farg1, f32 farg2);
void fn_80016CA8(f32 farg0, f32 farg1);

// ---- end of sweep code ----

void fn_800166E8(f32* pPos);
void fn_80016770(f32* pPos, f32* pColour);
void fn_80016800(f32* pPos, f32* pUV);
void fn_800168A0(f32* pPos, f32* pColour, f32* pUV);
void fn_80016C44(const f32* pViewport);

// Reset the view: whole-screen viewport, a 512 x 448 screen, scales of 1.
void fn_80016198(void) {
    fn_80016B6C(1.0f, 1.0f);
    fn_8001425C(0);
    fn_800141CC();
    lbl_80280E08->fF4 = 1.0f;
    lbl_80280E08->fF8 = 1.0f;
    fn_80016978(0.0f, 0.0f, 1.0f, 1.0f);
    fn_80016B54(512, 448, 1.0f, 1.0f);
    fn_80016208();
}

// Reset the view's matrices: an orthographic 0..1 projection and an identity view, whose depth
// scale depends on fn_800072E0.
void fn_80016208(void) {
    int n;

    C_MTXOrtho(lbl_80280E08->m0, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    PSMTXIdentity(lbl_80280E08->m40);
    n = 1 << fn_800072E0();
    lbl_80280E08->m40[2][2] = -((f32)n - 1.0f) / (f32)n;
}

// ---- sweep code (not yet cleaned up) ----

void fn_800162A0(void) {
}

void fn_800162A4(void) {
}

void fn_800162A8(void) {
}

// ---- end of sweep code ----

// Draw primitive 0xA1: each pair of vertices gives the opposite corners of a rectangle (x from
// one, y from the other), drawn as a 4-vertex strip (0x98). Texture coordinates are spread over
// the corners the same way; colours by row (the first vertex's on the first two corners).
void fn_800162AC(f32* pPos, f32* pColour, f32* pUV, int nVerts) {
    f32 aPos[4][4];
    f32 aColour[4][4];
    f32 aUV[4][4];
    f32* pQuadUV;
    int i;
    f32* pQuadColour;
    f32 fX0;
    f32 fX1;

    for (i = 0; i < nVerts - 1; i += 2) {
        fX0 = pPos[i * 4];
        fX1 = pPos[i * 4 + 4];
        Vec_Copy(&pPos[i * 4], aPos[0]);
        Vec_Copy(&pPos[i * 4], aPos[1]);
        aPos[1][0] = fX1;
        Vec_Copy(&pPos[(i + 1) * 4], aPos[2]);
        aPos[2][0] = fX0;
        Vec_Copy(&pPos[(i + 1) * 4], aPos[3]);
        if (pColour != NULL) {
            Vec_Copy(&pColour[i * 4], aColour[0]);
            Vec_Copy(&pColour[i * 4], aColour[1]);
            Vec_Copy(&pColour[(i + 1) * 4], aColour[2]);
            Vec_Copy(&pColour[(i + 1) * 4], aColour[3]);
            pQuadColour = aColour[0];
        } else {
            pQuadColour = NULL;
        }
        if (pUV != NULL) {
            fX0 = pUV[i * 4];
            fX1 = pUV[i * 4 + 4];
            Vec_Copy(&pUV[i * 4], aUV[0]);
            Vec_Copy(&pUV[i * 4], aUV[1]);
            aUV[1][0] = fX1;
            Vec_Copy(&pUV[(i + 1) * 4], aUV[2]);
            aUV[2][0] = fX0;
            Vec_Copy(&pUV[(i + 1) * 4], aUV[3]);
            pQuadUV = aUV[0];
        } else {
            pQuadUV = NULL;
        }
        fn_8001644C(0x98, aPos[0], pQuadColour, pQuadUV, 4);
    }
}

// Draw nVerts vertices as primitive ePrim in the view (0xA1 goes through fn_800162AC). Each
// vertex takes four floats of pPos, and of pColour and pUV when given; without pColour the view's
// colour is used. Unless the view's nD0 is set, its viewport and matrices are used for the draw
// and the GX state is put back afterwards.
void fn_8001644C(int ePrim, f32* pPos, f32* pColour, f32* pUV, int nVerts) {
    f32 aProjection[7];
    f32 aViewport[6];
    ViewState* pView = lbl_80280E08;
    int i;

    if (ePrim == 0xA1) {
        fn_800162AC(pPos, pColour, pUV, nVerts);
        return;
    }
    if (pView->nD0 == 0) {
        GXGetProjectionv(aProjection);
        GXGetViewportv(aViewport);
        GXSetViewport(pView->n70, pView->n74, pView->n78, pView->n7C, 0.0f, 1.0f);
        GXSetProjection(pView->m0, 1);
        GXLoadPosMtxImm(pView->m40, 0);
        GXSetCurrentMtx(0);
    }
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(11, 1);
    if (pUV != NULL) {
        GXSetVtxDesc(13, 1);
    }
    GXSetVtxAttrFmt(7, 9, 1, 4, 0);
    GXSetVtxAttrFmt(7, 11, 1, 5, 0);
    if (pUV != NULL) {
        GXSetVtxAttrFmt(7, 13, 1, 4, 0);
    }
    fn_80012520(ePrim, 7, nVerts);
    if (pUV != NULL) {
        if (pColour != NULL) {
            for (i = 0; i < nVerts; i++) {
                fn_800168A0(&pPos[i * 4], &pColour[i * 4], &pUV[i * 4]);
            }
        } else {
            for (i = 0; i < nVerts; i++) {
                fn_80016800(&pPos[i * 4], &pUV[i * 4]);
            }
        }
    } else if (pColour != NULL) {
        for (i = 0; i < nVerts; i++) {
            fn_80016770(&pPos[i * 4], &pColour[i * 4]);
        }
    } else {
        for (i = 0; i < nVerts; i++) {
            fn_800166E8(&pPos[i * 4]);
        }
    }
    fn_800124A8();
    if (pView->nD0 == 0) {
        GXSetProjectionv(aProjection);
        fn_80016C44(aViewport);
        fn_80016B9C();
        fn_80012EF8();
    }
}

// Send a vertex to the GPU: its position and the view's colour.
void fn_800166E8(f32* pPos) {
    fn_80016C94(pPos[0], pPos[1], pPos[2]);
    fn_80016C7C(255.0f * lbl_80280E08->aColour[0], 255.0f * lbl_80280E08->aColour[1],
                255.0f * lbl_80280E08->aColour[2], 255.0f * lbl_80280E08->aColour[3]);
}

// Send a vertex with its own colour (0..1 per channel).
void fn_80016770(f32* pPos, f32* pColour) {
    fn_80016C94(pPos[0], pPos[1], pPos[2]);
    fn_80016C7C(255.0f * pColour[0], 255.0f * pColour[1], 255.0f * pColour[2], 255.0f * pColour[3]);
}

// Send a vertex with the view's colour and a texture coordinate.
void fn_80016800(f32* pPos, f32* pUV) {
    fn_80016C94(pPos[0], pPos[1], pPos[2]);
    fn_80016C7C(255.0f * lbl_80280E08->aColour[0], 255.0f * lbl_80280E08->aColour[1],
                255.0f * lbl_80280E08->aColour[2], 255.0f * lbl_80280E08->aColour[3]);
    fn_80016CA8(pUV[0], pUV[1]);
}

// Send a vertex with its own colour and a texture coordinate.
void fn_800168A0(f32* pPos, f32* pColour, f32* pUV) {
    fn_80016C94(pPos[0], pPos[1], pPos[2]);
    fn_80016C7C(255.0f * pColour[0], 255.0f * pColour[1], 255.0f * pColour[2], 255.0f * pColour[3]);
    fn_80016CA8(pUV[0], pUV[1]);
}

// Draw to the whole screen.
void fn_80016948(void) {
    fn_80016978(0.0f, 0.0f, 1.0f, 1.0f);
}

// Set the viewport's corners, as fractions of the screen.
void fn_80016978(f32 x0, f32 y0, f32 x1, f32 y1) {
    ViewState* pView = lbl_80280E08;

    pView->fD4 = x0;
    pView->fD8 = y0;
    pView->fDC = x1;
    pView->fE0 = y1;
    fn_800169AC();
}

// Work out the viewport in pixels from its corners and the screen size, and its matrix.
void fn_800169AC(void) {
    ViewState* pView = lbl_80280E08;

    pView->n70 = pView->fD4 * pView->nE4;
    pView->n74 = pView->fD8 * pView->nE8;
    pView->n78 = pView->fDC * pView->nE4;
    pView->n7C = pView->fE0 * pView->nE8;
    pView->m40[0][0] = pView->fF4;
    pView->m40[1][1] = pView->fF8;
    fn_8000ADC0(pView->m80);
    pView->m80[3][0] += pView->n70;
    pView->m80[3][1] += pView->n74;
    pView->m80[0][0] = pView->n78 * pView->fF4;
    pView->m80[1][1] = pView->n7C * pView->fF8;
    pView->m80[2][2] = 0.0f;
}

void fn_80016B54(int nWidth, int nHeight, f32 fX, f32 fY) {
    ViewState* pView = lbl_80280E08;

    pView->nE4 = nWidth;
    pView->nE8 = nHeight;
    pView->fEC = fX;
    pView->fF0 = fY;
}

void fn_80016B6C(f32 fX, f32 fY) {
    lbl_80280E08->fF4 = fX;
    lbl_80280E08->fF8 = fY;
    fn_800169AC();
}

// Give the renderer the current camera's matrices, with rows 0 and 2 of the first negated.
void fn_80016B9C(void) {
    fn_8000A0E8(((Camera*)fn_8001614C())->m15C, lbl_801B8980.m34);
    fn_8000A0E8(((Camera*)fn_8001614C())->m9C, lbl_801B8980.m74);
    fn_80016C28(lbl_801B8980.m34[0], lbl_801B8980.m34[0]);
    fn_80016C28(lbl_801B8980.m34[2], lbl_801B8980.m34[2]);
    lbl_801B8980.u110 |= 0x100;
}

// Negates four floats from pSrc into pDst (paired singles).
#ifdef __MWERKS__
asm void fn_80016C28(register f32* pSrc, register f32* pDst) {
    nofralloc
    psq_l  f0, 0(pSrc), 0, 0
    psq_l  f1, 8(pSrc), 0, 0
    ps_neg f0, f0
    ps_neg f1, f1
    psq_st f0, 0(pDst), 0, 0
    psq_st f1, 8(pDst), 0, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_80016C28(f32* pSrc, f32* pDst) {
    pDst[0] = -pSrc[0];
    pDst[1] = -pSrc[1];
    pDst[2] = -pSrc[2];
    pDst[3] = -pSrc[3];
}
#endif

// Set the GX viewport from six values (GXGetViewportv's layout).
void fn_80016C44(const f32* pViewport) {
    GXSetViewport(pViewport[0], pViewport[1], pViewport[2], pViewport[3], pViewport[4], pViewport[5]);
}

// ---- sweep code (not yet cleaned up) ----

void fn_80016C7C(s32 p0, s32 p1, s32 p2, s32 p3) {
    *(volatile u8*)0xCC008000 = p0;
    *(volatile u8*)0xCC008000 = p1;
    *(volatile u8*)0xCC008000 = p2;
    *(volatile u8*)0xCC008000 = p3;
}

void fn_80016C94(f32 farg0, f32 farg1, f32 farg2) {
    *(f32* )0xCC008000 = farg0;
    *(f32* )0xCC008000 = farg1;
    *(f32* )0xCC008000 = farg2;
}

void fn_80016CA8(f32 farg0, f32 farg1) {
    *(f32* )0xCC008000 = farg0;
    *(f32* )0xCC008000 = farg1;
}

// ---- end of sweep code ----
