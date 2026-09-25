// Code80037AB8.c (our name): split off Skin.c at 0x80037AB8. Its own .sdata2 block (0x80283020, a
// second 0.0f after Skin.c's 0.0f at 0x80283018) shows a separate file: one file pools a value once.

#include "game_types.h"
#include "engine.h"
#include "charstate.h"

void  fn_801127C4(void* pDesc);                // hwsMaterial_Gc.c
void  Quat_Add(f32* pA, f32* pB, f32* pOut);                // Quaternion.c
void  Quat_RotateVector(f32* pQuat, f32* pIn, f32* pOut);            // Quaternion.c: pIn turned by pQuat
void  Quat_QuatToMatrix(f32* pQ, f32 (*pMtx)[4]);                   // Quaternion.c: to a matrix
void  fn_8000A798(f32 (*pSrc)[4], f32 (*pDst)[4]);            // UMemPool.c: inverts a matrix

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
            fn_8001E85C(aQuat, pSkin->pModel->p34[i].q0);
        }
        for (i = 0; i < pSkin->pModel->n14; i++) {
            Quat_QuatToMatrix(pSkin->pModel->p34[i].q0, aMtx);
            fn_8001E880(pSkin->pModel->p34[i].v10, aMtx[3]);
            fn_8000A798(aMtx, pSkin->p1088[i]);
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
            if (fn_8001E9CC(pBlock->aBits, i)) {
                fn_8011CADC(pSkin, i - 5, pBlock->af8[i]);
            }
        }
        fn_8001E938(pBlock->aBits, 20);
    }
}

// ---- sweep code (tidied) ----

void fn_80037CD8(Skin* pSkin) {
    SkinModel* pModel;

    fn_80037708(pSkin);
    fn_8011CD84(pSkin);
    fn_800CD56C(pSkin);
    pModel = pSkin->pModel;
    if (pModel != NULL) {
        if (pModel->pDesc != NULL) {
            fn_801127C4(pModel->pDesc);
            fn_80009E70(pSkin->pModel->pDesc);
        }
        fn_80009E70(pSkin->pModel);
    }
    if (pSkin->p1088 != NULL) {
        fn_80009E70(pSkin->p1088);
    }
    fn_80009E70(pSkin);
}

// ---- end of sweep code ----

// Frees the mesh bit data a description allocated for itself (flag 0x400000).
void fn_80037D5C(SkinDesc* pDesc) {
    int i;

    for (i = 0; i < pDesc->n2C; i++) {
        if (pDesc->p34[i].pBits != NULL && (pDesc->p34[i].uFlags & 0x400000)) {
            fn_80009E70(pDesc->p34[i].pBits);
        }
    }
}
