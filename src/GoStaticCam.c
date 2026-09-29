// GoStaticCam.c (EA's name, from its asserts; also in EA's 2002 source tree): the course's static
// cameras and fly-by camera paths. The course data brings them as 'Cact' objects (UKernel.c hands
// type 201 to StaticCam_ParseStaticCameraActor, type 200 to StaticCam_ParseFlybyCameraActor) and
// the paths' timing curves as a 'CAMC' stream object (StaticCam_LoadCAMCfromStream). The golf
// cameras (GoGolfCam.c) pick a static camera whose area holds the ball (StaticCam_ChooseScript) and
// fly along a path (StaticCam_GetFlyByCam, StaticCam_GetFlybyInformation).

#include "game_types.h"
#include "engine.h"
#include "endian.h"
#include "game.h"
#include "golfer.h"
#include "camera.h"

CamLens* Camera_GetLens(void* pCamera);                        // the render camera's lens
void mat44flt_EulerAngles(f32 (*pMtx)[4], f32 a, f32 b, f32 c);      // a rotation matrix from three angles
void LLMath_mat44fltMultiply33(f32 (*pMtx)[4], f32* pIn, f32* pOut);     // a vector through a matrix
f32  CamScript_fGetDistanceBetweenSplinePoints(f32* pPos0, f32* pPos1, f32* pPos2, f32* pPos3);

void StaticCam_LoadCAMCfromStream(UStreamObject* pObject);
void StaticCam_Reset(void);
void StaticCam_SetupFlybyCameraPointers(CamShot* pShot, CamShot** ppPrev, CamShot** ppNext, CamShot** ppAfter);
u8   StaticCam_CheckHotZone(CamShot* pShot, int nPlayer);
void StaticCam_Vec3Add(f32* pA, f32* pB, f32* pOut);
void StaticCam_Vec3Sub(f32* pA, f32* pB, f32* pOut);

StaticCams* lbl_80281E18;

// Registers the 'CAMC' stream handler (StaticCam_LoadCAMCfromStream), the fly-by paths' timing
// curves.
void StaticCam_RegisterStreamClients(void) {
    Stream_RegisterLoadChunkCallback('CAMC', StaticCam_LoadCAMCfromStream);
}

void StaticCam_UnRegisterStreamClients(void) {
    Stream_UnregisterLoadChunkCallback('CAMC');
}

// The 'CAMC' stream handler: the fly-by paths' timing curves (FlyByPath, found by
// StaticCam_GetFlybyTimeCurve), byte-swapped from the file (a two-word header into u1E5C and
// nPaths, then each path and its 0x22-byte keys) into a table allocated here. Frees the object.
void StaticCam_LoadCAMCfromStream(UStreamObject* pObject) {
    SwapField aHeader[2] = { { 4, 4 }, { 4, 4 } };
    SwapField aPath[4] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };
    SwapField aKey[3] = { { 0x20, 4 }, { 1, 1 }, { 1, 1 } };
    void* pSrc;
    void* pDst;
    u32 i;
    u32 j;

    pSrc = pObject->pData;
    pDst = &lbl_80281E18->u1E5C;
    ByteSwap_Records(&pSrc, &pDst, aHeader, 2, 1);
    if (lbl_80281E18->nPaths != 0) {
        lbl_80281E18->pPaths =
            StaticMem_Alloc(lbl_80281E18->nPaths * sizeof(FlyByPath), 2, 0, "GoStaticCam.c", 190);
    }
    for (i = 0; i < lbl_80281E18->nPaths; i++) {
        pDst = &lbl_80281E18->pPaths[i];
        ByteSwap_Records(&pSrc, &pDst, aPath, 4, 1);
        for (j = 0; j < lbl_80281E18->pPaths[i].nKeys; j++) {
            pDst = &lbl_80281E18->pPaths[i].aKeys[j];
            ByteSwap_Records(&pSrc, &pDst, aKey, 3, 1);
            pSrc = (u8*)pSrc + 2;   // the keys are 0x22 bytes in the file
        }
    }
    StaticMem_Free(pObject);
}

// Takes a fly-by camera (a 'Cact' object of type 200, from UKernel.c) as the next shot of aFlyBy,
// named "FlyBy Cam: <its number>": its position, field of view and look angles (degrees to
// radians), time f20 and f34 (clamped to 0..1). p40 and p44 keep the next camera's number and its
// own until StaticCam_GetFlyByCam links the paths; nA4 is its path (bAA 0 on path 9, else 1). The
// count is not checked against aFlyBy's 30 slots. Frees the object.
void StaticCam_ParseFlybyCameraActor(UStreamObject* pObject) {
    char szName[0x20];  // its size is unknown
    FlyByCamDef* pDef;

    lbl_80281E18->bLinked = 0;
    pDef = (FlyByCamDef*)pObject->pData;
    sprintf(szName, "FlyBy Cam: %d", pDef->nId);
    strcpy(lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].szName, szName);
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].bAD = 0;
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].bA8 = 0;
    if (pDef->f34 < 0.0f) {
        pDef->f34 = 0.0f;
    } else if (pDef->f34 > 1.0f) {
        pDef->f34 = 1.0f;
    }
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].f4C = pDef->f34;
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].f70 = 0.0f;
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].f74 = 0.0f;
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].v20[0] = pDef->aPos[0];
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].v20[1] = pDef->aPos[1];
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].v20[2] = pDef->aPos[2];
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].f78 = PI * (pDef->fFov / 180.0f);
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].bAB = 3;
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].f48 = pDef->f20;
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].bAC = 24;
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].bA9 = 1;
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].v30[0] = PI * (pDef->aLook[0] / 180.0f);
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].v30[1] = PI * (pDef->aLook[1] / 180.0f);
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].v30[2] = PI * (pDef->aLook[2] / 180.0f);
    // port: p40 and p44 hold the next camera's number and this one's until StaticCam_GetFlyByCam links them
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].p40 = (CamShot*)(uptr)(s32)pDef->nNext;
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].p44 = (CamShot*)(uptr)(s32)pDef->nId;
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].bB2 = 0;
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].f80 = 0.0f;
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].nA4 = pDef->nPath;
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].f88 = 0.0f;
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].f8C = 0.0f;
    lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].nA0 = 1;
    if (lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].nA4 != 9) {
        lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].bAA = 1;
    } else {
        lbl_80281E18->aFlyBy[lbl_80281E18->nFlyBy].bAA = 0;
    }
    lbl_80281E18->nFlyBy++;
    StaticMem_Free(pObject);
}

// Takes a static camera (a 'Cact' object of type 201, from UKernel.c) as the next shot of aStatic,
// named "Static Cam: <n>": its position, fields of view (degrees to radians), area (u.aArea), the
// shot kinds it serves (nA4) and the rest of its StaticCamDef; heights 0 to 10000. Its look-at
// point v30 is one unit from its position along its three angles (mat44flt_EulerAngles). The count
// is not checked against aStatic's 10 slots. Frees the object.
void StaticCam_ParseStaticCameraActor(UStreamObject* pObject) {
    f32 vAhead[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
    char szName[0x20];  // its size is unknown
    f32 m[4][4];
    StaticCamDef* pDef;
    CamShot* pShot;

    pDef = (StaticCamDef*)pObject->pData;
    sprintf(szName, "Static Cam: %d", lbl_80281E18->nStatic);
    strcpy(lbl_80281E18->aStatic[lbl_80281E18->nStatic].szName, szName);
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].bAD = 38;
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].bA8 = 0;
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].f4C = pDef->f34;
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].f70 = 0.0f;
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].f74 = 0.0f;
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].v20[0] = pDef->aPos[0];
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].v20[1] = pDef->aPos[1];
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].v20[2] = pDef->aPos[2];
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].f78 = PI * (pDef->fFov / 180.0f);
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].f7C = PI * (pDef->f2C / 180.0f);
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].p40 = NULL;
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].bAB = 5;
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].f48 = pDef->f30;
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].nAE = pDef->n1C;
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].bA9 = 0;
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].u.aArea[0] = pDef->aArea[0];
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].u.aArea[1] = pDef->aArea[1];
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].u.aArea[2] = pDef->aArea[2];
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].u.aArea[3] = pDef->aArea[3];
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].bAC = pDef->n20;
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].bB2 = 0;
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].f80 = 0.0f;
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].bAA = 1;
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].f88 = pDef->f54;
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].f8C = 0.0f;
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].f68 = 0.0f;
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].f6C = 10000.0f;
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].nA0 = pDef->n58;
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].nA4 = pDef->nKinds;
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].v30[0] = PI * (pDef->aAngle[0] / 180.0f);
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].v30[1] = PI * (pDef->aAngle[1] / 180.0f);
    lbl_80281E18->aStatic[lbl_80281E18->nStatic].v30[2] = PI * (pDef->aAngle[2] / 180.0f);
    pShot = &lbl_80281E18->aStatic[lbl_80281E18->nStatic];
    mat44flt_EulerAngles(m, pShot->v30[1], pShot->v30[0], pShot->v30[2]);
    LLMath_mat44fltMultiply33(m, vAhead, lbl_80281E18->aStatic[lbl_80281E18->nStatic].v30);
    StaticCam_Vec3Add(lbl_80281E18->aStatic[lbl_80281E18->nStatic].v30,
                lbl_80281E18->aStatic[lbl_80281E18->nStatic].v20,
                lbl_80281E18->aStatic[lbl_80281E18->nStatic].v30);
    lbl_80281E18->nStatic++;
    StaticMem_Free(pObject);
}

// Allocates the static cameras' state (gpStaticCams) and empties it (StaticCam_Reset). Called when
// a round starts up (GO_vInitIG).
void StaticCam_Init(void) {
    lbl_80281E18 = StaticMem_Alloc(sizeof(StaticCams), 2, 0, "GoStaticCam.c", 380);
    lbl_80281E18->pPaths = NULL;
    StaticCam_Reset();
}

// Empties the static cameras' state (StaticCam_Reset) and frees it.
void StaticCam_DeInit(void) {
    StaticCam_Reset();
    StaticMem_Free(lbl_80281E18);
    lbl_80281E18 = NULL;
}

// Forgets every static and fly-by camera and path, and frees the paths' timing curves. The hole
// loader calls it for each hole.
void StaticCam_Reset(void) {
    lbl_80281E18->nStatic = 0;
    lbl_80281E18->nFlyBy = 0;
    lbl_80281E18->bLinked = 0;
    lbl_80281E18->apPath[0] = NULL;
    lbl_80281E18->apPath[1] = NULL;
    lbl_80281E18->apPath[2] = NULL;
    lbl_80281E18->apPath[3] = NULL;
    lbl_80281E18->apPath[4] = NULL;
    lbl_80281E18->apPath[5] = NULL;
    lbl_80281E18->apPath[6] = NULL;
    lbl_80281E18->apPath[7] = NULL;
    lbl_80281E18->apPath[8] = NULL;
    lbl_80281E18->apPath[9] = NULL;
    if (lbl_80281E18->pPaths != NULL) {
        StaticMem_Free(lbl_80281E18->pPaths);
    }
    lbl_80281E18->pPaths = NULL;
    lbl_80281E18->nPaths = 0;
}

// A static or fly-by shot's camera position: its v20, into pOut. nPlayer is not used.
void StaticCam_ProcessScript(CamShot* pShot, int nPlayer, f32* pOut) {
    Vec3Copy(pShot->v20, pOut);
}

// A random static camera for shot kind nKind (a bit of its nA4) whose area holds one of nPlayer's
// ball positions (StaticCam_CheckHotZone), other than pNot; with bNotKind5 (TW07's dontShowGolfer)
// none whose look-at kind bAC is 5. Only on course 12 with Game_GetCurHoleNum 10 and for kind 0x20;
// NULL otherwise and when none fits.
CamShot* StaticCam_ChooseScript(int nPlayer, int nKind, u8 bNotKind5, CamShot* pNot) {
    int aFound[NUM_STATIC_CAMS];
    int* pFound;
    int i;
    int nFound;

    nFound = 0;
    if (Game_GetCourse() != 12 || Game_GetCurHoleNum() != 10 || nKind != 0x20) {
        return NULL;
    }
    pFound = aFound;
    for (i = 0; i < lbl_80281E18->nStatic; i++) {
        if (!lbl_80281E18->aStatic[i].bA9 && (nKind & lbl_80281E18->aStatic[i].nA4)
            && (!bNotKind5 || lbl_80281E18->aStatic[i].bAC != 5)
            && StaticCam_CheckHotZone(&lbl_80281E18->aStatic[i], nPlayer) && pNot
                    != &lbl_80281E18->aStatic[i]) {
            *pFound++ = i;
            nFound++;
        }
    }
    if (nFound == 0) {
        return NULL;
    }
    i = Misc_RandFunc(1) % nFound;
    return &lbl_80281E18->aStatic[aFound[i]];
}

// Fly-by path nPath's first shot (NULL for nPath 10 or more). The first call after loading links
// the fly-by cameras into their paths (p40 the next shot, p44 the one before, by the numbers
// StaticCam_ParseFlybyCameraActor kept), finds each path's first shot, sets each shot's f4C to its
// segment's length (fn_800C79BC; 0 for the last) and adds them up per path; a path whose last
// camera is marked -99 gets bAA 0 on all its cameras.
CamShot* StaticCam_GetFlyByCam(int nPath) {
    u8 abEnds[NUM_FLYBY_PATHS];   // the path ends on a camera marked -99
    CamShot* pShot;
    CamShot* pNext;
    CamShot* pPrev;
    CamShot* pAfter;
    s32 i;
    int j;
    u8 bFound;

    abEnds[0] = 0;
    abEnds[1] = 0;
    abEnds[2] = 0;
    abEnds[3] = 0;
    abEnds[4] = 0;
    abEnds[5] = 0;
    abEnds[6] = 0;
    abEnds[7] = 0;
    abEnds[8] = 0;
    abEnds[9] = 0;
    if (!lbl_80281E18->bLinked) {
        for (i = 0; i < lbl_80281E18->nFlyBy; i++) {
            bFound = 0;
            for (j = 0; j < lbl_80281E18->nFlyBy; j++) {
                if (lbl_80281E18->aFlyBy[i].p40 == lbl_80281E18->aFlyBy[j].p44) {
                    bFound = 1;
                    lbl_80281E18->aFlyBy[i].p40 = &lbl_80281E18->aFlyBy[j];
                    break;
                }
            }
            if (!bFound) {
                if ((s32)(uptr)lbl_80281E18->aFlyBy[i].p40 == -99) {
                    abEnds[lbl_80281E18->aFlyBy[i].nA4] = 1;
                }
                lbl_80281E18->aFlyBy[i].p40 = NULL;
            }
        }
        for (i = 0; i < lbl_80281E18->nFlyBy; i++) {
            bFound = 0;
            for (j = 0; j < lbl_80281E18->nFlyBy; j++) {
                if (&lbl_80281E18->aFlyBy[i] == lbl_80281E18->aFlyBy[j].p40
                    && lbl_80281E18->aFlyBy[i].nA4 == lbl_80281E18->aFlyBy[j].nA4) {
                    bFound = 1;
                    lbl_80281E18->aFlyBy[i].p44 = &lbl_80281E18->aFlyBy[j];
                    break;
                }
            }
            if (!bFound) {
                lbl_80281E18->aFlyBy[i].p44 = NULL;
                lbl_80281E18->aFlyBy[i].bA9 = 0;
            }
        }
        for (i = 0; i < lbl_80281E18->nFlyBy; i++) {
            if (!lbl_80281E18->aFlyBy[i].bAD && !lbl_80281E18->aFlyBy[i].bA9
                && lbl_80281E18->aFlyBy[i].p40 != NULL) {
                lbl_80281E18->apPath[lbl_80281E18->aFlyBy[i].nA4] = &lbl_80281E18->aFlyBy[i];
            }
            if (abEnds[lbl_80281E18->aFlyBy[i].nA4]) {
                lbl_80281E18->aFlyBy[i].bAA = 0;
            }
        }
        for (i = 0; i < NUM_FLYBY_PATHS; i++) {
            if (lbl_80281E18->apPath[i] != NULL) {
                lbl_80281E18->afPathLength[i] = 0.0f;
                if (lbl_80281E18->apPath != NULL) {   // always true: apPath is an array
                    for (pShot = lbl_80281E18->apPath[i]; pShot->p40 != NULL && pShot->p40->nA4 == i;
                         pShot = pShot->p40) {
                        // StaticCam_SetupFlybyCameraPointers's body
                        if (pShot->p44 != NULL) {
                            pPrev = pShot->p44;
                        } else {
                            pPrev = pShot;
                        }
                        if (pShot->p40 != NULL && pShot->nA4 == pShot->p40->nA4) {
                            pNext = pShot->p40;
                        } else {
                            pNext = pShot;
                        }
                        if (pNext != NULL && pNext->p40 != NULL && pShot->nA4 == pNext->p40->nA4) {
                            pAfter = pNext->p40;
                        } else {
                            pAfter = pNext;
                        }
                        pShot->f4C = CamScript_fGetDistanceBetweenSplinePoints(pPrev->v20, pShot->v20,
                                pNext->v20, pAfter->v20);
                        lbl_80281E18->afPathLength[i] += pShot->f4C;
                    }
                    if (pShot != NULL) {
                        pShot->f4C = 0.0f;
                    }
                }
            }
        }
        lbl_80281E18->bLinked = 1;
    }
    if (nPath >= NUM_FLYBY_PATHS) {
        return NULL;
    }
    return lbl_80281E18->apPath[nPath];
}

// Fly-by path uPath's timing curve, or NULL.
FlyByPath* StaticCam_GetFlybyTimeCurve(u32 uPath) {
    FlyByPath* pPaths;
    u32 i;

    pPaths = lbl_80281E18->pPaths;
    if (pPaths == NULL) {
        return NULL;
    }
    for (i = 0; i < lbl_80281E18->nPaths; i++) {
        if (uPath == lbl_80281E18->pPaths[i].uPath) {
            return &pPaths[i];
        }
    }
    return NULL;
}

// Flies the camera along fly-by path nPath to share fShare of the path's length: pCam and pSub come
// from fn_800C7480's spline through the v20 and v30 of the four shots around the current one
// (StaticCam_SetupFlybyCameraPointers), *pFov starts at the view's lens and goes to fn_800C7480
// with the shots' f78. It steps the spline 0.005 at a time until the camera has gone far enough,
// then homes in on the exact distance. The script keeps the shot it is on (pShot, and pNextShot its
// p40), the share of that shot's segment (fA0) and the distance so far (fA4).
void StaticCam_GetFlybyInformation(CamScript* pScript, int nPath, f32* pCam, f32* pSub, f32* pFov, int nPlayer,
                 f32 fShare) {
    f32 vLast[4];
    f32 vDiff[4];
    CamShot* pPrev;
    CamShot* pNext;
    CamShot* pAfter;
    CamShot* pShot;
    f32 fT;
    f32 fTarget;
    f32 fDist;
    f32 fLastDist;
    f32 fLoDist;
    f32 fFrom;
    f32 fEnd;
    f32 fStep;
    f32 fLastT;
    f32 fLoT;
    f32 fHiDist;
    f32 fHiT;
    int i;

    fDist = pScript->fA4;
    fT = pScript->fA0;
    pShot = pScript->pShot;
    fLastT = fT;
    fLastDist = fDist;
    fTarget = fShare * lbl_80281E18->afPathLength[nPath];
    StaticCam_SetupFlybyCameraPointers(pShot, &pPrev, &pNext, &pAfter);
    LLMath_CopyVec(pCam, vLast);
    *pFov = CA_fGetCameraFieldOfView(
        Camera_GetLens(ViewController_GetRenderContext(gPlayers[nPlayer].nView[0])));
    if (0.0f == fTarget) {
        CamScript_SplineCamerasByPositionAndLook(pPrev->v20, pShot->v20, pNext->v20, pAfter->v20, pPrev->v30,
                                                 pShot->v30, pNext->v30,
                    pAfter->v30, pCam, pSub, pFov, pShot->f78, pNext->f78, fT);
    } else {
        while (fTarget > fDist) {
            fLastT = fT;
            fT += 0.005f;
            fLastDist = fDist;
            if (fT > 1.0f) {
                if (pShot->p40 == NULL || pShot->p40->p40 == NULL) {
                    // the path ends: stop at its last shot
                    fDist = fTarget;
                    fT = 1.0f;
                    break;
                }
                pShot = pShot->p40;
                StaticCam_SetupFlybyCameraPointers(pShot, &pPrev, &pNext, &pAfter);
                fT = 0.0f;
            }
            CamScript_SplineCamerasByPositionAndLook(pPrev->v20, pShot->v20, pNext->v20, pAfter->v20,
                                                     pPrev->v30, pShot->v30, pNext->v30,
                        pAfter->v30, pCam, pSub, pFov, pShot->f78, pNext->f78, fT);
            StaticCam_Vec3Sub(pCam, vLast, vDiff);
            fDist += (f32)Math_Sqrt(Vec3_LengthSqClamped(vDiff));
            if (fTarget > fDist) {
                LLMath_CopyVec(pCam, vLast);
            }
        }
    }
    if (fT != fLastT) {
        // home in: halve the step four times, then interpolate the rest
        if (fT >= fLastT) {
            fFrom = fLastT;
            fEnd = fT;
        } else {
            fFrom = 0.0f;
            fEnd = fT;
        }
        fT = fEnd;
        fHiDist = fDist;
        fLoT = fT - 0.005f;
        fHiT = fT;
        fLoDist = fLastDist;
        if (fLoT < 0.0f) {
            fLoT = 0.0f;
        }
        i = 0;
        fStep = fT - fFrom;
        do {
            if (fDist < fTarget) {
                fLoDist = fDist;
                fLoT = fT;
                fT += fStep / (f32)(2 << i);
            } else if (fDist > fTarget) {
                fHiDist = fDist;
                fHiT = fT;
                fT -= fStep / (f32)(2 << i);
            }
            CamScript_SplineCamerasByPositionAndLook(pPrev->v20, pShot->v20, pNext->v20, pAfter->v20,
                                                     pPrev->v30, pShot->v30, pNext->v30,
                        pAfter->v30, pCam, pSub, pFov, pShot->f78, pNext->f78, fT);
            StaticCam_Vec3Sub(pCam, vLast, vDiff);
            fDist = fLastDist + (f32)Math_Sqrt(Vec3_LengthSqClamped(vDiff));
            i++;
        } while (i < 4);
        if (fDist > fTarget) {
            fEnd = (fT - fLoT) * (1.0f - (fDist - fTarget) / (fDist - fLoDist)) + fLoT;
            fT = fEnd;
            CamScript_SplineCamerasByPositionAndLook(pPrev->v20, pShot->v20, pNext->v20, pAfter->v20,
                                                     pPrev->v30, pShot->v30, pNext->v30,
                        pAfter->v30, pCam, pSub, pFov, pShot->f78, pNext->f78, fT);
            StaticCam_Vec3Sub(pCam, vLast, vDiff);
            fDist = fLastDist + (f32)Math_Sqrt(Vec3_LengthSqClamped(vDiff));
        } else if (fDist < fTarget) {
            fT = fT + (fHiT - fT) * ((fTarget - fDist) / (fHiDist - fDist));
            // fake match: empty tests (dead checks) on the shot pointers make the frontend load them
            // ahead of the call, so the call's arguments are scheduled as in the original
            if (pNext) {
            } else {
            }
            if (pAfter) {
            } else {
            }
            if (pPrev->v20) {
            } else {
            }
            CamScript_SplineCamerasByPositionAndLook(pPrev->v20, pShot->v20, pNext->v20, pAfter->v20,
                                                     pPrev->v30, pShot->v30, pNext->v30,
                        pAfter->v30, pCam, pSub, pFov, pShot->f78, pNext->f78, fT);
            StaticCam_Vec3Sub(pCam, vLast, vDiff);
            fDist = fLastDist + (f32)Math_Sqrt(Vec3_LengthSqClamped(vDiff));
        }
    }
    pScript->pShot = pShot;
    pScript->pNextShot = pShot->p40;
    pScript->fA4 = fDist;
    pScript->fA0 = fT;
}

// The four shots a fly-by spline runs through around pShot: the one before (pShot itself at the
// path's start), pShot, the next one and the one after that (each repeating the last at the end).
void StaticCam_SetupFlybyCameraPointers(CamShot* pShot, CamShot** ppPrev, CamShot** ppNext, CamShot** ppAfter) {
    if (pShot->p44 != NULL) {
        *ppPrev = pShot->p44;
    } else {
        *ppPrev = pShot;
    }
    if (pShot->p40 != NULL) {
        if (pShot->nA4 == pShot->p40->nA4) {
            *ppNext = pShot->p40;
        } else {
            *ppNext = pShot;
        }
    } else {
        *ppNext = pShot;
    }
    if (*ppNext != NULL && (*ppNext)->p40 != NULL) {
        if (pShot->nA4 == (*ppNext)->p40->nA4) {
            *ppAfter = (*ppNext)->p40;
        } else {
            *ppAfter = *ppNext;
        }
    } else {
        *ppAfter = *ppNext;
    }
}

// The static shot's area (x between u.aArea[0] and [2], z between [1] and [3], bounds excluded)
// holds nPlayer's vBall (tested first) or ball.vPos.
u8 StaticCam_CheckHotZone(CamShot* pShot, int nPlayer) {
    f32* pBallPos;
    f32* pVBall;
    int i;
    f32 v[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    i = 0;
    pBallPos = gPlayers[nPlayer].ball.vPos;
    pVBall = gPlayers[nPlayer].vBall;
    do {
        switch (i) {
        case 1:
            Vec3Copy(pBallPos, v);
            break;
        case 0:
            Vec3Copy(pVBall, v);
            break;
        }
        if (v[0] > pShot->u.aArea[0] && v[0] < pShot->u.aArea[2] && v[2] > pShot->u.aArea[1]
            && v[2] < pShot->u.aArea[3]) {
            return 1;
        }
    } while (++i < 2);
    return 0;
}

// pOut gets pB + pA (three floats).
#ifdef __MWERKS__
asm void StaticCam_Vec3Add(register f32* pA, register f32* pB, register f32* pOut) {
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
void StaticCam_Vec3Add(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
}
#endif

// pOut gets pA - pB (three floats).
#ifdef __MWERKS__
asm void StaticCam_Vec3Sub(register f32* pA, register f32* pB, register f32* pOut) {
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
void StaticCam_Vec3Sub(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif
