// ViewController.c (our name, after TW06's viewControllerID): the four views on screen
// (lbl_801B8BA8). Each view controller owns a render camera and the view's camera controller
// (View); these functions set a view up and shut it down, hand out its parts, and each frame run
// its camera controller and copy the camera's position into the render camera.

#include "unsorted/cull.h"

// .bss/.sbss in reverse address order (CodeWarrior lays them out last-defined-first)
ViewController lbl_801B8BA8[4];
ViewController* lbl_80281CA4;               // the current view's controller
int lbl_80281CA0;                           // the current view

void  fn_80062E40(View* pView);                     // set up a camera controller
void  fn_80038010(u8 a, int n, f32* pVec);
void  fn_80038054(u8 a, int n, f32 f1, f32 f2);
void  CameraController_Idle(View* pView, int nPlayer);
void  fn_8007656C(CamLens* pLens, f32* pPos, f32* pAt, f32* pUp);
void  fn_80076664(CamLens* pLens, f32* pPos, f32* pAt, f32* pF5C, f32* pF50);
void  fn_80017208(CamLens* pLens, f32* pPos, f32* pAngles);
void  fn_80013D68(void* pCamera);
void  fn_80076A54(f32* pRect);
void  fn_8000A194(f32 (*pMtx)[4], f32 fA, f32 fB, f32 fC);  // UMemPool.c: a rotation matrix from three angles
void  fn_8000A798(f32 (*pSrc)[4], f32 (*pDst)[4]);          // UMemPool.c: inverts a rotation+translation
void  fn_8001728C(CamLens* pLens);

ViewController* fn_80016E28(int nView);
s32  fn_800171B0(void);
f32* fn_800172B4(View* pView);
f32* fn_800172BC(View* pView);

void fn_80016CB8(void) {
    int i;

    for (i = 0; i < 4; i++) {
        lbl_801B8BA8[i].b274 = 0;
    }
}

void fn_80016CD8(int nView) {
    lbl_80281CA0 = nView;
    lbl_80281CA4 = &lbl_801B8BA8[nView];
}

ViewController* fn_80016CF4(void) {
    return lbl_80281CA4;
}

ViewController* fn_80016CFC(int nView) {
    return &lbl_801B8BA8[nView];
}

int fn_80016D10(void) {
    return lbl_80281CA0;
}

// Sets a view up on the screen rectangle x, y, w, h (fractions of the screen).
void fn_80016D18(int nView, f32 x, f32 y, f32 w, f32 h) {
    ViewController* pCtrl;
    CamLens* pLens;
    f32* pRect;

    pCtrl = fn_80016E28(nView);
    pLens = fn_80076400();
    pRect = fn_80076ACC();
    fn_800171D8(pRect, x, y, w, h);
    // port: fn_800171B0 is typed s32, but its value is the frame buffer
    pCtrl->pCamera = fn_8001371C(pLens, (GoFrameBuf*)fn_800171B0(), pRect);
    fn_80062E40(&pCtrl->view);
    pCtrl->nPlayer = 5;
    pCtrl->b274 = 1;
    fn_80038010(0, nView, NULL);
    fn_80038054(0, nView, 0.0f, 0.0f);
}

ViewController* fn_80016E28(int nView) {
    return &lbl_801B8BA8[nView];
}

// Shuts a view down: frees its render camera's lens and screen rectangle, then the camera.
void fn_80016E3C(int nView) {
    ViewController* pCtrl;

    pCtrl = fn_80016E28(nView);
    fn_8007644C(fn_80008370(pCtrl->pCamera));
    fn_80076B18(fn_80012EF0(pCtrl->pCamera));
    fn_800137B0(pCtrl->pCamera);
    pCtrl->b274 = 0;
}

// Runs the view's camera controller and moves the render camera to where it puts the camera.
void fn_80016E90(int nView) {
    void* pCamera;
    View* pView;

    pCamera = fn_80017004(nView);
    pView = fn_80017028(nView);
    CameraController_Idle(pView, fn_8001707C(nView));
    if (pView->nCurCamera == 2) {
        fn_8007656C(fn_80008370(pCamera), fn_8001731C(pView), fn_80017314(pView), pView->v20);
    } else if (fn_800172C4(pView)) {
        if (pView->nCurCamera == 4) {
            fn_80076664(fn_80008370(pCamera), fn_8001731C(pView), fn_80017314(pView),
                        fn_800172BC(pView), fn_800172B4(pView));
        } else {
            fn_8007656C(fn_80008370(pCamera), fn_8001731C(pView), fn_80017314(pView), pView->v20);
        }
    } else {
        fn_80017208(fn_80008370(pCamera), fn_8001731C(pView), fn_80017314(pView));
    }
    fn_80013D68(pCamera);
    fn_80013CCC(pCamera);
}

void* fn_80017004(int nView) {
    return fn_80016E28(nView)->pCamera;
}

View* fn_80017028(int nView) {
    return &fn_80016E28(nView)->view;
}

void fn_8001704C(int nView, int nPlayer) {
    fn_80016E28(nView)->nPlayer = nPlayer;
}

int fn_8001707C(int nView) {
    return fn_80016E28(nView)->nPlayer;
}

u8 fn_800170A0(int nView) {
    return fn_80016E28(nView)->b274;
}

void fn_800170C4(int nView, u8 b) {
    fn_80016E28(nView)->b274 = b;
}

// Saves the render camera's screen rectangle (fn_80017158 puts it back).
void fn_800170F4(int nView) {
    ViewController* pCtrl;
    f32* pRect;

    pCtrl = fn_80016E28(nView);
    pRect = fn_80012EF0(fn_80017004(nView));
    pCtrl->f284 = pRect[0];
    pCtrl->f280 = pRect[1];
    pCtrl->f27C = pRect[2];
    pCtrl->f278 = pRect[3];
}

void fn_80017158(int nView) {
    ViewController* pCtrl;

    pCtrl = fn_80016E28(nView);
    fn_800171D8(fn_80012EF0(fn_80017004(nView)), pCtrl->f284, pCtrl->f280, pCtrl->f27C, pCtrl->f278);
}

s32 fn_800171B0(void) {
    // port: fn_800171B0 (and fn_80092274's slot) are typed s32, but the value is the frame buffer
    return (s32)fn_80013E40(*lbl_80280DF0);
}

void fn_800171D8(f32* pRect, f32 x, f32 y, f32 w, f32 h) {
    pRect[0] = x;
    pRect[1] = y;
    pRect[2] = w;
    pRect[3] = h;
    fn_80076A54(pRect);
}

// Points the lens from pPos with the view's angles: builds its camera-to-world matrix (rotation from
// the angles, pPos as the translation row) and inverts it into the world-to-camera matrix m44.
void fn_80017208(CamLens* pLens, f32* pPos, f32* pAngles) {
    fn_8001728C(pLens);
    fn_8000A194(pLens->m4, pAngles[1], pAngles[0], pAngles[2]);
    pLens->m4[3][0] = pPos[0];
    pLens->m4[3][1] = pPos[1];
    pLens->m4[3][2] = pPos[2];
    pLens->m4[3][3] = 1.0f;
    fn_8000A798(pLens->m4, pLens->m44);
}

void fn_8001728C(CamLens* pLens) {
    pLens->m84[0][0] = 1.0f;
    pLens->m84[0][1] = 1.0f;
    pLens->m84[0][2] = 1.0f;
    pLens->m84[0][3] = 1.0f;
    pLens->m84[1][0] = 1.0f;
    pLens->m84[1][1] = 1.0f;
    pLens->m84[1][2] = 1.0f;
    pLens->m84[1][3] = 1.0f;
}

f32* fn_800172B4(View* pView) {
    return &pView->f50;
}

f32* fn_800172BC(View* pView) {
    return &pView->f5C;
}

u8 fn_800172C4(View* pView) {
    if (pView->script.pShot == NULL) return 1;
    if (pView->script.pShot->bAD) return 1;
    if (pView->script.pShot->p40 == NULL && pView->script.nCamera != 1) return 1;
    return 0;
}

f32* fn_80017314(View* pView) {
    return pView->v10;
}

f32* fn_8001731C(View* pView) {
    return pView->v0;
}
