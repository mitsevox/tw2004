// SunFlr_Gc.c (EA's name, from its asserts): the sun flare's GameCube part: per view, a copy of
// the depth around the sun and, from it, how much of the sun is visible (fn_8009A754).

#include "glows.h"
#include "camera.h"
#include "core/startup.h"

// Sets lbl_802813B8->f0 so that all of lbl_80189DA8's weights together make 1.
void fn_8009A250(void) {
    int y;
    int x;
    f32 fSum;

    fSum = 0.0f;
    for (y = 0; y < 6; y++) {
        for (x = 0; x < 8; x++) {
            fSum += lbl_80189DA8[y][x];
        }
    }
    lbl_802813B8->f0 = 1.0f / fSum;
}

// ---- sweep code (not yet cleaned up) ----

void fn_8009A340(void);
void fn_8009A344(s32 nView, SunFlrView* pView);
void fn_8009A3D0(s32 nView, SunFlrView* pView);
void fn_8009A704(s32 nView);
void fn_8009A844(void* pCamera, u8* pIn, u8* pOut);

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

// ---- end of sweep code ----

// Copies the depth buffer around the sun into view nView's part for the field three ahead: an 8 x 6
// window of pixels centred on the sun, clipped to the view, in a 12 x 12 Z24X8 texture. u18 is the
// sun's own depth, so fn_8009A754 later counts the pixels nothing is drawn in front of.
void fn_8009A3F4(s32 nView) {
    SunFlrView* pView;
    SunFlrPart* pPart;
    void* pCamera;
    int iPart;
    f32 v[4];
    int nSunX;
    int nSunY;
    int nLeft;
    int nTop;
    int nRight;
    int nBottom;
    int nX;
    int nY;
    int nEndX;
    int nEndY;
    int nCopyX;
    int nCopyY;
    int nReadX;
    int nReadY;
    int nWeightX;
    int nWeightY;
    int nWidth;
    int nHeight;

    pView = &lbl_802813B8->aView[nView];
    iPart = (lbl_80281B88 + 3) % 4;
    pCamera = fn_80017004(nView);

    // The sun on the screen, kept within +-500000.
    v[0] = pView->f98;
    v[1] = pView->f9C;
    v[2] = pView->fA0;
    v[3] = 1.0f;
    fn_8009A844(pCamera, (u8*)v, (u8*)v);
    if (v[0] < -500000.0f || v[0] > 500000.0f) {
        v[0] = 500000.0f;
    }
    if (v[1] < -500000.0f || v[1] > 500000.0f) {
        v[1] = 500000.0f;
    }
    nSunX = v[0];
    nSunY = v[1];

    // The view's corners on the screen.
    v[0] = 0.0f;
    v[1] = 0.0f;
    fn_8009A844(pCamera, (u8*)v, (u8*)v);
    nLeft = v[0];
    nTop = v[1];
    v[0] = 1.0f;
    v[1] = 1.0f;
    fn_8009A844(pCamera, (u8*)v, (u8*)v);
    nRight = v[0];
    nBottom = v[1];

    if (nLeft < 0) {
        nLeft = 0;
    }
    if (nTop < 0) {
        nTop = 0;
    }
    if (nRight > 512) {
        nBottom = 512; // EA bug: clamps nBottom where nRight is meant
    }
    if (nBottom > 448) {
        nBottom = 448;
    }

    // Columns: the copy starts on a multiple of 4 and stays inside the 512-wide screen.
    nX = nSunX - 4;
    nY = nSunY - 3;
    nEndX = nX + 8;
    nEndY = nY + 6;
    if (nX < nLeft) {
        nCopyX = nLeft & ~3;
        nWeightX = nLeft - nX;
        nReadX = nLeft - nCopyX;
        nWidth = 8 - nWeightX;
    } else if (nEndX >= 512) {
        nReadX = nX - 500;
        nCopyX = 500;
        nWeightX = 0;
        nWidth = 8;
    } else {
        nCopyX = nX & ~3;
        nWeightX = 0;
        nReadX = nX - nCopyX;
        nWidth = 8;
    }
    if (nEndX > nRight) {
        nWidth -= nEndX - nRight;
    }

    // Rows, the same within the 448-high screen.
    if (nY < nTop) {
        nCopyY = nTop & ~3;
        nWeightY = nTop - nY;
        nReadY = nTop - nCopyY;
        nHeight = 6 - nWeightY;
    } else if (nEndY >= 448) {
        nReadY = nY - 436;
        nCopyY = 436;
        nWeightY = 0;
        nHeight = 6;
    } else {
        nCopyY = nY & ~3;
        nWeightY = 0;
        nReadY = nY - nCopyY;
        nHeight = 6;
    }
    if (nEndY > nBottom) {
        nHeight -= nEndY - nBottom;
    }

    pPart = &pView->aPart[iPart];
    pPart->n8 = nWeightX;
    pPart->nC = nWeightY;
    pPart->n0 = nReadX;
    pPart->n4 = nReadY;
    pPart->n10 = nWidth;
    pPart->n14 = nHeight;

    // The sun's depth as a 24-bit Z value.
    pPart->u18 = 16777216.0f * (1.0f - pView->fA0);
    if (pPart->u18 & 0x80000000) {
        pPart->u18 = 0;
    }
    if (pPart->u18 & 0x7F000000) {
        pPart->u18 = 0x0FFFFFFF;
    }

    if (nHeight > 0 && nWidth > 0) {
        DCFlushRange(pPart->p1C, pView->nC);
        GXSetTexCopySrc(nCopyX, nCopyY, 12, 12);
        GXSetTexCopyDst(12, 12, 0x16, 0); // 0x16: GX's Z24X8 format
        GXCopyTex(pPart->p1C, 0);
    }
}

// ---- sweep code (not yet cleaned up) ----

// Does nothing (the view number is not used).
void fn_8009A704(s32 nView) {
}

// ---- end of sweep code ----

// The colour (0xRRGGBB) of pixel (x, y) of a 12 x 12 RGBA8 image: GX's 4 x 4 tiles of 64 bytes,
// the alpha and red pairs first, then the green and blue pairs.
u32 fn_8009A708(u8* pImage, int x, int y) {
    u8* p;
    u32 uColour;

    p = &pImage[(x / 4 + y / 4 * 3) * 64 + (y & 3) * 8 + (x & 3) * 2];
    uColour = (p[1] << 16) | (p[0x20] << 8);
    return uColour | p[0x21];
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
