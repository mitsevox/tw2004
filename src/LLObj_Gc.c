// LLObj_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree): building and freeing a
// model's mesh tree from its stream data, then the visibility tests (0x80007BC4..: move an
// object's bounding sphere into camera space, then cull it), written earlier as
// unsorted/code_80007BC4.c and not yet cleaned up.

#include "unsorted/cull.h"
#include "dynobj.h"

int fn_80007BC4(RenderObj* obj, Camera* cam, float* outDepth, int mode, float scale);
void fn_80007930(UObjModelRoot* pRoot, int nSet);
int fn_8000799C(UObjModelRoot* pRoot, UObjMesh* pMesh, int n, u8** ppData, u8** ppNext);
void fn_80007658(UObjModelRoot* pRoot, UObjMesh* pMesh, u8* pData, int nCount, int n);
void fn_80007824(UObjModelRoot* pRoot, int nSet);

// Builds a model from its stream data: the 'ARRA' chunks before the 'HEAD' chunk go to array set
// 0's setup, and the meshes, built from the chunks after it, follow the model in one block.
UObjModelRoot* fn_800073B4(u8* pData, int n) {
    u8* pNext;
    u8* pCur;
    UObjArrayList list;
    u32* pChunk;
    UObjModelHead* pHead;
    UObjModelRoot* pRoot;

    // The original stores pData, then pData + 8, and keeps pData in r30 (ours: r29): 95%.
    pCur = pData;
    pCur += 8;
    list.n = 0;
    for (;;) {
        pChunk = (u32*)pCur;
        pCur += 8;
        if (pChunk[0] == 0x41525241) {  // 'ARRA'
            list.ap[list.n++] = pCur;
            pCur += pChunk[1];
        } else if (pChunk[0] == 0x48454144) {   // 'HEAD'
            break;
        }
    }
    pHead = (UObjModelHead*)pCur;
    pCur += sizeof(UObjModelHead);
    pRoot = fn_80009B34(sizeof(UObjModelRoot) + pHead->nMeshes * sizeof(UObjMesh) +
                        (pHead->nMeshes - 1) * sizeof(UObjMesh*), 2, 16, "LLObj_Gc.c", 198);
    pNext = (u8*)pRoot;
    pRoot->pE4 = NULL;
    pRoot->pE0 = pData;
    pRoot->pE8 = pHead;
    fn_80005AE8(pRoot, 0, sizeof(pRoot->aSets));
    pRoot->aSets[0].n30 = -1;
    pRoot->aSets[1].n30 = -1;
    pRoot->aSets[2].n30 = -1;
    pRoot->aSets[3].n30 = -1;
    fn_800081C8(&pRoot->aSets[0], 0, &list);
    pNext += sizeof(UObjModelRoot);
    pRoot->pMesh = (UObjMesh*)pNext;
    pNext += pRoot->pE8->nMeshes * sizeof(UObjMesh);
    fn_8000799C(pRoot, pRoot->pMesh, n, &pCur, &pNext);
    return pRoot;
}

// Frees a mesh's used parts and table, then its children's, recursively.
void fn_80007524(UObjMesh* pMesh) {
    UObjMesh* pChild;
    int i;

    for (i = 0; i < 4; i++) {
        if (pMesh->a1C[i]) {
            fn_80008248(&pMesh->p18[i]);
        }
    }
    if (pMesh->p18 != NULL) {
        fn_80009E70(pMesh->p18);
    }
    pChild = pMesh->p10;
    for (i = 0; i < pMesh->pInfo->n0; i++) {
        fn_80007524(pChild);
        pChild = pChild->p14;
    }
}

// Frees a model fn_800073B4 made: its array sets 1-3, its mesh tree and itself.
void fn_800075CC(UObjModelRoot* pRoot) {
    int i;

    fn_80008214(&pRoot->aSets[0]);
    for (i = 1; i < 4; i++) {
        if (pRoot->aSets[i].n30 != -1) {
            fn_80007930(pRoot, i);
        }
    }
    fn_80007524(pRoot->pMesh);
    if (pRoot->pE4 != NULL) {
        fn_80009E70(pRoot->pE4);
    }
    fn_80009E70(pRoot);
}

// Makes a mesh's parts from the model data at pData: the first gets its type (pInfo->n88) set up
// from a UObjPartDesc; when that part is used, the others are copies of it, each on its own array
// set (made from set 0 when it is not yet). n is not used: fn_8000799C, the only caller, passes it.
void fn_80007658(UObjModelRoot* pRoot, UObjMesh* pMesh, u8* pData, int nCount, int n) {
    UObjPartDesc desc;
    int i;

    if (nCount == 0) {
        return;
    }
    pMesh->p18 = fn_80009B34(pMesh->pInfo->n8C * sizeof(UObjMeshPart), 2, 32, "LLObj_Gc.c", 398);
    fn_80005AE8(pMesh->p18, 0, pMesh->pInfo->n8C * sizeof(UObjMeshPart));
    fn_80005AE8(pMesh->a1C, 0, sizeof(pMesh->a1C));
    desc.n0 = pMesh->pInfo->n8C;
    desc.n4 = nCount;
    desc.u0A = 0;
    desc.pC = pData;
    desc.u8 = pMesh->pInfo->u8E;
    if (pMesh->pInfo->b8B & 1) {
        desc.u0A |= 1;
    }
    fn_8000827C(pMesh->p18, &pRoot->aSets[0], pMesh->pInfo->n88, &desc);
    if (pMesh->p18->u8 != 0) {
        pMesh->a1C[0] = 1;
        pMesh->n20 = pMesh->p18->u8;
        pMesh->n24 = pMesh->p18->n4;
    }
    if (pMesh->a1C[0] != 0) {
        for (i = 1; i < desc.n0; i++) {
            if (pRoot->aSets[i].n30 == -1) {
                fn_80007824(pRoot, i);
            }
            pMesh->p18[i] = pMesh->p18[0];
            pMesh->p18[i].pSet = &pRoot->aSets[i];
            pMesh->a1C[i] = 1;
        }
    }
}

// Makes array set nSet a copy of set 0, with its own copy of each array (sizes rounded up to 32).
void fn_80007824(UObjModelRoot* pRoot, int nSet) {
    u32 uSize;
    int i;

    pRoot->aSets[nSet] = pRoot->aSets[0];
    for (i = 0; i < 5; i++) {
        if (pRoot->aSets[0].an20[i] != 0) {
            uSize = (pRoot->aSets[0].an20[i] * 12 + 31) & ~31;
            pRoot->aSets[nSet].ap0[i] = fn_80009B34(uSize, 2, 32, "LLObj_Gc.c", 473);
            Mem_cpy(pRoot->aSets[nSet].ap0[i], pRoot->aSets[0].ap0[i], uSize);
        }
    }
}

// Frees array set nSet's arrays (those set 0 has rows for).
void fn_80007930(UObjModelRoot* pRoot, int nSet) {
    int i;

    for (i = 0; i < 5; i++) {
        if (pRoot->aSets[0].an20[i] != 0) {
            fn_80009E70(pRoot->aSets[nSet].ap0[i]);
        }
    }
}

// Builds a mesh and, depth first, its children from the model data at *ppData; the children's
// pointer tables are taken from *ppNext. The children follow the mesh in memory. Returns how many
// meshes it built.
int fn_8000799C(UObjModelRoot* pRoot, UObjMesh* pMesh, int n, u8** ppData, u8** ppNext) {
    int nCount = 1;
    UObjMesh* pChild;
    int nBuilt;
    int i;
    u8* pChunk;

    *ppData += 8;
    pChunk = *ppData;
    *ppData = pChunk + 0x94;
    pMesh->pInfo = (UObjMeshInfo*)pChunk;
    pMesh->n20 = 0;
    pMesh->n24 = 0;
    pMesh->p18 = NULL;
    pMesh->n28 = 0;
    fn_80005AE8(pMesh->a1C, 0, sizeof(pMesh->a1C));
    if (pMesh->pInfo->n4 != 0) {
        pChunk = *ppData;
        *ppData += 8;
        *ppData += 0x78;
        fn_80007658(pRoot, pMesh, *ppData, pMesh->pInfo->n4 * 2, n);
        *ppData = &(*ppData)[((u32*)pChunk)[1] - 0x78];   // past the chunk (its size at [1])
    }
    pMesh->p8 = NULL;
    if (pMesh->pInfo->n0 != 0) {
        pMesh->p8 = (UObjMesh**)*ppNext;
        *ppNext += pMesh->pInfo->n0 * sizeof(UObjMesh*);
        pChild = pMesh + 1;
        for (i = 0; i < pMesh->pInfo->n0; i++) {
            pMesh->p8[pMesh->pInfo->n0 - i - 1] = pChild;
            if (i != 0) {
                pChild->p14 = pMesh->p10;
            } else {
                pChild->p14 = NULL;
            }
            pMesh->p10 = pChild;
            pChild->pC = pMesh;
            nBuilt = fn_8000799C(pRoot, pChild, n, ppData, ppNext);
            nCount += nBuilt;
            pChild += nBuilt;
        }
    }
    return nCount;
}

// Culls a mesh by its bounding sphere scaled by fScale: 3 when it is out of view, 2 when it is
// wholly in view (mode 0, then mode 1), else 0. fDist and fHalfFovTan are not used.
int fn_80007B2C(UObjMesh* pMesh, void* pCamera, f32 fDist, f32 fHalfFovTan, f32 fScale) {
    int nClip;
    int nRet;

    nClip = fn_80007BC4((RenderObj*)pMesh, pCamera, NULL, 0, fScale);
    if (nClip == 2) {
        return 3;
    }
    if (nClip == 1) {
        return 2;
    }
    nClip = fn_80007BC4((RenderObj*)pMesh, pCamera, NULL, 1, fScale);
    nRet = 0;
    if (nClip == 1) {
        nRet = 2;
    }
    return nRet;
}

// ---- sweep code (not yet cleaned up) ----

int fn_80007BC4(RenderObj* obj, Camera* cam, float* outDepth, int mode, float scale) {
    Sphere sphere;
    Vec4 pos;
    Sphere* bounds = fn_800082F8(obj);

    Vec3Copy((f32*)bounds, (f32*)&pos);
    pos.w = 1.0f;
    fn_800BAD60(cam->viewMtx, &pos, &pos);
    Vec3Copy((f32*)&pos, (f32*)&sphere);
    if (outDepth != 0) {
        *outDepth = sphere.z;
    }
    sphere.radius = bounds->radius * scale;
    return fn_80007D74(&sphere, cam, mode);
}

void fn_80007C80(Camera* cam, const Vec3* src, Vec3* dst) {
    Vec4 pos;

    Vec3Copy((f32*)src, (f32*)&pos);
    pos.w = 1.0f;
    fn_800BAD60(cam->viewMtx, &pos, &pos);
    Vec3Copy((f32*)&pos, (f32*)dst);
}

int fn_80007CE8(RenderObj* obj, Camera* cam, int mode, float scale) {
    Sphere sphere;

    fn_80007C80(cam, (Vec3*)fn_800082F8(obj), (Vec3*)&sphere);
    sphere.radius = scale * fn_800082F8(obj)->radius;
    return fn_80007D74(&sphere, cam, mode);
}

// Sphere vs view frustum test. The sphere is already in camera space.
// Returns 1 = fully visible, 2 = not visible, 4 = touching a side edge, 8 = touching the near limit.
int fn_80007D74(Sphere* s, Camera* cam, int mode) {
    float a, b, c, d;
    float px, pz, dist;
    int result;

    if (fn_80008378(fn_80008370(cam)) == 0) {
        if (s->z + s->radius > fn_80008368(cam)) return 2;
        if (s->z + s->radius < fn_80008360(cam)) return 2;

        result = 1;
        switch (mode) {
        case 0:
            a = fn_80008358(cam); b = fn_80008350(cam);
            c = fn_80008348(cam); d = fn_80008340(cam);
            break;
        case 1:
            a = fn_80008338(cam); b = fn_80008330(cam);
            c = fn_80008328(cam); d = fn_80008320(cam);
            break;
        }

        px = s->x * b;
        pz = s->z * -a;
        dist = px + pz;
        if (dist >= s->radius) { result = 2; goto done; }
        if (-dist <= s->radius) result = 4;
        dist = -px + pz;
        if (dist >= s->radius) { result = 2; goto done; }
        if (-dist <= s->radius) result = 4;

        px = s->y * d;
        pz = s->z * -c;
        dist = px + pz;
        if (dist >= s->radius) { result = 2; goto done; }
        if (-dist <= s->radius) result = 4;
        dist = -px + pz;
        if (dist >= s->radius) { result = 2; goto done; }
        if (-dist <= s->radius) result = 4;

    done:
        if (result != 2) {
            if (s->z - s->radius <= fn_80008360(cam)) result = 8;
        }
        return result;
    } else {
        float top    =  fn_80008370(cam)->fB8 / 2.0f;
        float bottom = -fn_80008370(cam)->fB8 / 2.0f;
        float right  =  fn_80008370(cam)->fB4 / 2.0f;
        float left   = -fn_80008370(cam)->fB4 / 2.0f;

        if (s->z + s->radius > fn_80008368(cam)) return 2;
        if (s->z + s->radius < fn_80008360(cam)) return 2;
        if (s->x - s->radius > right || s->x + s->radius < left ||
            s->y - s->radius > top || s->y + s->radius < bottom) {
            return 2;
        }
        return 4;
    }
}

// ---- end of sweep code ----
