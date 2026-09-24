// DynChain.c (EA's name, from its asserts; TW06): bone chains that swing on their own (hair,
// cloth), made from a bone down through its children and updated with the model. Partly
// decompiled.

#include "engine.h"
#include "character.h"
#include "charstate.h"

f32  fn_800BAFC0(f32* pSrc, f32* pDst);     // VecMath.c: normalises pSrc into pDst, gives its length
void fn_80029BC8(f32* pVec);                // GoLighting.c
void fn_801164D4(f32* pA, f32* pB, f32* pOut);
f32  fn_80055F80(void);                     // Ball.c
void fn_80114540(CharModel* pModel, DynChain* pChain, f32 f);
void fn_80114A84(CharModel* pModel, DynChain* pChain, f32 f);
void fn_80115348(CharModel* pModel, DynChain* pChain, f32 f);
void fn_80115B2C(CharModel* pModel, DynChain* pChain, f32 f);

// ---- sweep code (not yet cleaned up) ----

void fn_8011407C(void);
void fn_80114080(void);

void fn_8011407C(void) {
}

void fn_80114080(void) {
    fn_80009E70(lbl_802824F8);
    lbl_802824F8 = NULL;
}

// ---- end of sweep code ----

// Set up a link on bone nBone as the model stands now.
void fn_801140AC(DynChainLink* pLink, CharModel* pModel, int nBone, s32 nType) {
    f32 vParent[4];
    f32 vBone[4];
    f32 vDiff[4];

    if (nBone != -1) {
        pLink->nBone = nBone;
        pLink->nParent = pModel->pBones[nBone].nParent;
        Vec_Copy(pModel->pMatrices[nBone][3], pLink->v04);
        if (nType == 0) {
            fn_8001E880(pModel->pPoses[pLink->nParent].v10, vParent);
            fn_8001E880(pModel->pPoses[nBone].v10, vBone);
            fn_801164D4(vBone, vParent, vDiff);
            pLink->fLength = fn_800BAFC0(vDiff, vDiff);
        } else {
            pLink->fLength = 0.5f;
        }
        pLink->f8C = 0.0f;
        fn_8001E85C(pModel->pPoses[nBone].q0, pLink->q44);
        fn_8001E85C(pModel->pPoses[nBone].q0, pLink->q54);
        fn_8001E85C(pModel->pPoses[nBone].v10, pLink->v64);
        fn_8001E85C(pModel->pPoses[nBone].v10, pLink->v74);
        fn_80029BC8(pLink->v24);
    }
}

// Set up every link again, from the top bone down.
void fn_801141F8(DynChain* pChain, CharModel* pModel) {
    int i;
    int nBone;

    if (pChain != NULL) {
        pChain->bReset = 1;
        nBone = pChain->nBone;
        for (i = 0; i < pChain->nLinks; i++) {
            fn_801140AC(&pChain->pLinks[i], pModel, nBone++, pChain->nType);
        }
    }
}

// A chain from bone nBone down: a link for it and each first child below it.
DynChain* fn_80114270(CharModel* pModel, int nBone, s32 nType, s32 n10) {
    DynChain* pChain;
    int nLinks;
    int nNext;
    int i;
    u8 bFound;

    nNext = nBone;
    if (nBone == 0xFF || nBone == -1) {
        return NULL;
    }
    bFound = 1;
    nLinks = 1;
    while (bFound == 1) {
        bFound = 0;
        for (i = 0; i < pModel->nBones; i++) {
            if (nNext == pModel->pBones[i].nParent) {
                nNext = i;
                bFound = 1;
                nLinks++;
                break;
            }
        }
    }
    pChain = fn_80009B34(sizeof(DynChain), 2, 64, "DynChain.c", 390);
    pChain->pLinks = fn_80009B34(nLinks * sizeof(DynChainLink), 2, 64, "DynChain.c", 392);
    pChain->nLinks = nLinks;
    pChain->nBone = nBone;
    pChain->nType = nType;
    pChain->n10 = n10;
    pChain->n14 = 0;
    pChain->n18 = 0;
    fn_801141F8(pChain, pModel);
    return pChain;
}

// Free a chain.
void fn_80114398(DynChain* pChain) {
    fn_80009E70(pChain->pLinks);
    fn_80009E70(pChain);
}

// pIn (x, y, z, w) through pMtx's rotation into pOut; w is copied.
void fn_801143D0(f32 (*pMtx)[4], f32* pIn, f32* pOut) {
    f32 fY = pIn[1];
    f32 fX = pIn[0];
    f32 fZ = pIn[2];
    f32 fOutX = fZ * pMtx[2][0] + (fX * pMtx[0][0] + fY * pMtx[1][0]);
    f32 fOutY = fZ * pMtx[2][1] + (fX * pMtx[0][1] + fY * pMtx[1][1]);
    f32 fOutZ = fZ * pMtx[2][2] + (fX * pMtx[0][2] + fY * pMtx[1][2]);

    pOut[0] = fOutX;
    pOut[1] = fOutY;
    pOut[2] = fOutZ;
    pOut[3] = pIn[3];
}

// Update a chain by fDelta: set it up again first if asked, then the update of its type.
void fn_8011443C(CharModel* pModel, DynChain* pChain, f32 fDelta) {
    if (fDelta != 0.0f && pChain != NULL) {
        if (pChain->bReset) {
            fn_801141F8(pChain, pModel);
            pChain->bReset = 0;
        }
        if (pChain->nBone != 0xFF) {
            if (pChain->nBone == -1) {
                return;
            }
            if (pChain->nType == 0) {
                fn_80114A84(pModel, pChain, fDelta);
            }
            if (pChain->nType == 1) {
                fn_80114540(pModel, pChain, fDelta);
            }
            if (pChain->nType == 2) {
                fn_80115348(pModel, pChain, fDelta);
            }
            if (pChain->nType == 3) {
                fn_80115B2C(pModel, pChain, fDelta);
            }
        }
    }
}

// The sway at frame nFrame: four sine waves with a 600-frame period (1, 3, 5 and 7 times the base
// frequency), offset by fPhase, on a base that grows with fStrength (0..35), limited to 0..1 and
// scaled by the settings' f90. 1 is no sway.
f32 fn_80116304(u32 nFrame, f32 fPhase, f32 fStrength) {
    f32 fPeriod = 600.0f;
    f32 fAngle;
    f32 fWave;
    f32 fBase;
    f32 fSway;

    fAngle = PI * (2.0f * ((f32)(nFrame % (u32)fPeriod) / fPeriod));
    fWave = fn_800095F0(3.0f * fAngle + 2.0f + fPhase);
    fWave = fn_800095F0(fAngle + fPhase) + fWave;
    fWave = fn_800095F0(5.0f * fAngle + 4.0f + fPhase) + fWave;
    fWave = fn_800095F0(7.0f * fAngle + 6.0f + fPhase) + fWave;
    fBase = lbl_802824F8->f94 + fStrength * (lbl_802824F8->f98 - lbl_802824F8->f94) / 35.0f;
    fSway = 1.0f * fWave + fBase;
    fSway = (fSway < 0.0f) ? 0.0f : ((fSway > 1.0f) ? 1.0f : fSway);
    return 1.0f - lbl_802824F8->f90 * (1.0f - fSway);
}

// The strength the chains sway with: the settings' nB8, or fn_80055F80's when it is -1; at least 5.
f32 fn_80116468(void) {
    f32 fStrength;

    if (lbl_802824F8->nB8 == -1) {
        fStrength = fn_80055F80();
    } else if (lbl_802824F8->nB8 == -1) {
        fStrength = 0.0f;
    } else {
        fStrength = lbl_802824F8->nB8;
    }
    if (fStrength < 5.0f) {
        fStrength = 5.0f;
    }
    return fStrength;
}

// Three floats: pOut gets pA minus pB.
#ifdef __MWERKS__
asm void fn_801164D4(register f32* pA, register f32* pB, register f32* pOut) {
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
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_801164D4(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// Three floats: pOut gets pB plus pA.
#ifdef __MWERKS__
asm void fn_801164F8(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_801164F8(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
}
#endif

// Four floats: pOut gets pA minus pB.
#ifdef __MWERKS__
asm void fn_8011651C(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 0, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 0, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 0, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_8011651C(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
    pOut[3] = pA[3] - pB[3];
}
#endif
