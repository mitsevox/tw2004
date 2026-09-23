// GoGolfCam.c (EA's name, from its asserts): the golf cameras (TW06's GolfCamera_*). Each camera
// mode has an init, called by View_SetCamera, and a per-frame process, called by
// CameraController_Idle; both drive the view's camera script. The shared camera state
// (lbl_80282220) is allocated here, with a per-course elevator camera height.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "camera.h"
#include "dyncam.h"

CamLens* fn_80008370(void* pCamera);                    // the render camera's lens
void     fn_80045470(CamLens* pLens, f32 fFov);
void     fn_800352BC(void);
void     fn_80035240(int a);
void     CameraScript_RecordCurrentCam(CamShot* pShot, void* pCam, void* pSub, int nPlayer, void* pScript,
                                       int a);
void     fn_8003F2E0(void* pScript, f32 fTime);
u8       fn_800635D0(int nPlayer);
u8       fn_800B36F4(View* pView, int nPlayer, f32 fFrameTime);
void     fn_800C73B8(f32* pA, f32* pB, f32* pOut);
void     fn_800C73DC(f32* pA, f32* pB, f32* pOut);
void     fn_800C7400(f32* pA, f32* pOut);
void     GolfCamera_ProcessBallFlightCamera(View* pView, int nPlayer);
f32      fn_800C741C(Character* pChar, u64 uEvent);
void     fn_800C6110(View* pView, int nPlayer, int a);
void     fn_8003E624(int nPlayer, void* pCam, void* pSub, void* pScript, CamShot* pShot, int a,
                     f32 fFrameTime);
CamShot* fn_8003A8C4(char* szName);
void     fn_8003DCE8(int nPlayer, void* pCam, void* pSub, void* pScript, CamShot* pShot, int a,
                     f32 fFrameTime);
void     fn_8003EA50(int nPlayer, void* pCam, void* pSub, void* pScript, CamShot* pShot, int a,
                     f32 fFrameTime);
CamShot* fn_8003A7C8(int nPlayer, int nKind, CamShot* pShot);
void     CameraScript_InterpToNewScript(void* pScript, CamShot* pShot, int nPlayer, void* pCam, void* pSub,
                                        int nA, f32 f1, f32 f2, int nB, f32 f3);
CamShot* fn_8006509C(s32 n);
void     fn_8006351C(View* pView, int nPlayer, int nCamera);
void     fn_800B3550(int a, View* pView, int nPlayer);
u8       fn_800B4908(void);
void     GolfCamera_ComputeSteepSlopeCamVectors(View* pView, int nPlayer);
void     fn_800C5D64(View* pView, f32* pCam, f32* pSub, int nPlayer);
u8       CameraScript_WillGolferBeOccludedInThisView(int nPlayer, CamShot* pShot, void* pScript);
u8       fn_800C708C(View* pView);
void     fn_80038010(u8 a, int n, f32* pVec);
void     fn_800380A8(u8 a, f32* pVec, u8 b, int nSlot, f32 f1, f32 f2);
u8       fn_8004562C(CamShot* pShot);                   // the shot's bAC is 0, 13..15 or 0x17
int      fn_800636EC(void);
void     fn_8000A194(f32 (*m)[4], f32 a, f32 b, f32 c);  // a rotation matrix from three angles
void     fn_800BADB4(f32 (*m)[4], f32* pIn, f32* pOut);  // a vector through a matrix
f32      fn_80014278(CamLens* pLens);                   // the lens's field of view
void     fn_800B5918(f32* pSrc, f32* pDst);             // copy three floats
void     fn_800636B4(int nPlayer);
void     fn_800C4AB0(f32* pFrom, f32* pTo, f32* pOut);
int      fn_800C4D2C(f32* pFrom, f32* pTo, f32* pOut, f32 fMax);
u8       fn_80069428(f32* pPos);                        // the point is in bounds
TNetwork* fn_80069498(void);                            // the course's boundary outline, if any
// The segment crosses the outline (at pHit).
u8       fn_8000C3C8(f32* pFrom, f32* pTo, TNetwork* pNet, s32 nNodes, f32* pHit);
u8       fn_8004B6F8(f32* pFrom, f32* pTo, f32* pHit);
f32      fn_8001EFFC(u8* pLens);                        // the lens's fB0 (char.c: its parameter is u8*)
void     fn_80038054(u8 a, int n, f32 f1, f32 f2);
// Keep pNew above the ground (by fClearance); the out values are optional (NULL): two flags and a
// float.
u8       CamScript_KeepAboveGround(int nPlayer, f32* pNew, f32* pOld, int a, u8* pb1, f32* pf, u8* pb2,
                                   f32 fClearance);
CamShot* fn_80064F7C(int nPlayer, int nKind, int a, CamShot* pShot);
u8       fn_8003C9D0(int nPlayer, int a, CamSequence** ppSeq, CamShot** ppShot);
CamShot* fn_800C4DF8(int nFirst, int nPlayer);
void     fn_800C5EC0(View* pView, f32* pCam, f32* pSub, int nPlayer);
CamSequence* DynamicCam_ChoosePreFlightSequence(int nPlayer, int nLie, int nKind);
f32      fn_800C7394(View* pView);
f32      fn_80009614(f32 x);                            // arc cosine
void     fn_8000AE28(f32* pIn, f32 f, f32* pOut);       // scale a vector
void     fn_8000923C(f32* pRot, f32* pQuat);            // a rotation vector (axis * angle) as a quaternion
void     fn_800090E4(f32* pQuat, f32* pIn, f32* pOut);  // rotate a vector by a quaternion
u8       fn_8006BEA4(void);                             // the GameBreaker letterbox is up, scripted
void     fn_800A68C0(u8 nPlayer);
u8       CameraScript_IsDefaultSwingCam(CamShot* pShot, int nPlayer, f32* pCam);
u8       fn_8003D7A0(CamSequence* pSequence, int nPlayer);
void     fn_80039344(int nView, f32 f);                 // a per-view float (Swing.c's declaration)
f32      fn_80014280(f32 x);                            // tan, as a float
void     fn_800638B8(View* pView, int nPlayer);
f32      fn_80044EA8(int nPlayer, CamScript* pScript);  // how far the ball's flight has run
u8       fn_800451A8(CamScript* pScript, CamShot* pShot, int nPlayer);   // GameEffects.c's declaration
f32      fn_800D04AC(int nPlayer);                      // Swing.c's declaration
void     CameraScript_UpdateLandingEstimate(CamScript* pScript, int nPlayer);
f32      fn_800C54FC(View* pView, f32* pCam, f32* pSub, int nPlayer);
f32      fn_800C5A70(View* pView, f32* pCam, f32* pSub, int nPlayer);
u8       fn_800C7450(void);
void     CameraScript_LagAimMarker(int nPlayer, f32* pSub, f32* pCam, CamShot* pShot, int a, int b, f32 f1,
                                   f32 f2, f32 f3);
void     fn_800130F8(int nPad, int n);                 // the pad's rumble (Swing.c's declaration)
u8       fn_8012022C(void);                            // (sweep code) lbl_80281900's +0x370 is nonzero
void     fn_8001966C(Character* pChar);                 // char.c
void     fn_8007325C(u8* pAnim);                        // set bit 2 of the animation player's flags
u8       fn_800C3FC0(View* pView, int nPlayer, f32* pSub, f32* pAim, f32* pCam);
u8       fn_800C43C0(View* pView, int nPlayer);
u8       fn_800C44F4(View* pView, int nPlayer);
void     fn_800C4FF0(View* pView, f32* pFrom, f32* pTo, int nPlayer);
void     fn_800C56B4(View* pView, f32* pFrom, f32* pTo, int nPlayer);
u8       Ter_CheckForGroundCollision(CourseInfo* pCourse, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal,
                                     SurfaceType** ppSurface, TerObject** ppObj);

// The first-person camera's state, per player (fn_800BF658).
f32 lbl_80191334[5] = {0};                          // the step's bob, 0..16
f32 lbl_80191348[5] = {8.0f, 8.0f, 8.0f, 8.0f};     // the sideways sway, 0..16
f32 lbl_8019135C[5] = {1.0f, 1.0f, 1.0f, 1.0f};     // the eye height: 1 standing, 0 in water
s32 lbl_80191370[5] = {0};                          // which side the sway is on
s32 lbl_80191384[5] = {0};                          // frames since the last step's rumble (-1: waiting)

// Allocate the shared camera state: every flag off, each course's elevator height 10.
void fn_800BD894(void) {
    int i;
    lbl_80282220 = fn_80009B34(0x200, 2, 0, "GoGolfCam.c", 164);
    lbl_80282220->b54 = 0;
    lbl_80282220->b55 = 0;
    lbl_80282220->b56 = 0;
    lbl_80282220->b57 = 0;
    lbl_80282220->b58 = 0;
    lbl_80282220->b59 = 0;
    lbl_80282220->b5A = 0;
    lbl_80282220->b5B = 0;
    lbl_80282220->n60 = 0;
    lbl_80282220->b5C = 0;
    lbl_80282220->f68 = 0.0f;
    for (i = 0; i < 5; i++) {
        lbl_80282220->n1EC[i] = 1;
    }
    for (i = 0; i < 21; i++) {
        lbl_80282220->fElevatorHeight[i] = 10.0f;
    }
}

void fn_800BDA04(void) {
    fn_80009E70(lbl_80282220);
    lbl_80282220 = NULL;
}

// Camera 0: the pre-flight sequence (the one after the current one when that is kind 1 -> 2), shot 2.
void fn_800BDA30(View* pView, int nPlayer) {
    f32* pCam;
    f32* pSub;
    CamShot* pShot;
    CamSequence* pSeq;
    int nA;
    f32 f1;
    f32 f2;
    int nB;
    f32 f3;
    int nLie;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    pView->b153 = 0;
    lbl_80282220->f68 = 0.0f;
    nLie = gPlayers[nPlayer].ball.nLie;
    pSeq = pView->p74;
    if (pSeq != NULL && pSeq->b44 == 1 && pSeq->p20 != pSeq && pSeq->p20 != NULL && pSeq->p20->b44 == 2) {
        pView->p74 = pSeq->p20;
    } else {
        pView->p74 = DynamicCam_ChoosePreFlightSequence(nPlayer, nLie, 2);
    }
    pShot = fn_8003A950(pView->p74, 2, &nA, &f1, &f2, &nB, &f3, nPlayer);
    if (pShot != NULL) {
        if (pView->p130 == NULL || pView->p130->bAD != 1 || pView->p134 != NULL) {
            nA = 5;
        }
        CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, nA, f1, f2, nB, f3);
        if (pView->nCamera != 0) {
            pView->f114 = 0.0f;
            pView->nCamera = 2;
        }
    }
}

// Camera 0: only the script's per-frame update.
void fn_800BDBA4(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    fn_8003DCE8(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
}

// The zoom-to-aim camera: start from the current camera, with its height over the ground (the
// lower ground height, else the higher, else the ball's) and the offsets of the last shot of the
// current run (up to one of kind 6 or 8..10). Unless the golfer is in shot setup, look from the
// camera through the aim point, as far out as the aim point is.
void GolfCamera_InitZoomToAimCamera(View* pView, int nPlayer) {
    f32 vAim[4];
    f32 v[4];
    f32 vDir[4];
    f32 fLow;
    f32 fHigh;
    CamShot* pShot;
    f32 fGround;
    f32* pCam;
    f32* pSub;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    Vec_Copy(gPlayers[nPlayer].vTargetCopy, vAim);
    Ter_GetEnclosingGroundHeight(fn_8000C594(), pCam, &fLow, &fHigh);
    if (fLow < -60000.0f) {
        if (!(fHigh < -60000.0f)) {
            fGround = fHigh;
        } else {
            fGround = gPlayers[nPlayer].vBall[1];
        }
    } else {
        fGround = fLow;
    }
    if (pView->p130 != NULL) {
        for (pShot = pView->p130; pShot->p40 != NULL; pShot = pShot->p40) {
            if (pShot->p40->bAB == 6 || pShot->p40->bAB == 8 || pShot->p40->bAB == 9
                || pShot->p40->bAB == 10) {
                break;
            }
        }
        pView->shot19C.f68 = pCam[1] - fGround;
        pView->shot19C.f74 = pShot->f74;
        pView->shot19C.f70 = 0.0f;
        if (pShot->bAF == 0x15 || pShot->bB0 == 0x15) {
            pView->shot19C.f64 = pShot->f60;
            pView->shot19C.f60 = -pShot->f64;
        } else {
            pView->shot19C.f60 = pShot->f60;
            pView->shot19C.f64 = pShot->f64;
        }
    } else {
        pView->shot19C.f68 = pCam[1] - fGround;
        pView->shot19C.f74 = 0.0f;
        pView->shot19C.f70 = 0.0f;
        pView->shot19C.f60 = 0.0f;
        pView->shot19C.f64 = 0.0f;
    }
    Vec3Copy(pCam, pView->shot19C.v30);
    pView->p130 = NULL;
    pView->f18C = -1.0f;
    if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) != GS_SHOT_SETUP) {
        fn_800C73DC(pSub, pCam, vDir);
        if (vDir[0] != 0.0f || vDir[1] != 0.0f || vDir[2] != 0.0f) {
            fn_800BAF04(vDir, vDir);
        }
        fn_800C73DC(vAim, pCam, v);
        fn_8001EF34(vDir, fn_80009680(fn_80009744(v)), vDir);
        fn_800C73B8(pCam, vDir, pSub);
    }
    EVENT_Trigger(nPlayer, 0x30, NULL, -1);
}

// The zoom-to-aim camera's tick: fly the camera to the goal fn_800C3FC0 works out, fast at first
// and slowing over the tuning's f8, with slow motion while it moves. f18C runs from below 0 (not
// set off yet) to 1 (arrived); on the way the height blends from the aim's ground plus the shot's
// f68 to the tuning's f14 over View.f15C (the ground at the target) in the second half. Arrived, it
// creeps on towards the goal and eases its height.
void GolfCamera_ProcessZoomToAimCamera(View* pView, int nPlayer) {
    f32 vMove[4];
    f32 vGoal[4];
    f32 vAimMove[4];
    f32 vCreep[4];
    f32 vTarget[4];
    f32 vStart[4];
    f32 vOld[4];
    f32 vAim[4];
    f32 fLow;
    f32 fHigh;
    u8 bHit;
    f32* pCam;
    f32* pSub;
    CourseInfo* pCourse;
    u8 bMirror;
    f32 fDist;
    f32 fTotal;
    f32 fBase;
    f32 fSlow;
    f32 fSpeed;
    f32 f;
    f32 fAimY;
    f32 fCamY;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    Vec3Copy(pCam, vOld);
    Vec_Copy(gPlayers[nPlayer].vTargetCopy, vTarget);
    bMirror = fn_800C3FC0(pView, nPlayer, pCam, vAim, vGoal);
    pCourse = fn_8000C594();
    fBase = lbl_80281F78->f0;
    fSlow = lbl_80281F78->f8;
    // flat distances to the goal: from the camera, from the aim and from where it started
    fn_800C73DC(vGoal, pCam, vMove);
    vMove[1] = 0.0f;
    fDist = fn_80009680(fn_80009744(vMove));
    fn_800C73DC(vGoal, vAim, vAimMove);
    vAimMove[1] = 0.0f;
    fTotal = fn_80009680(fn_80009744(vAimMove));
    fn_800C73DC(vGoal, pView->shot19C.v30, vStart);
    vStart[1] = 0.0f;
    f = fn_80009680(fn_80009744(vStart));
    if (fTotal < f) {
        fTotal = f;
    }
    if (pView->f18C >= 1.0f) {
        fn_800C73DC(vGoal, pCam, vCreep);
        vCreep[1] = 0.0f;
        fSlow = fn_80009680(fn_80009744(vCreep));
        if (vCreep[0] != 0.0f || vCreep[1] != 0.0f || vCreep[2] != 0.0f) {
            fn_800BAF04(vCreep, vCreep);
        }
        fSlow /= lbl_80281F78->f18;
        fn_8001EF34(vCreep, fSlow, vCreep);
        fn_800C73B8(pCam, vCreep, pCam);
        fDist = 0.0f;
    } else if (fDist > lbl_80281F78->f24 && fn_8000C5FC(vMove, vAimMove) > 0.0f && pView->f18C >= 0.0f) {
        // on the way: faster the further it is
        if (fDist < 1.0f) {
            f = fBase;
        } else {
            f = fBase * (f32)fn_80009680(fDist);
        }
        fSpeed = f;
        if (fTotal - fDist < fSlow && fSlow > 0.0f) {
            fSpeed *= (fTotal - fDist) / fSlow;
            if (fSpeed < fBase) {
                fSpeed = fBase - (fBase - fSpeed);
            }
            if (fSpeed < 0.5f) {
                fSpeed = 0.5f;
            }
        }
        fSlow = fSpeed * gSession.fFrameTime;
        if (fDist - fSlow < 0.0f) {
            fSlow = fDist;
        }
        if (vMove[0] != 0.0f || vMove[1] != 0.0f || vMove[2] != 0.0f) {
            fn_800BAF04(vMove, vMove);
        }
        fn_8001EF34(vMove, fSlow, vMove);
        fn_800C73B8(vMove, pCam, pCam);
        fSpeed = lbl_80281F78->f1C * (1.0f - fBase / fSpeed);
        if (fSpeed < 0.0f) {
            fSpeed = 0.0f;
        }
        if (gSession.nPaused == 0) {
            fn_80038054(1, fn_80016D10(), 0.0f, fSpeed);
        }
        pView->f18C = 1.0f - fDist / fTotal;
    } else if (pView->f18C >= 0.0f) {
        EVENT_Trigger(nPlayer, 0x31, NULL, -1);
        pView->f18C = 1.0f;
    } else if (gSession.nPaused == 0) {
        fn_80038054(1, fn_80016D10(), 0.0f, lbl_80281F78->f20);
    }
    if (pCourse != NULL) {
        Ter_GetEnclosingGroundHeight(pCourse, vTarget, &fLow, &fHigh);
        if (fLow < -60000.0f) {
            if (!(fHigh < -60000.0f)) {
                pView->f15C = fHigh;
            }
        } else {
            pView->f15C = fLow;
        }
    }
    // before setting off, far from the goal: rise to the course's elevator height
    if (pView->shot19C.f68 < lbl_80282220->fElevatorHeight[Game_GetCourse()] && pView->f18C < 0.0f
        && fDist > 20.0f) {
        pView->shot19C.f68 += 0.25f * (FRAME_RATE * gSession.fFrameTime);
    } else if (pView->f18C < 0.0f) {
        pView->f18C = 0.0f;
    }
    if (pCourse != NULL) {
        Ter_GetEnclosingGroundHeight(pCourse, vAim, &fLow, &fHigh);
        if (fLow < -60000.0f) {
            if (!(fHigh < -60000.0f)) {
                fAimY = fHigh;
            } else {
                fAimY = gPlayers[nPlayer].vBall[1];
            }
        } else {
            fAimY = fLow;
        }
        fAimY += pView->shot19C.f68;
        fCamY = pView->f15C + lbl_80281F78->f14;
    } else {
        fAimY = 0.0f;
        fCamY = 0.0f;
    }
    if (pView->f18C < 0.0f || fDist / fTotal > 0.5f) {
        f = fAimY;
    } else {
        f = fAimY + (1.0f - fDist / fTotal / 0.5f) * (fCamY - fAimY);
    }
    if (pView->f18C >= 1.0f) {
        pCam[1] = pCam[1] + lbl_80281F78->f2C * (f - pCam[1]);
    } else {
        pCam[1] = f;
    }
    CamScript_KeepAboveGround(nPlayer, pCam, vOld, 1, &bHit, NULL, NULL, 0.5f);
    if (pView->f18C >= 1.0f) {
        fSlow = 1.0f + lbl_80281F78->f4;
        fSlow *= 1.0f / fn_8001EFFC((u8*)fn_80008370(fn_80017004(gPlayers[nPlayer].nView[0])));
    } else {
        fSlow = 10000.0f;
    }
    if (!bHit && pCam[1] < lbl_80281F78->f168 + fn_8000C594()->fFloor) {
        pCam[1] = lbl_80281F78->f168 + fn_8000C594()->fFloor;
    }
    f = lbl_80281F78->f18 + fDist / fTotal * (lbl_80281F78->f28 - lbl_80281F78->f18);
    if (pView->f18C >= 0.0f) {
        CameraScript_LagAimMarker(nPlayer, pSub, pCam, &pView->shot19C, 1, bMirror != 0, f, fSlow,
                                  lbl_80281F78->fDC);
    }
}

// The zoom-to-aim camera on the green: as GolfCamera_InitZoomToAimCamera, but the height is the
// tuning's f168 over the pin or the ground under the camera, whichever is higher (0.5 more over
// anything but green, fringe or cup, at most 1 under the camera when it is more than 5 over), and
// the view's v20 is set from the lens.
void GolfCamera_InitGreenZoomToAimCamera(View* pView, int nPlayer) {
    f32 vAim[4];
    f32 vDir[4];
    f32 v[4];
    f32 vNormalHigh[4];
    f32 vNormalLow[4];
    SurfaceType* pSurfaceLow;
    SurfaceType* pSurfaceHigh;
    f32 fLow;
    f32 fHigh;
    CamShot* pShot;
    f32 fGround;
    f32 fTop;
    CourseInfo* pCourse;
    f32* pCam;
    f32* pSub;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    pCourse = fn_8000C594();
    if (pCourse != NULL) {
        Vec_Copy(gPlayers[nPlayer].vTargetCopy, vAim);
        Ter_GetEnclosingGroundData(fn_8000C594(), pCam, &fLow, &pSurfaceLow, vNormalLow, &fHigh,
                                   &pSurfaceHigh, vNormalHigh);
        if (fLow < -60000.0f) {
            if (!(fHigh < -60000.0f)) {
                fGround = fHigh;
                if (pSurfaceHigh != NULL && pSurfaceHigh->nClass != 3 && pSurfaceHigh->nClass != 4
                    && pSurfaceHigh->nClass != 12 && pSurfaceHigh->nClass != 18) {
                    fGround += 0.5f;
                }
            } else {
                fGround = 0.0f;
            }
        } else {
            fGround = fLow;
            if (pSurfaceLow != NULL && pSurfaceLow->nClass != 3 && pSurfaceLow->nClass != 4
                && pSurfaceLow->nClass != 12 && pSurfaceLow->nClass != 18) {
                fGround += 0.5f;
            }
        }
        if (pCam[1] - fGround > 5.0f) {
            fGround = pCam[1] - 1.0f;
        }
        if (pView->p130 != NULL) {
            for (pShot = pView->p130; pShot->p40 != NULL; pShot = pShot->p40) {
                if (pShot->p40->bAB == 6 || pShot->p40->bAB == 8 || pShot->p40->bAB == 9
                    || pShot->p40->bAB == 10) {
                    break;
                }
            }
            pView->shot19C.f74 = pShot->f74;
            pView->shot19C.f70 = 0.0f;
            if (pShot->bAF == 0x15 || pShot->bB0 == 0x15) {
                pView->shot19C.f64 = pShot->f60;
                pView->shot19C.f60 = -pShot->f64;
            } else {
                pView->shot19C.f60 = pShot->f60;
                pView->shot19C.f64 = pShot->f64;
            }
        } else {
            pView->shot19C.f74 = 0.0f;
            pView->shot19C.f70 = 0.0f;
            pView->shot19C.f60 = 0.0f;
            pView->shot19C.f64 = 0.0f;
        }
        fTop = pCourse->pin[Game_CurrentPinSet()].y;
        if (fTop <= fGround) {
            fTop = fGround;
        }
        pView->shot19C.f68 = lbl_80281F78->f168 + fTop;
        Vec3Copy(pCam, pView->shot19C.v30);
        pView->p130 = NULL;
        pView->f18C = -1.0f;
        pView->f190 = 100000000.0f;
        if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) != GS_SHOT_SETUP) {
            fn_800C73DC(pSub, pCam, vDir);
            if (vDir[0] != 0.0f || vDir[1] != 0.0f || vDir[2] != 0.0f) {
                fn_800BAF04(vDir, vDir);
            }
            fn_800C73DC(vAim, pCam, v);
            fn_8001EF34(vDir, fn_80009680(fn_80009744(v)), vDir);
            fn_800C73B8(pCam, vDir, pSub);
        }
        EVENT_Trigger(nPlayer, 0x30, NULL, -1);
        Vec3Copy(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[0]))->v4, pView->v20);
    }
}

// The green zoom-to-aim camera's tick: fly the camera to the goal fn_800C3FC0 works out, fast at
// first and slowing over the tuning's f30, rising to a height that keeps the pin in the lens, with
// slow motion while it moves. Arrived (f18C 1), it creeps on towards the goal and settles its
// height. The aim marker lags behind.
void GolfCamera_ProcessGreenZoomToAimCamera(View* pView, int nPlayer) {
    f32 vMove[4];
    f32 vGoal[4];
    f32 vAimMove[4];
    f32 vCreep[4];
    f32 vTarget[4];
    f32 vStart[4];
    f32 vOld[4];
    f32 vAim[4];
    f32 vPin[4];
    f32 vDiff[4];
    f32 vSide[4];
    u8 bHit;
    f32* pCam;
    f32* pSub;
    CourseInfo* pCourse;
    CamLens* pLens;
    u8 bArrived;
    int nPinSet;
    f32 fDist;
    f32 fTotal;
    f32 fSpeed;
    f32 fBase;
    f32 fSlow;
    f32 fHeight;
    f32 f;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    bArrived = 0;
    if (gSession.nPaused == 0) {
        Vec_Copy(gPlayers[nPlayer].vTargetCopy, vTarget);
        Vec3Copy(pCam, vOld);
        fn_800C3FC0(pView, nPlayer, pCam, vAim, vGoal);
        pCourse = fn_8000C594();
        fBase = lbl_80281F78->f34;
        fSlow = lbl_80281F78->f30;
        // flat distances to the goal: from the camera, from the aim and from where it started
        fn_800C73DC(vGoal, pCam, vMove);
        vMove[1] = 0.0f;
        fDist = fn_80009680(fn_80009744(vMove));
        fn_800C73DC(vGoal, vAim, vAimMove);
        vAimMove[1] = 0.0f;
        fTotal = fn_80009680(fn_80009744(vAimMove));
        fn_800C73DC(vGoal, pView->shot19C.v30, vStart);
        vStart[1] = 0.0f;
        f = fn_80009680(fn_80009744(vStart));
        if (fTotal < f) {
            fTotal = f;
        }
        fHeight = lbl_80281F78->f44;
        pLens = fn_80008370(fn_80017004(gPlayers[nPlayer].nView[0]));
        fHeight *= 1.0f / fn_8001EFFC((u8*)pLens);
        // high enough to see the pin (up to 20 from the target) through the lens
        nPinSet = Game_CurrentPinSet();
        fn_800C73DC(&pCourse->pin[nPinSet].x, vTarget, vPin);
        vPin[1] = 0.0f;
        f = fn_80009680(fn_80009744(vPin));
        f += 1.5f;
        fSpeed = (f < 0.0f) ? 0.0f : ((f > 20.0f) ? 20.0f : f);
        f = fSpeed / fn_80014280(fn_80014278(pLens) / 2.0f);
        if (f > fHeight) {
            fHeight = f;
        }
        if (pView->f18C >= 1.0f) {
            fn_800C73DC(vGoal, pCam, vCreep);
            vCreep[1] = 0.0f;
            fSlow = fn_80009680(fn_80009744(vCreep));
            if (vCreep[0] != 0.0f || vCreep[1] != 0.0f || vCreep[2] != 0.0f) {
                fn_800BAF04(vCreep, vCreep);
            }
            fSlow /= lbl_80281F78->f48;
            fn_8001EF34(vCreep, fSlow, vCreep);
            fn_800C73B8(pCam, vCreep, pCam);
            if (pCam[1] < pView->shot19C.f68 + fHeight) {
                pCam[1] += lbl_80281F78->f40;
                if (pCam[1] > pView->shot19C.f68 + fHeight) {
                    pCam[1] = pView->shot19C.f68 + fHeight;
                }
            } else if (pCam[1] > pView->shot19C.f68 + fHeight) {
                pCam[1] -= lbl_80281F78->f40;
                if (pCam[1] < pView->shot19C.f68 + fHeight) {
                    pCam[1] = pView->shot19C.f68 + fHeight;
                }
            }
        } else if (fDist > lbl_80281F78->f24 && fn_8000C5FC(vMove, vAimMove) > 0.0f) {
            // still short of the goal: faster the further it is
            if (fDist < 1.0f) {
                f = fBase;
            } else {
                f = fBase * (f32)fn_80009680(fDist);
            }
            fSpeed = f;
            if (fDist > pView->f190) {
                fSpeed = f + (fDist - pView->f190);
                pView->f190 = pView->f190 - 0.3f;
            } else {
                pView->f190 = fDist;
            }
            if (fTotal - fDist < fSlow) {
                fSpeed *= (fTotal - fDist) / fSlow;
                if (fSpeed < fBase) {
                    fSpeed = fBase - (fBase - fSpeed);
                }
                if (fSpeed < 0.5f) {
                    fSpeed = 0.5f;
                }
            }
            fSlow = fSpeed * gSession.fFrameTime;
            if (fDist - fSlow < 0.0f) {
                fSlow = fDist;
                bArrived = 1;
            } else if (fDist - fSlow < lbl_80281F78->f24) {
                bArrived = 1;
            }
            if (vMove[0] != 0.0f || vMove[1] != 0.0f || vMove[2] != 0.0f) {
                fn_800BAF04(vMove, vMove);
            }
            fn_8001EF34(vMove, fSlow, vMove);
            fn_800C73B8(vMove, pCam, pCam);
            fSlow = lbl_80281F78->f1C * (1.0f - fBase / fSpeed);
            if (fSlow < 0.0f) {
                fSlow = 0.0f;
            }
            if (gSession.nPaused == 0) {
                fn_80038054(1, fn_80016D10(), 0.0f, fSlow);
            }
            if (bArrived) {
                EVENT_Trigger(nPlayer, 0x31, NULL, -1);
                pView->f18C = 1.0f;
            } else {
                pView->f18C = 1.0f - fDist / fTotal;
            }
            f = fHeight + (pView->shot19C.f68 - pView->shot19C.v30[1]);
            f *= 1.0f - fDist / fTotal;
            pCam[1] = pView->shot19C.v30[1] + f;
        } else {
            EVENT_Trigger(nPlayer, 0x31, NULL, -1);
            pView->f18C = 1.0f;
        }
        bHit = 0;
        if ((CamScript_KeepAboveGround(nPlayer, pCam, vOld, 1, NULL, NULL, &bHit, lbl_80281F78->f168) || bHit)
            && pView->f18C < 0.0f) {
            pView->f18C = 0.0f;
        }
        f = lbl_80281F78->f48;
        if (pView->shot19C.f74 >= 0.0f) {
            pView->shot19C.f74 -= lbl_80281F78->f3C;
        }
        CameraScript_LagAimMarker(nPlayer, pSub, pCam, &pView->shot19C, 0, 0, f, 0.0f, lbl_80281F78->fDC);
        if (pView->f18C < 1.0f) {
            fn_800C73DC(pSub, pCam, vDiff);
            vSide[0] = vDiff[2];
            vSide[1] = 0.0f;
            vSide[2] = -vDiff[0];
        } else {
            fn_800C73DC(vTarget, gPlayers[nPlayer].vBall, vDiff);
            vSide[0] = vDiff[2];
            vSide[1] = 0.0f;
            vSide[2] = -vDiff[0];
        }
        fn_80063F08(pView->v20, vSide, pView->v20);
    }
}

void GolfCamera_InitSteepSlopeCamera(View* pView, int nPlayer) {
    if (pView->p130 != NULL) {
        GolfCamera_ComputeSteepSlopeCamVectors(pView, nPlayer);
        pView->p130 = NULL;
        CameraScript_RecordCurrentCam(&pView->shot19C, pView->v0, pView->v10, nPlayer, &pView->script, 0);
        strcpy(pView->shot19C.szName, "STEEPSLOPE CAM");
    }
}

void GolfCamera_ProcessSteepSlopeCamera(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    GolfCamera_ComputeSteepSlopeCamVectors(pView, nPlayer);
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    fn_8003DCE8(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
}

// Camera 3, the elevator camera: the current view raised by the course's elevator height.
void GolfCamera_InitElevatorCamera(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    if (pView->p130 != NULL) {
        CameraScript_RecordCurrentCam(&pView->shot19C, pCam, pSub, nPlayer, &pView->script, 0);
        strcpy(pView->shot19C.szName, "ELEVATOR CAM");
        pView->shot19C.v20[1] += lbl_80282220->fElevatorHeight[Game_GetCourse()];
        pView->shot19C.bAC = 9;
        CameraScript_InterpToNewScript(&pView->script, &pView->shot19C, nPlayer, pCam, pSub, 1,
                                       lbl_80281F78->f94, 100.0f, 0x19, 0.0f);
    }
}

// Camera 3 (the elevator camera).
void fn_800BF094(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    fn_8003DCE8(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
}

// Camera 8: a 60-degree lens, no script.
void fn_800BF110(View* pView, int nPlayer) {
    fn_8001731C(pView);
    fn_80017314(pView);
    fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[0])), DEG(60.0f));
    pView->n194 = 0;
    pView->p74 = NULL;
    pView->p130 = NULL;
}

// Camera 8: 10 back and up at 20 degrees from the ball's placement spot along its heading (fA88),
// over the ground there, which it follows smoothly; it looks at the placement spot.
void fn_800BF184(View* pView, int nPlayer) {
    f32 vOld[4];
    f32 v[4];
    f32 vHit[4];
    f32 vNormal[4];
    SurfaceType* pSurfaceAt;
    f32 fAbove;
    SurfaceType* pSurface;
    TerObject* pObj;
    f32* pCam;
    f32* pSub;
    CourseInfo* pCourse;
    int nPinSet;
    f32 fGround;
    f32 fUp;
    f32 fBack;
    f32 fSin;
    f32 fCos;
    f32 fX;
    f32 fZ;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    pCourse = fn_8000C594();
    if (pCourse != NULL) {
        Vec3Copy(pCam, vOld);
        nPinSet = Game_CurrentPinSet();
        if (gSession.nPaused == 0) {
            fn_8003F2E0(&pView->script, gSession.fFrameTime);
            pView->f114 += gSession.fFrameTime;
        }
        if (gSession.fFrameTime != 0.0f) {
            fUp = fn_800095F0(DEG(20.0f));
            fBack = fn_80009638(DEG(20.0f));
            fUp *= 10.0f;
            fBack *= 10.0f;
            fSin = fn_800095F0(gPlayers[nPlayer].fA88);
            fCos = fn_80009638(gPlayers[nPlayer].fA88);
            fX = fBack * -fSin;
            fZ = fBack * fCos;
            pCam[0] = fX + gPlayers[nPlayer].vPlacement[0];
            pCam[2] = fZ + gPlayers[nPlayer].vPlacement[2];
            fGround = Terrain_HeightAt(pCam, &pSurfaceAt);
            if (fGround < -60000.0f) {
                fGround = gPlayers[nPlayer].vPlacement[1];
            }
            if (pView->n194 == 0) {
                pView->f15C = fGround;
            } else if (gPlayers[nPlayer].uFlagsEF0 & 1) {
                if (pView->f15C > gPlayers[nPlayer].vPlacement[1]) {
                    if (gPlayers[nPlayer].vPlacement[1] > fGround) {
                        pView->f15C = gPlayers[nPlayer].vPlacement[1];
                    } else {
                        pView->f15C = fGround;
                    }
                } else if (pView->f15C < gPlayers[nPlayer].vPlacement[1]) {
                    if (gPlayers[nPlayer].vPlacement[1] > fGround) {
                        pView->f15C = gPlayers[nPlayer].vPlacement[1];
                    } else {
                        pView->f15C = fGround;
                    }
                }
                fGround = pView->f15C;
            } else {
                if (pView->f15C < gPlayers[nPlayer].vPlacement[1]) {
                    if (pView->f15C < pCourse->tee[gSession.nTeeSet[nPlayer]].y + 0.1f
                        || pView->f15C < pCourse->pin[nPinSet].y + 0.1f) {
                        if (pView->f15C > fGround) {
                            fGround = 0.1f + pView->f15C;
                        }
                        pView->f15C = fGround;
                    }
                } else {
                    if (pView->f15C > pCourse->tee[gSession.nTeeSet[nPlayer]].y - 0.1f
                        || pView->f15C > pCourse->pin[nPinSet].y - 0.1f) {
                        if (pView->f15C > fGround) {
                            fGround = pView->f15C - 0.1f;
                        }
                        pView->f15C = fGround;
                    }
                }
                fGround = pView->f15C;
            }
            if (fGround < -60000.0f) {
                fGround = 0.0f;
            }
            if (pView->n194 != 0) {
                fUp += fGround;
                v[0] = pCam[0];
                v[2] = pCam[2];
                v[1] = fUp;
                if (fUp > pCam[1]) {
                    if (Ter_CheckForGroundCollision(pCourse, v, pCam, vHit, vNormal, &pSurface, &pObj)) {
                        pCam[1] = fUp;
                        pView->f15C = fGround;
                    } else {
                        pCam[1] += (1.0f - lbl_80281F78->f98) * (fUp - pCam[1]);
                    }
                } else {
                    pCam[1] += (1.0f - lbl_80281F78->f98) * (fUp - pCam[1]);
                }
            } else {
                pCam[1] = fUp + fGround;
            }
            if (pView->n194 != 0) {
                CamScript_KeepAboveGround(nPlayer, pCam, vOld, 1, NULL, &fAbove, NULL,
                                          0.2f + lbl_80281F78->f168);
            }
            pSub[0] = gPlayers[nPlayer].vPlacement[0];
            pSub[2] = gPlayers[nPlayer].vPlacement[2];
            fGround = gPlayers[nPlayer].vPlacement[1];
            if (fGround < -60000.0f) {
                fGround = 0.0f;
            }
            if (pView->n194 != 0) {
                pSub[1] += (1.0f - lbl_80281F78->f98) * (fGround - pSub[1]);
                if (pCam[1] - pSub[1] > 5.0f) {
                    pSub[1] = pCam[1] - 5.0f;
                }
            } else {
                pSub[1] = fGround;
            }
            if (pView->n194 == 0) {
                pView->n194 = 1;
            }
        }
    }
}

// Camera 9: the same as camera 8.
void fn_800BF5E4(View* pView, int nPlayer) {
    fn_8001731C(pView);
    fn_80017314(pView);
    fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[0])), DEG(60.0f));
    pView->n194 = 0;
    pView->p130 = NULL;
    pView->p74 = NULL;
}

// The first-person camera's tick (camera 9's process, after camera 8's): the eye at the golfer's
// position facing along fA88, 1.4 over the ground (0.1 in water), bobbing and swaying with the
// golfer's steps, with a rumble on each step for a human player. It looks ahead, at a height set by
// the pad's stick (fA8C).
void fn_800BF658(View* pView, int nPlayer) {
    f32 vOld[4];
    f32 fAbove;
    f32* pCam;
    f32* pSub;
    SurfaceType* pSurface;
    u32 nClass;
    f32 fUp;
    f32 fBack;
    f32 fSin;
    f32 fCos;
    f32 fX;
    f32 fZ;
    f32 fStep;
    f32 fSway;
    f32 fY;
    f32 fDist = 10.0f;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    if (fn_8000C594() != NULL) {
        Vec3Copy(pCam, vOld);
        Game_CurrentPinSet();
        if (gSession.nPaused == 0) {
            fn_8003F2E0(&pView->script, gSession.fFrameTime);
            pView->f114 += gSession.fFrameTime;
        }
        if (gSession.fFrameTime != 0.0f) {
            // 20 degrees, written out: DEG(20.0f) rounds one bit lower than the original's constant
            fUp = fDist * fn_800095F0(20.0f * PI / 180.0f);    // camera 8's height over the golfer; unused
            fBack = fDist * fn_80009638(20.0f * PI / 180.0f);
            fSin = fn_800095F0(gPlayers[nPlayer].fA88);
            fCos = fn_80009638(gPlayers[nPlayer].fA88);
            fX = fBack * -fSin;
            fZ = fBack * fCos;
            pCam[0] = gPlayers[nPlayer].vPlacement[0];
            pCam[2] = gPlayers[nPlayer].vPlacement[2];
            pCam[1] = (1.4f + gPlayers[nPlayer].vPlacement[1]) * lbl_8019135C[nPlayer]
                      + (0.1f + gPlayers[nPlayer].vPlacement[1]) * (1.0f - lbl_8019135C[nPlayer]);
            CamScript_KeepAboveGround(nPlayer, pCam, vOld, 1, NULL, &fAbove, NULL, lbl_80281F78->f168);
            pSurface = Ter_GetSupportingWorldMaterial(gPlayers[nPlayer].ball.pCourse, pCam);
            if (pSurface != NULL) {
                nClass = pSurface->nClass;
            } else {
                nClass = 0;
            }
            // down to the water line in water (classes 7 and 16), back up out of it
            if (nClass == 7 || nClass == 16) {
                lbl_8019135C[nPlayer] -= 0.1f;
            } else {
                lbl_8019135C[nPlayer] += 0.1f;
            }
            if (lbl_8019135C[nPlayer] > 1.0f) {
                lbl_8019135C[nPlayer] = 1.0f;
            } else if (lbl_8019135C[nPlayer] < 0.0f) {
                lbl_8019135C[nPlayer] = 0.0f;
            }
            if (lbl_80191384[nPlayer] >= 0) {
                lbl_80191384[nPlayer]++;
            }
            if ((lbl_80191384[nPlayer] >= 2 || lbl_80191384[nPlayer] < 0) && !Player_IsCPU(nPlayer)) {
                fn_800130F8(gPlayers[nPlayer].nController, 0);
            }
            if (nClass != 7) {
                // the step: the length of three times vCBC's x and z, per 60th of a second
                fStep = (f32)fn_80009680((f32)(fn_8015F824(3.0f * gPlayers[nPlayer].vCBC[2], 2.0)
                                                + fn_8015F824(3.0f * gPlayers[nPlayer].vCBC[0], 2.0)))
                        / (FRAME_RATE / 60.0f);
                lbl_80191334[nPlayer] += fStep;
                lbl_80191348[nPlayer] += fStep;
                if (lbl_80191334[nPlayer] > 16.0f) {
                    lbl_80191334[nPlayer] -= 16.0f;
                }
                if (lbl_80191334[nPlayer] < 8.0f) {
                    pCam[1] += lbl_80191334[nPlayer] / 16.0f;
                    if (lbl_80191334[nPlayer] < 3.0f && !Player_IsCPU(nPlayer)) {
                        lbl_80191384[nPlayer] = -1;
                    }
                } else {
                    pCam[1] += 0.5f - (lbl_80191334[nPlayer] - 8.0f) / 16.0f;
                    if (lbl_80191334[nPlayer] > 14.0f && fStep > 0.1f && lbl_80191384[nPlayer] == -1
                        && !Player_IsCPU(nPlayer)) {
                        fn_800130F8(gPlayers[nPlayer].nController, 1);
                        lbl_80191384[nPlayer] = 0;
                    }
                }
                if (lbl_80191348[nPlayer] > 16.0f) {
                    lbl_80191348[nPlayer] -= 16.0f;
                    lbl_80191370[nPlayer] ^= 1;
                }
                if (lbl_80191370[nPlayer] == 0) {
                    if (lbl_80191348[nPlayer] < 8.0f) {
                        fSway = 0.7f * (lbl_80191348[nPlayer] / 16.0f);
                    } else {
                        fSway = 0.7f * (0.5f - (lbl_80191348[nPlayer] - 8.0f) / 16.0f);
                    }
                } else {
                    if (lbl_80191348[nPlayer] < 8.0f) {
                        fSway = 0.7f * -(lbl_80191348[nPlayer] / 16.0f);
                    } else {
                        fSway = 0.7f * -(0.5f - (lbl_80191348[nPlayer] - 8.0f) / 16.0f);
                    }
                }
                pCam[0] += fSway * fCos;
                pCam[2] += fSway * fSin;
            }
            if (pView->n194 != 0) {
                CamScript_KeepAboveGround(nPlayer, pCam, vOld, 1, NULL, &fAbove, NULL, lbl_80281F78->f168);
            }
            pSub[0] = gPlayers[nPlayer].vPlacement[0];
            pSub[2] = gPlayers[nPlayer].vPlacement[2];
            fY = gPlayers[nPlayer].vPlacement[1];
            if (fY < pCam[1] - 5.0f) {
                fY = pCam[1] - 5.0f;
            }
            if (fY < -60000.0f) {
                fY = 0.0f;
            }
            if (pView->n194 != 0) {
                if (fY - pSub[1] < -5.0f) {
                    pSub[1] = 5.0f + fY;
                }
                pSub[1] = fY - lbl_80281F78->f98 * (fY - pSub[1]);
            } else {
                pSub[1] = fY;
            }
            // the height just worked out is replaced: the stick tilts the view up and down
            pSub[1] = (4.0f * gPlayers[nPlayer].fA8C + pCam[1]) * lbl_8019135C[nPlayer]
                      + (2.0f + pCam[1] + gPlayers[nPlayer].fA8C) * (1.0f - lbl_8019135C[nPlayer]);
            pSub[0] = pCam[0] - fX / 3.0f;
            pSub[2] = pCam[2] - fZ / 3.0f;
            if (pView->n194 == 0) {
                pView->n194 = 1;
            }
        }
    }
}

// Camera 4 (holding button 0x30 starts it 2 s in): the ball and the pin into the script, a
// 30-degree lens.
void fn_800BFC80(View* pView, int nPlayer) {
    f32 v[4];
    f32* pCam;
    f32* pSub;
    CourseInfo* pCourse;
    int nPinSet;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    pCourse = fn_8000C594();
    if (pCourse != NULL) {
        pView->f50 = 1.0f;
        pView->f54 = 1.0f;
        pView->f58 = 1.0f;
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x30, 1)) {
            pView->fCamTime = 2.0f;
        } else {
            pView->fCamTime = 0.0f;
        }
        nPinSet = Game_CurrentPinSet();
        Vec_Copy(pCam, pView->v30);
        fn_800C73DC(pSub, pCam, v);
        if (v[0] != 0.0f || v[1] != 0.0f || v[2] != 0.0f) {
            fn_800BAF04(v, v);
        }
        fn_800C73B8(pCam, v, v);
        Vec_Copy(v, pView->v40);
        Vec_Copy(gPlayers[nPlayer].ball.vPos, pView->script.v0);
        Vec_Copy(&pCourse->pin[nPinSet].x, pView->script.v10);
        pView->shot19C.f6C = 1.0f;
        pView->shot19C.f68 = gPlayers[nPlayer].ball.vPos[1];
        pView->p130 = NULL;
        pView->p134 = NULL;
        fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[0])), DEG(30.0f));
    }
}

// Camera 4's tick. While shot19C.f6C is under 1 the camera moves in from where it started (v30/v40
// to v0/v10), faster with button 0x2E or 0x30 held, and back out without. In place, those buttons
// grow f54 and buttons 0x31/0x32 turn the camera round between the ball and the pin (fCamTime is
// its angle, -2..2). The camera stays above the ground and over the ball.
void fn_800BFE00(View* pView, int nPlayer) {
    f32 vOld[4];
    f32 vA[4];
    f32 vB[4];
    f32 fAbove;
    f32* pCam;
    f32* pSub;
    f32* pTo;
    f32* pFrom;
    f32 fT;
    f32 f;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    Vec3Copy(pCam, vOld);
    if (pView->shot19C.f6C < 1.0f) {
        pView->f54 = 1.001f;
        if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x2E, 1))
            || (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x30, 1))) {
            pView->shot19C.f6C += lbl_80281F78->f210 * gSession.fFrameTime;
        } else {
            pView->shot19C.f6C -= lbl_80281F78->f210 * gSession.fFrameTime;
            if (pView->shot19C.f6C <= 0.0f) {
                pView->shot19C.f6C = 0.0f;
                pView->f54 = 1.0f;
            }
        }
        fn_80039344(gPlayers[nPlayer].nView[0], pView->shot19C.f6C);
    } else {
        if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x2E, 1))
            || (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x30, 1))) {
            if (pView->f54 + lbl_80281F78->f214 * gSession.fFrameTime < lbl_80281F78->f20C) {
                pView->f54 += lbl_80281F78->f214 * gSession.fFrameTime;
            }
        } else {
            pView->f54 -= lbl_80281F78->f214 * gSession.fFrameTime;
            if (pView->f54 < 1.0f) {
                pView->f54 = 1.0f;
            }
        }
        fn_80039344(gPlayers[nPlayer].nView[0], 1.0f);
    }
    if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x31, 1)) {
        pView->fCamTime += lbl_80281F78->f218 * gSession.fFrameTime;
        if (pView->fCamTime >= 2.0f) {
            pView->fCamTime -= 4.0f;
        }
    } else if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x32, 1)) {
        pView->fCamTime -= lbl_80281F78->f218 * gSession.fFrameTime;
        if (pView->fCamTime < -2.0f) {
            pView->fCamTime = 4.0f + pView->fCamTime;
        }
    }
    if (fabsf(pView->fCamTime) > 0.5f && fabsf(pView->fCamTime) <= 1.5f) {
        // to one side: swing both the camera and its aim about the ball
        if (pView->fCamTime > 0.0f) {
            fT = -lbl_80281F78->f21C;
        } else {
            fT = lbl_80281F78->f21C;
        }
        fn_800C7D14(pView->script.v0, pView->script.v10, 1, 1, pCam, fabsf(pView->fCamTime) - 0.5f, fT);
        fn_800C7D14(pView->script.v0, pView->script.v10, 1, 1, pSub, fabsf(pView->fCamTime) - 0.5f, 0.0f);
    } else {
        // behind the ball (looking at the pin) or behind the pin (looking at the ball)
        if (fabsf(pView->fCamTime) >= 1.5f) {
            pFrom = pView->script.v0;
            pTo = pView->script.v10;
            fT = 2.5f + pView->fCamTime;
            if (fT > 1.0f) {
                fT -= 4.0f;
            }
        } else {
            pFrom = pView->script.v10;
            pTo = pView->script.v0;
            fT = 0.5f + pView->fCamTime;
        }
        fn_800C7D14(pFrom, pTo, 1, 1, vA, 1.0f, -lbl_80281F78->f21C);
        fn_800C7D14(pFrom, pTo, 1, 1, vB, 1.0f, lbl_80281F78->f21C);
        fn_800C7E50(vA, vB, pTo, 2, pCam, fT);
        Vec_Copy(pTo, pSub);
    }
    CamScript_KeepAboveGround(nPlayer, pCam, vOld, 1, NULL, &fAbove, NULL, lbl_80281F78->f220);
    if (pCam[1] < pView->shot19C.f68 + lbl_80281F78->f220) {
        pCam[1] = pView->shot19C.f68 + lbl_80281F78->f220;
    }
    f = lbl_80281F78->f224 * (lbl_80281F78->f20C - 1.0f);
    if (lbl_80281F78->n228) {
        pSub[1] = pCam[1];
    } else {
        pSub[1] += f;
    }
    if (pView->shot19C.f6C < 1.0f) {
        fn_800C7D14(pView->v30, pView->v0, 1, 1, pCam, pView->shot19C.f6C, 0.0f);
        fn_800C7D14(pView->v40, pView->v10, 1, 1, pSub, pView->shot19C.f6C, 0.0f);
    }
    if (pView->shot19C.f6C >= 1.0f) {
        pView->f5C = 0.0f;
        pView->f60 = pCam[1] - pView->shot19C.f68;
        pView->f64 = 0.0f;
    }
}

// Camera 5: look at the pin through a 30-degree lens.
void fn_800C0364(View* pView, int nPlayer) {
    void* pSub;
    CourseInfo* pCourse;
    fn_8001731C(pView);
    pSub = fn_80017314(pView);
    pCourse = fn_8000C594();
    if (pCourse != NULL) {
        int nPinSet = Game_CurrentPinSet();
        Vec3Copy(&pCourse->pin[nPinSet].x, pSub);
        pView->p130 = NULL;
        pView->p134 = NULL;
        fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[0])), DEG(30.0f));
        pView->fCamTime = 0.0f;
    }
}

// Camera 5: from where the ball lay before the shot, swing out away from the pin over a time
// and distance scaled by how far the ball is from it, looking at the pin.
void fn_800C0414(View* pView, int nPlayer) {
    f32 vDir[4];
    f32 vToPin[4];
    f32 vOld[4];
    f32* pCam;
    CourseInfo* pCourse;
    int nPinSet;
    f32 fDist;
    f32 fTime;
    f32 fOut;
    f32 t;
    pCam = fn_8001731C(pView);
    fn_80017314(pView);
    pCourse = fn_8000C594();
    if (pCourse != NULL) {
        Vec3Copy(pCam, vOld);
        nPinSet = Game_CurrentPinSet();
        fn_800C73DC(&pCourse->pin[nPinSet].x, gPlayers[nPlayer].vBall, vToPin);
        vToPin[1] = 0.0f;
        fDist = fn_80009680(fn_80009744(vToPin));
        if (fDist < 0.1f) {
            fDist = 0.1f;
        }
        if (fDist < lbl_80281F78->f50) {
            fDist /= lbl_80281F78->f50;
            fTime = lbl_80281F78->f54 * fDist;
            fOut = lbl_80281F78->f50 * fDist;
        } else {
            fTime = lbl_80281F78->f54;
            fOut = lbl_80281F78->f50;
        }
        if (pView->fCamTime < fTime) {
            fn_800C73DC(gPlayers[nPlayer].ballBefore.vPos, &pCourse->pin[nPinSet].x, vDir);
            vDir[1] = 0.0f;
            if (vDir[0] != 0.0f || vDir[1] != 0.0f || vDir[2] != 0.0f) {
                fn_800BAF04(vDir, vDir);
            }
            t = 1.0f - pView->fCamTime / fTime;
            fn_8001EF34(vDir, fOut * (t * t), vDir);
            fn_800C73B8(vDir, gPlayers[nPlayer].ballBefore.vPos, pCam);
        } else {
            Vec3Copy(gPlayers[nPlayer].ballBefore.vPos, pCam);
        }
        pCam[1] = lbl_80281F78->f4C + gPlayers[nPlayer].ballBefore.vPos[1];
        fn_8003F2E0(&pView->script, gSession.fFrameTime);
        pView->f114 += gSession.fFrameTime;
        pView->fCamTime += gSession.fFrameTime;
        CamScript_KeepAboveGround(nPlayer, pCam, vOld, 1, NULL, NULL, NULL, lbl_80281F78->f168);
    }
}

// Camera 6: script 0x3A.
void fn_800C0624(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    CamShot* pShot;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    pShot = fn_8003A7C8(nPlayer, 0x3A, NULL);
    if (pShot != NULL) {
        CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f, 0x19,
                                       0.0f);
    }
}

// Camera 6.
void fn_800C06C8(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    fn_8003DCE8(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
}

// Camera 7 (the knee cam): a shot built by hand from the current one, lasting 0.55 s.
void fn_800C0744(View* pView, int nPlayer) {
    fn_8001731C(pView);
    fn_80017314(pView);
    pView->shot19C.p40 = NULL;
    pView->shot19C.f60 = 0.0f;
    pView->shot19C.f64 = 0.0f;
    pView->shot19C.f68 = 0.5f;
    pView->shot19C.f6C = 0.5f;
    pView->shot19C.f70 = 0.0f;
    pView->shot19C.f74 = 0.0f;
    pView->shot19C.f78 = pView->p130->f78;
    pView->shot19C.f7C = pView->shot19C.f78;
    pView->shot19C.bAA = pView->p130->bAA;
    pView->shot19C.f84 = 0.0f;
    pView->shot19C.bA8 = 1;
    pView->shot19C.bAB = 1;
    pView->shot19C.bAC = 9;
    pView->shot19C.bAF = 1;
    pView->shot19C.bB0 = 9;
    pView->shot19C.bB1 = 2;
    pView->shot19C.bB2 = 0;
    pView->p134 = &pView->shot19C;
    pView->fCamTime = 0.0f;
    pView->n140 = 1;
    pView->f110 = 0.55f;
}

// Camera 7 (the knee cam).
void fn_800C0804(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    fn_8003DCE8(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
}

// Camera 10: start on the first shot fn_8006509C gives.
void fn_800C0880(View* pView, int nPlayer) {
    CamShot* pShot;
    s32 n;
    n = lbl_80282220->n60;  // fake match: read before the store below, in the original's order
    lbl_80282220->f68 = 0.0f;
    pShot = fn_8006509C(n);
    if (pShot != NULL) {
        pView->p130 = pShot;
        pView->p134 = pShot->p40;
        pView->f110 = pView->p130->f48;
        pView->n140 = pView->p130->bAB;
        pView->fCamTime = 0.0f;
        pView->f128 = 0.0f;
        pView->f124 = 0.0f;
    } else {
        pView->p130 = NULL;
        pView->fCamTime = 0.0f;
    }
    pView->b153 = 0;
}

// Camera 10: when the next shot is the last of its group (no successor, or one with another nA4)
// and less than lbl_80281F78->f178 of this shot is left, start the move to the tuning's position
// (nCamera 1, f118 the time left).
void fn_800C0914(View* pView, int nPlayer) {
    f32 v[4];
    void* pCam;
    void* pSub;
    f32 fLead;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    fLead = lbl_80281F78->f178;
    Vec_Copy(lbl_80281F78->v17C, v);
    fn_8003EA50(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
    if (pView->p134 != NULL && (pView->p134->p40 == NULL || pView->p134->nA4 != pView->p134->p40->nA4)
        && pView->nCamera == 0 && pView->f110 - pView->fCamTime < fLead) {
        pView->nCamera = 1;
        Vec_Copy(v, pView->vC4);
        pView->f114 = 0.0f;
        pView->f118 = pView->f110 - pView->fCamTime;
    }
}

// Camera 11, the pre-shot camera: shot 0x20 of the plan, or 49 times in 100 (or without one) shot 13
// of fn_8003C9D0's sequence or a new pre-flight sequence.
void GolfCamera_InitPreShotCamera(View* pView, int nPlayer) {
    int nLie;
    CamShot* pShot = NULL;
    int nA = 5;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32* pCam = fn_8001731C(pView);
    f32* pSub = fn_80017314(pView);
    int nB = 0x19;
    f32 f3 = 0.0f;
    lbl_80282220->f68 = 0.0f;
    pView->b153 = 0;
    nLie = gPlayers[nPlayer].ball.nLie;
    pView->p74 = NULL;
    pShot = fn_80064F7C(nPlayer, 0x20, 0, pView->p130);
    if (pShot == NULL || Rand_Next(1) % 100 > 50) {
        if (fn_8003C9D0(nPlayer, 0, &pView->p74, &pShot) && pView->p74 != NULL) {
            pShot = fn_8003A950(pView->p74, 13, &nA, &f1, &f2, &nB, &f3, nPlayer);
        }
        if (pShot == NULL) {
            pView->p74 = DynamicCam_ChoosePreFlightSequence(nPlayer, nLie, 1);
            pShot = fn_8003A950(pView->p74, 13, &nA, &f1, &f2, &nB, &f3, nPlayer);
        }
        pView->n14C = 13;
        pView->n148 = 13;
    }
    if (pShot != NULL) {
        CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, nA, f1, f2, nB, f3);
        if (pView->nCamera != 0) {
            pView->f114 = 0.0f;
            pView->nCamera = 2;
        }
        pView->n194 = 0;
        pView->n198 = 0;
    }
}

// Camera 11's process: start the shot kind asked for (n148) once it changes, but not before the
// swing animation has a second left, unless the camera has already cut (n194).
void fn_800C0C0C(View* pView, int nPlayer) {
    f32* pCam;
    f32* pSub;
    u8 bStart;
    f32 f1;
    f32 f2;
    int nB;
    f32 f3;
    int nA;
    CamShot* pShot;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    nB = 0x19;
    bStart = 1;
    f1 = 0.0f;
    f2 = 0.0f;
    f3 = 0.0f;
    nA = 5;
    if (pView->n148 != pView->n14C) {
        if (pView->n148 == 0x17) {
            pView->n194 = 1;
            if (pView->nCamera != 0) {
                pView->f114 = 0.0f;
                pView->nCamera = 2;
            }
        }
        if (pView->n194 < 1 && fn_80062C28(gPlayers[nPlayer].pChar) < 1.0f) {
            bStart = 0;
        }
        pView->n14C = pView->n148;
        if (bStart) {
            pShot = fn_8003A950(pView->p74, pView->n148, &nA, &f1, &f2, &nB, &f3, nPlayer);
            if (pShot != NULL) {
                CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, nA, f1, f2, nB,
                                               f3);
            }
        }
    }
    fn_8003DCE8(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
}

// Camera 12, the swing camera: pick the pre-flight sequence (the saved one, the follow-on of a kind 1
// or 2 sequence, or a new one for the lie) and its shot of kind 9. Outside a replay the choice is
// saved, and a big height difference to the target (over 10 up or down) or a ball on lies 3..5 can
// swap in one of the plan's own shots. A shot that would hide the golfer is swapped for one from
// sequence 0x1C.
void GolfCamera_InitSwingCamera(View* pView, int nPlayer) {
    int nA;
    f32 f1;
    f32 f2;
    int nB;
    f32 f3;
    CamSequence* pAltSeq;
    CamShot* pAltShot;
    CamShot* pShot = NULL;
    CamShot* pClear = NULL;
    f32* pCam;
    f32* pSub;
    int nLie;
    CamSequence* pOldSeq;
    CamSequence* pSeq;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    pOldSeq = pView->p74;
    pView->b153 = 0;
    lbl_80282220->f68 = 0.0f;
    lbl_80282220->b5D = 0;
    fn_800C1790(pView, nPlayer);
    pView->n260 = 0;
    lbl_80282220->b5A = 0;
    lbl_80282220->b5B = 0;
    nLie = gPlayers[nPlayer].ball.nLie;
    if (pView->p7C != NULL && !gSession.bReplay) {
        pView->p74 = pView->p7C;
    } else if (pView->p74 != NULL && (pView->p74->b44 == 2 || pView->p74->b44 == 1)
               && pView->p74->p20 != pView->p74 && pView->p74->p20 != NULL && pView->p74->p20->b44 == 3) {
        pView->p74 = pView->p74->p20;
        if (!gSession.bReplay) {
            pView->p7C = pView->p74;
        }
    } else {
        pView->p74 = DynamicCam_ChoosePreFlightSequence(nPlayer, nLie, 3);
        if (!gSession.bReplay) {
            pView->p7C = pView->p74;
        }
    }
    if (!gSession.bReplay) {
        pView->p7C = pView->p74;
        pView->p78 = pView->p74;
    }
    if (pView->p80 != NULL && !gSession.bReplay) {
        if (pView->n264 > 0 && pView->n264 != 3) {
            pShot = fn_800C4DF8(pView->n264, nPlayer);
        }
        if (pShot == NULL) {
            pShot = pView->p80;
        }
        f3 = 0.0f;
        nB = 0x19;
        nA = 5;
        f2 = 100.0f;
        f1 = 0.0f;
    } else {
        pShot = fn_8003A950(pView->p74, 9, &nA, &f1, &f2, &nB, &f3, nPlayer);
        if (!gSession.bReplay) {
            pView->n264 = 0;
            pView->p80 = pShot;
        }
        if (!Player_IsCPU(nPlayer)) {
            f32 fRise = gPlayers[nPlayer].vTarget[1] - gPlayers[nPlayer].vBall[1];
            if (fRise > 10.0f) {
                pShot = fn_800C4DF8(4, nPlayer);
                if (pShot == NULL) {
                    pShot = pView->p80;
                } else {
                    pView->n264 = 4;
                    f3 = 0.0f;
                    nB = 0x19;
                    nA = 5;
                    f2 = 100.0f;
                    f1 = 0.0f;
                }
            } else if (fRise < -10.0f) {
                pShot = fn_800C4DF8(2, nPlayer);
                if (pShot == NULL) {
                    pShot = pView->p80;
                } else {
                    pView->n264 = 2;
                    f3 = 0.0f;
                    nB = 0x19;
                    nA = 5;
                    f2 = 100.0f;
                    f1 = 0.0f;
                }
            }
        }
        if (fn_8012022C() && (nLie == 3 || nLie == 4 || nLie == 5) && gPlayers[nPlayer].nClub != 2) {
            pShot = fn_800C4DF8(5, nPlayer);
            if (pShot == NULL) {
                pShot = pView->p80;
            } else {
                pView->n264 = 5;
                f3 = 0.0f;
                nB = 0x19;
                nA = 5;
                f2 = 100.0f;
                f1 = 0.0f;
            }
        }
    }
    if (pShot != NULL) {
        if (CameraScript_WillGolferBeOccludedInThisView(nPlayer, pShot, &pView->script)) {
            pSeq = DynamicCam_ChoosePreFlightSequence(nPlayer, nLie, 0x1C);
            if (pSeq != NULL) {
                pClear = fn_8003A950(pSeq, 9, &nA, &f1, &f2, &nB, &f3, nPlayer);
            }
            if (pClear != NULL) {
                pView->p74 = pSeq;
                pShot = pClear;
                pView->n264 = 0;
            }
        }
        if (pView->p80 != NULL) {
            // the saved shot moves on to the last shot of its chain, stopping before kinds 6 and 8..10
            while (pView->p80->p40 != NULL) {
                if (pView->p80->p40->bAB == 6 || pView->p80->p40->bAB == 8 || pView->p80->p40->bAB == 9
                    || pView->p80->p40->bAB == 10) {
                    break;
                }
                pView->p80 = pView->p80->p40;
            }
        }
        if (gSession.nSplitScreen || pView->b268) {
            while (pShot->p40 != NULL) {
                if (pShot->p40->bAB == 6 || pShot->p40->bAB == 8 || pShot->p40->bAB == 9
                    || pShot->p40->bAB == 10) {
                    break;
                }
                pShot = pShot->p40;
            }
            nA = 5;
        }
        if (pView->nCurCamera != 0 || pOldSeq == NULL || pOldSeq->b47) {
            nA = 5;
            f1 = 0.0f;
        }
        if (fn_80095780(gPlayers[nPlayer].pChar) == 11 && fn_8003C9D0(nPlayer, 0, &pAltSeq, &pAltShot)) {
            if (pAltSeq != NULL) {
                pView->p74 = pAltSeq;
                pShot = fn_8003A950(pAltSeq, 9, &nA, &f1, &f2, &nB, &f3, nPlayer);
            } else if (pAltShot != NULL) {
                nA = 5;
                pShot = pAltShot;
                f1 = 0.0f;
                f2 = 100.0f;
                nB = 0x19;
                f3 = 0.0f;
            }
        }
        CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, nA, f1, f2, nB, f3);
        if (gSession.nSplitScreen) {
            pView->p134 = NULL;
        }
        pView->n194 = 0;
        if (pView->nCamera == 4) {
            pView->nCamera = 2;
            pView->f114 = 0.0f;
        }
    }
}

// Camera 12's process: shots of kind 6, 8, 9 and 10 have no next shot while the script runs; in super
// slow motion the script steps one fixed frame (FRAME_TIME); on the downswing kinds 9 and 10 give way
// to 0 and 2.
void fn_800C1338(View* pView, int nPlayer) {
    f32* pCam;
    f32* pSub;
    f32 fTime;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    fTime = gSession.fFrameTime;
    if (pView->n140 == 6 || pView->n140 == 8 || pView->n140 == 9 || pView->n140 == 10) {
        pView->p134 = NULL;
    }
    if (fn_800DC514(nPlayer) && gSession.nPaused == 0) {
        fTime = FRAME_TIME;
    }
    fn_8003DCE8(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, fTime);
    if (pView->n140 == 6 || pView->n140 == 8 || pView->n140 == 9 || pView->n140 == 10) {
        pView->p134 = pView->p130->p40;
    }
    if (gPlayers[nPlayer].swing.nState == SW_DOWN_SWING) {
        if (pView->n140 == 9) {
            pView->n140 = 0;
            pView->fCamTime = 0.0f;
            pView->b150 = 1;
        } else if (pView->n140 == 10) {
            pView->n140 = 2;
            pView->fCamTime = 0.0f;
            pView->b150 = 1;
        }
    }
}

// Camera 13.
void fn_800C14B0(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    pView->n194 = 0;
    pView->n198 = 0;
    pView->fCamTime = 0.0f;
    lbl_80282220->f68 = 0.0f;
    fn_800C5D64(pView, pCam, pSub, nPlayer);
}

// Camera 13's process.
void fn_800C1530(View* pView, int nPlayer) {
    f32 v[4];
    f32* pCam;
    f32* pSub;
    CamShot* pShot;
    s32 n140;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    if (fn_800C6D80()) {
        fn_800C5EC0(pView, pCam, pSub, nPlayer);
    }
    Vec_Copy(lbl_80281F78->v68, v);
    if (!fn_800C6D80() && pView->fCamTime < lbl_80281F78->f78) {
        v[3] *= 1.0f - pView->fCamTime / lbl_80281F78->f78;
        fn_80038010(1, fn_80016D10(), v);
    }
    pShot = pView->p130;
    n140 = pView->n140;
    fn_8003DCE8(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
    if (fn_800C6D80() && pShot != pView->p130) {
        if (pShot != NULL && pShot->p40 == NULL) {
            pView->n140 = 5;
            pView->f110 = 0.0f;
            pView->p134 = NULL;
        } else {
            pView->p134 = pShot;
            pView->n140 = n140;
        }
    }
}

// Camera 20.
void fn_800C1670(View* pView, int nPlayer) {
    if (pView->n260 == 9) {
        fn_800B3550(1, pView, nPlayer);
    } else {
        fn_800B3550(0, pView, nPlayer);
    }
    lbl_80282220->b56 = 1;
}

// Camera 20: once fn_800B36F4 is done, back to the full view and on to the flight camera (14).
void fn_800C16C4(View* pView, int nPlayer) {
    f32* pCam;
    f32* pSub;
    int nView;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    nView = gPlayers[nPlayer].nView[0];
    if (fn_800B36F4(pView, nPlayer, gSession.fFrameTime)) {
        fn_800C1790(pView, nPlayer);
        View_SetCamera(fn_80017028(nView), 14, nPlayer, nView);
        GolfCamera_ProcessBallFlightCamera(pView, nPlayer);
    } else {
        fn_8003DCE8(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
    }
}

// Undo what camera 20 set up (b56): the view's rectangle back to 0,0-1,1 and the render state reset.
void fn_800C1790(View* pView, int nPlayer) {
    int nView = gPlayers[nPlayer].nView[0];
    if (lbl_80282220->b56) {
        fn_800171D8(fn_80012EF0(fn_80017004(nView)), 0.0f, 0.0f, 1.0f, 1.0f);
        fn_800352BC();
        fn_80035240(0);
        fn_80013CCC(fn_8001614C());
        fn_80013EEC(fn_8001614C());
        fn_80016B9C();
        fn_80012EF8();
        lbl_80282220->b56 = 0;
        lbl_80282220->b57 = 0;
    }
}

// Camera 21, the heartbeat camera: script 0x2E of the current shot (13 without), and slow motion
// slowed so the swing up to event 2 lasts the tuning's beats.
void GolfCamera_InitHeartBeatCamera(View* pView, int nPlayer) {
    f32* pCam = fn_8001731C(pView);
    f32* pSub = fn_80017314(pView);
    f32 v[4] = {0.0f, 0.0f, 0.0f, 0.5f};
    CamShot* pShot;
    f32 fRate;
    f32 fStart = 0.0f;      // fake match: a variable, not the literal (x - 0.0f folds away)
    pShot = fn_8003A7C8(nPlayer, 0x2E, pView->p130);
    if (pShot == NULL) {
        pShot = fn_8003A7C8(nPlayer, 0xD, pView->p130);
    }
    if (pShot != NULL && !CameraScript_WillGolferBeOccludedInThisView(nPlayer, pShot, &pView->script)) {
        CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f, 0x19,
                                       0.0f);
    }
    pView->n194 = 0;
    pView->f18C = 0.0f;
    lbl_80282220->b5A = 1;
    fn_80063B98(pView, lbl_80281F78->fC4, v);
    fRate = (FRAME_RATE * (fn_800C741C(gPlayers[nPlayer].pChar, 2) - fStart))
          / (FRAME_RATE * (lbl_80281F78->fC4 * (lbl_80281F78->nBeatFrames * (lbl_80281F78->nBeats + 1))
                       + (lbl_80281F78->nBeats * lbl_80281F78->fC8
                          + lbl_80281F78->nBeats * lbl_80281F78->fCC)));
    GameEffects_SetSuperSlowMo(1, nPlayer, fRate);
    lbl_80282220->f64 = fRate;
}

// Camera 21: the heartbeats. While b5A is set, each beat (every nBeatFrames steps of n194, nBeats
// of them) cuts to a new angle, the last to the saved shot p80. The script steps one fixed frame
// (FRAME_TIME) at a time.
void GolfCamera_ProcessHeartBeatCamera(View* pView, int nPlayer) {
    f32* pCam = fn_8001731C(pView);
    f32* pSub = fn_80017314(pView);
    CamShot* pShot = NULL;
    f32 v[4] = {0.0f, 0.0f, 0.0f, 0.5f};
    if (gSession.nPaused == 0) {
        if (lbl_80282220->b5A) {
            if (pView->nCamera == 5) {
                if (pView->n194 >= lbl_80281F78->nBeats * lbl_80281F78->nBeatFrames) {
                    lbl_80282220->b5A = 0;
                } else if ((pView->n194 + 1) % lbl_80281F78->nBeatFrames == 0) {
                    fn_80063BF4(pView, lbl_80281F78->fC8, v);
                } else {
                    fn_80063B98(pView, lbl_80281F78->fC4, v);
                    pView->n194++;
                }
            } else if (pView->f18C > lbl_80281F78->fCC) {
                if (pView->n194 >= (lbl_80281F78->nBeats - 1) * lbl_80281F78->nBeatFrames) {
                    pShot = pView->p80;
                }
                if (pShot == NULL) {
                    pShot = fn_8003A7C8(nPlayer, 0x2E, pView->p130);
                }
                if (pShot == NULL) {
                    pShot = fn_8003A7C8(nPlayer, 0xD, pView->p130);
                }
                if (pShot != NULL
                    && !CameraScript_WillGolferBeOccludedInThisView(nPlayer, pShot, &pView->script)) {
                    CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 0.0f,
                                                   100.0f, 0x19, 0.0f);
                }
                fn_80063B98(pView, lbl_80281F78->fC4, v);
                pView->n194++;
                pView->f18C = 0.0f;
            } else if (pView->nCamera == 0 || pView->nCamera == 4 || pView->nCamera == 3) {
                pView->f18C += FRAME_TIME;
                fn_80063CBC(pView, v);
            }
        }
        fn_8003DCE8(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, FRAME_TIME);
    }
}

// Camera 22, the shutter camera: script 0x3E of the current shot (13 when it has none), with the
// game in super slow motion.
void GolfCamera_InitShutterCamera(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    CamShot* pShot;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    pShot = fn_8003A7C8(nPlayer, 0x3E, pView->p130);
    if (pShot == NULL) {
        pShot = fn_8003A7C8(nPlayer, 0xD, pView->p130);
    }
    if (pShot != NULL) {
        CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f, 0x19,
                                       0.0f);
    }
    pView->n194 = 0;
    pView->f18C = 0.0f;
    pView->f190 = 0.0f;
    lbl_80282220->b5B = 1;
    pView->f190 = 0.0f;     // stored twice, as in the original
    GameEffects_SetSuperSlowMo(1, nPlayer, 1.0f);
}

// Camera 22, the shutter camera: while f18C is below 0 the shutter (post effect 0) closes and
// reopens over 0.15 s either side; the script steps one fixed frame (FRAME_TIME). At a third and at
// two thirds of the swing up to event 2 the shutter fires (f18C back to -0.15), and when it reaches
// 0 the camera cuts to shot 0x3E + n194 of the current one.
void fn_800C1D3C(View* pView, int nPlayer) {
    f32* pCam = fn_8001731C(pView);
    f32* pSub = fn_80017314(pView);
    f32 v[4] = {0.0f, 0.0f, 0.0f, 0.5f};
    f32 fTime;
    f32 fSwing;
    f32 fStart = 0.0f;      // fake match: a variable, not the literal (x - 0.0f folds away)
    CamShot* pShot;
    if (gSession.nPaused == 0) {
        fTime = pView->f18C;
        if (fTime < 0.0f) {
            v[3] = 0.5f * (1.0f - -fTime / 0.15f);
            fn_800380A8(1, v, 0, 0, 0.5f, 0.5f);
            v[3] = v[3] * 2.0f;
            v[3] = v[3] * v[3];
            v[3] = v[3] / 2.0f;
            fn_80038010(1, 0, v);
        } else if (fTime < 0.15f) {
            v[3] = 0.5f * (1.0f - fTime / 0.15f);
            fn_800380A8(1, v, 0, 0, 0.5f, 0.5f);
            fn_80038010(1, 0, v);
            v[3] = v[3] * 2.0f;     // dead: v is not used again, as in the original
            v[3] = v[3] * v[3];
            v[3] = v[3] / 2.0f;
        }
        fn_8003DCE8(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, FRAME_TIME);
        pView->f18C += gSession.fFrameTime;
        pView->f190 += gSession.fFrameTime;
        fSwing = fn_800C741C(gPlayers[nPlayer].pChar, 2) - fStart;
        if (pView->n194 == 0 && pView->f190 > fSwing / 3.0f) {
            pView->f18C = -0.15f;
            pView->n194++;
        } else if (pView->n194 == 1 && pView->f190 > 2.0f * (fSwing / 3.0f)) {
            pView->f18C = -0.15f;
            pView->n194++;
        }
        if (fTime < 0.0f && gSession.fFrameTime + fTime >= 0.0f) {
            pShot = fn_8003A7C8(nPlayer, pView->n194 + 0x3E, pView->p130);
            if (pShot != NULL) {
                CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f,
                                               0x19, 0.0f);
            }
        }
    }
}

// The ball-flight camera: pick the flight sequence (the pre-flight sequence's follow-on if it has
// one, else one for the shot's lie, surface class and distance), fall back on the current camera as
// a hand-made shot, and start the matrix camera or the super zoom when the swing camera asks.
void GolfCamera_InitBallFlightCamera(View* pView, int nPlayer) {
    f32 vAim[4];
    f32 vDiff[4];
    f32* pCam;
    f32* pSub;
    int nClass;
    int nLie;
    f32 fDist;
    CamSequence* pSeq;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    pView->b153 = 0;
    nLie = gPlayers[nPlayer].ball.nLie;
    if (gSession.bReplay && gPlayers[nPlayer].ballBefore.nState == 0) {
        // a replay whose look-ahead ball has stopped: the distance that ball went
        if (gPlayers[nPlayer].ballBefore.nSurface >= 0) {
            nClass = gSurfaceTypes[gPlayers[nPlayer].ballBefore.nSurface].nClass;
        } else {
            nClass = 10;
        }
        fn_800C73DC(gPlayers[nPlayer].ballBefore.vPos, gPlayers[nPlayer].ball.vStart, vDiff);
        fDist = fn_80009680(fn_80009744(vDiff));
    } else {
        fDist = AI_MaxDistance(nPlayer, gPlayers[nPlayer].nShotKind, gPlayers[nPlayer].nClub);
        fDist *= fn_800510EC(&gPlayers[nPlayer].ball);
        fDist *= fn_8005B64C(nPlayer);
        nClass = 2;
        fn_800C73DC(gPlayers[nPlayer].vTarget, gPlayers[nPlayer].vBall, vAim);
        fn_800BAF04(vAim, vAim);
        fn_8001EF34(vAim, fDist, vAim);
        fn_800C73B8(vAim, gPlayers[nPlayer].vBall, pView->vD4);
    }
    if (pView->p74 != NULL && pView->p74->b44 == 3 && pView->p74->p20 != pView->p74 && pView->p74->p20 != NULL
        && pView->p74->p20->b44 == 4 && fn_8003D7A0(pView->p74->p20, nPlayer)) {
        pView->p74 = pView->p74->p20;
    } else {
        pView->p74 = fn_8003BDBC(nPlayer, nLie, nClass, 4, 1, fDist);
    }
    if (gPlayers[nPlayer].nShotKind == 5 && (pView->p74 == NULL || pView->p74->b46 != 6)) {
        pSeq = fn_8003BDBC(nPlayer, nLie, nClass, 4, 0, fDist);
        if (pSeq != NULL && pSeq->b46 == 6) {
            pView->p74 = pSeq;
        }
    }
    if (pView->p74 == NULL) {
        CameraScript_RecordCurrentCam(&pView->shot19C, pCam, pSub, nPlayer, &pView->script, 0);
        pView->p130 = &pView->shot19C;
        pView->p134 = NULL;
        pView->fCamTime = 0.0f;
        pView->p130->f98 = 0.0f;
    }
    if (pView->p130 != NULL && pView->p134 != NULL && pView->n140 == 6) {
        pView->n140 = 0;
        pView->fCamTime = 0.0f;
        pView->b150 = 1;
    } else if (pView->p130 != NULL && pView->p134 != NULL && pView->n140 == 8) {
        pView->n140 = 2;
        pView->fCamTime = 0.0f;
        pView->b150 = 1;
    } else if (pView->p134 == NULL) {
        if (fn_800C44A8(pView, nPlayer)) {
            CameraScript_RecordCurrentCam(&pView->shot19C, pCam, pSub, nPlayer, &pView->script, 0);
            pView->shot19C.p44 = pView->p130;
            pView->p130 = &pView->shot19C;
            pView->fCamTime = 0.0f;
            pView->p130->f4C = -1.0f;
            pView->p134 = NULL;
        } else {
            CameraScript_RecordCurrentCam(&pView->shot19C, pCam, pSub, nPlayer, &pView->script, 0);
            pView->shot19C.p44 = pView->p130;
            pView->p130 = &pView->shot19C;
            pView->fCamTime = 0.0f;
            if (pView->p74 != NULL) {
                pView->p130->f4C = pView->p74->f38;
            } else {
                pView->p130->f4C = 1.0f;
            }
            pView->p134 = NULL;
        }
    }
    pView->p13C = NULL;
    if (fn_800C43C0(pView, nPlayer)) {
        lbl_80282220->b54 = 1;
        fn_8001966C(gPlayers[nPlayer].pChar);
        fn_8007325C(gPlayers[nPlayer].pChar->anim);
        fn_800C4FF0(pView, pCam, pSub, nPlayer);
        pView->f18C = 0.0f;
        EVENT_Trigger(nPlayer, 0x32, NULL, -1);
    } else if (fn_800C44F4(pView, nPlayer)) {
        lbl_80282220->b58 = 1;
        fn_8007325C(gPlayers[nPlayer].pChar->anim);
        fn_800C56B4(pView, pCam, pSub, nPlayer);
        EVENT_Trigger(nPlayer, 0x39, NULL, -1);
    } else if (pView->n260 == 15 || pView->n260 == 16) {
        pView->p130->f4C = 1.0f;
    }
    pView->n148 = 0;
    pView->n14C = 9;
    if (pView->nCamera != 0 && pView->nCamera != 2) {
        pView->nCamera = 0;
    }
}

// The ball-flight camera's tick: when the current shot has run its time (or the swing camera's
// shots are done), pick the next one: a CPU's or a replay's shot for the flight, else the
// sequence's latest kind-0x18 choice the flight has reached, else a shot of the kind asked for.
// Kind-5 cuts wait for the golfer's follow-through (animation 14). Slow motion comes from the
// swing camera kinds' ticks (fn_800C54FC, fn_800C5A70).
void GolfCamera_ProcessBallFlightCamera(View* pView, int nPlayer) {
    f32 vOld[4];
    f32 f1;
    int nA;
    f32 f2;
    int nB;
    CamShot* pAltShot;
    CamSequence* pAltSeq;
    f32 f3;
    f32* pCam;
    f32* pSub;
    CamShot* pShot;
    CamSequence* pSeq;
    f32 fTime;
    f32 fBest;
    f32 fNow;
    f32 f;
    int nBest;
    int i;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    f1 = 0.0f;
    nA = 5;
    nB = 0x19;
    pShot = NULL;
    f3 = 0.0f;
    fn_80012EF0(fn_80017004(gPlayers[nPlayer].nView[0]));
    Vec3Copy(pCam, vOld);
    fn_800638B8(pView, nPlayer);
    if (fn_800C6D28()) {
        fTime = fn_800C54FC(pView, pCam, pSub, nPlayer);
    } else if (fn_800C6D64()) {
        fTime = fn_800C5A70(pView, pCam, pSub, nPlayer);
    } else {
        fTime = gSession.fFrameTime;
    }
    if (gSession.fFrameTime != 0.0f && !pView->b153
        && ((!lbl_80282220->b5D && fn_800C7450())
            || ((pView->p130 == NULL || pView->fCamTime > pView->p130->f4C)
                && (pView->p134 == NULL || pView->p80 != pView->p130)))) {
        if (gSession.bReplay || Player_IsCPU(nPlayer)) {
            if (gPlayers[nPlayer].nClub == 25) {
                pShot = fn_80064F7C(nPlayer, 0x10, 0, pView->p130);
            } else if (gPlayers[nPlayer].ball.nState == 2) {
                if (gPlayers[nPlayer].ball.nCollideCount > 0) {
                    pShot = fn_80064F7C(nPlayer, 4, 0, pView->p130);
                } else {
                    pShot = fn_80064F7C(nPlayer, 2, 0, pView->p130);
                }
            } else {
                pShot = fn_80064F7C(nPlayer, 8, 0, pView->p130);
            }
        }
        if (pShot == NULL) {
            // the latest kind-0x18 choice the flight has already reached
            fBest = 0.0f;
            nBest = -1;
            fNow = fn_80044EA8(nPlayer, &pView->script);
            pSeq = pView->p74;
            if (pSeq != NULL) {
                for (i = 0; i < pSeq->nChoices; i++) {
                    if (pSeq->p4C[i].b14 == 0x18 && pSeq->p4C[i].fC < fNow && pSeq->p4C[i].fC > fBest) {
                        fBest = pSeq->p4C[i].fC;
                        nBest = i;
                    }
                }
            }
            if (nBest >= 0 && pSeq != NULL && pSeq->p4C[nBest].p10 != pView->p13C) {
                pShot = pSeq->p4C[nBest].p10;
                nA = pSeq->p4C[nBest].b15;
                f1 = pView->p74->p4C[nBest].f0;
                f2 = pView->p74->p4C[nBest].f4;
                nB = pView->p74->p4C[nBest].b16;
                f3 = pView->p74->p4C[nBest].f8;
            } else {
                pShot = fn_8003A950(pSeq, pView->n148, &nA, &f1, &f2, &nB, &f3, nPlayer);
            }
            if (pShot == NULL && pView->n14C == 9) {
                pShot = fn_8003A950(pView->p74, 0, &nA, &f1, &f2, &nB, &f3, nPlayer);
            }
        }
        if (pShot != NULL && (pView->b153 || (pShot != pView->p13C && pView->n14C != pView->n148))) {
            if ((!CameraScript_IsDefaultSwingCam(pView->p130, nPlayer, pCam) && pView->p130->bA8 == 0
                 && pView->p130->bAD == 4)
                || pView->b153) {
                nA = 5;
                f1 = 0.0f;
                pView->b153 = 0;
            }
            pView->p13C = pShot;
            if (nA != 5 && pView->p130 != NULL && pView->p130->bAD != 4) {
                if (!CameraScript_IsDefaultSwingCam(pView->p130, nPlayer, pCam)) {
                    nA = 5;
                    f1 = 0.0f;
                } else {
                    f = fn_800D04AC(nPlayer);
                    if (f > 15.0f) {
                        f1 += 0.05f * (f - 15.0f);
                    }
                }
            }
            if (!fn_800451A8(&pView->script, pShot, nPlayer)) {
                if (nA == 5 && fn_8003DC78(pShot) && gPlayers[nPlayer].nShotKind != 0
                    && fn_80095780(gPlayers[nPlayer].pChar) != 9) {
                    // a cut: only once the golfer is in animation 14
                    if (fn_80095780(gPlayers[nPlayer].pChar) == 14) {
                        if (pView->p130->bA8 == 0 || (pView->p130->bAF != 12 && pView->p130->bB0 != 12)
                            || nA == 5) {
                            CameraScript_UpdateLandingEstimate(&pView->script, nPlayer);
                        }
                        if (f3 > fn_80062C28(gPlayers[nPlayer].pChar)) {
                            f3 = fn_80062C28(gPlayers[nPlayer].pChar);
                        }
                        if (fn_8003C9D0(nPlayer, 0, &pAltSeq, &pAltShot)) {
                            if (pAltShot == NULL) {
                                CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, nA,
                                                               f1, f2, nB, f3);
                                if (fn_800C7450()) {
                                    lbl_80282220->b5D = 1;
                                }
                            } else {
                                CameraScript_InterpToNewScript(&pView->script, pAltShot, nPlayer, pCam, pSub,
                                                               nA, f1, f2, nB, f3);
                                if (fn_800C7450()) {
                                    lbl_80282220->b5D = 1;
                                }
                            }
                        } else {
                            CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, nA, f1,
                                                           f2, nB, f3);
                            if (fn_800C7450()) {
                                lbl_80282220->b5D = 1;
                            }
                        }
                        pView->n14C = pView->n148;
                    } else {
                        fn_80095744(gPlayers[nPlayer].pChar, 14);
                        pView->p13C = pView->p130;
                    }
                } else {
                    CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, nA, f1, f2,
                                                   nB, f3);
                    pView->n14C = pView->n148;
                    if (fn_800C7450()) {
                        lbl_80282220->b5D = 1;
                    }
                }
            } else {
                pView->n14C = pView->n148;
            }
        }
    }
    if (pView->nCamera == 4) {
        pView->nCamera = 2;
        pView->f114 = 0.0f;
    }
    fn_8003DCE8(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, fTime);
    if (fn_800C6D28() && gSession.nPaused == 0) {
        fn_80038054(1, fn_80016D10(), 0.0f, lbl_80281F78->f64);
    }
}

// Camera 15, the post-shot camera: the crowd flyby, else shot 0x40 of the plan or shot 5 of the
// sequence. For a ball that ended near the green (lies 6..8, or kind 10 asked for) only after fB8.
void GolfCamera_InitPostShotCamera(View* pView, int nPlayer) {
    CamShot* pShot = NULL;
    f32* pCam = fn_8001731C(pView);
    f32* pSub = fn_80017314(pView);
    int nA = 5;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    int nB = 0x19;
    f32 f3 = 0.0f;
    pView->n194 = 0;
    pView->p78 = pView->p74;
    if (pView->n148 != 6 && pView->n148 != 8 && pView->n148 != 10) {
        fn_80063CF0(pView, 5, nPlayer);
    }
    pView->n198 = 0;
    if ((pView->n148 != 10 && gPlayers[nPlayer].ball.nLie != 6 && gPlayers[nPlayer].ball.nLie != 7
         && gPlayers[nPlayer].ball.nLie != 8)
        || pView->fCamTime >= lbl_80281F78->fB8) {
        if (fn_800C7160(pView)) {
            fn_800C6110(pView, nPlayer, 0x40);
        } else {
            if (GM_ShowPostShotCrowdFlyby()) {
                pShot = fn_8006509C(9);
            }
            if (pShot != NULL) {
                pView->p130 = pShot;
                pView->p134 = pShot->p40;
                pView->f110 = pView->p130->f48;
                pView->n140 = pView->p130->bAB;
                pView->fCamTime = 0.0f;
                pView->f124 = 0.0f;
                pView->f128 = 0.0f;
            } else {
                pShot = fn_80064F7C(nPlayer, 0x40, 1, pView->p130);
                if (pShot != NULL && fn_80062C28(gPlayers[nPlayer].pChar) < 1.0f) {
                    pView->n198 = 1;
                }
                if (pShot == NULL || pShot->bAC == 5) {
                    pShot = fn_8003A950(pView->p74, 5, &nA, &f1, &f2, &nB, &f3, nPlayer);
                    if (pShot != NULL && pShot->bAA == 0) {
                        pView->n198 = 1;
                    }
                }
                if (pShot != NULL) {
                    CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, nA, f1, f2, nB,
                                                   f3);
                    pView->b153 = 0;
                }
            }
        }
        pView->n194 = 1;
    }
}

// Camera 15, the post-shot camera. For a shot kind fn_8004562C accepts, f74 eases (1 in 100 a
// frame) towards the ball's ground distance / 30, kept to 0.5..5; for kind 10 the shot's height
// creeps up while it is less than 1 over the ground at the ball. Once the golfer's animation 9 is
// under way, the crowd flyby or the cut to the golfer. When the script runs out of shots, hold the
// camera as a hand-made shot looking along m's z axis (m built from pSub's three values).
void GolfCamera_ProcessPostShotCamera(View* pView, int nPlayer) {
    f32 m[4][4];
    f32 vDelta[4];
    f32* pCam = fn_8001731C(pView);
    f32* pSub = fn_80017314(pView);
    CamShot* pShot = NULL;
    f32 v[4] = {0.0f, 0.0f, 1.0f, 0.0f};
    f32 fDist;
    if (pView->n194 != 1) {
        GolfCamera_InitPostShotCamera(pView, nPlayer);
    }
    if (pView->b153 && fn_8004562C(pView->p130)) {
        fn_800C73DC(pCam, gPlayers[nPlayer].ball.vPos, vDelta);
        vDelta[1] = 0.0f;
        fDist = (f32)fn_80009680(fn_80009744(vDelta)) / 30.0f;
        fDist = (fDist < 0.5f) ? 0.5f : ((fDist > 5.0f) ? 5.0f : fDist);
        fDist = 0.01f * (fDist - pView->p130->f74);     // one local for the target and the step
        pView->p130->f74 += fDist;
    } else if (pView->n148 == 10) {
        if (pView->p130->v20[1] - fn_8004D620(fn_8000C594(), gPlayers[nPlayer].vBall) < 1.0f) {
            pView->p130->v20[1] += 0.01f;
        }
    }
    if ((fn_80062C1C(gPlayers[nPlayer].pChar) || fn_80062C10(gPlayers[nPlayer].pChar))
        && fn_80095780(gPlayers[nPlayer].pChar) == 9
        && (pView->p130 == NULL || (pView->p130->bAA && pView->p130->bAD && nPlayer != fn_800636EC()))) {
        if (GM_ShowPostShotCrowdFlyby()) {
            pShot = fn_8006509C(9);
        }
        if (pShot != NULL) {
            pView->p130 = pShot;
            pView->p134 = pShot->p40;
            pView->f110 = pView->p130->f48;
            pView->n140 = pView->p130->bAB;
            pView->fCamTime = 0.0f;
            pView->f128 = 0.0f;
            pView->f124 = 0.0f;
        } else {
            GolfCamera_CutToGolferDoneAnimatingCam(pView, nPlayer);
            pView->n198 = 1;
        }
    }
    if (pView->p130 != NULL && pView->p130->bAD == 0) {
        fn_8003EA50(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
        if (pView->p130 == NULL) {
            pView->fCamTime = 0.0f;
            pView->b153 = 0;
            fn_8000A194(m, pSub[1], pSub[0], pSub[2]);
            fn_800BADB4(m, v, pSub);
            fn_800C73B8(pSub, pCam, pSub);
            CameraScript_RecordCurrentCam(&pView->shot19C, pCam, pSub, nPlayer, &pView->script, 0);
            pView->p130 = &pView->shot19C;
            pView->p130->bAD = 5;
            pView->p134 = NULL;
            pView->fCamTime = 0.0f;
            pView->n164 = 0x19;
            pView->b153 = 1;
            pView->p130->f94 = 0.0f;
            pView->p130->f98 = 0.0f;
            pView->p130->bAA = 0;
        }
    } else {
        fn_8003DCE8(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
    }
}

// Camera 16, the in-the-hole camera.
void GolfCamera_InitInHoleCamera(View* pView, int nPlayer) {
    fn_8001731C(pView);
    fn_80017314(pView);
    GOLFERSTATE_GetCurrentState(nPlayer);  // the result is unused, as in the original
    pView->p78 = pView->p74;
    pView->n194 = 0;        // 0 and then 1, as in the original
    pView->n194 = 1;
    fn_80063CF0(pView, 8, nPlayer);
    if (gPlayers[nPlayer].bPlanReady == 1) {
        if (fn_800C7160(pView)) {
            fn_800C6110(pView, nPlayer, 1);
            pView->n198 = 0;
        } else if (pView->fCamTime > 2.0f) {
            GolfCamera_CutToGolferDoneAnimatingCam(pView, nPlayer);
        } else {
            pView->n194 = 0;
        }
    } else {
        // The state is compared as a signed byte (extsb), as if GOLFERSTATE_GetCurrentState returned s8.
        if (fn_800C7160(pView) || (s8)GOLFERSTATE_GetCurrentState(nPlayer) == GS_REMOVE_BALL) {
            fn_800C6110(pView, nPlayer, 1);
        }
        pView->n198 = 0;
    }
}

// Camera 16: once the golfer's animation 9 or 12 is under way, cut to the golfer (once).
void GolfCamera_ProcessInHoleCamera(View* pView, int nPlayer) {
    f32* pCam;
    f32* pSub;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    if (pView->n194 != 1) {
        GolfCamera_InitInHoleCamera(pView, nPlayer);
    }
    if ((fn_80062C1C(gPlayers[nPlayer].pChar) || fn_80062C10(gPlayers[nPlayer].pChar))
        && (fn_80095780(gPlayers[nPlayer].pChar) == 9
            || fn_80095780(gPlayers[nPlayer].pChar) == 12)
        && (pView->p130 == NULL || pView->p130->bAA) && pView->n198 < 1) {
        GolfCamera_CutToGolferDoneAnimatingCam(pView, nPlayer);
        pView->n198 = 1;
    }
    fn_8003DCE8(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
}

// Camera 17: move to a fixed spot unless the camera is already moving there.
void fn_800C3478(View* pView, int nPlayer) {
    f32 v[4] = {0.0f, 0.0f, 0.0f, 0.5f};
    if (fn_80063C7C(pView) || fn_80063C90(pView)) {
        fn_80063B98(pView, lbl_80281F78->f170, v);
    }
}

// Camera 17: the script steps on one fixed frame (FRAME_TIME) at a time.
void fn_800C34F8(View* pView, int nPlayer) {
    fn_8003F2E0(&pView->script, FRAME_TIME);
    pView->f114 += FRAME_TIME;
}

// Camera 18, the tutorial wait: the flagstick back in, and two hand-made "TUTORIAL WAIT" shots in the
// shared state, the current one (f60 15, its p40 pointing back at itself) and the next (f60 -15),
// with 40 s on the current one.
void GolfCamera_InitTutorialWaitCamera(View* pView, int nPlayer) {
    f32 v[4] = {0.0f, 0.0f, 0.0f, 0.5f};
    char szName[] = "TUTORIAL WAIT";
    if (fn_80063C7C(pView)) {
        fn_80063B98(pView, lbl_80281F78->f170, v);
    }
    fn_80016CFC(gPlayers[nPlayer].nView[0])->bFlagOut = 0;
    strcpy(lbl_80282220->shot6C.szName, szName);
    lbl_80282220->shot6C.p40 = &lbl_80282220->shot6C;
    lbl_80282220->shot6C.f60 = 15.0f;
    lbl_80282220->shot6C.f64 = 0.0f;
    lbl_80282220->shot6C.f70 = 0.0f;
    lbl_80282220->shot6C.f74 = 2.0f;
    lbl_80282220->shot6C.f68 = 2.0f;
    lbl_80282220->shot6C.f6C = 20.0f;
    lbl_80282220->shot6C.f78 = 40.0f * PI / 180.0f;    // not DEG(40.0f): that rounds one bit lower
    lbl_80282220->shot6C.f7C = lbl_80282220->shot6C.f78;
    lbl_80282220->shot6C.f80 = 0.0f;
    lbl_80282220->shot6C.f9C = 0.0f;
    lbl_80282220->shot6C.f84 = 0.0f;
    lbl_80282220->shot6C.bA8 = 1;
    lbl_80282220->shot6C.bAA = 0;
    lbl_80282220->shot6C.bAC = 10;
    lbl_80282220->shot6C.bB1 = 2;
    lbl_80282220->shot6C.bB2 = 0;
    lbl_80282220->shot6C.bAF = 10;
    lbl_80282220->shot6C.bB0 = 1;
    lbl_80282220->shot6C.bAD = 5;
    lbl_80282220->shot6C.p44 = NULL;
    strcpy(lbl_80282220->shot12C.szName, szName);
    lbl_80282220->shot12C.p40 = NULL;
    lbl_80282220->shot12C.f60 = -15.0f;
    lbl_80282220->shot12C.f64 = 0.0f;
    lbl_80282220->shot12C.f70 = 0.0f;
    lbl_80282220->shot12C.f74 = 2.0f;
    lbl_80282220->shot12C.f68 = 2.0f;
    lbl_80282220->shot12C.f6C = 20.0f;
    lbl_80282220->shot12C.f78 = 40.0f * PI / 180.0f;
    lbl_80282220->shot12C.f7C = lbl_80282220->shot12C.f78;
    lbl_80282220->shot12C.f80 = 0.0f;
    lbl_80282220->shot12C.f9C = 0.0f;
    lbl_80282220->shot12C.f84 = 0.0f;
    lbl_80282220->shot12C.bA8 = 1;
    lbl_80282220->shot12C.bAA = 0;
    lbl_80282220->shot12C.bAC = 10;
    lbl_80282220->shot12C.bB1 = 2;
    lbl_80282220->shot12C.bB2 = 0;
    lbl_80282220->shot12C.bAF = 10;
    lbl_80282220->shot12C.bB0 = 1;
    lbl_80282220->shot12C.bAD = 5;
    lbl_80282220->shot12C.p44 = NULL;
    pView->p130 = &lbl_80282220->shot6C;
    pView->p134 = &lbl_80282220->shot12C;
    pView->n140 = 7;
    pView->f110 = 40.0f;
    pView->n154 = 1;
    pView->fCamTime = 0.00001f;
}

// Camera 18 (the tutorial wait): with no shot to go to, fall back on the state's own two shots.
void fn_800C37FC(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    fn_8003DCE8(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
    if (pView->p134 == NULL) {
        pView->p130 = &lbl_80282220->shot6C;
        pView->p134 = &lbl_80282220->shot12C;
        pView->n140 = 7;
        pView->f110 = 40.0f;
        pView->n154 = 1;
        pView->fCamTime = 0.00001f;
    }
}

// Camera 23: from 1.5 up over the ball, looking along -x, script 0x23.
void fn_800C38BC(View* pView, int nPlayer) {
    f32* pCam;
    f32* pSub;
    CamShot* pShot;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    pView->p130 = NULL;
    pCam[0] = gPlayers[nPlayer].vBall[0];
    pCam[2] = gPlayers[nPlayer].vBall[2];
    pCam[1] = 1.5f;
    pSub[0] = pCam[0] - 1.0f;
    pSub[1] = pCam[1];
    pSub[2] = pCam[2];
    pShot = fn_8003A7C8(0, 0x23, NULL);
    if (pShot != NULL) {
        CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f, 0x19,
                                       0.0f);
        pView->p80 = pShot;
        pView->n194 = -1;
        pView->n198 = 0x23;
    }
}

// Camera 23's process: on the create-a-player screen, a new shot for the part being edited
// when it changes (the saved shot p80 if there is one).
void fn_800C39A8(View* pView, int nPlayer) {
    f32* pCam;
    f32* pSub;
    CamShot* pShot;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    pShot = NULL;
    // The original compares n198 and n0, both ints, as floats.
    if (lbl_80281EE0->pB4 != NULL && lbl_80281EE0->pB4->b18
        && (pView->n194 != lbl_80281EE0->pB4->nC || (f32)pView->n198 != lbl_80281EE0->n0)) {
        switch (lbl_80281EE0->n0) {
        case 0:
            pShot = fn_8003A7C8(0, 0x23, pView->p80);
            if (pShot == NULL) {
                pShot = fn_8003A7C8(0, 0x23, NULL);
            }
            break;
        case 1:
            if (fn_8001EDF4(lbl_80281EE0->pB4->pChar)) {
                pShot = fn_8003A7C8(0, 0x38, pView->p80);
                if (pShot == NULL) {
                    pShot = fn_8003A7C8(0, 0x38, NULL);
                }
            } else {
                pShot = fn_8003A7C8(0, 0x24, pView->p80);
                if (pShot == NULL) {
                    pShot = fn_8003A7C8(0, 0x24, NULL);
                }
            }
            break;
        case 2:
            pShot = fn_8003A7C8(0, 0x25, pView->p80);
            if (pShot == NULL) {
                pShot = fn_8003A7C8(0, 0x25, NULL);
            }
            break;
        case 3:
            pShot = fn_8003A7C8(0, 0x2F, pView->p80);
            if (pShot == NULL) {
                pShot = fn_8003A7C8(0, 0x2F, NULL);
            }
            break;
        case 4:
            pShot = fn_8003A7C8(0, 0x3D, pView->p80);
            if (pShot == NULL) {
                pShot = fn_8003A7C8(0, 0x3D, NULL);
            }
            break;
        }
        pView->n198 = lbl_80281EE0->n0;
        if (pShot == NULL) {
            return;
        }
        CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f, 0x19,
                                       0.0f);
        pView->p80 = pShot;
        pView->n194 = lbl_80281EE0->pB4->nC;
    }
    // EA bug: with no golfer (pB4 NULL) this reads b18 through the NULL pointer.
    if (lbl_80281EE0->pB4->b18) {
        fn_8003E624(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, FRAME_TIME);
    }
}

// Switch the CrAP camera to a named shot (with an 'f' in front for some models), else to shot
// 0x2F/0x37/0x39/0x3B/0x3C by nShot; bBlend records the current camera and blends from it.
void GolfCamera_SwitchCrAPCamera(View* pView, char* szName, int nShot, u8 bBlend, u8 bForce) {
    f32* pCam;
    f32* pSub;
    CamShot* pShot;
    char c;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    if (bForce && (nShot == 0 || nShot == 2)) {
        nShot = 1;
    }
    pShot = NULL;
    if (szName != NULL) {
        pShot = fn_8003A8C4(szName);
        if (pShot == NULL && lbl_80281EE0->pB4 != NULL && lbl_80281EE0->pB4->pChar != NULL
            && lbl_80281EE0->pB4->pChar->nSlot == 1) {
            c = szName[0];
            szName[0] = 'f';
            pShot = fn_8003A8C4(szName);
            szName[0] = c;
        }
    }
    if (pShot == NULL) {
        if (nShot == 0) {
            pShot = fn_8003A7C8(0, 0x2F, NULL);
        } else if (nShot == 1) {
            pShot = fn_8003A7C8(0, 0x37, NULL);
        } else if (nShot == 2) {
            pShot = fn_8003A7C8(0, 0x39, NULL);
        } else if (nShot == 3) {
            pShot = fn_8003A7C8(0, 0x3B, NULL);
        } else if (nShot == 4) {
            pShot = fn_8003A7C8(0, 0x3C, NULL);
        } else {
            pShot = NULL;
        }
    }
    if (pShot != NULL && pShot != pView->p130 && pShot != pView->p134) {
        if (bBlend) {
            CameraScript_RecordCurrentCam(pView->p138, pCam, pSub, 0, &pView->script, 0);
            pView->p130 = pView->p138;
            pView->p134 = NULL;
            CameraScript_InterpToNewScript(&pView->script, pShot, 0, pCam, pSub, 0, pShot->f48, 1000.0f, 0x19,
                                           0.0f);
        } else {
            CameraScript_InterpToNewScript(&pView->script, pShot, 0, pCam, pSub, 5, 0.0f, 1000.0f, 0x19,
                                           0.0f);
        }
    }
    // EA bug: pB4 may be NULL (tested above for the 'f' names), but it is read here without a test.
    pView->n194 = lbl_80281EE0->pB4->nC;
    pView->n198 = lbl_80281EE0->n0;
    if (lbl_80281EE0->pB4->b18) {
        fn_8003E624(0, pCam, pSub, &pView->script, &pView->shot19C, 0, FRAME_TIME);
    }
}

// Camera 24.
void fn_800C3EB8(View* pView, int nPlayer) {
    fn_8006351C(pView, nPlayer, 10);
}

// Camera 24: ride on the golfer's bone 10, looking forward along it.
void fn_800C3EDC(View* pView, int nPlayer) {
    f32* pCam;
    f32* pSub;
    f32 (*m)[4];
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    m = fn_8001ED08(gPlayers[nPlayer].pChar, 10);
    Vec_Copy(m[3], pSub);
    fn_8000C5D4(pSub, m[1], 0.1f, pSub);
    fn_8001EF34(m[2], 0.5f, pCam);
    fn_800C73B8(m[3], pCam, pCam);
    fn_8000C5D4(pCam, m[1], 0.1f, pCam);
    pCam[3] = 1.0f;
    fn_800C7400(m[0], pView->v20);
}

// The zoom-to-aim camera's positions. pAim: out from the ball towards the target by the shot's
// f60, then across by f64, at height v30[1]. pCam: back from the target through pAim, by the
// tuning's distance (f4, or f38 on a putt) scaled for the lens, closer in when the ball is near
// the target. When pCam is out of bounds it is pulled in to the boundary, or mirrored through the
// ball (returns 1). pCam never ends up past pAim.
u8 fn_800C3FC0(View* pView, int nPlayer, f32* pSub, f32* pAim, f32* pCam) {
    f32 vTarget[4];
    f32 vDir[4];
    f32 vOff[4];
    f32 vBack[4];
    f32 v[4];
    f32 vHit[4];
    f32* pBall;
    u8 bMoved;
    f32 fDist;
    f32 fBack;
    f32 fNear;
    TNetwork* pNet;
    bMoved = 0;
    fn_8000C594();
    Vec_Copy(gPlayers[nPlayer].vTargetCopy, vTarget);
    pBall = gPlayers[nPlayer].vBall;
    fn_800C73DC(vTarget, pBall, vDir);
    vDir[1] = 0.0f;
    fDist = fn_80009680(fn_80009744(vDir));
    if (vDir[0] != 0.0f || vDir[1] != 0.0f || vDir[2] != 0.0f) {
        fn_800BAF04(vDir, vDir);
    }
    fn_8001EF34(vDir, pView->shot19C.f60, vOff);
    fn_800C73B8(pBall, vOff, pAim);
    pAim[0] += pView->shot19C.f64 * -fn_8000C5FC(vDir, lbl_801913A8);
    pAim[2] += pView->shot19C.f64 * fn_8000C5FC(vDir, lbl_80191398);
    pAim[1] = pView->shot19C.v30[1];
    fn_800C73DC(vTarget, pAim, vBack);
    vBack[1] = 0.0f;
    if (vBack[0] != 0.0f || vBack[1] != 0.0f || vBack[2] != 0.0f) {
        fn_800BAF04(vBack, vBack);
    }
    if (gPlayers[nPlayer].nShotKind != 0) {
        fBack = lbl_80281F78->f4;
    } else {
        fBack = lbl_80281F78->f38;
    }
    fBack *= 1.0f / fn_8001EFFC((u8*)fn_80008370(fn_80017004(gPlayers[nPlayer].nView[0])));
    if (fBack + lbl_80281F78->fC > fDist && gPlayers[nPlayer].nShotKind != 0) {
        if (fBack > fDist) {
            fBack = lbl_80281F78->f10 * fDist;
        } else {
            fNear = lbl_80281F78->f10 * fDist;
            fBack = ((fDist - fBack) / lbl_80281F78->fC) * (fBack - fNear) + fNear;
        }
    }
    pCam[0] = vTarget[0] - vBack[0] * fBack;
    pCam[2] = vTarget[2] - vBack[2] * fBack;
    pCam[1] = 0.0f;
    if (!fn_80069428(pCam)) {
        pNet = fn_80069498();
        if (pNet != NULL) {
            if (fn_8000C3C8(pCam, pAim, pNet, pNet->nNumNodes, vHit)) {
                bMoved = 1;
                pCam[0] = vHit[0];
                pCam[2] = vHit[2];
            } else if (fn_8004B6F8(pCam, pAim, vHit)) {
                bMoved = 1;
                pCam[0] = vHit[0];
                pCam[2] = vHit[2];
            } else {
                bMoved = 1;
                pCam[0] = gPlayers[nPlayer].vBall[0] + (gPlayers[nPlayer].vBall[0] - pAim[0]);
                pCam[2] = gPlayers[nPlayer].vBall[2] + (gPlayers[nPlayer].vBall[2] - pAim[2]);
            }
        } else if (fn_8004B6F8(pCam, pAim, vHit)) {
            bMoved = 1;
            pCam[0] = vHit[0];
            pCam[2] = vHit[2];
        } else {
            bMoved = 1;
            pCam[0] = gPlayers[nPlayer].vBall[0] + (gPlayers[nPlayer].vBall[0] - pAim[0]);
            pCam[2] = gPlayers[nPlayer].vBall[2] + (gPlayers[nPlayer].vBall[2] - pAim[2]);
        }
    }
    fn_800C73DC(pCam, pSub, vBack);
    vBack[1] = 0.0f;
    fn_800C73DC(vTarget, pAim, vDir);
    fn_800C73DC(pCam, pAim, v);
    v[1] = 0.0f;
    vDir[1] = 0.0f;
    if (fn_8000C5FC(vDir, v) < 0.0f) {
        pCam[0] = pAim[0];
        pCam[2] = pAim[2];
        fn_800C73DC(pCam, pSub, vBack);
        vBack[1] = 0.0f;
    }
    return bMoved;
}

u8 fn_800C43C0(View* pView, int nPlayer) {
    fn_8001731C(pView);
    if (pView->n260 == 1) {
        return 1;
    }
    if (pView->n260 == 6) {
        return 1;
    }
    return pView->n260 == 10;
}

u8 fn_800C441C(View* pView, int nPlayer) {
    fn_8001731C(pView);
    if (pView->n260 == 2) {
        return 1;
    }
    if (pView->n260 == 5) {
        return 1;
    }
    if (pView->n260 == 8) {
        return 1;
    }
    if (pView->n260 == 11) {
        return 1;
    }
    if (pView->n260 == 15) {
        return 1;
    }
    return pView->n260 == 16;
}

u8 fn_800C44A8(View* pView, int nPlayer) {
    if (pView->n260 == 4) {
        return 1;
    }
    return pView->n260 == 9;
}

u8 fn_800C44CC(View* pView, int nPlayer) {
    return pView->n260 == 7;
}

u8 fn_800C44E0(View* pView, int nPlayer) {
    return pView->n260 == 3;
}

u8 fn_800C44F4(View* pView, int nPlayer) {
    if (pView->n260 == 13) {
        return 1;
    }
    return pView->n260 == 14;
}

int fn_800C4518(View* pView) {
    return pView->n194;
}

// Is the target too steep from the ball: |dy / dx| at least fUp going up, fDown going down.
u8 fn_800C4520(View* pView, int nPlayer, f32 fUp, f32 fDown) {
    f32 dx = gPlayers[nPlayer].vTarget[0] - gPlayers[nPlayer].ball.vPos[0];
    f32 dy = gPlayers[nPlayer].vTarget[1] - gPlayers[nPlayer].ball.vPos[1];
    f32 fSlope;
    if (fabs(dx) < 1e-6f) {
        return 0;
    }
    fSlope = fabs(dy / dx);
    if (dy > 0.0f) {
        if (fSlope < fUp) {
            return 0;
        }
    } else {
        if (fSlope < fDown) {
            return 0;
        }
    }
    return 1;
}

// Is anything in the way between the view and the player's target?
u8 fn_800C4604(View* pView, int nPlayer) {
    f32 vHit[4];
    f32 vNormal[4];
    SurfaceType* pSurface;
    TerObject* pObj;
    return Ter_CheckForGroundCollision(gPlayers[nPlayer].ball.pCourse, pView->v0, gPlayers[nPlayer].vTarget,
                                       vHit, vNormal, &pSurface, &pObj);
}

// Should the view be moved: never on a putt; else by the tuning's slope and terrain tests, and when
// fn_800635D0 fails.
u8 fn_800C4650(View* pView, int nPlayer) {
    u8 bMove;
    if (gPlayers[nPlayer].nClub == CLUB_PUTTER_e) {
        return 0;
    }
    bMove = 0;
    if (lbl_80281F78->bCheckSlope) {
        bMove = fn_800C4520(pView, nPlayer, lbl_80281F78->fSlopeUp, lbl_80281F78->fSlopeDown);
        if (!bMove) {
            return 0;
        }
    }
    if (!bMove && lbl_80281F78->bCheckTerrain) {
        bMove = fn_800C4604(pView, nPlayer);
    }
    if (!bMove) {
        bMove = fn_800635D0(nPlayer) == 0;
    }
    return bMove;
}

// The steep-slope camera: from a base point by the ball, back away from the target (and down when
// looking up, up when looking down) until the ground no longer hides the target, up to n1DC tries
// (at least 2; fewer when the target has not moved). The view moves there, looking at the target,
// no faster than the tuning allows, and not steeper than fn_800C4AB0 allows.
void GolfCamera_ComputeSteepSlopeCamVectors(View* pView, int nPlayer) {
    f32 vTarget[4];
    f32 vOldCam[4];
    f32 vOldSub[4];
    f32 vPrevCam[4];
    f32 vPrevSub[4];
    f32 vCam[4];
    f32 vBase[4];
    f32 vDelta[4];
    f32 vDir[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    f32 vHit[4];
    f32 vNormal[4];
    SurfaceType* pSurface;
    TerObject* pObj;
    int nMax;
    int nTries;
    u8 bUp;
    Ball* pBall;
    int i;
    u8 bDone;
    u8 bClear;
    f32* pTarget;
    u8 bHit;
    f32 fBack;
    pTarget = gPlayers[nPlayer].vTarget;
    Vec_Copy(pTarget, vTarget);
    pBall = &gPlayers[nPlayer].ball;
    if (lbl_80281520 == -1) {
        fn_800B5918(pTarget, lbl_801FA1E8);
        nMax = lbl_80281F78->n1DC;
    } else if (fn_800BB028(lbl_801FA1E8, pTarget) > 0.1f) {
        fn_800B5918(pTarget, lbl_801FA1E8);
        nMax = (lbl_80281520 + 1 <= lbl_80281F78->n1DC) ? lbl_80281520 + 1 : lbl_80281F78->n1DC;
    } else {
        nMax = lbl_80281520;
    }
    nTries = (nMax > 2) ? nMax : 2;
    vBase[0] = lbl_80281F78->f1E8 + gPlayers[nPlayer].ball.vPos[0];
    vBase[1] = lbl_80281F78->f1EC + gPlayers[nPlayer].ball.vPos[1];
    vBase[2] = gPlayers[nPlayer].ball.vPos[2];
    vBase[3] = 0.0f;
    fn_800C73DC(vTarget, vBase, vDelta);
    if (vDelta[0] != 0.0f || vDelta[1] != 0.0f || vDelta[2] != 0.0f) {
        fn_800BAF04(vDelta, vDir);
    } else {
        vDir[0] = 0.0f;
        vDir[1] = 0.0f;
        vDir[2] = 0.0f;
    }
    if (vDelta[1] > 0.0f) {
        bUp = 1;
    } else {
        bUp = 0;
    }
    fBack = lbl_80281F78->f1D8;
    fn_8000C5D4(vBase, vDir, -fBack, vCam);
    fn_800B5918(pView->v0, vOldCam);
    fn_800B5918(pView->v10, vOldSub);
    i = 0;
    bDone = 0;
    bClear = 0;
    while (!bDone && i < nTries) {
        vCam[1] = (pBall->vPos[1] + 0.1f <= vCam[1]) ? vCam[1] : pBall->vPos[1] + 0.1f;
        fn_800B5918(pView->v0, vPrevCam);
        fn_800B5918(pView->v10, vPrevSub);
        fn_800B5918(vCam, pView->v0);
        fn_800B5918(vTarget, pView->v10);
        fn_800352BC();
        fn_80013CCC(fn_8001614C());
        bHit = Ter_CheckForGroundCollision(pBall->pCourse, vCam, vTarget, vHit, vNormal, &pSurface, &pObj);
        fn_800636B4(nPlayer);
        if (bHit || bClear) {
            fBack += lbl_80281F78->f1D4;
            fn_8000C5D4(vBase, vDir, -fBack, vCam);
            if (bUp) {
                vCam[1] -= lbl_80281F78->f1D0;
            } else {
                vCam[1] += lbl_80281F78->f1D0;
            }
        } else {
            bClear = 1;
        }
        if (bClear && i >= lbl_80281520 - 2) {
            bDone = 1;
        }
        i++;
    }
    lbl_80281520 = i;
    fn_800C4D2C(vOldCam, pView->v0, pView->v0, lbl_80281F78->f1F0);
    fn_800C4D2C(vOldSub, pView->v10, pView->v10, lbl_80281F78->f1F4);
    fn_800C4AB0(pView->v0, pView->v10, pView->v10);
}

// The point pTo as seen from pFrom, but with the direction turned back towards the horizontal when
// it is steeper than the tuning's fMaxPitchUp (going up) or fMaxPitchDown (going down); into pOut.
// Nothing is written when the direction is within the limits.
void fn_800C4AB0(f32* pFrom, f32* pTo, f32* pOut) {
    f32 vQuat[4];
    f32 vAxis[4];
    f32 vFlat[4];
    f32 vFlatDir[4];
    f32 v[4];
    f32 vDir[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    f32 vOut[4];
    f32 fOver;
    f32 fMaxUp = PI * lbl_80281F78->fMaxPitchUp / 180.0f;
    f32 fMaxDown = PI * lbl_80281F78->fMaxPitchDown / 180.0f;
    f32 fAngle;
    u8 bClamp;
    fn_800C73DC(pTo, pFrom, v);
    vFlat[0] = v[0];
    vFlat[1] = 0.0f;
    vFlat[2] = v[2];
    vFlat[3] = 0.0f;
    if (v[0] != 0.0f || v[1] != 0.0f || v[2] != 0.0f) {
        fn_800BAF04(v, vDir);
    } else {
        vDir[0] = 0.0f;
        vDir[1] = 0.0f;
        vDir[2] = 0.0f;
    }
    if (vFlat[0] != 0.0f || vFlat[1] != 0.0f || vFlat[2] != 0.0f) {
        fn_800BAF04(vFlat, vFlatDir);
    } else {
        vFlatDir[0] = 0.0f;
        vFlatDir[1] = 0.0f;
        vFlatDir[2] = 0.0f;
    }
    fAngle = fn_80009614((fn_8000C5FC(vDir, vFlatDir) < -1.0f) ? -1.0f
                         : ((fn_8000C5FC(vDir, vFlatDir) > 1.0f) ? 1.0f : fn_8000C5FC(vDir, vFlatDir)));
    bClamp = 0;
    if (v[1] > 0.0f && fAngle > fMaxUp) {
        fOver = fAngle - fMaxUp;
        bClamp = 1;
    } else if (v[1] < 0.0f && fAngle > fMaxDown) {
        bClamp = 1;
        fOver = fAngle - fMaxDown;
    }
    if (bClamp) {
        vec4flt_CrossProduct(vDir, vFlatDir, vAxis);
        if (vAxis[0] != 0.0f || vAxis[1] != 0.0f || vAxis[2] != 0.0f) {
            fn_800BAF04(vAxis, vAxis);
        }
        fn_8000AE28(vAxis, fOver, vAxis);
        fn_8000923C(vAxis, vQuat);
        v[3] = 0.0f;
        fn_800090E4(vQuat, v, vOut);
        fn_800C73B8(pFrom, vOut, pOut);
    }
}

// Move pOut from pFrom towards pTo by at most fMax; nonzero if it had to stop short.
int fn_800C4D2C(f32* pFrom, f32* pTo, f32* pOut, f32 fMax) {
    int bClamped = 0;
    f32 v[4];
    f32 vStep[4];
    f32 fDist;
    fn_800C73DC(pTo, pFrom, v);
    fDist = fn_80009680(fn_80009744(v));
    if (fDist > fMax && fDist > 1e-6f) {
        fn_8001EF34(v, fMax / fDist, vStep);
        fn_800C73B8(pFrom, vStep, pOut);
        bClamped = 1;
    } else {
        Vec3Copy(pTo, pOut);
    }
    return bClamped;
}

// The first of shots 0x1C + n .. 0x21 the player's camera plan has (none in modes 6, 7 and 8).
CamShot* fn_800C4DF8(int nFirst, int nPlayer) {
    int i;
    CamShot* pShot;
    if (Game_GetMode() != 6 && Game_GetMode() != 7 && Game_GetMode() != 8) {
        for (i = nFirst; i <= 5; i++) {
            pShot = fn_8003A7C8(nPlayer, i + 0x1C, NULL);
            if (pShot != NULL) {
                return pShot;
            }
        }
    }
    return NULL;
}

// Cycle through shots 1..5 (fn_800C4DF8, skipping 3) when there is no next shot or it is of kind
// 6, 8, 9 or 10; with none, go back to the saved shot p80.
void fn_800C4E80(View* pView, int nPlayer) {
    f32* pCam;
    f32* pSub;
    CamShot* pShot;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    // fake match: the constant first in one compare keeps the 8/9 tests from becoming a range test
    if (pView->p134 == NULL || 6 == pView->n140 || pView->n140 == 9 || pView->n140 == 8
        || pView->n140 == 10) {
        pView->n264++;
        if (pView->n264 > 5) {
            pView->n264 = 1;
        }
        pShot = fn_800C4DF8(pView->n264, nPlayer);
        if (pShot != NULL && pView->n264 != 3) {
            CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, pShot->bAB, pShot->f48,
                                           1000.0f, 0x19, 0.0f);
        } else if (pView->p80 != NULL && pView->p80 != pView->p130) {
            if (pView->p130 != NULL) {
                CameraScript_InterpToNewScript(&pView->script, pView->p80, nPlayer, pCam, pSub,
                                               pView->p130->bAB, pView->p130->f48, 1000.0f, 0x19, 0.0f);
            } else {
                CameraScript_InterpToNewScript(&pView->script, pView->p80, nPlayer, pCam, pSub, 5, 0.0f,
                                               1000.0f, 0x19, 0.0f);
            }
        }
    }
}

// The matrix camera: two hand-made "MATRIX CAM" shots in the shared state, both looking at pTo, one
// from pFrom and one from the far side of pTo at pFrom's height (shot 0x22 of the player's camera
// plan instead, if it has one). The lens zooms to the view's field of view less the letterbox's
// change. Swing camera kind 6 starts on the far side; kinds other than 1 and 6 hold the first shot
// twice as long, with blend mode 7.
void fn_800C4FF0(View* pView, f32* pFrom, f32* pTo, int nPlayer) {
    f32 v[4];
    f32 vFrom[4];
    char szName[] = "MATRIX CAM";
    f32 fChange;
    CamShot* pShot;
    int nOrder;
    Vec_Copy(pFrom, vFrom);
    fn_800C73DC(pTo, vFrom, v);
    v[1] = 0.0f;
    fn_800C73B8(pTo, v, v);
    v[1] = vFrom[1];
    fn_80005AE8(&lbl_80282220->shot6C, 0, sizeof(CamShot) * 2);
    strcpy(lbl_80282220->shot6C.szName, szName);
    Vec3Copy(vFrom, lbl_80282220->shot6C.v20);
    Vec3Copy(pTo, lbl_80282220->shot6C.v30);
    lbl_80282220->shot6C.p40 = NULL;
    lbl_80282220->shot6C.f60 = 0.0f;
    lbl_80282220->shot6C.f64 = 0.0f;
    lbl_80282220->shot6C.f70 = 0.0f;
    lbl_80282220->shot6C.f74 = 0.0f;
    lbl_80282220->shot6C.f68 = 0.2f;
    lbl_80282220->shot6C.f6C = 20.0f;
    fChange = fn_800DC3A4();
    lbl_80282220->shot6C.f78 =
        fn_80014278(fn_80008370(fn_80016CFC(gPlayers[nPlayer].nView[0])->pCamera)) - fChange;
    lbl_80282220->shot6C.f7C = lbl_80282220->shot6C.f78;
    lbl_80282220->shot6C.f80 = 0.0f;
    lbl_80282220->shot6C.f4C = 1.0f;
    lbl_80282220->shot6C.f9C = 0.0f;
    lbl_80282220->shot6C.f84 = 0.0f;
    lbl_80282220->shot6C.bA8 = 0;
    lbl_80282220->shot6C.bAA = 1;
    lbl_80282220->shot6C.bAC = 0x18;
    lbl_80282220->shot6C.bB1 = 9;
    lbl_80282220->shot6C.bB2 = 0;
    lbl_80282220->shot6C.bAD = 3;
    lbl_80282220->shot6C.p44 = pView->p130;
    lbl_80282220->shot6C.f8C = 0.0f;
    pShot = fn_8003A7C8(nPlayer, 0x22, NULL);
    if (pShot != NULL) {
        Mem_cpy(&lbl_80282220->shot12C, pShot, sizeof(CamShot));
        lbl_80282220->shot12C.p40 = NULL;
        lbl_80282220->shot12C.p44 = pView->p130;
        lbl_80282220->shot12C.bAD = 3;
        lbl_80282220->shot12C.f8C = 0.0f;
        lbl_80282220->shot12C.f4C = 1.0f;
    } else {
        strcpy(lbl_80282220->shot12C.szName, szName);
        Vec3Copy(v, lbl_80282220->shot12C.v20);
        Vec3Copy(pTo, lbl_80282220->shot12C.v30);
        lbl_80282220->shot12C.p40 = NULL;
        lbl_80282220->shot12C.f60 = 0.0f;
        lbl_80282220->shot12C.f64 = 0.0f;
        lbl_80282220->shot12C.f70 = 0.0f;
        lbl_80282220->shot12C.f74 = 0.0f;
        lbl_80282220->shot12C.f68 = 0.2f;
        lbl_80282220->shot12C.f6C = 20.0f;
        lbl_80282220->shot12C.f78 = lbl_80282220->shot6C.f78;
        lbl_80282220->shot12C.f7C = lbl_80282220->shot12C.f78;
        lbl_80282220->shot12C.f80 = 0.0f;
        lbl_80282220->shot12C.f4C = 1.0f;
        lbl_80282220->shot12C.f9C = 0.0f;
        lbl_80282220->shot12C.f84 = 0.0f;
        lbl_80282220->shot12C.bA8 = 0;
        lbl_80282220->shot12C.bAA = 1;
        lbl_80282220->shot12C.bAC = 0x18;
        lbl_80282220->shot12C.bB1 = 9;
        lbl_80282220->shot12C.bB2 = 0;
        lbl_80282220->shot12C.bAD = 3;
        lbl_80282220->shot12C.p44 = pView->p130;
        lbl_80282220->shot12C.f8C = 0.0f;
    }
    switch (pView->n260) {
    case 1:
        nOrder = 0;
        break;
    case 6:
        nOrder = 1;
        break;
    case 10:
    default:
        nOrder = 2;
        break;
    }
    if (nOrder == 0) {
        pView->p130 = &lbl_80282220->shot6C;
        pView->p134 = &lbl_80282220->shot12C;
        pView->f110 = lbl_80281F78->f5C;
        pView->n140 = 2;
        if (fn_800453C8(nPlayer, NULL)) {
            pView->n154 = 2;
        } else {
            pView->n154 = 1;
        }
    } else if (nOrder == 1) {
        pView->p130 = &lbl_80282220->shot12C;
        pView->p134 = &lbl_80282220->shot6C;
        pView->f110 = lbl_80281F78->f5C;
        pView->n140 = 2;
        if (fn_800453C8(nPlayer, NULL)) {
            pView->n154 = 1;
        } else {
            pView->n154 = 1;
        }
    } else {
        pView->p130 = &lbl_80282220->shot6C;
        pView->p134 = &lbl_80282220->shot12C;
        pView->n140 = 7;
        pView->f110 = 2.0f * lbl_80281F78->f5C;
        if (fn_800453C8(nPlayer, NULL)) {
            pView->n154 = 2;
        } else {
            pView->n154 = 1;
        }
    }
    pView->fCamTime = 0.00001f;
    if (pView->p74 != NULL) {
        pView->p134->f4C = pView->p74->f38;
    } else {
        pView->p134->f4C = 0.5f;
    }
    pView->p134->f4C = 1.0f;    // f4C is set just above; the original overwrites it
}

// Camera 13's tick: with no next shot, clear b54 (a freeze-time flag: fn_800C6D9C tests it) and
// replay the current shot's p44 as a hand-made shot. Returns the frame time to run the script at (0
// while held).
f32 fn_800C54FC(View* pView, f32* pCam, f32* pSub, int nPlayer) {
    f32 fTime = 0.0f;
    CamShot* pShot;
    if (gSession.nPaused == 0) {
        fn_80038054(1, fn_80016D10(), 0.0f, lbl_80281F78->f64);
    }
    if (pView->p134 == NULL) {
        lbl_80282220->b54 = 0;
        fn_8007326C(gPlayers[nPlayer].pChar->anim);
        EVENT_Trigger(nPlayer, 0x33, NULL, -1);
        EVENT_Trigger(nPlayer, 0x3B, NULL, 1);
        pShot = pView->p130->p44;
        if (pShot != NULL) {
            Mem_cpy(&pView->shot19C, pShot, sizeof(CamShot));
            pView->shot19C.p44 = pShot;
            pView->p134 = &pView->shot19C;
            pView->fCamTime = 0.0f;
            pView->f110 = lbl_80281F78->f58;
            pView->n140 = 5;
            if (pView->p74 != NULL) {
                pView->p134->f4C = pView->p74->f38 - lbl_80281F78->f58;
            } else {
                pView->p134->f4C = lbl_80281F78->f58;
            }
            pView->p134->p44 = pShot;
        }
    }
    if (gSession.nPaused == 0) {
        if (lbl_80281F78->f60) {
            if (pView->f18C > lbl_80281F78->f60) {
                fTime = lbl_80281F78->f60;
                pView->f18C = 0.0f;
            } else {
                fTime = 0.0f;
                pView->f18C += FRAME_TIME;
            }
        } else {
            fTime = FRAME_TIME;
        }
    }
    return fTime;
}

// The super zoom: two hand-made "SUPER ZOOM" shots in the shared state, both looking at pTo; from
// pFrom for kind 13, else from the far side of pTo at pFrom's height. The second zooms the lens in
// to the view's field of view less the letterbox's change (stored in the tuning's f80). The first
// goes back to the current shot after the tuning's f84 seconds.
void fn_800C56B4(View* pView, f32* pFrom, f32* pTo, int nPlayer) {
    f32 v[4];
    f32 vFrom[4];
    char szName[] = "SUPER ZOOM";
    f32 fChange;
    CamTuning* pTune;
    Vec_Copy(pFrom, vFrom);
    fn_800C73DC(pTo, vFrom, v);
    v[1] = 0.0f;
    fn_800C73B8(pTo, v, v);
    v[1] = vFrom[1];
    strcpy(lbl_80282220->shot6C.szName, szName);
    if (pView->n260 == 13) {
        Vec3Copy(pFrom, lbl_80282220->shot6C.v20);
    } else {
        Vec3Copy(v, lbl_80282220->shot6C.v20);
    }
    Vec3Copy(pTo, lbl_80282220->shot6C.v30);
    lbl_80282220->shot6C.p40 = NULL;
    lbl_80282220->shot6C.f60 = 0.0f;
    lbl_80282220->shot6C.f64 = 0.0f;
    lbl_80282220->shot6C.f70 = 0.0f;
    lbl_80282220->shot6C.f74 = 0.0f;
    lbl_80282220->shot6C.f68 = 0.2f;
    lbl_80282220->shot6C.f6C = 20.0f;
    lbl_80282220->shot6C.f78 = lbl_80281F78->f7C;
    lbl_80282220->shot6C.f7C = lbl_80282220->shot6C.f78;
    lbl_80282220->shot6C.f80 = 0.0f;
    lbl_80282220->shot6C.f4C = 0.1f;
    lbl_80282220->shot6C.f9C = -2.0f * PI;
    lbl_80282220->shot6C.f84 = 0.0f;
    lbl_80282220->shot6C.bA8 = 0;
    lbl_80282220->shot6C.bAA = 1;
    lbl_80282220->shot6C.bAC = 0x18;
    lbl_80282220->shot6C.bB1 = 9;
    lbl_80282220->shot6C.bB2 = 0;
    lbl_80282220->shot6C.bAD = 3;
    lbl_80282220->shot6C.p44 = pView->p130;
    strcpy(lbl_80282220->shot12C.szName, szName);
    Vec3Copy(lbl_80282220->shot6C.v20, lbl_80282220->shot12C.v20);
    Vec3Copy(pTo, lbl_80282220->shot12C.v30);
    lbl_80282220->shot12C.p40 = NULL;
    lbl_80282220->shot12C.f60 = 0.0f;
    lbl_80282220->shot12C.f64 = 0.0f;
    lbl_80282220->shot12C.f70 = 0.0f;
    lbl_80282220->shot12C.f74 = 0.0f;
    lbl_80282220->shot12C.f68 = 0.2f;
    lbl_80282220->shot12C.f6C = 20.0f;
    fChange = fn_800DC3A4();
    lbl_80281F78->f80 = fn_80014278(fn_80008370(fn_80016CFC(gPlayers[nPlayer].nView[0])->pCamera)) - fChange;
    lbl_80282220->shot12C.f78 = lbl_80281F78->f80;
    lbl_80282220->shot12C.f7C = lbl_80282220->shot12C.f78;
    lbl_80282220->shot12C.f80 = 0.0f;
    lbl_80282220->shot12C.f4C = 0.1f;
    lbl_80282220->shot12C.f9C = 0.0f;
    lbl_80282220->shot12C.f84 = 0.0f;
    lbl_80282220->shot12C.bA8 = 0;
    lbl_80282220->shot12C.bAA = 1;
    lbl_80282220->shot12C.bAC = 0x18;
    lbl_80282220->shot12C.bB1 = 9;
    lbl_80282220->shot12C.bB2 = 0;
    lbl_80282220->shot12C.bAD = 3;
    lbl_80282220->shot12C.p44 = pView->p130;
    pTune = lbl_80281F78;
    fn_80038054(1, fn_80016D10(), pTune->f8C, pTune->f88);
    pView->p130 = &lbl_80282220->shot6C;
    pView->p134 = &lbl_80282220->shot12C;
    pView->f110 = lbl_80281F78->f84;
    pView->n140 = 1;
    pView->fCamTime = 0.0f;
    if (pView->p74 != NULL) {
        pView->p134->f4C = pView->p74->f38;
    } else {
        pView->p134->f4C = 0.5f;
    }
    pView->p134->bAD = 4;
    pView->p134->f4C = 1.0f;    // f4C is set just above; the original overwrites it
}

// Camera 13's tick for kinds 15 and 16: after 0.05 s with no next shot, clear b58 (a freeze-time
// flag: fn_800C6D9C tests it) and replay the current shot's p44; while a shot runs, ease the fallback
// shots' f78/f7C from f7C to f80.
f32 fn_800C5A70(View* pView, f32* pCam, f32* pSub, int nPlayer) {
    f32 fTime = 0.0f;
    CamShot* pShot;
    CamTuning* pTune;
    f32 t;
    f32 f;
    if (gSession.nPaused == 0) {
        pTune = lbl_80281F78;
        fn_80038054(1, fn_80016D10(), pTune->f8C, pTune->f88);
    }
    if (pView->p134 == NULL && pView->fCamTime > 0.05f) {
        lbl_80282220->b58 = 0;
        fn_8007326C(gPlayers[nPlayer].pChar->anim);
        EVENT_Trigger(nPlayer, 0x3A, NULL, -1);
        EVENT_Trigger(nPlayer, 0x3B, NULL, 1);
        if (pView->p130 != NULL) {
            lbl_80282220->shot6C.f78 = lbl_80281F78->f80;
            lbl_80282220->shot6C.f7C = lbl_80281F78->f80;
            lbl_80282220->shot12C.f78 = lbl_80281F78->f80;
            lbl_80282220->shot12C.f7C = lbl_80281F78->f80;
        }
        // EA bug: p130 was tested for NULL just above, but is read here without a test.
        pShot = pView->p130->p44;
        if (pShot != NULL) {
            Mem_cpy(&pView->shot19C, pShot, sizeof(CamShot));
            pView->shot19C.p44 = pShot;
            pView->p134 = &pView->shot19C;
            pView->fCamTime = 0.0f;
            pView->f110 = lbl_80281F78->f58;
            pView->n140 = 5;
            if (pView->p74 != NULL) {
                pView->p134->f4C = pView->p74->f38 - lbl_80281F78->f58;
            } else {
                pView->p134->f4C = lbl_80281F78->f58;
            }
            pView->p134->p44 = pShot;
        }
    } else if (pView->p134 != NULL) {
        t = pView->fCamTime / pView->f110;
        t *= t;
        f = t * (lbl_80281F78->f80 - lbl_80281F78->f7C) + lbl_80281F78->f7C;
        lbl_80282220->shot6C.f78 = f;
        lbl_80282220->shot6C.f7C = f;
        lbl_80282220->shot12C.f78 = f;
        lbl_80282220->shot12C.f7C = f;
    } else if (pView->p130 != NULL) {
        lbl_80282220->shot6C.f78 = lbl_80281F78->f80;
        lbl_80282220->shot6C.f7C = lbl_80281F78->f80;
        lbl_80282220->shot12C.f78 = lbl_80281F78->f80;
        lbl_80282220->shot12C.f7C = lbl_80281F78->f80;
    }
    if (gSession.nPaused == 0) {
        fTime = FRAME_TIME;
    }
    return fTime;
}

void fn_800C5CEC(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    pView->n194++;
    pView->fCamTime = 0.0f;
    fn_800C5D64(pView, pCam, pSub, nPlayer);
}

// Camera 13's process: a new angle from the current shot each time (script 13), up to
// fn_800C6B38's count, then back to the saved shot. Kinds 15 and 16 use script 0x22 instead.
// The callers pass the view's position and aim, but it fetches them again.
void fn_800C5D64(View* pView, f32* pViewCam, f32* pViewSub, int nPlayer) {
    CamShot* pShot;
    f32* pCam;
    f32* pSub;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    if (pView->n260 == 15 || pView->n260 == 16) {
        pShot = fn_8003A7C8(nPlayer, 0x22, NULL);
        if (pShot != NULL) {
            CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f, 0x19,
                                           0.0f);
            pView->f110 = 1.0f;
        }
        pView->f190 = 0.0f;
        lbl_80282220->b59 = 1;
    } else {
        if (pView->n194 == 0) {
            pView->shot19C.p44 = pView->p130;
        }
        if (pView->n194 >= fn_800C6B38(pView)) {
            pView->p130 = pView->shot19C.p44;
            pView->p134 = NULL;
            pView->fCamTime = 0.0f;
        } else {
            pShot = fn_8003A7C8(nPlayer, 0xD, pView->p130);
            if (pShot != NULL
                && !CameraScript_WillGolferBeOccludedInThisView(nPlayer, pShot, &pView->script)) {
                CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f,
                                               0x19, 0.0f);
            }
        }
    }
}

// The swing's progress as 0..1 in fCamTime: start to top of the backswing (f190 0), then top to
// the end (f190 1).
void fn_800C5EC0(View* pView, f32* pCam, f32* pSub, int nPlayer) {
    f32 fEnd;
    f32 fStart;
    f32 fTop;
    f32 fTime;
    f32 t;
    fEnd = fn_8005CB78(gPlayers[nPlayer].pChar, 2);
    fStart = fn_8005CB78(gPlayers[nPlayer].pChar, 0);
    fTop = fn_8005CB78(gPlayers[nPlayer].pChar, 1);
    fTime = gPlayers[nPlayer].pChar->fAnimTime;
    if (fTime < fTop) {
        t = (fTime - fStart) / (fTop - fStart);
        pView->fCamTime = (t < 0.0f) ? 0.0f : ((t > 1.0f) ? 1.0f : t);
        pView->f190 = 0.0f;
    } else {
        pView->f190 = 1.0f;
        t = (fTime - fTop) / (fEnd - fTop);
        pView->fCamTime = (t < 0.0f) ? 0.0f : ((t > 1.0f) ? 1.0f : t);
    }
}

u8 fn_800C5FE4(View* pView, int nPlayer) {
    u8 bOn = 0;
    if (pView->n260 == 11) {
        bOn = 1;
    }
    if (bOn) {
        pView->n198 = 1;
    }
    return bOn;
}

// Step shot19C.f60 up by 0.1 a frame, but keep it 3 short of the ground distance from the ball to the
// aim point.
void fn_800C6010(View* pView, int nPlayer) {
    f32 v[4];
    fn_800C73DC(gPlayers[nPlayer].vTarget2, gPlayers[nPlayer].vBall, v);
    v[1] = 0.0f;
    if (pView->shot19C.f60 + 0.1f < (f32)fn_80009680(fn_80009744(v)) - 3.0f) {
        pView->shot19C.f60 += 0.1f;
    } else if (pView->shot19C.f60 > (f32)fn_80009680(fn_80009744(v)) - 3.0f) {
        pView->shot19C.f60 = (f32)fn_80009680(fn_80009744(v)) - 3.0f;
        if (pView->shot19C.f60 < 0.0f) {
            pView->shot19C.f60 = 0.0f;
        }
    }
}

void fn_800C60E8(View* pView, int nPlayer) {
    pView->shot19C.f60 -= 0.1f;
    if (pView->shot19C.f60 < 0.0f) {
        pView->shot19C.f60 = 0.0f;
    }
}

// Cut to the golfer: a sequence for the ball's lie, surface class and how far the shot went (shot
// 9, else the asked-for kind, else 5).
void fn_800C6110(View* pView, int nPlayer, int a) {
    f32* pCam = fn_8001731C(pView);
    f32* pSub = fn_80017314(pView);
    f32 v[4];
    CamSequence* pSeq = NULL;
    CamShot* pShot = NULL;
    int nA = 5;
    f32 f1 = 0.0f;
    f32 f2 = 100.0f;
    int nB = 0x19;
    f32 f3 = 0.0f;
    int nLie = gPlayers[nPlayer].ball.nLie;
    int nClass;
    f32 fDist;
    // The original tests the ball's surface but looks up the one the ball lay on before the shot.
    if (gPlayers[nPlayer].ball.nSurface >= 0) {
        nClass = gSurfaceTypes[gPlayers[nPlayer].ballBefore.nSurface].nClass;
    } else {
        nClass = 10;
    }
    fn_800C73DC(gPlayers[nPlayer].ballBefore.vPos, gPlayers[nPlayer].ball.vStart, v);
    v[1] = 0.0f;
    fDist = fn_80009680(fn_80009744(v));
    if (!fn_8003C9D0(nPlayer, 1, &pSeq, &pShot) && pShot == NULL) {
        pSeq = fn_8003BDBC(nPlayer, nLie, nClass, 15, 1, fDist);
    }
    if (pSeq != NULL && pShot == NULL) {
        pShot = fn_8003A950(pSeq, 9, &nA, &f1, &f2, &nB, &f3, nPlayer);
        if (pShot != NULL) {
            pView->p74 = pSeq;
        }
    }
    if (pShot == NULL && pSeq != NULL) {
        pView->p74 = pSeq;
        pShot = fn_8003A950(pView->p74, pView->n148, &nA, &f1, &f2, &nB, &f3, nPlayer);
        if (pShot == NULL && pView->n148 != 5) {
            pShot = fn_8003A950(pView->p74, 5, &nA, &f1, &f2, &nB, &f3, nPlayer);
        }
    }
    if (pShot != NULL) {
        CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, nA, f1, f2, nB, f3);
        pView->b153 = 0;
    }
}

// Cut to the golfer once the golfer is done animating: shot 0x40 of the plan (not the current or
// the previous one), else shot 5 of the saved sequence, else the plan's shot 5; with none, hold the
// current camera as a hand-made shot. Nothing when the current shot has bAA clear.
void GolfCamera_CutToGolferDoneAnimatingCam(View* pView, int nPlayer) {
    int nA = 5;
    f32 f1 = 0.0f;
    f32 f2 = 100.0f;
    int nB = 0x19;
    f32 f3 = 0.0f;
    f32* pCam = fn_8001731C(pView);
    f32* pSub = fn_80017314(pView);
    CamShot* pShot;
    if (pView->p130 != NULL && pView->p130->bAA == 0) {
        pView->n198 = 1;
        pView->n194 = 1;
        return;
    }
    pShot = fn_80064F7C(nPlayer, 0x40, 1, pView->p130);
    if (pShot == NULL || pShot == pView->p130 || pShot == pView->p13C) {
        pShot = fn_8003A950(pView->p78, 5, &nA, &f1, &f2, &nB, &f3, nPlayer);
    }
    if (pShot != NULL) {
        if (pView->p130 != NULL && pView->p130->bAA == 0) {
            CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 1.0f, f2, nB, f3);
        } else {
            CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, nA, f1, f2, nB, f3);
        }
        pView->b153 = 0;
    } else {
        pShot = fn_8003A7C8(nPlayer, 5, NULL);
        if (pShot != NULL) {
            if (pView->p130 != NULL && pView->p130->bAA == 0) {
                CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 1.0f, 50.0f,
                                               0x19, 0.0f);
            } else {
                CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 0.0f, 50.0f,
                                               0x19, 0.0f);
            }
            pView->b153 = 0;
        } else if (pView->p130 != &pView->shot19C) {
            pView->p13C = pView->p130;
            pView->p130 = NULL;
            pView->fCamTime = 0.0f;
            pView->b153 = 0;
            CameraScript_RecordCurrentCam(&pView->shot19C, pCam, pSub, nPlayer, &pView->script, 0);
            pView->p130 = &pView->shot19C;
            pView->p130->bAD = 5;
            pView->p134 = NULL;
            pView->fCamTime = 0.0f;
            pView->n164 = 0x19;
            pView->b153 = 1;
            pView->p130->f94 = 0.0f;
            pView->p130->f98 = 0.0f;
        }
    }
    pView->n198 = 1;
    pView->n194 = 1;
}

u8 fn_800C6604(View* pView) {
    return pView->n198 > 0;
}

// Pick the swing camera kind (View.n260; 0 is none) for the shot about to be hit. Two-player
// sessions with both flags 0x4000 and 0x8000 set use kind 2 for player 0 and kind 11 for player 1
// on the tee. Otherwise only a shot of kind 1, not from the lies 6..8, with a short club (8 or
// under) unless on the tee, and powered at least the tuning's f9C gets one: kind 11 above fB4 with
// a short club, else, by chance, the player's next kind in turn (1..11), more likely the harder
// the shot; kind 11 falls back to 1 off the tee or with a long club. A scripted GameBreaker makes
// it kind 7 half the time.
void fn_800C6618(View* pView, int nPlayer) {
    u8 bSwingCam = 0;
    int bShortClub;     // fake match: int, tested as (u8); a u8 local truncates it where it is set
    f32* pCam;
    f32 fPower;
    pCam = fn_8001731C(pView);
    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) {
        if (nPlayer == 0 && gPlayers[nPlayer].ball.nLie == 0) {
            pView->n260 = 2;
            Swing_RumbleOff(nPlayer);
            fn_800A68C0(nPlayer);
            return;
        }
        if (nPlayer == 1 && gPlayers[nPlayer].ball.nLie == 0) {
            pView->n260 = 11;
            Swing_RumbleOff(nPlayer);
            fn_800A68C0(nPlayer);
            return;
        }
        pView->n260 = 0;
        return;
    }
    fPower = fn_8005C280(nPlayer);
    bShortClub = gPlayers[nPlayer].nClub <= 8;
    if (gSession.nSplitScreen) {
        bSwingCam = 0;
    } else if (gSession.bReplay) {
        bSwingCam = 0;
    } else if (!gpGame->b28A) {
        bSwingCam = 0;
    } else if (gPlayers[nPlayer].ball.nLie != 0 && gPlayers[nPlayer].nClub > 8) {
        bSwingCam = 0;
    } else if (gPlayers[nPlayer].ball.nLie == 6 || gPlayers[nPlayer].ball.nLie == 7
               || gPlayers[nPlayer].ball.nLie == 8) {
        bSwingCam = 0;
    } else if (gPlayers[nPlayer].nShotKind != 1) {
        bSwingCam = 0;
    } else if (!CameraScript_IsDefaultSwingCam(pView->p130, nPlayer, pCam) && Player_IsCPU(nPlayer)
               && (lbl_80282220->n1EC[nPlayer] == 1 || lbl_80282220->n1EC[nPlayer] == 6
                   || lbl_80282220->n1EC[nPlayer] == 10 || lbl_80282220->n1EC[nPlayer] == 11)) {
        bSwingCam = 0;
    } else if (fPower >= lbl_80281F78->f9C) {
        bSwingCam = 1;
    }
    if (bSwingCam) {
        if (fPower > lbl_80281F78->fB4 && (u8)bShortClub) {
            pView->n260 = 11;
        } else if (fPower > lbl_80281F78->fA4) {
            if ((f32)(s32)(Rand_Next(1) % 100) < lbl_80281F78->fB0) {
                pView->n260 = lbl_80282220->n1EC[nPlayer];
                lbl_80282220->n1EC[nPlayer]++;
                if (lbl_80282220->n1EC[nPlayer] >= 12) {
                    lbl_80282220->n1EC[nPlayer] = 1;
                }
            }
        } else if (fPower > lbl_80281F78->fA0) {
            if ((f32)(s32)(Rand_Next(1) % 100) < lbl_80281F78->fAC) {
                pView->n260 = lbl_80282220->n1EC[nPlayer];
                lbl_80282220->n1EC[nPlayer]++;
                if (lbl_80282220->n1EC[nPlayer] >= 12) {
                    lbl_80282220->n1EC[nPlayer] = 1;
                }
            }
        } else if (fPower > lbl_80281F78->f9C) {
            if ((f32)(s32)(Rand_Next(1) % 100) < lbl_80281F78->fA8) {
                pView->n260 = lbl_80282220->n1EC[nPlayer];
                lbl_80282220->n1EC[nPlayer]++;
                if (lbl_80282220->n1EC[nPlayer] >= 12) {
                    lbl_80282220->n1EC[nPlayer] = 1;
                }
            }
        }
        if (pView->n260 == 11 && (gPlayers[nPlayer].ball.nLie != 0 || !(u8)bShortClub)) {
            pView->n260 = 1;
            lbl_80282220->n1EC[nPlayer] = 2;
        }
        if (fn_8006BEA4() && Rand_Next(1) % 100 > 50) {
            pView->n260 = 7;
            lbl_80282220->n1EC[nPlayer] = 8;
            if (lbl_80282220->n1EC[nPlayer] >= 12) {
                lbl_80282220->n1EC[nPlayer] = 1;
            }
        }
        if (pView->n260 != 0) {
            Swing_RumbleOff(nPlayer);
            fn_800A68C0(nPlayer);
        }
    } else {
        pView->n260 = 0;
    }
}

int fn_800C6B38(View* pView) {
    switch (pView->n260) {
    case 2:
    case 8:
    case 11:
        return 2;
    case 5:
        return 1;
    case 3:     // these return the default, but the original lists them: its jump table sends
    case 4:     // them to their own return
    case 7:
    case 9:
    case 15:
    case 16:
        return 0;
    }
    return 0;
}

// The slow-motion rate for the swing camera kind.
f32 fn_800C6B7C(View* pView) {
    switch (pView->n260) {
    case 2:
        return 0.5f;
    case 8:
        if (fn_800C4518(pView) >= 2) {
            return 0.2f;
        }
        return 0.5f;
    case 11:
        if (fn_800C4518(pView) >= 2) {
            return 0.65f;
        }
        return 0.5f;
    case 5:
        if (fn_800C4518(pView) >= 1) {
            return 0.2f;
        }
        return 0.5f;
    case 15:
        if (pView->f190 > 0.0f) {
            return 1.5f;
        }
        return 0.2f;
    case 16:
        if (pView->f190 > 0.0f) {
            return 0.2f;
        }
        return 1.5f;
    case 4:
    case 9:
        return 1.0f;
    case 7:
        if (lbl_80282220->b5A) {
            return lbl_80282220->f64;
        }
        return 1.0f;
    case 3:     // the default, but listed in the original (its own entry in the jump table)
        return 1.0f;
    }
    return 1.0f;
}

void fn_800C6C8C(void) {
    fn_800C6DE4();
    fn_800C6DFC();
}

u8 fn_800C6CB0(void) {
    if (lbl_80282220 == NULL) {
        return 0;
    }
    return lbl_80282220->b56;
}

u8 fn_800C6CCC(void) {
    int bOn;
    if (lbl_80282220 == NULL) {
        return 0;
    }
    bOn = 0;
    if (lbl_80282220->b56 && fn_800B4908()) {
        bOn = 1;
    }
    return bOn;
}

u8 fn_800C6D28(void) {
    if (lbl_80282220 == NULL) {
        return 0;
    }
    return lbl_80282220->b54 || lbl_80282220->b55;
}

u8 fn_800C6D64(void) {
    if (lbl_80282220 == NULL) {
        return 0;
    }
    return lbl_80282220->b58;
}

u8 fn_800C6D80(void) {
    if (lbl_80282220 == NULL) {
        return 0;
    }
    return lbl_80282220->b59;
}

u8 fn_800C6D9C(void) {
    if (lbl_80282220 == NULL) {
        return 0;
    }
    if (lbl_80282220->b54 || lbl_80282220->b58 || lbl_80282220->b55) {
        return 1;
    }
    return 0;
}

void fn_800C6DE4(void) {
    if (lbl_80282220 != NULL) {
        lbl_80282220->b54 = 0;
    }
}

void fn_800C6DFC(void) {
    if (lbl_80282220 != NULL) {
        lbl_80282220->b58 = 0;
    }
}

void fn_800C6E14(void) {
    if (lbl_80282220 != NULL) {
        lbl_80282220->b59 = 0;
    }
}

void fn_800C6E2C(void) {
    if (lbl_80282220 != NULL) {
        lbl_80282220->b5A = 0;
    }
}

u8 fn_800C6E44(View* pView) {
    if (pView->n194 == 1 && pView->p134 == NULL
        && (pView->p130 == NULL || pView->p130->f4C < pView->fCamTime)) {
        return 1;
    }
    return 0;
}

u8 fn_800C6E88(View* pView, int nPlayer) {
    if (pView->b268 == 1) {
        return 0;
    }
    if (fn_8003A950(pView->p74, 0x17, NULL, NULL, NULL, NULL, NULL, nPlayer) != NULL) {
        return pView->n194 < 1;
    }
    return 0;
}

u8 fn_800C6F14(View* pView, int nPlayer) {
    if (pView->n164 != 0x19 && pView->p74 != NULL
        && fn_8003A950(pView->p74, pView->n164, NULL, NULL, NULL, NULL, NULL, nPlayer) != NULL) {
        return 1;
    }
    return 0;
}

// Is the camera ready to move on: yes with no sequence or no current shot, or once n198 is set; no
// while fn_800C6E88 or fn_800C6F14 has a shot to go to; else when less than fLeft is left on the
// next shot, or (with none) on both the current shot's f4C and f168 past f11C.
u8 fn_800C6F7C(View* pView, int nPlayer, f32 fLeft) {
    if (pView->p74 == NULL || pView->p130 == NULL) {
        return 1;
    }
    if (pView->n198 > 0) {
        return 1;
    }
    if (fn_800C6E88(pView, nPlayer)) {
        return 0;
    }
    if (fn_800C6F14(pView, nPlayer)) {
        return 0;
    }
    if (pView->p134 != NULL) {
        return fn_800C7394(pView) < fLeft;
    }
    if (pView->p130->f4C - pView->f11C < fLeft && pView->f168 - pView->f11C < fLeft) {
        return 1;
    }
    return 0;
}

void fn_800C7080(View* pView) {
    pView->n198 = 1;
}

u8 fn_800C708C(View* pView) {
    if (pView->p134 != NULL) {
        return fn_8003DC78(pView->p134) != 0;
    }
    if (pView->p130 != NULL) {
        return fn_8003DC78(pView->p130) != 0;
    }
    return 0;
}

void fn_800C70F8(View* pView, int a) {
    pView->b268 = a;
}

// No current shot, no next one, or more than 5 seconds on this one.
u8 fn_800C7100(View* pView) {
    if (pView->p130 == NULL || pView->p134 == NULL || pView->fCamTime > 5.0f) {
        return 1;
    }
    return 0;
}

int fn_800C7138(View* pView) {
    return pView->n260;
}

void fn_800C7140(int a) {
    lbl_80282220->b55 = a;
}

u8 fn_800C714C(void) {
    return lbl_80282220->b55;
}

void fn_800C7158(View* pView, int a) {
    pView->b269 = a;
}

u8 fn_800C7160(View* pView) {
    return pView->b269;
}

void fn_800C7168(View* pView, int a) {
    pView->b26A = a;
}

u8 fn_800C7170(View* pView) {
    return pView->b26A;
}

void fn_800C7178(View* pView, int nPlayer) {
    fn_800C1790(pView, nPlayer);
    fn_800C6E14();
    fn_800C6DE4();
    fn_800C6E2C();
}

// Is the ball behind the camera (on the far side from where it looks)? Never without a current
// shot, for shot kind 3 (bAD), or once f11C passes 5.
u8 fn_800C71A4(View* pView, int nPlayer) {
    f32 vLook[4];
    f32 vBall[4];
    if (pView->p130 == NULL) {
        return 0;
    }
    if (pView->p130->bAD == 3) {
        return 0;
    }
    if (pView->f11C > 5.0f) {
        return 0;
    }
    fn_800C73DC(pView->v10, pView->v0, vLook);
    fn_800C73DC(gPlayers[nPlayer].ball.vPos, pView->v0, vBall);
    vLook[1] = 0.0f;
    vBall[1] = 0.0f;
    if (vLook[0] != 0.0f || vLook[1] != 0.0f || vLook[2] != 0.0f) {
        fn_800BAF04(vLook, vLook);
    }
    if (vBall[0] != 0.0f || vBall[1] != 0.0f || vBall[2] != 0.0f) {
        fn_800BAF04(vBall, vBall);
    }
    if (fn_8000C5FC(vLook, vBall) > 0.0f) {
        return 0;
    }
    return fn_800C708C(pView);
}

u8 fn_800C72DC(View* pView) {
    return pView->n198 != 0;
}

void fn_800C72F0(void) {
    if (lbl_80282220 != NULL) {
        lbl_80282220->b54 = 0;
        lbl_80282220->b55 = 0;
        lbl_80282220->b56 = 0;
        lbl_80282220->b57 = 0;
        lbl_80282220->b58 = 0;
        lbl_80282220->b59 = 0;
        lbl_80282220->b5A = 0;
        lbl_80282220->b5B = 0;
    }
}

// Cameras 1 and 2 are done once f18C reaches 1; the others at once.
u8 fn_800C7340(View* pView, int nPlayer) {
    if (pView->nCurCamera == 1) {
        return pView->f18C >= 1.0f;
    }
    if (pView->nCurCamera == 2) {
        return pView->f18C >= 1.0f;
    }
    return 1;
}

// The time left on the current shot (0 when there is no next one).
f32 fn_800C7394(View* pView) {
    if (pView->p134 == NULL) {
        return 0.0f;
    }
    return pView->f110 - pView->fCamTime;
}

// Adds two vectors (three floats) into pOut.
#ifdef __MWERKS__
asm void fn_800C73B8(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_800C73B8(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
}
#endif

// Subtracts pB from pA (three floats) into pOut.
#ifdef __MWERKS__
asm void fn_800C73DC(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_800C73DC(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// Negates a vector (three floats) into pOut.
#ifdef __MWERKS__
asm void fn_800C7400(register f32* pA, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    ps_neg f0, f0
    ps_neg f1, f1
    psq_st f0, 0(pOut), 0, 0
    psq_st f1, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_800C7400(f32* pA, f32* pOut) {
    pOut[0] = -pA[0];
    pOut[1] = -pA[1];
    pOut[2] = -pA[2];
}
#endif

// An animation event's time in the character's blend tree (0 without a character).
f32 fn_800C741C(Character* pChar, u64 uEvent) {
    if (pChar == NULL) {
        return 0.0f;
    }
    return fn_80072CB8(&pChar->blend, uEvent);
}

// A predicted game breaker is on.
u8 fn_800C7450(void) {
    int bOn = 0;
    if (lbl_80202898.bGameBreaker && lbl_80202898.nGBType == 1) {
        bOn = 1;
    }
    return bOn;
}
