// TerrainData.c (our name; its network functions are TW07's UNetwork.c, in the same order): the
// hole's networks, the outlines on the course (free-drop areas, out of bounds, the situation
// zones), which arrive as 'Cnet' stream objects and are handed to the systems that registered for
// their type (Network_RegisterLoadNetworkCallback). Also the outline tests (point inside,
// wn_PnPoly; segment and ray crossings) and, at the end, code of other units: the course data's
// getter (TW07's GoTerrain_TGD.c), a UObject matrix product and two vector helpers. The file
// starts where the texture code's .bss ends, padded to 0x801A2A00, and its constants start the
// .sdata2 block after urandom.c's.

#include "game.h"
#include "ustream.h"

void LLMath_mat44fltMultiplyList33(f32 (*pMtx)[4], f32 (*pSrc)[4], f32 (*pDst)[4], int nRows);   // VecMath.c

// The globals, in reverse address order (CodeWarrior lays them out last-defined-first).
int gNetworkCount;                      // how many networks gNetworks holds
TNetwork* gNetworks[32];                // the hole's networks, in the order they came
CourseLoader gNetworkLoaders[8];        // (network type, loader), Network_RegisterLoadNetworkCallback
int gNetworkLoaderCount = -1;           // loaders registered; -1: Network_DownloadDataPNB calls none

void Network_FreeDownloadData(UStreamObject* pObject);
void Network_DownloadDataPNB(UStreamObject* pObject);
f32  isLeft(f32* pA, f32* pB, f32* pP);
u8   Network_LineIntersection(f32* pA, f32* pB, f32* pC, f32* pD, f32* pOut);
u8   Network_RayIntersection(f32* pA, f32* pB, f32* pC, f32* pD);

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 0.0f (0x80282AF0), before the 1.0f / 3.0f Network_GetNodeGroundHeight uses first; its body is unknown.
static f32 TerrainData_StrippedFn(f32 x) {
    if (x < 0.0f) {
        return 0.0f;
    }
    return x;
}

// The ground height under a network node: the supporting ground under a point a third of a unit
// above it. 0 when no course is loaded or there is no ground there. Network_DownloadDataPNB puts
// nodes stored at height 0 on the ground with it.
f32 Network_GetNodeGroundHeight(const f32* pPos) {
    f32 v[4];
    CourseInfo* pCourse;
    f32 fHeight;

    v[0] = pPos[0];
    v[1] = 1.0f / 3.0f + pPos[1];
    v[2] = pPos[2];
    v[3] = 1.0f;
    pCourse = Ter_GetTGD();
    if (pCourse != NULL) {
        fHeight = Ter_GetSupportingGroundHeight(pCourse, v);
        if (fHeight != -65536.125f) {
            return fHeight;
        }
    }
    return 0.0f;
}

// A network stream object is released (Network_DownloadDataPNB installs this as its release
// function): one network fewer in the hole's list.
void Network_FreeDownloadData(UStreamObject* pObject) {
    gNetworkCount = gNetworkCount - 1;
}

// The 'Cnet' stream handler (Network_InitModule registers it): keeps the object unless it is
// already kept, adds its network (after a 12-byte header) to the hole's list, puts nodes stored at
// height 0 on the ground, and hands the network to every loader registered for its type
// (TNetwork.nExportType).
void Network_DownloadDataPNB(UStreamObject* pObject) {
    TNetwork* pNet;
    int i;
    TNetNode* pNode;

    if (fn_8000B508(pObject)) {
        fn_8000B830(pObject);
        return;
    }
    pObject->pfn8 = Network_FreeDownloadData;
    fn_8000B4B8(pObject);
    pNet = (TNetwork*)(pObject->pData + 0xC);   // after a 12-byte header
    gNetworks[gNetworkCount++] = pNet;
    pNode = pNet->aNodes;
    for (i = 0; i < pNet->nNumNodes; i++) {
        if (!pNode->vPos[1]) {
            pNode->vPos[1] = Network_GetNodeGroundHeight(pNode->vPos);
        }
        pNode++;
    }
    for (i = 0; i < gNetworkLoaderCount; i++) {
        if (pNet->nExportType == gNetworkLoaders[i].nChunk) {
            gNetworkLoaders[i].pfn((u8*)pNet);
        }
    }
}

// Registers pfn as the loader for networks of type nChunk (TNetwork.nExportType):
// Network_DownloadDataPNB hands it every network of that type as it arrives. At most 8 loaders;
// returns 0 when they are full. TW07's version also takes should-load and post-load callbacks.
u8 Network_RegisterLoadNetworkCallback(int nChunk, void (*pfn)(u8*)) {
    if (gNetworkLoaderCount >= 8) {
        return 0;
    }
    gNetworkLoaders[gNetworkLoaderCount].nChunk = nChunk;
    gNetworkLoaders[gNetworkLoaderCount++].pfn = pfn;
    return 1;
}

// Shuts the networks down (gomainloop.c's shut-down list): forgets the hole's networks, and a
// loader count of -1 stops Network_DownloadDataPNB from calling any loader.
void Network_CloseModule(void) {
    gNetworkCount = 0;
    gNetworkLoaderCount = -1;
}

// Starts the networks (gomainloop.c's start-up list): registers Network_DownloadDataPNB for 'Cnet'
// stream objects, with no networks and no loaders yet.
void Network_InitModule(void) {
    Stream_RegisterLoadChunkCallback(TAG('C', 'n', 'e', 't'), Network_DownloadDataPNB);
    gNetworkCount = 0;
    gNetworkLoaderCount = 0;
}

// The winding number of the outline around pPos, in x and z: nonzero when pPos is inside.
s32 wn_PnPoly(f32* pPos, TNetwork* pNet, s32 nNodes) {
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
            if (pB[2] > pPos[2] && isLeft(pA, pB, pPos) > 0.0f) {
                nWinding++;
            }
        } else {
            if (pB[2] <= pPos[2] && isLeft(pA, pB, pPos) < 0.0f) {
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
f32 isLeft(f32* pA, f32* pB, f32* pP) {
    return (pB[0] - pA[0]) * (pP[2] - pA[2]) - (pP[0] - pA[0]) * (pB[2] - pA[2]);
}

// Where the segments a-b and c-d cross, in x and z (pOut's x and z); 0 if they do not.
u8 Network_LineIntersection(f32* pA, f32* pB, f32* pC, f32* pD, f32* pOut) {
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
    // fake match: the two differences go through fT1 and fT2 (both set again below before any
    // read); written inline, the product's operands or the loads' registers come out swapped
    fT1 = fBz - fAz;
    fT2 = fDx - fCx;
    fDen = (fDz - fCz) * (fBx - fAx) - fT2 * fT1;
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

// Whether the ray from a through b crosses the segment c-d, in x and z (TW07's version also returns
// the point).
u8 Network_RayIntersection(f32* pA, f32* pB, f32* pC, f32* pD) {
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

// Whether the segment from pFrom to pTo crosses the outline pNet (nNodes edges, walked from node 0
// along its links), and in pHit's x and z the crossing nearest pFrom. Ter_CollisionWithOOBNetwork
// and the golf camera use it.
u8 Network_LineNetworkIntersection(f32* pFrom, f32* pTo, TNetwork* pNet, s32 nNodes, f32* pHit) {
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
        if (Network_LineIntersection(pFrom, pTo, pA, pB, v)) {
            fDx = v[0] - pFrom[0];
            fDz = v[2] - pFrom[2];
            fDist = fDx * fDx;      // fake match: the squares as statements, so they do not fuse
            fDz = fDz * fDz;
            fDist = Math_Sqrt(fDist + fDz);
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

// Whether the ray from pFrom through pTo crosses any edge of the outline pNet (nNodes edges, walked
// from node 0 along its links). The camera scripts use it.
u8 Network_RayNetworkDoesIntersect(f32* pFrom, f32* pTo, TNetwork* pNet, s32 nNodes) {
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
        if (Network_RayIntersection(pFrom, pTo, pA, pB)) {
            return 1;
        }
        nPrev = nCur;
        nCur = nNext;
    }
    return 0;
}

// TW06: Ter_TerrainGameDataMgr::GetTGD.
CourseInfo* Ter_GetTGD(void) {
    return gTerrainRendererMgr.pCourse;
}

// An object's combined rotation: rows 0..2 of its third matrix (UObject.m80) become its second
// matrix's rows (m40) times its first (m0), 3x3 parts only (LLMath_mat44fltMultiplyList33); the
// position row m80[3] is left alone. pMtx is the object's m0.
void UObject_ComposeRotation(f32 (*pMtx)[4]) {
    LLMath_mat44fltMultiplyList33(pMtx + 4, pMtx, pMtx + 8, 3);
}

// Adds f times pB to pA, into pOut: three floats (paired-single assembly).
#ifdef __MWERKS__
asm void LLMath_AddScale3(register f32* pA, register f32* pB, register f32 f, register f32* pOut) {
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
void LLMath_AddScale3(f32* pA, f32* pB, f32 f, f32* pOut) {
    pOut[0] = pA[0] + f * pB[0];
    pOut[1] = pA[1] + f * pB[1];
    pOut[2] = pA[2] + f * pB[2];
}
#endif

f32 Vec3_Dot(f32* pA, f32* pB) {
    return pA[2] * pB[2] + (pA[0] * pB[0] + pA[1] * pB[1]);
}
