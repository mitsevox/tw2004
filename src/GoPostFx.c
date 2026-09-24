// GoPostFx.c (EA's name, from its asserts; TW06): the screen effects drawn over each view: a
// colour (lbl_801D50C0), and two effects made from a copy of the screen (lbl_801D5090,
// lbl_801D5020). Not decompiled yet beyond the functions below.

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "gx.h"
#include "camera.h"
#include "terrain.h"

void fn_80037E50(void);
void fn_80038A90(f32* pColour, u8 bCopy, int nView, int nField, f32 fCX, f32 fCY);
void fn_80038724(int nField, int nView, f32 fAlpha, f32 fShake);
void fn_80038E7C(f32* pXY, f32* pColour, f32* pUV, int n, f32* pSrc, f32 fCX, f32 fCY, f32 fX0,
                 f32 fY0, f32 fX1, f32 fY1, f32 fMaxDist);
void fn_800390CC(int nField, RenderCamera* pCamera);
void fn_800A6070(u8 nPlayer, u8 bLimit);      // GameAudio.c
void fn_80016948(void);
void fn_80016978(f32 x0, f32 y0, f32 x1, f32 y1);
void fn_800169AC(void);                     // apply lbl_80280E08's viewport
void fn_800140E8(int a, int nWidth, int nHeight, int nField, int b, int c);
void fn_80016B54(int nWidth, int nHeight, f32 fX, f32 fY);
void fn_80035F40(void* pCamera);            // Skin.c

// Makes the screen copy (game types 4..8 only) and clears every effect.
void PostFx_CopyScreenToBuffer(void) {
    lbl_80281D80 = NULL;
    if (gSession.nGameType >= 4 && gSession.nGameType <= 8) {
        lbl_80281D80 = fn_80009B34(GXGetTexBufferSize(256, 224, 6, 0, 0), 2, 0x20, "GoPostFx.c", 119);
    }
    fn_80037E50();
}

// Clears every view's effects and sets up the textures.
void fn_80037E50(void) {
    int i;

    for (i = 0; i < 4; i++) {
        lbl_801D50C0[i].b0 = 0;
        lbl_801D5090[i].b0 = 0;
        lbl_801D5020[i].b0 = 0;
        lbl_801D5010[i] = 0.0f;
    }
    if (gSession.nGameType >= 4 && gSession.nGameType <= 8) {
        fn_8002A528(&lbl_801D4FB0[0], 256, 224, lbl_80281D80, NULL, 6, 0, 0, 0);
        fn_8002A528(&lbl_801D4FB0[1], 256, 224, lbl_80281D80, NULL, 6, 0, 0, 0);
    }
    if (fn_8002A624() != NULL) {
        fn_8002A528(&lbl_801D4F80, 256, 128, fn_8002A624(), NULL, 4, 0, 0, 0);
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_80037F80(void);
void fn_80038010(u8 b, int nView, f32* pColour);
void fn_80037FB4(u8 b, f32* pColour);
u8 fn_8003944C();
void fn_80038314(void);
void fn_800383A8(void);
void fn_80038438(void);
void fn_800382E0(void);
void fn_80038A2C(int nField, RenderCamera* pCamera);
void fn_80038A6C(int nField, void* pCamera);
void fn_800392D0(void);

void fn_80037F80(void) {
    if (lbl_80281D80 != NULL) {
        fn_80009E70(lbl_80281D80);
        lbl_80281D80 = NULL;
    }
}

// Sets (b) or clears every view's colour.
void fn_80037FB4(u8 b, f32* pColour) {
    int i;

    for (i = 0; i < 4; i++) {
        fn_80038010(b, i, pColour);
    }
}

// ---- end of sweep code ----

// Sets (b) or clears view nView's colour.
void fn_80038010(u8 b, int nView, f32* pColour) {
    lbl_801D50C0[nView].b0 = b;
    if (b) {
        Vec_Copy(pColour, lbl_801D50C0[nView].aColour);
    }
}

void fn_80038054(u8 b, int nView, f32 f4, f32 f8) {
    lbl_801D5090[nView].b0 = b;
    lbl_801D5090[nView].f4 = f4;
    lbl_801D5090[nView].f8 = f8;
    if (f4 >= 0.035f) {
        fn_800A6070(0, 1);
    }
}

void fn_800380A8(u8 b, f32* pColour, u8 bCopy, int nView, f32 fX, f32 fY) {
    lbl_801D5020[nView].b0 = b;
    Vec_Copy(pColour, lbl_801D5020[nView].aColour);
    lbl_801D5020[nView].b1 = bCopy;
    lbl_801D5020[nView].fX = fX;
    lbl_801D5020[nView].fY = fY;
}

// Draws a thin vertical strip down the middle of the screen (x 0.495 to 0.505), clear at its edges
// and coloured (0.1, 0.2, 0.5, alpha 0.4) along its centre.
void fn_80038128(void) {
    f32 aXY[8][4];
    f32 aColour[8][4];

    fn_8001425C(0);
    fn_80012F34(0);
    fn_80012F18(7);
    fn_80014118(0x40);
    fn_80035118(4, 5);
    fn_80012F50(0, 6, 0x80);
    fn_80012EF8();

    aXY[0][0] = 0.495f;
    aXY[0][1] = 0.0f;
    aXY[0][2] = 1.0f;
    aXY[0][3] = 1.0f;
    aXY[1][0] = 0.495f;
    aXY[1][1] = 1.0f;
    aXY[1][2] = 1.0f;
    aXY[1][3] = 1.0f;
    aXY[2][0] = 0.5f;
    aXY[2][1] = 0.0f;
    aXY[2][2] = 1.0f;
    aXY[2][3] = 1.0f;
    aXY[3][0] = 0.5f;
    aXY[3][1] = 1.0f;
    aXY[3][2] = 1.0f;
    aXY[3][3] = 1.0f;
    aXY[4][0] = 0.5f;
    aXY[4][1] = 0.0f;
    aXY[4][2] = 1.0f;
    aXY[4][3] = 1.0f;
    aXY[5][0] = 0.5f;
    aXY[5][1] = 1.0f;
    aXY[5][2] = 1.0f;
    aXY[5][3] = 1.0f;
    aXY[6][0] = 0.505f;
    aXY[6][1] = 0.0f;
    aXY[6][2] = 1.0f;
    aXY[6][3] = 1.0f;
    aXY[7][0] = 0.505f;
    aXY[7][1] = 1.0f;
    aXY[7][2] = 1.0f;
    aXY[7][3] = 1.0f;

    aColour[0][0] = 0.0f;
    aColour[0][1] = 0.0f;
    aColour[0][2] = 0.0f;
    aColour[0][3] = 0.0f;
    aColour[1][0] = 0.0f;
    aColour[1][1] = 0.0f;
    aColour[1][2] = 0.0f;
    aColour[1][3] = 0.0f;
    aColour[2][0] = 0.1f;
    aColour[2][1] = 0.2f;
    aColour[2][2] = 0.5f;
    aColour[2][3] = 0.4f;
    aColour[3][0] = 0.1f;
    aColour[3][1] = 0.2f;
    aColour[3][2] = 0.5f;
    aColour[3][3] = 0.4f;
    aColour[4][0] = 0.1f;
    aColour[4][1] = 0.2f;
    aColour[4][2] = 0.5f;
    aColour[4][3] = 0.4f;
    aColour[5][0] = 0.1f;
    aColour[5][1] = 0.2f;
    aColour[5][2] = 0.5f;
    aColour[5][3] = 0.4f;
    aColour[6][0] = 0.0f;
    aColour[6][1] = 0.0f;
    aColour[6][2] = 0.0f;
    aColour[6][3] = 0.0f;
    aColour[7][0] = 0.0f;
    aColour[7][1] = 0.0f;
    aColour[7][2] = 0.0f;
    aColour[7][3] = 0.0f;

    fn_8001644C(0x98, aXY[0], aColour[0], NULL, 8);
    fn_80012F34(1);
    fn_80012F18(3);
    fn_80012F50(1, 6, 0x80);
    fn_80012EF8();
}

// ---- sweep code (not yet cleaned up) ----

void fn_800382E0(void) {
    if (fn_8003944C() == 0) {
        fn_80038314();
        fn_800383A8();
        fn_80038438();
    }
}

// ---- end of sweep code ----

// Draws the set lbl_801D5020 effects of the views in use, then clears them.
void fn_80038314(void) {
    int i;

    for (i = 0; i < 4; i++) {
        if (lbl_801D5020[i].b0 && fn_800170A0(i)) {
            fn_80038A90(lbl_801D5020[i].aColour, lbl_801D5020[i].b1, i, lbl_80281B88 & 1,
                        lbl_801D5020[i].fX, lbl_801D5020[i].fY);
            lbl_801D5020[i].b0 = 0;
        }
    }
}

// Draws the set lbl_801D5090 effects of the views in use, then clears them.
void fn_800383A8(void) {
    int i;

    for (i = 0; i < 4; i++) {
        if (lbl_801D5090[i].b0 && fn_800170A0(i)) {
            fn_80038724(lbl_80281B88 & 1, i, lbl_801D5090[i].f8, lbl_801D5090[i].f4);
            lbl_801D5090[i].b0 = 0;
        }
    }
    fn_8002A2FC();
}

// Draws the set colour of each view in use over that view's rectangle, then clears it. The
// current camera is given each view's rectangle for this and gets its own back after.
void fn_80038438(void) {
    int i;
    f32 aXY[8];
    f32* pRect;
    f32* pViewRect;
    f32 fX;
    f32 fY;
    f32 fH;
    f32 fW;

    fn_8001425C(0);
    fn_80012F34(0);
    fn_80014118(0x40);
    fn_80012F18(7);
    fn_80035118(4, 5);
    fn_80012F50(0, 6, 0x80);
    fn_80012EF8();
    pRect = fn_80012EF0(fn_8001614C());
    fX = pRect[0];
    fY = pRect[1];
    fW = pRect[2];
    fH = pRect[3];
    for (i = 0; i < 4; i++) {
        if (lbl_801D50C0[i].b0 && fn_800170A0(i)) {
            pViewRect = fn_80012EF0(fn_80017004(i));
            fn_800171D8(pRect, pViewRect[0], pViewRect[1], pViewRect[2], pViewRect[3]);
            fn_80035240(NULL);
            fn_80013CCC(fn_8001614C());
            fn_80013EEC(fn_8001614C());
            fn_80016B9C();
            fn_80012EF8();
            fn_800141F8(aXY, NULL, 0.0f, 0.0f, 1.0f, 1.0f);
            fn_80014194(lbl_801D50C0[i].aColour);
            fn_8001644C(0xA1, aXY, NULL, NULL, 2);
            lbl_801D50C0[i].b0 = 0;
        }
    }
    fn_800171D8(pRect, fX, fY, fW, fH);
    fn_80035240(NULL);
    fn_80013CCC(fn_8001614C());
    fn_80013EEC(fn_8001614C());
    fn_80016B9C();
    fn_80012F34(1);
    fn_80012F18(3);
    fn_80012F50(1, 6, 0x80);
    fn_80012EF8();
}

// Draws pColour over the whole screen.
void fn_80038624(f32* pColour) {
    f32 aXY[8];
    f32 aUV[8];

    fn_8001425C(0);
    fn_80016948();
    fn_80012F34(0);
    fn_80012F18(7);
    fn_80014118(0x40);
    fn_80035118(4, 5);
    fn_80012F50(0, 6, 0x80);
    fn_80012EF8();
    fn_800141F8(aXY, aUV, 0.0f, 0.0f, 1.0f, 1.0f);
    fn_80014194(pColour);
    fn_8001644C(0xA1, aXY, NULL, NULL, 2);
    fn_80012F34(1);
    fn_80012F18(3);
    fn_80012F50(1, 6, 0x80);
    fn_80012EF8();
}

// Draws the effect's screen copy (nField picks which of the two) over view nView's rectangle at
// fAlpha, shaken by up to half of fShake each way.
void fn_80038724(int nField, int nView, f32 fAlpha, f32 fShake) {
    f32 aColour[4];
    f32 aXY[8];
    f32 aUV[8];
    f32* pRect;
    f32 fX;
    f32 fY;
    f32 fRight;
    f32 fBottom;
    f32 fDX;
    f32 fDY;

    pRect = ((RenderCamera*)fn_80017004(nView))->pRect;
    fX = pRect[0];
    fY = pRect[1];
    fRight = pRect[0] + pRect[2];
    fBottom = pRect[1] + pRect[3];
    fn_80012F34(0);
    fn_80014118(0x50);
    fn_80035118(4, 5);
    fn_80035098(1);
    fn_80035154(255.0f * fAlpha);
    fn_80012F50(0, 1, 0x80);
    fn_80012F18(7);
    fn_8002A608(nField ? &lbl_801D4FB0[0] : &lbl_801D4FB0[1]);
    fn_800141F8(aXY, NULL, fX, fY, fRight, fBottom);
    aUV[0] = aXY[0];
    aUV[1] = aXY[1];
    aUV[4] = aXY[4];
    aUV[5] = aXY[5];
    fDX = fShake * (Rand_Float(1) - 0.5f);
    fDY = fShake * (Rand_Float(1) - 0.5f);
    aXY[0] += fDX;
    aXY[1] += fDY;
    aXY[4] += fDX;
    aXY[5] += fDY;
    aColour[0] = 0.5f;
    aColour[1] = 0.5f;
    aColour[2] = 0.5f;
    aColour[3] = 1.0f;
    fn_80014194(aColour);
    fn_8001425C(0);
    fn_80012EF8();
    GXSetTevAlphaIn(1, 7, 7, 7, 0);
    fn_8001644C(0xA1, aXY, NULL, aUV, 2);
    fn_80012F34(1);
    fn_80012F50(1, 6, 0x80);
    fn_80012F18(3);
    fn_80035098(0);
    fn_80012EF8();
}

// ---- sweep code (not yet cleaned up) ----

// Copies view nView's colour to pOut.
void fn_800386F0(int nView, f32* pOut) {
    Vec_Copy(lbl_801D50C0[nView].aColour, pOut);
}

// Turns fn_8002A024 on for the camera's screen rectangle (nField unused).
void fn_80038A2C(int nField, RenderCamera* pCamera) {
    fn_8002A024(1, pCamera->pRect[0], pCamera->pRect[1], pCamera->pRect[0] + pCamera->pRect[2],
                pCamera->pRect[1] + pCamera->pRect[3]);
}

// nField and pCamera are unused; fn_800389C0 passes them.
void fn_80038A6C(int nField, void* pCamera) {
    fn_8002A164(1);
}

// ---- end of sweep code ----

// With any view's lbl_801D5090 effect set, calls fn_80038A2C for the current render camera.
void fn_80038968(void) {
    int i;

    for (i = 0; i < 4; i++) {
        if (lbl_801D5090[i].b0) {
            fn_80038A2C(lbl_80281B88 & 1, fn_8001614C());
            return;
        }
    }
}

// Calls fn_80038A6C for the render camera of each view whose lbl_801D5090 effect is set.
void fn_800389C0(void) {
    int i;

    for (i = 0; i < 4; i++) {
        if (lbl_801D5090[i].b0) {
            fn_80038A6C(lbl_80281B88 & 1, fn_80017004(i));
        }
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_800392D0(void) {
    if (lbl_80281D80 != NULL) {
        GXSetZMode(0, 3, 0);
        GXSetTexCopySrc(0, 0, 0x200, 0x1C0);
        GXSetTexCopyDst(0x100, 0xE0, 6, 1);
        GXCopyTex(lbl_80281D80, 0);
        GXPixModeSync();
        GXInvalidateTexAll();
    }
}

// ---- end of sweep code ----

// Draws pColour over view nView as a fan from (fCX, fCY) (fractions of the view) to its edges, the
// alpha growing with the distance from the centre to pColour's alpha at the farthest corner.
// With bCopy, the effect's screen copy is drawn first.
void fn_80038A90(f32* pColour, u8 bCopy, int nView, int nField, f32 fCX, f32 fCY) {
    f32 aXY[18][4];
    f32 aUV[18][4];
    f32 aColour[18][4];
    RenderCamera* pCamera;
    f32* pRect;
    f32 fX0;
    f32 fX1;
    f32 fY0;
    f32 fY1;
    f32 fMaxDist;
    f32 fDXSq;
    f32 fDYSq;
    f32 fAlpha;

    pCamera = fn_80017004(nView);
    pRect = pCamera->pRect;
    if (bCopy) {
        fn_800390CC(nField, pCamera);
    }
    fX0 = pRect[0];
    fY0 = pRect[1];
    fX1 = fX0 + pRect[2];
    fY1 = fY0 + pRect[3];
    fCX = fCX * pRect[2] + pRect[0];
    fCY = fCY * pRect[3] + pRect[1];
    fn_80012F34(0);
    if (bCopy) {
        fn_80014118(0x50);
    } else {
        fn_80014118(0x40);
    }
    fn_80035118(4, 5);
    fn_80012F50(0, 1, 0x80);
    fn_80012F18(7);
    fn_8002A608(&lbl_801D4F80);
    fn_8001425C(0);
    fn_80012EF8();

    // The distance to the farthest corner of the screen.
    if (fCX < 0.5f) {
        if (fCY < 0.5f) {
            fMaxDist = fn_80009680((1.0f - fCY) * (1.0f - fCY) + (1.0f - fCX) * (1.0f - fCX));
        } else {
            fMaxDist = fn_80009680(fCY * fCY + (1.0f - fCX) * (1.0f - fCX));
        }
    } else if (fCY < 0.5f) {
        fMaxDist = fn_80009680(fCX * fCX + (1.0f - fCY) * (1.0f - fCY));
    } else {
        fMaxDist = fn_80009680(fCY * fCY + fCX * fCX);
    }

    aXY[0][0] = fCX;
    aXY[0][1] = fCY;
    aXY[0][2] = 1.0f;
    aXY[0][3] = 1.0f;
    aUV[0][0] = fCX;
    aUV[0][1] = 224.0f * fCY * (1.0f / 128.0f);
    aUV[0][2] = 1.0f;
    aUV[0][3] = 1.0f;
    aColour[0][0] = pColour[0];
    aColour[0][1] = pColour[1];
    aColour[0][2] = pColour[2];
    aColour[0][3] = 0.0f;
    fn_80038E7C(aXY[1], aColour[1], aUV[1], 4, pColour, fCX, fCY, fX0, fY0, fX1, fY0, fMaxDist);
    fn_80038E7C(aXY[5], aColour[5], aUV[5], 4, pColour, fCX, fCY, fX1, fY0, fX1, fY1, fMaxDist);
    fn_80038E7C(aXY[9], aColour[9], aUV[9], 4, pColour, fCX, fCY, fX1, fY1, fX0, fY1, fMaxDist);
    fn_80038E7C(aXY[13], aColour[13], aUV[13], 4, pColour, fCX, fCY, fX0, fY1, fX0, fY0, fMaxDist);

    // The fan closes at the first corner.
    fDXSq = (fX0 - fCX) * (fX0 - fCX);
    fDYSq = (fY0 - fCY) * (fY0 - fCY);
    aXY[17][0] = fX0;
    aXY[17][1] = fY0;
    aXY[17][2] = 1.0f;
    aXY[17][3] = 1.0f;
    aUV[17][0] = fX0;
    aUV[17][1] = 224.0f * fY0 * (1.0f / 128.0f);
    aUV[17][2] = 1.0f;
    aUV[17][3] = 1.0f;
    fAlpha = pColour[3] * ((f32)fn_80009680(fDXSq + fDYSq) / fMaxDist);
    fAlpha = (fAlpha < 0.0f) ? 0.0f : ((fAlpha > pColour[3]) ? pColour[3] : fAlpha);
    aColour[17][0] = pColour[0];
    aColour[17][1] = pColour[1];
    aColour[17][2] = pColour[2];
    aColour[17][3] = fAlpha;
    fn_8001644C(0xA0, aXY[0], aColour[0], aUV[0], 18);
    fn_80012F34(1);
    fn_80012F50(1, 6, 0x80);
    fn_80012F18(3);
    fn_80012EF8();
}

// Fills n vertices of a fan edge running from (fX0, fY0) towards (fX1, fY1): each gets pSrc's
// colour, with its alpha scaled by the vertex's distance from (fCX, fCY) over fMaxDist (at most
// pSrc's own alpha), and texture coordinates in the 256 x 128 screen copy.
void fn_80038E7C(f32* pXY, f32* pColour, f32* pUV, int n, f32* pSrc, f32 fCX, f32 fCY, f32 fX0,
                 f32 fY0, f32 fX1, f32 fY1, f32 fMaxDist) {
    int i;
    f32 fX;
    f32 fY;
    f32 fDXSq;
    f32 fDYSq;
    f32 fAlpha;

    for (i = 0; i < n; i++) {
        fX = (f32)i * ((fX1 - fX0) / (f32)n) + fX0;
        fY = (f32)i * ((fY1 - fY0) / (f32)n) + fY0;
        fDXSq = (fX - fCX) * (fX - fCX);
        fDYSq = (fY - fCY) * (fY - fCY);
        fAlpha = pSrc[3] * ((f32)fn_80009680(fDXSq + fDYSq) / fMaxDist);
        fAlpha = (fAlpha < 0.0f) ? 0.0f : ((fAlpha > pSrc[3]) ? pSrc[3] : fAlpha);
        pXY[0] = fX;
        pXY[1] = fY;
        pXY[2] = 1.0f;
        pXY[3] = 1.0f;
        pXY += 4;
        pUV[0] = fX;
        pUV[1] = 224.0f * fY * (1.0f / 128.0f);
        pUV[2] = 1.0f;
        pUV[3] = 1.0f;
        pUV += 4;
        pColour[0] = pSrc[0];
        pColour[1] = pSrc[1];
        pColour[2] = pSrc[2];
        pColour[3] = fAlpha;
        pColour += 4;
    }
}

// Draws the effect's screen copy (nField picks which of the two) at half size into pCamera's
// rectangle, stretched to twice its height.
void fn_800390CC(int nField, RenderCamera* pCamera) {
    f32 aColour[4];
    f32 aXY[8];
    f32 aUV[8];
    f32* pRect;
    f32 fX;
    f32 fY;
    f32 fRight;
    f32 fBottom;

    pRect = pCamera->pRect;
    fX = pRect[0];
    fY = pRect[1];
    fRight = pRect[0] + pRect[2];
    fBottom = pRect[1] + pRect[3];
    fn_80012F34(0);
    fn_80014118(0x10);
    fn_80012F50(0, 1, 0x80);
    fn_80012F18(7);
    fn_8002A608(nField ? &lbl_801D4FB0[1] : &lbl_801D4FB0[0]);
    fn_800140E8(1, 256, 224, 0, 1, 1);
    fn_80016B54(256, 128, 1.0f, 1.0f);
    fn_80016978(0.0f, 0.0f, 1.0f, 1.0f);
    fn_800169AC();
    fn_800141F8(aXY, NULL, fX, fY, fRight, 2.0f * fBottom);
    aUV[0] = aXY[0];
    aUV[1] = aXY[1];
    aUV[4] = aXY[4];
    aUV[5] = aXY[5];
    aColour[0] = 0.5f;
    aColour[1] = 0.5f;
    aColour[2] = 0.5f;
    aColour[3] = 1.0f;
    fn_80014194(aColour);
    fn_8001425C(0);
    fn_80012EF8();
    fn_8001644C(0xA1, aXY, NULL, aUV, 2);
    fn_800140E8(0, 512, 448, lbl_80281B88 & 1, 8, 1);
    fn_80012F34(1);
    fn_80012F50(1, 6, 0x80);
    fn_80012F18(3);
    fn_80016B54(512, 448, 1.0f, 1.0f);
    fn_80035F40(pCamera);
    fn_80012EF8();
}

void fn_80039344(int nView, f32 f) {
    lbl_801D5010[nView] = f;
}

// Covers the screen in black at view nView's share (lbl_801D5010) of CamTuning.f208's alpha.
void fn_80039358(int nView) {
    f32 aColour[4];
    f32 aXY[16];    // only 8 are filled; the frame has room for 16
    f32 f;

    f = lbl_801D5010[nView];
    if (f) {
        aColour[0] = 0.0f;
        aColour[1] = 0.0f;
        aColour[2] = 0.0f;
        aColour[3] = lbl_80281F78->f208 * f;
        fn_8001425C(0);
        fn_80014118(0x40);
        fn_80035118(4, 5);
        fn_80012F34(0);
        fn_80012F18(7);
        fn_80012F50(0, 6, 0x80);
        fn_80014194(aColour);
        fn_80012EF8();
        fn_800141F8(aXY, NULL, 0.0f, 0.0f, 1.0f, 1.0f);
        fn_8001644C(0xA1, aXY, NULL, NULL, 2);
        fn_80012F34(1);
        fn_80012F18(3);
        fn_80012F50(1, 6, 0x80);
        fn_80012EF8();
    }
}
