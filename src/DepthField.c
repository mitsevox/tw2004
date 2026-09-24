// DepthField.c (TW06's golf/sfx/depthfield.c, its DF_ functions; our spelling): the depth-of-field
// blur. It copies the screen into a half-size texture and draws it back over the scene
// (DF_vDrawBufferToScreen). Its state is lbl_801D5110; not decompiled yet beyond the functions below.

#include "engine.h"
#include "gx.h"

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
