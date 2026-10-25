// GoTerrainCollision.c (TW06's goterraincollision.c; TW07 splits it into GoTerrainCollision.c,
// GoTerrainCollision_Headgate.c and GoTerrainUtils.c): the course's ground as collision data
// (TGD_TerrainInfo): the free-drop and out-of-bounds networks, the drop
// checks, the ambient light, heights, normals and surfaces under a point, and the ball's collision
// tests against the ground, the pin and the course objects. The ground is triangle strips found
// through a grid; each triangle's surface is a row of gSurfaceTypes. It ends with small helpers
// (vector differences and negations, the point-in-triangle and barycentric tests, a surface's
// row). The file starts at 0x8004AFA0 (the code from there on shares one constant pool) and ends
// where Ball.c begins.

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "endian.h"
#include "dynobj.h"

#define AXIS3(n) ((n) == 0 ? 0 : 2)    // grid axis 0 (x) or 1 (z) as an index into a 3D vector
#define PIN_RADIUS_SQ 0.00077160494f   // the flagstick's radius squared: (1 inch)^2 in square yards
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
void  Ter_Subtract3(f32* pA, f32* pB, f32* pOut);           // a - b (paired-single assembly)
void  Ter_Negate3(f32* pSrc, f32* pDst);                  // negate (paired-single assembly)
void  Ter_Negate(f32* pSrc, f32* pDst);                  // negate, four floats (paired-single assembly)

static inline f32 Ter_Floor(f32 x) {
    return Math_Floor(x);
}

// The grid cell a coordinate falls in along one axis, given as cells from the grid's corner (the
// cell may be outside the grid).
static inline int Ter_GridCell(f32 fCells) {
    return (int)Ter_Floor(fCells);
}

void  Ter_ComputeHighestPointInEveryTriangle(TGD_TerrainInfo* pCourse);
static f32 Ter_CalcLowestPlayableWorldHeight(TGD_TerrainInfo* pTerrainData);
f32   Ter_GetLowestGroundTriangle(TGD_TerrainInfo* pCourse, f32* pPos, TGD_Cell** ppCell,
                                  TGD_PolygonReference** ppRef,
                                  f32 (**ppTri)[3],
                  s32* pTri);
f32   Ter_GetCoveringGroundTriangle(TGD_TerrainInfo* pCourse, f32* pPos, TGD_Cell** ppCell,
                                    TGD_PolygonReference** ppRef,
                                    f32 (**ppTri)[3],
                  s32* pTri);
void  Ter_GetSupportingAndCoveringGroundTriangles(TGD_TerrainInfo* pCourse, f32* pPos,
                                                  TGD_PolygonReference** ppRefLow,
                                                  f32* pLow, f32 (**ppTriLow)[3],
                  TGD_PolygonReference** ppRefHigh, f32* pHigh, f32 (**ppTriHigh)[3]);
u8    Ter_CheckForGroundCollision(TGD_TerrainInfo* pCourse, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal,
                                  TGD_MaterialInfo** ppSurface, TerObject** ppObj);
u8    Ter_CheckForWorldCollisionOneGrid(TGD_TerrainInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo,
                                        f32* pDir,
                                        f32 fMax, f32* pHit,
                  f32* pNormal, TGD_MaterialInfo** ppSurface, TerObject** ppObj, u8* pbFlags);
u8    Ter_CheckForSolidWorldCollisionOneGrid(TGD_TerrainInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo,
                                             f32* pDir, f32 fMax, f32* pHit,
                  f32* pNormal, TGD_MaterialInfo** ppSurface, TerObject** ppObj);
u8    Ter_CheckForGroundCollisionOneGrid(TGD_TerrainInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo,
                                         f32* pDir, f32 fMax, f32* pHit,
                  f32* pNormal, TGD_MaterialInfo** ppSurface, TerObject** ppObj);
u8    Ter_CheckForObjectCollisionOneGrid(TGD_TerrainInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo,
                                         f32* pDir, f32 fMax, f32* pHit,
                  f32* pNormal, TGD_MaterialInfo** ppSurface, TerObject** ppObj);
u8    Ter_LineSphereIntersection(f32* pFrom, f32* pDir, f32 fRange, f32* pCentre, f32 fRadius);
int   TerCollision_GetMeshFlags(UObjMesh* pModel, int n);

// fake match: never called, puts 1.0f first in the constant pool
static f32 GoTerrainCollision_StrippedFn(f32 x) {
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
    Ter_Subtract3(vB, vA, vE1);
    Ter_Subtract3(vC, vA, vE2);
    vec4flt_CrossProduct(pDir, vE2, vP);
    fDet = LLMath_DotProduct3(vE1, vP);
    if (fDet > -0.00001f && fDet < 0.00001f) return 0;
    fInv = 1.0f / fDet;
    Ter_Subtract3(pFrom, vA, vS);
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
    f32 (*pVert)[3];

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
                pVert = &pTerrainData->pVerts[TER_FIRST_VERTEX(&pTerrainData->pPolyRefs[i])];
                for (j = 0; j < pTerrainData->pPolyRefs[i].nTris + 2; j++) {
                    if (pVert[0][0] < gTerCupGeometryBounds[k].vMin[0]) {
                        gTerCupGeometryBounds[k].vMin[0] = pVert[0][0];
                    }
                    if (pVert[0][1] < gTerCupGeometryBounds[k].vMin[1]) {
                        gTerCupGeometryBounds[k].vMin[1] = pVert[0][1];
                    }
                    if (pVert[0][2] < gTerCupGeometryBounds[k].vMin[2]) {
                        gTerCupGeometryBounds[k].vMin[2] = pVert[0][2];
                    }
                    if (pVert[0][0] > gTerCupGeometryBounds[k].vMax[0]) {
                        gTerCupGeometryBounds[k].vMax[0] = pVert[0][0];
                    }
                    if (pVert[0][1] > gTerCupGeometryBounds[k].vMax[1]) {
                        gTerCupGeometryBounds[k].vMax[1] = pVert[0][1];
                    }
                    if (pVert[0][2] > gTerCupGeometryBounds[k].vMax[2]) {
                        gTerCupGeometryBounds[k].vMax[2] = pVert[0][2];
                    }
                    pVert++;
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
    TGD_PolygonReference* apRef[TER_NUM_CHECK_POINTS];
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
    TGD_PolygonReference* pRef;
    u16* pObjRef;
    f32 (*pVert)[3];
    u8* pFlags;
    u8 uFlags;
    UObjMesh* pModel;

    bObstructed = 0;
    bFirst = 1;
    pTerrainData = Ter_GetTGD();
    if (pTerrainData == NULL) return 0;
    for (i = 0; i < TER_NUM_CHECK_POINTS; i++) {
        aHeight[i] = -50000.0f;
        abFreeDrop[i] = 0;
        apRef[i] = NULL;
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
                pRef = &pTerrainData->pPolyRefs[pCell->uRefs >> CELL_REFS_FIRST_SHIFT];
                for (nRef = (pCell->uRefs & CELL_REFS_COUNT_MASK) - 1; nRef >= 0; nRef--) {
                    if (pRef->nObject != 0) {
                        pRef++;
                    } else {
                        pVert = &pTerrainData->pVerts[TER_FIRST_VERTEX(pRef)];
                        pFlags = pTerrainData->pTriFlags + TER_FIRST_VERTEX(pRef);
                        for (j = pRef->nTris - 1; j >= 0; j--) {
                            uFlags = pFlags[2];
                            if (bFirst) {
                                abFreeDrop[0] = Ter_PointInFreeDropNetwork(position);
                            }
                            if ((uFlags & TER_TRI_KICK_MASK)
                                && (fTop > pVert[TER_TRI_LOWEST_CORNER(uFlags)][1]
                                    || aHeight[0] < pVert[TER_TRI_HIGHEST_CORNER(uFlags)][1])
                                && Ter_PointInTriangleXZpY(pVert[0], pVert[1], pVert[2], position[0],
                                                           position[2])) {
                                Ter_GetBarycentricCoords(pVert, position, &fA, &fB, &fC);
                                fHeight = fA * pVert[0][1] + fB * pVert[1][1] + fC * pVert[2][1];
                                if (fHeight > aHeight[0] && fHeight <= fTop) {
                                    aHeight[0] = fHeight;
                                    apRef[0] = pRef;
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
                                    if ((uFlags & TER_TRI_KICK_MASK)
                                        && (fTop > pVert[TER_TRI_LOWEST_CORNER(uFlags)][1]
                                            || aHeight[k] < pVert[TER_TRI_HIGHEST_CORNER(uFlags)][1])
                                        && Ter_PointInTriangleXZpY(pVert[0], pVert[1], pVert[2], vPos[0],
                                                vPos[2])) {
                                        Ter_GetBarycentricCoords(pVert, vPos, &fA2, &fB2, &fC2);
                                        fHeight = fA2 * pVert[0][1] + fB2 * pVert[1][1] + fC2 * pVert[2][1];
                                        if (fHeight > aHeight[k] && fHeight <= fTop) {
                                            aHeight[k] = fHeight;
                                            apRef[k] = pRef;
                                        }
                                    }
                                    vPos[2] += 2.0f * hazardDistance;
                                    k++;
                                }
                                vPos[0] += 2.0f * hazardDistance;
                            }
                            bFirst = 0;
                            pVert++;
                            pFlags++;
                        }
                        pRef++;
                    }
                }
            }
        }
    }
    if (apRef[0] != NULL) {
        if (gSurfaceTypes[apRef[0]->nSurface].nClass == SURFCLASS_GREEN_e
            || gSurfaceTypes[apRef[0]->nSurface].nClass == SURFCLASS_FAIRWAY_e
            || gSurfaceTypes[apRef[0]->nSurface].nClass == SURFCLASS_SAND_e) {
            bObstructed = 0;
        }
    }
    if (bObstructed) return 1;
    if (bCheckLie) {
        if ((apRef[0] != NULL && !(gSurfaceTypes[apRef[0]->nSurface].uFlags & MATERIAL_BALL_CAN_LIE))
            || abFreeDrop[0]) {
            return 1;
        }
        for (k = 1; k < TER_NUM_CHECK_POINTS; k++) {
            if (abFreeDrop[k] || apRef[k] == NULL
                || (apRef[k] != NULL && !(gSurfaceTypes[apRef[k]->nSurface].uFlags & MATERIAL_BALL_CAN_LIE)
                    && gSurfaceTypes[apRef[k]->nSurface].nClass != SURFCLASS_CARTPATH_e)) {
                return 1;
            }
        }
    }
    if (bCheckHeight) {
        for (k = 1; k < TER_NUM_CHECK_POINTS; k++) {
            if (apRef[k] != NULL && fabsf(aHeight[k] - aHeight[0]) > hazardDistance * fMaxSlope) return 1;
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
// for a drop (a preferred one with bUsePreferredLies) on the ball's surface class, else the
// nearest. Returns 0 when the spot is where the shot started (x, z), or, with bRevertToTee and the
// shot played from the tee (vBall at vTeePos), when it is within 50 yards of the tee.
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
        Ter_Subtract3(&pTerrainData->pin[Game_CurrentPinSet()].x, pPlayer->ball.vPos, delta);
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

// The lowest ground triangle under a point (x, z), with the same outputs and strip rules as
// Ter_GetHighestGroundTriangle; TER_NO_GROUND when there is none.
f32 Ter_GetLowestGroundTriangle(TGD_TerrainInfo* pCourse, f32* pPos, TGD_Cell** ppCell,
                                TGD_PolygonReference** ppRef,
                                f32 (**ppTri)[3],
                s32* pTri) {
    u32 uPinSet = 1 << Game_CurrentPinSet();
    f32 fBest = 50000.0f;
    int nX = (int)Ter_Floor((pPos[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    int nZ = (int)Ter_Floor((pPos[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
    TGD_Cell* pCell;
    TGD_PolygonReference* pRef;
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
        pRef = &pCourse->pPolyRefs[pCell->uRefs >> CELL_REFS_FIRST_SHIFT];
        for (i = (pCell->uRefs & CELL_REFS_COUNT_MASK) - 1; i >= 0; i--) {
            if (pRef->nObject != 0) {
                pRef++;
            } else if (pRef->uFlags != 0
                       && (!(pRef->uFlags & uPinSet)
                           || ((pRef->uFlags & POLYREF_NOT_IN_SPLIT_SCREEN) && gSession.nSplitScreen))) {
                pRef++;
            } else {
                pVert = &pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
                pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
                for (j = pRef->nTris - 1; j >= 0; j--) {
                    if ((pFlags[2] & TER_TRI_KICK_MASK) && fBest > pVert[TER_TRI_LOWEST_CORNER(pFlags[2])][1]
                        && Ter_PointInTriangleXZpY(pVert[0], pVert[1], pVert[2], pPos[0], pPos[2])) {
                        Ter_GetBarycentricCoords(pVert, pPos, &fA, &fB, &fC);
                        fHeight = fA * pVert[0][1] + fB * pVert[1][1] + fC * pVert[2][1];
                        if (fHeight < fBest) {
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
        if (fBest == 50000.0f) return TER_NO_GROUND;
        return fBest;
    }
    return TER_NO_GROUND;
}

// The supporting triangle of the world under a point: the highest one at or below its height,
// object triangles included (so a ball on a bridge is on the bridge). Returns its height there,
// with the grid cell, strip and vertices; TER_NO_GROUND when there is none.
f32 Ter_GetSupportingWorldTriangle(TGD_TerrainInfo* pCourse, f32* pPos, TGD_Cell** ppCell,
                                   TGD_PolygonReference** ppRef,
                                   f32 (**ppTri)[3]) {
    u32 uPinSet = 1 << Game_CurrentPinSet();
    f32 fBest = TER_NO_GROUND;
    int nX = (int)Ter_Floor((pPos[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    int nZ = (int)Ter_Floor((pPos[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
    TGD_Cell* pCell;
    TGD_PolygonReference* pRef;
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
        if ((f32)pCell->nMinHeight > pPos[1]) return TER_NO_GROUND;
        pRef = &pCourse->pPolyRefs[pCell->uRefs >> CELL_REFS_FIRST_SHIFT];
        for (i = (pCell->uRefs & CELL_REFS_COUNT_MASK) - 1; i >= 0; i--) {
            if (pRef->uFlags != 0
                && (!(pRef->uFlags & uPinSet)
                    || ((pRef->uFlags & POLYREF_NOT_IN_SPLIT_SCREEN) && gSession.nSplitScreen))) {
                pRef++;
            } else {
                pVert = &pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
                pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
                for (j = pRef->nTris - 1; j >= 0; j--) {
                    if ((pFlags[2] & TER_TRI_KICK_MASK)
                        && (pPos[1] > pVert[TER_TRI_LOWEST_CORNER(pFlags[2])][1]
                            || fBest < pVert[TER_TRI_HIGHEST_CORNER(pFlags[2])][1])
                        && Ter_PointInTriangleXZpY(pVert[0], pVert[1], pVert[2], pPos[0], pPos[2])) {
                        Ter_GetBarycentricCoords(pVert, pPos, &fA, &fB, &fC);
                        fHeight = fA * pVert[0][1] + fB * pVert[1][1] + fC * pVert[2][1];
                        if (fHeight > fBest && fHeight <= pPos[1]) {
                            *ppCell = pCell;
                            fBest = fHeight;
                            *ppRef = pRef;
                            *ppTri = pVert;
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

// The covering ground triangle over a point: the lowest ground triangle at or above its height.
// Returns its height there, with the grid cell, strip, vertices and the triangle's number in the
// strip; TER_NO_GROUND when there is none.
f32 Ter_GetCoveringGroundTriangle(TGD_TerrainInfo* pCourse, f32* pPos, TGD_Cell** ppCell,
                                  TGD_PolygonReference** ppRef,
                                  f32 (**ppTri)[3],
                s32* pTri) {
    u32 uPinSet = 1 << Game_CurrentPinSet();
    f32 fBest = 65536.0f;
    int nX = (int)Ter_Floor((pPos[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    int nZ = (int)Ter_Floor((pPos[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
    TGD_Cell* pCell;
    TGD_PolygonReference* pRef;
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
        if ((f32)pCell->nMaxHeight < pPos[1]) return TER_NO_GROUND;
        pRef = &pCourse->pPolyRefs[pCell->uRefs >> CELL_REFS_FIRST_SHIFT];
        for (i = (pCell->uRefs & CELL_REFS_COUNT_MASK) - 1; i >= 0; i--) {
            if (pRef->nObject != 0) {
                pRef++;
            } else if (pRef->uFlags != 0
                       && (!(pRef->uFlags & uPinSet)
                           || ((pRef->uFlags & POLYREF_NOT_IN_SPLIT_SCREEN) && gSession.nSplitScreen))) {
                pRef++;
            } else {
                pVert = &pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
                pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
                for (j = pRef->nTris - 1; j >= 0; j--) {
                    if ((pFlags[2] & TER_TRI_KICK_MASK)
                        && (fBest > pVert[TER_TRI_LOWEST_CORNER(pFlags[2])][1]
                            || pPos[1] < pVert[TER_TRI_HIGHEST_CORNER(pFlags[2])][1])
                        && Ter_PointInTriangleXZpY(pVert[0], pVert[1], pVert[2], pPos[0], pPos[2])) {
                        Ter_GetBarycentricCoords(pVert, pPos, &fA, &fB, &fC);
                        fHeight = fA * pVert[0][1] + fB * pVert[1][1] + fC * pVert[2][1];
                        if (fHeight < fBest && fHeight >= pPos[1]) {
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
        if (fBest == 65536.0f) return TER_NO_GROUND;
        return fBest;
    }
    return TER_NO_GROUND;
}

// The ground triangles just below (supporting) and just above (covering) a point, in one pass:
// their heights (TER_NO_GROUND for none), strips and vertices. TW06:
// void Ter_GetSupportingAndCoveringGroundTriangles(TGD_TerrainInfo*, f32*, TGD_PolygonReference**, f32*,
// f32***, TGD_PolygonReference**, f32*, f32***), the same parameters.
void Ter_GetSupportingAndCoveringGroundTriangles(TGD_TerrainInfo* pCourse, f32* pPos,
                                                 TGD_PolygonReference** ppRefLow,
                                                 f32* pLow, f32 (**ppTriLow)[3],
                 TGD_PolygonReference** ppRefHigh, f32* pHigh, f32 (**ppTriHigh)[3]) {
    u32 uPinSet = 1 << Game_CurrentPinSet();
    f32 fHigh = 65536.0f;
    f32 fLow = -65536.0f;
    int nX = (int)Ter_Floor((pPos[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    int nZ = (int)Ter_Floor((pPos[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
    TGD_Cell* pCell;
    TGD_PolygonReference* pRef;
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
        pRef = &pCourse->pPolyRefs[pCell->uRefs >> CELL_REFS_FIRST_SHIFT];
        for (i = (pCell->uRefs & CELL_REFS_COUNT_MASK) - 1; i >= 0; i--) {
            if (pRef->nObject != 0) {
                pRef++;
            } else if (pRef->uFlags != 0
                       && (!(pRef->uFlags & uPinSet)
                           || ((pRef->uFlags & POLYREF_NOT_IN_SPLIT_SCREEN) && gSession.nSplitScreen))) {
                pRef++;
            } else {
                pVert = &pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
                pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
                for (j = pRef->nTris - 1; j >= 0; j--) {
                    if ((pFlags[2] & TER_TRI_KICK_MASK)
                        && (fHigh > pVert[TER_TRI_LOWEST_CORNER(pFlags[2])][1] || fLow
                            < pVert[TER_TRI_HIGHEST_CORNER(pFlags[2])][1])
                        && Ter_PointInTriangleXZpY(pVert[0], pVert[1], pVert[2], pPos[0], pPos[2])) {
                        Ter_GetBarycentricCoords(pVert, pPos, &fA, &fB, &fC);
                        fHeight = fA * pVert[0][1] + fB * pVert[1][1] + fC * pVert[2][1];
                        if (fHeight < fHigh && fHeight >= pPos[1]) {
                            *ppRefHigh = pRef;
                            fHigh = fHeight;
                            *ppTriHigh = pVert;
                        }
                        if (fHeight > fLow && fHeight <= pPos[1]) {
                            *ppRefLow = pRef;
                            fLow = fHeight;
                            *ppTriLow = pVert;
                        }
                    }
                    pVert++;
                    pFlags++;
                }
                pRef++;
            }
        }
        *pHigh = fHigh == 65536.0f ? TER_NO_GROUND : fHigh;
        *pLow = fLow == -65536.0f ? TER_NO_GROUND : fLow;
        return;
    }
    *pLow = TER_NO_GROUND;
    *pHigh = TER_NO_GROUND;
}

// The height of the highest ground triangle under a point (x, z), whatever the point's own height
// (Ter_GetHighestGroundTriangle); TER_NO_GROUND when there is none.
f32 Ter_GetHighestGroundHeight(TGD_TerrainInfo* pCourse, f32* pPos) {
    TGD_Cell* pCell;
    TGD_PolygonReference* pRef;
    f32 (*pTri)[3];
    s32 nTri;

    return Ter_GetHighestGroundTriangle(pCourse, pPos, &pCell, &pRef, &pTri, &nTri);
}

// The height of the lowest ground triangle under a point (x, z), whatever the point's own height
// (Ter_GetLowestGroundTriangle); TER_NO_GROUND when there is none.
f32 Ter_GetLowestGroundHeight(TGD_TerrainInfo* pCourse, f32* pPos) {
    TGD_Cell* pCell;
    TGD_PolygonReference* pRef;
    f32 (*pTri)[3];
    s32 nTri;

    return Ter_GetLowestGroundTriangle(pCourse, pPos, &pCell, &pRef, &pTri, &nTri);
}

// The height of the supporting ground under a point: the highest ground triangle at or below it
// (Ter_GetSupportingGroundTriangle); TER_NO_GROUND when there is none.
f32 Ter_GetSupportingGroundHeight(TGD_TerrainInfo* pCourse, f32* pPos) {
    TGD_Cell* pCell;
    TGD_PolygonReference* pRef;
    f32 (*pTri)[3];
    s32 nTri;

    return Ter_GetSupportingGroundTriangle(pCourse, pPos, &pCell, &pRef, &pTri, &nTri);
}

// The height of the ground covering a point (Ter_GetCoveringGroundTriangle) and, when there is one,
// that triangle's normal turned to face up; TER_NO_GROUND (pNormal untouched) when there is none.
f32 Ter_GetCoveringGroundHeightAndNormal(TGD_TerrainInfo* pCourse, f32* pPos, f32* pNormal) {
    f32 vA[4];
    f32 vB[4];
    f32 vC[4];
    f32 vAB[4];
    f32 vBC[4];
    TGD_Cell* pCell;
    TGD_PolygonReference* pRef;
    f32 (*pTri)[3];
    s32 nTri;
    f32 fHeight;

    fHeight = Ter_GetCoveringGroundTriangle(pCourse, pPos, &pCell, &pRef, &pTri, &nTri);
    if (fHeight != TER_NO_GROUND) {
        Vec3Copy(pTri[0], vA);
        Vec3Copy(pTri[1], vB);
        Vec3Copy(pTri[2], vC);
        Ter_Subtract3(vA, vB, vAB);
        Ter_Subtract3(vB, vC, vBC);
        vec4flt_CrossProduct(vAB, vBC, pNormal);
        LLMath_Normalize3(pNormal, pNormal);
        if (pNormal[1] < 0.0f) {
            Ter_Negate3(pNormal, pNormal);
        }
        return fHeight;     // fake match: a return of its own, so the other path skips the final fmr
    }
    return fHeight;
}

// TW06: bool Ter_GetSupportingGroundNormal(TGD_TerrainInfo*, f32*, f32*). The upward normal of the
// ground under a point; 0 when there is none.
u8 Ter_GetSupportingGroundNormal(TGD_TerrainInfo* pCourse, f32* pPos, f32* pNormal) {
    f32 vA[4];
    f32 vB[4];
    f32 vC[4];
    f32 vAB[4];
    f32 vBC[4];
    TGD_Cell* pCell;
    TGD_PolygonReference* pRef;
    f32 (*pTri)[3];
    s32 nTri;

    if (Ter_GetSupportingGroundTriangle(pCourse, pPos, &pCell, &pRef, &pTri, &nTri) != TER_NO_GROUND) {
        Vec3Copy(pTri[0], vA);
        Vec3Copy(pTri[1], vB);
        Vec3Copy(pTri[2], vC);
        Ter_Subtract3(vA, vB, vAB);
        Ter_Subtract3(vB, vC, vBC);
        vec4flt_CrossProduct(vAB, vBC, pNormal);
        LLMath_Normalize3(pNormal, pNormal);
        if (pNormal[1] < 0.0f) {
            Ter_Negate3(pNormal, pNormal);
        }
        return 1;
    }
    return 0;
}

// The height of whatever supports a point, object triangles included
// (Ter_GetSupportingWorldTriangle); TER_NO_GROUND when there is none.
f32 Ter_GetSupportingWorldHeight(TGD_TerrainInfo* pCourse, f32* pPos) {
    TGD_Cell* pCell;
    TGD_PolygonReference* pRef;
    f32 (*pTri)[3];

    return Ter_GetSupportingWorldTriangle(pCourse, pPos, &pCell, &pRef, &pTri);
}

// TW06: TGD_MaterialInfo* Ter_GetSupportingWorldMaterial(TGD_TerrainInfo*, f32*). The surface
// under a point, objects included; NULL when there is none.
TGD_MaterialInfo* Ter_GetSupportingWorldMaterial(TGD_TerrainInfo* pCourse, f32* pPos) {
    TGD_Cell* pCell;
    TGD_PolygonReference* pRef;
    f32 (*pTri)[3];

    if (Ter_GetSupportingWorldTriangle(pCourse, pPos, &pCell, &pRef, &pTri) != TER_NO_GROUND) {
        return &gSurfaceTypes[pRef->nSurface];
    }
    return NULL;
}

// TW06: f32 Ter_GetSupportingGroundData(const TGD_TerrainInfo*, const f32*, TGD_MaterialInfo**, f32*).
// The ground height under a point, with the surface there and the triangle's upward normal; with
// nothing under the point, TER_NO_GROUND and no surface.
f32 Ter_GetSupportingGroundData(TGD_TerrainInfo* pCourse, f32* pPos, TGD_MaterialInfo** ppSurface,
                                f32* pNormal) {
    f32 vA[4];
    f32 vB[4];
    f32 vC[4];
    f32 vAB[4];
    f32 vBC[4];
    TGD_Cell* pCell;
    TGD_PolygonReference* pRef;
    f32 (*pTri)[3];
    s32 nTri;
    f32 fHeight;

    fHeight = Ter_GetSupportingGroundTriangle(pCourse, pPos, &pCell, &pRef, &pTri, &nTri);
    if (fHeight != TER_NO_GROUND) {
        Vec3Copy(pTri[0], vA);
        Vec3Copy(pTri[1], vB);
        Vec3Copy(pTri[2], vC);
        Ter_Subtract3(vA, vB, vAB);
        Ter_Subtract3(vB, vC, vBC);
        vec4flt_CrossProduct(vAB, vBC, pNormal);
        LLMath_Normalize3(pNormal, pNormal);
        if (pNormal[1] < 0.0f) {
            Ter_Negate3(pNormal, pNormal);
        }
        *ppSurface = &gSurfaceTypes[pRef->nSurface];
    } else {
        *ppSurface = NULL;
    }
    return fHeight;
}

// TW06: void Ter_GetEnclosingGroundHeight(TGD_TerrainInfo*, f32*, f32*, f32*). The heights of the
// ground just below and just above a point (TER_NO_GROUND for none).
void Ter_GetEnclosingGroundHeight(TGD_TerrainInfo* pCourse, f32* pPos, f32* pLow, f32* pHigh) {
    TGD_PolygonReference* pRefLow;
    TGD_PolygonReference* pRefHigh;
    f32 (*pTriLow)[3];
    f32 (*pTriHigh)[3];

    Ter_GetSupportingAndCoveringGroundTriangles(pCourse, pPos, &pRefLow, pLow, &pTriLow, &pRefHigh, pHigh,
                                                &pTriHigh);
}

// TW06: void Ter_GetEnclosingGroundData(TGD_TerrainInfo*, f32*, f32*, TGD_MaterialInfo**, f32*, f32*,
// TGD_MaterialInfo**, f32*). The ground just below and just above a point: heights,
// surfaces and upward normals (TER_NO_GROUND and NULL for none).
void Ter_GetEnclosingGroundData(TGD_TerrainInfo* pCourse, f32* pPos, f32* pLow,
                                TGD_MaterialInfo** ppSurfaceLow, f32* pNormalLow, f32* pHigh,
                                TGD_MaterialInfo** ppSurfaceHigh, f32* pNormalHigh) {
    f32 vA[4];
    f32 vB[4];
    f32 vC[4];
    f32 vAB[4];
    f32 vBC[4];
    f32 vAB2[4];
    f32 vBC2[4];
    TGD_PolygonReference* pRefLow;
    TGD_PolygonReference* pRefHigh;
    f32 (*pTriLow)[3];
    f32 (*pTriHigh)[3];

    Ter_GetSupportingAndCoveringGroundTriangles(pCourse, pPos, &pRefLow, pLow, &pTriLow, &pRefHigh, pHigh,
                                                &pTriHigh);
    if (*pLow != TER_NO_GROUND) {
        Vec3Copy(pTriLow[0], vA);
        Vec3Copy(pTriLow[1], vB);
        Vec3Copy(pTriLow[2], vC);
        Ter_Subtract3(vA, vB, vAB);
        Ter_Subtract3(vB, vC, vBC);
        vec4flt_CrossProduct(vAB, vBC, pNormalLow);
        LLMath_Normalize3(pNormalLow, pNormalLow);
        if (pNormalLow[1] < 0.0f) {
            Ter_Negate3(pNormalLow, pNormalLow);
        }
        *ppSurfaceLow = &gSurfaceTypes[pRefLow->nSurface];
    } else {
        *ppSurfaceLow = NULL;
    }
    if (*pHigh != TER_NO_GROUND) {
        Vec3Copy(pTriHigh[0], vA);
        Vec3Copy(pTriHigh[1], vB);
        Vec3Copy(pTriHigh[2], vC);
        Ter_Subtract3(vA, vB, vAB2);
        Ter_Subtract3(vB, vC, vBC2);
        vec4flt_CrossProduct(vAB2, vBC2, pNormalHigh);
        LLMath_Normalize3(pNormalHigh, pNormalHigh);
        if (pNormalHigh[1] < 0.0f) {
            Ter_Negate3(pNormalHigh, pNormalHigh);
        }
        *ppSurfaceHigh = &gSurfaceTypes[pRefHigh->nSurface];
    } else {
        *ppSurfaceHigh = NULL;
    }
}

// TW06: f32 Ter_GetSupportingWorldData(TGD_TerrainInfo*, f32*, TGD_MaterialInfo**, f32*). The
// height of whatever supports a point (objects included), with its surface and upward normal;
// TER_NO_GROUND and no surface when there is none.
f32 Ter_GetSupportingWorldData(TGD_TerrainInfo* pCourse, f32* pPos, TGD_MaterialInfo** ppSurface,
                               f32* pNormal) {
    f32 vA[4];
    f32 vB[4];
    f32 vC[4];
    f32 vAB[4];
    f32 vBC[4];
    TGD_Cell* pCell;
    TGD_PolygonReference* pRef;
    f32 (*pTri)[3];
    f32 fHeight;

    fHeight = Ter_GetSupportingWorldTriangle(pCourse, pPos, &pCell, &pRef, &pTri);
    if (fHeight != TER_NO_GROUND) {
        Vec3Copy(pTri[0], vA);
        Vec3Copy(pTri[1], vB);
        Vec3Copy(pTri[2], vC);
        Ter_Subtract3(vA, vB, vAB);
        Ter_Subtract3(vB, vC, vBC);
        vec4flt_CrossProduct(vAB, vBC, pNormal);
        LLMath_Normalize3(pNormal, pNormal);
        if (pNormal[1] < 0.0f) {
            Ter_Negate3(pNormal, pNormal);
        }
        *ppSurface = &gSurfaceTypes[pRef->nSurface];
    } else {
        *ppSurface = NULL;
    }
    return fHeight;
}

// TW06: u32 Ter_GetTerrainLayers(TGD_TerrainInfo*, f32*, TGD_MaterialInfo**, f32*, u32). Every
// ground triangle over or under a point (x, z), up to nMax: their heights and surfaces, in the
// order found. Returns how many.
u32 Ter_GetTerrainLayers(TGD_TerrainInfo* pCourse, f32* pPos, TGD_MaterialInfo** ppSurfaces, f32* pHeights,
                         u32 nMax) {
    u32 uPinSet = 1 << Game_CurrentPinSet();
    u32 nFound = 0;
    int nX = (int)Ter_Floor((pPos[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    int nZ = (int)Ter_Floor((pPos[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
    TGD_Cell* pCell;
    TGD_PolygonReference* pRef;
    f32 (*pVert)[3];
    u8* pFlags;
    int i;
    int j;
    f32 fA;
    f32 fB;
    f32 fC;

    if (nX >= 0 && nX < pCourse->nGridWidth && nZ >= 0 && nZ < pCourse->nGridLength) {
        pCell = &pCourse->pGrid[nX + nZ * pCourse->nGridWidth];
        pRef = &pCourse->pPolyRefs[pCell->uRefs >> CELL_REFS_FIRST_SHIFT];
        for (i = (pCell->uRefs & CELL_REFS_COUNT_MASK) - 1; i >= 0; i--) {
            if (pRef->nObject != 0) {
                pRef++;
            } else if (pRef->uFlags != 0
                       && (!(pRef->uFlags & uPinSet)
                           || ((pRef->uFlags & POLYREF_NOT_IN_SPLIT_SCREEN) && gSession.nSplitScreen))) {
                pRef++;
            } else {
                pVert = &pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
                pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
                for (j = pRef->nTris - 1; j >= 0; j--) {
                    if ((pFlags[2] & TER_TRI_KICK_MASK)
                        && Ter_PointInTriangleXZpY(pVert[0], pVert[1], pVert[2], pPos[0], pPos[2])) {
                        Ter_GetBarycentricCoords(pVert, pPos, &fA, &fB, &fC);
                        pHeights[nFound] = fA * pVert[0][1] + fB * pVert[1][1] + fC * pVert[2][1];
                        ppSurfaces[nFound] = &gSurfaceTypes[pRef->nSurface];
                        nFound++;
                        if (nFound == nMax) return nFound;
                    }
                    pVert++;
                    pFlags++;
                }
                pRef++;
            }
        }
        return nFound;
    }
    return 0;
}

// Mark in gTerObjectMarks the objects of grid cell (nX, nZ) that the line from pFrom along pDir
// passes within fRange of (entry 0 is always marked).
void Ter_MarkObjectsNearLine(TGD_TerrainInfo* pCourse, f32* pFrom, f32* pDir, int nX, int nZ, f32 fRange) {
    int i;
    u16* pRefs;
    TGD_Cell* pCell;
    TerObject* pObj;

    gTerObjectMarks[0] = 1;
    for (i = 1; i < MAX_OBJECTS; i++) {
        gTerObjectMarks[i] = 0;
    }
    if (nX >= 0 && nX < pCourse->nGridWidth && nZ >= 0 && nZ < pCourse->nGridLength) {
        pCell = &pCourse->pGrid[nX + nZ * pCourse->nGridWidth];
        pRefs = &pCourse->pObjRefs[pCell->nObjRefOffset];
        for (i = pCell->nObjRefs - 1; i >= 0; i--) {
            pObj = &pCourse->pObjects[*pRefs];
            if (Ter_LineSphereIntersection(pFrom, pDir, fRange, pObj->vCentre, pObj->fRadius)) {
                gTerObjectMarks[*pRefs] = 1;
            }
            pRefs++;
        }
    }
}

// TW06: bool Ter_LineSphereIntersection(f32*, f32*, f32, f32*, f32). Whether a sphere (centre,
// radius) is within fRange of pFrom and either around it or ahead of it along pDir.
u8 Ter_LineSphereIntersection(f32* pFrom, f32* pDir, f32 fRange, f32* pCentre, f32 fRadius) {
    f32 vTo[4];
    f32 vCentre[4];
    f32 fDist;

    vCentre[0] = pCentre[0];
    vCentre[1] = pCentre[1];
    vCentre[2] = pCentre[2];
    vCentre[3] = 1.0f;
    Ter_Subtract3(vCentre, pFrom, vTo);
    fDist = (f32)Math_Sqrt(LLMath_LengthSquared3(vTo)) - fRadius;
    if (fDist > fRange) return 0;
    if (fDist < 0.0f) return 1;
    if (LLMath_DotProduct3(vTo, pDir) < 0.0f) return 0;
    return 1;
}

// TW06: bool Ter_CheckForPinCollision(TGD_TerrainInfo*, s32, f32*, f32*, f32[4]*, f32[4]*,
// TGD_MaterialInfo**, TGD_ObjectInstanceInfo**). Whether the line from pFrom to pTo hits the
// flagstick of the current hole: a vertical cylinder of one inch radius, 2 yards tall, at the pin.
// Not for nobody's ball, nor when the player's view has the flagstick out (bFlagOut). On
// a hit: the point, the stick's outward normal and surface 90 (the cup).
u8 Ter_CheckForPinCollision(TGD_TerrainInfo* pCourse, int nPlayer, f32* pFrom, f32* pTo, f32* pHit,
                            f32* pNormal,
                            TGD_MaterialInfo** ppSurface, TerObject** ppObj) {
    f32 vDelta[4];
    f32 vFlat[4];
    f32 vFrom[4];
    f32 vTo[4];
    f32 vIn[4];
    f32 fA;
    f32 fB;
    f32 fC;
    f32 fDisc;
    f32 fRoot;
    f32 fT1;
    f32 fT2;
    f32 fT;

    if (nPlayer < 0) return 0;
    if (ViewController_GetIndexedViewController(gPlayers[nPlayer].nView[0])->bFlagOut) return 0;
    // the line relative to the pin
    vFrom[0] = pFrom[0] - pCourse->pin[Game_CurrentPinSet()].x;
    vFrom[1] = pFrom[1] - pCourse->pin[Game_CurrentPinSet()].y;
    vFrom[2] = pFrom[2] - pCourse->pin[Game_CurrentPinSet()].z;
    vFrom[3] = 1.0f;
    vTo[0] = pTo[0] - pCourse->pin[Game_CurrentPinSet()].x;
    vTo[1] = pTo[1] - pCourse->pin[Game_CurrentPinSet()].y;
    vTo[2] = pTo[2] - pCourse->pin[Game_CurrentPinSet()].z;
    vTo[3] = 1.0f;
    Ter_Subtract3(vTo, vFrom, vDelta);
    Vec3Copy(vDelta, vFlat);
    vFlat[1] = 0.0f;
    vIn[0] = -vFrom[0];
    vIn[1] = 0.0f;
    vIn[2] = -vFrom[2];
    vIn[3] = 1.0f;
    if (LLMath_DotProduct3(vIn, vFlat) < 0.0f) return 0; // moving away from the stick
    fC = vFrom[0] * vFrom[0] + vFrom[2] * vFrom[2];
    if (fC < PIN_RADIUS_SQ) return 0;                   // already inside it
    // where the line meets the cylinder: a t^2 + b t + c = 0
    fA = vDelta[0] * vDelta[0] + vDelta[2] * vDelta[2];
    fB = 2.0f * vDelta[0] * vFrom[0] + 2.0f * vDelta[2] * vFrom[2];
    fDisc = fB * fB - 4.0f * fA * (fC - PIN_RADIUS_SQ);
    if (fDisc <= 0.0f) return 0;
    fRoot = (f32)Math_Sqrt(fDisc);
    fT1 = (-fB + fRoot) / (2.0f * fA);
    fT2 = (-fB - fRoot) / (2.0f * fA);
    fT = fT1 <= fT2 ? fT1 : fT2;
    if (fT <= 0.0f) {
        fT = fT1 <= fT2 ? fT2 : fT1;
    }
    if (fT <= 0.0f || fT >= 1.0f) return 0;
    pHit[0] = vDelta[0] * fT + vFrom[0];
    pHit[1] = vDelta[1] * fT + vFrom[1];
    pHit[2] = vDelta[2] * fT + vFrom[2];
    pHit[3] = 1.0f;
    if (pHit[1] >= 2.0f) return 0;                      // over the top of the stick
    pNormal[0] = pHit[0];
    pNormal[1] = 0.0f;
    pNormal[2] = pHit[2];
    pNormal[3] = 1.0f;
    LLMath_Normalize3(pNormal, pNormal);
    pHit[0] += pCourse->pin[Game_CurrentPinSet()].x;
    pHit[1] += pCourse->pin[Game_CurrentPinSet()].y;
    pHit[2] += pCourse->pin[Game_CurrentPinSet()].z;
    *ppSurface = &gSurfaceTypes[90];
    *ppObj = NULL;
    return 1;
}

// TW06: bool Ter_CheckForWorldCollision(TGD_TerrainInfo*, s32, f32*, f32*, f32[4]*, f32[4]*,
// TGD_MaterialInfo**, TGD_ObjectInstanceInfo**, u8*). The first thing the line from pFrom to pTo
// hits: the pin, the ground or an object. Walks the grid cells the line crosses, testing each
// cell's triangles (Ter_CheckForWorldCollisionOneGrid) and keeping the hit nearest pFrom. The
// normal is turned to face the line.
u8 Ter_CheckForWorldCollision(TGD_TerrainInfo* pCourse, int nPlayer, f32* pFrom, f32* pTo, f32* pHit,
                              f32* pNormal,
               TGD_MaterialInfo** ppSurface, TerObject** ppObj, u8* pbFlags) {
    int nEndX;
    int nEndZ;
    f32 vHit[4];
    f32 vNormal[4];
    f32 vDir[4];
    f32 vPrev[4];
    f32 vPos[4];
    int nCell[2];
    f32 vDelta[2];
    f32 vStart[2];
    f32 vEdge[2];
    TGD_MaterialInfo* pSurface;
    TerObject* pObj;
    f32 fBest = 1000000.0f;
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

    if (pFrom[0] == pTo[0] && pFrom[1] == pTo[1] && pFrom[2] == pTo[2]) return 0;
    if (Ter_CheckForPinCollision(pCourse, nPlayer, pFrom, pTo, pHit, pNormal, ppSurface, ppObj)) {
        fBest = LLMath_SquareDistanceBetween3(pFrom, pHit);
    }
    nCell[0] = Ter_GridCell((pFrom[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    nCell[1] = Ter_GridCell((pFrom[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
    nEndX = Ter_GridCell((pTo[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    nEndZ = Ter_GridCell((pTo[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
    vDelta[0] = pTo[0] - pFrom[0];
    vDelta[1] = pTo[2] - pFrom[2];
    vStart[0] = pFrom[0];
    vStart[1] = pFrom[2];
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
    vEdge[nMajor] = pCourse->fGridCellSize[nMajor] * nCell[nMajor] + pCourse->fGridOrigin[nMajor];
    vEdge[nMinor] = pCourse->fGridCellSize[nMinor] * nCell[nMinor] + pCourse->fGridOrigin[nMinor];
    if (nStepMajor > 0) {
        vEdge[nMajor] += pCourse->fGridCellSize[nMajor];
    }
    if (nStepMinor > 0) {
        vEdge[nMinor] += pCourse->fGridCellSize[nMinor];
    }
    Ter_Subtract3(pTo, pFrom, vDir);
    LLMath_Normalize3(vDir, vDir);
    LLMath_CopyVec(pFrom, vPos);
    for (;;) {
        LLMath_CopyVec(vPos, vPrev);
        if (nCell[0] == nEndX && nCell[1] == nEndZ) {
            LLMath_CopyVec(pTo, vPos);
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
                vPos[AXIS3(nMinor)] = vStart[nMinor] + fRun * vDir[AXIS3(nMinor)] / vDir[AXIS3(nMajor)];
                vPos[1] = pFrom[1] + vDir[1] * fRun / vDir[AXIS3(nMajor)];
            } else {
                fRun = vEdge[nMinor] - vStart[nMinor];
                vPos[AXIS3(nMinor)] = vEdge[nMinor];
                nLast = nMinor;
                vPos[AXIS3(nMajor)] = vStart[nMajor] + fRun * vDir[AXIS3(nMajor)] / vDir[AXIS3(nMinor)];
                vPos[1] = pFrom[1] + vDir[1] * fRun / vDir[AXIS3(nMinor)];
            }
        }
        fLen = LLMath_DistanceBetween3(vPos, vPrev);
        Ter_MarkObjectsNearLine(pCourse, vPrev, vDir, nCell[0], nCell[1], fLen);
        if (Ter_CheckForWorldCollisionOneGrid(pCourse, nCell[0], nCell[1], vPrev, vPos, vDir, fLen, vHit,
                                              vNormal, &pSurface, &pObj,
                        pbFlags)) {
            fDist = LLMath_SquareDistanceBetween3(pFrom, vHit);
            if (fDist < fBest) {
                fBest = fDist;
                LLMath_CopyVec(vHit, pHit);
                LLMath_CopyVec(vNormal, pNormal);
                *ppSurface = pSurface;
                *ppObj = pObj;
            }
        }
        if (nCell[0] == nEndX && nCell[1] == nEndZ) break;
        if (nLast == nMajor) {
            nCell[nMajor] += nStepMajor;
            vEdge[nMajor] += pCourse->fGridCellSize[nMajor] * nStepMajor;
        } else {
            nCell[nMinor] += nStepMinor;
            vEdge[nMinor] += pCourse->fGridCellSize[nMinor] * nStepMinor;
        }
    }
    if (fBest != 1000000.0f && LLMath_DotProduct3(vDir, pNormal) > 0.0f) {
        Ter_Negate(pNormal, pNormal);
    }
    return fBest != 1000000.0f;
}

// TW06: bool Ter_CheckForWorldCollisionOneGrid(TGD_TerrainInfo*, s32, s32, f32*, f32*, f32*, f32,
// f32[4]*, f32[4]*, TGD_MaterialInfo**, TGD_ObjectInstanceInfo**, u8*). The nearest triangle of
// grid cell (nX, nZ) that the line from pFrom along pDir meets before fMax (pTo is the line's end,
// for the cell's height test): the point, normal, surface, the object it belongs to (NULL for the
// ground) and, when pbFlags is given, the triangle's flag bits. Only the objects marked in
// gTerObjectMarks are tested.
u8 Ter_CheckForWorldCollisionOneGrid(TGD_TerrainInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo,
                                     f32* pDir,
                                     f32 fMax, f32* pHit,
               f32* pNormal, TGD_MaterialInfo** ppSurface, TerObject** ppObj, u8* pbFlags) {
    f32 vHit[4];
    f32 vNormal[4];
    f32 fT;
    u8 bHit = 0;
    u32 uPinSet = 1 << Game_CurrentPinSet();
    TGD_Cell* pCell;
    TGD_PolygonReference* pRef;
    f32 (*pVert)[3];
    u8* pFlags;
    int i;
    int j;
    f32 fLow;
    f32 fHigh;

    if (nX >= 0 && nX < pCourse->nGridWidth && nZ >= 0 && nZ < pCourse->nGridLength) {
        fLow = pFrom[1];
        fHigh = pFrom[1];
        pCell = &pCourse->pGrid[nX + nZ * pCourse->nGridWidth];
        if (pTo[1] > pFrom[1]) {
            fHigh = pTo[1];
        }
        if (pTo[1] < fLow) {
            fLow = pTo[1];
        }
        if (fLow > (f32)pCell->nMaxHeight || fHigh < (f32)pCell->nMinHeight) return 0;
        pRef = &pCourse->pPolyRefs[pCell->uRefs >> CELL_REFS_FIRST_SHIFT];
        for (i = (pCell->uRefs & CELL_REFS_COUNT_MASK) - 1; i >= 0; i--) {
            if (pRef->uFlags != 0
                && (!(pRef->uFlags & uPinSet)
                    || ((pRef->uFlags & POLYREF_NOT_IN_SPLIT_SCREEN) && gSession.nSplitScreen))) {
                pRef++;
            } else if (gTerObjectMarks[pRef->nObject] == 0) {
                pRef++;
            } else {
                pVert = &pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
                pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
                for (j = pRef->nTris - 1; j >= 0; j--) {
                    if ((pFlags[2] & TER_TRI_KICK_MASK)
                        && Ter_LineTriangleIntersection(pFrom, pDir, fMax, pVert, &fT, vHit, vNormal)) {
                        fMax = fT;
                        LLMath_CopyVec(vHit, pHit);
                        LLMath_CopyVec(vNormal, pNormal);
                        if (pbFlags != NULL) {
                            *pbFlags = pFlags[2] & TER_TRI_KICK_MASK;
                        }
                        *ppSurface = &gSurfaceTypes[pRef->nSurface];
                        if (pRef->nObject != 0) {
                            *ppObj = &pCourse->pObjects[pRef->nObject];
                        } else {
                            *ppObj = NULL;
                        }
                        bHit = 1;
                    }
                    pVert++;
                    pFlags++;
                }
                pRef++;
            }
        }
        return bHit;
    }
    return 0;
}

// TW06: bool Ter_CheckForSolidWorldCollision(...), the same parameters without the flags. As
// Ter_CheckForWorldCollision, but a ball passes through branches and leaves
// (Ter_CheckForSolidWorldCollisionOneGrid).
u8 Ter_CheckForSolidWorldCollision(TGD_TerrainInfo* pCourse, int nPlayer, f32* pFrom, f32* pTo, f32* pHit,
                                   f32* pNormal,
               TGD_MaterialInfo** ppSurface, TerObject** ppObj) {
    int nEndX;
    int nEndZ;
    f32 vHit[4];
    f32 vNormal[4];
    f32 vDir[4];
    f32 vPrev[4];
    f32 vPos[4];
    int nCell[2];
    f32 vDelta[2];
    f32 vStart[2];
    f32 vEdge[2];
    TGD_MaterialInfo* pSurface;
    TerObject* pObj;
    f32 fBest = 1000000.0f;
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

    if (pFrom[0] == pTo[0] && pFrom[1] == pTo[1] && pFrom[2] == pTo[2]) return 0;
    if (Ter_CheckForPinCollision(pCourse, nPlayer, pFrom, pTo, pHit, pNormal, ppSurface, ppObj)) {
        fBest = LLMath_SquareDistanceBetween3(pFrom, pHit);
    }
    nCell[0] = Ter_GridCell((pFrom[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    nCell[1] = Ter_GridCell((pFrom[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
    nEndX = Ter_GridCell((pTo[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    nEndZ = Ter_GridCell((pTo[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
    vDelta[0] = pTo[0] - pFrom[0];
    vDelta[1] = pTo[2] - pFrom[2];
    vStart[0] = pFrom[0];
    vStart[1] = pFrom[2];
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
    vEdge[nMajor] = pCourse->fGridCellSize[nMajor] * nCell[nMajor] + pCourse->fGridOrigin[nMajor];
    vEdge[nMinor] = pCourse->fGridCellSize[nMinor] * nCell[nMinor] + pCourse->fGridOrigin[nMinor];
    if (nStepMajor > 0) {
        vEdge[nMajor] += pCourse->fGridCellSize[nMajor];
    }
    if (nStepMinor > 0) {
        vEdge[nMinor] += pCourse->fGridCellSize[nMinor];
    }
    Ter_Subtract3(pTo, pFrom, vDir);
    LLMath_Normalize3(vDir, vDir);
    LLMath_CopyVec(pFrom, vPos);
    for (;;) {
        LLMath_CopyVec(vPos, vPrev);
        if (nCell[0] == nEndX && nCell[1] == nEndZ) {
            LLMath_CopyVec(pTo, vPos);
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
                vPos[AXIS3(nMinor)] = vStart[nMinor] + fRun * vDir[AXIS3(nMinor)] / vDir[AXIS3(nMajor)];
                vPos[1] = pFrom[1] + vDir[1] * fRun / vDir[AXIS3(nMajor)];
            } else {
                fRun = vEdge[nMinor] - vStart[nMinor];
                vPos[AXIS3(nMinor)] = vEdge[nMinor];
                nLast = nMinor;
                vPos[AXIS3(nMajor)] = vStart[nMajor] + fRun * vDir[AXIS3(nMajor)] / vDir[AXIS3(nMinor)];
                vPos[1] = pFrom[1] + vDir[1] * fRun / vDir[AXIS3(nMinor)];
            }
        }
        fLen = LLMath_DistanceBetween3(vPos, vPrev);
        Ter_MarkObjectsNearLine(pCourse, vPrev, vDir, nCell[0], nCell[1], fLen);
        if (Ter_CheckForSolidWorldCollisionOneGrid(pCourse, nCell[0], nCell[1], vPrev, vPos, vDir, fLen,
                                                   vHit, vNormal, &pSurface,
                        &pObj)) {
            fDist = LLMath_SquareDistanceBetween3(pFrom, vHit);
            if (fDist < fBest) {
                fBest = fDist;
                LLMath_CopyVec(vHit, pHit);
                LLMath_CopyVec(vNormal, pNormal);
                *ppSurface = pSurface;
                *ppObj = pObj;
            }
        }
        if (nCell[0] == nEndX && nCell[1] == nEndZ) break;
        if (nLast == nMajor) {
            nCell[nMajor] += nStepMajor;
            vEdge[nMajor] += pCourse->fGridCellSize[nMajor] * nStepMajor;
        } else {
            nCell[nMinor] += nStepMinor;
            vEdge[nMinor] += pCourse->fGridCellSize[nMinor] * nStepMinor;
        }
    }
    if (fBest != 1000000.0f && LLMath_DotProduct3(vDir, pNormal) > 0.0f) {
        Ter_Negate(pNormal, pNormal);
    }
    return fBest != 1000000.0f;
}

// TW06: bool Ter_CheckForSolidWorldCollisionOneGrid(...), the same parameters without the flags.
// As Ter_CheckForWorldCollisionOneGrid, but surfaces with a negative bounce (branches and leaves,
// which a ball passes through) do not count.
u8 Ter_CheckForSolidWorldCollisionOneGrid(TGD_TerrainInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo,
                                          f32* pDir, f32 fMax, f32* pHit,
               f32* pNormal, TGD_MaterialInfo** ppSurface, TerObject** ppObj) {
    f32 vHit[4];
    f32 vNormal[4];
    f32 fT;
    u8 bHit = 0;
    u32 uPinSet = 1 << Game_CurrentPinSet();
    TGD_Cell* pCell;
    TGD_PolygonReference* pRef;
    f32 (*pVert)[3];
    u8* pFlags;
    int i;
    int j;
    f32 fLow;
    f32 fHigh;

    if (nX >= 0 && nX < pCourse->nGridWidth && nZ >= 0 && nZ < pCourse->nGridLength) {
        fLow = pFrom[1];
        fHigh = pFrom[1];
        pCell = &pCourse->pGrid[nX + nZ * pCourse->nGridWidth];
        if (pTo[1] > pFrom[1]) {
            fHigh = pTo[1];
        }
        if (pTo[1] < fLow) {
            fLow = pTo[1];
        }
        if (fLow > (f32)pCell->nMaxHeight || fHigh < (f32)pCell->nMinHeight) return 0;
        pRef = &pCourse->pPolyRefs[pCell->uRefs >> CELL_REFS_FIRST_SHIFT];
        for (i = (pCell->uRefs & CELL_REFS_COUNT_MASK) - 1; i >= 0; i--) {
            if (pRef->uFlags != 0
                && (!(pRef->uFlags & uPinSet)
                    || ((pRef->uFlags & POLYREF_NOT_IN_SPLIT_SCREEN) && gSession.nSplitScreen))) {
                pRef++;
            } else if (gTerObjectMarks[pRef->nObject] == 0) {
                pRef++;
            } else if (gSurfaceTypes[pRef->nSurface].f0C < 0.0f) {
                pRef++;
            } else {
                pVert = &pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
                pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
                for (j = pRef->nTris - 1; j >= 0; j--) {
                    if ((pFlags[2] & TER_TRI_KICK_MASK)
                        && Ter_LineTriangleIntersection(pFrom, pDir, fMax, pVert, &fT, vHit, vNormal)) {
                        fMax = fT;
                        LLMath_CopyVec(vHit, pHit);
                        LLMath_CopyVec(vNormal, pNormal);
                        *ppSurface = &gSurfaceTypes[pRef->nSurface];
                        if (pRef->nObject != 0) {
                            *ppObj = &pCourse->pObjects[pRef->nObject];
                        } else {
                            *ppObj = NULL;
                        }
                        bHit = 1;
                    }
                    pVert++;
                    pFlags++;
                }
                pRef++;
            }
        }
        return bHit;
    }
    return 0;
}
// TW06: bool Ter_CheckForGroundCollision(TGD_TerrainInfo*, f32*, f32*, f32[4]*, f32[4]*,
// TGD_MaterialInfo**, TGD_ObjectInstanceInfo**). The first ground the line from pFrom to pTo hits
// (Ter_CheckForGroundCollisionOneGrid per cell); no pin and no objects.
u8 Ter_CheckForGroundCollision(TGD_TerrainInfo* pCourse, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal,
                               TGD_MaterialInfo** ppSurface, TerObject** ppObj) {
    int nEndX;
    int nEndZ;
    f32 vHit[4];
    f32 vNormal[4];
    f32 vDir[4];
    f32 vPrev[4];
    f32 vPos[4];
    int nCell[2];
    f32 vDelta[2];
    f32 vStart[2];
    f32 vEdge[2];
    TGD_MaterialInfo* pSurface;
    TerObject* pObj;
    f32 fBest = 1000000.0f;
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

    if (pFrom[0] == pTo[0] && pFrom[1] == pTo[1] && pFrom[2] == pTo[2]) return 0;
    nCell[0] = Ter_GridCell((pFrom[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    nCell[1] = Ter_GridCell((pFrom[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
    nEndX = Ter_GridCell((pTo[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    nEndZ = Ter_GridCell((pTo[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
    vDelta[0] = pTo[0] - pFrom[0];
    vDelta[1] = pTo[2] - pFrom[2];
    vStart[0] = pFrom[0];
    vStart[1] = pFrom[2];
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
    vEdge[nMajor] = pCourse->fGridCellSize[nMajor] * nCell[nMajor] + pCourse->fGridOrigin[nMajor];
    vEdge[nMinor] = pCourse->fGridCellSize[nMinor] * nCell[nMinor] + pCourse->fGridOrigin[nMinor];
    if (nStepMajor > 0) {
        vEdge[nMajor] += pCourse->fGridCellSize[nMajor];
    }
    if (nStepMinor > 0) {
        vEdge[nMinor] += pCourse->fGridCellSize[nMinor];
    }
    Ter_Subtract3(pTo, pFrom, vDir);
    LLMath_Normalize3(vDir, vDir);
    LLMath_CopyVec(pFrom, vPos);
    for (;;) {
        LLMath_CopyVec(vPos, vPrev);
        if (nCell[0] == nEndX && nCell[1] == nEndZ) {
            LLMath_CopyVec(pTo, vPos);
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
                vPos[AXIS3(nMinor)] = vStart[nMinor] + fRun * vDir[AXIS3(nMinor)] / vDir[AXIS3(nMajor)];
                vPos[1] = pFrom[1] + vDir[1] * fRun / vDir[AXIS3(nMajor)];
            } else {
                fRun = vEdge[nMinor] - vStart[nMinor];
                vPos[AXIS3(nMinor)] = vEdge[nMinor];
                nLast = nMinor;
                vPos[AXIS3(nMajor)] = vStart[nMajor] + fRun * vDir[AXIS3(nMajor)] / vDir[AXIS3(nMinor)];
                vPos[1] = pFrom[1] + vDir[1] * fRun / vDir[AXIS3(nMinor)];
            }
        }
        fLen = LLMath_DistanceBetween3(vPos, vPrev);
        if (Ter_CheckForGroundCollisionOneGrid(pCourse, nCell[0], nCell[1], vPrev, vPos, vDir, fLen, vHit,
                                               vNormal, &pSurface,
                        &pObj)) {
            fDist = LLMath_SquareDistanceBetween3(pFrom, vHit);
            if (fDist < fBest) {
                fBest = fDist;
                LLMath_CopyVec(vHit, pHit);
                LLMath_CopyVec(vNormal, pNormal);
                *ppSurface = pSurface;
                *ppObj = pObj;
            }
        }
        if (nCell[0] == nEndX && nCell[1] == nEndZ) break;
        if (nLast == nMajor) {
            nCell[nMajor] += nStepMajor;
            vEdge[nMajor] += pCourse->fGridCellSize[nMajor] * nStepMajor;
        } else {
            nCell[nMinor] += nStepMinor;
            vEdge[nMinor] += pCourse->fGridCellSize[nMinor] * nStepMinor;
        }
    }
    if (fBest != 1000000.0f && LLMath_DotProduct3(vDir, pNormal) > 0.0f) {
        Ter_Negate(pNormal, pNormal);
    }
    return fBest != 1000000.0f;
}

// TW06: bool Ter_CheckForGroundCollisionOneGrid(...). As Ter_CheckForSolidWorldCollisionOneGrid,
// the ground only (no objects).
u8 Ter_CheckForGroundCollisionOneGrid(TGD_TerrainInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo,
                                      f32* pDir,
                                      f32 fMax, f32* pHit,
               f32* pNormal, TGD_MaterialInfo** ppSurface, TerObject** ppObj) {
    f32 vHit[4];
    f32 vNormal[4];
    f32 fT;
    u8 bHit = 0;
    u32 uPinSet = 1 << Game_CurrentPinSet();
    TGD_Cell* pCell;
    TGD_PolygonReference* pRef;
    f32 (*pVert)[3];
    u8* pFlags;
    int i;
    int j;
    f32 fLow;
    f32 fHigh;

    if (nX >= 0 && nX < pCourse->nGridWidth && nZ >= 0 && nZ < pCourse->nGridLength) {
        fLow = pFrom[1];
        fHigh = pFrom[1];
        pCell = &pCourse->pGrid[nX + nZ * pCourse->nGridWidth];
        if (pTo[1] > pFrom[1]) {
            fHigh = pTo[1];
        }
        if (pTo[1] < fLow) {
            fLow = pTo[1];
        }
        if (fLow > (f32)pCell->nMaxHeight || fHigh < (f32)pCell->nMinHeight) return 0;
        pRef = &pCourse->pPolyRefs[pCell->uRefs >> CELL_REFS_FIRST_SHIFT];
        for (i = (pCell->uRefs & CELL_REFS_COUNT_MASK) - 1; i >= 0; i--) {
            if (pRef->nObject != 0) {
                pRef++;
            } else if (pRef->uFlags != 0
                       && (!(pRef->uFlags & uPinSet)
                           || ((pRef->uFlags & POLYREF_NOT_IN_SPLIT_SCREEN) && gSession.nSplitScreen))) {
                pRef++;
            } else {
                pVert = &pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
                pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
                for (j = pRef->nTris - 1; j >= 0; j--) {
                    if ((pFlags[2] & TER_TRI_KICK_MASK)
                        && Ter_LineTriangleIntersection(pFrom, pDir, fMax, pVert, &fT, vHit, vNormal)) {
                        fMax = fT;
                        LLMath_CopyVec(vHit, pHit);
                        LLMath_CopyVec(vNormal, pNormal);
                        *ppSurface = &gSurfaceTypes[pRef->nSurface];
                        if (pRef->nObject != 0) {
                            *ppObj = &pCourse->pObjects[pRef->nObject];
                        } else {
                            *ppObj = NULL;
                        }
                        bHit = 1;
                    }
                    pVert++;
                    pFlags++;
                }
                pRef++;
            }
        }
        return bHit;
    }
    return 0;
}
// TW06: bool Ter_CheckForObjectCollision(...), the same parameters. The first object the line hits
// (Ter_CheckForObjectCollisionOneGrid per cell).
u8 Ter_CheckForObjectCollision(TGD_TerrainInfo* pCourse, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal,
                               TGD_MaterialInfo** ppSurface,
               TerObject** ppObj) {
    int nEndX;
    int nEndZ;
    f32 vHit[4];
    f32 vNormal[4];
    f32 vDir[4];
    f32 vPrev[4];
    f32 vPos[4];
    int nCell[2];
    f32 vDelta[2];
    f32 vStart[2];
    f32 vEdge[2];
    TGD_MaterialInfo* pSurface;
    TerObject* pObj;
    f32 fBest = 1000000.0f;
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

    if (pFrom[0] == pTo[0] && pFrom[1] == pTo[1] && pFrom[2] == pTo[2]) return 0;
    nCell[0] = Ter_GridCell((pFrom[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    nCell[1] = Ter_GridCell((pFrom[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
    nEndX = Ter_GridCell((pTo[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    nEndZ = Ter_GridCell((pTo[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
    vDelta[0] = pTo[0] - pFrom[0];
    vDelta[1] = pTo[2] - pFrom[2];
    vStart[0] = pFrom[0];
    vStart[1] = pFrom[2];
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
    vEdge[nMajor] = pCourse->fGridCellSize[nMajor] * nCell[nMajor] + pCourse->fGridOrigin[nMajor];
    vEdge[nMinor] = pCourse->fGridCellSize[nMinor] * nCell[nMinor] + pCourse->fGridOrigin[nMinor];
    if (nStepMajor > 0) {
        vEdge[nMajor] += pCourse->fGridCellSize[nMajor];
    }
    if (nStepMinor > 0) {
        vEdge[nMinor] += pCourse->fGridCellSize[nMinor];
    }
    Ter_Subtract3(pTo, pFrom, vDir);
    LLMath_Normalize3(vDir, vDir);
    LLMath_CopyVec(pFrom, vPos);
    for (;;) {
        LLMath_CopyVec(vPos, vPrev);
        if (nCell[0] == nEndX && nCell[1] == nEndZ) {
            LLMath_CopyVec(pTo, vPos);
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
                vPos[AXIS3(nMinor)] = vStart[nMinor] + fRun * vDir[AXIS3(nMinor)] / vDir[AXIS3(nMajor)];
                vPos[1] = pFrom[1] + vDir[1] * fRun / vDir[AXIS3(nMajor)];
            } else {
                fRun = vEdge[nMinor] - vStart[nMinor];
                vPos[AXIS3(nMinor)] = vEdge[nMinor];
                nLast = nMinor;
                vPos[AXIS3(nMajor)] = vStart[nMajor] + fRun * vDir[AXIS3(nMajor)] / vDir[AXIS3(nMinor)];
                vPos[1] = pFrom[1] + vDir[1] * fRun / vDir[AXIS3(nMinor)];
            }
        }
        fLen = LLMath_DistanceBetween3(vPos, vPrev);
        if (Ter_CheckForObjectCollisionOneGrid(pCourse, nCell[0], nCell[1], vPrev, vPos, vDir, fLen, vHit,
                                               vNormal, &pSurface,
                        &pObj)) {
            fDist = LLMath_SquareDistanceBetween3(pFrom, vHit);
            if (fDist < fBest) {
                fBest = fDist;
                LLMath_CopyVec(vHit, pHit);
                LLMath_CopyVec(vNormal, pNormal);
                *ppSurface = pSurface;
                *ppObj = pObj;
            }
        }
        if (nCell[0] == nEndX && nCell[1] == nEndZ) break;
        if (nLast == nMajor) {
            nCell[nMajor] += nStepMajor;
            vEdge[nMajor] += pCourse->fGridCellSize[nMajor] * nStepMajor;
        } else {
            nCell[nMinor] += nStepMinor;
            vEdge[nMinor] += pCourse->fGridCellSize[nMinor] * nStepMinor;
        }
    }
    if (fBest != 1000000.0f && LLMath_DotProduct3(vDir, pNormal) > 0.0f) {
        Ter_Negate(pNormal, pNormal);
    }
    return fBest != 1000000.0f;
}

// TW06: bool Ter_CheckForObjectCollisionOneGrid(...). As Ter_CheckForSolidWorldCollisionOneGrid,
// objects only, plus ground whose surface has flag 0x80.
u8 Ter_CheckForObjectCollisionOneGrid(TGD_TerrainInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo,
                                      f32* pDir,
                                      f32 fMax, f32* pHit,
               f32* pNormal, TGD_MaterialInfo** ppSurface, TerObject** ppObj) {
    f32 vHit[4];
    f32 vNormal[4];
    f32 fT;
    u8 bHit = 0;
    u32 uPinSet = 1 << Game_CurrentPinSet();
    TGD_Cell* pCell;
    TGD_PolygonReference* pRef;
    f32 (*pVert)[3];
    u8* pFlags;
    int i;
    int j;
    f32 fLow;
    f32 fHigh;

    if (nX >= 0 && nX < pCourse->nGridWidth && nZ >= 0 && nZ < pCourse->nGridLength) {
        fLow = pFrom[1];
        fHigh = pFrom[1];
        pCell = &pCourse->pGrid[nX + nZ * pCourse->nGridWidth];
        if (pTo[1] > pFrom[1]) {
            fHigh = pTo[1];
        }
        if (pTo[1] < fLow) {
            fLow = pTo[1];
        }
        if (fLow > (f32)pCell->nMaxHeight || fHigh < (f32)pCell->nMinHeight) return 0;
        pRef = &pCourse->pPolyRefs[pCell->uRefs >> CELL_REFS_FIRST_SHIFT];
        for (i = (pCell->uRefs & CELL_REFS_COUNT_MASK) - 1; i >= 0; i--) {
            if (pRef->nObject == 0 && !(gSurfaceTypes[pRef->nSurface].uFlags & 0x80)) {
                pRef++;
            } else if (pRef->uFlags != 0
                       && (!(pRef->uFlags & uPinSet)
                           || ((pRef->uFlags & POLYREF_NOT_IN_SPLIT_SCREEN) && gSession.nSplitScreen))) {
                pRef++;
            } else {
                pVert = &pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
                pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
                for (j = pRef->nTris - 1; j >= 0; j--) {
                    if ((pFlags[2] & TER_TRI_KICK_MASK)
                        && Ter_LineTriangleIntersection(pFrom, pDir, fMax, pVert, &fT, vHit, vNormal)) {
                        fMax = fT;
                        LLMath_CopyVec(vHit, pHit);
                        LLMath_CopyVec(vNormal, pNormal);
                        *ppSurface = &gSurfaceTypes[pRef->nSurface];
                        if (pRef->nObject != 0) {
                            *ppObj = &pCourse->pObjects[pRef->nObject];
                        } else {
                            *ppObj = NULL;
                        }
                        bHit = 1;
                    }
                    pVert++;
                    pFlags++;
                }
                pRef++;
            }
        }
        return bHit;
    }
    return 0;
}
// Mark the highest and lowest corner of every ground triangle whose flags are not 0 in those
// flags (bits 4-5 and 6-7), using bit 3 to do each triangle once, then clear bit 3 again. TW06 has
// the two halves as Ter_ComputeHighestPointInEveryTriangle and Ter_ClearVertexProcessedBit.
void Ter_ComputeHighestPointInEveryTriangle(TGD_TerrainInfo* pCourse) {
    u8 uFlags;
    f32 (*pVert)[3];
    u32 i;
    u32 j;
    u8* pFlags;
    TGD_PolygonReference* pRef;
    u8 nHigh;
    u8 nLow;

    pRef = pCourse->pPolyRefs;
    i = pCourse->nPolyRefs;
    while (i != 0) {
        pVert = &pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
        pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
        j = pRef->nTris;
        while (j != 0) {
            uFlags = pFlags[2];
            if (!(uFlags & TER_TRI_DONE) && uFlags != 0) {
                if (pVert[0][1] > pVert[1][1]) {
                    if (pVert[0][1] > pVert[2][1]) {
                        nHigh = 0;
                    } else {
                        nHigh = 2;
                    }
                } else if (pVert[1][1] > pVert[2][1]) {
                    nHigh = 1;
                } else {
                    nHigh = 2;
                }
                if (pVert[0][1] < pVert[1][1]) {
                    if (pVert[0][1] < pVert[2][1]) {
                        nLow = 0;
                    } else {
                        nLow = 2;
                    }
                } else if (pVert[1][1] < pVert[2][1]) {
                    nLow = 1;
                } else {
                    nLow = 2;
                }
                pFlags[2] = uFlags | (nHigh << TER_TRI_HIGHEST_SHIFT) | (nLow << TER_TRI_LOWEST_SHIFT)
                        | TER_TRI_DONE;
            }
            j--;
            pFlags++;
            pVert++;
        }
        i--;
        pRef++;
    }
    pRef = pCourse->pPolyRefs;
    i = pCourse->nPolyRefs;
    while (i != 0) {
        // fake match: pVert is set and stepped but not read in this pass (as in the first pass);
        // without it CodeWarrior adds the flag array to the vertex index's low half first.
        pVert = &pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
        pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
        j = pRef->nTris;
        while (j != 0) {
            pFlags[2] &= (u8)~TER_TRI_DONE;
            j--;
            pFlags++;
            pVert++;
        }
        i--;
        pRef++;
    }
}

// The difference a - b of two three-float vectors, into pOut.
#ifdef __MWERKS__
asm void Ter_Subtract3(register f32* pA, register f32* pB, register f32* pOut) {
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
void Ter_Subtract3(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// A three-float vector negated, into pOut.
#ifdef __MWERKS__
asm void Ter_Negate3(register f32* pA, register f32* pOut) {
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
void Ter_Negate3(f32* pA, f32* pOut) {
    pOut[0] = -pA[0];
    pOut[1] = -pA[1];
    pOut[2] = -pA[2];
}
#endif

// A four-float vector negated, into pOut.
#ifdef __MWERKS__
asm void Ter_Negate(register f32* pA, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 0, 0
    ps_neg f0, f0
    ps_neg f1, f1
    psq_st f0, 0(pOut), 0, 0
    psq_st f1, 8(pOut), 0, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void Ter_Negate(f32* pA, f32* pOut) {
    pOut[0] = -pA[0];
    pOut[1] = -pA[1];
    pOut[2] = -pA[2];
    pOut[3] = -pA[3];
}
#endif

// TW06: Ter_GetBarycentricCoords (an inline in goterrainutils.h there). The weights of a point
// against a triangle's three corners, in the x-z plane.
void Ter_GetBarycentricCoords(f32 (*pTri)[3], f32* pPos, f32* pA, f32* pB, f32* pC) {
    f32 fX;
    f32 fZ;
    f32 fInv;

    fX = pPos[0];
    fZ = pPos[2];
    fInv = 1.0f / ((pTri[1][0] - pTri[0][0]) * (pTri[2][2] - pTri[0][2])
                   - (pTri[2][0] - pTri[0][0]) * (pTri[1][2] - pTri[0][2]));
    *pA = fInv * ((pTri[1][0] - fX) * (pTri[2][2] - fZ) - (pTri[2][0] - fX) * (pTri[1][2] - fZ));
    *pB = fInv * ((pTri[2][0] - fX) * (pTri[0][2] - fZ) - (pTri[0][0] - fX) * (pTri[2][2] - fZ));
    *pC = fInv * ((pTri[0][0] - fX) * (pTri[1][2] - fZ) - (pTri[1][0] - fX) * (pTri[0][2] - fZ));
}

// TW06: bool Ter_PointInTriangleXZpY(f32*, f32*, f32*, f32, f32, f32*). Whether (x, z) lies inside
// the triangle abc seen from above, either way round; a flat (edge-on) triangle holds nothing.
u8 Ter_PointInTriangleXZpY(f32* pA, f32* pB, f32* pC, f32 fX, f32 fZ) {
    f32 fABx = pB[0] - pA[0];
    f32 fABz = pB[2] - pA[2];
    f32 fBCx = pC[0] - pB[0];
    f32 fBCz = pC[2] - pB[2];
    f32 fTurn = fABx * fBCz - fABz * fBCx;
    f32 fSide;

    if (fTurn == 0.0f) return 0;
    fSide = fABx * (fZ - pA[2]) - fABz * (fX - pA[0]);
    if (fTurn < 0.0f) {
        if (fSide > 0.0f) return 0;
        if (fBCx * (fZ - pB[2]) - fBCz * (fX - pB[0]) > 0.0f) return 0;
        if ((pA[0] - pC[0]) * (fZ - pC[2]) - (pA[2] - pC[2]) * (fX - pC[0]) > 0.0f) return 0;
    } else {
        if (fSide < 0.0f) return 0;
        if (fBCx * (fZ - pB[2]) - fBCz * (fX - pB[0]) < 0.0f) return 0;
        if ((pA[0] - pC[0]) * (fZ - pC[2]) - (pA[2] - pC[2]) * (fX - pC[0]) < 0.0f) return 0;
    }
    return 1;
}

// A flag byte of a course object's model (GoTerrain.c's Ter_GetMeshFlags reads the same bytes).
// Ter_CheckObjectAndHazardObstruction finds the model with Ter_GetObjectListModel (from the object's nPatch
// and nObjList) and tests UOBJMESH_F0_NO_FADE in byte n = 0.
int TerCollision_GetMeshFlags(UObjMesh* pModel, int n) {
    return pModel->pInfo->aFlags[n];
}

// TW06: s32 MaterialTypes::getMaterialID(const TGD_MaterialInfo*). A surface's row in
// gSurfaceTypes, or -1.
s32 MaterialTypes_getMaterialID(TGD_MaterialInfo* pSurface) {
    s32 n;

    if (pSurface == NULL) return -1;
    n = ((u8*)pSurface - (u8*)gSurfaceTypes) / sizeof(TGD_MaterialInfo);
    if (n < 0 || n >= NUM_SURFACE_TYPES) return -1;
    return n;
}
