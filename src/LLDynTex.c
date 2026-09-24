// LLDynTex.c (EA's name, from its asserts): textures whose pixels the game rewrites while they are
// shown; the menu golfer (FEgolferanim.c) drives them, and char_tex_manager.c puts the user logos
// in them. Partly decompiled.

#include "engine.h"
#include "gx.h"
#include "core/startup.h"
#include "lldyntex.h"
#include "character.h"

// ---- sweep code (not yet cleaned up) ----

void fn_8001052C(s16 n);
void fn_8010A668(DynTex* pTex);
void* fn_8010A780(DynTex* pTex);
void fn_8010B098(void* p);
int fn_8010C458(int nFormat);
s32 fn_8010B664(DynTexPalette* pPal);
void fn_8000FBAC();

// ---- end of sweep code ----

void fn_8010B7C0(void);
void fn_8010A930(DynTexObj* pObj, u8* pBuf, f32 (*pMtx)[3], s32 nMode);
s32  fn_8010B754(DynTexObj* pObj);
s32  fn_8010B338(DynTex* pTex, DynTexObj* pObj, DynTexPalette* pPal, u8* pPixels, u8* pPalette,
                 void* p, s32 n);   // adds a texture; gives its index
DynTexJob* fn_8010B960(void);
u8   fn_8010BF3C(void);
u8   fn_8010BFE0(void);
void fn_80007254(void);                 // LLDisp_Gc.c
int  fn_800106F0(TexBank* pBank);       // LLTexGrp.c
int  fn_8001005C(TexBank* pBank, u64 uHash);       // LLTex.c: the texture's index, or 0x80000000
TexEntry* fn_800107E4(TexBank* pBank, int nTex);  // LLTexGrp.c
s32  fn_8010BC94(const void* pA, const void* pB);
s32  fn_8010B5F8(DynTexObj* pObj);
void fn_80007368(void);
void fn_80007328(void);

// Set up: the state and its nSize-byte block (gomainloop.c: 0x18000, later 0x6000).
void fn_8010A448(int nSize) {
    lbl_80282488 = fn_80009B34(sizeof(DynTexState), 2, 16, "LLDynTex.c", 105);
    lbl_80282488->p0 = fn_80009B34(nSize, 2, 64, "LLDynTex.c", 108);
    lbl_80282488->p8 = NULL;
    lbl_80282488->b975 = 1;
    lbl_80282488->b974 = 1;
    lbl_80282488->n980 = 0;
    lbl_80282488->nA98 = nSize;
    fn_8010B7C0();
}

// ---- sweep code (not yet cleaned up) ----

void fn_8010A4E8(void) {
    if (lbl_80282488->p0 != NULL) {
        fn_80009E70(lbl_80282488->p0);
    }
    fn_80009E70(lbl_80282488);
}

// ---- end of sweep code ----

// Make a dynamic texture for nC textures with an nSize-byte pixel buffer: one block holds the
// DynTex, its header and the per-texture tables; the header is registered as a texture bank.
DynTex* fn_8010A520(int nC, int nSize, int n2, int n3, int n4) {
    s32 nBytes;
    DynTex* pTex;
    DynTexHeader* pHeader;

    // fake match: the terms' order and the casts only steer CW's regrouping (still 90%)
    nBytes = nC * (s32)sizeof(DynTexObj) + nC * (s32)sizeof(DynTexEntry) +
             nC * (s32)sizeof(DynTexPalette) + nC * (s32)sizeof(DynTex40) +
             nC * (s32)sizeof(DynTex18) + (s32)sizeof(DynTex);
    pTex = fn_80009B34(nBytes, 2, 16, "LLDynTex.c", 151);
    memset(pTex, 0, nBytes);
    pTex->nC = nC;
    pTex->p18 = fn_80009B34(nSize, 2, 32, "LLDynTex.c", 161);
    pTex->n10 = nSize;
    pHeader = &pTex->header;
    pTex->p4 = pHeader;
    pTex->p0 = (DynTexEntry*)(pHeader + 1);
    pTex->header.p8 = (DynTexObj*)(pTex->p0 + nC);
    pTex->header.pC = (DynTexPalette*)(pTex->header.p8 + nC);
    pTex->header.p10 = (DynTex40*)(pTex->header.pC + nC);
    pTex->header.p14 = (DynTex18*)(pTex->header.p10 + nC);
    pTex->header.p18 = pTex->p18;
    pTex->header.p20 = pTex->p18;
    pTex->header.b2C = 0;
    // port: the header has TexBank's layout (see lldyntex.h)
    pTex->n1C = fn_800106F0((TexBank*)pTex->p4);
    pTex->p4->n7 = n4;
    pTex->p4->n28 = n2;
    pTex->p4->n0 = n3;
    return pTex;
}

// ---- sweep code (not yet cleaned up) ----

// Free a dynamic texture.
void fn_8010A668(DynTex* pTex) {
    fn_8001052C(pTex->n1C);
    fn_80009E70(pTex->p18);
    fn_80009E70(pTex);
}

// ---- end of sweep code ----

// pDst emptied, then every texture of pSrc added to it (with its palette when it has one).
void fn_8010A6A8(DynTex* pSrc, DynTex* pDst) {
    int i;
    u8* pPixels;
    u8* pPalettes;
    DynTexObj* pObj;
    DynTexPalette* pPal;

    fn_8010B098(pDst);
    pPixels = pSrc->p4->p18;
    pPalettes = pSrc->p4->p20;
    for (i = 0; i < pSrc->n8; i++) {
        pObj = &pSrc->p4->p8[i];
        pPal = NULL;
        if (pObj->n3C != -1) {
            pPal = &pSrc->p4->pC[i];
        }
        if (pPal != NULL) {
            fn_8010B338(pDst, pObj, pPal, pPixels + pObj->aBlocks[0].nOffset, pPalettes + pPal->nOffset,
                        NULL, 0);
        } else {
            fn_8010B338(pDst, pObj, NULL, pPixels + pObj->aBlocks[0].nOffset, NULL, NULL, 0);
        }
    }
}

// ---- sweep code (not yet cleaned up) ----

void* fn_8010A780(DynTex* pTex) {
    return pTex->p4;
}

// ---- end of sweep code ----

// Put an RGB colour through a 3x3 colour matrix, clamped to 0..1. Mode 0 combines each input
// channel's part like light (one minus the product of what each lets through), mode 1 adds them.
void fn_8010A788(f32* pIn, f32* pOut, f32 (*pMtx)[3], s32 nMode) {
    if (nMode == 0) {
        pOut[0] = 1.0f - (1.0f - pIn[0] * pMtx[0][0]) * (1.0f - pIn[1] * pMtx[1][0]) *
                             (1.0f - pIn[2] * pMtx[2][0]);
        pOut[1] = 1.0f - (1.0f - pIn[0] * pMtx[0][1]) * (1.0f - pIn[1] * pMtx[1][1]) *
                             (1.0f - pIn[2] * pMtx[2][1]);
        pOut[2] = 1.0f - (1.0f - pIn[0] * pMtx[0][2]) * (1.0f - pIn[1] * pMtx[1][2]) *
                             (1.0f - pIn[2] * pMtx[2][2]);
    } else if (nMode == 1) {
        pOut[0] = pIn[0] * pMtx[0][0] + pIn[1] * pMtx[1][0] + pIn[2] * pMtx[2][0];
        pOut[1] = pIn[0] * pMtx[0][1] + pIn[1] * pMtx[1][1] + pIn[2] * pMtx[2][1];
        pOut[2] = pIn[0] * pMtx[0][2] + pIn[1] * pMtx[1][2] + pIn[2] * pMtx[2][2];
    }
    pOut[0] = (pOut[0] < 0.0f) ? 0.0f : ((pOut[0] > 1.0f) ? 1.0f : pOut[0]);
    pOut[1] = (pOut[1] < 0.0f) ? 0.0f : ((pOut[1] > 1.0f) ? 1.0f : pOut[1]);
    pOut[2] = (pOut[2] < 0.0f) ? 0.0f : ((pOut[2] > 1.0f) ? 1.0f : pOut[2]);
}

// Recolour a compressed (GX_TF_CMPR) texture through a colour matrix (fn_8010A788): in each 4x4
// block, both key colours (RGB565) go through it; when that flips which one is larger, which
// switches the block between its four- and three-colour modes, they are stored the other way
// round and the block's 2-bit indices are remapped to match.
void fn_8010A930(DynTexObj* pObj, u8* pBuf, f32 (*pMtx)[3], s32 nMode) {
    u8* pPixels = pBuf + pObj->aBlocks[0].nOffset;
    u16* pBlock = (u16*)pPixels;
    int y;
    int x;
    f32 aIn[3];
    f32 aOut[3];
    u16 uOld0;
    u16 uOld1;
    u16 uNew0;
    u16 uNew1;
    u8 bSwap;
    u8 bThree;
    int i;
    int nShift;
    u8 uIndices;
    u8 uIndex;
    u8* pIndices;

    if (pObj->n40 != 14) {      // GX_TF_CMPR
        return;
    }
    for (y = 0; y < pObj->n3A / 4; y++) {
        for (x = 0; x < pObj->n38 / 4; x++, pBlock += 4) {
            uOld0 = pBlock[0];
            aIn[0] = (f32)(u32)((uOld0 >> 8) & 0xF8) / 255.0f;
            aIn[1] = (f32)(u32)((uOld0 >> 3) & 0xFC) / 255.0f;
            aIn[2] = (f32)(u32)((uOld0 << 3) & 0xF8) / 255.0f;
            fn_8010A788(aIn, aOut, pMtx, nMode);
            uNew0 = (u16)((((u8)(int)(aOut[0] * 255.0f + 0.5f) >> 3) << 11) |
                          (((u8)(int)(aOut[1] * 255.0f + 0.5f) >> 2) << 5) |
                          ((u8)(int)(aOut[2] * 255.0f + 0.5f) >> 3));
            uOld1 = pBlock[1];
            aIn[0] = (f32)(u32)((uOld1 >> 8) & 0xF8) / 255.0f;
            aIn[1] = (f32)(u32)((uOld1 >> 3) & 0xFC) / 255.0f;
            aIn[2] = (f32)(u32)((uOld1 << 3) & 0xF8) / 255.0f;
            fn_8010A788(aIn, aOut, pMtx, nMode);
            uNew1 = (u16)((((u8)(int)(aOut[0] * 255.0f + 0.5f) >> 3) << 11) |
                          (((u8)(int)(aOut[1] * 255.0f + 0.5f) >> 2) << 5) |
                          ((u8)(int)(aOut[2] * 255.0f + 0.5f) >> 3));
            if (uOld0 > uOld1) {
                // Four colours: the first key colour must stay the larger.
                if (uNew0 > uNew1) {
                    pBlock[0] = uNew0;
                    bSwap = 0;
                    pBlock[1] = uNew1;
                } else {
                    pBlock[0] = uNew1;
                    bSwap = 1;
                    bThree = 0;
                    pBlock[1] = uNew0;
                }
            } else if (uNew0 > uNew1) {
                // Three colours: the first key colour must stay the smaller.
                pBlock[0] = uNew1;
                bSwap = 1;
                bThree = 1;
                pBlock[1] = uNew0;
            } else {
                pBlock[0] = uNew0;
                bSwap = 0;
                pBlock[1] = uNew1;
            }
            if (!bSwap) {
                continue;
            }
            pIndices = (u8*)&pBlock[2];
            for (i = 0; i < 4; i++, pIndices++) {
                uIndices = 0;
                for (nShift = 0; nShift < 8; nShift += 2) {
                    // EA bug: the mask keeps every bit from nShift up, not just the index's two,
                    // and the indices are compared as if 0x10 and 0x11 were binary 10 and 11;
                    // only the top index of a row is read right.
                    uIndex = (*pIndices & (0xFF << nShift)) >> nShift;
                    if (bThree) {
                        if (uIndex == 0) {
                            uIndices |= 1 << nShift;
                        } else if (uIndex == 1) {
                            uIndices |= 0 << nShift;
                        } else if (uIndex == 0x10) {
                            uIndices |= 0x10 << nShift;
                        } else if (uIndex == 0x11) {
                            uIndices |= 0x11 << nShift;
                        }
                    } else {
                        if (uIndex == 0) {
                            uIndices |= 1 << nShift;
                        } else if (uIndex == 1) {
                            uIndices |= 0 << nShift;
                        } else if (uIndex == 0x10) {
                            uIndices |= 0x11 << nShift;
                        } else if (uIndex == 0x11) {
                            uIndices |= 0x10 << nShift;
                        }
                    }
                }
                *pIndices = uIndices;
            }
        }
    }
    DCFlushRange(pPixels, fn_8010B754(pObj));
    GXInvalidateTexAll();
}

// ---- sweep code (not yet cleaned up) ----

s32 fn_8010AD10(DynTex* pTex) {
    return pTex->n8;
}

// ---- end of sweep code ----

u64 fn_8010AD18(DynTex* pTex, int nTex) {
    if (nTex >= 0 && nTex < pTex->n8) {
        return pTex->p0[nTex].uId;
    }
    return 0;
}

void fn_8010AD50(DynTex* pTex, u64 uId) {
    int i;

    for (i = 0; i < pTex->n8; i++) {
        if (pTex->p0[i].uId == uId) {
            pTex->p0[i].uId = 0;
        }
    }
}

// Drops the textures whose id was cleared (fn_8010AD50): the others' pixels and palettes move
// down in p18 over the gaps, and from the first dropped one on their entries and descriptions
// are packed to the front.
void fn_8010ADA4(DynTex* pTex) {
    DynTexHeader* pHdr = pTex->p4;
    DynTexEntry* pOutEntry = pTex->p0;
    DynTexObj* pOutObj = pHdr->p8;
    DynTexPalette* pOutPal = pHdr->pC;
    DynTex40* pOut10 = pHdr->p10;
    DynTex18* pOut14 = pHdr->p14;
    DynTexEntry* pEntry;
    u32 nSrc = 0;
    u32 nDst = 0;
    s32 nKept = 0;
    s32 nObjs = 0;
    u8 bMoved = 0;
    int i;
    int j;

    for (i = 0; i < pTex->n8; i++) {
        pEntry = &pTex->p0[i];
        if (pEntry->uId == 0) {
            bMoved = 1;
            for (j = 0; j < pEntry->n8; j++) {
                nSrc += pEntry->aC[j];
            }
            nSrc += pEntry->n1C;
            continue;
        }
        for (j = 0; j < pEntry->n8; j++) {
            if (pEntry->aC[j] != 0 && nDst != nSrc) {
                memmove(pTex->p18 + nDst, pTex->p18 + nSrc, pEntry->aC[j]);
                pHdr->p8[i].aBlocks[j].nOffset = nDst;
            }
            nDst += pEntry->aC[j];
            nSrc += pEntry->aC[j];
        }
        if (pEntry->n1C != 0 && nDst != nSrc) {
            memmove(pTex->p18 + nDst, pTex->p18 + nSrc, pEntry->n1C);
            pHdr->pC[i].nOffset = nDst;
        }
        nDst += pEntry->n1C;
        nSrc += pEntry->n1C;
        if (bMoved) {
            memcpy(pOutObj, &pHdr->p8[i], sizeof(DynTexObj));
            memcpy(pOutPal, &pHdr->pC[i], sizeof(DynTexPalette));
            memcpy(pOut10, &pHdr->p10[i], sizeof(DynTex40));
            memcpy(pOut14, &pHdr->p14[i], sizeof(DynTex18));
            memcpy(pOutEntry, pEntry, sizeof(DynTexEntry));
            if (pEntry->n1C != 0) {
                pOutObj->n3C = (s8)nKept;
            }
        }
        pOutEntry++;
        pOutObj++;
        pOutPal++;
        pOut10++;
        pOut14++;
        nObjs++;
        nKept++;
    }
    pTex->n8 = nKept;
    pHdr->n2 = nKept;
    pHdr->n4 = nObjs;
    pTex->n14 = nDst;
}

// Fills pEntry for pObj and its palette pPal (if any): the id, each level's bytes and the
// palette's, each rounded up to 16. Returns them all added up.
s32 fn_8010B0C0(DynTexObj* pObj, DynTexPalette* pPal, DynTexEntry* pEntry) {
    u32 nA = pObj->n38;
    u32 nB = pObj->n3A;
    s32 nTotal = 0;
    int nBits;
    int i;

    memset(pEntry, 0, sizeof(DynTexEntry));
    pEntry->uId = pObj->uId;
    pEntry->n8 = pObj->n41;
    nBits = fn_8010C458(pObj->n40);
    for (i = 0; i < pEntry->n8; i++) {
        pEntry->aC[i] = (nB * (nBits * nA) + 7) >> 3;
        nA >>= 1;
        nB >>= 1;
        pEntry->aC[i] = (pEntry->aC[i] + 15) & ~15;
        nTotal += pEntry->aC[i];
    }
    if (pPal != NULL) {
        pEntry->n1C = (pPal->nEntries * (u32)fn_8010C458(pPal->nFormat) + 7) >> 3;
    } else {
        pEntry->n1C = 0;
    }
    pEntry->n1C = (pEntry->n1C + 15) & ~15;
    nTotal += pEntry->n1C;
    return nTotal;
}

// New pixels for texture nTex (pPixels holds its blocks back to back), each block flushed to the
// GPU; p and n are handed to fn_8010A930 when p is set.
void fn_8010B1D4(DynTex* pTex, int nTex, u8* pPixels, void* p, s32 n) {
    DynTexEntry* pEntry = &pTex->p0[nTex];
    DynTexObj* pObj = &pTex->p4->p8[nTex];
    s32 nBase = pObj->aBlocks[0].nOffset;
    int i;

    for (i = 0; i < pEntry->n8; i++) {
        if (pPixels != NULL) {
            memcpy(pTex->p18 + pObj->aBlocks[i].nOffset,
                   pPixels + (pObj->aBlocks[i].nOffset - nBase), pEntry->aC[i]);
        }
        if (p != NULL) {
            fn_8010A930(pObj, pTex->p18, p, n);
        }
        DCFlushRange(pTex->p18 + pObj->aBlocks[i].nOffset, pEntry->aC[i]);
        GXInvalidateTexAll();
    }
}

// A new palette for texture nTex.
void fn_8010B2A8(DynTex* pTex, int nTex, s16* pPalette) {
    DynTexPalette* aPalettes = pTex->p4->pC;
    DynTexEntry* pEntry = &pTex->p0[nTex];

    if (pPalette != NULL) {
        memcpy(pTex->p18 + aPalettes[nTex].nOffset, pPalette, pEntry->n1C);
    }
    DCFlushRange(pTex->p18 + aPalettes[nTex].nOffset, pEntry->n1C);
    GXInvalidateTexAll();
}

// Add a copy of texture pObj (and its palette pPal, if any) to pTex: its pixel blocks and palette
// get room at the end of pTex's buffer, filled from pPixels and pPalette when given, the pixels
// then recoloured by fn_8010A930 when p is set. Returns the new texture's index, or 0 when the
// buffer is full.
s32 fn_8010B338(DynTex* pTex, DynTexObj* pObj, DynTexPalette* pPal, u8* pPixels, u8* pPalette,
                void* p, s32 n) {
    char szName[16];            // the size is not known (fn_800CB8F0 writes the name)
    s32 nTex;
    DynTexObj* pNew;
    DynTexPalette* pNewPal;
    s32 nBytes;
    int i;
    DynTexEntry* pEntry;
    s32 nFirst;

    pNewPal = NULL;
    nTex = pTex->n8;
    memcpy(&pTex->p4->p8[nTex], pObj, sizeof(DynTexObj));
    pNew = &pTex->p4->p8[nTex];
    pEntry = &pTex->p0[nTex];
    if (pPal != NULL) {
        pNew->n3C = (s8)nTex;
        memcpy(&pTex->p4->pC[nTex], pPal, sizeof(DynTexPalette));
        pNewPal = &pTex->p4->pC[nTex];
    } else {
        memset(&pTex->p4->pC[nTex], 0, sizeof(DynTexPalette));
    }
    if ((u32)(pTex->n14 + fn_8010B0C0(pNew, pNewPal, pEntry)) > (u32)pTex->n10) {
        fn_800CB8F0(&pObj->uId, szName);
        return 0;
    }
    nFirst = pNew->aBlocks[0].nOffset;
    for (i = 0; i < pEntry->n8; i++) {
        nBytes = pEntry->aC[i];
        if (pPixels != NULL) {
            memcpy(pTex->p18 + pTex->n14, pPixels + (pNew->aBlocks[i].nOffset - nFirst), nBytes);
        }
        pNew->aBlocks[i].nOffset = pTex->n14;
        if (pPixels != NULL && p != NULL) {
            fn_8010A930(pNew, pTex->p18, p, n);
        }
        pTex->n14 += nBytes;
    }
    if (pEntry->n1C != 0) {
        if (pPalette != NULL) {
            memcpy(pTex->p18 + pTex->n14, pPalette, pEntry->n1C);
        }
        pNewPal->nOffset = pTex->n14;
        pTex->n14 += pEntry->n1C;
    }
    pTex->n8++;
    pTex->p4->n2++;
    if (pNewPal != NULL) {
        pTex->p4->n4++;
    }
    return pTex->n8 - 1;
}

// A free job, or NULL.
DynTexJob* fn_8010B8EC(void) {
    int i;

    for (i = 0; i < 10; i++) {
        if (!lbl_80282488->aJobs[i].bUsed) {
            return &lbl_80282488->aJobs[i];
        }
    }
    return NULL;
}

// Queue a job.
void fn_8010B930(DynTexJob* pJob) {
    lbl_80282488->apQueue[lbl_80282488->nA84] = pJob;
    pJob->bUsed = 1;
    lbl_80282488->nA84++;
}

// Take the first queued job off the queue.
DynTexJob* fn_8010B960(void) {
    int i;
    DynTexJob* pJob = lbl_80282488->apQueue[0];

    for (i = 0; i < lbl_80282488->nA84; i++) {
        if (i == 9) {
            lbl_80282488->apQueue[i] = NULL;
        } else {
            lbl_80282488->apQueue[i] = lbl_80282488->apQueue[i + 1];
        }
    }
    lbl_80282488->nA84--;
    return pJob;
}

// Free every job: the one in pA88 and all queued ones.
void fn_8010B9BC(void) {
    if (lbl_80282488->pA88 != NULL) {
        lbl_80282488->pA88->bUsed = 0;
    }
    while (lbl_80282488->nA84 != 0) {
        fn_8010B960()->bUsed = 0;
    }
    if (lbl_80282488->n980 != 0) {
        lbl_80282488->n980 = 3;
    }
}

// ---- sweep code (not yet cleaned up) ----

// Empties pTex: no textures, nothing of p18 used (char.c and FEgolferanim.c pass it as a void*).
void fn_8010B098(void* p) {
    DynTex* pTex = p;

    if (pTex != NULL) {
        pTex->n8 = 0;
        pTex->p4->n2 = 0;
        pTex->p4->n4 = 0;
        pTex->n14 = 0;
    }
}

s32 fn_8010B664(DynTexPalette* pPal) {
    if (pPal->nEntries > 16) {
        if (fn_8010C458(pPal->nFormat) == 16) {
            return 0x20;
        }
        return 0x40;
    }
    return 4;
}

// Set up GX's texture (and palette) objects of each of pTex's textures in use, first moving
// their blocks' n8 by the header's n28. Always 1.
s32 fn_8010BA2C(DynTex* pTex) {
    int i;
    DynTexObj* pObj;
    DynTexPalette* pPal;
    DynTex40* pTexObj;
    DynTex18* pTlut;
    int j;

    if (pTex == NULL) {
        return 1;
    }
    for (i = 0; i < pTex->n8; i++) {
        DynTexEntry* pEntry = &pTex->p0[i];

        pObj = &pTex->p4->p8[i];
        pPal = &pTex->p4->pC[i];
        pTexObj = &pTex->p4->p10[i];
        pTlut = &pTex->p4->p14[i];
        if (pEntry->uId == 0) {
            continue;
        }
        for (j = 0; j < pObj->n41; j++) {
            pObj->aBlocks[j].n8 += (s16)pTex->p4->n28;
        }
        if (pObj->n3C == -1) {
            // fake match: the same wrap tests as below, written another way (82.7% -> 86.6%)
            GXInitTexObj(&pTexObj->tex, pTex->p4->p18 + pObj->aBlocks[0].nOffset, pObj->n38,
                         pObj->n3A, pObj->n40, !(pObj->b46 & 1), !((pObj->b46 >> 1) & 1),
                         pObj->n41 > 1);
        } else {
            GXInitTexObjCI(&pTexObj->tex, pTex->p4->p18 + pObj->aBlocks[0].nOffset, pObj->n38,
                           pObj->n3A, pObj->n40, (pObj->b46 & 1) == 0, (pObj->b46 & 2) == 0, 0,
                           0);
            if (pPal != NULL) {
                GXInitTlutObj(&pTlut->tlut, pTex->p4->p20 + pPal->nOffset, pPal->nFormat,
                              pPal->nEntries);
            }
        }
        if (pObj->n41 > 1) {
            GXInitTexObjLOD(&pTexObj->tex, 5, 1, 0.0f, pObj->n41 - 1.0f, -2.0f, 0, 0, 0);
        }
        pObj->n3E = i;
    }
    return 1;
}

void fn_8010BC64(u8* p) {
    fn_8000FBAC(*(s32*)(p + 0x4));
}

void fn_8010BC88(void* p) {
    lbl_80282488->p8 = p;
}

// qsort's order for DynTexUse entries: by where their textures' pixels start in the bank.
s32 fn_8010BC94(const void* pA, const void* pB) {
    // port: EA converts the offsets as signed
    f32 fA = (s32)((DynTexUse*)pA)->pTex->uPixels;
    f32 fB = (s32)((DynTexUse*)pB)->pTex->uPixels;

    return (fA < fB) ? -1 : (fA >= fB);
}

// Note that a skin uses the bank's texture uId (and the one paired with it), with p and n; an
// unknown name is only turned into text.
void fn_8010BCFC(u64 uId, void* p, s32 n) {
    char szName[16];            // the size is not known (fn_800CB868 writes the name)
    int nTex = fn_8001005C(*lbl_80282488->p8, uId);

    if (nTex != (int)0x80000000) {
        lbl_80282488->aUses[lbl_80282488->n96C].pTex = fn_800107E4(*lbl_80282488->p8, nTex);
    } else {
        fn_800CB868(&uId, szName);
        return;
    }
    lbl_80282488->aUses[lbl_80282488->n96C].p4 = p;
    lbl_80282488->aUses[lbl_80282488->n96C].n8 = n;
    lbl_80282488->aUses[lbl_80282488->n96C].nC = -1;
    lbl_80282488->n96C++;
    if (lbl_80282488->aUses[lbl_80282488->n96C - 1].pTex->b47 & 1) {
        lbl_80282488->aUses[lbl_80282488->n96C].pTex = fn_800107E4(*lbl_80282488->p8, nTex + 1);
        lbl_80282488->aUses[lbl_80282488->n96C].p4 = p;
        lbl_80282488->aUses[lbl_80282488->n96C].n8 = n;
        lbl_80282488->aUses[lbl_80282488->n96C].nC = -1;
        // The pair is kept only when it has the same name.
        if ((lbl_80282488->aUses[lbl_80282488->n96C - 1].pTex->b47 & 1) &&
            lbl_80282488->aUses[lbl_80282488->n96C - 1].pTex->u0 ==
                lbl_80282488->aUses[lbl_80282488->n96C].pTex->u0) {
            lbl_80282488->n96C++;
        }
    }
}

void fn_8010BEC4(void) {
    lbl_80282488->n96C = 0;
}

// Start over: sort the textures in use by where their pixels start (fn_8010BC94).
void fn_8010BED4(void) {
    lbl_80282488->b975 = 1;
    lbl_80282488->b974 = 1;
    lbl_80282488->n978 = 0;
    lbl_80282488->n970 = 0;
    lbl_80282488->n980 = 1;
    qsort(lbl_80282488->aUses, lbl_80282488->n96C, sizeof(DynTexUse), fn_8010BC94);
}

// Whether the queue is empty and nothing is left to do.
u8 fn_8010BF3C(void) {
    int bDone = 0;

    if (lbl_80282488->nA84 == 0 && lbl_80282488->n980 == 0) {
        bDone = 1;
    }
    return bDone;
}

// fn_80006444's callback: nBytes more arrived (nError is not read).
void fn_8010BFA0(int nBytes, int nError) {
    lbl_80282488->n97C = nBytes;
    lbl_80282488->n978 = lbl_80282488->n978 + nBytes;
    lbl_80282488->b974 = 1;
    if (lbl_80282488->n978 >= lbl_80282488->n984) {
        lbl_80282488->b975 = 1;
    }
}

// The Character whose p50 ppBank points at (char.c hands LLDynTex.c &Character.p50; the fields
// after it, up to p60, are read through it).
// port: EA likely had these five fields in a struct of their own inside Character
#define DYNTEX_CHAR(ppBank) ((Character*)((u8*)(ppBank) - 0x50))

// The loader, run each frame by fn_8010BF68: takes the next queued job, then streams the pixels
// and palette of each texture in use (aUses) from the job's character file in reads of at most
// nA98 bytes, fn_80006444 reading in the background and fn_8010BFA0 counting what arrived, and
// copies them into the character's DynTex. Returns whether it is still busy.
u8 fn_8010BFE0(void) {
    TexEntry* pEntry;
    TexPalette* pPal;
    DynTex* pTex;
    u32 bReady;
    s32 nLen;

    if (lbl_80282488->n980 == 3) {
        if (lbl_80282488->b974 != 0) {
            lbl_80282488->n980 = 0;
            return 0;
        }
        return 1;
    }
    if (lbl_80282488->n980 == 0) {
        if (lbl_80282488->nA84 != 0) {
            lbl_80282488->pA88 = fn_8010B960();
            lbl_80282488->pA88->pfnA(lbl_80282488->pA88->pChar);
            if (DYNTEX_CHAR(lbl_80282488->p8)->hFile < 0) {
                lbl_80282488->pA88->pfnB(lbl_80282488->pA88->pChar);
                lbl_80282488->n980 = 0;
                lbl_80282488->pA88->bUsed = 0;
                return 0;
            }
        } else {
            return 0;
        }
    }
    fn_80007368();
    bReady = lbl_80282488->b974 != 0;
    fn_80007328();
    if (bReady) {
        pTex = DYNTEX_CHAR(lbl_80282488->pA88->p0)->p60;
        // Copy what the last read brought.
        if (lbl_80282488->n978 != 0) {
            Mem_cpy(lbl_80282488->p4, lbl_80282488->p0 + lbl_80282488->nA94, lbl_80282488->nA90);
            lbl_80282488->p4 += lbl_80282488->nA90;
            lbl_80282488->nA8C += lbl_80282488->nA90;
            lbl_80282488->nA94 = 0;
        }
        if (lbl_80282488->b975) {
            if (lbl_80282488->n980 == 1) {
                // The last texture is all in: flush it (and its palette) to the GPU.
                if (lbl_80282488->n978 != 0 && lbl_80282488->n970 > 0) {
                    fn_8010B1D4(pTex, lbl_80282488->aUses[lbl_80282488->n970 - 1].nC, NULL,
                                lbl_80282488->aUses[lbl_80282488->n970 - 1].p4,
                                lbl_80282488->aUses[lbl_80282488->n970 - 1].n8);
                    if (pTex->p0[lbl_80282488->aUses[lbl_80282488->n970 - 1].nC].n1C != 0) {
                        // port: EA passes two arguments fn_8010B2A8 ignores
                        ((void (*)(DynTex*, int, s16*, void*, s32))fn_8010B2A8)(
                            pTex, lbl_80282488->aUses[lbl_80282488->n970 - 1].nC, NULL,
                            lbl_80282488->aUses[lbl_80282488->n970 - 1].p4,
                            lbl_80282488->aUses[lbl_80282488->n970 - 1].n8);
                    }
                }
                if (lbl_80282488->n970 == lbl_80282488->n96C) {
                    // All done.
                    lbl_80282488->pA88->pfnB(lbl_80282488->pA88->pChar);
                    lbl_80282488->pA88->bUsed = 0;
                    lbl_80282488->n980 = 0;
                    return 0;
                }
                // The next texture: add it to the DynTex and work out the 2 KB-aligned read.
                pEntry = lbl_80282488->aUses[lbl_80282488->n970].pTex;
                lbl_80282488->n984 = 0;
                pPal = NULL;
                if (pEntry->nPalette != -1) {
                    pPal = &(*lbl_80282488->p8)->pC[pEntry->nPalette];
                }
                lbl_80282488->aUses[lbl_80282488->n970].nC =
                    fn_8010B338(DYNTEX_CHAR(lbl_80282488->pA88->p0)->p60, (DynTexObj*)pEntry,
                                (DynTexPalette*)pPal, NULL, NULL,
                                lbl_80282488->aUses[lbl_80282488->n970].p4,
                                lbl_80282488->aUses[lbl_80282488->n970].n8);
                lbl_80282488->p4 = pTex->p18 +
                    pTex->p4->p8[lbl_80282488->aUses[lbl_80282488->n970].nC].aBlocks[0].nOffset;
                lbl_80282488->u98C = DYNTEX_CHAR(lbl_80282488->p8)->n58 + pEntry->uPixels;
                lbl_80282488->u98C &= ~0x7FF;
                lbl_80282488->nA94 =
                    DYNTEX_CHAR(lbl_80282488->p8)->n58 + pEntry->uPixels - lbl_80282488->u98C;
                lbl_80282488->n988 = fn_8010B5F8((DynTexObj*)pEntry) << 4;
                if (pPal != NULL) {
                    lbl_80282488->n988 += fn_8010B664((DynTexPalette*)pPal) << 4;
                }
                lbl_80282488->n984 = lbl_80282488->n988 + lbl_80282488->nA94;
                lbl_80282488->n984 = (lbl_80282488->n984 + 0x7FF) & ~0x7FF;
                lbl_80282488->n970++;
            }
            lbl_80282488->n978 = 0;
            lbl_80282488->nA8C = 0;
            lbl_80282488->b975 = 0;
        }
        // Start the next read.
        nLen = lbl_80282488->n984 - lbl_80282488->n978;
        if (nLen > lbl_80282488->nA98) {
            nLen = lbl_80282488->nA98;
        }
        lbl_80282488->nA90 = nLen - lbl_80282488->nA94;
        if (lbl_80282488->nA8C + lbl_80282488->nA90 > lbl_80282488->n988) {
            lbl_80282488->nA90 = lbl_80282488->n988 - lbl_80282488->nA8C;
        }
        lbl_80282488->b974 = 0;
        fn_80006444(DYNTEX_CHAR(lbl_80282488->p8)->hFile, lbl_80282488->p0, nLen,
                    lbl_80282488->u98C + lbl_80282488->n978, fn_8010BFA0);
        return 1;
    }
    return 1;
}

// ---- end of sweep code ----

// Empty the job pool and the queue.
void fn_8010B7C0(void) {
    int i;

    lbl_80282488->nA84 = 0;
    for (i = 0; i < 10; i++) {
        lbl_80282488->aJobs[i].bUsed = 0;
        lbl_80282488->aJobs[i].pfnA = NULL;
        lbl_80282488->aJobs[i].pfnB = NULL;
        lbl_80282488->apQueue[i] = NULL;
    }
}

// The bytes of level nLevel of a texture: its size halved nLevel times, at its format's bits per
// pixel, rounded up to whole bytes.
u32 fn_8010B6AC(DynTexObj* pObj, int nLevel) {
    u32 nA = pObj->n3A;
    u32 nB = pObj->n38;
    int i;

    for (i = 0; i < nLevel; i++) {
        nA >>= 1;
        nB >>= 1;
    }
    return (nA * nB * fn_8010C458(pObj->n40) + 7) >> 3;
}

// The bytes of all of a texture's levels.
s32 fn_8010B754(DynTexObj* pObj) {
    int i;
    s32 nBytes = 0;

    for (i = 0; i < pObj->n41; i++) {
        nBytes += fn_8010B6AC(pObj, i);
    }
    return nBytes;
}

// The same level's size in 16-byte units, rounded up.
u32 fn_8010B548(DynTexObj* pObj, int nLevel) {
    u32 nA = pObj->n3A;
    u32 nB = pObj->n38;
    int i;

    for (i = 0; i < nLevel; i++) {
        nA >>= 1;
        nB >>= 1;
    }
    return (((nA * nB * fn_8010C458(pObj->n40) + 7) >> 3) + 15) >> 4;
}

// The 16-byte units of all of a texture's levels.
s32 fn_8010B5F8(DynTexObj* pObj) {
    int i;
    s32 nUnits = 0;

    for (i = 0; i < pObj->n41; i++) {
        nUnits += fn_8010B548(pObj, i);
    }
    return nUnits;
}

// Bits per pixel of texture format nFormat (0 for the formats it does not list).
int fn_8010C458(int nFormat) {
    int nBits = 0;

    switch (nFormat) {
    case 0:
    case 8:
    case 14:
        nBits = 4;
        break;
    case 1:
    case 9:
        nBits = 8;
        break;
    case 2:
    case 4:
    case 5:
        nBits = 16;
        break;
    case 6:
        nBits = 32;
        break;
    }
    return nBits;
}

// Run the jobs until the queue is empty and nothing is left to do, a frame at a time.
void fn_8010BF68(void) {
    while (!fn_8010BF3C()) {
        fn_800B7490();
        fn_8010BFE0();
        fn_80007254();
    }
}
