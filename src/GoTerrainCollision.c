// GoTerrainCollision.c (TW06's goterraincollision.c): the course's ground as collision data
// (CourseInfo, TW06's TGD_TerrainInfo): the free-drop and out-of-bounds networks, the drop
// checks, heights, normals and surfaces under a point, and the ball's collision tests against the
// ground, the pin and the course objects. The ground is triangle strips found through a grid;
// each triangle's surface is a row of gSurfaceTypes. The file starts at 0x8004AFA0 (the code from
// there on shares one constant pool) and ends where Ball.c begins.

#include "golfer.h"
#include "game.h"
#include "engine.h"

#define AXIS3(n) ((n) == 0 ? 0 : 2)    // grid axis 0 (x) or 1 (z) as an index into a 3D vector
#define PIN_RADIUS_SQ 0.00077160494f   // the flagstick's radius squared: (1 inch)^2 in square yards
#define TER_RELOCATE(pCourse, field) ((pCourse)->field = (void*)((u8*)(pCourse) + (u32)(pCourse)->field))

u8    Course_RegisterLoader(int nChunk, void (*pfn)(u8*));   // 0x8000C0B4
s32   fn_8000C140(f32* pPos, TNetwork* pNet, s32 nNodes);   // point in outline. TW06: wn_PnPoly
u8    fn_8000C3C8(f32* pFrom, f32* pTo, TNetwork* pNet, s32 nNodes, f32* pHit);   // segment crosses outline
void  fn_8004B588(TNetwork* pNet);
u8    Ter_LieIsPreferred(u32 nClass);
void  fn_8004B63C(TNetwork* pNet);
void  vec4flt_CrossProduct(f32* pA, f32* pB, f32* pOut);
f32   fn_8000C5FC(f32* pA, f32* pB);                      // dot product
void  fn_8005097C(f32* pA, f32* pB, f32* pOut);           // a - b (paired-single assembly)
void  fn_800509A0(f32* pSrc, f32* pDst);                  // negate (paired-single assembly)
void  fn_800509BC(f32* pSrc, f32* pDst);                  // negate, four floats (paired-single assembly)
// The ground triangle under a point: its height there, the grid cell, the strip, the triangle's
// first vertex and its number in the strip. Probably TW06's Ter_GetSupportingGroundTriangle.
f32   fn_800CBEE0(CourseInfo* pCourse, f32* pPos, TerCell** ppCell, TerPolyRef** ppRef, f32 (**ppTri)[3],
                  s32* pTri);
f32   fn_80035074(f32 x);                                 // floor

// The grid cell a coordinate falls in (it may be outside the grid). The four line walkers go
// through this helper; the point lookups write the cast out (each matches only its own way).
static inline int Ter_GridCell(f32 fCells) {
    return (int)fn_80035074(fCells);
}

f32   fn_8004C8E0(CourseInfo* pCourse, f32* pPos, TerCell** ppCell, TerPolyRef** ppRef, f32 (**ppTri)[3],
                  s32* pTri);
void  fn_80050794(CourseInfo* pCourse);
f32   Ter_CalcLowestPlayableWorldHeight(CourseInfo* pCourse);
f32   fn_8004CB30(CourseInfo* pCourse, f32* pPos, TerCell** ppCell, TerPolyRef** ppRef, f32 (**ppTri)[3],
                  s32* pTri);
f32   fn_8004D01C(CourseInfo* pCourse, f32* pPos, TerCell** ppCell, TerPolyRef** ppRef, f32 (**ppTri)[3],
                  s32* pTri);
void  fn_8004D2E0(CourseInfo* pCourse, f32* pPos, TerPolyRef** ppRefLow, f32* pLow, f32 (**ppTriLow)[3],
                  TerPolyRef** ppRefHigh, f32* pHigh, f32 (**ppTriHigh)[3]);
f32   fn_800BB028(f32* pA, f32* pB);                      // squared distance
u8    Ter_CheckForGroundCollision(CourseInfo* pCourse, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal,
                                  SurfaceType** ppSurface, TerObject** ppObj);
u8    fn_8004EB7C(CourseInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo, f32* pDir, f32 fMax, f32* pHit,
                  f32* pNormal, SurfaceType** ppSurface, TerObject** ppObj, u8* pbFlags);
u8    fn_8004F43C(CourseInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo, f32* pDir, f32 fMax, f32* pHit,
                  f32* pNormal, SurfaceType** ppSurface, TerObject** ppObj);
u8    fn_8004FCB4(CourseInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo, f32* pDir, f32 fMax, f32* pHit,
                  f32* pNormal, SurfaceType** ppSurface, TerObject** ppObj);
u8    fn_800504F4(CourseInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo, f32* pDir, f32 fMax, f32* pHit,
                  f32* pNormal, SurfaceType** ppSurface, TerObject** ppObj);
u8    fn_8004E0D4(f32* pFrom, f32* pDir, f32 fRange, f32* pCentre, f32 fRadius);
u8    fn_80050A9C(f32* pA, f32* pB, f32* pC, f32 fX, f32 fZ);
void  fn_800509D8(f32* pTri, f32* pPos, f32* pA, f32* pB, f32* pC);

// TW06: bool Ter_LineTriangleIntersection(f32*, f32*, f32, f32**, f32*, f32[4]*, f32[4]*). Where the
// line from pFrom along pDir meets a triangle, as a fraction t of pDir (0 < t < fMax): t, the point
// and the triangle's normal. A line along the triangle's plane never meets it.
u8 fn_8004AFA0(f32* pFrom, f32* pDir, f32 fMax, f32 (*pTri)[3], f32* pT, f32* pHit, f32* pNormal) {
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
    fn_8005097C(vB, vA, vE1);
    fn_8005097C(vC, vA, vE2);
    vec4flt_CrossProduct(pDir, vE2, vP);
    fDet = fn_8000C5FC(vE1, vP);
    if (fDet > -0.00001f && fDet < 0.00001f) return 0;
    fInv = 1.0f / fDet;
    fn_8005097C(pFrom, vA, vS);
    fU = fInv * fn_8000C5FC(vS, vP);
    if (fU < 0.0f || fU > 1.0f) return 0;
    vec4flt_CrossProduct(vS, vE1, vQ);
    fV = fInv * fn_8000C5FC(pDir, vQ);
    if (fV < 0.0f || fU + fV > 1.0f) return 0;
    fT = fInv * fn_8000C5FC(vE2, vQ);
    if (fT <= 0.0f || fT >= fMax) return 0;
    *pT = fT;
    pHit[0] = fT * pDir[0] + pFrom[0];
    pHit[1] = fT * pDir[1] + pFrom[1];
    pHit[2] = fT * pDir[2] + pFrom[2];
    vec4flt_CrossProduct(vE1, vE2, pNormal);
    fn_800BAF04(pNormal, pNormal);
    return 1;
}

// Probably TW06's Ter_Init (the same size and file): register the course-file loaders for the
// out-of-bounds (chunk 1) and free-drop (chunk 4) networks and forget the old ones.
void fn_8004B1A4(void) {
    Course_RegisterLoader(1, (void (*)(u8*))fn_8004B63C);
    Course_RegisterLoader(4, (void (*)(u8*))fn_8004B588);
    lbl_80281DC8 = 0;
    lbl_80281DC4 = 0;
}

// TW06: void Ter_InitTGD(TGD_TerrainInfo*). Get a course's collision data ready once it is loaded:
// its offsets become pointers; strips flagged for one of the four pin positions (flags 1, 2, 4, 8)
// are 3D cup geometry, and each pin goes at the centre top of its geometry's bounds; then the
// triangles' high and low corners and the course floor.
void fn_8004B1EC(CourseInfo* pCourse) {
    u32 i;
    int k;
    int j;
    int nBit;
    int nPinSet;
    f32 (*pVert)[3];

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
    lbl_80281DC0 = 0;
    for (k = 0; k < NUM_CUP_POSITIONS; k++) {
        lbl_801D53A8[k].vMin[0] = 1000000.0f;
        lbl_801D53A8[k].vMin[1] = 1000000.0f;
        lbl_801D53A8[k].vMin[2] = 1000000.0f;
        lbl_801D53A8[k].vMax[0] = -1000000.0f;
        lbl_801D53A8[k].vMax[1] = -1000000.0f;
        lbl_801D53A8[k].vMax[2] = -1000000.0f;
    }
    for (i = 0; i < pCourse->nPolyRefs; i++) {
        nBit = 1;
        for (k = 0; k < NUM_CUP_POSITIONS; k++) {
            if (nBit == (pCourse->pPolyRefs[i].u4 & 0xF)) {
                lbl_80281DC0 = 1;
                pVert = &pCourse->pVerts[TER_FIRST_VERTEX(&pCourse->pPolyRefs[i])];
                for (j = 0; j < pCourse->pPolyRefs[i].nTris + 2; j++) {
                    if (pVert[0][0] < lbl_801D53A8[k].vMin[0]) {
                        lbl_801D53A8[k].vMin[0] = pVert[0][0];
                    }
                    if (pVert[0][1] < lbl_801D53A8[k].vMin[1]) {
                        lbl_801D53A8[k].vMin[1] = pVert[0][1];
                    }
                    if (pVert[0][2] < lbl_801D53A8[k].vMin[2]) {
                        lbl_801D53A8[k].vMin[2] = pVert[0][2];
                    }
                    if (pVert[0][0] > lbl_801D53A8[k].vMax[0]) {
                        lbl_801D53A8[k].vMax[0] = pVert[0][0];
                    }
                    if (pVert[0][1] > lbl_801D53A8[k].vMax[1]) {
                        lbl_801D53A8[k].vMax[1] = pVert[0][1];
                    }
                    if (pVert[0][2] > lbl_801D53A8[k].vMax[2]) {
                        lbl_801D53A8[k].vMax[2] = pVert[0][2];
                    }
                    pVert++;
                }
            }
            nBit <<= 1;
        }
    }
    if (lbl_80281DC0) {
        nPinSet = Game_CurrentPinSet();
        for (k = 0; k < NUM_CUP_POSITIONS; k++) {
            gpGame->nPinSet[Game_CurHoleIndex()] = k;
            lbl_801D3CB0.pCourse->pin[k].x = (lbl_801D53A8[k].vMin[0] + lbl_801D53A8[k].vMax[0]) / 2.0f;
            lbl_801D3CB0.pCourse->pin[k].z = (lbl_801D53A8[k].vMin[2] + lbl_801D53A8[k].vMax[2]) / 2.0f;
            lbl_801D3CB0.pCourse->pin[k].y = lbl_801D53A8[k].vMax[1];
            lbl_801D3CB0.pCourse->pin[k].w = 1.0f;
        }
        gpGame->nPinSet[Game_CurHoleIndex()] = nPinSet;
    }
    gpGame->p130 = &lbl_801D3CB0.pCourse->pin[Game_CurrentPinSet()].x;
    lbl_80281DC8 = 0;
    lbl_80281DC4 = 0;
    fn_80050794(lbl_801D3CB0.pCourse);
    lbl_801D3CB0.pCourse->fFloor = Ter_CalcLowestPlayableWorldHeight(lbl_801D3CB0.pCourse);
}

// TW06: bool Ter_Use3DCupGeometry(void). Whether the cup is real geometry the ball drops into;
// without it, GameRound.c holes a ball that stops within half a yard of the pin.
u8 Ter_Use3DCupGeometry(void) {
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
s32 Ter_iNumOOBNetworksLoaded(void) {
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

// TW06: f32 Ter_GetAmbientLight(TGD_TerrainInfo*, f32*). The light on the ground at a point, 0..1,
// from the brightness byte of the triangle's vertex: the ground covering the point when it is
// less than a quarter of a yard above it, else the ground under it, else the lowest ground. 1
// (full light) with no ground, or on an object.
f32 fn_8004B78C(CourseInfo* pCourse, f32* pPos) {
    TerCell* pCell;
    TerPolyRef* pRef;
    f32 (*pTri)[3];
    s32 nTri;
    f32 fHeight;

    fHeight = fn_8004D01C(pCourse, pPos, &pCell, &pRef, &pTri, &nTri);
    if (fHeight == TER_NO_GROUND || fHeight > pPos[1] + 0.25f) {
        fHeight = fn_800CBEE0(pCourse, pPos, &pCell, &pRef, &pTri, &nTri);
        if (fHeight == TER_NO_GROUND) {
            fHeight = fn_8004CB30(pCourse, pPos, &pCell, &pRef, &pTri, &nTri);
        }
    }
    if (fHeight != TER_NO_GROUND && pRef->n2 == 0) {
        return (pCourse->pLight + TER_FIRST_VERTEX(pRef))[nTri] / 255.0f;
    }
    return 1.0f;
}

// TW06: Ter_SearchAreaForDropLocation (its parameters differ here). Where a player's ball is to be
// dropped (pOut): the last good drop spot (lbl_801D5888), or with bPreferred the last spot with a
// preferred lie (lbl_801D58C8) when that is not the shot's own start and is less than 10 yards
// further away. If the spot is over 3 yards off, or (not preferred) on another class of surface
// than the ball, search rings of 1 to 4 yards around the ball, every 45 degrees starting towards
// the pin, for a drop on the same class, else the nearest. Returns 0 when the spot is where the
// shot started, or (with bCheck) within 50 yards of vA44.
// Not exact yet (96.6%): the frame is 0x10 bigger and the saved registers differ; unfinished
// when work was frozen.
u8 Ter_SearchAreaForDropLocation(int nPlayer, u8 bPreferred, u8 bCheck, f32* pOut) {
    f32 vPos[4];
    f32 vDir[4];
    SurfaceType* pSurface;
    u8 bDrop;
    u8 bPreferredLie;
    Player* p = &gPlayers[nPlayer];
    CourseInfo* pCourse = fn_8000C594();
    f32 fDist;
    f32 fDropDist;
    f32 fAngle;
    f32 fTurn;
    f32 fRadius;
    f32 fLift;
    f32 fSin;
    f32 fHeading;
    int nRing;
    SurfaceType* pGround;

    fDist = fn_800BB028(&pCourse->pin[Game_CurrentPinSet()].x, p->ball.vStart);
    if (fn_800BB028(p->ball.vPos, p->ball.vStart) > fDist) {
        bPreferred = 0;                 // past the pin
    }
    if (bPreferred) {
        fDist = Vec_Distance(lbl_801D58C8[nPlayer], p->ball.vPos);
        fDropDist = Vec_Distance(lbl_801D5888[nPlayer], p->ball.vPos);
        if ((p->ball.vStart[0] != lbl_801D58C8[nPlayer][0] || p->ball.vStart[1] != lbl_801D58C8[nPlayer][1]
             || p->ball.vStart[2] != lbl_801D58C8[nPlayer][2])
            && fDist - fDropDist < 10.0f) {
            Vec_Copy(lbl_801D58C8[nPlayer], pOut);
        } else {
            Vec_Copy(lbl_801D5888[nPlayer], pOut);
        }
    } else {
        Vec_Copy(lbl_801D5888[nPlayer], pOut);
    }
    vPos[0] = pOut[0];
    vPos[1] = 1000000.0f;
    vPos[2] = pOut[2];
    vPos[3] = 1.0f;
    pGround = fn_800CC190(pCourse, vPos);
    fDist = fn_800BB028(pOut, p->ball.vPos);
    if (fDist > 9.0f
        || (!bPreferred && pGround->nClass != gSurfaceTypes[p->ball.nSurface].nClass)) {
        fn_8005097C(&pCourse->pin[Game_CurrentPinSet()].x, p->ball.vPos, vDir);
        fn_800BAF04(vDir, vDir);
        fHeading = fn_8000AD78(vDir[0], vDir[2]);
        fRadius = 1.0f;
        for (nRing = 0; nRing < 4; nRing++) {
            fLift = 2.0f * fRadius;
            for (fTurn = 0.0f; fTurn <= 6.265732f; fTurn += 0.7853982f) {
                fAngle = fHeading + fTurn;
                fSin = fn_800095F0(fAngle);
                vPos[0] = fn_80009638(fAngle) * fRadius + p->ball.vPos[0];
                vPos[2] = fSin * fRadius + p->ball.vPos[2];
                // from water, only straight towards the pin
                if (gSurfaceTypes[p->ball.nSurface].nClass == 7 && fTurn > 0.0f) continue;
                vPos[1] = p->ball.vPos[1] + fLift;
                vPos[1] = Ter_CheckForDropLocation(pCourse, vPos, 0, &bDrop, &bPreferredLie, &pSurface);
                if ((bPreferred && bPreferredLie) || (!bPreferred && bDrop)) {
                    if (pSurface->nClass == gSurfaceTypes[p->ball.nSurface].nClass) {
                        Vec_Copy(vPos, pOut);
                        goto done;  // fake match: leaves both loops (not yet verified)
                    }
                    if (fRadius < fDist) {      // EA bug: a distance against a squared one
                        Vec_Copy(vPos, pOut);
                        fDist = fRadius;
                    }
                }
            }
            fRadius += 1.0f;
        }
    }
done:
    if (p->ball.vStart[0] == pOut[0] && p->ball.vStart[2] == pOut[2]) return 0;
    if (bCheck && p->vA44[0] == p->fBallX && p->vA44[2] == p->fBallZ
        && fn_800BB028(pOut, p->vA44) < 2500.0f) {
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
              && pSurface->nClass != 10 && fn_8000AD9C(vNormal[1]) > 0.86603f
              && Ter_PointInOOBNetwork(vPos) && !Ter_PointInFreeDropNetwork(vPos)
              && (bOnDropSurface || !Ter_CheckObjectAndHazardObstruction(vPos, 1.5f, 0, 1, 2.0f, 1, 0.577f));
    } else {
        fHeight = 0.0f;
        if (ppSurface != NULL) {
            *ppSurface = NULL;
        }
        bOk = Ter_PointInOOBNetwork(vPos) && !Ter_PointInFreeDropNetwork(vPos);
    }
    vPos[1] = fHeight;
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
// the lie ID; not a Lie_t) is a preferred place for a drop: classes 1 to 4, the ones Ball_SetLie
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
// ground strips count (not objects), and only those used on the current hole: a strip with
// hole bits in its flags is skipped on other holes, and one with 0x10 in split screen.
f32 fn_8004C8E0(CourseInfo* pCourse, f32* pPos, TerCell** ppCell, TerPolyRef** ppRef, f32 (**ppTri)[3],
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

// The lowest ground triangle under a point (x, z), with the same outputs and strip rules as
// fn_8004C8E0; TER_NO_GROUND when there is none.
f32 fn_8004CB30(CourseInfo* pCourse, f32* pPos, TerCell** ppCell, TerPolyRef** ppRef, f32 (**ppTri)[3],
                s32* pTri) {
    u32 uPinSet = 1 << Game_CurrentPinSet();
    f32 fBest = 50000.0f;
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
                       && (!(pRef->u4 & uPinSet) || ((pRef->u4 & 0x10) && gSession.nSplitScreen))) {
                pRef++;
            } else {
                pVert = &pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
                pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
                for (j = pRef->nTris - 1; j >= 0; j--) {
                    if ((pFlags[2] & 7) && fBest > pVert[(pFlags[2] >> 6) & 3][1]
                        && fn_80050A9C(pVert[0], pVert[1], pVert[2], pPos[0], pPos[2])) {
                        fn_800509D8(pVert[0], pPos, &fA, &fB, &fC);
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

// The highest triangle under a point at or below its height, objects included (so a ball on a
// bridge is on the bridge); TER_NO_GROUND when there is none. Probably TW06's
// Ter_GetSupportingWorldTriangle.
f32 fn_8004CD94(CourseInfo* pCourse, f32* pPos, TerCell** ppCell, TerPolyRef** ppRef, f32 (**ppTri)[3]) {
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
                        && fn_80050A9C(pVert[0], pVert[1], pVert[2], pPos[0], pPos[2])) {
                        fn_800509D8(pVert[0], pPos, &fA, &fB, &fC);
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

// The lowest ground triangle under a point at or above its height (the ground covering it);
// TER_NO_GROUND when there is none. Probably TW06's Ter_GetCoveringGroundTriangle.
f32 fn_8004D01C(CourseInfo* pCourse, f32* pPos, TerCell** ppCell, TerPolyRef** ppRef, f32 (**ppTri)[3],
                s32* pTri) {
    u32 uPinSet = 1 << Game_CurrentPinSet();
    f32 fBest = 65536.0f;
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
                        && fn_80050A9C(pVert[0], pVert[1], pVert[2], pPos[0], pPos[2])) {
                        fn_800509D8(pVert[0], pPos, &fA, &fB, &fC);
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
void fn_8004D2E0(CourseInfo* pCourse, f32* pPos, TerPolyRef** ppRefLow, f32* pLow, f32 (**ppTriLow)[3],
                 TerPolyRef** ppRefHigh, f32* pHigh, f32 (**ppTriHigh)[3]) {
    u32 uPinSet = 1 << Game_CurrentPinSet();
    f32 fHigh = 65536.0f;
    f32 fLow = -65536.0f;
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
                       && (!(pRef->u4 & uPinSet) || ((pRef->u4 & 0x10) && gSession.nSplitScreen))) {
                pRef++;
            } else {
                pVert = &pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
                pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
                for (j = pRef->nTris - 1; j >= 0; j--) {
                    if ((pFlags[2] & 7)
                        && (fHigh > pVert[(pFlags[2] >> 6) & 3][1] || fLow < pVert[(pFlags[2] >> 4) & 3][1])
                        && fn_80050A9C(pVert[0], pVert[1], pVert[2], pPos[0], pPos[2])) {
                        fn_800509D8(pVert[0], pPos, &fA, &fB, &fC);
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

// The height of the ground covering a point, with that triangle's upward normal. Probably TW06's
// Ter_GetCoveringGroundHeightAndNormal.
f32 fn_8004D650(CourseInfo* pCourse, f32* pPos, f32* pNormal) {
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

    fHeight = fn_8004D01C(pCourse, pPos, &pCell, &pRef, &pTri, &nTri);
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

// The height of whatever supports a point, objects included. Probably TW06's
// Ter_GetSupportingWorldHeight.
f32 fn_8004D80C(CourseInfo* pCourse, f32* pPos) {
    TerCell* pCell;
    TerPolyRef* pRef;
    f32 (*pTri)[3];

    return fn_8004CD94(pCourse, pPos, &pCell, &pRef, &pTri);
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

// TW06: void Ter_GetEnclosingGroundHeight(TGD_TerrainInfo*, f32*, f32*, f32*). The heights of the
// ground just below and just above a point (TER_NO_GROUND for none).
void Ter_GetEnclosingGroundHeight(CourseInfo* pCourse, f32* pPos, f32* pLow, f32* pHigh) {
    TerPolyRef* pRefLow;
    TerPolyRef* pRefHigh;
    f32 (*pTriLow)[3];
    f32 (*pTriHigh)[3];

    fn_8004D2E0(pCourse, pPos, &pRefLow, pLow, &pTriLow, &pRefHigh, pHigh, &pTriHigh);
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

    fn_8004D2E0(pCourse, pPos, &pRefLow, pLow, &pTriLow, &pRefHigh, pHigh, &pTriHigh);
    if (*pLow != TER_NO_GROUND) {
        Vec3Copy(pTriLow[0], vA);
        Vec3Copy(pTriLow[1], vB);
        Vec3Copy(pTriLow[2], vC);
        fn_8005097C(vA, vB, vAB);
        fn_8005097C(vB, vC, vBC);
        vec4flt_CrossProduct(vAB, vBC, pNormalLow);
        fn_800BAF04(pNormalLow, pNormalLow);
        if (pNormalLow[1] < 0.0f) {
            fn_800509A0(pNormalLow, pNormalLow);
        }
        *ppSurfaceLow = &gSurfaceTypes[pRefLow->nSurface];
    } else {
        *ppSurfaceLow = NULL;
    }
    if (*pHigh != TER_NO_GROUND) {
        Vec3Copy(pTriHigh[0], vA);
        Vec3Copy(pTriHigh[1], vB);
        Vec3Copy(pTriHigh[2], vC);
        fn_8005097C(vA, vB, vAB2);
        fn_8005097C(vB, vC, vBC2);
        vec4flt_CrossProduct(vAB2, vBC2, pNormalHigh);
        fn_800BAF04(pNormalHigh, pNormalHigh);
        if (pNormalHigh[1] < 0.0f) {
            fn_800509A0(pNormalHigh, pNormalHigh);
        }
        *ppSurfaceHigh = &gSurfaceTypes[pRefHigh->nSurface];
    } else {
        *ppSurfaceHigh = NULL;
    }
}

// TW06: f32 Ter_GetSupportingWorldData(TGD_TerrainInfo*, f32*, TGD_MaterialInfo**, f32*). The
// height of whatever supports a point (objects included), with its surface and upward normal;
// TER_NO_GROUND and no surface when there is none.
f32 fn_8004DBB0(CourseInfo* pCourse, f32* pPos, SurfaceType** ppSurface, f32* pNormal) {
    f32 vA[4];
    f32 vB[4];
    f32 vC[4];
    f32 vAB[4];
    f32 vBC[4];
    TerCell* pCell;
    TerPolyRef* pRef;
    f32 (*pTri)[3];
    f32 fHeight;

    fHeight = fn_8004CD94(pCourse, pPos, &pCell, &pRef, &pTri);
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

// TW06: u32 Ter_GetTerrainLayers(TGD_TerrainInfo*, f32*, TGD_MaterialInfo**, f32*, u32). Every
// ground triangle over or under a point (x, z), up to nMax: their heights and surfaces, in the
// order found. Returns how many.
u32 fn_8004DCC4(CourseInfo* pCourse, f32* pPos, SurfaceType** ppSurfaces, f32* pHeights, u32 nMax) {
    u32 uPinSet = 1 << Game_CurrentPinSet();
    u32 nFound = 0;
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
                    if ((pFlags[2] & 7) && fn_80050A9C(pVert[0], pVert[1], pVert[2], pPos[0], pPos[2])) {
                        fn_800509D8(pVert[0], pPos, &fA, &fB, &fC);
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

// Mark in lbl_801D54A0 the objects of grid cell (nX, nZ) that the line from pFrom along pDir
// passes within fRange of (entry 0 is always marked).
void fn_8004DF10(CourseInfo* pCourse, f32* pFrom, f32* pDir, int nX, int nZ, f32 fRange) {
    int i;
    u16* pRefs;
    TerCell* pCell;
    TerObject* pObj;

    lbl_801D54A0[0] = 1;
    for (i = 1; i < MAX_OBJECTS; i++) {
        lbl_801D54A0[i] = 0;
    }
    if (nX >= 0 && nX < pCourse->nGridWidth && nZ >= 0 && nZ < pCourse->nGridLength) {
        pCell = &pCourse->pGrid[nX + nZ * pCourse->nGridWidth];
        pRefs = &pCourse->pObjRefs[pCell->nObjRefOffset];
        for (i = pCell->nObjRefs - 1; i >= 0; i--) {
            pObj = &pCourse->pObjects[*pRefs];
            if (fn_8004E0D4(pFrom, pDir, fRange, pObj->vCentre, pObj->fRadius)) {
                lbl_801D54A0[*pRefs] = 1;
            }
            pRefs++;
        }
    }
}

// TW06: bool Ter_LineSphereIntersection(f32*, f32*, f32, f32*, f32). Whether a sphere (centre,
// radius) is within fRange of pFrom and either around it or ahead of it along pDir.
u8 fn_8004E0D4(f32* pFrom, f32* pDir, f32 fRange, f32* pCentre, f32 fRadius) {
    f32 vTo[4];
    f32 vCentre[4];
    f32 fDist;

    vCentre[0] = pCentre[0];
    vCentre[1] = pCentre[1];
    vCentre[2] = pCentre[2];
    vCentre[3] = 1.0f;
    fn_8005097C(vCentre, pFrom, vTo);
    fDist = (f32)fn_80009680(fn_80009744(vTo)) - fRadius;
    if (fDist > fRange) return 0;
    if (fDist < 0.0f) return 1;
    if (fn_8000C5FC(vTo, pDir) < 0.0f) return 0;
    return 1;
}

// TW06: bool Ter_CheckForPinCollision(TGD_TerrainInfo*, s32, f32*, f32*, f32[4]*, f32[4]*,
// TGD_MaterialInfo**, TGD_ObjectInstanceInfo**). Whether the line from pFrom to pTo hits the
// flagstick of the current hole: a vertical cylinder of one inch radius, 2 yards tall, at the pin.
// Not for nobody's ball, nor when the player's view has the flagstick out (view byte 0x275). On
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
    if (fn_80016CFC(gPlayers[nPlayer].nView0)[0x275]) return 0;
    // the line relative to the pin
    vFrom[0] = pFrom[0] - pCourse->pin[Game_CurrentPinSet()].x;
    vFrom[1] = pFrom[1] - pCourse->pin[Game_CurrentPinSet()].y;
    vFrom[2] = pFrom[2] - pCourse->pin[Game_CurrentPinSet()].z;
    vFrom[3] = 1.0f;
    vTo[0] = pTo[0] - pCourse->pin[Game_CurrentPinSet()].x;
    vTo[1] = pTo[1] - pCourse->pin[Game_CurrentPinSet()].y;
    vTo[2] = pTo[2] - pCourse->pin[Game_CurrentPinSet()].z;
    vTo[3] = 1.0f;
    fn_8005097C(vTo, vFrom, vDelta);
    Vec3Copy(vDelta, vFlat);
    vFlat[1] = 0.0f;
    vIn[0] = -vFrom[0];
    vIn[1] = 0.0f;
    vIn[2] = -vFrom[2];
    vIn[3] = 1.0f;
    if (fn_8000C5FC(vIn, vFlat) < 0.0f) return 0;      // moving away from the stick
    fC = vFrom[0] * vFrom[0] + vFrom[2] * vFrom[2];
    if (fC < PIN_RADIUS_SQ) return 0;                   // already inside it
    // where the line meets the cylinder: a t^2 + b t + c = 0
    fA = vDelta[0] * vDelta[0] + vDelta[2] * vDelta[2];
    fB = 2.0f * vDelta[0] * vFrom[0] + 2.0f * vDelta[2] * vFrom[2];
    fDisc = fB * fB - 4.0f * fA * (fC - PIN_RADIUS_SQ);
    if (fDisc <= 0.0f) return 0;
    fRoot = (f32)fn_80009680(fDisc);
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
    fn_800BAF04(pNormal, pNormal);
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
// cell's triangles (fn_8004EB7C) and keeping the hit nearest pFrom. The normal is turned to face
// the line.
u8 fn_8004E558(CourseInfo* pCourse, int nPlayer, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal,
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
        fBest = fn_800BB028(pFrom, pHit);
    }
    nCell[0] = Ter_GridCell((pFrom[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    nCell[1] = Ter_GridCell((pFrom[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
    nEndX = Ter_GridCell((pTo[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    nEndZ = Ter_GridCell((pTo[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
    vDelta[0] = pTo[0] - pFrom[0];
    vDelta[1] = pTo[2] - pFrom[2];
    vStart[0] = pFrom[0];
    vStart[1] = pFrom[2];
    if (fn_8000AD9C(vDelta[0]) > fn_8000AD9C(vDelta[1])) {
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
    fn_8005097C(pTo, pFrom, vDir);
    fn_800BAF04(vDir, vDir);
    Vec_Copy(pFrom, vPos);
    for (;;) {
        Vec_Copy(vPos, vPrev);
        if (nCell[0] == nEndX && nCell[1] == nEndZ) {
            Vec_Copy(pTo, vPos);
        } else {
            if (vEdge[nMajor] - vStart[nMajor] != 0.0f) {
                fRatio = (vEdge[nMinor] - vStart[nMinor]) / (vEdge[nMajor] - vStart[nMajor]);
            } else {
                fRatio = 1000000.0f;
            }
            if (fn_8000AD9C(fSlope) < fn_8000AD9C(fRatio)) {
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
        fLen = Vec_Distance(vPos, vPrev);
        fn_8004DF10(pCourse, vPrev, vDir, nCell[0], nCell[1], fLen);
        if (fn_8004EB7C(pCourse, nCell[0], nCell[1], vPrev, vPos, vDir, fLen, vHit, vNormal, &pSurface, &pObj,
                        pbFlags)) {
            fDist = fn_800BB028(pFrom, vHit);
            if (fDist < fBest) {
                fBest = fDist;
                Vec_Copy(vHit, pHit);
                Vec_Copy(vNormal, pNormal);
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
    if (fBest != 1000000.0f && fn_8000C5FC(vDir, pNormal) > 0.0f) {
        fn_800509BC(pNormal, pNormal);
    }
    return fBest != 1000000.0f;
}

// TW06: bool Ter_CheckForWorldCollisionOneGrid(TGD_TerrainInfo*, s32, s32, f32*, f32*, f32*, f32,
// f32[4]*, f32[4]*, TGD_MaterialInfo**, TGD_ObjectInstanceInfo**, u8*). The nearest triangle of
// grid cell (nX, nZ) that the line from pFrom along pDir meets before fMax (pTo is the line's end,
// for the cell's height test): the point, normal, surface, the object it belongs to (NULL for the
// ground) and, when pbFlags is given, the triangle's flag bits. Only the objects marked in
// lbl_801D54A0 are tested.
u8 fn_8004EB7C(CourseInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo, f32* pDir, f32 fMax, f32* pHit,
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
            } else if (lbl_801D54A0[pRef->n2] == 0) {
                pRef++;
            } else {
                pVert = &pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
                pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
                for (j = pRef->nTris - 1; j >= 0; j--) {
                    if ((pFlags[2] & 7) && fn_8004AFA0(pFrom, pDir, fMax, pVert, &fT, vHit, vNormal)) {
                        fMax = fT;
                        Vec_Copy(vHit, pHit);
                        Vec_Copy(vNormal, pNormal);
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
// fn_8004E558, but a ball passes through branches and leaves (fn_8004F43C).
u8 fn_8004EE20(CourseInfo* pCourse, int nPlayer, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal,
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
        fBest = fn_800BB028(pFrom, pHit);
    }
    nCell[0] = Ter_GridCell((pFrom[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    nCell[1] = Ter_GridCell((pFrom[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
    nEndX = Ter_GridCell((pTo[0] - pCourse->fGridOrigin[0]) / pCourse->fGridCellSize[0]);
    nEndZ = Ter_GridCell((pTo[2] - pCourse->fGridOrigin[1]) / pCourse->fGridCellSize[1]);
    vDelta[0] = pTo[0] - pFrom[0];
    vDelta[1] = pTo[2] - pFrom[2];
    vStart[0] = pFrom[0];
    vStart[1] = pFrom[2];
    if (fn_8000AD9C(vDelta[0]) > fn_8000AD9C(vDelta[1])) {
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
    fn_8005097C(pTo, pFrom, vDir);
    fn_800BAF04(vDir, vDir);
    Vec_Copy(pFrom, vPos);
    for (;;) {
        Vec_Copy(vPos, vPrev);
        if (nCell[0] == nEndX && nCell[1] == nEndZ) {
            Vec_Copy(pTo, vPos);
        } else {
            if (vEdge[nMajor] - vStart[nMajor] != 0.0f) {
                fRatio = (vEdge[nMinor] - vStart[nMinor]) / (vEdge[nMajor] - vStart[nMajor]);
            } else {
                fRatio = 1000000.0f;
            }
            if (fn_8000AD9C(fSlope) < fn_8000AD9C(fRatio)) {
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
        fLen = Vec_Distance(vPos, vPrev);
        fn_8004DF10(pCourse, vPrev, vDir, nCell[0], nCell[1], fLen);
        if (fn_8004F43C(pCourse, nCell[0], nCell[1], vPrev, vPos, vDir, fLen, vHit, vNormal, &pSurface,
                        &pObj)) {
            fDist = fn_800BB028(pFrom, vHit);
            if (fDist < fBest) {
                fBest = fDist;
                Vec_Copy(vHit, pHit);
                Vec_Copy(vNormal, pNormal);
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
    if (fBest != 1000000.0f && fn_8000C5FC(vDir, pNormal) > 0.0f) {
        fn_800509BC(pNormal, pNormal);
    }
    return fBest != 1000000.0f;
}

// TW06: bool Ter_CheckForSolidWorldCollisionOneGrid(...), the same parameters without the flags.
// As fn_8004EB7C, but surfaces with a negative bounce (branches and leaves, which a ball passes
// through) do not count.
u8 fn_8004F43C(CourseInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo, f32* pDir, f32 fMax, f32* pHit,
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
            } else if (lbl_801D54A0[pRef->n2] == 0) {
                pRef++;
            } else if (gSurfaceTypes[pRef->nSurface].f0C < 0.0f) {
                pRef++;
            } else {
                pVert = &pCourse->pVerts[TER_FIRST_VERTEX(pRef)];
                pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
                for (j = pRef->nTris - 1; j >= 0; j--) {
                    if ((pFlags[2] & 7) && fn_8004AFA0(pFrom, pDir, fMax, pVert, &fT, vHit, vNormal)) {
                        fMax = fT;
                        Vec_Copy(vHit, pHit);
                        Vec_Copy(vNormal, pNormal);
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
// (fn_8004FCB4 per cell); no pin and no objects.
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
    if (fn_8000AD9C(vDelta[0]) > fn_8000AD9C(vDelta[1])) {
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
    fn_8005097C(pTo, pFrom, vDir);
    fn_800BAF04(vDir, vDir);
    Vec_Copy(pFrom, vPos);
    for (;;) {
        Vec_Copy(vPos, vPrev);
        if (nCell[0] == nEndX && nCell[1] == nEndZ) {
            Vec_Copy(pTo, vPos);
        } else {
            if (vEdge[nMajor] - vStart[nMajor] != 0.0f) {
                fRatio = (vEdge[nMinor] - vStart[nMinor]) / (vEdge[nMajor] - vStart[nMajor]);
            } else {
                fRatio = 1000000.0f;
            }
            if (fn_8000AD9C(fSlope) < fn_8000AD9C(fRatio)) {
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
        fLen = Vec_Distance(vPos, vPrev);
        if (fn_8004FCB4(pCourse, nCell[0], nCell[1], vPrev, vPos, vDir, fLen, vHit, vNormal, &pSurface,
                        &pObj)) {
            fDist = fn_800BB028(pFrom, vHit);
            if (fDist < fBest) {
                fBest = fDist;
                Vec_Copy(vHit, pHit);
                Vec_Copy(vNormal, pNormal);
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
    if (fBest != 1000000.0f && fn_8000C5FC(vDir, pNormal) > 0.0f) {
        fn_800509BC(pNormal, pNormal);
    }
    return fBest != 1000000.0f;
}

// TW06: bool Ter_CheckForGroundCollisionOneGrid(...). As fn_8004F43C, the ground only (no objects).
u8 fn_8004FCB4(CourseInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo, f32* pDir, f32 fMax, f32* pHit,
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
                    if ((pFlags[2] & 7) && fn_8004AFA0(pFrom, pDir, fMax, pVert, &fT, vHit, vNormal)) {
                        fMax = fT;
                        Vec_Copy(vHit, pHit);
                        Vec_Copy(vNormal, pNormal);
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
// (fn_800504F4 per cell).
u8 fn_8004FF34(CourseInfo* pCourse, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal, SurfaceType** ppSurface,
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
    if (fn_8000AD9C(vDelta[0]) > fn_8000AD9C(vDelta[1])) {
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
    fn_8005097C(pTo, pFrom, vDir);
    fn_800BAF04(vDir, vDir);
    Vec_Copy(pFrom, vPos);
    for (;;) {
        Vec_Copy(vPos, vPrev);
        if (nCell[0] == nEndX && nCell[1] == nEndZ) {
            Vec_Copy(pTo, vPos);
        } else {
            if (vEdge[nMajor] - vStart[nMajor] != 0.0f) {
                fRatio = (vEdge[nMinor] - vStart[nMinor]) / (vEdge[nMajor] - vStart[nMajor]);
            } else {
                fRatio = 1000000.0f;
            }
            if (fn_8000AD9C(fSlope) < fn_8000AD9C(fRatio)) {
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
        fLen = Vec_Distance(vPos, vPrev);
        if (fn_800504F4(pCourse, nCell[0], nCell[1], vPrev, vPos, vDir, fLen, vHit, vNormal, &pSurface,
                        &pObj)) {
            fDist = fn_800BB028(pFrom, vHit);
            if (fDist < fBest) {
                fBest = fDist;
                Vec_Copy(vHit, pHit);
                Vec_Copy(vNormal, pNormal);
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
    if (fBest != 1000000.0f && fn_8000C5FC(vDir, pNormal) > 0.0f) {
        fn_800509BC(pNormal, pNormal);
    }
    return fBest != 1000000.0f;
}

// TW06: bool Ter_CheckForObjectCollisionOneGrid(...). As fn_8004F43C, objects only, plus ground
// whose surface has flag 0x80.
u8 fn_800504F4(CourseInfo* pCourse, int nX, int nZ, f32* pFrom, f32* pTo, f32* pDir, f32 fMax, f32* pHit,
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
                    if ((pFlags[2] & 7) && fn_8004AFA0(pFrom, pDir, fMax, pVert, &fT, vHit, vNormal)) {
                        fMax = fT;
                        Vec_Copy(vHit, pHit);
                        Vec_Copy(vNormal, pNormal);
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
// Mark every ground triangle's highest and lowest corner in its flags (bits 4-5 and 6-7), using
// bit 3 to do each triangle once, then clear bit 3 again. TW06 has the two halves as
// Ter_ComputeHighestPointInEveryTriangle and Ter_ClearVertexProcessedBit.
// Not exact yet (94.5%): only the second pass's registers differ (it adds the strip's base to
// the vertex index last; ours adds the low half first). Tried: its own block locals and
// orders, a static inline helper, a named index (int, s32, u32), for/while forms.
void fn_80050794(CourseInfo* pCourse) {
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
        pFlags = pCourse->pTriFlags + TER_FIRST_VERTEX(pRef);
        j = pRef->nTris;
        while (j != 0) {
            pFlags[2] &= 0xF7;
            j--;
            pFlags++;
        }
        i--;
        pRef++;
    }
}

// The difference a - b of two three-float vectors, into pOut.
asm void fn_8005097C(register f32* pA, register f32* pB, register f32* pOut) {
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

// A three-float vector negated, into pOut.
asm void fn_800509A0(register f32* pA, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    ps_neg f0, f0
    ps_neg f1, f1
    psq_st f0, 0(pOut), 0, 0
    psq_st f1, 8(pOut), 1, 0
    blr
}

// A four-float vector negated, into pOut.
asm void fn_800509BC(register f32* pA, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 0, 0
    ps_neg f0, f0
    ps_neg f1, f1
    psq_st f0, 0(pOut), 0, 0
    psq_st f1, 8(pOut), 0, 0
    blr
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

// A signed byte of an object's data: its +0x24 + n. Called with n = 0 from
// Ter_CheckObjectAndHazardObstruction, which tests bit 0x40 of it.
int fn_80050BD8(s8** ppData, int n) {
    return (*ppData)[n + 0x24];
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
