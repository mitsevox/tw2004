// hwsMaterial_Gc.c (EA's name, from its asserts): a skin's materials, one SkinDesc18 per SkinDesc14
// entry, each with its texture looked up by name.

#include "game_types.h"
#include "engine.h"
#include "charstate.h"

int  fn_8001005C(TexBank* pBank, u64 uHash);       // LLTex.c: the texture's index, or 0x80000000
TexEntry* fn_800107E4(TexBank* pBank, int nTex);  // LLTexGrp.c

// Set up a material from its entry: its texture (none with flag 1), found by the entry's name in
// pBank or (pBank NULL) in every bank, and its texture scale and offset.
void fn_80112614(SkinDesc14* pEntry, SkinDesc18* pMaterial, TexBank* pBank) {
    TexEntry* pTex;
    int nTex;

    pTex = NULL;
    if (!(pEntry->u08 & 1)) {
        if (pBank == NULL) {
            fn_800102DC(pEntry->uId, &pBank, &pTex);
        } else {
            nTex = fn_8001005C(pBank, pEntry->uId);
            if (nTex != 0x80000000) {
                pTex = fn_800107E4(pBank, nTex);
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
    pMaterial->uvC = pEntry->a20;
}

// Make the skin's materials (allocated on first use), textures from any bank. nUnused: the one
// caller, fn_801127A0, passes 0; it is not read.
void fn_801126F0(SkinDesc* pDesc, s32 nUnused) {
    int i;

    if (pDesc->p18 == NULL) {
        pDesc->p18 = fn_80009B34(pDesc->n10 * sizeof(SkinDesc18), 2, 16, "hwsMaterial_Gc.c", 76);
    }
    for (i = 0; i < pDesc->n10; i++) {
        fn_80112614(&pDesc->p14[i], &pDesc->p18[i], NULL);
    }
}

void fn_801127A0(SkinDesc* pDesc) {
    fn_801126F0(pDesc, 0);
}

// Free the skin's materials.
void fn_801127C4(SkinDesc* pDesc) {
    if (pDesc->p18 != NULL) {
        fn_80009E70(pDesc->p18);
        pDesc->p18 = NULL;
    }
}
