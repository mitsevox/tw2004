// GxUtil.c (our name): graphics helpers. A 256 x 224 copy of the screen used as a texture (made at
// start-up, filled by fn_8002A164, drawn through TEV stage 0), the alpha clear of a rectangle, and
// fn_8002A528, which fills in a texture object for GoPostFx, DepthField and the front end.

#include "engine.h"
#include "gx.h"

GxTexture lbl_801C64A8;         // the screen copy
s32 lbl_80281D2C;               // fn_8002A164's mode: 0, or blend by the copy's alpha
u8  lbl_80281D28;               // the screen copy is set up for drawing

void fn_80029FC8(void) {
    fn_8002A528(&lbl_801C64A8, 256, 224, fn_8002A624(), NULL, 1, 0, 0, 0);
    lbl_80281D28 = 0;
}

void fn_8002A020(void) {
}

// With bOn, clear the alpha of the rectangle (x0, y0)-(x1, y1) (colour writes off meanwhile);
// always leaves alpha writes on.
void fn_8002A024(u8 bOn, f32 x0, f32 y0, f32 x1, f32 y1) {
    f32 xy[8];
    f32 colour[4];

    if (bOn) {
        GXSetAlphaUpdate(1);
        GXSetColorUpdate(0);
        fn_80012F34(0);
        fn_80012F50(0, 6, 0x80);
        fn_80012F18(7);
        fn_80014118(0);
        fn_800141F8(xy, NULL, x0, y0, x1, y1);
        colour[0] = 0.0f;
        colour[1] = 0.0f;
        colour[2] = 0.0f;
        colour[3] = 0.0f;
        fn_80014194(colour);
        fn_8001425C(0);
        fn_80012EF8();
        fn_8001644C(0xA1, xy, 0, NULL, 2);
        fn_80012F34(1);
        fn_80012F50(1, 6, 0x80);
        fn_80012F18(3);
        GXSetColorUpdate(1);
        fn_80012EF8();
    } else {
        GXSetAlphaUpdate(1);
    }
}

// Copy the screen (512 x 448) into the half-size texture and draw with it through TEV stage 0.
void fn_8002A164(int nMode) {
    GXSetTexCopySrc(0, 0, 512, 448);
    GXSetTexCopyDst(256, 224, 0x27, 1);
    GXCopyTex(fn_8002A624(), 0);
    GXPixModeSync();
    fn_8002A608(&lbl_801C64A8);
    fn_80012F50(1, 4, 0);
    fn_80012EF8();
    GXSetNumTevStages(1);
    if (nMode == 0) {
        GXSetTevOrder(0, 0, 0, 4);
        GXSetTevColorIn(0, 15, 15, 15, 12);
        GXSetTevColorOp(0, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn(0, 7, 7, 7, 4);
        GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    } else {
        GXSetTevOrder(0, 0, 0, 4);
        GXSetTevColorIn(0, 15, 15, 15, 12);
        GXSetTevColorOp(0, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn(0, 2, 7, 4, 7);
        GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    }
    lbl_80281D2C = nMode;
    lbl_80281D28 = 1;
}

// The end of fn_8002A164's drawing.
void fn_8002A2FC(void) {
    GXSetAlphaUpdate(0);
    fn_80012F50(1, 6, 0x80);
    fn_80012EF8();
    lbl_80281D28 = 0;
    GXSetTevColorIn(1, 15, 0, 12, 15);
    GXSetTevColorOp(1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(1, 7, 4, 5, 7);
    GXSetTevAlphaOp(1, 0, 0, 1, 1, 0);
}

u8 fn_8002A3A4(void) {
    return lbl_80281D28;
}

void fn_8002A3AC(u8 b) {
    GXColor colour = {0, 0, 0, 255};

    GXSetTevColor(2, colour);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevColorIn(0, 15, 15, 15, 12);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    if (lbl_80281D2C == 0) {
        if (b) {
            GXSetTevAlphaIn(0, 7, 4, 6, 7);
            GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
        } else {
            GXSetTevAlphaIn(0, 7, 7, 7, 4);
            GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
        }
    } else {
        if (b) {
            GXSetTevAlphaIn(0, 6, 7, 4, 7);
            GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
        } else {
            GXSetTevAlphaIn(0, 2, 7, 4, 7);
            GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
        }
    }
}

void fn_8002A528(GxTexture* pTex, int nWidth, int nHeight, void* pImage, void* pLut, int eFormat,
                 int eLutFormat, int eWrapS, int eWrapT) {
    u16 nEntries;

    if (pLut != NULL) {
        GXInitTexObjCI(&pTex->tex, pImage, nWidth, nHeight, eFormat, eWrapS, eWrapT, 0, 0);
        // the palette's size by format: C4 16 entries, C8 256
        if (eFormat == 8) {
            nEntries = 16;
        } else if (eFormat == 9) {
            nEntries = 256;
        } else if (eFormat == 9) {  // EA bug: tests C8 again, so the 128 is never used
            nEntries = 128;
        } else {
            nEntries = 256;
        }
        GXInitTlutObj(&pTex->tlut, pLut, eLutFormat, nEntries);
        pTex->bPalette = 1;
    } else {
        GXInitTexObj(&pTex->tex, pImage, nWidth, nHeight, eFormat, eWrapS, eWrapT, 0);
        pTex->bPalette = 0;
    }
}

void fn_8002A608(GxTexture* pTex) {
    lbl_801B8980.pTex108 = pTex;
    lbl_801B8980.uFlags |= 2;
}

void* fn_8002A624(void) {
    return *lbl_80281100;
}
