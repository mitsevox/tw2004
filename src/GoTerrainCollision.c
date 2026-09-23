// GoTerrainCollision.c (TW06's goterraincollision.c): the course's ground as collision data
// (CourseInfo, TW06's TGD_TerrainInfo): the free-drop and out-of-bounds networks, the drop
// checks, heights, normals and surfaces under a point, and the ball's collision tests against the
// ground, the pin and the course objects. The ground is triangle strips found through a grid;
// each triangle's surface is a row of gSurfaceTypes. The file starts at 0x8004AFA0 (the code from
// there on shares one constant pool) and ends where Ball.c begins.

#include "golfer.h"
#include "physics.h"
#include "game.h"

// One strip of ground triangles (8 bytes). TW06: TGD_PolygonReference (0xC bytes, the same up
// to 0x8).
typedef struct TerPolyRef {
    u16  nVertex;               // 0x0  first vertex, low 16 bits. TW06: uiVertexIndex
    u16  n2;                    // 0x2  TW06: uiSourceObject
    u8   u4;                    // 0x4  TW06: flags
    u8   nTris;                 // 0x5  TW06: uiPolygonCount
    u8   nSurface;              // 0x6  row of gSurfaceTypes. TW06: uiMaterialType
    u8   nVertexHi;             // 0x7  first vertex, high bits. TW06: uiVertexIndexHi
} TerPolyRef;
// One cell of the ground grid (0xC bytes). TW06: TGD_Cell.
typedef struct TerCell {
    s16  nMaxHeight;            // 0x0  highest ground in the cell, whole yards. TW06: maxHeightYards
    s16  nMinHeight;            // 0x2  TW06: minHeightYards
    u32  uRefs;                 // 0x4  its strips: the first TerPolyRef << 12 | the count.
                                //      TW06: uiPolygonReferenceListOffsetAndCount
    u16  nObjRefs;              // 0x8  TW06: uiObjectReferenceCount
    u16  nObjRefOffset;         // 0xA  TW06: uiObjectReferenceListOffset
} TerCell;

#define TER_FIRST_VERTEX(pRef) (((pRef)->nVertexHi << 16) + (pRef)->nVertex)

// A closed outline on the course (TW06: TNetwork, 0x14 bytes): the free-drop areas and the
// in-bounds outlines. Only the header is read here.
typedef struct TNetwork {
    s16  nExportType;           // 0x0  TW06: ExportType
    s16  nNumNodes;             // 0x2  TW06: NumNodes
} TNetwork;

#define MAX_FREE_DROP_NETWORKS 25
#define MAX_OOB_NETWORKS       5

#define TER_NO_GROUND -65536.125f   // the height the lookups return when nothing is under the point

extern u8        lbl_80281DC0;                          // the cup is real geometry
extern s32       lbl_80281DC4;                          // free-drop networks loaded
extern s32       lbl_80281DC8;                          // out-of-bounds networks loaded
extern TNetwork* lbl_801D5428[MAX_FREE_DROP_NETWORKS];
extern TNetwork* lbl_801D548C[MAX_OOB_NETWORKS];

u8    Course_RegisterLoader(int nChunk, void (*pfn)(u8*));   // 0x8000C0B4
s32   fn_8000C140(f32* pPos, TNetwork* pNet, s32 nNodes);   // point in outline. TW06: wn_PnPoly
u8    fn_8000C3C8(f32* pFrom, f32* pTo, TNetwork* pNet, s32 nNodes, f32* pHit);   // segment crosses outline
void  fn_8004B588(TNetwork* pNet);
void  fn_8004B63C(TNetwork* pNet);
void  Vec3Copy(f32* pSrc, f32* pDst);                     // 0x80008304
void  vec4flt_CrossProduct(f32* pA, f32* pB, f32* pOut);
void  fn_800BAF04(f32* pSrc, f32* pDst);                  // normalise
void  fn_8005097C(f32* pA, f32* pB, f32* pOut);           // a - b (paired-single assembly)
void  fn_800509A0(f32* pSrc, f32* pDst);                  // negate (paired-single assembly)
// The ground triangle under a point: its height there, the grid cell, the strip, the triangle's
// first vertex and its number in the strip. Probably TW06's Ter_GetSupportingGroundTriangle.
f32   fn_800CBEE0(CourseInfo* pCourse, f32* pPos, TerCell** ppCell, TerPolyRef** ppRef, f32 (**ppTri)[3],
                  s32* pTri);
f32   fn_80035074(f32 x);                                 // floor
f32   fn_8004CB30(CourseInfo* pCourse, f32* pPos, TerCell** ppCell, TerPolyRef** ppRef, f32 (**ppTri)[3],
                  s32* pTri);
f32   fn_8004CD94(CourseInfo* pCourse, f32* pPos, TerCell** ppCell, TerPolyRef** ppRef, f32 (**ppTri)[3]);
u8    fn_80050A9C(f32* pA, f32* pB, f32* pC, f32 fX, f32 fZ);
void  fn_800509D8(f32* pTri, f32* pPos, f32* pA, f32* pB, f32* pC);

// Probably TW06's Ter_Init (the same size and file): register the course-file loaders for the
// out-of-bounds (chunk 1) and free-drop (chunk 4) networks and forget the old ones.
void fn_8004B1A4(void) {
    Course_RegisterLoader(1, (void (*)(u8*))fn_8004B63C);
    Course_RegisterLoader(4, (void (*)(u8*))fn_8004B588);
    lbl_80281DC8 = 0;
    lbl_80281DC4 = 0;
}

// TW06: bool Ter_Use3DCupGeometry(void). Whether the cup is real geometry the ball drops into;
// without it, GameRound.c holes a ball that stops within half a yard of the pin.
u8 fn_8004B580(void) {
    return lbl_80281DC0;
}

// TW06: void Ter_FreeDropNetworkLoadCallback(TNetwork*).
void fn_8004B588(TNetwork* pNet) {
    lbl_801D5428[lbl_80281DC4] = pNet;
    lbl_80281DC4++;
}

// TW06: bool Ter_PointInFreeDropNetwork(f32*). Whether a point is inside a free-drop area.
u8 Ter_PointInFreeDropNetwork(f32* pPos) {
    int i;

    if (lbl_80281DC4 == 0) return 0;
    for (i = 0; i < lbl_80281DC4; i++) {
        if (fn_8000C140(pPos, lbl_801D5428[i], lbl_801D5428[i]->nNumNodes)) return 1;
    }
    return 0;
}

// TW06: void Ter_OOBNetworkLoadCallback(TNetwork*).
void fn_8004B63C(TNetwork* pNet) {
    lbl_801D548C[lbl_80281DC8] = pNet;
    lbl_80281DC8++;
}

// TW06: s32 Ter_iNumOOBNetworksLoaded(void).
s32 fn_8004B65C(void) {
    return lbl_80281DC8;
}

// TW06: bool Ter_PointInOOBNetwork(f32*). Whether a point is inside one of the outlines; a course
// with none loaded has no out of bounds, so every point is inside.
u8 Ter_PointInOOBNetwork(f32* pPos) {
    int i;

    if (lbl_80281DC8 == 0) return 1;
    for (i = 0; i < lbl_80281DC8; i++) {
        if (fn_8000C140(pPos, lbl_801D548C[i], lbl_801D548C[i]->nNumNodes)) return 1;
    }
    return 0;
}

// TW06: bool Ter_CollisionWithOOBNetwork(const f32*, const f32*, f32*). Whether the segment from
// pFrom to pTo crosses one of the outlines, and where.
u8 fn_8004B6F8(f32* pFrom, f32* pTo, f32* pHit) {
    int i;

    if (lbl_80281DC8 == 0) return 0;
    for (i = 0; i < lbl_80281DC8; i++) {
        if (fn_8000C3C8(pFrom, pTo, lbl_801D548C[i], lbl_801D548C[i]->nNumNodes, pHit)) return 1;
    }
    return 0;
}

// Lies from which a drop may be taken: the fairways and the roughs.
u8 Ter_LieIsPreferred(u32 nLie) {
    if (nLie == LIE_FAIRWAY_e || nLie == LIE_FAIRWAY_TIGHT_e || nLie == LIE_ROUGH_HIGH_e
        || nLie == LIE_ROUGH_e) {
        return 1;
    }
    return 0;
}

// TW06: bool Ter_IsValidDropSurface(s32). Surfaces of class 3, 12 and 18.
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
// ground strips count (not objects), and only those used on the current hole: a strip with
// hole bits in its flags is skipped on other holes, and one with 0x10 in split screen.
f32 fn_8004C8E0(CourseInfo* pCourse, f32* pPos, TerCell** ppCell, TerPolyRef** ppRef, f32 (**ppTri)[3],
                s32* pTri) {
    u32 uHole = 1 << Game_CurrentHole();
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
        pRef = &pCourse->pPolyRefs[pCell->uRefs >> 12];
        for (i = (pCell->uRefs & 0xFFF) - 1; i >= 0; i--) {
            if (pRef->n2 != 0) {
                pRef++;
            } else if (pRef->u4 != 0
                       && (!(pRef->u4 & uHole) || ((pRef->u4 & 0x10) && gSession.nSplitScreen))) {
                pRef++;
            } else {
                pVert = &pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
                pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
                for (j = pRef->nTris - 1; j >= 0; j--) {
                    if ((pFlags[2] & 7) && fBest < pVert[(pFlags[2] >> 4) & 3][1]
                        && fn_80050A9C(pVert[0], pVert[1], pVert[2], pPos[0], pPos[2])) {
                        fn_800509D8(pVert[0], pPos, &fA, &fB, &fC);
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

// The height of the highest ground under a point. Probably TW06's Ter_GetHighestGroundHeight.
f32 fn_8004D5C0(CourseInfo* pCourse, f32* pPos) {
    TerCell* pCell;
    TerPolyRef* pRef;
    f32 (*pTri)[3];
    s32 nTri;

    return fn_8004C8E0(pCourse, pPos, &pCell, &pRef, &pTri, &nTri);
}

// The same through fn_8004CB30.
f32 fn_8004D5F0(CourseInfo* pCourse, f32* pPos) {
    TerCell* pCell;
    TerPolyRef* pRef;
    f32 (*pTri)[3];
    s32 nTri;

    return fn_8004CB30(pCourse, pPos, &pCell, &pRef, &pTri, &nTri);
}

// The height of the ground that supports a point. Probably TW06's Ter_GetSupportingGroundHeight.
f32 fn_8004D620(CourseInfo* pCourse, f32* pPos) {
    TerCell* pCell;
    TerPolyRef* pRef;
    f32 (*pTri)[3];
    s32 nTri;

    return fn_800CBEE0(pCourse, pPos, &pCell, &pRef, &pTri, &nTri);
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

    if (fn_800CBEE0(pCourse, pPos, &pCell, &pRef, &pTri, &nTri) != TER_NO_GROUND) {
        Vec3Copy(pTri[0], vA);
        Vec3Copy(pTri[1], vB);
        Vec3Copy(pTri[2], vC);
        fn_8005097C(vA, vB, vAB);
        fn_8005097C(vB, vC, vBC);
        vec4flt_CrossProduct(vAB, vBC, pNormal);
        fn_800BAF04(pNormal, pNormal);
        if (pNormal[1] < 0.0f) {
            fn_800509A0(pNormal, pNormal);
        }
        return 1;
    }
    return 0;
}

// TW06: TGD_MaterialInfo* Ter_GetSupportingWorldMaterial(TGD_TerrainInfo*, f32*). The surface
// under a point, objects included; NULL when there is none.
SurfaceType* Ter_GetSupportingWorldMaterial(CourseInfo* pCourse, f32* pPos) {
    TerCell* pCell;
    TerPolyRef* pRef;
    f32 (*pTri)[3];

    if (fn_8004CD94(pCourse, pPos, &pCell, &pRef, &pTri) != TER_NO_GROUND) {
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

    fHeight = fn_800CBEE0(pCourse, pPos, &pCell, &pRef, &pTri, &nTri);
    if (fHeight != TER_NO_GROUND) {
        Vec3Copy(pTri[0], vA);
        Vec3Copy(pTri[1], vB);
        Vec3Copy(pTri[2], vC);
        fn_8005097C(vA, vB, vAB);
        fn_8005097C(vB, vC, vBC);
        vec4flt_CrossProduct(vAB, vBC, pNormal);
        fn_800BAF04(pNormal, pNormal);
        if (pNormal[1] < 0.0f) {
            fn_800509A0(pNormal, pNormal);
        }
        *ppSurface = &gSurfaceTypes[pRef->nSurface];
    } else {
        *ppSurface = NULL;
    }
    return fHeight;
}

// TW06: Ter_GetBarycentricCoords (an inline in goterrainutils.h there). The weights of a point
// against a triangle's three corners, in the x-z plane.
void fn_800509D8(f32* pTri, f32* pPos, f32* pA, f32* pB, f32* pC) {
    f32 fX;
    f32 fZ;
    f32 fInv;

    fX = pPos[0];
    fZ = pPos[2];
    fInv = 1.0f / ((pTri[3] - pTri[0]) * (pTri[8] - pTri[2]) - (pTri[6] - pTri[0]) * (pTri[5] - pTri[2]));
    *pA = fInv * ((pTri[3] - fX) * (pTri[8] - fZ) - (pTri[6] - fX) * (pTri[5] - fZ));
    *pB = fInv * ((pTri[6] - fX) * (pTri[2] - fZ) - (pTri[0] - fX) * (pTri[8] - fZ));
    *pC = fInv * ((pTri[0] - fX) * (pTri[5] - fZ) - (pTri[3] - fX) * (pTri[2] - fZ));
}

// TW06: bool Ter_PointInTriangleXZpY(f32*, f32*, f32*, f32, f32, f32*). Whether (x, z) lies inside
// the triangle abc seen from above, either way round; a flat (edge-on) triangle holds nothing.
u8 fn_80050A9C(f32* pA, f32* pB, f32* pC, f32 fX, f32 fZ) {
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

// TW06: s32 MaterialTypes::getMaterialID(const TGD_MaterialInfo*). A surface's row in
// gSurfaceTypes, or -1.
s32 fn_80050BEC(SurfaceType* pSurface) {
    s32 n;

    if (pSurface == NULL) return -1;
    n = ((u8*)pSurface - (u8*)gSurfaceTypes) / sizeof(SurfaceType);
    if (n < 0 || n >= NUM_SURFACE_TYPES) return -1;
    return n;
}
