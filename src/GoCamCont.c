// GoCamCont.c (TW06's golf/cameras/gocamcont.c; our spelling): the camera controller of each view
// (View, TW06's CameraController): picking the camera mode (View_SetCamera), its idle state and the
// small setters and tests the camera code uses. Not decompiled yet beyond the functions below.

#include "golfer.h"
#include "camera.h"

u8   fn_800C72DC(View* pView);
u8   fn_80063608(int nPlayer, f32* pPos, f32 fMargin);
u8   fn_800637C4(int nPlayer, int nView);
void fn_800642A4(View* pView, f32 f174, f32 f178);

// Sets a view up: no camera (25), no shots, the script cleared.
void fn_80062E40(View* pView) {
    int i;

    for (i = 0; i < 4; i++) {
        pView->v0[i] = 0.0f;
    }
    for (i = 0; i < 4; i++) {
        pView->v10[i] = 0.0f;
    }
    pView->f50 = 1.0f;
    pView->f54 = 1.0f;
    pView->f58 = 1.0f;
    pView->nCurCamera = 25;
    pView->script.fCamTime = 0.0f;
    pView->script.f98 = 0.0f;
    pView->script.f90 = 0.0f;
    pView->script.pShot = NULL;
    pView->script.pNextShot = NULL;
    pView->script.nCamera = 0;
    pView->script.bCC = 0;
    pView->script.bCF = 0;
    pView->b16C = 0;
    pView->script.pB4 = &pView->shot19C;
    pView->f174 = 0.0f;
    pView->p78 = NULL;
    pView->p7C = NULL;
    pView->p80 = NULL;
    pView->p74 = NULL;
    pView->n260 = 0;
    pView->bFade = 0;
    for (i = 0; i < 4; i++) {
        pView->script.v0[i] = 0.0f;
    }
    for (i = 0; i < 4; i++) {
        pView->script.v10[i] = 0.0f;
    }
    for (i = 0; i < 8; i++) {
        pView->script.a20[i] = 0.0f;
    }
    pView->script.v70[0] = 0.0f;
    pView->script.v70[1] = 0.0f;
    pView->script.v70[2] = 0.0f;
    pView->script.v70[3] = 1.0f;
}

// Forgets the view's camera sequences and shot.
void fn_80062F1C(View* pView) {
    pView->p78 = NULL;
    pView->p7C = NULL;
    pView->p80 = NULL;
    pView->p74 = NULL;
    pView->b268 = 0;
}

// Starts the player's shot of kind nKind on the view and runs its script from the start.
void fn_8006351C(View* pView, int nPlayer, int nKind) {
    f32* pPos = fn_8001731C(pView);
    f32* pAt = fn_80017314(pView);
    CamShot* pShot = fn_8003A7C8(nPlayer, nKind, NULL);

    if (pShot != NULL) {
        pView->script.pNextShot = pShot->p40;
        if (pShot->p40 != NULL) {
            pView->script.nBC = pShot->p40->bAB;
            pView->script.f8C = pShot->p40->f48;
        }
    }
    pView->script.fCamTime = 0.0f;
    pView->script.pShot = pShot;
    fn_8003DCE8(nPlayer, pPos, pAt, &pView->script, &pView->shot19C, 0, 0.0f);
}

// The player's target is on screen, 0.1 in from the edges.
u8 fn_800635D0(int nPlayer) {
    return fn_80063608(nPlayer, gPlayers[nPlayer].vTarget, 0.1f);
}

// pPos is on the player's screen, at least fMargin in from every edge.
u8 fn_80063608(int nPlayer, f32* pPos, f32 fMargin) {
    f32 fX;
    f32 fY;
    f32 fZ;

    if (fn_8006434C(fn_80017004(gPlayers[nPlayer].nView[0]), pPos, &fX, &fY, &fZ)) {
        if (fX < 1.0f - fMargin && fX > fMargin && fY < 1.0f - fMargin && fY > fMargin) {
            return 1;
        }
    }
    return 0;
}

// The player's ball is on screen, 0.1 in from the edges.
u8 fn_800636B4(int nPlayer) {
    return fn_80063608(nPlayer, gPlayers[nPlayer].ball.vPos, 0.1f);
}

// The first player for whom fn_800637C4 holds on the current view; -2 if none.
int fn_800636EC(void) {
    int nView = fn_80016D10();
    int i = 0;

    while (i < gNumPlayersSetUp) {
        if (fn_800637C4(i, nView)) {
            return i;
        }
        i++;
    }
    return -2;
}

// The same as fn_800636EC.
int fn_80063758(void) {
    int nView = fn_80016D10();
    int i = 0;

    while (i < gNumPlayersSetUp) {
        if (fn_800637C4(i, nView)) {
            return i;
        }
        i++;
    }
    return -2;
}

// View nView is the player's first view and fn_8001707C gives it to the player, and its shots pass
// the tests below (camera 4 when it has no shot).
u8 fn_800637C4(int nPlayer, int nView) {
    View* pView;

    if (gPlayers[nPlayer].nView[0] == nView && fn_8001707C(nView) == nPlayer) {
        pView = fn_80017028(gPlayers[nPlayer].nView[0]);
        if (pView->script.pShot != NULL) {
            if ((pView->nCurCamera == 15 || pView->nCurCamera == 16) && fn_800C72DC(pView)) {
                return 1;
            }
            if (pView->script.pShot->bAA == 0) {
                if (pView->script.pNextShot == NULL || pView->script.pNextShot->bAA == 0
                    || pView->script.nBC == 5) {
                    return 1;
                }
            }
        } else if (pView->nCurCamera == 4) {
            return 1;
        }
    }
    return 0;
}

// Switches the view to camera 2 once the player's ball falls below the tuning height before its
// first bounce.
void fn_800638B8(View* pView, int nPlayer) {
    if (gPlayers[nPlayer].ball.vVel[1] < 0.0f && gPlayers[nPlayer].ball.fHeight < lbl_80281F78->f90
        && gPlayers[nPlayer].ball.nCollideCount < 1) {
        fn_80063CF0(pView, 2, nPlayer);
    }
}

// Camera 2 on the point pVec, over fTime.
void fn_80063B98(View* pView, f32 fTime, f32* pVec) {
    pView->script.nCamera = 2;
    Vec_Copy(pVec, pView->script.v40);
    pView->script.f90 = 0.0f;
    pView->script.f94 = fTime;
}

// Camera 1 on the point pVec, over fTime.
void fn_80063BF4(View* pView, f32 fTime, f32* pVec) {
    pView->script.nCamera = 1;
    Vec_Copy(pVec, pView->script.v40);
    pView->script.f90 = 0.0f;
    pView->script.f94 = fTime;
}

u8 fn_80063C50(View* pView) {
    if (pView->script.nCamera == 5 || pView->script.nCamera == 4 || pView->script.nCamera == 3) {
        return 1;
    }
    return 0;
}

u8 fn_80063C7C(View* pView) {
    return pView->script.nCamera == 4;
}

u8 fn_80063C90(View* pView) {
    if (pView->script.nCamera == 2 || pView->script.nCamera == 1 || pView->script.nCamera == 4) {
        return 1;
    }
    return 0;
}

void fn_80063CBC(View* pView, f32* pVec) {
    pView->script.nCamera = 3;
    Vec_Copy(pVec, pView->script.v40);
}

// Shakes the camera: moves its position by up to half of f178 each way.
void fn_8006421C(View* pView) {
    pView->v0[0] += pView->f178 * (Rand_Float(0) - 0.5f);
    pView->v0[1] += pView->f178 * (Rand_Float(0) - 0.5f);
    pView->v0[2] += pView->f178 * (Rand_Float(0) - 0.5f);
}

void fn_800642A4(View* pView, f32 f174, f32 f178) {
    pView->f174 = f174;
    pView->f178 = f178;
}

u8 fn_800642B0(void) {
    return fn_800C6CB0();
}

// Blends the view's script into its current shot.
void fn_800642D0(View* pView, int nPlayer) {
    f32* pPos = fn_8001731C(pView);

    CameraScript_InterpToNewScript(&pView->script, pView->script.pShot, nPlayer, pPos, fn_80017314(pView), 5,
                                   0.0f, 100.0f, 25, 0.0f);
}

// a - b into out, three floats; the same helper as Ball.c's fn_80055EA0.
#ifdef __MWERKS__
asm void fn_80064478(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_80064478(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif
