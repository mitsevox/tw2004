// GoTerrainCollision.c: the course's ground as collision data (TGD_TerrainInfo): the hole's
// in-bounds and free-drop outlines, where a ball may be dropped, the light, height and surface under
// a point, and the ball's collision with the ground, the pin and the course objects.

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "endian.h"
#include "dynobj.h"

#define AXIS3(n) ((n) == 0 ? 0 : 2)    // grid axis 0 (x) or 1 (z) as an index into a 3D vector
#define TER_NUM_CHECK_POINTS 5         // Ter_CheckObjectAndHazardObstruction: the spot and its four corners
// The course data keeps each list's offset from its own start in the pointer field, big-endian;
// Ter_InitTGD turns it into the pointer in place.
#define TER_RELOCATE(pTerrainData, field) \
    ((pTerrainData)->field = (void*)((u8*)(pTerrainData) + BE32(&(pTerrainData)->field)))

u8        gTerObjectMarks[MAX_OBJECTS];         // 1: a course object near the line being tested
                                                  // (Ter_MarkObjectsNearLine; entry 0 always 1)
TNetwork* gTerOOBNetworks[MAX_OOB_NETWORKS];      // the hole's in-bounds outlines
TNetwork* gTerFreeDropNetworks[MAX_FREE_DROP_NETWORKS];    // the hole's free-drop outlines
TerBox    gTerCupGeometryBounds[NUM_CUP_POSITIONS];        // per pin position: its cup geometry's box
s32       gTerNumOOBNetworks;                     // outlines in gTerOOBNetworks
s32       gTerNumFreeDropNetworks;                // outlines in gTerFreeDropNetworks
u8        gTerUse3DCupGeometry;                   // 1: the course has cup geometry (Ter_InitTGD)

static void Ter_FreeDropNetworkLoadCallback(TNetwork* netPtr);
static u8   Ter_LieIsPreferred(u32 lieID);
static void Ter_OOBNetworkLoadCallback(TNetwork* netPtr);
static void LLMath_Subtract3(const f32* src1, const f32* src2, f32* dest);
static void LLMath_Negate3(const f32* src, f32* dest);
static void LLMath_Negate(const f32* src, f32* dest);
static void Ter_ComputeHighestPointInEveryTriangle(TGD_TerrainInfo* pTerrainData);
static f32 Ter_CalcLowestPlayableWorldHeight(TGD_TerrainInfo* pTerrainData);
static f32 Ter_GetLowestGroundTriangle(TGD_TerrainInfo* pTerrainData, f32* location, TGD_Cell** pCellResult,
                                       TGD_PolygonReference** pPolyRefResult, f32 (**pTriangleVertices)[3],
                                       s32* iFirstVertexIndex);
static f32 Ter_GetCoveringGroundTriangle(TGD_TerrainInfo* pTerrainData, f32* location, TGD_Cell** pCellResult,
                                         TGD_PolygonReference** pPolyRefResult, f32 (**pTriangleVertices)[3],
                                         s32* iFirstVertexIndex);
static u8 Ter_CheckForWorldCollisionOneGrid(TGD_TerrainInfo* pTerrainData, int nX, int nZ, f32* pFrom,
                                            f32* pTo, f32* pDir, f32 fMax, f32* pCollisionPoint,
                                            f32* pSurfaceNormal, TGD_MaterialInfo** pSurfaceInfo,
                                            TGD_ObjectInstanceInfo** pSurfaceObject, u8* pDrawKick);
static u8 Ter_CheckForSolidWorldCollisionOneGrid(TGD_TerrainInfo* pTerrainData, int nX, int nZ, f32* pFrom,
                                                 f32* pTo, f32* pDir, f32 fMax, f32* pCollisionPoint,
                                                 f32* pSurfaceNormal, TGD_MaterialInfo** pSurfaceInfo,
                                                 TGD_ObjectInstanceInfo** pSurfaceObject);
static u8 Ter_CheckForGroundCollisionOneGrid(TGD_TerrainInfo* pTerrainData, int nX, int nZ, f32* pFrom,
                                             f32* pTo, f32* pDir, f32 fMax, f32* pCollisionPoint,
                                             f32* pSurfaceNormal, TGD_MaterialInfo** pSurfaceInfo,
                                             TGD_ObjectInstanceInfo** pSurfaceObject);
static u8 Ter_CheckForObjectCollisionOneGrid(TGD_TerrainInfo* pTerrainData, int nX, int nZ, f32* pFrom,
                                             f32* pTo, f32* pDir, f32 fMax, f32* pCollisionPoint,
                                             f32* pSurfaceNormal, TGD_MaterialInfo** pSurfaceInfo,
                                             TGD_ObjectInstanceInfo** pSurfaceObject);
static u8 Ter_LineSphereIntersection(f32* pFrom, f32* pDir, f32 fRange, f32* pCentre, f32 fRadius);
static int TerCollision_GetMeshFlags(UObjMesh* pMesh, int i);

static inline f32 Ter_Floor(f32 x) {
    return Math_Floor(x);
}

// The grid cell a coordinate falls in along one axis, given as cells from the grid's corner (the
// cell may be outside the grid).
static inline int Ter_GridCell(f32 fCells) {
    return (int)Ter_Floor(fCells);
}

// fake match: never called, puts 1.0f first in the constant pool
static f32 Ter_OrderConstants(f32 x) {
    return x + 1.0f;
}

// Where the line from pFrom along pDir meets the triangle pTri, as a multiple t of pDir with
// 0 < t < fMax: t, the point and the triangle's unit normal. A line (nearly) parallel to the
// triangle's plane never meets it.
static u8 Ter_LineTriangleIntersection(f32* pFrom, f32* pDir, f32 fMax, f32 (*pTri)[3], f32* pT,
                                       f32* pHit, f32* pNormal) {
    f32 vP[4];
    f32 vQ[4];
    f32 vS[4];
    f32 vE1[4];
    f32 vE2[4];
    f32 vA[4];
    f32 vB[4];
    f32 vC[4];
    f32 fDet;
    f32 fInv;
    f32 fU;
    f32 fV;
    f32 fT;

    Vec3Copy(pTri[0], vA);
    Vec3Copy(pTri[1], vB);
    Vec3Copy(pTri[2], vC);
    LLMath_Subtract3(vB, vA, vE1);
    LLMath_Subtract3(vC, vA, vE2);
    vec4flt_CrossProduct(pDir, vE2, vP);
    fDet = LLMath_DotProduct3(vE1, vP);
    if (fDet > -0.00001f && fDet < 0.00001f) return 0;
    fInv = 1.0f / fDet;
    LLMath_Subtract3(pFrom, vA, vS);
    fU = fInv * LLMath_DotProduct3(vS, vP);
    if (fU < 0.0f || fU > 1.0f) return 0;
    vec4flt_CrossProduct(vS, vE1, vQ);
    fV = fInv * LLMath_DotProduct3(pDir, vQ);
    if (fV < 0.0f || fU + fV > 1.0f) return 0;
    fT = fInv * LLMath_DotProduct3(vE2, vQ);
    if (fT <= 0.0f || fT >= fMax) return 0;
    *pT = fT;
    pHit[0] = fT * pDir[0] + pFrom[0];
    pHit[1] = fT * pDir[1] + pFrom[1];
    pHit[2] = fT * pDir[2] + pFrom[2];
    vec4flt_CrossProduct(vE1, vE2, pNormal);
    LLMath_Normalize3(pNormal, pNormal);
    return 1;
}

// Registers the loaders of the hole's in-bounds (NETWORK_TYPE_OUT_OF_BOUNDS) and free-drop
// (NETWORK_TYPE_FREE_DROP) outlines and forgets the old ones. Called when a round starts
// (GO_vInitIG).
void Ter_Init(void) {
    Network_RegisterLoadNetworkCallback(NETWORK_TYPE_OUT_OF_BOUNDS, Ter_OOBNetworkLoadCallback);
    Network_RegisterLoadNetworkCallback(NETWORK_TYPE_FREE_DROP, Ter_FreeDropNetworkLoadCallback);
    gTerNumOOBNetworks = 0;
    gTerNumFreeDropNetworks = 0;
}

// Readies a hole's course data once it is loaded (Ter_CourseLoadCallback, with
// gTerRenderer.pCourse): its offsets become pointers. A strip for exactly one pin position (one
// bit of POLYREF_PIN_POSITIONS) is part of that position's 3D cup geometry; when the course has
// any, each pin position's pin goes to the centre top of its geometry's box (the hole's pin set
// is set to each position in turn, then put back). The hole's outlines are forgotten, the
// triangles' highest and lowest corners marked and the course floor (fFloor) worked out.
void Ter_InitTGD(TGD_TerrainInfo* pTerrainData) {
    u32 i;
    int k;
    int j;
    int nBit;
    int nPinSet;
    f32 (*pVertexData)[3];

    TER_RELOCATE(pTerrainData, pVerts);
    TER_RELOCATE(pTerrainData, pTriFlags);
    TER_RELOCATE(pTerrainData, pLight);
    TER_RELOCATE(pTerrainData, pGrid);
    TER_RELOCATE(pTerrainData, pObjects);
    TER_RELOCATE(pTerrainData, pPolyRefs);
    TER_RELOCATE(pTerrainData, pObjRefs);
    if (pTerrainData->pFog != NULL) {
        TER_RELOCATE(pTerrainData, pFog);
    }
    if (pTerrainData->pRainFog != NULL) {
        TER_RELOCATE(pTerrainData, pRainFog);
    }
    if (pTerrainData->pGlow != NULL) {
        TER_RELOCATE(pTerrainData, pGlow);
    }
    if (pTerrainData->pUnread != NULL) {
        TER_RELOCATE(pTerrainData, pUnread);
    }
    gTerUse3DCupGeometry = 0;
    for (k = 0; k < NUM_CUP_POSITIONS; k++) {
        gTerCupGeometryBounds[k].vMin[0] = 1000000.0f;
        gTerCupGeometryBounds[k].vMin[1] = 1000000.0f;
        gTerCupGeometryBounds[k].vMin[2] = 1000000.0f;
        gTerCupGeometryBounds[k].vMax[0] = -1000000.0f;
        gTerCupGeometryBounds[k].vMax[1] = -1000000.0f;
        gTerCupGeometryBounds[k].vMax[2] = -1000000.0f;
    }
    for (i = 0; i < pTerrainData->nPolyRefs; i++) {
        nBit = 1;
        for (k = 0; k < NUM_CUP_POSITIONS; k++) {
            if (nBit == (pTerrainData->pPolyRefs[i].uFlags & POLYREF_PIN_POSITIONS)) {
                gTerUse3DCupGeometry = 1;
                pVertexData = &pTerrainData->pVerts[TER_FIRST_VERTEX(&pTerrainData->pPolyRefs[i])];
                for (j = 0; j < pTerrainData->pPolyRefs[i].nTris + 2; j++) {
                    if (pVertexData[0][0] < gTerCupGeometryBounds[k].vMin[0]) {
                        gTerCupGeometryBounds[k].vMin[0] = pVertexData[0][0];
                    }
                    if (pVertexData[0][1] < gTerCupGeometryBounds[k].vMin[1]) {
                        gTerCupGeometryBounds[k].vMin[1] = pVertexData[0][1];
                    }
                    if (pVertexData[0][2] < gTerCupGeometryBounds[k].vMin[2]) {
                        gTerCupGeometryBounds[k].vMin[2] = pVertexData[0][2];
                    }
                    if (pVertexData[0][0] > gTerCupGeometryBounds[k].vMax[0]) {
                        gTerCupGeometryBounds[k].vMax[0] = pVertexData[0][0];
                    }
                    if (pVertexData[0][1] > gTerCupGeometryBounds[k].vMax[1]) {
                        gTerCupGeometryBounds[k].vMax[1] = pVertexData[0][1];
                    }
                    if (pVertexData[0][2] > gTerCupGeometryBounds[k].vMax[2]) {
                        gTerCupGeometryBounds[k].vMax[2] = pVertexData[0][2];
                    }
                    pVertexData++;
                }
            }
            nBit <<= 1;
        }
    }
    if (gTerUse3DCupGeometry) {
        nPinSet = Game_CurrentPinSet();
        for (k = 0; k < NUM_CUP_POSITIONS; k++) {
            gpGame->nPinSet[Game_CurHoleIndex()] = k;
            gTerRenderer.pCourse->pin[k].x
                = (gTerCupGeometryBounds[k].vMin[0] + gTerCupGeometryBounds[k].vMax[0]) / 2.0f;
            gTerRenderer.pCourse->pin[k].z
                = (gTerCupGeometryBounds[k].vMin[2] + gTerCupGeometryBounds[k].vMax[2]) / 2.0f;
            gTerRenderer.pCourse->pin[k].y = gTerCupGeometryBounds[k].vMax[1];
            gTerRenderer.pCourse->pin[k].w = 1.0f;
        }
        gpGame->nPinSet[Game_CurHoleIndex()] = nPinSet;
    }
    gpGame->pPinPos = &gTerRenderer.pCourse->pin[Game_CurrentPinSet()].x;
    gTerNumOOBNetworks = 0;
    gTerNumFreeDropNetworks = 0;
    Ter_ComputeHighestPointInEveryTriangle(gTerRenderer.pCourse);
    gTerRenderer.pCourse->fFloor = Ter_CalcLowestPlayableWorldHeight(gTerRenderer.pCourse);
}

// Whether the cup is real geometry the ball drops into; without it, GM_CheckForBallInHole counts
// a ball within half a yard of the pin as holed.
u8 Ter_Use3DCupGeometry(void) {
    return gTerUse3DCupGeometry;
}

// The free-drop outlines' loader (Ter_Init registers it): adds a loaded outline to the hole's
// list, with no check against MAX_FREE_DROP_NETWORKS.
static void Ter_FreeDropNetworkLoadCallback(TNetwork* netPtr) {
    gTerFreeDropNetworks[gTerNumFreeDropNetworks] = netPtr;
    gTerNumFreeDropNetworks++;
}

// Whether a point (x, z) is inside one of the hole's free-drop areas.
u8 Ter_PointInFreeDropNetwork(f32* location) {
    int i;

    if (gTerNumFreeDropNetworks == 0) return 0;
    for (i = 0; i < gTerNumFreeDropNetworks; i++) {
        if (wn_PnPoly(location, gTerFreeDropNetworks[i], gTerFreeDropNetworks[i]->nNumNodes)) return 1;
    }
    return 0;
}

// The in-bounds outlines' loader (Ter_Init registers it): adds a loaded outline to the hole's
// list, with no check against MAX_OOB_NETWORKS.
static void Ter_OOBNetworkLoadCallback(TNetwork* netPtr) {
    gTerOOBNetworks[gTerNumOOBNetworks] = netPtr;
    gTerNumOOBNetworks++;
}

// How many in-bounds outlines the hole has.
s32 Ter_iNumOOBNetworksLoaded(void) {
    return gTerNumOOBNetworks;
}

// Whether a point (x, z) is in bounds: inside one of the hole's in-bounds outlines. A hole with
// none has no out of bounds, so every point is in.
u8 Ter_PointInOOBNetwork(f32* location) {
    int i;

    if (gTerNumOOBNetworks == 0) return 1;
    for (i = 0; i < gTerNumOOBNetworks; i++) {
        if (wn_PnPoly(location, gTerOOBNetworks[i], gTerOOBNetworks[i]->nNumNodes)) return 1;
    }
    return 0;
}

// Whether the segment from start to end crosses one of the hole's in-bounds outlines (x, z);
// intersect gets the crossing nearest start on the first outline, in load order, that it crosses.
u8 Ter_CollisionWithOOBNetwork(f32* start, f32* end, f32* intersect) {
    int i;

    if (gTerNumOOBNetworks == 0) return 0;
    for (i = 0; i < gTerNumOOBNetworks; i++) {
        if (Network_LineNetworkIntersection(start, end, gTerOOBNetworks[i], gTerOOBNetworks[i]->nNumNodes,
                                            intersect)) return 1;
    }
    return 0;
}

// The light on the ground at a point, 0..1: the light byte of the first vertex of the triangle
// found (TGD_TerrainInfo.pLight). The ground covering the point counts when it is at most a
// quarter of a yard above it, else the ground under it, else the lowest ground. 1 (full light)
// with no ground under or over the point.
f32 Ter_GetAmbientLight(TGD_TerrainInfo* pTerrainData, f32* position) {
    TGD_Cell* pCell;
    TGD_PolygonReference* pPolyRef;
    f32 (*pVertexData)[3];
    s32 iFirstVertexIndex;
    f32 fHeight;

    fHeight = Ter_GetCoveringGroundTriangle(pTerrainData, position, &pCell, &pPolyRef, &pVertexData,
                                            &iFirstVertexIndex);
    if (fHeight == TER_NO_GROUND || fHeight > position[1] + 0.25f) {
        fHeight = Ter_GetSupportingGroundTriangle(pTerrainData, position, &pCell, &pPolyRef, &pVertexData,
                                                  &iFirstVertexIndex);
        if (fHeight == TER_NO_GROUND) {
            fHeight = Ter_GetLowestGroundTriangle(pTerrainData, position, &pCell, &pPolyRef, &pVertexData,
                                                  &iFirstVertexIndex);
        }
    }
    if (fHeight != TER_NO_GROUND && pPolyRef->nObject == 0) {
        return (pTerrainData->pLight + TER_FIRST_VERTEX(pPolyRef))[iFirstVertexIndex] / 255.0f;
    }
    return 1.0f;
}

// fake match: the first column through an inline with a local, so its start is stored before the loop
static inline int Ter_FloorToInt(f32 fCells) {
    int n = (int)Math_Floor(fCells);
    return n;
}

// fake match: an identity, for the registers around fWide
static inline f32 Ter_Identity(f32 f) {
    return f;
}

// Whether something spoils a spot for a ball. The ground is looked up at the spot and at the
// four corners hazardDistance away in x and z: the highest ground at most 2 x hazardDistance
// above the spot. The spot is spoilt by a course object of the grid cells around it whose base
// is within obstructionDistance of it (obstructionDistance + 0.5 when the base's radius is over
// 4 yards; with bOnlyCheckNonFadingObjects, only an object whose model has UOBJMESH_F0_NO_FADE),
// unless the ground at the spot is the fairway, the green or sand (SURFCLASS_FAIRWAY_e,
// SURFCLASS_GREEN_e, SURFCLASS_SAND_e). With bCheckLie it is spoilt by a free-drop area at any of
// the five points (looked up once the cells show any ground), ground at the spot without
// MATERIAL_BALL_CAN_LIE, or at a corner no ground or ground with neither MATERIAL_BALL_CAN_LIE
// nor SURFCLASS_CARTPATH_e; with bCheckHeight, by a corner's ground more than
// hazardDistance x fMaxSlope above or below the spot's.
u8 Ter_CheckObjectAndHazardObstruction(f32* position, f32 obstructionDistance, u8 bOnlyCheckNonFadingObjects,
                                       u8 bCheckLie, f32 hazardDistance, u8 bCheckHeight, f32 fMaxSlope) {
    int j;
    TGD_Cell* pCell;
    f32 aHeight[TER_NUM_CHECK_POINTS];
    TGD_PolygonReference* apPolyRef[TER_NUM_CHECK_POINTS];
    f32 vPos[4];
    u8 abFreeDrop[TER_NUM_CHECK_POINTS];
    f32 fA;
    f32 fB;
    f32 fC;
    f32 fA2;
    f32 fB2;
    f32 fC2;
    u8 bFirst;
    int nRef;
    TGD_TerrainInfo* pTerrainData;
    f32 fMinX;
    f32 fMaxX;
    f32 fMinZ;
    f32 fMaxZ;
    f32 fTop;
    f32 fWide;
    f32 fDist;
    f32 fDx;
    f32 fDz;
    f32 fObjRadius;
    f32 fHeight;
    int nMinZ;
    int nMaxX;
    int nMaxZ;
    int nX;
    int nMinX;
    int nZ;
    int i;
    int k;
    u8 bObstructed;
    int nSideX;
    int nSideZ;
    TGD_PolygonReference* pPolyRef;
    u16* pObjRef;
    f32 (*pVertexData)[3];
    u8* pDrawKickData;
    u8 uiDrawKick;
    UObjMesh* pModel;

    bObstructed = 0;
    bFirst = 1;
    pTerrainData = Ter_GetTGD();
    if (pTerrainData == NULL) return 0;
    for (i = 0; i < TER_NUM_CHECK_POINTS; i++) {
        aHeight[i] = -50000.0f;
        abFreeDrop[i] = 0;
        apPolyRef[i] = NULL;
    }
    fMinX = position[0] - (obstructionDistance <= hazardDistance ? hazardDistance : obstructionDistance);
    fMaxX = position[0] + (obstructionDistance <= hazardDistance ? hazardDistance : obstructionDistance);
    fMinZ = position[2] - (obstructionDistance <= hazardDistance ? hazardDistance : obstructionDistance);
    fMaxZ = position[2] + (obstructionDistance <= hazardDistance ? hazardDistance : obstructionDistance);
    fTop = position[1] + 2.0f * hazardDistance;
    nMinX = Ter_FloorToInt((fMinX - pTerrainData->fGridOrigin[0]) / pTerrainData->fGridCellSize[0]);
    nMinZ = (int)Ter_Floor((fMinZ - pTerrainData->fGridOrigin[1]) / pTerrainData->fGridCellSize[1]);
    nMaxX = (int)Ter_Floor((fMaxX - pTerrainData->fGridOrigin[0]) / pTerrainData->fGridCellSize[0]);
    nMaxZ = (int)Ter_Floor((fMaxZ - pTerrainData->fGridOrigin[1]) / pTerrainData->fGridCellSize[1]);
    fWide = Ter_Identity(0.5f + obstructionDistance);
    for (nX = nMinX; nX <= nMaxX; nX++) {
        for (nZ = nMinZ; nZ <= nMaxZ; nZ++) {
            if (nX >= 0 && nX < pTerrainData->nGridWidth && nZ >= 0 && nZ < pTerrainData->nGridLength) {
                pCell = &pTerrainData->pGrid[nX + nZ * pTerrainData->nGridWidth];
                pObjRef = &pTerrainData->pObjRefs[pCell->nObjRefOffset];
                for (i = pCell->nObjRefs - 1; i >= 0; i--) {
                    fDx = pTerrainData->pObjects[*pObjRef].vBase[0] - position[0];
                    fDz = pTerrainData->pObjects[*pObjRef].vBase[2] - position[2];
                    fObjRadius = pTerrainData->pObjects[*pObjRef].fBaseRadius;
                    fDist = (f32)Math_Sqrt(fDx * fDx + fDz * fDz) - fObjRadius;
                    if (fDist < obstructionDistance || (fObjRadius > 4.0f && fDist < fWide)) {
                        if (bOnlyCheckNonFadingObjects) {
                            pModel = Ter_GetObjectListModel(pTerrainData->pObjects[*pObjRef].nPatch,
                                                 pTerrainData->pObjects[*pObjRef].nObjList);
                            if (pModel != NULL
                                && (TerCollision_GetMeshFlags(pModel, 0) & UOBJMESH_F0_NO_FADE)) {
                                bObstructed = 1;
                            }
                        } else {
                            bObstructed = 1;
                        }
                    }
                    pObjRef++;
                }
                pPolyRef = &pTerrainData->pPolyRefs[pCell->uRefs >> CELL_REFS_FIRST_SHIFT];
                for (nRef = (pCell->uRefs & CELL_REFS_COUNT_MASK) - 1; nRef >= 0; nRef--) {
                    if (pPolyRef->nObject != 0) {
                        pPolyRef++;
                    } else {
                        pVertexData = &pTerrainData->pVerts[TER_FIRST_VERTEX(pPolyRef)];
                        pDrawKickData = pTerrainData->pTriFlags + TER_FIRST_VERTEX(pPolyRef);
                        for (j = pPolyRef->nTris - 1; j >= 0; j--) {
                            uiDrawKick = pDrawKickData[2];
                            if (bFirst) {
                                abFreeDrop[0] = Ter_PointInFreeDropNetwork(position);
                            }
                            if ((uiDrawKick & TER_TRI_KICK_MASK)
                                && (fTop > pVertexData[TER_TRI_LOWEST_CORNER(uiDrawKick)][1]
                                    || aHeight[0] < pVertexData[TER_TRI_HIGHEST_CORNER(uiDrawKick)][1])
                                && Ter_PointInTriangleXZpY(pVertexData[0], pVertexData[1], pVertexData[2],
                                                           position[0], position[2])) {
                                Ter_GetBarycentricCoords(pVertexData, position, &fA, &fB, &fC);
                                fHeight = fA * pVertexData[0][1] + fB * pVertexData[1][1]
                                          + fC * pVertexData[2][1];
                                if (fHeight > aHeight[0] && fHeight <= fTop) {
                                    aHeight[0] = fHeight;
                                    apPolyRef[0] = pPolyRef;
                                }
                            }
                            k = 1;
                            vPos[0] = position[0] - hazardDistance;
                            for (nSideX = 0; nSideX < 2; nSideX++) {
                                vPos[2] = position[2] - hazardDistance;
                                for (nSideZ = 0; nSideZ < 2; nSideZ++) {
                                    if (bFirst) {
                                        abFreeDrop[k] = Ter_PointInFreeDropNetwork(vPos);
                                    }
                                    if ((uiDrawKick & TER_TRI_KICK_MASK)
                                        && (fTop > pVertexData[TER_TRI_LOWEST_CORNER(uiDrawKick)][1]
                                            || aHeight[k]
                                                   < pVertexData[TER_TRI_HIGHEST_CORNER(uiDrawKick)][1])
                                        && Ter_PointInTriangleXZpY(pVertexData[0], pVertexData[1],
                                                                   pVertexData[2], vPos[0], vPos[2])) {
                                        Ter_GetBarycentricCoords(pVertexData, vPos, &fA2, &fB2, &fC2);
                                        fHeight = fA2 * pVertexData[0][1] + fB2 * pVertexData[1][1]
                                                  + fC2 * pVertexData[2][1];
                                        if (fHeight > aHeight[k] && fHeight <= fTop) {
                                            aHeight[k] = fHeight;
                                            apPolyRef[k] = pPolyRef;
                                        }
                                    }
                                    vPos[2] += 2.0f * hazardDistance;
                                    k++;
                                }
                                vPos[0] += 2.0f * hazardDistance;
                            }
                            bFirst = 0;
                            pVertexData++;
                            pDrawKickData++;
                        }
                        pPolyRef++;
                    }
                }
            }
        }
    }
    if (apPolyRef[0] != NULL) {
        if (gSurfaceTypes[apPolyRef[0]->nSurface].nClass == SURFCLASS_GREEN_e
            || gSurfaceTypes[apPolyRef[0]->nSurface].nClass == SURFCLASS_FAIRWAY_e
            || gSurfaceTypes[apPolyRef[0]->nSurface].nClass == SURFCLASS_SAND_e) {
            bObstructed = 0;
        }
    }
    if (bObstructed) return 1;
    if (bCheckLie) {
        if ((apPolyRef[0] != NULL && !(gSurfaceTypes[apPolyRef[0]->nSurface].uFlags & MATERIAL_BALL_CAN_LIE))
            || abFreeDrop[0]) {
            return 1;
        }
        for (k = 1; k < TER_NUM_CHECK_POINTS; k++) {
            if (abFreeDrop[k] || apPolyRef[k] == NULL
                || (apPolyRef[k] != NULL
                    && !(gSurfaceTypes[apPolyRef[k]->nSurface].uFlags & MATERIAL_BALL_CAN_LIE)
                    && gSurfaceTypes[apPolyRef[k]->nSurface].nClass != SURFCLASS_CARTPATH_e)) {
                return 1;
            }
        }
    }
    if (bCheckHeight) {
        for (k = 1; k < TER_NUM_CHECK_POINTS; k++) {
            if (apPolyRef[k] != NULL && fabsf(aHeight[k] - aHeight[0]) > hazardDistance * fMaxSlope) return 1;
        }
    }
    return 0;
}

// Where a player's ball is to be dropped (dropPosition): the last spot where it could have been
// dropped (gBallDropSpot) or, with bUsePreferredLies, the last such spot with a preferred lie
// (gBallPreferredLieSpot) when that is not where the shot started and is less than 10 yards
// further from the ball. bUsePreferredLies is dropped for a ball that went further from the
// shot's start than the pin is. When the spot is over 3 yards from the ball, or (without
// bUsePreferredLies) on ground of another surface class than the ball's, rings 1 to 4 yards
// around the ball are searched, 8 points a ring (only the first of them from SURFCLASS_WATER_e),
// for a drop (a preferred one with bUsePreferredLies) on the ball's surface class, else a nearer
// spot (see docs/game/ea-bugs.md). Returns 0 when the spot is where the shot started (x, z), or,
// with bRevertToTee and the shot played from the tee (vBall at vTeePos), when it is within 50
// yards of the tee.
u8 Ter_SearchForDropLocation(int PlayerID, u8 bUsePreferredLies, u8 bRevertToTee, f32* dropPosition) {
    f32 angle;
    f32 testPosition[4];
    f32 delta[4];
    TGD_MaterialInfo* pDropMaterial;
    u8 bIsAcceptable;
    u8 bIsPreferred;
    TGD_TerrainInfo* pTerrainData;
    f32 fPinDistSq;
    f32 fPreferredDropDistance;
    f32 fAcceptableDropDistance;
    f32 distanceFromBall;
    f32 dropPositionDistance;
    f32 fTurn;
    f32 tempSin;
    f32 angleToCup;
    f32 tempCos;
    int nRing;
    Player* pPlayer;
    TGD_MaterialInfo* pNormalDropLocationMaterial;

    pPlayer = &gPlayers[PlayerID];
    pTerrainData = Ter_GetTGD();
    fPinDistSq = LLMath_SquareDistanceBetween3(&pTerrainData->pin[Game_CurrentPinSet()].x,
                                               pPlayer->ball.vStart);
    if (LLMath_SquareDistanceBetween3(pPlayer->ball.vPos, pPlayer->ball.vStart) > fPinDistSq) {
        bUsePreferredLies = 0;
    }
    if (bUsePreferredLies) {
        fPreferredDropDistance =
            LLMath_DistanceBetween3(gBallPreferredLieSpot[PlayerID], pPlayer->ball.vPos);
        fAcceptableDropDistance = LLMath_DistanceBetween3(gBallDropSpot[PlayerID], pPlayer->ball.vPos);
        if ((pPlayer->ball.vStart[0] != gBallPreferredLieSpot[PlayerID][0]
             || pPlayer->ball.vStart[1] != gBallPreferredLieSpot[PlayerID][1]
             || pPlayer->ball.vStart[2] != gBallPreferredLieSpot[PlayerID][2])
            && fPreferredDropDistance - fAcceptableDropDistance < 10.0f) {
            LLMath_CopyVec(gBallPreferredLieSpot[PlayerID], dropPosition);
        } else {
            LLMath_CopyVec(gBallDropSpot[PlayerID], dropPosition);
        }
    } else {
        LLMath_CopyVec(gBallDropSpot[PlayerID], dropPosition);
    }
    testPosition[0] = dropPosition[0];
    testPosition[1] = 1000000.0f;
    testPosition[2] = dropPosition[2];
    testPosition[3] = 1.0f;
    pNormalDropLocationMaterial = Ter_GetSupportingGroundMaterial(pTerrainData, testPosition);
    dropPositionDistance = LLMath_SquareDistanceBetween3(dropPosition, pPlayer->ball.vPos);
    if (dropPositionDistance > 9.0f
        || (!bUsePreferredLies
            && pNormalDropLocationMaterial->nClass != gSurfaceTypes[pPlayer->ball.nSurface].nClass)) {
        LLMath_Subtract3(&pTerrainData->pin[Game_CurrentPinSet()].x, pPlayer->ball.vPos, delta);
        LLMath_Normalize3(delta, delta);
        angleToCup = atan2f(delta[0], delta[2]);
        distanceFromBall = 1.0f;
        for (nRing = 0; nRing < 4; nRing++) {
            for (fTurn = 0.0f; fTurn <= 6.265732f; fTurn += 0.7853982f) {
                angle = angleToCup + fTurn;
                tempSin = sinf(angle);
                tempCos = cosf(angle);
                testPosition[0] = tempCos * distanceFromBall + pPlayer->ball.vPos[0];
                testPosition[2] = tempSin * distanceFromBall + pPlayer->ball.vPos[2];
                if (gSurfaceTypes[pPlayer->ball.nSurface].nClass == SURFCLASS_WATER_e
                    && fTurn > 0.0f) continue;
                testPosition[1] = pPlayer->ball.vPos[1] + 2.0f * distanceFromBall;
                testPosition[1] = Ter_CheckForDropLocation(pTerrainData, testPosition, 0, &bIsAcceptable,
                                                           &bIsPreferred, &pDropMaterial);
                if ((bUsePreferredLies && bIsPreferred) || (!bUsePreferredLies && bIsAcceptable)) {
                    if (pDropMaterial->nClass == gSurfaceTypes[pPlayer->ball.nSurface].nClass) {
                        LLMath_CopyVec(testPosition, dropPosition);
                        goto done;
                    }
                    if (distanceFromBall < dropPositionDistance) {
                        LLMath_CopyVec(testPosition, dropPosition);
                        dropPositionDistance = distanceFromBall;
                    }
                }
            }
            distanceFromBall += 1.0f;
        }
    }
done:
    if (pPlayer->ball.vStart[0] == dropPosition[0] && pPlayer->ball.vStart[2] == dropPosition[2]) return 0;
    if (bRevertToTee && pPlayer->vTeePos[0] == pPlayer->vBall[0] && pPlayer->vTeePos[2] == pPlayer->vBall[2]
        && LLMath_SquareDistanceBetween3(dropPosition, pPlayer->vTeePos) < 2500.0f) {
        return 0;
    }
    return 1;
}

// Whether a ball could be dropped at a location (*pbIsAcceptable): in bounds and outside the
// free-drop areas and, unless networksOnly, on ground (the highest at or below the location)
// with MATERIAL_BALL_CAN_LIE, not SURFCLASS_OTHER_e, no steeper than 30 degrees and clear of
// objects and hazards (Ter_CheckObjectAndHazardObstruction). *pbIsPreferred: acceptable and,
// unless networksOnly, a preferred surface class (Ter_LieIsPreferred). *pDropMaterial, when
// given, gets that ground's surface (NULL with networksOnly). Returns the ground's height (0 with
// networksOnly).
f32 Ter_CheckForDropLocation(TGD_TerrainInfo* pTerrainData, f32* location, u8 networksOnly,
                             u8* pbIsAcceptable, u8* pbIsPreferred, TGD_MaterialInfo** pDropMaterial) {
    f32 testPosition[4];
    f32 normal[4];
    TGD_MaterialInfo* pMaterialInfo;
    f32 fTerrainHeight;
    u8 acceptable;

    testPosition[0] = location[0];
    testPosition[1] = location[1];
    testPosition[2] = location[2];
    testPosition[3] = 1.0f;
    if (!networksOnly) {
        fTerrainHeight = Ter_GetSupportingGroundData(pTerrainData, testPosition, &pMaterialInfo, normal);
        if (pDropMaterial != NULL) {
            *pDropMaterial = pMaterialInfo;
        }
        acceptable = fTerrainHeight != TER_NO_GROUND && pMaterialInfo != NULL
                     && (pMaterialInfo->uFlags & MATERIAL_BALL_CAN_LIE)
                     && pMaterialInfo->nClass != SURFCLASS_OTHER_e
                     && fabsf(normal[1]) > 0.86603f && Ter_PointInOOBNetwork(testPosition)
                     && !Ter_PointInFreeDropNetwork(testPosition)
                     && (networksOnly
                         || !Ter_CheckObjectAndHazardObstruction(testPosition, 1.5f, 0, 1, 2.0f, 1, 0.577f));
    } else {
        fTerrainHeight = 0.0f;
        if (pDropMaterial != NULL) {
            *pDropMaterial = NULL;
        }
        acceptable = Ter_PointInOOBNetwork(testPosition) && !Ter_PointInFreeDropNetwork(testPosition);
    }
    testPosition[1] = fTerrainHeight;
    if (acceptable) {
        *pbIsAcceptable = 1;
        if (networksOnly || Ter_LieIsPreferred(pMaterialInfo->nClass)) {
            *pbIsPreferred = 1;
        } else {
            *pbIsPreferred = 0;
        }
    } else {
        *pbIsAcceptable = 0;
        *pbIsPreferred = 0;
    }
    return fTerrainHeight;
}

// Whether a surface class (TGD_MaterialInfo.nClass) is a preferred lie for a drop: the tee box,
// the fairway, the green or the fringe, the classes Physics_SetLie makes the fairway, green and
// fringe lies.
static u8 Ter_LieIsPreferred(u32 lieID) {
    if (lieID == SURFCLASS_TEE_e || lieID == SURFCLASS_FAIRWAY_e || lieID == SURFCLASS_GREEN_e
        || lieID == SURFCLASS_FRINGE_e) {
        return 1;
    }
    return 0;
}

// Whether a surface type (a row of gSurfaceTypes) is the green, the cup or the green at the cup
// (SURFCLASS_GREEN_e, SURFCLASS_CUP_e, SURFCLASS_CUP_GREEN_e). Physics_Simulate checks a ball on
// one only against the outlines (Ter_CheckForDropLocation's networksOnly).
u8 Ter_IsValidDropSurface(s32 surfaceID) {
    u32 nClass;

    if (surfaceID < 0 || surfaceID >= NUM_SURFACE_TYPES) return 0;
    nClass = gSurfaceTypes[surfaceID].nClass;
    if (nClass == SURFCLASS_GREEN_e || nClass == SURFCLASS_CUP_e || nClass == SURFCLASS_CUP_GREEN_e) return 1;
    return 0;
}

// The height of the lowest vertex of any triangle (TER_TRI_KICK_MASK set; the objects' too) whose
// strip's surface is not SURFCLASS_NOT_PLAYABLE_e; the course keeps it as TGD_TerrainInfo.fFloor.
static f32 Ter_CalcLowestPlayableWorldHeight(TGD_TerrainInfo* pTerrainData) {
    f32 fLowestHeight = 1000000.0f;
    u32 i;
    int k;
    TGD_PolygonReference* pPolyRef = pTerrainData->pPolyRefs;

    for (i = 0; i < pTerrainData->nPolyRefs; i++) {
        if (pPolyRef->nSurface < NUM_SURFACE_TYPES
            && gSurfaceTypes[pPolyRef->nSurface].nClass == SURFCLASS_NOT_PLAYABLE_e) {
            pPolyRef++;
        } else {
            f32* pVertexData = pTerrainData->pVerts[TER_FIRST_VERTEX(pPolyRef)];
            u8* pDrawKickData = pTerrainData->pTriFlags + TER_FIRST_VERTEX(pPolyRef);

            // a vertex is part of the triangles ending at it and at the next two vertices
            for (k = 0; k < pPolyRef->nTris + 2; k++) {
                if ((k <= pPolyRef->nTris + 1 && (pDrawKickData[0] & TER_TRI_KICK_MASK))
                    || (k <= pPolyRef->nTris && (pDrawKickData[1] & TER_TRI_KICK_MASK))
                    || (k <= pPolyRef->nTris - 1 && (pDrawKickData[2] & TER_TRI_KICK_MASK))) {
                    if (pVertexData[1] < fLowestHeight) {
                        fLowestHeight = pVertexData[1];
                    }
                }
                pVertexData += 3;
                pDrawKickData++;
            }
            pPolyRef++;
        }
    }
    return fLowestHeight;
}

// The highest ground triangle over or under a point (x, z), whatever the point's height: its
// height there, the grid cell, the strip, the triangle's first vertex and that vertex's number in
// the strip; TER_NO_GROUND when there is none. Object strips are left out, and so is a strip with
// uFlags set that does not name the current pin position (POLYREF_PIN_POSITIONS), or that has
// POLYREF_NOT_IN_SPLIT_SCREEN while the screen is split.
static f32 Ter_GetHighestGroundTriangle(TGD_TerrainInfo* pTerrainData, f32* location, TGD_Cell** pCellResult,
                                        TGD_PolygonReference** pPolyRefResult, f32 (**pTriangleVertices)[3],
                                        s32* iFirstVertexIndex) {
    u32 iCurrentPlacementMask = 1 << Game_CurrentPinSet();
    f32 fHighestPoint = TER_NO_GROUND;
    int gx = (int)Ter_Floor((location[0] - pTerrainData->fGridOrigin[0]) / pTerrainData->fGridCellSize[0]);
    int gy = (int)Ter_Floor((location[2] - pTerrainData->fGridOrigin[1]) / pTerrainData->fGridCellSize[1]);
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
                    // a triangle whose highest corner is no higher than the best so far is passed over
                    if ((pDrawKickData[2] & TER_TRI_KICK_MASK)
                        && fHighestPoint < pVertexData[TER_TRI_HIGHEST_CORNER(pDrawKickData[2])][1]
                        && Ter_PointInTriangleXZpY(pVertexData[0], pVertexData[1], pVertexData[2],
                                                   location[0], location[2])) {
                        Ter_GetBarycentricCoords(pVertexData, location, &fA, &fB, &fC);
                        fHeight = fA * pVertexData[0][1] + fB * pVertexData[1][1] + fC * pVertexData[2][1];
                        if (fHeight > fHighestPoint) {
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

// The lowest ground triangle over or under a point (x, z), whatever the point's height, with the
// same outputs and strip rules as Ter_GetHighestGroundTriangle; TER_NO_GROUND when there is none.
static f32 Ter_GetLowestGroundTriangle(TGD_TerrainInfo* pTerrainData, f32* location, TGD_Cell** pCellResult,
                                       TGD_PolygonReference** pPolyRefResult, f32 (**pTriangleVertices)[3],
                                       s32* iFirstVertexIndex) {
    u32 iCurrentPlacementMask = 1 << Game_CurrentPinSet();
    f32 fLowestPoint = 50000.0f;
    int gx = (int)Ter_Floor((location[0] - pTerrainData->fGridOrigin[0]) / pTerrainData->fGridCellSize[0]);
    int gy = (int)Ter_Floor((location[2] - pTerrainData->fGridOrigin[1]) / pTerrainData->fGridCellSize[1]);
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
                    // a triangle whose lowest corner is no lower than the best so far is passed over
                    if ((pDrawKickData[2] & TER_TRI_KICK_MASK)
                        && fLowestPoint > pVertexData[TER_TRI_LOWEST_CORNER(pDrawKickData[2])][1]
                        && Ter_PointInTriangleXZpY(pVertexData[0], pVertexData[1], pVertexData[2],
                                                   location[0], location[2])) {
                        Ter_GetBarycentricCoords(pVertexData, location, &fA, &fB, &fC);
                        fHeight = fA * pVertexData[0][1] + fB * pVertexData[1][1] + fC * pVertexData[2][1];
                        if (fHeight < fLowestPoint) {
                            *pCellResult = pCell;
                            fLowestPoint = fHeight;
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
        if (fLowestPoint == 50000.0f) return TER_NO_GROUND;
        return fLowestPoint;
    }
    return TER_NO_GROUND;
}

// The supporting triangle of the world under a point: the highest triangle at or below its
// height, object strips included. Returns its height there, with the grid cell, strip and the
// triangle's first vertex; TER_NO_GROUND when there is none, or when the cell's lowest ground
// (TGD_Cell.nMinHeight) is above the point. Strips with uFlags set follow
// Ter_GetHighestGroundTriangle's rules.
static f32 Ter_GetSupportingWorldTriangle(TGD_TerrainInfo* pTerrainData, f32* location,
                                          TGD_Cell** pCellResult, TGD_PolygonReference** pPolyRefResult,
                                          f32 (**pTriangleVertices)[3]) {
    u32 iCurrentPlacementMask = 1 << Game_CurrentPinSet();
    f32 fHighestPoint = TER_NO_GROUND;
    int gx = (int)Ter_Floor((location[0] - pTerrainData->fGridOrigin[0]) / pTerrainData->fGridCellSize[0]);
    int gy = (int)Ter_Floor((location[2] - pTerrainData->fGridOrigin[1]) / pTerrainData->fGridCellSize[1]);
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
        if ((f32)pCell->nMinHeight > location[1]) return TER_NO_GROUND;
        pPolyRef = &pTerrainData->pPolyRefs[pCell->uRefs >> CELL_REFS_FIRST_SHIFT];
        for (j = (pCell->uRefs & CELL_REFS_COUNT_MASK) - 1; j >= 0; j--) {
            if (pPolyRef->uFlags != 0
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

// The covering ground triangle over a point: the lowest ground triangle at or above its height.
// Returns its height there, with the grid cell, strip, the triangle's first vertex and that
// vertex's number in the strip; TER_NO_GROUND when there is none, or when the cell's highest
// ground (TGD_Cell.nMaxHeight) is below the point. The strip rules are
// Ter_GetHighestGroundTriangle's.
static f32 Ter_GetCoveringGroundTriangle(TGD_TerrainInfo* pTerrainData, f32* location, TGD_Cell** pCellResult,
                                         TGD_PolygonReference** pPolyRefResult, f32 (**pTriangleVertices)[3],
                                         s32* iFirstVertexIndex) {
    u32 iCurrentPlacementMask = 1 << Game_CurrentPinSet();
    f32 fLowestPoint = 65536.0f;
    int gx = (int)Ter_Floor((location[0] - pTerrainData->fGridOrigin[0]) / pTerrainData->fGridCellSize[0]);
    int gy = (int)Ter_Floor((location[2] - pTerrainData->fGridOrigin[1]) / pTerrainData->fGridCellSize[1]);
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
        if ((f32)pCell->nMaxHeight < location[1]) return TER_NO_GROUND;
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
                    // the corner test passes a triangle over only when its highest corner is at or
                    // below the point and its lowest no lower than the best so far
                    if ((pDrawKickData[2] & TER_TRI_KICK_MASK)
                        && (fLowestPoint > pVertexData[TER_TRI_LOWEST_CORNER(pDrawKickData[2])][1]
                            || location[1] < pVertexData[TER_TRI_HIGHEST_CORNER(pDrawKickData[2])][1])
                        && Ter_PointInTriangleXZpY(pVertexData[0], pVertexData[1], pVertexData[2],
                                                   location[0], location[2])) {
                        Ter_GetBarycentricCoords(pVertexData, location, &fA, &fB, &fC);
                        fHeight = fA * pVertexData[0][1] + fB * pVertexData[1][1] + fC * pVertexData[2][1];
                        if (fHeight < fLowestPoint && fHeight >= location[1]) {
                            *pCellResult = pCell;
                            fLowestPoint = fHeight;
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
        if (fLowestPoint == 65536.0f) return TER_NO_GROUND;
        return fLowestPoint;
    }
    return TER_NO_GROUND;
}

// The ground triangles just below (supporting) and just above (covering) a point, in one walk of
// its grid cell: their heights (TER_NO_GROUND for none), strips and first vertices. A triangle at
// the point's own height is both. The strip rules are Ter_GetHighestGroundTriangle's.
static void Ter_GetSupportingAndCoveringGroundTriangles(TGD_TerrainInfo* pTerrainData, f32* location,
                                                        TGD_PolygonReference** pSupportingPolyRef,
                                                        f32* fSupportingGroundHeight,
                                                        f32 (**pSupportingVertices)[3],
                                                        TGD_PolygonReference** pCoveringPolyRef,
                                                        f32* fCoveringGroundHeight,
                                                        f32 (**pCoveringVertices)[3]) {
    u32 iCurrentPlacementMask = 1 << Game_CurrentPinSet();
    f32 fLowestPoint = 65536.0f;
    f32 fHighestPoint = -65536.0f;
    int gx = (int)Ter_Floor((location[0] - pTerrainData->fGridOrigin[0]) / pTerrainData->fGridCellSize[0]);
    int gy = (int)Ter_Floor((location[2] - pTerrainData->fGridOrigin[1]) / pTerrainData->fGridCellSize[1]);
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
                    // a triangle with its lowest corner no lower than the best covering one so far
                    // and its highest no higher than the best supporting one is passed over
                    if ((pDrawKickData[2] & TER_TRI_KICK_MASK)
                        && (fLowestPoint > pVertexData[TER_TRI_LOWEST_CORNER(pDrawKickData[2])][1]
                            || fHighestPoint < pVertexData[TER_TRI_HIGHEST_CORNER(pDrawKickData[2])][1])
                        && Ter_PointInTriangleXZpY(pVertexData[0], pVertexData[1], pVertexData[2],
                                                   location[0], location[2])) {
                        Ter_GetBarycentricCoords(pVertexData, location, &fA, &fB, &fC);
                        fHeight = fA * pVertexData[0][1] + fB * pVertexData[1][1] + fC * pVertexData[2][1];
                        if (fHeight < fLowestPoint && fHeight >= location[1]) {
                            *pCoveringPolyRef = pPolyRef;
                            fLowestPoint = fHeight;
                            *pCoveringVertices = pVertexData;
                        }
                        if (fHeight > fHighestPoint && fHeight <= location[1]) {
                            *pSupportingPolyRef = pPolyRef;
                            fHighestPoint = fHeight;
                            *pSupportingVertices = pVertexData;
                        }
                    }
                    pVertexData++;
                    pDrawKickData++;
                }
                pPolyRef++;
            }
        }
        *fCoveringGroundHeight = fLowestPoint == 65536.0f ? TER_NO_GROUND : fLowestPoint;
        *fSupportingGroundHeight = fHighestPoint == -65536.0f ? TER_NO_GROUND : fHighestPoint;
        return;
    }
    *fSupportingGroundHeight = TER_NO_GROUND;
    *fCoveringGroundHeight = TER_NO_GROUND;
}

// The height of the highest ground triangle over or under a point (x, z), whatever the point's own
// height (Ter_GetHighestGroundTriangle); TER_NO_GROUND when there is none.
f32 Ter_GetHighestGroundHeight(TGD_TerrainInfo* pTerrainData, f32* location) {
    TGD_Cell* pCell;
    TGD_PolygonReference* pPolyRef;
    f32 (*pVertexData)[3];
    s32 iFirstVertexIndex;

    return Ter_GetHighestGroundTriangle(pTerrainData, location, &pCell, &pPolyRef, &pVertexData,
                                        &iFirstVertexIndex);
}

// The height of the lowest ground triangle over or under a point (x, z), whatever the point's own
// height (Ter_GetLowestGroundTriangle); TER_NO_GROUND when there is none.
f32 Ter_GetLowestGroundHeight(TGD_TerrainInfo* pTerrainData, f32* location) {
    TGD_Cell* pCell;
    TGD_PolygonReference* pPolyRef;
    f32 (*pVertexData)[3];
    s32 iFirstVertexIndex;

    return Ter_GetLowestGroundTriangle(pTerrainData, location, &pCell, &pPolyRef, &pVertexData,
                                       &iFirstVertexIndex);
}

// The height of the supporting ground under a point: the highest ground triangle at or below it
// (Ter_GetSupportingGroundTriangle); TER_NO_GROUND when there is none.
f32 Ter_GetSupportingGroundHeight(TGD_TerrainInfo* pTerrainData, f32* location) {
    TGD_Cell* pCell;
    TGD_PolygonReference* pPolyRef;
    f32 (*pVertexData)[3];
    s32 iFirstVertexIndex;

    return Ter_GetSupportingGroundTriangle(pTerrainData, location, &pCell, &pPolyRef, &pVertexData,
                                           &iFirstVertexIndex);
}

// The height of the ground covering a point (Ter_GetCoveringGroundTriangle) and, when there is one,
// that triangle's normal turned to face up; TER_NO_GROUND (normal untouched) when there is none.
f32 Ter_GetCoveringGroundHeightAndNormal(TGD_TerrainInfo* pTerrainData, f32* location, f32* normal) {
    f32 v0[4];
    f32 v1[4];
    f32 v2[4];
    f32 edge1[4];
    f32 edge2[4];
    TGD_Cell* pCell;
    TGD_PolygonReference* pPolyRef;
    f32 (*pVertexData)[3];
    s32 iFirstVertexIndex;
    f32 fHeight;

    fHeight = Ter_GetCoveringGroundTriangle(pTerrainData, location, &pCell, &pPolyRef, &pVertexData,
                                            &iFirstVertexIndex);
    if (fHeight != TER_NO_GROUND) {
        Vec3Copy(pVertexData[0], v0);
        Vec3Copy(pVertexData[1], v1);
        Vec3Copy(pVertexData[2], v2);
        LLMath_Subtract3(v0, v1, edge1);
        LLMath_Subtract3(v1, v2, edge2);
        vec4flt_CrossProduct(edge1, edge2, normal);
        LLMath_Normalize3(normal, normal);
        if (normal[1] < 0.0f) {
            LLMath_Negate3(normal, normal);
        }
        return fHeight;
    }
    return fHeight;
}

// The upward normal of the supporting ground under a point (Ter_GetSupportingGroundTriangle); 0
// (normal untouched) when there is none.
u8 Ter_GetSupportingGroundNormal(TGD_TerrainInfo* pTerrainData, f32* location, f32* normal) {
    f32 v0[4];
    f32 v1[4];
    f32 v2[4];
    f32 edge1[4];
    f32 edge2[4];
    TGD_Cell* pCell;
    TGD_PolygonReference* pPolyRef;
    f32 (*pVertexData)[3];
    s32 iFirstVertexIndex;

    if (Ter_GetSupportingGroundTriangle(pTerrainData, location, &pCell, &pPolyRef, &pVertexData,
                                        &iFirstVertexIndex)
        != TER_NO_GROUND) {
        Vec3Copy(pVertexData[0], v0);
        Vec3Copy(pVertexData[1], v1);
        Vec3Copy(pVertexData[2], v2);
        LLMath_Subtract3(v0, v1, edge1);
        LLMath_Subtract3(v1, v2, edge2);
        vec4flt_CrossProduct(edge1, edge2, normal);
        LLMath_Normalize3(normal, normal);
        if (normal[1] < 0.0f) {
            LLMath_Negate3(normal, normal);
        }
        return 1;
    }
    return 0;
}

// The height of whatever supports a point, object triangles included
// (Ter_GetSupportingWorldTriangle); TER_NO_GROUND when there is none.
f32 Ter_GetSupportingWorldHeight(TGD_TerrainInfo* pTerrainData, f32* location) {
    TGD_Cell* pCell;
    TGD_PolygonReference* pPolyRef;
    f32 (*pVertexData)[3];

    return Ter_GetSupportingWorldTriangle(pTerrainData, location, &pCell, &pPolyRef, &pVertexData);
}

// The surface of whatever supports a point, objects included (Ter_GetSupportingWorldTriangle);
// NULL when there is none.
TGD_MaterialInfo* Ter_GetSupportingWorldMaterial(TGD_TerrainInfo* pTerrainData, f32* location) {
    TGD_Cell* pCell;
    TGD_PolygonReference* pPolyRef;
    f32 (*pVertexData)[3];

    if (Ter_GetSupportingWorldTriangle(pTerrainData, location, &pCell, &pPolyRef, &pVertexData)
        != TER_NO_GROUND) {
        return &gSurfaceTypes[pPolyRef->nSurface];
    }
    return NULL;
}

// The supporting ground under a point (Ter_GetSupportingGroundTriangle): its height, its surface
// and the triangle's upward normal; with nothing under the point, TER_NO_GROUND and a NULL surface
// (normal untouched).
f32 Ter_GetSupportingGroundData(TGD_TerrainInfo* pTerrainData, f32* location, TGD_MaterialInfo** pSurfaceInfo,
                                f32* normal) {
    f32 v0[4];
    f32 v1[4];
    f32 v2[4];
    f32 edge1[4];
    f32 edge2[4];
    TGD_Cell* pCell;
    TGD_PolygonReference* pPolyRef;
    f32 (*pVertexData)[3];
    s32 iFirstVertexIndex;
    f32 fHeight;

    fHeight = Ter_GetSupportingGroundTriangle(pTerrainData, location, &pCell, &pPolyRef, &pVertexData,
                                              &iFirstVertexIndex);
    if (fHeight != TER_NO_GROUND) {
        Vec3Copy(pVertexData[0], v0);
        Vec3Copy(pVertexData[1], v1);
        Vec3Copy(pVertexData[2], v2);
        LLMath_Subtract3(v0, v1, edge1);
        LLMath_Subtract3(v1, v2, edge2);
        vec4flt_CrossProduct(edge1, edge2, normal);
        LLMath_Normalize3(normal, normal);
        if (normal[1] < 0.0f) {
            LLMath_Negate3(normal, normal);
        }
        *pSurfaceInfo = &gSurfaceTypes[pPolyRef->nSurface];
    } else {
        *pSurfaceInfo = NULL;
    }
    return fHeight;
}

// The heights of the ground just below and just above a point (TER_NO_GROUND for none;
// Ter_GetSupportingAndCoveringGroundTriangles).
void Ter_GetEnclosingGroundHeight(TGD_TerrainInfo* pTerrainData, f32* location, f32* fSupportingGroundHeight,
                                  f32* fCoveringGroundHeight) {
    TGD_PolygonReference* pSupportingPolyRef;
    TGD_PolygonReference* pCoveringPolyRef;
    f32 (*pSupportingVertices)[3];
    f32 (*pCoveringVertices)[3];

    Ter_GetSupportingAndCoveringGroundTriangles(pTerrainData, location, &pSupportingPolyRef,
                                                fSupportingGroundHeight, &pSupportingVertices,
                                                &pCoveringPolyRef, fCoveringGroundHeight, &pCoveringVertices);
}

// The ground just below and just above a point (Ter_GetSupportingAndCoveringGroundTriangles):
// heights, surfaces and upward normals; TER_NO_GROUND and a NULL surface (normal untouched) for
// none.
void Ter_GetEnclosingGroundData(TGD_TerrainInfo* pTerrainData, f32* location, f32* fSupportingGroundHeight,
                                TGD_MaterialInfo** pSupportingSurfaceInfo, f32* supportingNormal,
                                f32* fCoveringGroundHeight, TGD_MaterialInfo** pCoveringSurfaceInfo,
                                f32* coveringNormal) {
    f32 v0[4];
    f32 v1[4];
    f32 v2[4];
    f32 edge1[4];
    f32 edge2[4];
    f32 coveringEdge1[4];
    f32 coveringEdge2[4];
    TGD_PolygonReference* pSupportingPolyRef;
    TGD_PolygonReference* pCoveringPolyRef;
    f32 (*pSupportingVertices)[3];
    f32 (*pCoveringVertices)[3];

    Ter_GetSupportingAndCoveringGroundTriangles(pTerrainData, location, &pSupportingPolyRef,
                                                fSupportingGroundHeight, &pSupportingVertices,
                                                &pCoveringPolyRef, fCoveringGroundHeight, &pCoveringVertices);
    if (*fSupportingGroundHeight != TER_NO_GROUND) {
        Vec3Copy(pSupportingVertices[0], v0);
        Vec3Copy(pSupportingVertices[1], v1);
        Vec3Copy(pSupportingVertices[2], v2);
        LLMath_Subtract3(v0, v1, edge1);
        LLMath_Subtract3(v1, v2, edge2);
        vec4flt_CrossProduct(edge1, edge2, supportingNormal);
        LLMath_Normalize3(supportingNormal, supportingNormal);
        if (supportingNormal[1] < 0.0f) {
            LLMath_Negate3(supportingNormal, supportingNormal);
        }
        *pSupportingSurfaceInfo = &gSurfaceTypes[pSupportingPolyRef->nSurface];
    } else {
        *pSupportingSurfaceInfo = NULL;
    }
    if (*fCoveringGroundHeight != TER_NO_GROUND) {
        Vec3Copy(pCoveringVertices[0], v0);
        Vec3Copy(pCoveringVertices[1], v1);
        Vec3Copy(pCoveringVertices[2], v2);
        LLMath_Subtract3(v0, v1, coveringEdge1);
        LLMath_Subtract3(v1, v2, coveringEdge2);
        vec4flt_CrossProduct(coveringEdge1, coveringEdge2, coveringNormal);
        LLMath_Normalize3(coveringNormal, coveringNormal);
        if (coveringNormal[1] < 0.0f) {
            LLMath_Negate3(coveringNormal, coveringNormal);
        }
        *pCoveringSurfaceInfo = &gSurfaceTypes[pCoveringPolyRef->nSurface];
    } else {
        *pCoveringSurfaceInfo = NULL;
    }
}

// Whatever supports a point, objects included (Ter_GetSupportingWorldTriangle): its height, its
// surface and the triangle's upward normal; TER_NO_GROUND and a NULL surface (normal untouched)
// when there is none.
f32 Ter_GetSupportingWorldData(TGD_TerrainInfo* pTerrainData, f32* location, TGD_MaterialInfo** pSurfaceInfo,
                               f32* normal) {
    f32 v0[4];
    f32 v1[4];
    f32 v2[4];
    f32 edge1[4];
    f32 edge2[4];
    TGD_Cell* pCell;
    TGD_PolygonReference* pPolyRef;
    f32 (*pVertexData)[3];
    f32 fHeight;

    fHeight = Ter_GetSupportingWorldTriangle(pTerrainData, location, &pCell, &pPolyRef, &pVertexData);
    if (fHeight != TER_NO_GROUND) {
        Vec3Copy(pVertexData[0], v0);
        Vec3Copy(pVertexData[1], v1);
        Vec3Copy(pVertexData[2], v2);
        LLMath_Subtract3(v0, v1, edge1);
        LLMath_Subtract3(v1, v2, edge2);
        vec4flt_CrossProduct(edge1, edge2, normal);
        LLMath_Normalize3(normal, normal);
        if (normal[1] < 0.0f) {
            LLMath_Negate3(normal, normal);
        }
        *pSurfaceInfo = &gSurfaceTypes[pPolyRef->nSurface];
    } else {
        *pSurfaceInfo = NULL;
    }
    return fHeight;
}

// Every ground triangle over or under a point (x, z), whatever the point's height, until
// iMaxResults are found: their heights and surfaces, in the order the cell lists them. Returns how
// many. The strip rules are Ter_GetHighestGroundTriangle's.
u32 Ter_GetTerrainLayers(TGD_TerrainInfo* pTerrainData, f32* location, TGD_MaterialInfo** pSurfaceInfoList,
                         f32* pHeightList, u32 iMaxResults) {
    u32 iCurrentPlacementMask = 1 << Game_CurrentPinSet();
    u32 count = 0;
    int gx = (int)Ter_Floor((location[0] - pTerrainData->fGridOrigin[0]) / pTerrainData->fGridCellSize[0]);
    int gy = (int)Ter_Floor((location[2] - pTerrainData->fGridOrigin[1]) / pTerrainData->fGridCellSize[1]);
    TGD_Cell* pCell;
    TGD_PolygonReference* pPolyRef;
    f32 (*pVertexData)[3];
    u8* pDrawKickData;
    int j;
    int k;
    f32 fA;
    f32 fB;
    f32 fC;

    if (gx >= 0 && gx < pTerrainData->nGridWidth && gy >= 0 && gy < pTerrainData->nGridLength) {
        pCell = &pTerrainData->pGrid[gx + gy * pTerrainData->nGridWidth];
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
                    if ((pDrawKickData[2] & TER_TRI_KICK_MASK)
                        && Ter_PointInTriangleXZpY(pVertexData[0], pVertexData[1], pVertexData[2],
                                                   location[0], location[2])) {
                        Ter_GetBarycentricCoords(pVertexData, location, &fA, &fB, &fC);
                        pHeightList[count]
                            = fA * pVertexData[0][1] + fB * pVertexData[1][1] + fC * pVertexData[2][1];
                        pSurfaceInfoList[count] = &gSurfaceTypes[pPolyRef->nSurface];
                        count++;
                        if (count == iMaxResults) return count;
                    }
                    pVertexData++;
                    pDrawKickData++;
                }
                pPolyRef++;
            }
        }
        return count;
    }
    return 0;
}

// Marks in gTerObjectMarks the course objects of grid cell (nX, nZ) whose bounding sphere comes
// within fRange of pFrom, around it or ahead of it along pDir (Ter_LineSphereIntersection), and
// clears the other entries. Entry 0 stays marked, so the ground's strips (nObject 0) are tested by
// Ter_CheckForWorldCollisionOneGrid and Ter_CheckForSolidWorldCollisionOneGrid.
static void Ter_MarkObjectsNearLine(TGD_TerrainInfo* pTerrainData, f32* pFrom, f32* pDir, int nX, int nZ,
                                    f32 fRange) {
    int i;
    u16* pObjRef;
    TGD_Cell* pCell;
    TGD_ObjectInstanceInfo* pObj;

    gTerObjectMarks[0] = 1;
    for (i = 1; i < MAX_OBJECTS; i++) {
        gTerObjectMarks[i] = 0;
    }
    if (nX >= 0 && nX < pTerrainData->nGridWidth && nZ >= 0 && nZ < pTerrainData->nGridLength) {
        pCell = &pTerrainData->pGrid[nX + nZ * pTerrainData->nGridWidth];
        pObjRef = &pTerrainData->pObjRefs[pCell->nObjRefOffset];
        for (i = pCell->nObjRefs - 1; i >= 0; i--) {
            pObj = &pTerrainData->pObjects[*pObjRef];
            if (Ter_LineSphereIntersection(pFrom, pDir, fRange, pObj->vCentre, pObj->fRadius)) {
                gTerObjectMarks[*pObjRef] = 1;
            }
            pObjRef++;
        }
    }
}

// Whether a sphere (pCentre, fRadius) comes within fRange of pFrom and either holds pFrom or has
// its centre ahead of pFrom along pDir.
static u8 Ter_LineSphereIntersection(f32* pFrom, f32* pDir, f32 fRange, f32* pCentre, f32 fRadius) {
    f32 vToCentre[4];
    f32 vCentre[4];
    f32 fDist;

    vCentre[0] = pCentre[0];
    vCentre[1] = pCentre[1];
    vCentre[2] = pCentre[2];
    vCentre[3] = 1.0f;
    LLMath_Subtract3(vCentre, pFrom, vToCentre);
    fDist = (f32)Math_Sqrt(LLMath_LengthSquared3(vToCentre)) - fRadius;
    if (fDist > fRange) return 0;
    if (fDist < 0.0f) return 1;
    if (LLMath_DotProduct3(vToCentre, pDir) < 0.0f) return 0;
    return 1;
}

// Whether the segment from fPoint0 to fPoint1 runs into the flagstick at the hole's current pin
// from outside: a vertical cylinder of one inch radius up to 2 yards above the pin. Never for
// nobody's ball (playerID below 0), nor while the player's view has the flagstick out (bFlagOut),
// nor for a segment moving away from the stick. On a hit: pCollisionPoint, the stick's level
// outward normal, pSurfaceInfo the flagstick's surface (SURFACE_FLAGSTICK) and pSurfaceObject NULL.
u8 Ter_CheckForPinCollision(TGD_TerrainInfo* pTerrainData, int playerID, f32* fPoint0, f32* fPoint1,
                            f32* pCollisionPoint, f32* pSurfaceNormal, TGD_MaterialInfo** pSurfaceInfo,
                            TGD_ObjectInstanceInfo** pSurfaceObject) {
    f32 vDelta[4];
    f32 vFlat[4];
    f32 vFrom[4];
    f32 vTo[4];
    f32 startToPin[4];
    f32 fA;
    f32 fB;
    f32 fC;
    f32 fDisc;
    f32 fRoot;
    f32 fT1;
    f32 fT2;
    f32 fT;
    const f32 pinRadiusSqr = 0.00077160494f;    // (1 inch)^2 in square yards

    if (playerID < 0) return 0;
    if (ViewController_GetIndexedViewController(gPlayers[playerID].nView[0])->bFlagOut) return 0;
    // the segment relative to the pin
    vFrom[0] = fPoint0[0] - pTerrainData->pin[Game_CurrentPinSet()].x;
    vFrom[1] = fPoint0[1] - pTerrainData->pin[Game_CurrentPinSet()].y;
    vFrom[2] = fPoint0[2] - pTerrainData->pin[Game_CurrentPinSet()].z;
    vFrom[3] = 1.0f;
    vTo[0] = fPoint1[0] - pTerrainData->pin[Game_CurrentPinSet()].x;
    vTo[1] = fPoint1[1] - pTerrainData->pin[Game_CurrentPinSet()].y;
    vTo[2] = fPoint1[2] - pTerrainData->pin[Game_CurrentPinSet()].z;
    vTo[3] = 1.0f;
    LLMath_Subtract3(vTo, vFrom, vDelta);
    Vec3Copy(vDelta, vFlat);
    vFlat[1] = 0.0f;
    startToPin[0] = -vFrom[0];
    startToPin[1] = 0.0f;
    startToPin[2] = -vFrom[2];
    startToPin[3] = 1.0f;
    if (LLMath_DotProduct3(startToPin, vFlat) < 0.0f) return 0; // moving away from the stick
    fC = vFrom[0] * vFrom[0] + vFrom[2] * vFrom[2];
    if (fC < pinRadiusSqr) return 0;                            // already inside it
    // where the segment meets the cylinder: a t^2 + b t + c = 0
    fA = vDelta[0] * vDelta[0] + vDelta[2] * vDelta[2];
    fB = 2.0f * vDelta[0] * vFrom[0] + 2.0f * vDelta[2] * vFrom[2];
    fDisc = fB * fB - 4.0f * fA * (fC - pinRadiusSqr);
    if (fDisc <= 0.0f) return 0;
    fRoot = (f32)Math_Sqrt(fDisc);
    fT1 = (-fB + fRoot) / (2.0f * fA);
    fT2 = (-fB - fRoot) / (2.0f * fA);
    fT = fT1 <= fT2 ? fT1 : fT2;
    if (fT <= 0.0f) {
        fT = fT1 <= fT2 ? fT2 : fT1;
    }
    if (fT <= 0.0f || fT >= 1.0f) return 0;
    pCollisionPoint[0] = vDelta[0] * fT + vFrom[0];
    pCollisionPoint[1] = vDelta[1] * fT + vFrom[1];
    pCollisionPoint[2] = vDelta[2] * fT + vFrom[2];
    pCollisionPoint[3] = 1.0f;
    if (pCollisionPoint[1] >= 2.0f) return 0;                   // over the top of the stick
    pSurfaceNormal[0] = pCollisionPoint[0];
    pSurfaceNormal[1] = 0.0f;
    pSurfaceNormal[2] = pCollisionPoint[2];
    pSurfaceNormal[3] = 1.0f;
    LLMath_Normalize3(pSurfaceNormal, pSurfaceNormal);
    pCollisionPoint[0] += pTerrainData->pin[Game_CurrentPinSet()].x;
    pCollisionPoint[1] += pTerrainData->pin[Game_CurrentPinSet()].y;
    pCollisionPoint[2] += pTerrainData->pin[Game_CurrentPinSet()].z;
    *pSurfaceInfo = &gSurfaceTypes[SURFACE_FLAGSTICK];
    *pSurfaceObject = NULL;
    return 1;
}

// The first thing the segment from fPoint0 to fPoint1 hits: the flagstick (Ter_CheckForPinCollision)
// or a triangle of the ground or of a course object. Walks the grid cells the segment crosses,
// marking the objects near each stretch (Ter_MarkObjectsNearLine) and testing the cell's triangles
// (Ter_CheckForWorldCollisionOneGrid), and keeps the hit nearest fPoint0: its point, normal (turned
// to face back along the segment), surface and object (NULL for the ground and the flagstick).
// *pDrawKick gets the TER_TRI_KICK_MASK bits of each cell's nearest triangle hit, so it ends with
// the last such cell's, which need not be the hit kept (untouched when no triangle is hit).
u8 Ter_CheckForWorldCollision(TGD_TerrainInfo* pTerrainData, int playerID, f32* fPoint0, f32* fPoint1,
                              f32* pCollisionPoint, f32* pSurfaceNormal, TGD_MaterialInfo** pSurfaceInfo,
                              TGD_ObjectInstanceInfo** pSurfaceObject, u8* pDrawKick) {
    int nEndX;
    int nEndZ;
    f32 vHit[4];
    f32 vNormal[4];
    f32 fRayUnitVector[4];
    f32 vPrev[4];
    f32 vPos[4];
    int nCell[2];
    f32 vDelta[2];
    f32 vStart[2];
    f32 vEdge[2];
    TGD_MaterialInfo* pSurface;
    TGD_ObjectInstanceInfo* pObj;
    f32 fClosest = 1000000.0f;
    int nMajor;
    int nMinor;
    int nStepMajor;
    int nStepMinor;
    int nLast;
    f32 fSlope;
    f32 fRatio;
    f32 fRun;
    f32 fLen;
    f32 fDist;

    if (fPoint0[0] == fPoint1[0] && fPoint0[1] == fPoint1[1] && fPoint0[2] == fPoint1[2]) return 0;
    if (Ter_CheckForPinCollision(pTerrainData, playerID, fPoint0, fPoint1, pCollisionPoint, pSurfaceNormal,
                                 pSurfaceInfo, pSurfaceObject)) {
        fClosest = LLMath_SquareDistanceBetween3(fPoint0, pCollisionPoint);
    }
    nCell[0] = Ter_GridCell((fPoint0[0] - pTerrainData->fGridOrigin[0]) / pTerrainData->fGridCellSize[0]);
    nCell[1] = Ter_GridCell((fPoint0[2] - pTerrainData->fGridOrigin[1]) / pTerrainData->fGridCellSize[1]);
    nEndX = Ter_GridCell((fPoint1[0] - pTerrainData->fGridOrigin[0]) / pTerrainData->fGridCellSize[0]);
    nEndZ = Ter_GridCell((fPoint1[2] - pTerrainData->fGridOrigin[1]) / pTerrainData->fGridCellSize[1]);
    vDelta[0] = fPoint1[0] - fPoint0[0];
    vDelta[1] = fPoint1[2] - fPoint0[2];
    vStart[0] = fPoint0[0];
    vStart[1] = fPoint0[2];
    if (fabsf(vDelta[0]) > fabsf(vDelta[1])) {
        nMajor = 0;
        nMinor = 1;
        fSlope = vDelta[1] / vDelta[0];
    } else {
        nMajor = 1;
        nMinor = 0;
        if (vDelta[1]) {
            fSlope = vDelta[0] / vDelta[1];
        } else {
            fSlope = 0.0f;
        }
    }
    if (vDelta[nMajor] > 0.0f) {
        nStepMajor = 1;
    } else {
        nStepMajor = -1;
    }
    if (vDelta[nMinor] > 0.0f) {
        nStepMinor = 1;
    } else {
        nStepMinor = -1;
    }
    // the next cell edge the segment crosses on each axis
    vEdge[nMajor] = pTerrainData->fGridCellSize[nMajor] * nCell[nMajor] + pTerrainData->fGridOrigin[nMajor];
    vEdge[nMinor] = pTerrainData->fGridCellSize[nMinor] * nCell[nMinor] + pTerrainData->fGridOrigin[nMinor];
    if (nStepMajor > 0) {
        vEdge[nMajor] += pTerrainData->fGridCellSize[nMajor];
    }
    if (nStepMinor > 0) {
        vEdge[nMinor] += pTerrainData->fGridCellSize[nMinor];
    }
    LLMath_Subtract3(fPoint1, fPoint0, fRayUnitVector);
    LLMath_Normalize3(fRayUnitVector, fRayUnitVector);
    LLMath_CopyVec(fPoint0, vPos);
    for (;;) {
        LLMath_CopyVec(vPos, vPrev);
        if (nCell[0] == nEndX && nCell[1] == nEndZ) {
            LLMath_CopyVec(fPoint1, vPos);
        } else {
            if (vEdge[nMajor] - vStart[nMajor] != 0.0f) {
                fRatio = (vEdge[nMinor] - vStart[nMinor]) / (vEdge[nMajor] - vStart[nMajor]);
            } else {
                fRatio = 1000000.0f;
            }
            if (fabsf(fSlope) < fabsf(fRatio)) {
                fRun = vEdge[nMajor] - vStart[nMajor];
                vPos[AXIS3(nMajor)] = vEdge[nMajor];
                nLast = nMajor;
                vPos[AXIS3(nMinor)] = vStart[nMinor]
                                      + fRun * fRayUnitVector[AXIS3(nMinor)] / fRayUnitVector[AXIS3(nMajor)];
                vPos[1] = fPoint0[1] + fRayUnitVector[1] * fRun / fRayUnitVector[AXIS3(nMajor)];
            } else {
                fRun = vEdge[nMinor] - vStart[nMinor];
                vPos[AXIS3(nMinor)] = vEdge[nMinor];
                nLast = nMinor;
                vPos[AXIS3(nMajor)] = vStart[nMajor]
                                      + fRun * fRayUnitVector[AXIS3(nMajor)] / fRayUnitVector[AXIS3(nMinor)];
                vPos[1] = fPoint0[1] + fRayUnitVector[1] * fRun / fRayUnitVector[AXIS3(nMinor)];
            }
        }
        fLen = LLMath_DistanceBetween3(vPos, vPrev);
        Ter_MarkObjectsNearLine(pTerrainData, vPrev, fRayUnitVector, nCell[0], nCell[1], fLen);
        if (Ter_CheckForWorldCollisionOneGrid(pTerrainData, nCell[0], nCell[1], vPrev, vPos, fRayUnitVector,
                                              fLen, vHit, vNormal, &pSurface, &pObj, pDrawKick)) {
            fDist = LLMath_SquareDistanceBetween3(fPoint0, vHit);
            if (fDist < fClosest) {
                fClosest = fDist;
                LLMath_CopyVec(vHit, pCollisionPoint);
                LLMath_CopyVec(vNormal, pSurfaceNormal);
                *pSurfaceInfo = pSurface;
                *pSurfaceObject = pObj;
            }
        }
        if (nCell[0] == nEndX && nCell[1] == nEndZ) break;
        if (nLast == nMajor) {
            nCell[nMajor] += nStepMajor;
            vEdge[nMajor] += pTerrainData->fGridCellSize[nMajor] * nStepMajor;
        } else {
            nCell[nMinor] += nStepMinor;
            vEdge[nMinor] += pTerrainData->fGridCellSize[nMinor] * nStepMinor;
        }
    }
    if (fClosest != 1000000.0f && LLMath_DotProduct3(fRayUnitVector, pSurfaceNormal) > 0.0f) {
        LLMath_Negate(pSurfaceNormal, pSurfaceNormal);
    }
    return fClosest != 1000000.0f;
}

// The nearest triangle of grid cell (nX, nZ) that the line from pFrom along the unit vector pDir
// meets within fMax of pFrom (Ter_LineTriangleIntersection): its point, its normal (not turned
// either way), its surface, its course object (NULL for the ground) and its TER_TRI_KICK_MASK bits
// (*pDrawKick); the outputs are untouched when there is none. The cell is skipped when the segment
// from pFrom to pTo is wholly above or below its ground (TGD_Cell.nMaxHeight, nMinHeight). Strips
// with uFlags set follow Ter_GetHighestGroundTriangle's rules, and an object's strips count only
// when gTerObjectMarks marks the object (Ter_MarkObjectsNearLine; the ground's always count).
static u8 Ter_CheckForWorldCollisionOneGrid(TGD_TerrainInfo* pTerrainData, int nX, int nZ, f32* pFrom,
                                            f32* pTo, f32* pDir, f32 fMax, f32* pCollisionPoint,
                                            f32* pSurfaceNormal, TGD_MaterialInfo** pSurfaceInfo,
                                            TGD_ObjectInstanceInfo** pSurfaceObject, u8* pDrawKick) {
    f32 vHit[4];
    f32 vNormal[4];
    f32 fT;
    u8 bHit = 0;
    u32 iCurrentPlacementMask = 1 << Game_CurrentPinSet();
    TGD_Cell* pCell;
    TGD_PolygonReference* pPolyRef;
    f32 (*pVertexData)[3];
    u8* pDrawKickData;
    int j;
    int k;
    f32 fLow;
    f32 fHigh;

    if (nX >= 0 && nX < pTerrainData->nGridWidth && nZ >= 0 && nZ < pTerrainData->nGridLength) {
        fLow = pFrom[1];
        fHigh = pFrom[1];
        pCell = &pTerrainData->pGrid[nX + nZ * pTerrainData->nGridWidth];
        if (pTo[1] > pFrom[1]) {
            fHigh = pTo[1];
        }
        if (pTo[1] < fLow) {
            fLow = pTo[1];
        }
        if (fLow > (f32)pCell->nMaxHeight || fHigh < (f32)pCell->nMinHeight) return 0;
        pPolyRef = &pTerrainData->pPolyRefs[pCell->uRefs >> CELL_REFS_FIRST_SHIFT];
        for (j = (pCell->uRefs & CELL_REFS_COUNT_MASK) - 1; j >= 0; j--) {
            if (pPolyRef->uFlags != 0
                && (!(pPolyRef->uFlags & iCurrentPlacementMask)
                    || ((pPolyRef->uFlags & POLYREF_NOT_IN_SPLIT_SCREEN) && gSession.nSplitScreen))) {
                pPolyRef++;
            } else if (gTerObjectMarks[pPolyRef->nObject] == 0) {
                pPolyRef++;
            } else {
                pVertexData = &pTerrainData->pVerts[TER_FIRST_VERTEX(pPolyRef)];
                pDrawKickData = pTerrainData->pTriFlags + TER_FIRST_VERTEX(pPolyRef);
                for (k = pPolyRef->nTris - 1; k >= 0; k--) {
                    if ((pDrawKickData[2] & TER_TRI_KICK_MASK)
                        && Ter_LineTriangleIntersection(pFrom, pDir, fMax, pVertexData, &fT, vHit, vNormal)) {
                        fMax = fT;
                        LLMath_CopyVec(vHit, pCollisionPoint);
                        LLMath_CopyVec(vNormal, pSurfaceNormal);
                        if (pDrawKick != NULL) {
                            *pDrawKick = pDrawKickData[2] & TER_TRI_KICK_MASK;
                        }
                        *pSurfaceInfo = &gSurfaceTypes[pPolyRef->nSurface];
                        if (pPolyRef->nObject != 0) {
                            *pSurfaceObject = &pTerrainData->pObjects[pPolyRef->nObject];
                        } else {
                            *pSurfaceObject = NULL;
                        }
                        bHit = 1;
                    }
                    pVertexData++;
                    pDrawKickData++;
                }
                pPolyRef++;
            }
        }
        return bHit;
    }
    return 0;
}

// Ter_CheckForWorldCollision without the kick bits, and a triangle whose surface has a negative
// fRestitution does not count (Ter_CheckForSolidWorldCollisionOneGrid): the flagstick or the
// nearest triangle of the ground or of a course object that the segment from fPoint0 to fPoint1
// hits, with the same outputs.
u8 Ter_CheckForSolidWorldCollision(TGD_TerrainInfo* pTerrainData, int playerID, f32* fPoint0, f32* fPoint1,
                                   f32* pCollisionPoint, f32* pSurfaceNormal, TGD_MaterialInfo** pSurfaceInfo,
                                   TGD_ObjectInstanceInfo** pSurfaceObject) {
    int nEndX;
    int nEndZ;
    f32 vHit[4];
    f32 vNormal[4];
    f32 fRayUnitVector[4];
    f32 vPrev[4];
    f32 vPos[4];
    int nCell[2];
    f32 vDelta[2];
    f32 vStart[2];
    f32 vEdge[2];
    TGD_MaterialInfo* pSurface;
    TGD_ObjectInstanceInfo* pObj;
    f32 fClosest = 1000000.0f;
    int nMajor;
    int nMinor;
    int nStepMajor;
    int nStepMinor;
    int nLast;
    f32 fSlope;
    f32 fRatio;
    f32 fRun;
    f32 fLen;
    f32 fDist;

    if (fPoint0[0] == fPoint1[0] && fPoint0[1] == fPoint1[1] && fPoint0[2] == fPoint1[2]) return 0;
    if (Ter_CheckForPinCollision(pTerrainData, playerID, fPoint0, fPoint1, pCollisionPoint, pSurfaceNormal,
                                 pSurfaceInfo, pSurfaceObject)) {
        fClosest = LLMath_SquareDistanceBetween3(fPoint0, pCollisionPoint);
    }
    nCell[0] = Ter_GridCell((fPoint0[0] - pTerrainData->fGridOrigin[0]) / pTerrainData->fGridCellSize[0]);
    nCell[1] = Ter_GridCell((fPoint0[2] - pTerrainData->fGridOrigin[1]) / pTerrainData->fGridCellSize[1]);
    nEndX = Ter_GridCell((fPoint1[0] - pTerrainData->fGridOrigin[0]) / pTerrainData->fGridCellSize[0]);
    nEndZ = Ter_GridCell((fPoint1[2] - pTerrainData->fGridOrigin[1]) / pTerrainData->fGridCellSize[1]);
    vDelta[0] = fPoint1[0] - fPoint0[0];
    vDelta[1] = fPoint1[2] - fPoint0[2];
    vStart[0] = fPoint0[0];
    vStart[1] = fPoint0[2];
    if (fabsf(vDelta[0]) > fabsf(vDelta[1])) {
        nMajor = 0;
        nMinor = 1;
        fSlope = vDelta[1] / vDelta[0];
    } else {
        nMajor = 1;
        nMinor = 0;
        if (vDelta[1]) {
            fSlope = vDelta[0] / vDelta[1];
        } else {
            fSlope = 0.0f;
        }
    }
    if (vDelta[nMajor] > 0.0f) {
        nStepMajor = 1;
    } else {
        nStepMajor = -1;
    }
    if (vDelta[nMinor] > 0.0f) {
        nStepMinor = 1;
    } else {
        nStepMinor = -1;
    }
    // the next cell edge the line crosses on each axis
    vEdge[nMajor] = pTerrainData->fGridCellSize[nMajor] * nCell[nMajor] + pTerrainData->fGridOrigin[nMajor];
    vEdge[nMinor] = pTerrainData->fGridCellSize[nMinor] * nCell[nMinor] + pTerrainData->fGridOrigin[nMinor];
    if (nStepMajor > 0) {
        vEdge[nMajor] += pTerrainData->fGridCellSize[nMajor];
    }
    if (nStepMinor > 0) {
        vEdge[nMinor] += pTerrainData->fGridCellSize[nMinor];
    }
    LLMath_Subtract3(fPoint1, fPoint0, fRayUnitVector);
    LLMath_Normalize3(fRayUnitVector, fRayUnitVector);
    LLMath_CopyVec(fPoint0, vPos);
    for (;;) {
        LLMath_CopyVec(vPos, vPrev);
        if (nCell[0] == nEndX && nCell[1] == nEndZ) {
            LLMath_CopyVec(fPoint1, vPos);
        } else {
            if (vEdge[nMajor] - vStart[nMajor] != 0.0f) {
                fRatio = (vEdge[nMinor] - vStart[nMinor]) / (vEdge[nMajor] - vStart[nMajor]);
            } else {
                fRatio = 1000000.0f;
            }
            if (fabsf(fSlope) < fabsf(fRatio)) {
                fRun = vEdge[nMajor] - vStart[nMajor];
                vPos[AXIS3(nMajor)] = vEdge[nMajor];
                nLast = nMajor;
                vPos[AXIS3(nMinor)] = vStart[nMinor]
                                      + fRun * fRayUnitVector[AXIS3(nMinor)] / fRayUnitVector[AXIS3(nMajor)];
                vPos[1] = fPoint0[1] + fRayUnitVector[1] * fRun / fRayUnitVector[AXIS3(nMajor)];
            } else {
                fRun = vEdge[nMinor] - vStart[nMinor];
                vPos[AXIS3(nMinor)] = vEdge[nMinor];
                nLast = nMinor;
                vPos[AXIS3(nMajor)] = vStart[nMajor]
                                      + fRun * fRayUnitVector[AXIS3(nMajor)] / fRayUnitVector[AXIS3(nMinor)];
                vPos[1] = fPoint0[1] + fRayUnitVector[1] * fRun / fRayUnitVector[AXIS3(nMinor)];
            }
        }
        fLen = LLMath_DistanceBetween3(vPos, vPrev);
        Ter_MarkObjectsNearLine(pTerrainData, vPrev, fRayUnitVector, nCell[0], nCell[1], fLen);
        if (Ter_CheckForSolidWorldCollisionOneGrid(pTerrainData, nCell[0], nCell[1], vPrev, vPos,
                                                   fRayUnitVector, fLen, vHit, vNormal, &pSurface, &pObj)) {
            fDist = LLMath_SquareDistanceBetween3(fPoint0, vHit);
            if (fDist < fClosest) {
                fClosest = fDist;
                LLMath_CopyVec(vHit, pCollisionPoint);
                LLMath_CopyVec(vNormal, pSurfaceNormal);
                *pSurfaceInfo = pSurface;
                *pSurfaceObject = pObj;
            }
        }
        if (nCell[0] == nEndX && nCell[1] == nEndZ) break;
        if (nLast == nMajor) {
            nCell[nMajor] += nStepMajor;
            vEdge[nMajor] += pTerrainData->fGridCellSize[nMajor] * nStepMajor;
        } else {
            nCell[nMinor] += nStepMinor;
            vEdge[nMinor] += pTerrainData->fGridCellSize[nMinor] * nStepMinor;
        }
    }
    if (fClosest != 1000000.0f && LLMath_DotProduct3(fRayUnitVector, pSurfaceNormal) > 0.0f) {
        LLMath_Negate(pSurfaceNormal, pSurfaceNormal);
    }
    return fClosest != 1000000.0f;
}

// Ter_CheckForWorldCollisionOneGrid without the kick bits, and a strip whose surface has a negative
// fRestitution does not count.
static u8 Ter_CheckForSolidWorldCollisionOneGrid(TGD_TerrainInfo* pTerrainData, int nX, int nZ, f32* pFrom,
                                                 f32* pTo, f32* pDir, f32 fMax, f32* pCollisionPoint,
                                                 f32* pSurfaceNormal, TGD_MaterialInfo** pSurfaceInfo,
                                                 TGD_ObjectInstanceInfo** pSurfaceObject) {
    f32 vHit[4];
    f32 vNormal[4];
    f32 fT;
    u8 bHit = 0;
    u32 iCurrentPlacementMask = 1 << Game_CurrentPinSet();
    TGD_Cell* pCell;
    TGD_PolygonReference* pPolyRef;
    f32 (*pVertexData)[3];
    u8* pDrawKickData;
    int j;
    int k;
    f32 fLow;
    f32 fHigh;

    if (nX >= 0 && nX < pTerrainData->nGridWidth && nZ >= 0 && nZ < pTerrainData->nGridLength) {
        fLow = pFrom[1];
        fHigh = pFrom[1];
        pCell = &pTerrainData->pGrid[nX + nZ * pTerrainData->nGridWidth];
        if (pTo[1] > pFrom[1]) {
            fHigh = pTo[1];
        }
        if (pTo[1] < fLow) {
            fLow = pTo[1];
        }
        if (fLow > (f32)pCell->nMaxHeight || fHigh < (f32)pCell->nMinHeight) return 0;
        pPolyRef = &pTerrainData->pPolyRefs[pCell->uRefs >> CELL_REFS_FIRST_SHIFT];
        for (j = (pCell->uRefs & CELL_REFS_COUNT_MASK) - 1; j >= 0; j--) {
            if (pPolyRef->uFlags != 0
                && (!(pPolyRef->uFlags & iCurrentPlacementMask)
                    || ((pPolyRef->uFlags & POLYREF_NOT_IN_SPLIT_SCREEN) && gSession.nSplitScreen))) {
                pPolyRef++;
            } else if (gTerObjectMarks[pPolyRef->nObject] == 0) {
                pPolyRef++;
            } else if (gSurfaceTypes[pPolyRef->nSurface].fRestitution < 0.0f) {
                pPolyRef++;
            } else {
                pVertexData = &pTerrainData->pVerts[TER_FIRST_VERTEX(pPolyRef)];
                pDrawKickData = pTerrainData->pTriFlags + TER_FIRST_VERTEX(pPolyRef);
                for (k = pPolyRef->nTris - 1; k >= 0; k--) {
                    if ((pDrawKickData[2] & TER_TRI_KICK_MASK)
                        && Ter_LineTriangleIntersection(pFrom, pDir, fMax, pVertexData, &fT, vHit, vNormal)) {
                        fMax = fT;
                        LLMath_CopyVec(vHit, pCollisionPoint);
                        LLMath_CopyVec(vNormal, pSurfaceNormal);
                        *pSurfaceInfo = &gSurfaceTypes[pPolyRef->nSurface];
                        if (pPolyRef->nObject != 0) {
                            *pSurfaceObject = &pTerrainData->pObjects[pPolyRef->nObject];
                        } else {
                            *pSurfaceObject = NULL;
                        }
                        bHit = 1;
                    }
                    pVertexData++;
                    pDrawKickData++;
                }
                pPolyRef++;
            }
        }
        return bHit;
    }
    return 0;
}

// The nearest ground triangle the segment from fPoint0 to fPoint1 hits, walking the grid cells as
// Ter_CheckForWorldCollision does (Ter_CheckForGroundCollisionOneGrid); no flagstick and no course
// objects. On a hit: its point, normal (turned to face back along the segment), surface and a NULL
// object; untouched otherwise.
u8 Ter_CheckForGroundCollision(TGD_TerrainInfo* pTerrainData, f32* fPoint0, f32* fPoint1,
                               f32* pCollisionPoint, f32* pSurfaceNormal, TGD_MaterialInfo** pSurfaceInfo,
                               TGD_ObjectInstanceInfo** pSurfaceObject) {
    int nEndX;
    int nEndZ;
    f32 vHit[4];
    f32 vNormal[4];
    f32 fRayUnitVector[4];
    f32 vPrev[4];
    f32 vPos[4];
    int nCell[2];
    f32 vDelta[2];
    f32 vStart[2];
    f32 vEdge[2];
    TGD_MaterialInfo* pSurface;
    TGD_ObjectInstanceInfo* pObj;
    f32 fClosest = 1000000.0f;
    int nMajor;
    int nMinor;
    int nStepMajor;
    int nStepMinor;
    int nLast;
    f32 fSlope;
    f32 fRatio;
    f32 fRun;
    f32 fLen;
    f32 fDist;

    if (fPoint0[0] == fPoint1[0] && fPoint0[1] == fPoint1[1] && fPoint0[2] == fPoint1[2]) return 0;
    nCell[0] = Ter_GridCell((fPoint0[0] - pTerrainData->fGridOrigin[0]) / pTerrainData->fGridCellSize[0]);
    nCell[1] = Ter_GridCell((fPoint0[2] - pTerrainData->fGridOrigin[1]) / pTerrainData->fGridCellSize[1]);
    nEndX = Ter_GridCell((fPoint1[0] - pTerrainData->fGridOrigin[0]) / pTerrainData->fGridCellSize[0]);
    nEndZ = Ter_GridCell((fPoint1[2] - pTerrainData->fGridOrigin[1]) / pTerrainData->fGridCellSize[1]);
    vDelta[0] = fPoint1[0] - fPoint0[0];
    vDelta[1] = fPoint1[2] - fPoint0[2];
    vStart[0] = fPoint0[0];
    vStart[1] = fPoint0[2];
    if (fabsf(vDelta[0]) > fabsf(vDelta[1])) {
        nMajor = 0;
        nMinor = 1;
        fSlope = vDelta[1] / vDelta[0];
    } else {
        nMajor = 1;
        nMinor = 0;
        if (vDelta[1]) {
            fSlope = vDelta[0] / vDelta[1];
        } else {
            fSlope = 0.0f;
        }
    }
    if (vDelta[nMajor] > 0.0f) {
        nStepMajor = 1;
    } else {
        nStepMajor = -1;
    }
    if (vDelta[nMinor] > 0.0f) {
        nStepMinor = 1;
    } else {
        nStepMinor = -1;
    }
    // the next cell edge the line crosses on each axis
    vEdge[nMajor] = pTerrainData->fGridCellSize[nMajor] * nCell[nMajor] + pTerrainData->fGridOrigin[nMajor];
    vEdge[nMinor] = pTerrainData->fGridCellSize[nMinor] * nCell[nMinor] + pTerrainData->fGridOrigin[nMinor];
    if (nStepMajor > 0) {
        vEdge[nMajor] += pTerrainData->fGridCellSize[nMajor];
    }
    if (nStepMinor > 0) {
        vEdge[nMinor] += pTerrainData->fGridCellSize[nMinor];
    }
    LLMath_Subtract3(fPoint1, fPoint0, fRayUnitVector);
    LLMath_Normalize3(fRayUnitVector, fRayUnitVector);
    LLMath_CopyVec(fPoint0, vPos);
    for (;;) {
        LLMath_CopyVec(vPos, vPrev);
        if (nCell[0] == nEndX && nCell[1] == nEndZ) {
            LLMath_CopyVec(fPoint1, vPos);
        } else {
            if (vEdge[nMajor] - vStart[nMajor] != 0.0f) {
                fRatio = (vEdge[nMinor] - vStart[nMinor]) / (vEdge[nMajor] - vStart[nMajor]);
            } else {
                fRatio = 1000000.0f;
            }
            if (fabsf(fSlope) < fabsf(fRatio)) {
                fRun = vEdge[nMajor] - vStart[nMajor];
                vPos[AXIS3(nMajor)] = vEdge[nMajor];
                nLast = nMajor;
                vPos[AXIS3(nMinor)] = vStart[nMinor]
                                      + fRun * fRayUnitVector[AXIS3(nMinor)] / fRayUnitVector[AXIS3(nMajor)];
                vPos[1] = fPoint0[1] + fRayUnitVector[1] * fRun / fRayUnitVector[AXIS3(nMajor)];
            } else {
                fRun = vEdge[nMinor] - vStart[nMinor];
                vPos[AXIS3(nMinor)] = vEdge[nMinor];
                nLast = nMinor;
                vPos[AXIS3(nMajor)] = vStart[nMajor]
                                      + fRun * fRayUnitVector[AXIS3(nMajor)] / fRayUnitVector[AXIS3(nMinor)];
                vPos[1] = fPoint0[1] + fRayUnitVector[1] * fRun / fRayUnitVector[AXIS3(nMinor)];
            }
        }
        fLen = LLMath_DistanceBetween3(vPos, vPrev);
        if (Ter_CheckForGroundCollisionOneGrid(pTerrainData, nCell[0], nCell[1], vPrev, vPos, fRayUnitVector,
                                               fLen, vHit, vNormal, &pSurface, &pObj)) {
            fDist = LLMath_SquareDistanceBetween3(fPoint0, vHit);
            if (fDist < fClosest) {
                fClosest = fDist;
                LLMath_CopyVec(vHit, pCollisionPoint);
                LLMath_CopyVec(vNormal, pSurfaceNormal);
                *pSurfaceInfo = pSurface;
                *pSurfaceObject = pObj;
            }
        }
        if (nCell[0] == nEndX && nCell[1] == nEndZ) break;
        if (nLast == nMajor) {
            nCell[nMajor] += nStepMajor;
            vEdge[nMajor] += pTerrainData->fGridCellSize[nMajor] * nStepMajor;
        } else {
            nCell[nMinor] += nStepMinor;
            vEdge[nMinor] += pTerrainData->fGridCellSize[nMinor] * nStepMinor;
        }
    }
    if (fClosest != 1000000.0f && LLMath_DotProduct3(fRayUnitVector, pSurfaceNormal) > 0.0f) {
        LLMath_Negate(pSurfaceNormal, pSurfaceNormal);
    }
    return fClosest != 1000000.0f;
}

// Ter_CheckForWorldCollisionOneGrid without the kick bits, on the ground's strips only (no course
// objects; gTerObjectMarks is not read).
static u8 Ter_CheckForGroundCollisionOneGrid(TGD_TerrainInfo* pTerrainData, int nX, int nZ, f32* pFrom,
                                             f32* pTo, f32* pDir, f32 fMax, f32* pCollisionPoint,
                                             f32* pSurfaceNormal, TGD_MaterialInfo** pSurfaceInfo,
                                             TGD_ObjectInstanceInfo** pSurfaceObject) {
    f32 vHit[4];
    f32 vNormal[4];
    f32 fT;
    u8 bHit = 0;
    u32 iCurrentPlacementMask = 1 << Game_CurrentPinSet();
    TGD_Cell* pCell;
    TGD_PolygonReference* pPolyRef;
    f32 (*pVertexData)[3];
    u8* pDrawKickData;
    int j;
    int k;
    f32 fLow;
    f32 fHigh;

    if (nX >= 0 && nX < pTerrainData->nGridWidth && nZ >= 0 && nZ < pTerrainData->nGridLength) {
        fLow = pFrom[1];
        fHigh = pFrom[1];
        pCell = &pTerrainData->pGrid[nX + nZ * pTerrainData->nGridWidth];
        if (pTo[1] > pFrom[1]) {
            fHigh = pTo[1];
        }
        if (pTo[1] < fLow) {
            fLow = pTo[1];
        }
        if (fLow > (f32)pCell->nMaxHeight || fHigh < (f32)pCell->nMinHeight) return 0;
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
                    if ((pDrawKickData[2] & TER_TRI_KICK_MASK)
                        && Ter_LineTriangleIntersection(pFrom, pDir, fMax, pVertexData, &fT, vHit, vNormal)) {
                        fMax = fT;
                        LLMath_CopyVec(vHit, pCollisionPoint);
                        LLMath_CopyVec(vNormal, pSurfaceNormal);
                        *pSurfaceInfo = &gSurfaceTypes[pPolyRef->nSurface];
                        if (pPolyRef->nObject != 0) {
                            *pSurfaceObject = &pTerrainData->pObjects[pPolyRef->nObject];
                        } else {
                            *pSurfaceObject = NULL;
                        }
                        bHit = 1;
                    }
                    pVertexData++;
                    pDrawKickData++;
                }
                pPolyRef++;
            }
        }
        return bHit;
    }
    return 0;
}

// The nearest triangle of a course object, or of ground whose surface has
// MATERIAL_COLLIDES_AS_OBJECT, that the segment from fPoint0 to fPoint1 hits, walking the grid cells
// as Ter_CheckForWorldCollision does (Ter_CheckForObjectCollisionOneGrid); no flagstick. The outputs
// are Ter_CheckForGroundCollision's, the object NULL for such ground.
u8 Ter_CheckForObjectCollision(TGD_TerrainInfo* pTerrainData, f32* fPoint0, f32* fPoint1,
                               f32* pCollisionPoint, f32* pSurfaceNormal, TGD_MaterialInfo** pSurfaceInfo,
                               TGD_ObjectInstanceInfo** pSurfaceObject) {
    int nEndX;
    int nEndZ;
    f32 vHit[4];
    f32 vNormal[4];
    f32 fRayUnitVector[4];
    f32 vPrev[4];
    f32 vPos[4];
    int nCell[2];
    f32 vDelta[2];
    f32 vStart[2];
    f32 vEdge[2];
    TGD_MaterialInfo* pSurface;
    TGD_ObjectInstanceInfo* pObj;
    f32 fClosest = 1000000.0f;
    int nMajor;
    int nMinor;
    int nStepMajor;
    int nStepMinor;
    int nLast;
    f32 fSlope;
    f32 fRatio;
    f32 fRun;
    f32 fLen;
    f32 fDist;

    if (fPoint0[0] == fPoint1[0] && fPoint0[1] == fPoint1[1] && fPoint0[2] == fPoint1[2]) return 0;
    nCell[0] = Ter_GridCell((fPoint0[0] - pTerrainData->fGridOrigin[0]) / pTerrainData->fGridCellSize[0]);
    nCell[1] = Ter_GridCell((fPoint0[2] - pTerrainData->fGridOrigin[1]) / pTerrainData->fGridCellSize[1]);
    nEndX = Ter_GridCell((fPoint1[0] - pTerrainData->fGridOrigin[0]) / pTerrainData->fGridCellSize[0]);
    nEndZ = Ter_GridCell((fPoint1[2] - pTerrainData->fGridOrigin[1]) / pTerrainData->fGridCellSize[1]);
    vDelta[0] = fPoint1[0] - fPoint0[0];
    vDelta[1] = fPoint1[2] - fPoint0[2];
    vStart[0] = fPoint0[0];
    vStart[1] = fPoint0[2];
    if (fabsf(vDelta[0]) > fabsf(vDelta[1])) {
        nMajor = 0;
        nMinor = 1;
        fSlope = vDelta[1] / vDelta[0];
    } else {
        nMajor = 1;
        nMinor = 0;
        if (vDelta[1]) {
            fSlope = vDelta[0] / vDelta[1];
        } else {
            fSlope = 0.0f;
        }
    }
    if (vDelta[nMajor] > 0.0f) {
        nStepMajor = 1;
    } else {
        nStepMajor = -1;
    }
    if (vDelta[nMinor] > 0.0f) {
        nStepMinor = 1;
    } else {
        nStepMinor = -1;
    }
    // the next cell edge the line crosses on each axis
    vEdge[nMajor] = pTerrainData->fGridCellSize[nMajor] * nCell[nMajor] + pTerrainData->fGridOrigin[nMajor];
    vEdge[nMinor] = pTerrainData->fGridCellSize[nMinor] * nCell[nMinor] + pTerrainData->fGridOrigin[nMinor];
    if (nStepMajor > 0) {
        vEdge[nMajor] += pTerrainData->fGridCellSize[nMajor];
    }
    if (nStepMinor > 0) {
        vEdge[nMinor] += pTerrainData->fGridCellSize[nMinor];
    }
    LLMath_Subtract3(fPoint1, fPoint0, fRayUnitVector);
    LLMath_Normalize3(fRayUnitVector, fRayUnitVector);
    LLMath_CopyVec(fPoint0, vPos);
    for (;;) {
        LLMath_CopyVec(vPos, vPrev);
        if (nCell[0] == nEndX && nCell[1] == nEndZ) {
            LLMath_CopyVec(fPoint1, vPos);
        } else {
            if (vEdge[nMajor] - vStart[nMajor] != 0.0f) {
                fRatio = (vEdge[nMinor] - vStart[nMinor]) / (vEdge[nMajor] - vStart[nMajor]);
            } else {
                fRatio = 1000000.0f;
            }
            if (fabsf(fSlope) < fabsf(fRatio)) {
                fRun = vEdge[nMajor] - vStart[nMajor];
                vPos[AXIS3(nMajor)] = vEdge[nMajor];
                nLast = nMajor;
                vPos[AXIS3(nMinor)] = vStart[nMinor]
                                      + fRun * fRayUnitVector[AXIS3(nMinor)] / fRayUnitVector[AXIS3(nMajor)];
                vPos[1] = fPoint0[1] + fRayUnitVector[1] * fRun / fRayUnitVector[AXIS3(nMajor)];
            } else {
                fRun = vEdge[nMinor] - vStart[nMinor];
                vPos[AXIS3(nMinor)] = vEdge[nMinor];
                nLast = nMinor;
                vPos[AXIS3(nMajor)] = vStart[nMajor]
                                      + fRun * fRayUnitVector[AXIS3(nMajor)] / fRayUnitVector[AXIS3(nMinor)];
                vPos[1] = fPoint0[1] + fRayUnitVector[1] * fRun / fRayUnitVector[AXIS3(nMinor)];
            }
        }
        fLen = LLMath_DistanceBetween3(vPos, vPrev);
        if (Ter_CheckForObjectCollisionOneGrid(pTerrainData, nCell[0], nCell[1], vPrev, vPos, fRayUnitVector,
                                               fLen, vHit, vNormal, &pSurface, &pObj)) {
            fDist = LLMath_SquareDistanceBetween3(fPoint0, vHit);
            if (fDist < fClosest) {
                fClosest = fDist;
                LLMath_CopyVec(vHit, pCollisionPoint);
                LLMath_CopyVec(vNormal, pSurfaceNormal);
                *pSurfaceInfo = pSurface;
                *pSurfaceObject = pObj;
            }
        }
        if (nCell[0] == nEndX && nCell[1] == nEndZ) break;
        if (nLast == nMajor) {
            nCell[nMajor] += nStepMajor;
            vEdge[nMajor] += pTerrainData->fGridCellSize[nMajor] * nStepMajor;
        } else {
            nCell[nMinor] += nStepMinor;
            vEdge[nMinor] += pTerrainData->fGridCellSize[nMinor] * nStepMinor;
        }
    }
    if (fClosest != 1000000.0f && LLMath_DotProduct3(fRayUnitVector, pSurfaceNormal) > 0.0f) {
        LLMath_Negate(pSurfaceNormal, pSurfaceNormal);
    }
    return fClosest != 1000000.0f;
}

// Ter_CheckForWorldCollisionOneGrid without the kick bits, on the strips of course objects and of
// ground whose surface has MATERIAL_COLLIDES_AS_OBJECT (gTerObjectMarks is not read).
static u8 Ter_CheckForObjectCollisionOneGrid(TGD_TerrainInfo* pTerrainData, int nX, int nZ, f32* pFrom,
                                             f32* pTo, f32* pDir, f32 fMax, f32* pCollisionPoint,
                                             f32* pSurfaceNormal, TGD_MaterialInfo** pSurfaceInfo,
                                             TGD_ObjectInstanceInfo** pSurfaceObject) {
    f32 vHit[4];
    f32 vNormal[4];
    f32 fT;
    u8 bHit = 0;
    u32 iCurrentPlacementMask = 1 << Game_CurrentPinSet();
    TGD_Cell* pCell;
    TGD_PolygonReference* pPolyRef;
    f32 (*pVertexData)[3];
    u8* pDrawKickData;
    int j;
    int k;
    f32 fLow;
    f32 fHigh;

    if (nX >= 0 && nX < pTerrainData->nGridWidth && nZ >= 0 && nZ < pTerrainData->nGridLength) {
        fLow = pFrom[1];
        fHigh = pFrom[1];
        pCell = &pTerrainData->pGrid[nX + nZ * pTerrainData->nGridWidth];
        if (pTo[1] > pFrom[1]) {
            fHigh = pTo[1];
        }
        if (pTo[1] < fLow) {
            fLow = pTo[1];
        }
        if (fLow > (f32)pCell->nMaxHeight || fHigh < (f32)pCell->nMinHeight) return 0;
        pPolyRef = &pTerrainData->pPolyRefs[pCell->uRefs >> CELL_REFS_FIRST_SHIFT];
        for (j = (pCell->uRefs & CELL_REFS_COUNT_MASK) - 1; j >= 0; j--) {
            if (pPolyRef->nObject == 0
                && !(gSurfaceTypes[pPolyRef->nSurface].uFlags & MATERIAL_COLLIDES_AS_OBJECT)) {
                pPolyRef++;
            } else if (pPolyRef->uFlags != 0
                       && (!(pPolyRef->uFlags & iCurrentPlacementMask)
                           || ((pPolyRef->uFlags & POLYREF_NOT_IN_SPLIT_SCREEN) && gSession.nSplitScreen))) {
                pPolyRef++;
            } else {
                pVertexData = &pTerrainData->pVerts[TER_FIRST_VERTEX(pPolyRef)];
                pDrawKickData = pTerrainData->pTriFlags + TER_FIRST_VERTEX(pPolyRef);
                for (k = pPolyRef->nTris - 1; k >= 0; k--) {
                    if ((pDrawKickData[2] & TER_TRI_KICK_MASK)
                        && Ter_LineTriangleIntersection(pFrom, pDir, fMax, pVertexData, &fT, vHit, vNormal)) {
                        fMax = fT;
                        LLMath_CopyVec(vHit, pCollisionPoint);
                        LLMath_CopyVec(vNormal, pSurfaceNormal);
                        *pSurfaceInfo = &gSurfaceTypes[pPolyRef->nSurface];
                        if (pPolyRef->nObject != 0) {
                            *pSurfaceObject = &pTerrainData->pObjects[pPolyRef->nObject];
                        } else {
                            *pSurfaceObject = NULL;
                        }
                        bHit = 1;
                    }
                    pVertexData++;
                    pDrawKickData++;
                }
                pPolyRef++;
            }
        }
        return bHit;
    }
    return 0;
}

// Marks in the flag byte of every triangle of the course data whose byte is not 0 (objects' strips
// included) which corner is highest and which lowest (TER_TRI_HIGHEST_SHIFT, TER_TRI_LOWEST_SHIFT),
// for the lookups' corner tests. A byte that already has TER_TRI_DONE is left as it is; the bit is
// set on each byte marked and cleared again from every byte afterwards.
static void Ter_ComputeHighestPointInEveryTriangle(TGD_TerrainInfo* pTerrainData) {
    u8 uiDrawKick;
    f32 (*pVertices)[3];
    u32 uiPolygonReference;
    u32 uiVertex;
    u8* pDrawKicks;
    TGD_PolygonReference* pPolygonReference;
    u8 uiMax;
    u8 uiMin;

    pPolygonReference = pTerrainData->pPolyRefs;
    uiPolygonReference = pTerrainData->nPolyRefs;
    while (uiPolygonReference != 0) {
        pVertices = &pTerrainData->pVerts[TER_FIRST_VERTEX(pPolygonReference)];
        pDrawKicks = pTerrainData->pTriFlags + TER_FIRST_VERTEX(pPolygonReference);
        uiVertex = pPolygonReference->nTris;
        while (uiVertex != 0) {
            uiDrawKick = pDrawKicks[2];
            if (!(uiDrawKick & TER_TRI_DONE) && uiDrawKick != 0) {
                if (pVertices[0][1] > pVertices[1][1]) {
                    if (pVertices[0][1] > pVertices[2][1]) {
                        uiMax = 0;
                    } else {
                        uiMax = 2;
                    }
                } else if (pVertices[1][1] > pVertices[2][1]) {
                    uiMax = 1;
                } else {
                    uiMax = 2;
                }
                if (pVertices[0][1] < pVertices[1][1]) {
                    if (pVertices[0][1] < pVertices[2][1]) {
                        uiMin = 0;
                    } else {
                        uiMin = 2;
                    }
                } else if (pVertices[1][1] < pVertices[2][1]) {
                    uiMin = 1;
                } else {
                    uiMin = 2;
                }
                pDrawKicks[2] = uiDrawKick | (uiMax << TER_TRI_HIGHEST_SHIFT)
                                | (uiMin << TER_TRI_LOWEST_SHIFT) | TER_TRI_DONE;
            }
            uiVertex--;
            pDrawKicks++;
            pVertices++;
        }
        uiPolygonReference--;
        pPolygonReference++;
    }
    pPolygonReference = pTerrainData->pPolyRefs;
    uiPolygonReference = pTerrainData->nPolyRefs;
    while (uiPolygonReference != 0) {
        pVertices = &pTerrainData->pVerts[TER_FIRST_VERTEX(pPolygonReference)];
        pDrawKicks = pTerrainData->pTriFlags + TER_FIRST_VERTEX(pPolygonReference);
        uiVertex = pPolygonReference->nTris;
        while (uiVertex != 0) {
            pDrawKicks[2] &= (u8)~TER_TRI_DONE;
            uiVertex--;
            pDrawKicks++;
            pVertices++;
        }
        uiPolygonReference--;
        pPolygonReference++;
    }
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

// Negates the first three floats of src into dest; dest's fourth float is left as it is.
static asm void LLMath_Negate3(register const f32* src, register f32* dest) {
    nofralloc
    psq_l  f0, 0(src), 0, 0
    psq_l  f1, 8(src), 1, 0
    ps_neg f0, f0
    ps_neg f1, f1
    psq_st f0, 0(dest), 0, 0
    psq_st f1, 8(dest), 1, 0
    blr
}

// Negates all four floats of src into dest.
static asm void LLMath_Negate(register const f32* src, register f32* dest) {
    nofralloc
    psq_l  f0, 0(src), 0, 0
    psq_l  f1, 8(src), 0, 0
    ps_neg f0, f0
    ps_neg f1, f1
    psq_st f0, 0(dest), 0, 0
    psq_st f1, 8(dest), 0, 0
    blr
}

// The weights of a point (x, z) against a triangle's three corners in the x-z plane, which add up
// to 1: the point's height on the triangle is the corners' heights weighted by them.
void Ter_GetBarycentricCoords(f32 (*pVertexData)[3], f32* location, f32* pWeight0, f32* pWeight1,
                              f32* pWeight2) {
    f32 fX;
    f32 fZ;
    f32 fInv;

    fX = location[0];
    fZ = location[2];
    fInv = 1.0f / ((pVertexData[1][0] - pVertexData[0][0]) * (pVertexData[2][2] - pVertexData[0][2])
                   - (pVertexData[2][0] - pVertexData[0][0]) * (pVertexData[1][2] - pVertexData[0][2]));
    *pWeight0 = fInv * ((pVertexData[1][0] - fX) * (pVertexData[2][2] - fZ)
                        - (pVertexData[2][0] - fX) * (pVertexData[1][2] - fZ));
    *pWeight1 = fInv * ((pVertexData[2][0] - fX) * (pVertexData[0][2] - fZ)
                        - (pVertexData[0][0] - fX) * (pVertexData[2][2] - fZ));
    *pWeight2 = fInv * ((pVertexData[0][0] - fX) * (pVertexData[1][2] - fZ)
                        - (pVertexData[1][0] - fX) * (pVertexData[0][2] - fZ));
}

// Whether (x, z) lies inside the triangle t0 t1 t2 seen from above, whichever way round it turns;
// a point on an edge counts as inside, and a triangle seen edge-on holds nothing.
u8 Ter_PointInTriangleXZpY(f32* t0, f32* t1, f32* t2, f32 x, f32 z) {
    f32 fEdgeT0T1x = t1[0] - t0[0];
    f32 fEdgeT0T1z = t1[2] - t0[2];
    f32 fEdgeT1T2x = t2[0] - t1[0];
    f32 fEdgeT1T2z = t2[2] - t1[2];
    f32 tCross = fEdgeT0T1x * fEdgeT1T2z - fEdgeT0T1z * fEdgeT1T2x;
    f32 xzCross;

    if (tCross == 0.0f) return 0;
    xzCross = fEdgeT0T1x * (z - t0[2]) - fEdgeT0T1z * (x - t0[0]);
    if (tCross < 0.0f) {
        if (xzCross > 0.0f) return 0;
        if (fEdgeT1T2x * (z - t1[2]) - fEdgeT1T2z * (x - t1[0]) > 0.0f) return 0;
        if ((t0[0] - t2[0]) * (z - t2[2]) - (t0[2] - t2[2]) * (x - t2[0]) > 0.0f) return 0;
    } else {
        if (xzCross < 0.0f) return 0;
        if (fEdgeT1T2x * (z - t1[2]) - fEdgeT1T2z * (x - t1[0]) < 0.0f) return 0;
        if ((t0[0] - t2[0]) * (z - t2[2]) - (t0[2] - t2[2]) * (x - t2[0]) < 0.0f) return 0;
    }
    return 1;
}

// The mesh's flag byte i. Ter_CheckObjectAndHazardObstruction tests UOBJMESH_F0_NO_FADE in byte 0
// of a course object's model.
static int TerCollision_GetMeshFlags(UObjMesh* pMesh, int i) {
    return pMesh->pInfo->aFlags[i];
}
