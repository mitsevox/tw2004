// Skeleton.c (EA's name, from its asserts; also in EA's 2002 source tree; TW07 splits the same code
// into golf/animation/Skeleton.c and skeleton_shared.c): the golfer's skeleton. A character's model
// (CharModel) is its bones, loaded by SKEL_LoadFromMem; SKEL_UpdateState copies an animation pose
// into them (mirrored for a left-hander) and SKEL_TransformBones builds each bone's pose, matrix and
// skinning matrix from its parent's, with the per-bone scales the character sliders set.
// The swing IK (Skeleton): the first chain runs from the root up the spine and right arm to the club
// head, the second is the left arm. SKEL_InitIKSkeleton solves the first onto the ball at address
// by moving the hips (SKEL_AdjustHipPosition, SKEL_AdjustHipHeight); each frame
// SKEL_PreTransformIKSkeleton and SKEL_PostTransformIKSkeleton keep the club twist on the right
// shoulder and the left hand on the grip. The IK weight (0..1) sets how strongly the solution is
// applied; SKEL_RelaxIK and SKEL_TransitionIK blend it out and in. The types are in character.h.

#include "character.h"
#include "charstate.h"
#include "golfer.h"
#include "unsorted/cull.h"

f32 gSkelIdentityQuat[4];              // the identity rotation (quaternion), set by SKEL_InitModule
struct Character* gSkelIKCharacter;     // the character SKEL_InitIKSkeleton sets the IK up for
                                        // (SKEL_AdjustHipPosition moves its test points)

void vec4flt_Zero(f32* pVec);
void SKEL_TransformIKChain(CharModel* pModel, IKChain* pChain);
f32  SKEL_ItterateIKChain(CharModel* pModel, IKChain* pChain, f32* pTarget, int nLast, int nFirst);
void SKEL_InitIKChain(CharModel* pModel, IKChain* pChain);
void SKEL_TransformIKChainFromBones(CharModel* pModel, IKChain* pChain);
f32  SKEL_AdjustHipHeight(CharModel* pModel, IKChain* pChain, f32* pTarget);
void Quat_Invert(f32* pQ, f32* pOut);                   // Quaternion.c
void SKA_BlendVec3(f32* pA, f32* pB, f32* pOut, f32 fT);  // a blend of two points by fT
void SKA_BlendQuat(f32* pA, f32* pB, f32* pOut, f32 fT);  // a blend of two rotations by fT
void LLMath_mat44fltMultiply(f32 mtx[4][4], Vec4* src, Vec4* dst);  // VecMath.c: a point through a matrix
void LLMath_InvertNormalized(f32 (*pSrc)[4], f32 (*pDst)[4]);  // UMemPool.c: rotation+translation inverse
void Quat_BuildFromMatrix(f32 (*m)[4], f32* pQ);                 // Quaternion.c: a rotation matrix's quaternion
void Character_UpdateFeetTerrainInfo(Character* pChar, int bNormals);   // char.c
void Character_PlaceFeetOnGround(Character* pChar);                     // char.c
// VecMath.c: each of pSrc's rows through pMtx into pDst.
void LLMath_mat44fltMultiplyList(f32 (*pMtx)[4], f32 (*pSrc)[4], f32 (*pDst)[4], int nRows);
void fn_80113E60(void);                                 // DynChain.c
void fn_80114080(void);                                 // DynChain.c
void fn_80114398(struct DynChain* pChain);              // DynChain.c: frees a chain
void fn_8011443C(CharModel* pModel, struct DynChain* pChain, f32 fDelta);   // DynChain.c
void Quat_Add(f32* pA, f32* pB, f32* pOut);           // Quaternion.c
void Quat_RotateVector(f32* pQuat, f32* pIn, f32* pOut);       // Quaternion.c: a vector turned by it
void Quat_BuildFromVectorAndScale(f32* pAxis, f32* pOut, f32 fAngle);     // Quaternion.c: an axis-angle rotation
void Quat_QuatToMatrix(f32* pQ, f32 (*pMtx)[4]);               // Quaternion.c: a rotation's matrix
void LLMath_CopyMat44(f32 (*pSrc)[4], f32 (*pDst)[4]);        // copies a matrix
void LLMath_IdentifyMat(f32 (*pMtx)[4]);                        // identity
void SKEL_SetHalfJoint(CharModel* pModel, int nBone, int nJoint, f32* pRest);
void SKEL_Vec3Add(f32* pA, f32* pB, f32* pOut);
void SKEL_VecSub(f32* pA, f32* pB, f32* pOut);
void SKEL_Vec3Sub(f32* pA, f32* pB, f32* pOut);
void BitArray_ShiftUp(u32* aSrc, u32* aDst, u32 nBits, u32 nShift);
void BitArray_CopyArray(u32* pSrc, u32* pDst, u32 nBits);
void SKEL_GenerateBoneLookupTable(CharModel* pModel);
void SKEL_GenerateLeftHandedTable(CharModel* pModel);
void SKEL_TransformBones(CharModel* pModel, u32* aBits);
Skeleton* SKEL_CreateIKSkeleton(CharModel* pModel, CharModelDefs* pDefs);
struct DynChain* fn_80114270(CharModel* pModel, int nBone, s32 nType, s32 n10);   // DynChain.c

// Names the club bone (IGdriver, 0x52) can go by in a model (SKEL_GenerateBoneLookupTable).
char* gSkelClubBoneNames[5] = { "IGDriver", "IGputter", "IGiron3", "IGiron7", "IGwedge" };

// Left/right bone id pairs, each way round: the arms and hands (0x10-0x1E rcolr..rt3 with
// 0x23-0x31 lcolr..lt3) and the legs (0x36-0x3A rhip..rtoe with 0x44-0x48 lhip..ltoe). A
// left-handed golfer's pose puts each bone's rotation on its partner (SKEL_GenerateLeftHandedTable
// reads the first 41; the last two are empty).
u8 gSkelLeftHandedPairs[42][2] = {
    { 0x23, 0x10 }, { 0x24, 0x11 }, { 0x25, 0x12 }, { 0x26, 0x13 }, { 0x27, 0x14 }, { 0x28, 0x15 },
    { 0x29, 0x16 }, { 0x2A, 0x17 }, { 0x2B, 0x18 }, { 0x2C, 0x19 }, { 0x2D, 0x1A }, { 0x2E, 0x1B },
    { 0x2F, 0x1C }, { 0x30, 0x1D }, { 0x31, 0x1E }, { 0x10, 0x23 }, { 0x11, 0x24 }, { 0x12, 0x25 },
    { 0x13, 0x26 }, { 0x14, 0x27 }, { 0x15, 0x28 }, { 0x16, 0x29 }, { 0x17, 0x2A }, { 0x18, 0x2B },
    { 0x19, 0x2C }, { 0x1A, 0x2D }, { 0x1B, 0x2E }, { 0x1C, 0x2F }, { 0x1D, 0x30 }, { 0x1E, 0x31 },
    { 0x44, 0x36 }, { 0x45, 0x37 }, { 0x46, 0x38 }, { 0x47, 0x39 }, { 0x48, 0x3A }, { 0x36, 0x44 },
    { 0x37, 0x45 }, { 0x38, 0x46 }, { 0x39, 0x47 }, { 0x3A, 0x48 },
};

// The top bones of the model's dynamic chains (SKEL_LoadFromMem): the trouser legs (rbpnt1,
// rfpnt1, ropnt1, lbpnt1, lfpnt1, lopnt1: kind 2) and the sleeves (rslvBjnt, rslvFjnt, rslvHjnt and
// the left ones: kind 3).
u8 gSkelPantBones[6] = { 0x3E, 0x41, 0x3B, 0x4C, 0x4F, 0x49 };
u8 gSkelSleeveBones[6] = { 0x1F, 0x20, 0x21, 0x32, 0x33, 0x34 };
u8 gSkelIKEnabled = 1;                  // SKEL_EnableIK; while 0 the IK functions do nothing

// Makes each turning link's accumulated turn (v58, axis times angle) its bone's IK rotation in the
// skeleton (pIKRots). Links with fTurnShare at 0 are left alone.
void SKEL_SetIKChainRotations(Skeleton* pSkel, IKChain* pChain) {
    int i;
    for (i = 0; i < pChain->nLinks; i++) {
        IKLink* pLink = &pChain->pLinks[i];
        if (pLink->fTurnShare > 0.0f) {
            Quat_BuildFromVector(pLink->v58, pSkel->pIKRots[pLink->nBone]);
        }
    }
}

// Poses the chain's bones from its links: the first link's bone takes the link's rotation (q18) and
// position (v28); each later bone is placed at its offset (v28) from the link before it, turned by
// that link's rotation, and turned by its IK rotation (pIKRots) and its rotation from that link
// (q18). Only the poses change, not the matrices.
void SKEL_TransformIKChain(CharModel* pModel, IKChain* pChain) {
    f32 vOffset[4];
    f32 qRot[4];
    int i;
    IKLink* pLink;
    BonePose* pPose;
    BonePose* pPrev;
    int nBone;
    Skeleton* pSkel = pModel->pSkel;

    Quat_Copy(pChain->pLinks[0].q18, pModel->pPoses[pChain->pLinks[0].nBone].q0);
    Quat_Copy(pChain->pLinks[0].v28, pModel->pPoses[pChain->pLinks[0].nBone].v10);
    for (i = 1; i < pChain->nLinks; i++) {
        pLink = &pChain->pLinks[i];
        nBone = pLink->nBone;
        pPose = &pModel->pPoses[nBone];
        pPrev = &pModel->pPoses[pChain->pLinks[pLink->nPrev].nBone];
        Quat_RotateVector(pPrev->q0, pLink->v28, vOffset);
        Quat_Add(pPrev->v10, vOffset, pPose->v10);
        pPose->v10[3] = 0.0f;
        Quat_Multiply(pSkel->pIKRots[nBone], pLink->q18, qRot);
        Quat_Multiply(qRot, pPrev->q0, pPose->q0);
    }
}

// One IK iteration (cyclic coordinate descent): from link nLast back to link nFirst, turns each
// link with fTurnShare above 0 so the chain's end swings toward pTarget, by fTurnShare of the angle
// between them (none under PI/5000), never about the bone's own y axis or the link's locked axis
// nLockedAxis (-1: none); each turn adds up in the link's v58 and the IK rotations are then set
// (SKEL_SetIKChainRotations). The chain's end position goes into v8. Returns how far the end then
// is from pTarget.
f32 SKEL_ItterateIKChain(CharModel* pModel, IKChain* pChain, f32* pTarget, int nLast, int nFirst) {
    f32 vEnd[4];
    f32 vDiff[4];
    f32 vToEnd[4];
    f32 vToTarget[4];
    f32 qInv[4];
    f32 vAxis[4];
    f32 qTurn[4];
    f32 vTurned[4];
    f32 qRot[4];
    f32 vLocal[4];
    Skeleton* pSkel = pModel->pSkel;
    IKLink* pLink;
    int nBone;
    f32 fLen;
    f32 fAngle;
    f32 fCos;
    int i;

    Vec4_CopyPoint(pModel->pPoses[pChain->pLinks[pChain->nLinks - 1].nBone].v10, vEnd);
    vToEnd[3] = 0.0f;
    vEnd[3] = 1.0f;
    for (i = nLast; i >= nFirst; i--) {
        pLink = &pChain->pLinks[i];
        if (pLink->fTurnShare > 0.0f) {
            nBone = pLink->nBone;
            SKEL_Vec3Sub(vEnd, pModel->pPoses[nBone].v10, vToEnd);
            SKEL_Vec3Sub(pTarget, pModel->pPoses[nBone].v10, vToTarget);
            fLen = Math_Sqrtf(Vec3_LengthSqClamped(vToEnd) * Vec3_LengthSqClamped(vToTarget));
            fCos = Vec3_Dot(vToEnd, vToTarget) / fLen;
            if (fCos > 1.0f || fCos < -1.0f) {
                fAngle = 0.0f;
            } else {
                fAngle = pLink->fTurnShare * Math_Acos(fCos);
            }
            if (fabsf(fAngle) > PI / 5000.0f) {
                // the turn's axis in the bone's own frame, without its y and locked (nLockedAxis)
                // components
                vec4flt_CrossProduct(vToEnd, vToTarget, vAxis);
                Quat_Invert(pModel->pPoses[nBone].q0, qInv);
                Quat_RotateVector(qInv, vAxis, vLocal);
                vLocal[3] = 0.0f;
                vLocal[1] = 0.0f;
                if (pLink->nLockedAxis >= 0) {
                    vLocal[pLink->nLockedAxis] = 0.0f;
                }
                LLMath_Normalize3(vLocal, vLocal);
                Vec3_Scale(fAngle, vLocal, vLocal);
                SKEL_Vec3Add(vLocal, pLink->v58, pLink->v58);
                Quat_BuildFromVectorAndScale(vLocal, qTurn, fAngle);
                Quat_RotateVector(pModel->pPoses[nBone].q0, qTurn, qRot);
                Quat_RotateVector(qRot, vToEnd, vTurned);
                SKEL_Vec3Add(pModel->pPoses[nBone].v10, vTurned, vEnd);
            }
        }
    }
    SKEL_SetIKChainRotations(pSkel, pChain);
    Vec3Copy(vEnd, pChain->v8);
    SKEL_Vec3Sub(vEnd, pTarget, vDiff);
    return (f32)Math_Sqrt(Vec3_LengthSqClamped(vDiff));
}

// fake match: EA's file had a function here that the linker stripped; it used the 0.5 and 3.0 of
// Math_Sqrtf's square root first, which puts them here in .sdata2.
static f64 Skeleton_StrippedFn(f64 x) {
    return 0.5 * x * (3.0 - x);
}

// Takes the chain's IK off its bones: each link's IK rotation (pIKRots) becomes the identity and
// its bone leaves the skeleton's IK set (aIKBones); all links with bAll, else only those that turn
// (fTurnShare above 0).
void SKEL_ResetIKChain(Skeleton* pSkel, IKChain* pChain, u8 bAll) {
    int i;
    for (i = 0; i < pChain->nLinks; i++) {
        if (bAll || pChain->pLinks[i].fTurnShare > 0.0f) {
            int nBone = pChain->pLinks[i].nBone;
            Quat_IdentifyForMul(pSkel->pIKRots[nBone]);
            BitArray_ClearBit(pSkel->aIKBones, nBone);
        }
    }
}

// Copies the chain's IK rotations (pIKRots) into pWeightedRots; below full weight, each is scaled
// down by the weight (a slerp from no rotation) and normalized.
void SKEL_WeightIKChain(Skeleton* pSkel, IKChain* pChain, f32 fWeight) {
    int i;
    for (i = 0; i < pChain->nLinks; i++) {
        int nBone = pChain->pLinks[i].nBone;
        Quat_Copy(pSkel->pIKRots[nBone], pSkel->pWeightedRots[nBone]);
        if (fWeight < 1.0f) {
            Quat_Slerp(gSkelIdentityQuat, pSkel->pWeightedRots[nBone], fWeight);
            LLMath_Normalize(pSkel->pWeightedRots[nBone], pSkel->pWeightedRots[nBone]);
        }
    }
}

// Puts the IK back at rest: no transition running (fIKBlendLeft), no bone in the IK set, no hip
// offset (vHipOffset, vHipOffsetWeighted), f10C4 at 1, and every chain's IK rotations reset
// (SKEL_ResetIKChain).
void SKEL_ResetIKSkeleton(Skeleton* pSkel) {
    int i;

    pSkel->fIKBlendLeft = 0.0f;
    BitArray_ClearArray(pSkel->aIKBones, 0x80);
    vec4flt_Zero(pSkel->vHipOffset);
    vec4flt_Zero(pSkel->vHipOffsetWeighted);
    pSkel->f10C4 = 1.0f;
    for (i = 0; i < pSkel->nChains; i++) {
        SKEL_ResetIKChain(pSkel, &pSkel->pChains[i], 1);
    }
}

// Sets the chain's links up from the model's current pose: each link's rotation (q18) and offset
// (v28) from the link before it, taken from its bone's own local ones when that link's bone is its
// parent, else worked out from the two poses (and kept in q38/v48, with b0 bit 1, when the bone's
// parent comes before the chain's first bone); the first link takes its bone's pose. Every turn
// (v58) is cleared and each turning link's bone (fTurnShare above 0) joins the skeleton's IK set
// (aIKBones). v8 takes the chain end's position.
void SKEL_InitIKChain(CharModel* pModel, IKChain* pChain) {
    f32 qInv[4];
    f32 vDelta[4];
    int nPrevBone;
    int nBone;
    IKLink* pLink;
    Bone* pBone;
    Skeleton* pSkel = pModel->pSkel;
    int i;
    IKLink* pPrevLink;
    int nPrev;

    for (i = 0; i < pChain->nLinks; i++) {
        pLink = &pChain->pLinks[i];
        nBone = pLink->nBone;
        nPrev = pLink->nPrev;
        nPrevBone = pChain->pLinks[nPrev].nBone;
        pBone = &pModel->pBones[nBone];
        pLink->b0 = 0;
        vec4flt_Zero(pLink->v58);
        if (pLink->fTurnShare > 0.0f) {
            BitArray_SetBit(pSkel->aIKBones, pLink->nBone);
        }
        if (i > 0) {
            pPrevLink = &pChain->pLinks[nPrev];
        } else {
            pPrevLink = NULL;
        }
        if (i == 0) {
            Quat_Copy(pModel->pPoses[nBone].q0, pLink->q18);
        } else {
            Quat_Copy(pBone->q0C, pLink->q18);
        }
        if (pPrevLink != NULL && pPrevLink->nBone != pBone->nParent) {
            Quat_Invert(pModel->pPoses[nPrevBone].q0, qInv);
            Quat_Multiply(pModel->pPoses[nBone].q0, qInv, pLink->q18);
            SKEL_VecSub(pModel->pPoses[nBone].v10, pModel->pPoses[nPrevBone].v10, vDelta);
            vDelta[3] = 0.0f;
            Quat_RotateVector(qInv, vDelta, pLink->v28);
            pLink->v28[3] = 0.0f;
            if (pBone->nParent < pChain->pLinks[0].nBone) {
                Quat_Copy(pLink->q18, pLink->q38);
                Quat_Copy(pLink->v28, pLink->v48);
                pLink->b0 |= 1;
            }
        } else if (i == 0) {
            LLMath_CopyVec(pModel->pPoses[nBone].v10, pLink->v28);
        } else {
            LLMath_CopyVec(pBone->v1C, pLink->v28);
        }
    }
    Vec4_CopyPoint(pModel->pPoses[pChain->pLinks[pChain->nLinks - 1].nBone].v10, pChain->v8);
}

// Solves the chain toward pTarget: up to nIterations IK iterations (SKEL_ItterateIKChain over all
// links but the last, then the chain posed again), each error taken from pfnError when one is
// given; stops once the error is below fTolerance.
void SKEL_SolveIKChain(CharModel* pModel, IKChain* pChain, f32* pTarget, s32 nIterations,
                       f32 (*pfnError)(CharModel* pModel, IKChain* pChain, f32* pTarget), f32 fTolerance) {
    s32 i;
    f32 fError;

    for (i = 0; i < nIterations; i++) {
        fError = SKEL_ItterateIKChain(pModel, pChain, pTarget, pChain->nLinks - 2, 0);
        if (pfnError != NULL) {
            fError = pfnError(pModel, pChain, pTarget);
        }
        SKEL_TransformIKChain(pModel, pChain);
        if (fError < fTolerance) {
            break;
        }
    }
}

// Re-poses the chain's bones and their matrices from the link before each: a link with b0 bit 1
// from its kept rotation and offset (q38, v48), any other, once the link before it was redone, from
// its bone's own local rotation and position (q0C, v1C), so without the IK rotations.
void SKEL_TransformIKChainFromBones(CharModel* pModel, IKChain* pChain) {
    f32 vOffset[4];
    IKLink* pLink;
    int i;
    int nBone;
    int nPrevBone;
    u64 uDone;
    u64 uPrevBit;
    int bKept;
    Bone* pBone;
    BonePose* pPose;
    BonePose* pPrev;

    CharModel_GetBoneIndex(pModel, 0x52);  // EA drops the answer
    uDone = 0;
    for (i = 0; i < pChain->nLinks; i++) {
        pLink = &pChain->pLinks[i];
        nBone = pLink->nBone;
        nPrevBone = pChain->pLinks[pLink->nPrev].nBone;
        // EA bug: a bone past 63 shifts out of the 64-bit mask (the compiler's helper gives 0)
        uPrevBit = (u64)1 << nPrevBone;
        bKept = pLink->b0 & 1;
        if (bKept || (uDone & uPrevBit)) {
            pBone = &pModel->pBones[nBone];
            pPose = &pModel->pPoses[nBone];
            pPrev = &pModel->pPoses[nPrevBone];
            if (bKept) {
                Quat_RotateVector(pPrev->q0, pLink->v48, vOffset);
                Quat_Multiply(pLink->q38, pPrev->q0, pPose->q0);
            } else {
                Quat_RotateVector(pPrev->q0, pBone->v1C, vOffset);
                Quat_Multiply(pBone->q0C, pPrev->q0, pPose->q0);
            }
            Quat_Add(pPrev->v10, vOffset, pPose->v10);
            pPose->v10[3] = 0.0f;
            Quat_QuatToMatrix(pPose->q0, pModel->pMatrices[nBone]);
            Vec4_CopyPoint(pPose->v10, pModel->pMatrices[nBone][3]);
            uDone |= (u64)1 << nBone;
        }
    }
}

// SKEL_InitIKSkeleton's IK error for the first chain (the root up to the club head): raises or
// lowers the hips toward the target. The height miss (pTarget's less the chain end's) adds
// fHipFollow of itself to the hip offset vHipOffset[1], held between -fHipLowerMax and
// fHipRaiseMax; the chain's end moves by the miss less what the clamp cut, and the root link's
// height becomes fRootLinkHeight plus the offset. Returns how far the chain's end then is from
// pTarget.
f32 SKEL_AdjustHipHeight(CharModel* pModel, IKChain* pChain, f32* pTarget) {
    f32 vDiff[4];
    Skeleton* pSkel = pModel->pSkel;
    f32 fDy;

    CharModel_GetBoneIndex(pModel, 1);  // EA drops the answer
    fDy = pTarget[1] - pChain->v8[1];
    pSkel->vHipOffset[1] = fDy * pSkel->fHipFollow + pSkel->vHipOffset[1];
    if (pSkel->vHipOffset[1] > pSkel->fHipRaiseMax) {
        pChain->v8[1] += fDy - (pSkel->vHipOffset[1] - pSkel->fHipRaiseMax);
        pSkel->vHipOffset[1] = pSkel->fHipRaiseMax;
    } else if (pSkel->vHipOffset[1] < -pSkel->fHipLowerMax) {
        pChain->v8[1] += fDy - (pSkel->vHipOffset[1] + pSkel->fHipLowerMax);
        pSkel->vHipOffset[1] = -pSkel->fHipLowerMax;
    } else {
        pChain->v8[1] += fDy;
    }
    pChain->pLinks[0].v28[1] = pSkel->fRootLinkHeight + pSkel->vHipOffset[1];
    SKEL_Vec3Sub(pTarget, pChain->v8, vDiff);
    Vec4_CopyPoint(pChain->v8, pModel->pPoses[pChain->pLinks[pChain->nLinks - 1].nBone].v10);
    return (f32)Math_Sqrt(Vec3_LengthSqClamped(vDiff));
}

// Turns the IK on or off.
void SKEL_EnableIK(u8 bOn) {
    gSkelIKEnabled = bOn;
}

// Applies the IK weight to the first IK chain.
void SKEL_WeightFirstIKChain(Skeleton* pSkel, f32 fWeight) {
    SKEL_WeightIKChain(pSkel, pSkel->pChains, fWeight);
}

// Sets how strongly the IK solution is applied (fWeight, 0..1). At 0 or 1 the bones use the IK
// rotations (pIKRots) as they are; in between, the first chain's are scaled by the weight into
// pWeightedRots and those are used. The hip offset used (vHipOffsetWeighted) becomes vHipOffset
// times the weight, and f10C4 the weight. Nothing without a skeleton or while the IK is off.
void SKEL_SetIKSolutionWeight(Skeleton* pSkel, f32 fWeight) {
    if (pSkel == NULL || gSkelIKEnabled == 0) return;
    pSkel->fIKWeight = fWeight;
    if (0.0f == fWeight) {
        pSkel->pUsedRots = pSkel->pIKRots;
    } else if (1.0f == fWeight) {
        pSkel->pUsedRots = pSkel->pIKRots;
    } else {
        SKEL_WeightFirstIKChain(pSkel, fWeight);
        pSkel->pUsedRots = pSkel->pWeightedRots;
    }
    pSkel->f10C4 = fWeight;
    Vec3_Scale(fWeight, pSkel->vHipOffset, pSkel->vHipOffsetWeighted);
}

// Sets the extra rotation (quaternion pRot) put on the right shoulder while the IK runs
// (SKEL_PreTransformIKSkeleton): the club twist SW_vUIAdjustClub reads off the stick. Below full IK
// weight it is scaled down by the weight. Nothing without a skeleton.
void SKEL_SetExtraRightShoulderRotation(CharModel* pModel, f32* pRot) {
    if (pModel->pSkel != NULL) {
        Quat_Copy(pRot, pModel->pSkel->qShoulderRot);
        if (pModel->pSkel->fIKWeight < 1.0f) {
            Quat_Slerp(gSkelIdentityQuat, pModel->pSkel->qShoulderRot, pModel->pSkel->fIKWeight);
            LLMath_Normalize(pModel->pSkel->qShoulderRot, pModel->pSkel->qShoulderRot);
        }
    }
}

// Before the bones are posed (Character_UpdateAnimation): while nShoulderFrames counts down
// (Swing.c sets it to 4), puts the extra right-shoulder rotation (qShoulderRot) on the right
// shoulder's (0x11) IK rotation; SKEL_PostTransformIKSkeleton takes it off again. Nothing without a
// skeleton, with the IK off or at no weight.
void SKEL_PreTransformIKSkeleton(CharModel* pModel) {
    Skeleton* pSkel = pModel->pSkel;
    f32 qRot[4];

    if (pSkel == NULL) return;
    if (gSkelIKEnabled == 0 || pSkel->fIKWeight <= 0.0f) return;
    if (pSkel->nShoulderFrames != 0) {
        Quat_Multiply(pSkel->qShoulderRot, pSkel->pIKRots[CharModel_GetBoneIndexMapped(pModel, 0x11)], qRot);
        Quat_Copy(qRot, pModel->pSkel->pIKRots[CharModel_GetBoneIndexMapped(pModel, 0x11)]);
    }
}

// Starts blending the IK out over 0.25 seconds (fIKBlendLeft the time left, fIKBlendTime its
// length; SKEL_PostTransformIKSkeleton runs it), when the IK has any weight. Nothing without a
// skeleton or while the IK is off.
void SKEL_RelaxIK(Skeleton* pSkel) {
    if (pSkel == NULL || gSkelIKEnabled == 0) return;
    if (pSkel->fIKWeight > 0.0f) {
        pSkel->fIKBlendLeft = 0.25f;
        pSkel->fIKBlendTime = 0.25f;
    }
}

// Starts blending the IK out (bRelax, when it has any weight) or in (below full weight) over fTime
// seconds; a negative time left (fIKBlendLeft) marks a blend in. SKEL_PostTransformIKSkeleton runs
// it. Nothing without a skeleton or while the IK is off.
void SKEL_TransitionIK(Skeleton* pSkel, u8 bRelax, f32 fTime) {
    if (pSkel == NULL || gSkelIKEnabled == 0) return;
    if (bRelax) {
        if (pSkel->fIKWeight > 0.0f) {
            pSkel->fIKBlendTime = fTime;
            pSkel->fIKBlendLeft = fTime;
        }
    } else if (pSkel->fIKWeight < 1.0f) {
        pSkel->fIKBlendLeft = -fTime;
        pSkel->fIKBlendTime = fTime;
    }
}

// After the bones are posed each frame (Character_UpdateAnimation). First runs a blend of the IK
// weight (SKEL_RelaxIK, SKEL_TransitionIK): a positive time left (fIKBlendLeft) counts down to 0
// and the weight with it, a negative one counts up to 0 and the weight up to 1. Then, with any
// weight: takes the extra right-shoulder rotation (qShoulderRot) back off while nShoulderFrames
// counts down; re-poses the first chain from its bones' own rotations
// (SKEL_TransformIKChainFromBones), and while the character's flag 0x4000 is set and the weight is
// between 0 and 1, blends the club bone (0x52) from its pose before that toward the new one by the
// weight. Then solves the second chain (the left arm) so the left wrist (0x28) reaches its place on
// the club (v108C, SKEL_SaveLeftHandGrip), turns the wrist to its rotation on the club (q107C),
// rebuilds the bones from the left collarbone (0x23) down, keeping the wrist's rotation, and the
// club head's test point from the club's matrix, and resets the left arm's chain. Bone ids go
// through the left-handed map.
void SKEL_PostTransformIKSkeleton(Character* pChar) {
    f32 vGrip[4];
    f32 qGrip[4];
    f32 vTarget[4];
    u32 aBits[4];
    f32 qRot[4];
    f32 qInv[4];
    CharModel* pModel = pChar->pModel;
    Skeleton* pSkel = pModel->pSkel;
    IKChain* pChain;
    BonePose* pGrip;
    BonePose* pWrist;
    int nGrip;
    int nWrist;

    if (pSkel == NULL || gSkelIKEnabled == 0) return;
    if (pSkel->fIKBlendLeft > 0.0f) {
        pModel->pSkel->fIKBlendLeft -= gSession.fFrameTime;
        if (pModel->pSkel->fIKBlendLeft < 0.0f) {
            pModel->pSkel->fIKBlendLeft = 0.0f;
        }
        SKEL_SetIKSolutionWeight(pModel->pSkel, pModel->pSkel->fIKBlendLeft / pModel->pSkel->fIKBlendTime);
    } else if (pSkel->fIKBlendLeft < 0.0f) {
        pSkel->fIKBlendLeft += gSession.fFrameTime;
        if (pModel->pSkel->fIKBlendLeft > 0.0f) {
            pModel->pSkel->fIKBlendLeft = 0.0f;
        }
        SKEL_SetIKSolutionWeight(pModel->pSkel, 1.0f + pModel->pSkel->fIKBlendLeft
                                 / pModel->pSkel->fIKBlendTime);
    }
    if (pSkel->fIKWeight <= 0.0f) return;

    pChain = &pSkel->pChains[1];
    if (pSkel->nShoulderFrames != 0) {
        Quat_Invert(pSkel->qShoulderRot, qInv);
        Quat_Multiply(qInv, pSkel->pIKRots[CharModel_GetBoneIndexMapped(pModel, 0x11)], qRot);
        Quat_Copy(qRot, pSkel->pIKRots[CharModel_GetBoneIndexMapped(pModel, 0x11)]);
        pSkel->nShoulderFrames--;
    }
    nGrip = CharModel_GetBoneIndex(pModel, 0x52);
    pGrip = &pModel->pPoses[nGrip];
    if (pSkel->fIKWeight > 0.0f && pSkel->fIKWeight < 1.0f) {
        Quat_Copy(pGrip->q0, qGrip);
        Quat_Copy(pGrip->v10, vGrip);
    }
    SKEL_TransformIKChainFromBones(pModel, pSkel->pChains);
    if (pChar->uCharFlags & 0x4000) {
        if (pSkel->fIKWeight > 0.0f && pSkel->fIKWeight < 1.0f) {
            Quat_Slerp(qGrip, pGrip->q0, pSkel->fIKWeight);
            SKA_BlendVec3(pGrip->v10, vGrip, pGrip->v10, 1.0f - pSkel->fIKWeight);
            Quat_QuatToMatrix(pGrip->q0, pModel->pMatrices[nGrip]);
            Vec4_CopyPoint(pGrip->v10, pModel->pMatrices[nGrip][3]);
        }
    }
    SKEL_InitIKChain(pModel, pChain);
    CharModel_GetBoneIndexMapped(pModel, 0x15);  // EA drops the answer
    nWrist = CharModel_GetBoneIndexMapped(pModel, 0x28);
    pWrist = &pModel->pPoses[nWrist];
    Quat_RotateVector(pGrip->q0, pSkel->v108C, vTarget);
    Quat_Add(vTarget, pGrip->v10, vTarget);
    vTarget[3] = 0.0f;
    SKEL_SolveIKChain(pModel, pChain, vTarget, pChain->nMaxIterations, NULL, pChain->fTolerance);
    if (pSkel->fIKWeight < 1.0f) {
        SKEL_WeightIKChain(pSkel, pChain, pSkel->fIKWeight);
    }
    Quat_Multiply(pSkel->q107C, pGrip->q0, pWrist->q0);
    BitArray_ClearBit(pModel->a14, nWrist);
    BitArray_ClearArray(aBits, 0x80);
    BitArray_SetBit(aBits, CharModel_GetBoneIndexMapped(pModel, 0x23));
    SKEL_TransformBones(pModel, aBits);
    LLMath_mat44fltMultiply(pModel->pMatrices[nGrip], (Vec4*)pChar->pClubSet->a3C[pChar->nClubClass],
                (Vec4*)pChar->aTestPoints[4]);
    SKEL_ResetIKChain(pModel->pSkel, pChain, 0);
}

// Records where the left wrist (0x28) sits on the club (bone 0x52, mirrored in x for a
// left-hander): its rotation relative to the club's into the skeleton's q107C and its position into
// v108C, where SKEL_PostTransformIKSkeleton puts it back each frame. Nothing without a skeleton or
// while the IK is off.
void SKEL_SaveLeftHandGrip(Character* pChar) {
    f32 mInv[4][4];
    f32 mRel[4][4];
    f32 mGrip[4][4];
    f32 mFlip[4][4];
    CharModel* pModel = pChar->pModel;
    f32 (*pWristMtx)[4];
    f32 (*pGripMtx)[4];

    if (pModel->pSkel == NULL || gSkelIKEnabled == 0) return;
    pWristMtx = pModel->pMatrices[CharModel_GetBoneIndexMapped(pModel, 0x28)];
    pGripMtx = pModel->pMatrices[CharModel_GetBoneIndex(pModel, 0x52)];
    if (pModel->bLeftHanded) {
        LLMath_IdentifyMat(mFlip);
        mFlip[0][0] = -1.0f;
        LLMath_mat44fltMultiplyList(pGripMtx, mFlip, mGrip, 4);
    } else {
        LLMath_CopyMat44(pGripMtx, mGrip);
    }
    LLMath_InvertNormalized(mGrip, mInv);
    LLMath_mat44fltMultiplyList(mInv, pWristMtx, mRel, 4);
    Quat_BuildFromMatrix(mRel, pModel->pSkel->q107C);
    LLMath_mat44fltMultiply(mInv, (Vec4*)pWristMtx[3], (Vec4*)pModel->pSkel->v108C);
    pModel->pSkel->v108C[3] = 0.0f;
}

// Moves the chain's bones up by fDy, in their poses and matrices.
void SKEL_TranslateIKChainY(CharModel* pModel, IKChain* pChain, f32 fDy) {
    int i;
    for (i = 0; i < pChain->nLinks; i++) {
        int nBone = pChain->pLinks[i].nBone;
        pModel->pPoses[nBone].v10[1] += fDy;
        pModel->pMatrices[nBone][3][1] += fDy;
    }
}

// When the club head (the chain's end) hangs more than 0.1 below pTarget, moves the golfer back
// from it: the drop (eased in between 0.1 and 0.2, at most 0.65, then times 0.75) becomes a
// sideways move, away from the club head, of the chain's root, the model's root bone and
// gSkelIKCharacter's test points. The feet are then put back on the ground (with bNormals, their
// terrain is read first), and the chain takes the root's height change and is posed again. Returns
// the move (negative), 0 when nothing moved.
f32 SKEL_AdjustHipPosition(CharModel* pModel, IKChain* pChain, f32* pTarget, int bNormals) {
    f32 fDx;
    f32 fDz;
    f32 fDrop;
    f32 fOldY;
    f32 fScale;
    f32 fDy;
    int i;

    fDrop = pChain->v8[1] - pTarget[1];
    if (fDrop < -0.1f) {
        if (fDrop > -0.2f) {
            fDrop *= (fDrop - -0.1f) / -0.1f;
        }
        if (fDrop < -0.65f) {
            fDrop = -0.65f;
        }
        fDrop *= 0.75f;
        fDx = pChain->v8[0] - pChain->pLinks[0].v28[0];
        fDz = pChain->v8[2] - pChain->pLinks[0].v28[2];
        fScale = fDrop / (f32)Math_Sqrt(fDx * fDx + fDz * fDz);
        fDx *= fScale;
        fDz *= fScale;
        pChain->pLinks[0].v28[0] += fDx;
        pChain->pLinks[0].v28[2] += fDz;
        pModel->pBones[0].v1C[0] += fDx;
        pModel->pBones[0].v1C[2] += fDz;
        for (i = 0; i < 5; i++) {
            gSkelIKCharacter->aTestPoints[i][0] += fDx;
            gSkelIKCharacter->aTestPoints[i][2] += fDz;
        }
        if (bNormals) {
            Character_UpdateFeetTerrainInfo(gSkelIKCharacter, 1);
        }
        fOldY = pModel->pBones[0].v1C[1];
        Character_PlaceFeetOnGround(gSkelIKCharacter);
        fDy = pModel->pBones[0].v1C[1] - fOldY;
        pChain->pLinks[0].v28[1] += fDy;
        pChain->v8[1] += fDy;
        pChain->v8[0] += fDx;
        pChain->v8[2] += fDz;
        SKEL_TransformIKChain(pModel, pChain);
        return fDrop;
    }
    return 0.0f;
}

// Sets the golfer's IK up for a shot (Character_SetupForShot) toward pTarget, the club head's goal:
// records the left hand on the club (SKEL_SaveLeftHandGrip), sets the first chain (the root up to
// the club head) up from the pose, moves the golfer back when the club head hangs too low
// (SKEL_AdjustHipPosition), resets the hip offset (at most 0.025 up and 0.15 down, 0.025 of each
// miss) and the extra shoulder rotation, solves the chain with SKEL_AdjustHipHeight as its error
// and applies it at full weight. Returns SKEL_AdjustHipPosition's move (0 or negative); 0 while the
// IK is off.
f32 SKEL_InitIKSkeleton(Character* pChar, f32* pTarget, int bNormals) {
    CharModel* pModel = pChar->pModel;
    Skeleton* pSkel = pModel->pSkel;
    IKChain* pChain = pModel->pSkel->pChains;   // EA bug: read before the NULL test below
    f32 fDrop;

    if (pSkel == NULL || gSkelIKEnabled == 0) return 0.0f;
    gSkelIKCharacter = pChar;
    SKEL_SaveLeftHandGrip(pChar);
    SKEL_InitIKChain(pModel, pChain);
    fDrop = SKEL_AdjustHipPosition(pModel, pChain, pTarget, bNormals);
    pSkel->fHipRaiseMax = 0.025f;
    pSkel->fHipLowerMax = 0.15f;
    vec4flt_Zero(pSkel->vHipOffset);
    vec4flt_Zero(pSkel->vHipOffsetWeighted);
    pSkel->f10C4 = 1.0f;
    pSkel->fRootLinkHeight = pChain->pLinks[0].v28[1];
    pSkel->fHipFollow = 0.025f;
    pSkel->f10D0 = 0.05f;
    pSkel->nShoulderFrames = 0;
    Quat_IdentifyForMul(pSkel->qShoulderRot);
    SKEL_SolveIKChain(pModel, pChain, pTarget, pChain->nMaxIterations, SKEL_AdjustHipHeight,
                      pChain->fTolerance);
    SKEL_SetIKSolutionWeight(pSkel, 1.0f);
    pSkel->fIKBlendLeft = 0.0f;
    return fDrop;
}

// Builds an IK chain from its setup: a link per bone, each after the one before it.
void SKEL_CreateIKChain(CharModel* pModel, IKChain* pChain, IKChainDef* pDef) {
    int i;
    s8 nPrev = -1;
    IKLink* pLink;

    pChain->nLinks = pDef->nLinks;
    pChain->nMaxIterations = pDef->nMaxIterations;
    pChain->fTolerance = pDef->fTolerance;
    pChain->pLinks = StaticMem_Alloc(pChain->nLinks * sizeof(IKLink), 2, 64, "Skeleton.c", 1126);
    for (i = 0; i < pChain->nLinks; i++) {
        pLink = &pChain->pLinks[i];
        pLink->nBone = CharModel_GetBoneIndexMapped(pModel, pDef->pLinks[i].nBone);
        pLink->nPrev = nPrev;
        nPrev = i;
        pLink->b0 = 0;
        pLink->nLockedAxis = pDef->pLinks[i].nLockedAxis;
        pLink->fTurnShare = pDef->pLinks[i].fTurnShare;
        pLink->fC = pDef->pLinks[i].fC;
        pLink->f10 = pDef->pLinks[i].f10;
    }
}

// Makes a model's IK skeleton: an IK chain per setup in pDefs, the two per-bone IK rotation sets,
// the indexes of bones 0x24, 0x25, 0x11 and 0x12 (the shoulders and upper arms) in a1108, and the
// IK at rest: weight 0, hip offset limits 0.1 up and 0.2 down, no extra shoulder rotation.
Skeleton* SKEL_CreateIKSkeleton(CharModel* pModel, CharModelDefs* pDefs) {
    int i;
    Skeleton* pSkel;
    IKChainDef* pChainDefs = pDefs->pDefs;
    s8 nChains = pDefs->nDefs;

    pSkel = StaticMem_Alloc(sizeof(Skeleton), 2, 64, "Skeleton.c", 1148);
    pSkel->nChains = nChains;
    pSkel->pChains = StaticMem_Alloc(nChains * sizeof(IKChain), 2, 64, "Skeleton.c", 1151);
    for (i = 0; i < pSkel->nChains; i++) {
        SKEL_CreateIKChain(pModel, &pSkel->pChains[i], &pChainDefs[i]);
    }
    pSkel->pDefs = pChainDefs;
    pSkel->pIKRots = StaticMem_Alloc(pModel->nBones * sizeof(f32[4]), 2, 64, "Skeleton.c", 1159);
    pSkel->pWeightedRots = StaticMem_Alloc(pModel->nBones * sizeof(f32[4]), 2, 64, "Skeleton.c", 1160);
    pSkel->pUsedRots = pSkel->pIKRots;
    pSkel->nIKClubClass = -1;
    pSkel->nIKClipKey = -1;
    BitArray_ClearArray(pSkel->aIKBones, 0x80);
    pSkel->n0 = 0;
    pSkel->fHipRaiseMax = 0.1f;
    pSkel->fHipLowerMax = 0.2f;
    vec4flt_Zero(pSkel->vHipOffset);
    vec4flt_Zero(pSkel->vHipOffsetWeighted);
    pSkel->f10C4 = 1.0f;
    pSkel->fRootLinkHeight = 0.0f;
    pSkel->fHipFollow = 1.0f;
    pSkel->pClip = NULL;
    pSkel->fIKBlendLeft = 0.0f;
    pSkel->a1108[0] = CharModel_GetBoneIndexMapped(pModel, 0x24);
    pSkel->a1108[1] = CharModel_GetBoneIndexMapped(pModel, 0x25);
    pSkel->a1108[2] = CharModel_GetBoneIndexMapped(pModel, 0x11);
    pSkel->a1108[3] = CharModel_GetBoneIndexMapped(pModel, 0x12);
    SKEL_SetIKSolutionWeight(pSkel, 0.0f);
    Quat_IdentifyForMul(pSkel->qShoulderRot);
    pSkel->nShoulderFrames = 0;
    return pSkel;
}

// Frees a skeleton: its chains' links, the chains, and both rotation sets.
void SKEL_FreeIKSkeleton(Skeleton* pSkel) {
    int i;
    for (i = 0; i < pSkel->nChains; i++) {
        StaticMem_Free(pSkel->pChains[i].pLinks);
    }
    StaticMem_Free(pSkel->pChains);
    StaticMem_Free(pSkel->pWeightedRots);
    StaticMem_Free(pSkel->pIKRots);
    StaticMem_Free(pSkel);
}

// Loads a character model from pData (read through BYTESWAP_SWAPDATA): its bone count, two floats
// (fLeftFootLen, fRightFootLen), then per bone its 8-byte name, parent and position. nExtra more
// bones are added at the root, named GBall1 (0x54); a negative nExtra asks for that many bones in
// all. Then its matrices and poses, the bone lookup and left-handed tables (bLeftHanded), its IK
// skeleton when pDefs is given, every bone posed, its dynamic chains (the tail1 hair, the chest,
// gSkelPantBones' and gSkelSleeveBones' six each) and every bone's scale at 1.
CharModel* SKEL_LoadFromMem(u8* pData, s8 nExtra, CharModelDefs* pDefs, int bLeftHanded) {
    u32 aAll[4];
    s8 nTotal;
    CharModel* pModel;
    int i;

    pModel = StaticMem_Alloc(sizeof(CharModel), 2, 64, "Skeleton.c", 1228);
    memset(pModel, 0, sizeof(CharModel));
    BYTESWAP_SWAPDATA(&pData, (u8*)&pModel->nBones, 4, 4);
    if (nExtra < 0) {
        nExtra = -nExtra - pModel->nBones;
        if (nExtra < 0) {
            nExtra = 0;
        }
    }
    nTotal = pModel->nBones + nExtra;
    pModel->pBones = StaticMem_Alloc(nTotal * sizeof(Bone), 2, 64, "Skeleton.c", 1242);
    BYTESWAP_SWAPDATA(&pData, (u8*)&pModel->fRightFootLen, 4, 4);
    BYTESWAP_SWAPDATA(&pData, (u8*)&pModel->fLeftFootLen, 4, 4);
    for (i = 0; i < pModel->nBones; i++) {
        BYTESWAP_SWAPDATA(&pData, (u8*)&pModel->pBones[i].uId, 8, -8);
        BYTESWAP_SWAPDATA(&pData, (u8*)&pModel->pBones[i].nParent, 1, 1);
        BYTESWAP_SWAPDATA(&pData, (u8*)pModel->pBones[i].v1C, 0x10, 4);
    }
    for (; i < nTotal; i++) {
        pModel->pBones[i].nParent = 0;
        pModel->pBones[i].v1C[0] = 0.0f;
        pModel->pBones[i].v1C[1] = 0.0f;
        pModel->pBones[i].v1C[2] = 0.0f;
        pModel->pBones[i].v1C[3] = 1.0f;
        pModel->pBones[i].uId = *(u64*)gSkelBoneNames[0x54];  // port: reads 8 bytes of the name
    }
    pModel->nBones = nTotal;
    pModel->pMatrices = StaticMem_Alloc(pModel->nBones * sizeof(f32[4][4]), 2, 64, "Skeleton.c", 1265);
    pModel->pPoses = StaticMem_Alloc(pModel->nBones * sizeof(BonePose), 2, 64, "Skeleton.c", 1266);
    for (i = 0; i < pModel->nBones; i++) {
        Quat_IdentifyForMul(pModel->pBones[i].q0C);
    }
    BitArray_FillArray(pModel->a14, 0x80);
    BitArray_FillArray(pModel->a24, 0x80);
    pModel->bLeftHanded = bLeftHanded;
    SKEL_GenerateBoneLookupTable(pModel);
    SKEL_GenerateLeftHandedTable(pModel);
    if (pDefs != NULL) {
        pModel->pSkel = SKEL_CreateIKSkeleton(pModel, pDefs);
    } else {
        pModel->pSkel = NULL;
    }
    BitArray_FillArray(aAll, 0x80);
    SKEL_TransformBones(pModel, aAll);
    pModel->pF0 = fn_80114270(pModel, CharModel_GetBoneIndex(pModel, 0xB), 0, 0);
    pModel->pF4 = fn_80114270(pModel, CharModel_GetBoneIndex(pModel, 0x57), 1, 0);
    pModel->pF8 = fn_80114270(pModel, CharModel_GetBoneIndex(pModel, 0x58), 1, 1);
    for (i = 0; i < 6; i++) {
        pModel->apFC[i] = fn_80114270(pModel, CharModel_GetBoneIndex(pModel, gSkelPantBones[i]), 2, i);
    }
    for (i = 0; i < 6; i++) {
        pModel->ap114[i] = fn_80114270(pModel, CharModel_GetBoneIndex(pModel, gSkelSleeveBones[i]), 3, i);
    }
    for (i = 0; i < pModel->nBones; i++) {
        pModel->a140[i][0] = 1.0f;
        pModel->a140[i][1] = 1.0f;
        pModel->a140[i][2] = 1.0f;
    }
    pModel->p760 = NULL;
    pModel->p768 = NULL;
    return pModel;
}

// Resets every bone's scale (a140) to 1 on all three axes, before the character sliders set them
// again.
void SKEL_ResetBoneScales(CharModel* pModel) {
    int i;
    for (i = 0; i < pModel->nBones; i++) {
        pModel->a140[i][0] = 1.0f;
        pModel->a140[i][1] = 1.0f;
        pModel->a140[i][2] = 1.0f;
    }
}

// Multiplies bone nBone's scale (a140) by fScale on the axes in uAxes: bit 1 x, bit 4 y, bit 2 z.
// The character sliders shape the body with it; SKEL_TransformBones applies it. A bone out of
// range: nothing.
void SKEL_ScaleBone(CharModel* pModel, int nBone, u32 uAxes, f32 fScale) {
    if (nBone < 0 || nBone >= pModel->nBones) return;
    if (uAxes & 1) {
        pModel->a140[nBone][0] *= fScale;
    }
    if (uAxes & 4) {
        pModel->a140[nBone][1] *= fScale;
    }
    if (uAxes & 2) {
        pModel->a140[nBone][2] *= fScale;
    }
}

// Rebuilds the pose and matrix of every bone in aBits and every bone below one (a rebuilt bone adds
// its bit to aBits, so its children follow), walking the bones in order with aCur holding the
// current bone's bit. A bone's pose comes from its parent's and its own local rotation and
// position, each only where the model's a14 (rotation) and a24 (position) bits allow (both are set
// again at the end); its IK rotation (pUsedRots) goes first while the IK has weight and the bone is
// in the IK set, and bone 1 (the root) is moved by the IK's hip offset (vHipOffsetWeighted). Its
// matrix takes the bone's scale (a140; the club's x negated for a left-hander), and its skinning
// matrix is updated. On the way the shoulders' half joints are set (SKEL_SetHalfJoint) and redone
// with their shoulders: at rshlddef (0x22) from q740, and at lshlddef (0x35), where EA sets 0x22
// again, from q750.
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

    BitArray_ClearArray(aCur, 0x80);
    BitArray_SetBit(aCur, 0);
    if (pModel->pSkel != NULL) {
        if (0.0f == pModel->pSkel->fIKWeight) {
            BitArray_ClearArray(aSkel, 0x80);
        } else {
            BitArray_CopyArray(pModel->pSkel->aIKBones, aSkel, 0x80);
        }
        pSkelRot = pModel->pSkel->pUsedRots;
    } else {
        BitArray_ClearArray(aSkel, 0x80);
        pSkelRot = NULL;
    }
    BitArray_MergeArrayWithOr(pModel->a14, pModel->a24, aModel, 0x80);
    LLMath_IdentifyMat(mScale);
    if (BitArray_Intersects(aBits, aCur, 0x80)) {
        Quat_Copy(pModel->pBones[0].q0C, pModel->pPoses[0].q0);
        Quat_Copy(pModel->pBones[0].v1C, pModel->pPoses[0].v10);
        Quat_QuatToMatrix(pModel->pPoses[0].q0, pModel->pMatrices[0]);
        mScale[0][0] = pModel->a140[0][0];
        mScale[1][1] = pModel->a140[0][1];
        mScale[2][2] = pModel->a140[0][2];
        LLMath_mat44fltMultiplyList(pModel->pMatrices[0], mScale, mOut, 4);
        LLMath_CopyMat44(mOut, pModel->pMatrices[0]);
        Vec4_CopyPoint(pModel->pPoses[0].v10, pModel->pMatrices[0][3]);
        SKEL_UpdateSkinningMatrix(pModel, pModel->pMatrices[0], 0);
    }
    BitArray_ShiftUp(aCur, aCur, 4, 1);

    for (i = 1; i < pModel->nBones; i++) {
        pBone = &pModel->pBones[i];
        BitArray_ClearArray(aParent, 0x80);
        BitArray_SetBit(aParent, pBone->nParent);
        if (i == CharModel_GetBoneIndex(pModel, 0x22)) {
            SKEL_SetHalfJoint(pModel, CharModel_GetBoneIndex(pModel, 0x22),
                              CharModel_GetBoneIndex(pModel, 0x11),
                        pModel->q740);
            if (BitArray_TestBit(aBits, CharModel_GetBoneIndex(pModel, 0x11))) {
                BitArray_SetBit(aBits, CharModel_GetBoneIndex(pModel, 0x22));
            }
            if (BitArray_TestBit(aModel, CharModel_GetBoneIndex(pModel, 0x11))) {
                BitArray_SetBit(aModel, CharModel_GetBoneIndex(pModel, 0x22));
            }
            BitArray_SetBit(pModel->a24, CharModel_GetBoneIndex(pModel, 0x22));
            if (BitArray_TestBit(pModel->a14, CharModel_GetBoneIndex(pModel, 0x11))) {
                BitArray_SetBit(pModel->a14, CharModel_GetBoneIndex(pModel, 0x22));
            }
        } else if (i == CharModel_GetBoneIndex(pModel, 0x35)) {
            // EA passes bone 0x22 here too
            SKEL_SetHalfJoint(pModel, CharModel_GetBoneIndex(pModel, 0x22),
                              CharModel_GetBoneIndex(pModel, 0x11),
                        pModel->q750);
            if (BitArray_TestBit(aBits, CharModel_GetBoneIndex(pModel, 0x24))) {
                BitArray_SetBit(aBits, CharModel_GetBoneIndex(pModel, 0x35));
            }
            if (BitArray_TestBit(aModel, CharModel_GetBoneIndex(pModel, 0x24))) {
                BitArray_SetBit(aModel, CharModel_GetBoneIndex(pModel, 0x35));
            }
            BitArray_SetBit(pModel->a24, CharModel_GetBoneIndex(pModel, 0x35));
            if (BitArray_TestBit(pModel->a14, CharModel_GetBoneIndex(pModel, 0x24))) {
                BitArray_SetBit(pModel->a14, CharModel_GetBoneIndex(pModel, 0x35));
            }
        }
        if (BitArray_Intersects(aBits, aCur, 0x80) || BitArray_Intersects(aBits, aParent, 0x80)) {
            pPose = &pModel->pPoses[i];
            if (BitArray_Intersects(aModel, aCur, 0x80)) {
                pParentPose = &pModel->pPoses[pBone->nParent];
                if (BitArray_Intersects(pModel->a24, aCur, 0x80)) {
                    Quat_RotateVector(pParentPose->q0, pBone->v1C, vPos);
                    Quat_Add(pParentPose->v10, vPos, pPose->v10);
                    pPose->v10[3] = 0.0f;
                    if (i == 1) {
                        pSkel = pModel->pSkel;
                        if (pSkel != NULL && 0.0f != pSkel->f10C4) {
                            SKEL_Vec3Add(pPose->v10, pSkel->vHipOffsetWeighted, pPose->v10);
                        }
                    }
                }
                if (BitArray_Intersects(pModel->a14, aCur, 0x80)) {
                    if (BitArray_Intersects(aSkel, aCur, 0x80)) {
                        Quat_Multiply(pSkelRot[i], pBone->q0C, qRot);
                        Quat_Multiply(qRot, pParentPose->q0, pPose->q0);
                    } else {
                        Quat_Multiply(pBone->q0C, pParentPose->q0, pPose->q0);
                    }
                }
            }
            Quat_QuatToMatrix(pPose->q0, pModel->pMatrices[i]);
            mScale[0][0] = pModel->a140[i][0];
            mScale[1][1] = pModel->a140[i][1];
            mScale[2][2] = pModel->a140[i][2];
            if (pModel->bLeftHanded && i == CharModel_GetBoneIndex(pModel, 0x52)) {
                mScale[0][0] = -mScale[0][0];   // with bLeftHanded set, bone 0x52's x is flipped
            }
            LLMath_mat44fltMultiplyList(pModel->pMatrices[i], mScale, mOut, 4);
            LLMath_CopyMat44(mOut, pModel->pMatrices[i]);
            Vec4_CopyPoint(pPose->v10, pModel->pMatrices[i][3]);
            SKEL_UpdateSkinningMatrix(pModel, pModel->pMatrices[i], i);
            BitArray_MergeArrayWithOr(aBits, aCur, aBits, 0x80);
        }
        BitArray_ShiftUp(aCur, aCur, 0x80, 1);
    }
    BitArray_FillArray(pModel->a14, 0x80);
    BitArray_FillArray(pModel->a24, 0x80);
}

// Copies pPose's local rotations (bones in its a0 bits) and positions (a10 bits) into the model's
// bones; with bTransform, then rebuilds those bones (SKEL_TransformBones). For a left-handed golfer
// (bLeftHanded) the pose is mirrored: each rotation goes to the bone's left/right partner (aBone2,
// whose a0 bit is set), the root's (1) and a club hanging from the root (0x52) turned half a turn
// in pitch; the root's, GBall1's (0x54) and the club's positions have z negated (the club's x
// instead when it has a parent).
void SKEL_UpdateState(CharModel* pModel, SkelPose* pPose, u8 bTransform) {
    u32 aRot[4];
    u32 aPos[4];
    u32 aAll[4];
    f32 qTurn[4];
    int i;

    BitArray_CopyArray(pPose->a0, aRot, 0x80);
    BitArray_CopyArray(pPose->a10, aPos, 0x80);
    for (i = 0; i < pModel->nBones; i++) {
        if (BitArray_TestBit(aRot, i)) {
            if (pModel->bLeftHanded) {
                if (i == CharModel_GetBoneIndex(pModel, 1)) {
                    Legacy_Quat_BuildFromPitch(PI, qTurn);
                    Quat_Multiply(pPose->aBones[i].q0, qTurn, pModel->pBones[pModel->aBone2[i]].q0C);
                    BitArray_SetBit(pPose->a0, pModel->aBone2[i]);
                } else if (i == CharModel_GetBoneIndex(pModel, 0x52)) {
                    if (pModel->pBones[i].nParent == 0) {
                        Legacy_Quat_BuildFromPitch(PI, qTurn);
                        Quat_Multiply(pPose->aBones[i].q0, qTurn, pModel->pBones[pModel->aBone2[i]].q0C);
                        BitArray_SetBit(pPose->a0, pModel->aBone2[i]);
                    } else {
                        Quat_Copy(pPose->aBones[i].q0, pModel->pBones[pModel->aBone2[i]].q0C);
                        BitArray_SetBit(pPose->a0, pModel->aBone2[i]);
                    }
                } else {
                    Quat_Copy(pPose->aBones[i].q0, pModel->pBones[pModel->aBone2[i]].q0C);
                    BitArray_SetBit(pPose->a0, pModel->aBone2[i]);
                }
            } else {
                Quat_Copy(pPose->aBones[i].q0, pModel->pBones[i].q0C);
            }
        }
        if (BitArray_TestBit(aPos, i)) {
            if (pModel->bLeftHanded
                && (i == CharModel_GetBoneIndex(pModel, 1) || i == CharModel_GetBoneIndex(pModel, 0x54))) {
                Quat_Copy(pPose->aBones[i].v10, pModel->pBones[i].v1C);
                pModel->pBones[i].v1C[2] = -pModel->pBones[i].v1C[2];
            } else if (pModel->bLeftHanded && i == CharModel_GetBoneIndex(pModel, 0x52)) {
                if (pModel->pBones[i].nParent == 0) {
                    Quat_Copy(pPose->aBones[i].v10, pModel->pBones[i].v1C);
                    pModel->pBones[i].v1C[2] = -pModel->pBones[i].v1C[2];
                } else {
                    Quat_Copy(pPose->aBones[i].v10, pModel->pBones[i].v1C);
                    pModel->pBones[i].v1C[0] = -pModel->pBones[i].v1C[0];
                }
            } else {
                Quat_Copy(pPose->aBones[i].v10, pModel->pBones[i].v1C);
            }
        }
    }
    if (bTransform) {
        BitArray_MergeArrayWithOr(pPose->a10, pPose->a0, aAll, 0x80);
        SKEL_TransformBones(pModel, aAll);
    }
}

// Blends poses pA and pB into pOut by fT (0: pA, 1: pB) for nCount bones from nBone: a rotation
// where both carry the bone's rotation (their a30 bits), a position where both carry its position
// (a20 bits); pOut's a0 and a10 bits say which bones got a rotation and a position. The animation
// blender's two-child blend.
void SKEL_BlendPoses(int nBone, int nCount, SkelPose* pA, SkelPose* pB, SkelPose* pOut, f32 fT) {
    u32 aCur[4];
    u32 aPos[4];
    u32 aRot[4];
    int i;
    int nLast;

    BitArray_MergeArrayWithAnd(pA->a20, pB->a20, aPos, 0x80);
    BitArray_MergeArrayWithAnd(pA->a30, pB->a30, aRot, 0x80);
    BitArray_ClearArray(aCur, 0x80);
    BitArray_SetBit(aCur, nBone);
    nLast = nBone + nCount - 1;
    BitArray_ClearArray(pOut->a0, 0x80);
    BitArray_ClearArray(pOut->a10, 0x80);
    for (i = nBone; i <= nLast; i++) {
        if (BitArray_Intersects(aRot, aCur, 0x80)) {
            SKA_BlendQuat(pA->aBones[i].q0, pB->aBones[i].q0, pOut->aBones[i].q0, fT);
            BitArray_MergeArrayWithOr(pOut->a0, aCur, pOut->a0, 0x80);
        }
        if (BitArray_Intersects(aPos, aCur, 0x80)) {
            SKA_BlendVec3(pA->aBones[i].v10, pB->aBones[i].v10, pOut->aBones[i].v10, fT);
            BitArray_MergeArrayWithOr(pOut->a10, aCur, pOut->a10, 0x80);
        }
        BitArray_ShiftUp(aCur, aCur, 0x80, 1);
    }
}

// Sets the module up: gSkelIdentityQuat to the identity rotation, then the dynamic chains' settings
// (DynChain.c).
void SKEL_InitModule(void) {
    Quat_IdentifyForMul(gSkelIdentityQuat);
    fn_80113E60();
}

// Shuts down the dynamic chains.
void SKEL_CloseModule(void) {
    fn_80114080();
}

// Frees a model: its bones, matrices and poses, its dynamic chains, its IK skeleton and the model.
void SKEL_Free(CharModel* pModel) {
    int i;

    StaticMem_Free(pModel->pBones);
    StaticMem_Free(pModel->pMatrices);
    StaticMem_Free(pModel->pPoses);
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
        SKEL_FreeIKSkeleton(pModel->pSkel);
    }
    StaticMem_Free(pModel);
}

// Fills in aBone, each bone id's index in the model (0xFF: none; id 0 is bone 0): each model bone
// from 1 is found by its name (the first 8 bytes of uId) in mtalib.c's bone names (gSkelBoneNames).
// The club bone (0x52) can go by other names: of the first 30 bones with no known name, the first
// named in gSkelClubBoneNames becomes it.
void SKEL_GenerateBoneLookupTable(CharModel* pModel) {
    char szName[9];
    u8 aUnknown[30];
    int nUnknown = 0;
    int i;
    int j;
    s32 nId;

    pModel->aBone[0] = 0;
    for (nId = 1; nId < 0x59; nId++) {
        pModel->aBone[nId] = 0xFF;
    }
    for (nId = 1; nId < pModel->nBones; nId++) {
        strncpy(szName, (char*)&pModel->pBones[nId].uId, 8);
        szName[8] = '\0';
        for (j = 0; j < 0x59; j++) {
            if (strcmp(szName, gSkelBoneNames[j]) == 0) {
                pModel->aBone[j] = nId;
                break;
            }
        }
        if (j == 0x59 && nUnknown < 30) {
            aUnknown[nUnknown] = nId;
            nUnknown++;
        }
    }
    if (nUnknown != 0) {
        for (i = 0; i < nUnknown; i++) {
            strncpy(szName, (char*)&pModel->pBones[aUnknown[i]].uId, 8);
            szName[8] = '\0';
            for (j = 0; j < sizeof(gSkelClubBoneNames) / sizeof(gSkelClubBoneNames[0]); j++) {
                if (strcmp(gSkelClubBoneNames[j], szName) == 0) {
                    pModel->aBone[0x52] = aUnknown[i];
                    return;
                }
            }
        }
    }
}

// Fills in aBone2, the bone a left-handed golfer's pose puts each bone's rotation on
// (SKEL_UpdateState): itself, except each bone of gSkelLeftHandedPairs (the arms, hands and legs),
// which maps to its partner when the model has both.
void SKEL_GenerateLeftHandedTable(CharModel* pModel) {
    int i;
    int nA;
    int nB;

    pModel->aBone2[0] = 0;  // fake match: bone 0 on its own (a loop from 0 unrolls differently)
    for (i = 1; i < 0x59; i++) {
        pModel->aBone2[i] = i;
    }
    for (i = 0; i < 0x29; i++) {
        nA = CharModel_GetBoneIndex(pModel, gSkelLeftHandedPairs[i][0]);
        nB = CharModel_GetBoneIndex(pModel, gSkelLeftHandedPairs[i][1]);
        if (nA != 0xFF && nB != 0xFF) {
            pModel->aBone2[nA] = nB;
        }
    }
}

// The index of the model's bone whose 8-byte name is uId, -1 for none.
int SKEL_GetBoneIDFromNameID(CharModel* pModel, u64 uId) {
    int i;
    for (i = 0; i < pModel->nBones; i++) {
        if (pModel->pBones[i].uId == uId) {
            return i;
        }
    }
    return -1;
}

// Steps one of the model's dynamic chains (hair, chest, trouser legs, sleeves) by fDelta
// (DynChain.c).
void SKEL_UpdateDynChain(CharModel* pModel, struct DynChain* pChain, f32 fDelta) {
    fn_8011443C(pModel, pChain, fDelta);
}

// Keeps the rest rotation of the right shoulder's half joint (rshlddef, 0x22) in pPose as q740 and
// q750, for SKEL_TransformBones, when the model has that bone. EA looks bone 0x22 up for both (the
// left one, lshlddef, is 0x35; SKEL_TransformBones makes the same slip).
void SKEL_InitHalfJoints(CharModel* pModel, SkelPose* pPose) {
    int nFirst = CharModel_GetBoneIndex(pModel, 0x22);
    int nSecond = CharModel_GetBoneIndex(pModel, 0x22);

    if (nFirst != 0xFF) {
        Quat_Copy(pPose->aBones[nFirst].q0, pModel->q740);
    }
    if (nSecond != 0xFF) {
        Quat_Copy(pPose->aBones[nSecond].q0, pModel->q750);
    }
}

// Sets half joint nBone's local rotation halfway between its rest rotation pRest and bone nJoint's
// (the shoulder deformer between its rest and the shoulder's turn).
void SKEL_SetHalfJoint(CharModel* pModel, int nBone, int nJoint, f32* pRest) {
    Quat_Copy(pRest, pModel->pBones[nBone].q0C);
    Quat_Slerp(pModel->pBones[nJoint].q0C, pModel->pBones[nBone].q0C, 0.5f);
}

// Hands the model its skin's bone poses (SkinModel.p34); SKEL_UpdateSkinningMatrix does nothing
// without them.
void SKEL_SetSkinBonePoses(CharModel* pModel, void* pPoses) {
    pModel->p760 = pPoses;
}

// Hands the model the skin's nMatrices skinning matrices (Skin.p108C) that
// SKEL_UpdateSkinningMatrix fills in; NULL and 0 take them back.
void SKEL_SetSkinningMatrices(CharModel* pModel, f32 (*pMatrices)[4][4], s32 nMatrices) {
    pModel->p768 = pMatrices;
    pModel->n76C = nMatrices;
}

// Hands the model the skin's default world-to-bone matrices (Skin.p1088), one per bone, that
// SKEL_UpdateSkinningMatrix puts each bone's matrix on.
void SKEL_SetDefaultWorld2BoneMatrices(CharModel* pModel, f32 (*pMatrices)[4][4]) {
    pModel->p764 = pMatrices;
}

// Updates bone nBone's skinning matrix (p768): its default world-to-bone matrix (p764) through the
// bone's matrix pMtx. Nothing for a bone past n76C or until the skin's poses and both matrix sets
// are handed over (Character_SetPreferedPos).
void SKEL_UpdateSkinningMatrix(CharModel* pModel, f32 (*pMtx)[4], int nBone) {
    if (nBone >= pModel->n76C || pModel->p760 == NULL || pModel->p768 == NULL || pModel->p764 == NULL) {
        return;
    }
    LLMath_mat44fltMultiplyList(pMtx, pModel->p764[nBone], pModel->p768[nBone], 4);
}

// Updates every bone's skinning matrix from its current matrix (SKEL_UpdateSkinningMatrix).
void SKEL_UpdateAllSkinningMatrices(CharModel* pModel) {
    int i;
    for (i = 0; i < pModel->nBones; i++) {
        SKEL_UpdateSkinningMatrix(pModel, pModel->pMatrices[i], i);
    }
}

// Square root of a float: MSL's inline sqrtf (math.h), emitted here out of line: three Newton steps
// from the reciprocal-root estimate; x itself when x <= 0.
f32 Math_Sqrtf(f32 x) {
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

// Sets a four-float vector to zero.
void vec4flt_Zero(f32* pVec) {
    LLMath_CopyVec(lbl_80186838, pVec);
}

// pA + pB into out, three floats (paired singles).
#ifdef __MWERKS__
asm void SKEL_Vec3Add(register f32* pA, register f32* pB, register f32* pOut) {
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
void SKEL_Vec3Add(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
}
#endif

// pA - pB into out, three floats (paired singles).
#ifdef __MWERKS__
asm void SKEL_Vec3Sub(register f32* pA, register f32* pB, register f32* pOut) {
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
void SKEL_Vec3Sub(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// pA - pB into out, four floats (paired singles).
#ifdef __MWERKS__
asm void SKEL_VecSub(register f32* pA, register f32* pB, register f32* pOut) {
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
void SKEL_VecSub(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
    pOut[3] = pA[3] - pB[3];
}
#endif

// Shifts a bit array of nBits bits (whole words) from aSrc into aDst up by nShift places (bit i to
// bit i + nShift): each word's top nShift bits carry into the bottom of the next word, and the last
// word's are lost. nShift must be 1..31 (the callers pass 1).
void BitArray_ShiftUp(u32* aSrc, u32* aDst, u32 nBits, u32 nShift) {
    u32 uWord;
    u32 i;
    u32 j;
    u32 uCarry = 0;
    u32 uMask = 0;

    for (i = 0; i < nShift; i++) {
        uMask |= 1 << (31 - i);
    }
    for (j = 0; j < (nBits + 31) >> 5; j++) {
        uWord = aSrc[j];
        aDst[j] = uWord << nShift;
        aDst[j] |= uCarry >> (32 - nShift);
        uCarry = uMask & uWord;
    }
}

// Copies a bit array of nBits bits (whole words) from pSrc to pDst, when both are given.
void BitArray_CopyArray(u32* pSrc, u32* pDst, u32 nBits) {
    u32 nWords;
    u32 i;

    if (pDst == NULL || pSrc == NULL) return;
    nWords = (nBits + 31) >> 5;
    for (i = 0; i < nWords; i++) {
        pDst[i] = pSrc[i];
    }
}
