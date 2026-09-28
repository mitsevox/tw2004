// hwsBurn.c (EA's name, from its asserts; TW06 and TW2005 keep it in
// Lo-Rendering/Shader/Hardware): the renderer's side of burning a golfer's skin (SkinBurn.c's
// SkinBurn_BurnSkin drives it). A burn (HwsBurn_Create) is told which variant and option each part
// keeps, which morph targets to drop and which blended meshes replace the description's;
// HwsBurn_Burn then marks everything those choices use (the Mark steps) and HwsBurn_BuildDesc
// copies only that into one new skin description in a single block, every table renumbered, so the
// chosen look is baked into fixed render data. No EA function names survive in any reference build
// (TW06 names only the type, HWBurnStateT), so the HwsBurn_ names are ours.

#include "engine.h"
#include "charstate.h"

SkinIter* fn_80113A9C(u8* pBuf, SkinIterArgs* pArgs);   // hwsRender_Gc.c
void  fn_80113B14(SkinIter* pIter);                     // hwsRender_Gc.c: ends the iterator
s32   SkinPart_FindDescPart(SkinDesc* pDesc, u64 uId);
void  HwsBurn_MarkEntry(HwsBurn* pBurn, int n);
SkinDesc* HwsBurn_BuildDesc(HwsBurn* pBurn);

// Makes a burn of pDesc in static memory: each table and bit array sized from the description, the
// bit arrays clear, no morph target dropped, and no part's variant or option chosen yet (-1: every
// one stays). The caller then makes its choices (HwsBurn_SetPartVariant, HwsBurn_SetPartOption,
// HwsBurn_DropMorphTarget, HwsBurn_SetOverrideTable), gets the burnt description from HwsBurn_Burn
// and frees the burn with HwsBurn_Destroy.
HwsBurn* HwsBurn_Create(SkinDesc* pDesc) {
    HwsBurn* pBurn = StaticMem_Alloc(sizeof(HwsBurn), 1, 16, "hwsBurn.c", 46);
    int i;
    int nBytes;

    memset(pBurn, 0, sizeof(HwsBurn));
    pBurn->pDesc = pDesc;
    pBurn->nParts = pDesc->nParts;
    pBurn->n18 = SkinMorph_GetNumTargets(pDesc);
    pBurn->n20 = pDesc->nMeshes;
    pBurn->n34 = pDesc->n40;
    pBurn->n4C = pDesc->n38;
    pBurn->n60 = pDesc->n10;
    pBurn->n70 = pDesc->n88;
    if (pBurn->nParts != 0) {
        pBurn->aVariant = StaticMem_Alloc(pBurn->nParts * 4, 1, 16, "hwsBurn.c", 61);
        memset(pBurn->aVariant, 0, pBurn->nParts * 4);
        pBurn->aOption = StaticMem_Alloc(pBurn->nParts * 4, 1, 16, "hwsBurn.c", 64);
        for (i = 0; i < pBurn->nParts; i++) {
            pBurn->aOption[i] = -1;
            pBurn->aVariant[i] = -1;
        }
    }
    if (pBurn->n18 != 0) {
        pBurn->a1C = StaticMem_Alloc(pBurn->n18 * 4, 1, 16, "hwsBurn.c", 76);
        memset(pBurn->a1C, 0, pBurn->n18 * 4);
    }
    if (pBurn->n20 != 0) {
        pBurn->p28 = StaticMem_Alloc((pBurn->n20 + 31) / 32 * 4, 1, 16, "hwsBurn.c", 83);
        BitArray_ClearArray(pBurn->p28, pBurn->n20);
        pBurn->a2C = StaticMem_Alloc(pBurn->n20 * 4, 1, 16, "hwsBurn.c", 87);
        pBurn->a30 = StaticMem_Alloc(pBurn->n20 * 4, 1, 16, "hwsBurn.c", 88);
    }
    if (pBurn->n34 != 0) {
        nBytes = (pBurn->n34 + 31) / 32 * 4;
        pBurn->p3C = StaticMem_Alloc(nBytes, 1, 16, "hwsBurn.c", 94);
        pBurn->p40 = StaticMem_Alloc(nBytes, 1, 16, "hwsBurn.c", 95);
        BitArray_ClearArray(pBurn->p3C, pBurn->n34);
        BitArray_ClearArray(pBurn->p40, pBurn->n34);
        pBurn->a44 = StaticMem_Alloc(pBurn->n34 * 4, 1, 16, "hwsBurn.c", 100);
        pBurn->a48 = StaticMem_Alloc(pBurn->n34 * 4, 1, 16, "hwsBurn.c", 101);
    }
    if (pBurn->n4C != 0) {
        pBurn->p54 = StaticMem_Alloc((pBurn->n4C + 31) / 32 * 4, 1, 16, "hwsBurn.c", 107);
        BitArray_ClearArray(pBurn->p54, pBurn->n4C);
        pBurn->a58 = StaticMem_Alloc(pBurn->n4C * 4, 1, 16, "hwsBurn.c", 111);
        pBurn->a5C = StaticMem_Alloc(pBurn->n4C * 4, 1, 16, "hwsBurn.c", 112);
    }
    if (pBurn->n60 != 0) {
        pBurn->a64 = StaticMem_Alloc(pBurn->n60 * sizeof(SkinDesc14), 1, 16, "hwsBurn.c", 120);
    }
    if (pBurn->n70 != 0) {
        pBurn->p78 = StaticMem_Alloc((pBurn->n70 + 31) / 32 * 4, 1, 16, "hwsBurn.c", 127);
        BitArray_ClearArray(pBurn->p78, pBurn->n70);
        pBurn->a7C = StaticMem_Alloc(pBurn->n70 * 4, 1, 16, "hwsBurn.c", 131);
        pBurn->a80 = StaticMem_Alloc(pBurn->n70 * 4, 1, 16, "hwsBurn.c", 132);
    }
    return pBurn;
}

// Free a burn and its tables.
void HwsBurn_Destroy(HwsBurn* pBurn) {
    if (pBurn->aVariant != NULL) {
        StaticMem_Free(pBurn->aVariant);
    }
    if (pBurn->aOption != NULL) {
        StaticMem_Free(pBurn->aOption);
    }
    if (pBurn->a1C != NULL) {
        StaticMem_Free(pBurn->a1C);
    }
    if (pBurn->p28 != NULL) {
        StaticMem_Free(pBurn->p28);
    }
    if (pBurn->a2C != NULL) {
        StaticMem_Free(pBurn->a2C);
    }
    if (pBurn->a30 != NULL) {
        StaticMem_Free(pBurn->a30);
    }
    if (pBurn->p3C != NULL) {
        StaticMem_Free(pBurn->p3C);
    }
    if (pBurn->p40 != NULL) {
        StaticMem_Free(pBurn->p40);
    }
    if (pBurn->a44 != NULL) {
        StaticMem_Free(pBurn->a44);
    }
    if (pBurn->a48 != NULL) {
        StaticMem_Free(pBurn->a48);
    }
    if (pBurn->p54 != NULL) {
        StaticMem_Free(pBurn->p54);
    }
    if (pBurn->a58 != NULL) {
        StaticMem_Free(pBurn->a58);
    }
    if (pBurn->a5C != NULL) {
        StaticMem_Free(pBurn->a5C);
    }
    if (pBurn->a64 != NULL) {
        StaticMem_Free(pBurn->a64);
    }
    if (pBurn->p78 != NULL) {
        StaticMem_Free(pBurn->p78);
    }
    if (pBurn->a7C != NULL) {
        StaticMem_Free(pBurn->a7C);
    }
    if (pBurn->a80 != NULL) {
        StaticMem_Free(pBurn->a80);
    }
    StaticMem_Free(pBurn);
}

// Sets the callback the burn runs with pSkin on each material entry (SkinDesc.p14) it copies
// (SkinBurn_BurnSkin passes SkinBurn_BakeMaterialEntry).
void HwsBurn_SetMaterialCallback(HwsBurn* pBurn, void (*pfn)(Skin* pSkin, SkinDesc14* pEntry), Skin* pSkin) {
    pBurn->pfn68 = pfn;
    pBurn->pSkin = pSkin;
}

// Keeps only variant nVariant (counted within the part) of part nPart; left at -1, every variant of
// the part stays.
void HwsBurn_SetPartVariant(HwsBurn* pBurn, int nPart, s32 nVariant) {
    pBurn->aVariant[nPart] = nVariant;
}

// Keeps only option nOption of part nPart's variants; left at -1, every option stays.
void HwsBurn_SetPartOption(HwsBurn* pBurn, int nPart, s32 nOption) {
    pBurn->aOption[nPart] = nOption;
}

// Sets the mesh table whose meshes are copied in place of the description's (SkinBurn_BurnSkin
// passes the morph-blended meshes from SkinMorph_CreateBlended); NULL: none.
void HwsBurn_SetOverrideTable(HwsBurn* pBurn, HwsOverrideTable* pOverride) {
    pBurn->pOverride = pOverride;
}

// Drops morph target n from the burn: its entry is not marked, so its meshes are not kept
// (SkinBurn_BurnSkin drops the targets it blends into the meshes).
void HwsBurn_DropMorphTarget(HwsBurn* pBurn, int n) {
    pBurn->a1C[n] = 1;
}

// fake match: an identity read, for the register order of HwsBurn_MarkEntry.
static inline SkinIter* fn_80110A38_Read(SkinIter* pIter) {
    return pIter;
}

// Marks what SkinDesc.p44 entry n uses: the entry itself (p3C: its meshes stay; p40: it stays),
// each mesh its iterator walks (p28), and its SkinDesc.p3C entries from n0 (p54: as many as its
// SkinDesc.p28 entry's pairs add up to, one without one). With morph targets, each target the burn
// keeps (a1C clear) is marked the same way, and every target up to the last kept one stays listed
// (p40).
void HwsBurn_MarkEntry(HwsBurn* pBurn, int n) {
    SkinIterArgs args;
    SkinMeshIter iterBuf;
    SkinIter* pIter;
    SkinDesc44* pEntry;
    SkinDesc28* p28;
    s32 nCount;
    int nLast;
    int i;
    int j;

    BitArray_SetBit(pBurn->p3C, n);
    BitArray_SetBit(pBurn->p40, n);
    args.pDesc = pBurn->pDesc;
    args.nEntry = n;
    pIter = fn_80110A38_Read(fn_80113910((u8*)&iterBuf, &args));  // fake match: through fn_80110A38_Read
    while (SkinIter_IsValid(pIter)) {
        BitArray_SetBit(pBurn->p28, SkinIter_GetIndex(pIter));
        SkinIter_Next(pIter);
    }
    fn_80113A7C(pIter);

    pEntry = &pBurn->pDesc->p44[n];
    if (pEntry->u24 & 2) {
        nLast = 0;
        for (j = 0; j < pEntry->n14; j++) {
            if (pBurn->a1C[pEntry->n18 + j] == 0) {
                HwsBurn_MarkEntry(pBurn, pEntry->n10 + j);
                nLast = j + 1;
            }
        }
        for (j = 0; j < nLast; j++) {
            BitArray_SetBit(pBurn->p40, pEntry->n10 + j);
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
        BitArray_SetBit(pBurn->p54, pEntry->n0 + i);
    }
}

// Marks what option n (a SkinDesc.p5C entry) uses: HwsBurn_MarkEntry on each SkinDesc.p44 entry it
// lists.
void HwsBurn_MarkOption(HwsBurn* pBurn, int n) {
    SkinIterArgs args;
    u8 aBuf[0x48];                      // size unknown
    SkinIter* pIter;
    int nMesh;

    args.pDesc = pBurn->pDesc;
    args.nEntry = n;
    for (pIter = fn_80113A9C(aBuf, &args); SkinIter_IsValid(pIter); SkinIter_Next(pIter)) {
        nMesh = SkinIter_GetIndex(pIter);
        HwsBurn_MarkEntry(pBurn, nMesh);
    }
    fn_80113B14(pIter);
}

// Marks what variant nVariant (a SkinDesc.pVariants number) of part nPart uses: its chosen option
// (every option when none is chosen), and for each of its links to a part the description has, that
// option of the linked part's chosen variant (of every variant when none is chosen), where the
// variant has it.
void HwsBurn_MarkVariant(HwsBurn* pBurn, int nPart, int nVariant) {
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
            HwsBurn_MarkOption(pBurn, nFirst + i);
        }
    }
    nFirstLink = pBurn->pDesc->pVariants[nVariant].nFirstLink;
    for (i = 0; i < pBurn->pDesc->pVariants[nVariant].nLinks; i++) {
        pLink = &pBurn->pDesc->pLinks[nFirstLink + i];
        nLinked = SkinPart_FindDescPart(pBurn->pDesc, pLink->uPart);
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
                        HwsBurn_MarkOption(pBurn, pOther->nFirstOption + nOption);
                    }
                }
            }
        }
    }
}

// The bytes nCount items of nSize take, rounded up to nAlign (a power of two); 0 when p is NULL
// (the table is absent).
s32 HwsBurn_AlignedSize(void* p, s32 nCount, s32 nSize, s32 nAlign) {
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
void* HwsBurn_CopyAligned(u8* pBase, s32* pOffset, void* pSrc, s32 nSize, s32 nAlign) {
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

// Lists the SkinDesc.p44 entries the burn keeps (p40) in order (a44, n38 of them) and gives each
// its new number (a48). Adds no bytes (returns 0: HwsBurn_BuildDesc sizes the entries itself);
// nAlign is unused.
s32 HwsBurn_ListEntries(HwsBurn* pBurn, s32 nAlign) {
    int i;
    int nBits = pBurn->pDesc->n40;
    int n = 0;

    for (i = 0; i < nBits; i++) {
        if (BitArray_TestBit(pBurn->p40, i)) {
            pBurn->a44[n] = i;
            pBurn->a48[i] = n;
            n++;
        }
    }
    pBurn->n38 = n;
    return 0;
}

// fake match: an identity read, for the register order of HwsBurn_CopyEntries.
static inline int fn_80110FB4_Read(int n) {
    return n;
}

// Copies the kept SkinDesc.p44 entries (HwsBurn_ListEntries) to pBase + *pOffset, renumbered for
// the burn: an entry whose meshes are not kept (p3C clear) loses them (n0, n8 and its morph flag
// cleared), else n0 moves to its new SkinDesc.p3C number; a morph entry (flag 2) keeps its targets
// up to the first one not kept. n10 moves to its entry's new number (a48) with flag 2, and again
// with flag 4. NULL when none are kept.
SkinDesc44* HwsBurn_CopyEntries(HwsBurn* pBurn, u8* pBase, s32* pOffset, s32 nAlign) {
    int n = fn_80110FB4_Read(pBurn->n38);  // fake match: through fn_80110FB4_Read
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
        pOut = HwsBurn_CopyAligned(pBase, pOffset, &pDesc->p44[nEntry], sizeof(SkinDesc44), 1);
        if (!BitArray_TestBit(pBurn->p3C, nEntry)) {
            pOut->n8 = 0;
            pOut->n0 = 0;
            pOut->u24 &= ~2;
        } else {
            pOut->n0 = pBurn->a5C[pOut->n0];
        }
        if (pOut->u24 & 2) {
            nMorphs = pOut->n14;
            for (j = 0; j < nMorphs; j++) {
                if (!BitArray_TestBit(pBurn->p40, pOut->n10 + j)) {
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

// Lists the meshes the burn keeps (p28 bits, one per SkinDesc.p34 mesh) in order (a2C, n24 of them)
// and gives each its new number (a30). Gives the bytes their data takes, each rounded up to nAlign
// (meshes with flag 0x400000 left out).
s32 HwsBurn_ListMeshes(HwsBurn* pBurn, s32 nAlign) {
    SkinDesc* pDesc = pBurn->pDesc;
    int i;
    int nBits = pDesc->nMeshes;
    int n;
    s32 nBytes;

    nBytes = 0;
    n = 0;

    for (i = 0; i < nBits; i++) {
        if (BitArray_TestBit(pBurn->p28, i)) {
            pBurn->a2C[n] = i;
            pBurn->a30[i] = n;
            n++;
            if (!(pDesc->p34[i].uFlags & 0x400000)) {
                nBytes += HwsBurn_AlignedSize(pDesc->p34[i].pBits, pDesc->p34[i].nSize, 1, nAlign);
            }
        }
    }
    pBurn->n24 = n;
    return nBytes;
}

// Copies the kept meshes (HwsBurn_ListMeshes) to pBase + *pOffset, then each one's data after them:
// the override table's when it has that mesh (the blended one), else the description's. NULL when
// none are kept.
SkinMesh* HwsBurn_CopyMeshes(HwsBurn* pBurn, u8* pBase, s32* pOffset, s32 nAlign) {
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
            aOut[i].pBits = HwsBurn_CopyAligned(pBase, pOffset, pSrc, pDesc->p34[nMesh].nSize, nAlign);
        }
    }
    return aOut;
}

// The bytes the blocks of all the SkinDesc.p28 entries take, each rounded up to nAlign (the burn
// keeps every p28 entry).
s32 HwsBurn_SizeDesc28Blocks(HwsBurn* pBurn, s32 nAlign) {
    SkinDesc* pDesc = pBurn->pDesc;
    int i;
    int n = pDesc->n24;
    s32 nBytes = 0;

    for (i = 0; i < n; i++) {
        nBytes += HwsBurn_AlignedSize(pDesc->p28[i].p10, pDesc->p28[i].n14, 1, nAlign);
    }
    return nBytes;
}

// Copies all the SkinDesc.p28 entries to pBase + *pOffset, then each entry's block (p10, n14 bytes)
// after them, the copies pointing at the copied blocks.
SkinDesc28* HwsBurn_CopyDesc28(HwsBurn* pBurn, u8* pBase, s32* pOffset, s32 nAlign) {
    SkinDesc* pDesc = pBurn->pDesc;
    int i;
    int n = pDesc->n24;
    SkinDesc28* aCopy = HwsBurn_CopyAligned(pBase, pOffset, pDesc->p28, n * sizeof(SkinDesc28), nAlign);

    for (i = 0; i < n; i++) {
        aCopy[i].p10 = HwsBurn_CopyAligned(pBase, pOffset, pDesc->p28[i].p10, pDesc->p28[i].n14, nAlign);
    }
    return aCopy;
}

// Lists the SkinDesc.p3C entries (mesh numbers) the burn keeps (p54) in order (a58, n50 of them)
// and gives each its new number (a5C). Adds no bytes (returns 0); nAlign is unused.
s32 HwsBurn_ListMeshRefs(HwsBurn* pBurn, s32 nAlign) {
    int i;
    int nBits = pBurn->pDesc->n38;
    int n = 0;

    for (i = 0; i < nBits; i++) {
        if (BitArray_TestBit(pBurn->p54, i)) {
            pBurn->a58[n] = i;
            pBurn->a5C[i] = n;
            n++;
        }
    }
    pBurn->n50 = n;
    return 0;
}

// Copies the kept SkinDesc.p3C entries (HwsBurn_ListMeshRefs) to pBase + *pOffset, each mesh number
// moved to the mesh's new number (a30; -1, none, stays -1). NULL when none are kept.
s32* HwsBurn_CopyMeshRefs(HwsBurn* pBurn, u8* pBase, s32* pOffset, s32 nAlign) {
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

// Copies SkinDesc.p6C (the SkinDesc.p44 entries each option lists) to pBase + *pOffset, each entry
// moved to its new number (a48; an entry the burn dropped becomes -1).
s32* HwsBurn_CopyOptionEntries(HwsBurn* pBurn, u8* pBase, s32* pOffset, s32 nAlign) {
    SkinDesc* pDesc = pBurn->pDesc;
    int n = pDesc->n68;
    s32* aCopy = HwsBurn_CopyAligned(pBase, pOffset, pDesc->p6C, n * 4, nAlign);
    int i;

    for (i = 0; i < n; i++) {
        if (aCopy[i] >= 0) {
            aCopy[i] = pBurn->a48[aCopy[i]];
        }
    }
    return aCopy;
}

// Copies the description's material entries (SkinDesc.p14) into a64 and runs the burn's callback
// (HwsBurn_SetMaterialCallback) with its skin on each.
void HwsBurn_PrepareMaterials(HwsBurn* pBurn) {
    int i;
    int n = pBurn->n60;

    memcpy(pBurn->a64, pBurn->pDesc->p14, n * sizeof(SkinDesc14));
    for (i = 0; i < n; i++) {
        if (pBurn->pfn68 != NULL) {
            pBurn->pfn68(pBurn->pSkin, &pBurn->a64[i]);
        }
    }
}

// Marks the set options (SkinDesc.p8C entries) the material entries use (a64 n18, in p78), lists
// them in order (a7C, n74 of them) and gives each its new number (a80). Adds no bytes (returns 0);
// nAlign is unused.
s32 HwsBurn_ListSetOptions(HwsBurn* pBurn, s32 nAlign) {
    int i;
    int nEntries = pBurn->n60;
    int nBits = pBurn->n70;
    int n;

    BitArray_ClearArray(pBurn->p78, pBurn->n70);
    for (i = 0; i < nEntries; i++) {
        if (pBurn->a64[i].n18 >= 0 && pBurn->a64[i].n18 < nBits) {
            BitArray_SetBit(pBurn->p78, pBurn->a64[i].n18);
        }
    }
    n = 0;
    for (i = 0; i < nBits; i++) {
        if (BitArray_TestBit(pBurn->p78, i)) {
            pBurn->a7C[n] = i;
            pBurn->a80[i] = n;
            n++;
        }
    }
    pBurn->n74 = n;
    return 0;
}

// Copies the set options the material entries use (HwsBurn_ListSetOptions) to pBase + *pOffset.
SkinDesc8C* HwsBurn_CopySetOptions(HwsBurn* pBurn, u8* pBase, s32* pOffset, s32 nAlign) {
    SkinDesc* pDesc = pBurn->pDesc;
    SkinDesc8C* aOut;
    int i;
    int n = pBurn->n74;
    s32 nEnd;

    aOut = (SkinDesc8C*)(pBase + *pOffset);
    *pOffset += n * sizeof(SkinDesc8C);
    nEnd = *pOffset;
    nEnd += nAlign - 1;
    *pOffset = nEnd & ~(nAlign - 1);
    for (i = 0; i < n; i++) {
        memcpy(&aOut[i], &pDesc->p8C[pBurn->a7C[i]], sizeof(SkinDesc8C));
    }
    return aOut;
}

// Moves each material entry's set option (n18) to its new number (a80), then copies the entries
// (a64, as the callback left them) to pBase + *pOffset.
SkinDesc14* HwsBurn_CopyMaterials(HwsBurn* pBurn, u8* pBase, s32* pOffset, s32 nAlign) {
    int i;
    int n = pBurn->n60;

    for (i = 0; i < n; i++) {
        if (pBurn->a64[i].n18 >= 0) {
            pBurn->a64[i].n18 = pBurn->a80[pBurn->a64[i].n18];
        }
    }
    return HwsBurn_CopyAligned(pBase, pOffset, pBurn->a64, pBurn->n60 * sizeof(SkinDesc14), nAlign);
}

// Builds the burnt description from what the Mark steps marked, in one static-memory block: the
// material entries go through the callback, each table is listed and sized, then the description
// and each table it keeps are copied into the block, renumbered for the burn. The set tables p74,
// p7C and p84 are dropped (the callback has baked the chosen sets into the material entries). An
// entry left with no morph targets loses its morph flag, and its meshes that have both 0x100000 and
// 0x10 lose 0x100000; nOverrideMeshes ends after the last mesh still flagged 0x100000.
SkinDesc* HwsBurn_BuildDesc(HwsBurn* pBurn) {
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
    HwsBurn_PrepareMaterials(pBurn);
    memset(pBurn->a2C, -1, pBurn->n20 * 4);
    memset(pBurn->a30, -1, pBurn->n20 * 4);
    memset(pBurn->a44, -1, pBurn->n34 * 4);
    memset(pBurn->a48, -1, pBurn->n34 * 4);
    memset(pBurn->a58, -1, pBurn->n4C * 4);
    memset(pBurn->a5C, -1, pBurn->n4C * 4);
    memset(pBurn->a7C, -1, pBurn->n70 * 4);
    memset(pBurn->a80, -1, pBurn->n70 * 4);

    // The size of the block.
    nBytes = HwsBurn_SizeDesc28Blocks(pBurn, 16);
    nBytes += HwsBurn_ListEntries(pBurn, 16);
    nBytes += HwsBurn_ListMeshes(pBurn, 16);
    nBytes += HwsBurn_ListMeshRefs(pBurn, 16);
    nBytes += HwsBurn_ListSetOptions(pBurn, 16);
    // port: HwsBurn_AlignedSize only tests its pointer for NULL; EA passes the local's address.
    nBytes += HwsBurn_AlignedSize(&pDesc, 1, sizeof(SkinDesc), 16);
    nBytes += HwsBurn_AlignedSize(pDesc->p14, pBurn->n60, sizeof(SkinDesc14), 16);
    nBytes += HwsBurn_AlignedSize(pDesc->p20, pDesc->n1C, 4, 16);
    nBytes += HwsBurn_AlignedSize(pDesc->p28, pDesc->n24, sizeof(SkinDesc28), 16);
    nBytes += HwsBurn_AlignedSize(pDesc->p44, pBurn->n38, sizeof(SkinDesc44), 16);
    nBytes += HwsBurn_AlignedSize(pDesc->p34, pBurn->n24, sizeof(SkinMesh), 16);
    nBytes += HwsBurn_AlignedSize(pDesc->p3C, pBurn->n50, 4, 16);
    nBytes += HwsBurn_AlignedSize(pDesc->pParts, pDesc->nParts, sizeof(SkinPartDef), 16);
    nBytes += HwsBurn_AlignedSize(pDesc->pVariants, pDesc->nVariants, sizeof(SkinVariant), 16);
    nBytes += HwsBurn_AlignedSize(pDesc->p5C, pDesc->n58, sizeof(SkinDesc5C), 16);
    nBytes += HwsBurn_AlignedSize(pDesc->pLinks, pDesc->nLinks, sizeof(SkinLink), 16);
    nBytes += HwsBurn_AlignedSize(pDesc->p6C, pDesc->n68, 4, 16);
    nBytes += HwsBurn_AlignedSize(pDesc->p8C, pBurn->n74, sizeof(SkinDesc8C), 16);
    nBytes += HwsBurn_AlignedSize(pDesc->p94, pDesc->n90, 0x50, 16);
    nBytes += HwsBurn_AlignedSize(pDesc->pA4, pDesc->nA0, 2, 16);
    nBytes += HwsBurn_AlignedSize(pDesc->pAC, pDesc->nA8, 2, 16);
    nBytes += HwsBurn_AlignedSize(pDesc->pB8, pDesc->nB4, sizeof(SkinDescB8), 16);

    // The copy.
    pBlock = StaticMem_Alloc(nBytes, 2, 16, "hwsBurn.c", 979);
    nOffset = 0;
    pOut = HwsBurn_CopyAligned(pBlock, &nOffset, pDesc, sizeof(SkinDesc), 16);
    pOut->n08 = nBytes;
    pOut->nMeshes = pBurn->n24;
    pOut->n40 = pBurn->n38;
    pOut->n38 = pBurn->n50;
    pOut->n70 = 0;
    pOut->n78 = 0;
    pOut->n80 = 0;
    pOut->p74 = NULL;
    pOut->p7C = NULL;
    pOut->p84 = NULL;
    pOut->p28 = HwsBurn_CopyDesc28(pBurn, pBlock, &nOffset, 16);
    pOut->p44 = HwsBurn_CopyEntries(pBurn, pBlock, &nOffset, 16);
    pOut->p34 = HwsBurn_CopyMeshes(pBurn, pBlock, &nOffset, 16);
    pOut->p3C = HwsBurn_CopyMeshRefs(pBurn, pBlock, &nOffset, 16);
    pOut->p6C = HwsBurn_CopyOptionEntries(pBurn, pBlock, &nOffset, 16);
    pOut->p14 = HwsBurn_CopyMaterials(pBurn, pBlock, &nOffset, 16);
    pOut->p8C = HwsBurn_CopySetOptions(pBurn, pBlock, &nOffset, 16);
    pOut->p20 = HwsBurn_CopyAligned(pBlock, &nOffset, pDesc->p20, pDesc->n1C * 4, 16);
    pOut->pParts = HwsBurn_CopyAligned(pBlock, &nOffset, pDesc->pParts, pDesc->nParts * sizeof(SkinPartDef),
                                       16);
    pOut->pVariants =
        HwsBurn_CopyAligned(pBlock, &nOffset, pDesc->pVariants, pDesc->nVariants * sizeof(SkinVariant), 16);
    pOut->p5C = HwsBurn_CopyAligned(pBlock, &nOffset, pDesc->p5C, pDesc->n58 * sizeof(SkinDesc5C), 16);
    pOut->pLinks = HwsBurn_CopyAligned(pBlock, &nOffset, pDesc->pLinks, pDesc->nLinks * sizeof(SkinLink), 16);
    pOut->p94 = HwsBurn_CopyAligned(pBlock, &nOffset, pDesc->p94, pDesc->n90 * 0x50, 16);
    pOut->pA4 = HwsBurn_CopyAligned(pBlock, &nOffset, pDesc->pA4, pDesc->nA0 * 2, 16);
    pOut->pAC = HwsBurn_CopyAligned(pBlock, &nOffset, pDesc->pAC, pDesc->nA8 * 2, 16);
    pOut->pB8 = HwsBurn_CopyAligned(pBlock, &nOffset, pDesc->pB8, pDesc->nB4 * sizeof(SkinDescB8), 16);

    // Entries that kept no morph targets.
    for (i = 0; i < pOut->n40; i++) {
        pEntry = &pOut->p44[i];
        if ((pEntry->u24 & 2) && pEntry->n14 == 0) {
            pEntry->u24 &= ~2;
            args.pDesc = pOut;
            args.nEntry = i;
            pIter = fn_80113910((u8*)&iterBuf, &args);
            while (SkinIter_IsValid(pIter)) {
                pMesh = SkinIter_GetMesh(pIter);
                if ((pMesh->uFlags & 0x100000) && (pMesh->uFlags & 0x10)) {
                    pMesh->uFlags &= ~0x100000;
                }
                SkinIter_Next(pIter);
            }
        }
    }
    nLast = 0;
    for (j = 0; j < pOut->nMeshes; j++) {
        if (pOut->p34[j].uFlags & 0x100000) {
            nLast = j + 1;
        }
    }
    pOut->nOverrideMeshes = nLast;
    return pOut;
}

// Burns the description: marks what each part's chosen variant uses (every variant of a part with
// none chosen), then builds and returns the burnt description (HwsBurn_BuildDesc).
SkinDesc* HwsBurn_Burn(HwsBurn* pBurn) {
    SkinDesc* pDesc = pBurn->pDesc;
    s32 nChosen;
    int i;
    int j;

    for (i = 0; i < pBurn->nParts; i++) {
        nChosen = pBurn->aVariant[i];
        for (j = 0; j < pDesc->pParts[i].nVariants; j++) {
            if (nChosen == -1 || j == nChosen) {
                HwsBurn_MarkVariant(pBurn, i, j + pDesc->pParts[i].nFirst);
            }
        }
    }
    return HwsBurn_BuildDesc(pBurn);
}
