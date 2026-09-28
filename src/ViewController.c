// ViewController.c (our name): EA's GoViewCont.c (Golf/Hi-Rendering), whose TW07 copy has the
// same functions in the same order, ViewController_ResetAll to ViewController_TurnOnViewController.
// The four views on screen (gViewControllers): each view controller owns a render context (render
// camera: lens, viewport, frame buffer) and the view's camera controller (View), and follows one
// player. These functions set a view up and shut it down, hand out its parts, and each frame run
// its camera controller and aim the render context's lens from it. After them come helpers TW07
// has as header inlines (GoViewport.h, GoCamera.h), out of line here, and the camera-controller
// getters of TW07's GoCamCont.h (CameraController_GetCameraViewScale to GetCameraOrigin).

#include "unsorted/cull.h"

// .bss/.sbss in reverse address order (CodeWarrior lays them out last-defined-first)
ViewController gViewControllers[4];
ViewController* gpCurViewController;               // the current view's controller
int gCurViewControllerID;                           // the current view

void  fn_80062E40(View* pView);                     // set up a camera controller
void  fn_80038010(u8 a, int n, f32* pVec);
void  fn_80038054(u8 a, int n, f32 f1, f32 f2);
void  CameraController_Idle(View* pView, int nPlayer);
void  CA_vSetLookAtSide(CamLens* pLens, f32* pPos, f32* pAt, f32* pUp);
void  Camera_SetCameraPositionAndTargetWithOffsetAndScale(CamLens* pLens, f32* pPos, f32* pAt, f32* pF5C, f32* pF50);
void  Camera_SetCameraYawPitchRollAndPosition(CamLens* pLens, f32* pPos, f32* pAngles);
void  fn_80013D68(void* pCamera);
void  VM_vUpdateInternalViewportRectData(f32* pRect);
void  mat44flt_EulerAngles(f32 (*pMtx)[4], f32 fYaw, f32 fPitch, f32 fRoll);   // UMemPool.c
void  LLMath_InvertNormalized(f32 (*pSrc)[4], f32 (*pDst)[4]);   // UMemPool.c: rotation+translation inverse
void  CA_vSetDefaultScalingVectors(CamLens* pLens);

ViewController* ViewController_GetDataPtr(int nView);
s32  RC_GetCurrentFrameBuffer(void);
f32* CameraController_GetCameraViewScale(View* pView);
f32* CameraController_GetCameraViewOffset(View* pView);

// Only clears each of the four view controllers' active flag (bActive).
void ViewController_ResetAll(void) {
    int i;

    for (i = 0; i < 4; i++) {
        gViewControllers[i].bActive = 0;
    }
}

// Makes view nView the current view (the one ViewController_GetCurrentViewController and
// ViewController_GetCurrentViewControllerID return).
void ViewController_SetCurrentViewController(int nView) {
    gCurViewControllerID = nView;
    gpCurViewController = &gViewControllers[nView];
}

ViewController* ViewController_GetCurrentViewController(void) {
    return gpCurViewController;
}

ViewController* ViewController_GetIndexedViewController(int nView) {
    return &gViewControllers[nView];
}

int ViewController_GetCurrentViewControllerID(void) {
    return gCurViewControllerID;
}

// Sets view nView up on the screen rectangle fLeft, fTop, fWidth, fHeight (fractions of the
// screen): a new lens and viewport, a render context drawing them to the current frame buffer, and
// a reset camera controller. It follows no player yet (5), is marked active, and its two
// post-effect settings are switched off (GoPostFx.c fn_80038010, fn_80038054).
void ViewController_Init(int nView, f32 fLeft, f32 fTop, f32 fWidth, f32 fHeight) {
    ViewController* pViewController;
    CamLens* pLens;
    f32* pViewport;

    pViewController = ViewController_GetDataPtr(nView);
    pLens = CA_spCreateCamera();
    pViewport = VM_spCreateViewport();
    VM_vSetViewportRect(pViewport, fLeft, fTop, fWidth, fHeight);
    // port: RC_GetCurrentFrameBuffer is typed s32, but its value is the frame buffer
    pViewController->pCamera =
        RC_spCreateRenderCtx(pLens, (GoFrameBuf*)RC_GetCurrentFrameBuffer(), pViewport);
    fn_80062E40(&pViewController->view);
    pViewController->nPlayer = 5;
    pViewController->bActive = 1;
    fn_80038010(0, nView, NULL);
    fn_80038054(0, nView, 0.0f, 0.0f);
}

// The controller of view nView (one per split-screen view).
ViewController* ViewController_GetDataPtr(int nView) {
    return &gViewControllers[nView];
}

// Shuts view nView down: frees its render context's lens and viewport, then the render context, and
// marks it inactive.
void ViewController_Delete(int nView) {
    ViewController* pViewController;

    pViewController = ViewController_GetDataPtr(nView);
    CA_vReleaseCamera(Camera_GetLens(pViewController->pCamera));
    VM_vReleaseViewport(RC_spGetRenderCtxViewport(pViewController->pCamera));
    RC_vReleaseRenderCtx(pViewController->pCamera);
    pViewController->bActive = 0;
}

// Runs view nView's camera controller for its player and aims the render context's lens from it:
// with the controller's side vector (v20) for camera 2 and for shots that aim at a point, with
// offset and scale for camera 4, and by angles (Camera_SetCameraYawPitchRollAndPosition) when the
// script's shot aims by angles. Then recomputes the render context's screen and transformation
// matrices.
void ViewController_Update(int nView) {
    void* pRenderContext;
    View* pCameraController;

    pRenderContext = ViewController_GetRenderContext(nView);
    pCameraController = ViewController_GetCameraControl(nView);
    CameraController_Idle(pCameraController, ViewController_GetActivePlayerNumber(nView));
    if (pCameraController->nCurCamera == 2) {
        CA_vSetLookAtSide(Camera_GetLens(pRenderContext),
                          CameraController_GetCameraOrigin(pCameraController),
                          CameraController_GetCameraLookPoint(pCameraController),
                          pCameraController->v20);
    } else if (CameraController_IsFlybyDone(pCameraController)) {
        if (pCameraController->nCurCamera == 4) {
            Camera_SetCameraPositionAndTargetWithOffsetAndScale(
                Camera_GetLens(pRenderContext), CameraController_GetCameraOrigin(pCameraController),
                CameraController_GetCameraLookPoint(pCameraController),
                CameraController_GetCameraViewOffset(pCameraController),
                CameraController_GetCameraViewScale(pCameraController));
        } else {
            CA_vSetLookAtSide(Camera_GetLens(pRenderContext),
                              CameraController_GetCameraOrigin(pCameraController),
                              CameraController_GetCameraLookPoint(pCameraController),
                              pCameraController->v20);
        }
    } else {
        Camera_SetCameraYawPitchRollAndPosition(Camera_GetLens(pRenderContext),
                                                CameraController_GetCameraOrigin(pCameraController),
                                                CameraController_GetCameraLookPoint(pCameraController));
    }
    fn_80013D68(pRenderContext);
    RC_vUpdateRenderCtxTransformationMatrices(pRenderContext);
}

// The render context (render camera) view nView draws with.
void* ViewController_GetRenderContext(int nView) {
    return ViewController_GetDataPtr(nView)->pCamera;
}

View* ViewController_GetCameraControl(int nView) {
    return &ViewController_GetDataPtr(nView)->view;
}

void ViewController_SetActivePlayerNumber(int nView, int nPlayer) {
    ViewController_GetDataPtr(nView)->nPlayer = nPlayer;
}

// The player view nView follows, as ViewController_SetActivePlayerNumber set it (5 after
// ViewController_Init: none yet).
int ViewController_GetActivePlayerNumber(int nView) {
    return ViewController_GetDataPtr(nView)->nPlayer;
}

u8 ViewController_IsActive(int nView) {
    return ViewController_GetDataPtr(nView)->bActive;
}

// Turns view nView on or off (bActive, the flag ViewController_IsActive returns).
void ViewController_TurnOnViewController(int nView, u8 bActive) {
    ViewController_GetDataPtr(nView)->bActive = bActive;
}

// Saves the rectangle of view nView's viewport, which the initial fly-by changes
// (ViewController_RestoreViewportRect puts it back).
void ViewController_SaveViewportRect(int nView) {
    ViewController* pViewController;
    f32* pViewport;

    pViewController = ViewController_GetDataPtr(nView);
    pViewport = RC_spGetRenderCtxViewport(ViewController_GetRenderContext(nView));
    pViewController->f284 = pViewport[0];
    pViewController->f280 = pViewport[1];
    pViewController->f27C = pViewport[2];
    pViewController->f278 = pViewport[3];
}

// Puts back the viewport rectangle ViewController_SaveViewportRect saved for view nView.
void ViewController_RestoreViewportRect(int nView) {
    ViewController* pViewController;

    pViewController = ViewController_GetDataPtr(nView);
    VM_vSetViewportRect(RC_spGetRenderCtxViewport(ViewController_GetRenderContext(nView)),
                        pViewController->f284, pViewController->f280, pViewController->f27C,
                        pViewController->f278);
}

// The frame buffer the current render context draws to.
s32 RC_GetCurrentFrameBuffer(void) {
    // port: RC_GetCurrentFrameBuffer (and fn_80092274's slot) are typed s32, but the value is the
    //       frame buffer
    return (s32)fn_80013E40(*lbl_80280DF0);
}

// Sets a viewport's rectangle (fractions of the frame buffer: 0, 0, 1, 1 is all of it) and
// recomputes its derived values.
void VM_vSetViewportRect(f32* pViewport, f32 fLeft, f32 fTop, f32 fWidth, f32 fHeight) {
    pViewport[0] = fLeft;
    pViewport[1] = fTop;
    pViewport[2] = fWidth;
    pViewport[3] = fHeight;
    VM_vUpdateInternalViewportRectData(pViewport);
}

// Points the lens from pPos with the angles pAngles (yaw, pitch, roll by TW07's argument name):
// resets its scaling, builds its camera-to-world matrix m4 (rotation from the angles, pPos as the
// translation row) and inverts it into the world-to-camera matrix m44.
void Camera_SetCameraYawPitchRollAndPosition(CamLens* pLens, f32* pPos, f32* pAngles) {
    CA_vSetDefaultScalingVectors(pLens);
    mat44flt_EulerAngles(pLens->m4, pAngles[1], pAngles[0], pAngles[2]);
    pLens->m4[3][0] = pPos[0];
    pLens->m4[3][1] = pPos[1];
    pLens->m4[3][2] = pPos[2];
    pLens->m4[3][3] = 1.0f;
    LLMath_InvertNormalized(pLens->m4, pLens->m44);
}

// Sets both of the lens's scaling vectors (m84[0], m84[1]) to 1: no scaling (the scaled look-at of
// GoCamera.c sets others).
void CA_vSetDefaultScalingVectors(CamLens* pLens) {
    pLens->m84[0][0] = 1.0f;
    pLens->m84[0][1] = 1.0f;
    pLens->m84[0][2] = 1.0f;
    pLens->m84[0][3] = 1.0f;
    pLens->m84[1][0] = 1.0f;
    pLens->m84[1][1] = 1.0f;
    pLens->m84[1][2] = 1.0f;
    pLens->m84[1][3] = 1.0f;
}

// The camera's view scale (View.f50, three floats) that camera 4's look-at applies
// (Camera_SetCameraPositionAndTargetWithOffsetAndScale).
f32* CameraController_GetCameraViewScale(View* pView) {
    return &pView->f50;
}

// The camera's view offset (View.f5C, three floats) that camera 4's look-at applies
// (Camera_SetCameraPositionAndTargetWithOffsetAndScale).
f32* CameraController_GetCameraViewOffset(View* pView) {
    return &pView->f5C;
}

// Whether the camera is out of its fly-by: true when its script has no current shot, when that
// shot's bAD is set, or when the shot has no next shot (p40) and the script's move is not kind 1.
// ViewController_Update aims the lens at the look point when true and by the shot's angles when
// false.
u8 CameraController_IsFlybyDone(View* pView) {
    if (pView->script.pShot == NULL) return 1;
    if (pView->script.pShot->bAD) return 1;
    if (pView->script.pShot->p40 == NULL && pView->script.nFade != 1) return 1;
    return 0;
}

// Where the camera looks (View.v10; the pin, for camera 5). During a fly-by
// (CameraController_IsFlybyDone false) ViewController_Update reads it as the yaw, pitch and roll
// instead.
f32* CameraController_GetCameraLookPoint(View* pView) {
    return pView->v10;
}

// The camera's position (View.v0).
f32* CameraController_GetCameraOrigin(View* pView) {
    return pView->v0;
}
