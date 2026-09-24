// SkinBurn.c (EA's name, from its asserts): "burns" a skin: drops the unused parts of a skin's data,
// renumbers what is left and packs it into one allocation. Only partly decompiled.
#include "game_types.h"
#include "platform.h"
#include "engine.h"
#include "charstate.h"

void fn_801272B4(Skin* pSkin);
void fn_801276E4(Skin* pSkin);

char lbl_802819A8[8] = "";      // the folder the signature file is looked for in

// Checks that the signature file "Signat.sig" is on the disc (GoEntry.c calls it at start-up);
// without it the game stops.
void fn_801270F0(void) {
    char szPath[256];   // the size is not known (the frame leaves room for 256 bytes)
    int hFile;

    sprintf(szPath, "%sSignat.sig", lbl_802819A8);
    hFile = fn_800060E0(szPath);
    if (hFile < 0) {
        // EA: stops the game on purpose with a write to address 0 (undefined in C; a port should
        // abort here instead)
        *(volatile s32*)0 = 0;
    }
    fn_8000633C(hFile);
}

// Renumbers the model's p44 entries with pBurn's table and packs the kept ones to the front,
// dropping those whose new number is -1.
void fn_80127140(Skin* pSkin, HwsBurn* pBurn) {
    SkinModel44* pEntries;
    SkinModel44* pSrc;
    SkinModel44* pDst;
    s32 i;
    s32 nCount;
    s32 nKept;
    s32 nNew;

    nKept = 0;
    i = 0;
    pEntries = pSkin->pModel->p44;
    nCount = pSkin->pModel->n40;
    pSrc = pEntries;
    pDst = pEntries;
    for (; i < nCount; i++) {
        nNew = pBurn->p30[pSrc->n0];
        if (nNew != -1) {
            if (pSrc != pDst) {
                memcpy(pDst, pSrc, sizeof(SkinModel44));
            }
            pDst->n0 = nNew;
            pDst++;
            nKept++;
        }
        pSrc++;
    }
    pSkin->pModel->n40 = nKept;
}

// Renumbers a mesh's bits with lbl_802825A8 (pSkin is unused).
void fn_801271E0(Skin* pSkin, SkinMesh* pMesh) {
    SkinMeshBit* pBit = pMesh->pBits;
    s32 i;

    for (i = 0; i < pMesh->n8; i++) {
        pBit->nBit = lbl_802825A8[pBit->nBit];
        pBit++;
    }
}

// Renumbers the bits of the meshes of the skin's p5C entry n that have flags 1 and 0x10.
void fn_80127218(Skin* pSkin, s32 n) {
    SkinIterArgs args;
    u8 aBuf[0x48];      // the iterator's buffer; its size is not known
    SkinIter* pIter;
    SkinMesh* pMesh;

    args.pDesc = pSkin->pModel->pDesc;
    args.n = n;
    pIter = fn_80113B34(aBuf, &args);
    while (fn_800CEEC0(pIter)) {
        pMesh = fn_800CEEF4(pIter);
        if ((pMesh->uFlags & 1) && (pMesh->uFlags & 0x10)) {
            fn_801271E0(pSkin, pMesh);
        }
        fn_800CEEC8(pIter);
    }
    fn_80113BAC(pIter);
}

// The bytes nCount items of nSize take, rounded up to nAlign (a power of two); 0 when there is no
// array (p is NULL).
s32 fn_801275F0(const void* p, s32 nCount, s32 nSize, s32 nAlign) {
    s32 n = 0;

    if (p != NULL) {
        n = (nCount * nSize + (nAlign - 1)) & ~(nAlign - 1);
    }
    return n;
}

// Copies nSize bytes of pSrc to pBase + *pOffset and moves *pOffset past them, rounded up to
// nAlign. Gives where the copy went (NULL, and nothing done, when pSrc is NULL).
void* fn_80127614(u8* pBase, s32* pOffset, const void* pSrc, s32 nSize, s32 nAlign) {
    void* pDst = NULL;

    if (pSrc != NULL) {
        pDst = pBase + *pOffset;
        memcpy(pDst, pSrc, nSize);
        *pOffset += nSize;
        *pOffset = (*pOffset + (nAlign - 1)) & ~(nAlign - 1);
    }
    return pDst;
}

// Takes nSize bytes at pBase + *pOffset the same way, without copying anything.
void* fn_801276A8(u8* pBase, s32* pOffset, s32 nSize, s32 nAlign) {
    void* p = NULL;

    if (nSize != 0) {
        p = pBase + *pOffset;
        *pOffset += nSize;
        *pOffset = (*pOffset + (nAlign - 1)) & ~(nAlign - 1);
    }
    return p;
}

void fn_80127B10(Skin* pSkin, HwsBurn* pBurn) {
    fn_80127140(pSkin, pBurn);
    fn_801272B4(pSkin);
    fn_801276E4(pSkin);
}

// The callback fn_80127B98 hands hwsBurn.c: copies an entry and clears its bit 1 and n16.
void fn_80127B4C(Skin* pSkin, SkinDesc14* pEntry) {
    fn_800CE224(pSkin, pEntry, NULL, NULL, 0);
    pEntry->u08 &= ~2;
    pEntry->n16 = 0;
}
