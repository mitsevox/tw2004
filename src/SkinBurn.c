// SkinBurn.c (EA's name, from its asserts; TW06 has a golf/animation/skinburn.c too): "burns" the
// golfer's body skin once a look is chosen (SkinPart_BurnBodySkin, SkinBurn_BurnSkin). hwsBurn.c
// builds a description holding only the chosen variants and options, with the dropped morph
// targets blended in at their weights (SkinMorph_CreateBlended); this file then drops what the
// skin's model no longer uses, renumbers what is left and packs it into one allocation. The first
// function, the start-up check for the disc's signature file, has nothing to do with skins.
#include "game_types.h"
#include "platform.h"
#include "engine.h"
#include "charstate.h"

s32* gBurnNewMtxBits;   // while SkinBurn_DropUnusedMatrices runs: per old Skin.aMtxBits bit, its
                        // new number (-1 dropped); SkinBurn_RenumberMeshBits reads it

s32   SkinBurn_GetAlignedSize(const void* p, s32 nCount, s32 nSize, s32 nAlign);
void* SkinBurn_CopyAligned(u8* pBase, s32* pOffset, const void* pSrc, s32 nSize, s32 nAlign);
void* SkinBurn_TakeAligned(u8* pBase, s32* pOffset, s32 nSize, s32 nAlign);

char gSignatureDir[8] = "";      // the folder the signature file is looked for in

// Checks at start-up (GoEntry.c) that the signature file Signat.sig is on the disc, in the folder
// gSignatureDir names; without it the game stops on purpose. Nothing to do with skins: it lies in
// the margin at SkinBurn.c's start (docs/sourcefiles.md) and may belong to the file before.
void SkinBurn_CheckSignatureFile(void) {
    char szPath[256];   // the size is not known (the frame leaves room for 256 bytes)
    int hFile;

    sprintf(szPath, "%sSignat.sig", gSignatureDir);
    hFile = fn_800060E0(szPath);
    if (hFile < 0) {
        // port: EA stops the game on purpose with a write to address 0 (undefined in C); a port
        //       should abort here instead
        *(volatile s32*)0 = 0;
    }
    fn_8000633C(hFile);
}

// Renumbers each SkinModel.p44 entry's n0 (a SkinDesc.n2C bit) to its number in the burnt
// description (pBurn->a30) and packs the kept entries to the front, dropping those the burn dropped
// (-1). SkinModel.n40 becomes the count kept.
void SkinBurn_RenumberMeshEntries(Skin* pSkin, HwsBurn* pBurn) {
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

// Renumbers a mesh's matrix bits (SkinMeshBit.nBit) with gBurnNewMtxBits (pSkin is unused).
void SkinBurn_RenumberMeshBits(Skin* pSkin, SkinMesh* pMesh) {
    SkinMeshBit* pBit = pMesh->pBits;
    s32 i;

    for (i = 0; i < pMesh->n8; i++) {
        pBit->nBit = gBurnNewMtxBits[pBit->nBit];
        pBit++;
    }
}

// Renumbers the matrix bits (SkinBurn_RenumberMeshBits) of the meshes of option n (SkinDesc.p5C
// entry) that have flags 1 and 0x10.
void SkinBurn_RenumberOptionBits(Skin* pSkin, s32 n) {
    SkinIterArgs args;
    u8 aBuf[0x48];      // the iterator's buffer; its size is not known
    SkinIter* pIter;
    SkinMesh* pMesh;

    args.pDesc = pSkin->pModel->pDesc;
    args.nEntry = n;
    pIter = fn_80113B34(aBuf, &args);
    while (SkinIter_IsValid(pIter)) {
        pMesh = SkinIter_GetMesh(pIter);
        if ((pMesh->uFlags & 1) && (pMesh->uFlags & 0x10)) {
            SkinBurn_RenumberMeshBits(pSkin, pMesh);
        }
        SkinIter_Next(pIter);
    }
    fn_80113BAC(pIter);
}

// Drops the blended matrices no option uses (SkinModel.p54, one per bit of Skin.aMtxBits). It keeps
// the first SkinModel.n14 and those SkinPart_MarkAllOptions marks (marked into a scratch bit array
// put in place of Skin.aMtxBits), packs their p54 entries to the front, numbered 0, 1, 2..., and
// renumbers the bits of every option's meshes to match (gBurnNewMtxBits: each old bit's new
// number, -1 dropped). Then each SkinModel.p44 entry's list (p4) is cut to the kept count at most
// and numbered 0, 1, 2..., and the SkinDesc.p34 mesh of the same index gets that count (n8) and a
// size of 64 bytes each. SkinModel.n50 becomes the count kept.
void SkinBurn_DropUnusedMatrices(Skin* pSkin) {
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
    s32 n;
    s32 j;
    s32 k;
    SkinModel44* pEntry;

    pModel = pSkin->pModel;
    nBits = pModel->n50;
    nEntries = pModel->n40;
    if (nBits == 0) {
        return;
    }
    aBits = StaticMem_Alloc((nBits + 31) / 32 * sizeof(u32), 1, 16, "SkinBurn.c", 205);
    BitArray_ClearArray(aBits, nBits);
    aOld = StaticMem_Alloc(nBits * sizeof(s32), 1, 16, "SkinBurn.c", 209);
    aNew = StaticMem_Alloc(nBits * sizeof(s32), 1, 16, "SkinBurn.c", 210);
    memset(aOld, -1, nBits * sizeof(s32));
    memset(aNew, -1, nBits * sizeof(s32));

    // The first n14 bits always stay.
    n = pModel->n14;
    if (n > pModel->n50) {
        n = pModel->n50;
    }
    for (i = 0; i < n; i++) {
        BitArray_SetBit(aBits, i);
    }

    // Mark the bits the options use, in aBits instead of the skin's own array.
    pSaved = pSkin->aMtxBits;
    pSkin->aMtxBits = aBits;
    SkinPart_MarkAllOptions(pSkin);
    pSkin->aMtxBits = pSaved;

    pOld = aOld;
    pNew = aNew;
    nKept = 0;
    gBurnNewMtxBits = aNew;
    for (j = 0; j < nBits; j++) {
        if (BitArray_TestBit(aBits, j)) {
            *pOld++ = j;
            *pNew = nKept++;
        }
        pNew++;
    }

    for (j = 0; j < nKept; j++) {
        if (j != aOld[j]) {
            memcpy(&pModel->p54[j], &pModel->p54[aOld[j]], sizeof(SkinModel54));
        }
    }

    for (i = 0; i < pModel->pDesc->n58; i++) {
        SkinBurn_RenumberOptionBits(pSkin, i);
    }

    // n is reused for each entry's count, and this loop has its own counter.
    for (k = 0; k < nEntries; k++) {
        pEntry = &pModel->p44[k];
        if (nKept < pEntry->n8) {
            pEntry->n8 = nKept;
        }
        n = pEntry->n8;
        for (j = 0; j < n; j++) {
            pEntry->p4[j] = j;
        }
        pSkin->pModel->pDesc->p34[k].n8 = n;
        pSkin->pModel->pDesc->p34[k].nSize = n << 6;
    }

    pModel->n50 = nKept;
    StaticMem_Free(aBits);
    StaticMem_Free(aOld);
    StaticMem_Free(aNew);
}

// The bytes nCount items of nSize take, rounded up to nAlign (a power of two); 0 when there is no
// array (p is NULL).
s32 SkinBurn_GetAlignedSize(const void* p, s32 nCount, s32 nSize, s32 nAlign) {
    s32 n = 0;

    if (p != NULL) {
        n = (nCount * nSize + (nAlign - 1)) & ~(nAlign - 1);
    }
    return n;
}

// Copies nSize bytes of pSrc to pBase + *pOffset and moves *pOffset past them, rounded up to
// nAlign. Gives where the copy went (NULL, and nothing done, when pSrc is NULL).
void* SkinBurn_CopyAligned(u8* pBase, s32* pOffset, const void* pSrc, s32 nSize, s32 nAlign) {
    void* pDst = NULL;

    if (pSrc != NULL) {
        pDst = pBase + *pOffset;
        memcpy(pDst, pSrc, nSize);
        *pOffset += nSize;
        *pOffset = (*pOffset + (nAlign - 1)) & ~(nAlign - 1);
    }
    return pDst;
}

// Takes nSize bytes at pBase + *pOffset without copying anything, moving *pOffset past them rounded
// up to nAlign; gives where they start (NULL, and nothing done, when nSize is 0).
void* SkinBurn_TakeAligned(u8* pBase, s32* pOffset, s32 nSize, s32 nAlign) {
    void* p = NULL;

    if (nSize != 0) {
        p = pBase + *pOffset;
        *pOffset += nSize;
        *pOffset = (*pOffset + (nAlign - 1)) & ~(nAlign - 1);
    }
    return p;
}

// Packs the skin's model and its arrays (p34, p38, p3C, p54, p44, each 16-byte aligned) into one
// allocation, which becomes Skin.pModel (its n08: the allocation's size), and frees the old model.
// The p44 entries' p4 lists go into one shared block of indexes at the end.
void SkinBurn_PackModel(Skin* pSkin) {
    SkinModel* pOld;
    s32 nOffset;
    s32 nSize;
    s32 nIndexes;
    s32 i;
    u8* pBase;
    SkinModel* pNew;
    s32* aIndexes;
    SkinModel44* pEntry;
    s32 j;
    s32 n;
    s32 nEntries;
    s32 nFirst;

    pOld = pSkin->pModel;
    // EA passes &pOld where an array is expected: never NULL, so the model always counts
    nSize = SkinBurn_GetAlignedSize(&pOld, 1, sizeof(SkinModel), 16);
    nSize += SkinBurn_GetAlignedSize(pOld->p34, pOld->n14, 0x20, 16);
    nSize += SkinBurn_GetAlignedSize(pOld->p38, 1, 0x50, 16);
    nSize += SkinBurn_GetAlignedSize(pOld->p3C, pOld->n0C, 0x50, 16);
    nSize += SkinBurn_GetAlignedSize(pOld->p54, pOld->n50, sizeof(SkinModel54), 16);
    nSize += SkinBurn_GetAlignedSize(pOld->p44, pOld->n40, sizeof(SkinModel44), 16);
    nIndexes = 0;
    nEntries = pOld->n40;
    for (i = 0; i < nEntries; i++) {
        nIndexes += pOld->p44[i].n8;
    }
    nSize += SkinBurn_GetAlignedSize(&pOld, nIndexes, sizeof(s32), 16);

    pBase = StaticMem_Alloc(nSize, 2, 16, "SkinBurn.c", 434);
    nOffset = 0;
    pNew = SkinBurn_CopyAligned(pBase, &nOffset, pOld, sizeof(SkinModel), 16);
    pNew->n08 = nSize;
    pNew->p34 = SkinBurn_CopyAligned(pBase, &nOffset, pOld->p34, pOld->n14 * 0x20, 16);
    pNew->p38 = SkinBurn_CopyAligned(pBase, &nOffset, pOld->p38, 0x50, 16);
    pNew->p3C = SkinBurn_CopyAligned(pBase, &nOffset, pOld->p3C, pOld->n0C * 0x50, 16);
    pNew->p54 = SkinBurn_CopyAligned(pBase, &nOffset, pOld->p54, pOld->n50 * sizeof(SkinModel54), 16);
    pNew->p44 = SkinBurn_CopyAligned(pBase, &nOffset, pOld->p44, pOld->n40 * sizeof(SkinModel44), 16);
    aIndexes = SkinBurn_TakeAligned(pBase, &nOffset, nIndexes * sizeof(s32), 16);

    nFirst = 0;
    nEntries = pNew->n40;
    for (i = 0; i < nEntries; i++) {
        pEntry = &pNew->p44[i];
        n = pEntry->n8;
        for (j = 0; j < n; j++) {
            aIndexes[j + nFirst] = pEntry->p4[j];
        }
        pEntry->p4 = &aIndexes[nFirst];
        nFirst += pEntry->n8;
    }
    pSkin->pModel = pNew;
    StaticMem_Free(pOld);
}

// Brings the skin's model in line with pBurn's burnt description: renumbers and packs its mesh
// entries, drops the matrices no option uses, and packs it into one allocation.
void SkinBurn_BurnModel(Skin* pSkin, HwsBurn* pBurn) {
    SkinBurn_RenumberMeshEntries(pSkin, pBurn);
    SkinBurn_DropUnusedMatrices(pSkin);
    SkinBurn_PackModel(pSkin);
}

// The callback SkinBurn_BurnSkin hands the burn (hwsBurn.c calls it on each material entry):
// patches the entry for copy 0's chosen sets (SkinPart_ApplySetsToMaterialEntry), then clears flag
// 2 and n16, so the burnt entry keeps the texture scale and offset it got and no longer looks one
// up in SkinDesc.pB8.
void SkinBurn_BakeMaterialEntry(Skin* pSkin, SkinDesc14* pEntry) {
    SkinPart_ApplySetsToMaterialEntry(pSkin, pEntry, NULL, NULL, 0);
    pEntry->u08 &= ~2;
    pEntry->n16 = 0;
}

// Burns the skin down to its current look (SkinPart_BurnBodySkin). Each part keeps only its current
// option (copy 0), and only its current variant unless aParts lists it (then every variant stays).
// The morph targets aList lists stay, set to weight 0; every other one is blended into the meshes
// at its current weight (SkinMorph_CreateBlended) and dropped (hwsBurn a1C). The burnt description
// replaces the model's (the old one is freed) and the skin's first override table points at it;
// then the model is burnt to match (SkinBurn_BurnModel). aParts and aList end with -1.
void SkinBurn_BurnSkin(Skin* pSkin, s32* aParts, s32* aList) {
    HwsBurn* pBurn;
    s32 nCount;
    s32 i;
    s32 j;
    SkinDesc* pOld;
    SkinDesc* pDesc;
    HwsMemBlock* pBlock;
    HwsOverrideTable* pTable;

    if (pSkin->pModel == NULL || pSkin->pModel->pDesc == NULL) return;

    pBurn = fn_801104AC(pSkin->pModel->pDesc);
    fn_801109F0(pBurn, SkinBurn_BakeMaterialEntry, pSkin);
    nCount = SkinPart_GetNumParts(pSkin);
    for (i = 0; i < nCount; i++) {
        for (j = 0; aParts[j] >= 0; j++) {
            if (i == aParts[j]) break;
        }
        if (aParts[j] < 0) {
            fn_801109FC(pBurn, i, SkinPart_GetPartVariant(pSkin, i, 0));
        }
        fn_80110A0C(pBurn, i, SkinPart_GetPartOption(pSkin, i, 0));
    }

    nCount = SkinMorph_GetNumTargets(pSkin->pModel->pDesc);
    for (i = 0; i < nCount; i++) {
        for (j = 0; aList[j] >= 0; j++) {
            if (i == aList[j]) break;
        }
        if (aList[j] < 0) {
            fn_80110A24(pBurn, i);
        } else {
            SkinMorph_SetTargetWeight(pSkin, i, 0.0f);
        }
    }

    SkinMorph_CreateBlended(pSkin, &pBlock, &pTable);
    fn_80110A1C(pBurn, pTable);
    pDesc = fn_80111EB0(pBurn);
    pOld = pSkin->pModel->pDesc;
    pSkin->pModel->pDesc = pDesc;
    StaticMem_Free(pOld);
    SkinBurn_BurnModel(pSkin, pBurn);
    fn_801108B0(pBurn);
    SkinMorph_FreeBlended(pSkin, pBlock, pTable);
    if (pSkin->a10A0[0] != NULL) {
        pSkin->a10A0[0]->pDesc = pDesc;
    }
}
