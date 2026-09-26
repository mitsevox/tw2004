// LLTex.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): texture banks, made
// from the 'txf ' objects of the stream files, and the byte-code scripts that drive the texture
// items (fn_8000EA1C).

#include "engine.h"
#include "gx.h"
#include "dynobj.h"

void fn_80010114(int nDst, int nFunc, int nSrc, int nMtx);

// Builds a bank from a 'txf ' object's data, into pInto or, when it is NULL, a new allocation.
TexBank* TX_spParseTextureGroupFromStream(u8* pData, TexBank* pInto, int n);

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

// Runs a byte-code script on a value stack and returns what is left on top (1 for an empty
// script, 0 while lbl_80281C68 stops scripts). nPush, when not negative, starts on the stack
// above a 1. An opcode with bit 7 set pushes its low 7 bits; opcodes 9 and up pop one operand (a,
// the top), 0x1D and up a second (b), 0x38 and up a third (c). Every opcode that makes a value
// pushes it.
int fn_8000EA1C(u8* pCode, int nArg, int nPush, DynObj* pObj) {
    int a;
    int b;
    int nSp;
    int c;
    int nOp;

    a = 0;
    b = a;
    c = 0;
    if ((s8)*pCode == 0) {
        return 1;
    }
    if (lbl_80281C68) {
        return 0;
    }
    lbl_801A3438[0] = 1;
    nSp = 1;
    if (nPush >= 0) {
        lbl_801A3438[1] = nPush;
        nSp = 2;
    }
    do {
        nOp = (s8)*pCode++;
        if (nOp & 0x80) {
            a = nOp & 0x7F;
        } else {
            if (nOp >= 9) {
                a = lbl_801A3438[--nSp];
                if (nOp >= 0x1D) {
                    b = lbl_801A3438[--nSp];
                    if (nOp >= 0x38) {
                        c = lbl_801A3438[--nSp];
                    }
                }
            }
            switch (nOp) {
            case 1:
                a = -(*pCode++ + 1);
                break;
            case 2:
                a = *pCode;
                pCode++;
                a += 0x80;
                break;
            case 3:
                a = pCode[1] | ((s8)pCode[0] << 8);
                pCode += 2;
                break;
            case 4:
                a = (s8)*pCode++ << 8;
                a = (a | *pCode++) << 8;
                a = (a | *pCode++) << 8;
                a |= *pCode++;
                break;
            case 5:
                a = nArg;
                break;
            case 8:
                pCode += *pCode;
                continue;
            case 16:
                a = lbl_80281C6C[a];
                break;
            case 17:
                // port: entries from 7 on hold 32-bit addresses (also in case 30)
                a = *(int*)lbl_80281C6C[a + 7];
                break;
            case 18:
                a = lbl_80281C74[a] / 5;
                break;
            case 19:
                a = lbl_80281C78[a];
                break;
            case 29:
                lbl_80281C6C[a] = b;
                continue;
            case 30:
                *(int*)lbl_80281C6C[a + 7] = b;
                continue;
            case 31:
                lbl_80281C74[a] = b * 5;
                continue;
            case 32:
                lbl_80281C78[a] = b;
                continue;
            // The steps below change the entry itself, also for the entries from 7 on that hold
            // addresses, and leave the index on the stack.
            case 21:
                lbl_80281C6C[a]++;
                break;
            case 22:
                lbl_80281C6C[a + 7]++;
                break;
            case 23:
                lbl_80281C74[a] += 5;
                break;
            case 24:
                lbl_80281C78[a]++;
                break;
            case 25:
                lbl_80281C6C[a]--;
                break;
            case 26:
                lbl_80281C6C[a + 7]--;
                break;
            case 27:
                lbl_80281C74[a] -= 5;
                break;
            case 28:
                lbl_80281C78[a]--;
                break;
            case 48:
                lbl_80281C6C[a] += b;
                break;
            case 49:
                lbl_80281C6C[a + 7] += b;
                break;
            case 50:
                lbl_80281C74[a] += b * 5;
                break;
            case 51:
                lbl_80281C78[a] += b;
                break;
            case 52:
                lbl_80281C6C[a] -= b;
                break;
            case 53:
                lbl_80281C6C[a + 7] -= b;
                break;
            case 54:
                lbl_80281C74[a] -= b * 5;
                break;
            case 55:
                lbl_80281C78[a] -= b;
                break;
            case 9:
                a = a == 0;
                break;
            case 11:
                // a random number below -a, or entry a of the 32 in lbl_80281C70
                if (a >= 0) {
                    a %= 32;
                    a = lbl_80281C70[a];
                } else {
                    int nRand = (int)Misc_RandFunc(0);
                    int nSign = nRand >> 31;
                    b = (nSign + nRand) ^ nSign;   // EA's abs
                    a = b % -a;
                }
                break;
            case 20:
                // a jump taken when a is 0
                if (a == 0) {
                    pCode += *pCode;
                } else {
                    pCode++;
                }
                continue;
            case 33:
                a = b == a;
                break;
            case 34:
                a = b != a;
                break;
            case 35:
                a = b > a;
                break;
            case 36:
                a = b >= a;
                break;
            case 37:
                a = b < a;
                break;
            case 38:
                a = b <= a;
                break;
            case 39:
                a += b;
                break;
            case 40:
                a = b - a;
                break;
            case 41:
                a *= b;
                break;
            case 42:
                a = b / a;
                break;
            case 43:
                if (a <= 0) {
                    a = b & -a;
                } else {
                    a = b % a;
                }
                break;
            case 44:
                a = b != 0 && a != 0;
                break;
            case 45:
                a = b != 0 || a != 0;
                break;
            case 46:
                if (b == 0) {
                    return a;
                }
                continue;
            case 47:
                // b 0: ask this object for its value a; b above 0: the first object with id b
                if (b == 0) {
                    a = pObj->pfnHandler(9, pObj, (void*)a, NULL);
                } else if (b >= 0) {
                    a = fn_800494AC(b, a);
                }
                break;
            case 56:
                if (c == 0) {
                    fn_80049304(pObj->n140, b, a);
                } else {
                    fn_80049304(c, b, a);
                }
                continue;
            case 58:
                fn_80049424(c, b, a);
                continue;
            case 57:
                fn_8004939C(c, b, a);
                continue;
            case 60:
                fn_8000E948(c, b, a);
                continue;
            case 61:
                fn_8000E9A8(c, b, a);
                continue;
            case 10:
            case 14:
            case 15:
                break;
            default:
                continue;
            }
        }
        lbl_801A3438[nSp++] = a;
    } while ((s8)*pCode != 0);
    return lbl_801A3438[--nSp];
}

// Resets the constant-alpha flag, TEV stages 0-2's constant alpha selector (0x1C) and texture
// coordinates 0-2 (from texture coordinate 0 through matrix 0x3C).
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

// The data (after a 0x10-byte header) is the bank's first 8 bytes, then four sections: the
// textures, the palette rows, the pixels and the palette colours. With n -2 the bank keeps
// pointing into the data; otherwise it copies the pixels and colours and sets up the GX objects.
// n is added to every level's n8 and every palette's n6.
TexBank* TX_spParseTextureGroupFromStream(u8* p, TexBank* pInto, int n) {
    TexBank* pBank;
    TexBank* pHead;
    TexSection* pSection;
    TexSection* pPixels;
    u8* pTables;
    u8* pColors;
    TexEntry* pTex;
    TexPalette* pPalette;
    int nSize;
    int i;
    int j;
    int nOffset;

    p += 0x10;
    pHead = (TexBank*)p;
    p += 8;
    nSize = sizeof(TexBank) + pHead->n2 * sizeof(TexEntry) + pHead->n4 * sizeof(TexPalette) +
            pHead->n4 * sizeof(TexGXTlut) + pHead->n2 * sizeof(TexGXObj);
    if (pInto != NULL) {
        pBank = pInto;
    } else {
        pBank = fn_80009B34(nSize, 2, 0x10, "LLTex.c", 0x717);
        pInto = pBank;
    }
    // the bank's first 8 bytes (its counts n2 and n4) as the data has them
    ((u32*)pBank)[0] = ((u32*)pHead)[0];
    ((u32*)pBank)[1] = ((u32*)pHead)[1];
    pTables = (u8*)(pInto + 1);
    pBank->p8 = (TexEntry*)pTables;
    pTables += pBank->n2 * sizeof(TexEntry);
    pBank->pC = (TexPalette*)pTables;
    pTables += pBank->n4 * sizeof(TexPalette);
    pBank->p10 = (TexGXObj*)pTables;
    pTables += pBank->n2 * sizeof(TexGXObj);
    pBank->p14 = (TexGXTlut*)pTables;

    pSection = (TexSection*)p;
    p += 8;
    Mem_cpy(pBank->p8, p, pBank->n2 * sizeof(TexEntry));
    p += pSection->nSize;
    pSection = (TexSection*)p;
    p += 8;
    Mem_cpy(pBank->pC, p, pBank->n4 * sizeof(TexPalette));
    p += pSection->nSize;
    pPixels = (TexSection*)p;
    p += 8;
    if (n != -2) {
        pBank->p18 = fn_80009B34(pPixels->nSize, 2, 0x20, "LLTex.c", 0x77A);
        pBank->n1C = pPixels->nSize;
        Mem_cpy(pBank->p18, p, pPixels->nSize);
    } else {
        pBank->p18 = p;
    }
    pBank->n28 = n;
    pBank->b2C = 0;
    if (n == -1) {
        pBank->b2D = 1;
    } else {
        pBank->b2D = 0;
    }

    for (i = 0, nOffset = 0; i < pBank->n2; nOffset += sizeof(TexEntry), i++) {
        pTex = (TexEntry*)((u8*)pBank->p8 + nOffset);
        for (j = 0; j < pTex->n41; j++) {
            pTex->aMips[j].n8 += (s16)pBank->n28;
        }
        if (n != -2) {
            if (pTex->nPalette == -1) {
                GXInitTexObj((GXTexObj*)&pBank->p10[pTex->n3E], pBank->p18 + pTex->aMips[0].uPixels,
                             pTex->nWidth, pTex->nHeight, pTex->b40, (pTex->b46 & 1) == 0,
                             (pTex->b46 & 2) == 0, pTex->n41 > 1);
            } else {
                GXInitTexObjCI((GXTexObj*)&pBank->p10[pTex->n3E], pBank->p18 + pTex->aMips[0].uPixels,
                               pTex->nWidth, pTex->nHeight, pTex->b40, (pTex->b46 & 1) == 0,
                               (pTex->b46 & 2) == 0, 0, 0);
            }
            if (pTex->n41 > 1) {
                GXInitTexObjLOD((GXTexObj*)&pBank->p10[pTex->n3E], 5, 1, 0.0f, pTex->n41 - 1.0f, -2.0f,
                                0, 0, 0);
            }
        }
    }

    p += pPixels->nSize;
    pSection = (TexSection*)p;
    p += 8;
    if (pSection->nSize > 0) {
        if (n != -2) {
            pBank->p20 = fn_80009B34(pSection->nSize, 2, 0x20, "LLTex.c", 0x7FB);
            pBank->n24 = pSection->nSize;
            Mem_cpy(pBank->p20, p, pSection->nSize);
        } else {
            pBank->p20 = p;
        }
    } else {
        pBank->p20 = NULL;
    }
    if (pBank->n24 == 0x200) {
        pColors = pBank->p20;
    } else {
        pColors = p;
    }

    for (i = 0, nOffset = 0; i < pBank->n4; nOffset += sizeof(TexPalette), i++) {
        pPalette = (TexPalette*)((u8*)pBank->pC + nOffset);
        pPalette->n6 += (s16)pBank->n28;
        if (n != -2) {
            // every row is set up into the bank's first GX palette object
            GXInitTlutObj((GXTlutObj*)pBank->p14, pColors + pPalette->uColors, pPalette->nFormat,
                          pPalette->nEntries);
        }
    }
    return pBank;
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

void fn_800100B0(TexBank* pBank, TexEntry* p8, TexPalette* pC, void* p10, void* p14, int nNumTex,
                 int nNumPalettes) {
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
