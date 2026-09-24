// DepthField.c (TW06's golf/sfx/depthfield.c, its DF_ functions; our spelling): the depth-of-field
// blur. It copies the screen into a half-size texture and draws it back over the scene
// (DF_vDrawBufferToScreen). Its state is lbl_801D5110; not decompiled yet beyond the functions below.

#include "engine.h"
#include "gx.h"
#include "camera.h"
#include "terrain.h"

void fn_800140E8(int a, int nWidth, int nHeight, int nField, int b, int c);
void fn_80016B54(int nWidth, int nHeight, f32 fX, f32 fY);
void fn_80035F1C(void);

void DF_vDrawBufferToScreen(int n);
void fn_80045880(int n);
void fn_800458B4(void);

// Sets up the five layers (off, grey) and the textures of the image buffers and the screen copy.
void fn_80045660(void) {
    lbl_80281D94 = 0.0f;
    lbl_80281110->p8 = fn_8002A624();
    lbl_801D5110[0].b0 = 0;
    lbl_801D5110[0].f4 = 0.0f;
    lbl_801D5110[0].f14 = 1.0f;
    lbl_801D5110[0].aColour[0] = 0.5f;
    lbl_801D5110[0].aColour[1] = 0.5f;
    lbl_801D5110[0].aColour[2] = 0.5f;
    lbl_801D5110[1].b0 = 0;
    lbl_801D5110[1].f4 = 0.0f;
    lbl_801D5110[1].f14 = 1.0f;
    lbl_801D5110[1].aColour[0] = 0.5f;
    lbl_801D5110[1].aColour[1] = 0.5f;
    lbl_801D5110[1].aColour[2] = 0.5f;
    lbl_801D5110[2].b0 = 0;
    lbl_801D5110[2].f4 = 0.0f;
    lbl_801D5110[2].f14 = 1.0f;
    lbl_801D5110[2].aColour[0] = 0.5f;
    lbl_801D5110[2].aColour[1] = 0.5f;
    lbl_801D5110[2].aColour[2] = 0.5f;
    lbl_801D5110[3].b0 = 0;
    lbl_801D5110[3].f4 = 0.0f;
    lbl_801D5110[3].f14 = 1.0f;
    lbl_801D5110[3].aColour[0] = 0.5f;
    lbl_801D5110[3].aColour[1] = 0.5f;
    lbl_801D5110[3].aColour[2] = 0.5f;
    lbl_801D5110[4].b0 = 0;
    lbl_801D5110[4].f4 = 0.0f;
    lbl_801D5110[4].f14 = 1.0f;
    lbl_801D5110[4].aColour[0] = 0.5f;
    lbl_801D5110[4].aColour[1] = 0.5f;
    lbl_801D5110[4].aColour[2] = 0.5f;
    fn_8002A528(&lbl_801D51C8[0], 512, 448, lbl_80281BA4[0], NULL, 6, 0, 0, 0);
    fn_8002A528(&lbl_801D51C8[1], 512, 448, lbl_80281BA4[1], NULL, 6, 0, 0, 0);
    fn_8002A528(&lbl_801D5198, 256, 224, lbl_80281110->p8, NULL, 4, 0, 0, 0);
}

// Turns layer n on with an amount clamped to 0..1 (off at 0).
void fn_800457B8(int n, f32 fAmount) {
    if (fAmount > 1.0f) {
        fAmount = 1.0f;
    }
    if (fAmount < 0.0f) {
        fAmount = 0.0f;
    }
    lbl_801D5110[n].f4 = 0.8f;
    if (0.0f == fAmount) {
        lbl_801D5110[n].b0 = 0;
    } else {
        lbl_801D5110[n].b0 = 1;
    }
    lbl_801D5110[n].f14 = fAmount;
}

// Turns layer n off.
void fn_80045824(int n) {
    lbl_801D5110[n].f4 = 0.0f;
    lbl_801D5110[n].b0 = 0;
}

// Draws layer n if it is on.
void fn_80045848(int n) {
    if (lbl_801D5110[n].b0) {
        fn_80045880(n);
    }
}

void fn_80045880(int n) {
    fn_800458B4();
    DF_vDrawBufferToScreen(n);
}

// Copies the 512 x 448 screen into the 256 x 224 screen-copy texture.
void fn_800458B4(void) {
    GXSetTexCopySrc(0, 0, 512, 448);
    GXSetTexCopyDst(256, 224, 4, 1);
    GXCopyTex(fn_8002A624(), 0);
    GXInvalidateTexAll();
}

// Clamps f to fLo..fHi (our helper). fake match: the f64 parameter gives the original's frsp before
// the compares; 97.9%, the original reloads fHi instead of reusing the loop's 1.0 register.
static inline f32 DF_Clamp(f64 f, f32 fLo, f32 fHi) {
    return ((f32)f < fLo) ? fLo : (((f32)f > fHi) ? fHi : (f32)f);
}

// Draws the screen copy back over the camera's rectangle five times in layer n's colour, each
// pass shifted less than the one before (alternately left and right) and fainter.
void DF_vDrawBufferToScreen(int n) {
    int i;
    int bOdd;
    f32 aXY[8];
    f32 aUV[8];
    f32 aColour[4];
    f32* pRect;
    f32 fX0;
    f32 fY0;
    f32 fX1;
    f32 fY1;
    f32 fOffset;
    f32 fZ;

    aColour[0] = lbl_801D5110[n].aColour[0];
    aColour[1] = lbl_801D5110[n].aColour[1];
    aColour[2] = lbl_801D5110[n].aColour[2];
    aColour[3] = lbl_801D5110[n].f4 * lbl_801D5110[n].f14;
    pRect = ((RenderCamera*)fn_8001614C())->pRect;
    fX0 = pRect[0];
    fY0 = pRect[1];
    fX1 = fX0 + pRect[2];
    fY1 = fY0 + pRect[3];
    fn_8002A608(&lbl_801D5198);
    fn_8001425C(0);
    fn_80014118(0x50);
    fn_80012F34(0);
    fn_80035118(4, 5);
    fn_80035098(1);
    fn_80012F50(0, 6, 0x80);
    for (i = 0; i < 5; i++) {
        fn_800141F8(aXY, aUV, 0.0f, 0.0f, 1.0f, 1.0f);
        bOdd = i % 2;
        aUV[0] = fX0;
        fOffset = lbl_80281114 * ((f32)(5 - i) / (lbl_8028111C * (f32)(i + 1)));
        aUV[1] = fY0;
        aUV[4] = fX1;
        aUV[5] = fY1;
        fZ = 1.0f - ((f32)i * ((f32)i * (lbl_80281118 * (f32)i)) + lbl_80281D90);

        aXY[2] = fZ;
        aXY[2] = DF_Clamp(aXY[2], 0.0f, 1.0f);
        if (bOdd) {
            aXY[0] += fOffset;
            aXY[1] -= fOffset;
        } else {
            aXY[0] -= fOffset;
            aXY[1] -= fOffset;
        }
        aXY[6] = fZ;
        aXY[6] = DF_Clamp(aXY[6], 0.0f, 1.0f);
        if (bOdd) {
            aXY[4] += fOffset;
            aXY[5] -= fOffset;
        } else {
            aXY[4] -= fOffset;
            aXY[5] -= fOffset;
        }

        aColour[0] = lbl_801D5110[n].aColour[0];
        aColour[1] = lbl_801D5110[n].aColour[1];
        aColour[2] = lbl_801D5110[n].aColour[2];
        aColour[3] = lbl_801D5110[n].f4 * lbl_801D5110[n].f14 / (lbl_80281120[0] * (f32)(i + 1));
        fn_80014194(aColour);
        fn_80035154(255.0f * aColour[3]);
        fn_80012EF8();
        fn_8001644C(0xA1, aXY, NULL, aUV, 2);
    }
    fn_80012F34(1);
    fn_800140E8(0, 512, 448, lbl_80281B88 & 1, 8, 1);
    fn_80016B54(512, 448, 1.0f, 1.0f);
    fn_80035F1C();
    fn_80012F50(1, 6, 0x80);
    fn_80035098(0);
    fn_80012EF8();
}
