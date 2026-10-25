// SkinUtil.c: a skin's rest pose and its default world-to-bone matrices, the morph weights a
// golfer's animation hands his body skin, and freeing a skin with everything it holds. Also
// defines the screen copy (gScreenCopy).

#include "game_types.h"
#include "engine.h"
#include "charstate.h"

// The pose block morph that drives the skin's morph target 0: morphs from here on drive targets 0,
// 1, 2... (SKN_ApplyPoseMorphWeights).
#define SKELPOSE_FIRST_SKIN_MORPH  5

ScreenCopy gScreenCopy;
ScreenCopy* gpScreenCopy = &gScreenCopy;

// Once per skin model (SKINMODEL_DEFAULT_POSE): puts its bone poses, each relative to its parent,
// into model space, from bone 1 on, then stores each bone's inverse matrix as its default
// world-to-bone matrix (Skin.pWorld2Bone). A bone's parent is the one pSkeleton's bone of the same
// index has, or, from bone skelOffsetStart on, its bone skelOffset further along (the club skins:
// their bones from SKEL_BONE_IGDRIVER on are that far along in the character's skeleton).
void SKN_InitDefaultPose(Skin* pSkin, CharModel* pSkeleton, int skelOffset, int skelOffsetStart) {
    f32 qRot[4];
    f32 vTurned[4];
    f32 mBone[4][4];
    BonePose* pParent;
    int nSkelBone;
    int i;

    if (pSkin->pModel != NULL && !(pSkin->pModel->uFlags & SKINMODEL_DEFAULT_POSE)) {
        pSkin->pModel->uFlags |= SKINMODEL_DEFAULT_POSE;
        for (i = 1; i < pSkin->pModel->nBones; i++) {
            nSkelBone = i;
            if (i >= skelOffsetStart) {
                nSkelBone = i + skelOffset;
            }
            pParent = &pSkin->pModel->pPoses[pSkeleton->pBones[nSkelBone].nParent];
            Quat_RotateVector(pParent->qRot, pSkin->pModel->pPoses[i].vPos, vTurned);
            Quat_Add(pParent->vPos, vTurned, pSkin->pModel->pPoses[i].vPos);
            pSkin->pModel->pPoses[i].vPos[3] = 0.0f;
            Quat_Multiply(pSkin->pModel->pPoses[i].qRot, pParent->qRot, qRot);
            Quat_Copy(qRot, pSkin->pModel->pPoses[i].qRot);
        }
        for (i = 0; i < pSkin->pModel->nBones; i++) {
            Quat_QuatToMatrix(pSkin->pModel->pPoses[i].qRot, mBone);
            Vec4_CopyPoint(pSkin->pModel->pPoses[i].vPos, mBone[3]);
            LLMath_InvertNormalized(mBone, pSkin->pWorld2Bone[i]);
        }
    }
}

// Hands the skin the morph weights set in the first block of a format 1 pose buffer: each set morph
// from SKELPOSE_FIRST_SKIN_MORPH on becomes the weight of the skin's morph target it drives. Then
// clears all the block's bits. Nothing without a skin.
void SKN_ApplyPoseMorphWeights(Skin* pSkin, SkelPose* pPose) {
    SkelPoseBlock* pBlock;
    int i;

    if (pSkin != NULL) {
        pBlock = &((SkelPose1*)pPose)->aBlocks[0];
        for (i = SKELPOSE_FIRST_SKIN_MORPH; i < SKELPOSE_NUM_MORPHS; i++) {
            if (BitArray_TestBit(pBlock->aBits, i)) {
                SkinMorph_SetTargetWeight(pSkin, i - SKELPOSE_FIRST_SKIN_MORPH, pBlock->afMorphWeights[i]);
            }
        }
        BitArray_ClearArray(pBlock->aBits, SKELPOSE_NUM_MORPHS);
    }
}

// Frees a skin: its render data, morph state and part choices, then its model's description (its
// materials first), the model, the default world-to-bone matrices and the skin itself.
void SKN_Free(Skin* pSKN) {
    SkinModel* pModel;

    SKN_FreeRenderData(pSKN);
    SkinMorph_Destroy(pSKN);
    SkinPart_FreeChoices(pSKN);
    pModel = pSKN->pModel;
    if (pModel != NULL) {
        if (pModel->pDesc != NULL) {
            HwsMaterial_Destroy(pModel->pDesc);
            Circular_MemFreeFunc(pSKN->pModel->pDesc);
        }
        Circular_MemFreeFunc(pSKN->pModel);
    }
    if (pSKN->pWorld2Bone != NULL) {
        Circular_MemFreeFunc(pSKN->pWorld2Bone);
    }
    Circular_MemFreeFunc(pSKN);
}

// Frees the bit data of each of the description's meshes that owns it (SKINMESH_OWN_BITS).
void SKN_FreeDescMeshBits(SkinDesc* pDesc) {
    int i;

    for (i = 0; i < pDesc->nMeshes; i++) {
        if (pDesc->pMeshes[i].pBits != NULL && (pDesc->pMeshes[i].uFlags & SKINMESH_OWN_BITS)) {
            Circular_MemFreeFunc(pDesc->pMeshes[i].pBits);
        }
    }
}
