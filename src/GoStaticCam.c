// GoStaticCam.c: the course's static cameras and fly-by routes, loaded from the hole's 'Cact' and
// 'CAMC' objects. The golf cameras pick a static camera whose area holds the ball, and fly a route's
// cameras along a spline at the pace its timing curve sets.

#include "game_types.h"
#include "engine.h"
#include "endian.h"
#include "game.h"
#include "golfer.h"
#include "camera.h"
#include "unsorted/cull.h"

static void StaticCam_LoadCAMCfromStream(UStreamObject* pStreamData);
static void StaticCam_SetupFlybyCameraPointers(CamShot* pCurCamera, CamShot** pPrevCamera,
                                               CamShot** pNextCamera, CamShot** pNextNextCamera);
static u8   StaticCam_CheckHotZone(CamShot* script, int ePlayerID);
static void LLMath_Add3(const f32* src1, const f32* src2, f32* dest);
static void LLMath_Subtract3(const f32* src1, const f32* src2, f32* dest);

StaticCamManager_t* gpStaticCams;   // the hole's static and fly-by cameras (StaticCam_Init)

// Has the hole streamer (StreamManagerHole_RegisterStreamClients) hand this file its 'CAMC'
// objects, the fly-by routes' timing curves.
void StaticCam_RegisterStreamClients(void) {
    Stream_RegisterLoadChunkCallback('CAMC', StaticCam_LoadCAMCfromStream);
}

void StaticCam_UnRegisterStreamClients(void) {
    Stream_UnregisterLoadChunkCallback('CAMC');
}

// The 'CAMC' stream handler: the fly-by routes' timing curves, byte-swapped from the file (a
// two-word header into uUnread and nCurves, then each curve's four words and its keys, 0x22 bytes
// each in the file) into pCurves, allocated here. Frees the object.
static void StaticCam_LoadCAMCfromStream(UStreamObject* pStreamData) {
    SwapField headerElmDef[2] = { { 4, 4 }, { 4, 4 } };
    SwapField curveElmDef[4] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };
    SwapField nodeElmDef[3] = { { 0x20, 4 }, { 1, 1 }, { 1, 1 } };
    void* pSrc;
    void* pDst;
    u32 i;
    u32 j;

    pSrc = pStreamData->pData;
    pDst = &gpStaticCams->uUnread;
    ByteSwap_Records(&pSrc, &pDst, headerElmDef, 2, 1);
    if (gpStaticCams->nCurves != 0) {
        gpStaticCams->pCurves =
            Circular_MemAllocFunc(gpStaticCams->nCurves * sizeof(Curve), MEMTYPE_LOW, 0,
                                  "GoStaticCam.c", 190);
    }
    for (i = 0; i < gpStaticCams->nCurves; i++) {
        pDst = &gpStaticCams->pCurves[i];
        ByteSwap_Records(&pSrc, &pDst, curveElmDef, 4, 1);
        for (j = 0; j < gpStaticCams->pCurves[i].nKeys; j++) {
            pDst = &gpStaticCams->pCurves[i].aKeys[j];
            ByteSwap_Records(&pSrc, &pDst, nodeElmDef, 3, 1);
            pSrc = (u8*)pSrc + 2;   // the keys are 0x22 bytes in the file
        }
    }
    Circular_MemFreeFunc(pStreamData);
}

// Takes a fly-by camera (a 'Cact' object of type 200, from UKernel.c) as the next of aFlyBy, named
// "FlyBy Cam: <its nId>": its route, position, field of view, look angles (degrees to radians, into
// vLook), fInterpTime and fLength (clamped to 0..1), moved along a spline (CAMINTERP_SPLINE) and
// looking by its angles. Until StaticCam_GetFlyByCam links the cameras, pNext and pPrev hold the
// next camera's number and its own, and bHasPrev is set; bLinked is cleared so that the next
// StaticCam_GetFlyByCam links them again. The crowd fly-by's cameras (STATICCAM_CROWD_ROUTE) hide
// the golfer. Frees the object.
void StaticCam_ParseFlybyCameraActor(UStreamObject* pCameraActor) {
    char szName[0x20];
    FlybyCamFromTess_t* camPtr;

    gpStaticCams->bLinked = 0;
    camPtr = (FlybyCamFromTess_t*)pCameraActor->pData;
    sprintf(szName, "FlyBy Cam: %d", camPtr->nId);
    strcpy(gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].szName, szName);
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].nStateType = CAMSTATE_FLYBY;
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].bDynamic = 0;
    if (camPtr->fLength < 0.0f) {
        camPtr->fLength = 0.0f;
    } else if (camPtr->fLength > 1.0f) {
        camPtr->fLength = 1.0f;
    }
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].fLength = camPtr->fLength;
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].fLookSideOffset = 0.0f;
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].fLookUpOffset = 0.0f;
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].vPos[0] = camPtr->aPos[0];
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].vPos[1] = camPtr->aPos[1];
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].vPos[2] = camPtr->aPos[2];
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].fFovStart = PI * (camPtr->fFov / 180.0f);
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].nInterpType = CAMINTERP_SPLINE;
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].fInterpTime = camPtr->fInterpTime;
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].nLookAtKind = CAMPOINT_SHOT_POINT;
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].bHasPrev = 1;
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].vLook[0] = PI * (camPtr->aLook[0] / 180.0f);
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].vLook[1] = PI * (camPtr->aLook[1] / 180.0f);
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].vLook[2] = PI * (camPtr->aLook[2] / 180.0f);
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].pNext = (CamShot*)(uptr)(s32)camPtr->nNext;
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].pPrev = (CamShot*)(uptr)(s32)camPtr->nId;
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].nHeightRef = CAMHEIGHT_NONE;
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].fHeightOffset = 0.0f;
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].pick.nRoute = camPtr->nRoute;
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].fDepthOfField = 0.0f;
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].fBlur = 0.0f;
    gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].nTimeMode = SHOTTIME_NORMAL;
    if (gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].pick.nRoute != STATICCAM_CROWD_ROUTE) {
        gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].bShowGolfer = 1;
    } else {
        gpStaticCams->aFlyBy[gpStaticCams->nFlyBy].bShowGolfer = 0;
    }
    gpStaticCams->nFlyBy++;
    Circular_MemFreeFunc(pCameraActor);
}

// Takes a static camera (a 'Cact' object of type 201, from UKernel.c) as the next of aStatic, named
// "Static Cam: <its index>": its position, fields of view (degrees to radians), area (u.aArea), the
// events it serves and the rest of its StaticCamFromTess_t; a cut (CAMINTERP_CUT) to it, heights 0
// to 10000. The point it looks at, vLook, is one unit from its position along its three angles
// (mat44flt_EulerAngles). Frees the object.
void StaticCam_ParseStaticCameraActor(UStreamObject* pCameraActor) {
    f32 straightAheadVec[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
    char szName[0x20];
    f32 eulerMatrix[4][4];
    StaticCamFromTess_t* camPtr;
    CamShot* pShot;

    camPtr = (StaticCamFromTess_t*)pCameraActor->pData;
    sprintf(szName, "Static Cam: %d", gpStaticCams->nStatic);
    strcpy(gpStaticCams->aStatic[gpStaticCams->nStatic].szName, szName);
    gpStaticCams->aStatic[gpStaticCams->nStatic].nStateType = CAMSTATE_STATIC;
    gpStaticCams->aStatic[gpStaticCams->nStatic].bDynamic = 0;
    gpStaticCams->aStatic[gpStaticCams->nStatic].fLength = camPtr->fLength;
    gpStaticCams->aStatic[gpStaticCams->nStatic].fLookSideOffset = 0.0f;
    gpStaticCams->aStatic[gpStaticCams->nStatic].fLookUpOffset = 0.0f;
    gpStaticCams->aStatic[gpStaticCams->nStatic].vPos[0] = camPtr->aPos[0];
    gpStaticCams->aStatic[gpStaticCams->nStatic].vPos[1] = camPtr->aPos[1];
    gpStaticCams->aStatic[gpStaticCams->nStatic].vPos[2] = camPtr->aPos[2];
    gpStaticCams->aStatic[gpStaticCams->nStatic].fFovStart = PI * (camPtr->fFovStart / 180.0f);
    gpStaticCams->aStatic[gpStaticCams->nStatic].fFovEnd = PI * (camPtr->fFovEnd / 180.0f);
    gpStaticCams->aStatic[gpStaticCams->nStatic].pNext = NULL;
    gpStaticCams->aStatic[gpStaticCams->nStatic].nInterpType = CAMINTERP_CUT;
    gpStaticCams->aStatic[gpStaticCams->nStatic].fInterpTime = camPtr->fInterpTime;
    gpStaticCams->aStatic[gpStaticCams->nStatic].nUnread = camPtr->nUnread;
    gpStaticCams->aStatic[gpStaticCams->nStatic].bHasPrev = 0;
    gpStaticCams->aStatic[gpStaticCams->nStatic].u.aArea[0] = camPtr->aArea[0];
    gpStaticCams->aStatic[gpStaticCams->nStatic].u.aArea[1] = camPtr->aArea[1];
    gpStaticCams->aStatic[gpStaticCams->nStatic].u.aArea[2] = camPtr->aArea[2];
    gpStaticCams->aStatic[gpStaticCams->nStatic].u.aArea[3] = camPtr->aArea[3];
    gpStaticCams->aStatic[gpStaticCams->nStatic].nLookAtKind = camPtr->nLookAtKind;
    gpStaticCams->aStatic[gpStaticCams->nStatic].nHeightRef = CAMHEIGHT_NONE;
    gpStaticCams->aStatic[gpStaticCams->nStatic].fHeightOffset = 0.0f;
    gpStaticCams->aStatic[gpStaticCams->nStatic].bShowGolfer = 1;
    gpStaticCams->aStatic[gpStaticCams->nStatic].fDepthOfField = camPtr->fDepthOfField;
    gpStaticCams->aStatic[gpStaticCams->nStatic].fBlur = 0.0f;
    gpStaticCams->aStatic[gpStaticCams->nStatic].fMinHeight = 0.0f;
    gpStaticCams->aStatic[gpStaticCams->nStatic].fMaxHeight = 10000.0f;
    gpStaticCams->aStatic[gpStaticCams->nStatic].nTimeMode = camPtr->nTimeMode;
    gpStaticCams->aStatic[gpStaticCams->nStatic].pick.uEvents = camPtr->uEvents;
    gpStaticCams->aStatic[gpStaticCams->nStatic].vLook[0] = PI * (camPtr->aAngle[0] / 180.0f);
    gpStaticCams->aStatic[gpStaticCams->nStatic].vLook[1] = PI * (camPtr->aAngle[1] / 180.0f);
    gpStaticCams->aStatic[gpStaticCams->nStatic].vLook[2] = PI * (camPtr->aAngle[2] / 180.0f);
    pShot = &gpStaticCams->aStatic[gpStaticCams->nStatic];
    mat44flt_EulerAngles(pShot->vLook[1], pShot->vLook[0], pShot->vLook[2], eulerMatrix);
    LLMath_mat44fltMultiply33(eulerMatrix, straightAheadVec,
                              gpStaticCams->aStatic[gpStaticCams->nStatic].vLook);
    LLMath_Add3(gpStaticCams->aStatic[gpStaticCams->nStatic].vLook,
                gpStaticCams->aStatic[gpStaticCams->nStatic].vPos,
                gpStaticCams->aStatic[gpStaticCams->nStatic].vLook);
    gpStaticCams->nStatic++;
    Circular_MemFreeFunc(pCameraActor);
}

// Allocates the cameras' state (gpStaticCams), with no timing curves, and empties it
// (StaticCam_Reset). Called when a round starts up (GO_vInitIG).
void StaticCam_Init(void) {
    gpStaticCams = Circular_MemAllocFunc(sizeof(StaticCamManager_t), MEMTYPE_LOW, 0, "GoStaticCam.c", 380);
    gpStaticCams->pCurves = NULL;
    StaticCam_Reset();
}

// Empties the cameras' state (StaticCam_Reset) and frees it, when a round shuts down.
void StaticCam_DeInit(void) {
    StaticCam_Reset();
    Circular_MemFreeFunc(gpStaticCams);
    gpStaticCams = NULL;
}

// Forgets every static and fly-by camera and route, and frees the routes' timing curves. The hole
// loader calls it when it closes a hole (HoleLoader_CloseCurrentHole).
void StaticCam_Reset(void) {
    gpStaticCams->nStatic = 0;
    gpStaticCams->nFlyBy = 0;
    gpStaticCams->bLinked = 0;
    gpStaticCams->apRoute[0] = NULL;
    gpStaticCams->apRoute[1] = NULL;
    gpStaticCams->apRoute[2] = NULL;
    gpStaticCams->apRoute[3] = NULL;
    gpStaticCams->apRoute[4] = NULL;
    gpStaticCams->apRoute[5] = NULL;
    gpStaticCams->apRoute[6] = NULL;
    gpStaticCams->apRoute[7] = NULL;
    gpStaticCams->apRoute[8] = NULL;
    gpStaticCams->apRoute[9] = NULL;
    if (gpStaticCams->pCurves != NULL) {
        Circular_MemFreeFunc(gpStaticCams->pCurves);
    }
    gpStaticCams->pCurves = NULL;
    gpStaticCams->nCurves = 0;
}

// A static or fly-by shot's camera position: its vPos, into position. ePlayerID is not used.
void StaticCam_ProcessScript(CamShot* link, int ePlayerID, f32* position) {
    Vec3Copy(link->vPos, position);
}

// A random static camera for event (one of its pick.uEvents) whose area holds one of the player's
// ball positions (StaticCam_CheckHotZone), other than ignoreLink; with dontShowGolfer none that looks
// at the golfer (CAMPOINT_GOLFER_WAIST_LAGGED). Only on course 12 with Game_GetCurHoleNum 10, and only
// for STATICCAM_EVENT_PRE_SHOT; NULL otherwise and when none fits.
CamShot* StaticCam_ChooseScript(int ePlayerID, StaticCam_EventType_e event, u8 dontShowGolfer,
                                CamShot* ignoreLink) {
    int possibleScripts[STATICCAM_NUM_STATIC];
    int* pFound;
    int i;
    int scriptsFound;

    scriptsFound = 0;
    if (Game_GetCourse() != 12 || Game_GetCurHoleNum() != 10 || event != STATICCAM_EVENT_PRE_SHOT) {
        return NULL;
    }
    pFound = possibleScripts;
    for (i = 0; i < gpStaticCams->nStatic; i++) {
        if (!gpStaticCams->aStatic[i].bHasPrev && (event & gpStaticCams->aStatic[i].pick.uEvents)
            && (!dontShowGolfer || gpStaticCams->aStatic[i].nLookAtKind != CAMPOINT_GOLFER_WAIST_LAGGED)
            && StaticCam_CheckHotZone(&gpStaticCams->aStatic[i], ePlayerID)
            && ignoreLink != &gpStaticCams->aStatic[i]) {
            *pFound++ = i;
            scriptsFound++;
        }
    }
    if (scriptsFound == 0) {
        return NULL;
    }
    i = Misc_RandFunc(RANDOM_PRESENTATION) % scriptsFound;
    return &gpStaticCams->aStatic[possibleScripts[i]];
}

// Fly-by route iRouteNum_p's first shot (NULL for 10 or more). The first call after the fly-by
// cameras load links them: each camera's pNext becomes the camera whose number its next-camera
// number names (on any route; none, NULL), its pPrev the camera of its own route whose pNext it is
// (none: NULL and bHasPrev cleared). A route's first shot is a camera with no pPrev and a pNext; a
// route whose camera names FLYBY_END_HIDE_GOLFER as its next hides the golfer on all its cameras.
// Along each route, as far as its pNext chain stays on the route, each shot's fLength becomes the
// length of the spline to the next one (CamScript_fGetDistanceBetweenSplinePoints, through the
// shots StaticCam_SetupFlybyCameraPointers would pick; 0 on the last) and afRouteLength their sum.
CamShot* StaticCam_GetFlyByCam(int iRouteNum_p) {
    u8 disableGolferDraw[STATICCAM_NUM_ROUTES];
    CamShot* pCurCamera;
    CamShot* pNextCamera;
    CamShot* pPrevCamera;
    CamShot* pNextNextCamera;
    s32 i;
    int j;
    u8 matchFound;

    disableGolferDraw[0] = 0;
    disableGolferDraw[1] = 0;
    disableGolferDraw[2] = 0;
    disableGolferDraw[3] = 0;
    disableGolferDraw[4] = 0;
    disableGolferDraw[5] = 0;
    disableGolferDraw[6] = 0;
    disableGolferDraw[7] = 0;
    disableGolferDraw[8] = 0;
    disableGolferDraw[9] = 0;
    if (!gpStaticCams->bLinked) {
        for (i = 0; i < gpStaticCams->nFlyBy; i++) {
            matchFound = 0;
            for (j = 0; j < gpStaticCams->nFlyBy; j++) {
                if (gpStaticCams->aFlyBy[i].pNext == gpStaticCams->aFlyBy[j].pPrev) {
                    matchFound = 1;
                    gpStaticCams->aFlyBy[i].pNext = &gpStaticCams->aFlyBy[j];
                    break;
                }
            }
            if (!matchFound) {
                if ((s32)(uptr)gpStaticCams->aFlyBy[i].pNext == FLYBY_END_HIDE_GOLFER) {
                    disableGolferDraw[gpStaticCams->aFlyBy[i].pick.nRoute] = 1;
                }
                gpStaticCams->aFlyBy[i].pNext = NULL;
            }
        }
        for (i = 0; i < gpStaticCams->nFlyBy; i++) {
            matchFound = 0;
            for (j = 0; j < gpStaticCams->nFlyBy; j++) {
                if (&gpStaticCams->aFlyBy[i] == gpStaticCams->aFlyBy[j].pNext
                    && gpStaticCams->aFlyBy[i].pick.nRoute == gpStaticCams->aFlyBy[j].pick.nRoute) {
                    matchFound = 1;
                    gpStaticCams->aFlyBy[i].pPrev = &gpStaticCams->aFlyBy[j];
                    break;
                }
            }
            if (!matchFound) {
                gpStaticCams->aFlyBy[i].pPrev = NULL;
                gpStaticCams->aFlyBy[i].bHasPrev = 0;
            }
        }
        for (i = 0; i < gpStaticCams->nFlyBy; i++) {
            if (gpStaticCams->aFlyBy[i].nStateType == CAMSTATE_FLYBY && !gpStaticCams->aFlyBy[i].bHasPrev
                && gpStaticCams->aFlyBy[i].pNext != NULL) {
                gpStaticCams->apRoute[gpStaticCams->aFlyBy[i].pick.nRoute] = &gpStaticCams->aFlyBy[i];
            }
            if (disableGolferDraw[gpStaticCams->aFlyBy[i].pick.nRoute]) {
                gpStaticCams->aFlyBy[i].bShowGolfer = 0;
            }
        }
        for (i = 0; i < STATICCAM_NUM_ROUTES; i++) {
            if (gpStaticCams->apRoute[i] != NULL) {
                gpStaticCams->afRouteLength[i] = 0.0f;
                if (gpStaticCams->apRoute != NULL) {   // always true: apRoute is an array
                    for (pCurCamera = gpStaticCams->apRoute[i];
                         pCurCamera->pNext != NULL && pCurCamera->pNext->pick.nRoute == i;
                         pCurCamera = pCurCamera->pNext) {
                        // the four shots StaticCam_SetupFlybyCameraPointers picks
                        if (pCurCamera->pPrev != NULL) {
                            pPrevCamera = pCurCamera->pPrev;
                        } else {
                            pPrevCamera = pCurCamera;
                        }
                        if (pCurCamera->pNext != NULL
                            && pCurCamera->pick.nRoute == pCurCamera->pNext->pick.nRoute) {
                            pNextCamera = pCurCamera->pNext;
                        } else {
                            pNextCamera = pCurCamera;
                        }
                        if (pNextCamera != NULL && pNextCamera->pNext != NULL
                            && pCurCamera->pick.nRoute == pNextCamera->pNext->pick.nRoute) {
                            pNextNextCamera = pNextCamera->pNext;
                        } else {
                            pNextNextCamera = pNextCamera;
                        }
                        pCurCamera->fLength = CamScript_fGetDistanceBetweenSplinePoints(
                            pPrevCamera->vPos, pCurCamera->vPos, pNextCamera->vPos, pNextNextCamera->vPos);
                        gpStaticCams->afRouteLength[i] += pCurCamera->fLength;
                    }
                    if (pCurCamera != NULL) {
                        pCurCamera->fLength = 0.0f;
                    }
                }
            }
        }
        gpStaticCams->bLinked = 1;
    }
    if (iRouteNum_p >= STATICCAM_NUM_ROUTES) {
        return NULL;
    }
    return gpStaticCams->apRoute[iRouteNum_p];
}

// Fly-by route iRouteNum_p's timing curve, or NULL.
Curve* StaticCam_GetFlybyTimeCurve(u32 iRouteNum_p) {
    Curve* pCurves;
    u32 i;

    pCurves = gpStaticCams->pCurves;
    if (pCurves == NULL) {
        return NULL;
    }
    for (i = 0; i < gpStaticCams->nCurves; i++) {
        if (iRouteNum_p == gpStaticCams->pCurves[i].nRoute) {
            return &pCurves[i];
        }
    }
    return NULL;
}

// Flies the camera to share fParam of fly-by route nRoute's length (afRouteLength): position,
// lookat (look angles) and fov come from CamScript_SplineCamerasByPositionAndLook's splines through
// the vPos and vLook of the four shots around the current one (StaticCam_SetupFlybyCameraPointers),
// the field of view from the view's lens towards the shots' fFovStart. From the script's current
// shot (pShot), the share of that shot's segment (fSegmentShare) and the distance flown so far
// (fFlyByDist), it steps along the spline 0.005 of a segment at a time, going on to the next shot
// at a segment's end and stopping at the route's last, until the camera has flown far enough; then
// it homes in on the exact distance. The script keeps the shot reached, its pNext (pNextShot), the
// segment share and the distance.
void StaticCam_GetFlybyInformation(CameraScriptSettings_t* settings, int nRoute, f32* position, f32* lookat,
                                   f32* fov, int ePlayerID, f32 fParam) {
    f32 origPos[4];
    f32 origPosDiff[4];
    CamShot* pPrevCamera;
    CamShot* pNextCamera;
    CamShot* pNextNextCamera;
    CamShot* pCurCamera;
    f32 sectionPercent;
    f32 idealDist;
    f32 currentDist;
    f32 prevDist;
    f32 lastUnderDist;
    f32 startPercent;
    f32 endPercent;
    f32 percentDiff;
    f32 prevPercent;
    f32 lastUnderPercent;
    f32 lastOverDist;
    f32 lastOverPercent;
    int i;

    currentDist = settings->fFlyByDist;
    sectionPercent = settings->fSegmentShare;
    pCurCamera = settings->pShot;
    prevPercent = sectionPercent;
    prevDist = currentDist;
    idealDist = fParam * gpStaticCams->afRouteLength[nRoute];
    StaticCam_SetupFlybyCameraPointers(pCurCamera, &pPrevCamera, &pNextCamera, &pNextNextCamera);
    LLMath_CopyVec(position, origPos);
    *fov = CA_fGetCameraFieldOfView(
        Camera_GetLens(ViewController_GetRenderContext(gPlayers[ePlayerID].nView[0])));
    if (0.0f == idealDist) {
        CamScript_SplineCamerasByPositionAndLook(pPrevCamera->vPos, pCurCamera->vPos, pNextCamera->vPos,
                                                 pNextNextCamera->vPos, pPrevCamera->vLook, pCurCamera->vLook,
                                                 pNextCamera->vLook, pNextNextCamera->vLook, position, lookat,
                                                 fov, pCurCamera->fFovStart, pNextCamera->fFovStart,
                                                 sectionPercent);
    } else {
        while (idealDist > currentDist) {
            prevPercent = sectionPercent;
            sectionPercent += 0.005f;
            prevDist = currentDist;
            if (sectionPercent > 1.0f) {
                if (pCurCamera->pNext == NULL || pCurCamera->pNext->pNext == NULL) {
                    // the route ends: stop at its last shot
                    currentDist = idealDist;
                    sectionPercent = 1.0f;
                    break;
                }
                pCurCamera = pCurCamera->pNext;
                StaticCam_SetupFlybyCameraPointers(pCurCamera, &pPrevCamera, &pNextCamera, &pNextNextCamera);
                sectionPercent = 0.0f;
            }
            CamScript_SplineCamerasByPositionAndLook(pPrevCamera->vPos, pCurCamera->vPos, pNextCamera->vPos,
                                                     pNextNextCamera->vPos, pPrevCamera->vLook,
                                                     pCurCamera->vLook, pNextCamera->vLook,
                                                     pNextNextCamera->vLook, position, lookat, fov,
                                                     pCurCamera->fFovStart, pNextCamera->fFovStart,
                                                     sectionPercent);
            LLMath_Subtract3(position, origPos, origPosDiff);
            currentDist += (f32)Math_Sqrt(LLMath_LengthSquared3(origPosDiff));
            if (idealDist > currentDist) {
                LLMath_CopyVec(position, origPos);
            }
        }
    }
    if (sectionPercent != prevPercent) {
        // home in: halve the step four times, then interpolate the rest
        if (sectionPercent >= prevPercent) {
            startPercent = prevPercent;
            endPercent = sectionPercent;
        } else {
            startPercent = 0.0f;
            endPercent = sectionPercent;
        }
        sectionPercent = endPercent;
        lastOverDist = currentDist;
        lastUnderPercent = sectionPercent - 0.005f;
        lastOverPercent = sectionPercent;
        lastUnderDist = prevDist;
        if (lastUnderPercent < 0.0f) {
            lastUnderPercent = 0.0f;
        }
        i = 0;
        percentDiff = sectionPercent - startPercent;
        do {
            if (currentDist < idealDist) {
                lastUnderDist = currentDist;
                lastUnderPercent = sectionPercent;
                sectionPercent += percentDiff / (f32)(2 << i);
            } else if (currentDist > idealDist) {
                lastOverDist = currentDist;
                lastOverPercent = sectionPercent;
                sectionPercent -= percentDiff / (f32)(2 << i);
            }
            CamScript_SplineCamerasByPositionAndLook(pPrevCamera->vPos, pCurCamera->vPos, pNextCamera->vPos,
                                                     pNextNextCamera->vPos, pPrevCamera->vLook,
                                                     pCurCamera->vLook, pNextCamera->vLook,
                                                     pNextNextCamera->vLook, position, lookat, fov,
                                                     pCurCamera->fFovStart, pNextCamera->fFovStart,
                                                     sectionPercent);
            LLMath_Subtract3(position, origPos, origPosDiff);
            currentDist = prevDist + (f32)Math_Sqrt(LLMath_LengthSquared3(origPosDiff));
            i++;
        } while (i < 4);
        if (currentDist > idealDist) {
            endPercent = (sectionPercent - lastUnderPercent)
                    * (1.0f - (currentDist - idealDist) / (currentDist - lastUnderDist)) + lastUnderPercent;
            sectionPercent = endPercent;
            CamScript_SplineCamerasByPositionAndLook(pPrevCamera->vPos, pCurCamera->vPos, pNextCamera->vPos,
                                                     pNextNextCamera->vPos, pPrevCamera->vLook,
                                                     pCurCamera->vLook, pNextCamera->vLook,
                                                     pNextNextCamera->vLook, position, lookat, fov,
                                                     pCurCamera->fFovStart, pNextCamera->fFovStart,
                                                     sectionPercent);
            LLMath_Subtract3(position, origPos, origPosDiff);
            currentDist = prevDist + (f32)Math_Sqrt(LLMath_LengthSquared3(origPosDiff));
        } else if (currentDist < idealDist) {
            sectionPercent = sectionPercent
                    + (lastOverPercent - sectionPercent)
                    * ((idealDist - currentDist) / (lastOverDist - currentDist));
            // fake match: empty tests that only make the shot pointers load before the call
            if (pNextCamera) {
            } else {
            }
            if (pNextNextCamera) {
            } else {
            }
            if (pPrevCamera->vPos) {
            } else {
            }
            CamScript_SplineCamerasByPositionAndLook(pPrevCamera->vPos, pCurCamera->vPos, pNextCamera->vPos,
                                                     pNextNextCamera->vPos, pPrevCamera->vLook,
                                                     pCurCamera->vLook, pNextCamera->vLook,
                                                     pNextNextCamera->vLook, position, lookat, fov,
                                                     pCurCamera->fFovStart, pNextCamera->fFovStart,
                                                     sectionPercent);
            LLMath_Subtract3(position, origPos, origPosDiff);
            currentDist = prevDist + (f32)Math_Sqrt(LLMath_LengthSquared3(origPosDiff));
        }
    }
    settings->pShot = pCurCamera;
    settings->pNextShot = pCurCamera->pNext;
    settings->fFlyByDist = currentDist;
    settings->fSegmentShare = sectionPercent;
}

// The four shots a fly-by spline runs through around pCurCamera: the one before (pCurCamera itself
// at the route's start), pCurCamera, the next one and the one after that (each the one before it
// at the route's end; a next shot on another route counts as the end).
static void StaticCam_SetupFlybyCameraPointers(CamShot* pCurCamera, CamShot** pPrevCamera,
                                               CamShot** pNextCamera, CamShot** pNextNextCamera) {
    if (pCurCamera->pPrev != NULL) {
        *pPrevCamera = pCurCamera->pPrev;
    } else {
        *pPrevCamera = pCurCamera;
    }
    if (pCurCamera->pNext != NULL) {
        if (pCurCamera->pick.nRoute == pCurCamera->pNext->pick.nRoute) {
            *pNextCamera = pCurCamera->pNext;
        } else {
            *pNextCamera = pCurCamera;
        }
    } else {
        *pNextCamera = pCurCamera;
    }
    if (*pNextCamera != NULL && (*pNextCamera)->pNext != NULL) {
        if (pCurCamera->pick.nRoute == (*pNextCamera)->pNext->pick.nRoute) {
            *pNextNextCamera = (*pNextCamera)->pNext;
        } else {
            *pNextNextCamera = *pNextCamera;
        }
    } else {
        *pNextNextCamera = *pNextCamera;
    }
}

// The static camera's area (x between u.aArea[0] and [2], z between [1] and [3], bounds excluded)
// holds the player's vBall (tested first) or ball.vPos.
static u8 StaticCam_CheckHotZone(CamShot* script, int ePlayerID) {
    f32* pBallPos;
    f32* pVBall;
    int i;
    f32 trackingPos[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    i = 0;
    pBallPos = gPlayers[ePlayerID].ball.vPos;
    pVBall = gPlayers[ePlayerID].vBall;
    do {
        switch (i) {
        case 1:
            Vec3Copy(pBallPos, trackingPos);
            break;
        case 0:
            Vec3Copy(pVBall, trackingPos);
            break;
        }
        if (trackingPos[0] > script->u.aArea[0] && trackingPos[0] < script->u.aArea[2]
            && trackingPos[2] > script->u.aArea[1] && trackingPos[2] < script->u.aArea[3]) {
            return 1;
        }
    } while (++i < 2);
    return 0;
}

// dest = src2 + src1 (x, y and z; dest's w as it was).
static asm void LLMath_Add3(register const f32* src1, register const f32* src2, register f32* dest) {
    nofralloc
    psq_l  f0, 0(src1), 0, 0
    psq_l  f1, 8(src1), 1, 0
    psq_l  f2, 0(src2), 0, 0
    psq_l  f3, 8(src2), 1, 0
    ps_add f2, f2, f0
    ps_add f3, f3, f1
    psq_st f2, 0(dest), 0, 0
    psq_st f3, 8(dest), 1, 0
    blr
}

// dest = src1 - src2 (x, y and z; dest's w as it was).
static asm void LLMath_Subtract3(register const f32* src1, register const f32* src2, register f32* dest) {
    nofralloc
    psq_l  f0, 0(src1), 0, 0
    psq_l  f1, 8(src1), 1, 0
    psq_l  f2, 0(src2), 0, 0
    psq_l  f3, 8(src2), 1, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(dest), 0, 0
    psq_st f3, 8(dest), 1, 0
    blr
}
