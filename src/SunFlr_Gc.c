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
