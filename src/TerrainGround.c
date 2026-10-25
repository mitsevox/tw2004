// TerrainGround.c: the ground of the current hole's terrain grid: the ground strips under a quad,
// which the golfer's shadow is drawn on, and the supporting ground triangle under a point, with its
// height and its surface.

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "endian.h"

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

// The ground strips of the grid cells under a quad (the x/z box around its four points): fills
// pReturnResult with runs of their triangles (first vertex, triangle count, surface) and returns
// how many, at most iListSize - 1 (once the list is full, the last entry is overwritten). Object
// strips, the cup ground of other pin positions (and in split screen, cup ground marked
// POLYREF_NOT_IN_SPLIT_SCREEN) and strips whose surface has a flag in uiIgnoreFlags are left out.
// A strip listed in several cells goes in once: its triangles' TER_TRI_DONE bits are set while
// collecting and cleared after.
int Ter_GetGroundPolyList(TGD_TerrainInfo* pTerrainData, f32* p1, f32* p2, f32* p3, f32* p4,
                          TGD_PolygonReference* pReturnResult, int iListSize, u32 uiIgnoreFlags) {
    f32 fMinZ = 10000.0f;
    f32 fMaxZ = -10000.0f;
    f32 fMinX = fMinZ;
    f32 fMaxX = fMaxZ;
    int nCount = 0;
    int nX0;
    int nZ0;
    int nX1;
    int nZ1;
    int nX;
    int nZ;
    u32 uPlacementMask;
    TGD_PolygonReference* pPolyRef;
    f32 (*pVertexData)[3];
    u8* pDrawKickData;
    u32 uRefs;
    int i;
    int j;
    u8 nRun;

    uPlacementMask = 1 << Game_CurrentPinSet();
    TER_GROW_BOX(p1);
    TER_GROW_BOX(p2);
    TER_GROW_BOX(p3);
    TER_GROW_BOX(p4);

    nX0 = (int)Math_Floor((fMinX - pTerrainData->fGridOrigin[0]) / pTerrainData->fGridCellSize[0]);
    nZ0 = (int)Math_Floor((fMinZ - pTerrainData->fGridOrigin[1]) / pTerrainData->fGridCellSize[1]);
    nX1 = (int)Math_Floor((fMaxX - pTerrainData->fGridOrigin[0]) / pTerrainData->fGridCellSize[0]);
    nZ1 = (int)Math_Floor((fMaxZ - pTerrainData->fGridOrigin[1]) / pTerrainData->fGridCellSize[1]);

    for (nX = nX0; nX <= nX1; nX++) {
        for (nZ = nZ0; nZ <= nZ1; nZ++) {
            if (nX >= 0 && nX < pTerrainData->nGridWidth && nZ >= 0 && nZ < pTerrainData->nGridLength) {
                uRefs = pTerrainData->pGrid[nX + nZ * pTerrainData->nGridWidth].uRefs;
                pPolyRef = &pTerrainData->pPolyRefs[uRefs >> CELL_REFS_FIRST_SHIFT];
                for (i = (uRefs & CELL_REFS_COUNT_MASK) - 1; i >= 0; i--) {
                    if (pPolyRef->nObject != 0) {
                        pPolyRef++;
                    } else if (pPolyRef->uFlags != 0
                               && (!(pPolyRef->uFlags & uPlacementMask)
                                   || ((pPolyRef->uFlags & POLYREF_NOT_IN_SPLIT_SCREEN)
                                       && gSession.nSplitScreen))) {
                        pPolyRef++;
                    } else if (uiIgnoreFlags & gSurfaceTypes[pPolyRef->nSurface].uFlags) {
                        pPolyRef++;
                    } else {
                        nRun = 0;
                        pDrawKickData = pTerrainData->pTriFlags + TER_FIRST_VERTEX(pPolyRef);
                        pVertexData = &pTerrainData->pVerts[TER_FIRST_VERTEX(pPolyRef)];
                        for (j = pPolyRef->nTris - 1; j >= 0; j--) {
                            if (!(pDrawKickData[2] & TER_TRI_DONE)) {
                                // a run starts: where its first triangle is and what it is made of
                                if (nRun == 0) {
                                    pReturnResult[nCount].nVertex = pVertexData - pTerrainData->pVerts;
                                    pReturnResult[nCount].nVertexHi =
                                        (pVertexData - pTerrainData->pVerts) >> 16;
                                    pReturnResult[nCount].nSurface = pPolyRef->nSurface;
                                }
                                nRun++;
                                pDrawKickData[2] |= TER_TRI_DONE;
                            } else if (nRun != 0) {
                                pReturnResult[nCount].nTris = nRun;
                                if (nCount < iListSize - 1) {
                                    nCount++;
                                }
                                nRun = 0;
                            }
                            pVertexData++;
                            pDrawKickData++;
                        }
                        if (nRun != 0) {
                            pReturnResult[nCount].nTris = nRun;
                            if (nCount < iListSize - 1) {
                                nCount++;
                            }
                        }
                        pPolyRef++;
                    }
                }
            }
        }
    }

    // clear the done bits again, on every ground strip of the cells
    for (nX = nX0; nX <= nX1; nX++) {
        for (nZ = nZ0; nZ <= nZ1; nZ++) {
            if (nX >= 0 && nX < pTerrainData->nGridWidth && nZ >= 0 && nZ < pTerrainData->nGridLength) {
                uRefs = pTerrainData->pGrid[nX + nZ * pTerrainData->nGridWidth].uRefs;
                pPolyRef = &pTerrainData->pPolyRefs[uRefs >> CELL_REFS_FIRST_SHIFT];
                for (i = (uRefs & CELL_REFS_COUNT_MASK) - 1; i >= 0; i--) {
                    if (pPolyRef->nObject != 0) {
                        pPolyRef++;
                    } else {
                        pDrawKickData = pTerrainData->pTriFlags + TER_FIRST_VERTEX(pPolyRef);
                        for (j = pPolyRef->nTris - 1; j >= 0; j--) {
                            pDrawKickData[2] &= (u8)~TER_TRI_DONE;
                            pDrawKickData++;
                        }
                        pPolyRef++;
                    }
                }
            }
        }
    }
    return nCount;
}

// The supporting ground triangle under a point: the highest one at or below it, among the ground
// strips of its cell that Ter_GetGroundPolyList would take, whatever their surface. Returns its
// height there and gives its cell, its strip, its first vertex and that vertex's number in the
// strip; TER_NO_GROUND when the point is off the grid, below the whole cell or above no triangle.
f32 Ter_GetSupportingGroundTriangle(TGD_TerrainInfo* pTerrainData, f32* location, TGD_Cell** pCellResult,
                                    TGD_PolygonReference** pPolyRefResult, f32 (**pTriangleVertices)[3],
                                    s32* iFirstVertexIndex) {
    u32 iCurrentPlacementMask = 1 << Game_CurrentPinSet();
    f32 fHighestPoint = TER_NO_GROUND;
    int gx = (int)Math_Floor((location[0] - pTerrainData->fGridOrigin[0]) / pTerrainData->fGridCellSize[0]);
    int gy = (int)Math_Floor((location[2] - pTerrainData->fGridOrigin[1]) / pTerrainData->fGridCellSize[1]);
    TGD_Cell* pCell;
    TGD_PolygonReference* pPolyRef;
    f32 (*pVertexData)[3];
    u8* pDrawKickData;
    int j;
    int k;
    f32 fA;
    f32 fB;
    f32 fC;
    f32 fHeight;

    if (gx >= 0 && gx < pTerrainData->nGridWidth && gy >= 0 && gy < pTerrainData->nGridLength) {
        pCell = &pTerrainData->pGrid[gx + gy * pTerrainData->nGridWidth];
        if (pCell->nMinHeight > location[1]) {
            return TER_NO_GROUND;
        }
        pPolyRef = &pTerrainData->pPolyRefs[pCell->uRefs >> CELL_REFS_FIRST_SHIFT];
        for (j = (pCell->uRefs & CELL_REFS_COUNT_MASK) - 1; j >= 0; j--) {
            if (pPolyRef->nObject != 0) {
                pPolyRef++;
            } else if (pPolyRef->uFlags != 0
                       && (!(pPolyRef->uFlags & iCurrentPlacementMask)
                           || ((pPolyRef->uFlags & POLYREF_NOT_IN_SPLIT_SCREEN) && gSession.nSplitScreen))) {
                pPolyRef++;
            } else {
                pVertexData = &pTerrainData->pVerts[TER_FIRST_VERTEX(pPolyRef)];
                pDrawKickData = pTerrainData->pTriFlags + TER_FIRST_VERTEX(pPolyRef);
                for (k = pPolyRef->nTris - 1; k >= 0; k--) {
                    // the corner test passes a triangle over only when its lowest corner is at or
                    // above the point and its highest no higher than the best so far
                    if ((pDrawKickData[2] & TER_TRI_KICK_MASK)
                        && (location[1] > pVertexData[TER_TRI_LOWEST_CORNER(pDrawKickData[2])][1]
                            || fHighestPoint < pVertexData[TER_TRI_HIGHEST_CORNER(pDrawKickData[2])][1])
                        && Ter_PointInTriangleXZpY(pVertexData[0], pVertexData[1], pVertexData[2],
                                                   location[0], location[2])) {
                        Ter_GetBarycentricCoords(pVertexData, location, &fA, &fB, &fC);
                        fHeight = fA * pVertexData[0][1] + fB * pVertexData[1][1] + fC * pVertexData[2][1];
                        if (fHeight > fHighestPoint && fHeight <= location[1]) {
                            *pCellResult = pCell;
                            fHighestPoint = fHeight;
                            *pPolyRefResult = pPolyRef;
                            *pTriangleVertices = pVertexData;
                            *iFirstVertexIndex = pPolyRef->nTris - k - 1;
                        }
                    }
                    pVertexData++;
                    pDrawKickData++;
                }
                pPolyRef++;
            }
        }
        return fHighestPoint;
    }
    return TER_NO_GROUND;
}

// The surface of the supporting ground triangle under a point; NULL when there is none.
TGD_MaterialInfo* Ter_GetSupportingGroundMaterial(TGD_TerrainInfo* pTerrainData, f32* location) {
    TGD_Cell* pCell;
    TGD_PolygonReference* pPolyRef;
    f32 (*pTriangleVertices)[3];
    s32 iFirstVertexIndex;

    if (Ter_GetSupportingGroundTriangle(pTerrainData, location, &pCell, &pPolyRef, &pTriangleVertices,
                                        &iFirstVertexIndex)
        != TER_NO_GROUND) {
        return &gSurfaceTypes[pPolyRef->nSurface];
    }
    return NULL;
}
