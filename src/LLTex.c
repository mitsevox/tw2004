// LLTex.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): texture banks, made
// from the 'txf ' objects of the stream files. Not yet decompiled; the code below is the matched
// small functions.

#include "engine.h"

// Builds a bank from a 'txf ' object's data, into pBank or, when it is NULL, a new allocation.
TexBank* TX_spParseTextureGroupFromStream(u8* pData, TexBank* pBank, int n);
void GXSetTexCoordGen2(int nDst, int nFunc, int nSrc, int nMtx, u8 bNormalize, int nPostMtx);

void fn_8000F0E8(void) {
}

TexBank* fn_8000FB88(UStreamObject* pObject, TexBank* pBank, int n) {
    return TX_spParseTextureGroupFromStream(pObject->pData, pBank, n);
}

void fn_8000FBAC(void) {
}

// Frees a bank's pixel and palette data, unless they are not its own.
void fn_8000FFAC(TexBank* pBank) {
    if (pBank->p18 != NULL && pBank->b2D != 1) {
        fn_80009E70(pBank->p18);
        pBank->p18 = NULL;
    }
    if (pBank->p20 != NULL && pBank->p20 != pBank->p18 && pBank->b2D != 1) {
        fn_80009E70(pBank->p20);
        pBank->p20 = NULL;
    }
}

void fn_80010028(TexBank* pBank) {
    fn_8000FFAC(pBank);
    fn_80009E70(pBank);
}

// The index of the bank's texture with this name hash, or 0x80000000.
int fn_8001005C(TexBank* pBank, u64 uHash) {
    u32 i;

    for (i = 0; i < pBank->nNumTex; i++) {
        if (uHash == pBank->p8[i].uHash) return i;
    }
    return 0x80000000;
}

void fn_800100B0(TexBank* pBank, TexEntry* p8, TexPalette* pC, void* p10, void* p14, s16 nNumTex,
                 s16 nNumPalettes) {
    fn_80005AE8(pBank, 0, sizeof(TexBank));
    pBank->p8 = p8;
    pBank->pC = pC;
    pBank->p10 = p10;
    pBank->p14 = p14;
    pBank->nNumTex = nNumTex;
    pBank->nNumPalettes = nNumPalettes;
}

void fn_80010114(int nDst, int nFunc, int nSrc, int nMtx) {
    GXSetTexCoordGen2(nDst, nFunc, nSrc, nMtx, 0, 125);   // no normalizing, GX_PTIDENTITY
}
