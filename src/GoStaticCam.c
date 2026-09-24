// GoStaticCam.c (EA's name, from its asserts; also in EA's 2002 source tree): the course's static
// cameras and fly-by camera paths. The course data brings them as 'Cact' objects (UKernel.c hands
// type 201 to fn_80064A0C, type 200 to fn_800646D0) and the paths' splines
// as a 'CAMC' stream object (fn_800644F4). The golf cameras (GoGolfCam.c) pick a static camera
// that sees the golfer or the ball (fn_80064F7C) and fly along a path (fn_8006509C, fn_80065488).

#include "game_types.h"
#include "engine.h"
#include "endian.h"
#include "game.h"
#include "golfer.h"
#include "camera.h"

CamLens* fn_80008370(void* pCamera);                        // the render camera's lens
f32  fn_80014278(CamLens* pLens);                           // the lens's field of view
void fn_8000A194(f32 (*pMtx)[4], f32 a, f32 b, f32 c);      // a rotation matrix from three angles
void fn_800BADB4(f32 (*pMtx)[4], f32* pIn, f32* pOut);     // a vector through a matrix
f32  fn_800C79BC(f32* pPos0, f32* pPos1, f32* pPos2, f32* pPos3);

void fn_800644F4(UStreamObject* pObject);
void fn_80064EA4(void);
void fn_8006596C(CamShot* pShot, CamShot** ppPrev, CamShot** ppNext, CamShot** ppAfter);
u8   fn_800659F4(CamShot* pShot, int nPlayer);
void fn_80065AFC(f32* pA, f32* pB, f32* pOut);
void fn_80065B20(f32* pA, f32* pB, f32* pOut);

// Register the 'CAMC' stream handler.
void fn_8006449C(void) {
    UStream_RegisterHandler('CAMC', fn_800644F4);
}

void fn_800644CC(void) {
    UStream_UnregisterHandler('CAMC');
}

// The 'CAMC' handler: the fly-by paths' splines, byte-swapped into a new table.
void fn_800644F4(UStreamObject* pObject) {
    SwapField aHeader[2] = { { 4, 4 }, { 4, 4 } };
    SwapField aPath[4] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };
    SwapField aKey[3] = { { 0x20, 4 }, { 1, 1 }, { 1, 1 } };
    void* pSrc;
    void* pDst;
    u32 i;
    u32 j;

    pSrc = pObject->pData;
    pDst = &lbl_80281E18->u1E5C;
    fn_8001F08C(&pSrc, &pDst, aHeader, 2, 1);
    if (lbl_80281E18->nPaths != 0) {
        lbl_80281E18->pPaths =
            fn_80009B34(lbl_80281E18->nPaths * sizeof(FlyByPath), 2, 0, "GoStaticCam.c", 190);
    }
    for (i = 0; i < lbl_80281E18->nPaths; i++) {
        pDst = &lbl_80281E18->pPaths[i];
        fn_8001F08C(&pSrc, &pDst, aPath, 4, 1);
        for (j = 0; j < lbl_80281E18->pPaths[i].nKeys; j++) {
            pDst = &lbl_80281E18->pPaths[i].aKeys[j];
            fn_8001F08C(&pSrc, &pDst, aKey, 3, 1);
            pSrc = (u8*)pSrc + 2;   // the keys are 0x22 bytes in the file
        }
    }
    fn_80009E70(pObject);
}

// A fly-by camera arrived (a 'Cact' object of type 200): the next one of aFlyBy.
void fn_800646D0(UStreamObject* pObject) {
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
    // port: p40 and p44 hold the next camera's number and this one's until fn_8006509C links them
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
    fn_80009E70(pObject);
}

// A static camera arrived (a 'Cact' object of type 201): the next one of aStatic. It looks from
// its position along its three angles.
void fn_80064A0C(UStreamObject* pObject) {
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
    fn_8000A194(m, pShot->v30[1], pShot->v30[0], pShot->v30[2]);
    fn_800BADB4(m, vAhead, lbl_80281E18->aStatic[lbl_80281E18->nStatic].v30);
    pShot = &lbl_80281E18->aStatic[lbl_80281E18->nStatic];
    fn_80065AFC(pShot->v30, pShot->v20, pShot->v30);
    lbl_80281E18->nStatic++;
    fn_80009E70(pObject);
}

void fn_80064E2C(void) {
    lbl_80281E18 = fn_80009B34(sizeof(StaticCams), 2, 0, "GoStaticCam.c", 380);
    lbl_80281E18->pPaths = NULL;
    fn_80064EA4();
}

void fn_80064E74(void) {
    fn_80064EA4();
    fn_80009E70(lbl_80281E18);
    lbl_80281E18 = NULL;
}

// Forget every camera and path.
void fn_80064EA4(void) {
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
        fn_80009E70(lbl_80281E18->pPaths);
    }
    lbl_80281E18->pPaths = NULL;
    lbl_80281E18->nPaths = 0;
}

void fn_80064F54(CamShot* pShot, int nPlayer, f32* pOut) {
    Vec3Copy(pShot->v20, pOut);
}

// A random static camera for shot kind nKind that sees nPlayer's golfer or ball, other than pNot
// (bNotKind5: none with bAC 5). Only course 12 in mode 10 has them, and only for kind 0x20.
CamShot* fn_80064F7C(int nPlayer, int nKind, u8 bNotKind5, CamShot* pNot) {
    int aFound[NUM_STATIC_CAMS];
    int* pFound;
    int i;
    int nFound;

    nFound = 0;
    if (Game_GetCourse() != 12 || fn_80015464() != 10 || nKind != 0x20) {
        return NULL;
    }
    pFound = aFound;
    for (i = 0; i < lbl_80281E18->nStatic; i++) {
        if (!lbl_80281E18->aStatic[i].bA9 && (nKind & lbl_80281E18->aStatic[i].nA4)
            && (!bNotKind5 || lbl_80281E18->aStatic[i].bAC != 5)
            && fn_800659F4(&lbl_80281E18->aStatic[i], nPlayer) && pNot != &lbl_80281E18->aStatic[i]) {
            *pFound++ = i;
            nFound++;
        }
    }
    if (nFound == 0) {
        return NULL;
    }
    return &lbl_80281E18->aStatic[aFound[Rand_Next(1) % nFound]];
}

// Fly-by path nPath's first shot. The first call after loading chains the fly-by cameras into their
// paths (p40 the next shot, p44 the one before), finds each path's first shot and measures the
// paths.
CamShot* fn_8006509C(int nPath) {
    u8 abEnds[NUM_FLYBY_PATHS] = { 0 };   // the path ends on a camera marked -99
    CamShot* pShot;
    CamShot* pPrev;
    CamShot* pNext;
    CamShot* pAfter;
    int i;
    int j;
    u8 bFound;

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
            pShot = &lbl_80281E18->aFlyBy[i];
            if (!pShot->bAD && !pShot->bA9 && pShot->p40 != NULL) {
                lbl_80281E18->apPath[pShot->nA4] = pShot;
            }
            pShot = &lbl_80281E18->aFlyBy[i];
            if (abEnds[pShot->nA4]) {
                pShot->bAA = 0;
            }
        }
        for (i = 0; i < NUM_FLYBY_PATHS; i++) {
            if (lbl_80281E18->apPath[i] != NULL) {
                lbl_80281E18->afPathLength[i] = 0.0f;
                if (lbl_80281E18->apPath != NULL) {   // always true: apPath is an array
                    for (pShot = lbl_80281E18->apPath[i]; pShot->p40 != NULL && pShot->p40->nA4 == i;
                         pShot = pShot->p40) {
                        // fn_8006596C's body
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
                        pShot->f4C = fn_800C79BC(pPrev->v20, pShot->v20, pNext->v20, pAfter->v20);
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

// Fly-by path uPath's spline, or NULL.
FlyByPath* fn_80065424(u32 uPath) {
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

// Fly the camera along fly-by path nPath to share fShare of the path's length: step the spline in
// 0.005 steps until the camera has gone far enough, then home in on the exact distance. The script
// keeps the shot it is on (pShot), the share of that shot's segment (fA0) and the distance so far
// (fA4).
void fn_80065488(CamScript* pScript, int nPath, f32* pCam, f32* pSub, f32* pFov, int nPlayer,
                 f32 fShare) {
    f32 vLast[4];
    f32 vDiff[4];
    CamShot* pPrev;
    CamShot* pNext;
    CamShot* pAfter;
    CamShot* pShot;
    f32 fDist;
    f32 fT;
    f32 fLastT;
    f32 fLastDist;
    f32 fTarget;
    f32 fFrom;
    f32 fStep;
    f32 fLoT;
    f32 fHiT;
    f32 fLoDist;
    f32 fHiDist;
    int i;

    fDist = pScript->fA4;
    fT = pScript->fA0;
    pShot = pScript->pShot;
    fLastT = fT;
    fLastDist = fDist;
    fTarget = fShare * lbl_80281E18->afPathLength[nPath];
    fn_8006596C(pShot, &pPrev, &pNext, &pAfter);
    Vec_Copy(pCam, vLast);
    *pFov = fn_80014278(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[0])));
    if (0.0f == fTarget) {
        fn_800C7480(pPrev->v20, pShot->v20, pNext->v20, pAfter->v20, pPrev->v30, pShot->v30, pNext->v30,
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
                    continue;
                }
                pShot = pShot->p40;
                fn_8006596C(pShot, &pPrev, &pNext, &pAfter);
                fT = 0.0f;
            }
            fn_800C7480(pPrev->v20, pShot->v20, pNext->v20, pAfter->v20, pPrev->v30, pShot->v30, pNext->v30,
                        pAfter->v30, pCam, pSub, pFov, pShot->f78, pNext->f78, fT);
            fn_80065B20(pCam, vLast, vDiff);
            fDist += (f32)fn_80009680(fn_80009744(vDiff));
            if (fTarget > fDist) {
                Vec_Copy(pCam, vLast);
            }
        }
    }
    if (fT != fLastT) {
        // home in: halve the step four times, then interpolate the rest
        fFrom = (fT >= fLastT) ? fLastT : 0.0f;
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
            fn_800C7480(pPrev->v20, pShot->v20, pNext->v20, pAfter->v20, pPrev->v30, pShot->v30, pNext->v30,
                        pAfter->v30, pCam, pSub, pFov, pShot->f78, pNext->f78, fT);
            fn_80065B20(pCam, vLast, vDiff);
            i++;
            fDist = fLastDist + (f32)fn_80009680(fn_80009744(vDiff));
        } while (i < 4);
        if (fDist > fTarget) {
            fT = (fT - fLoT) * (1.0f - (fDist - fTarget) / (fDist - fLoDist)) + fLoT;
            fn_800C7480(pPrev->v20, pShot->v20, pNext->v20, pAfter->v20, pPrev->v30, pShot->v30, pNext->v30,
                        pAfter->v30, pCam, pSub, pFov, pShot->f78, pNext->f78, fT);
            fn_80065B20(pCam, vLast, vDiff);
            fDist = fLastDist + (f32)fn_80009680(fn_80009744(vDiff));
        } else if (fDist < fTarget) {
            fT += (fHiT - fT) * ((fTarget - fDist) / (fHiDist - fDist));
            fn_800C7480(pPrev->v20, pShot->v20, pNext->v20, pAfter->v20, pPrev->v30, pShot->v30, pNext->v30,
                        pAfter->v30, pCam, pSub, pFov, pShot->f78, pNext->f78, fT);
            fn_80065B20(pCam, vLast, vDiff);
            fDist = fLastDist + (f32)fn_80009680(fn_80009744(vDiff));
        }
    }
    pScript->pShot = pShot;
    pScript->pNextShot = pShot->p40;
    pScript->fA4 = fDist;
    pScript->fA0 = fT;
}

// ---- sweep code (not yet cleaned up) ----

void fn_8006596C(CamShot* pShot, CamShot** ppPrev, CamShot** ppNext, CamShot** ppAfter) {
    void* temp_r0;
    void* temp_r4;
    void* temp_r5;
    void* temp_r7;

    temp_r0 = (*(void**)((u8*)(pShot) + 0x44));
    if (temp_r0 != NULL) {
        *ppPrev = temp_r0;
    } else {
        *ppPrev = pShot;
    }
    temp_r7 = (*(void**)((u8*)(pShot) + 0x40));
    if (temp_r7 != NULL) {
        if ((s32) (*(s32*)((u8*)(pShot) + 0xA4)) == (s32) (*(s32*)((u8*)(temp_r7) + 0xA4))) {
            *ppNext = temp_r7;
        } else {
            *ppNext = pShot;
        }
    } else {
        *ppNext = pShot;
    }
    temp_r4 = *ppNext;
    if (temp_r4 != NULL) {
        temp_r5 = (*(void**)((u8*)(temp_r4) + 0x40));
        if (temp_r5 != NULL) {
            if ((s32) (*(s32*)((u8*)(pShot) + 0xA4)) == (s32) (*(s32*)((u8*)(temp_r5) + 0xA4))) {
                *ppAfter = temp_r5;
                return;
            }
            *ppAfter = temp_r4;
            return;
        }
    }
    *ppAfter = temp_r4;
}

// ---- end of sweep code ----

// nPlayer's golfer or ball is inside the shot's area.
u8 fn_800659F4(CamShot* pShot, int nPlayer) {
    f32 v[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    Player* pPlayer;
    int i;

    i = 0;
    pPlayer = &gPlayers[nPlayer];
    do {
        switch (i) {
        case 0:
            Vec3Copy(pPlayer->vBall, v);
            break;
        case 1:
            Vec3Copy(pPlayer->ball.vPos, v);
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
asm void fn_80065AFC(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_80065AFC(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
}
#endif

// pOut gets pA - pB (three floats).
#ifdef __MWERKS__
asm void fn_80065B20(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_80065B20(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif
