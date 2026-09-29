// GoTerrainCollision.c (TW06's goterraincollision.c; TW07 splits it into GoTerrainCollision.c,
// GoTerrainCollision_Headgate.c and GoTerrainUtils.c): the course's ground as collision data
// (CourseInfo, TW06's TGD_TerrainInfo): the free-drop and out-of-bounds networks, the drop
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
// port: the course file keeps each list's offset from its start in the pointer field itself, and
//       loading turns it into the pointer in place; with 64-bit pointers the file needs its own
//       layout.
#define TER_RELOCATE(pCourse, field) ((pCourse)->field = (void*)((u8*)(pCourse) + BE32(&(pCourse)->field)))

// .bss and .sbss in reverse address order (CodeWarrior lays them out backwards).
u8        gTerObjectMarks[MAX_OBJECTS];         // 1: a course object near the line being tested
                                                  // (Ter_MarkObjectsNearLine; entry 0 always 1)
TNetwork* gTerOOBNetworks[MAX_OOB_NETWORKS];      // the hole's out-of-bounds outlines
TNetwork* gTerFreeDropNetworks[MAX_FREE_DROP_NETWORKS];    // the hole's free-drop outlines
TerBox    gTerCupGeometryBounds[NUM_CUP_POSITIONS];        // per pin position: its cup geometry's box
s32       gTerNumOOBNetworks;                     // outlines in gTerOOBNetworks
s32       gTerNumFreeDropNetworks;                // outlines in gTerFreeDropNetworks
u8        gTerUse3DCupGeometry;                   // 1: the course has cup geometry (Ter_InitTGD)

void  Ter_FreeDropNetworkLoadCallback(TNetwork* pNet);
u8    Ter_LieIsPreferred(u32 nClass);
void  Ter_OOBNetworkLoadCallback(TNetwork* pNet);
void  Ter_Subtract3(f32* pA, f32* pB, f32* pOut);           // a - b (paired-single assembly)
void  Ter_Negate3(f32* pSrc, f32* pDst);                  // negate (paired-single assembly)
void  Ter_Negate(f32* pSrc, f32* pDst);                  // negate, four floats (paired-single assembly)

// Every floor in this file goes through an inline (probably EA's wrapper around floorf):
// Ter_GetTerrainLayers matches only that way, and every other function matches either way.
static inline f32 Ter_Floor(f32 x) {
    return Math_Floor(x);
}

// The grid cell a coordinate falls in (it may be outside the grid). The four line walkers go
// through this helper; the point lookups write the cast out (each matches only its own way).
static inline int Ter_GridCell(f32 fCells) {
    return (int)Ter_Floor(fCells);
}

f32   Ter_GetHighestGroundTriangle(CourseInfo* pCourse, f32* pPos, TerCell** ppCell, TerPolyRef** ppRef,
                                   f32 (**ppTri)[3],
                  s32* pTri);
void  Ter_ComputeHighestPointInEveryTriangle(CourseInfo* pCourse);
f32   Ter_CalcLowestPlayableWorldHeight(CourseInfo* pCourse);
f32   Ter_GetLowestGroundTriangle(CourseInfo* pCourse, f32* pPos, TerCell** ppCell, TerPolyRef** ppRef,
                                  f32 (**ppTri)[3],
                  s32* pTri);
f32   Ter_GetCoveringGroundTriangle(CourseInfo* pCourse, f32* pPos, TerCell** ppCell, TerPolyRef** ppRef,
                                    f32 (**ppTri)[3],
                  s32* pTri);
void  Ter_GetSupportingAndCoveringGroundTriangles(CourseInfo* pCourse, f32* pPos, TerPolyRef** ppRefLow,
                                                  f32* pLow, f32 (**ppTriLow)[3],
                  TerPolyRef** ppRefHigh, f32* pHigh, f32 (**ppTriHigh)[3]);
u8    Ter_CheckForGroundCollision(CourseInfo* pCourse, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal,
                                  SurfaceType** ppSurface, TerObject** ppObj);
u8    Ter_CheckForWorldCollisionOneGrid(CourseInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo, f32* pDir,
                                        f32 fMax, f32* pHit,
                  f32* pNormal, SurfaceType** ppSurface, TerObject** ppObj, u8* pbFlags);
u8    Ter_CheckForSolidWorldCollisionOneGrid(CourseInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo,
                                             f32* pDir, f32 fMax, f32* pHit,
                  f32* pNormal, SurfaceType** ppSurface, TerObject** ppObj);
u8    Ter_CheckForGroundCollisionOneGrid(CourseInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo,
                                         f32* pDir, f32 fMax, f32* pHit,
                  f32* pNormal, SurfaceType** ppSurface, TerObject** ppObj);
u8    Ter_CheckForObjectCollisionOneGrid(CourseInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo,
                                         f32* pDir, f32 fMax, f32* pHit,
                  f32* pNormal, SurfaceType** ppSurface, TerObject** ppObj);
u8    Ter_LineSphereIntersection(f32* pFrom, f32* pDir, f32 fRange, f32* pCentre, f32 fRadius);
int   TerCollision_GetMeshFlags(UObjMesh* pModel, int n);

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f (0x80283288), before the constants Ter_LineTriangleIntersection uses first; its body is unknown.
static f32 GoTerrainCollision_StrippedFn(f32 x) {
    return x + 1.0f;
}

// TW06: bool Ter_LineTriangleIntersection(f32*, f32*, f32, f32**, f32*, f32[4]*, f32[4]*). Where the
// line from pFrom along pDir meets a triangle, as a fraction t of pDir (0 < t < fMax): t, the point
// and the triangle's normal. A line along the triangle's plane never meets it.
u8 Ter_LineTriangleIntersection(f32* pFrom, f32* pDir, f32 fMax, f32 (*pTri)[3], f32* pT, f32* pHit,
                                f32* pNormal) {
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
    fDet = Vec3_Dot(vE1, vP);
    if (fDet > -0.00001f && fDet < 0.00001f) return 0;
    fInv = 1.0f / fDet;
    Ter_Subtract3(pFrom, vA, vS);
    fU = fInv * Vec3_Dot(vS, vP);
    if (fU < 0.0f || fU > 1.0f) return 0;
    vec4flt_CrossProduct(vS, vE1, vQ);
    fV = fInv * Vec3_Dot(pDir, vQ);
    if (fV < 0.0f || fU + fV > 1.0f) return 0;
    fT = fInv * Vec3_Dot(vE2, vQ);
    if (fT <= 0.0f || fT >= fMax) return 0;
    *pT = fT;
    pHit[0] = fT * pDir[0] + pFrom[0];
    pHit[1] = fT * pDir[1] + pFrom[1];
    pHit[2] = fT * pDir[2] + pFrom[2];
    vec4flt_CrossProduct(vE1, vE2, pNormal);
    LLMath_Normalize3(pNormal, pNormal);
    return 1;
}

// TW07's Ter_Init (the first function of GoTerrainCollision.c there): register the loaders for the
// out-of-bounds (network type 1) and free-drop (network type 4) networks and forget the old ones.
// Called when the game's systems start.
void Ter_Init(void) {
    Network_RegisterLoadNetworkCallback(1, (void (*)(u8*))Ter_OOBNetworkLoadCallback);
    Network_RegisterLoadNetworkCallback(4, (void (*)(u8*))Ter_FreeDropNetworkLoadCallback);
    gTerNumOOBNetworks = 0;
    gTerNumFreeDropNetworks = 0;
}

// TW06: void Ter_InitTGD(TGD_TerrainInfo*). Get a course's collision data ready once it is loaded:
// its offsets become pointers; strips flagged for one of the four pin positions (flags 1, 2, 4, 8)
// are 3D cup geometry, and each pin goes at the centre top of its geometry's bounds; then the
// triangles' high and low corners and the course floor.
void Ter_InitTGD(CourseInfo* pCourse) {
    u32 i;
    int k;
    int j;
    int nBit;
    int nPinSet;
    f32 (*pVert)[3];

    // port: the course data is big-endian and read in place through CourseInfo, TerCell,
    //       TerPolyRef, TerObject and the vertex list: a little-endian port converts it here,
    //       before the offsets are turned into pointers (docs/format-byteorder.md)
    TER_RELOCATE(pCourse, pVerts);
    TER_RELOCATE(pCourse, pTriFlags);
    TER_RELOCATE(pCourse, pLight);
    TER_RELOCATE(pCourse, pGrid);
    TER_RELOCATE(pCourse, pObjects);
    TER_RELOCATE(pCourse, pPolyRefs);
    TER_RELOCATE(pCourse, pObjRefs);
    if (pCourse->p38 != NULL) {
        TER_RELOCATE(pCourse, p38);
    }
    if (pCourse->p44 != NULL) {
        TER_RELOCATE(pCourse, p44);
    }
    if (pCourse->p3C != NULL) {
        TER_RELOCATE(pCourse, p3C);
    }
    if (pCourse->p40 != NULL) {
        TER_RELOCATE(pCourse, p40);
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
    for (i = 0; i < pCourse->nPolyRefs; i++) {
        nBit = 1;
        for (k = 0; k < NUM_CUP_POSITIONS; k++) {
            if (nBit == (pCourse->pPolyRefs[i].u4 & 0xF)) {
                gTerUse3DCupGeometry = 1;
                pVert = &pCourse->pVerts[TER_FIRST_VERTEX(&pCourse->pPolyRefs[i])];
                for (j = 0; j < pCourse->pPolyRefs[i].nTris + 2; j++) {
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
            lbl_801D3CB0.pCourse->pin[k].x
                = (gTerCupGeometryBounds[k].vMin[0] + gTerCupGeometryBounds[k].vMax[0]) / 2.0f;
            lbl_801D3CB0.pCourse->pin[k].z
                = (gTerCupGeometryBounds[k].vMin[2] + gTerCupGeometryBounds[k].vMax[2]) / 2.0f;
            lbl_801D3CB0.pCourse->pin[k].y = gTerCupGeometryBounds[k].vMax[1];
            lbl_801D3CB0.pCourse->pin[k].w = 1.0f;
        }
        gpGame->nPinSet[Game_CurHoleIndex()] = nPinSet;
    }
    gpGame->pPinPos = &lbl_801D3CB0.pCourse->pin[Game_CurrentPinSet()].x;
    gTerNumOOBNetworks = 0;
    gTerNumFreeDropNetworks = 0;
    Ter_ComputeHighestPointInEveryTriangle(lbl_801D3CB0.pCourse);
    lbl_801D3CB0.pCourse->fFloor = Ter_CalcLowestPlayableWorldHeight(lbl_801D3CB0.pCourse);
}

// TW06: bool Ter_Use3DCupGeometry(void). Whether the cup is real geometry the ball drops into;
// without it, GameRound.c holes a ball that stops within half a yard of the pin.
u8 Ter_Use3DCupGeometry(void) {
    return gTerUse3DCupGeometry;
}

// Network type 4's load callback (Ter_Init registers it): adds a loaded free-drop outline to the
// hole's list. No check against MAX_FREE_DROP_NETWORKS.
void Ter_FreeDropNetworkLoadCallback(TNetwork* pNet) {
    gTerFreeDropNetworks[gTerNumFreeDropNetworks] = pNet;
    gTerNumFreeDropNetworks++;
}

// TW06: bool Ter_PointInFreeDropNetwork(f32*). Whether a point is inside a free-drop area.
u8 Ter_PointInFreeDropNetwork(f32* pPos) {
    int i;

    if (gTerNumFreeDropNetworks == 0) return 0;
    for (i = 0; i < gTerNumFreeDropNetworks; i++) {
        if (wn_PnPoly(pPos, gTerFreeDropNetworks[i], gTerFreeDropNetworks[i]->nNumNodes)) return 1;
    }
    return 0;
}

// Network type 1's load callback (Ter_Init registers it): adds a loaded out-of-bounds outline to
// the hole's list. No check against MAX_OOB_NETWORKS.
void Ter_OOBNetworkLoadCallback(TNetwork* pNet) {
    gTerOOBNetworks[gTerNumOOBNetworks] = pNet;
    gTerNumOOBNetworks++;
}

// TW06: s32 Ter_iNumOOBNetworksLoaded(void).
s32 Ter_iNumOOBNetworksLoaded(void) {
    return gTerNumOOBNetworks;
}

// TW06: bool Ter_PointInOOBNetwork(f32*). Whether a point is inside one of the outlines; a course
// with none loaded has no out of bounds, so every point is inside.
u8 Ter_PointInOOBNetwork(f32* pPos) {
    int i;

    if (gTerNumOOBNetworks == 0) return 1;
    for (i = 0; i < gTerNumOOBNetworks; i++) {
        if (wn_PnPoly(pPos, gTerOOBNetworks[i], gTerOOBNetworks[i]->nNumNodes)) return 1;
    }
    return 0;
}

// TW06: bool Ter_CollisionWithOOBNetwork(const f32*, const f32*, f32*). Whether the segment from
// pFrom to pTo crosses one of the outlines, and where.
u8 Ter_CollisionWithOOBNetwork(f32* pFrom, f32* pTo, f32* pHit) {
    int i;

    if (gTerNumOOBNetworks == 0) return 0;
    for (i = 0; i < gTerNumOOBNetworks; i++) {
        if (Network_LineNetworkIntersection(pFrom, pTo, gTerOOBNetworks[i], gTerOOBNetworks[i]->nNumNodes,
                                            pHit)) return 1;
    }
    return 0;
}

// TW06: f32 Ter_GetAmbientLight(TGD_TerrainInfo*, f32*). The light on the ground at a point, 0..1,
// from the brightness byte of the triangle's vertex: the ground covering the point when it is
// less than a quarter of a yard above it, else the ground under it, else the lowest ground. 1
// (full light) with no ground, or on an object.
f32 Ter_GetAmbientLight(CourseInfo* pCourse, f32* pPos) {
    TerCell* pCell;
    TerPolyRef* pRef;
    f32 (*pTri)[3];
    s32 nTri;
    f32 fHeight;

    fHeight = Ter_GetCoveringGroundTriangle(pCourse, pPos, &pCell, &pRef, &pTri, &nTri);
    if (fHeight == TER_NO_GROUND || fHeight > pPos[1] + 0.25f) {
        fHeight = Ter_GetSupportingGroundTriangle(pCourse, pPos, &pCell, &pRef, &pTri, &nTri);
        if (fHeight == TER_NO_GROUND) {
            fHeight = Ter_GetLowestGroundTriangle(pCourse, pPos, &pCell, &pRef, &pTri, &nTri);
        }
    }
    if (fHeight != TER_NO_GROUND && pRef->n2 == 0) {
        return (pCourse->pLight + TER_FIRST_VERTEX(pRef))[nTri] / 255.0f;
    }
    return 1.0f;
}

// fake match: the first grid column goes through an inline with a local; written out, the compiler
// moves the conversion into the column loop instead of storing the start before it as EA does.
// It calls Math_Floor itself: through Ter_Floor the registers drift further.
static inline int fn_8004B89C_Calc(f32 fCells) {
    int n = (int)Math_Floor(fCells);
    return n;
}

// fake match: an identity read; gives EA's register choices around fWide.
static inline f32 fn_8004B89C_Read(f32 f) {
    return f;
}

// TW06: bool Ter_CheckObjectAndHazardObstruction(f32*, f32, bool, bool, f32, bool, f32). Whether
// something spoils a spot for a ball. Checked at the spot and at four corners fStep away in x and
// z, against the highest ground at most 2 x fStep above the spot: an object (with bModels, only
// one whose model has flag 0x40) standing within fRadius of the spot, or within fRadius + 0.5 when
// the object is over 4 yards wide, unless the spot is on surface class 2, 3 or 6; with bHazards,
// no ground or a free-drop area at any of the five points, ground without surface flag 1 at the
// spot, or at a corner ground that has neither flag 1 nor class 8; with bSlope, a corner more
// than fStep x fMaxSlope above or below the spot.
u8 Ter_CheckObjectAndHazardObstruction(f32* pPos, f32 fRadius, u8 bModels, u8 bHazards, f32 fStep, u8 bSlope,
                                       f32 fMaxSlope) {
    int j;
    TerCell* pCell;
    f32 aHeight[5];
    TerPolyRef* apRef[5];
    f32 vPos[4];
    u8 abFreeDrop[5];
    f32 fA;
    f32 fB;
    f32 fC;
    f32 fA2;
    f32 fB2;
    f32 fC2;
    u8 bFirst;
    int n;
    CourseInfo* pCourse;
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
    int nCorner;
    int nInner;
    TerPolyRef* pRef;
    u16* pObjRef;
    f32 (*pVert)[3];
    u8* pFlags;
    u8 uFlags;
    UObjMesh* pModel;

    bObstructed = 0;
    bFirst = 1;
    pCourse = Ter_GetTGD();
    if (pCourse == NULL) return 0;
    for (i = 0; i < 5; i++) {
        aHeight[i] = -50000.0f;
        abFreeDrop[i] = 0;
        apRef[i] = NULL;
    }
    fMinX = pPos[0] - (fRadius <= fStep ? fStep : fRadius);
    fMaxX = pPos[0] + (fRadius <= fStep ? fStep : fRadius);
    fMinZ = pPos[2] - (fRadius <= fStep ? fStep : fRadius);
    fMaxZ = pPos[2] + (fRadius <= fStep ? fStep : fRadius);
    fTop = pPos[1] + 2.0f * fStep;
    nMinX = fn_8004B89C_Calc((fMinX - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    nMinZ = (int)Ter_Floor((fMinZ - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
    nMaxX = (int)Ter_Floor((fMaxX - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    nMaxZ = (int)Ter_Floor((fMaxZ - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
    fWide = fn_8004B89C_Read(0.5f + fRadius);
    for (nX = nMinX; nX <= nMaxX; nX++) {
        for (nZ = nMinZ; nZ <= nMaxZ; nZ++) {
            if (nX >= 0 && nX < pCourse->nGridWidth && nZ >= 0 && nZ < pCourse->nGridLength) {
                pCell = &pCourse->pGrid[nX + nZ * pCourse->nGridWidth];
                pObjRef = &pCourse->pObjRefs[pCell->nObjRefOffset];
                for (i = pCell->nObjRefs - 1; i >= 0; i--) {
                    fDx = pCourse->pObjects[*pObjRef].vBase[0] - pPos[0];
                    fDz = pCourse->pObjects[*pObjRef].vBase[2] - pPos[2];
                    fObjRadius = pCourse->pObjects[*pObjRef].fBaseRadius;
                    fDist = (f32)Math_Sqrt(fDx * fDx + fDz * fDz) - fObjRadius;
                    if (fDist < fRadius || (fObjRadius > 4.0f && fDist < fWide)) {
                        if (bModels) {
                            pModel = Ter_GetObjectListModel(pCourse->pObjects[*pObjRef].nPatch,
                                                 pCourse->pObjects[*pObjRef].nObjList);
                            if (pModel != NULL && (TerCollision_GetMeshFlags(pModel, 0) & 0x40)) {
                                bObstructed = 1;
                            }
                        } else {
                            bObstructed = 1;
                        }
                    }
                    pObjRef++;
                }
                pRef = &pCourse->pPolyRefs[pCell->uRefs >> 12];
                for (n = (pCell->uRefs & 0xFFF) - 1; n >= 0; n--) {
                    if (pRef->n2 != 0) {
                        pRef++;
                    } else {
                        pVert = &pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
                        pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
                        for (j = pRef->nTris - 1; j >= 0; j--) {
                            uFlags = pFlags[2];
                            if (bFirst) {
                                abFreeDrop[0] = Ter_PointInFreeDropNetwork(pPos);
                            }
                            if ((uFlags & 7)
                                && (fTop > pVert[(uFlags >> 6) & 3][1]
                                    || aHeight[0] < pVert[(uFlags >> 4) & 3][1])
                                && Ter_PointInTriangleXZpY(pVert[0], pVert[1], pVert[2], pPos[0], pPos[2])) {
                                Ter_GetBarycentricCoords(pVert, pPos, &fA, &fB, &fC);
                                fHeight = fA * pVert[0][1] + fB * pVert[1][1] + fC * pVert[2][1];
                                if (fHeight > aHeight[0] && fHeight <= fTop) {
                                    aHeight[0] = fHeight;
                                    apRef[0] = pRef;
                                }
                            }
                            k = 1;
                            vPos[0] = pPos[0] - fStep;
                            for (nCorner = 0; nCorner < 2; nCorner++) {
                                vPos[2] = pPos[2] - fStep;
                                for (nInner = 0; nInner < 2; nInner++) {
                                    if (bFirst) {
                                        abFreeDrop[k] = Ter_PointInFreeDropNetwork(vPos);
                                    }
                                    if ((uFlags & 7)
                                        && (fTop > pVert[(uFlags >> 6) & 3][1]
                                            || aHeight[k] < pVert[(uFlags >> 4) & 3][1])
                                        && Ter_PointInTriangleXZpY(pVert[0], pVert[1], pVert[2], vPos[0],
                                                vPos[2])) {
                                        Ter_GetBarycentricCoords(pVert, vPos, &fA2, &fB2, &fC2);
                                        fHeight = fA2 * pVert[0][1] + fB2 * pVert[1][1] + fC2 * pVert[2][1];
                                        if (fHeight > aHeight[k] && fHeight <= fTop) {
                                            aHeight[k] = fHeight;
                                            apRef[k] = pRef;
                                        }
                                    }
                                    vPos[2] += 2.0f * fStep;
                                    k++;
                                }
                                vPos[0] += 2.0f * fStep;
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
        if (gSurfaceTypes[apRef[0]->nSurface].nClass == 3 || gSurfaceTypes[apRef[0]->nSurface].nClass == 2
            || gSurfaceTypes[apRef[0]->nSurface].nClass == 6) {
            bObstructed = 0;
        }
    }
    if (bObstructed) return 1;
    if (bHazards) {
        if ((apRef[0] != NULL && !(gSurfaceTypes[apRef[0]->nSurface].u34 & 1)) || abFreeDrop[0]) return 1;
        for (k = 1; k < 5; k++) {
            if (abFreeDrop[k] || apRef[k] == NULL
                || (apRef[k] != NULL && !(gSurfaceTypes[apRef[k]->nSurface].u34 & 1)
                    && gSurfaceTypes[apRef[k]->nSurface].nClass != 8)) {
                return 1;
            }
        }
    }
    if (bSlope) {
        for (k = 1; k < 5; k++) {
            if (apRef[k] != NULL && fabsf(aHeight[k] - aHeight[0]) > fStep * fMaxSlope) return 1;
        }
    }
    return 0;
}

// TW06: bool Ter_SearchForDropLocation(s32, bool, bool, f32*), with the ring search that TW06 split
// out as Ter_SearchAreaForDropLocation written inline. Where a player's ball is to be
// dropped (pOut): the last good drop spot (gBallDropSpot), or with bPreferred the last spot with a
// preferred lie (gBallPreferredLieSpot) when that is not the shot's own start and is less than 10 yards
// further away. If the spot is over 3 yards off, or (not preferred) on another class of surface
// than the ball, search rings of 1 to 4 yards around the ball, every 45 degrees starting towards
// the pin, for a drop on the same class, else the nearest. Returns 0 when the spot is where the
// shot started, or (with bCheck, while vBall is still at vA44) within 50 yards of vA44.
u8 Ter_SearchForDropLocation(int nPlayer, u8 bPreferred, u8 bCheck, f32* pOut) {
    f32 fAngle;
    f32 vPos[4];
    f32 vDir[4];
    SurfaceType* pSurface;
    u8 bDrop;
    u8 bPreferredLie;
    CourseInfo* pCourse;
    f32 fDist;
    f32 fDropDist;
    f32 fRadius;
    f32 fDist2;
    f32 fTurn;
    f32 fSin;
    f32 fHeading;
    f32 fCos;
    int nRing;
    Player* p;
    SurfaceType* pGround;

    p = &gPlayers[nPlayer];
    pCourse = Ter_GetTGD();
    fDist = LLMath_SquareDistanceBetween3(&pCourse->pin[Game_CurrentPinSet()].x, p->ball.vStart);
    if (LLMath_SquareDistanceBetween3(p->ball.vPos, p->ball.vStart) > fDist) {
        bPreferred = 0;                 // past the pin
    }
    if (bPreferred) {
        fDist = LLMath_DistanceBetween3(gBallPreferredLieSpot[nPlayer], p->ball.vPos);
        fDropDist = LLMath_DistanceBetween3(gBallDropSpot[nPlayer], p->ball.vPos);
        if ((p->ball.vStart[0] != gBallPreferredLieSpot[nPlayer][0] || p->ball.vStart[1] != gBallPreferredLieSpot[nPlayer][1]
             || p->ball.vStart[2] != gBallPreferredLieSpot[nPlayer][2])
            && fDist - fDropDist < 10.0f) {
            LLMath_CopyVec(gBallPreferredLieSpot[nPlayer], pOut);
        } else {
            LLMath_CopyVec(gBallDropSpot[nPlayer], pOut);
        }
    } else {
        LLMath_CopyVec(gBallDropSpot[nPlayer], pOut);
    }
    vPos[0] = pOut[0];
    vPos[1] = 1000000.0f;
    vPos[2] = pOut[2];
    vPos[3] = 1.0f;
    pGround = Ter_GetSupportingGroundMaterial(pCourse, vPos);
    fDist2 = LLMath_SquareDistanceBetween3(pOut, p->ball.vPos);
    if (fDist2 > 9.0f
        || (!bPreferred && pGround->nClass != gSurfaceTypes[p->ball.nSurface].nClass)) {
        Ter_Subtract3(&pCourse->pin[Game_CurrentPinSet()].x, p->ball.vPos, vDir);
        LLMath_Normalize3(vDir, vDir);
        fHeading = atan2f(vDir[0], vDir[2]);
        fRadius = 1.0f;
        for (nRing = 0; nRing < 4; nRing++) {
            for (fTurn = 0.0f; fTurn <= 6.265732f; fTurn += 0.7853982f) {
                fAngle = fHeading + fTurn;
                fSin = Math_Sin(fAngle);
                fCos = Math_Cos(fAngle);
                vPos[0] = fCos * fRadius + p->ball.vPos[0];
                vPos[2] = fSin * fRadius + p->ball.vPos[2];
                // from water, only straight towards the pin
                if (gSurfaceTypes[p->ball.nSurface].nClass == 7 && fTurn > 0.0f) continue;
                vPos[1] = p->ball.vPos[1] + 2.0f * fRadius;
                vPos[1] = Ter_CheckForDropLocation(pCourse, vPos, 0, &bDrop, &bPreferredLie, &pSurface);
                if ((bPreferred && bPreferredLie) || (!bPreferred && bDrop)) {
                    if (pSurface->nClass == gSurfaceTypes[p->ball.nSurface].nClass) {
                        LLMath_CopyVec(vPos, pOut);
                        goto done;  // fake match: the original branches straight to the end, past the
                                    // ring loop's compare, which a break (and a flag) would keep
                    }
                    if (fRadius < fDist2) {     // EA bug: a distance against a squared one
                        LLMath_CopyVec(vPos, pOut);
                        fDist2 = fRadius;
                    }
                }
            }
            fRadius += 1.0f;
        }
    }
done:
    if (p->ball.vStart[0] == pOut[0] && p->ball.vStart[2] == pOut[2]) return 0;
    if (bCheck && p->vA44[0] == p->vBall[0] && p->vA44[2] == p->vBall[2]
        && LLMath_SquareDistanceBetween3(pOut, p->vA44) < 2500.0f) {
        return 0;
    }
    return 1;
}

// TW06: f32 Ter_CheckForDropLocation(TGD_TerrainInfo*, f32*, bool, bool*, bool*, TGD_MaterialInfo**).
// Whether a ball could be dropped at a point: in bounds and outside the free-drop areas, and, unless
// bOnDropSurface, on ground that allows a drop (surface flag 1, not surface class 10) that is no
// steeper than 30 degrees and clear of objects and hazards. *pbPreferred says the surface there is
// a preferred one (always, when bOnDropSurface). Returns the ground height (0 when bOnDropSurface).
f32 Ter_CheckForDropLocation(CourseInfo* pCourse, f32* pPos, u8 bOnDropSurface, u8* pbDrop, u8* pbPreferred,
                             SurfaceType** ppSurface) {
    f32 vPos[4];
    f32 vNormal[4];
    SurfaceType* pSurface;
    f32 fHeight;
    u8 bOk;

    vPos[0] = pPos[0];
    vPos[1] = pPos[1];
    vPos[2] = pPos[2];
    vPos[3] = 1.0f;
    if (!bOnDropSurface) {
        fHeight = Ter_GetSupportingGroundData(pCourse, vPos, &pSurface, vNormal);
        if (ppSurface != NULL) {
            *ppSurface = pSurface;
        }
        bOk = fHeight != TER_NO_GROUND && pSurface != NULL && (pSurface->u34 & 1)
              && pSurface->nClass != 10 && fabsf(vNormal[1]) > 0.86603f
              && Ter_PointInOOBNetwork(vPos) && !Ter_PointInFreeDropNetwork(vPos)
              // fake match: bOnDropSurface is always 0 here, but the original tests it again
              && (bOnDropSurface || !Ter_CheckObjectAndHazardObstruction(vPos, 1.5f, 0, 1, 2.0f, 1, 0.577f));
    } else {
        fHeight = 0.0f;
        if (ppSurface != NULL) {
            *ppSurface = NULL;
        }
        bOk = Ter_PointInOOBNetwork(vPos) && !Ter_PointInFreeDropNetwork(vPos);
    }
    vPos[1] = fHeight;              // fake match: never read again, but the original stores it
    if (bOk) {
        *pbDrop = 1;
        if (bOnDropSurface || Ter_LieIsPreferred(pSurface->nClass)) {
            *pbPreferred = 1;
        } else {
            *pbPreferred = 0;
        }
    } else {
        *pbDrop = 0;
        *pbPreferred = 0;
    }
    return fHeight;
}

// TW06: bool Ter_LieIsPreferred(u32). Whether a surface class (SurfaceType.nClass, which TW06 calls
// the lie ID; not a Lie_t) is a preferred place for a drop: classes 1 to 4, the ones Physics_SetLie
// turns into the fairway (1, 2), green (3) and fringe (4) lies.
u8 Ter_LieIsPreferred(u32 nClass) {
    if (nClass == 1 || nClass == 2 || nClass == 3 || nClass == 4) {
        return 1;
    }
    return 0;
}

// TW06: bool Ter_IsValidDropSurface(s32). Whether a surface type (a row of gSurfaceTypes) is of
// surface class 3 (green), 12 (the cup) or 18 (green by the cup): Physics_Simulate records a ball
// there as a drop spot without the ground checks.
u8 Ter_IsValidDropSurface(s32 nSurface) {
    u32 nClass;

    if (nSurface < 0 || nSurface >= NUM_SURFACE_TYPES) return 0;
    nClass = gSurfaceTypes[nSurface].nClass;
    if (nClass == 3 || nClass == 12 || nClass == 18) return 1;
    return 0;
}

// The height of the lowest vertex of any ground triangle whose surface is not class 19; the
// course keeps it as CourseInfo.fFloor.
f32 Ter_CalcLowestPlayableWorldHeight(CourseInfo* pCourse) {
    f32 fLowest = 1000000.0f;
    u32 i;
    int k;
    TerPolyRef* pRef = pCourse->pPolyRefs;

    for (i = 0; i < pCourse->nPolyRefs; i++) {
        if (pRef->nSurface < NUM_SURFACE_TYPES && gSurfaceTypes[pRef->nSurface].nClass == 19) {
            pRef++;
        } else {
            f32* pVert = pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
            u8* pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);

            // a vertex is part of the triangles ending at it and at the next two vertices
            for (k = 0; k < pRef->nTris + 2; k++) {
                if ((k <= pRef->nTris + 1 && (pFlags[0] & 7)) || (k <= pRef->nTris && (pFlags[1] & 7))
                    || (k <= pRef->nTris - 1 && (pFlags[2] & 7))) {
                    if (pVert[1] < fLowest) {
                        fLowest = pVert[1];
                    }
                }
                pVert += 3;
                pFlags++;
            }
            pRef++;
        }
    }
    return fLowest;
}

// The highest ground triangle under a point (x, z): its height there, the grid cell, the strip,
// the triangle's vertices and its number in the strip; TER_NO_GROUND when there is none. Only
// ground strips count (not objects), and only those in use: a strip flagged as the cup geometry of
// some pin positions (bits 1, 2, 4, 8) is skipped unless the current one is among them, and one
// with 0x10 in split screen.
f32 Ter_GetHighestGroundTriangle(CourseInfo* pCourse, f32* pPos, TerCell** ppCell, TerPolyRef** ppRef,
                                 f32 (**ppTri)[3],
                s32* pTri) {
    u32 uPinSet = 1 << Game_CurrentPinSet();
    f32 fBest = TER_NO_GROUND;
    int nX = (int)Ter_Floor((pPos[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    int nZ = (int)Ter_Floor((pPos[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
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
                    if ((pFlags[2] & 7) && fBest < pVert[(pFlags[2] >> 4) & 3][1]
                        && Ter_PointInTriangleXZpY(pVert[0], pVert[1], pVert[2], pPos[0], pPos[2])) {
                        Ter_GetBarycentricCoords(pVert, pPos, &fA, &fB, &fC);
                        fHeight = fA * pVert[0][1] + fB * pVert[1][1] + fC * pVert[2][1];
                        if (fHeight > fBest) {
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

// The lowest ground triangle under a point (x, z), with the same outputs and strip rules as
// Ter_GetHighestGroundTriangle; TER_NO_GROUND when there is none.
f32 Ter_GetLowestGroundTriangle(CourseInfo* pCourse, f32* pPos, TerCell** ppCell, TerPolyRef** ppRef,
                                f32 (**ppTri)[3],
                s32* pTri) {
    u32 uPinSet = 1 << Game_CurrentPinSet();
    f32 fBest = 50000.0f;
    int nX = (int)Ter_Floor((pPos[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    int nZ = (int)Ter_Floor((pPos[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
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
                    if ((pFlags[2] & 7) && fBest > pVert[(pFlags[2] >> 6) & 3][1]
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
f32 Ter_GetSupportingWorldTriangle(CourseInfo* pCourse, f32* pPos, TerCell** ppCell, TerPolyRef** ppRef,
                                   f32 (**ppTri)[3]) {
    u32 uPinSet = 1 << Game_CurrentPinSet();
    f32 fBest = TER_NO_GROUND;
    int nX = (int)Ter_Floor((pPos[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    int nZ = (int)Ter_Floor((pPos[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
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
        if ((f32)pCell->nMinHeight > pPos[1]) return TER_NO_GROUND;
        pRef = &pCourse->pPolyRefs[pCell->uRefs >> 12];
        for (i = (pCell->uRefs & 0xFFF) - 1; i >= 0; i--) {
            if (pRef->u4 != 0 && (!(pRef->u4 & uPinSet) || ((pRef->u4 & 0x10) && gSession.nSplitScreen))) {
                pRef++;
            } else {
                pVert = &pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
                pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
                for (j = pRef->nTris - 1; j >= 0; j--) {
                    if ((pFlags[2] & 7)
                        && (pPos[1] > pVert[(pFlags[2] >> 6) & 3][1]
                            || fBest < pVert[(pFlags[2] >> 4) & 3][1])
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
f32 Ter_GetCoveringGroundTriangle(CourseInfo* pCourse, f32* pPos, TerCell** ppCell, TerPolyRef** ppRef,
                                  f32 (**ppTri)[3],
                s32* pTri) {
    u32 uPinSet = 1 << Game_CurrentPinSet();
    f32 fBest = 65536.0f;
    int nX = (int)Ter_Floor((pPos[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    int nZ = (int)Ter_Floor((pPos[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
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
        if ((f32)pCell->nMaxHeight < pPos[1]) return TER_NO_GROUND;
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
                        && (fBest > pVert[(pFlags[2] >> 6) & 3][1]
                            || pPos[1] < pVert[(pFlags[2] >> 4) & 3][1])
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
void Ter_GetSupportingAndCoveringGroundTriangles(CourseInfo* pCourse, f32* pPos, TerPolyRef** ppRefLow,
                                                 f32* pLow, f32 (**ppTriLow)[3],
                 TerPolyRef** ppRefHigh, f32* pHigh, f32 (**ppTriHigh)[3]) {
    u32 uPinSet = 1 << Game_CurrentPinSet();
    f32 fHigh = 65536.0f;
    f32 fLow = -65536.0f;
    int nX = (int)Ter_Floor((pPos[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    int nZ = (int)Ter_Floor((pPos[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
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
                        && (fHigh > pVert[(pFlags[2] >> 6) & 3][1] || fLow < pVert[(pFlags[2] >> 4) & 3][1])
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
f32 Ter_GetHighestGroundHeight(CourseInfo* pCourse, f32* pPos) {
    TerCell* pCell;
    TerPolyRef* pRef;
    f32 (*pTri)[3];
    s32 nTri;

    return Ter_GetHighestGroundTriangle(pCourse, pPos, &pCell, &pRef, &pTri, &nTri);
}

// The height of the lowest ground triangle under a point (x, z), whatever the point's own height
// (Ter_GetLowestGroundTriangle); TER_NO_GROUND when there is none.
f32 Ter_GetLowestGroundHeight(CourseInfo* pCourse, f32* pPos) {
    TerCell* pCell;
    TerPolyRef* pRef;
    f32 (*pTri)[3];
    s32 nTri;

    return Ter_GetLowestGroundTriangle(pCourse, pPos, &pCell, &pRef, &pTri, &nTri);
}

// The height of the supporting ground under a point: the highest ground triangle at or below it
// (Ter_GetSupportingGroundTriangle); TER_NO_GROUND when there is none.
f32 Ter_GetSupportingGroundHeight(CourseInfo* pCourse, f32* pPos) {
    TerCell* pCell;
    TerPolyRef* pRef;
    f32 (*pTri)[3];
    s32 nTri;

    return Ter_GetSupportingGroundTriangle(pCourse, pPos, &pCell, &pRef, &pTri, &nTri);
}

// The height of the ground covering a point (Ter_GetCoveringGroundTriangle) and, when there is one,
// that triangle's normal turned to face up; TER_NO_GROUND (pNormal untouched) when there is none.
f32 Ter_GetCoveringGroundHeightAndNormal(CourseInfo* pCourse, f32* pPos, f32* pNormal) {
    f32 vA[4];
    f32 vB[4];
    f32 vC[4];
    f32 vAB[4];
    f32 vBC[4];
    TerCell* pCell;
    TerPolyRef* pRef;
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
u8 Ter_GetSupportingGroundNormal(CourseInfo* pCourse, f32* pPos, f32* pNormal) {
    f32 vA[4];
    f32 vB[4];
    f32 vC[4];
    f32 vAB[4];
    f32 vBC[4];
    TerCell* pCell;
    TerPolyRef* pRef;
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
f32 Ter_GetSupportingWorldHeight(CourseInfo* pCourse, f32* pPos) {
    TerCell* pCell;
    TerPolyRef* pRef;
    f32 (*pTri)[3];

    return Ter_GetSupportingWorldTriangle(pCourse, pPos, &pCell, &pRef, &pTri);
}

// TW06: TGD_MaterialInfo* Ter_GetSupportingWorldMaterial(TGD_TerrainInfo*, f32*). The surface
// under a point, objects included; NULL when there is none.
SurfaceType* Ter_GetSupportingWorldMaterial(CourseInfo* pCourse, f32* pPos) {
    TerCell* pCell;
    TerPolyRef* pRef;
    f32 (*pTri)[3];

    if (Ter_GetSupportingWorldTriangle(pCourse, pPos, &pCell, &pRef, &pTri) != TER_NO_GROUND) {
        return &gSurfaceTypes[pRef->nSurface];
    }
    return NULL;
}

// TW06: f32 Ter_GetSupportingGroundData(const TGD_TerrainInfo*, const f32*, TGD_MaterialInfo**, f32*).
// The ground height under a point, with the surface there and the triangle's upward normal; with
// nothing under the point, TER_NO_GROUND and no surface.
f32 Ter_GetSupportingGroundData(CourseInfo* pCourse, f32* pPos, SurfaceType** ppSurface, f32* pNormal) {
    f32 vA[4];
    f32 vB[4];
    f32 vC[4];
    f32 vAB[4];
    f32 vBC[4];
    TerCell* pCell;
    TerPolyRef* pRef;
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
void Ter_GetEnclosingGroundHeight(CourseInfo* pCourse, f32* pPos, f32* pLow, f32* pHigh) {
    TerPolyRef* pRefLow;
    TerPolyRef* pRefHigh;
    f32 (*pTriLow)[3];
    f32 (*pTriHigh)[3];

    Ter_GetSupportingAndCoveringGroundTriangles(pCourse, pPos, &pRefLow, pLow, &pTriLow, &pRefHigh, pHigh,
                                                &pTriHigh);
}

// TW06: void Ter_GetEnclosingGroundData(TGD_TerrainInfo*, f32*, f32*, TGD_MaterialInfo**, f32*, f32*,
// TGD_MaterialInfo**, f32*). The ground just below and just above a point: heights,
// surfaces and upward normals (TER_NO_GROUND and NULL for none).
void Ter_GetEnclosingGroundData(CourseInfo* pCourse, f32* pPos, f32* pLow, SurfaceType** ppSurfaceLow,
                                f32* pNormalLow, f32* pHigh, SurfaceType** ppSurfaceHigh, f32* pNormalHigh) {
    f32 vA[4];
    f32 vB[4];
    f32 vC[4];
    f32 vAB[4];
    f32 vBC[4];
    f32 vAB2[4];
    f32 vBC2[4];
    TerPolyRef* pRefLow;
    TerPolyRef* pRefHigh;
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
f32 Ter_GetSupportingWorldData(CourseInfo* pCourse, f32* pPos, SurfaceType** ppSurface, f32* pNormal) {
    f32 vA[4];
    f32 vB[4];
    f32 vC[4];
    f32 vAB[4];
    f32 vBC[4];
    TerCell* pCell;
    TerPolyRef* pRef;
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
u32 Ter_GetTerrainLayers(CourseInfo* pCourse, f32* pPos, SurfaceType** ppSurfaces, f32* pHeights, u32 nMax) {
    u32 uPinSet = 1 << Game_CurrentPinSet();
    u32 nFound = 0;
    int nX = (int)Ter_Floor((pPos[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    int nZ = (int)Ter_Floor((pPos[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
    TerCell* pCell;
    TerPolyRef* pRef;
    f32 (*pVert)[3];
    u8* pFlags;
    int i;
    int j;
    f32 fA;
    f32 fB;
    f32 fC;

    if (nX >= 0 && nX < pCourse->nGridWidth && nZ >= 0 && nZ < pCourse->nGridLength) {
        pCell = &pCourse->pGrid[nX + nZ * pCourse->nGridWidth];
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
void Ter_MarkObjectsNearLine(CourseInfo* pCourse, f32* pFrom, f32* pDir, int nX, int nZ, f32 fRange) {
    int i;
    u16* pRefs;
    TerCell* pCell;
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
    fDist = (f32)Math_Sqrt(Vec3_LengthSqClamped(vTo)) - fRadius;
    if (fDist > fRange) return 0;
    if (fDist < 0.0f) return 1;
    if (Vec3_Dot(vTo, pDir) < 0.0f) return 0;
    return 1;
}

// TW06: bool Ter_CheckForPinCollision(TGD_TerrainInfo*, s32, f32*, f32*, f32[4]*, f32[4]*,
// TGD_MaterialInfo**, TGD_ObjectInstanceInfo**). Whether the line from pFrom to pTo hits the
// flagstick of the current hole: a vertical cylinder of one inch radius, 2 yards tall, at the pin.
// Not for nobody's ball, nor when the player's view has the flagstick out (bFlagOut). On
// a hit: the point, the stick's outward normal and surface 90 (the cup).
u8 Ter_CheckForPinCollision(CourseInfo* pCourse, int nPlayer, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal,
                            SurfaceType** ppSurface, TerObject** ppObj) {
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
    if (Vec3_Dot(vIn, vFlat) < 0.0f) return 0;      // moving away from the stick
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
u8 Ter_CheckForWorldCollision(CourseInfo* pCourse, int nPlayer, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal,
               SurfaceType** ppSurface, TerObject** ppObj, u8* pbFlags) {
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
    SurfaceType* pSurface;
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
    if (fBest != 1000000.0f && Vec3_Dot(vDir, pNormal) > 0.0f) {
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
u8 Ter_CheckForWorldCollisionOneGrid(CourseInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo, f32* pDir,
                                     f32 fMax, f32* pHit,
               f32* pNormal, SurfaceType** ppSurface, TerObject** ppObj, u8* pbFlags) {
    f32 vHit[4];
    f32 vNormal[4];
    f32 fT;
    u8 bHit = 0;
    u32 uPinSet = 1 << Game_CurrentPinSet();
    TerCell* pCell;
    TerPolyRef* pRef;
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
        pRef = &pCourse->pPolyRefs[pCell->uRefs >> 12];
        for (i = (pCell->uRefs & 0xFFF) - 1; i >= 0; i--) {
            if (pRef->u4 != 0 && (!(pRef->u4 & uPinSet) || ((pRef->u4 & 0x10) && gSession.nSplitScreen))) {
                pRef++;
            } else if (gTerObjectMarks[pRef->n2] == 0) {
                pRef++;
            } else {
                pVert = &pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
                pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
                for (j = pRef->nTris - 1; j >= 0; j--) {
                    if ((pFlags[2] & 7)
                        && Ter_LineTriangleIntersection(pFrom, pDir, fMax, pVert, &fT, vHit, vNormal)) {
                        fMax = fT;
                        LLMath_CopyVec(vHit, pHit);
                        LLMath_CopyVec(vNormal, pNormal);
                        if (pbFlags != NULL) {
                            *pbFlags = pFlags[2] & 7;
                        }
                        *ppSurface = &gSurfaceTypes[pRef->nSurface];
                        if (pRef->n2 != 0) {
                            *ppObj = &pCourse->pObjects[pRef->n2];
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
u8 Ter_CheckForSolidWorldCollision(CourseInfo* pCourse, int nPlayer, f32* pFrom, f32* pTo, f32* pHit,
                                   f32* pNormal,
               SurfaceType** ppSurface, TerObject** ppObj) {
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
    SurfaceType* pSurface;
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
    if (fBest != 1000000.0f && Vec3_Dot(vDir, pNormal) > 0.0f) {
        Ter_Negate(pNormal, pNormal);
    }
    return fBest != 1000000.0f;
}

// TW06: bool Ter_CheckForSolidWorldCollisionOneGrid(...), the same parameters without the flags.
// As Ter_CheckForWorldCollisionOneGrid, but surfaces with a negative bounce (branches and leaves,
// which a ball passes through) do not count.
u8 Ter_CheckForSolidWorldCollisionOneGrid(CourseInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo,
                                          f32* pDir, f32 fMax, f32* pHit,
               f32* pNormal, SurfaceType** ppSurface, TerObject** ppObj) {
    f32 vHit[4];
    f32 vNormal[4];
    f32 fT;
    u8 bHit = 0;
    u32 uPinSet = 1 << Game_CurrentPinSet();
    TerCell* pCell;
    TerPolyRef* pRef;
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
        pRef = &pCourse->pPolyRefs[pCell->uRefs >> 12];
        for (i = (pCell->uRefs & 0xFFF) - 1; i >= 0; i--) {
            if (pRef->u4 != 0 && (!(pRef->u4 & uPinSet) || ((pRef->u4 & 0x10) && gSession.nSplitScreen))) {
                pRef++;
            } else if (gTerObjectMarks[pRef->n2] == 0) {
                pRef++;
            } else if (gSurfaceTypes[pRef->nSurface].f0C < 0.0f) {
                pRef++;
            } else {
                pVert = &pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
                pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
                for (j = pRef->nTris - 1; j >= 0; j--) {
                    if ((pFlags[2] & 7)
                        && Ter_LineTriangleIntersection(pFrom, pDir, fMax, pVert, &fT, vHit, vNormal)) {
                        fMax = fT;
                        LLMath_CopyVec(vHit, pHit);
                        LLMath_CopyVec(vNormal, pNormal);
                        *ppSurface = &gSurfaceTypes[pRef->nSurface];
                        if (pRef->n2 != 0) {
                            *ppObj = &pCourse->pObjects[pRef->n2];
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
u8 Ter_CheckForGroundCollision(CourseInfo* pCourse, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal,
                               SurfaceType** ppSurface, TerObject** ppObj) {
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
    SurfaceType* pSurface;
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
    if (fBest != 1000000.0f && Vec3_Dot(vDir, pNormal) > 0.0f) {
        Ter_Negate(pNormal, pNormal);
    }
    return fBest != 1000000.0f;
}

// TW06: bool Ter_CheckForGroundCollisionOneGrid(...). As Ter_CheckForSolidWorldCollisionOneGrid,
// the ground only (no objects).
u8 Ter_CheckForGroundCollisionOneGrid(CourseInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo, f32* pDir,
                                      f32 fMax, f32* pHit,
               f32* pNormal, SurfaceType** ppSurface, TerObject** ppObj) {
    f32 vHit[4];
    f32 vNormal[4];
    f32 fT;
    u8 bHit = 0;
    u32 uPinSet = 1 << Game_CurrentPinSet();
    TerCell* pCell;
    TerPolyRef* pRef;
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
                        && Ter_LineTriangleIntersection(pFrom, pDir, fMax, pVert, &fT, vHit, vNormal)) {
                        fMax = fT;
                        LLMath_CopyVec(vHit, pHit);
                        LLMath_CopyVec(vNormal, pNormal);
                        *ppSurface = &gSurfaceTypes[pRef->nSurface];
                        if (pRef->n2 != 0) {
                            *ppObj = &pCourse->pObjects[pRef->n2];
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
u8 Ter_CheckForObjectCollision(CourseInfo* pCourse, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal,
                               SurfaceType** ppSurface,
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
    SurfaceType* pSurface;
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
    if (fBest != 1000000.0f && Vec3_Dot(vDir, pNormal) > 0.0f) {
        Ter_Negate(pNormal, pNormal);
    }
    return fBest != 1000000.0f;
}

// TW06: bool Ter_CheckForObjectCollisionOneGrid(...). As Ter_CheckForSolidWorldCollisionOneGrid,
// objects only, plus ground whose surface has flag 0x80.
u8 Ter_CheckForObjectCollisionOneGrid(CourseInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo, f32* pDir,
                                      f32 fMax, f32* pHit,
               f32* pNormal, SurfaceType** ppSurface, TerObject** ppObj) {
    f32 vHit[4];
    f32 vNormal[4];
    f32 fT;
    u8 bHit = 0;
    u32 uPinSet = 1 << Game_CurrentPinSet();
    TerCell* pCell;
    TerPolyRef* pRef;
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
        pRef = &pCourse->pPolyRefs[pCell->uRefs >> 12];
        for (i = (pCell->uRefs & 0xFFF) - 1; i >= 0; i--) {
            if (pRef->n2 == 0 && !(gSurfaceTypes[pRef->nSurface].u34 & 0x80)) {
                pRef++;
            } else if (pRef->u4 != 0
                       && (!(pRef->u4 & uPinSet) || ((pRef->u4 & 0x10) && gSession.nSplitScreen))) {
                pRef++;
            } else {
                pVert = &pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
                pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
                for (j = pRef->nTris - 1; j >= 0; j--) {
                    if ((pFlags[2] & 7)
                        && Ter_LineTriangleIntersection(pFrom, pDir, fMax, pVert, &fT, vHit, vNormal)) {
                        fMax = fT;
                        LLMath_CopyVec(vHit, pHit);
                        LLMath_CopyVec(vNormal, pNormal);
                        *ppSurface = &gSurfaceTypes[pRef->nSurface];
                        if (pRef->n2 != 0) {
                            *ppObj = &pCourse->pObjects[pRef->n2];
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
void Ter_ComputeHighestPointInEveryTriangle(CourseInfo* pCourse) {
    u8 uFlags;
    f32 (*pVert)[3];
    u32 i;
    u32 j;
    u8* pFlags;
    TerPolyRef* pRef;
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
            if (!(uFlags & 8) && uFlags != 0) {
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
                pFlags[2] = uFlags | (nHigh << 4) | (nLow << 6) | 8;
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
            pFlags[2] &= 0xF7;
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
// and nObjList) and tests bit 0x40 of byte n = 0.
int TerCollision_GetMeshFlags(UObjMesh* pModel, int n) {
    return pModel->pInfo->a24[n];
}

// TW06: s32 MaterialTypes::getMaterialID(const TGD_MaterialInfo*). A surface's row in
// gSurfaceTypes, or -1.
s32 fn_80050BEC(SurfaceType* pSurface) {
    s32 n;

    if (pSurface == NULL) return -1;
    n = ((u8*)pSurface - (u8*)gSurfaceTypes) / sizeof(SurfaceType);
    if (n < 0 || n >= NUM_SURFACE_TYPES) return -1;
    return n;
}
