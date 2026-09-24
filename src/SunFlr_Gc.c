// SunFlr_Gc.c (EA's name, from its asserts): not yet decompiled; the sweep code below is the
// matched small functions.

#include "glows.h"

// ---- sweep code (not yet cleaned up) ----

void fn_8009A340(void);
void fn_8009A344(s32 nView, SunFlrView* pView);
void fn_8009A3D0(s32 nView, SunFlrView* pView);
void fn_8009A704(s32 nView);

void fn_8009A340(void) {
}

// Sets up view nView's part (the view number is not used): a buffer of four 576-byte parts.
void fn_8009A344(s32 nView, SunFlrView* pView) {
    memset(pView->aPart, 0, 2 * sizeof(pView->aPart[0]));
    pView->nC = 576;
    pView->pBuffer = fn_80009B34(4 * 576, 2, 32, "SunFlr_Gc.c", 110);
    pView->aPart[0].p1C = pView->pBuffer;
    pView->aPart[1].p1C = pView->pBuffer + 576;
    pView->aPart[2].p1C = pView->pBuffer + 2 * 576;
    pView->aPart[3].p1C = pView->pBuffer + 3 * 576;
}

// Frees view nView's buffer (the view number is not used).
void fn_8009A3D0(s32 nView, SunFlrView* pView) {
    fn_80009E70(pView->pBuffer);
}

// Does nothing (the view number is not used).
void fn_8009A704(s32 nView) {
}

// ---- end of sweep code ----

// The colour (0xRRGGBB) of pixel (x, y) of a 12 x 12 RGBA8 image: GX's 4 x 4 tiles of 64 bytes,
// the alpha and red pairs first, then the green and blue pairs.
u32 fn_8009A708(u8* pImage, int x, int y) {
    u8* p;

    p = pImage + (x / 4 + y / 4 * 3) * 64;
    p += (y & 3) * 8;
    p += (x & 3) * 2;
    return (p[1] << 16) | (p[0x20] << 8) | p[0x21];
}

// View nView's part for the field being drawn: the weights of the pixels read whose colour is
// above the part's u18, added up and scaled by lbl_802813B8->f0.
f32 fn_8009A754(s32 nView, SunFlrView* pView) {
    int y;
    int x;
    f32 fSum;
    SunFlrPart* pPart;
    u8* pImage;
    u32 uColour;

    fSum = 0.0f;
    pPart = &pView->aPart[lbl_80281B88 % 4];
    pImage = pPart->p1C;
    for (y = 0; y < pPart->n14; y++) {
        for (x = 0; x < pPart->n10; x++) {
            uColour = fn_8009A708(pImage, x + pPart->n0, y + pPart->n4);
            if (uColour > pPart->u18) {
                fSum += lbl_80189DA8[y + pPart->nC][x + pPart->n8];
            }
        }
    }
    fSum *= lbl_802813B8->f0;
    return fSum;
}
