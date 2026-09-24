// UFstPart.c (EA's name, from its asserts; also in EA's 2002 source tree): the particle emitters
// (PsEmitter in include/psmgr.h). Mostly not yet decompiled.

#include "game_types.h"
#include "psmgr.h"
#include "camera.h"
#include "unsorted/cull.h"

// Skin.c
void fn_80036054(ShaderObject* pObj, int nRow, const void* pDesc);
void fn_800360A0(ShaderObject* pObj);
void fn_800360D4(ShaderObject* pObj);
void fn_80036100(ShaderObject* pObj, const void* pData, int n);

void fn_80098BDC(PsEmitter* pEmitter);
u32  fn_8009912C(PsEmitter* pEmitter, int n, f32 fTime, f32 f);  // not yet decompiled
// Sort the list (next pointer in word n of each emitter?) by pfnCompare; not yet decompiled.
PsEmitter* fn_80099C50(PsEmitter* pList, int n, int (*pfnCompare)(PsEmitter*, PsEmitter*));
int  fn_80099E34(PsEmitter* pA, PsEmitter* pB);

// Make the six fixed emitters, each with a 128-particle system (row 9, the particle shader).
void fn_80098A98(void) {
    ParticleCreate create;
    ParticleParams params;
    int i;

    for (i = 0; i < 6; i++) {
        lbl_801DB888[i] = fn_80009B34(sizeof(PsEmitter), 2, 64, "UFstPart.c", 408);
        lbl_801DB888[i]->b5C = 0;
        memset(&params, 0, sizeof(ParticleParams));
        params.nCount = 128;
        create.bAlloc = 1;
        create.pParams = &params;
        fn_80036054(&lbl_801DB888[i]->mesh, 9, &create);
    }
}

// Free every emitter on the list, then the six fixed ones.
void fn_80098B5C(void) {
    PsEmitter* pEmitter;
    PsEmitter* pDone;
    int i;

    pEmitter = lbl_80281F88;
    while (pEmitter != NULL) {
        pDone = pEmitter;
        pEmitter = pEmitter->p40;
        fn_80098BDC(pDone);
    }
    lbl_80281F88 = NULL;
    for (i = 0; i < 6; i++) {
        fn_800360A0(&lbl_801DB888[i]->mesh);
        fn_80009E70(lbl_801DB888[i]);
    }
}

void fn_80098BDC(PsEmitter* pEmitter) {
    pEmitter->b5C = 0;
    if (pEmitter->params.n54 < 0) {
        fn_800360A0(&pEmitter->mesh);
        fn_80009E70(pEmitter);
    }
}

// Free every emitter on the list.
void fn_80098C28(void) {
    PsEmitter* pEmitter;
    PsEmitter* pNext;

    pEmitter = lbl_80281F88;
    while (pEmitter != NULL) {
        pNext = pEmitter->p40;
        fn_80098BDC(pEmitter);
        pEmitter = pNext;
    }
    lbl_80281F88 = NULL;
}

void fn_80098C70(void) {
    lbl_801DB888[0]->params.u58 |= 0x80000000;
    lbl_801DB888[1]->params.u58 |= 0x80000000;
    lbl_801DB888[2]->params.u58 |= 0x80000000;
    lbl_801DB888[3]->params.u58 |= 0x80000000;
    lbl_801DB888[4]->params.u58 |= 0x80000000;
    lbl_801DB888[5]->params.u58 |= 0x80000000;
}

// Emit n particles through the emitter's matrix (ages spread over fAgeSpread), and add n to its n50.
void fn_800990BC(PsEmitter* pEmitter, int n, f32 f10, f32 fAgeSpread) {
    ParticleMsg msg;

    msg.nWhat = 1;
    msg.pParams = &pEmitter->params;
    msg.u.emit.pMtx = pEmitter->mtx;
    msg.u.emit.nCount = n;
    msg.u.emit.f10 = f10;
    msg.u.emit.fAgeSpread = fAgeSpread;
    fn_80036100(&pEmitter->mesh, &msg, 1);
    pEmitter->n50 += n;
}

// Move every emitter on the list on to fTime. One that fn_8009912C reports (flag 0x80000000) is
// marked 0x40000000 on the next pass and freed and unlinked on the one after.
void fn_80099344(f32 fTime) {
    PsEmitter* pEmitter;
    PsEmitter* pPrev;
    PsEmitter* pNext;
    u32 uFlags;

    pEmitter = lbl_80281F88;
    pPrev = NULL;
    while (pEmitter != NULL) {
        uFlags = pEmitter->params.u58;
        pNext = pEmitter->p40;
        if (uFlags & 0x80000000) {
            if (uFlags & 0x40000000) {
                fn_80098BDC(pEmitter);
                if (pPrev != NULL) {
                    pPrev->p40 = pNext;
                } else {
                    lbl_80281F88 = pNext;
                }
            } else {
                pPrev = pEmitter;
                pEmitter->params.u58 = uFlags | 0x40000000;
            }
        } else {
            if (!(uFlags & 0x2000) || pEmitter->n58 < 119) {
                if (fn_8009912C(pEmitter, pEmitter->n58++, fTime, fTime + pEmitter->f4C)) {
                    pEmitter->params.u58 |= 0x80000000;
                }
                pEmitter->f4C = fTime;
            }
            pPrev = pEmitter;
        }
        pEmitter = pNext;
    }
}

// Whether fn_80099BA0 passes the emitter: always with a radius over 1000, else unless
// fn_80007D74 gives 2 for its sphere (params.v70 in view space, radius params.f24).
u32 fn_80099AE4(PsEmitter* pEmitter, Camera* pCamera) {
    Vec4 vView;
    Sphere sphere;

    if (pEmitter->params.f24 > 1000.0f) {
        return 1;
    }
    fn_800BAD60(pCamera->viewMtx, (Vec4*)pEmitter->params.v70, &vView);
    Vec3Copy(&vView.x, &sphere.x);
    sphere.radius = pEmitter->params.f24;
    return fn_80007D74(&sphere, pCamera, 0) != 2;
}

void fn_80099B74(PsEmitter* pEmitter) {
    pEmitter->n58 = 0;
    fn_800360D4(&pEmitter->mesh);
}

// Sort the emitters by their distance from pCamera's lens (fn_80099E34), then fn_80099B74 each
// one fn_80099AE4 passes (skipping those with flag 0x80000000).
void fn_80099BA0(Camera* pCamera) {
    CamLens* pLens = fn_80008370(pCamera);
    PsEmitter* pEmitter;

    lbl_801DB878[0] = pLens->v34[0];
    lbl_801DB878[1] = pLens->v34[1];
    lbl_801DB878[2] = pLens->v34[2];
    lbl_801DB878[3] = 1.0f;
    lbl_80281F88 = fn_80099C50(lbl_80281F88, 16, fn_80099E34);
    for (pEmitter = lbl_80281F88; pEmitter != NULL; pEmitter = pEmitter->p40) {
        if (!(pEmitter->params.u58 & 0x80000000) && fn_80099AE4(pEmitter, pCamera)) {
            fn_80099B74(pEmitter);
        }
    }
}

// fn_80099BA0's sort order: pB's squared distance from lbl_801DB878 minus pA's.
int fn_80099E34(PsEmitter* pA, PsEmitter* pB) {
    f32 fA = fn_800BB028(lbl_801DB878, pA->params.v80);
    return fn_800BB028(lbl_801DB878, pB->params.v80) - fA;
}

void fn_80099EA4(PsEmitter* pEmitter) {
    pEmitter->p40 = lbl_80281F88;
    lbl_80281F88 = pEmitter;
}

// b + a into out (three floats); the same as Ball.c's fn_80055E7C.
#ifdef __MWERKS__
asm void fn_80099EB4(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_add f2, f2, f0
    ps_add f3, f3, f1
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_80099EB4(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
}
#endif
