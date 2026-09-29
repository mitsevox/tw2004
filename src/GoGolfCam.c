// GoGolfCam.c (EA's name, from its asserts; TW06 and TW07 golf/cameras/GoGolfCam.c): the golf
// cameras, EA's GolfCamera_*. Each camera mode (View.nCurCamera) has an init, called by
// CameraController_SetCameraMode, and a per-frame process, called by CameraController_Idle; both
// drive the view's camera script (gocamscripts.c). The modes: 0 shot setup, 1 zoom to the aim
// point, 2 the same on the green, 3 elevator, 4 green, 5 putt preview (TW07's green roll), 6
// reverse putt, 7 knee cam, 8 ball placement, 9 speed golf's run to the ball, 10 hole fly-by, 11
// pre-shot, 12 swing, 13 slow-motion swing replay, 14 ball flight, 15 post-shot, 16 ball in the
// hole, 17 scorecard, 18 tutorial wait, 19 steep slope, 20 3-screen comic, 21 heartbeat, 22
// shutter, 23 front end (create-a-player), 24 golfer bone (unused). Also here: the special swing
// cameras picked at the hit (View.n260: matrix, super zoom, swing replays, comic, heartbeat,
// shutter) and the shared camera state (gGolfCamState), with a per-course elevator camera height.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "camera.h"
#include "dyncam.h"

CamLens* Camera_GetLens(void* pCamera);                    // the render camera's lens
void     RC_UpdateCurrentScreenMatrices(void);
u8       CameraController_TargetIsOnScreen(int nPlayer);
u8       ComicCam_UpdateComicCam(View* pView, int nPlayer, f32 fFrameTime);
void     GolfCam_Vec3Add(f32* pA, f32* pB, f32* pOut);
void     GolfCam_Vec3Sub(f32* pA, f32* pB, f32* pOut);
void     GolfCam_Vec3Negate(f32* pA, f32* pOut);
f32      GolfCam_GetBlendTagTime(Character* pChar, u64 uEvent);
void     GolfCamera_ChooseReactionCam(View* pView, int nPlayer, int a);
void     CameraController_StartScriptOfKind(View* pView, int nPlayer, int nCamera);
void     ComicCam_StartComicCam(int a, View* pView, int nPlayer);
u8       ComicCam_IsScreenFrozen(void);
void     GolfCamera_ComputeSteepSlopeCamVectors(View* pView, int nPlayer);
void     GolfCamera_CreateReplayCamera(View* pView, f32* pCam, f32* pSub, int nPlayer);
u8       GolfCamera_IsCameraTrackingPlayer(View* pView);
void     fn_80038010(u8 a, int n, f32* pVec);
void     fn_800380A8(u8 a, f32* pVec, u8 b, int nSlot, f32 f1, f32 f2);
int      CameraController_GetClippedGolfer(void);
void     mat44flt_EulerAngles(f32 (*m)[4], f32 a, f32 b, f32 c);  // a rotation matrix from three angles
void     LLMath_mat44fltMultiply33(f32 (*m)[4], f32* pIn, f32* pOut);  // a vector through a matrix
void     CameraController_BallIsOnScreen(int nPlayer);
void     GolfCamera_ClampLookAngle(f32* pFrom, f32* pTo, f32* pOut);
int      GolfCamera_LimitPositionChange(f32* pFrom, f32* pTo, f32* pOut, f32 fMax);
// The segment crosses the outline (at pHit).
u8       fn_8004B6F8(f32* pFrom, f32* pTo, f32* pHit);
f32      Camera_GetLensFovScale(CamLens* pLens);            // char.c: the lens's fB0
void     fn_80038054(u8 a, int n, f32 f1, f32 f2);
CamShot* GolfCamera_GetAlternateSwingCamera(int nFirst, int nPlayer);
void     GolfCamera_UpdateSwingSlowMoCamera(View* pView, f32* pCam, f32* pSub, int nPlayer);
f32      GolfCamera_GetTimeToNextShot(View* pView);
void     Quat_RotateVector(f32* pQuat, f32* pIn, f32* pOut);  // rotate a vector by a quaternion
u8       fn_8006BEA4(void);                             // emotion.c: a scripted GameBreaker's letterbox is up
void     Gaud_InitSpecialShot(u8 nPlayer);
void     fn_80039344(int nView, f32 f);                 // a per-view float (Swing.c's declaration)
f32      Math_Tan(f32 x);                            // tan, as a float
void     CameraController_CheckForEvents(View* pView, int nPlayer);
f32      fn_800D04AC(int nPlayer);                      // Swing.c's declaration
f32      GolfCamera_UpdateMatrixCamera(View* pView, f32* pCam, f32* pSub, int nPlayer);
f32      GolfCamera_UpdateSuperZoomCamera(View* pView, f32* pCam, f32* pSub, int nPlayer);
u8       GameEffects_IsPredictedGameBreakerOn(void);
u8       fn_8012022C(void);                            // (sweep code) lbl_80281900's +0x370 is nonzero
void     Character_AlignCharacterForShotImpact(Character* pChar);                 // char.c
void     SKATime_Pause(u8* pAnim);                        // set the player's pause bit (0x2)
u8       GolfCamera_ZoomCamGetStartAndEndVecs(View* pView, int nPlayer, f32* pSub, f32* pAim, f32* pCam);
u8       GolfCamera_ChooseImpactMatrixCam(View* pView, int nPlayer);
u8       GolfCamera_ChooseSuperZoomCam(View* pView, int nPlayer);
void     GolfCamera_CreateMatrixCamera(View* pView, f32* pFrom, f32* pTo, int nPlayer);
void     GolfCamera_CreateSuperZoomCamera(View* pView, f32* pFrom, f32* pTo, int nPlayer);
u8       Ter_CheckForGroundCollision(CourseInfo* pCourse, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal,
                                     SurfaceType** ppSurface, TerObject** ppObj);

// .bss and .sbss (one object each, so the reverse-order rule does not come into it).
f32 gSteepSlopeCamLastTarget[4];    // the target the steep-slope camera last worked for
GolfCamState* gGolfCamState;        // the shared state (GolfCamera_Init)

s32 gSteepSlopeCamLastTries = -1;   // the steep-slope camera's tries last time (-1: none yet)

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f (0x802842C0), before the 0.0f and 10.0f GolfCamera_Init uses first; its body is unknown.
static f32 GoGolfCam_StrippedFn(f32 x) {
    return x + 1.0f;
}

// Allocate the golf cameras' shared state (gGolfCamState, 0x200 bytes of static memory) and reset
// it: the special-camera flags (b54..b5C) off, fly-by route 0 (n60), each player's next special
// swing camera kind 1 and each course's elevator camera height 10. Called when a round's systems
// start (fn_80062E00).
void GolfCamera_Init(void) {
    int i;
    gGolfCamState = StaticMem_Alloc(0x200, 2, 0, "GoGolfCam.c", 164);
    gGolfCamState->b54 = 0;
    gGolfCamState->b55 = 0;
    gGolfCamState->b56 = 0;
    gGolfCamState->b57 = 0;
    gGolfCamState->b58 = 0;
    gGolfCamState->b59 = 0;
    gGolfCamState->b5A = 0;
    gGolfCamState->b5B = 0;
    gGolfCamState->n60 = 0;
    gGolfCamState->b5C = 0;
    gGolfCamState->f68 = 0.0f;
    for (i = 0; i < 5; i++) {
        gGolfCamState->n1EC[i] = 1;
    }
    for (i = 0; i < 21; i++) {
        gGolfCamState->fElevatorHeight[i] = 10.0f;
    }
}

// Free the shared camera state GolfCamera_Init allocated (called when a round's systems stop,
// fn_80062E20).
void GolfCamera_DeInit(void) {
    StaticMem_Free(gGolfCamState);
    gGolfCamState = NULL;
}

// Camera 0, the shot-setup camera: the follow-on of the current sequence when that is a type-1
// sequence followed by a type-2 one, else a new pre-flight sequence (type 2) for the ball's lie;
// its kind-2 shot starts, as a cut unless the current shot is of type 1 (bAD) with nothing queued.
// A colour fade in progress is restarted at stage 2.
void GolfCamera_InitShotSetupCamera(View* pView, int nPlayer) {
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
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    pView->script.bCF = 0;
    gGolfCamState->f68 = 0.0f;
    nLie = gPlayers[nPlayer].ball.nLie;
    pSeq = pView->p74;
    if (pSeq != NULL && pSeq->b44 == 1 && pSeq->p20 != pSeq && pSeq->p20 != NULL && pSeq->p20->b44 == 2) {
        pView->p74 = pSeq->p20;
    } else {
        pView->p74 = DynamicCam_ChoosePreFlightSequence(nPlayer, nLie, 2);
    }
    pShot = DynamicCam_ChooseScriptInSequence(pView->p74, 2, &nA, &f1, &f2, &nB, &f3, nPlayer);
    if (pShot != NULL) {
        if (pView->script.pShot == NULL || pView->script.pShot->bAD != 1 || pView->script.pNextShot != NULL) {
            nA = 5;
        }
        CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, nA, f1, f2, nB, f3);
        if (pView->script.nFade != 0) {
            pView->script.fFadeTime = 0.0f;
            pView->script.nFade = 2;
        }
    }
}

// Camera 0: only the script's per-frame update.
void GolfCamera_ProcessShotSetupCamera(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    CamScript_RunScript(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
}

// Camera 1, the zoom-to-aim camera (the view flies out to look at the aim point): start from the
// current camera, keeping its height over the ground (the lower ground height, else the higher,
// else the ball's) and the offsets of the last shot of the current run (up to one of kind 6 or
// 8..10; across and along swapped, one negated, for shots of type 0x15). Unless the golfer is in
// shot setup, the look-at point moves along the current look direction to the aim point's distance.
// Fires event 0x30.
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
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    LLMath_CopyVec(gPlayers[nPlayer].vTargetCopy, vAim);
    Ter_GetEnclosingGroundHeight(Ter_GetTGD(), pCam, &fLow, &fHigh);
    if (fLow < -60000.0f) {
        if (!(fHigh < -60000.0f)) {
            fGround = fHigh;
        } else {
            fGround = gPlayers[nPlayer].vBall[1];
        }
    } else {
        fGround = fLow;
    }
    if (pView->script.pShot != NULL) {
        for (pShot = pView->script.pShot; pShot->p40 != NULL; pShot = pShot->p40) {
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
    pView->script.pShot = NULL;
    pView->script.f108 = -1.0f;
    if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) != GS_SHOT_SETUP) {
        GolfCam_Vec3Sub(pSub, pCam, vDir);
        if (vDir[0] != 0.0f || vDir[1] != 0.0f || vDir[2] != 0.0f) {
            LLMath_Normalize3(vDir, vDir);
        }
        GolfCam_Vec3Sub(vAim, pCam, v);
        Vec3_Scale(Math_Sqrt(Vec3_LengthSqClamped(v)), vDir, vDir);
        GolfCam_Vec3Add(pCam, vDir, pSub);
    }
    EVENT_Trigger(nPlayer, 0x30, NULL, -1);
}

// The zoom-to-aim camera's tick: fly the camera to the goal GolfCamera_ZoomCamGetStartAndEndVecs
// works out, fast at first and slowing over the tuning's f8, with GoPostFx's screen effect
// (fn_80038054) set by its speed while it moves. script.f108 runs from below 0 (not set off yet) to
// 1 (arrived); on the way the height blends from the aim's ground plus the shot's f68 to the
// tuning's f14 over View.script.fD8 (the ground at the target) in the second half. Arrived, it
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
    s8 bKeepFlat; // fake match: s8 (TW07's bool keepFlat); as u8 or int the srwi is scheduled early
    f32 fSpeed;
    f32 fDist;
    f32 fTotal;
    f32 fBase;
    f32 f;
    f32 fSlow;
    f32 fAimY;
    f32 fCamY;
    f32 fFlatDist;
    f32 fAmount;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    Vec3Copy(pCam, vOld);
    LLMath_CopyVec(gPlayers[nPlayer].vTargetCopy, vTarget);
    bMirror = GolfCamera_ZoomCamGetStartAndEndVecs(pView, nPlayer, pCam, vAim, vGoal);
    pCourse = Ter_GetTGD();
    fBase = lbl_80281F78->f0;
    fSlow = lbl_80281F78->f8;
    // flat distances to the goal: from the camera, from the aim and from where it started
    GolfCam_Vec3Sub(vGoal, pCam, vMove);
    vMove[1] = 0.0f;
    fDist = Math_Sqrt(Vec3_LengthSqClamped(vMove));
    GolfCam_Vec3Sub(vGoal, vAim, vAimMove);
    vAimMove[1] = 0.0f;
    fTotal = Math_Sqrt(Vec3_LengthSqClamped(vAimMove));
    GolfCam_Vec3Sub(vGoal, pView->shot19C.v30, vStart);
    vStart[1] = 0.0f;
    f = Math_Sqrt(Vec3_LengthSqClamped(vStart));
    if (fTotal < f) {
        fTotal = f;
    }
    if (pView->script.f108 >= 1.0f) {
        GolfCam_Vec3Sub(vGoal, pCam, vCreep);
        vCreep[1] = 0.0f;
        fSlow = Math_Sqrt(Vec3_LengthSqClamped(vCreep));
        if (vCreep[0] != 0.0f || vCreep[1] != 0.0f || vCreep[2] != 0.0f) {
            LLMath_Normalize3(vCreep, vCreep);
        }
        fSlow /= lbl_80281F78->f18;
        Vec3_Scale(fSlow, vCreep, vCreep);
        GolfCam_Vec3Add(pCam, vCreep, pCam);
        fDist = 0.0f;
    } else if (fDist > lbl_80281F78->f24 && Vec3_Dot(vMove, vAimMove) > 0.0f
               && pView->script.f108 >= 0.0f) {
        // on the way: faster the further it is
        if (fDist < 1.0f) {
            f = fBase;
        } else {
            f = fBase * (f32)Math_Sqrt(fDist);
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
            LLMath_Normalize3(vMove, vMove);
        }
        Vec3_Scale(fSlow, vMove, vMove);
        GolfCam_Vec3Add(vMove, pCam, pCam);
        fAmount = lbl_80281F78->f1C * (1.0f - fBase / fSpeed);
        if (fAmount < 0.0f) {
            fAmount = 0.0f;
        }
        if (gSession.nPaused == 0) {
            fn_80038054(1, ViewController_GetCurrentViewControllerID(), 0.0f, fAmount);
        }
        pView->script.f108 = 1.0f - fDist / fTotal;
    } else if (pView->script.f108 >= 0.0f) {
        EVENT_Trigger(nPlayer, 0x31, NULL, -1);
        pView->script.f108 = 1.0f;
    } else if (gSession.nPaused == 0) {
        fn_80038054(1, ViewController_GetCurrentViewControllerID(), 0.0f, lbl_80281F78->f20);
    }
    if (pCourse != NULL) {
        Ter_GetEnclosingGroundHeight(pCourse, vTarget, &fLow, &fHigh);
        if (fLow < -60000.0f) {
            if (!(fHigh < -60000.0f)) {
                pView->script.fD8 = fHigh;
            }
        } else {
            pView->script.fD8 = fLow;
        }
    }
    // before setting off, far from the goal: rise to the course's elevator height
    if (pView->shot19C.f68 < gGolfCamState->fElevatorHeight[Game_GetCourse()] && pView->script.f108 < 0.0f
        && fDist > 20.0f) {
        pView->shot19C.f68 += 0.25f * (FRAME_RATE * gSession.fFrameTime);
    } else if (pView->script.f108 < 0.0f) {
        pView->script.f108 = 0.0f;
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
        fCamY = pView->script.fD8;
        fCamY += lbl_80281F78->f14;
    } else {
        fAimY = 0.0f;
        fCamY = 0.0f;
    }
    if (pView->script.f108 < 0.0f || fDist / fTotal > 0.5f) {
        f = fAimY;
    } else {
        f = fDist / fTotal;
        f *= 2.0f;
        f = fAimY + (1.0f - f) * (fCamY - fAimY);
    }
    if (pView->script.f108 >= 1.0f) {
        pCam[1] = pCam[1] + lbl_80281F78->f2C * (f - pCam[1]);
    } else {
        pCam[1] = f;
    }
    CamScript_KeepAboveGround(nPlayer, pCam, vOld, 1, &bHit, NULL, NULL, 0.5f);
    if (pView->script.f108 >= 1.0f) {
        fFlatDist = 1.0f + lbl_80281F78->f4;
        fFlatDist *= 1.0f
                / Camera_GetLensFovScale(
                    Camera_GetLens(ViewController_GetRenderContext(gPlayers[nPlayer].nView[0])));
    } else {
        fFlatDist = 10000.0f;
    }
    if (!bHit && pCam[1] < lbl_80281F78->f168 + Ter_GetTGD()->fFloor) {
        pCam[1] = lbl_80281F78->f168 + Ter_GetTGD()->fFloor;
    }
    f = lbl_80281F78->f18 + fDist / fTotal * (lbl_80281F78->f28 - lbl_80281F78->f18);
    if (pView->script.f108 >= 0.0f) {
        bKeepFlat = bMirror != 0;
        CameraScript_LagAimMarker(nPlayer, pSub, pCam, &pView->shot19C, 1, bKeepFlat, f, fFlatDist,
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
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    pCourse = Ter_GetTGD();
    if (pCourse != NULL) {
        LLMath_CopyVec(gPlayers[nPlayer].vTargetCopy, vAim);
        Ter_GetEnclosingGroundData(Ter_GetTGD(), pCam, &fLow, &pSurfaceLow, vNormalLow, &fHigh,
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
        if (pView->script.pShot != NULL) {
            for (pShot = pView->script.pShot; pShot->p40 != NULL; pShot = pShot->p40) {
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
        pView->script.pShot = NULL;
        pView->script.f108 = -1.0f;
        pView->script.f10C = 100000000.0f;
        if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) != GS_SHOT_SETUP) {
            GolfCam_Vec3Sub(pSub, pCam, vDir);
            if (vDir[0] != 0.0f || vDir[1] != 0.0f || vDir[2] != 0.0f) {
                LLMath_Normalize3(vDir, vDir);
            }
            GolfCam_Vec3Sub(vAim, pCam, v);
            Vec3_Scale(Math_Sqrt(Vec3_LengthSqClamped(v)), vDir, vDir);
            GolfCam_Vec3Add(pCam, vDir, pSub);
        }
        EVENT_Trigger(nPlayer, 0x30, NULL, -1);
        Vec3Copy(Camera_GetLens(ViewController_GetRenderContext(gPlayers[nPlayer].nView[0]))->m4[0],
                 pView->v20);
    }
}

// The green zoom-to-aim camera's tick: fly the camera to the goal
// GolfCamera_ZoomCamGetStartAndEndVecs works out, fast at first and slowing over the tuning's f30,
// rising to a height that keeps the pin in the lens, with the screen effect fn_80038054 stronger
// the faster it moves. Arrived (script.f108 1), it creeps on towards the goal and settles its
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
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    bArrived = 0;
    if (gSession.nPaused == 0) {
        LLMath_CopyVec(gPlayers[nPlayer].vTargetCopy, vTarget);
        Vec3Copy(pCam, vOld);
        GolfCamera_ZoomCamGetStartAndEndVecs(pView, nPlayer, pCam, vAim, vGoal);
        pCourse = Ter_GetTGD();
        fBase = lbl_80281F78->f34;
        fSlow = lbl_80281F78->f30;
        // flat distances to the goal: from the camera, from the aim and from where it started
        GolfCam_Vec3Sub(vGoal, pCam, vMove);
        vMove[1] = 0.0f;
        fDist = Math_Sqrt(Vec3_LengthSqClamped(vMove));
        GolfCam_Vec3Sub(vGoal, vAim, vAimMove);
        vAimMove[1] = 0.0f;
        fTotal = Math_Sqrt(Vec3_LengthSqClamped(vAimMove));
        GolfCam_Vec3Sub(vGoal, pView->shot19C.v30, vStart);
        vStart[1] = 0.0f;
        f = Math_Sqrt(Vec3_LengthSqClamped(vStart));
        if (fTotal < f) {
            fTotal = f;
        }
        fHeight = lbl_80281F78->f44;
        pLens = Camera_GetLens(ViewController_GetRenderContext(gPlayers[nPlayer].nView[0]));
        fHeight *= 1.0f / Camera_GetLensFovScale(pLens);
        // high enough to see the pin (up to 20 from the target) through the lens
        nPinSet = Game_CurrentPinSet();
        GolfCam_Vec3Sub(&pCourse->pin[nPinSet].x, vTarget, vPin);
        vPin[1] = 0.0f;
        f = Math_Sqrt(Vec3_LengthSqClamped(vPin));
        f += 1.5f;
        fSpeed = (f < 0.0f) ? 0.0f : ((f > 20.0f) ? 20.0f : f);
        f = fSpeed / Math_Tan(CA_fGetCameraFieldOfView(pLens) / 2.0f);
        if (f > fHeight) {
            fHeight = f;
        }
        if (pView->script.f108 >= 1.0f) {
            GolfCam_Vec3Sub(vGoal, pCam, vCreep);
            vCreep[1] = 0.0f;
            fSlow = Math_Sqrt(Vec3_LengthSqClamped(vCreep));
            if (vCreep[0] != 0.0f || vCreep[1] != 0.0f || vCreep[2] != 0.0f) {
                LLMath_Normalize3(vCreep, vCreep);
            }
            fSlow /= lbl_80281F78->f48;
            Vec3_Scale(fSlow, vCreep, vCreep);
            GolfCam_Vec3Add(pCam, vCreep, pCam);
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
        } else if (fDist > lbl_80281F78->f24 && Vec3_Dot(vMove, vAimMove) > 0.0f) {
            // still short of the goal: faster the further it is
            if (fDist < 1.0f) {
                f = fBase;
            } else {
                f = fBase * (f32)Math_Sqrt(fDist);
            }
            fSpeed = f;
            if (fDist > pView->script.f10C) {
                fSpeed = f + (fDist - pView->script.f10C);
                pView->script.f10C = pView->script.f10C - 0.3f;
            } else {
                pView->script.f10C = fDist;
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
                LLMath_Normalize3(vMove, vMove);
            }
            Vec3_Scale(fSlow, vMove, vMove);
            GolfCam_Vec3Add(vMove, pCam, pCam);
            fSlow = lbl_80281F78->f1C * (1.0f - fBase / fSpeed);
            if (fSlow < 0.0f) {
                fSlow = 0.0f;
            }
            if (gSession.nPaused == 0) {
                fn_80038054(1, ViewController_GetCurrentViewControllerID(), 0.0f, fSlow);
            }
            if (bArrived) {
                EVENT_Trigger(nPlayer, 0x31, NULL, -1);
                pView->script.f108 = 1.0f;
            } else {
                pView->script.f108 = 1.0f - fDist / fTotal;
            }
            f = fHeight + (pView->shot19C.f68 - pView->shot19C.v30[1]);
            f *= 1.0f - fDist / fTotal;
            pCam[1] = pView->shot19C.v30[1] + f;
        } else {
            EVENT_Trigger(nPlayer, 0x31, NULL, -1);
            pView->script.f108 = 1.0f;
        }
        bHit = 0;
        if ((CamScript_KeepAboveGround(nPlayer, pCam, vOld, 1, NULL, NULL, &bHit, lbl_80281F78->f168) || bHit)
            && pView->script.f108 < 0.0f) {
            pView->script.f108 = 0.0f;
        }
        f = lbl_80281F78->f48;
        if (pView->shot19C.f74 >= 0.0f) {
            pView->shot19C.f74 -= lbl_80281F78->f3C;
        }
        CameraScript_LagAimMarker(nPlayer, pSub, pCam, &pView->shot19C, 0, 0, f, 0.0f, lbl_80281F78->fDC);
        if (pView->script.f108 < 1.0f) {
            GolfCam_Vec3Sub(pSub, pCam, vDiff);
            vSide[0] = vDiff[2];
            vSide[1] = 0.0f;
            vSide[2] = -vDiff[0];
        } else {
            GolfCam_Vec3Sub(vTarget, gPlayers[nPlayer].vBall, vDiff);
            vSide[0] = vDiff[2];
            vSide[1] = 0.0f;
            vSide[2] = -vDiff[0];
        }
        CameraController_LagSideVector(pView->v20, vSide, pView->v20);
    }
}

// Camera 19, the steep-slope camera (the view moved clear of a slope that hides the target): only
// while a shot is running, the position is worked out (GolfCamera_ComputeSteepSlopeCamVectors) and
// held as the hand-made shot "STEEPSLOPE CAM".
void GolfCamera_InitSteepSlopeCamera(View* pView, int nPlayer) {
    if (pView->script.pShot != NULL) {
        GolfCamera_ComputeSteepSlopeCamVectors(pView, nPlayer);
        pView->script.pShot = NULL;
        CameraScript_RecordCurrentCam(&pView->shot19C, pView->v0, pView->v10, nPlayer, &pView->script, 0);
        strcpy(pView->shot19C.szName, "STEEPSLOPE CAM");
    }
}

// Camera 19's tick: the steep-slope position worked out again
// (GolfCamera_ComputeSteepSlopeCamVectors), then the script's update.
void GolfCamera_ProcessSteepSlopeCamera(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    GolfCamera_ComputeSteepSlopeCamVectors(pView, nPlayer);
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    CamScript_RunScript(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
}

// Camera 3, the elevator camera: while a shot is running, the current camera is recorded as the
// hand-made shot "ELEVATOR CAM", raised by the course's elevator height (fElevatorHeight) and
// blended to (kind 1) over the tuning's f94.
void GolfCamera_InitElevatorCamera(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    if (pView->script.pShot != NULL) {
        CameraScript_RecordCurrentCam(&pView->shot19C, pCam, pSub, nPlayer, &pView->script, 0);
        strcpy(pView->shot19C.szName, "ELEVATOR CAM");
        pView->shot19C.v20[1] += gGolfCamState->fElevatorHeight[Game_GetCourse()];
        pView->shot19C.bAC = 9;
        CameraScript_InterpToNewScript(&pView->script, &pView->shot19C, nPlayer, pCam, pSub, 1,
                                       lbl_80281F78->f94, 100.0f, 0x19, 0.0f);
    }
}

// Camera 3's tick (the elevator camera): the script's per-frame update.
void GolfCamera_ProcessElevatorCamera(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    CamScript_RunScript(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
}

// Camera 8, the ball-placement camera (STATEFUNC_PlaceBallInit): a 60-degree lens and no shot or
// sequence; script.n110 0 makes the first tick place the camera outright.
void GolfCamera_InitPlaceBallCamera(View* pView, int nPlayer) {
    CameraController_GetCameraOrigin(pView);
    CameraController_GetCameraLookPoint(pView);
    CA_vSetCameraFieldOfView(Camera_GetLens(ViewController_GetRenderContext(gPlayers[nPlayer].nView[0])),
                             DEG(60.0f));
    pView->script.n110 = 0;
    pView->p74 = NULL;
    pView->script.pShot = NULL;
}

// Camera 8: 10 back and up at 20 degrees from the ball's placement spot along its heading
// (fPlaceHeading), over the ground there, which it follows smoothly; it looks at the placement
// spot.
void GolfCamera_ProcessPlaceBallCamera(View* pView, int nPlayer) {
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
    f32 fDiff;
    f32 fUp;
    f32 fBack;
    f32 fSin;
    f32 fCos;
    f32 fX;
    f32 fZ;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    pCourse = Ter_GetTGD();
    if (pCourse != NULL) {
        Vec3Copy(pCam, vOld);
        nPinSet = Game_CurrentPinSet();
        if (gSession.nPaused == 0) {
            CamScript_Fade(&pView->script, gSession.fFrameTime);
            pView->script.fFadeTime += gSession.fFrameTime;
        }
        if (gSession.fFrameTime != 0.0f) {
            fSin = Math_Sin(20.0f * PI / 180.0f);    // not DEG(20.0f): see GolfCamera_ProcessSpeedGolfRunCamera
            fCos = Math_Cos(20.0f * PI / 180.0f);
            fUp = 10.0f * fSin;
            fBack = 10.0f * fCos;
            fSin = Math_Sin(gPlayers[nPlayer].fPlaceHeading);
            fCos = Math_Cos(gPlayers[nPlayer].fPlaceHeading);
            fX = fBack * -fSin;
            fZ = fBack * fCos;
            pCam[0] = fX + gPlayers[nPlayer].vPlacement[0];
            pCam[2] = fZ + gPlayers[nPlayer].vPlacement[2];
            fGround = CamScript_GuessBestPlayableHeight(pCam, &pSurfaceAt);
            if (fGround < -60000.0f) {
                fGround = gPlayers[nPlayer].vPlacement[1];
            }
            if (pView->script.n110 == 0) {
                pView->script.fD8 = fGround;
            } else if (gPlayers[nPlayer].uFlagsEF0 & 1) {
                if (pView->script.fD8 > gPlayers[nPlayer].vPlacement[1]) {
                    if (gPlayers[nPlayer].vPlacement[1] > fGround) {
                        pView->script.fD8 = gPlayers[nPlayer].vPlacement[1];
                    } else {
                        pView->script.fD8 = fGround;
                    }
                } else if (pView->script.fD8 < gPlayers[nPlayer].vPlacement[1]) {
                    if (gPlayers[nPlayer].vPlacement[1] > fGround) {
                        pView->script.fD8 = gPlayers[nPlayer].vPlacement[1];
                    } else {
                        pView->script.fD8 = fGround;
                    }
                }
                fGround = pView->script.fD8;
            } else {
                if (pView->script.fD8 < gPlayers[nPlayer].vPlacement[1]) {
                    if (pView->script.fD8 < pCourse->tee[gSession.nTeeSet[nPlayer]].y + 0.1f
                        || pView->script.fD8 < pCourse->pin[nPinSet].y + 0.1f) {
                        if (pView->script.fD8 > fGround) {
                            fGround = 0.1f + pView->script.fD8;
                        }
                        pView->script.fD8 = fGround;
                    }
                } else {
                    if (pView->script.fD8 > pCourse->tee[gSession.nTeeSet[nPlayer]].y - 0.1f
                        || pView->script.fD8 > pCourse->pin[nPinSet].y - 0.1f) {
                        if (pView->script.fD8 > fGround) {
                            fGround = pView->script.fD8 - 0.1f;
                        }
                        pView->script.fD8 = fGround;
                    }
                }
                fGround = pView->script.fD8;
            }
            if (fGround < -60000.0f) {
                fGround = 0.0f;
            }
            if (pView->script.n110 != 0) {
                fUp += fGround;
                v[0] = pCam[0];
                v[2] = pCam[2];
                v[1] = fUp;
                if (fUp > pCam[1]) {
                    if (Ter_CheckForGroundCollision(pCourse, v, pCam, vHit, vNormal, &pSurface, &pObj)) {
                        pCam[1] = fUp;
                        pView->script.fD8 = fGround;
                    } else {
                        fDiff = fUp;
                        fDiff -= pCam[1];
                        pCam[1] += (1.0f - lbl_80281F78->f98) * fDiff;
                    }
                } else {
                    pCam[1] += (1.0f - lbl_80281F78->f98) * (fUp - pCam[1]);
                }
            } else {
                pCam[1] = fUp + fGround;
            }
            if (pView->script.n110 != 0) {
                CamScript_KeepAboveGround(nPlayer, pCam, vOld, 1, NULL, &fAbove, NULL,
                                          0.2f + lbl_80281F78->f168);
            }
            pSub[0] = gPlayers[nPlayer].vPlacement[0];
            pSub[2] = gPlayers[nPlayer].vPlacement[2];
            fGround = gPlayers[nPlayer].vPlacement[1];
            if (fGround < -60000.0f) {
                fGround = 0.0f;
            }
            if (pView->script.n110 != 0) {
                fDiff = fGround;
                fDiff -= pSub[1];
                pSub[1] += (1.0f - lbl_80281F78->f98) * fDiff;
                if (pCam[1] - pSub[1] > 5.0f) {
                    pSub[1] = pCam[1] - 5.0f;
                }
            } else {
                pSub[1] = fGround;
            }
            if (pView->script.n110 == 0) {
                pView->script.n110 = 1;
            }
        }
    }
}

// Camera 9, speed golf's run to the ball (set by GameMode8.c): as camera 8's init, a 60-degree lens
// and no shot or sequence.
void GolfCamera_InitSpeedGolfRunCamera(View* pView, int nPlayer) {
    CameraController_GetCameraOrigin(pView);
    CameraController_GetCameraLookPoint(pView);
    CA_vSetCameraFieldOfView(Camera_GetLens(ViewController_GetRenderContext(gPlayers[nPlayer].nView[0])),
                             DEG(60.0f));
    pView->script.n110 = 0;
    pView->script.pShot = NULL;
    pView->p74 = NULL;
}

// Speed golf's first-person run camera's state (camera 9), per player, and two axes
// GolfCamera_ZoomCamGetStartAndEndVecs uses. Defined here, after the functions that use the file's
// string literals, so the .data comes out in the original's order.
f32 gSpeedGolfRunCamBob[5] = {0};                               // the step's bob, 0..16
f32 gSpeedGolfRunCamSway[5] = {8.0f, 8.0f, 8.0f, 8.0f};         // the sideways sway, 0..16
f32 gSpeedGolfRunCamEyeHeight[5] = {1.0f, 1.0f, 1.0f, 1.0f};    // eye height blend: 1 standing
                                                                //   (1.4 up), 0 in water (0.1 up)
s32 gSpeedGolfRunCamSwaySide[5] = {0};                          // which side the sway is on
s32 gSpeedGolfRunCamRumble[5] = {0};    // the step rumble: -1 armed for the next step, else
                                        // frames since it started (it stops at 2)
f32 gGolfCamAxisX[4] = {1.0f, 0.0f, 0.0f, 0.0f};                // the x axis
f32 gGolfCamAxisZ[4] = {0.0f, 0.0f, 1.0f, 0.0f};                // the z axis

// Camera 9's tick, speed golf's run to the ball in first person: the eye at vPlacement (the running
// golfer), 1.4 over it (0.1 in water, blending by 0.1 a frame), bobbing and swaying in steps as
// long as three times vCBC's x and z, with a rumble on each step for a human player. It looks ahead
// along fPlaceHeading, up or down with the pad's stick (fA8C). The colour fade steps on while the
// game is not paused.
void GolfCamera_ProcessSpeedGolfRunCamera(View* pView, int nPlayer) {
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
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    if (Ter_GetTGD() != NULL) {
        Vec3Copy(pCam, vOld);
        Game_CurrentPinSet();
        if (gSession.nPaused == 0) {
            CamScript_Fade(&pView->script, gSession.fFrameTime);
            pView->script.fFadeTime += gSession.fFrameTime;
        }
        if (gSession.fFrameTime != 0.0f) {
            // 20 degrees, written out: DEG(20.0f) rounds one bit lower than the original's constant
            fUp = fDist * Math_Sin(20.0f * PI / 180.0f);    // camera 8's height over the golfer; unused
            fBack = fDist * Math_Cos(20.0f * PI / 180.0f);
            fSin = Math_Sin(gPlayers[nPlayer].fPlaceHeading);
            fCos = Math_Cos(gPlayers[nPlayer].fPlaceHeading);
            fX = fBack * -fSin;
            fZ = fBack * fCos;
            pCam[0] = gPlayers[nPlayer].vPlacement[0];
            pCam[2] = gPlayers[nPlayer].vPlacement[2];
            pCam[1] = (1.4f + gPlayers[nPlayer].vPlacement[1]) * gSpeedGolfRunCamEyeHeight[nPlayer]
                      + (0.1f + gPlayers[nPlayer].vPlacement[1])
                            * (1.0f - gSpeedGolfRunCamEyeHeight[nPlayer]);
            CamScript_KeepAboveGround(nPlayer, pCam, vOld, 1, NULL, &fAbove, NULL, lbl_80281F78->f168);
            pSurface = Ter_GetSupportingWorldMaterial(gPlayers[nPlayer].ball.pCourse, pCam);
            if (pSurface != NULL) {
                nClass = pSurface->nClass;
            } else {
                nClass = 0;
            }
            // down to the water line in water (classes 7 and 16), back up out of it
            if (nClass == 7 || nClass == 16) {
                gSpeedGolfRunCamEyeHeight[nPlayer] -= 0.1f;
            } else {
                gSpeedGolfRunCamEyeHeight[nPlayer] += 0.1f;
            }
            if (gSpeedGolfRunCamEyeHeight[nPlayer] > 1.0f) {
                gSpeedGolfRunCamEyeHeight[nPlayer] = 1.0f;
            } else if (gSpeedGolfRunCamEyeHeight[nPlayer] < 0.0f) {
                gSpeedGolfRunCamEyeHeight[nPlayer] = 0.0f;
            }
            if (gSpeedGolfRunCamRumble[nPlayer] >= 0) {
                gSpeedGolfRunCamRumble[nPlayer]++;
            }
            if ((gSpeedGolfRunCamRumble[nPlayer] >= 2 || gSpeedGolfRunCamRumble[nPlayer] < 0)
                && !Player_IsCPU(nPlayer)) {
                Input_vVibrateBuzz(gPlayers[nPlayer].nController, 0);
            }
            if (nClass != 7) {
                // the step: the length of three times vCBC's x and z, per 60th of a second
                fStep = (f32)Math_Sqrt((f32)(pow(3.0f * gPlayers[nPlayer].vCBC[2], 2.0)
                                                + pow(3.0f * gPlayers[nPlayer].vCBC[0], 2.0)))
                        / (FRAME_RATE / 60.0f);
                gSpeedGolfRunCamBob[nPlayer] += fStep;
                gSpeedGolfRunCamSway[nPlayer] += fStep;
                if (gSpeedGolfRunCamBob[nPlayer] > 16.0f) {
                    gSpeedGolfRunCamBob[nPlayer] -= 16.0f;
                }
                if (gSpeedGolfRunCamBob[nPlayer] < 8.0f) {
                    pCam[1] += gSpeedGolfRunCamBob[nPlayer] / 16.0f;
                    if (gSpeedGolfRunCamBob[nPlayer] < 3.0f && !Player_IsCPU(nPlayer)) {
                        gSpeedGolfRunCamRumble[nPlayer] = -1;
                    }
                } else {
                    pCam[1] += 0.5f - (gSpeedGolfRunCamBob[nPlayer] - 8.0f) / 16.0f;
                    if (gSpeedGolfRunCamBob[nPlayer] > 14.0f && fStep > 0.1f
                        && gSpeedGolfRunCamRumble[nPlayer] == -1 && !Player_IsCPU(nPlayer)) {
                        Input_vVibrateBuzz(gPlayers[nPlayer].nController, 1);
                        gSpeedGolfRunCamRumble[nPlayer] = 0;
                    }
                }
                if (gSpeedGolfRunCamSway[nPlayer] > 16.0f) {
                    gSpeedGolfRunCamSway[nPlayer] -= 16.0f;
                    gSpeedGolfRunCamSwaySide[nPlayer] ^= 1;
                }
                if (gSpeedGolfRunCamSwaySide[nPlayer] == 0) {
                    if (gSpeedGolfRunCamSway[nPlayer] < 8.0f) {
                        fSway = 0.7f * (gSpeedGolfRunCamSway[nPlayer] / 16.0f);
                    } else {
                        fSway = 0.7f * (0.5f - (gSpeedGolfRunCamSway[nPlayer] - 8.0f) / 16.0f);
                    }
                } else {
                    if (gSpeedGolfRunCamSway[nPlayer] < 8.0f) {
                        fSway = 0.7f * -(gSpeedGolfRunCamSway[nPlayer] / 16.0f);
                    } else {
                        fSway = 0.7f * -(0.5f - (gSpeedGolfRunCamSway[nPlayer] - 8.0f) / 16.0f);
                    }
                }
                pCam[0] += fSway * fCos;
                pCam[2] += fSway * fSin;
            }
            if (pView->script.n110 != 0) {
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
            if (pView->script.n110 != 0) {
                if (fY - pSub[1] < -5.0f) {
                    pSub[1] = 5.0f + fY;
                }
                pSub[1] = fY - lbl_80281F78->f98 * (fY - pSub[1]);
            } else {
                pSub[1] = fY;
            }
            // the height just worked out is replaced: the stick tilts the view up and down
            pSub[1] = (4.0f * gPlayers[nPlayer].fA8C + pCam[1]) * gSpeedGolfRunCamEyeHeight[nPlayer]
                      + (2.0f + pCam[1] + gPlayers[nPlayer].fA8C)
                            * (1.0f - gSpeedGolfRunCamEyeHeight[nPlayer]);
            pSub[0] = pCam[0] - fX / 3.0f;
            pSub[2] = pCam[2] - fZ / 3.0f;
            if (pView->script.n110 == 0) {
                pView->script.n110 = 1;
            }
        }
    }
}

// Camera 4, the green camera (STATEFUNC_GreenInit and GreenMorphInit): the ball and the pin go into
// the script (script.v0, v10) as the two ends it turns between, the current camera and a point 1
// along its look are kept (v30, v40) to move in from, and the lens goes to 30 degrees. The turn
// (fCamTime) starts at 2 with button 0x30 held, else at 0; f50..f58 back to 1.
void GolfCamera_InitGreenCamera(View* pView, int nPlayer) {
    f32 v[4];
    f32* pCam;
    f32* pSub;
    CourseInfo* pCourse;
    int nPinSet;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    pCourse = Ter_GetTGD();
    if (pCourse != NULL) {
        pView->f50 = 1.0f;
        pView->f54 = 1.0f;
        pView->f58 = 1.0f;
        if (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0x30, 1)) {
            pView->script.fCamTime = 2.0f;
        } else {
            pView->script.fCamTime = 0.0f;
        }
        nPinSet = Game_CurrentPinSet();
        LLMath_CopyVec(pCam, pView->v30);
        GolfCam_Vec3Sub(pSub, pCam, v);
        if (v[0] != 0.0f || v[1] != 0.0f || v[2] != 0.0f) {
            LLMath_Normalize3(v, v);
        }
        GolfCam_Vec3Add(pCam, v, v);
        LLMath_CopyVec(v, pView->v40);
        LLMath_CopyVec(gPlayers[nPlayer].ball.vPos, pView->script.v0);
        LLMath_CopyVec(&pCourse->pin[nPinSet].x, pView->script.v10);
        pView->shot19C.f6C = 1.0f;
        pView->shot19C.f68 = gPlayers[nPlayer].ball.vPos[1];
        pView->script.pShot = NULL;
        pView->script.pNextShot = NULL;
        CA_vSetCameraFieldOfView(Camera_GetLens(ViewController_GetRenderContext(gPlayers[nPlayer].nView[0])),
                                 DEG(30.0f));
    }
}

// Camera 4's tick. While shot19C.f6C is under 1 the camera moves in from where it started (v30/v40
// to v0/v10) while button 0x2E or 0x30 is held, and back out without. In place, those buttons
// grow f54 and buttons 0x31/0x32 turn the camera round between the ball and the pin (fCamTime is
// its angle, -2..2). The camera stays above the ground and over the ball.
void GolfCamera_ProcessGreenCamera(View* pView, int nPlayer) {
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
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    Vec3Copy(pCam, vOld);
    if (pView->shot19C.f6C < 1.0f) {
        pView->f54 = 1.001f;
        if ((Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0x2E, 1))
            || (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0x30, 1))) {
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
        if ((Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0x2E, 1))
            || (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0x30, 1))) {
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
    if (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0x31, 1)) {
        pView->script.fCamTime += lbl_80281F78->f218 * gSession.fFrameTime;
        if (pView->script.fCamTime >= 2.0f) {
            pView->script.fCamTime -= 4.0f;
        }
    } else if (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0x32, 1)) {
        pView->script.fCamTime -= lbl_80281F78->f218 * gSession.fFrameTime;
        if (pView->script.fCamTime < -2.0f) {
            pView->script.fCamTime = 4.0f + pView->script.fCamTime;
        }
    }
    if (fabsf(pView->script.fCamTime) > 0.5f && fabsf(pView->script.fCamTime) <= 1.5f) {
        // to one side: swing both the camera and its aim about the ball
        if (pView->script.fCamTime > 0.0f) {
            fT = -lbl_80281F78->f21C;
        } else {
            fT = lbl_80281F78->f21C;
        }
        CamUtils_vGetPositionBetweenTwoPoints(pView->script.v0, pView->script.v10, 1, 1, pCam,
                                              fabsf(pView->script.fCamTime) - 0.5f,
                    fT);
        CamUtils_vGetPositionBetweenTwoPoints(pView->script.v0, pView->script.v10, 1, 1, pSub,
                                              fabsf(pView->script.fCamTime) - 0.5f,
                    0.0f);
    } else {
        // behind the ball (looking at the pin) or behind the pin (looking at the ball)
        if (fabsf(pView->script.fCamTime) >= 1.5f) {
            pFrom = pView->script.v0;
            pTo = pView->script.v10;
            fT = 2.5f + pView->script.fCamTime;
            if (fT > 1.0f) {
                fT -= 4.0f;
            }
        } else {
            pFrom = pView->script.v10;
            pTo = pView->script.v0;
            fT = 0.5f + pView->script.fCamTime;
        }
        CamUtils_vGetPositionBetweenTwoPoints(pFrom, pTo, 1, 1, vA, 1.0f, -lbl_80281F78->f21C);
        CamUtils_vGetPositionBetweenTwoPoints(pFrom, pTo, 1, 1, vB, 1.0f, lbl_80281F78->f21C);
        CamUtils_vCalcArcPosition(vA, vB, pTo, 2, pCam, fT);
        LLMath_CopyVec(pTo, pSub);
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
        CamUtils_vGetPositionBetweenTwoPoints(pView->v30, pView->v0, 1, 1, pCam, pView->shot19C.f6C, 0.0f);
        CamUtils_vGetPositionBetweenTwoPoints(pView->v40, pView->v10, 1, 1, pSub, pView->shot19C.f6C, 0.0f);
    }
    if (pView->shot19C.f6C >= 1.0f) {
        pView->f5C = 0.0f;
        pView->f60 = pCam[1] - pView->shot19C.f68;
        pView->f64 = 0.0f;
    }
}

// Camera 5, the putt preview camera (STATEFUNC_GreenWatchRollInit): look at the pin through a
// 30-degree lens, with no shot and the clock (fCamTime) at 0.
void GolfCamera_InitGreenRollCamera(View* pView, int nPlayer) {
    void* pSub;
    CourseInfo* pCourse;
    CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    pCourse = Ter_GetTGD();
    if (pCourse != NULL) {
        int nPinSet = Game_CurrentPinSet();
        Vec3Copy(&pCourse->pin[nPinSet].x, pSub);
        pView->script.pShot = NULL;
        pView->script.pNextShot = NULL;
        CA_vSetCameraFieldOfView(Camera_GetLens(ViewController_GetRenderContext(gPlayers[nPlayer].nView[0])),
                                 DEG(30.0f));
        pView->script.fCamTime = 0.0f;
    }
}

// Camera 5's tick, the putt preview: follow the ghost ball (ballBefore) at the tuning's f4C over
// it, starting out behind it on the side away from the pin and closing in (by the square of the
// time left) over the tuning's f54 seconds from f50 out, both scaled down when the real ball is
// less than f50 from the pin; it keeps looking at the pin. The colour fade and the clock step on.
void GolfCamera_ProcessGreenRollCamera(View* pView, int nPlayer) {
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
    pCam = CameraController_GetCameraOrigin(pView);
    CameraController_GetCameraLookPoint(pView);
    pCourse = Ter_GetTGD();
    if (pCourse != NULL) {
        Vec3Copy(pCam, vOld);
        nPinSet = Game_CurrentPinSet();
        GolfCam_Vec3Sub(&pCourse->pin[nPinSet].x, gPlayers[nPlayer].vBall, vToPin);
        vToPin[1] = 0.0f;
        fDist = Math_Sqrt(Vec3_LengthSqClamped(vToPin));
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
        if (pView->script.fCamTime < fTime) {
            GolfCam_Vec3Sub(gPlayers[nPlayer].ballBefore.vPos, &pCourse->pin[nPinSet].x, vDir);
            vDir[1] = 0.0f;
            if (vDir[0] != 0.0f || vDir[1] != 0.0f || vDir[2] != 0.0f) {
                LLMath_Normalize3(vDir, vDir);
            }
            t = 1.0f - pView->script.fCamTime / fTime;
            Vec3_Scale(fOut * (t * t), vDir, vDir);
            GolfCam_Vec3Add(vDir, gPlayers[nPlayer].ballBefore.vPos, pCam);
        } else {
            Vec3Copy(gPlayers[nPlayer].ballBefore.vPos, pCam);
        }
        pCam[1] = lbl_80281F78->f4C + gPlayers[nPlayer].ballBefore.vPos[1];
        CamScript_Fade(&pView->script, gSession.fFrameTime);
        pView->script.fFadeTime += gSession.fFrameTime;
        pView->script.fCamTime += gSession.fFrameTime;
        CamScript_KeepAboveGround(nPlayer, pCam, vOld, 1, NULL, NULL, NULL, lbl_80281F78->f168);
    }
}

// Camera 6, the reverse putt view (STATEFUNC_GreenReversePuttInit, while its button is held): a cut
// to a shot of kind 0x3A.
void GolfCamera_InitReversePuttCamera(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    CamShot* pShot;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    pShot = DynamicCam_ChooseScript(nPlayer, 0x3A, NULL);
    if (pShot != NULL) {
        CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f, 0x19,
                                       0.0f);
    }
}

// Camera 6's tick (the reverse putt view): the script's per-frame update.
void GolfCamera_ProcessReversePuttCamera(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    CamScript_RunScript(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
}

// Camera 7, the knee cam (STATEFUNC_KneeCamInit): a hand-made shot (shot19C) with the current
// shot's lens (f78) and bAA, queued as the next shot with a kind-1 blend of 0.55 s.
void GolfCamera_InitKneeCamera(View* pView, int nPlayer) {
    CameraController_GetCameraOrigin(pView);
    CameraController_GetCameraLookPoint(pView);
    pView->shot19C.p40 = NULL;
    pView->shot19C.f60 = 0.0f;
    pView->shot19C.f64 = 0.0f;
    pView->shot19C.f68 = 0.5f;
    pView->shot19C.f6C = 0.5f;
    pView->shot19C.f70 = 0.0f;
    pView->shot19C.f74 = 0.0f;
    pView->shot19C.f78 = pView->script.pShot->f78;
    pView->shot19C.f7C = pView->shot19C.f78;
    pView->shot19C.bAA = pView->script.pShot->bAA;
    pView->shot19C.f84 = 0.0f;
    pView->shot19C.bA8 = 1;
    pView->shot19C.bAB = 1;
    pView->shot19C.bAC = 9;
    pView->shot19C.bAF = 1;
    pView->shot19C.bB0 = 9;
    pView->shot19C.bB1 = 2;
    pView->shot19C.bB2 = 0;
    pView->script.pNextShot = &pView->shot19C;
    pView->script.fCamTime = 0.0f;
    pView->script.nBC = 1;
    pView->script.f8C = 0.55f;
}

// Camera 7's tick (the knee cam): the script's per-frame update.
void GolfCamera_ProcessKneeCamera(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    CamScript_RunScript(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
}

// Camera 10, the hole fly-by (STATEFUNC_InitialFlyByInit and MidHoleFlyByInit): the script starts
// on the first shot of fly-by route gGolfCamState->n60 (StaticCam_GetFlyByCam), its follow-on
// queued; with none, no shot.
void GolfCamera_InitFlyByCamera(View* pView, int nPlayer) {
    CamShot* pShot;
    s32 n;
    n = gGolfCamState->n60;  // fake match: read before the store below, in the original's order
    gGolfCamState->f68 = 0.0f;
    pShot = StaticCam_GetFlyByCam(n);
    if (pShot != NULL) {
        pView->script.pShot = pShot;
        pView->script.pNextShot = pShot->p40;
        pView->script.f8C = pView->script.pShot->f48;
        pView->script.nBC = pView->script.pShot->bAB;
        pView->script.fCamTime = 0.0f;
        pView->script.fA4 = 0.0f;
        pView->script.fA0 = 0.0f;
    } else {
        pView->script.pShot = NULL;
        pView->script.fCamTime = 0.0f;
    }
    pView->script.bCF = 0;
}

// Camera 10: run the fly-by script; when the next shot is the last of its group (no successor, or
// one with another nA4), no colour fade is running and less than
// lbl_80281F78->f178 of this shot is left, start a colour fade (script.nFade 1) to the tuning's
// v17C over the time left (fFadeLength).
void GolfCamera_ProcessFlyByCamera(View* pView, int nPlayer) {
    f32 v[4];
    void* pCam;
    void* pSub;
    f32 fLead;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    fLead = lbl_80281F78->f178;
    LLMath_CopyVec(lbl_80281F78->v17C, v);
    CamScript_RunFlybyCamera(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
    if (pView->script.pNextShot != NULL
        && (pView->script.pNextShot->p40 == NULL
            || pView->script.pNextShot->nA4 != pView->script.pNextShot->p40->nA4)
        && pView->script.nFade == 0 && pView->script.f8C - pView->script.fCamTime < fLead) {
        pView->script.nFade = 1;
        LLMath_CopyVec(v, pView->script.vFadeColor);
        pView->script.fFadeTime = 0.0f;
        pView->script.fFadeLength = pView->script.f8C - pView->script.fCamTime;
    }
}

// Camera 11, the pre-shot camera: a static camera of kind 0x20, or 49 times in 100 (or without one)
// shot 13 of DynamicCam_ChoosePairedSequenceOrCamera's sequence or of a new pre-flight sequence.
void GolfCamera_InitPreShotCamera(View* pView, int nPlayer) {
    int nLie;
    CamShot* pShot = NULL;
    int nA = 5;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32* pCam = CameraController_GetCameraOrigin(pView);
    f32* pSub = CameraController_GetCameraLookPoint(pView);
    int nB = 0x19;
    f32 f3 = 0.0f;
    gGolfCamState->f68 = 0.0f;
    pView->script.bCF = 0;
    nLie = gPlayers[nPlayer].ball.nLie;
    pView->p74 = NULL;
    pShot = StaticCam_ChooseScript(nPlayer, 0x20, 0, pView->script.pShot);
    if (pShot == NULL || Misc_RandFunc(1) % 100 > 50) {
        if (DynamicCam_ChoosePairedSequenceOrCamera(nPlayer, 0, &pView->p74, &pShot) && pView->p74 != NULL) {
            pShot = DynamicCam_ChooseScriptInSequence(pView->p74, 13, &nA, &f1, &f2, &nB, &f3, nPlayer);
        }
        if (pShot == NULL) {
            pView->p74 = DynamicCam_ChoosePreFlightSequence(nPlayer, nLie, 1);
            pShot = DynamicCam_ChooseScriptInSequence(pView->p74, 13, &nA, &f1, &f2, &nB, &f3, nPlayer);
        }
        pView->script.nC8 = 13;
        pView->script.nC4 = 13;
    }
    if (pShot != NULL) {
        CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, nA, f1, f2, nB, f3);
        if (pView->script.nFade != 0) {
            pView->script.fFadeTime = 0.0f;
            pView->script.nFade = 2;
        }
        pView->script.n110 = 0;
        pView->script.n114 = 0;
    }
}

// Camera 11's process: when the shot kind asked for (script.nC4) changes, start it; the request is
// dropped when the golfer's animation has less than a second left and the camera has not cut yet
// (script.n110, which kind 0x17 sets).
void GolfCamera_ProcessPreShotCamera(View* pView, int nPlayer) {
    f32* pCam;
    f32* pSub;
    u8 bStart;
    f32 f1;
    f32 f2;
    int nB;
    f32 f3;
    int nA;
    CamShot* pShot;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    nB = 0x19;
    bStart = 1;
    f1 = 0.0f;
    f2 = 0.0f;
    f3 = 0.0f;
    nA = 5;
    if (pView->script.nC4 != pView->script.nC8) {
        if (pView->script.nC4 == 0x17) {
            pView->script.n110 = 1;
            if (pView->script.nFade != 0) {
                pView->script.fFadeTime = 0.0f;
                pView->script.nFade = 2;
            }
        }
        if (pView->script.n110 < 1 && fn_80062C28(gPlayers[nPlayer].pChar) < 1.0f) {
            bStart = 0;
        }
        pView->script.nC8 = pView->script.nC4;
        if (bStart) {
            pShot = DynamicCam_ChooseScriptInSequence(pView->p74, pView->script.nC4, &nA, &f1, &f2, &nB, &f3, nPlayer);
            if (pShot != NULL) {
                CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, nA, f1, f2, nB,
                                               f3);
            }
        }
    }
    CamScript_RunScript(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
}

// Camera 12, the swing camera. The special swing is cleared (View.n260 0, heartbeat and shutter
// off, the comic camera off) and the pre-flight sequence picked: outside a replay the one already
// saved (p7C), else the follow-on (type 3) of a type-1 or type-2 sequence, or a new one for the
// lie; outside a replay the pick is saved (p7C, p78). The shot: outside a replay the saved one
// (p80, or alternate n264), else the sequence's kind-9 shot, which a human's big height difference
// to the target (over 10 up or down: alternates 4 and 2) or, with fn_8012022C, lies 3..5 without
// club 2 (alternate 5) can replace. A shot that would hide the golfer is swapped for one from
// sequence 0x1C, split screen skips to the chain's last shot, and golfer animation 11 takes the
// paired sequence or camera. It starts as a cut unless the view comes from camera 0 with a sequence
// that allows a blend (b47 0).
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
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    pOldSeq = pView->p74;
    pView->script.bCF = 0;
    gGolfCamState->f68 = 0.0f;
    gGolfCamState->b5D = 0;
    GolfCamera_TurnOffComicCam(pView, nPlayer);
    pView->n260 = 0;
    gGolfCamState->b5A = 0;
    gGolfCamState->b5B = 0;
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
            pShot = GolfCamera_GetAlternateSwingCamera(pView->n264, nPlayer);
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
        pShot = DynamicCam_ChooseScriptInSequence(pView->p74, 9, &nA, &f1, &f2, &nB, &f3, nPlayer);
        if (!gSession.bReplay) {
            pView->n264 = 0;
            pView->p80 = pShot;
        }
        if (!Player_IsCPU(nPlayer)) {
            f32 fRise = gPlayers[nPlayer].vTarget[1] - gPlayers[nPlayer].vBall[1];
            if (fRise > 10.0f) {
                pShot = GolfCamera_GetAlternateSwingCamera(4, nPlayer);
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
                pShot = GolfCamera_GetAlternateSwingCamera(2, nPlayer);
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
            pShot = GolfCamera_GetAlternateSwingCamera(5, nPlayer);
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
                pClear = DynamicCam_ChooseScriptInSequence(pSeq, 9, &nA, &f1, &f2, &nB, &f3, nPlayer);
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
        if (fn_80095780(gPlayers[nPlayer].pChar) == 11 && DynamicCam_ChoosePairedSequenceOrCamera(nPlayer, 0, &pAltSeq, &pAltShot)) {
            if (pAltSeq != NULL) {
                pView->p74 = pAltSeq;
                pShot = DynamicCam_ChooseScriptInSequence(pAltSeq, 9, &nA, &f1, &f2, &nB, &f3, nPlayer);
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
            pView->script.pNextShot = NULL;
        }
        pView->script.n110 = 0;
        if (pView->script.nFade == 4) {
            pView->script.nFade = 2;
            pView->script.fFadeTime = 0.0f;
        }
    }
}

// Camera 12's process: shots of kind 6, 8, 9 and 10 have no next shot while the script runs; in super
// slow motion the script steps one fixed frame (FRAME_TIME); on the downswing kinds 9 and 10 give way
// to 0 and 2.
void GolfCamera_ProcessSwingCamera(View* pView, int nPlayer) {
    f32* pCam;
    f32* pSub;
    f32 fTime;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    fTime = gSession.fFrameTime;
    if (pView->script.nBC == 6 || pView->script.nBC == 8 || pView->script.nBC == 9
        || pView->script.nBC == 10) {
        pView->script.pNextShot = NULL;
    }
    if (GameEffects_IsSlowDownSwingOn(nPlayer) && gSession.nPaused == 0) {
        fTime = FRAME_TIME;
    }
    CamScript_RunScript(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, fTime);
    if (pView->script.nBC == 6 || pView->script.nBC == 8 || pView->script.nBC == 9
        || pView->script.nBC == 10) {
        pView->script.pNextShot = pView->script.pShot->p40;
    }
    if (gPlayers[nPlayer].swing.nState == SW_DOWN_SWING) {
        if (pView->script.nBC == 9) {
            pView->script.nBC = 0;
            pView->script.fCamTime = 0.0f;
            pView->script.bCC = 1;
        } else if (pView->script.nBC == 10) {
            pView->script.nBC = 2;
            pView->script.fCamTime = 0.0f;
            pView->script.bCC = 1;
        }
    }
}

// Camera 13, the slow-motion swing replay (GS_REPLAY_SWING): the replay's counters (script.n110,
// n114) and clock to 0, then the replay camera set up from the view's position and aim.
void GolfCamera_InitReplaySwingCamera(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    pView->script.n110 = 0;
    pView->script.n114 = 0;
    pView->script.fCamTime = 0.0f;
    gGolfCamState->f68 = 0.0f;
    GolfCamera_CreateReplayCamera(pView, pCam, pSub, nPlayer);
}

// Camera 13's tick. While the slow-motion swing camera is on, its own tick moves the camera first;
// otherwise the tuning's colour (v68) is drawn over the view, fading out over the tuning's f78
// seconds. After the script's update, if the slow-motion camera is on and the shot changed: from a
// shot with no follow-on the script ends (blend kind 5, no next shot), else the old shot is queued
// again.
void GolfCamera_ProcessReplaySwingCamera(View* pView, int nPlayer) {
    f32 v[4];
    f32* pCam;
    f32* pSub;
    CamShot* pShot;
    s32 nNextKind;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    if (GolfCamera_IsSlowMoSwingCamActive()) {
        GolfCamera_UpdateSwingSlowMoCamera(pView, pCam, pSub, nPlayer);
    }
    LLMath_CopyVec(lbl_80281F78->v68, v);
    if (!GolfCamera_IsSlowMoSwingCamActive() && pView->script.fCamTime < lbl_80281F78->f78) {
        v[3] *= 1.0f - pView->script.fCamTime / lbl_80281F78->f78;
        fn_80038010(1, ViewController_GetCurrentViewControllerID(), v);
    }
    pShot = pView->script.pShot;
    nNextKind = pView->script.nBC;
    CamScript_RunScript(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
    if (GolfCamera_IsSlowMoSwingCamActive() && pShot != pView->script.pShot) {
        if (pShot != NULL && pShot->p40 == NULL) {
            pView->script.nBC = 5;
            pView->script.f8C = 0.0f;
            pView->script.pNextShot = NULL;
        } else {
            pView->script.pNextShot = pShot;
            pView->script.nBC = nNextKind;
        }
    }
}

// Camera 20, the 3-screen comic-book swing camera: start the comic camera (GoComicCam; layout 1 for
// special swing kind 9, else 0) and mark it on (b56).
void GolfCamera_Init3ScreenCamera(View* pView, int nPlayer) {
    if (pView->n260 == 9) {
        ComicCam_StartComicCam(1, pView, nPlayer);
    } else {
        ComicCam_StartComicCam(0, pView, nPlayer);
    }
    gGolfCamState->b56 = 1;
}

// Camera 20's tick: when the comic camera's own tick (GoComicCam) says it is done, the comic camera
// is turned off and the view goes straight on to the ball-flight camera (14), running its first
// tick; until then the script's update.
void GolfCamera_Process3ScreenCamera(View* pView, int nPlayer) {
    f32* pCam;
    f32* pSub;
    int nView;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    nView = gPlayers[nPlayer].nView[0];
    if (ComicCam_UpdateComicCam(pView, nPlayer, gSession.fFrameTime)) {
        GolfCamera_TurnOffComicCam(pView, nPlayer);
        CameraController_SetCameraMode(ViewController_GetCameraControl(nView), 14, nPlayer, nView);
        GolfCamera_ProcessBallFlightCamera(pView, nPlayer);
    } else {
        CamScript_RunScript(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
    }
}

// When the comic camera is on (b56): the view's viewport back to the whole screen (0,0-1,1), the
// render context's matrices and the render state updated, and b56 and b57 cleared. Called by the
// swing camera's init, camera 20's tick, when the ball's flight ends (STATEFUNC_SimulateExit) and
// when the special swings are stopped.
void GolfCamera_TurnOffComicCam(View* pView, int nPlayer) {
    int nView = gPlayers[nPlayer].nView[0];
    if (gGolfCamState->b56) {
        VM_vSetViewportRect(RC_spGetRenderCtxViewport(ViewController_GetRenderContext(nView)), 0.0f, 0.0f,
                            1.0f, 1.0f);
        RC_UpdateCurrentScreenMatrices();
        RC_vSetCurrentRenderCtxTransformationMatrix(NULL);
        RC_vUpdateRenderCtxTransformationMatrices(RC_spGetCurrentRenderCtx());
        RenderState_SetViewport(RC_spGetCurrentRenderCtx());
        RenderState_SetCameraMatrices();
        RenderState_Flush();
        gGolfCamState->b56 = 0;
        gGolfCamState->b57 = 0;
    }
}

// Camera 21, the heartbeat camera: a cut to a random shot of kind 0x2E (13 without one) other than
// the current one, unless it would hide the golfer; a fade in, and super slow motion slowed so the
// swing up to event 2 lasts the tuning's beats.
void GolfCamera_InitHeartBeatCamera(View* pView, int nPlayer) {
    f32* pCam = CameraController_GetCameraOrigin(pView);
    f32* pSub = CameraController_GetCameraLookPoint(pView);
    f32 v[4] = {0.0f, 0.0f, 0.0f, 0.5f};
    CamShot* pShot;
    f32 fRate;
    f32 fStart = 0.0f;      // fake match: a variable, not the literal (x - 0.0f folds away)
    pShot = DynamicCam_ChooseScript(nPlayer, 0x2E, pView->script.pShot);
    if (pShot == NULL) {
        pShot = DynamicCam_ChooseScript(nPlayer, 0xD, pView->script.pShot);
    }
    if (pShot != NULL && !CameraScript_WillGolferBeOccludedInThisView(nPlayer, pShot, &pView->script)) {
        CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f, 0x19,
                                       0.0f);
    }
    pView->script.n110 = 0;
    pView->script.f108 = 0.0f;
    gGolfCamState->b5A = 1;
    CameraController_FadeIn(pView, lbl_80281F78->fC4, v);
    fRate = (FRAME_RATE * (GolfCam_GetBlendTagTime(gPlayers[nPlayer].pChar, 2) - fStart))
          / (FRAME_RATE * (lbl_80281F78->fC4 * (lbl_80281F78->nBeatFrames * (lbl_80281F78->nBeats + 1))
                       + (lbl_80281F78->nBeats * lbl_80281F78->fC8
                          + lbl_80281F78->nBeats * lbl_80281F78->fCC)));
    GameEffects_SetSuperSlowMo(1, nPlayer, fRate);
    gGolfCamState->f64 = fRate;
}

// Camera 21: the heartbeats. While b5A is set, each beat (every nBeatFrames steps of script.n110, nBeats
// of them) cuts to a new angle, the last to the saved shot p80. The script steps one fixed frame
// (FRAME_TIME) at a time.
void GolfCamera_ProcessHeartBeatCamera(View* pView, int nPlayer) {
    f32* pCam = CameraController_GetCameraOrigin(pView);
    f32* pSub = CameraController_GetCameraLookPoint(pView);
    CamShot* pShot = NULL;
    f32 v[4] = {0.0f, 0.0f, 0.0f, 0.5f};
    if (gSession.nPaused == 0) {
        if (gGolfCamState->b5A) {
            if (pView->script.nFade == 5) {
                if (pView->script.n110 >= lbl_80281F78->nBeats * lbl_80281F78->nBeatFrames) {
                    gGolfCamState->b5A = 0;
                } else if ((pView->script.n110 + 1) % lbl_80281F78->nBeatFrames == 0) {
                    CameraController_FadeOut(pView, lbl_80281F78->fC8, v);
                } else {
                    CameraController_FadeIn(pView, lbl_80281F78->fC4, v);
                    pView->script.n110++;
                }
            } else if (pView->script.f108 > lbl_80281F78->fCC) {
                if (pView->script.n110 >= (lbl_80281F78->nBeats - 1) * lbl_80281F78->nBeatFrames) {
                    pShot = pView->p80;
                }
                if (pShot == NULL) {
                    pShot = DynamicCam_ChooseScript(nPlayer, 0x2E, pView->script.pShot);
                }
                if (pShot == NULL) {
                    pShot = DynamicCam_ChooseScript(nPlayer, 0xD, pView->script.pShot);
                }
                if (pShot != NULL
                    && !CameraScript_WillGolferBeOccludedInThisView(nPlayer, pShot, &pView->script)) {
                    CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 0.0f,
                                                   100.0f, 0x19, 0.0f);
                }
                CameraController_FadeIn(pView, lbl_80281F78->fC4, v);
                pView->script.n110++;
                pView->script.f108 = 0.0f;
            } else if (pView->script.nFade == 0 || pView->script.nFade == 4
                       || pView->script.nFade == 3) {
                pView->script.f108 += FRAME_TIME;
                CameraController_HoldFadeColor(pView, v);
            }
        }
        CamScript_RunScript(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, FRAME_TIME);
    }
}

// Camera 22, the shutter camera: a cut to a random shot of kind 0x3E (13 without one) other than
// the current one, with the game in super slow motion.
void GolfCamera_InitShutterCamera(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    CamShot* pShot;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    pShot = DynamicCam_ChooseScript(nPlayer, 0x3E, pView->script.pShot);
    if (pShot == NULL) {
        pShot = DynamicCam_ChooseScript(nPlayer, 0xD, pView->script.pShot);
    }
    if (pShot != NULL) {
        CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f, 0x19,
                                       0.0f);
    }
    pView->script.n110 = 0;
    pView->script.f108 = 0.0f;
    pView->script.f10C = 0.0f;
    gGolfCamState->b5B = 1;
    pView->script.f10C = 0.0f;     // stored twice, as in the original
    GameEffects_SetSuperSlowMo(1, nPlayer, 1.0f);
}

// Camera 22, the shutter camera: around script.f108 0 the shutter (black overlays, fn_800380A8 and
// fn_80038010, on view 0) closes and reopens over 0.15 s either side; the script steps one fixed
// frame (FRAME_TIME). At a third and at two thirds of the swing up to event 2 the shutter fires
// (script.f108 back to -0.15), and when it reaches 0 the camera cuts to a random shot of kind
// 0x3E + script.n110 other than the current one.
void GolfCamera_ProcessShutterCamera(View* pView, int nPlayer) {
    f32* pCam = CameraController_GetCameraOrigin(pView);
    f32* pSub = CameraController_GetCameraLookPoint(pView);
    f32 v[4] = {0.0f, 0.0f, 0.0f, 0.5f};
    f32 fTime;
    f32 fSwing;
    f32 fStart = 0.0f;      // fake match: a variable, not the literal (x - 0.0f folds away)
    CamShot* pShot;
    if (gSession.nPaused == 0) {
        fTime = pView->script.f108;
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
        CamScript_RunScript(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, FRAME_TIME);
        pView->script.f108 += gSession.fFrameTime;
        pView->script.f10C += gSession.fFrameTime;
        fSwing = GolfCam_GetBlendTagTime(gPlayers[nPlayer].pChar, 2) - fStart;
        if (pView->script.n110 == 0 && pView->script.f10C > fSwing / 3.0f) {
            pView->script.f108 = -0.15f;
            pView->script.n110++;
        } else if (pView->script.n110 == 1 && pView->script.f10C > 2.0f * (fSwing / 3.0f)) {
            pView->script.f108 = -0.15f;
            pView->script.n110++;
        }
        if (fTime < 0.0f && gSession.fFrameTime + fTime >= 0.0f) {
            pShot = DynamicCam_ChooseScript(nPlayer, pView->script.n110 + 0x3E, pView->script.pShot);
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
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    pView->script.bCF = 0;
    nLie = gPlayers[nPlayer].ball.nLie;
    if (gSession.bReplay && gPlayers[nPlayer].ballBefore.nState == 0) {
        // a replay whose look-ahead ball has stopped: the distance that ball went
        if (gPlayers[nPlayer].ballBefore.nSurface >= 0) {
            nClass = gSurfaceTypes[gPlayers[nPlayer].ballBefore.nSurface].nClass;
        } else {
            nClass = 10;
        }
        GolfCam_Vec3Sub(gPlayers[nPlayer].ballBefore.vPos, gPlayers[nPlayer].ball.vStart, vDiff);
        fDist = Math_Sqrt(Vec3_LengthSqClamped(vDiff));
    } else {
        fDist = AI_MaxDistance(nPlayer, gPlayers[nPlayer].nShotKind, gPlayers[nPlayer].nClub);
        fDist *= Physics_GetLiePowerPercentage(&gPlayers[nPlayer].ball);
        fDist *= SW_vGetShotPower(nPlayer);
        nClass = 2;
        GolfCam_Vec3Sub(gPlayers[nPlayer].vTarget, gPlayers[nPlayer].vBall, vAim);
        LLMath_Normalize3(vAim, vAim);
        Vec3_Scale(fDist, vAim, vAim);
        GolfCam_Vec3Add(vAim, gPlayers[nPlayer].vBall, pView->script.v50);
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
        pView->script.pShot = &pView->shot19C;
        pView->script.pNextShot = NULL;
        pView->script.fCamTime = 0.0f;
        pView->script.pShot->f98 = 0.0f;
    }
    if (pView->script.pShot != NULL && pView->script.pNextShot != NULL && pView->script.nBC == 6) {
        pView->script.nBC = 0;
        pView->script.fCamTime = 0.0f;
        pView->script.bCC = 1;
    } else if (pView->script.pShot != NULL && pView->script.pNextShot != NULL && pView->script.nBC == 8) {
        pView->script.nBC = 2;
        pView->script.fCamTime = 0.0f;
        pView->script.bCC = 1;
    } else if (pView->script.pNextShot == NULL) {
        if (GolfCamera_Choose3ScreenCam(pView, nPlayer)) {
            CameraScript_RecordCurrentCam(&pView->shot19C, pCam, pSub, nPlayer, &pView->script, 0);
            pView->shot19C.p44 = pView->script.pShot;
            pView->script.pShot = &pView->shot19C;
            pView->script.fCamTime = 0.0f;
            pView->script.pShot->f4C = -1.0f;
            pView->script.pNextShot = NULL;
        } else {
            CameraScript_RecordCurrentCam(&pView->shot19C, pCam, pSub, nPlayer, &pView->script, 0);
            pView->shot19C.p44 = pView->script.pShot;
            pView->script.pShot = &pView->shot19C;
            pView->script.fCamTime = 0.0f;
            if (pView->p74 != NULL) {
                pView->script.pShot->f4C = pView->p74->f38;
            } else {
                pView->script.pShot->f4C = 1.0f;
            }
            pView->script.pNextShot = NULL;
        }
    }
    pView->script.pB8 = NULL;
    if (GolfCamera_ChooseImpactMatrixCam(pView, nPlayer)) {
        gGolfCamState->b54 = 1;
        Character_AlignCharacterForShotImpact(gPlayers[nPlayer].pChar);
        SKATime_Pause(gPlayers[nPlayer].pChar->anim);
        GolfCamera_CreateMatrixCamera(pView, pCam, pSub, nPlayer);
        pView->script.f108 = 0.0f;
        EVENT_Trigger(nPlayer, 0x32, NULL, -1);
    } else if (GolfCamera_ChooseSuperZoomCam(pView, nPlayer)) {
        gGolfCamState->b58 = 1;
        SKATime_Pause(gPlayers[nPlayer].pChar->anim);
        GolfCamera_CreateSuperZoomCamera(pView, pCam, pSub, nPlayer);
        EVENT_Trigger(nPlayer, 0x39, NULL, -1);
    } else if (pView->n260 == 15 || pView->n260 == 16) {
        pView->script.pShot->f4C = 1.0f;
    }
    pView->script.nC4 = 0;
    pView->script.nC8 = 9;
    if (pView->script.nFade != 0 && pView->script.nFade != 2) {
        pView->script.nFade = 0;
    }
}

// The ball-flight camera's tick: when the current shot has run its time (or, once, when
// GameEffects_IsPredictedGameBreakerOn's GameBreaker comes on), pick the next one: for a CPU player
// or in a replay a static camera for the ball's state, else the sequence's latest kind-0x18 choice
// the flight has reached, else a shot of the kind asked for. Some cuts (type 5) wait for the
// golfer's animation 14, which is started first. While the matrix camera or the super zoom runs,
// its tick (GolfCamera_UpdateMatrixCamera, GolfCamera_UpdateSuperZoomCamera) gives the script's time step.
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
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    f1 = 0.0f;
    nA = 5;
    nB = 0x19;
    pShot = NULL;
    f3 = 0.0f;
    RC_spGetRenderCtxViewport(ViewController_GetRenderContext(gPlayers[nPlayer].nView[0]));
    Vec3Copy(pCam, vOld);
    CameraController_CheckForEvents(pView, nPlayer);
    if (GolfCamera_IsMatrixCamActive()) {
        fTime = GolfCamera_UpdateMatrixCamera(pView, pCam, pSub, nPlayer);
    } else if (GolfCamera_IsSuperZoomCamActive()) {
        fTime = GolfCamera_UpdateSuperZoomCamera(pView, pCam, pSub, nPlayer);
    } else {
        fTime = gSession.fFrameTime;
    }
    if (gSession.fFrameTime != 0.0f && !pView->script.bCF
        && ((!gGolfCamState->b5D && GameEffects_IsPredictedGameBreakerOn())
            || ((pView->script.pShot == NULL || pView->script.fCamTime > pView->script.pShot->f4C)
                && (pView->script.pNextShot == NULL || pView->p80 != pView->script.pShot)))) {
        if (gSession.bReplay || Player_IsCPU(nPlayer)) {
            if (gPlayers[nPlayer].nClub == 25) {
                pShot = StaticCam_ChooseScript(nPlayer, 0x10, 0, pView->script.pShot);
            } else if (gPlayers[nPlayer].ball.nState == 2) {
                if (gPlayers[nPlayer].ball.nCollideCount > 0) {
                    pShot = StaticCam_ChooseScript(nPlayer, 4, 0, pView->script.pShot);
                } else {
                    pShot = StaticCam_ChooseScript(nPlayer, 2, 0, pView->script.pShot);
                }
            } else {
                pShot = StaticCam_ChooseScript(nPlayer, 8, 0, pView->script.pShot);
            }
        }
        if (pShot == NULL) {
            // the latest kind-0x18 choice the flight has already reached
            fBest = 0.0f;
            nBest = -1;
            fNow = CamScript_EstimateBallFlightPercent(nPlayer, &pView->script);
            pSeq = pView->p74;
            if (pSeq != NULL) {
                for (i = 0; i < pSeq->nChoices; i++) {
                    if (pSeq->p4C[i].b14 == 0x18 && pSeq->p4C[i].fC < fNow && pSeq->p4C[i].fC > fBest) {
                        fBest = pSeq->p4C[i].fC;
                        nBest = i;
                    }
                }
            }
            if (nBest >= 0 && pSeq != NULL && pSeq->p4C[nBest].p10 != pView->script.pB8) {
                pShot = pSeq->p4C[nBest].p10;
                nA = pSeq->p4C[nBest].b15;
                f1 = pView->p74->p4C[nBest].f0;
                f2 = pView->p74->p4C[nBest].f4;
                nB = pView->p74->p4C[nBest].b16;
                f3 = pView->p74->p4C[nBest].f8;
            } else {
                pShot = DynamicCam_ChooseScriptInSequence(pSeq, pView->script.nC4, &nA, &f1, &f2, &nB, &f3, nPlayer);
            }
            if (pShot == NULL && pView->script.nC8 == 9) {
                pShot = DynamicCam_ChooseScriptInSequence(pView->p74, 0, &nA, &f1, &f2, &nB, &f3, nPlayer);
            }
        }
        if (pShot != NULL
            && (pView->script.bCF
                || (pShot != pView->script.pB8 && pView->script.nC8 != pView->script.nC4))) {
            if ((!CameraScript_IsDefaultSwingCam(pView->script.pShot, nPlayer, pCam)
                 && pView->script.pShot->bA8 == 0
                 && pView->script.pShot->bAD == 4)
                || pView->script.bCF) {
                nA = 5;
                f1 = 0.0f;
                pView->script.bCF = 0;
            }
            pView->script.pB8 = pShot;
            if (nA != 5 && pView->script.pShot != NULL && pView->script.pShot->bAD != 4) {
                if (!CameraScript_IsDefaultSwingCam(pView->script.pShot, nPlayer, pCam)) {
                    nA = 5;
                    f1 = 0.0f;
                } else {
                    f = fn_800D04AC(nPlayer);
                    if (f > 15.0f) {
                        f1 += 0.05f * (f - 15.0f);
                    }
                }
            }
            if (!CamScript_SkipLookBackCam(&pView->script, pShot, nPlayer)) {
                if (nA == 5 && fn_8003DC78(pShot) && gPlayers[nPlayer].nShotKind != 0
                    && fn_80095780(gPlayers[nPlayer].pChar) != 9) {
                    // a cut: only once the golfer is in animation 14
                    if (fn_80095780(gPlayers[nPlayer].pChar) == 14) {
                        if (pView->script.pShot->bA8 == 0
                            || (pView->script.pShot->bAF != 12 && pView->script.pShot->bB0 != 12)
                            || nA == 5) {
                            CameraScript_UpdateLandingEstimate(&pView->script, nPlayer);
                        }
                        if (f3 > fn_80062C28(gPlayers[nPlayer].pChar)) {
                            f3 = fn_80062C28(gPlayers[nPlayer].pChar);
                        }
                        if (DynamicCam_ChoosePairedSequenceOrCamera(nPlayer, 0, &pAltSeq, &pAltShot)) {
                            if (pAltShot == NULL) {
                                CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, nA,
                                                               f1, f2, nB, f3);
                                if (GameEffects_IsPredictedGameBreakerOn()) {
                                    gGolfCamState->b5D = 1;
                                }
                            } else {
                                CameraScript_InterpToNewScript(&pView->script, pAltShot, nPlayer, pCam, pSub,
                                                               nA, f1, f2, nB, f3);
                                if (GameEffects_IsPredictedGameBreakerOn()) {
                                    gGolfCamState->b5D = 1;
                                }
                            }
                        } else {
                            CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, nA, f1,
                                                           f2, nB, f3);
                            if (GameEffects_IsPredictedGameBreakerOn()) {
                                gGolfCamState->b5D = 1;
                            }
                        }
                        pView->script.nC8 = pView->script.nC4;
                    } else {
                        fn_80095744(gPlayers[nPlayer].pChar, 14);
                        pView->script.pB8 = pView->script.pShot;
                    }
                } else {
                    CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, nA, f1, f2,
                                                   nB, f3);
                    pView->script.nC8 = pView->script.nC4;
                    if (GameEffects_IsPredictedGameBreakerOn()) {
                        gGolfCamState->b5D = 1;
                    }
                }
            } else {
                pView->script.nC8 = pView->script.nC4;
            }
        }
    }
    if (pView->script.nFade == 4) {
        pView->script.nFade = 2;
        pView->script.fFadeTime = 0.0f;
    }
    CamScript_RunScript(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, fTime);
    if (GolfCamera_IsMatrixCamActive() && gSession.nPaused == 0) {
        fn_80038054(1, ViewController_GetCurrentViewControllerID(), 0.0f, lbl_80281F78->f64);
    }
}

// Camera 15, the post-shot camera: with b269 set (GolfCamera_ShowPostShotAnimations) the
// GolfCamera_ChooseReactionCam shot; else the crowd flyby, else a static camera of kind 0x40 or
// shot 5 of the sequence. For a ball in sand (lies 6..8) or kind 10 asked for, only once fCamTime
// reaches fB8.
void GolfCamera_InitPostShotCamera(View* pView, int nPlayer) {
    CamShot* pShot = NULL;
    f32* pCam = CameraController_GetCameraOrigin(pView);
    f32* pSub = CameraController_GetCameraLookPoint(pView);
    int nA = 5;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    int nB = 0x19;
    f32 f3 = 0.0f;
    pView->script.n110 = 0;
    pView->p78 = pView->p74;
    if (pView->script.nC4 != 6 && pView->script.nC4 != 8 && pView->script.nC4 != 10) {
        CameraController_PostEvent(pView, 5, nPlayer);
    }
    pView->script.n114 = 0;
    if ((pView->script.nC4 != 10 && gPlayers[nPlayer].ball.nLie != 6 && gPlayers[nPlayer].ball.nLie != 7
         && gPlayers[nPlayer].ball.nLie != 8)
        || pView->script.fCamTime >= lbl_80281F78->fB8) {
        if (GolfCamera_ShowPostShotAnimations(pView)) {
            GolfCamera_ChooseReactionCam(pView, nPlayer, 0x40);
        } else {
            if (GM_ShowPostShotCrowdFlyby()) {
                pShot = StaticCam_GetFlyByCam(9);
            }
            if (pShot != NULL) {
                pView->script.pShot = pShot;
                pView->script.pNextShot = pShot->p40;
                pView->script.f8C = pView->script.pShot->f48;
                pView->script.nBC = pView->script.pShot->bAB;
                pView->script.fCamTime = 0.0f;
                pView->script.fA0 = 0.0f;
                pView->script.fA4 = 0.0f;
            } else {
                pShot = StaticCam_ChooseScript(nPlayer, 0x40, 1, pView->script.pShot);
                if (pShot != NULL && fn_80062C28(gPlayers[nPlayer].pChar) < 1.0f) {
                    pView->script.n114 = 1;
                }
                if (pShot == NULL || pShot->bAC == 5) {
                    pShot = DynamicCam_ChooseScriptInSequence(pView->p74, 5, &nA, &f1, &f2, &nB, &f3, nPlayer);
                    if (pShot != NULL && pShot->bAA == 0) {
                        pView->script.n114 = 1;
                    }
                }
                if (pShot != NULL) {
                    CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, nA, f1, f2, nB,
                                                   f3);
                    pView->script.bCF = 0;
                }
            }
        }
        pView->script.n110 = 1;
    }
}

// Camera 15, the post-shot camera. For a held hand-made shot (script.bCF) of a kind
// CamScript_DoesScriptTrackBall accepts, f74 eases (1 in 100 a frame) towards the camera's flat
// distance to the ball / 30, kept to 0.5..5; for kind 10 the shot's height creeps up while it is
// less than 1 over the ground at the ball. Once the golfer's animation 9 is under way, the crowd
// flyby or the cut to the golfer. When a fly-by shot (bAD 0) runs out, hold the camera as a
// hand-made shot looking along m's z axis (m built from pSub's three values).
void GolfCamera_ProcessPostShotCamera(View* pView, int nPlayer) {
    f32 m[4][4];
    f32 vDelta[4];
    f32* pCam = CameraController_GetCameraOrigin(pView);
    f32* pSub = CameraController_GetCameraLookPoint(pView);
    CamShot* pShot = NULL;
    f32 v[4] = {0.0f, 0.0f, 1.0f, 0.0f};
    f32 fDist;
    if (pView->script.n110 != 1) {
        GolfCamera_InitPostShotCamera(pView, nPlayer);
    }
    if (pView->script.bCF && CamScript_DoesScriptTrackBall(pView->script.pShot)) {
        GolfCam_Vec3Sub(pCam, gPlayers[nPlayer].ball.vPos, vDelta);
        vDelta[1] = 0.0f;
        fDist = (f32)Math_Sqrt(Vec3_LengthSqClamped(vDelta)) / 30.0f;
        fDist = (fDist < 0.5f) ? 0.5f : ((fDist > 5.0f) ? 5.0f : fDist);
        fDist = 0.01f * (fDist - pView->script.pShot->f74);     // one local for the target and the step
        pView->script.pShot->f74 += fDist;
    } else if (pView->script.nC4 == 10) {
        if (pView->script.pShot->v20[1]
            - Ter_GetSupportingGroundHeight(Ter_GetTGD(), gPlayers[nPlayer].vBall) < 1.0f) {
            pView->script.pShot->v20[1] += 0.01f;
        }
    }
    if ((fn_80062C1C(gPlayers[nPlayer].pChar) || fn_80062C10(gPlayers[nPlayer].pChar))
        && fn_80095780(gPlayers[nPlayer].pChar) == 9
        && (pView->script.pShot == NULL
            || (pView->script.pShot->bAA && pView->script.pShot->bAD && nPlayer
                != CameraController_GetClippedGolfer()))) {
        if (GM_ShowPostShotCrowdFlyby()) {
            pShot = StaticCam_GetFlyByCam(9);
        }
        if (pShot != NULL) {
            pView->script.pShot = pShot;
            pView->script.pNextShot = pShot->p40;
            pView->script.f8C = pView->script.pShot->f48;
            pView->script.nBC = pView->script.pShot->bAB;
            pView->script.fCamTime = 0.0f;
            pView->script.fA4 = 0.0f;
            pView->script.fA0 = 0.0f;
        } else {
            GolfCamera_CutToGolferDoneAnimatingCam(pView, nPlayer);
            pView->script.n114 = 1;
        }
    }
    if (pView->script.pShot != NULL && pView->script.pShot->bAD == 0) {
        CamScript_RunFlybyCamera(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
        if (pView->script.pShot == NULL) {
            pView->script.fCamTime = 0.0f;
            pView->script.bCF = 0;
            mat44flt_EulerAngles(m, pSub[1], pSub[0], pSub[2]);
            LLMath_mat44fltMultiply33(m, v, pSub);
            GolfCam_Vec3Add(pSub, pCam, pSub);
            CameraScript_RecordCurrentCam(&pView->shot19C, pCam, pSub, nPlayer, &pView->script, 0);
            pView->script.pShot = &pView->shot19C;
            pView->script.pShot->bAD = 5;
            pView->script.pNextShot = NULL;
            pView->script.fCamTime = 0.0f;
            pView->script.nE0 = 0x19;
            pView->script.bCF = 1;
            pView->script.pShot->f94 = 0.0f;
            pView->script.pShot->f98 = 0.0f;
            pView->script.pShot->bAA = 0;
        }
    } else {
        CamScript_RunScript(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
    }
}

// Camera 16, the ball-in-the-hole camera: the sequence is kept (p78) and shot kind 8 asked for
// (fn_80063CF0). Once the golfer's plan is ready (bPlanReady 1): the reaction camera when the
// post-shot animations show, else after 2 s of the current shot the cut to the golfer, else it
// tries again on the next tick (script.n110 0: the process calls this until it has run). Before
// that, the reaction camera only with the animations shown or the golfer taking the ball out
// (GS_REMOVE_BALL).
void GolfCamera_InitInHoleCamera(View* pView, int nPlayer) {
    CameraController_GetCameraOrigin(pView);
    CameraController_GetCameraLookPoint(pView);
    GOLFERSTATE_GetCurrentState(nPlayer);  // the result is unused, as in the original
    pView->p78 = pView->p74;
    pView->script.n110 = 0;        // 0 and then 1, as in the original
    pView->script.n110 = 1;
    CameraController_PostEvent(pView, 8, nPlayer);
    if (gPlayers[nPlayer].bPlanReady == 1) {
        if (GolfCamera_ShowPostShotAnimations(pView)) {
            GolfCamera_ChooseReactionCam(pView, nPlayer, 1);
            pView->script.n114 = 0;
        } else if (pView->script.fCamTime > 2.0f) {
            GolfCamera_CutToGolferDoneAnimatingCam(pView, nPlayer);
        } else {
            pView->script.n110 = 0;
        }
    } else {
        // The state is compared as a signed byte (extsb), as if GOLFERSTATE_GetCurrentState returned s8.
        if (GolfCamera_ShowPostShotAnimations(pView) || (s8)GOLFERSTATE_GetCurrentState(nPlayer) == GS_REMOVE_BALL) {
            GolfCamera_ChooseReactionCam(pView, nPlayer, 1);
        }
        pView->script.n114 = 0;
    }
}

// Camera 16: once the golfer's animation 9 or 12 is under way, cut to the golfer (once).
void GolfCamera_ProcessInHoleCamera(View* pView, int nPlayer) {
    f32* pCam;
    f32* pSub;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    if (pView->script.n110 != 1) {
        GolfCamera_InitInHoleCamera(pView, nPlayer);
    }
    if ((fn_80062C1C(gPlayers[nPlayer].pChar) || fn_80062C10(gPlayers[nPlayer].pChar))
        && (fn_80095780(gPlayers[nPlayer].pChar) == 9
            || fn_80095780(gPlayers[nPlayer].pChar) == 12)
        && (pView->script.pShot == NULL || pView->script.pShot->bAA) && pView->script.n114 < 1) {
        GolfCamera_CutToGolferDoneAnimatingCam(pView, nPlayer);
        pView->script.n114 = 1;
    }
    CamScript_RunScript(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
}

// Camera 17, the scorecard camera (the hole or the game is over: GameMode.c): fade in, over the
// tuning's f170, when a colour fade is running or held (fn_80063C7C, fn_80063C90).
void GolfCamera_InitScoreCardCamera(View* pView, int nPlayer) {
    f32 v[4] = {0.0f, 0.0f, 0.0f, 0.5f};
    if (CameraController_IsFadeOutDone(pView) || CameraController_IsFadeOn(pView)) {
        CameraController_FadeIn(pView, lbl_80281F78->f170, v);
    }
}

// Camera 17's tick: the camera stays put; only the colour fade (fn_8003F2E0) steps on, one fixed
// frame (FRAME_TIME) a call, paused or not.
void GolfCamera_ProcessScoreCardCamera(View* pView, int nPlayer) {
    CamScript_Fade(&pView->script, FRAME_TIME);
    pView->script.fFadeTime += FRAME_TIME;
}

// Camera 18, the tutorial wait: the flagstick back in, and two hand-made "TUTORIAL WAIT" shots in the
// shared state, the current one (f60 15, its p40 pointing back at itself) and the next (f60 -15),
// with 40 s on the current one.
void GolfCamera_InitTutorialWaitCamera(View* pView, int nPlayer) {
    f32 v[4] = {0.0f, 0.0f, 0.0f, 0.5f};
    char szName[] = "TUTORIAL WAIT";
    if (CameraController_IsFadeOutDone(pView)) {
        CameraController_FadeIn(pView, lbl_80281F78->f170, v);
    }
    ViewController_GetIndexedViewController(gPlayers[nPlayer].nView[0])->bFlagOut = 0;
    strcpy(gGolfCamState->shot6C.szName, szName);
    gGolfCamState->shot6C.p40 = &gGolfCamState->shot6C;
    gGolfCamState->shot6C.f60 = 15.0f;
    gGolfCamState->shot6C.f64 = 0.0f;
    gGolfCamState->shot6C.f70 = 0.0f;
    gGolfCamState->shot6C.f74 = 2.0f;
    gGolfCamState->shot6C.f68 = 2.0f;
    gGolfCamState->shot6C.f6C = 20.0f;
    gGolfCamState->shot6C.f78 = 40.0f * PI / 180.0f;    // not DEG(40.0f): that rounds one bit lower
    gGolfCamState->shot6C.f7C = gGolfCamState->shot6C.f78;
    gGolfCamState->shot6C.f80 = 0.0f;
    gGolfCamState->shot6C.f9C = 0.0f;
    gGolfCamState->shot6C.f84 = 0.0f;
    gGolfCamState->shot6C.bA8 = 1;
    gGolfCamState->shot6C.bAA = 0;
    gGolfCamState->shot6C.bAC = 10;
    gGolfCamState->shot6C.bB1 = 2;
    gGolfCamState->shot6C.bB2 = 0;
    gGolfCamState->shot6C.bAF = 10;
    gGolfCamState->shot6C.bB0 = 1;
    gGolfCamState->shot6C.bAD = 5;
    gGolfCamState->shot6C.p44 = NULL;
    strcpy(gGolfCamState->shot12C.szName, szName);
    gGolfCamState->shot12C.p40 = NULL;
    gGolfCamState->shot12C.f60 = -15.0f;
    gGolfCamState->shot12C.f64 = 0.0f;
    gGolfCamState->shot12C.f70 = 0.0f;
    gGolfCamState->shot12C.f74 = 2.0f;
    gGolfCamState->shot12C.f68 = 2.0f;
    gGolfCamState->shot12C.f6C = 20.0f;
    gGolfCamState->shot12C.f78 = 40.0f * PI / 180.0f;
    gGolfCamState->shot12C.f7C = gGolfCamState->shot12C.f78;
    gGolfCamState->shot12C.f80 = 0.0f;
    gGolfCamState->shot12C.f9C = 0.0f;
    gGolfCamState->shot12C.f84 = 0.0f;
    gGolfCamState->shot12C.bA8 = 1;
    gGolfCamState->shot12C.bAA = 0;
    gGolfCamState->shot12C.bAC = 10;
    gGolfCamState->shot12C.bB1 = 2;
    gGolfCamState->shot12C.bB2 = 0;
    gGolfCamState->shot12C.bAF = 10;
    gGolfCamState->shot12C.bB0 = 1;
    gGolfCamState->shot12C.bAD = 5;
    gGolfCamState->shot12C.p44 = NULL;
    pView->script.pShot = &gGolfCamState->shot6C;
    pView->script.pNextShot = &gGolfCamState->shot12C;
    pView->script.nBC = 7;
    pView->script.f8C = 40.0f;
    pView->script.nD0 = 1;
    pView->script.fCamTime = 0.00001f;
}

// Camera 18 (the tutorial wait): with no shot to go to, fall back on the state's own two shots.
void GolfCamera_ProcessTutorialWaitCamera(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    CamScript_RunScript(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, gSession.fFrameTime);
    if (pView->script.pNextShot == NULL) {
        pView->script.pShot = &gGolfCamState->shot6C;
        pView->script.pNextShot = &gGolfCamState->shot12C;
        pView->script.nBC = 7;
        pView->script.f8C = 40.0f;
        pView->script.nD0 = 1;
        pView->script.fCamTime = 0.00001f;
    }
}

// Camera 23: from over the ball at height 1.5, looking along -x, a cut to a random shot of kind
// 0x23 (kept in p80).
void GolfCamera_InitFECamera(View* pView, int nPlayer) {
    f32* pCam;
    f32* pSub;
    CamShot* pShot;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    pView->script.pShot = NULL;
    pCam[0] = gPlayers[nPlayer].vBall[0];
    pCam[2] = gPlayers[nPlayer].vBall[2];
    pCam[1] = 1.5f;
    pSub[0] = pCam[0] - 1.0f;
    pSub[1] = pCam[1];
    pSub[2] = pCam[2];
    pShot = DynamicCam_ChooseScript(0, 0x23, NULL);
    if (pShot != NULL) {
        CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f, 0x19,
                                       0.0f);
        pView->p80 = pShot;
        pView->script.n110 = -1;
        pView->script.n114 = 0x23;
    }
}

// Camera 23's process: on the create-a-player screen, when the part being edited changes, a cut
// to a random shot for it (by page, gpCrAPState->nScreenKind), other than the last one (p80) if it
// can.
void GolfCamera_ProcessFECamera(View* pView, int nPlayer) {
    f32* pCam;
    f32* pSub;
    CamShot* pShot;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    pShot = NULL;
    // The original compares script.n114 and n0, both ints, as floats.
    if (gpCrAPState->pB4 != NULL && gpCrAPState->pB4->bLoaded
        && (pView->script.n110 != gpCrAPState->pB4->nGolferId || (f32)pView->script.n114
            != gpCrAPState->nScreenKind)) {
        switch (gpCrAPState->nScreenKind) {
        case 0:
            pShot = DynamicCam_ChooseScript(0, 0x23, pView->p80);
            if (pShot == NULL) {
                pShot = DynamicCam_ChooseScript(0, 0x23, NULL);
            }
            break;
        case 1:
            if (Character_IsLeftHanded(gpCrAPState->pB4->pChar)) {
                pShot = DynamicCam_ChooseScript(0, 0x38, pView->p80);
                if (pShot == NULL) {
                    pShot = DynamicCam_ChooseScript(0, 0x38, NULL);
                }
            } else {
                pShot = DynamicCam_ChooseScript(0, 0x24, pView->p80);
                if (pShot == NULL) {
                    pShot = DynamicCam_ChooseScript(0, 0x24, NULL);
                }
            }
            break;
        case 2:
            pShot = DynamicCam_ChooseScript(0, 0x25, pView->p80);
            if (pShot == NULL) {
                pShot = DynamicCam_ChooseScript(0, 0x25, NULL);
            }
            break;
        case 3:
            pShot = DynamicCam_ChooseScript(0, 0x2F, pView->p80);
            if (pShot == NULL) {
                pShot = DynamicCam_ChooseScript(0, 0x2F, NULL);
            }
            break;
        case 4:
            pShot = DynamicCam_ChooseScript(0, 0x3D, pView->p80);
            if (pShot == NULL) {
                pShot = DynamicCam_ChooseScript(0, 0x3D, NULL);
            }
            break;
        }
        pView->script.n114 = gpCrAPState->nScreenKind;
        if (pShot == NULL) {
            return;
        }
        CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f, 0x19,
                                       0.0f);
        pView->p80 = pShot;
        pView->script.n110 = gpCrAPState->pB4->nGolferId;
    }
    // EA bug: with no golfer (pB4 NULL) this reads bLoaded through the NULL pointer.
    if (gpCrAPState->pB4->bLoaded) {
        CamScript_RunFEScript(nPlayer, pCam, pSub, &pView->script, &pView->shot19C, 0, FRAME_TIME);
    }
}

// Switch the CrAP camera to a named shot (with an 'f' in front for some models), else to shot
// 0x2F/0x37/0x39/0x3B/0x3C by nShot; bBlend records the current camera and blends from it. The
// callers pass a sixth argument, n6 (0 or 1), that is not read.
void GolfCamera_SwitchCrAPCamera(View* pView, char* szName, int nShot, u8 bBlend, u8 bForce, int n6) {
    f32* pCam;
    f32* pSub;
    CamShot* pShot;
    char c;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    if (bForce && (nShot == 0 || nShot == 2)) {
        nShot = 1;
    }
    pShot = NULL;
    if (szName != NULL) {
        pShot = DynamicCam_ChooseScriptByName(szName);
        if (pShot == NULL && gpCrAPState->pB4 != NULL && gpCrAPState->pB4->pChar != NULL
            && gpCrAPState->pB4->pChar->nSlot == 1) {
            c = szName[0];
            szName[0] = 'f';
            pShot = DynamicCam_ChooseScriptByName(szName);
            szName[0] = c;
        }
    }
    if (pShot == NULL) {
        if (nShot == 0) {
            pShot = DynamicCam_ChooseScript(0, 0x2F, NULL);
        } else if (nShot == 1) {
            pShot = DynamicCam_ChooseScript(0, 0x37, NULL);
        } else if (nShot == 2) {
            pShot = DynamicCam_ChooseScript(0, 0x39, NULL);
        } else if (nShot == 3) {
            pShot = DynamicCam_ChooseScript(0, 0x3B, NULL);
        } else if (nShot == 4) {
            pShot = DynamicCam_ChooseScript(0, 0x3C, NULL);
        } else {
            pShot = NULL;
        }
    }
    if (pShot != NULL && pShot != pView->script.pShot && pShot != pView->script.pNextShot) {
        if (bBlend) {
            CameraScript_RecordCurrentCam(pView->script.pB4, pCam, pSub, 0, &pView->script, 0);
            pView->script.pShot = pView->script.pB4;
            pView->script.pNextShot = NULL;
            CameraScript_InterpToNewScript(&pView->script, pShot, 0, pCam, pSub, 0, pShot->f48, 1000.0f, 0x19,
                                           0.0f);
        } else {
            CameraScript_InterpToNewScript(&pView->script, pShot, 0, pCam, pSub, 5, 0.0f, 1000.0f, 0x19,
                                           0.0f);
        }
    }
    // EA bug: pB4 may be NULL (tested above for the 'f' names), but it is read here without a test.
    pView->script.n110 = gpCrAPState->pB4->nGolferId;
    pView->script.n114 = gpCrAPState->nScreenKind;
    if (gpCrAPState->pB4->bLoaded) {
        CamScript_RunFEScript(0, pCam, pSub, &pView->script, &pView->shot19C, 0, FRAME_TIME);
    }
}

// Camera 24, the golfer bone camera (nothing in this build switches to it): shot kind 10 started
// from its beginning (fn_8006351C).
void GolfCamera_InitGolferBoneCamera(View* pView, int nPlayer) {
    CameraController_StartScriptOfKind(pView, nPlayer, 10);
}

// Camera 24's tick: the camera 0.5 out along the golfer's bone 10's z axis and 0.1 up its y axis,
// looking at the bone (0.1 up its y axis); the view's v20 is the bone's x axis, negated. The script
// is not run.
void GolfCamera_ProcessGolferBoneCamera(View* pView, int nPlayer) {
    f32* pCam;
    f32* pSub;
    f32 (*m)[4];
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    m = Character_GetBoneMatrix(gPlayers[nPlayer].pChar, 10);
    LLMath_CopyVec(m[3], pSub);
    fn_8000C5D4(pSub, m[1], 0.1f, pSub);
    Vec3_Scale(0.5f, m[2], pCam);
    GolfCam_Vec3Add(m[3], pCam, pCam);
    fn_8000C5D4(pCam, m[1], 0.1f, pCam);
    pCam[3] = 1.0f;
    GolfCam_Vec3Negate(m[0], pView->v20);
}

// The zoom-to-aim camera's positions. pAim: out from the ball towards the target by the shot's
// f60, then across by f64, at height v30[1]. pCam: back from the target through pAim, by the
// tuning's distance (f4, or f38 on a putt) scaled for the lens, closer in when the ball is near
// the target. When pCam is out of bounds it is pulled in to the boundary, or mirrored through the
// ball (returns 1). pCam never ends up past pAim.
u8 GolfCamera_ZoomCamGetStartAndEndVecs(View* pView, int nPlayer, f32* pSub, f32* pAim, f32* pCam) {
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
    Ter_GetTGD();
    LLMath_CopyVec(gPlayers[nPlayer].vTargetCopy, vTarget);
    pBall = gPlayers[nPlayer].vBall;
    GolfCam_Vec3Sub(vTarget, pBall, vDir);
    vDir[1] = 0.0f;
    fDist = Math_Sqrt(Vec3_LengthSqClamped(vDir));
    if (vDir[0] != 0.0f || vDir[1] != 0.0f || vDir[2] != 0.0f) {
        LLMath_Normalize3(vDir, vDir);
    }
    Vec3_Scale(pView->shot19C.f60, vDir, vOff);
    GolfCam_Vec3Add(pBall, vOff, pAim);
    pAim[0] += pView->shot19C.f64 * -Vec3_Dot(vDir, gGolfCamAxisZ);
    pAim[2] += pView->shot19C.f64 * Vec3_Dot(vDir, gGolfCamAxisX);
    pAim[1] = pView->shot19C.v30[1];
    GolfCam_Vec3Sub(vTarget, pAim, vBack);
    vBack[1] = 0.0f;
    if (vBack[0] != 0.0f || vBack[1] != 0.0f || vBack[2] != 0.0f) {
        LLMath_Normalize3(vBack, vBack);
    }
    if (gPlayers[nPlayer].nShotKind != 0) {
        fBack = lbl_80281F78->f4;
    } else {
        fBack = lbl_80281F78->f38;
    }
    fBack *= 1.0f / Camera_GetLensFovScale(
                        Camera_GetLens(ViewController_GetRenderContext(gPlayers[nPlayer].nView[0])));
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
    if (!PlaceBall_CheckInBounds(pCam)) {
        pNet = PlaceBall_GetPlaceBallNetwork();
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
    GolfCam_Vec3Sub(pCam, pSub, vBack);
    vBack[1] = 0.0f;
    GolfCam_Vec3Sub(vTarget, pAim, vDir);
    GolfCam_Vec3Sub(pCam, pAim, v);
    v[1] = 0.0f;
    vDir[1] = 0.0f;
    if (Vec3_Dot(vDir, v) < 0.0f) {
        pCam[0] = pAim[0];
        pCam[2] = pAim[2];
        GolfCam_Vec3Sub(pCam, pSub, vBack);
        vBack[1] = 0.0f;
    }
    return bMoved;
}

// Is the special swing camera (View.n260) one of the matrix camera kinds 1, 6 and 10? The
// ball-flight camera then starts the matrix camera at impact.
u8 GolfCamera_ChooseImpactMatrixCam(View* pView, int nPlayer) {
    CameraController_GetCameraOrigin(pView);
    if (pView->n260 == 1) {
        return 1;
    }
    if (pView->n260 == 6) {
        return 1;
    }
    return pView->n260 == 10;
}

// Is the special swing camera (View.n260) one of the swing-replay kinds 2, 5, 8, 11, 15 and 16?
// After the hit, these (like the 3-screen, heartbeat and shutter kinds) send the golfer to the
// slow-motion swing replay (GS_REPLAY_SWING).
u8 GolfCamera_Choose3ShotCam(View* pView, int nPlayer) {
    CameraController_GetCameraOrigin(pView);
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

// Is the special swing camera (View.n260) the 3-screen comic camera, kind 4 or 9 (9 with the other
// panel layout)?
u8 GolfCamera_Choose3ScreenCam(View* pView, int nPlayer) {
    if (pView->n260 == 4) {
        return 1;
    }
    return pView->n260 == 9;
}

// Is the special swing camera (View.n260) the heartbeat camera, kind 7?
u8 GolfCamera_ChooseHeartBeatCam(View* pView, int nPlayer) {
    return pView->n260 == 7;
}

// Is the special swing camera (View.n260) the shutter camera, kind 3?
u8 GolfCamera_ChooseShutterCam(View* pView, int nPlayer) {
    return pView->n260 == 3;
}

// Is the special swing camera (View.n260) the super zoom, kind 13 or 14 (13 zooms in from the
// camera's own side)? The ball-flight camera then starts it at impact.
u8 GolfCamera_ChooseSuperZoomCam(View* pView, int nPlayer) {
    if (pView->n260 == 13) {
        return 1;
    }
    return pView->n260 == 14;
}

// How many shots of the slow-motion swing replay have run (script.n110);
// STATEFUNC_ReplaySwingUpdate launches the ball once it reaches the replay's planned count.
int GolfCamera_NumCompletedReplayCams(View* pView) {
    return pView->script.n110;
}

// Is the player's target steep from the ball: the slope |dy / dx| (the height change over the x
// difference alone, not the flat distance) at least fUp when the target is higher, fDown when it is
// lower. 0 when the x difference is under 1e-6.
u8 GolfCamera_SteepSlopeCamCheckSlope(View* pView, int nPlayer, f32 fUp, f32 fDown) {
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

// Does the ground (Ter_CheckForGroundCollision) lie between the view's camera position (v0) and the
// player's target?
u8 GolfCamera_SteepSlopeCamCheckCollision(View* pView, int nPlayer) {
    f32 vHit[4];
    f32 vNormal[4];
    SurfaceType* pSurface;
    TerObject* pObj;
    return Ter_CheckForGroundCollision(gPlayers[nPlayer].ball.pCourse, pView->v0, gPlayers[nPlayer].vTarget,
                                       vHit, vNormal, &pSurface, &pObj);
}

// Does the aim view need the steep-slope camera (mode 0x13) instead of the elevator camera
// (STATEFUNC_ElevatorInit asks)? Never with the putter. With the tuning's bCheckSlope on: only when
// the target is steep from the ball (GolfCamera_SteepSlopeCamCheckSlope), and then yes. Otherwise
// yes when the tuning's bCheckTerrain finds the ground in the way
// (GolfCamera_SteepSlopeCamCheckCollision) or the target is off screen (fn_800635D0: not 0.1 in
// from the edges).
u8 GolfCamera_NeedSteepSlopeCam(View* pView, int nPlayer) {
    u8 bMove;
    if (gPlayers[nPlayer].nClub == CLUB_PUTTER_e) {
        return 0;
    }
    bMove = 0;
    if (lbl_80281F78->bCheckSlope) {
        bMove = GolfCamera_SteepSlopeCamCheckSlope(pView, nPlayer, lbl_80281F78->fSlopeUp,
                                                   lbl_80281F78->fSlopeDown);
        if (!bMove) {
            return 0;
        }
    }
    if (!bMove && lbl_80281F78->bCheckTerrain) {
        bMove = GolfCamera_SteepSlopeCamCheckCollision(pView, nPlayer);
    }
    if (!bMove) {
        bMove = CameraController_TargetIsOnScreen(nPlayer) == 0;
    }
    return bMove;
}

// The steep-slope camera: from a base point by the ball, back away from the target (and down when
// looking up, up when looking down) until the ground no longer hides the target, up to n1DC tries
// (at least 2; fewer when the target has not moved). The view moves there, looking at the target,
// no faster than the tuning allows, and not steeper than GolfCamera_ClampLookAngle allows.
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
    LLMath_CopyVec(pTarget, vTarget);
    pBall = &gPlayers[nPlayer].ball;
    if (gSteepSlopeCamLastTries == -1) {
        fn_800B5918(pTarget, gSteepSlopeCamLastTarget);
        nMax = lbl_80281F78->n1DC;
    } else if (LLMath_SquareDistanceBetween3(gSteepSlopeCamLastTarget, pTarget) > 0.1f) {
        fn_800B5918(pTarget, gSteepSlopeCamLastTarget);
        nMax = (gSteepSlopeCamLastTries + 1 <= lbl_80281F78->n1DC) ? gSteepSlopeCamLastTries + 1
                                                                   : lbl_80281F78->n1DC;
    } else {
        nMax = gSteepSlopeCamLastTries;
    }
    nTries = (nMax > 2) ? nMax : 2;
    vBase[0] = lbl_80281F78->f1E8 + gPlayers[nPlayer].ball.vPos[0];
    vBase[1] = lbl_80281F78->f1EC + gPlayers[nPlayer].ball.vPos[1];
    vBase[2] = gPlayers[nPlayer].ball.vPos[2];
    vBase[3] = 0.0f;
    GolfCam_Vec3Sub(vTarget, vBase, vDelta);
    if (vDelta[0] != 0.0f || vDelta[1] != 0.0f || vDelta[2] != 0.0f) {
        LLMath_Normalize3(vDelta, vDir);
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
        RC_UpdateCurrentScreenMatrices();
        RC_vUpdateRenderCtxTransformationMatrices(RC_spGetCurrentRenderCtx());
        bHit = Ter_CheckForGroundCollision(pBall->pCourse, vCam, vTarget, vHit, vNormal, &pSurface, &pObj);
        CameraController_BallIsOnScreen(nPlayer);
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
        if (bClear && i >= gSteepSlopeCamLastTries - 2) {
            bDone = 1;
        }
        i++;
    }
    gSteepSlopeCamLastTries = i;
    GolfCamera_LimitPositionChange(vOldCam, pView->v0, pView->v0, lbl_80281F78->f1F0);
    GolfCamera_LimitPositionChange(vOldSub, pView->v10, pView->v10, lbl_80281F78->f1F4);
    GolfCamera_ClampLookAngle(pView->v0, pView->v10, pView->v10);
}

// The point pTo as seen from pFrom, but with the direction turned back towards the horizontal when
// it is steeper than the tuning's fMaxPitchUp (going up) or fMaxPitchDown (going down); into pOut.
// Nothing is written when the direction is within the limits.
void GolfCamera_ClampLookAngle(f32* pFrom, f32* pTo, f32* pOut) {
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
    GolfCam_Vec3Sub(pTo, pFrom, v);
    vFlat[0] = v[0];
    vFlat[1] = 0.0f;
    vFlat[2] = v[2];
    vFlat[3] = 0.0f;
    if (v[0] != 0.0f || v[1] != 0.0f || v[2] != 0.0f) {
        LLMath_Normalize3(v, vDir);
    } else {
        vDir[0] = 0.0f;
        vDir[1] = 0.0f;
        vDir[2] = 0.0f;
    }
    if (vFlat[0] != 0.0f || vFlat[1] != 0.0f || vFlat[2] != 0.0f) {
        LLMath_Normalize3(vFlat, vFlatDir);
    } else {
        vFlatDir[0] = 0.0f;
        vFlatDir[1] = 0.0f;
        vFlatDir[2] = 0.0f;
    }
    fAngle = Math_Acos((Vec3_Dot(vDir, vFlatDir) < -1.0f) ? -1.0f
                         : ((Vec3_Dot(vDir, vFlatDir) > 1.0f) ? 1.0f : Vec3_Dot(vDir, vFlatDir)));
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
            LLMath_Normalize3(vAxis, vAxis);
        }
        LLMath_Scale(fOver, vAxis, vAxis);
        Quat_BuildFromVector(vAxis, vQuat);
        v[3] = 0.0f;
        Quat_RotateVector(vQuat, v, vOut);
        GolfCam_Vec3Add(pFrom, vOut, pOut);
    }
}

// Move pOut from pFrom towards pTo by at most fMax; nonzero if it had to stop short.
int GolfCamera_LimitPositionChange(f32* pFrom, f32* pTo, f32* pOut, f32 fMax) {
    int bClamped = 0;
    f32 v[4];
    f32 vStep[4];
    f32 fDist;
    GolfCam_Vec3Sub(pTo, pFrom, v);
    fDist = Math_Sqrt(Vec3_LengthSqClamped(v));
    if (fDist > fMax && fDist > 1e-6f) {
        Vec3_Scale(fMax / fDist, v, vStep);
        GolfCam_Vec3Add(pFrom, vStep, pOut);
        bClamped = 1;
    } else {
        Vec3Copy(pTo, pOut);
    }
    return bClamped;
}

// The first of shots 0x1C + n .. 0x21 the player's camera plan has (none in modes 6, 7 and 8).
CamShot* GolfCamera_GetAlternateSwingCamera(int nFirst, int nPlayer) {
    int i;
    CamShot* pShot;
    if (Game_GetMode() != 6 && Game_GetMode() != 7 && Game_GetMode() != 8) {
        for (i = nFirst; i <= 5; i++) {
            pShot = DynamicCam_ChooseScript(nPlayer, i + 0x1C, NULL);
            if (pShot != NULL) {
                return pShot;
            }
        }
    }
    return NULL;
}

// The alternate-swing button (GM_CheckForShotChanges): when no blend to a next shot is under way,
// or it is of blend kind 6, 8, 9 or 10 (script.nBC), step n264 through 1..5 and cut to that
// alternate swing camera (GolfCamera_GetAlternateSwingCamera); at 3, or with none, go back to the
// saved shot p80.
void GolfCamera_vSwitchToNextAlternateSwingCamera(View* pView, int nPlayer) {
    f32* pCam;
    f32* pSub;
    CamShot* pShot;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    // fake match: the constant first in one compare keeps the 8/9 tests from becoming a range test
    if (pView->script.pNextShot == NULL || 6 == pView->script.nBC || pView->script.nBC == 9
        || pView->script.nBC == 8 || pView->script.nBC == 10) {
        pView->n264++;
        if (pView->n264 > 5) {
            pView->n264 = 1;
        }
        pShot = GolfCamera_GetAlternateSwingCamera(pView->n264, nPlayer);
        if (pShot != NULL && pView->n264 != 3) {
            CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, pShot->bAB, pShot->f48,
                                           1000.0f, 0x19, 0.0f);
        } else if (pView->p80 != NULL && pView->p80 != pView->script.pShot) {
            if (pView->script.pShot != NULL) {
                CameraScript_InterpToNewScript(&pView->script, pView->p80, nPlayer, pCam, pSub,
                                               pView->script.pShot->bAB, pView->script.pShot->f48, 1000.0f,
                                               0x19, 0.0f);
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
void GolfCamera_CreateMatrixCamera(View* pView, f32* pFrom, f32* pTo, int nPlayer) {
    f32 v[4];
    f32 vFrom[4];
    char szName[] = "MATRIX CAM";
    f32 fChange;
    CamShot* pShot;
    int nOrder;
    LLMath_CopyVec(pFrom, vFrom);
    GolfCam_Vec3Sub(pTo, vFrom, v);
    v[1] = 0.0f;
    GolfCam_Vec3Add(pTo, v, v);
    v[1] = vFrom[1];
    Mem_set(&gGolfCamState->shot6C, 0, sizeof(CamShot) * 2);
    strcpy(gGolfCamState->shot6C.szName, szName);
    Vec3Copy(vFrom, gGolfCamState->shot6C.v20);
    Vec3Copy(pTo, gGolfCamState->shot6C.v30);
    gGolfCamState->shot6C.p40 = NULL;
    gGolfCamState->shot6C.f60 = 0.0f;
    gGolfCamState->shot6C.f64 = 0.0f;
    gGolfCamState->shot6C.f70 = 0.0f;
    gGolfCamState->shot6C.f74 = 0.0f;
    gGolfCamState->shot6C.f68 = 0.2f;
    gGolfCamState->shot6C.f6C = 20.0f;
    fChange = GameEffects_FieldOfViewChange();
    gGolfCamState->shot6C.f78 =
        CA_fGetCameraFieldOfView(
                Camera_GetLens(ViewController_GetIndexedViewController(gPlayers[nPlayer].nView[0])->pCamera))
                        - fChange;
    gGolfCamState->shot6C.f7C = gGolfCamState->shot6C.f78;
    gGolfCamState->shot6C.f80 = 0.0f;
    gGolfCamState->shot6C.f4C = 1.0f;
    gGolfCamState->shot6C.f9C = 0.0f;
    gGolfCamState->shot6C.f84 = 0.0f;
    gGolfCamState->shot6C.bA8 = 0;
    gGolfCamState->shot6C.bAA = 1;
    gGolfCamState->shot6C.bAC = 0x18;
    gGolfCamState->shot6C.bB1 = 9;
    gGolfCamState->shot6C.bB2 = 0;
    gGolfCamState->shot6C.bAD = 3;
    gGolfCamState->shot6C.p44 = pView->script.pShot;
    gGolfCamState->shot6C.f8C = 0.0f;
    pShot = DynamicCam_ChooseScript(nPlayer, 0x22, NULL);
    if (pShot != NULL) {
        Mem_cpy(&gGolfCamState->shot12C, pShot, sizeof(CamShot));
        gGolfCamState->shot12C.p40 = NULL;
        gGolfCamState->shot12C.p44 = pView->script.pShot;
        gGolfCamState->shot12C.bAD = 3;
        gGolfCamState->shot12C.f8C = 0.0f;
        gGolfCamState->shot12C.f4C = 1.0f;
    } else {
        strcpy(gGolfCamState->shot12C.szName, szName);
        Vec3Copy(v, gGolfCamState->shot12C.v20);
        Vec3Copy(pTo, gGolfCamState->shot12C.v30);
        gGolfCamState->shot12C.p40 = NULL;
        gGolfCamState->shot12C.f60 = 0.0f;
        gGolfCamState->shot12C.f64 = 0.0f;
        gGolfCamState->shot12C.f70 = 0.0f;
        gGolfCamState->shot12C.f74 = 0.0f;
        gGolfCamState->shot12C.f68 = 0.2f;
        gGolfCamState->shot12C.f6C = 20.0f;
        gGolfCamState->shot12C.f78 = gGolfCamState->shot6C.f78;
        gGolfCamState->shot12C.f7C = gGolfCamState->shot12C.f78;
        gGolfCamState->shot12C.f80 = 0.0f;
        gGolfCamState->shot12C.f4C = 1.0f;
        gGolfCamState->shot12C.f9C = 0.0f;
        gGolfCamState->shot12C.f84 = 0.0f;
        gGolfCamState->shot12C.bA8 = 0;
        gGolfCamState->shot12C.bAA = 1;
        gGolfCamState->shot12C.bAC = 0x18;
        gGolfCamState->shot12C.bB1 = 9;
        gGolfCamState->shot12C.bB2 = 0;
        gGolfCamState->shot12C.bAD = 3;
        gGolfCamState->shot12C.p44 = pView->script.pShot;
        gGolfCamState->shot12C.f8C = 0.0f;
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
        pView->script.pShot = &gGolfCamState->shot6C;
        pView->script.pNextShot = &gGolfCamState->shot12C;
        pView->script.f8C = lbl_80281F78->f5C;
        pView->script.nBC = 2;
        if (CameraScript_FlipCameraForLefty(nPlayer, NULL)) {
            pView->script.nD0 = 2;
        } else {
            pView->script.nD0 = 1;
        }
    } else if (nOrder == 1) {
        pView->script.pShot = &gGolfCamState->shot12C;
        pView->script.pNextShot = &gGolfCamState->shot6C;
        pView->script.f8C = lbl_80281F78->f5C;
        pView->script.nBC = 2;
        if (CameraScript_FlipCameraForLefty(nPlayer, NULL)) {
            pView->script.nD0 = 1;
        } else {
            pView->script.nD0 = 1;
        }
    } else {
        pView->script.pShot = &gGolfCamState->shot6C;
        pView->script.pNextShot = &gGolfCamState->shot12C;
        pView->script.nBC = 7;
        pView->script.f8C = 2.0f * lbl_80281F78->f5C;
        if (CameraScript_FlipCameraForLefty(nPlayer, NULL)) {
            pView->script.nD0 = 2;
        } else {
            pView->script.nD0 = 1;
        }
    }
    pView->script.fCamTime = 0.00001f;
    if (pView->p74 != NULL) {
        pView->script.pNextShot->f4C = pView->p74->f38;
    } else {
        pView->script.pNextShot->f4C = 0.5f;
    }
    pView->script.pNextShot->f4C = 1.0f;    // f4C is set just above; the original overwrites it
}

// The matrix camera's tick (the ball-flight camera runs it while GolfCamera_IsMatrixCamActive): the
// screen effect fn_80038054 (the tuning's f64); with no next shot, clear b54 (a freeze-time flag:
// GolfCamera_IsFreezeTimeActive tests it), release the golfer's animation (SKATime_UnPause) and queue
// the shot from before (the current shot's p44) as a hand-made shot. Returns the time to run the
// script by: with the tuning's f60 set, f60 once every f60 seconds and 0 in between, else one
// frame (FRAME_TIME); 0 while paused.
f32 GolfCamera_UpdateMatrixCamera(View* pView, f32* pCam, f32* pSub, int nPlayer) {
    f32 fTime = 0.0f;
    CamShot* pShot;
    if (gSession.nPaused == 0) {
        fn_80038054(1, ViewController_GetCurrentViewControllerID(), 0.0f, lbl_80281F78->f64);
    }
    if (pView->script.pNextShot == NULL) {
        gGolfCamState->b54 = 0;
        SKATime_UnPause(gPlayers[nPlayer].pChar->anim);
        EVENT_Trigger(nPlayer, 0x33, NULL, -1);
        EVENT_Trigger(nPlayer, 0x3B, NULL, 1);
        pShot = pView->script.pShot->p44;
        if (pShot != NULL) {
            Mem_cpy(&pView->shot19C, pShot, sizeof(CamShot));
            pView->shot19C.p44 = pShot;
            pView->script.pNextShot = &pView->shot19C;
            pView->script.fCamTime = 0.0f;
            pView->script.f8C = lbl_80281F78->f58;
            pView->script.nBC = 5;
            if (pView->p74 != NULL) {
                pView->script.pNextShot->f4C = pView->p74->f38 - lbl_80281F78->f58;
            } else {
                pView->script.pNextShot->f4C = lbl_80281F78->f58;
            }
            pView->script.pNextShot->p44 = pShot;
        }
    }
    if (gSession.nPaused == 0) {
        if (lbl_80281F78->f60) {
            if (pView->script.f108 > lbl_80281F78->f60) {
                fTime = lbl_80281F78->f60;
                pView->script.f108 = 0.0f;
            } else {
                fTime = 0.0f;
                pView->script.f108 += FRAME_TIME;
            }
        } else {
            fTime = FRAME_TIME;
        }
    }
    return fTime;
}

// The super zoom: two hand-made "SUPER ZOOM" shots in the shared state, both looking at pTo; from
// pFrom for kind 13, else from the far side of pTo at pFrom's height. The first, with the tuning's
// f7C lens, blends into the second over the tuning's f84 seconds; the second has the view's field
// of view less the letterbox's change (stored in the tuning's f80). Both keep the current shot
// (p44) to go back to; the screen effect fn_80038054 starts with the tuning's f8C and f88.
void GolfCamera_CreateSuperZoomCamera(View* pView, f32* pFrom, f32* pTo, int nPlayer) {
    f32 v[4];
    f32 vFrom[4];
    char szName[] = "SUPER ZOOM";
    f32 fChange;
    CamTuning* pTune;
    LLMath_CopyVec(pFrom, vFrom);
    GolfCam_Vec3Sub(pTo, vFrom, v);
    v[1] = 0.0f;
    GolfCam_Vec3Add(pTo, v, v);
    v[1] = vFrom[1];
    strcpy(gGolfCamState->shot6C.szName, szName);
    if (pView->n260 == 13) {
        Vec3Copy(pFrom, gGolfCamState->shot6C.v20);
    } else {
        Vec3Copy(v, gGolfCamState->shot6C.v20);
    }
    Vec3Copy(pTo, gGolfCamState->shot6C.v30);
    gGolfCamState->shot6C.p40 = NULL;
    gGolfCamState->shot6C.f60 = 0.0f;
    gGolfCamState->shot6C.f64 = 0.0f;
    gGolfCamState->shot6C.f70 = 0.0f;
    gGolfCamState->shot6C.f74 = 0.0f;
    gGolfCamState->shot6C.f68 = 0.2f;
    gGolfCamState->shot6C.f6C = 20.0f;
    gGolfCamState->shot6C.f78 = lbl_80281F78->f7C;
    gGolfCamState->shot6C.f7C = gGolfCamState->shot6C.f78;
    gGolfCamState->shot6C.f80 = 0.0f;
    gGolfCamState->shot6C.f4C = 0.1f;
    gGolfCamState->shot6C.f9C = -2.0f * PI;
    gGolfCamState->shot6C.f84 = 0.0f;
    gGolfCamState->shot6C.bA8 = 0;
    gGolfCamState->shot6C.bAA = 1;
    gGolfCamState->shot6C.bAC = 0x18;
    gGolfCamState->shot6C.bB1 = 9;
    gGolfCamState->shot6C.bB2 = 0;
    gGolfCamState->shot6C.bAD = 3;
    gGolfCamState->shot6C.p44 = pView->script.pShot;
    strcpy(gGolfCamState->shot12C.szName, szName);
    Vec3Copy(gGolfCamState->shot6C.v20, gGolfCamState->shot12C.v20);
    Vec3Copy(pTo, gGolfCamState->shot12C.v30);
    gGolfCamState->shot12C.p40 = NULL;
    gGolfCamState->shot12C.f60 = 0.0f;
    gGolfCamState->shot12C.f64 = 0.0f;
    gGolfCamState->shot12C.f70 = 0.0f;
    gGolfCamState->shot12C.f74 = 0.0f;
    gGolfCamState->shot12C.f68 = 0.2f;
    gGolfCamState->shot12C.f6C = 20.0f;
    fChange = GameEffects_FieldOfViewChange();
    lbl_80281F78->f80
            = CA_fGetCameraFieldOfView(
                    Camera_GetLens(
                            ViewController_GetIndexedViewController(gPlayers[nPlayer].nView[0])->pCamera))
            - fChange;
    gGolfCamState->shot12C.f78 = lbl_80281F78->f80;
    gGolfCamState->shot12C.f7C = gGolfCamState->shot12C.f78;
    gGolfCamState->shot12C.f80 = 0.0f;
    gGolfCamState->shot12C.f4C = 0.1f;
    gGolfCamState->shot12C.f9C = 0.0f;
    gGolfCamState->shot12C.f84 = 0.0f;
    gGolfCamState->shot12C.bA8 = 0;
    gGolfCamState->shot12C.bAA = 1;
    gGolfCamState->shot12C.bAC = 0x18;
    gGolfCamState->shot12C.bB1 = 9;
    gGolfCamState->shot12C.bB2 = 0;
    gGolfCamState->shot12C.bAD = 3;
    gGolfCamState->shot12C.p44 = pView->script.pShot;
    pTune = lbl_80281F78;
    fn_80038054(1, ViewController_GetCurrentViewControllerID(), pTune->f8C, pTune->f88);
    pView->script.pShot = &gGolfCamState->shot6C;
    pView->script.pNextShot = &gGolfCamState->shot12C;
    pView->script.f8C = lbl_80281F78->f84;
    pView->script.nBC = 1;
    pView->script.fCamTime = 0.0f;
    if (pView->p74 != NULL) {
        pView->script.pNextShot->f4C = pView->p74->f38;
    } else {
        pView->script.pNextShot->f4C = 0.5f;
    }
    pView->script.pNextShot->bAD = 4;
    pView->script.pNextShot->f4C = 1.0f;    // f4C is set just above; the original overwrites it
}

// The super zoom's tick (the ball-flight camera runs it while GolfCamera_IsSuperZoomCamActive):
// with no next shot and 0.05 s gone, clear b58 (a freeze-time flag: GolfCamera_IsFreezeTimeActive
// tests it), release the golfer's animation (SKATime_UnPause) and queue the shot from before (the
// current shot's p44) as a hand-made shot; while a next shot is pending, ease the two super-zoom
// shots' lens (f78/f7C) from the tuning's f7C to f80. Returns one frame (FRAME_TIME), 0 paused.
f32 GolfCamera_UpdateSuperZoomCamera(View* pView, f32* pCam, f32* pSub, int nPlayer) {
    f32 fTime = 0.0f;
    CamShot* pShot;
    CamTuning* pTune;
    f32 t;
    f32 f;
    if (gSession.nPaused == 0) {
        pTune = lbl_80281F78;
        fn_80038054(1, ViewController_GetCurrentViewControllerID(), pTune->f8C, pTune->f88);
    }
    if (pView->script.pNextShot == NULL && pView->script.fCamTime > 0.05f) {
        gGolfCamState->b58 = 0;
        SKATime_UnPause(gPlayers[nPlayer].pChar->anim);
        EVENT_Trigger(nPlayer, 0x3A, NULL, -1);
        EVENT_Trigger(nPlayer, 0x3B, NULL, 1);
        if (pView->script.pShot != NULL) {
            gGolfCamState->shot6C.f78 = lbl_80281F78->f80;
            gGolfCamState->shot6C.f7C = lbl_80281F78->f80;
            gGolfCamState->shot12C.f78 = lbl_80281F78->f80;
            gGolfCamState->shot12C.f7C = lbl_80281F78->f80;
        }
        // EA bug: the script's pShot was tested for NULL just above, but is read here without a test.
        pShot = pView->script.pShot->p44;
        if (pShot != NULL) {
            Mem_cpy(&pView->shot19C, pShot, sizeof(CamShot));
            pView->shot19C.p44 = pShot;
            pView->script.pNextShot = &pView->shot19C;
            pView->script.fCamTime = 0.0f;
            pView->script.f8C = lbl_80281F78->f58;
            pView->script.nBC = 5;
            if (pView->p74 != NULL) {
                pView->script.pNextShot->f4C = pView->p74->f38 - lbl_80281F78->f58;
            } else {
                pView->script.pNextShot->f4C = lbl_80281F78->f58;
            }
            pView->script.pNextShot->p44 = pShot;
        }
    } else if (pView->script.pNextShot != NULL) {
        t = pView->script.fCamTime / pView->script.f8C;
        t *= t;
        f = t * (lbl_80281F78->f80 - lbl_80281F78->f7C) + lbl_80281F78->f7C;
        gGolfCamState->shot6C.f78 = f;
        gGolfCamState->shot6C.f7C = f;
        gGolfCamState->shot12C.f78 = f;
        gGolfCamState->shot12C.f7C = f;
    } else if (pView->script.pShot != NULL) {
        gGolfCamState->shot6C.f78 = lbl_80281F78->f80;
        gGolfCamState->shot6C.f7C = lbl_80281F78->f80;
        gGolfCamState->shot12C.f78 = lbl_80281F78->f80;
        gGolfCamState->shot12C.f7C = lbl_80281F78->f80;
    }
    if (gSession.nPaused == 0) {
        fTime = FRAME_TIME;
    }
    return fTime;
}

// Move the replay swing camera to its next angle (STATEFUNC_ReplaySwingUpdate, each time the
// replayed swing reaches the ball): count it in script.n110, restart the camera clock and let
// GolfCamera_CreateReplayCamera choose the shot.
void GolfCamera_PickNextSwingReplayCam(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    pView->script.n110++;
    pView->script.fCamTime = 0.0f;
    GolfCamera_CreateReplayCamera(pView, pCam, pSub, nPlayer);
}

// The replay swing camera's (mode 13) next shot, from its init (fn_800C14B0) and
// GolfCamera_PickNextSwingReplayCam: while script.n110 is under GolfCamera_HowManyReplaySwings, cut
// to another shot of kind 13 (DynamicCam_ChooseScript, passing the current one) unless it would
// hide the golfer; after that, back to the shot it started from (kept in shot19C.p44). Swing camera
// kinds 15 and 16 cut to a shot of kind 0x22 instead and turn the slow-motion swing camera on
// (b59). The callers pass the view's position and aim, but it fetches them again.
void GolfCamera_CreateReplayCamera(View* pView, f32* pViewCam, f32* pViewSub, int nPlayer) {
    CamShot* pShot;
    f32* pCam;
    f32* pSub;
    pCam = CameraController_GetCameraOrigin(pView);
    pSub = CameraController_GetCameraLookPoint(pView);
    if (pView->n260 == 15 || pView->n260 == 16) {
        pShot = DynamicCam_ChooseScript(nPlayer, 0x22, NULL);
        if (pShot != NULL) {
            CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f, 0x19,
                                           0.0f);
            pView->script.f8C = 1.0f;
        }
        pView->script.f10C = 0.0f;
        gGolfCamState->b59 = 1;
    } else {
        if (pView->script.n110 == 0) {
            pView->shot19C.p44 = pView->script.pShot;
        }
        if (pView->script.n110 >= GolfCamera_HowManyReplaySwings(pView)) {
            pView->script.pShot = pView->shot19C.p44;
            pView->script.pNextShot = NULL;
            pView->script.fCamTime = 0.0f;
        } else {
            pShot = DynamicCam_ChooseScript(nPlayer, 0xD, pView->script.pShot);
            if (pShot != NULL
                && !CameraScript_WillGolferBeOccludedInThisView(nPlayer, pShot, &pView->script)) {
                CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f,
                                               0x19, 0.0f);
            }
        }
    }
}

// The replay swing camera's clock follows the swing (GolfCamera_ProcessReplaySwingCamera): fCamTime
// is the share 0..1 from the swing's start (animation tag 0) to the top of the backswing (tag 1),
// with script.f10C 0, then from the top to the ball hit (tag 2), with f10C 1.
void GolfCamera_UpdateSwingSlowMoCamera(View* pView, f32* pCam, f32* pSub, int nPlayer) {
    f32 fEnd;
    f32 fStart;
    f32 fTop;
    f32 fTime;
    f32 t;
    fEnd = Character_GetTagTime(gPlayers[nPlayer].pChar, 2);
    fStart = Character_GetTagTime(gPlayers[nPlayer].pChar, 0);
    fTop = Character_GetTagTime(gPlayers[nPlayer].pChar, 1);
    fTime = gPlayers[nPlayer].pChar->fAnimTime;
    if (fTime < fTop) {
        t = (fTime - fStart) / (fTop - fStart);
        pView->script.fCamTime = (t < 0.0f) ? 0.0f : ((t > 1.0f) ? 1.0f : t);
        pView->script.f10C = 0.0f;
    } else {
        pView->script.f10C = 1.0f;
        t = (fTime - fTop) / (fEnd - fTop);
        pView->script.fCamTime = (t < 0.0f) ? 0.0f : ((t > 1.0f) ? 1.0f : t);
    }
}

// On the replay swing's last angle (STATEFUNC_ReplaySwingUpdate): is this swing camera kind 11, the
// super swing? Then script.n114 is set and it answers 1, and the swing is replayed with blend 0x12.
u8 GolfCamera_ChooseSuperSwing(View* pView, int nPlayer) {
    u8 bOn = 0;
    if (pView->n260 == 11) {
        bOn = 1;
    }
    if (bOn) {
        pView->script.n114 = 1;
    }
    return bOn;
}

// The knee (green-reading) camera zooming in while button 4 is held (STATEFUNC_KneeCamUpdate):
// shot19C.f60 steps up by 0.1 a frame, but stays at most 3 short of the flat distance from the ball
// (vBall) to the aim point (vTarget2), and not below 0.
void GolfCamera_ZoomGreenCamera(View* pView, int nPlayer) {
    f32 v[4];
    GolfCam_Vec3Sub(gPlayers[nPlayer].vTarget2, gPlayers[nPlayer].vBall, v);
    v[1] = 0.0f;
    if (pView->shot19C.f60 + 0.1f < (f32)Math_Sqrt(Vec3_LengthSqClamped(v)) - 3.0f) {
        pView->shot19C.f60 += 0.1f;
    } else if (pView->shot19C.f60 > (f32)Math_Sqrt(Vec3_LengthSqClamped(v)) - 3.0f) {
        pView->shot19C.f60 = (f32)Math_Sqrt(Vec3_LengthSqClamped(v)) - 3.0f;
        if (pView->shot19C.f60 < 0.0f) {
            pView->shot19C.f60 = 0.0f;
        }
    }
}

// The knee camera zooming back out while button 4 is up (STATEFUNC_KneeCamUpdate): shot19C.f60
// steps down by 0.1 a frame, not below 0.
void GolfCamera_UnZoomGreenCamera(View* pView, int nPlayer) {
    pView->shot19C.f60 -= 0.1f;
    if (pView->shot19C.f60 < 0.0f) {
        pView->shot19C.f60 = 0.0f;
    }
}

// The reaction camera once the ball stops (GolfCamera_InitPostShotCamera,
// GolfCamera_InitInHoleCamera): the paired sequence or camera
// (DynamicCam_ChoosePairedSequenceOrCamera), else a sequence for the ball's lie, the surface class
// and the shot's flat distance (fn_8003BDBC); from the sequence shot kind 9, else the kind asked
// for (script.nC4), else 5, and the sequence becomes the view's p74. The camera cuts to the shot
// with the blend the sequence gives. a is not used (TW07's takes two arguments).
void GolfCamera_ChooseReactionCam(View* pView, int nPlayer, int a) {
    f32* pCam = CameraController_GetCameraOrigin(pView);
    f32* pSub = CameraController_GetCameraLookPoint(pView);
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
    GolfCam_Vec3Sub(gPlayers[nPlayer].ballBefore.vPos, gPlayers[nPlayer].ball.vStart, v);
    v[1] = 0.0f;
    fDist = Math_Sqrt(Vec3_LengthSqClamped(v));
    if (!DynamicCam_ChoosePairedSequenceOrCamera(nPlayer, 1, &pSeq, &pShot) && pShot == NULL) {
        pSeq = fn_8003BDBC(nPlayer, nLie, nClass, 15, 1, fDist);
    }
    if (pSeq != NULL && pShot == NULL) {
        pShot = DynamicCam_ChooseScriptInSequence(pSeq, 9, &nA, &f1, &f2, &nB, &f3, nPlayer);
        if (pShot != NULL) {
            pView->p74 = pSeq;
        }
    }
    if (pShot == NULL && pSeq != NULL) {
        pView->p74 = pSeq;
        pShot = DynamicCam_ChooseScriptInSequence(pView->p74, pView->script.nC4, &nA, &f1, &f2, &nB, &f3, nPlayer);
        if (pShot == NULL && pView->script.nC4 != 5) {
            pShot = DynamicCam_ChooseScriptInSequence(pView->p74, 5, &nA, &f1, &f2, &nB, &f3, nPlayer);
        }
    }
    if (pShot != NULL) {
        CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, nA, f1, f2, nB, f3);
        pView->script.bCF = 0;
    }
}

// Cut to the golfer once he is done animating: a static camera of kind 0x40 (not the current or the
// previous one), else shot 5 of the sequence kept from before the post-shot cameras (p78), else the
// plan's shot 5; with none, hold the current camera as a hand-made shot (shot19C). No cut when the
// current shot has bAA clear. Either way script.n114 and n110 become 1
// (GolfCamera_IsPostShotCamFinalCutDone, GolfCamera_IsGolferDoneAnimating).
void GolfCamera_CutToGolferDoneAnimatingCam(View* pView, int nPlayer) {
    int nA = 5;
    f32 f1 = 0.0f;
    f32 f2 = 100.0f;
    int nB = 0x19;
    f32 f3 = 0.0f;
    f32* pCam = CameraController_GetCameraOrigin(pView);
    f32* pSub = CameraController_GetCameraLookPoint(pView);
    CamShot* pShot;
    if (pView->script.pShot != NULL && pView->script.pShot->bAA == 0) {
        pView->script.n114 = 1;
        pView->script.n110 = 1;
        return;
    }
    pShot = StaticCam_ChooseScript(nPlayer, 0x40, 1, pView->script.pShot);
    if (pShot == NULL || pShot == pView->script.pShot || pShot == pView->script.pB8) {
        pShot = DynamicCam_ChooseScriptInSequence(pView->p78, 5, &nA, &f1, &f2, &nB, &f3, nPlayer);
    }
    if (pShot != NULL) {
        if (pView->script.pShot != NULL && pView->script.pShot->bAA == 0) {
            CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 1.0f, f2, nB, f3);
        } else {
            CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, nA, f1, f2, nB, f3);
        }
        pView->script.bCF = 0;
    } else {
        pShot = DynamicCam_ChooseScript(nPlayer, 5, NULL);
        if (pShot != NULL) {
            if (pView->script.pShot != NULL && pView->script.pShot->bAA == 0) {
                CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 1.0f, 50.0f,
                                               0x19, 0.0f);
            } else {
                CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 0.0f, 50.0f,
                                               0x19, 0.0f);
            }
            pView->script.bCF = 0;
        } else if (pView->script.pShot != &pView->shot19C) {
            pView->script.pB8 = pView->script.pShot;
            pView->script.pShot = NULL;
            pView->script.fCamTime = 0.0f;
            pView->script.bCF = 0;
            CameraScript_RecordCurrentCam(&pView->shot19C, pCam, pSub, nPlayer, &pView->script, 0);
            pView->script.pShot = &pView->shot19C;
            pView->script.pShot->bAD = 5;
            pView->script.pNextShot = NULL;
            pView->script.fCamTime = 0.0f;
            pView->script.nE0 = 0x19;
            pView->script.bCF = 1;
            pView->script.pShot->f94 = 0.0f;
            pView->script.pShot->f98 = 0.0f;
        }
    }
    pView->script.n114 = 1;
    pView->script.n110 = 1;
}

// The post-shot camera's final cut has been made (script.n114, set by
// GolfCamera_CutToGolferDoneAnimatingCam).
u8 GolfCamera_IsPostShotCamFinalCutDone(View* pView) {
    return pView->script.n114 > 0;
}

// Pick the swing camera kind (View.n260; 0 none) for the shot about to be hit
// (STATEFUNC_SwingUpdate). With session flags 0x4000 and 0x8000 both set: player 0 gets kind 2 and
// player 1 kind 11 on the tee, any other shot none. Otherwise none in split screen, in a replay,
// with gpGame->b28A clear, off the tee with a long club (over 8), from lies 6..8, for a shot kind
// other than 1, or for a CPU player not on the default swing camera whose next kind is 1, 6, 10 or
// 11. Else a shot powered at least the tuning's f9C gets one: kind 11 above fB4 with a short club;
// else, by chance (fB0, fAC or fA8 percent above fA4, fA0 or f9C), the player's next kind in turn
// (n1EC, 1..11). Kind 11 falls back to 1 (next 2) off the tee or with a long club; a scripted
// GameBreaker (fn_8006BEA4) makes it kind 7 about half the time. A kind chosen stops the pad's
// rumble and starts the special-shot audio.
void GolfCamera_ChooseSpecialSwing(View* pView, int nPlayer) {
    u8 bSwingCam = 0;
    int bShortClub;     // fake match: int, tested as (u8); a u8 local truncates it where it is set
    f32* pCam;
    f32 fPower;
    pCam = CameraController_GetCameraOrigin(pView);
    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) {
        if (nPlayer == 0 && gPlayers[nPlayer].ball.nLie == 0) {
            pView->n260 = 2;
            SW_KillVibration(nPlayer);
            Gaud_InitSpecialShot(nPlayer);
            return;
        }
        if (nPlayer == 1 && gPlayers[nPlayer].ball.nLie == 0) {
            pView->n260 = 11;
            SW_KillVibration(nPlayer);
            Gaud_InitSpecialShot(nPlayer);
            return;
        }
        pView->n260 = 0;
        return;
    }
    fPower = SW_vGetNonPowerAttributeAffectedShotPower(nPlayer);
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
    } else if (!CameraScript_IsDefaultSwingCam(pView->script.pShot, nPlayer, pCam) && Player_IsCPU(nPlayer)
               && (gGolfCamState->n1EC[nPlayer] == 1 || gGolfCamState->n1EC[nPlayer] == 6
                   || gGolfCamState->n1EC[nPlayer] == 10 || gGolfCamState->n1EC[nPlayer] == 11)) {
        bSwingCam = 0;
    } else if (fPower >= lbl_80281F78->f9C) {
        bSwingCam = 1;
    }
    if (bSwingCam) {
        if (fPower > lbl_80281F78->fB4 && (u8)bShortClub) {
            pView->n260 = 11;
        } else if (fPower > lbl_80281F78->fA4) {
            if ((f32)(s32)(Misc_RandFunc(1) % 100) < lbl_80281F78->fB0) {
                pView->n260 = gGolfCamState->n1EC[nPlayer];
                gGolfCamState->n1EC[nPlayer]++;
                if (gGolfCamState->n1EC[nPlayer] >= 12) {
                    gGolfCamState->n1EC[nPlayer] = 1;
                }
            }
        } else if (fPower > lbl_80281F78->fA0) {
            if ((f32)(s32)(Misc_RandFunc(1) % 100) < lbl_80281F78->fAC) {
                pView->n260 = gGolfCamState->n1EC[nPlayer];
                gGolfCamState->n1EC[nPlayer]++;
                if (gGolfCamState->n1EC[nPlayer] >= 12) {
                    gGolfCamState->n1EC[nPlayer] = 1;
                }
            }
        } else if (fPower > lbl_80281F78->f9C) {
            if ((f32)(s32)(Misc_RandFunc(1) % 100) < lbl_80281F78->fA8) {
                pView->n260 = gGolfCamState->n1EC[nPlayer];
                gGolfCamState->n1EC[nPlayer]++;
                if (gGolfCamState->n1EC[nPlayer] >= 12) {
                    gGolfCamState->n1EC[nPlayer] = 1;
                }
            }
        }
        if (pView->n260 == 11 && (gPlayers[nPlayer].ball.nLie != 0 || !(u8)bShortClub)) {
            pView->n260 = 1;
            gGolfCamState->n1EC[nPlayer] = 2;
        }
        if (fn_8006BEA4() && Misc_RandFunc(1) % 100 > 50) {
            pView->n260 = 7;
            gGolfCamState->n1EC[nPlayer] = 8;
            if (gGolfCamState->n1EC[nPlayer] >= 12) {
                gGolfCamState->n1EC[nPlayer] = 1;
            }
        }
        if (pView->n260 != 0) {
            SW_KillVibration(nPlayer);
            Gaud_InitSpecialShot(nPlayer);
        }
    } else {
        pView->n260 = 0;
    }
}

// How many more replay angles the replay swing camera cuts to for the swing camera kind
// (View.n260): 2 for kinds 2, 8 and 11, 1 for kind 5, none otherwise. STATEFUNC_ReplaySwingUpdate
// replays the swing until script.n110 reaches it.
int GolfCamera_HowManyReplaySwings(View* pView) {
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

// The time rate the replay swing plays at (STATEFUNC_ReplaySwingInit and Update hand it to
// GameEffects_SetSuperSlowMo), by the swing camera kind (View.n260): 0.5 for kind 2; for kinds 8,
// 11 and 5, 0.5 at first, then 0.2, 0.65 and 0.2 once 2 (kind 5: 1) replay angles are done
// (fn_800C4518); kinds 15 and 16 switch between 0.2 and 1.5 at the top of the backswing
// (script.f10C); kind 7 the shared state's f64 while b5A (the heart beat camera) is on; 1
// otherwise.
f32 GolfCamera_ReplaySwingSpeed(View* pView) {
    switch (pView->n260) {
    case 2:
        return 0.5f;
    case 8:
        if (GolfCamera_NumCompletedReplayCams(pView) >= 2) {
            return 0.2f;
        }
        return 0.5f;
    case 11:
        if (GolfCamera_NumCompletedReplayCams(pView) >= 2) {
            return 0.65f;
        }
        return 0.5f;
    case 5:
        if (GolfCamera_NumCompletedReplayCams(pView) >= 1) {
            return 0.2f;
        }
        return 0.5f;
    case 15:
        if (pView->script.f10C > 0.0f) {
            return 1.5f;
        }
        return 0.2f;
    case 16:
        if (pView->script.f10C > 0.0f) {
            return 0.2f;
        }
        return 1.5f;
    case 4:
    case 9:
        return 1.0f;
    case 7:
        if (gGolfCamState->b5A) {
            return gGolfCamState->f64;
        }
        return 1.0f;
    case 3:     // the default, but listed in the original (its own entry in the jump table)
        return 1.0f;
    }
    return 1.0f;
}

// Called when the hole is restarted (GM_RestartHole): turns the matrix camera and the super zoom
// off (GolfCamera_DisableMatrixCam, GolfCamera_DisableSuperZoomCam).
void GolfCamera_RestartHole(void) {
    GolfCamera_DisableMatrixCam();
    GolfCamera_DisableSuperZoomCam();
}

// The comic (3-screen) camera is on (the shared state's b56); 0 before the state exists.
u8 GolfCamera_bIs3ScreenCamOn(void) {
    if (gGolfCamState == NULL) {
        return 0;
    }
    return gGolfCamState->b56;
}

// The comic camera is on (b56) and moving between panels (ComicCam_IsScreenFrozen); 0 before the
// shared state exists. GameEffects_AdjustTimeRate stops time for it.
u8 GolfCamera_bIs3ScreenFreezeOn(void) {
    int bOn;
    if (gGolfCamState == NULL) {
        return 0;
    }
    bOn = 0;
    if (gGolfCamState->b56 && ComicCam_IsScreenFrozen()) {
        bOn = 1;
    }
    return bOn;
}

// The matrix camera is running (b54) or the camera script's matrix mode is on (b55,
// GolfCamera_SetCameraMatrixMode); 0 before the shared state exists. The ball-flight camera then
// takes its time step from the matrix camera's tick.
u8 GolfCamera_IsMatrixCamActive(void) {
    if (gGolfCamState == NULL) {
        return 0;
    }
    return gGolfCamState->b54 || gGolfCamState->b55;
}

// The super zoom is running (b58); 0 before the shared state exists.
u8 GolfCamera_IsSuperZoomCamActive(void) {
    if (gGolfCamState == NULL) {
        return 0;
    }
    return gGolfCamState->b58;
}

// The slow-motion swing camera is on (b59, set when the replay camera cuts to shot kind 0x22 for
// swing kinds 15 and 16); 0 before the shared state exists.
u8 GolfCamera_IsSlowMoSwingCamActive(void) {
    if (gGolfCamState == NULL) {
        return 0;
    }
    return gGolfCamState->b59;
}

// Time is frozen for a camera: the matrix camera (b54), the super zoom (b58) or the camera script's
// matrix mode (b55) is on; 0 before the shared state exists.
u8 GolfCamera_IsFreezeTimeActive(void) {
    if (gGolfCamState == NULL) {
        return 0;
    }
    if (gGolfCamState->b54 || gGolfCamState->b58 || gGolfCamState->b55) {
        return 1;
    }
    return 0;
}

// Clear b54 (the matrix camera is running), if the shared state exists.
void GolfCamera_DisableMatrixCam(void) {
    if (gGolfCamState != NULL) {
        gGolfCamState->b54 = 0;
    }
}

// Clear b58 (the super zoom is running), if the shared state exists.
void GolfCamera_DisableSuperZoomCam(void) {
    if (gGolfCamState != NULL) {
        gGolfCamState->b58 = 0;
    }
}

// Clear b59 (the slow-motion swing camera is on), if the shared state exists. EA's name spells
// Swing as Sing.
void GolfCamera_DisableSlowMoSingCam(void) {
    if (gGolfCamState != NULL) {
        gGolfCamState->b59 = 0;
    }
}

// Clear b5A (the heart beat camera is beating), if the shared state exists.
void GolfCamera_DisableHeartBeatCam(void) {
    if (gGolfCamState != NULL) {
        gGolfCamState->b5A = 0;
    }
}

// Is the post-shot camera done (GM_vIsPostShotCameraDone): its one cut made (script.n110 is 1), no
// next shot queued, and no current shot or one past its length (f4C).
u8 GolfCamera_IsPostShotCamDone(View* pView) {
    if (pView->script.n110 == 1 && pView->script.pNextShot == NULL
        && (pView->script.pShot == NULL || pView->script.pShot->f4C < pView->script.fCamTime)) {
        return 1;
    }
    return 0;
}

// Is a time-triggered camera still to come in the pre-shot sequence: the sequence (p74) has a shot
// of kind 0x17 and it has not started yet (script.n110 under 1; GolfCamera_ProcessPreShotCamera
// sets it when kind 0x17 starts). Never with the fancy pre-shot cameras skipped (b268 is 1,
// GolfCamera_SetSkipFancyPreshotCams).
u8 GolfCamera_IsThereACameraGoingToBeTimeTriggered(View* pView, int nPlayer) {
    if (pView->b268 == 1) {
        return 0;
    }
    if (DynamicCam_ChooseScriptInSequence(pView->p74, 0x17, NULL, NULL, NULL, NULL, NULL, nPlayer) != NULL) {
        return pView->script.n110 < 1;
    }
    return 0;
}

// Does the pre-shot sequence (p74) have a shot of the kind queued in script.nE0 (0x19 means none)?
u8 GolfCamera_IsThereAPostPreShotCamera(View* pView, int nPlayer) {
    if (pView->script.nE0 != 0x19 && pView->p74 != NULL
        && DynamicCam_ChooseScriptInSequence(pView->p74, pView->script.nE0, NULL, NULL, NULL, NULL, NULL, nPlayer) != NULL) {
        return 1;
    }
    return 0;
}

// Can the pre-shot camera start its fade, fLeft seconds before it ends (STATEFUNC_PreShotUpdate):
// yes with no sequence or no current shot, or once script.n114 is set
// (GolfCamera_ForcePreShotEnding); no while a time-triggered camera
// (GolfCamera_IsThereACameraGoingToBeTimeTriggered) or a queued one
// (GolfCamera_IsThereAPostPreShotCamera) is still to come; else when less than fLeft is left before
// the next shot (GolfCamera_GetTimeToNextShot), or with none queued, when both the current shot's
// length f4C and script.fE4 are within fLeft of script.f98.
u8 GolfCamera_IsPreShotCamReadyForFade(View* pView, int nPlayer, f32 fLeft) {
    if (pView->p74 == NULL || pView->script.pShot == NULL) {
        return 1;
    }
    if (pView->script.n114 > 0) {
        return 1;
    }
    if (GolfCamera_IsThereACameraGoingToBeTimeTriggered(pView, nPlayer)) {
        return 0;
    }
    if (GolfCamera_IsThereAPostPreShotCamera(pView, nPlayer)) {
        return 0;
    }
    if (pView->script.pNextShot != NULL) {
        return GolfCamera_GetTimeToNextShot(pView) < fLeft;
    }
    if (pView->script.pShot->f4C - pView->script.f98 < fLeft
        && pView->script.fE4 - pView->script.f98 < fLeft) {
        return 1;
    }
    return 0;
}

// End the pre-shot camera now: set script.n114, which GolfCamera_IsPreShotCamReadyForFade takes as
// ready (STATEFUNC_PreShotUpdate).
void GolfCamera_ForcePreShotEnding(View* pView) {
    pView->script.n114 = 1;
}

// Does the next shot (else the current one) follow the golfer: its move kind bAC is one of 1..7
// (fn_8003DC78)? 0 with neither.
u8 GolfCamera_IsCameraTrackingPlayer(View* pView) {
    if (pView->script.pNextShot != NULL) {
        return fn_8003DC78(pView->script.pNextShot) != 0;
    }
    if (pView->script.pShot != NULL) {
        return fn_8003DC78(pView->script.pShot) != 0;
    }
    return 0;
}

// Set b268, which skips the fancy pre-shot cameras (GolfCamera_InitPreShotCamera and
// GolfCamera_IsThereACameraGoingToBeTimeTriggered read it); GM_PlayerTakeMulligan and
// STATEFUNC_PreShotUpdate set it.
void GolfCamera_SetSkipFancyPreshotCams(View* pView, int a) {
    pView->b268 = a;
}

// Is the shot set-up camera done (STATEFUNC_ShotSetupUpdate): no current shot, no next shot queued,
// or more than 5 seconds on this one.
u8 GolfCamera_IsSetUpCameraDone(View* pView) {
    if (pView->script.pShot == NULL || pView->script.pNextShot == NULL || pView->script.fCamTime > 5.0f) {
        return 1;
    }
    return 0;
}

// The swing camera kind (View.n260) GolfCamera_ChooseSpecialSwing chose for this shot; the
// special-shot audio (Gaud_InitSpecialShot and the rest) asks.
int GolfCamera_GetSpecialSwingType(View* pView) {
    return pView->n260;
}

// Turn the camera script's matrix mode (b55) on or off; while on, GolfCamera_IsFreezeTimeActive
// answers yes. Unlike the other setters it does not test for the shared state.
void GolfCamera_SetCameraMatrixMode(int a) {
    gGolfCamState->b55 = a;
}

// The camera script's matrix mode (b55, GolfCamera_SetCameraMatrixMode).
u8 GolfCamera_IsScriptMatrixModeOn(void) {
    return gGolfCamState->b55;
}

// Set b269: whether the golfer's post-shot animations are shown (GolfCamera_ShowPostShotAnimations
// reads it).
void GolfCamera_SetPostShowPostShotAnimations(View* pView, int a) {
    pView->b269 = a;
}

// Are the golfer's post-shot animations shown (b269, GolfCamera_SetPostShowPostShotAnimations)?
u8 GolfCamera_ShowPostShotAnimations(View* pView) {
    return pView->b269;
}

// Set b26A (TW07: whether the post-shot ball removal is shown); STATEFUNC_InTheHoleUpdate sets it
// and reads it back (GolfCamera_ShowPostRemoveBall).
void GolfCamera_SetPostShowRemoveBall(View* pView, int a) {
    pView->b26A = a;
}

// Is the post-shot ball removal shown (b26A, GolfCamera_SetPostShowRemoveBall)?
u8 GolfCamera_ShowPostRemoveBall(View* pView) {
    return pView->b26A;
}

// Stop every special swing camera when a putt is conceded (STATEFUNC_ConcededInit): the comic
// camera (fn_800C1790), the slow-motion swing, the matrix and the heart beat cameras.
void GolfCamera_AbortAllSpecialSwings(View* pView, int nPlayer) {
    GolfCamera_TurnOffComicCam(pView, nPlayer);
    GolfCamera_DisableSlowMoSingCam();
    GolfCamera_DisableMatrixCam();
    GolfCamera_DisableHeartBeatCam();
}

// Should the ball's flight wait this frame (GM_SimulateBallMovement skips its physics steps when
// gpGame->n294 is set): the ball is behind the camera on the flat (the look and to-ball directions'
// dot product not above 0) and the camera follows the golfer (GolfCamera_IsCameraTrackingPlayer).
// Never without a current shot, for a shot of kind 3 (bAD), or once script.f98 passes 5.
u8 GolfCamera_IsBallFlightPaused(View* pView, int nPlayer) {
    f32 vLook[4];
    f32 vBall[4];
    if (pView->script.pShot == NULL) {
        return 0;
    }
    if (pView->script.pShot->bAD == 3) {
        return 0;
    }
    if (pView->script.f98 > 5.0f) {
        return 0;
    }
    GolfCam_Vec3Sub(pView->v10, pView->v0, vLook);
    GolfCam_Vec3Sub(gPlayers[nPlayer].ball.vPos, pView->v0, vBall);
    vLook[1] = 0.0f;
    vBall[1] = 0.0f;
    if (vLook[0] != 0.0f || vLook[1] != 0.0f || vLook[2] != 0.0f) {
        LLMath_Normalize3(vLook, vLook);
    }
    if (vBall[0] != 0.0f || vBall[1] != 0.0f || vBall[2] != 0.0f) {
        LLMath_Normalize3(vBall, vBall);
    }
    if (Vec3_Dot(vLook, vBall) > 0.0f) {
        return 0;
    }
    return GolfCamera_IsCameraTrackingPlayer(pView);
}

// The golfer-done-animating cut has been made (script.n114, set by
// GolfCamera_CutToGolferDoneAnimatingCam); fn_800637C4 asks in camera modes 15 and 16.
u8 GolfCamera_IsGolferDoneAnimating(View* pView) {
    return pView->script.n114 != 0;
}

// Clear the shared state's special camera flags b54..b5B (the matrix camera, the script's matrix
// mode, the comic camera, the super zoom, the slow-motion swing, the heart beat and the rest), if
// the state exists. The hole loader (fn_8006F518) calls it.
void GolfCamera_ResetSpecialCameraStates(void) {
    if (gGolfCamState != NULL) {
        gGolfCamState->b54 = 0;
        gGolfCamState->b55 = 0;
        gGolfCamState->b56 = 0;
        gGolfCamState->b57 = 0;
        gGolfCamState->b58 = 0;
        gGolfCamState->b59 = 0;
        gGolfCamState->b5A = 0;
        gGolfCamState->b5B = 0;
    }
}

// Is the zoom-to-aim camera done (STATEFUNC_ZoomUpdate): in modes 1 and 2
// (GolfCamera_InitZoomToAimCamera, GolfCamera_InitGreenZoomToAimCamera) once script.f108 reaches 1;
// in any other mode at once.
u8 GolfCamera_IsZoomCamDone(View* pView, int nPlayer) {
    if (pView->nCurCamera == 1) {
        return pView->script.f108 >= 1.0f;
    }
    if (pView->nCurCamera == 2) {
        return pView->script.f108 >= 1.0f;
    }
    return 1;
}

// The time left before the camera script moves on to its next shot (script.f8C less fCamTime); 0
// with no next shot queued.
f32 GolfCamera_GetTimeToNextShot(View* pView) {
    if (pView->script.pNextShot == NULL) {
        return 0.0f;
    }
    return pView->script.f8C - pView->script.fCamTime;
}

// Adds two vectors (three floats) into pOut.
#ifdef __MWERKS__
asm void GolfCam_Vec3Add(register f32* pA, register f32* pB, register f32* pOut) {
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
void GolfCam_Vec3Add(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
}
#endif

// Subtracts pB from pA (three floats) into pOut.
#ifdef __MWERKS__
asm void GolfCam_Vec3Sub(register f32* pA, register f32* pB, register f32* pOut) {
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
void GolfCam_Vec3Sub(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// Negates a vector (three floats) into pOut.
#ifdef __MWERKS__
asm void GolfCam_Vec3Negate(register f32* pA, register f32* pOut) {
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
void GolfCam_Vec3Negate(f32* pA, f32* pOut) {
    pOut[0] = -pA[0];
    pOut[1] = -pA[1];
    pOut[2] = -pA[2];
}
#endif

// An animation event's time in the character's blend tree (0 without a character).
f32 GolfCam_GetBlendTagTime(Character* pChar, u64 uEvent) {
    if (pChar == NULL) {
        return 0.0f;
    }
    return SKABlender_GetTagTime(&pChar->blend, uEvent);
}

// GameEffects.h's inline (TW07), out of line here: a GameBreaker is on and it is a predicted one
// (nGBType 1). The ball-flight camera asks.
u8 GameEffects_IsPredictedGameBreakerOn(void) {
    int bOn = 0;
    if (gGameEffects.bGameBreaker && gGameEffects.nGBType == 1) {
        bOn = 1;
    }
    return bOn;
}
