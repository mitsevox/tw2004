// Skeleton.c (EA's name, from its asserts; also in EA's 2002 source tree): the golfer's skeleton.
// Its IK chains and the IK weight that sets how strongly their solution is applied (0..1), and
// per-bone factors of the character's model. The types are in character.h.

#include "character.h"

void fn_8001E85C(f32* pSrc, f32* pDst);                 // copy a quaternion
void fn_8000883C(f32* pA, f32* pB, f32 fT);             // blend two quaternions by fT, into pB
void fn_8002703C(Skeleton* pSkel, IKChain* pChain, f32 fWeight);

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

// ---- sweep code (not yet cleaned up) ----

void fn_80009710();
void fn_80113E60();
void fn_80029530(void);
void fn_80114080();
void fn_8002955C(void);
void fn_8011443C();
void fn_80029948(void);
void fn_80029A74(u8* p, s32 v);
void fn_80029A7C(u8* p0, s32 p1, s32 p2);
void fn_80029A88(u8* p, s32 v);

void fn_80029530(void) {
    fn_80009710(lbl_801C6498);
    fn_80113E60();
}

void fn_8002955C(void) {
    fn_80114080();
}

void fn_80029948(void) {
    fn_8011443C();
}

void fn_80029A74(u8* p, s32 v) {
    *(s32*)(p + 0x760) = v;
}

void fn_80029A7C(u8* p0, s32 p1, s32 p2) {
    *(s32*)(p0 + 0x768) = p1;
    *(s32*)(p0 + 0x76C) = p2;
}

void fn_80029A88(u8* p, s32 v) {
    *(s32*)(p + 0x764) = v;
}

// ---- end of sweep code ----
