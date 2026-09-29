// GoCamCont.c (TW06's golf/cameras/gocamcont.c, TW07's GoCamCont.c, the CameraController_
// functions in TW07's order): the camera controller of each view (View, TW07's
// CameraController_t): setting it up, its frame (CameraController_Idle runs the current camera
// mode's process from GoGolfCam.c), switching camera modes (CameraController_SetCameraMode),
// camera events that ask for shots (CameraController_PostEvent), the colour fades, camera shake,
// the on-screen tests, which golfer the current view hides, and putting a camera that runs into
// an object back on the fairway. RC_vComputeRenderCtxWorldToPrimitiveCoordinate, a GoRenderCtx.h
// inline in TW07, is compiled here as a function.

#include "golfer.h"
#include "game.h"
#include "camera.h"
#include "unsorted/cull.h"

u8   GolfCamera_IsGolferDoneAnimating(View* pView);
u8   CameraController_PointIsOnScreen(int nPlayer, f32* pPos, f32 fMargin);
u8   CameraController_HideGolfer(int nPlayer, int nView);
void CameraController_SetShakeAmount(View* pView, f32 fF0, f32 fF4);
void CameraController_Vec3Sub(f32* pA, f32* pB, f32* pOut);
void Quat_RotateVector(f32* pQuat, f32* pIn, f32* pOut);      // Quaternion.c: a vector turned by it
void ViewController_SetCurrentViewController(int nView);    // ViewController.c: sets the current view
void fn_80045824(int n);                                // DepthField.c: turns depth-of-field layer n off
void Gaud_CameraShake(u8 nPlayer, u8 bLimit);                // GameAudio.c
f32  Vec4_LengthSqClamped(f32* pV);                              // Swing.c
void CameraController_ShakeCamera(View* pView);
void CameraController_ComputeCurrentSideVector(View* pView);

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f (0x802836E8), before the 0.0f CameraController_InitOneCamera uses first; its body is unknown.
static f32 GoCamCont_StrippedFn(f32 x) {
    return x + 1.0f;
}

// Sets a view's camera controller up: no camera mode (25), no camera sequences or shots, the
// script's clocks, fade, flags, positions and shake cleared, f50..f58 1, the script's v70 (0, 0, 0,
// 1), and the script's recording shot pB4 pointed at the view's own shot19C.
void CameraController_InitOneCamera(View* pView) {
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
    pView->script.fFadeTime = 0.0f;
    pView->script.pShot = NULL;
    pView->script.pNextShot = NULL;
    pView->script.nFade = 0;
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

// Forgets the view's camera sequences (p74, p78, p7C) and shot p80, and clears b268.
void CameraController_ResetCameraState(View* pView) {
    pView->p78 = NULL;
    pView->p7C = NULL;
    pView->p80 = NULL;
    pView->p74 = NULL;
    pView->b268 = 0;
}

// The view's camera controller, every frame: runs the current camera mode's process (mode 25, no
// camera, only runs the colour fade, a finished fade-out staying held); on the golfer's animation
// events 5..14 (not on the CrAP screen, game type 3) starts a camera shake (CamTuning.f204 long,
// f200 strong) with its sound (Gaud_CameraShake); shakes the camera while the shake lasts; keeps
// the side vector v20 nonzero (CameraController_ComputeCurrentSideVector, else (1, 0, 0)) and runs
// the view's fade clock fFade.
void CameraController_Idle(View* pView, int nPlayer) {
    int i;
    int nMove;

    if (pView->nCurCamera != 2) {
        pView->v20[0] = 0.0f;
        pView->v20[1] = 0.0f;
        pView->v20[2] = 0.0f;
        pView->v20[3] = 0.0f;
    }
    switch (pView->nCurCamera) {
    case 10:
        GolfCamera_ProcessFlyByCamera(pView, nPlayer);
        break;
    case 0:
        GolfCamera_ProcessShotSetupCamera(pView, nPlayer);
        break;
    case 11:
        GolfCamera_ProcessPreShotCamera(pView, nPlayer);
        break;
    case 1:
        GolfCamera_ProcessZoomToAimCamera(pView, nPlayer);
        break;
    case 2:
        GolfCamera_ProcessGreenZoomToAimCamera(pView, nPlayer);
        break;
    case 3:
        GolfCamera_ProcessElevatorCamera(pView, nPlayer);
        break;
    case 4:
        GolfCamera_ProcessGreenCamera(pView, nPlayer);
        break;
    case 5:
        GolfCamera_ProcessGreenRollCamera(pView, nPlayer);
        break;
    case 6:
        GolfCamera_ProcessReversePuttCamera(pView, nPlayer);
        break;
    case 7:
        GolfCamera_ProcessKneeCamera(pView, nPlayer);
        break;
    case 8:
        GolfCamera_ProcessPlaceBallCamera(pView, nPlayer);
        break;
    case 9:
        GolfCamera_ProcessSpeedGolfRunCamera(pView, nPlayer);
        break;
    case 12:
        GolfCamera_ProcessSwingCamera(pView, nPlayer);
        break;
    case 13:
        GolfCamera_ProcessReplaySwingCamera(pView, nPlayer);
        break;
    case 14:
        GolfCamera_ProcessBallFlightCamera(pView, nPlayer);
        break;
    case 15:
        GolfCamera_ProcessPostShotCamera(pView, nPlayer);
        break;
    case 16:
        GolfCamera_ProcessInHoleCamera(pView, nPlayer);
        break;
    case 17:
        GolfCamera_ProcessScoreCardCamera(pView, nPlayer);
        break;
    case 18:
        GolfCamera_ProcessTutorialWaitCamera(pView, nPlayer);
        break;
    case 23:
        GolfCamera_ProcessFECamera(pView, nPlayer);
        break;
    case 24:
        GolfCamera_ProcessGolferBoneCamera(pView, nPlayer);
        break;
    case 19:
        GolfCamera_ProcessSteepSlopeCamera(pView, nPlayer);
        break;
    case 20:
        GolfCamera_Process3ScreenCamera(pView, nPlayer);
        break;
    case 21:
        GolfCamera_ProcessHeartBeatCamera(pView, nPlayer);
        break;
    case 22:
        GolfCamera_ProcessShutterCamera(pView, nPlayer);
        break;
    case 25:
        nMove = pView->script.nFade;
        if (gSession.nPaused == 0) {
            // port: one NTSC frame a call, not gSession.fFrameTime
            CamScript_Fade(&pView->script, FRAME_TIME);
            // A colour fade held in state 4 (after fading up) stays on instead of ending.
            if (nMove == 4 && pView->script.nFade == 0) {
                pView->script.nFade = 4;
            }
            pView->script.fFadeTime += FRAME_TIME;
        }
        break;
    }
    if (gPlayers[nPlayer].pChar != NULL && gSession.nGameType != 3) {
        for (i = 0; i <= 9; i++) {
            if (fn_80048574(gPlayers[nPlayer].pChar, i + 5) && fn_80062BB0(gPlayers[nPlayer].pChar, i + 5)) {
                fn_80062B98(gPlayers[nPlayer].pChar, i + 5);
                CameraController_SetShakeAmount(pView, lbl_80281F78->f204, lbl_80281F78->f200);
                Gaud_CameraShake(nPlayer, 0);
            }
        }
    }
    if (pView->script.fF0 > 0.0f && gSession.fFrameTime > 0.0f) {
        CameraController_ShakeCamera(pView);
        pView->script.fF0 -= gSession.fFrameTime;
    }
    if ((f32)Math_Sqrt(Vec4_LengthSqClamped(pView->v20)) == 0.0f) {
        CameraController_ComputeCurrentSideVector(pView);
        if ((f32)Math_Sqrt(Vec4_LengthSqClamped(pView->v20)) == 0.0f) {
            pView->v20[0] = 1.0f;
            pView->v20[1] = 0.0f;
            pView->v20[2] = 0.0f;
        }
    }
    if (pView->bFade) {
        pView->fFade += gSession.fFrameTime;
    }
}

// Switches the view to camera mode nCamera (0..24, the golf cameras of GoGolfCam.c) for the player:
// nothing when it is already in that mode; otherwise the mode's init runs with nView as the current
// view, and the depth-of-field layer nPlayer is turned off (fn_80045824).
void CameraController_SetCameraMode(View* pView, int nCamera, int nPlayer, int nView) {
    int nPrevView;

    CameraController_GetCameraOrigin(pView);
    CameraController_GetCameraLookPoint(pView);
    if (pView->nCurCamera == nCamera) {
        return;
    }
    nPrevView = ViewController_GetCurrentViewControllerID();
    ViewController_SetCurrentViewController(nView);
    switch (nCamera) {
    case 10:
        GolfCamera_InitFlyByCamera(pView, nPlayer);
        break;
    case 0:
        GolfCamera_InitShotSetupCamera(pView, nPlayer);
        break;
    case 11:
        GolfCamera_InitPreShotCamera(pView, nPlayer);
        break;
    case 1:
        GolfCamera_InitZoomToAimCamera(pView, nPlayer);
        break;
    case 2:
        GolfCamera_InitGreenZoomToAimCamera(pView, nPlayer);
        break;
    case 3:
        GolfCamera_InitElevatorCamera(pView, nPlayer);
        break;
    case 8:
        GolfCamera_InitPlaceBallCamera(pView, nPlayer);
        break;
    case 9:
        GolfCamera_InitSpeedGolfRunCamera(pView, nPlayer);
        break;
    case 4:
        GolfCamera_InitGreenCamera(pView, nPlayer);
        break;
    case 5:
        GolfCamera_InitGreenRollCamera(pView, nPlayer);
        break;
    case 6:
        GolfCamera_InitReversePuttCamera(pView, nPlayer);
        break;
    case 7:
        GolfCamera_InitKneeCamera(pView, nPlayer);
        break;
    case 12:
        GolfCamera_InitSwingCamera(pView, nPlayer);
        break;
    case 13:
        GolfCamera_InitReplaySwingCamera(pView, nPlayer);
        break;
    case 14:
        GolfCamera_InitBallFlightCamera(pView, nPlayer);
        break;
    case 15:
        GolfCamera_InitPostShotCamera(pView, nPlayer);
        break;
    case 16:
        GolfCamera_InitInHoleCamera(pView, nPlayer);
        break;
    case 17:
        GolfCamera_InitScoreCardCamera(pView, nPlayer);
        break;
    case 18:
        GolfCamera_InitTutorialWaitCamera(pView, nPlayer);
        break;
    case 23:
        GolfCamera_InitFECamera(pView, nPlayer);
        break;
    case 24:
        GolfCamera_InitGolferBoneCamera(pView, nPlayer);
        break;
    case 19:
        GolfCamera_InitSteepSlopeCamera(pView, nPlayer);
        break;
    case 20:
        GolfCamera_Init3ScreenCamera(pView, nPlayer);
        break;
    case 21:
        GolfCamera_InitHeartBeatCamera(pView, nPlayer);
        break;
    case 22:
        GolfCamera_InitShutterCamera(pView, nPlayer);
        break;
    }
    pView->nCurCamera = nCamera;
    fn_80045824(nPlayer);
    ViewController_SetCurrentViewController(nPrevView);
}

// Starts a shot of kind nKind chosen for the player (DynamicCam_ChooseScript; none leaves the
// script without a shot) on the view, with its follow-on as the next shot, and runs the script's
// first frame at once. Camera mode 24's init starts kind 10 with it.
void CameraController_StartScriptOfKind(View* pView, int nPlayer, int nKind) {
    f32* pPos = CameraController_GetCameraOrigin(pView);
    f32* pAt = CameraController_GetCameraLookPoint(pView);
    CamShot* pShot = DynamicCam_ChooseScript(nPlayer, nKind, NULL);

    if (pShot != NULL) {
        pView->script.pNextShot = pShot->p40;
        if (pShot->p40 != NULL) {
            pView->script.nBC = pShot->p40->bAB;
            pView->script.f8C = pShot->p40->f48;
        }
    }
    pView->script.fCamTime = 0.0f;
    pView->script.pShot = pShot;
    CamScript_RunScript(nPlayer, pPos, pAt, &pView->script, &pView->shot19C, 0, 0.0f);
}

// The player's target is on screen, 0.1 in from the edges.
u8 CameraController_TargetIsOnScreen(int nPlayer) {
    return CameraController_PointIsOnScreen(nPlayer, gPlayers[nPlayer].vTarget, 0.1f);
}

// fake match: stands in for code the original compiled here and the linker stripped: the pool has
// RC_vComputeRenderCtxWorldToPrimitiveCoordinate's constants (-0.0001f, 0.0001f, -10000.0f,
// 10000.0f, 0x802836F8) at this point, not after CameraController_ResetAimMarkerInSwingCamera's;
// the body is unknown.
static f32 GoCamCont_StrippedFn2(f32 x) {
    if (x < -0.0001f || x > 0.0001f) {
        return 1.0f / x;
    }
    if (x < 0.0f) {
        return -10000.0f;
    }
    return 10000.0f;
}

// pPos is on the player's screen, at least fMargin in from every edge.
u8 CameraController_PointIsOnScreen(int nPlayer, f32* pPos, f32 fMargin) {
    f32 fX;
    f32 fY;
    f32 fZ;

    if (RC_vComputeRenderCtxWorldToPrimitiveCoordinate(
            ViewController_GetRenderContext(gPlayers[nPlayer].nView[0]), pPos, &fX, &fY, &fZ)) {
        if (fX < 1.0f - fMargin && fX > fMargin && fY < 1.0f - fMargin && fY > fMargin) {
            return 1;
        }
    }
    return 0;
}

// The player's ball is on screen, 0.1 in from the edges.
u8 CameraController_BallIsOnScreen(int nPlayer) {
    return CameraController_PointIsOnScreen(nPlayer, gPlayers[nPlayer].ball.vPos, 0.1f);
}

// The player whose golfer the current view hides: the first one CameraController_HideGolfer names
// for it, -2 if none. char.c leaves that golfer out when drawing the characters (iPlayer2Clip).
int CameraController_GetClippedGolfer(void) {
    int nView = ViewController_GetCurrentViewControllerID();
    int i = 0;

    while (i < gNumPlayersSetUp) {
        if (CameraController_HideGolfer(i, nView)) {
            return i;
        }
        i++;
    }
    return -2;
}

// The player whose golfer's shadow the current view hides: the same test as
// CameraController_GetClippedGolfer (gomainloop.c draws no shadow for that player).
int CameraController_GetClippedShadow(void) {
    int nView = ViewController_GetCurrentViewControllerID();
    int i = 0;

    while (i < gNumPlayersSetUp) {
        if (CameraController_HideGolfer(i, nView)) {
            return i;
        }
        i++;
    }
    return -2;
}

// Whether view nView hides the player's golfer: the view is the player's own (his nView[0], and
// ViewController_GetActivePlayerNumber gives it to him) and either its current shot does not show
// the golfer (bAA 0) and neither does the next shot it blends to (unless blend kind 5), or the
// camera is in mode 15 or 16 and GolfCamera_IsGolferDoneAnimating holds; with no shot, only in camera mode 4.
u8 CameraController_HideGolfer(int nPlayer, int nView) {
    View* pView;

    if (gPlayers[nPlayer].nView[0] == nView && ViewController_GetActivePlayerNumber(nView) == nPlayer) {
        pView = ViewController_GetCameraControl(gPlayers[nPlayer].nView[0]);
        if (pView->script.pShot != NULL) {
            if ((pView->nCurCamera == 15 || pView->nCurCamera == 16)
                && GolfCamera_IsGolferDoneAnimating(pView)) {
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

// Posts camera event 2 (CameraController_PostEvent) while the player's ball is coming down below
// CamTuning.f90 before its first collision.
void CameraController_CheckForEvents(View* pView, int nPlayer) {
    if (gPlayers[nPlayer].ball.vVel[1] < 0.0f && gPlayers[nPlayer].ball.fHeight < lbl_80281F78->f90
        && gPlayers[nPlayer].ball.nCollideCount < 1) {
        CameraController_PostEvent(pView, 2, nPlayer);
    }
}

// Called by the terrain drawing (GoTerrain.c) when view nView's camera touches the bounds of an
// object centred at pBounds. While the ball is in flight (GS_SIMULATE, with gpGame->b289 set), with
// no next shot and a current shot not of kind 3 at least 0.5 into it, a camera moving towards the
// object (flat, no faster than CamTuning.f1BC) while heading and looking towards it (cosines at
// least f1B4 and f1B8) goes back on the fairway (CamScript_PutBackOnFairway).
void CameraController_CameraCollision(int nView, f32* pBounds) {
    View* pView = ViewController_GetCameraControl(nView);
    int nPlayer = ViewController_GetActivePlayerNumber(nView);
    f32 vObj[4];
    f32 vToObj[4];
    f32 vLook[4];
    f32 vMove[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    f32* pPos;
    f32* pAt;
    f32 fSpeed;
    f32 fMoveCos;
    f32 fLookCos;

    if (pView == NULL) {
        return;
    }
    if (!gpGame->b289) {
        return;
    }
    pPos = CameraController_GetCameraOrigin(pView);
    pAt = CameraController_GetCameraLookPoint(pView);
    if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) != GS_SIMULATE) {   // fake match: (s8), see game.h
        return;
    }
    if (pView->script.pShot != NULL && pView->script.pShot->bAD == 3) {
        return;
    }
    if (pView->script.pNextShot != NULL) {
        return;
    }
    if (pView->script.fCamTime < 0.5f) {
        return;
    }
    vObj[0] = pBounds[0];
    vObj[1] = 0.0f;
    vObj[2] = pBounds[2];
    vMove[0] = pView->script.v60[0];
    vMove[1] = 0.0f;
    vMove[2] = pView->script.v60[2];
    fSpeed = Math_Sqrt(Vec3_LengthSqClamped(vMove));
    CameraController_Vec3Sub(vObj, pPos, vToObj);
    vToObj[1] = 0.0f;
    if (vToObj[0] != 0.0f || vToObj[1] != 0.0f || vToObj[2] != 0.0f) {
        LLMath_Normalize3(vToObj, vToObj);
    }
    if (vMove[0] != 0.0f || vMove[1] != 0.0f || vMove[2] != 0.0f) {
        LLMath_Normalize3(vMove, vMove);
    }
    fMoveCos = Vec3_Dot(vMove, vToObj);
    CameraController_Vec3Sub(pAt, pPos, vLook);
    vLook[1] = 0.0f;
    if (vLook[0] != 0.0f || vLook[1] != 0.0f || vLook[2] != 0.0f) {
        LLMath_Normalize3(vLook, vLook);
    }
    fLookCos = Vec3_Dot(vLook, vToObj);
    if (fSpeed <= 0.0f) {
        return;
    }
    if (fSpeed > lbl_80281F78->f1BC) {
        return;
    }
    if (fMoveCos < lbl_80281F78->f1B4 || fLookCos < lbl_80281F78->f1B8) {
        return;
    }
    CamScript_PutBackOnFairway(&pView->script, pPos, pAt, nPlayer, &pView->shot19C, pPos);
}

// Colour fade state 2 (CamScript_Fade): the colour pVec over the view, its alpha falling from pVec[3]
// to 0 over fTime.
void CameraController_FadeIn(View* pView, f32 fTime, f32* pVec) {
    pView->script.nFade = 2;
    LLMath_CopyVec(pVec, pView->script.vFadeColor);
    pView->script.fFadeTime = 0.0f;
    pView->script.fFadeLength = fTime;
}

// Colour fade state 1 (CamScript_Fade): the colour pVec over the view, its alpha rising from 0 to
// pVec[3] over fTime.
void CameraController_FadeOut(View* pView, f32 fTime, f32* pVec) {
    pView->script.nFade = 1;
    LLMath_CopyVec(pVec, pView->script.vFadeColor);
    pView->script.fFadeTime = 0.0f;
    pView->script.fFadeLength = fTime;
}

// Whether the view's colour fade has finished (states 4 and 5) or is held (3).
u8 CameraController_IsFadeDone(View* pView) {
    if (pView->script.nFade == 5 || pView->script.nFade == 4 || pView->script.nFade == 3) {
        return 1;
    }
    return 0;
}

// Whether a fade-out (state 1) has finished and holds its colour (state 4).
u8 CameraController_IsFadeOutDone(View* pView) {
    return pView->script.nFade == 4;
}

// Whether a colour fade covers the view: fading in (2), fading out (1) or finished fading out (4).
u8 CameraController_IsFadeOn(View* pView) {
    if (pView->script.nFade == 2 || pView->script.nFade == 1 || pView->script.nFade == 4) {
        return 1;
    }
    return 0;
}

// Covers the view with the colour pVec, held (fade state 3) until another fade starts.
void CameraController_HoldFadeColor(View* pView, f32* pVec) {
    pView->script.nFade = 3;
    LLMath_CopyVec(pVec, pView->script.vFadeColor);
}

// Posts camera event nKind to the view: it becomes the view's requested event (CamScript.nC4).
// Without a club (25) only events 0, 5, 8, 11 and 23 are taken. Event 12 first holds the current
// camera (recorded into shot19C) for 0.3 while the view fades to grey (alpha 0.5), then goes on to
// a kind-12 shot chosen for the player; event 7 becomes 10 when the ball lies on surface class 7 or
// 16. While event 12 waits only 5, 8 and 10 replace it; 6 is never taken; 2 and 3 do not replace
// 7, nor 7 them.
void CameraController_PostEvent(View* pView, int nKind, int nPlayer) {
    f32* pPos = CameraController_GetCameraOrigin(pView);
    f32* pAt = CameraController_GetCameraLookPoint(pView);
    f32 vNormal[4];
    f32 vGrey[4] = {0.1f, 0.1f, 0.1f, 0.5f};
    SurfaceType* pSurface;
    CamShot* pShot;
    int nAsked;

    if (gPlayers[nPlayer].nClub == 25 && nKind != 0 && nKind != 8 && nKind != 5 && nKind != 11
        && nKind != 23) {
        return;
    }
    if (nKind == 12 && pView->script.nC4 != 12) {
        pShot = DynamicCam_ChooseScript(nPlayer, 12, NULL);
        if (pShot != NULL) {
            CameraScript_RecordCurrentCam(&pView->shot19C, pPos, pAt, nPlayer, &pView->script, 0);
            pView->shot19C.p40 = pShot;
            CameraScript_InterpToNewScript(&pView->script, &pView->shot19C, nPlayer, pPos, pAt, 5, 0.0f,
                                           100.0f, 25, 0.0f);
            pView->script.nBC = 5;
            pView->script.f8C = 0.3f;
            CameraController_FadeOut(pView, 0.3f, vGrey);
        }
    }
    if (nKind == 7) {
        if (!(fn_8004DBB0(Ter_GetTGD(), gPlayers[nPlayer].ball.vPos, &pSurface, vNormal) < -60000.0f)
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

// Lags a side vector: turns the current direction pA a fifth of the way towards the desired pB
// (both normalised first) into pOut; while the game is paused pA is copied unchanged.
void CameraController_LagSideVector(f32* pA, f32* pB, f32* pOut) {
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
        LLMath_Normalize3(pA, vA);
    } else {
        vA[0] = 0.0f;
        vA[1] = 0.0f;
        vA[2] = 0.0f;
    }
    if (pB[0] != 0.0f || pB[1] != 0.0f || pB[2] != 0.0f) {
        LLMath_Normalize3(pB, vB);
    } else {
        vB[0] = 0.0f;
        vB[1] = 0.0f;
        vB[2] = 0.0f;
    }
    fAngle = Math_Acos(Vec3_Dot(vA, vB) < -1.0f  ? -1.0f
                         : Vec3_Dot(vA, vB) > 1.0f ? 1.0f
                                                      : Vec3_Dot(vA, vB));
    fAngle *= 0.2f;
    vec4flt_CrossProduct(vA, vB, vAxis);
    if (vAxis[0] != 0.0f || vAxis[1] != 0.0f || vAxis[2] != 0.0f) {
        LLMath_Normalize3(vAxis, vAxis);
    }
    Vec3_Scale(fAngle, vAxis, vAxis);
    Quat_BuildFromVector(vAxis, qTurn);
    vA[3] = 0.0f;
    Quat_RotateVector(qTurn, vA, pOut);
}

// The view's side vector v20: square to its flat look direction (v0 to v10) and turned about that
// direction by the script's roll fA8 (0 without a shot).
void CameraController_ComputeCurrentSideVector(View* pView) {
    f32 vDir[4];
    f32 vUp[4] = {0.0f, 1.0f, 0.0f, 0.0f};
    f32 vSide[4];
    f32 qTurn[4];

    CameraController_Vec3Sub(pView->v10, pView->v0, vDir);
    vDir[1] = 0.0f;
    if (vDir[0] == 0.0f && vDir[2] == 0.0f) {
        vDir[0] = 0.01f;
    }
    if (vDir[0] != 0.0f || vDir[1] != 0.0f || vDir[2] != 0.0f) {
        LLMath_Normalize3(vDir, vDir);
    }
    vec4flt_CrossProduct(vUp, vDir, vSide);
    if (pView->script.pShot == NULL) {
        pView->script.fA8 = 0.0f;
    }
    Vec3_Scale(pView->script.fA8, vDir, vDir);
    Quat_BuildFromVector(vDir, qTurn);
    vSide[3] = 0.0f;
    Quat_RotateVector(qTurn, vSide, pView->v20);
}

// Shakes the camera: moves its position a random amount, up to half the shake strength (the
// script's fF4, CameraController_SetShakeAmount) each way.
void CameraController_ShakeCamera(View* pView) {
    pView->v0[0] += pView->script.fF4 * (Misc_RandFuncf(0) - 0.5f);
    pView->v0[1] += pView->script.fF4 * (Misc_RandFuncf(0) - 0.5f);
    pView->v0[2] += pView->script.fF4 * (Misc_RandFuncf(0) - 0.5f);
}

// Starts a camera shake lasting fF0 (the script's fF0, counted down by CameraController_Idle) at
// strength fF4 (fF4).
void CameraController_SetShakeAmount(View* pView, f32 fF0, f32 fF4) {
    pView->script.fF0 = fF0;
    pView->script.fF4 = fF4;
}

// Whether the view's frame buffer is kept rather than cleared: the golf cameras' b56 flag
// (GolfCamera_bIs3ScreenCamOn; 0 before they are set up). gomainloop.c then draws its full-screen
// quad with flags 1 instead of 3.
u8 CameraController_bDontClearFrameBuffer(void) {
    return GolfCamera_bIs3ScreenCamOn();
}

// Restarts the view's current shot: the script cuts to it again (blend 5), so its look-at point
// (the aim marker's lag, CameraScript_LagAimMarker) starts over. stateFunc.c calls it when the
// player picks another target before swinging.
void CameraController_ResetAimMarkerInSwingCamera(View* pView, int nPlayer) {
    f32* pPos = CameraController_GetCameraOrigin(pView);

    CameraScript_InterpToNewScript(&pView->script, pView->script.pShot, nPlayer, pPos,
                                   CameraController_GetCameraLookPoint(pView), 5,
                                   0.0f, 100.0f, 25, 0.0f);
}

// Puts the point pPos through the camera onto the screen: pX and pY from 0 to 1 across it, pZ its
// depth. 0 when the point is behind the camera.
u8 RC_vComputeRenderCtxWorldToPrimitiveCoordinate(void* pCamera, f32* pPos, f32* pX, f32* pY, f32* pZ) {
    f32 v[4];
    u8 bInFront = 1;

    pPos[3] = 1.0f;
    LLMath_mat44fltMultiply(((Camera*)pCamera)->mDC, (Vec4*)pPos, (Vec4*)v);
    if (v[3] >= 0.0f) {
        bInFront = 0;
    }
    if (v[3] < -0.0001f || v[3] > 0.0001f) {
        LLMath_Scale(1.0f / v[3], v, v);
    } else if (v[3] < 0.0f) {
        LLMath_Scale(-10000.0f, v, v);
    } else {
        LLMath_Scale(10000.0f, v, v);
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
asm void CameraController_Vec3Sub(register f32* pA, register f32* pB, register f32* pOut) {
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
void CameraController_Vec3Sub(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif
