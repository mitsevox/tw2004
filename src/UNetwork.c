// UNetwork.c: the hole's networks (out of bounds, free-drop areas, commentary zones...), which arrive
// as 'Cnet' stream objects and go to the loader registered for their type, and the outline tests;
// then the terrain data's getter, an object's matrix update and two vector helpers.

#include "game.h"
#include "dynobj.h"
#include "ustream.h"

static int gNetworkCount;                       // the networks in gNetworks
static TNetwork* gNetworks[NETWORK_NUM_NETWORKS];       // the hole's networks, in the order they
                                                        // came; nothing reads the list
static TLoadNetworkCallbackListEntry gNetworkLoaders[NETWORK_NUM_LOADERS];
static int gNetworkLoaderCount = -1;            // loaders registered; -1 while the networks are shut
                                                // down, when Network_DownloadDataPNB calls none

static f32 isLeft(f32* P0, f32* P1, f32* P2);

// fake match: never called, puts 0.0f first in the constant pool
static f32 UNetwork_StrippedFn(f32 x) {
    if (x < 0.0f) {
        return 0.0f;
    }
    return x;
}

// The ground height under a network node: the supporting ground under a point a third of a unit
// above it; 0 when no course is loaded or there is no ground there.
static f32 Network_GetNodeGroundHeight(const f32* pPos) {
    f32 vAbove[4];
    TGD_TerrainInfo* pCourse;
    f32 fHeight;

    vAbove[0] = pPos[0];
    vAbove[1] = 1.0f / 3.0f + pPos[1];
    vAbove[2] = pPos[2];
    vAbove[3] = 1.0f;
    pCourse = Ter_GetTGD();
    if (pCourse != NULL) {
        fHeight = Ter_GetSupportingGroundHeight(pCourse, vAbove);
        if (fHeight != TER_NO_GROUND) {
            return fHeight;
        }
    }
    return 0.0f;
}

// A network's stream object is released (Resources_Del calls this first): one network fewer. Its
// slot in gNetworks is not cleared.
static void Network_FreeDownloadData(TStreamData* pStreamClass) {
    gNetworkCount = gNetworkCount - 1;
}

// The 'Cnet' stream handler. When the resource list already holds an object of the same type and
// id, Resources_UpdateOldResourceTime frees the arriving copy and no loader gets it. Otherwise the
// object is kept, its network (after a 12-byte header) joins the hole's list, nodes stored at
// height 0 are put on the ground, and the network goes to every loader registered for its
// nExportType.
static void Network_DownloadDataPNB(TStreamData* pStreamClass) {
    TNetwork* pCurNetwork;
    int i;
    TNode* pNode;

    if (Resources_IsAlreadyThere(pStreamClass)) {
        Resources_UpdateOldResourceTime(pStreamClass);
        return;
    }
    pStreamClass->pfnFree = Network_FreeDownloadData;
    Resources_Add(pStreamClass);
    pCurNetwork = (TNetwork*)(pStreamClass->pData + 0xC);
    gNetworks[gNetworkCount++] = pCurNetwork;
    pNode = pCurNetwork->aNodes;
    for (i = 0; i < pCurNetwork->nNumNodes; i++) {
        if (!pNode->vPos[1]) {
            pNode->vPos[1] = Network_GetNodeGroundHeight(pNode->vPos);
        }
        pNode++;
    }
    for (i = 0; i < gNetworkLoaderCount; i++) {
        if (pCurNetwork->nExportType == gNetworkLoaders[i].nNetType) {
            gNetworkLoaders[i].pfnLoad(pCurNetwork);
        }
    }
}

// Registers callbackPtr as the loader of networks of type netType: Network_DownloadDataPNB hands it
// each one as it arrives. Returns 0 when all NETWORK_NUM_LOADERS are taken.
u8 Network_RegisterLoadNetworkCallback(int netType, LoadNetworkCallbackPtr callbackPtr) {
    if (gNetworkLoaderCount >= NETWORK_NUM_LOADERS) {
        return 0;
    }
    gNetworkLoaders[gNetworkLoaderCount].nNetType = netType;
    gNetworkLoaders[gNetworkLoaderCount++].pfnLoad = callbackPtr;
    return 1;
}

// Shuts the networks down when a game mode's systems stop (gomainloop.c): the list is emptied and,
// with the loader count at -1, Network_DownloadDataPNB calls no loader.
void Network_CloseModule(void) {
    gNetworkCount = 0;
    gNetworkLoaderCount = -1;
}

// Starts the networks when a game mode's systems start (gomainloop.c): Network_DownloadDataPNB
// takes the 'Cnet' stream objects, with no networks and no loaders yet (a round's systems register
// theirs after this).
void Network_InitModule(void) {
    Stream_RegisterLoadChunkCallback(TAG('C', 'n', 'e', 't'), Network_DownloadDataPNB);
    gNetworkCount = 0;
    gNetworkLoaderCount = 0;
}

// The winding number of the outline around P, in x and z: nonzero when P is inside. The outline is
// walked from node 0 for n edges, each time to the neighbour it did not come from, and stops early
// when it is back at node 0.
s32 wn_PnPoly(f32* P, TNetwork* network, s32 n) {
    s32 i;
    s32 wn = 0;
    int node = 0;
    int prevNode = -1;
    int nextNode;
    f32* V0;
    f32* V1;

    for (i = 0; i < n; i++) {
        V0 = network->aNodes[node].vPos;
        if (prevNode != network->aNodes[node].aLinks[0]) {
            nextNode = network->aNodes[node].aLinks[0];
            V1 = network->aNodes[nextNode].vPos;
        } else {
            nextNode = network->aNodes[node].aLinks[1];
            V1 = network->aNodes[nextNode].vPos;
        }
        if (V0[2] <= P[2]) {
            if (V1[2] > P[2] && isLeft(V0, V1, P) > 0.0f) {
                wn++;
            }
        } else {
            if (V1[2] <= P[2] && isLeft(V0, V1, P) < 0.0f) {
                wn--;
            }
        }
        prevNode = node;
        node = nextNode;
        if (nextNode == 0) {
            break;
        }
    }
    return wn;
}

// Which side of the line from P0 through P1 the point P2 is on, in x and z: above 0 one side, below
// 0 the other, 0 on the line.
static f32 isLeft(f32* P0, f32* P1, f32* P2) {
    return (P1[0] - P0[0]) * (P2[2] - P0[2]) - (P2[0] - P0[0]) * (P1[2] - P0[2]);
}

// Whether the segments start1-end1 and start2-end2 cross, in x and z; where they do goes into
// intersect's x and z (its y is left alone).
static u8 Network_LineIntersection(f32* start1, f32* end1, f32* start2, f32* end2, f32* intersect) {
    f32 n1;
    f32 n2;
    f32 x2;
    f32 x3;
    f32 x4;
    f32 z1;
    f32 x1;
    f32 z2;
    f32 z3;
    f32 z4;
    f32 denom;
    f32 numer;

    x1 = start1[0];
    x2 = end1[0];
    z2 = end1[2];
    x3 = start2[0];
    z1 = start1[2];
    z3 = start2[2];
    x4 = end2[0];
    z4 = end2[2];
    n1 = z2 - z1;       // fake match: the two differences go through n1 and n2, both set again below
    n2 = x4 - x3;
    denom = (z4 - z3) * (x2 - x1) - n2 * n1;
    if (0.0f == denom) {
        return 0;
    }
    numer = (x4 - x3) * (z1 - z3) - (z4 - z3) * (x1 - x3);
    n1 = numer / denom;
    numer = (x2 - x1) * (z1 - z3) - (z2 - z1) * (x1 - x3);
    n2 = numer / denom;
    if (n1 < 0.0f || n1 > 1.0f || n2 < 0.0f || n2 > 1.0f) {
        return 0;
    }
    intersect[0] = n1 * (x2 - x1) + x1;
    intersect[2] = n1 * (z2 - z1) + z1;
    return 1;
}

// Whether the ray from start1 through end1 crosses the segment start2-end2, in x and z.
static u8 Network_RayIntersection(f32* start1, f32* end1, f32* start2, f32* end2) {
    f32 n1;
    f32 n2;
    f32 x1;
    f32 x2;
    f32 x3;
    f32 x4;
    f32 z1;
    f32 z2;
    f32 z3;
    f32 z4;
    f32 denom;
    f32 numer;

    x1 = start1[0];
    x2 = end1[0];
    z2 = end1[2];
    x3 = start2[0];
    z1 = start1[2];
    z3 = start2[2];
    x4 = end2[0];
    z4 = end2[2];
    denom = (z4 - z3) * (x2 - x1) - (x4 - x3) * (z2 - z1);
    if (0.0f == denom) {
        return 0;
    }
    numer = (x4 - x3) * (z1 - z3) - (z4 - z3) * (x1 - x3);
    n1 = numer / denom;
    numer = (x2 - x1) * (z1 - z3) - (z2 - z1) * (x1 - x3);
    n2 = numer / denom;
    if (n1 < 0.0f) {
        return 0;
    }
    if (n2 < 0.0f || n2 > 1.0f) {
        return 0;
    }
    return 1;
}

// Whether the segment from start to end crosses the outline, in x and z (n edges, walked from node 0
// like wn_PnPoly but without its early stop); the crossing nearest start goes into intersect's x
// and z.
u8 Network_LineNetworkIntersection(f32* start, f32* end, TNetwork* network, s32 n, f32* intersect) {
    s32 i;
    f32 minDist = 100000000.0f;
    int node = 0;
    int prevNode = -1;
    int nextNode;
    u8 found = 0;
    f32* V0;
    f32* V1;
    f32 intersection[3];
    f32 fDx;
    f32 fDz;
    f32 dist;

    for (i = 0; i < n; i++) {
        V0 = network->aNodes[node].vPos;
        if (prevNode != network->aNodes[node].aLinks[0]) {
            nextNode = network->aNodes[node].aLinks[0];
            V1 = network->aNodes[nextNode].vPos;
        } else {
            nextNode = network->aNodes[node].aLinks[1];
            V1 = network->aNodes[nextNode].vPos;
        }
        if (Network_LineIntersection(start, end, V0, V1, intersection)) {
            fDx = intersection[0] - start[0];
            fDz = intersection[2] - start[2];
            dist = fDx * fDx;       // fake match: the squares as statements, so they do not fuse
            fDz = fDz * fDz;
            dist = Math_Sqrt(dist + fDz);
            found = 1;
            if (dist < minDist) {
                minDist = dist;
                intersect[0] = intersection[0];
                intersect[2] = intersection[2];
            }
        }
        prevNode = node;
        node = nextNode;
    }
    return found;
}

// Whether the ray from start through end crosses any edge of the outline, in x and z (n edges,
// walked from node 0 like wn_PnPoly but without its early stop).
u8 Network_RayNetworkDoesIntersect(f32* start, f32* end, TNetwork* network, s32 n) {
    s32 i;
    int node = 0;
    int prevNode = -1;
    int nextNode;
    f32* V0;
    f32* V1;

    for (i = 0; i < n; i++) {
        V0 = network->aNodes[node].vPos;
        if (prevNode != network->aNodes[node].aLinks[0]) {
            nextNode = network->aNodes[node].aLinks[0];
            V1 = network->aNodes[nextNode].vPos;
        } else {
            nextNode = network->aNodes[node].aLinks[1];
            V1 = network->aNodes[nextNode].vPos;
        }
        if (Network_RayIntersection(start, end, V0, V1)) {
            return 1;
        }
        prevNode = node;
        node = nextNode;
    }
    return 0;
}

// The current hole's terrain data (its ground as collision data), NULL until one is loaded.
TGD_TerrainInfo* Ter_GetTGD(void) {
    return gTerRenderer.pCourse;
}

// Puts an object's rotation and scale together: the 3x3 parts of rows 0..2 of its mObjToWorld
// become mRotation x mScale. The position row, mObjToWorld[3], is left alone.
void Object_UpdateO2WComposed33(TObject* pObject) {
    LLMath_mat44fltMultiplyList33(pObject->mScale, pObject->mRotation, pObject->mObjToWorld, 3);
}

// Adds Bmul times B to A, into dest: x, y and z only (dest's w is left alone).
asm void LLMath_AddScale3(register f32* A, register f32* B, register f32 Bmul, register f32* dest) {
    nofralloc
    fmr       f4, Bmul
    psq_l     f0, 0(A), 0, 0
    psq_l     f1, 8(A), 1, 0
    psq_l     f2, 0(B), 0, 0
    psq_l     f3, 8(B), 1, 0
    ps_madds0 f2, f2, f4, f0
    ps_madds0 f3, f3, f4, f1
    psq_st    f2, 0(dest), 0, 0
    psq_st    f3, 8(dest), 1, 0
    blr
}

// The dot product of A and B in x, y and z.
f32 LLMath_DotProduct3(f32* A, f32* B) {
    return A[2] * B[2] + (A[0] * B[0] + A[1] * B[1]);
}
