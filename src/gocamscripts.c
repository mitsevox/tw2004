// gocamscripts.c (TW06's golf/cameras/gocamscripts.c; the CameraScript_ / CamScript_ functions): the
// camera scripts. A view's script moves the camera from shot to shot (the shots and sequences
// GoDynamicCam.c picks), keeps it above the ground and on the fairway, and aims it at the ball
// and the pin. The unit covers the file's certain core; its edges are still open.

#include "golfer.h"
#include "game.h"
#include "camera.h"
#include "unsorted/cull.h"

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
void fn_80038054(u8 a, int n, f32 f1, f32 f2);
void fn_800457B8(int nPlayer, f32 f);
f32  fn_800DC45C(f32 f);
u8   fn_80044E74(CamShot* pShot);
void fn_8003A148(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pSub, f32* pPrev,
                 f32 fTime);
void fn_8003EE68(CamScript* pScript, f32* pCam, u8 b, int nPlayer, f32 fMaxStep);
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
f32  fn_80044F58(int nPlayer, CamScript* pScript);
CamLens* fn_8001F004(void);             // the current camera's lens
f32  fn_8001EFFC(u8* pLens);            // the lens's fB0 (char.c: its parameter is u8*)
u8   fn_8004561C(void);
u8   fn_80044E2C(int n);
u8   fn_80044AA8(SurfaceType* pSurface);
void fn_8000ADC0(f32 (*pMtx)[4]);                   // identity
void fn_8000A6C8(f32 (*pSrc)[4], f32 (*pDst)[4]);   // UMemPool.c: transposes the 3x3 part
void fn_800BADB4(f32 (*pMtx)[4], f32* pIn, f32* pOut);     // a vector through a matrix
void fn_80038010(u8 a, int n, f32* pVec);
void fn_800386F0(int n, f32* pVec);
f32  fn_8003F194(CamShot* pShot, f32 fA, f32 fB, f32 fTime);
u8   Ter_CheckObjectAndHazardObstruction(f32* pPos, f32 fRadius, u8 bModels, u8 bHazards, f32 fStep,
                                         u8 bSlope, f32 fMaxSlope);   // GoTerrainCollision.c
void fn_800C7898(f32* p0, f32* p1, f32* p2, f32* p3, f32* pOut, f32 fT);    // a point on the spline
f32  fn_800C7970(CamShot* pShot, f32 f1, f32 f2, f32 f3, f32 f4, f32 f5, f32 f6);
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

// The script frame of a fly-by path (GoStaticCam.c). When the path has a spline, the camera flies
// along it (fn_80065488) by the share of the spline's length the time has reached, and at the end
// goes on to the next path's shots; without one, the camera moves between the shots like
// fn_8003E624 does.
void fn_8003EA50(int nPlayer, f32* pCam, f32* pSub, CamScript* pScript, CamShot* pShot, u8 b, f32 fTime) {
    f32 vPrev[4];
    f32 vMove[4];
    f32 vMoveTo[4];
    f32 fFov;
    FlyByPath* pPath;
    f32 fShare;
    f32 fStep;
    f32 fMoveTime = lbl_80281F78->f178;

    if (pScript->pShot == NULL) return;
    Vec_Copy(lbl_80281F78->v17C, vMoveTo);
    if (!b && !fn_80043388(pScript, pScript->pShot)
        && (gSession.nPaused != 0 || (0.0f == gSession.fFrameTime && 0.0f == fTime))) {
        if (pScript->nCamera != 0) {
            fn_8003F2E0(pScript, fTime);
        }
        return;
    }
    pPath = fn_80065424(pScript->pShot->nA4);
    if (pPath != NULL) {
        fShare = pScript->fCamTime / pPath->fLength;
        if (fShare > 1.0f) {
            fShare = 1.0f;
        }
        fn_80065488(pScript, pScript->pShot->nA4, pCam, pSub, &fFov, nPlayer, fn_800C7A9C(pPath, fShare));
        fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[0])), fFov);
        if (pScript->nCamera != 0) {
            fn_8003F2E0(pScript, fTime);
        }
        if (fShare >= 1.0f) {
            if (pScript->pNextShot != NULL && pScript->pNextShot->p40 != NULL) {
                pScript->pShot = pScript->pNextShot;
                pScript->pNextShot = pScript->pNextShot->p40;
                pScript->fCamTime = 0.0f;
                pScript->nCamera = 2;
                Vec_Copy(vMoveTo, pScript->v40);
                pScript->f90 = 0.0f;
                pScript->f94 = fMoveTime;
                pScript->fA0 = 0.0f;
                pScript->fA4 = 0.0f;
                return;
            }
            pScript->pShot = NULL;
            pScript->pNextShot = NULL;
            return;
        }
        pScript->f84 = pScript->fCamTime;
        pScript->fCamTime += fTime;
        pScript->f90 += fTime;
        pScript->f8C = pPath->fLength;
        return;
    }
    Vec_Copy(pCam, vPrev);
    fn_80064F54(pScript->pShot, nPlayer, pScript->v0);
    if (pScript->pNextShot != NULL) {
        fn_80064F54(pScript->pNextShot, nPlayer, pScript->v10);
    }
    if (pScript->pNextShot == NULL || pScript->nBC == 5 || pScript->nBC == 4) {
        Vec3Copy(pScript->v0, pCam);
        CamScript_GetLookAtPoint(pScript->pShot, nPlayer, pSub, pCam, pScript, vPrev, fTime);
        fFov = pScript->pShot->f78;
        fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[0])), fFov);
    } else {
        CamScript_SplineCameras(nPlayer, pCam, pSub, pScript, vPrev, fTime);
    }
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
    fn_80043388(pScript, pScript->pShot);   // its answer is not used
    fStep = fn_8003F064(pScript, fTime);
    pScript->f84 = pScript->fCamTime;
    pScript->fCamTime += fStep;
    pScript->f90 += fStep;
    if (0.0f != fStep) {
        pScript->bCC = 0;
    }
    if (pScript->pNextShot != NULL && pScript->fCamTime > pScript->f8C) {
        CameraScript_GoToNewScript(pScript, pScript->pNextShot, nPlayer, pCam, pSub, pShot);
        fn_80043388(pScript, pScript->pShot);   // its answer is not used
        if (fStep > 0.0f) {
            fn_80045428(vPrev, pCam, vMove);
            pScript->fD4 = (f32)fn_80009680(fn_80009744(vMove)) / fStep;
        }
    }
}

// Eases the script's ground height fD8 towards the ground under pCam (by CamTuning.fE0 a frame; with
// no ground found, fD8 stays and bE8 asks for the fairway fix). With fMaxStep 0 or more, fD8 is
// kept within it of the ground; never below the course's floor. Not while a held shot of bA8 1
// plays.
void fn_8003EE68(CamScript* pScript, f32* pCam, u8 b, int nPlayer, f32 fMaxStep) {
    CourseInfo* pCourse = fn_8000C594();
    f32 fGround;
    f32 fRate;
    f32 fStep;

    pScript->bE8 = 0;
    if (!fn_80043388(pScript, pScript->pShot) || pScript->pShot->bA8 != 1) {
        if (pCourse != NULL) {
            fGround = fn_8004D620(pCourse, pCam);
        } else {
            fGround = pScript->fD8;
        }
        if (fGround < -60000.0f) {
            fGround = fn_8004D5F0(pCourse, pCam);
            if (fGround < -60000.0f) {
                fGround = pScript->fD8;
                pScript->bE8 = 1;
            }
        }
        fRate = lbl_80281F78->fE0;
        // All three cases ease the same way (only a level fD8 is left alone in golfer state 12).
        if (!b || gPlayers[nPlayer].ball.nCollideCount > 0
            || (s8)GOLFERSTATE_GetCurrentState(nPlayer) != 12) {
            fStep = fGround - pScript->fD8;
            fStep *= fRate;
            pScript->fD8 += fStep;
        } else if (fGround > pScript->fD8) {
            fStep = fGround - pScript->fD8;
            fStep *= fRate;
            pScript->fD8 += fStep;
        } else if (fGround < pScript->fD8) {
            fStep = fGround - pScript->fD8;
            fStep *= fRate;
            pScript->fD8 += fStep;
        }
        if (fMaxStep >= 0.0f) {
            if (fGround > pScript->fD8 && fGround - pScript->fD8 > fMaxStep) {
                pScript->fD8 = fGround - fMaxStep;
            } else if (fGround < pScript->fD8 && pScript->fD8 - fGround > fMaxStep) {
                pScript->fD8 = fGround + fMaxStep;
            }
        }
        if (pScript->fD8 < -60000.0f || pScript->fD8 < fn_8000C594()->fFloor) {
            pScript->fD8 = fn_8000C594()->fFloor;
        }
    }
}

// The script's time step for a kind-0 (bAD) shot on a chain: fTime, unless the camera's speed fD4
// lies outside the spline speeds at the blend's start and now; then fn_800C7970 works out a step
// that eases between them.
f32 fn_8003F064(CamScript* pScript, f32 fTime) {
    f32 fSpeed;
    f32 fNow;
    f32 fStart;

    if (pScript == NULL || pScript->pShot == NULL || pScript->pShot->bAD != 0) return fTime;
    if (pScript->pShot->p44 == NULL || pScript->pShot->p40 == NULL) return fTime;
    fSpeed = pScript->fD4;
    fNow = fn_8003F194(pScript->pShot, pScript->fCamTime / pScript->f8C,
                       (pScript->fCamTime + fTime) / pScript->f8C, fTime);
    fStart = fn_8003F194(pScript->pShot, 0.0f, fTime / pScript->f8C, fTime);
    if (fSpeed < fStart && fSpeed > fNow) return fTime;
    if (fSpeed > fStart && fSpeed < fNow) return fTime;
    return fn_800C7970(pScript->pShot, fSpeed, fNow, pScript->pShot->f48, pScript->pShot->f4C,
                       pScript->fCamTime, fTime);
}

// The camera's speed on the spline through the shot's chain (p44, the shot, its p40 and that one's
// p40; a missing end repeats its neighbour) between shares fA and fB, over fTime; 0 without p40.
f32 fn_8003F194(CamShot* pShot, f32 fA, f32 fB, f32 fTime) {
    f32 vPrev[4];
    f32 vFrom[4];
    f32 vTo[4];
    f32 vNext[4];
    f32 vA[4];
    f32 vB[4];
    f32 vDiff[4];
    CamShot* pNext = pShot->p40;

    if (pNext == NULL) return 0.0f;
    Vec3Copy(pShot->v20, vFrom);
    Vec3Copy(pShot->p40->v20, vTo);
    if (pShot->p44 == NULL) {
        Vec3Copy(pShot->v20, vPrev);
    } else {
        Vec3Copy(pShot->p44->v20, vPrev);
    }
    if (pNext->p40 == NULL) {
        Vec3Copy(pShot->p40->v20, vNext);
    } else {
        Vec3Copy(pNext->p40->v20, vNext);
    }
    fn_800C7898(vPrev, vFrom, vTo, vNext, vA, fA);
    fn_800C7898(vPrev, vFrom, vTo, vNext, vB, fB);
    fn_80045428(vB, vA, vDiff);
    return (f32)fn_80009680(fn_80009744(vDiff)) / fTime;
}

// The script's slow-motion move (nCamera, set by fn_80063B98 and its kin): hands fn_80038010 the
// vector v40 with its [3] eased in (1), out (2), held (3) or kept from the current value (4),
// within 0..v40[3]. Once the move's time f90 passes its length f94, 1 turns into 4 and 2 into 5;
// 4 and 5 end (0) on the next frame that has time in it.
void fn_8003F2E0(CamScript* pScript, f32 fTime) {
    f32 v[4];

    Vec3Copy(pScript->v40, v);
    switch (pScript->nCamera) {
    case 1:
        v[3] = pScript->v40[3] * (pScript->f90 / pScript->f94);
        v[3] = v[3] < 0.0f ? 0.0f : (v[3] > pScript->v40[3] ? pScript->v40[3] : v[3]);
        fn_80038010(1, fn_80016D10(), v);
        break;
    case 2:
        v[3] = pScript->f94 > 0.0f ? pScript->v40[3] - pScript->v40[3] * (pScript->f90 / pScript->f94) : 0.0f;
        v[3] = v[3] < 0.0f ? 0.0f : (v[3] > pScript->v40[3] ? pScript->v40[3] : v[3]);
        fn_80038010(1, fn_80016D10(), v);
        break;
    case 3:
        v[3] = pScript->v40[3];
        v[3] = v[3] < 0.0f ? 0.0f : (v[3] > pScript->v40[3] ? pScript->v40[3] : v[3]);
        fn_80038010(1, fn_80016D10(), v);
        break;
    case 4:
        fn_800386F0(fn_80016D10(), v);
        v[3] = v[3] < 0.0f ? 0.0f : (v[3] > pScript->v40[3] ? pScript->v40[3] : v[3]);
        fn_80038010(1, fn_80016D10(), v);
        break;
    case 5:
        break;
    }
    if (fTime > 0.0f) {
        if (pScript->nCamera == 4 || pScript->nCamera == 5) {
            pScript->nCamera = 0;
        } else if (pScript->nCamera == 2 && pScript->f90 > pScript->f94) {
            pScript->nCamera = 5;
        } else if (pScript->nCamera == 1 && pScript->f90 > pScript->f94) {
            pScript->nCamera = 4;
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

// The blend's share so far: fCamTime over the length f8C, eased (half a cosine wave) when bCE is
// set or past the middle.
f32 fn_8003F790(CamScript* pScript) {
    f32 fT = pScript->fCamTime / pScript->f8C;

    if (pScript->bCE || fT > 0.5f) {
        fT = 1.0f - (0.5f * fn_80009638(PI * fT) + 0.5f);
    }
    return fT;
}

// Blend kind 13: as kind 0, but the share is how far the ball's flight has run (fn_80044EA8) over
// the blend's length f8C, 0..1, never going back (kept in fF8).
void fn_8003F7EC(int nPlayer, f32* pCam, f32* pSub, CamScript* pScript, f32* pPrev, f32 fTime) {
    f32 vMove[4];
    f32 vPos[4];
    CamShot* pShot = pScript->pShot;
    CamShot* pNext = pScript->pNextShot;
    f32 fT = fn_80044EA8(nPlayer, pScript) / pScript->f8C;
    f32 f;
    f32 f90;

    if (fT < 0.0f) {
        fT = 0.0f;
    } else if (fT > 1.0f) {
        fT = 1.0f;
    }
    if (fT < pScript->fF8) {
        fT = pScript->fF8;
    }
    pScript->fF8 = fT;
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

// Blend kind 15: as kind 13, with fn_80044F58's share of the way to the pin.
void fn_8003FAA0(int nPlayer, f32* pCam, f32* pSub, CamScript* pScript, f32* pPrev, f32 fTime) {
    f32 vMove[4];
    f32 vPos[4];
    CamShot* pShot = pScript->pShot;
    CamShot* pNext = pScript->pNextShot;
    f32 fT = fn_80044F58(nPlayer, pScript) / pScript->f8C;
    f32 f;
    f32 f90;

    if (fT < 0.0f) {
        fT = 0.0f;
    } else if (fT > 1.0f) {
        fT = 1.0f;
    }
    if (fT < pScript->fF8) {
        fT = pScript->fF8;
    }
    pScript->fF8 = fT;
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

// Blend kind 3: the camera and the point it looks at follow splines (fn_800C7480) through the
// current and next shots, with the shot before the current one (p44) and the one after the next
// (p40) as the outer points (or the shots' own when there are none); field of view, fn_800457B8's
// value, slow motion and fA8 are blended by the camera's time over the blend's length f8C.
void CamScript_SplineCameras(int nPlayer, f32* pCam, f32* pSub, CamScript* pScript, f32* pPrev, f32 fTime) {
    f32 vPrevPos[4];
    f32 vPos0[4];
    f32 vPos1[4];
    f32 vNextPos[4];
    f32 vPrevLook[4];
    f32 vLook0[4];
    f32 vLook1[4];
    f32 vNextLook[4];
    f32 fFov;
    f32 f;
    f32 f90;
    f32 fT = pScript->fCamTime / pScript->f8C;
    CamShot* pShot = pScript->pShot;
    CamShot* pNext = pScript->pNextShot;

    Vec3Copy(pScript->v0, vPos0);
    Vec3Copy(pScript->v10, vPos1);
    if (pShot->p44 == NULL) {
        Vec3Copy(pScript->v0, vPrevPos);
    } else {
        Vec3Copy(pShot->p44->v20, vPrevPos);
    }
    if (pNext->p40 == NULL) {
        Vec3Copy(pScript->v10, vNextPos);
    } else {
        Vec3Copy(pNext->p40->v20, vNextPos);
    }
    Vec3Copy(pSub, vLook0);
    CamScript_GetLookAtPoint(pShot, nPlayer, vLook0, pScript->v0, pScript, pPrev, fTime);
    Vec3Copy(pSub, vLook1);
    CamScript_GetLookAtPoint(pNext, nPlayer, vLook1, pScript->v10, pScript, pPrev, fTime);
    if (pShot->p44 == NULL) {
        Vec3Copy(vLook0, vPrevLook);
    } else {
        Vec3Copy(pSub, vPrevLook);
        CamScript_GetLookAtPoint(pShot->p44, nPlayer, vPrevLook, pCam, pScript, pPrev, fTime);
    }
    if (pNext->p40 == NULL) {
        Vec3Copy(vLook1, vNextLook);
    } else {
        Vec3Copy(pSub, vNextLook);
        CamScript_GetLookAtPoint(pNext->p40, nPlayer, vNextLook, pCam, pScript, pPrev, fTime);
    }
    fn_800C7480(vPrevPos, vPos0, vPos1, vNextPos, vPrevLook, vLook0, vLook1, vNextLook, pCam, pSub, &fFov,
                pShot->f78, pNext->f78, fT);
    fFov += fn_800DC3A4();
    if (fn_80044E74(pScript->pShot)) {
        fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[1])), fFov);
    } else {
        fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[0])), fFov);
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

// Picks the side (nD0: 1 or 2, 0 for neither) of a curved move between the two shots. For next-shot
// kinds 12 and 11 the side follows fn_800453C8; otherwise each side's curve midpoint (fn_800C7E50,
// through the halfway point of the two look-at points) is tested against the in-bounds outlines,
// and the side whose point alone is inside is taken.
void fn_80040CF0(CamScript* pScript, f32* pSub, int nPlayer, f32* pPrev, f32 fTime) {
    f32 vSide1[4];
    f32 vSide2[4];
    f32 vLook[4];
    f32 vNextLook[4];
    f32 vMid[4];
    f32 vHalf[4];
    f32 vPoint[4];
    u8 bIn1;
    u8 bIn2;

    if (pScript->nBC == 12) {
        if (fn_800453C8(nPlayer, NULL)) {
            pScript->nD0 = 2;
        } else {
            pScript->nD0 = 1;
        }
    } else if (pScript->nBC == 11) {
        if (fn_800453C8(nPlayer, NULL)) {
            pScript->nD0 = 1;
        } else {
            pScript->nD0 = 2;
        }
    } else {
        Vec3Copy(pSub, vLook);
        CamScript_GetLookAtPoint(pScript->pShot, nPlayer, vLook, pScript->v0, pScript, pPrev, fTime);
        Vec3Copy(pSub, vNextLook);
        CamScript_GetLookAtPoint(pScript->pNextShot, nPlayer, vNextLook, pScript->v10, pScript, pPrev, fTime);
        fn_80045428(vNextLook, vLook, vHalf);
        fn_8001EF34(vHalf, 0.5f, vPoint);
        fn_8004544C(vPoint, vLook, vPoint);
        Vec3Copy(vPoint, vMid);
        pScript->nD0 = 1;
        fn_800C7E50(pScript->v0, pScript->v10, vMid, 1, vSide1, 0.5f);
        pScript->nD0 = 2;
        fn_800C7E50(pScript->v0, pScript->v10, vMid, 2, vSide2, 0.5f);
        bIn1 = Ter_PointInOOBNetwork(vSide1);
        bIn2 = Ter_PointInOOBNetwork(vSide2);
        if (!bIn1) {
            if (!bIn2) {
                pScript->nD0 = 0;
            } else {
                pScript->nD0 = 2;
            }
        } else if (!bIn2) {
            pScript->nD0 = 1;
        } else {
            pScript->nD0 = 0;
        }
    }
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
            fStep = vPos[1] - pOut[1];
            fStep *= lbl_80281F78->f164;
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

// The shot's wobble at time fTime (at speed fSpeed, 0.1 at least): three sums of cosines at unrelated
// rates, each 0..1 less a half, scaled by the shot's f94 over the tuning's f11C. Nothing without f94.
void fn_800418B0(CamShot* pShot, f32* pOut, f32 fTime, f32 fSpeed) {
    f32 fX;
    f32 fY;
    f32 fT;
    f32 fZ;

    if (pShot->f94 > 0.0f) {
        if (fSpeed < 0.1f) {
            fT = 0.1f * fTime;
        } else {
            fT = fTime * fSpeed;
        }
        fX = (fn_80009638(5.0f * fT) + fn_80009638(7.0f * fT / 3.0f) + fn_80009638(fT / 5.0f) + 3.0f) / 6.0f;
        fY = (fX + fn_80009638(9.0f * fT)) / 2.0f;
        fZ = (fY + fn_80009638(11.0f * fT)) / 2.0f;
        fX -= 0.5f;
        fY -= 0.5f;
        fZ -= 0.5f;
        pOut[0] = fX * (pShot->f94 / lbl_80281F78->f11C);
        pOut[1] = fY * (pShot->f94 / lbl_80281F78->f11C);
        pOut[2] = fZ * (pShot->f94 / lbl_80281F78->f11C);
        pOut[3] = 0.0f;
    } else {
        pOut[0] = 0.0f;
        pOut[1] = 0.0f;
        pOut[2] = 0.0f;
        pOut[3] = 0.0f;
    }
}

// Turns the look-at point pSub (level, about the camera pCam) towards the aim marker
// (Player.vTargetCopy, moved by pShot's f74 and f70 when there is one) by the angle between them
// over fRate frames' worth (at least 1), keeping the goal's level distance; its height moves by
// fYShare of the way. With bClose an aim nearer than CamTuning.fF0 is pushed out; with bLimit, one
// farther than fMinDist sits at most f12C below the camera. Nothing while paused.
void CameraScript_LagAimMarker(int nPlayer, f32* pSub, f32* pCam, CamShot* pShot, u8 bClose, u8 bLimit,
                               f32 fRate, f32 fMinDist, f32 fYShare) {
    f32 vGoal[4];
    f32 vCur[4];
    f32 vDir[4];
    f32 vToGoal[4];
    f32 vFlat[4];
    f32 qTurn[4];
    f32 vAxis[4];
    f32 vAim[4];
    f32 fFrames;
    f32 fDiv;
    f32 fMin;
    f32 fOldY;
    f32 fDist;
    f32 fAngle;
    f32 fAimY;

    fFrames = 0.0f;
    if (0.0f != gSession.fFrameTime) {
        fFrames = fRate * (1.0f / (FRAME_RATE * gSession.fFrameTime));
    }
    fDiv = 1.0f;
    if (fDiv <= fFrames) {
        fDiv = fFrames;
    }
    if (0.0f != fDiv && gSession.nPaused == 0) {
        Vec3Copy(gPlayers[nPlayer].vTargetCopy, vAim);
        fOldY = pSub[1];
        Vec3Copy(vAim, vGoal);
        if (pShot != NULL) {
            fn_8004255C(vGoal, pCam, pShot->f74, pShot->f70);
        }
        fn_80045428(vGoal, pCam, vDir);
        fMin = lbl_80281F78->fF0;
        fMin *= 1.0f / fn_8001EFFC((u8*)fn_80008370(fn_80017004(gPlayers[nPlayer].nView[0])));
        fMin *= -vDir[1];
        vDir[1] = 0.0f;
        if ((f32)fn_80009680(fn_80009744(vDir)) < fMin && bClose) {
            if (0.0f != vDir[0] || 0.0f != vDir[1] || 0.0f != vDir[2]) {
                fn_800BAF04(vDir, vDir);
            }
            fn_8001EF34(vDir, fMin, vDir);
            fAimY = vAim[1];
            vDir[1] = (vGoal[1] - fAimY) * ((f32)fn_80009680(fn_80009744(vDir)) / fMin) + fAimY - pCam[1];
            fn_8004544C(vDir, pCam, vGoal);
        }
        if (bLimit) {
            fn_80045428(vGoal, pCam, vFlat);
            vFlat[1] = 0.0f;
            if ((f32)fn_80009680(fn_80009744(vFlat)) > fMinDist
                && pCam[1] - vGoal[1] > lbl_80281F78->f12C) {
                vGoal[1] = pCam[1] - lbl_80281F78->f12C;
            }
        }
        fn_80045428(pSub, pCam, vCur);
        vCur[1] = 0.0f;
        fn_80045428(vGoal, pCam, vDir);
        vDir[1] = 0.0f;
        fDist = fn_80009680(fn_80009744(vDir));
        fn_80045428(vGoal, pSub, vToGoal);  // vToGoal is not read
        if (0.0f != vCur[0] || 0.0f != vCur[1] || 0.0f != vCur[2]) {
            fn_800BAF04(vCur, vCur);
        }
        if (0.0f != vDir[0] || 0.0f != vDir[1] || 0.0f != vDir[2]) {
            fn_800BAF04(vDir, vDir);
        }
        // the dot product is taken up to three times, as a clamp macro would
        fAngle = fn_80009614(fn_8000C5FC(vCur, vDir) < -1.0f ? -1.0f
                             : (fn_8000C5FC(vCur, vDir) > 1.0f ? 1.0f : fn_8000C5FC(vCur, vDir)))
                 / fDiv;
        vec4flt_CrossProduct(vCur, vDir, vAxis);
        if (0.0f != vAxis[0] || 0.0f != vAxis[1] || 0.0f != vAxis[2]) {
            fn_800BAF04(vAxis, vAxis);
        }
        fn_8001EF34(vAxis, fAngle, vAxis);
        fn_8000923C(vAxis, qTurn);
        vCur[3] = 0.0f;
        fn_800090E4(qTurn, vCur, vDir);
        if (0.0f != vDir[0] || 0.0f != vDir[1] || 0.0f != vDir[2]) {
            fn_800BAF04(vDir, vDir);
        }
        fn_8001EF34(vDir, fDist, vDir);
        fn_8004544C(pCam, vDir, pSub);
        pSub[1] = fYShare * (vGoal[1] - fOldY) + fOldY;
    }
}

// The look-at point following the ball, one step per ball update this frame: the aim moves from
// the script's v70 towards the ball (fn_8003D9AC) at its fn_80043420 height, moved by the shot's
// f74 and f70; during a fairway fix (bCF) a steep look down is limited (as in
// CamScript_GetLookAtPoint). pOut eases towards it by a share that grows with the distance
// (CamTuning.f138, f13C) and eases in over the move (fD4, f158); its height eases in by f15C/f160
// and slows near the ground once the ball comes down. Then the aim lags by fE4 (fn_8004349C). pVec
// is not read.
void fn_80041EA8(int nPlayer, f32* pOut, f32* pCam, CamShot* pShot, CamScript* pScript, f32* pVec,
                 f32 fTime) {
    f32 vMove[4];
    f32 vAim[4];
    f32 vBall[4];
    f32 vLast[4];
    f32 vDir[4];
    f32 vStep[4];
    int nUpdates;
    int i;
    f32 fBase;
    f32 fDrop;
    f32 fLen;
    f32 fLimit;
    f32 fShare;
    f32 fRate;

    Vec3Copy(pOut, vLast);  // vLast is not read
    nUpdates = GameEffects_BallUpdatesThisFrame(nPlayer);
    if (nUpdates == 0) return;
    fn_8003D9AC(pScript, pShot, nPlayer, vBall, 0);
    for (i = 0; i < nUpdates; i++) {
        fn_80045428(vBall, pScript->v70, vStep);
        fn_8001EF34(vStep, (f32)(i + 1) / (f32)nUpdates, vStep);
        fn_8004544C(pScript->v70, vStep, vAim);
        vAim[1] = fn_80043420(nPlayer, pScript, vAim, pCam, pShot);
        fn_8004255C(vAim, pCam, pShot->f74, pShot->f70);
        if (pScript->bCF) {
            fn_80045428(vAim, pCam, vDir);
            fBase = pCam[1];
            if (pScript->pShot != NULL) {
                fBase = pCam[1] - pScript->pShot->f68;
                fDrop = vDir[1] + pScript->pShot->f68;
            }
            vDir[1] = 0.0f;
            fLen = fn_80009680(fn_80009744(vDir));
            // EA bug: fDrop is not set when the script has no current shot
            if (0.0f != fLen && fDrop / fLen < -0.2f) {
                fLimit = fBase - 0.2f * fLen;
                if (fLen > lbl_80281F78->f128) {
                    vAim[1] = fLimit;
                } else {
                    vAim[1] = vAim[1] + fLen * (fLimit - vAim[1]) / lbl_80281F78->f128;
                }
            }
        }
        fn_80045428(vAim, pOut, vMove);
        fRate = fn_80009680(fn_80009744(vMove));
        fRate = lbl_80281F78->f138 * (fRate / lbl_80281F78->f13C);
        fShare = fRate < 0.0f ? 0.0f : (fRate > 1.0f ? 1.0f : fRate);
        if (pScript->f98 < lbl_80281F78->fD4) {
            fRate = 1.0f - (f32)fn_80009680(pScript->f98 / lbl_80281F78->fD4) * (1.0f - fShare);
        } else {
            fRate = fShare < 0.0f ? 0.0f : (fShare > lbl_80281F78->f138 ? lbl_80281F78->f138 : fShare);
        }
        if (pScript->f88 < lbl_80281F78->f158) {
            fRate *= pScript->f88 / lbl_80281F78->f158;
        }
        if (pScript->f88 < lbl_80281F78->f15C) {
            vMove[1] *= powf(pScript->f88 / lbl_80281F78->f15C, lbl_80281F78->f160)
                        * (fTime / (1.0f / FRAME_RATE));
        }
        if (gPlayers[nPlayer].ball.bHitTopArc) {
            if (gPlayers[nPlayer].ball.fHeight <= 0.15f) {
                vMove[1] *= lbl_80281F78->f140;
            } else if (!(gPlayers[nPlayer].ball.fHeight > 10.0f)) {
                vMove[1] *= (1.0f - lbl_80281F78->f140) * ((gPlayers[nPlayer].ball.fHeight - 0.15f) / 10.0f)
                            + lbl_80281F78->f140;
            }
        }
        fn_8001EF34(vMove, fRate, vMove);
        fn_8004544C(vMove, pOut, pOut);
        Vec3Copy(pOut, vLast);
    }
    if (pScript->bCF) {
        fn_8004349C(nPlayer, pCam, pOut, vAim, pScript, lbl_80281F78->fE4);
    } else {
        fn_8004349C(nPlayer, pCam, pOut, gPlayers[nPlayer].ball.vPos, pScript, lbl_80281F78->fE4);
    }
}

// Eases the look-at point pOut towards pTarget (moved by the shot's f74 up and f70 sideways, the
// other way round for fn_800453C8), level and in height separately, by CamTuning.f144 and f148 a
// frame; slower when it is close. Right after a cut to the next shot it jumps there. Without a
// next shot (or with blend 5) the aim lags by fLag (fn_8004349C). Only on frames with ball updates
// or with fn_800C714C.
void fn_800422C4(int nPlayer, f32* pOut, f32* pCam, f32* pTarget, CamShot* pShot, CamScript* pScript,
                 f32 fTime, f32 fLag) {
    f32 vMove[4];
    f32 vAim[4];
    f32 vSpan[4];
    f32 fFrames;
    f32 fDist;
    f32 fRange;
    f32 fDy;
    f32 fRate;
    f32 fNear;

    if (GameEffects_BallUpdatesThisFrame(nPlayer) != 0 || fn_800C714C()) {
        Vec3Copy(pTarget, vAim);
        if (fn_800453C8(nPlayer, pShot)) {
            fn_8004255C(vAim, pCam, pShot->f74, -pShot->f70);
        } else {
            fn_8004255C(vAim, pCam, pShot->f74, pShot->f70);
        }
        fn_80045428(vAim, pOut, vMove);
        vMove[1] = 0.0f;
        fDist = fn_80009680(fn_80009744(vMove));
        fFrames = fTime / (1.0f / FRAME_RATE);
        fRate = lbl_80281F78->f144 * fFrames;
        fn_80045428(vAim, pCam, vSpan);
        fRange = fn_80009680(fn_80009744(vSpan));
        fRange *= pShot->f78 / DEG(60.0f);
        fNear = lbl_80281F78->f134 * fRange;
        if (fDist < fNear) {
            fRate *= fRange / fNear;    // EA bug: fDist was likely meant (this is always 1 / f134)
        }
        if (pScript->pNextShot != NULL && pScript->nBC != 5 && pShot == pScript->pNextShot
            && 0.0f == pScript->fCamTime) {
            fRate = 1.0f;
        }
        fn_8001EF34(vMove, fRate, vMove);
        fn_8004544C(vMove, pOut, pOut);
        fn_80045428(vAim, pOut, vMove);
        fDy = vAim[1] - pOut[1];
        fRate = lbl_80281F78->f148 * fFrames;
        if (fabsf(fDy) < fNear) {
            fRate *= fabsf(fDy) / fNear;
        }
        if (pScript->pNextShot != NULL && pScript->nBC != 5 && pShot == pScript->pNextShot
            && 0.0f == pScript->fCamTime) {
            fRate = 1.0f;
        }
        fDy *= fRate;
        pOut[1] += fDy;
        if (pScript->pNextShot == NULL || pScript->nBC == 5) {
            fn_8004349C(nPlayer, pCam, pOut, vAim, pScript, fLag);
        }
    }
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

// Records the camera as it is now into pShot ("ON THE FLY CAM"): positioned at pCam, looking at
// pSub (bAC 24), with the lens's field of view (less the letterbox's change, except with bView1:
// the player's second view) and the current shot's timing and slow motion. The script is left with
// no next shot.
void CameraScript_RecordCurrentCam(CamShot* pShot, f32* pCam, f32* pSub, int nPlayer, CamScript* pScript,
                                   u8 bView1) {
    char szName[] = "ON THE FLY CAM";

    strcpy(pShot->szName, szName);
    pShot->bA8 = 0;
    Vec3Copy(pCam, pShot->v20);
    pShot->p40 = NULL;
    pScript->pNextShot = NULL;
    pScript->f8C = 0.0f;
    pShot->f4C = 2.0f;
    pScript->nBC = 0;
    pShot->bAD = 4;
    pShot->bAC = 24;
    Vec3Copy(pSub, pShot->v30);
    pShot->f70 = 0.0f;
    pShot->f74 = 0.0f;
    pShot->bAA = 1;
    if (bView1) {
        pShot->f78 = fn_80014278(fn_80008370(fn_80016CFC(gPlayers[nPlayer].nView[1])->pCamera));
    } else {
        pShot->f78 = fn_80014278(fn_80008370(fn_80016CFC(gPlayers[nPlayer].nView[0])->pCamera));
        pShot->f78 -= fn_800DC3A4();
    }
    pShot->f7C = pShot->f78;
    pShot->f9C = pScript->fA8;
    if (pScript->pShot != NULL) {
        pShot->bAD = pScript->pShot->bAD;
        pShot->f8C = pScript->pShot->f8C;
        pShot->f90 = pScript->pShot->f90;
        pShot->f88 = pScript->pShot->f88;
        pShot->nA0 = pScript->pShot->nA0;
        pShot->f68 = pScript->pShot->f68;
        pShot->f68 = pScript->pShot->f6C;  // EA bug: f68 is stored twice; f6C was likely meant
    } else {
        pShot->f8C = 0.0f;
        pShot->f90 = 0.0f;
        pShot->f88 = 0.0f;
        pShot->nA0 = 1;
        pShot->f68 = 0.0f;
        pShot->f68 = 1000.0f;   // EA bug: likewise (CamScript_PutBackOnFairway puts 1000 in f6C)
    }
    pShot->f94 = 0.0f;
    pShot->f98 = 0.0f;
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

// The script is on its first frame (fCamTime 0, no blend running) for pShot: not for the current
// shot while a next shot waits that is not its follow-on, nor for that next shot when it shares
// the current shot's bB1.
u8 fn_80043388(CamScript* pScript, CamShot* pShot) {
    if (0.0f != pScript->fCamTime) return 0;
    if (pScript->bCC) return 0;
    if (pScript->pNextShot != NULL && pScript->pShot->p40 != pScript->pNextShot && pScript->pShot == pShot) {
        return 0;
    }
    if (pScript->pNextShot != NULL && pScript->pNextShot == pShot && pScript->pShot->p40 != pScript->pNextShot
        && pScript->pShot->bB1 == pScript->pNextShot->bB1) {
        return 0;
    }
    return 1;
}

// pPos's height, brought down by CamTuning.fD0 times how far it is above the shot's f6C over the
// script's ground height fD8 (times the lens's fB0).
f32 fn_80043420(int nPlayer, CamScript* pScript, f32* pPos, f32* pCam, CamShot* pShot) {
    f32 fY = pPos[1];
    f32 fAbove = pPos[1] - pScript->fD8;
    f32 fDrop;

    if (fAbove > pShot->f6C) {
        fDrop = lbl_80281F78->fD0 * (fAbove - pShot->f6C);
        fDrop *= fn_8001EFFC((u8*)fn_8001F004());
        fY -= fDrop;
    }
    return fY;
}

// Lags the look-at point pOut behind pTarget as seen from pCam: when the angle between the two
// directions is more than the script's lag angle fDC, pOut is turned towards pTarget (about their
// common perpendicular) until it is fDC away; otherwise fDC takes the angle once it passes fLag
// (scaled by the lens's fB0). nPlayer is not read.
void fn_8004349C(int nPlayer, f32* pCam, f32* pOut, f32* pTarget, CamScript* pScript, f32 fLag) {
    f32 vToTarget[4];
    f32 vTargetDir[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    f32 vToOut[4];
    f32 vOutDir[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    f32 vTurned[4];
    f32 vAxis[4];
    f32 mAlign[4][4];
    f32 mBack[4][4];
    f32 mTurn[4][4];
    f32 fDot;
    f32 fAngle;
    f32 fLen;
    f32 fInv;
    f32 fCos;
    f32 fSin;

    fLag *= fn_8001EFFC((u8*)fn_8001F004());
    fn_80045428(pTarget, pCam, vToTarget);
    fn_80045428(pOut, pCam, vToOut);
    if (0.0f == vToTarget[0] && 0.0f == vToTarget[1] && 0.0f == vToTarget[2]) return;
    if (0.0f == vToOut[0] && 0.0f == vToOut[1] && 0.0f == vToOut[2]) return;
    fn_800BAF04(vToTarget, vTargetDir);
    fn_800BAF04(vToOut, vOutDir);
    fDot = fn_8000C5FC(vTargetDir, vOutDir);
    if (fDot < -1.0f) {
        fDot = -1.0f;
    } else if (fDot > 1.0f) {
        fDot = 1.0f;
    }
    fAngle = fn_80009614(fDot);
    if (fabsf(fAngle) > pScript->fDC) {
        fAngle -= pScript->fDC;
        vec4flt_CrossProduct(vOutDir, vTargetDir, vAxis);
        if (0.0f == vAxis[0] && 0.0f == vAxis[1] && 0.0f == vAxis[2]) return;
        fn_800BAF04(vAxis, vAxis);
        fLen = fn_80009680(vAxis[1] * vAxis[1] + vAxis[2] * vAxis[2]);
        if (0.0f == fLen) return;
        // mAlign turns the axis onto x; mTurn turns by fAngle about it there; mBack turns back
        fInv = 1.0f / fLen;
        mAlign[0][0] = fLen;
        mAlign[0][1] = 0.0f;
        mAlign[0][2] = vAxis[0];
        mAlign[0][3] = 0.0f;
        mAlign[1][0] = fInv * -(vAxis[0] * vAxis[1]);
        mAlign[1][1] = vAxis[2] * fInv;
        mAlign[1][2] = vAxis[1];
        mAlign[1][3] = 0.0f;
        mAlign[2][0] = fInv * -(vAxis[0] * vAxis[2]);
        mAlign[2][1] = -vAxis[1] * fInv;
        mAlign[2][2] = vAxis[2];
        mAlign[2][3] = 0.0f;
        mAlign[3][0] = 0.0f;
        mAlign[3][1] = 0.0f;
        mAlign[3][2] = 0.0f;
        mAlign[3][3] = 1.0f;
        fCos = fn_80009638(fAngle);
        fSin = fn_80009680(1.0f - fCos * fCos);
        mTurn[0][0] = fCos;
        mTurn[0][1] = fSin;
        mTurn[0][2] = 0.0f;
        mTurn[0][3] = 0.0f;
        mTurn[1][0] = -fSin;
        mTurn[1][1] = fCos;
        mTurn[1][2] = 0.0f;
        mTurn[1][3] = 0.0f;
        mTurn[2][0] = 0.0f;
        mTurn[2][1] = 0.0f;
        mTurn[2][2] = 1.0f;
        mTurn[2][3] = 0.0f;
        mTurn[3][0] = 0.0f;
        mTurn[3][1] = 0.0f;
        mTurn[3][2] = 0.0f;
        mTurn[3][3] = 1.0f;
        fn_8000ADC0(mBack);
        fn_8000A6C8(mAlign, mBack);
        fn_800BADB4(mAlign, vToOut, vTurned);
        fn_800BADB4(mTurn, vTurned, vTurned);
        fn_800BADB4(mBack, vTurned, vTurned);
        fn_8004544C(pCam, vTurned, pOut);
    } else if (fabsf(fAngle) > fLag) {
        pScript->fDC = fAngle;
    }
}

// The shot is the default swing camera: kind (bAD) 3 or 29..33, and the camera looks along the aim
// (the level directions from Player.vBall to the aim and from the camera to the ball agree past
// CamTuning.fFC).
u8 CameraScript_IsDefaultSwingCam(CamShot* pShot, int nPlayer, f32* pCam) {
    f32 vAim[4];
    f32 vCam[4];
    f32* pBall;

    if (pShot == NULL) return 0;
    if (pShot->bAD != 3 && (pShot->bAD < 29 || pShot->bAD > 33)) return 0;
    pBall = gPlayers[nPlayer].vBall;
    fn_80045428(gPlayers[nPlayer].vTargetCopy, pBall, vAim);
    vAim[1] = 0.0f;
    if (0.0f != vAim[0] || 0.0f != vAim[1] || 0.0f != vAim[2]) {
        fn_800BAF04(vAim, vAim);
    }
    fn_80045428(pBall, pCam, vCam);
    vCam[1] = 0.0f;
    if (0.0f != vCam[0] || 0.0f != vCam[1] || 0.0f != vCam[2]) {
        fn_800BAF04(vCam, vCam);
    }
    if (fn_8000C5FC(vCam, vAim) > lbl_80281F78->fFC) return 1;
    return 0;
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

// The camera at pCam is in the way: inside an object, or (with the flagstick in) close to the pin
// and not far above it (CamTuning.f120, f124).
u8 fn_800439E4(f32* pCam, int nPlayer) {
    f32 vDiff[4];
    int nPin = Game_CurrentPinSet();
    CourseInfo* pCourse = fn_8000C594();
    u8 bBlocked;
    f32 fDist;

    if (pCourse == NULL) return 0;
    bBlocked = Ter_CheckObjectAndHazardObstruction(pCam, 0.0f, 1, 0, 0.0f, 0, 0.0f);
    if (!bBlocked && !fn_80016CFC(gPlayers[nPlayer].nView[0])->bFlagOut) {
        fn_80045428(pCam, &pCourse->pin[nPin].x, vDiff);
        vDiff[1] = 0.0f;
        fDist = fn_80009680(fn_80009744(vDiff));
        fDist *= fn_8001EFFC((u8*)fn_8001F004());
        if (fDist < lbl_80281F78->f120
            && pCam[1] - pCourse->pin[nPin].y < lbl_80281F78->f124) {
            bBlocked = 1;
        }
    }
    return bBlocked;
}

// Puts the camera back on the fairway (CamScript_PutBackOnFairway) when the script asks for it
// (bE8), or, once the camera has run CamTuning.fEC (or b) and the move is past half way (f98), when
// the camera has left the hole's outline (fn_80069498) while the ball's next step stays inside it.
// Only for a next shot (or, without one, a current shot) of bAD 4, and not while fn_800C6D9C holds.
void CamScript_CheckOutOfBounds(CamScript* pScript, f32* pCam, f32* pSub, int nPlayer, CamShot* pSaved,
                                f32* pPrev, u8 b) {
    f32 vNext[4];
    TNetwork* pNet;
    u8 bEarly = 0;

    if (fn_8000C594() == NULL) return;
    if (!b && pScript->fCamTime < lbl_80281F78->fEC) {
        bEarly = 1;
    }
    if (fn_800C6D9C()) return;
    if (pScript->pShot != NULL) {
        if (pScript->pNextShot != NULL) {
            if (pScript->pNextShot->bAD != 4) return;
        } else if (pScript->pShot->bAD != 4) {
            return;
        }
    }
    if (!bEarly && (b || pScript->f98 > 0.5f)) {
        pNet = fn_80069498();
        if (pNet != NULL && fn_8000C140(pCam, pNet, pNet->nNumNodes) == 0) {
            fn_8004544C(gPlayers[nPlayer].ball.vVel, gPlayers[nPlayer].ball.vPos, vNext);
            if (!fn_8000C4E0(gPlayers[nPlayer].ball.vPos, vNext, pNet, pNet->nNumNodes)) {
                CamScript_PutBackOnFairway(pScript, pCam, pSub, nPlayer, pSaved, pPrev);
            }
        }
    }
    if (pScript->bE8) {
        CamScript_PutBackOnFairway(pScript, pCam, pSub, nPlayer, pSaved, pPrev);
    }
}

// With the flagstick in and no fairway fix running (bCF), a camera close to the pin (level
// distance times the lens's fB0 under CamTuning.f120) is raised towards f124 above it, more the
// closer it is.
void fn_800441E4(CamScript* pScript, f32* pCam, f32* pSub, int nPlayer, CamShot* pSaved, f32* pPrev) {
    f32 vDiff[4];
    int nPin = Game_CurrentPinSet();
    CourseInfo* pCourse = fn_8000C594();
    f32 fDist;
    f32 fAbove;

    if (pCourse == NULL) return;
    if (pScript->bCF) return;
    if (fn_80016CFC(gPlayers[nPlayer].nView[0])->bFlagOut) return;
    fn_80045428(pCam, &pCourse->pin[nPin].x, vDiff);
    vDiff[1] = 0.0f;
    fDist = fn_80009680(fn_80009744(vDiff));
    fDist *= fn_8001EFFC((u8*)fn_8001F004());
    if (fDist < lbl_80281F78->f120) {
        if (pCam[1] - pCourse->pin[nPin].y < lbl_80281F78->f124) {
            fAbove = pCam[1] - pCourse->pin[nPin].y;
            pCam[1] += (lbl_80281F78->f124 - fAbove) * (1.0f - fDist / lbl_80281F78->f120);
        }
    }
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

// Moves the camera to a spot on the fairway (fn_80043C74) and makes pShot a still shot there: it
// looks straight at its target (bAC 0) for 1000 seconds at the tuning's field of view (f114), with no
// wobble or slow motion. The script cuts to it (blend 5) with the ground height fD8 at the spot (the
// course's floor at least), and the lens takes the new field of view at once.
void CamScript_PutBackOnFairway(CamScript* pScript, f32* pCam, f32* pSub, int nPlayer, CamShot* pShot,
                                f32* pPrev) {
    f32 fHeight;
    f32 fFov;

    fn_80043C74(pScript, pCam, pSub, nPlayer, pShot, pPrev, &fHeight);
    CameraScript_RecordCurrentCam(pShot, pCam, pSub, nPlayer, pScript, 0);
    pScript->pShot = pShot;
    pScript->pNextShot = NULL;
    pScript->fCamTime = 0.0f;
    pScript->pShot->bAC = 0;
    pScript->pShot->f68 = lbl_80281F78->f10C;
    pScript->pShot->f6C = 1000.0f;
    pScript->pShot->f4C = 2.0f;
    pScript->pShot->f8C = 0.0f;
    pScript->pShot->f90 = 0.0f;
    pScript->pShot->f88 = 0.0f;
    pScript->pShot->nA0 = 1;
    pScript->pShot->bB2 = 0;
    pScript->pShot->f94 = 0.0f;
    pScript->pShot->f78 = lbl_80281F78->f114;
    pScript->pShot->f7C = pScript->pShot->f78;
    pScript->nE0 = 25;
    pScript->fD8 = fHeight;
    if (pScript->fD8 < -60000.0f || pScript->fD8 < fn_8000C594()->fFloor) {
        pScript->fD8 = fn_8000C594()->fFloor;
    }
    CameraScript_InterpToNewScript(pScript, pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f, 25, 0.0f);
    CamScript_GetLookAtPoint(pScript->pShot, nPlayer, pSub, pCam, pScript, pPrev, 0.0f);
    fFov = pScript->pShot->f78 + fn_800DC3A4();
    fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[0])), fFov);
    pScript->bCF = 1;
    pScript->fCamTime = 0.001f;
}

// The fairway camera: narrows the current shot's field of view down to CamTuning.f110, and once the
// camera has run f104, is at least f100 (level) from the spot fn_80044768 picks by the ball, and
// the ball is not heading back past it (f108), cuts the shot to that spot (f10C above the ground
// there) with the same field of view.
void CamScript_UpdateFairwayCam(CamScript* pScript, f32* pCam, f32* pSub, int nPlayer) {
    f32 vSpot[4];
    f32 vToSpot[4];
    f32 vFromBall[4];
    f32 vDir[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    f32 vVel[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    f32 fFov;
    f32 fDist;
    f32 fDot;
    f32 fHeight;

    if (pScript->pShot != NULL && pScript->pShot->f78 > lbl_80281F78->f110) {
        pScript->pShot->f78 -= lbl_80281F78->f118;
        pScript->pShot->f7C = pScript->pShot->f78;
    }
    if (pScript->fCamTime > lbl_80281F78->f104) {
        fFov = pScript->pShot->f78;
        fn_80044768(gPlayers[nPlayer].ball.vPos, vSpot);
        fn_80045428(pCam, vSpot, vToSpot);
        vToSpot[1] = 0.0f;
        fDist = fn_80009680(fn_80009744(vToSpot));
        fn_80045428(pCam, gPlayers[nPlayer].ball.vPos, vFromBall);
        vFromBall[1] = 0.0f;
        if (0.0f != vFromBall[0] || 0.0f != vFromBall[1] || 0.0f != vFromBall[2]) {
            fn_800BAF04(vFromBall, vDir);
        } else {
            vDir[0] = 0.0f;
            vDir[1] = 0.0f;
            vDir[2] = 0.0f;
        }
        Vec3Copy(gPlayers[nPlayer].ball.vVel, vVel);
        vVel[1] = 0.0f;
        if (0.0f != vVel[0] || 0.0f != vVel[1] || 0.0f != vVel[2]) {
            fn_800BAF04(vVel, vVel);
        }
        fDot = fn_8000C5FC(vDir, vVel);
        if (fDist < lbl_80281F78->f100) return;
        if (fDot > lbl_80281F78->f108) return;
        fHeight = Terrain_HeightAt(vSpot, NULL);
        if (!(fHeight < -60000.0f) && pScript->pShot != NULL) {
            Vec3Copy(vSpot, pScript->pShot->v20);
            pScript->pShot->v20[1] = fHeight + lbl_80281F78->f10C;
            pScript->pShot->f78 = fFov;
            pScript->pShot->f7C = pScript->pShot->f78;
            CameraScript_InterpToNewScript(pScript, pScript->pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f, 25,
                                           0.0f);
            Vec3Copy(pScript->pShot->v20, pCam);
            CamScript_GetLookAtPoint(pScript->pShot, nPlayer, pSub, pCam, pScript, pCam, 0.0f);
        }
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

// The height of the ground the camera would stand on at pPos (0 with no course, TER_NO_GROUND with
// no ground at all), and its surface in ppSurface. From the top down (starting just above the
// highest layer), it takes the first standing surface (fn_80044AA8) with a gap of more than
// CamTuning.f130 above it; a layer under another kind of surface (water, say) starts a region
// whose top counts unless a standing surface is found below it. Surface 149 and, on course 7's
// hole 2, anything above 10 are passed over; there a region's lower standing surface wins.
f32 Terrain_HeightAt(f32* pPos, SurfaceType** ppSurface) {
    f32 vPos[4];
    SurfaceType* aSurfaces[20];
    f32 aHeights[20];
    u8 bRegion = 0;
    int nLower = -1;
    CourseInfo* pCourse;
    u32 nHeights;
    u32 i;
    int nIdx;
    int nTop;
    int j;
    f32 fCeiling;
    f32 fBest;
    f32 fTop;
    f32 fLower;
    f32 fLast = 10000000.0f;

    pCourse = fn_8000C594();
    if (pCourse == NULL) return 0.0f;
    Vec3Copy(pPos, vPos);
    nHeights = fn_8004DCC4(pCourse, vPos, aSurfaces, aHeights, 20);
    if (nHeights == 0) return TER_NO_GROUND;
    if (nHeights == 1) {
        fTop = aHeights[0];
        nTop = 0;
    } else {
        fTop = aHeights[0];
        nIdx = 0;
        nTop = 0;
        for (j = 1; j < (int)nHeights; j++) {
            if (aHeights[j] > fTop) {
                fTop = aHeights[j];
                nTop = j;
                nIdx = j;
            }
        }
        fCeiling = 0.1f + (fTop + lbl_80281F78->f130);
        for (i = 0; i < nHeights; i++) {
            fBest = -10000000.0f;
            for (j = 0; j < (int)nHeights; j++) {
                if (aHeights[j] < fCeiling && aHeights[j] > fBest) {
                    nIdx = j;
                    fBest = aHeights[j];
                }
            }
            // fake match: the row number from the addresses as unsigned integers; pointer
            // subtraction (or the u8* spelling) swaps two registers
            if ((int)(((uptr)aSurfaces[nIdx] - (uptr)gSurfaceTypes) / sizeof(SurfaceType)) != 149
                && !(Game_GetCourse() == 7 && fn_80015464() == 2 && fBest > 10.0f)) {
                if (fn_80044AA8(aSurfaces[nIdx])) {
                    if (!bRegion && fLast - aHeights[nIdx] > lbl_80281F78->f130) {
                        fTop = aHeights[nIdx];
                        nTop = nIdx;
                        break;
                    }
                    // EA bug: fLower is read before it is set when no region has started yet
                    if (fLast - aHeights[nIdx] > lbl_80281F78->f130 && fLower < aHeights[nIdx]) {
                        fLower = aHeights[nIdx];
                        nLower = nIdx;
                    }
                } else if (bRegion) {
                    bRegion = 0;
                } else {
                    fLower = -100000000.0f;
                    fTop = aHeights[nIdx];
                    nTop = nIdx;
                    bRegion = 1;
                    nLower = -1;
                }
                fLast = aHeights[nIdx];
            }
            fCeiling = aHeights[nIdx];
        }
    }
    if (Game_GetCourse() == 7 && fn_80015464() == 2 && bRegion && nLower >= 0) {
        nTop = nLower;
        fTop = fLower;
    }
    if (ppSurface != NULL) {
        *ppSurface = aSurfaces[nTop];
    }
    return fTop;
}

// The surface is ground the camera stands on: classes 1..12 but 9 and 10, and 18.
u8 fn_80044AA8(SurfaceType* pSurface) {
    u32 nClass;

    if (pSurface == NULL) {
        return 0;
    }
    nClass = pSurface->nClass;
    if (nClass == 10) {
        return 0;
    }
    if (nClass == 9) {
        return 0;
    }
    if (nClass >= 1 && nClass <= 12) {
        return 1;
    }
    if (nClass == 18) {
        return 1;
    }
    return 0;
}

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

// Keeps the camera at pNew at least fClearance above the ground under it (the highest ground below
// it, else the lowest above). With bCheckPath, when the move from pOld crosses the ground (other
// than the one surface fn_80044E2C excuses), the ground just under or over the crossing counts
// instead, and the answer is 1. pbFound: any ground under pNew at all (none answers 1); pfGround:
// the ground height used; pbRaised: pNew was raised. nPlayer is not read.
u8 CamScript_KeepAboveGround(int nPlayer, f32* pNew, f32* pOld, u8 bCheckPath, u8* pbFound, f32* pfGround,
                             u8* pbRaised, f32 fClearance) {
    f32 vHit[4];
    f32 vNormal[4];
    SurfaceType* aSurfaces[20];
    f32 aHeights[20];
    TerObject* pObj;
    CourseInfo* pCourse = fn_8000C594();
    u8 bCrossed = 0;
    u32 nHeights;
    u8 bHit;
    int nSurface;
    f32 fBelow;
    f32 fAbove;
    f32 fGround;

    if (pbRaised != NULL) {
        *pbRaised = 0;
    }
    if (pCourse == NULL) return 0;
    pCourse = fn_8000C594();    // fetched a second time (two calls in the original)
    nHeights = fn_8004DCC4(pCourse, pNew, aSurfaces, aHeights, 20);
    if (nHeights == 0) {
        if (pbFound != NULL) {
            *pbFound = 0;
        }
        return 1;
    }
    if (pbFound != NULL) {
        *pbFound = 1;
    }
    fBelow = fn_80044B0C(aHeights, nHeights, pNew[1]);
    fAbove = fn_80044B70(aHeights, nHeights, pNew[1]);
    if (bCheckPath) {
        bHit = Ter_CheckForGroundCollision(pCourse, pOld, pNew, vHit, vNormal, aSurfaces, &pObj);
        if (bHit) {
            nSurface = ((u8*)aSurfaces[0] - (u8*)gSurfaceTypes) / sizeof(SurfaceType);
        }
        if (bHit && !fn_80044E2C(nSurface)) {
            bCrossed = 1;
            if (vNormal[1] > 0.0f) {
                vHit[1] -= 0.001f;
                fGround = fn_80044B70(aHeights, nHeights, vHit[1]);
            } else {
                vHit[1] += 0.1f;
                fGround = fn_80044B70(aHeights, nHeights, vHit[1]);
            }
            if (fGround < -60000.0f) {
                fGround = fn_80044B0C(aHeights, nHeights, vHit[1]);
            }
        } else {
            fGround = fBelow;
            if (fBelow < -60000.0f) {
                fGround = fAbove;
            }
        }
    } else {
        fGround = fBelow;
        if (fBelow < -60000.0f) {
            fGround = fAbove;
        }
    }
    if (!(fGround < -60000.0f) && pNew[1] - fGround < fClearance) {
        pNew[1] = fGround + fClearance;
        if (pbRaised != NULL) {
            *pbRaised = 1;
        }
    }
    if (pfGround != NULL) {
        *pfGround = fGround;
    }
    return bCrossed;
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

// How far the ball's flight has run: 1 less its distance to the landing estimate (v50) over the
// shot's start's; 1 when the start is on it.
f32 fn_80044EA8(int nPlayer, CamScript* pScript) {
    f32 vStart[4];
    f32 vBall[4];
    f32 fStart;
    f32 fBall;

    fn_80045428(pScript->v50, gPlayers[nPlayer].ball.vStart, vStart);
    fn_80045428(pScript->v50, gPlayers[nPlayer].ball.vPos, vBall);
    fStart = fn_80009680(fn_80009744(vStart));
    fBall = fn_80009680(fn_80009744(vBall));
    if (fStart > 0.0f) {
        return 1.0f - fBall / fStart;
    }
    return 1.0f;
}

// How much of the way from the shot's start to the pin the ball has covered, over the ground
// (0 at the start, 1 at the pin); 0 without a hole loaded. The script is not read (fn_8003FAA0
// passes it, as it does to fn_80044EA8).
f32 fn_80044F58(int nPlayer, CamScript* pScript) {
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

// Whether something would stand between the camera and the golfer if the script cut to pShot: a
// copy of the script is cut to it and run one frame, and the ground is tested from that camera to
// 1 above Player.vBall.
u8 CameraScript_WillGolferBeOccludedInThisView(int nPlayer, CamShot* pShot, CamScript* pScript) {
    CourseInfo* pCourse = fn_8000C594();
    CamScript script;
    CamShot shot;
    CamShot shotSaved;
    f32 vHit[4];
    f32 vNormal[4];
    f32 vCam[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    f32 vSub[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    f32 vGolfer[4];
    TerObject* pObj;
    SurfaceType* pSurface;

    if (pCourse == NULL) return 0;
    Mem_cpy(&shot, pShot, sizeof(CamShot));
    Mem_cpy(&script, pScript, sizeof(CamScript));
    CameraScript_InterpToNewScript(&script, &shot, nPlayer, vCam, vSub, 5, 0.0f, 100.0f, 25, 0.0f);
    fn_8003DCE8(nPlayer, vCam, vSub, &script, &shotSaved, 0, 0.0f);
    Vec_Copy(gPlayers[nPlayer].vBall, vGolfer);
    vGolfer[1] += 1.0f;
    return Ter_CheckForGroundCollision(pCourse, vCam, vGolfer, vHit, vNormal, &pSurface, &pObj) != 0;
}

// Whether a pShot of fn_8003DC78's kinds must be passed over (1) for the player: always for a CPU
// player, a current shot outside fn_8004562C's kinds, b10, a ball coming down below 5, one that
// has bounced or is within 40 yards of the pin; a tee shot unless it is a full (0.9) kind-1 shot at
// a par 4 or 5 with a club below 9 aimed at surface 14; otherwise unless GameBreaker is on or the
// aim is at water (16). Never without a club (25).
u8 fn_800451A8(CamScript* pScript, CamShot* pShot, int nPlayer) {
    if (!fn_8003DC78(pShot)) return 0;
    if (gPlayers[nPlayer].nClub == 25) return 0;
    if (Player_IsCPU(nPlayer)) return 1;
    if (pScript->pShot != NULL && !fn_8004562C(pScript->pShot)) return 1;
    if (fn_8004561C()) return 1;
    if (gPlayers[nPlayer].ball.fHeight < 5.0f && gPlayers[nPlayer].ball.vVel[1] < 0.0f) return 1;
    if (gPlayers[nPlayer].ball.b99) return 1;
    if (gPlayers[nPlayer].ball.nCollideCount > 0) return 1;
    if (fn_800D0478(nPlayer) < 40.0f) return 1;
    if (gPlayers[nPlayer].ball.nLie == 0) {
        if (fn_800D2AD8(fn_80015464()) != 4 && fn_800D2AD8(fn_80015464()) != 5) return 1;
        if (gPlayers[nPlayer].nShotKind != 1) return 1;
        if (gPlayers[nPlayer].nSurface != 14) return 1;
        if (gPlayers[nPlayer].nClub >= 9) return 1;
        if (gPlayers[nPlayer].fPower < 0.9f) return 1;
    } else if (!fn_8004560C() && gPlayers[nPlayer].nSurface != 16) {
        return 1;
    }
    return 0;
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
