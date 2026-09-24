// GoRenderCtx_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the render
// camera (lens, frame buffer, screen rectangle and the matrices made from them), the renderer's
// screen state, and the pads' button masks.

#include "game_types.h"
#include "engine.h"
#include "unsorted/cull.h"

void fn_80013D58(Camera* pCamera);
void fn_80013D68(Camera* pCamera);
void fn_80013E28(Camera* pCamera, f32* pRect);
void fn_80013E30(Camera* pCamera, GoFrameBuf* pBuf);
void fn_80013E38(Camera* pCamera, CamLens* pLens);
GoFrameBuf* fn_80013E40(Camera* pCamera);
void fn_80013E48(Camera* pCamera);
void fn_80013EA0(Camera* pCamera);
void fn_800140E8(int a, int nWidth, int nHeight, int nField, int b, int c);
void fn_8000AB80(f32 (*pMtx)[4], f32 f1, f32 f2);   // matrix builders, not decompiled yet
void fn_8000ABE8(f32 (*pMtx)[4], f32 f1, f32 f2, f32 f3, f32 f4, f32 f5);
void fn_8000AC5C(f32 (*pMtx)[4], f32 f1, f32 f2, f32 f3, f32 f4);
f32 fn_80014268(u8* p);
f32 fn_80014270(u8* p);
f32 fn_80014280(f32 x0);

// Makes a render camera from a lens, a frame buffer and a screen rectangle.
void* fn_8001371C(CamLens* pLens, GoFrameBuf* pBuf, f32* pRect) {
    Camera* pCamera;

    pCamera = fn_80009B34(0x234, 2, 16, "GoRenderCtx_Gc.c", 96);
    fn_80013E38(pCamera, pLens);
    fn_80013E30(pCamera, pBuf);
    fn_80013E28(pCamera, pRect);
    fn_80013E48(pCamera);
    fn_80013D68(pCamera);
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

s32 fn_8000A0E8();
s32 fn_8000A714();
s32 fn_800BADF8();
void fn_80013D5C(s32 v);
void RC_vUpdateRenderCtxScreenMatricesAndInfo(Camera* pCamera);   // not decompiled yet
void fn_80013DD0(u8* arg0, f32 (*arg1)[4]);
s32 fn_8000ADC0();
f32 fn_80014134(u8* p);
f32 fn_8001413C(u8* p);
f32 fn_80014144(u8* p);
f32 fn_8001414C(u8* p);
f32 fn_80014154(u8* p);
f32 fn_8001417C(u8* p);
f32 fn_80014184(u8* p);
f32 fn_8001418C(u8* p);

void fn_800137B0(void* pCamera) {
    fn_80009E70(pCamera);
}

void fn_800137D0(Camera* pCamera) {
    fn_80013EEC(pCamera);
    fn_80012EF8();
    fn_80013EA0(pCamera);
}

// Covers the screen with one colour (pColour: r, g, b; NULL: the default grey). uFlags bit 0: keep
// fn_80012F34's setting; bit 1: pass 1 instead of 2 to the first fn_800140E8.
void fn_80013808(f32* pColour, u32 uFlags) {
    f32 aXY[8];

    fn_8001425C(0);
    fn_80014118(0);
    if (!(uFlags & 1)) {
        fn_80012F34(0);
    }
    if (!(uFlags & 2)) {
        fn_800140E8(0, 512, 448, lbl_80281B88 & 1, 2, 1);
    } else {
        fn_800140E8(0, 512, 448, lbl_80281B88 & 1, 1, 1);
    }
    fn_80012F50(0, 6, 0x80);
    fn_80012F18(7);
    fn_80012EF8();
    fn_800141F8(aXY, NULL, 0.0f, 0.0f, 1.0f, 1.0f);
    aXY[2] = 0.0f;
    aXY[6] = 0.0f;
    fn_80014194(pColour);
    fn_8001644C(0xA1, aXY, NULL, NULL, 2);
    fn_80012F50(0, 6, 0x80);
    fn_80012F18(3);
    fn_80012F34(1);
    fn_800140E8(0, 512, 448, lbl_80281B88 & 1, 8, 1);
    fn_80012EF8();
}

// Works out the camera's screen values from its lens, screen rectangle and frame buffer, then its
// projection (perspective, or flat when fn_80008378 says so) and the matrices made from it.
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
    pCamera->f224 = fn_80014280(fn_80014278(pLens) * 0.5f);
    pCamera->f228 = 1.0f / pCamera->f224;
    fn_8001415C(pBuf);
    fn_8001416C(pBuf);
    pCamera->f1E0 = pCamera->f228 * (fn_80012ED8(pRect) * fn_8001416C(pBuf) * 0.5f);
    pCamera->n22C = 0;
    pCamera->f230 = -(fn_8000AF7C(pCamera->f1E0 * (1.0f / 554.256f)) * 1.442695f);
    pCamera->f1E4 = fn_80012EE8(pRect) + fn_80012ED8(pRect) * 0.5f;
    pCamera->f1E8 = 1.0f - (fn_80012EE0(pRect) + fn_80012ED0(pRect) * 0.5f);
    pCamera->unk1F4 = pCamera->f1E0 * (fn_80014270((u8*)pLens) / 554.256f);
    pCamera->unk1F8 = fn_80014268((u8*)pLens);
    pCamera->f1FC = pCamera->f224 * fn_80014154((u8*)pRect) * fn_8001418C((u8*)pBuf);
    pCamera->f200 = fn_80014184((u8*)pBuf) * (fn_8001414C((u8*)pRect) *
                    (pCamera->f224 * fn_80014144((u8*)pRect) * fn_8001417C((u8*)pBuf)));

    aSrc[0] = 1.0f;
    aSrc[1] = pCamera->f1FC;
    aSrc[2] = 0.0f;
    fn_800BAF04(aSrc, aDst);
    pCamera->unk204 = aDst[1];
    pCamera->unk20C = aDst[0];
    aSrc[0] = 1.0f;
    aSrc[1] = pCamera->f200;
    aSrc[2] = 0.0f;
    fn_800BAF04(aSrc, aDst);
    pCamera->unk208 = aDst[1];
    pCamera->unk210 = aDst[0];
    aSrc[0] = 1.0f;
    aSrc[1] = pCamera->f1FC * 2.0f;
    aSrc[2] = 0.0f;
    fn_800BAF04(aSrc, aDst);
    pCamera->unk214 = aDst[1];
    pCamera->unk218 = aDst[0];
    aSrc[0] = 1.0f;
    aSrc[1] = pCamera->f200 * 2.0f;
    aSrc[2] = 0.0f;
    fn_800BAF04(aSrc, aDst);
    pCamera->unk21C = aDst[1];
    pCamera->unk220 = aDst[0];

    if (fn_80008378(pLens) == 0) {
        f = pRect[2] * (1.0f / fn_80014134((u8*)pRect)) / pRect[3];
        fn_8000ABE8(pCamera->m5C, pCamera->f228, 1.0f / fn_8001413C((u8*)pRect), f,
                    pCamera->unk1F4, pCamera->unk1F8);
    } else {
        fn_8000AB80(mFlat, pLens->fB4, pLens->fB8);
        f = pRect[2] * (1.0f / fn_80014134((u8*)pRect)) / pRect[3];
        fn_8000AC5C(mProj, 1.0f / fn_8001413C((u8*)pRect), f, pCamera->unk1F4, pCamera->unk1F8);
        fn_800BADF8(mProj, mFlat, pCamera->m5C, 4);
    }
    fn_8000A714(pCamera->m5C, pCamera->m9C);
    fn_800BADF8(pCamera->m5C, pLens->m44, pCamera->mDC, 4);
}

void fn_80013CCC(void* pCamera) {
    u8* arg0 = pCamera;

    if ((u8) (*(u8*)((u8*)(arg0) + 0x1DC)) != 0) {
        fn_8000A0E8((*(s32*)((u8*)(arg0) + 0x10)) + 0x44, arg0 + 0x11C);
        fn_8000A0E8(arg0 + 0xDC, arg0 + 0x19C);
    } else {
        fn_800BADF8((*(s32*)((u8*)(arg0) + 0x10)) + 0x44, arg0 + 0x1C, arg0 + 0x11C, 4);
        fn_800BADF8(arg0 + 0xDC, arg0 + 0x1C, arg0 + 0x19C, 4);
    }
    fn_8000A714(arg0 + 0x11C, arg0 + 0x15C);
}

void fn_80013D58(Camera* pCamera) {
}

void fn_80013D5C(s32 v) {
    *(s32*)(lbl_80280DF0 + 0x0) = v;
}

void fn_80013D68(Camera* pCamera) {
    RC_vUpdateRenderCtxScreenMatricesAndInfo(pCamera);
    fn_80013D58(pCamera);
}

// Gives the camera the view matrix pMtx (NULL: the identity).
void fn_80013D9C(void* pCamera, f32 (*pMtx)[4]) {
    fn_80013DD0(pCamera, pMtx);
    fn_80013CCC(pCamera);
}

void fn_80013DD0(u8* arg0, f32 (*arg1)[4]) {
    if (arg1 == NULL) {
        fn_8000ADC0(arg0 + 0x1C);
        (*(s8*)((u8*)(arg0) + 0x1DC)) = 1;
        return;
    }
    fn_8000A0E8(arg1, arg0 + 0x1C);
    (*(s8*)((u8*)(arg0) + 0x1DC)) = 0;
}

void fn_80013E28(Camera* pCamera, f32* pRect) {
    pCamera->pRect = pRect;
}

void fn_80013E30(Camera* pCamera, GoFrameBuf* pBuf) {
    pCamera->pBuf = pBuf;
}

void fn_80013E38(Camera* pCamera, CamLens* pLens) {
    pCamera->unk10 = pLens;
}

GoFrameBuf* fn_80013E40(Camera* pCamera) {
    return pCamera->pBuf;
}

// Starts a new camera: the identity view matrix and its first values.
void fn_80013E48(Camera* pCamera) {
    fn_80013D9C(pCamera, NULL);
    pCamera->a0[0] = 0.0f;
    pCamera->a0[1] = 0.0f;
    pCamera->a0[2] = 0.5f;
    pCamera->a0[3] = 0.0f;
    pCamera->f1EC = 1.0f;
    pCamera->f1F0 = 16773216.0f;
}

// Hands the camera's two values fn_80008360 and fn_80008368 to the renderer.
void fn_80013EA0(Camera* pCamera) {
    lbl_801B8980.fB4 = fn_80008360(pCamera);
    lbl_801B8980.fB8 = fn_80008368(pCamera);
}

// Hands the renderer the camera's screen rectangle: in frame buffer units (bit 0x800), and in
// 512 x 448 screen pixels, left, right, top, bottom (bit 0x200).
void fn_80013EEC(void* pCamera) {
    f32* pRect;
    GoFrameBuf* pBuf;

    pRect = fn_80012EF0(pCamera);
    pBuf = fn_80013E40(pCamera);
    lbl_801B8980.fCC = fn_80014174(pBuf) + fn_80012EE8(pRect) * fn_8001416C(pBuf);
    lbl_801B8980.fD0 = fn_80014164(pBuf) + fn_80012EE0(pRect) * fn_8001415C(pBuf);
    lbl_801B8980.fD4 = fn_80012ED8(pRect) * fn_8001416C(pBuf);
    lbl_801B8980.fD8 = fn_80012ED0(pRect) * fn_8001415C(pBuf);
    lbl_801B8980.fDC = 0.0f;
    lbl_801B8980.fE0 = 1.0f;
    lbl_801B8980.u110 |= 0x800;
    lbl_801B8980.nBC = fn_80012EE8(pRect) * 512.0f;
    lbl_801B8980.nC0 = (int)((fn_80012EE8(pRect) + fn_80012ED8(pRect)) * 512.0f) - 1;
    lbl_801B8980.nC4 = fn_80012EE0(pRect) * 448.0f;
    lbl_801B8980.nC8 = (int)((fn_80012EE0(pRect) + fn_80012ED0(pRect)) * 448.0f) - 1;
    lbl_801B8980.u110 |= 0x200;
}

void fn_800140E8(int a, int nWidth, int nHeight, int nField, int b, int c) {
    lbl_801B8980.nE4 = a;
    lbl_801B8980.nE8 = nWidth;
    lbl_801B8980.nEC = nHeight;
    lbl_801B8980.nF0 = nField;
    lbl_801B8980.nF4 = b;
    lbl_801B8980.nF8 = c;
    lbl_801B8980.u110 |= 0x1000;
}

void fn_80014118(int a) {
    lbl_801B8980.u20 = a;
    lbl_801B8980.u110 |= 0x20;
}

f32 fn_80014134(u8* p) {
    return *(f32*)(p + 0x14);
}

f32 fn_8001413C(u8* p) {
    return *(f32*)(p + 0x10);
}

f32 fn_80014144(u8* p) {
    return *(f32*)(p + 0x2C);
}

f32 fn_8001414C(u8* p) {
    return *(f32*)(p + 0x30);
}

f32 fn_80014154(u8* p) {
    return *(f32*)(p + 0x28);
}

f32 fn_8001415C(GoFrameBuf* pBuf) {
    return pBuf->fHeight;
}

f32 fn_80014164(GoFrameBuf* pBuf) {
    return pBuf->f4;
}

f32 fn_8001416C(GoFrameBuf* pBuf) {
    return pBuf->fWidth;
}

f32 fn_80014174(GoFrameBuf* pBuf) {
    return pBuf->f0;
}

f32 fn_8001417C(u8* p) {
    return *(f32*)(p + 0x2C);
}

f32 fn_80014184(u8* p) {
    return *(f32*)(p + 0x30);
}

f32 fn_8001418C(u8* p) {
    return *(f32*)(p + 0x28);
}

// ---- end of sweep code ----

// Set the colour of the view's vertices (r, g, b; alpha is kept); NULL: the default grey.
void fn_80014194(f32* pColour) {
    if (pColour == NULL) {
        fn_800141CC();
        return;
    }
    Vec_Copy(pColour, lbl_80280E08->aColour);
}

// The default vertex colour: half grey, opaque.
void fn_800141CC(void) {
    lbl_80280E08->aColour[0] = 0.5f;
    lbl_80280E08->aColour[1] = 0.5f;
    lbl_80280E08->aColour[2] = 0.5f;
    lbl_80280E08->aColour[3] = 1.0f;
}

// Fill a screen quad's two corners (x0, y0)-(x1, y1) and its texture coordinates (0,0)-(1,1),
// four floats per vertex.
void fn_800141F8(f32* pXY, f32* pUV, f32 x0, f32 y0, f32 x1, f32 y1) {
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
void fn_800142A4(s8 v);
void fn_800131C4(int nController);
void fn_8001437C(void);

void fn_8001425C(int a) {
    lbl_80280E08->nD0 = a;
}

f32 fn_80014268(u8* p) {
    return *(f32*)(p + 0xAC);
}

f32 fn_80014270(u8* p) {
    return *(f32*)(p + 0xA8);
}

f32 fn_80014278(CamLens* pLens) {
    return pLens->fFov;
}

f32 fn_80014280(f32 x0) {
    f32 t0;
    t0 = tan(x0);
    return t0;
}

void fn_800142A4(s8 v) {
    lbl_80281C98 = v;
}

// A button's mask in the row in use; bShift moves it up 16 bits.
u32 fn_800142AC(int nButton, u8 bShift) {
    if (bShift) {
        return lbl_80186AF0[lbl_80281C98][nButton] << 16;
    }
    return lbl_80186AF0[lbl_80281C98][nButton];
}

// Whether any of the four pads has any of the buttons in uMask (0: any button at all).
u8 fn_80014300(u32 uMask) {
    u8 bPressed = 0;
    int nController = 0;

    do {
        if ((uMask == 0 && fn_800136DC(nController) != 0) || (uMask & fn_800136DC(nController))) {
            bPressed = 1;
        }
        nController++;
    } while (nController < 4);
    return bPressed;
}

void fn_8001437C(void) {
    s32 var_r31;

    var_r31 = 0;
    do {
        fn_800131C4(var_r31);
        var_r31 += 1;
    } while (var_r31 < 4);
}

// ---- end of sweep code ----
