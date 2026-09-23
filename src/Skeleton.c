// Skeleton.c (EA's name, from its asserts; also in EA's 2002 source tree): the golfer's skeleton.
// Its IK chains and the IK weight that sets how strongly their solution is applied (0..1), and
// per-bone factors of the character's model. The types are in character.h.

#include "character.h"

void fn_8002703C(Skeleton* pSkel, IKChain* pChain, f32 fWeight);
void fn_800284DC(Skeleton* pSkel);                      // frees a skeleton
void fn_80009710(f32* pQuat);                           // identity (0, 0, 0, 1)
void fn_800BADF8(f32 (*pMtx)[4], f32 (*pSrc)[4], f32 (*pDst)[4], int nRows);   // pDst = pSrc's rows
                                                                                // through pMtx
void fn_80113E60(void);                                 // DynChain.c
void fn_80114080(void);                                 // DynChain.c
void fn_80114398(struct DynChain* pChain);              // DynChain.c: frees a chain
void fn_8011443C(CharModel* pModel, struct DynChain* pChain, f32 f);   // DynChain.c

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
    fn_80009E70(pModel->p34);
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
