// Skeleton.c (EA's name, from its asserts; also in EA's 2002 source tree): the golfer's skeleton.
// Its IK chains and the IK weight that sets how strongly their solution is applied (0..1), and
// per-bone factors of the character's model. The types are in character.h.

#include "character.h"
#include "charstate.h"

void fn_80029BC8(f32* pVec);                            // sets a vector to lbl_80186838
void fn_80026BF4(Skeleton* pSkel, IKChain* pChain);
f32  fn_80026D18(Skeleton* pSkel, IKChain* pChain, f32* pTarget, int nLink, int n);   // an IK step's
                                                                                     // remaining error
void fn_800BADF8(f32 (*pMtx)[4], f32 (*pSrc)[4], f32 (*pDst)[4], int nRows);   // pDst = pSrc's rows
                                                                                // through pMtx
void fn_80113E60(void);                                 // DynChain.c
void fn_80114080(void);                                 // DynChain.c
void fn_80114398(struct DynChain* pChain);              // DynChain.c: frees a chain
void fn_8011443C(CharModel* pModel, struct DynChain* pChain, f32 f);   // DynChain.c
void fn_800090A0(f32* pA, f32* pB, f32* pOut);           // Quaternion.c
void fn_800090E4(f32* pQuat, f32* pIn, f32* pOut);       // Quaternion.c: a vector turned by it
void fn_8000914C(f32* pQ, f32 (*pMtx)[4]);               // Quaternion.c: a rotation's matrix
void fn_8000A0E8(f32 (*pSrc)[4], f32 (*pDst)[4]);        // copies a matrix
void fn_8000ADC0(f32 (*pMtx)[4]);                        // identity
void fn_80021980(u32* aA, u32* aB, u32* aOut, u32 nBits);   // aOut = aA | aB, bit arrays
void fn_80029A00(CharModel* pModel, int nA, int nB, f32* pRot);
void fn_80029BF4(f32* pA, f32* pB, f32* pOut);
void fn_80029C60(u32* aSrc, u32* aDst, u32 n, u32 nShift);
void fn_80029EF4(u32* pSrc, u32* pDst, u32 nBits);

// Poses the chain's links (those with f4 above 0) from their rotation vectors.
void fn_80026B4C(Skeleton* pSkel, IKChain* pChain) {
    int i;
    for (i = 0; i < pChain->nLinks; i++) {
        IKLink* pLink = &pChain->pLinks[i];
        if (pLink->f4 > 0.0f) {
            fn_8000923C(pLink->v58, pSkel->p20[pLink->nBone]);
        }
    }
}

// Resets the chain's links (all of them with bAll, else those with f4 above 0) to no rotation and
// clears their bones' bits.
void fn_80026F90(Skeleton* pSkel, IKChain* pChain, u8 bAll) {
    int i;
    for (i = 0; i < pChain->nLinks; i++) {
        if (bAll || pChain->pLinks[i].f4 > 0.0f) {
            int nBone = pChain->pLinks[i].nBone;
            fn_80009710(pSkel->p20[nBone]);
            fn_8001EB6C(pSkel->a10, nBone);
        }
    }
}

// Copies the chain's rotations from p20 to p24; below full weight, blends them toward the
// identity by the weight.
void fn_8002703C(Skeleton* pSkel, IKChain* pChain, f32 fWeight) {
    int i;
    for (i = 0; i < pChain->nLinks; i++) {
        int nBone = pChain->pLinks[i].nBone;
        fn_8001E85C(pSkel->p20[nBone], pSkel->p24[nBone]);
        if (fWeight < 1.0f) {
            fn_8000883C(lbl_801C6498, pSkel->p24[nBone], fWeight);
            Vec_Normalize(pSkel->p24[nBone], pSkel->p24[nBone]);
        }
    }
}

// Resets the skeleton: no transition, every bone's bit clear, v10A4 and v10B4 reset, full weight
// in f10C4, and every chain reset.
void fn_80027108(Skeleton* pSkel) {
    int i;

    pSkel->f1074 = 0.0f;
    fn_8001E938(pSkel->a10, 0x80);
    fn_80029BC8(pSkel->v10A4);
    fn_80029BC8(pSkel->v10B4);
    pSkel->f10C4 = 1.0f;
    for (i = 0; i < pSkel->nChains; i++) {
        fn_80026F90(pSkel, &pSkel->pChains[i], 1);
    }
}

// Solves the chain toward pTarget: up to nIterations steps, each followed by pfnError (when given)
// in place of the step's own error; stops once the error is below fTolerance.
void fn_800273BC(Skeleton* pSkel, IKChain* pChain, f32* pTarget, s32 nIterations,
                 f32 (*pfnError)(Skeleton* pSkel, IKChain* pChain, f32* pTarget), f32 fTolerance) {
    s32 i;
    f32 fError;

    for (i = 0; i < nIterations; i++) {
        fError = fn_80026D18(pSkel, pChain, pTarget, pChain->nLinks - 2, 0);
        if (pfnError != NULL) {
            fError = pfnError(pSkel, pChain, pTarget);
        }
        fn_80026BF4(pSkel, pChain);
        if (fError < fTolerance) {
            break;
        }
    }
}

// Turns the IK on or off.
void fn_80027738(u8 bOn) {
    lbl_802810A6 = bOn;
}

// Applies the IK weight to the first IK chain.
void fn_80027740(Skeleton* pSkel, f32 fWeight) {
    fn_8002703C(pSkel, pSkel->pChains, fWeight);
}

// Sets how strongly the IK solution is applied. At 0 or 1 the bones use p20 as they are; in
// between, the first chain is weighted into p24 and that is used.
void SKEL_SetIKSolutionWeight(Skeleton* pSkel, f32 fWeight) {
    if (pSkel == NULL || lbl_802810A6 == 0) return;
    pSkel->fIKWeight = fWeight;
    if (0.0f == fWeight) {
        pSkel->p28 = pSkel->p20;
    } else if (1.0f == fWeight) {
        pSkel->p28 = pSkel->p20;
    } else {
        fn_80027740(pSkel, fWeight);
        pSkel->p28 = pSkel->p24;
    }
    pSkel->f10C4 = fWeight;
    fn_8001EF34(pSkel->v10A4, fWeight, pSkel->v10B4);
}

// Gives the skeleton a rotation; below full IK weight it is blended toward the identity by the
// weight.
void fn_80027808(CharModel* pModel, f32* pRot) {
    if (pModel->pSkel != NULL) {
        fn_8001E85C(pRot, pModel->pSkel->q10D4);
        if (pModel->pSkel->fIKWeight < 1.0f) {
            fn_8000883C(lbl_801C6498, pModel->pSkel->q10D4, pModel->pSkel->fIKWeight);
            Vec_Normalize(pModel->pSkel->q10D4, pModel->pSkel->q10D4);
        }
    }
}

// Turns bone 0x11's rotation by the skeleton's q10D4 while n10E4 is set, unless the IK is off or at
// no weight.
void fn_8002787C(CharModel* pModel) {
    Skeleton* pSkel = pModel->pSkel;
    f32 qRot[4];

    if (pSkel != NULL && lbl_802810A6 != 0) {
        if (pSkel->fIKWeight <= 0.0f) {
            return;
        }
        if (pSkel->n10E4 != 0) {
            fn_80008FCC(pSkel->q10D4, pSkel->p20[fn_8001EEE4(pModel, 0x11)], qRot);
            fn_8001E85C(qRot, pModel->pSkel->p20[fn_8001EEE4(pModel, 0x11)]);
        }
    }
}

// With any IK weight, sets f1074 and f1078 to 0.25.
void fn_8002792C(Skeleton* pSkel) {
    if (pSkel == NULL || lbl_802810A6 == 0) return;
    if (pSkel->fIKWeight > 0.0f) {
        pSkel->f1074 = 0.25f;
        pSkel->f1078 = 0.25f;
    }
}

// TW06: SKEL_TransitionIK. With b set and any IK weight, f1074 and f1078 become f; without b and
// below full weight, f1074 becomes -f and f1078 f.
void SKEL_TransitionIK(Skeleton* pSkel, u8 b, f32 f) {
    if (pSkel == NULL || lbl_802810A6 == 0) return;
    if (b) {
        if (pSkel->fIKWeight > 0.0f) {
            pSkel->f1078 = f;
            pSkel->f1074 = f;
        }
    } else if (pSkel->fIKWeight < 1.0f) {
        pSkel->f1074 = -f;
        pSkel->f1078 = f;
    }
}

// TW06: SKEL_TranslateIKChainY. Moves the chain's bones up by f, in their poses and matrices.
void SKEL_TranslateIKChainY(CharModel* pModel, IKChain* pChain, f32 f) {
    int i;
    for (i = 0; i < pChain->nLinks; i++) {
        int nBone = pChain->pLinks[i].nBone;
        pModel->pPoses[nBone].v10[1] += f;
        pModel->pMatrices[nBone][3][1] += f;
    }
}

// Frees a skeleton: its chains' links, the chains, and both rotation sets.
void fn_800284DC(Skeleton* pSkel) {
    int i;
    for (i = 0; i < pSkel->nChains; i++) {
        fn_80009E70(pSkel->pChains[i].pLinks);
    }
    fn_80009E70(pSkel->pChains);
    fn_80009E70(pSkel->p24);
    fn_80009E70(pSkel->p20);
    fn_80009E70(pSkel);
}

// Resets every bone's factors to 1.
void fn_80028A3C(CharModel* pModel) {
    int i;
    for (i = 0; i < pModel->nBones; i++) {
        pModel->a140[i][0] = 1.0f;
        pModel->a140[i][1] = 1.0f;
        pModel->a140[i][2] = 1.0f;
    }
}

// Multiplies a bone's factors by f: uAxes 1 the first, 4 the second, 2 the third.
void fn_80028A70(CharModel* pModel, int nBone, u32 uAxes, f32 f) {
    if (nBone < 0 || nBone >= pModel->nBones) return;
    if (uAxes & 1) {
        pModel->a140[nBone][0] *= f;
    }
    if (uAxes & 4) {
        pModel->a140[nBone][1] *= f;
    }
    if (uAxes & 2) {
        pModel->a140[nBone][2] *= f;
    }
}

// TW06: SKEL_TransformBones. Builds each bone's pose and matrix from its parent's, walking the bones
// in order with aCur holding the current bone's bit: bones in aBits (or whose parent was just done)
// are redone. Bone 0x22 and 0x35 take rotations between two bones first.
void SKEL_TransformBones(CharModel* pModel, u32* aBits) {
    f32 mScale[4][4];
    f32 mOut[4][4];
    f32 vPos[4];
    u32 aCur[4];
    u32 aParent[4];
    u32 aSkel[4];
    u32 aModel[4];
    f32 qRot[4];
    Bone* pBone;
    BonePose* pPose;
    BonePose* pParentPose;
    Skeleton* pSkel;
    f32 (*pSkelRot)[4];
    int i;

    fn_8001E938(aCur, 0x80);
    fn_8001EA34(aCur, 0);
    if (pModel->pSkel != NULL) {
        if (0.0f == pModel->pSkel->fIKWeight) {
            fn_8001E938(aSkel, 0x80);
        } else {
            fn_80029EF4(pModel->pSkel->a10, aSkel, 0x80);
        }
        pSkelRot = pModel->pSkel->p28;
    } else {
        fn_8001E938(aSkel, 0x80);
        pSkelRot = NULL;
    }
    fn_80021980(pModel->a14, pModel->a24, aModel, 0x80);
    fn_8000ADC0(mScale);
    if (fn_8001E9F4(aBits, aCur, 0x80)) {
        fn_8001E85C(pModel->pBones[0].q0C, pModel->pPoses[0].q0);
        fn_8001E85C(pModel->pBones[0].v1C, pModel->pPoses[0].v10);
        fn_8000914C(pModel->pPoses[0].q0, pModel->pMatrices[0]);
        mScale[0][0] = pModel->a140[0][0];
        mScale[1][1] = pModel->a140[0][1];
        mScale[2][2] = pModel->a140[0][2];
        fn_800BADF8(pModel->pMatrices[0], mScale, mOut, 4);
        fn_8000A0E8(mOut, pModel->pMatrices[0]);
        fn_8001E880(pModel->pPoses[0].v10, pModel->pMatrices[0][3]);
        fn_80029A90(pModel, pModel->pMatrices[0], 0);
    }
    fn_80029C60(aCur, aCur, 4, 1);

    for (i = 1; i < pModel->nBones; i++) {
        pBone = &pModel->pBones[i];
        fn_8001E938(aParent, 0x80);
        fn_8001EA34(aParent, pBone->nParent);
        if (i == fn_8001EED8(pModel, 0x22)) {
            fn_80029A00(pModel, fn_8001EED8(pModel, 0x22), fn_8001EED8(pModel, 0x11), pModel->q740);
            if (fn_8001E9CC(aBits, fn_8001EED8(pModel, 0x11))) {
                fn_8001EA34(aBits, fn_8001EED8(pModel, 0x22));
            }
            if (fn_8001E9CC(aModel, fn_8001EED8(pModel, 0x11))) {
                fn_8001EA34(aModel, fn_8001EED8(pModel, 0x22));
            }
            fn_8001EA34(pModel->a24, fn_8001EED8(pModel, 0x22));
            if (fn_8001E9CC(pModel->a14, fn_8001EED8(pModel, 0x11))) {
                fn_8001EA34(pModel->a14, fn_8001EED8(pModel, 0x22));
            }
        } else if (i == fn_8001EED8(pModel, 0x35)) {
            // EA passes bone 0x22 here too
            fn_80029A00(pModel, fn_8001EED8(pModel, 0x22), fn_8001EED8(pModel, 0x11), pModel->q750);
            if (fn_8001E9CC(aBits, fn_8001EED8(pModel, 0x24))) {
                fn_8001EA34(aBits, fn_8001EED8(pModel, 0x35));
            }
            if (fn_8001E9CC(aModel, fn_8001EED8(pModel, 0x24))) {
                fn_8001EA34(aModel, fn_8001EED8(pModel, 0x35));
            }
            fn_8001EA34(pModel->a24, fn_8001EED8(pModel, 0x35));
            if (fn_8001E9CC(pModel->a14, fn_8001EED8(pModel, 0x24))) {
                fn_8001EA34(pModel->a14, fn_8001EED8(pModel, 0x35));
            }
        }
        if (fn_8001E9F4(aBits, aCur, 0x80) || fn_8001E9F4(aBits, aParent, 0x80)) {
            pPose = &pModel->pPoses[i];
            if (fn_8001E9F4(aModel, aCur, 0x80)) {
                pParentPose = &pModel->pPoses[pBone->nParent];
                if (fn_8001E9F4(pModel->a24, aCur, 0x80)) {
                    fn_800090E4(pParentPose->q0, pBone->v1C, vPos);
                    fn_800090A0(pParentPose->v10, vPos, pPose->v10);
                    pPose->v10[3] = 0.0f;
                    if (i == 1) {
                        pSkel = pModel->pSkel;
                        if (pSkel != NULL && 0.0f != pSkel->f10C4) {
                            fn_80029BF4(pPose->v10, pSkel->v10B4, pPose->v10);
                        }
                    }
                }
                if (fn_8001E9F4(pModel->a14, aCur, 0x80)) {
                    if (fn_8001E9F4(aSkel, aCur, 0x80)) {
                        fn_80008FCC(pSkelRot[i], pBone->q0C, qRot);
                        fn_80008FCC(qRot, pParentPose->q0, pPose->q0);
                    } else {
                        fn_80008FCC(pBone->q0C, pParentPose->q0, pPose->q0);
                    }
                }
            }
            fn_8000914C(pPose->q0, pModel->pMatrices[i]);
            mScale[0][0] = pModel->a140[i][0];
            mScale[1][1] = pModel->a140[i][1];
            mScale[2][2] = pModel->a140[i][2];
            if (pModel->bEE && i == fn_8001EED8(pModel, 0x52)) {
                mScale[0][0] = -mScale[0][0];   // with bEE set, bone 0x52's x is flipped
            }
            fn_800BADF8(pModel->pMatrices[i], mScale, mOut, 4);
            fn_8000A0E8(mOut, pModel->pMatrices[i]);
            fn_8001E880(pPose->v10, pModel->pMatrices[i][3]);
            fn_80029A90(pModel, pModel->pMatrices[i], i);
            fn_80021980(aBits, aCur, aBits, 0x80);
        }
        fn_80029C60(aCur, aCur, 0x80, 1);
    }
    fn_8001E8A4(pModel->a14, 0x80);
    fn_8001E8A4(pModel->a24, 0x80);
}

// Sets up: the identity rotation, then the dynamic chains.
void fn_80029530(void) {
    fn_80009710(lbl_801C6498);
    fn_80113E60();
}

// Shuts down the dynamic chains.
void fn_8002955C(void) {
    fn_80114080();
}

// Frees a model: its bones, matrices, dynamic chains and skeleton.
void fn_8002957C(CharModel* pModel) {
    int i;

    fn_80009E70(pModel->pBones);
    fn_80009E70(pModel->pMatrices);
    fn_80009E70(pModel->pPoses);
    if (pModel->pF0 != NULL) {
        fn_80114398(pModel->pF0);
    }
    if (pModel->pF4 != NULL) {
        fn_80114398(pModel->pF4);
    }
    if (pModel->pF8 != NULL) {
        fn_80114398(pModel->pF8);
    }
    for (i = 0; i < 6; i++) {
        if (pModel->apFC[i] != NULL) {
            fn_80114398(pModel->apFC[i]);
        }
    }
    for (i = 0; i < 6; i++) {
        if (pModel->ap114[i] != NULL) {
            fn_80114398(pModel->ap114[i]);
        }
    }
    if (pModel->pSkel != NULL) {
        fn_800284DC(pModel->pSkel);
    }
    fn_80009E70(pModel);
}

// Fills in aBone2: each bone maps to itself, except the pairs in lbl_8018742C, where the model's
// first bone maps to its second.
void fn_80029804(CharModel* pModel) {
    int i;
    int nA;
    int nB;

    pModel->aBone2[0] = 0;  // fake match: bone 0 on its own (a loop from 0 unrolls differently)
    for (i = 1; i < 0x59; i++) {
        pModel->aBone2[i] = i;
    }
    for (i = 0; i < 0x29; i++) {
        nA = fn_8001EED8(pModel, lbl_8018742C[i][0]);
        nB = fn_8001EED8(pModel, lbl_8018742C[i][1]);
        if (nA != 0xFF && nB != 0xFF) {
            pModel->aBone2[nA] = nB;
        }
    }
}

// The index of the bone with this id, -1 for none.
int fn_800298F4(CharModel* pModel, u64 uId) {
    int i;
    for (i = 0; i < pModel->nBones; i++) {
        if (pModel->pBones[i].uId == uId) {
            return i;
        }
    }
    return -1;
}

// Steps one of the model's dynamic chains.
void fn_80029948(CharModel* pModel, struct DynChain* pChain, f32 f) {
    fn_8011443C(pModel, pChain, f);
}

// Copies bone 0x22's rotation in pPose into q740 and q750, when the model has that bone. EA looks
// the same bone up for both.
void fn_80029968(CharModel* pModel, SkelPose* pPose) {
    int nFirst = fn_8001EED8(pModel, 0x22);
    int nSecond = fn_8001EED8(pModel, 0x22);

    if (nFirst != 0xFF) {
        fn_8001E85C(pPose->aBones[nFirst].q0, pModel->q740);
    }
    if (nSecond != 0xFF) {
        fn_8001E85C(pPose->aBones[nSecond].q0, pModel->q750);
    }
}

// Gives bone nA the rotation pRot, then sets it halfway between that and bone nB's.
void fn_80029A00(CharModel* pModel, int nA, int nB, f32* pRot) {
    fn_8001E85C(pRot, pModel->pBones[nA].q0C);
    fn_8000883C(pModel->pBones[nB].q0C, pModel->pBones[nA].q0C, 0.5f);
}

void fn_80029A74(CharModel* pModel, void* p) {
    pModel->p760 = p;
}

void fn_80029A7C(CharModel* pModel, f32 (*pMatrices)[4][4], s32 nMatrices) {
    pModel->p768 = pMatrices;
    pModel->n76C = nMatrices;
}

void fn_80029A88(CharModel* pModel, f32 (*pMatrices)[4][4]) {
    pModel->p764 = pMatrices;
}

// Transforms a bone's p764 matrix through pMtx into its p768 matrix, when the model has them.
void fn_80029A90(CharModel* pModel, f32 (*pMtx)[4], int nBone) {
    if (nBone >= pModel->n76C || pModel->p760 == NULL || pModel->p768 == NULL || pModel->p764 == NULL) {
        return;
    }
    fn_800BADF8(pMtx, pModel->p764[nBone], pModel->p768[nBone], 4);
}

// Does that for every bone, through the bone's own matrix.
void fn_80029AF8(CharModel* pModel) {
    int i;
    for (i = 0; i < pModel->nBones; i++) {
        fn_80029A90(pModel, pModel->pMatrices[i], i);
    }
}

// Square root: three Newton steps from the reciprocal-root estimate; x itself when x <= 0.
f32 fn_80029B64(f32 x) {
    volatile f32 y;     // fake match: the result goes through the stack
    f64 g;

    if (x > 0.0f) {
        g = __frsqrte((f64)x);
        g = 0.5 * g * (3.0 - g * g * x);
        g = 0.5 * g * (3.0 - g * g * x);
        g = 0.5 * g * (3.0 - g * g * x);
        y = (f32)(x * g);
        return y;
    }
    return x;
}

// Sets a vector to zero.
void fn_80029BC8(f32* pVec) {
    Vec_Copy(lbl_80186838, pVec);
}

// pA + pB into out, three floats (paired singles).
#ifdef __MWERKS__
asm void fn_80029BF4(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_80029BF4(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
}
#endif

// pA - pB into out, three floats (paired singles).
#ifdef __MWERKS__
asm void fn_80029C18(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_80029C18(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// pA - pB into out, four floats (paired singles).
#ifdef __MWERKS__
asm void fn_80029C3C(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_80029C3C(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
    pOut[3] = pA[3] - pB[3];
}
#endif

// Copies a bit array of nBits bits (whole words) from pSrc to pDst, when both are given.
void fn_80029EF4(u32* pSrc, u32* pDst, u32 nBits) {
    u32 nWords;
    u32 i;

    if (pDst != NULL) {
        if (pSrc == NULL) {
            return;
        }
        nWords = (nBits + 31) >> 5;
        for (i = 0; i < nWords; i++) {
            pDst[i] = pSrc[i];
        }
    }
}
