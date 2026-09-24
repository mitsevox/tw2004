// LLTex.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): texture banks, made
// from the 'txf ' objects of the stream files. Not yet decompiled; the code below is the matched
// small functions.

#include "engine.h"
#include "gx.h"
#include "dynobj.h"

u8   fn_8002A3A4(void);                 // GxUtil.c
void fn_8002A3AC(u8 b);                 // GxUtil.c
void fn_80010114(int nDst, int nFunc, int nSrc, int nMtx);

// Builds a bank from a 'txf ' object's data, into pBank or, when it is NULL, a new allocation.
TexBank* TX_spParseTextureGroupFromStream(u8* pData, TexBank* pBank, int n);

// A DynObj is going: every item whose def has its id counts one fewer (n1A).
void fn_8000E830(DynObj* pObj) {
    LLTexItem** ppItem;
    LLTexItemState* pState;
    int n;
    int nId;

    if (lbl_80281C60 != NULL) {
        ppItem = (LLTexItem**)(lbl_80281C60 + 1);
        nId = pObj->n140;
        for (n = lbl_80281C60->nItems; n > 0; n--) {
            pState = (*ppItem++)->p4;
            if (pState->p2C->n0 == nId) {
                pState->n1A--;
            }
        }
    }
}

// Message 0x46 turns n16 bit 0 on (bOn 0) or off, clearing u10; message 0x47 counts one of n16's
// two 4-bit counters up (bits 8-11 when bOn, else 12-15) while n1A is below n14 and n18 is set.
void fn_8000E884(LLTexItem* pItem, int nMsg, int bOn) {
    LLTexItemState* pState = pItem->p4;
    s16 n;

    switch (nMsg) {
    case 0x46:
        if (bOn) {
            if (pState->n16 & 1) {
                pState->n16 &= ~1;
                pState->u10 = 0;
            }
        } else {
            pState->n16 |= 1;
        }
        break;
    case 0x47:
        if (pState->n1A < pState->n14 && pState->n18 != 0) {
            if (bOn) {
                n = pState->n16;
                pState->n16 = n & 0xF0FF;
                pState->n16 |= (((n >> 8) & 0xF) + 1) << 8;
            } else {
                n = pState->n16;
                pState->n16 = n & 0xFFF;
                pState->n16 |= (((n >> 12) & 0xF) + 1) << 12;
            }
        }
        break;
    }
}

// Sends a message to the first item whose def has this id.
void fn_8000E948(int nId, int nMsg, int bOn) {
    LLTexItem** ppItem = (LLTexItem**)(lbl_80281C60 + 1);
    LLTexItem* pItem;
    int n;

    for (n = lbl_80281C60->nItems; n > 0; n--) {
        pItem = *ppItem++;
        if (pItem->p4->p2C->n0 == nId) {
            fn_8000E884(pItem, nMsg, bOn);
            return;
        }
    }
}

// Sends a message to every item whose def's n18 is this id.
void fn_8000E9A8(int nId, int nMsg, int bOn) {
    LLTexItem** ppItem = (LLTexItem**)(lbl_80281C60 + 1);
    LLTexItem* pItem;
    int n;

    for (n = lbl_80281C60->nItems; n > 0; n--) {
        pItem = *ppItem++;
        if (pItem->p4->p2C->n18 == nId) {
            fn_8000E884(pItem, nMsg, bOn);
        }
    }
}

// Resets the constant-alpha flag and texture coordinates 0-2 for a frame: each stage's constant
// alpha selector 0x1C and each coordinate from texture coordinate 0 through matrix 0x3C.
void fn_8000F060(void) {
    *lbl_80280DC8 = 0;
    GXSetTevKAlphaSel(0, 0x1C);
    GXSetTevKAlphaSel(1, 0x1C);
    GXSetTevKAlphaSel(2, 0x1C);
    fn_80010114(0, 1, 4, 0x3C);
    fn_80010114(1, 1, 4, 0x3C);
    fn_80010114(2, 1, 4, 0x3C);
}

void fn_8000F0E8(void) {
}

// Set up the texture stages for one of a bank's textures, after the stage fn_8002A3AC sets up when
// fn_8002A3A4 says so. A texture with a second GX object (b47 bit 0) gets two stages; a palettized
// one (b40 9) loads its palette. With no bank, one untextured stage draws the vertex colour.
void fn_8000F0EC(TexBank* pBank, TexEntry* pTex) {
    // The second texture's map and stage, or a palettized texture's. fake match: s32, so the copy
    // of the int nStage keeps its own register.
    s32 nMap;
    GXTexObj* pObj;
    int nStage;

    nStage = 0;
    if (fn_8002A3A4()) {
        nStage = 1;
    }
    if (pBank != NULL) {
        pObj = (GXTexObj*)&pBank->p10[pTex->n3E];
        if (pTex->b47 & 1) {
            GXSetNumTexGens(nStage + 1);
            GXSetTevOrder(nStage, nStage, nStage, 4);
            nMap = nStage + 1;
            GXSetTevOrder(nMap, nStage, nMap, 4);
            if (pTex->nPalette != -1) {
                GXLoadTlut((GXTlutObj*)&pBank->p14[pTex->n3E], nStage);
            }
            GXLoadTexObj(pObj, nStage);
            GXLoadTexObj((GXTexObj*)((TexGXObj*)pObj + 1), nMap);
            GXSetNumTevStages(nStage + 2);
            if (*lbl_80280DC8) {
                if (fn_8002A3A4()) {
                    fn_8002A3AC(*lbl_80280DC8);
                    GXSetTevColorIn(nStage, 0xF, 8, 0xA, 0xF);
                    GXSetTevColorOp(nStage, 0, 0, 1, 1, 0);
                    GXSetTevAlphaIn(nStage, 7, 0, 6, 7);
                    GXSetTevAlphaOp(nStage, 0, 0, 1, 1, 0);
                } else {
                    GXSetTevColorIn(nStage, 0xF, 8, 0xA, 0xF);
                    GXSetTevColorOp(nStage, 0, 0, 1, 1, 0);
                    GXSetTevAlphaIn(nStage, 7, 6, 6, 7);
                    GXSetTevAlphaOp(nStage, 0, 0, 1, 1, 0);
                }
                GXSetTevColorIn(nMap, 0xF, 0, 0xC, 0xF);
                GXSetTevColorOp(nMap, 0, 0, 0, 1, 0);
                GXSetTevAlphaIn(nMap, 4, 7, 6, 7);
                GXSetTevAlphaOp(nMap, 0xE, 0, 1, 1, 0);
            } else {
                if (fn_8002A3A4()) {
                    fn_8002A3AC(*lbl_80280DC8);
                    GXSetTevColorIn(nStage, 0xF, 8, 0xA, 0xF);
                    GXSetTevColorOp(nStage, 0, 0, 1, 1, 0);
                    GXSetTevAlphaIn(nStage, 7, 0, 6, 7);
                    GXSetTevAlphaOp(nStage, 0, 0, 1, 1, 0);
                } else {
                    GXSetTevColorIn(nStage, 0xF, 8, 0xA, 0xF);
                    GXSetTevColorOp(nStage, 0, 0, 1, 1, 0);
                    GXSetTevAlphaIn(nStage, 7, 6, 6, 7);
                    GXSetTevAlphaOp(nStage, 0, 0, 1, 1, 0);
                }
                GXSetTevColorIn(nMap, 0xF, 0, 0xC, 0xF);
                GXSetTevColorOp(nMap, 0, 0, 0, 1, 0);
                GXSetTevAlphaIn(nMap, 7, 4, 5, 7);
                GXSetTevAlphaOp(nMap, 0, 0, 1, 1, 0);
            }
            return;
        }
        if (pTex->b40 == 0 || pTex->b40 == 1) {
            GXSetNumTexGens(nStage + 1);
            GXSetTevOrder(nStage, nStage, nStage, 4);
            GXLoadTexObj(pObj, nStage);
            GXSetNumTevStages(nStage + 1);
            if (*lbl_80280DC8) {
                if (fn_8002A3A4()) {
                    fn_8002A3AC(*lbl_80280DC8);
                    return;
                }
                GXSetTevColorIn(nStage, 0xF, 0xA, 0xC, 0xF);
                GXSetTevColorOp(nStage, 0, 0, 1, 1, 0);
                GXSetTevAlphaIn(nStage, 7, 4, 6, 7);
                GXSetTevAlphaOp(nStage, 0, 0, 1, 1, 0);
            } else {
                if (fn_8002A3A4()) {
                    fn_8002A3AC(*lbl_80280DC8);
                    return;
                }
                GXSetTevColorIn(nStage, 0xF, 0xA, 0xC, 0xF);
                GXSetTevColorOp(nStage, 0, 0, 1, 1, 0);
                GXSetTevAlphaIn(nStage, 7, 4, 5, 7);
                GXSetTevAlphaOp(nStage, 0, 0, 1, 1, 0);
            }
        } else if (pTex->b40 == 9) {
            GXSetNumTexGens(nStage + 1);
            nMap = nStage;
            GXSetTevOrder(nStage, nStage, nStage, 4);
            if (pTex->nPalette != -1) {
                GXLoadTlut((GXTlutObj*)&pBank->p14[pTex->n3E], nStage);
            }
            GXLoadTexObj(pObj, nMap);
            GXSetNumTevStages(nStage + 1);
            if (*lbl_80280DC8) {
                if (fn_8002A3A4()) {
                    fn_8002A3AC(*lbl_80280DC8);
                    GXSetTevColorIn(nMap, 0xF, 8, 0xA, 0xF);
                    GXSetTevColorOp(nMap, 0, 0, 1, 1, 0);
                    GXSetTevAlphaIn(nMap, 7, 4, 0, 7);
                    GXSetTevAlphaOp(nMap, 0, 0, 1, 1, 0);
                } else {
                    GXSetTevColorIn(nMap, 0xF, 8, 0xA, 0xF);
                    GXSetTevColorOp(nMap, 0, 0, 1, 1, 0);
                    GXSetTevAlphaIn(nMap, 7, 4, 6, 7);
                    GXSetTevAlphaOp(nMap, 0, 0, 1, 1, 0);
                }
            } else {
                if (fn_8002A3A4()) {
                    fn_8002A3AC(*lbl_80280DC8);
                    GXSetTevColorIn(nMap, 0xF, 8, 0xA, 0xF);
                    GXSetTevColorOp(nMap, 0, 0, 1, 1, 0);
                    GXSetTevAlphaIn(nMap, 7, 4, 0, 7);
                    GXSetTevAlphaOp(nMap, 0, 0, 1, 1, 0);
                } else {
                    GXSetTevColorIn(nMap, 0xF, 8, 0xA, 0xF);
                    GXSetTevColorOp(nMap, 0, 0, 1, 1, 0);
                    GXSetTevAlphaIn(nMap, 7, 4, 6, 7);
                    GXSetTevAlphaOp(nMap, 0, 0, 1, 1, 0);
                }
            }
        } else {
            GXSetNumTexGens(nStage + 1);
            GXSetTevOrder(nStage, nStage, nStage, 4);
            GXLoadTexObj(pObj, nStage);
            GXSetNumTevStages(nStage + 1);
            if (*lbl_80280DC8) {
                if (fn_8002A3A4()) {
                    fn_8002A3AC(*lbl_80280DC8);
                    GXSetTevColorIn(nStage, 0xF, 8, 0xA, 0xF);
                    GXSetTevColorOp(nStage, 0, 0, 1, 1, 0);
                    GXSetTevAlphaIn(nStage, 7, 4, 0, 7);
                    GXSetTevAlphaOp(nStage, 0, 0, 1, 1, 0);
                } else {
                    GXSetTevColorIn(nStage, 0xF, 8, 0xA, 0xF);
                    GXSetTevColorOp(nStage, 0, 0, 1, 1, 0);
                    GXSetTevAlphaIn(nStage, 7, 4, 6, 7);
                    GXSetTevAlphaOp(nStage, 0, 0, 1, 1, 0);
                }
            } else {
                if (fn_8002A3A4()) {
                    fn_8002A3AC(*lbl_80280DC8);
                    GXSetTevColorIn(nStage, 0xF, 8, 0xA, 0xF);
                    GXSetTevColorOp(nStage, 0, 0, 1, 1, 0);
                    GXSetTevAlphaIn(nStage, 7, 4, 0, 7);
                    GXSetTevAlphaOp(nStage, 0, 0, 1, 1, 0);
                } else {
                    GXSetTevColorIn(nStage, 0xF, 8, 0xA, 0xF);
                    GXSetTevColorOp(nStage, 0, 0, 1, 1, 0);
                    GXSetTevAlphaIn(nStage, 7, 4, 5, 7);
                    GXSetTevAlphaOp(nStage, 0, 0, 1, 1, 0);
                }
            }
        }
        return;
    }
    if (*lbl_80280DC8) {
        GXSetTevColorIn(nStage, 0xF, 0xF, 0xF, 0xA);
        GXSetTevColorOp(nStage, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn(nStage, 7, 7, 7, 6);
        GXSetTevAlphaOp(nStage, 0, 0, 1, 1, 0);
        return;
    }
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0xFF, 0xFF, 4);
    GXSetTevColorIn(nStage, 0xF, 0xF, 0xF, 0xA);
    GXSetTevColorOp(nStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(nStage, 7, 7, 7, 5);
    GXSetTevAlphaOp(nStage, 0, 0, 1, 1, 0);
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
    u64 uEntryHash;

    for (i = 0; i < pBank->n2; i++) {
        uEntryHash = pBank->p8[i].u0;
        if (uEntryHash == uHash) return i;
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
    pBank->n2 = nNumTex;
    pBank->n4 = nNumPalettes;
}

void fn_80010114(int nDst, int nFunc, int nSrc, int nMtx) {
    GXSetTexCoordGen2(nDst, nFunc, nSrc, nMtx, 0, 125);   // no normalizing, GX_PTIDENTITY
}
