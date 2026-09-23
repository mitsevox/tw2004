// GoCamCont.c (TW06's golf/cameras/gocamcont.c; our spelling): the camera controller of each view
// (View, TW06's CameraController): picking the camera mode (View_SetCamera), its idle state and the
// small setters and tests the camera code uses. Not decompiled yet beyond the functions below.

#include "engine.h"
#include "camera.h"

void fn_800642A4(View* pView, f32 f174, f32 f178);

// Forgets the view's camera sequences and shot.
void fn_80062F1C(View* pView) {
    pView->p78 = NULL;
    pView->p7C = NULL;
    pView->p80 = NULL;
    pView->p74 = NULL;
    pView->b268 = 0;
}

u8 fn_80063C7C(View* pView) {
    return pView->nCamera == 4;
}

void fn_80063CBC(View* pView, f32* pVec) {
    pView->nCamera = 3;
    Vec_Copy(pVec, pView->vC4);
}

void fn_800642A4(View* pView, f32 f174, f32 f178) {
    pView->f174 = f174;
    pView->f178 = f178;
}

void fn_800642B0(void) {
    fn_800C6CB0();
}
