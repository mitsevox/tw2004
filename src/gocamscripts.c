// gocamscripts.c (TW06's golf/cameras/gocamscripts.c; the CameraScript_ / CamScript_ functions): the
// camera scripts. A view's script moves the camera from shot to shot (the shots and sequences
// GoDynamicCam.c picks), keeps it above the ground and on the fairway, and aims it at the ball
// and the pin. The unit covers the file's certain core; its edges are still open.

#include "golfer.h"
#include "game.h"
#include "camera.h"

void fn_80045428(f32* pA, f32* pB, f32* pOut);  // pOut = pA - pB (paired singles)
void fn_8004544C(f32* pA, f32* pB, f32* pOut);  // pOut = pA + pB (paired singles)
void fn_800418B0(CamShot* pShot, f32* pOut, f32 f1, f32 f2);
void fn_80041EA8(int nPlayer, f32* pOut, f32* pCam, CamShot* pShot, CamScript* pScript, f32* pVec,
                 f32 fTime);
void fn_800422C4(int nPlayer, f32* pOut, f32* pCam, f32* pTarget, CamShot* pShot, CamScript* pScript,
                 f32 fTime, f32 fLag);
f32  fn_80043420(int nPlayer, CamScript* pScript, f32* pPos, f32* pCam, CamShot* pShot);
void fn_8004349C(int nPlayer, f32* pCam, f32* pOut, f32* pTarget, CamScript* pScript, f32 fLag);
void fn_8004255C(f32* pPos, f32* pTarget, f32 fUp, f32 fSide);
void fn_8001EB8C(Character* pChar, int nBone, f32* pPos);   // char.c: a bone's position
CamLens* fn_80008370(void* pCamera);                        // the render camera's lens
void fn_80038054(u8 a, int n, f32 f1, f32 f2);
void fn_800457B8(int nPlayer, f32 f);
f32  fn_800DC45C(f32 f);
u8   fn_80044E74(CamShot* pShot);
void fn_80064F54(CamShot* pShot, int nPlayer, f32* pOut);
void fn_8003A148(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pSub, f32* pPrev,
                 f32 fTime);
void fn_8003EE68(CamScript* pScript, f32* pCam, int a, int nPlayer, f32 f);
f32  fn_8003F064(CamScript* pScript, f32 fTime);
void fn_8003F518(int nPlayer, f32* pCam, f32* pSub, CamScript* pScript, f32* pPrev, f32 fTime);
void fn_8003F7EC(int nPlayer, f32* pCam, f32* pSub, CamScript* pScript, f32* pPrev, f32 fTime);
void fn_8003FAA0(int nPlayer, f32* pCam, f32* pSub, CamScript* pScript, f32* pPrev, f32 fTime);
void fn_8003FD54(int nPlayer, f32* pCam, f32* pSub, CamScript* pScript, f32* pPrev, f32 fTime);
void fn_8004017C(int nPlayer, f32* pCam, f32* pSub, CamScript* pScript, f32* pPrev, f32 fTime);
void CamScript_SplineCameras(int nPlayer, f32* pCam, f32* pSub, CamScript* pScript, f32* pPrev, f32 fTime);
void fn_800407D4(int nPlayer, f32* pCam, f32* pSub, CamScript* pScript, f32* pPrev, f32 fTime);
void fn_80040A58(int nPlayer, f32* pCam, f32* pSub, CamScript* pScript, f32* pPrev, f32 fTime);
void CamScript_GetLookAtPoint(CamShot* pShot, int nPlayer, f32* pOut, f32* pCam, CamScript* pScript,
                              f32* pVec, f32 fTime);
void CameraScript_GoToNewScript(CamScript* pScript, CamShot* pShot, int nPlayer, f32* pCam, f32* pSub,
                                CamShot* pSaved);
void CamScript_CheckOutOfBounds(CamScript* pScript, f32* pCam, f32* pSub, int nPlayer, CamShot* pSaved,
                                f32* pPrev, u8 b);
void CamScript_UpdateFairwayCam(CamScript* pScript, f32* pCam, f32* pSub, int nPlayer);
void fn_800441E4(CamScript* pScript, f32* pCam, f32* pSub, int nPlayer, CamShot* pSaved, f32* pPrev);
u8   fn_800439E4(f32* pCam, int nPlayer);
u8   fn_80043920(CamScript* pScript, int nPlayer);
void fn_80044768(f32* pPos, f32* pOut);
f32  fn_8003F790(CamScript* pScript);   // the blend's share (0..1) so far
void fn_80040CF0(CamScript* pScript, f32* pSub, int nPlayer, f32* pPrev, f32 fTime);
void fn_800090E4(f32* pTurn, f32* pVec, f32* pOut);     // the vector turned by it
u8   fn_800DC464(int nPlayer);          // GameEffects.c: the ball is simulated from its position
u8   Ter_CheckForGroundCollision(CourseInfo* pCourse, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal,
                                 SurfaceType** ppSurface, TerObject** ppObj);

// The camera script's frame (a view's &View.script; pShot is the view's hand-built shot19C). Unless
// paused (or b), it eases the ball-update rate fEC, places the camera for the current and next
// shots (fn_8003A148 for script shots, bA8 1), then either takes the current shot outright (its look-at
// point, field of view, slow motion) or blends by the next shot's kind nBC (0, 1, 2/11/12, 3, 7,
// 13, 14, 15); keeps the camera above the ground and on the fairway, steps the script's clocks, and
// moves on to the next shot when the current one has run its f8C (kinds 13 and 15: when fF8
// reaches 1).
void fn_8003DCE8(int nPlayer, f32* pCam, f32* pSub, CamScript* pScript, CamShot* pShot, u8 b, f32 fTime) {
    f32 vBall[4];
    f32 vMove[4];
    f32 vPrev[4];
    f32 fFov;
    f32 fStep;
    f32 f88;
    f32 f8C;
    f32 f90;
    int nUpdates;

    if (pScript->pShot == NULL) return;
    if (fn_800C714C()) {
        fTime = FRAME_TIME;
    }
    fn_8000C594();  // the result is not used
    if (!b && !fn_80043388(pScript, pScript->pShot)
        && (gSession.nPaused != 0 || (0.0f == gSession.fFrameTime && 0.0f == fTime))) {
        if (pScript->nCamera != 0) {
            fn_8003F2E0(pScript, fTime);
        }
        return;
    }
    if (fn_80043920(pScript, nPlayer)) {
        if (pScript->nCamera != 0) {
            fn_8003F2E0(pScript, fTime);
        }
        return;
    }
    Vec_Copy(pCam, vPrev);
    nUpdates = GameEffects_BallUpdatesThisFrame(nPlayer);
    if (nUpdates > 0 && (f32)nUpdates != pScript->fEC) {
        if (pScript->fEC < (f32)nUpdates) {
            pScript->fEC += lbl_80281F78->f1A8;
            if (pScript->fEC > (f32)nUpdates) {
                pScript->fEC = nUpdates;
            }
        } else {
            pScript->fEC -= lbl_80281F78->f1A8;
            if (pScript->fEC < (f32)nUpdates) {
                pScript->fEC = nUpdates;
            }
        }
    }
    pScript->fEC = pScript->fEC < 1.0f ? 1.0f : (pScript->fEC > 2.0f ? 2.0f : pScript->fEC);
    if (pScript->pShot->bA8 == 1) {
        fn_8003A148(pScript->pShot, nPlayer, pScript, pScript->v0, pSub, vPrev, fTime);
    } else {
        fn_80064F54(pScript->pShot, nPlayer, pScript->v0);
    }
    if (pScript->pNextShot != NULL) {
        if (pScript->pNextShot->bA8 == 1) {
            fn_8003A148(pScript->pNextShot, nPlayer, pScript, pScript->v10, pSub, vPrev, fTime);
        } else {
            fn_80064F54(pScript->pNextShot, nPlayer, pScript->v10);
        }
    }
    if (pScript->pNextShot == NULL || pScript->nBC == 5 || pScript->nBC == 6
        || (pScript->nBC >= 8 && pScript->nBC <= 10) || pScript->nBC == 4) {
        Vec3Copy(pScript->v0, pCam);
        CamScript_GetLookAtPoint(pScript->pShot, nPlayer, pScript->a20, pCam, pScript, vPrev, fTime);
        Vec3Copy(pScript->a20, pSub);
        if (pScript->pShot->f78 == pScript->pShot->f7C) {
            fFov = pScript->pShot->f78;
        } else if (pScript->fCamTime < pScript->pShot->f48) {
            fFov = pScript->fCamTime / pScript->pShot->f48 * (pScript->pShot->f7C - pScript->pShot->f78)
                   + pScript->pShot->f78;
        } else {
            fFov = pScript->pShot->f7C;
        }
        fFov += fn_800DC3A4();
        if (fn_80044E74(pScript->pShot)) {
            fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[1])), fFov);
        } else {
            fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[0])), fFov);
        }
        f88 = pScript->pShot->f88;
        fn_800457B8(nPlayer, f88 + fn_800DC45C(f88));
        f8C = pScript->pShot->f8C;
        f90 = pScript->pShot->f90;
        if (f8C > 0.0f || f90 > 0.0f) {
            if (fn_80044E74(pScript->pShot)) {
                fn_80038054(1, gPlayers[nPlayer].nView[1], f90, f8C);
            } else {
                fn_80038054(1, gPlayers[nPlayer].nView[0], f90, f8C);
            }
        }
        pScript->fA8 = pScript->pShot->f9C;
    } else {
        switch (pScript->nBC) {
        case 0:
            fn_8003F518(nPlayer, pCam, pSub, pScript, vPrev, fTime);
            break;
        case 13:
            fn_8003F7EC(nPlayer, pCam, pSub, pScript, vPrev, fTime);
            break;
        case 15:
            fn_8003FAA0(nPlayer, pCam, pSub, pScript, vPrev, fTime);
            break;
        case 1:
            fn_8003FD54(nPlayer, pCam, pSub, pScript, vPrev, fTime);
            break;
        case 14:
            fn_8004017C(nPlayer, pCam, pSub, pScript, vPrev, fTime);
            break;
        case 2:
        case 11:
        case 12:
            fn_800407D4(nPlayer, pCam, pSub, pScript, vPrev, fTime);
            break;
        case 7:
            fn_80040A58(nPlayer, pCam, pSub, pScript, vPrev, fTime);
            break;
        case 3:
            CamScript_SplineCameras(nPlayer, pCam, pSub, pScript, vPrev, fTime);
            break;
        }
    }
    if (pScript->pShot->bA8 == 1 || (pScript->pNextShot != NULL && pScript->pNextShot->bA8 == 1)) {
        if (pScript->pShot->bAD != 0
            && CamScript_KeepAboveGround(nPlayer, pCam, vPrev, fn_80043388(pScript, pScript->pShot) == 0,
                                         NULL, NULL, NULL, lbl_80281F78->f168)
            && (s8)GOLFERSTATE_GetCurrentState(nPlayer) == 12 && pScript->pShot->bAD != 3) {
            CamScript_PutBackOnFairway(pScript, pCam, pSub, nPlayer, pShot, vPrev);
        }
        if (pScript->bCF) {
            if (pScript->pShot != NULL && pScript->pShot->bAD == 4) {
                CamScript_UpdateFairwayCam(pScript, pCam, pSub, nPlayer);
            }
        }
        if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) == 12 && fn_80043388(pScript, pScript->pShot)
            && pScript->pShot->bAD != 3 && !pScript->bCF && fn_800439E4(pCam, nPlayer) && !fn_800E39F0()) {
            CamScript_PutBackOnFairway(pScript, pCam, pSub, nPlayer, pShot, vPrev);
        }
        if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) == 12 && !fn_8003A76C(pScript->pShot)) {
            fn_800441E4(pScript, pCam, pSub, nPlayer, pShot, vPrev);
        }
    }
    fn_8003EE68(pScript, pCam, 1, nPlayer, -1.0f);
    if (pScript->nCamera != 0) {
        fn_8003F2E0(pScript, fTime);
    }
    if (fn_80043388(pScript, pScript->pShot)) {
        pScript->v60[0] = 0.0f;
        pScript->v60[1] = 0.0f;
        pScript->v60[2] = 0.0f;
    } else {
        fn_80045428(pCam, vPrev, pScript->v60);
    }
    b = fn_80043388(pScript, pScript->pShot);
    fStep = fn_8003F064(pScript, fTime);
    pScript->f84 = pScript->fCamTime;
    pScript->fCamTime += fStep;
    pScript->f90 += fStep;
    pScript->f88 += fStep;
    pScript->f9C = pScript->f98;
    pScript->f98 += fStep;
    fn_8003D9AC(pScript, pScript->pShot, nPlayer, vBall, 1);
    if (pScript->nE0 != 25 && pScript->f98 > pScript->fE4) {
        pScript->nC4 = pScript->nE0;
    }
    if (0.0f != fStep) {
        pScript->bCC = 0;
    }
    if (pScript->pNextShot != NULL) {
        if ((pScript->nBC == 13 || pScript->nBC == 15) ? pScript->fF8 >= 1.0f
                                                         : pScript->fCamTime > pScript->f8C) {
            if (pScript->nBC == 7) {
                pScript->pNextShot = NULL;
                pScript->nBC = 5;
            } else {
                CameraScript_GoToNewScript(pScript, pScript->pNextShot, nPlayer, pCam, pSub, pShot);
                b = fn_80043388(pScript, pScript->pShot);
            }
            if (fStep > 0.0f) {
                fn_80045428(vPrev, pCam, vMove);
                pScript->fD4 = (f32)fn_80009680(fn_80009744(vMove)) / fStep;
            }
        }
    }
    if (pScript->pShot->bA8 == 1) {
        CamScript_CheckOutOfBounds(pScript, pCam, pSub, nPlayer, pShot, vPrev, b);
    }
}

// A plainer script frame than fn_8003DCE8 (a is not read): no pause test, no ball-rate easing, no
// ground or fairway checks; the look-at point goes straight to pSub and fD8 is cleared.
void fn_8003E624(int nPlayer, f32* pCam, f32* pSub, CamScript* pScript, CamShot* pShot, int a, f32 fTime) {
    f32 vPrev[4];
    f32 vMove[4];
    f32 fFov;

    if (pScript->pShot == NULL) return;
    pScript->fD8 = 0.0f;
    Vec_Copy(pCam, vPrev);
    if (pScript->pShot->bA8 == 1) {
        fn_8003A148(pScript->pShot, nPlayer, pScript, pScript->v0, pSub, vPrev, fTime);
    } else {
        fn_80064F54(pScript->pShot, nPlayer, pScript->v0);
    }
    if (pScript->pNextShot != NULL) {
        if (pScript->pNextShot->bA8 == 1) {
            fn_8003A148(pScript->pNextShot, nPlayer, pScript, pScript->v10, pSub, vPrev, fTime);
        } else {
            fn_80064F54(pScript->pNextShot, nPlayer, pScript->v10);
        }
    }
    if (pScript->pNextShot == NULL || pScript->nBC == 5 || pScript->nBC == 4) {
        Vec3Copy(pScript->v0, pCam);
        CamScript_GetLookAtPoint(pScript->pShot, nPlayer, pSub, pCam, pScript, vPrev, fTime);
        if (pScript->pShot->f78 == pScript->pShot->f7C) {
            fFov = pScript->pShot->f78;
        } else if (pScript->fCamTime < pScript->pShot->f48) {
            fFov = pScript->fCamTime / pScript->pShot->f48 * (pScript->pShot->f7C - pScript->pShot->f78)
                   + pScript->pShot->f78;
        } else {
            fFov = pScript->pShot->f7C;
        }
        fFov += fn_800DC3A4();
        if (fn_80044E74(pScript->pShot)) {
            fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[1])), fFov);
        } else {
            fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[0])), fFov);
        }
        pScript->fA8 = pScript->pShot->f9C;
    } else {
        switch (pScript->nBC) {
        case 0:
            fn_8003F518(nPlayer, pCam, pSub, pScript, vPrev, fTime);
            break;
        case 13:
            fn_8003F7EC(nPlayer, pCam, pSub, pScript, vPrev, fTime);
            break;
        case 15:
            fn_8003FAA0(nPlayer, pCam, pSub, pScript, vPrev, fTime);
            break;
        case 1:
            fn_8003FD54(nPlayer, pCam, pSub, pScript, vPrev, fTime);
            break;
        case 14:
            fn_8004017C(nPlayer, pCam, pSub, pScript, vPrev, fTime);
            break;
        case 2:
        case 11:
        case 12:
            fn_800407D4(nPlayer, pCam, pSub, pScript, vPrev, fTime);
            break;
        case 7:
            fn_80040A58(nPlayer, pCam, pSub, pScript, vPrev, fTime);
            break;
        case 3:
            CamScript_SplineCameras(nPlayer, pCam, pSub, pScript, vPrev, fTime);
            break;
        }
    }
    fn_80043388(pScript, pScript->pShot);   // its answer is not used
    pScript->f84 = pScript->fCamTime;
    pScript->fCamTime += fTime;
    pScript->f90 += fTime;
    pScript->f88 += fTime;
    pScript->f9C = pScript->f98;
    pScript->f98 += fTime;
    if (pScript->nE0 != 25 && pScript->f98 > pScript->fE4) {
        pScript->nC4 = pScript->nE0;
    }
    if (0.0f != fTime) {
        pScript->bCC = 0;
    }
    if (pScript->pNextShot != NULL && pScript->fCamTime > pScript->f8C) {
        if (pScript->nBC == 7) {
            pScript->pNextShot = NULL;
            pScript->nBC = 5;
        } else {
            CameraScript_GoToNewScript(pScript, pScript->pNextShot, nPlayer, pCam, pSub, pShot);
            fn_80043388(pScript, pScript->pShot);   // its answer is not used
        }
        if (fTime > 0.0f) {
            fn_80045428(vPrev, pCam, vMove);
            pScript->fD4 = (f32)fn_80009680(fn_80009744(vMove)) / fTime;
        }
    }
}

// Blend kind 0: the camera and the point it looks at both move on straight lines from the current
// shot's to the next one's (by fn_8003F790's share); field of view, fn_800457B8's value, slow
// motion and fA8 blend the same.
void fn_8003F518(int nPlayer, f32* pCam, f32* pSub, CamScript* pScript, f32* pPrev, f32 fTime) {
    f32 vMove[4];
    f32 vPos[4];
    CamShot* pShot = pScript->pShot;
    CamShot* pNext = pScript->pNextShot;
    f32 fT = fn_8003F790(pScript);
    f32 f;
    f32 f90;

    fn_80045428(pScript->v10, pScript->v0, vMove);
    fn_8001EF34(vMove, fT, vPos);
    fn_8004544C(vPos, pScript->v0, vPos);
    Vec3Copy(vPos, pCam);
    CamScript_GetLookAtPoint(pShot, nPlayer, pScript->a20, pScript->v0, pScript, pPrev, fTime);
    CamScript_GetLookAtPoint(pNext, nPlayer, &pScript->a20[4], pScript->v10, pScript, pPrev, fTime);
    fn_80045428(&pScript->a20[4], pScript->a20, vMove);
    fn_8001EF34(vMove, fT, vPos);
    fn_8004544C(vPos, pScript->a20, vPos);
    Vec3Copy(vPos, pSub);
    f = fT * (pNext->f78 - pShot->f78) + pShot->f78;
    f += fn_800DC3A4();
    if (fn_80044E74(pScript->pShot)) {
        fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[1])), f);
    } else {
        fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[0])), f);
    }
    f = fT * (pNext->f88 - pShot->f88) + pShot->f88;
    f += fn_800DC45C(f);
    fn_800457B8(nPlayer, f);
    f = fT * (pNext->f8C - pShot->f8C) + pShot->f8C;
    f90 = fT * (pNext->f90 - pShot->f90) + pShot->f90;
    if (f > 0.0f || f90 > 0.0f) {
        if (fn_80044E74(pShot)) {
            fn_80038054(1, gPlayers[nPlayer].nView[1], f90, f);
        } else {
            fn_80038054(1, gPlayers[nPlayer].nView[0], f90, f);
        }
    }
    pScript->fA8 = fT * (pNext->f9C - pShot->f9C) + pShot->f9C;
}

// Blend kind 1: the camera moves on the straight line from the current shot's position to the next
// one's (by fn_8003F790's share), and its view direction turns from the one shot's to the other's
// about their common axis; field of view, fn_800457B8's value, slow motion and fA8 blend the same.
void fn_8003FD54(int nPlayer, f32* pCam, f32* pSub, CamScript* pScript, f32* pPrev, f32 fTime) {
    f32 vDir0[4];
    f32 vDir1[4];
    f32 vFromAim[4];
    f32 vToAim[4];
    f32 vMove[4];
    f32 vPos[4];
    f32 vAxis[4];
    f32 qTurn[4];
    CamShot* pShot = pScript->pShot;
    CamShot* pNext = pScript->pNextShot;
    f32 fT = fn_8003F790(pScript);
    f32 fDot;
    f32 fAngle;
    f32 fDist;
    f32 f;
    f32 f90;

    fn_80045428(pScript->v10, pScript->v0, vMove);
    fn_8001EF34(vMove, fT, vPos);
    fn_8004544C(vPos, pScript->v0, vPos);
    Vec3Copy(vPos, pCam);
    fn_80045428(pScript->v0, pScript->a20, vFromAim);
    CamScript_GetLookAtPoint(pShot, nPlayer, pScript->a20, pScript->v0, pScript, pPrev, fTime);
    fn_80045428(pScript->a20, pScript->v0, vDir0);
    if (0.0f != vDir0[0] || 0.0f != vDir0[1] || 0.0f != vDir0[2]) {
        fn_800BAF04(vDir0, vDir0);
    }
    fn_80045428(pScript->v10, &pScript->a20[4], vToAim);
    CamScript_GetLookAtPoint(pNext, nPlayer, &pScript->a20[4], pScript->v10, pScript, pPrev, fTime);
    fn_80045428(&pScript->a20[4], pScript->v10, vDir1);
    if (0.0f != vDir1[0] || 0.0f != vDir1[1] || 0.0f != vDir1[2]) {
        fn_800BAF04(vDir1, vDir1);
    }
    // the dot product clamped to -1..1 (worked out again for each test)
    fDot = fn_8000C5FC(vDir0, vDir1) < -1.0f ? -1.0f
         : (fn_8000C5FC(vDir0, vDir1) > 1.0f ? 1.0f : fn_8000C5FC(vDir0, vDir1));
    fAngle = fn_80009614(fDot);
    fAngle *= fT;
    vec4flt_CrossProduct(vDir0, vDir1, vAxis);
    if (0.0f != vAxis[0] || 0.0f != vAxis[1] || 0.0f != vAxis[2]) {
        fn_800BAF04(vAxis, vAxis);
    }
    fn_8001EF34(vAxis, fAngle, vAxis);
    fn_8000923C(vAxis, qTurn);
    vDir0[3] = 0.0f;
    fn_800090E4(qTurn, vDir0, pSub);
    if (0.0f != pSub[0] || 0.0f != pSub[1] || 0.0f != pSub[2]) {
        fn_800BAF04(pSub, pSub);
    }
    fDist = fn_80009680(fn_80009744(vFromAim));
    fn_8001EF34(pSub, fT * ((f32)fn_80009680(fn_80009744(vToAim)) - fDist) + fDist, pSub);
    fn_8004544C(pCam, pSub, pSub);
    f = fT * (pNext->f78 - pShot->f78) + pShot->f78;
    f += fn_800DC3A4();
    if (fn_80044E74(pScript->pShot)) {
        fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[1])), f);
    } else {
        fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[0])), f);
    }
    f = fT * (pNext->f88 - pShot->f88) + pShot->f88;
    f += fn_800DC45C(f);
    fn_800457B8(nPlayer, f);
    f = fT * (pNext->f8C - pShot->f8C) + pShot->f8C;
    f90 = fT * (pNext->f90 - pShot->f90) + pShot->f90;
    if (f > 0.0f || f90 > 0.0f) {
        if (fn_80044E74(pShot)) {
            fn_80038054(1, gPlayers[nPlayer].nView[1], f90, f);
        } else {
            fn_80038054(1, gPlayers[nPlayer].nView[0], f90, f);
        }
    }
    pScript->fA8 = fT * (pNext->f9C - pShot->f9C) + pShot->f9C;
}

// Blend kinds 2, 11 and 12: the point looked at moves on the straight line between the two shots'
// look-at points; the camera follows the curve fn_800C7E50 makes through the two positions (kind
// nD0), set up by fn_80040CF0 on the blend's first frame.
void fn_800407D4(int nPlayer, f32* pCam, f32* pSub, CamScript* pScript, f32* pPrev, f32 fTime) {
    f32 vMove[4];
    f32 vPos[4];
    CamShot* pShot = pScript->pShot;
    CamShot* pNext = pScript->pNextShot;
    f32 fT = fn_8003F790(pScript);
    f32 f;
    f32 f90;

    CamScript_GetLookAtPoint(pShot, nPlayer, pScript->a20, pScript->v0, pScript, pPrev, fTime);
    CamScript_GetLookAtPoint(pNext, nPlayer, &pScript->a20[4], pScript->v10, pScript, pPrev, fTime);
    fn_80045428(&pScript->a20[4], pScript->a20, vMove);
    fn_8001EF34(vMove, fT, vPos);
    fn_8004544C(vPos, pScript->a20, vPos);
    Vec3Copy(vPos, pSub);
    if (0.0f == pScript->fCamTime) {
        fn_80040CF0(pScript, pSub, nPlayer, pPrev, fTime);
    }
    fn_800C7E50(pScript->v0, pScript->v10, pSub, pScript->nD0, pCam, fT);
    f = fT * (pNext->f78 - pShot->f78) + pShot->f78;
    f += fn_800DC3A4();
    if (fn_80044E74(pScript->pShot)) {
        fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[1])), f);
    } else {
        fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[0])), f);
    }
    f = fT * (pNext->f88 - pShot->f88) + pShot->f88;
    f += fn_800DC45C(f);
    fn_800457B8(nPlayer, f);
    f = fT * (pNext->f8C - pShot->f8C) + pShot->f8C;
    f90 = fT * (pNext->f90 - pShot->f90) + pShot->f90;
    if (f > 0.0f || f90 > 0.0f) {
        if (fn_80044E74(pShot)) {
            fn_80038054(1, gPlayers[nPlayer].nView[1], f90, f);
        } else {
            fn_80038054(1, gPlayers[nPlayer].nView[0], f90, f);
        }
    }
    pScript->fA8 = fT * (pNext->f9C - pShot->f9C) + pShot->f9C;
}

// Blend kind 7: there and back. The share runs to 1 at the blend's middle and back to 0; the
// point looked at and the camera's curve (fn_800C7E50, kind nD0: 1 going, 2 coming back) follow
// it. fA8 blends by the doubled share itself.
void fn_80040A58(int nPlayer, f32* pCam, f32* pSub, CamScript* pScript, f32* pPrev, f32 fTime) {
    f32 vMove[4];
    f32 vPos[4];
    CamShot* pShot = pScript->pShot;
    CamShot* pNext = pScript->pNextShot;
    f32 fT = fn_8003F790(pScript);
    f32 f;
    f32 fShare;
    f32 f90;

    fT *= 2.0f;
    if (fT > 1.0f) {
        fShare = 1.0f - (fT - 1.0f);
    } else {
        fShare = fT;
    }
    CamScript_GetLookAtPoint(pShot, nPlayer, pScript->a20, pScript->v0, pScript, pPrev, fTime);
    CamScript_GetLookAtPoint(pNext, nPlayer, &pScript->a20[4], pScript->v10, pScript, pPrev, fTime);
    fn_80045428(&pScript->a20[4], pScript->a20, vMove);
    fn_8001EF34(vMove, fShare, vPos);
    fn_8004544C(vPos, pScript->a20, vPos);
    Vec3Copy(vPos, pSub);
    if (fT < 1.0f) {
        pScript->nD0 = 1;
    } else {
        pScript->nD0 = 2;
    }
    fn_800C7E50(pScript->v0, pScript->v10, pSub, pScript->nD0, pCam, fShare);
    f = fShare * (pNext->f78 - pShot->f78) + pShot->f78;
    f += fn_800DC3A4();
    if (fn_80044E74(pScript->pShot)) {
        fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[1])), f);
    } else {
        fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[0])), f);
    }
    f = fShare * (pNext->f88 - pShot->f88) + pShot->f88;
    f += fn_800DC45C(f);
    fn_800457B8(nPlayer, f);
    f = fShare * (pNext->f8C - pShot->f8C) + pShot->f8C;
    f90 = fShare * (pNext->f90 - pShot->f90) + pShot->f90;
    if (f > 0.0f || f90 > 0.0f) {
        if (fn_80044E74(pShot)) {
            fn_80038054(1, gPlayers[nPlayer].nView[1], f90, f);
        } else {
            fn_80038054(1, gPlayers[nPlayer].nView[0], f90, f);
        }
    }
    pScript->fA8 = fT * (pNext->f9C - pShot->f9C) + pShot->f9C;
}

// Blend kind 14: the camera moves on the straight line between the two shots' positions. The point
// it looks at is the current shot's look-at point plus the share of the way from a point along the
// current view direction (at the ball's level distance from the camera) to the next shot's look-at
// point.
void fn_8004017C(int nPlayer, f32* pCam, f32* pSub, CamScript* pScript, f32* pPrev, f32 fTime) {
    f32 vDir[4];
    f32 vFromAim[4];
    f32 vMove[4];
    f32 vPos[4];
    f32 vBall[4];
    CamShot* pShot = pScript->pShot;
    CamShot* pNext = pScript->pNextShot;
    f32 fT = fn_8003F790(pScript);
    f32 f;
    f32 f90;

    fn_80045428(pScript->v10, pScript->v0, vMove);
    fn_8001EF34(vMove, fT, vPos);
    fn_8004544C(vPos, pScript->v0, vPos);
    Vec3Copy(vPos, pCam);
    fn_80045428(pScript->v0, pScript->a20, vFromAim);
    CamScript_GetLookAtPoint(pShot, nPlayer, pScript->a20, pScript->v0, pScript, pPrev, fTime);
    fn_80045428(pScript->a20, pScript->v0, vDir);
    if (0.0f != vDir[0] || 0.0f != vDir[1] || 0.0f != vDir[2]) {
        fn_800BAF04(vDir, vDir);
    }
    fn_80045428(gPlayers[nPlayer].ball.vPos, pScript->v0, vBall);
    vBall[1] = 0.0f;
    fn_8001EF34(vDir, fn_80009680(fn_80009744(vBall)), vDir);
    fn_8004544C(vDir, pScript->v0, vDir);
    CamScript_GetLookAtPoint(pNext, nPlayer, &pScript->a20[4], pScript->v10, pScript, pPrev, fTime);
    fn_80045428(&pScript->a20[4], vDir, vMove);
    fn_8001EF34(vMove, fT, vPos);
    fn_8004544C(vPos, pScript->a20, vPos);
    Vec3Copy(vPos, pSub);
    f = fT * (pNext->f78 - pShot->f78) + pShot->f78;
    f += fn_800DC3A4();
    if (fn_80044E74(pScript->pShot)) {
        fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[1])), f);
    } else {
        fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[0])), f);
    }
    f = fT * (pNext->f88 - pShot->f88) + pShot->f88;
    f += fn_800DC45C(f);
    fn_800457B8(nPlayer, f);
    f = fT * (pNext->f8C - pShot->f8C) + pShot->f8C;
    f90 = fT * (pNext->f90 - pShot->f90) + pShot->f90;
    if (f > 0.0f || f90 > 0.0f) {
        if (fn_80044E74(pShot)) {
            fn_80038054(1, gPlayers[nPlayer].nView[1], f90, f);
        } else {
            fn_80038054(1, gPlayers[nPlayer].nView[0], f90, f);
        }
    }
    pScript->fA8 = fT * (pNext->f9C - pShot->f9C) + pShot->f9C;
}

// Where the camera looks for the shot's bAC, into pOut: the pin (kind 0 in golfer state 18), the
// ball, Player.vBall, bones of the golfer, the tee, the aim; the look-at point is moved by the
// shot's f74 up and f70 sideways (fn_8004255C), and by fn_800418B0's offset before and after.
// fn_80043388 decides between setting the point outright and easing it there.
void CamScript_GetLookAtPoint(CamShot* pShot, int nPlayer, f32* pOut, f32* pCam, CamScript* pScript,
                              f32* pVec, f32 fTime) {
    f32 vPos[4];
    f32 vDiff[4];
    f32 vBone47[4];
    f32 vBone39[4];
    f32 vMid[4];
    f32 vBone1[4];
    f32 vBone10[4];
    f32 vVel[4];
    f32 vAim[4];
    f32 vOffset[4];
    f32 fY;
    f32 fLen;
    f32 fStep;
    CourseInfo* pCourse;

    if (!fn_80043388(pScript, pShot)) {
        fn_800418B0(pShot, vOffset, pScript->f9C, pShot->f98);
        fn_80045428(pOut, vOffset, pOut);
    }
    switch (pShot->bAC) {
    case 0:
        if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) == 18) {  // fake match: see game.h
            pCourse = fn_8000C594();
            Vec3Copy(&pCourse->pin[Game_CurrentPinSet()].x, pOut);
        } else if (fn_80043388(pScript, pShot)) {
            fn_8003D9AC(pScript, pShot, nPlayer, vPos, 0);
            if (pScript->bCF) {
                // not steeper than 1 in 5 down to the camera, once it is far enough out
                fn_80045428(vPos, pCam, vDiff);
                fY = vDiff[1];
                vDiff[1] = 0.0f;
                fLen = fn_80009680(fn_80009744(vDiff));
                if (fY / fLen < -0.2f && fLen > lbl_80281F78->f128) {
                    vPos[1] = pCam[1] - 0.2f * fLen;
                }
            }
            Vec3Copy(vPos, pOut);
            pOut[1] = fn_80043420(nPlayer, pScript, vPos, pCam, pShot);
            fn_8004255C(pOut, pCam, pShot->f74, pShot->f70);
            pScript->fDC = lbl_80281F78->fE4;
        } else {
            fn_80041EA8(nPlayer, pOut, pCam, pShot, pScript, pVec, fTime);
        }
        break;
    case 23:
        Vec3Copy(gPlayers[nPlayer].ball.vVel, vVel);
        vVel[1] = 0.0f;
        fn_8001EF34(vVel, pShot->f70, vVel);
        vVel[1] = pShot->f74 * gPlayers[nPlayer].ball.vVel[1];
        fn_8004544C(gPlayers[nPlayer].ball.vPos, vVel, vAim);
        if (fn_80043388(pScript, pShot)) {
            Vec3Copy(vAim, pOut);
        } else {
            fn_800422C4(nPlayer, pOut, pCam, vAim, pShot, pScript, fTime, lbl_80281F78->fE4);
        }
        fn_8004349C(nPlayer, pCam, pOut, gPlayers[nPlayer].ball.vPos, pScript, lbl_80281F78->fE4);
        break;
    case 15:
        Vec3Copy(gPlayers[nPlayer].ball.vPos, pOut);
        break;
    case 13:
        if (fn_80043388(pScript, pShot)) {
            Vec3Copy(gPlayers[nPlayer].ball.vPos, pOut);
            pOut[1] -= gPlayers[nPlayer].ball.fHeight;
            fn_8004255C(pOut, pCam, pShot->f74, pShot->f70);
        } else if (GameEffects_BallUpdatesThisFrame(nPlayer) != 0) {
            Vec3Copy(gPlayers[nPlayer].ball.vPos, vPos);
            vPos[1] -= gPlayers[nPlayer].ball.fHeight;
            fn_8004255C(vPos, pCam, pShot->f74, pShot->f70);
            fStep = (vPos[1] - pOut[1]) * lbl_80281F78->f164;
            pOut[0] = vPos[0];
            pOut[1] += fStep;
            pOut[2] = vPos[2];
            fn_8004349C(nPlayer, pCam, pOut, pOut, pScript, lbl_80281F78->fE4);
        }
        break;
    case 1:
        Vec3Copy(gPlayers[nPlayer].vBall, pOut);
        if (fn_800453C8(nPlayer, pShot)) {
            fn_8004255C(pOut, pCam, pShot->f74, -pShot->f70);
        } else {
            fn_8004255C(pOut, pCam, pShot->f74, pShot->f70);
        }
        break;
    case 2:
        fn_8001EB8C(gPlayers[nPlayer].pChar, 0x39, vBone39);
        fn_8001EB8C(gPlayers[nPlayer].pChar, 0x47, vBone47);
        fn_8004544C(vBone39, vBone47, vMid);
        fn_8001EF34(vMid, 0.5f, vMid);
        Vec3Copy(vMid, pOut);
        if (fn_800453C8(nPlayer, pShot)) {
            fn_8004255C(pOut, pCam, pShot->f74, -pShot->f70);
        } else {
            fn_8004255C(pOut, pCam, pShot->f74, pShot->f70);
        }
        break;
    case 3:
        fn_8001EB8C(gPlayers[nPlayer].pChar, 0x39, vBone39);
        fn_8001EB8C(gPlayers[nPlayer].pChar, 0x47, vBone47);
        fn_8004544C(vBone39, vBone47, vMid);
        fn_8001EF34(vMid, 0.5f, vMid);
        if (fn_80043388(pScript, pShot)) {
            Vec3Copy(vMid, pOut);
            if (fn_800453C8(nPlayer, pShot)) {
                fn_8004255C(pOut, pCam, pShot->f74, -pShot->f70);
            } else {
                fn_8004255C(pOut, pCam, pShot->f74, pShot->f70);
            }
        } else {
            fn_800422C4(nPlayer, pOut, pCam, vMid, pShot, pScript, fTime, lbl_80281F78->fE8);
        }
        break;
    case 4:
        fn_8001EB8C(gPlayers[nPlayer].pChar, 1, vBone1);
        Vec3Copy(vBone1, pOut);
        if (fn_800453C8(nPlayer, pShot)) {
            fn_8004255C(pOut, pCam, pShot->f74, -pShot->f70);
        } else {
            fn_8004255C(pOut, pCam, pShot->f74, pShot->f70);
        }
        break;
    case 5:
        fn_8001EB8C(gPlayers[nPlayer].pChar, 1, vBone1);
        if (fn_80043388(pScript, pShot)) {
            Vec3Copy(vBone1, pOut);
            if (fn_800453C8(nPlayer, pShot)) {
                fn_8004255C(pOut, pCam, pShot->f74, -pShot->f70);
            } else {
                fn_8004255C(pOut, pCam, pShot->f74, pShot->f70);
            }
        } else {
            fn_800422C4(nPlayer, pOut, pCam, vBone1, pShot, pScript, fTime, lbl_80281F78->fE8);
        }
        break;
    case 6:
        fn_8001EB8C(gPlayers[nPlayer].pChar, 10, vBone10);
        Vec3Copy(vBone10, pOut);
        if (fn_800453C8(nPlayer, pShot)) {
            fn_8004255C(pOut, pCam, pShot->f74, -pShot->f70);
        } else {
            fn_8004255C(pOut, pCam, pShot->f74, pShot->f70);
        }
        break;
    case 7:
        fn_8001EB8C(gPlayers[nPlayer].pChar, 10, vBone10);
        if (fn_80043388(pScript, pShot)) {
            Vec3Copy(vBone10, pOut);
            if (fn_800453C8(nPlayer, pShot)) {
                fn_8004255C(pOut, pCam, pShot->f74, -pShot->f70);
            } else {
                fn_8004255C(pOut, pCam, pShot->f74, pShot->f70);
            }
        } else {
            fn_800422C4(nPlayer, pOut, pCam, vBone10, pShot, pScript, fTime, lbl_80281F78->fE8);
        }
        break;
    case 10:
        Vec3Copy(&fn_8000C594()->pin[Game_CurrentPinSet()].x, pOut);
        fn_8004255C(pOut, pCam, pShot->f74, pShot->f70);
        break;
    case 11:
        Vec3Copy(&fn_8000C594()->tee[gSession.nTeeSet[nPlayer]].x, pOut);
        fn_8004255C(pOut, pCam, pShot->f74, pShot->f70);
        break;
    case 9:
        if (fn_80043388(pScript, pShot) || pScript->pNextShot != NULL) {
            Vec3Copy(gPlayers[nPlayer].vTargetCopy, pOut);
            fn_8004255C(pOut, pCam, pShot->f74, pShot->f70);
        } else {
            CameraScript_LagAimMarker(nPlayer, pOut, pCam, pShot, 1, 0, lbl_80281F78->fD8, 0.0f,
                                      lbl_80281F78->fDC);
        }
        break;
    case 14:
        fn_8003D9AC(pScript, pShot, nPlayer, pOut, 0);
        pOut[1] = pCam[1];
        fn_8004255C(pOut, pCam, pShot->f74, pShot->f70);
        break;
    case 22:
    case 24:
        Vec3Copy(pShot->v30, pOut);
        fn_8004255C(pOut, pCam, pShot->f74, pShot->f70);
        break;
    }
    fn_80043388(pScript, pShot);    // its answer is not used
    fn_800418B0(pShot, vOffset, pScript->f98, pShot->f98);
    fn_8004544C(pOut, vOffset, pOut);
}

// Raises pPos by fUp and moves it fSide sideways, square to the line from pTarget to it.
void fn_8004255C(f32* pPos, f32* pTarget, f32 fUp, f32 fSide) {
    f32 vDir[4];

    pPos[1] += fUp;
    fn_80045428(pPos, pTarget, vDir);
    if (0.0f != vDir[0] || 0.0f != vDir[1] || 0.0f != vDir[2]) {
        fn_800BAF04(vDir, vDir);
    }
    pPos[0] += fSide * -vDir[2];
    pPos[2] += fSide * vDir[0];
}

// Where the ball-flight camera expects the ball to land, into the script's v50: the ball as it lay
// before the shot while it has not been hit (single view), the target without a club (25); in
// flight, where the line of its velocity (lifted while rising, bent down by its height) meets the
// ground, pulled back to the club's reach; at rest, 5 x fn_800510EC along its velocity, on the ground.
void CameraScript_UpdateLandingEstimate(CamScript* pScript, int nPlayer) {
    f32 vHit[4];
    f32 vNormal[4];
    f32 vVel[4];
    f32 vStep[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    f32 vPos[4];
    f32 vLand[4];
    f32 vUp[4];
    f32 vTarget[4];
    SurfaceType* pSurface;
    TerObject* pObj;
    Ball* pBall;
    CourseInfo* pCourse;
    f32 fRise;
    f32 fDist;
    f32 fReach;
    f32 fHeight;

    Vec3Copy(gPlayers[nPlayer].vTargetCopy, vTarget);
    if (gSession.nSplitScreen == 0
        && (gPlayers[nPlayer].ballBefore.nState == 0 || gPlayers[nPlayer].ballBefore.nState == 1
            || gPlayers[nPlayer].ballBefore.nState == 5)) {
        Vec3Copy(gPlayers[nPlayer].ballBefore.vPos, pScript->v50);
        return;
    }
    if (gPlayers[nPlayer].nClub == 25) {
        Vec3Copy(vTarget, pScript->v50);
        return;
    }
    if (gSession.nSplitScreen != 0) {
        pBall = &gPlayers[nPlayer].ball;
    } else if (fn_800DC464(nPlayer)) {
        pBall = &gPlayers[nPlayer].ballBefore;
    } else {
        pBall = &gPlayers[nPlayer].ball;
    }
    Vec3Copy(pBall->vPos, vPos);
    if (pBall->nState == 2) {
        Vec3Copy(pBall->vVel, vVel);
        if (0.0f != vVel[0] || 0.0f != vVel[1] || 0.0f != vVel[2]) {
            if (vVel[1] >= 0.0f) {
                fRise = pBall->vVel[1] / 0.10717f;
                fn_800BAF04(vVel, vStep);
                fn_8001EF34(vStep, fRise, vUp);
                fn_8004544C(vPos, vUp, vPos);
                vVel[1] = 0.0f;
            }
            vVel[1] -= powf(pBall->fHeight, 0.7f) / 1.4f;
            fn_800BAF04(vVel, vVel);
            fn_8001EF34(vVel, 200.0f, vVel);
            fn_8004544C(vPos, vVel, vStep);
            pCourse = fn_8000C594();
            if (Ter_CheckForGroundCollision(pCourse, vPos, vStep, vHit, vNormal, &pSurface, &pObj)) {
                fn_80045428(vHit, pBall->vStart, vLand);
                fDist = fn_80009680(fn_80009744(vLand));
                fReach = AI_MaxDistance(nPlayer, gPlayers[nPlayer].nShotKind, gPlayers[nPlayer].nClub);
                fReach *= fn_800510EC(&gPlayers[nPlayer].ball);
                fReach *= fn_8005B64C(nPlayer);
                if (fReach < fDist - 50.0f) {
                    fn_800BAF04(vLand, vLand);
                    fn_8001EF34(vLand, fReach, vLand);
                    fn_8004544C(pBall->vStart, vLand, vLand);
                    fHeight = Terrain_HeightAt(vLand, NULL);
                    if (!(fHeight < -60000.0f)) {
                        vLand[1] = fHeight;
                        Vec3Copy(vLand, pScript->v50);
                        return;
                    }
                    Vec3Copy(vHit, pScript->v50);
                    return;
                }
                Vec3Copy(vHit, pScript->v50);
            }
        }
    } else {
        Vec3Copy(pBall->vVel, vStep);
        fn_8001EF34(vStep, 5.0f * fn_800510EC(&gPlayers[nPlayer].ball), vStep);
        fn_8004544C(vPos, vStep, vStep);
        fHeight = Terrain_HeightAt(vStep, NULL);
        if (!(fHeight < -60000.0f)) {
            vStep[1] = fHeight;
            Vec3Copy(vStep, pScript->v50);
        }
    }
}

// Moves the script on to pShot (its next shot): the next shot's positions become the current
// ones, pShot's follow-on (p40) the next shot, with its length and kind (the new shot's own when
// its bAD is 0). Unless the move is a blend (bCC), the landing estimate is refreshed and the script
// runs one frame at once. Kinds 6, 8, 9 and 10 are dropped outside golfer states 10 and 11.
void CameraScript_GoToNewScript(CamScript* pScript, CamShot* pShot, int nPlayer, f32* pCam, f32* pSub,
                                CamShot* pSaved) {
    s32 nOldA0 = pScript->pShot->nA0;

    if (pScript->pShot->bAD == 0) {
        pScript->fCamTime -= pScript->f8C;
    } else {
        pScript->f88 = lbl_80281F78->f15C;
        pScript->fCamTime = 0.0f;
    }
    if (pScript->pNextShot->bAC == 0) {
        if (pScript->nBC != 5) {
            pScript->bCD = 0;
        } else {
            pScript->bCD = 1;
        }
    } else {
        pScript->bCD = 0;
    }
    if (pScript->nBC != 5 && (pScript->nBC != 4 || pShot->bAB == 4)) {
        pScript->bCC = 1;
    } else {
        pScript->fDC = PI;
    }
    pScript->pShot = pShot;
    pScript->pNextShot = pScript->pShot->p40;
    if (pScript->pNextShot != NULL) {
        if (pScript->pShot->bAD == 0) {
            pScript->f8C = pScript->pShot->f48;
            pScript->nBC = pScript->pShot->bAB;
        } else {
            pScript->f8C = pScript->pNextShot->f48;
            pScript->nBC = pScript->pNextShot->bAB;
        }
    } else {
        pScript->nBC = 5;
    }
    Vec3Copy(pScript->v10, pScript->v0);
    Vec3Copy(&pScript->a20[4], pScript->a20);
    if (pScript->pShot->bAF == 16 && pScript->bCC == 0) {
        Vec3Copy(gPlayers[nPlayer].ball.vPos, pScript->v0);
    }
    if (pScript->pNextShot != NULL && pScript->pShot->bB0 == 16) {
        Vec3Copy(gPlayers[nPlayer].ball.vPos, pScript->v10);
    }
    if (pScript->bCC == 0) {
        CameraScript_UpdateLandingEstimate(pScript, nPlayer);
        fn_8003DCE8(nPlayer, pCam, pSub, pScript, pSaved, 0, 0.0f);
        pScript->fCamTime += 0.001f;
        pScript->f88 += 0.001f;
        pScript->f98 = 0.0f;
    }
    pScript->bCE = 1;
    if ((pScript->nBC == 6 || pScript->nBC == 8 || pScript->nBC == 9 || pScript->nBC == 10)
        && (s8)GOLFERSTATE_GetCurrentState(nPlayer) != 10
        && (s8)GOLFERSTATE_GetCurrentState(nPlayer) != 11) {   // fake match: see game.h
        pScript->nBC = 5;
        pScript->pNextShot = NULL;
    }
    if (gSession.nSplitScreen == 0 && pScript->pShot != NULL && nOldA0 != pScript->pShot->nA0) {
        if (pScript->pShot->nA0 == 0) {
            fn_80045558(0, nPlayer);
            fn_80045494(1, nPlayer);
        } else if (pScript->pShot->nA0 == 2) {
            fn_80045558(1, nPlayer);
            fn_80045494(0, nPlayer);
        } else {
            fn_80045558(0, nPlayer);
            fn_80045494(0, nPlayer);
        }
        if (pScript->pShot->nA0 == 3) {
            fn_800C7140(1);
        } else {
            fn_800C7140(0);
        }
    }
    if (pScript->bCC == 0) {
        pScript->fEC = GameEffects_BallUpdatesThisFrame(nPlayer);
    }
}

// Starts pShot on the script. Blend kind nA 4 (the swing camera's cut) takes it outright unless
// the current shot is a kind-3 shot that is not the default swing camera; a blend with time f1
// makes pShot the next shot (first recording the current camera into pB4 when both shots are
// script shots), its length f1 at least the move's distance over speed f2; otherwise pShot and
// its follow-on start at once. nB and f3 go to nE0/fE4; single-view play sets fn_80045494,
// fn_80045558 and fn_800C7140 from the shot's nA0.
void CameraScript_InterpToNewScript(CamScript* pScript, CamShot* pShot, int nPlayer, f32* pCam, f32* pSub,
                                    int nA, f32 f1, f32 f2, int nB, f32 f3) {
    f32 vDiff[4];
    f32 vPos[4];
    f32 fDist;

    if (pShot == NULL) return;
    if (nA == 4) {
        if (pScript->pShot == NULL) {
            nA = 5;
            pScript->f88 = lbl_80281F78->f15C;
        } else if (pScript->pShot->bAD == 3) {
            if (pScript->pShot->p44 == NULL) {
                nA = 5;
                pScript->f88 = lbl_80281F78->f15C;
            } else if (CameraScript_IsDefaultSwingCam(pScript->pShot, nPlayer, pCam)) {
                pScript->f88 = 0.0f;
            } else {
                nA = 5;
                pScript->f88 = lbl_80281F78->f15C;
            }
        }
    } else {
        pScript->f88 = lbl_80281F78->f15C;
    }
    if (pShot->bAC == 0) {
        if (nA != 5 || pShot->bAF == 0 || pShot->bB0 == 0) {
            pScript->bCD = 0;
        } else {
            pScript->bCD = 1;
        }
    } else {
        pScript->bCD = 0;
    }
    if (nA == 4) {
        pScript->pShot = pShot;
        pScript->fDC = PI;
        pScript->pNextShot = pShot->p40;
        if (pShot->p40 != NULL) {
            f1 = pShot->p40->f48;
        }
        // both branches are the same in the original
        if (pScript->nBC == 4) {
            pScript->fCamTime = 0.01f;
            pScript->nBC = nA;
            pScript->f8C = f1;
        } else {
            pScript->fCamTime = 0.01f;
            pScript->nBC = nA;
            pScript->f8C = f1;
        }
    } else if (f1 > 0.0f && pScript->pShot != NULL) {
        if (pScript->pNextShot != NULL && pScript->pShot != pScript->pB4 && pScript->pNextShot != pScript->pB4
            && pShot != pScript->pB4) {
            CameraScript_RecordCurrentCam(pScript->pB4, pCam, pSub, nPlayer, pScript, 0);
            pScript->pShot = pScript->pB4;
        }
        pScript->pNextShot = pShot;
        pScript->fCamTime = 0.0f;
        pScript->nBC = nA;
        pScript->f8C = f1;
        pScript->bCC = 1;
        if (pShot->bB1 == 5) {
            if (pScript->pShot->bB1 == 5) {
                Vec3Copy(pScript->v0, pScript->v10);
            } else {
                Vec3Copy(pCam, pScript->v10);
            }
        } else if (pShot->bAF == 16) {
            Vec3Copy(gPlayers[nPlayer].ball.vPos, pScript->v10);
        } else {
            Vec3Copy(pCam, pScript->v10);
        }
        if (nA != 5) {
            Vec_Copy(pScript->v10, vPos);
            fn_8003A148(pScript->pNextShot, nPlayer, pScript, vPos, pSub, pCam, 0.0f);
            fn_80045428(vPos, pCam, vDiff);
            fDist = fn_80009680(fn_80009744(vDiff));
            if (fDist / f1 > f2) {
                pScript->f8C = fDist / f2;
            }
        }
    } else {
        pScript->fDC = PI;
        pScript->pShot = pShot;
        pScript->pNextShot = pShot->p40;
        pScript->f98 = 0.0f;
        if (pShot->p40 != NULL) {
            nA = pShot->p40->bAB;
            f1 = pShot->p40->f48;
        }
        if (pShot->bAF == 16) {
            Vec3Copy(gPlayers[nPlayer].ball.vPos, pScript->v0);
        }
        if (pScript->pNextShot != NULL && pScript->pShot->bB0 == 16) {
            Vec3Copy(gPlayers[nPlayer].ball.vPos, pScript->v10);
        }
        pScript->fCamTime = 0.0f;
        pScript->nBC = nA;
        pScript->f8C = f1;
        pScript->bCC = 0;
        pScript->fEC = GameEffects_BallUpdatesThisFrame(nPlayer);
    }
    pScript->bCE = 1;
    pScript->nE0 = nB;
    pScript->fE4 = f3;
    if (gSession.nSplitScreen == 0 && pScript->pShot != NULL) {
        if (pScript->pShot->nA0 == 0) {
            fn_80045558(0, nPlayer);
            fn_80045494(1, nPlayer);
        } else if (pScript->pShot->nA0 == 2) {
            fn_80045558(1, nPlayer);
            fn_80045494(0, nPlayer);
        } else {
            fn_80045558(0, nPlayer);
            fn_80045494(0, nPlayer);
        }
        if (pScript->pShot->nA0 == 3) {
            fn_800C7140(1);
        } else {
            fn_800C7140(0);
        }
    }
    pScript->fF8 = 0.0f;
}

// The script's shot, or else its next one (unless the next kind is 5), has bAC 0 or 13 while the
// ball makes no update this frame.
u8 fn_80043920(CamScript* pScript, int nPlayer) {
    if (pScript->pShot == NULL) {
        return 0;
    }
    if ((pScript->pShot->bAC == 0 || pScript->pShot->bAC == 13)
        && GameEffects_BallUpdatesThisFrame(nPlayer) < 1) {
        return 1;
    }
    if (pScript->pNextShot == NULL || pScript->nBC == 5) {
        return 0;
    }
    if ((pScript->pNextShot->bAC == 0 || pScript->pNextShot->bAC == 13)
        && GameEffects_BallUpdatesThisFrame(nPlayer) < 1) {
        return 1;
    }
    return 0;
}

// A spot for the camera by the green: of the AI targets nearest the ball, nearest the player's
// target and 3 past the pin (as seen from Player.vBall), taken nearest the ball first, the first
// whose level direction to the ball is far enough from the camera's (pSub to the ball); else the
// target nearest the point halfway from tee 0 to the pin. Kept at least 2 from the pin, raised
// CamTuning.f10C over the ground there into pOut (not when there is no ground); the ground height
// goes to *pHeight when it is not NULL.
void fn_80043C74(CamScript* pScript, f32* pOut, f32* pCam, int nPlayer, CamShot* pShot, f32* pSub,
                 f32* pHeight) {
    f32 vNearBall[4];
    f32 vNearTarget[4];
    f32 vPastPin[4];
    f32 vSpot[4];
    f32 vCamDir[4];
    f32 vDir[4];
    f32 vHalf[4];
    f32 vFirst[4];
    f32 vSecond[4];
    f32 vThird[4];
    f32 vDiff[4];
    u8 bFound = 0;
    CourseInfo* pCourse = fn_8000C594();
    f32* pBall;
    f32* pPin;
    int nPin;
    f32 fBall;
    f32 fTarget;
    f32 fPin;
    f32 fHeight;

    if (pCourse == NULL) return;
    nPin = Game_CurrentPinSet();
    pBall = gPlayers[nPlayer].ball.vPos;
    fn_80045428(pBall, pSub, vCamDir);
    vCamDir[1] = 0.0f;
    if (0.0f != vCamDir[0] || 0.0f != vCamDir[1] || 0.0f != vCamDir[2]) {
        fn_800BAF04(vCamDir, vCamDir);
    }
    fn_80044768(pBall, vNearBall);
    fn_80044768(gPlayers[nPlayer].vTarget, vNearTarget);
    pPin = &pCourse->pin[nPin].x;
    fn_80045428(pPin, gPlayers[nPlayer].vBall, vPastPin);
    vSpot[1] = 0.0f;    // EA bug: meant vPastPin[1]; the direction past the pin is not levelled
    if (0.0f != vPastPin[0] || 0.0f != vPastPin[1] || 0.0f != vPastPin[2]) {
        fn_800BAF04(vPastPin, vPastPin);
    }
    fn_8001EF34(vPastPin, 3.0f, vPastPin);
    fn_8004544C(pPin, vPastPin, vPastPin);

    fn_80045428(pBall, vNearBall, vDiff);
    vDiff[1] = 0.0f;
    fBall = fn_80009680(fn_80009744(vDiff));
    fn_80045428(pBall, vNearTarget, vDiff);
    vDiff[1] = 0.0f;
    fTarget = fn_80009680(fn_80009744(vDiff));
    fn_80045428(pBall, vPastPin, vDiff);
    vDiff[1] = 0.0f;
    fPin = fn_80009680(fn_80009744(vDiff));
    // the three spots in order of their distance from the ball
    if (fBall <= fTarget) {
        if (fBall <= fPin) {
            Vec3Copy(vNearBall, vFirst);
            if (fPin <= fTarget) {
                Vec3Copy(vPastPin, vSecond);
                Vec3Copy(vNearTarget, vThird);
            } else {
                Vec3Copy(vNearTarget, vSecond);
                Vec3Copy(vPastPin, vThird);
            }
        } else {
            Vec3Copy(vPastPin, vFirst);
            Vec3Copy(vNearBall, vSecond);
            Vec3Copy(vNearTarget, vThird);
        }
    } else if (fBall <= fPin) {
        Vec3Copy(vNearTarget, vFirst);
        Vec3Copy(vNearBall, vSecond);
        Vec3Copy(vPastPin, vThird);
    } else if (fPin <= fTarget) {
        Vec3Copy(vPastPin, vFirst);
        Vec3Copy(vNearTarget, vSecond);
        Vec3Copy(vNearBall, vThird);
    } else {
        Vec3Copy(vNearTarget, vFirst);
        Vec3Copy(vPastPin, vSecond);
        Vec3Copy(vNearBall, vThird);
    }

    Vec3Copy(vFirst, vSpot);
    fn_80045428(pBall, vSpot, vDir);
    vDir[1] = 0.0f;
    if (0.0f != vDir[0] || 0.0f != vDir[1] || 0.0f != vDir[2]) {
        fn_800BAF04(vDir, vDir);
    }
    if (fn_8000C5FC(vDir, vCamDir) < lbl_80281F78->fF4) {
        bFound = 1;
    }
    if (!bFound) {
        Vec3Copy(vSecond, vSpot);
        fn_80045428(pBall, vSpot, vDir);
        vDir[1] = 0.0f;
        if (0.0f != vDir[0] || 0.0f != vDir[1] || 0.0f != vDir[2]) {
            fn_800BAF04(vDir, vDir);
        }
        if (fn_8000C5FC(vDir, vCamDir) < lbl_80281F78->fF4) {
            bFound = 1;
        }
    }
    if (!bFound) {
        Vec3Copy(vThird, vSpot);
        fn_80045428(pBall, vSpot, vDir);
        vDir[1] = 0.0f;
        if (0.0f != vDir[0] || 0.0f != vDir[1] || 0.0f != vDir[2]) {
            fn_800BAF04(vDir, vDir);
        }
        if (fn_8000C5FC(vDir, vCamDir) < lbl_80281F78->fF4) {
            bFound = 1;
        }
    }
    if (!bFound) {
        fn_80045428(pPin, &pCourse->tee[0].x, vHalf);
        fn_8001EF34(vHalf, 0.5f, vHalf);
        fn_8004544C(&pCourse->tee[0].x, vHalf, vHalf);
        fn_80044768(vHalf, vSpot);
    }
    fn_80045428(pPin, vSpot, vPastPin);
    vPastPin[1] = 0.0f;
    if ((f32)fn_80009680(fn_80009744(vPastPin)) < 2.0f) {
        fn_80045428(pPin, pBall, vNearBall);
        vNearBall[1] = 0.0f;
        if (0.0f != vNearBall[0] || 0.0f != vNearBall[1] || 0.0f != vNearBall[2]) {
            fn_800BAF04(vNearBall, vNearBall);
        }
        fn_8001EF34(vNearBall, 2.0f, vNearBall);
        fn_8004544C(pPin, vNearBall, vSpot);
    }
    fHeight = Terrain_HeightAt(vSpot, NULL);
    if (pHeight != NULL) {
        *pHeight = fHeight;
    }
    if (!(fHeight < -60000.0f)) {
        vSpot[1] = fHeight + lbl_80281F78->f10C;
        Vec_Copy(vSpot, pOut);
    }
}

// The pin, when pPos is near no AI target: pOut gets the nearest target, or the current pin
// position of the hole.
void fn_80044768(f32* pPos, f32* pOut) {
    CourseInfo* pCourse = fn_8000C594();

    if (pCourse != NULL && AI_NearestTarget(pPos, pOut) < 0) {
        int nPin = Game_CurrentPinSet();

        Vec3Copy(&pCourse->pin[nPin].x, pOut);
    }
}

// ---- sweep code (not yet cleaned up) ----

s32 fn_80044AA8(void* arg0);

s32 fn_80044AA8(void* arg0) {
    u32 temp_r0;

    if (arg0 == NULL) {
        return 0;
    }
    temp_r0 = (*(u32*)((u8*)(arg0) + 0x2C));
    if (temp_r0 == 0xAU) {
        return 0;
    }
    if (temp_r0 == 9U) {
        return 0;
    }
    if ((temp_r0 >= 1U) && (temp_r0 <= 0xCU)) {
        return 1;
    }
    if (temp_r0 == 0x12U) {
        return 1;
    }
    return 0;
}

// ---- end of sweep code ----

// The highest of the nCount heights that is not above fMax, or TER_NO_GROUND.
f32 fn_80044B0C(f32* pHeights, u32 nCount, f32 fMax) {
    f32 fBest = -10000000.0f;
    u8 bFound = 0;
    u32 i;

    if (nCount == 0) return TER_NO_GROUND;
    for (i = 0; i < nCount; i++) {
        if (pHeights[i] >= fBest && pHeights[i] <= fMax) {
            fBest = pHeights[i];
            bFound = 1;
        }
    }
    if (bFound) return fBest;
    return TER_NO_GROUND;
}

// The lowest of the nCount heights that is not below fMin, or TER_NO_GROUND.
f32 fn_80044B70(f32* pHeights, u32 nCount, f32 fMin) {
    f32 fBest = 10000000.0f;
    u8 bFound = 0;
    u32 i;

    if (nCount == 0) return TER_NO_GROUND;
    for (i = 0; i < nCount; i++) {
        if (pHeights[i] <= fBest && pHeights[i] >= fMin) {
            fBest = pHeights[i];
            bFound = 1;
        }
    }
    if (bFound) return fBest;
    return TER_NO_GROUND;
}

// n is 149 on course 7's hole 2.
u8 fn_80044E2C(int n) {
    if (n == 149 && Game_GetCourse() == 7 && fn_80015464() == 2) {
        return 1;
    }
    return 0;
}

// The shot is of kind 6..10.
u8 fn_80044E74(CamShot* pShot) {
    if (pShot == NULL) return 0;
    if (pShot->bAD >= 6 && pShot->bAD <= 10) {
        return 1;
    }
    return 0;
}

// How much of the way from the shot's start to the pin the ball has covered, over the ground
// (0 at the start, 1 at the pin); 0 without a hole loaded.
f32 fn_80044F58(int nPlayer) {
    CourseInfo* pCourse = fn_8000C594();
    f32* pPin;
    f32 vStart[4];
    f32 vBall[4];
    f32 fStart;
    f32 fBall;

    if (pCourse == NULL) return 0.0f;
    pPin = &pCourse->pin[Game_CurrentPinSet()].x;
    fn_80045428(pPin, gPlayers[nPlayer].ball.vStart, vStart);
    fn_80045428(pPin, gPlayers[nPlayer].ball.vPos, vBall);
    vStart[1] = 0.0f;
    vBall[1] = 0.0f;
    fStart = fn_80009680(fn_80009744(vStart));
    fBall = fn_80009680(fn_80009744(vBall));
    if (fStart > 0.0f) {
        return 1.0f - fBall / fStart;
    }
    return 1.0f;
}

u8 fn_800453C8(int nPlayer, CamShot* pShot) {
    if (gSession.nGameType == 3) {
        return 0;
    }
    return fn_8001EDF4(gPlayers[nPlayer].pChar) != 0;
}

// a - b into out (three floats)
#ifdef __MWERKS__
asm void fn_80045428(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_80045428(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// a + b into out (three floats)
#ifdef __MWERKS__
asm void fn_8004544C(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_add f2, f2, f0
    ps_add f3, f3, f1
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_8004544C(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
}
#endif

void fn_80045470(CamLens* pLens, f32 fFov) {
    pLens->fFov = fFov;
    fn_800763BC(pLens);
}

// The quarter-speed slow motion (GameEffects.b11) on or off, with its sound events (0x35 on,
// 0x36 off); every second of its frames moves the ball.
void fn_80045494(u8 bOn, int nPlayer) {
    lbl_80202898.n2C = 2;
    if (bOn) {
        if (!lbl_80202898.b11) {
            EVENT_Trigger(nPlayer, 0x35, gPlayers[nPlayer].vBall, -1);
            lbl_80202898.b11 = bOn;
            lbl_80202898.n28 = 0;
        }
    } else if (lbl_80202898.b11) {
        EVENT_Trigger(nPlayer, 0x36, gPlayers[nPlayer].vBall, -1);
        lbl_80202898.b11 = bOn;
    }
}

// The half-speed slow motion (GameEffects.b10) on or off, with its sound events (0x37 on, 0x38 off).
void fn_80045558(u8 bOn, int nPlayer) {
    if (bOn) {
        if (!lbl_80202898.b10) {
            EVENT_Trigger(nPlayer, 0x37, gPlayers[nPlayer].vBall, -1);
            lbl_80202898.b10 = bOn;
        }
    } else if (lbl_80202898.b10) {
        EVENT_Trigger(nPlayer, 0x38, gPlayers[nPlayer].vBall, -1);
        lbl_80202898.b10 = bOn;
    }
}

// ---- sweep code (not yet cleaned up) ----

u8 fn_8004561C(void);

u8 fn_8004560C(void) {
    return lbl_80202898.bGameBreaker;
}

u8 fn_8004561C(void) {
    return lbl_80202898.b10;
}

u8 fn_8004562C(CamShot* pShot) {
    u8 nKind = pShot->bAC;

    if (nKind == 0 || (u8)(nKind - 13) <= 2U || nKind == 23) {
        return 1;
    }
    return 0;
}

// ---- end of sweep code ----
