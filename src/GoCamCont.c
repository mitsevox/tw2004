// GoCamCont.c (TW06's golf/cameras/gocamcont.c; our spelling): the camera controller of each view
// (View, TW06's CameraController): picking the camera mode (View_SetCamera), its idle state and the
// small setters and tests the camera code uses. Not decompiled yet beyond the functions below.

#include "golfer.h"
#include "camera.h"
#include "unsorted/cull.h"

u8   fn_800C72DC(View* pView);
u8   fn_80063608(int nPlayer, f32* pPos, f32 fMargin);
u8   fn_800637C4(int nPlayer, int nView);
void fn_800642A4(View* pView, f32 fF0, f32 fF4);
void fn_80064478(f32* pA, f32* pB, f32* pOut);
void fn_800090E4(f32* pQuat, f32* pIn, f32* pOut);      // Quaternion.c: a vector turned by it

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
    pView->script.bE8 = 0;
    pView->script.pB4 = &pView->shot19C;
    pView->script.fF0 = 0.0f;
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

// Asks for shot kind nKind on the player's view. With club 25 only kinds 0, 5, 8, 11 and 23 are
// taken. Kind 12 first records the current camera and blends from it into the player's kind 12
// shot; kind 7 becomes 10 when the ball lies on surface class 7 or 16. While kind 12 is asked for
// only 5, 8 and 10 replace it, 6 is never taken, 2 and 3 do not replace 7, and 7 does not replace
// 2 or 3.
void fn_80063CF0(View* pView, int nKind, int nPlayer) {
    f32* pPos = fn_8001731C(pView);
    f32* pAt = fn_80017314(pView);
    f32 vNormal[4];
    f32 vSpeed[4] = {0.1f, 0.1f, 0.1f, 0.5f};
    SurfaceType* pSurface;
    CamShot* pShot;
    int nAsked;

    if (gPlayers[nPlayer].nClub == 25 && nKind != 0 && nKind != 8 && nKind != 5 && nKind != 11
        && nKind != 23) {
        return;
    }
    if (nKind == 12 && pView->script.nC4 != 12) {
        pShot = fn_8003A7C8(nPlayer, 12, NULL);
        if (pShot != NULL) {
            CameraScript_RecordCurrentCam(&pView->shot19C, pPos, pAt, nPlayer, &pView->script, 0);
            pView->shot19C.p40 = pShot;
            CameraScript_InterpToNewScript(&pView->script, &pView->shot19C, nPlayer, pPos, pAt, 5, 0.0f,
                                           100.0f, 25, 0.0f);
            pView->script.nBC = 5;
            pView->script.f8C = 0.3f;
            fn_80063BF4(pView, 0.3f, vSpeed);
        }
    }
    if (nKind == 7) {
        if (!(fn_8004DBB0(fn_8000C594(), gPlayers[nPlayer].ball.vPos, &pSurface, vNormal) < -60000.0f)
            && pSurface != NULL && (pSurface->nClass == 7 || pSurface->nClass == 16)) {
            nKind = 10;
        }
    }
    nAsked = pView->script.nC4;
    if (nAsked == 12 && nKind != 8 && nKind != 5 && nKind != 6 && nKind != 10) {
        return;
    }
    if (nKind == 6) {
        return;
    }
    if (nKind == 3 || nKind == 2) {
        if (nAsked != 7) {
            pView->script.nC4 = nKind;
        }
    } else if (nKind == 7) {
        if (nAsked != 3 && nAsked != 2) {
            pView->script.nC4 = nKind;
        }
    } else {
        pView->script.nC4 = nKind;
    }
}

// Shakes the camera: moves its position by up to half of the script's fF4 each way.
void fn_8006421C(View* pView) {
    pView->v0[0] += pView->script.fF4 * (Rand_Float(0) - 0.5f);
    pView->v0[1] += pView->script.fF4 * (Rand_Float(0) - 0.5f);
    pView->v0[2] += pView->script.fF4 * (Rand_Float(0) - 0.5f);
}

void fn_800642A4(View* pView, f32 fF0, f32 fF4) {
    pView->script.fF0 = fF0;
    pView->script.fF4 = fF4;
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

// Turns pA a fifth of the way towards pB (both taken as directions) into pOut; while the game is
// paused pA is copied unchanged.
void fn_80063F08(f32* pA, f32* pB, f32* pOut) {
    f32 qTurn[4];
    f32 vAxis[4];
    f32 vA[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    f32 vB[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    f32 fAngle;

    if (gSession.nPaused != 0) {
        Vec3Copy(pA, pOut);
        return;
    }
    if (pA[0] != 0.0f || pA[1] != 0.0f || pA[2] != 0.0f) {
        fn_800BAF04(pA, vA);
    } else {
        vA[0] = 0.0f;
        vA[1] = 0.0f;
        vA[2] = 0.0f;
    }
    if (pB[0] != 0.0f || pB[1] != 0.0f || pB[2] != 0.0f) {
        fn_800BAF04(pB, vB);
    } else {
        vB[0] = 0.0f;
        vB[1] = 0.0f;
        vB[2] = 0.0f;
    }
    fAngle = fn_80009614(fn_8000C5FC(vA, vB) < -1.0f  ? -1.0f
                         : fn_8000C5FC(vA, vB) > 1.0f ? 1.0f
                                                      : fn_8000C5FC(vA, vB));
    fAngle *= 0.2f;
    vec4flt_CrossProduct(vA, vB, vAxis);
    if (vAxis[0] != 0.0f || vAxis[1] != 0.0f || vAxis[2] != 0.0f) {
        fn_800BAF04(vAxis, vAxis);
    }
    fn_8001EF34(vAxis, fAngle, vAxis);
    fn_8000923C(vAxis, qTurn);
    vA[3] = 0.0f;
    fn_800090E4(qTurn, vA, pOut);
}

// The view's v20: the side vector of its flat look direction, turned about that direction by the
// current shot's fA8 (0 without a shot).
void fn_80064108(View* pView) {
    f32 vDir[4];
    f32 vUp[4] = {0.0f, 1.0f, 0.0f, 0.0f};
    f32 vSide[4];
    f32 qTurn[4];

    fn_80064478(pView->v10, pView->v0, vDir);
    vDir[1] = 0.0f;
    if (vDir[0] == 0.0f && vDir[2] == 0.0f) {
        vDir[0] = 0.01f;
    }
    if (vDir[0] != 0.0f || vDir[1] != 0.0f || vDir[2] != 0.0f) {
        fn_800BAF04(vDir, vDir);
    }
    vec4flt_CrossProduct(vUp, vDir, vSide);
    if (pView->script.pShot == NULL) {
        pView->script.fA8 = 0.0f;
    }
    fn_8001EF34(vDir, pView->script.fA8, vDir);
    fn_8000923C(vDir, qTurn);
    vSide[3] = 0.0f;
    fn_800090E4(qTurn, vSide, pView->v20);
}

// Puts the point pPos through the camera onto the screen: pX and pY from 0 to 1 across it, pZ its
// depth. 0 when the point is behind the camera.
u8 fn_8006434C(void* pCamera, f32* pPos, f32* pX, f32* pY, f32* pZ) {
    f32 v[4];
    u8 bInFront = 1;

    pPos[3] = 1.0f;
    fn_800BAD60(((Camera*)pCamera)->mDC, (Vec4*)pPos, (Vec4*)v);
    if (v[3] >= 0.0f) {
        bInFront = 0;
    }
    if (v[3] < -0.0001f || v[3] > 0.0001f) {
        fn_8000AE28(v, 1.0f / v[3], v);
    } else if (v[3] < 0.0f) {
        fn_8000AE28(v, -10000.0f, v);
    } else {
        fn_8000AE28(v, 10000.0f, v);
    }
    if (pX != NULL) {
        *pX = 0.5f * (1.0f + v[0]);
    }
    if (pY != NULL) {
        *pY = 0.5f * (1.0f + v[1]);
    }
    if (pZ != NULL) {
        *pZ = v[2];
    }
    return bInFront;
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
