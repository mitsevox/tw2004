// TerrainGround.c (our name): the ground under a point on the course's terrain grid: the
// supporting triangle and its height, and the surface type there. Split out of AnimStream's
// extent: its own .sdata2 block (0x80284460-0x80284478) and its callers (GoTerrainCollision,
// Golfer, emotion, GameEffects) show it is terrain code, not animation streaming.

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "endian.h"

#define TER_DONE 8   // pTriFlags bit 3: the triangle is already in the list being built

// Widens the x/z box (fMinX..fMaxX, fMinZ..fMaxZ) to take in a point.
#define TER_GROW_BOX(p)          \
    if ((p)[2] < fMinZ) {        \
        fMinZ = (p)[2];          \
    }                            \
    if ((p)[0] < fMinX) {        \
        fMinX = (p)[0];          \
    }                            \
    if ((p)[2] > fMaxZ) {        \
        fMaxZ = (p)[2];          \
    }                            \
    if ((p)[0] > fMaxX) {        \
        fMaxX = (p)[0];          \
    }

// The ground strips under a quad (four points, x and z), for the shadows: fills pList with runs
// of their triangles, at most nMax, and returns how many. The same strips as the lookups below
// count, less those whose surface has a flag in uSkip; each triangle goes in once even when it
// lies in several cells (its done bit is set while collecting and cleared again at the end).
int fn_800CB950(CourseInfo* pCourse, f32* pA, f32* pB, f32* pC, f32* pD, TerPolyRef* pList, int nMax,
                u32 uSkip) {
    u32 uPinSet;
    f32 fMinZ = 10000.0f;
    f32 fMaxZ = -10000.0f;
    f32 fMinX = fMinZ;
    f32 fMaxX = fMaxZ;
    int nX0;
    int nZ0;
    int nX1;
    int nZ1;
    int nCount = 0;
    int nX;
    int nZ;
    TerPolyRef* pRef;
    f32 (*pVert)[3];
    u8* pFlags;
    u32 uRefs;
    int i;
    int j;
    u8 nRun;

    uPinSet = 1 << Game_CurrentPinSet();
    TER_GROW_BOX(pA);
    TER_GROW_BOX(pB);
    TER_GROW_BOX(pC);
    TER_GROW_BOX(pD);

    nX0 = (int)fn_80035074((fMinX - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    nZ0 = (int)fn_80035074((fMinZ - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
    nX1 = (int)fn_80035074((fMaxX - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    nZ1 = (int)fn_80035074((fMaxZ - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);

    for (nX = nX0; nX <= nX1; nX++) {
        for (nZ = nZ0; nZ <= nZ1; nZ++) {
            if (nX >= 0 && nX < pCourse->nGridWidth && nZ >= 0 && nZ < pCourse->nGridLength) {
                uRefs = pCourse->pGrid[nX + nZ * pCourse->nGridWidth].uRefs;
                pRef = &pCourse->pPolyRefs[uRefs >> 12];
                for (i = (uRefs & 0xFFF) - 1; i >= 0; i--) {
                    if (pRef->n2 != 0) {
                        pRef++;
                    } else if (pRef->u4 != 0
                               && (!(pRef->u4 & uPinSet) || ((pRef->u4 & 0x10) && gSession.nSplitScreen))) {
                        pRef++;
                    } else if (uSkip & gSurfaceTypes[pRef->nSurface].u34) {
                        pRef++;
                    } else {
                        nRun = 0;
                        pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
                        pVert = &pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
                        for (j = pRef->nTris - 1; j >= 0; j--) {
                            if (!(pFlags[2] & TER_DONE)) {
                                // a run starts: where its first triangle is and what it is made of
                                if (nRun == 0) {
                                    pList[nCount].nVertex = pVert - pCourse->pVerts;
                                    pList[nCount].nVertexHi = (pVert - pCourse->pVerts) >> 16;
                                    pList[nCount].nSurface = pRef->nSurface;
                                }
                                nRun++;
                                pFlags[2] |= TER_DONE;
                            } else if (nRun != 0) {
                                pList[nCount].nTris = nRun;
                                if (nCount < nMax - 1) {
                                    nCount++;
                                }
                                nRun = 0;
                            }
                            pVert++;
                            pFlags++;
                        }
                        if (nRun != 0) {
                            pList[nCount].nTris = nRun;
                            if (nCount < nMax - 1) {
                                nCount++;
                            }
                        }
                        pRef++;
                    }
                }
            }
        }
    }

    // clear the done bits again
    for (nX = nX0; nX <= nX1; nX++) {
        for (nZ = nZ0; nZ <= nZ1; nZ++) {
            if (nX >= 0 && nX < pCourse->nGridWidth && nZ >= 0 && nZ < pCourse->nGridLength) {
                uRefs = pCourse->pGrid[nX + nZ * pCourse->nGridWidth].uRefs;
                pRef = &pCourse->pPolyRefs[uRefs >> 12];
                for (i = (uRefs & 0xFFF) - 1; i >= 0; i--) {
                    if (pRef->n2 != 0) {
                        pRef++;
                    } else {
                        pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
                        for (j = pRef->nTris - 1; j >= 0; j--) {
                            pFlags[2] &= 0xF7;   // ~TER_DONE as a byte
                            pFlags++;
                        }
                        pRef++;
                    }
                }
            }
        }
    }
    return nCount;
}

// The supporting ground triangle under a point: the highest one at or below it. Its height
// there, the grid cell, the strip, the triangle's vertices and its number in the strip;
// TER_NO_GROUND when there is none. The strips that count are those of fn_8004C8E0.
f32 fn_800CBEE0(CourseInfo* pCourse, f32* pPos, TerCell** ppCell, TerPolyRef** ppRef, f32 (**ppTri)[3],
                s32* pTri) {
    u32 uPinSet = 1 << Game_CurrentPinSet();
    f32 fBest = TER_NO_GROUND;
    int nX = (int)fn_80035074((pPos[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    int nZ = (int)fn_80035074((pPos[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
    TerCell* pCell;
    TerPolyRef* pRef;
    f32 (*pVert)[3];
    u8* pFlags;
    int i;
    int j;
    f32 fA;
    f32 fB;
    f32 fC;
    f32 fHeight;

    if (nX >= 0 && nX < pCourse->nGridWidth && nZ >= 0 && nZ < pCourse->nGridLength) {
        pCell = &pCourse->pGrid[nX + nZ * pCourse->nGridWidth];
        // the whole cell is above the point
        if (pCell->nMinHeight > pPos[1]) {
            return TER_NO_GROUND;
        }
        pRef = &pCourse->pPolyRefs[pCell->uRefs >> 12];
        for (i = (pCell->uRefs & 0xFFF) - 1; i >= 0; i--) {
            if (pRef->n2 != 0) {
                pRef++;
            } else if (pRef->u4 != 0
                       && (!(pRef->u4 & uPinSet) || ((pRef->u4 & 0x10) && gSession.nSplitScreen))) {
                pRef++;
            } else {
                pVert = &pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
                pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
                for (j = pRef->nTris - 1; j >= 0; j--) {
                    if ((pFlags[2] & 7)
                        && (pPos[1] > pVert[(pFlags[2] >> 6) & 3][1]
                            || fBest < pVert[(pFlags[2] >> 4) & 3][1])
                        && fn_80050A9C(pVert[0], pVert[1], pVert[2], pPos[0], pPos[2])) {
                        fn_800509D8(pVert, pPos, &fA, &fB, &fC);
                        fHeight = fA * pVert[0][1] + fB * pVert[1][1] + fC * pVert[2][1];
                        if (fHeight > fBest && fHeight <= pPos[1]) {
                            *ppCell = pCell;
                            fBest = fHeight;
                            *ppRef = pRef;
                            *ppTri = pVert;
                            *pTri = pRef->nTris - j - 1;
                        }
                    }
                    pVert++;
                    pFlags++;
                }
                pRef++;
            }
        }
        return fBest;
    }
    return TER_NO_GROUND;
}

// The surface type under a point (its supporting triangle's), NULL when there is no ground.
SurfaceType* fn_800CC190(CourseInfo* pCourse, f32* pPos) {
    TerCell* pCell;
    TerPolyRef* pRef;
    f32 (*pTri)[3];
    s32 nTri;

    if (fn_800CBEE0(pCourse, pPos, &pCell, &pRef, &pTri, &nTri) != TER_NO_GROUND) {
        return &gSurfaceTypes[pRef->nSurface];
    }
    return NULL;
}
