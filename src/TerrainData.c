// TerrainData.c (our name): the hole's networks, the outlines on the course (free-drop areas,
// out of bounds, the situation zones), which arrive as 'Cnet' stream objects and are handed to
// the systems that registered for their kind (Course_RegisterLoader). Also the outline tests
// (point inside, segment crossing; TW06's wn_PnPoly) and a few small vector helpers. The file
// starts where the texture code's .bss ends, padded to 0x801A2A00, and its constants start the
// .sdata2 block after urandom.c's.

#include "game.h"
#include "ustream.h"

void fn_800BAE5C(f32 (*pMtx)[4], f32 (*pSrc)[4], f32 (*pDst)[4], int nRows);   // VecMath.c

// The globals, in reverse address order (CodeWarrior lays them out last-defined-first).
int lbl_80281C10;                       // how many networks lbl_801A2A40 holds
TNetwork* lbl_801A2A40[32];             // the hole's networks, in the order they came
CourseLoader lbl_801A2A00[8];
int lbl_80280DB0 = -1;                  // loaders registered; -1: the 'Cnet' handler calls none

void fn_8000BF8C(UStreamObject* pObject);
void fn_8000BF9C(UStreamObject* pObject);
f32  fn_8000C244(f32* pA, f32* pB, f32* pP);
u8   fn_8000C278(f32* pA, f32* pB, f32* pC, f32* pD, f32* pOut);
u8   fn_8000C328(f32* pA, f32* pB, f32* pC, f32* pD);

// The ground height under a node, from a point a third of a unit above it; 0 if there is none.
f32 fn_8000BF20(const f32* pPos) {
    f32 v[4];
    CourseInfo* pCourse;
    f32 fHeight;

    v[0] = pPos[0];
    v[1] = 1.0f / 3.0f + pPos[1];
    v[2] = pPos[2];
    v[3] = 1.0f;
    pCourse = fn_8000C594();
    if (pCourse != NULL) {
        fHeight = Ter_GetSupportingGroundHeight(pCourse, v);
        if (fHeight != -65536.125f) {
            return fHeight;
        }
    }
    return 0.0f;
}

// A network object is released: one network fewer.
void fn_8000BF8C(UStreamObject* pObject) {
    lbl_80281C10 = lbl_80281C10 - 1;
}

// The 'Cnet' handler: keep the object (unless it is already kept), put its nodes on the ground
// where their height is 0, and hand the network to the loaders of its kind.
void fn_8000BF9C(UStreamObject* pObject) {
    TNetwork* pNet;
    int i;
    TNetNode* pNode;

    if (fn_8000B508(pObject)) {
        fn_8000B830(pObject);
        return;
    }
    pObject->pfn8 = fn_8000BF8C;
    fn_8000B4B8(pObject);
    pNet = (TNetwork*)(pObject->pData + 0xC);   // after a 12-byte header
    lbl_801A2A40[lbl_80281C10++] = pNet;
    pNode = pNet->aNodes;
    for (i = 0; i < pNet->nNumNodes; i++) {
        if (!pNode->vPos[1]) {
            pNode->vPos[1] = fn_8000BF20(pNode->vPos);
        }
        pNode++;
    }
    for (i = 0; i < lbl_80280DB0; i++) {
        if (pNet->nExportType == lbl_801A2A00[i].nChunk) {
            lbl_801A2A00[i].pfn((u8*)pNet);
        }
    }
}

u8 Course_RegisterLoader(int nChunk, void (*pfn)(u8*)) {
    if (lbl_80280DB0 >= 8) {
        return 0;
    }
    lbl_801A2A00[lbl_80280DB0].nChunk = nChunk;
    lbl_801A2A00[lbl_80280DB0++].pfn = pfn;
    return 1;
}

void fn_8000C0F0(void) {
    lbl_80281C10 = 0;
    lbl_80280DB0 = -1;
}

void fn_8000C104(void) {
    Stream_RegisterLoadChunkCallback(TAG('C', 'n', 'e', 't'), fn_8000BF9C);
    lbl_80281C10 = 0;
    lbl_80280DB0 = 0;
}

// The winding number of the outline around pPos, in x and z: nonzero when pPos is inside.
s32 fn_8000C140(f32* pPos, TNetwork* pNet, s32 nNodes) {
    s32 i;
    s32 nWinding = 0;
    int nCur = 0;
    int nPrev = -1;
    int nNext;
    f32* pA;
    f32* pB;

    for (i = 0; i < nNodes; i++) {
        pA = pNet->aNodes[nCur].vPos;
        if (nPrev != pNet->aNodes[nCur].aLinks[0]) {
            nNext = pNet->aNodes[nCur].aLinks[0];
            pB = pNet->aNodes[nNext].vPos;
        } else {
            nNext = pNet->aNodes[nCur].aLinks[1];
            pB = pNet->aNodes[nNext].vPos;
        }
        if (pA[2] <= pPos[2]) {
            if (pB[2] > pPos[2] && fn_8000C244(pA, pB, pPos) > 0.0f) {
                nWinding++;
            }
        } else {
            if (pB[2] <= pPos[2] && fn_8000C244(pA, pB, pPos) < 0.0f) {
                nWinding--;
            }
        }
        nPrev = nCur;
        nCur = nNext;
        if (nNext == 0) {
            break;
        }
    }
    return nWinding;
}

// Which side of the line from a to b the point p is on (x and z): above 0 left, below 0 right.
// TW06: isLeft.
f32 fn_8000C244(f32* pA, f32* pB, f32* pP) {
    return (pB[0] - pA[0]) * (pP[2] - pA[2]) - (pP[0] - pA[0]) * (pB[2] - pA[2]);
}

// Where the segments a-b and c-d cross, in x and z (pOut's x and z); 0 if they do not.
u8 fn_8000C278(f32* pA, f32* pB, f32* pC, f32* pD, f32* pOut) {
    f32 fT1;
    f32 fT2;
    f32 fBx;
    f32 fCx;
    f32 fDx;
    f32 fAz;
    f32 fAx;
    f32 fBz;
    f32 fCz;
    f32 fDz;
    f32 fDen;
    f32 fNum;

    fAx = pA[0];
    fBx = pB[0];
    fBz = pB[2];
    fCx = pC[0];
    fAz = pA[2];
    fCz = pC[2];
    fDx = pD[0];
    fDz = pD[2];
    fDen = (fDz - fCz) * (fBx - fAx) - (fBz - fAz) * (fDx - fCx);
    if (0.0f == fDen) {
        return 0;
    }
    fNum = (fDx - fCx) * (fAz - fCz) - (fDz - fCz) * (fAx - fCx);
    fT1 = fNum / fDen;
    fNum = (fBx - fAx) * (fAz - fCz) - (fBz - fAz) * (fAx - fCx);
    fT2 = fNum / fDen;
    if (fT1 < 0.0f || fT1 > 1.0f || fT2 < 0.0f || fT2 > 1.0f) {
        return 0;
    }
    pOut[0] = fT1 * (fBx - fAx) + fAx;
    pOut[2] = fT1 * (fBz - fAz) + fAz;
    return 1;
}

// The ray from a through b crosses the segment c-d (x and z).
u8 fn_8000C328(f32* pA, f32* pB, f32* pC, f32* pD) {
    f32 fT1;
    f32 fT2;
    f32 fAx;
    f32 fBx;
    f32 fCx;
    f32 fDx;
    f32 fAz;
    f32 fBz;
    f32 fCz;
    f32 fDz;
    f32 fDen;
    f32 fNum;

    fAx = pA[0];
    fBx = pB[0];
    fBz = pB[2];
    fCx = pC[0];
    fAz = pA[2];
    fCz = pC[2];
    fDx = pD[0];
    fDz = pD[2];
    fDen = (fDz - fCz) * (fBx - fAx) - (fDx - fCx) * (fBz - fAz);
    if (0.0f == fDen) {
        return 0;
    }
    fNum = (fDx - fCx) * (fAz - fCz) - (fDz - fCz) * (fAx - fCx);
    fT1 = fNum / fDen;
    fNum = (fBx - fAx) * (fAz - fCz) - (fBz - fAz) * (fAx - fCx);
    fT2 = fNum / fDen;
    if (fT1 < 0.0f) {
        return 0;
    }
    if (fT2 < 0.0f || fT2 > 1.0f) {
        return 0;
    }
    return 1;
}

u8 fn_8000C3C8(f32* pFrom, f32* pTo, TNetwork* pNet, s32 nNodes, f32* pHit) {
    s32 i;
    f32 fBest = 100000000.0f;
    int nCur = 0;
    int nPrev = -1;
    int nNext;
    u8 bHit = 0;
    f32* pA;
    f32* pB;
    f32 v[3];
    f32 fDx;
    f32 fDz;
    f32 fDist;

    for (i = 0; i < nNodes; i++) {
        pA = pNet->aNodes[nCur].vPos;
        if (nPrev != pNet->aNodes[nCur].aLinks[0]) {
            nNext = pNet->aNodes[nCur].aLinks[0];
            pB = pNet->aNodes[nNext].vPos;
        } else {
            nNext = pNet->aNodes[nCur].aLinks[1];
            pB = pNet->aNodes[nNext].vPos;
        }
        if (fn_8000C278(pFrom, pTo, pA, pB, v)) {
            fDx = v[0] - pFrom[0];
            fDz = v[2] - pFrom[2];
            fDist = fDx * fDx;      // fake match: the squares as statements, so they do not fuse
            fDz = fDz * fDz;
            fDist = fn_80009680(fDist + fDz);
            bHit = 1;
            if (fDist < fBest) {
                fBest = fDist;
                pHit[0] = v[0];
                pHit[2] = v[2];
            }
        }
        nPrev = nCur;
        nCur = nNext;
    }
    return bHit;
}

u8 fn_8000C4E0(f32* pFrom, f32* pTo, TNetwork* pNet, s32 nNodes) {
    s32 i;
    int nCur = 0;
    int nPrev = -1;
    int nNext;
    f32* pA;
    f32* pB;

    for (i = 0; i < nNodes; i++) {
        pA = pNet->aNodes[nCur].vPos;
        if (nPrev != pNet->aNodes[nCur].aLinks[0]) {
            nNext = pNet->aNodes[nCur].aLinks[0];
            pB = pNet->aNodes[nNext].vPos;
        } else {
            nNext = pNet->aNodes[nCur].aLinks[1];
            pB = pNet->aNodes[nNext].vPos;
        }
        if (fn_8000C328(pFrom, pTo, pA, pB)) {
            return 1;
        }
        nPrev = nCur;
        nCur = nNext;
    }
    return 0;
}

// TW06: Ter_TerrainGameDataMgr::GetTGD.
CourseInfo* fn_8000C594(void) {
    return lbl_801D3CB0.pCourse;
}

// Rows 4..6 of the matrix block times rows 0..2, into rows 8..10.
void fn_8000C5A4(f32 (*pMtx)[4]) {
    fn_800BAE5C(pMtx + 4, pMtx, pMtx + 8, 3);
}

#ifdef __MWERKS__
asm void fn_8000C5D4(register f32* pA, register f32* pB, register f32 f, register f32* pOut) {
    nofralloc
    fmr       f4, f
    psq_l     f0, 0(pA), 0, 0
    psq_l     f1, 8(pA), 1, 0
    psq_l     f2, 0(pB), 0, 0
    psq_l     f3, 8(pB), 1, 0
    ps_madds0 f2, f2, f4, f0
    ps_madds0 f3, f3, f4, f1
    psq_st    f2, 0(pOut), 0, 0
    psq_st    f3, 8(pOut), 1, 0
    blr
}
#else
// port: the plain-C version for compilers without paired singles.
void fn_8000C5D4(f32* pA, f32* pB, f32 f, f32* pOut) {
    pOut[0] = pA[0] + f * pB[0];
    pOut[1] = pA[1] + f * pB[1];
    pOut[2] = pA[2] + f * pB[2];
}
#endif

f32 fn_8000C5FC(f32* pA, f32* pB) {
    return pA[2] * pB[2] + (pA[0] * pB[0] + pA[1] * pB[1]);
}
