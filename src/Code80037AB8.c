// Code80037AB8.c (our name): split off Skin.c at 0x80037AB8. Its own .sdata2 block (0x80283020, a
// second 0.0f after Skin.c's 0.0f at 0x80283018) shows a separate file: one file pools a value once.

#include "game_types.h"
#include "engine.h"
#include "charstate.h"

void  fn_801127C4(void* pDesc);                // hwsMaterial_Gc.c
void  Quat_Add(f32* pA, f32* pB, f32* pOut);                // Quaternion.c
void  Quat_RotateVector(f32* pQuat, f32* pIn, f32* pOut);            // Quaternion.c: pIn turned by pQuat
void  Quat_QuatToMatrix(f32* pQ, f32 (*pMtx)[4]);                   // Quaternion.c: to a matrix
void  LLMath_InvertNormalized(f32 (*pSrc)[4], f32 (*pDst)[4]);            // UMemPool.c: inverts a matrix

// section note: the .bss between Skin.c's and GoPostFx.c's; only lbl_80281100 (the .sdata right
// after Skin.c's) points at it. Skin.c's tail fits the addresses equally well.
ScreenCopy lbl_801D4F68;

// This file's .sdata (engine.h).
ScreenCopy* lbl_80281100 = &lbl_801D4F68;

// Once per skin model (flag 0x8000): turns its bone poses from relative to their parent (the
// character model's bone parents, from bone nFirst on nSkip further along) into model space, then
// stores each bone's inverse matrix in p1088.
void fn_80037AB8(Skin* pSkin, CharModel* pCharModel, int nSkip, int nFirst) {
    f32 aQuat[4];
    f32 aTurned[4];
    f32 aMtx[4][4];
    BonePose* pParent;
    int nBone;
    int i;

    if (pSkin->pModel != NULL && !(pSkin->pModel->u30 & 0x8000)) {
        pSkin->pModel->u30 |= 0x8000;
        for (i = 1; i < pSkin->pModel->n14; i++) {
            nBone = i;
            if (i >= nFirst) {
                nBone = i + nSkip;
            }
            pParent = &pSkin->pModel->p34[pCharModel->pBones[nBone].nParent];
            Quat_RotateVector(pParent->q0, pSkin->pModel->p34[i].v10, aTurned);
            Quat_Add(pParent->v10, aTurned, pSkin->pModel->p34[i].v10);
            pSkin->pModel->p34[i].v10[3] = 0.0f;
            Quat_Multiply(pSkin->pModel->p34[i].q0, pParent->q0, aQuat);
            Quat_Copy(aQuat, pSkin->pModel->p34[i].q0);
        }
        for (i = 0; i < pSkin->pModel->n14; i++) {
            Quat_QuatToMatrix(pSkin->pModel->p34[i].q0, aMtx);
            Vec4_CopyPoint(pSkin->pModel->p34[i].v10, aMtx[3]);
            LLMath_InvertNormalized(aMtx, pSkin->p1088[i]);
        }
    }
}

// Hands the skin the morph weights a format 1 pose buffer changed (bits 5..19 of its first block),
// then clears the block's bits.
void fn_80037C48(Skin* pSkin, SkelPose* pPose) {
    SkelPoseBlock* pBlock;
    int i;

    if (pSkin != NULL) {
        pBlock = &((SkelPose1*)pPose)->aBlocks[0];
        for (i = 5; i < 20; i++) {
            if (BitArray_TestBit(pBlock->aBits, i)) {
                fn_8011CADC(pSkin, i - 5, pBlock->af8[i]);
            }
        }
        BitArray_ClearArray(pBlock->aBits, 20);
    }
}

// ---- sweep code (tidied) ----

void fn_80037CD8(Skin* pSkin) {
    SkinModel* pModel;

    SKN_FreeRenderData(pSkin);
    fn_8011CD84(pSkin);
    SkinPart_FreeChoices(pSkin);
    pModel = pSkin->pModel;
    if (pModel != NULL) {
        if (pModel->pDesc != NULL) {
            fn_801127C4(pModel->pDesc);
            StaticMem_Free(pSkin->pModel->pDesc);
        }
        StaticMem_Free(pSkin->pModel);
    }
    if (pSkin->p1088 != NULL) {
        StaticMem_Free(pSkin->p1088);
    }
    StaticMem_Free(pSkin);
}

// ---- end of sweep code ----

// Frees the mesh bit data a description allocated for itself (flag 0x400000).
void fn_80037D5C(SkinDesc* pDesc) {
    int i;

    for (i = 0; i < pDesc->n2C; i++) {
        if (pDesc->p34[i].pBits != NULL && (pDesc->p34[i].uFlags & 0x400000)) {
            StaticMem_Free(pDesc->p34[i].pBits);
        }
    }
}
