// SkinMorph.c (EA's name, from its asserts): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"
#include "platform.h"
#include "engine.h"
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

// Whether a morph target of p44 entry n that changed for view nView has a mesh to draw.
u8 fn_8011C8B0(Skin* pSkin, int nView, int n) {
    SkinDesc* pDesc;
    SkinDesc44* pEntry;
    SkinMorphState* pMorph;
    s32 i;
    s32 nFirst;
    s32 nCount;
    SkinDesc44* pTarget;
    s32 j;
    s32 nMesh;

    pMorph = pSkin->pMorph;
    pDesc = pSkin->pModel->pDesc;
    pEntry = &pDesc->p44[n];
    if (!(pEntry->u24 & 2)) {
        return 0;
    }
    nFirst = pEntry->n18;
    nCount = pEntry->n14;
    for (i = 0; i < nCount; i++) {
        if (fn_8001E9CC(pMorph->aChanged[nView], nFirst + i)) {
            pTarget = &pDesc->p44[pEntry->n10 + i];
            for (j = 0; j < pTarget->n8; j++) {
                nMesh = pDesc->p3C[pTarget->n0 + j];
                if (nMesh >= 0 && pDesc->p34[nMesh].n8 > 0) {
                    break;
                }
            }
            if (j != pTarget->n8) {
                return 1;
            }
        }
    }
    return 0;
}

// Makes the skin's morph state: every weight 0, nothing changed.
void fn_8011C9B0(Skin* pSkin) {
    SkinMorphState* pMorph;
    s32 nMorphs;
    s32 nBytes;

    if (pSkin->pModel->pDesc == NULL) {
        return;
    }
    nMorphs = fn_8011C850(pSkin->pModel->pDesc);
    if (nMorphs == 0) {
        return;
    }
    pMorph = fn_80009B34(sizeof(SkinMorphState), 2, 16, "SkinMorph.c", 91);
    memset(pMorph, 0, sizeof(SkinMorphState));
    pMorph->nMorphs = nMorphs;
    nBytes = (nMorphs + 31) / 32 * sizeof(u32);
    pMorph->aChanged[0] = fn_80009B34(nBytes, 2, 16, "SkinMorph.c", 95);
    pMorph->aChanged[1] = fn_80009B34(nBytes, 2, 16, "SkinMorph.c", 96);
    fn_8001E938(pMorph->aChanged[0], nMorphs);
    fn_8001E938(pMorph->aChanged[1], nMorphs);
    nBytes = nMorphs * sizeof(f32);
    pMorph->afWeights = fn_80009B34(nBytes, 2, 16, "SkinMorph.c", 100);
    memset(pMorph->afWeights, 0, nBytes);
    pSkin->pMorph = pMorph;
}

// Sets the weight of morph target nMorph and marks it changed.
void fn_8011CADC(Skin* pSkin, int nMorph, f32 fWeight) {
    SkinMorphState* pMorph = pSkin->pMorph;

    if (pMorph == NULL || nMorph < 0 || nMorph >= pMorph->nMorphs) {
        return;
    }
    if (fWeight != pMorph->afWeights[nMorph]) {
        pMorph->afWeights[nMorph] = fWeight;
        fn_8001EA34(pMorph->aChanged[0], nMorph);
        fn_8001EA34(pMorph->aChanged[1], nMorph);
    }
}

// Frees the skin's morph state (Skin.c calls it).
void fn_8011CD84(Skin* pSkin) {
    if (pSkin->pMorph != NULL) {
        fn_80009E70(pSkin->pMorph->afWeights);
        fn_80009E70(pSkin->pMorph->aChanged[0]);
        fn_80009E70(pSkin->pMorph->aChanged[1]);
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
    fn_8001E8A4(pSkin->pMorph->aChanged[0], pSkin->pMorph->nMorphs);
    fn_8001E8A4(pSkin->pMorph->aChanged[1], pSkin->pMorph->nMorphs);
}
