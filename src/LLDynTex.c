// LLDynTex.c (EA's name, from its asserts): textures whose pixels the game rewrites while they are
// shown; the menu golfer (FEgolferanim.c) drives them, and char_tex_manager.c puts the user logos
// in them. Partly decompiled.

#include "engine.h"
#include "gx.h"
#include "core/startup.h"
#include "lldyntex.h"

// ---- sweep code (not yet cleaned up) ----

void fn_8001052C(s16 n);
void fn_8010A668(DynTex* pTex);
void* fn_8010A780(DynTex* pTex);
void fn_8010B098(void* arg0);
int fn_8010C458(int nFormat);
s32 fn_8010B664(void* arg0);
void fn_8000FBAC();

// ---- end of sweep code ----

void fn_8010B7C0(void);
void fn_8010A930(DynTexObj* pObj, u8* pBuf, void* p, s32 n);
DynTexJob* fn_8010B960(void);
u8   fn_8010BF3C(void);
u8   fn_8010BFE0(void);
void fn_80007254(void);                 // LLDisp_Gc.c

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

// Free a dynamic texture.
void fn_8010A668(DynTex* pTex) {
    fn_8001052C(pTex->n1C);
    fn_80009E70(pTex->p18);
    fn_80009E70(pTex);
}

void* fn_8010A780(DynTex* pTex) {
    return pTex->p4;
}

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

// Fills pEntry for pObj and its palette pPal (if any): the id, each level's bytes and the
// palette's, each rounded up to 16. Returns them all added up.
s32 fn_8010B0C0(DynTexObj* pObj, DynTexPalObj* pPal, DynTexEntry* pEntry) {
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

void fn_8010B098(void* arg0) {
    if (arg0 != NULL) {
        (*(s32*)((u8*)(arg0) + 8)) = 0;
        (*(s16*)((u8*)((*(void**)((u8*)(arg0) + 4))) + 2)) = 0;
        (*(s16*)((u8*)((*(void**)((u8*)(arg0) + 4))) + 4)) = 0;
        (*(s32*)((u8*)(arg0) + 0x14)) = 0;
    }
}

s32 fn_8010B664(void* arg0) {
    s32 var_r3;

    if ((s16) (*(s16*)((u8*)(arg0) + 8)) > 0x10) {
        var_r3 = 0x40;
        if (fn_8010C458((*(s16*)((u8*)(arg0) + 0xA))) == 0x10) {
            return 0x20;
        }
        /* Duplicate return node #4. Try simplifying control flow for better match */
        return var_r3;
    }
    var_r3 = 4;
    return var_r3;
}

void fn_8010BC64(u8* p) {
    fn_8000FBAC(*(s32*)(p + 0x4));
}

void fn_8010BC88(void* p) {
    lbl_80282488->p8 = p;
}

void fn_8010BEC4(void) {
    lbl_80282488->n96C = 0;
}

// Whether the queue is empty and nothing is left to do.
u8 fn_8010BF3C(void) {
    int bDone = 0;

    if (lbl_80282488->nA84 == 0 && lbl_80282488->n980 == 0) {
        bDone = 1;
    }
    return bDone;
}

void fn_8010BFA0(s32 n) {
    lbl_80282488->n97C = n;
    lbl_80282488->n978 = lbl_80282488->n978 + n;
    lbl_80282488->b974 = 1;
    if (lbl_80282488->n978 >= lbl_80282488->n984) {
        lbl_80282488->b975 = 1;
    }
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
