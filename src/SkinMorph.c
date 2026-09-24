// SkinMorph.c (EA's name, from its asserts): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"
#include "charstate.h"

// ---- sweep code (not yet cleaned up) ----

void fn_8011C46C(void* arg0);
void fn_8011C1FC();
void fn_8011C4D4(s32 p0, s32 p1);
void fn_8011C580(s32 p0, s32 p1);
s32 fn_8011C58C(s32 p0, s32 p1, s32 p2);
void fn_8011C59C(s32 p0, s32 p1);
void fn_8011C5A8(s32 p0, s32 p1);
void fn_8011C84C(void);

void fn_8011C46C(void* arg0) {
    s32 temp_r4;

    temp_r4 = (*(s32*)((u8*)(arg0) + 0x10020));
    (*(s32*)((u8*)(arg0) + 0x10020)) = (s32) (*(s32*)((u8*)(arg0) + 0x10024));
    (*(s32*)((u8*)(arg0) + 0x10024)) = temp_r4;
}

void fn_8011C4D4(s32 p0, s32 p1) {
    fn_8011C1FC(p0, p1, (p1 + (*(u16*)(((u8*)(p0 + 0x10000)) + 0x18) << 3)), *(u16*)(((u8*)(p0 + 0x10000)) + 0x18));
}

void fn_8011C580(s32 p0, s32 p1) {
    *(s32*)(((u8*)(p0 + 0x10000)) + 0x228) = p1;
}

s32 fn_8011C58C(s32 p0, s32 p1, s32 p2) {
    *(s32*)(((u8*)(p0 + 0x10000)) + 0x230) = p1;
    *(s32*)(((u8*)(p0 + 0x10000)) + 0x22C) = p2;
    return (p0 + 0x10000);
}

void fn_8011C59C(s32 p0, s32 p1) {
    *(s32*)(((u8*)(p0 + 0x10000)) + 0x238) = p1;
}

void fn_8011C5A8(s32 p0, s32 p1) {
    *(s32*)(((u8*)(p0 + 0x10000)) + 0x234) = p1;
}

void fn_8011C84C(void) {
}

// ---- end of sweep code ----

// The morph targets a skin description needs: the highest n14 + n18 of its p44 entries that have
// morph targets.
s32 fn_8011C850(SkinDesc* pDesc) {
    SkinDesc44* pEntry;
    s32 nMax;
    s32 i;

    nMax = 0;
    for (i = 0; i < pDesc->n40; i++) {
        pEntry = &pDesc->p44[i];
        if ((pEntry->u24 & 2) && pEntry->n8 > 0 && pEntry->n18 + pEntry->n14 > nMax) {
            nMax = pEntry->n18 + pEntry->n14;
        }
    }
    return nMax;
}

// Sets the weight of morph target nMorph and marks it changed.
void fn_8011CADC(Skin* pSkin, int nMorph, f32 fWeight) {
    SkinMorphState* pMorph = pSkin->pMorph;

    if (pMorph == NULL || nMorph < 0 || nMorph >= pMorph->nMorphs) {
        return;
    }
    if (fWeight != pMorph->afWeights[nMorph]) {
        pMorph->afWeights[nMorph] = fWeight;
        fn_8001EA34(pMorph->p8, nMorph);
        fn_8001EA34(pMorph->pC, nMorph);
    }
}

// Frees the skin's morph state (Skin.c calls it).
void fn_8011CD84(Skin* pSkin) {
    if (pSkin->pMorph != NULL) {
        fn_80009E70(pSkin->pMorph->afWeights);
        fn_80009E70(pSkin->pMorph->p8);
        fn_80009E70(pSkin->pMorph->pC);
        fn_80009E70(pSkin->pMorph);
        pSkin->pMorph = NULL;
    }
}

// The bytes of the skin's meshes that have both flags 0x100000 and 0x10.
s32 fn_8011CDE8(Skin* pSkin) {
    SkinDesc* pDesc;
    SkinMesh* pMesh;
    s32 nSize;
    s32 i;

    if (pSkin == NULL || (pDesc = pSkin->pModel->pDesc) == NULL) {
        return 0;
    }
    nSize = 0;
    for (i = 0; i < pDesc->n30; i++) {
        pMesh = &pDesc->p34[i];
        if ((pMesh->uFlags & 0x100010) == 0x100010) {
            nSize += pMesh->nSize;
        }
    }
    return nSize;
}

// Marks every morph target changed.
void fn_8011CE58(Skin* pSkin) {
    if (pSkin == NULL || pSkin->pMorph == NULL) {
        return;
    }
    fn_8001E8A4(pSkin->pMorph->p8, pSkin->pMorph->nMorphs);
    fn_8001E8A4(pSkin->pMorph->pC, pSkin->pMorph->nMorphs);
}
