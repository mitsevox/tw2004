// hwsBurn.c (EA's name, from its asserts): "burns" a skin description into one block: an HwsBurn
// (fn_801104AC) records which parts, variants and options are chosen and which bits of the
// description they use, and fn_80111850 copies what they use into one new block.

#include "engine.h"
#include "charstate.h"

SkinIter* fn_80113A9C(u8* pBuf, SkinIterArgs* pArgs);   // hwsRender_Gc.c
void  fn_80113B14(SkinIter* pIter);                     // hwsRender_Gc.c: ends the iterator
s32   fn_800CF104(SkinDesc* pDesc, u64 uId);
void  fn_80110A38(HwsBurn* pBurn, int n);
SkinDesc* fn_80111850(HwsBurn* pBurn);

// A burn of pDesc: its tables sized from the description, the bit sets cleared, and no variant or
// option chosen yet (-1).
HwsBurn* fn_801104AC(SkinDesc* pDesc) {
    HwsBurn* pBurn = fn_80009B34(sizeof(HwsBurn), 1, 16, "hwsBurn.c", 46);
    int i;
    int nBytes;

    memset(pBurn, 0, sizeof(HwsBurn));
    pBurn->pDesc = pDesc;
    pBurn->nParts = pDesc->nParts;
    pBurn->n18 = fn_8011C850(pDesc);
    pBurn->n20 = pDesc->n2C;
    pBurn->n34 = pDesc->n40;
    pBurn->n4C = pDesc->n38;
    pBurn->n60 = pDesc->n10;
    pBurn->n70 = pDesc->n88;
    if (pBurn->nParts != 0) {
        pBurn->aVariant = fn_80009B34(pBurn->nParts * 4, 1, 16, "hwsBurn.c", 61);
        memset(pBurn->aVariant, 0, pBurn->nParts * 4);
        pBurn->aOption = fn_80009B34(pBurn->nParts * 4, 1, 16, "hwsBurn.c", 64);
        for (i = 0; i < pBurn->nParts; i++) {
            pBurn->aOption[i] = -1;
            pBurn->aVariant[i] = -1;
        }
    }
    if (pBurn->n18 != 0) {
        pBurn->a1C = fn_80009B34(pBurn->n18 * 4, 1, 16, "hwsBurn.c", 76);
        memset(pBurn->a1C, 0, pBurn->n18 * 4);
    }
    if (pBurn->n20 != 0) {
        pBurn->p28 = fn_80009B34((pBurn->n20 + 31) / 32 * 4, 1, 16, "hwsBurn.c", 83);
        fn_8001E938(pBurn->p28, pBurn->n20);
        pBurn->a2C = fn_80009B34(pBurn->n20 * 4, 1, 16, "hwsBurn.c", 87);
        pBurn->a30 = fn_80009B34(pBurn->n20 * 4, 1, 16, "hwsBurn.c", 88);
    }
    if (pBurn->n34 != 0) {
        nBytes = (pBurn->n34 + 31) / 32 * 4;
        pBurn->p3C = fn_80009B34(nBytes, 1, 16, "hwsBurn.c", 94);
        pBurn->p40 = fn_80009B34(nBytes, 1, 16, "hwsBurn.c", 95);
        fn_8001E938(pBurn->p3C, pBurn->n34);
        fn_8001E938(pBurn->p40, pBurn->n34);
        pBurn->a44 = fn_80009B34(pBurn->n34 * 4, 1, 16, "hwsBurn.c", 100);
        pBurn->a48 = fn_80009B34(pBurn->n34 * 4, 1, 16, "hwsBurn.c", 101);
    }
    if (pBurn->n4C != 0) {
        pBurn->p54 = fn_80009B34((pBurn->n4C + 31) / 32 * 4, 1, 16, "hwsBurn.c", 107);
        fn_8001E938(pBurn->p54, pBurn->n4C);
        pBurn->a58 = fn_80009B34(pBurn->n4C * 4, 1, 16, "hwsBurn.c", 111);
        pBurn->a5C = fn_80009B34(pBurn->n4C * 4, 1, 16, "hwsBurn.c", 112);
    }
    if (pBurn->n60 != 0) {
        pBurn->a64 = fn_80009B34(pBurn->n60 * sizeof(SkinDesc14), 1, 16, "hwsBurn.c", 120);
    }
    if (pBurn->n70 != 0) {
        pBurn->p78 = fn_80009B34((pBurn->n70 + 31) / 32 * 4, 1, 16, "hwsBurn.c", 127);
        fn_8001E938(pBurn->p78, pBurn->n70);
        pBurn->a7C = fn_80009B34(pBurn->n70 * 4, 1, 16, "hwsBurn.c", 131);
        pBurn->a80 = fn_80009B34(pBurn->n70 * 4, 1, 16, "hwsBurn.c", 132);
    }
    return pBurn;
}

// Free a burn and its tables.
void fn_801108B0(HwsBurn* pBurn) {
    if (pBurn->aVariant != NULL) {
        fn_80009E70(pBurn->aVariant);
    }
    if (pBurn->aOption != NULL) {
        fn_80009E70(pBurn->aOption);
    }
    if (pBurn->a1C != NULL) {
        fn_80009E70(pBurn->a1C);
    }
    if (pBurn->p28 != NULL) {
        fn_80009E70(pBurn->p28);
    }
    if (pBurn->a2C != NULL) {
        fn_80009E70(pBurn->a2C);
    }
    if (pBurn->a30 != NULL) {
        fn_80009E70(pBurn->a30);
    }
    if (pBurn->p3C != NULL) {
        fn_80009E70(pBurn->p3C);
    }
    if (pBurn->p40 != NULL) {
        fn_80009E70(pBurn->p40);
    }
    if (pBurn->a44 != NULL) {
        fn_80009E70(pBurn->a44);
    }
    if (pBurn->a48 != NULL) {
        fn_80009E70(pBurn->a48);
    }
    if (pBurn->p54 != NULL) {
        fn_80009E70(pBurn->p54);
    }
    if (pBurn->a58 != NULL) {
        fn_80009E70(pBurn->a58);
    }
    if (pBurn->a5C != NULL) {
        fn_80009E70(pBurn->a5C);
    }
    if (pBurn->a64 != NULL) {
        fn_80009E70(pBurn->a64);
    }
    if (pBurn->p78 != NULL) {
        fn_80009E70(pBurn->p78);
    }
    if (pBurn->a7C != NULL) {
        fn_80009E70(pBurn->a7C);
    }
    if (pBurn->a80 != NULL) {
        fn_80009E70(pBurn->a80);
    }
    fn_80009E70(pBurn);
}

void fn_801109F0(HwsBurn* pBurn, void (*pfn)(Skin* pSkin, SkinDesc14* pEntry), Skin* pSkin) {
    pBurn->pfn68 = pfn;
    pBurn->pSkin = pSkin;
}

void fn_801109FC(HwsBurn* pBurn, int nPart, s32 nVariant) {
    pBurn->aVariant[nPart] = nVariant;
}

void fn_80110A0C(HwsBurn* pBurn, int nPart, s32 nOption) {
    pBurn->aOption[nPart] = nOption;
}

void fn_80110A1C(HwsBurn* pBurn, HwsOverrideTable* pOverride) {
    pBurn->pOverride = pOverride;
}

void fn_80110A24(HwsBurn* pBurn, int n) {
    pBurn->a1C[n] = 1;
}

// Marks SkinDesc.p44 entry n used: its bits, its meshes' bits, the morph-target entries after it
// (those whose a1C flag is clear, each marked the same way) and its SkinDesc.p3C entries.
void fn_80110A38(HwsBurn* pBurn, int n) {
    SkinIterArgs args;
    SkinMeshIter iterBuf;
    SkinIter* pIter;
    SkinDesc44* pEntry;
    SkinDesc28* p28;
    s32 nCount;
    int nLast;
    int i;

    fn_8001EA34(pBurn->p3C, n);
    fn_8001EA34(pBurn->p40, n);
    args.pDesc = pBurn->pDesc;
    args.n = n;
    pIter = fn_80113910((u8*)&iterBuf, &args);
    while (fn_800CEEC0(pIter)) {
        fn_8001EA34(pBurn->p28, fn_800CEEFC(pIter));
        fn_800CEEC8(pIter);
    }
    fn_80113A7C(pIter);

    pEntry = &pBurn->pDesc->p44[n];
    if (pEntry->u24 & 2) {
        nLast = 0;
        for (i = 0; i < pEntry->n14; i++) {
            if (pBurn->a1C[pEntry->n18 + i] == 0) {
                fn_80110A38(pBurn, pEntry->n10 + i);
                nLast = i + 1;
            }
        }
        for (i = 0; i < nLast; i++) {
            fn_8001EA34(pBurn->p40, pEntry->n10 + i);
        }
    }

    nCount = 1;
    if (pEntry->nC >= 0) {
        p28 = &pBurn->pDesc->p28[pEntry->nC];
        nCount = 0;
        for (i = 0; p28->n0 > i; i++) {
            nCount += p28->a8[i].n1;
        }
    }
    for (i = 0; i < nCount; i++) {
        fn_8001EA34(pBurn->p54, pEntry->n0 + i);
    }
}

// Everything the SkinDesc.p44 entries of SkinDesc.p5C entry n use (fn_80110A38 on each).
void fn_80110C88(HwsBurn* pBurn, int n) {
    SkinIterArgs args;
    u8 aBuf[0x48];                      // size unknown
    SkinIter* pIter;
    int nMesh;

    args.pDesc = pBurn->pDesc;
    args.n = n;
    for (pIter = fn_80113A9C(aBuf, &args); fn_800CEEC0(pIter); fn_800CEEC8(pIter)) {
        nMesh = fn_800CEEFC(pIter);
        fn_80110A38(pBurn, nMesh);
    }
    fn_80113B14(pIter);
}

// What variant nVariant of part nPart uses: its chosen options (all without one), and the
// options its links pick on other parts' chosen variants.
void fn_80110D10(HwsBurn* pBurn, int nPart, int nVariant) {
    int i;
    SkinLink* pLink;
    SkinVariant* pOther;
    s32 nOption;
    s32 nFirst;
    int nLinked;
    s32 nFirstLink;

    nOption = pBurn->aOption[nPart];
    nFirst = pBurn->pDesc->pVariants[nVariant].nFirstOption;
    for (i = 0; i < pBurn->pDesc->pVariants[nVariant].nOptions; i++) {
        if (nOption == -1 || nOption == i) {
            fn_80110C88(pBurn, nFirst + i);
        }
    }
    nFirstLink = pBurn->pDesc->pVariants[nVariant].nFirstLink;
    for (i = 0; i < pBurn->pDesc->pVariants[nVariant].nLinks; i++) {
        pLink = &pBurn->pDesc->pLinks[nFirstLink + i];
        nLinked = fn_800CF104(pBurn->pDesc, pLink->uPart);
        if (nLinked != -1) {
            int j;
            s32 nFirstVariant;
            s32 nChosen;

            nOption = pLink->nOption;
            nChosen = pBurn->aVariant[nLinked];
            nFirstVariant = pBurn->pDesc->pParts[nLinked].nFirst;
            for (j = 0; j < pBurn->pDesc->pParts[nLinked].nVariants; j++) {
                if ((nChosen == -1 || nChosen == j) && nOption >= 0) {
                    pOther = &pBurn->pDesc->pVariants[nFirstVariant + j];
                    if (nOption < pOther->nOptions) {
                        fn_80110C88(pBurn, pOther->nFirstOption + nOption);
                    }
                }
            }
        }
    }
}

// The bytes nCount items of nSize take, rounded up to nAlign (a power of two); 0 without p.
s32 fn_80110E74(void* p, s32 nCount, s32 nSize, s32 nAlign) {
    s32 n = 0;

    if (p != NULL) {
        n = nCount * nSize;
        n = nAlign + n;
        n = (n - 1) & ~(nAlign - 1);
    }
    return n;
}

// Copy nSize bytes of pSrc to pBase + *pOffset and move *pOffset past them, rounded up to nAlign.
// Gives where they went (NULL without pSrc).
void* fn_80110E98(u8* pBase, s32* pOffset, void* pSrc, s32 nSize, s32 nAlign) {
    void* pDst = NULL;
    s32 n;

    if (pSrc != NULL) {
        pDst = pBase + *pOffset;
        memcpy(pDst, pSrc, nSize);
        *pOffset += nSize;
        n = nAlign + *pOffset;
        n = (n - 1) & ~(nAlign - 1);
        *pOffset = n;
    }
    return pDst;
}

// List the bits of p40 that are set (a44) and give each its place in the list (a48). nAlign is
// unused (fn_80111850 passes it, as to the other sizing steps).
s32 fn_80110F2C(HwsBurn* pBurn, s32 nAlign) {
    int i;
    int nBits = pBurn->pDesc->n40;
    int n = 0;

    for (i = 0; i < nBits; i++) {
        if (fn_8001E9CC(pBurn->p40, i)) {
            pBurn->a44[n] = i;
            pBurn->a48[i] = n;
            n++;
        }
    }
    pBurn->n38 = n;
    return 0;
}

// Copies of the SkinDesc.p44 entries fn_80110F2C listed (a44) at pBase + *pOffset, renumbered
// for the burn: an entry whose p3C bit is clear loses its meshes, and the morph targets stop at
// the first one not kept. NULL when none are listed.
SkinDesc44* fn_80110FB4(HwsBurn* pBurn, u8* pBase, s32* pOffset, s32 nAlign) {
    int n = pBurn->n38;
    SkinDesc* pDesc;
    SkinDesc44* aOut;
    SkinDesc44* pOut;
    s32 nEntry;
    int j;
    int nMorphs;
    int i;
    s32 nEnd;

    if (n == 0) {
        return NULL;
    }
    aOut = (SkinDesc44*)(pBase + *pOffset);
    pDesc = pBurn->pDesc;
    for (i = 0; i < n; i++) {
        nEntry = pBurn->a44[i];
        pOut = fn_80110E98(pBase, pOffset, &pDesc->p44[nEntry], sizeof(SkinDesc44), 1);
        if (!fn_8001E9CC(pBurn->p3C, nEntry)) {
            pOut->n8 = 0;
            pOut->n0 = 0;
            pOut->u24 &= ~2;
        } else {
            pOut->n0 = pBurn->a5C[pOut->n0];
        }
        if (pOut->u24 & 2) {
            nMorphs = pOut->n14;
            for (j = 0; j < nMorphs; j++) {
                if (!fn_8001E9CC(pBurn->p40, pOut->n10 + j)) {
                    break;
                }
            }
            pOut->n14 = j;
            pOut->n10 = pBurn->a48[pOut->n10];
        }
        if (pOut->u24 & 4) {
            pOut->n10 = pBurn->a48[pOut->n10];
        }
    }
    nEnd = nAlign + *pOffset;
    nEnd = (nEnd - 1) & ~(nAlign - 1);
    *pOffset = nEnd;
    return aOut;
}

// The bytes the SkinDesc.p28 blocks take, each rounded up to nAlign.
s32 fn_80111310(HwsBurn* pBurn, s32 nAlign) {
    SkinDesc* pDesc = pBurn->pDesc;
    int i;
    int n = pDesc->n24;
    s32 nBytes = 0;

    for (i = 0; i < n; i++) {
        nBytes += fn_80110E74(pDesc->p28[i].p10, pDesc->p28[i].n14, 1, nAlign);
    }
    return nBytes;
}

// Copy SkinDesc.p28 and each entry's block to pBase + *pOffset (see fn_80110E98).
SkinDesc28* fn_80111384(HwsBurn* pBurn, u8* pBase, s32* pOffset, s32 nAlign) {
    SkinDesc* pDesc = pBurn->pDesc;
    int i;
    int n = pDesc->n24;
    SkinDesc28* aCopy = fn_80110E98(pBase, pOffset, pDesc->p28, n * sizeof(SkinDesc28), nAlign);

    for (i = 0; i < n; i++) {
        aCopy[i].p10 = fn_80110E98(pBase, pOffset, pDesc->p28[i].p10, pDesc->p28[i].n14, nAlign);
    }
    return aCopy;
}

// List the bits of p54 that are set (a58) and give each its place in the list (a5C). nAlign is
// unused (see fn_80110F2C).
s32 fn_80111424(HwsBurn* pBurn, s32 nAlign) {
    int i;
    int nBits = pBurn->pDesc->n38;
    int n = 0;

    for (i = 0; i < nBits; i++) {
        if (fn_8001E9CC(pBurn->p54, i)) {
            pBurn->a58[n] = i;
            pBurn->a5C[i] = n;
            n++;
        }
    }
    pBurn->n50 = n;
    return 0;
}

// The SkinDesc.p3C entries of the listed bits of p54, each through a30 (-1 stays -1), at
// pBase + *pOffset. NULL when none are listed.
s32* fn_801114AC(HwsBurn* pBurn, u8* pBase, s32* pOffset, s32 nAlign) {
    SkinDesc* pDesc = pBurn->pDesc;
    int n = pBurn->n50;
    s32* aOut;
    s32 v;
    int i;
    s32 nEnd;

    if (n == 0) {
        return NULL;
    }
    aOut = (s32*)(pBase + *pOffset);
    *pOffset += n * 4;
    nEnd = nAlign + *pOffset;
    nEnd = (nEnd - 1) & ~(nAlign - 1);
    *pOffset = nEnd;
    for (i = 0; i < n; i++) {
        v = pBurn->a58[i];
        v = pDesc->p3C[v];
        if (v != -1) {
            v = pBurn->a30[v];
        }
        aOut[i] = v;
    }
    return aOut;
}

// Copy SkinDesc.p6C, each entry moved to its place in the a44 list.
s32* fn_80111540(HwsBurn* pBurn, u8* pBase, s32* pOffset, s32 nAlign) {
    SkinDesc* pDesc = pBurn->pDesc;
    int n = pDesc->n68;
    s32* aCopy = fn_80110E98(pBase, pOffset, pDesc->p6C, n * 4, nAlign);
    int i;

    for (i = 0; i < n; i++) {
        if (aCopy[i] >= 0) {
            aCopy[i] = pBurn->a48[aCopy[i]];
        }
    }
    return aCopy;
}

// List the bits of p28 that are set (a2C, a30) and give the bytes the meshes they stand for take
// (each rounded up to nAlign; meshes with flag 0x400000 left out).
s32 fn_80111124(HwsBurn* pBurn, s32 nAlign) {
    SkinDesc* pDesc = pBurn->pDesc;
    int i;
    int nBits = pDesc->n2C;
    int n;
    s32 nBytes;

    nBytes = 0;
    n = 0;

    for (i = 0; i < nBits; i++) {
        if (fn_8001E9CC(pBurn->p28, i)) {
            pBurn->a2C[n] = i;
            pBurn->a30[i] = n;
            n++;
            if (!(pDesc->p34[i].uFlags & 0x400000)) {
                nBytes += fn_80110E74(pDesc->p34[i].pBits, pDesc->p34[i].nSize, 1, nAlign);
            }
        }
    }
    pBurn->n24 = n;
    return nBytes;
}

// Copies of the meshes fn_80111124 listed at pBase + *pOffset, then each one's data after them
// (from the override table when it has the mesh). NULL when none are listed.
SkinMesh* fn_801111E8(HwsBurn* pBurn, u8* pBase, s32* pOffset, s32 nAlign) {
    SkinDesc* pDesc = pBurn->pDesc;
    SkinMesh* aOut;
    void* pSrc;
    s32 nMesh;
    int i;
    int n = pBurn->n24;
    s32 nEnd;

    if (n == 0) {
        return NULL;
    }
    aOut = (SkinMesh*)(pBase + *pOffset);
    *pOffset += n * sizeof(SkinMesh);
    nEnd = nAlign + *pOffset;
    nEnd = (nEnd - 1) & ~(nAlign - 1);
    *pOffset = nEnd;
    for (i = 0; i < n; i++) {
        nMesh = pBurn->a2C[i];
        memcpy(&aOut[i], &pDesc->p34[nMesh], sizeof(SkinMesh));
        if (aOut[i].pBits != NULL) {
            if (pBurn->pOverride != NULL && nMesh < pBurn->pOverride->nMeshes &&
                pBurn->pOverride->apMesh[nMesh] != NULL) {
                pSrc = pBurn->pOverride->apMesh[nMesh];
            } else {
                pSrc = pDesc->p34[nMesh].pBits;
            }
            aOut[i].pBits = fn_80110E98(pBase, pOffset, pSrc, pDesc->p34[nMesh].nSize, nAlign);
        }
    }
    return aOut;
}

// Mark the SkinDesc.p8C entries the a64 entries use (p78) and list them (a7C, a80). nAlign is
// unused (see fn_80110F2C).
s32 fn_80111658(HwsBurn* pBurn, s32 nAlign) {
    int nEntries = pBurn->n60;
    int nBits = pBurn->n70;
    int i;
    int j;
    int n;

    fn_8001E938(pBurn->p78, nBits);
    for (i = 0; i < nEntries; i++) {
        if (pBurn->a64[i].n18 >= 0 && pBurn->a64[i].n18 < nBits) {
            fn_8001EA34(pBurn->p78, pBurn->a64[i].n18);
        }
    }
    n = 0;
    for (j = 0; j < nBits; j++) {
        if (fn_8001E9CC(pBurn->p78, j)) {
            pBurn->a7C[n] = j;
            pBurn->a80[j] = n;
            n++;
        }
    }
    pBurn->n74 = n;
    return 0;
}

// Copy the listed SkinDesc.p8C entries to pBase + *pOffset.
SkinDesc8C* fn_8011172C(HwsBurn* pBurn, u8* pBase, s32* pOffset, s32 nAlign) {
    SkinDesc8C* aOut;
    int i;
    int n = pBurn->n74;
    SkinDesc* pDesc = pBurn->pDesc;
    s32 nEnd;

    aOut = (SkinDesc8C*)(pBase + *pOffset);
    *pOffset += n * sizeof(SkinDesc8C);
    nEnd = nAlign + *pOffset;
    nEnd = (nEnd - 1) & ~(nAlign - 1);
    *pOffset = nEnd;
    for (i = 0; i < n; i++) {
        memcpy(&aOut[i], &pDesc->p8C[pBurn->a7C[i]], sizeof(SkinDesc8C));
    }
    return aOut;
}

// Point the a64 entries at their place in the a7C list, then copy a64 to pBase + *pOffset.
SkinDesc14* fn_801117D0(HwsBurn* pBurn, u8* pBase, s32* pOffset, s32 nAlign) {
    int i;
    int n = pBurn->n60;

    for (i = 0; i < n; i++) {
        if (pBurn->a64[i].n18 >= 0) {
            pBurn->a64[i].n18 = pBurn->a80[pBurn->a64[i].n18];
        }
    }
    return fn_80110E98(pBase, pOffset, pBurn->a64, pBurn->n60 * sizeof(SkinDesc14), nAlign);
}

// Copy SkinDesc.p14 and hand each entry to the burn's callback.
void fn_801115C4(HwsBurn* pBurn) {
    int i;
    int n = pBurn->n60;

    memcpy(pBurn->a64, pBurn->pDesc->p14, n * sizeof(SkinDesc14));
    for (i = 0; i < n; i++) {
        if (pBurn->pfn68 != NULL) {
            pBurn->pfn68(pBurn->pSkin, &pBurn->a64[i]);
        }
    }
}

// Burn the description: list what is used, then copy the description and each table it keeps into
// one block, renumbered for the burn. p74, p7C and p84 are dropped. An entry left with no morph
// targets loses its flag, and so do its meshes' 0x100000 flags where they also have 0x10; n30 ends
// after the last mesh still flagged 0x100000.
SkinDesc* fn_80111850(HwsBurn* pBurn) {
    SkinMeshIter iterBuf;
    SkinIterArgs args;
    SkinDesc* pDesc;
    s32 nOffset;
    int i;
    SkinIter* pIter;
    SkinMesh* pMesh;
    SkinDesc44* pEntry;
    SkinDesc* pOut;
    u8* pBlock;
    s32 nBytes;
    int j;
    int nLast;

    pDesc = pBurn->pDesc;
    fn_801115C4(pBurn);
    memset(pBurn->a2C, -1, pBurn->n20 * 4);
    memset(pBurn->a30, -1, pBurn->n20 * 4);
    memset(pBurn->a44, -1, pBurn->n34 * 4);
    memset(pBurn->a48, -1, pBurn->n34 * 4);
    memset(pBurn->a58, -1, pBurn->n4C * 4);
    memset(pBurn->a5C, -1, pBurn->n4C * 4);
    memset(pBurn->a7C, -1, pBurn->n70 * 4);
    memset(pBurn->a80, -1, pBurn->n70 * 4);

    // The size of the block.
    nBytes = fn_80111310(pBurn, 16);
    nBytes += fn_80110F2C(pBurn, 16);
    nBytes += fn_80111124(pBurn, 16);
    nBytes += fn_80111424(pBurn, 16);
    nBytes += fn_80111658(pBurn, 16);
    // port: fn_80110E74 only tests its pointer for NULL; EA passes the local's address.
    nBytes += fn_80110E74(&pDesc, 1, sizeof(SkinDesc), 16);
    nBytes += fn_80110E74(pDesc->p14, pBurn->n60, sizeof(SkinDesc14), 16);
    nBytes += fn_80110E74(pDesc->p20, pDesc->n1C, 4, 16);
    nBytes += fn_80110E74(pDesc->p28, pDesc->n24, sizeof(SkinDesc28), 16);
    nBytes += fn_80110E74(pDesc->p44, pBurn->n38, sizeof(SkinDesc44), 16);
    nBytes += fn_80110E74(pDesc->p34, pBurn->n24, sizeof(SkinMesh), 16);
    nBytes += fn_80110E74(pDesc->p3C, pBurn->n50, 4, 16);
    nBytes += fn_80110E74(pDesc->pParts, pDesc->nParts, sizeof(SkinPartDef), 16);
    nBytes += fn_80110E74(pDesc->pVariants, pDesc->nVariants, sizeof(SkinVariant), 16);
    nBytes += fn_80110E74(pDesc->p5C, pDesc->n58, sizeof(SkinDesc5C), 16);
    nBytes += fn_80110E74(pDesc->pLinks, pDesc->nLinks, sizeof(SkinLink), 16);
    nBytes += fn_80110E74(pDesc->p6C, pDesc->n68, 4, 16);
    nBytes += fn_80110E74(pDesc->p8C, pBurn->n74, sizeof(SkinDesc8C), 16);
    nBytes += fn_80110E74(pDesc->p94, pDesc->n90, 0x50, 16);
    nBytes += fn_80110E74(pDesc->pA4, pDesc->nA0, 2, 16);
    nBytes += fn_80110E74(pDesc->pAC, pDesc->nA8, 2, 16);
    nBytes += fn_80110E74(pDesc->pB8, pDesc->nB4, sizeof(SkinDescB8), 16);

    // The copy.
    pBlock = fn_80009B34(nBytes, 2, 16, "hwsBurn.c", 979);
    nOffset = 0;
    pOut = fn_80110E98(pBlock, &nOffset, pDesc, sizeof(SkinDesc), 16);
    pOut->n08 = nBytes;
    pOut->n2C = pBurn->n24;
    pOut->n40 = pBurn->n38;
    pOut->n38 = pBurn->n50;
    pOut->n70 = 0;
    pOut->n78 = 0;
    pOut->n80 = 0;
    pOut->p74 = NULL;
    pOut->p7C = NULL;
    pOut->p84 = NULL;
    pOut->p28 = fn_80111384(pBurn, pBlock, &nOffset, 16);
    pOut->p44 = fn_80110FB4(pBurn, pBlock, &nOffset, 16);
    pOut->p34 = fn_801111E8(pBurn, pBlock, &nOffset, 16);
    pOut->p3C = fn_801114AC(pBurn, pBlock, &nOffset, 16);
    pOut->p6C = fn_80111540(pBurn, pBlock, &nOffset, 16);
    pOut->p14 = fn_801117D0(pBurn, pBlock, &nOffset, 16);
    pOut->p8C = fn_8011172C(pBurn, pBlock, &nOffset, 16);
    pOut->p20 = fn_80110E98(pBlock, &nOffset, pDesc->p20, pDesc->n1C * 4, 16);
    pOut->pParts = fn_80110E98(pBlock, &nOffset, pDesc->pParts, pDesc->nParts * sizeof(SkinPartDef), 16);
    pOut->pVariants =
        fn_80110E98(pBlock, &nOffset, pDesc->pVariants, pDesc->nVariants * sizeof(SkinVariant), 16);
    pOut->p5C = fn_80110E98(pBlock, &nOffset, pDesc->p5C, pDesc->n58 * sizeof(SkinDesc5C), 16);
    pOut->pLinks = fn_80110E98(pBlock, &nOffset, pDesc->pLinks, pDesc->nLinks * sizeof(SkinLink), 16);
    pOut->p94 = fn_80110E98(pBlock, &nOffset, pDesc->p94, pDesc->n90 * 0x50, 16);
    pOut->pA4 = fn_80110E98(pBlock, &nOffset, pDesc->pA4, pDesc->nA0 * 2, 16);
    pOut->pAC = fn_80110E98(pBlock, &nOffset, pDesc->pAC, pDesc->nA8 * 2, 16);
    pOut->pB8 = fn_80110E98(pBlock, &nOffset, pDesc->pB8, pDesc->nB4 * sizeof(SkinDescB8), 16);

    // Entries that kept no morph targets.
    for (i = 0; i < pOut->n40; i++) {
        pEntry = &pOut->p44[i];
        if ((pEntry->u24 & 2) && pEntry->n14 == 0) {
            pEntry->u24 &= ~2;
            args.pDesc = pOut;
            args.n = i;
            pIter = fn_80113910((u8*)&iterBuf, &args);
            while (fn_800CEEC0(pIter)) {
                pMesh = fn_800CEEF4(pIter);
                if ((pMesh->uFlags & 0x100000) && (pMesh->uFlags & 0x10)) {
                    pMesh->uFlags &= ~0x100000;
                }
                fn_800CEEC8(pIter);
            }
        }
    }
    nLast = 0;
    for (j = 0; j < pOut->n2C; j++) {
        if (pOut->p34[j].uFlags & 0x100000) {
            nLast = j + 1;
        }
    }
    pOut->n30 = nLast;
    return pOut;
}

// Everything the chosen variants of every part use (all variants of a part without a choice),
// then fn_80111850 makes the burnt description.
SkinDesc* fn_80111EB0(HwsBurn* pBurn) {
    SkinDesc* pDesc = pBurn->pDesc;
    s32 nChosen;
    int i;
    int j;

    for (i = 0; i < pBurn->nParts; i++) {
        nChosen = pBurn->aVariant[i];
        for (j = 0; j < pDesc->pParts[i].nVariants; j++) {
            if (nChosen == -1 || j == nChosen) {
                fn_80110D10(pBurn, i, j + pDesc->pParts[i].nFirst);
            }
        }
    }
    return fn_80111850(pBurn);
}
