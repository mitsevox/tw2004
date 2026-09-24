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
void fn_80038A90(f32* pV, u8 b, int nView, int nField, f32 f14, f32 f18);
void fn_80038724(int nField, int nView, f32 f8, f32 f4);
void fn_800A6070(u8 nPlayer, u8 bLimit);      // GameAudio.c
void fn_80016948(void);

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

void fn_800380A8(u8 b, f32* pV, u8 b1, int nView, f32 f14, f32 f18) {
    lbl_801D5020[nView].b0 = b;
    Vec_Copy(pV, lbl_801D5020[nView].v4);
    lbl_801D5020[nView].b1 = b1;
    lbl_801D5020[nView].f14 = f14;
    lbl_801D5020[nView].f18 = f18;
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
            fn_80038A90(lbl_801D5020[i].v4, lbl_801D5020[i].b1, i, lbl_80281B88 & 1,
                        lbl_801D5020[i].f14, lbl_801D5020[i].f18);
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
    fRight = fX + pRect[2];
    fBottom = fY + pRect[3];
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
    aXY[4] += fDX;
    aXY[1] += fDY;
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

void fn_80039344(int nView, f32 f) {
    lbl_801D5010[nView] = f;
}

// Covers the screen in black at view nView's share (lbl_801D5010) of CamTuning.f208's alpha.
void fn_80039358(int nView) {
    f32 aColour[4];
    f32 aXY[8];
    f32 f = lbl_801D5010[nView];

    if (f != 0.0f) {
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
