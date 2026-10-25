// hwsMaterial_Gc.c: the GameCube skin renderer's materials, one per material entry of a skin's
// description (SkinDesc.pMaterials from pMaterialEntries), each with the texture its entry names
// and the entry's texture scale and offset.

#include "game_types.h"
#include "engine.h"
#include "charstate.h"

static void HwsMaterial_SetupMaterials(SkinDesc* pDesc, s32 nUnused);

// Sets up a material from its entry: the texture the entry names, looked up in pBank or, when pBank
// is NULL, in the loaded texture banks (TG_iFindTextureGroupAndTexture), and the entry's texture
// scale and offset. An untextured entry, or a name not found, leaves the material with no texture
// and no bank.
void HwsMaterial_SetupMaterial(SkinDesc14* pEntry, SkinDesc18* pMaterial, TexBank* pBank) {
    TexEntry* pTex;
    int nTex;

    pTex = NULL;
    if (!(pEntry->uFlags & SKINDESC14_UNTEXTURED)) {
        if (pBank == NULL) {
            TG_iFindTextureGroupAndTexture(pEntry->uId, &pBank, &pTex);
        } else {
            nTex = TX_uiFindTextureIndexFromID(pBank, pEntry->uId);
            if (nTex != TX_NO_TEXTURE_INDEX) {
                pTex = TX_spGetTextureGroupTextureFromIndex(pBank, nTex);
            }
        }
    }
    if (pTex != NULL) {
        pMaterial->pTex = pTex;
        pMaterial->pBank = pBank;
    } else {
        pMaterial->pTex = NULL;
        pMaterial->pBank = NULL;
    }
    pMaterial->uvScaleOffset = pEntry->uvScaleOffset;
}

// Sets up every material of the description, allocating them when it has none, with textures from
// the loaded texture banks. nUnused is not read (HwsMaterial_Create passes 0).
static void HwsMaterial_SetupMaterials(SkinDesc* pDesc, s32 nUnused) {
    int i;

    if (pDesc->pMaterials == NULL) {
        pDesc->pMaterials = Circular_MemAllocFunc(pDesc->nMaterials * sizeof(SkinDesc18), MEMTYPE_LOW,
                                                  16, "hwsMaterial_Gc.c", 76);
    }
    for (i = 0; i < pDesc->nMaterials; i++) {
        HwsMaterial_SetupMaterial(&pDesc->pMaterialEntries[i], &pDesc->pMaterials[i], NULL);
    }
}

// Makes a skin description's materials (SKN_Create calls it for a new skin that has a description).
void HwsMaterial_Create(SkinDesc* pDesc) {
    HwsMaterial_SetupMaterials(pDesc, 0);
}

// Frees a skin description's materials (called as a skin is freed, just before its description).
void HwsMaterial_Destroy(SkinDesc* pDesc) {
    if (pDesc->pMaterials != NULL) {
        Circular_MemFreeFunc(pDesc->pMaterials);
        pDesc->pMaterials = NULL;
    }
}
