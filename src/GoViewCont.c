// GoViewCont.c: the views on screen (up to four), each with its render context, the camera
// controller that aims it and the player it follows: set up, run every frame and shut down. At the
// end, the small render, viewport, lens and camera-controller helpers these functions call.

#include "unsorted/cull.h"

ViewController_t gViewControllers[VIEWCONTROLLER_NUM_VIEWS];
ViewController_t* gpCurViewController;              // the current view's controller
int gCurViewControllerID;                           // the current view

static ViewController_t* ViewController_GetDataPtr(int iViewControllerID);
static void Camera_SetCameraYawPitchRollAndPosition(CamLens* spCamera_p, f32* xCameraPosition_p,
                                                    f32* yawPitchRoll_p);
static f32* CameraController_GetCameraViewScale(View* pCameraController);
static f32* CameraController_GetCameraViewOffset(View* pCameraController);

// Marks every view unused.
void ViewController_ResetAll(void) {
    int i;

    for (i = 0; i < VIEWCONTROLLER_NUM_VIEWS; i++) {
        gViewControllers[i].bActive = 0;
    }
}

// Makes view iViewControllerID the current view (the one ViewController_GetCurrentViewController
// and ViewController_GetCurrentViewControllerID return).
void ViewController_SetCurrentViewController(int iViewControllerID) {
    gCurViewControllerID = iViewControllerID;
    gpCurViewController = &gViewControllers[iViewControllerID];
}

ViewController_t* ViewController_GetCurrentViewController(void) {
    return gpCurViewController;
}

ViewController_t* ViewController_GetIndexedViewController(int iViewControllerID) {
    return &gViewControllers[iViewControllerID];
}

int ViewController_GetCurrentViewControllerID(void) {
    return gCurViewControllerID;
}

// Opens view iViewControllerID on the rectangle fLeft, fTop, fWidth, fHeight (fractions of the frame
// buffer): a new lens and viewport, a render context drawing them to the current frame buffer, and
// a reset camera controller. It follows no player yet, is marked in use, and its colour and blur
// screen effects are switched off.
void ViewController_Init(int iViewControllerID, f32 fLeft, f32 fTop, f32 fWidth, f32 fHeight) {
    ViewController_t* pViewController;
    CamLens* pCamera;
    VM_SViewport* pViewport;

    pViewController = ViewController_GetDataPtr(iViewControllerID);
    pCamera = CA_spCreateCamera();
    pViewport = VM_spCreateViewport();
    VM_vSetViewportRect(pViewport, fLeft, fTop, fWidth, fHeight);
    pViewController->pRenderContext =
        RC_spCreateRenderCtx(pCamera, RC_spGetCurrentRenderCtxFrameBuffer(), pViewport);
    CameraController_InitOneCamera(&pViewController->cameraController);
    pViewController->nPlayer = VIEWCONTROLLER_NO_PLAYER;
    pViewController->bActive = 1;
    PostFx_SetFade(0, iViewControllerID, NULL);
    PostFx_SetBlur(0, iViewControllerID, 0.0f, 0.0f);
}

static ViewController_t* ViewController_GetDataPtr(int iViewControllerID) {
    return &gViewControllers[iViewControllerID];
}

// Shuts view iViewControllerID down: frees its render context's lens and viewport, then the render
// context, and marks it unused.
void ViewController_Delete(int iViewControllerID) {
    ViewController_t* pViewController;

    pViewController = ViewController_GetDataPtr(iViewControllerID);
    CA_vReleaseCamera(Camera_GetLens(pViewController->pRenderContext));
    VM_vReleaseViewport(RC_spGetRenderCtxViewport(pViewController->pRenderContext));
    RC_vReleaseRenderCtx(pViewController->pRenderContext);
    pViewController->bActive = 0;
}

// Runs view iViewControllerID's camera controller for its player and aims the render context's
// lens from it: in the green zoom-to-aim camera, with the controller's side vector; out of a fly-by,
// in the green camera with its view scale and offset, in any other with the side vector; during a
// fly-by, by the yaw, pitch and roll the controller keeps in its look point. Then rebuilds the
// render context's screen values and its transformation matrices.
void ViewController_Update(int iViewControllerID) {
    void* pRenderContext;
    View* pCameraController;

    pRenderContext = ViewController_GetRenderContext(iViewControllerID);
    pCameraController = ViewController_GetCameraControl(iViewControllerID);
    CameraController_Idle(pCameraController, ViewController_GetActivePlayerNumber(iViewControllerID));
    if (pCameraController->nCurCamera == CAMMODE_GREEN_ZOOM_TO_AIM) {
        Camera_SetCameraPositionAndTargetWithSideVector(Camera_GetLens(pRenderContext),
                          CameraController_GetCameraOrigin(pCameraController),
                          CameraController_GetCameraLookPoint(pCameraController),
                          pCameraController->vSide);
    } else if (CameraController_IsFlybyDone(pCameraController)) {
        if (pCameraController->nCurCamera == CAMMODE_GREEN) {
            Camera_SetCameraPositionAndTargetWithOffsetAndScale(
                Camera_GetLens(pRenderContext), CameraController_GetCameraOrigin(pCameraController),
                CameraController_GetCameraLookPoint(pCameraController),
                CameraController_GetCameraViewOffset(pCameraController),
                CameraController_GetCameraViewScale(pCameraController));
        } else {
            Camera_SetCameraPositionAndTargetWithSideVector(Camera_GetLens(pRenderContext),
                              CameraController_GetCameraOrigin(pCameraController),
                              CameraController_GetCameraLookPoint(pCameraController),
                              pCameraController->vSide);
        }
    } else {
        Camera_SetCameraYawPitchRollAndPosition(Camera_GetLens(pRenderContext),
                                                CameraController_GetCameraOrigin(pCameraController),
                                                CameraController_GetCameraLookPoint(pCameraController));
    }
    RC_vUpdateRenderCtxScreen(pRenderContext);
    RC_vUpdateRenderCtxTransformationMatrices(pRenderContext);
}

void* ViewController_GetRenderContext(int iViewControllerID) {
    return ViewController_GetDataPtr(iViewControllerID)->pRenderContext;
}

View* ViewController_GetCameraControl(int iViewControllerID) {
    return &ViewController_GetDataPtr(iViewControllerID)->cameraController;
}

void ViewController_SetActivePlayerNumber(int iViewControllerID, int ePlayerID) {
    ViewController_GetDataPtr(iViewControllerID)->nPlayer = ePlayerID;
}

// The player the view follows (VIEWCONTROLLER_NO_PLAYER from ViewController_Init until one is set).
int ViewController_GetActivePlayerNumber(int iViewControllerID) {
    return ViewController_GetDataPtr(iViewControllerID)->nPlayer;
}

u8 ViewController_IsActive(int iViewControllerID) {
    return ViewController_GetDataPtr(iViewControllerID)->bActive;
}

// Marks the view in use or not (what ViewController_IsActive returns).
void ViewController_TurnOnViewController(int iViewControllerID, u8 active) {
    ViewController_GetDataPtr(iViewControllerID)->bActive = active;
}

// Keeps the rectangle of the view's viewport, which the initial fly-by changes
// (ViewController_RestoreViewportRect puts it back when the fly-by ends).
void ViewController_SaveViewportRect(int iViewControllerID) {
    ViewController_t* pViewController;
    VM_SViewport* pViewport;

    pViewController = ViewController_GetDataPtr(iViewControllerID);
    pViewport = RC_spGetRenderCtxViewport(ViewController_GetRenderContext(iViewControllerID));
    pViewController->fSavedLeft = pViewport->fLeft;
    pViewController->fSavedTop = pViewport->fTop;
    pViewController->fSavedWidth = pViewport->fWidth;
    pViewController->fSavedHeight = pViewport->fHeight;
}

void ViewController_RestoreViewportRect(int iViewControllerID) {
    ViewController_t* pViewController;

    pViewController = ViewController_GetDataPtr(iViewControllerID);
    VM_vSetViewportRect(RC_spGetRenderCtxViewport(ViewController_GetRenderContext(iViewControllerID)),
                        pViewController->fSavedLeft, pViewController->fSavedTop,
                        pViewController->fSavedWidth, pViewController->fSavedHeight);
}

FB_SFrameBuffer* RC_spGetCurrentRenderCtxFrameBuffer(void) {
    return RC_spGetRenderCtxFrameBuffer(*gppCurrentRenderCtx);
}

// Sets a viewport's rectangle (fractions of the frame buffer: 0, 0, 1, 1 is all of it) and works out
// its derived values.
void VM_vSetViewportRect(VM_SViewport* spViewport_p, f32 fLeft_p, f32 fTop_p, f32 fWidth_p, f32 fHeight_p) {
    spViewport_p->fLeft = fLeft_p;
    spViewport_p->fTop = fTop_p;
    spViewport_p->fWidth = fWidth_p;
    spViewport_p->fHeight = fHeight_p;
    VM_vUpdateInternalViewportRectData(spViewport_p);
}

// Points the lens from xCameraPosition_p by the angles yawPitchRoll_p ([1] the yaw, [0] the pitch,
// [2] the roll, as mat44flt_EulerAngles takes them): no scaling, the camera-to-world matrix from the
// angles with the position as its translation row, and its inverse as the world-to-camera matrix.
static void Camera_SetCameraYawPitchRollAndPosition(CamLens* spCamera_p, f32* xCameraPosition_p,
                                                    f32* yawPitchRoll_p) {
    CA_vSetDefaultScalingVectors(spCamera_p);
    mat44flt_EulerAngles(spCamera_p->mViewToWorld, yawPitchRoll_p[1], yawPitchRoll_p[0], yawPitchRoll_p[2]);
    spCamera_p->mViewToWorld[3][0] = xCameraPosition_p[0];
    spCamera_p->mViewToWorld[3][1] = xCameraPosition_p[1];
    spCamera_p->mViewToWorld[3][2] = xCameraPosition_p[2];
    spCamera_p->mViewToWorld[3][3] = 1.0f;
    LLMath_InvertNormalized(spCamera_p->mViewToWorld, spCamera_p->mWorldToView);
}

// Sets both of the lens's scaling vectors to 1: the world is not scaled.
void CA_vSetDefaultScalingVectors(CamLens* spCamera_p) {
    spCamera_p->aScalingVectors[0][0] = 1.0f;
    spCamera_p->aScalingVectors[0][1] = 1.0f;
    spCamera_p->aScalingVectors[0][2] = 1.0f;
    spCamera_p->aScalingVectors[0][3] = 1.0f;
    spCamera_p->aScalingVectors[1][0] = 1.0f;
    spCamera_p->aScalingVectors[1][1] = 1.0f;
    spCamera_p->aScalingVectors[1][2] = 1.0f;
    spCamera_p->aScalingVectors[1][3] = 1.0f;
}

static f32* CameraController_GetCameraViewScale(View* pCameraController) {
    return pCameraController->vViewScale;
}

static f32* CameraController_GetCameraViewOffset(View* pCameraController) {
    return pCameraController->vViewOffset;
}

// Whether the camera is out of a fly-by: true when its script has no current shot, when that shot
// serves a script state (nStateType), or when no shot follows it (pNext) and no fade out is running.
// While it is false, ViewController_Update aims the lens by the angles in the look point (outside
// the green zoom-to-aim camera).
u8 CameraController_IsFlybyDone(View* pCameraController) {
    if (pCameraController->script.pShot == NULL) return 1;
    if (pCameraController->script.pShot->nStateType) return 1;
    if (pCameraController->script.pShot->pNext == NULL && pCameraController->script.nFade != CAMFADE_OUT) {
        return 1;
    }
    return 0;
}

f32* CameraController_GetCameraLookPoint(View* pCameraController) {
    return pCameraController->vLookPoint;
}

f32* CameraController_GetCameraOrigin(View* pCameraController) {
    return pCameraController->vOrigin;
}
