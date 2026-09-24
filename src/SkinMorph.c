// SkinMorph.c (EA's name, from its asserts): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"
#include "platform.h"
#include "engine.h"
#include "charstate.h"

// Unpacks nVerts vertices into the work area: positions (four s16 each) and normals (four s8
// each) to 16.16 fixed point.
void fn_8011C068(SkinMorphWork* pWork, s16* pPos, s8* pNrm, u32 nVerts) {
    SkinMorphVert* pVert = pWork->aVerts;
    u32 i;

    for (i = 0; i < nVerts; i++) {
        pVert->aPos[0] = pPos[0] << 16;
        pVert->aPos[1] = pPos[1] << 16;
        pVert->aPos[2] = pPos[2] << 16;
        pVert->aPos[3] = pPos[3] << 16;
        pPos += 4;
        pVert->aNrm[0] = pNrm[0] << 16;
        pVert->aNrm[1] = pNrm[1] << 16;
        pVert->aNrm[2] = pNrm[2] << 16;
        pVert->aNrm[3] = pNrm[3] << 16;
        pNrm += 4;
        pVert++;
    }
}

s32  fn_8011CDE8(Skin* pSkin);

// Packs the work area's nVerts vertices back: positions as four s16 each, normals as four s8.
void fn_8011C1FC(SkinMorphWork* pWork, s16* pPos, s8* pNrm, u32 nVerts) {
    SkinMorphVert* pVert = pWork->aVerts;
    u32 i;

    for (i = 0; i < nVerts; i++) {
        pPos[0] = pVert->aPos[0] >> 16;
        pPos[1] = pVert->aPos[1] >> 16;
        pPos[2] = pVert->aPos[2] >> 16;
        pPos[3] = pVert->aPos[3] >> 16;
        pPos += 4;
        pNrm[0] = pVert->aNrm[0] >> 16;
        pNrm[1] = pVert->aNrm[1] >> 16;
        pNrm[2] = pVert->aNrm[2] >> 16;
        pNrm[3] = pVert->aNrm[3] >> 16;
        pVert++;
        pNrm += 4;
    }
}

// Adds a morph target to the vertices it moves: nVerts position offsets (four s16 each), normal
// offsets (four s8 each) and vertex numbers (s16), each offset times fScale.
void fn_8011C27C(SkinMorphWork* pWork, void* pTarget, u32 nVerts, f32 fScale) {
    s16* pPos = pTarget;
    uptr uNrmSize = nVerts * 4;
    s8* pNrm = (s8*)(pPos + nVerts * 4);
    // fake match: the vertex numbers' address added as integers, size first (EA's add order;
    // pNrm + nVerts * 4 puts the pointer first)
    s16* pIndex = (s16*)(uNrmSize + (uptr)pNrm);
    SkinMorphVert* pVert;
    u32 i;

    for (i = 0; i < nVerts; i++) {
        pVert = &pWork->aVerts[*pIndex];
        pVert->aPos[0] = pPos[0] * fScale + pVert->aPos[0];
        pVert->aPos[1] = pPos[1] * fScale + pVert->aPos[1];
        pVert->aPos[2] = pPos[2] * fScale + pVert->aPos[2];
        pVert->aNrm[0] = pNrm[0] * fScale + pVert->aNrm[0];
        pVert->aNrm[1] = pNrm[1] * fScale + pVert->aNrm[1];
        pVert->aNrm[2] = pNrm[2] * fScale + pVert->aNrm[2];
        pIndex++;
        pPos += 4;
        pNrm += 4;
    }
}

// Swaps the work area's two buffers.
void fn_8011C46C(SkinMorphWork* pWork) {
    void* p = pWork->p10020;

    pWork->p10020 = pWork->p10024;
    pWork->p10024 = p;
}

// Points the second buffer at a target mesh's vertices; gives its vertex count.
s32 fn_8011C484(SkinMorphWork* pWork, SkinMesh* pMesh) {
    s32 nVerts = pMesh->n8;

    pWork->p10024 = pMesh->pBits;
    return nVerts;
}

// Unpacks a mesh's vertices (and the bytes after them) into the work area.
void fn_8011C49C(SkinMorphWork* pWork, SkinMesh* pMesh) {
    s32 nVerts = pMesh->n8;
    SkinMeshBit* aVerts = pMesh->pBits;

    pWork->n10018 = nVerts;
    fn_8011C068(pWork, (s16*)aVerts, (s8*)(aVerts + nVerts), nVerts);
}

// Packs the work area's vertices back into pDst.
void fn_8011C4D4(SkinMorphWork* pWork, u8* pDst) {
    u32 nVerts = pWork->n10018;

    fn_8011C1FC(pWork, (s16*)pDst, (s8*)(pDst + (nVerts << 3)), nVerts);  // 8 bytes a vertex
}

// Adds nVerts vertices of the target in the first buffer, scaled by fWeight, to the work area
// (in 16.16 fixed point: the scale is fWeight * 65536, rounded).
void fn_8011C504(SkinMorphWork* pWork, s32 nVerts, f32 fWeight) {
    fn_8011C27C(pWork, pWork->p10020, nVerts, (s32)(65536.0f * fWeight + 0.5f));
}

// Clears the work area's skin and gives it.
SkinMorphWork* fn_8011C564(void) {
    SkinMorphWork* pWork = lbl_80281880;

    pWork->pDesc = NULL;
    pWork->afWeights = NULL;
    pWork->nMorphs = 0;
    return pWork;
}

void fn_8011C580(SkinMorphWork* pWork, SkinDesc* pDesc) {
    pWork->pDesc = pDesc;
}

void fn_8011C58C(SkinMorphWork* pWork, f32* afWeights, s32 nMorphs) {
    pWork->afWeights = afWeights;
    pWork->nMorphs = nMorphs;
}

void fn_8011C59C(SkinMorphWork* pWork, HwsOverrideTable* pTable) {
    pWork->pTable = pTable;
}

void fn_8011C5A8(SkinMorphWork* pWork, HwsMemBlock* pBlock) {
    pWork->pBlock = pBlock;
}

// Picks the meshes to blend for vertex set nSet of p44 entry pEntry: each morph target with a
// weight other than 0 that has that set. Gives how many (in apTargets and afTargets).
s32 fn_8011C5B4(SkinMorphWork* pWork, SkinDesc44* pEntry, int nSet) {
    s32 i;
    s32 nPicked = 0;
    s32 nFirst;
    SkinDesc44* pTarget;
    SkinMesh* aMeshes;
    s32* aIndexes;
    s32 nCount;
    SkinDesc* pDesc;
    s32 nMesh;
    f32 fWeight;
    s32 nLeft;

    nFirst = pEntry->n18;
    pDesc = pWork->pDesc;
    aMeshes = pDesc->p34;
    aIndexes = pDesc->p3C;
    pTarget = &pDesc->p44[pEntry->n10];
    nLeft = pWork->nMorphs - nFirst;
    nCount = pEntry->n14;
    if (nCount > nLeft) {
        nCount = nLeft;
    }
    for (i = 0; i < nCount; i++, pTarget++) {
        if (nSet < pTarget->n8) {
            fWeight = pWork->afWeights[nFirst + i];
            if (fWeight != 0.0f) {
                nMesh = aIndexes[pTarget->n0 + nSet];
                if (nMesh >= 0) {
                    pWork->apTargets[nPicked] = &aMeshes[nMesh];
                    pWork->afTargets[nPicked] = fWeight;
                    nPicked++;
                }
            }
        }
    }
    return nPicked;
}

// Blends the morph targets of p44 entry n into its meshes' overrides: each mesh with flags
// 0x100000 and 0x10 gets its own vertices plus every picked target times its weight, or a plain
// copy when no target is picked.
void fn_8011C68C(SkinMorphWork* pWork, int n) {
    SkinDesc* pDesc = pWork->pDesc;
    SkinIterArgs args;
    u8 aBuf[0x20];      // the iterator's buffer; its size is not known
    SkinIter* pIter;
    SkinDesc44* pEntry;
    SkinMesh* pMesh;
    s32 nSet;
    s32 nPicked;
    u8* pDst;
    s32 i;
    s32 nVerts;
    s32 nNext;

    if (pDesc == NULL || n < 0 || n >= pDesc->n40) {
        return;
    }
    pEntry = &pDesc->p44[n];
    if (!(pEntry->u24 & 2)) {
        return;
    }
    args.pDesc = pDesc;
    args.n = n;
    pIter = fn_80113910(aBuf, &args);
    nSet = 0;
    while (fn_800CEEC0(pIter)) {
        pMesh = fn_800CEEF4(pIter);
        if ((pMesh->uFlags & 0x100010) == 0x100010) {
            nPicked = fn_8011C5B4(pWork, pEntry, nSet);
            nSet++;
            pDst = fn_80112A80(pWork->pBlock, pWork->pTable, fn_800CEEFC(pIter), 1);
            if (pDst != NULL) {
                if (nPicked == 0) {
                    memcpy(pDst, pMesh->pBits, pMesh->nSize);
                } else {
                    fn_8011C49C(pWork, pMesh);
                    nVerts = fn_8011C484(pWork, pWork->apTargets[0]);
                    for (i = 1; i < nPicked; i++) {
                        fn_8011C46C(pWork);
                        nNext = fn_8011C484(pWork, pWork->apTargets[i]);
                        fn_8011C504(pWork, nVerts, pWork->afTargets[i - 1]);
                        nVerts = nNext;
                    }
                    fn_8011C46C(pWork);
                    fn_8011C504(pWork, nVerts, pWork->afTargets[i - 1]);
                    fn_8011C4D4(pWork, pDst);
                }
            }
        }
        fn_800CEEC8(pIter);
    }
}

// Every caller passes the work area; it is unused.
void fn_8011C84C(SkinMorphWork* pWork) {
}

// The morph targets a skin description needs: the highest n14 + n18 of its p44 entries that have
// morph targets.
s32 fn_8011C850(SkinDesc* pDesc) {
    SkinDesc44* pEntry;
    s32 nMax;
    s32 i;

    nMax = 0;
    for (i = 0; i < pDesc->n40; i++) {
        pEntry = &pDesc->p44[i];
        if ((pEntry->u24 & 2) && pEntry->n8 > 0 && pEntry->n18 + pEntry->n14 > nMax) {
            nMax = pEntry->n18 + pEntry->n14;
        }
    }
    return nMax;
}

// Whether a morph target of p44 entry n that changed for view nView has a mesh to draw.
u8 fn_8011C8B0(Skin* pSkin, int nView, int n) {
    SkinDesc* pDesc;
    SkinDesc44* pEntry;
    SkinMorphState* pMorph;
    s32 i;
    s32 nFirst;
    s32 nCount;
    SkinDesc44* pTarget;
    s32 j;
    s32 nMesh;

    pMorph = pSkin->pMorph;
    pDesc = pSkin->pModel->pDesc;
    pEntry = &pDesc->p44[n];
    if (!(pEntry->u24 & 2)) {
        return 0;
    }
    nFirst = pEntry->n18;
    nCount = pEntry->n14;
    for (i = 0; i < nCount; i++) {
        if (fn_8001E9CC(pMorph->aChanged[nView], nFirst + i)) {
            pTarget = &pDesc->p44[pEntry->n10 + i];
            for (j = 0; j < pTarget->n8; j++) {
                nMesh = pDesc->p3C[pTarget->n0 + j];
                if (nMesh >= 0 && pDesc->p34[nMesh].n8 > 0) {
                    break;
                }
            }
            if (j != pTarget->n8) {
                return 1;
            }
        }
    }
    return 0;
}

// Makes the skin's morph state: every weight 0, nothing changed.
void fn_8011C9B0(Skin* pSkin) {
    SkinMorphState* pMorph;
    s32 nMorphs;
    s32 nBytes;

    if (pSkin->pModel->pDesc == NULL) {
        return;
    }
    nMorphs = fn_8011C850(pSkin->pModel->pDesc);
    if (nMorphs == 0) {
        return;
    }
    pMorph = fn_80009B34(sizeof(SkinMorphState), 2, 16, "SkinMorph.c", 91);
    memset(pMorph, 0, sizeof(SkinMorphState));
    pMorph->nMorphs = nMorphs;
    nBytes = (nMorphs + 31) / 32 * sizeof(u32);
    pMorph->aChanged[0] = fn_80009B34(nBytes, 2, 16, "SkinMorph.c", 95);
    pMorph->aChanged[1] = fn_80009B34(nBytes, 2, 16, "SkinMorph.c", 96);
    fn_8001E938(pMorph->aChanged[0], nMorphs);
    fn_8001E938(pMorph->aChanged[1], nMorphs);
    pMorph->afWeights = fn_80009B34(nMorphs * sizeof(f32), 2, 16, "SkinMorph.c", 100);
    memset(pMorph->afWeights, 0, nMorphs * sizeof(f32));
    pSkin->pMorph = pMorph;
}

// Sets the weight of morph target nMorph and marks it changed.
void fn_8011CADC(Skin* pSkin, int nMorph, f32 fWeight) {
    SkinMorphState* pMorph = pSkin->pMorph;

    if (pMorph == NULL || nMorph < 0 || nMorph >= pMorph->nMorphs) {
        return;
    }
    if (fWeight != pMorph->afWeights[nMorph]) {
        pMorph->afWeights[nMorph] = fWeight;
        fn_8001EA34(pMorph->aChanged[0], nMorph);
        fn_8001EA34(pMorph->aChanged[1], nMorph);
    }
}

// Blends the morph targets that changed for view nView into the skin's meshes.
void fn_8011CB5C(Skin* pSkin, int nView) {
    SkinMorphWork* pWork;
    s32 i;
    s32 nEntries;

    if (pSkin->pMorph == NULL) {
        return;
    }
    pWork = fn_8011C564();
    fn_8011C580(pWork, pSkin->pModel->pDesc);
    fn_8011C58C(pWork, pSkin->pMorph->afWeights, pSkin->pMorph->nMorphs);
    fn_8011C59C(pWork, pSkin->a10A0[nView]);
    fn_8011C5A8(pWork, pSkin->a1098[nView]);
    nEntries = pSkin->pModel->pDesc->n40;
    for (i = 0; i < nEntries; i++) {
        if (fn_8011C8B0(pSkin, nView, i)) {
            fn_8011C68C(pWork, i);
        }
    }
    fn_8011C84C(pWork);
    fn_8001E938(pSkin->pMorph->aChanged[nView], pSkin->pMorph->nMorphs);
}

// Makes a mesh table and a memory block for the skin's morphed meshes and blends every morph
// target into them; gives both (NULL without a morph state or such meshes).
void fn_8011CC40(Skin* pSkin, HwsMemBlock** ppBlock, HwsOverrideTable** ppTable) {
    HwsMemBlock* pBlock;
    HwsOverrideTable* pTable;
    SkinMorphWork* pWork;
    SkinDesc* pDesc;
    s32 i;
    s32 nEntries;
    s32 nSize;

    nSize = fn_8011CDE8(pSkin);
    *ppBlock = NULL;
    *ppTable = NULL;
    if (pSkin->pMorph == NULL || nSize == 0) {
        return;
    }
    pTable = fn_80112A34(pSkin->pModel->pDesc, 0);
    pBlock = fn_801128EC(pSkin->pModel->pDesc, nSize);
    pDesc = pSkin->pModel->pDesc;
    pWork = fn_8011C564();
    fn_8011C580(pWork, pDesc);
    fn_8011C58C(pWork, pSkin->pMorph->afWeights, pSkin->pMorph->nMorphs);
    fn_8011C59C(pWork, pTable);
    fn_8011C5A8(pWork, pBlock);
    nEntries = pDesc->n40;
    for (i = 0; i < nEntries; i++) {
        fn_8011C68C(pWork, i);
    }
    fn_8011C84C(pWork);
    *ppBlock = pBlock;
    *ppTable = pTable;
}

// Frees the block and table fn_8011CC40 made (SkinBurn.c calls it); pSkin is unused.
void fn_8011CD3C(Skin* pSkin, HwsMemBlock* pBlock, HwsOverrideTable* pTable) {
    if (pBlock != NULL) {
        fn_80112910(pBlock);
    }
    if (pTable != NULL) {
        fn_80112A58(pTable);
    }
}

// Frees the skin's morph state (Skin.c calls it).
void fn_8011CD84(Skin* pSkin) {
    if (pSkin->pMorph != NULL) {
        fn_80009E70(pSkin->pMorph->afWeights);
        fn_80009E70(pSkin->pMorph->aChanged[0]);
        fn_80009E70(pSkin->pMorph->aChanged[1]);
        fn_80009E70(pSkin->pMorph);
        pSkin->pMorph = NULL;
    }
}

// The bytes of the skin's meshes that have both flags 0x100000 and 0x10.
s32 fn_8011CDE8(Skin* pSkin) {
    SkinDesc* pDesc;
    SkinMesh* pMesh;
    s32 nSize;
    s32 i;

    if (pSkin == NULL || (pDesc = pSkin->pModel->pDesc) == NULL) {
        return 0;
    }
    nSize = 0;
    for (i = 0; i < pDesc->n30; i++) {
        pMesh = &pDesc->p34[i];
        if ((pMesh->uFlags & 0x100010) == 0x100010) {
            nSize += pMesh->nSize;
        }
    }
    return nSize;
}

// Marks every morph target changed.
void fn_8011CE58(Skin* pSkin) {
    if (pSkin == NULL || pSkin->pMorph == NULL) {
        return;
    }
    fn_8001E8A4(pSkin->pMorph->aChanged[0], pSkin->pMorph->nMorphs);
    fn_8001E8A4(pSkin->pMorph->aChanged[1], pSkin->pMorph->nMorphs);
}
