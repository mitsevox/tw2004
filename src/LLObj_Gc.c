// LLObj_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled.
// The code below is the visibility tests (0x80007BC4..: move an object's bounding sphere into
// camera space, then cull it), written earlier as unsorted/code_80007BC4.c and not yet cleaned up.

#include "unsorted/cull.h"
#include "dynobj.h"

int fn_80007BC4(RenderObj* obj, Camera* cam, float* outDepth, int mode, float scale);
void fn_80008214(void);
void fn_80008248(void* p);
void fn_80007930(UObjModelRoot* pRoot, int nSet);

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

    fn_80008214();
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
