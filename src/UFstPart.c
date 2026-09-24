// UFstPart.c (EA's name, from its asserts; also in EA's 2002 source tree): the particle emitters
// (PsEmitter in include/psmgr.h). Mostly not yet decompiled.

#include "game_types.h"
#include "psmgr.h"
#include "camera.h"
#include "golfer.h"
#include "unsorted/cull.h"

// Skin.c
void fn_80036054(ShaderObject* pObj, int nRow, const void* pDesc);
void fn_800360A0(ShaderObject* pObj);
void fn_800360D4(ShaderObject* pObj);
void fn_80036100(ShaderObject* pObj, const void* pData, int n);

void fn_8000A194(f32 (*pMtx)[4], f32 a, f32 b, f32 c);  // a rotation matrix from three angles
f32  fn_800351D8(u32 n, f32 fPeriod);                   // GoTerrain.c

void fn_80098BDC(PsEmitter* pEmitter);
u32  fn_8009912C(PsEmitter* pEmitter, int n, f32 fStep, f32 fLiveStep);
// Sort the list (next pointer in word n of each emitter?) by pfnCompare; not yet decompiled.
PsEmitter* fn_80099C50(PsEmitter* pList, int n, int (*pfnCompare)(PsEmitter*, PsEmitter*));
int  fn_80099E34(PsEmitter* pA, PsEmitter* pB);
void fn_80099EB4(f32* pA, f32* pB, f32* pOut);

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

// A new particle's position and velocity (w 1 and 0) for pParams, both in the direction of a random
// turn about y at the angle f38 above the plane: its distance f30 and speed f28, each with some
// random added when a flag says so. Flag 0x200 spreads the position further by v60 (0x400: along
// a random direction, else by a random -1..1 per axis). Then the three values after them.
void fn_80098CDC(ParticleParams* pParams, f32* pPos, f32* pVel, f32* p18, f32* p1C, f32* p20) {
    f32 fSpread;
    f32 fSpeed;
    f32 fDist;
    f32 fRand;
    f32 fTurn;
    f32 fSinTurn;
    f32 fCosTurn;
    f32 fPitch;
    f32 fSinPitch;
    f32 fCosPitch;
    f32 fDistXZ;
    f32 fSpeedXZ;
    f32 fRandXZ;
    f32 x;
    f32 y;
    f32 z;

    if (pParams->u58 & 8) {
        fSpeed = pParams->f2C * Rand_Float(1) + pParams->f28;
    } else {
        fSpeed = pParams->f28;
    }
    if (pParams->u58 & 0x20) {
        fRand = Rand_Float(1);
        if (pParams->u58 & 0x10) {
            fRand = fn_80009680(fRand);
        }
        fDist = pParams->f34 * fRand + pParams->f30;
    } else {
        fDist = pParams->f30;
    }
    fTurn = 2.0f * PI * Rand_Float(1);
    fSinTurn = fn_800095F0(fTurn);
    fCosTurn = fn_80009638(fTurn);
    if (pParams->u58 & 0x40) {
        fPitch = pParams->f3C * Rand_Float(1) + pParams->f38;
    } else {
        fPitch = pParams->f38;
    }
    fSinPitch = fn_800095F0(fPitch);
    fCosPitch = fn_80009638(fPitch);
    fDistXZ = fDist * fCosPitch;
    fSpeedXZ = fSpeed * fCosPitch;
    pPos[0] = fDistXZ * fCosTurn;
    pPos[1] = fDist * fSinPitch;
    pPos[2] = fDistXZ * fSinTurn;
    pPos[3] = 1.0f;
    pVel[0] = fSpeedXZ * fCosTurn;
    pVel[1] = fSpeed * fSinPitch;
    pVel[2] = fSpeedXZ * fSinTurn;
    pVel[3] = 0.0f;
    if (pParams->u58 & 0x200) {
        if (pParams->u58 & 0x400) {
            fTurn = 2.0f * PI * Rand_Float(1);
            fSinTurn = fn_800095F0(fTurn);
            fCosTurn = fn_80009638(fTurn);
            fPitch = 2.0f * PI * Rand_Float(1);
            fSinPitch = fn_800095F0(fPitch);
            fCosPitch = fn_80009638(fPitch);
            if (pParams->u58 & 0x800) {
                fRand = Rand_Float(1);
                fRandXZ = fRand * fSinPitch;
                y = fRand * fCosPitch;
                z = fRandXZ * fSinTurn;
                x = fRandXZ * fCosTurn;
            } else if (pParams->v60[1]) {
                x = fCosTurn;
                y = 0.0f;
                z = fSinTurn;
            } else {
                z = fSinTurn;
                x = fCosTurn;
                y = fCosPitch;
            }
            if (pParams->v60[0]) {
                pPos[0] += pParams->v60[0] * x;
            }
            if (pParams->v60[1]) {
                pPos[1] += pParams->v60[1] * y;
            }
            if (pParams->v60[2]) {
                pPos[2] += pParams->v60[2] * z;
            }
        } else {
            fSpread = pParams->v60[0];
            if (fSpread) {
                pPos[0] += fSpread * (2.0f * Rand_Float(1) - 1.0f);
            }
            fSpread = pParams->v60[1];
            if (fSpread) {
                pPos[1] += fSpread * (2.0f * Rand_Float(1) - 1.0f);
            }
            fSpread = pParams->v60[2];
            if (fSpread) {
                pPos[2] += fSpread * (2.0f * Rand_Float(1) - 1.0f);
            }
        }
    }
    if (pParams->u58 & 1) {
        *p18 = 2.0f * PI * Rand_Float(1);
    } else {
        *p18 = 0.0f;
    }
    if (pParams->u58 & 2) {
        *p1C = pParams->fC * -(2.0f * Rand_Float(1) - 1.0f);
    } else {
        *p1C = pParams->fC;
    }
    if (pParams->u58 & 4) {
        *p20 = pParams->f14 * Rand_Float(1) + pParams->f10;
    } else {
        *p20 = pParams->f10;
    }
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

// Move the emitter on by fStep: age its particles, turn it by f4C, then emit what is due (a
// timed burst with flag 0x10000 on the first pass; a stream while f44 runs, restarted each time
// f48 runs out). Returns 1 when it has no live particles left and has emitted all it may (n54).
// n is not used (fn_80099344 passes the emitter's n58).
u32 fn_8009912C(PsEmitter* pEmitter, int n, f32 fStep, f32 fLiveStep) {
    u32 nLive;
    ParticleMsg msg;
    u32 bEmpty;
    f32 fLife;
    f32 fCycles;
    f32 fRate;
    u32 nCount;

    msg.nWhat = 0;
    msg.pParams = &pEmitter->params;
    bEmpty = 0;
    msg.u.age.fCarried = fStep;
    msg.u.age.fStep = fLiveStep;
    msg.u.age.pnLive = &nLive;
    fn_80036100(&pEmitter->mesh, &msg, 1);
    if (nLive == 0) {
        bEmpty = 1;
    }
    if (pEmitter->params.f4C != 0.0f) {
        pEmitter->params.f40 += pEmitter->params.f4C * fStep;
        fn_8000A194(pEmitter->mtx, pEmitter->params.f40, pEmitter->params.f44, pEmitter->params.f48);
    }
    if ((pEmitter->params.u58 & 0x10000) && pEmitter->n50 == 0) {
        fLife = pEmitter->params.f4;
        fCycles = fLife / (0.5f * pEmitter->params.f1C + pEmitter->params.f18);
        fRate = (pEmitter->params.f20 <= 1.0f / 60.0f) ? 1.0f / 60.0f : pEmitter->params.f20;
        nCount = pEmitter->params.u5C * (fCycles * (60.0f * fRate));
        if (nCount != 0) {
            fn_800990BC(pEmitter, nCount, fStep, fLife);
            bEmpty = 0;
        }
    }
    pEmitter->f48 -= fStep;
    if (pEmitter->f48 < 0.0f) {
        pEmitter->f48 += pEmitter->params.f1C * Rand_Float(1) + pEmitter->params.f18;
        pEmitter->f44 = pEmitter->params.f20;
    }
    if (pEmitter->f44 >= 0.0f) {
        pEmitter->f44 = pEmitter->f44 - fStep;
        if (pEmitter->params.n54 < 0 || pEmitter->n50 < pEmitter->params.n54) {
            fn_800990BC(pEmitter, pEmitter->params.u5C, fStep, 0.0f);
            bEmpty = 0;
        }
    }
    return bEmpty && pEmitter->params.n54 >= 0 && pEmitter->n50 >= pEmitter->params.n54;
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

// Start the emitter a stream record (uSize bytes: 0xFC, or 0xF0 without fF0..fF8) places, unless
// its flags keep it out of this view mode.
void fn_8009943C(PsEmitterRecord* pRecord, u32 uSize) {
    ParticleParams params;
    PsEmitter* pEmitter;

    if (gSession.nSplitScreen != 0 && (pRecord->uE8 & 0x44000)) {
        return;
    }
    if (gSession.nSplitScreen == 0 && (pRecord->uE8 & 0x4000)) {
        return;
    }
    if (uSize != 0xFC && uSize != 0xF0) {
        return;
    }
    params.n0 = pRecord->n1C;
    params.f4 = pRecord->fA8;
    params.fC = pRecord->fA4;
    params.f10 = pRecord->f54;
    params.f14 = pRecord->f58;
    params.f18 = pRecord->fD4;
    params.f1C = pRecord->fD8;
    params.f20 = pRecord->fDC;
    params.f24 = 0.0f;
    params.f28 = pRecord->f4C;
    params.f2C = pRecord->f50;
    params.f30 = pRecord->f44;
    params.f34 = pRecord->f48;
    params.f38 = pRecord->f34;
    params.f3C = pRecord->f38;
    params.f40 = pRecord->f2C;
    params.f44 = pRecord->f28;
    params.f48 = pRecord->f30;
    params.f4C = pRecord->f3C;
    params.f50 = pRecord->f40;
    params.n54 = -1;
    params.u58 = pRecord->uE8;
    params.u5C = pRecord->nE0;
    params.nCount = pRecord->n24;
    params.v60[0] = pRecord->vC8[0];
    params.v60[1] = pRecord->vC8[1];
    params.v60[2] = pRecord->vC8[2];
    params.nTexture = pRecord->n20;
    params.n6E = pRecord->nE4;
    params.v70[0] = 0.0f;
    params.v70[1] = 0.0f;
    params.v70[2] = 0.0f;
    params.v70[3] = 1.0f;
    params.v80[0] = pRecord->v10[0];
    params.v80[1] = pRecord->v10[1];
    params.v80[2] = pRecord->v10[2];
    params.v80[3] = 1.0f;
    params.n90 = pRecord->n8;
    params.f94 = pRecord->fEC;
    params.vA0[0] = pRecord->vB8[0];
    params.vA0[1] = pRecord->vB8[1];
    params.vA0[2] = pRecord->vB8[2];
    params.vA0[3] = 1.0f;
    params.vB0[0] = pRecord->vAC[0];
    params.vB0[1] = pRecord->vAC[1];
    params.vB0[2] = pRecord->vAC[2];
    params.vB0[3] = 1.0f;
    params.vC0[0] = pRecord->v74[0];
    params.vC0[1] = pRecord->v74[1];
    params.vC0[2] = pRecord->v74[2];
    params.vC0[3] = pRecord->v74[3];
    params.vD0[0] = pRecord->v84[0];
    params.vD0[1] = pRecord->v84[1];
    params.vD0[2] = pRecord->v84[2];
    params.vD0[3] = pRecord->v84[3];
    params.vE0[0] = pRecord->v94[0];
    params.vE0[1] = pRecord->v94[1];
    params.vE0[2] = pRecord->v94[2];
    params.vE0[3] = pRecord->v94[3];
    params.vF0[0] = pRecord->v64[0];
    params.vF0[1] = pRecord->v64[1];
    params.vF0[2] = pRecord->v64[2];
    params.vF0[3] = pRecord->v64[3];
    params.f110 = pRecord->fC4 ? pRecord->fC4 : 0.001f;
    params.f114 = pRecord->f60;
    params.f118 = pRecord->f5C;
    params.f11C = pRecord->fC4 ? 1.0f / pRecord->fC4 : 1.0f / 0.001f;
    if (uSize == 0xFC) {
        params.f104 = pRecord->fF0;
        params.f108 = pRecord->fF4;
        params.f100 = pRecord->fF8 ? pRecord->fF8 : 0.001f;
    } else {
        params.f104 = 0.0f;
        params.f108 = 512.0f;
        params.f100 = 0.001f;
    }
    params.f10C = params.f100 ? 1.0f / params.f100 : 1.0f / 0.001f;
    pEmitter = fn_80099758(&params);
    if (pEmitter != NULL) {
        fn_80099EA4(pEmitter);
    }
}

// Start an emitter from pParams:a new one when pParams->n54 is below 0, else the next of the six
// fixed ones (NULL when that one is still in use). Its particle count is capped to what its
// settings can have alive at once, but kept at 16 or more (a fixed one's is always 128).
PsEmitter* fn_80099758(ParticleParams* pParams) {
    PsEmitter* pEmitter;
    ParticleCreate create;
    int nFrames;
    f32 fRate;
    int nCount;

    if (pParams->f18 < 1.0f / 60.0f) {
        pParams->f18 = 1.0f / 60.0f;
    }
    nFrames = 1.0f + pParams->f4 / pParams->f18;
    fRate = (pParams->f20 <= 1.0f / 60.0f) ? 1.0f / 60.0f : pParams->f20;
    nCount = (int)(pParams->u5C * (nFrames * (60.0f * fRate + 1.0f))) + 1;
    if (pParams->n54 < 0 && nCount < pParams->nCount) {
        pParams->nCount = nCount;
    }
    if (pParams->nCount < 16) {
        pParams->nCount = 16;
    }
    if (pParams->n54 >= 0) {
        pParams->nCount = 128;
    }
    if (pParams->n54 < 0) {
        pEmitter = fn_80009B34(sizeof(PsEmitter), 2, 64, "UFstPart.c", 967);
        create.bAlloc = 1;
        create.pParams = pParams;
        fn_80036054(&pEmitter->mesh, 9, &create);
    } else {
        if (lbl_801DB888[lbl_80281F8C]->b5C != 0) {
            return NULL;
        }
        pEmitter = lbl_801DB888[lbl_80281F8C];
        if (++lbl_80281F8C == 6) {
            lbl_80281F8C = 0;
        }
    }
    if (pEmitter == NULL) {
        return NULL;
    }
    pEmitter->params = *pParams;
    pEmitter->b5C = 1;
    pEmitter->params.v70[0] = 0.0f;
    pEmitter->params.v70[1] = 0.0f;
    pEmitter->params.v70[2] = 0.0f;
    pEmitter->params.v70[3] = 1.0f;
    if (pEmitter->params.f4) {
        pEmitter->params.f24 = pParams->f114 + (pEmitter->params.f30 + pEmitter->params.f34);
        pEmitter->params.f24 += (f32)fn_80009680(pEmitter->params.v60[0] * pEmitter->params.v60[0]
                                                 + pEmitter->params.v60[1] * pEmitter->params.v60[1]
                                                 + pEmitter->params.v60[2] * pEmitter->params.v60[2]);
        pEmitter->params.f24 += pEmitter->params.f4
                              * (pEmitter->params.f28 + pEmitter->params.f2C
                                 + (f32)fn_80009680(fn_80009744(pEmitter->params.vA0)));
        pEmitter->params.f24 += 0.5f * pEmitter->params.f4 * pEmitter->params.f4
                              * (f32)fn_80009680(fn_80009744(pParams->vB0));
    }
    pEmitter->p40 = NULL;
    fn_8000A194(pEmitter->mtx, pEmitter->params.f40, pEmitter->params.f44, pEmitter->params.f48);
    Vec_Copy(pEmitter->params.v80, pEmitter->mtx[3]);
    pEmitter->f4C = 0.0f;
    pEmitter->n50 = 0;
    if (pEmitter->params.f94) {
        pEmitter->f48 = pEmitter->params.f94 - fn_800351D8(gSession.nFrameCount, pEmitter->params.f18);
        if (pEmitter->f48 < 0.0f) {
            pEmitter->f48 += pEmitter->params.f18;
        }
    } else {
        pEmitter->f48 = pEmitter->params.f94;
    }
    pEmitter->f44 = -1.0f;
    fn_80099EB4(pEmitter->mtx[3], pEmitter->params.v70, pEmitter->params.v70);
    pEmitter->n54 = -1;
    pEmitter->params.f38 = PI / 2 - pEmitter->params.f38;
    create.bAlloc = 0;
    create.pParams = pParams;
    fn_80036054(&pEmitter->mesh, 9, &create);
    return pEmitter;
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
