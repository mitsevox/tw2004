// Code8009AA28.c (our name; what it does is not known yet): code that drives SunFlr_Gc.c's
// functions (fn_8009A250-fn_8009A754) from its own state (lbl_802813B8) and tables, and queues
// glows (fn_8009B260). Its extent so far is the functions that use its data: the .sdata
// 0x802813B8-0x802813C0, the .sdata2 0x80283E90-0x80283EC0 and the .data 0x80189E78-0x8018C6C8
// (fn_8009AA28-fn_8009B18C, and fn_8009B260 between them and fn_8009B314). Where it starts after
// SunFlr_Gc.c (0x8009A844 at the earliest) is not proven, and fn_8009B314 is still in a sweep file
// with the next file's first function.

#include "glows.h"
#include "camera.h"

// SunFlr_Gc.c
void fn_8009A340(void);
void fn_8009A3D0(s32 nView, SunFlrView* pView);
void fn_8009A704(s32 nView);
void fn_8009A3F4(s32 nView);
f32  fn_8009A754(s32 nView, SunFlrView* pView);

s32  fn_800171B0(void);                 // ViewController.c
CamLens* fn_8001F004(void);
f32  fn_8001415C(u8* p);                // GoRenderCtx_Gc.c
f32  fn_8001416C(u8* p);

// Frees each view's part.
void fn_8009B0D0(void) {
    s32 i;

    fn_8009A340();
    for (i = 0; i < lbl_802813B8->nViews; i++) {
        fn_8009A3D0(i, &lbl_802813B8->aView[i]);
    }
}

void fn_8009B134(void) {
    s32 i;

    if (lbl_802813B8->b1BF0) {
        for (i = 0; i < lbl_802813B8->nViews; i++) {
            fn_8009A704(i);
        }
    }
}

// Per view, each frame: where v4 falls on the view's screen, and fn_8009A754's result for the
// field being drawn. The getters called first have their results thrown away.
void fn_8009B18C(s32 nView) {
    f32* pRect;
    s32 nCtx;
    SunFlrState* pState;
    SunFlrView* pView;

    pRect = fn_8003526C();
    nCtx = fn_800171B0();
    fn_8001F004();
    // port: fn_800171B0 is typed s32 in ViewController.c, but its value is a render context pointer
    fn_8001416C((u8*)nCtx);
    fn_80012ED8(pRect);
    fn_8001415C((u8*)nCtx);
    fn_80012ED0(pRect);
    pState = lbl_802813B8;
    if (pState->b1BF0) {
        pView = &pState->aView[nView];
        pView->bA4 = fn_8006434C(fn_80017004(nView), pState->v4, &pView->f98, &pView->f9C, &pView->fA0);
        pView->af90[1 - (lbl_80281B88 & 1)] = fn_8009A754(nView, pView);
        fn_8009A3F4(nView);
    }
}

void fn_8009B260(f32* pPos, u32 uColorA, u32 uColorB, u8 n25, u8 n24, u8 n26, f32 f18, f32 f1C,
                 f32 f20) {
    GlowQueued* pGlow;

    if (lbl_80281F80->nCount < NUM_GLOW_QUEUE) {
        pGlow = &lbl_80281F80->a[lbl_80281F80->nCount++];
        pGlow->uColorA = uColorA;
        pGlow->uColorB = uColorB;
        Vec_Copy(pPos, pGlow->vPos);
        pGlow->f18 = f18;
        pGlow->f1C = f1C;
        pGlow->f20 = f20;
        pGlow->n24 = n24;
        pGlow->n26 = n26;
        pGlow->n25 = n25;
    }
}
