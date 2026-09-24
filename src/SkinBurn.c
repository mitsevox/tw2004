// SkinBurn.c (EA's name, from its asserts): "burns" a skin: drops the unused parts of a skin's data,
// renumbers what is left and packs it into one allocation. Only partly decompiled.
#include "game_types.h"
#include "platform.h"
#include "engine.h"
#include "charstate.h"

s32   fn_801275F0(const void* p, s32 nCount, s32 nSize, s32 nAlign);
void* fn_80127614(u8* pBase, s32* pOffset, const void* pSrc, s32 nSize, s32 nAlign);
void* fn_801276A8(u8* pBase, s32* pOffset, s32 nSize, s32 nAlign);

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

// Renumbers the model's p44 entries with pBurn->a30 and packs the kept ones to the front,
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
        nNew = pBurn->a30[pSrc->n0];
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

// Drops the bits (Skin.p10CC) no option uses: keeps the first n14 and those fn_800CD9EC marks,
// renumbers them 0, 1, 2... (lbl_802825A8 holds the new number of each old one) and packs their
// SkinModel.p54 entries to the front.
void fn_801272B4(Skin* pSkin) {
    SkinModel* pModel;
    u32* aBits;
    s32* aOld;
    s32* aNew;
    s32 nKept;
    s32 nBits;
    s32 nEntries;
    s32 i;
    s32* pOld;
    s32* pNew;
    u32* pSaved;
    s32 nFixed;
    s32 j;
    s32 n;
    SkinModel44* pEntry;

    pModel = pSkin->pModel;
    nBits = pModel->n50;
    nEntries = pModel->n40;
    if (nBits == 0) {
        return;
    }
    aBits = fn_80009B34((nBits + 31) / 32 * sizeof(u32), 1, 16, "SkinBurn.c", 205);
    fn_8001E938(aBits, nBits);
    aOld = fn_80009B34(nBits * sizeof(s32), 1, 16, "SkinBurn.c", 209);
    aNew = fn_80009B34(nBits * sizeof(s32), 1, 16, "SkinBurn.c", 210);
    memset(aOld, -1, nBits * sizeof(s32));
    memset(aNew, -1, nBits * sizeof(s32));

    // The first n14 bits always stay.
    nFixed = pModel->n14;
    if (nFixed > pModel->n50) {
        nFixed = pModel->n50;
    }
    for (i = 0; i < nFixed; i++) {
        fn_8001EA34(aBits, i);
    }

    // Mark the bits the options use, in aBits instead of the skin's own array.
    pSaved = pSkin->p10CC;
    pSkin->p10CC = aBits;
    fn_800CD9EC(pSkin);
    pSkin->p10CC = pSaved;

    pOld = aOld;
    pNew = aNew;
    nKept = 0;
    lbl_802825A8 = aNew;
    for (j = 0; j < nBits; j++) {
        if (fn_8001E9CC(aBits, j)) {
            *pOld++ = j;
            *pNew = nKept++;
        }
        pNew++;
    }

    pOld = aOld;
    for (j = 0; j < nKept; j++) {
        if (j != *pOld) {
            memcpy(&pModel->p54[j], &pModel->p54[*pOld], sizeof(SkinModel54));
        }
        pOld++;
    }

    for (i = 0; i < pModel->pDesc->n58; i++) {
        fn_80127218(pSkin, i);
    }

    for (i = 0; i < nEntries; i++) {
        pEntry = &pModel->p44[i];
        if (nKept < pEntry->n8) {
            pEntry->n8 = nKept;
        }
        n = pEntry->n8;
        for (j = 0; j < n; j++) {
            pEntry->p4[j] = j;
        }
        pSkin->pModel->pDesc->p34[i].n8 = n;
        pSkin->pModel->pDesc->p34[i].nSize = n << 6;
    }

    pModel->n50 = nKept;
    fn_80009E70(aBits);
    fn_80009E70(aOld);
    fn_80009E70(aNew);
}

// Packs the skin's model and its arrays into one allocation and frees the old one. Each
// SkinModel44's p4 array goes into one shared block of indexes at the end.
void fn_801276E4(Skin* pSkin) {
    SkinModel* pOld;
    s32 nOffset;
    s32 nSize;
    s32 nIndexes;
    s32 i;
    u8* pBase;
    SkinModel* pNew;
    s32* aIndexes;
    s32 nFirst;
    SkinModel44* pEntry;
    s32 j;
    s32 n;

    pOld = pSkin->pModel;
    // EA passes &pOld where an array is expected: never NULL, so the model always counts
    nSize = fn_801275F0(&pOld, 1, sizeof(SkinModel), 16);
    nSize += fn_801275F0(pOld->p34, pOld->n14, 0x20, 16);
    nSize += fn_801275F0(pOld->p38, 1, 0x50, 16);
    nSize += fn_801275F0(pOld->p3C, pOld->n0C, 0x50, 16);
    nSize += fn_801275F0(pOld->p54, pOld->n50, sizeof(SkinModel54), 16);
    nSize += fn_801275F0(pOld->p44, pOld->n40, sizeof(SkinModel44), 16);
    nIndexes = 0;
    for (i = 0; i < pOld->n40; i++) {
        nIndexes += pOld->p44[i].n8;
    }
    nSize += fn_801275F0(&pOld, nIndexes, sizeof(s32), 16);

    pBase = fn_80009B34(nSize, 2, 16, "SkinBurn.c", 434);
    nOffset = 0;
    pNew = fn_80127614(pBase, &nOffset, pOld, sizeof(SkinModel), 16);
    pNew->n08 = nSize;
    pNew->p34 = fn_80127614(pBase, &nOffset, pOld->p34, pOld->n14 * 0x20, 16);
    pNew->p38 = fn_80127614(pBase, &nOffset, pOld->p38, 0x50, 16);
    pNew->p3C = fn_80127614(pBase, &nOffset, pOld->p3C, pOld->n0C * 0x50, 16);
    pNew->p54 = fn_80127614(pBase, &nOffset, pOld->p54, pOld->n50 * sizeof(SkinModel54), 16);
    pNew->p44 = fn_80127614(pBase, &nOffset, pOld->p44, pOld->n40 * sizeof(SkinModel44), 16);
    aIndexes = fn_801276A8(pBase, &nOffset, nIndexes * sizeof(s32), 16);

    nFirst = 0;
    for (i = 0; i < pNew->n40; i++) {
        pEntry = &pNew->p44[i];
        n = pEntry->n8;
        for (j = 0; j < n; j++) {
            aIndexes[nFirst + j] = pEntry->p4[j];
        }
        pEntry->p4 = &aIndexes[nFirst];
        nFirst += pEntry->n8;
    }
    pSkin->pModel = pNew;
    fn_80009E70(pOld);
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
