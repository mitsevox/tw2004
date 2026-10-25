// LLObj_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree): building and freeing a
// model's mesh tree from its stream data, then the visibility tests (0x80007BC4..: move an
// object's bounding sphere into camera space, then cull it), written earlier as
// unsorted/code_80007BC4.c and not yet cleaned up. Last come the vertex format set-up, the model
// parts' and array sets' type-function dispatch, and the camera and drawable getters.

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
    u8* pStart;
    UObjArrayList list;
    u32* pChunk;
    UObjModelHead* pHead;
    UObjModelRoot* pRoot;

    // The original stores pData, then pData + 8, and keeps pData in r30 (ours: r29): 95%.
    pStart = pData;
    pData = pStart + 8;
    list.n = 0;
    for (;;) {
        pChunk = (u32*)pData;
        pData += 8;
        if (pChunk[0] == 0x41525241) {  // 'ARRA'
            list.ap[list.n++] = pData;
            pData += pChunk[1];
        } else if (pChunk[0] == 0x48454144) {   // 'HEAD'
            break;
        }
    }
    pHead = (UObjModelHead*)pData;
    pData += sizeof(UObjModelHead);
    pRoot = Circular_MemAllocFunc(sizeof(UObjModelRoot) + pHead->nMeshes * sizeof(UObjMesh) +
                        (pHead->nMeshes - 1) * sizeof(UObjMesh*), MEMTYPE_LOW, 16, "LLObj_Gc.c", 198);
    pNext = (u8*)pRoot;
    pRoot->pE4 = NULL;
    pRoot->pE0 = pStart;
    pRoot->pE8 = pHead;
    Mem_set(pRoot, 0, sizeof(pRoot->aSets));
    pRoot->aSets[0].n30 = -1;
    pRoot->aSets[1].n30 = -1;
    pRoot->aSets[2].n30 = -1;
    pRoot->aSets[3].n30 = -1;
    fn_800081C8(&pRoot->aSets[0], 0, &list);
    pNext += sizeof(UObjModelRoot);
    pRoot->pMesh = (UObjMesh*)pNext;
    pNext += pRoot->pE8->nMeshes * sizeof(UObjMesh);
    fn_8000799C(pRoot, pRoot->pMesh, n, &pData, &pNext);
    return pRoot;
}

// Frees a mesh's used parts and table, then its children's, recursively.
void fn_80007524(UObjMesh* pMesh) {
    UObjMesh* pChild;
    int i;

    for (i = 0; i < 4; i++) {
        if (pMesh->abPartUsed[i]) {
            fn_80008248(&pMesh->pParts[i]);
        }
    }
    if (pMesh->pParts != NULL) {
        Circular_MemFreeFunc(pMesh->pParts);
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
        Circular_MemFreeFunc(pRoot->pE4);
    }
    Circular_MemFreeFunc(pRoot);
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
    pMesh->pParts = Circular_MemAllocFunc(pMesh->pInfo->n8C * sizeof(UObjMeshPart), MEMTYPE_LOW, 32,
                                       "LLObj_Gc.c", 398);
    Mem_set(pMesh->pParts, 0, pMesh->pInfo->n8C * sizeof(UObjMeshPart));
    Mem_set(pMesh->abPartUsed, 0, sizeof(pMesh->abPartUsed));
    desc.n0 = pMesh->pInfo->n8C;
    desc.n4 = nCount;
    desc.u0A = 0;
    desc.pC = pData;
    desc.u8 = pMesh->pInfo->u8E;
    if (pMesh->pInfo->b8B & 1) {
        desc.u0A |= 1;
    }
    fn_8000827C(pMesh->pParts, &pRoot->aSets[0], pMesh->pInfo->n88, &desc);
    if (pMesh->pParts->u8 != 0) {
        pMesh->abPartUsed[0] = 1;
        pMesh->n20 = pMesh->pParts->u8;
        pMesh->n24 = pMesh->pParts->n4;
    }
    if (pMesh->abPartUsed[0] != 0) {
        for (i = 1; i < desc.n0; i++) {
            if (pRoot->aSets[i].n30 == -1) {
                fn_80007824(pRoot, i);
            }
            pMesh->pParts[i] = pMesh->pParts[0];
            pMesh->pParts[i].pSet = &pRoot->aSets[i];
            pMesh->abPartUsed[i] = 1;
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
            pRoot->aSets[nSet].ap0[i] = Circular_MemAllocFunc(uSize, MEMTYPE_LOW, 32, "LLObj_Gc.c", 473);
            Mem_cpy(pRoot->aSets[nSet].ap0[i], pRoot->aSets[0].ap0[i], uSize);
        }
    }
}

// Frees array set nSet's arrays (those set 0 has rows for).
void fn_80007930(UObjModelRoot* pRoot, int nSet) {
    int i;

    for (i = 0; i < 5; i++) {
        if (pRoot->aSets[0].an20[i] != 0) {
            Circular_MemFreeFunc(pRoot->aSets[nSet].ap0[i]);
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
    pMesh->pParts = NULL;
    pMesh->nDrawPart = 0;
    Mem_set(pMesh->abPartUsed, 0, sizeof(pMesh->abPartUsed));
    if (pMesh->pInfo->n4 != 0) {
        pChunk = *ppData;
        *ppData += 8;
        *ppData += 0x78;
        fn_80007658(pRoot, pMesh, *ppData, pMesh->pInfo->n4 * 2, n);
        *ppData = &(*ppData)[((u32*)pChunk)[1] - 0x78];   // past the chunk (its size at [1])
    }
    pMesh->ppChildren = NULL;
    if (pMesh->pInfo->n0 != 0) {
        pMesh->ppChildren = (UObjMesh**)*ppNext;
        *ppNext += pMesh->pInfo->n0 * sizeof(UObjMesh*);
        pChild = pMesh + 1;
        for (i = 0; i < pMesh->pInfo->n0; i++) {
            pMesh->ppChildren[pMesh->pInfo->n0 - i - 1] = pChild;
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

// Culls a mesh by its bounding sphere scaled by fScale: DC_CLIP_OUT_OF_VIEW, DC_CLIP_IN_VIEW when
// it is wholly in view (mode 0, then mode 1), else DC_CLIP_PARTLY_IN_VIEW. fDist and fHalfFovTan
// are not used.
int LLObj_GetMeshClipMethod(UObjMesh* pMesh, void* pCamera, f32 fDist, f32 fHalfFovTan, f32 fScale) {
    int nClip;
    int nRet;

    nClip = fn_80007BC4((RenderObj*)pMesh, pCamera, NULL, 0, fScale);
    if (nClip == 2) {
        return DC_CLIP_OUT_OF_VIEW;
    }
    if (nClip == 1) {
        return DC_CLIP_IN_VIEW;
    }
    nClip = fn_80007BC4((RenderObj*)pMesh, pCamera, NULL, 1, fScale);
    nRet = DC_CLIP_PARTLY_IN_VIEW;
    if (nClip == 1) {
        nRet = DC_CLIP_IN_VIEW;
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
    LLMath_mat44fltMultiply(cam->viewMtx, &pos, &pos);
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
    LLMath_mat44fltMultiply(cam->viewMtx, &pos, &pos);
    Vec3Copy((f32*)&pos, (f32*)dst);
}

int fn_80007CE8(RenderObj* obj, Camera* cam, int mode, float scale) {
    Sphere sphere;

    fn_80007C80(cam, (Vec3*)fn_800082F8(obj), (Vec3*)&sphere);
    sphere.radius = scale * fn_800082F8(obj)->radius;
    return fn_80007D74(&sphere, cam, mode);
}

// Sphere vs view frustum test. The sphere is already in camera space.
// Returns 1 = fully visible, 2 = not visible, 4 = touching a side edge, 8 = touching the near limit;
// an orthographic camera gives only 2 or 4 (in view).
int fn_80007D74(Sphere* s, Camera* cam, int mode) {
    float a, b, c, d;
    float px, pz, dist;
    int result;

    if (fn_80008378(Camera_GetLens(cam)) == 0) {
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
        float top    =  Camera_GetLens(cam)->fFlatHeight / 2.0f;
        float bottom = -Camera_GetLens(cam)->fFlatHeight / 2.0f;
        float right  =  Camera_GetLens(cam)->fFlatWidth / 2.0f;
        float left   = -Camera_GetLens(cam)->fFlatWidth / 2.0f;

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

// ---- sweep code (not yet cleaned up) ----

typedef struct {
    /* 0x00 */ char pad0[0x10];
    /* 0x10 */ int unk10;
    /* 0x14 */ int unk14;
} UnkGlobal;

UnkGlobal lbl_801A24A0;
UnkGlobal* lbl_80280DA0 = &lbl_801A24A0;

void GXSetVtxAttrFmt(int vtxfmt, int attr, int cnt, int type, unsigned char frac);

void fn_800080D0(void) {
}

void fn_800080D4(void) {
    lbl_80280DA0->unk14 = 0;
    lbl_80280DA0->unk10 = 0;
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 11, 1, 5, 0);
    GXSetVtxAttrFmt(0, 13, 1, 3, 9);
    GXSetVtxAttrFmt(0, 14, 1, 3, 9);
    GXSetVtxAttrFmt(0, 10, 0, 4, 0);
    GXSetVtxAttrFmt(4, 9, 1, 4, 0);
    GXSetVtxAttrFmt(4, 11, 1, 5, 0);
    GXSetVtxAttrFmt(4, 13, 1, 4, 0);
}

void fn_800081C4(void) {
}

// Gives an array set its mode and runs the mode's setup function on it.
void fn_800081C8(UObjArraySet* pSet, int nMode, void* pArg) {
    pSet->n30 = nMode;
    pSet->pFuncs = &lbl_80188E78[nMode].set;
    pSet->pFuncs->p0(pSet, pArg);
}

void fn_80008214(UObjArraySet* pSet) {
    if (pSet->pFuncs->p4 != NULL) {
        pSet->pFuncs->p4(pSet);
    }
}

void fn_80008248(UObjMeshPart* pPart) {
    if (pPart->pFuncs->p4 != NULL) {
        pPart->pFuncs->p4(pPart);
    }
}

// Gives a mesh part its type and array set and runs the type's setup function on it.
void fn_8000827C(UObjMeshPart* pPart, UObjArraySet* pSet, int nType, void* pArg) {
    pPart->n0 = nType;
    pPart->pSet = pSet;
    pPart->pFuncs = &lbl_80188E88[nType].part;
    pPart->pFuncs->p0(pPart, pArg);
}

// Draws a mesh part through its type's draw function (UObjPartFuncs.p8).
void ObjPart_Draw(UObjMeshPart* pPart) {
    pPart->pFuncs->p8(pPart);
}

extern void fn_800070DC(void);

Sphere* fn_800082F8(RenderObj* obj) {
    return &obj->data->bounds;
}

void Vec3Copy(const f32* pSrc, f32* pDst) {
    pDst[0] = pSrc[0];
    pDst[1] = pSrc[1];
    pDst[2] = pSrc[2];
}

float fn_80008320(Camera* cam) { return cam->unk220; }
float fn_80008328(Camera* cam) { return cam->unk21C; }
float fn_80008330(Camera* cam) { return cam->unk218; }
float fn_80008338(Camera* cam) { return cam->unk214; }
float fn_80008340(Camera* cam) { return cam->unk210; }
float fn_80008348(Camera* cam) { return cam->unk208; }
float fn_80008350(Camera* cam) { return cam->unk20C; }
float fn_80008358(Camera* cam) { return cam->unk204; }
float fn_80008360(Camera* cam) { return cam->unk1F4; }
float fn_80008368(Camera* cam) { return cam->unk1F8; }
CamLens* Camera_GetLens(Camera* cam) { return cam->unk10; }
int fn_80008378(CamLens* sub) { return sub->nType; }

// Waits until the GPU has drawn up to the last break point.
void DL_vWaitForEndOfDrawing(void) {
    fn_800070DC();
}

void LLObj_FrameEmpty(void) {
}

// ---- end of sweep code ----
