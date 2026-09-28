// SkinMorph.c (EA's name, from its asserts; TW07's golf/animation/SkinMorph.c keeps
// SkinMorph_SetTargetWeight, SkinMorph_Destroy and SkinMorph_UpdateAllTargets): the morph targets
// of a skin, such as the golfer's body shape the character sliders set (CharSliders.c). A target
// moves some vertices of some of the skin's meshes by offsets; the skin keeps a weight per target
// (Skin.pMorph, SkinMorphState) and, per view, a bit per target whose weight changed. Each frame
// SkinMorph_Update blends a view's changed targets into that view's copies of the morphed meshes
// (hwsOverride_Gc.c's override table and memory block), in a work area of 16.16 fixed-point
// vertices. SkinBurn.c blends them in for good (SkinMorph_CreateBlended).

#include "game_types.h"
#include "platform.h"
#include "engine.h"
#include "charstate.h"

s32  SkinMorph_GetBlendSize(Skin* pSkin);

SkinMorphWork gSkinMorphWork;                        // the one morph work area
SkinMorphWork* gpSkinMorphWork = &gSkinMorphWork;   // what SkinMorph_GetWork hands out

// Unpacks nVerts vertices into the work area in 16.16 fixed point: positions (four s16 each: x, y,
// z and the vertex's matrix bit, SkinMeshBit.nBit) and normals (four s8 each).
void SkinMorph_UnpackVerts(SkinMorphWork* pWork, s16* pPos, s8* pNrm, u32 nVerts) {
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

// Packs the work area's nVerts vertices back out of 16.16 fixed point, in the layout
// SkinMorph_UnpackVerts reads: positions as four s16 each, normals as four s8 each.
void SkinMorph_PackVerts(SkinMorphWork* pWork, s16* pPos, s8* pNrm, u32 nVerts) {
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

// Adds a morph target to the work area's vertices. pTarget holds nVerts position offsets (four s16
// each), then nVerts normal offsets (four s8 each), then the numbers of the nVerts vertices they
// move (s16 each). Each offset's x, y and z are added times fScale (the weight in 16.16 fixed
// point: 65536 is 1.0); the fourth value (the matrix bit) is left alone.
void SkinMorph_AddTarget(SkinMorphWork* pWork, void* pTarget, u32 nVerts, f32 fScale) {
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

// Swaps the work area's current target (pCurTarget, the one SkinMorph_AddCurrentTarget adds) and
// next target (pNextTarget, the one SkinMorph_SetNextTarget sets).
void SkinMorph_SwapTargets(SkinMorphWork* pWork) {
    void* p = pWork->pCurTarget;

    pWork->pCurTarget = pWork->pNextTarget;
    pWork->pNextTarget = p;
}

// Makes a target mesh's data (its pBits) the work area's next target; gives how many vertices it
// moves (its n8).
s32 SkinMorph_SetNextTarget(SkinMorphWork* pWork, SkinMesh* pMesh) {
    s32 nVerts = pMesh->n8;

    pWork->pNextTarget = pMesh->pBits;
    return nVerts;
}

// Unpacks a morphed mesh into the work area (SkinMorph_UnpackVerts): its n8 vertices (SkinMeshBit,
// 8 bytes each) and the normals stored after them.
void SkinMorph_LoadMesh(SkinMorphWork* pWork, SkinMesh* pMesh) {
    s32 nVerts = pMesh->n8;
    SkinMeshBit* aVerts = pMesh->pBits;

    pWork->nVerts = nVerts;
    SkinMorph_UnpackVerts(pWork, (s16*)aVerts, (s8*)(aVerts + nVerts), nVerts);
}

// Packs the work area's vertices (SkinMorph_PackVerts) into pDst: nVerts vertices of 8 bytes, then
// their normals.
void SkinMorph_StoreMesh(SkinMorphWork* pWork, u8* pDst) {
    u32 nVerts = pWork->nVerts;

    SkinMorph_PackVerts(pWork, (s16*)pDst, (s8*)(pDst + (nVerts << 3)), nVerts);  // 8 bytes a vertex
}

// Adds the work area's current target (pCurTarget), which moves nVerts vertices, at weight fWeight:
// SkinMorph_AddTarget's scale is fWeight in 16.16 fixed point, rounded to a whole number (fWeight *
// 65536 + 0.5).
void SkinMorph_AddCurrentTarget(SkinMorphWork* pWork, s32 nVerts, f32 fWeight) {
    SkinMorph_AddTarget(pWork, pWork->pCurTarget, nVerts, (s32)(65536.0f * fWeight + 0.5f));
}

// The morph work area (there is one, gpSkinMorphWork), with its description and weights cleared:
// the caller sets them, the override table and the memory block before blending.
SkinMorphWork* SkinMorph_GetWork(void) {
    SkinMorphWork* pWork = gpSkinMorphWork;

    pWork->pDesc = NULL;
    pWork->afWeights = NULL;
    pWork->nMorphs = 0;
    return pWork;
}

void SkinMorph_SetWorkDesc(SkinMorphWork* pWork, SkinDesc* pDesc) {
    pWork->pDesc = pDesc;
}

void SkinMorph_SetWorkWeights(SkinMorphWork* pWork, f32* afWeights, s32 nMorphs) {
    pWork->afWeights = afWeights;
    pWork->nMorphs = nMorphs;
}

void SkinMorph_SetWorkOverrideTable(SkinMorphWork* pWork, HwsOverrideTable* pTable) {
    pWork->pTable = pTable;
}

void SkinMorph_SetWorkMemBlock(SkinMorphWork* pWork, HwsMemBlock* pBlock) {
    pWork->pBlock = pBlock;
}

// Picks the target meshes to add to morphed mesh nSet of entry pEntry (SkinDesc.p44): of each of
// the entry's morph targets (weights from its n18 on, at most as many as the work area has) with a
// weight other than 0, its mesh nSet, if it has one. Fills apTargets and afTargets; gives how many.
s32 SkinMorph_PickTargets(SkinMorphWork* pWork, SkinDesc44* pEntry, int nSet) {
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

// Blends the morph targets of entry nEntry (SkinDesc.p44; one with morph targets, u24 & 2) into
// its morphed meshes. Each of its meshes with flags 0x100000 and 0x10 gets its override memory
// (the work area's table and block; kept from before) filled with its own vertices plus every
// picked target (SkinMorph_PickTargets) times its weight, or a plain copy of them when no target
// has a weight.
void SkinMorph_BlendEntry(SkinMorphWork* pWork, int nEntry) {
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

    if (pDesc == NULL || nEntry < 0 || nEntry >= pDesc->n40) {
        return;
    }
    pEntry = &pDesc->p44[nEntry];
    if (!(pEntry->u24 & 2)) {
        return;
    }
    args.pDesc = pDesc;
    args.nEntry = nEntry;
    pIter = fn_80113910(aBuf, &args);
    nSet = 0;
    while (SkinIter_IsValid(pIter)) {
        pMesh = SkinIter_GetMesh(pIter);
        if ((pMesh->uFlags & 0x100010) == 0x100010) {
            nPicked = SkinMorph_PickTargets(pWork, pEntry, nSet);
            nSet++;
            pDst = fn_80112A80(pWork->pBlock, pWork->pTable, SkinIter_GetIndex(pIter), 1);
            if (pDst != NULL) {
                if (nPicked == 0) {
                    memcpy(pDst, pMesh->pBits, pMesh->nSize);
                } else {
                    SkinMorph_LoadMesh(pWork, pMesh);
                    nVerts = SkinMorph_SetNextTarget(pWork, pWork->apTargets[0]);
                    for (i = 1; i < nPicked; i++) {
                        SkinMorph_SwapTargets(pWork);
                        nNext = SkinMorph_SetNextTarget(pWork, pWork->apTargets[i]);
                        SkinMorph_AddCurrentTarget(pWork, nVerts, pWork->afTargets[i - 1]);
                        nVerts = nNext;
                    }
                    SkinMorph_SwapTargets(pWork);
                    SkinMorph_AddCurrentTarget(pWork, nVerts, pWork->afTargets[i - 1]);
                    SkinMorph_StoreMesh(pWork, pDst);
                }
            }
        }
        SkinIter_Next(pIter);
    }
}

// Does nothing: called with the work area when a blend is done (pWork is unused).
void SkinMorph_EndWork(SkinMorphWork* pWork) {
}

// How many morph target weights a skin description uses: the highest n18 + n14 (its first target's
// number plus its count) of its SkinDesc.p44 entries that have morph targets and meshes; 0: none.
s32 SkinMorph_GetNumTargets(SkinDesc* pDesc) {
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

// Whether entry nEntry (SkinDesc.p44) needs blending for view nView: it has morph targets, and one
// that changed since that view last blended moves at least one vertex.
u8 SkinMorph_EntryNeedsBlend(Skin* pSkin, int nView, int nEntry) {
    SkinDesc* pDesc;
    SkinDesc44* pEntry;
    SkinMorphState* pMorphState;
    s32 i;
    s32 nFirst;
    s32 nCount;
    SkinDesc44* pTarget;
    s32 j;
    s32 nMesh;

    pMorphState = pSkin->pMorph;
    pDesc = pSkin->pModel->pDesc;
    pEntry = &pDesc->p44[nEntry];
    if (!(pEntry->u24 & 2)) {
        return 0;
    }
    nFirst = pEntry->n18;
    nCount = pEntry->n14;
    for (i = 0; i < nCount; i++) {
        if (BitArray_TestBit(pMorphState->aChanged[nView], nFirst + i)) {
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

// Gives the skin its morph state (Skin.pMorph) when its description has morph targets: every weight
// 0 and no target changed. SKN_Create calls it.
void SkinMorph_Create(Skin* pSkin) {
    SkinMorphState* pMorphState;
    s32 nMorphs;
    s32 nBytes;

    if (pSkin->pModel->pDesc == NULL) {
        return;
    }
    nMorphs = SkinMorph_GetNumTargets(pSkin->pModel->pDesc);
    if (nMorphs == 0) {
        return;
    }
    pMorphState = StaticMem_Alloc(sizeof(SkinMorphState), 2, 16, "SkinMorph.c", 91);
    memset(pMorphState, 0, sizeof(SkinMorphState));
    pMorphState->nMorphs = nMorphs;
    nBytes = (nMorphs + 31) / 32 * sizeof(u32);
    pMorphState->aChanged[0] = StaticMem_Alloc(nBytes, 2, 16, "SkinMorph.c", 95);
    pMorphState->aChanged[1] = StaticMem_Alloc(nBytes, 2, 16, "SkinMorph.c", 96);
    BitArray_ClearArray(pMorphState->aChanged[0], nMorphs);
    BitArray_ClearArray(pMorphState->aChanged[1], nMorphs);
    pMorphState->afWeights = StaticMem_Alloc(nMorphs * sizeof(f32), 2, 16, "SkinMorph.c", 100);
    memset(pMorphState->afWeights, 0, nMorphs * sizeof(f32));
    pSkin->pMorph = pMorphState;
}

// Sets a morph target's weight. A new value marks the target changed for both views, so
// SkinMorph_Update blends it again; out of range, or no morph state: nothing.
void SkinMorph_SetTargetWeight(Skin* pSkin, int nMorph, f32 fWeight) {
    SkinMorphState* pMorphState = pSkin->pMorph;

    if (pMorphState == NULL || nMorph < 0 || nMorph >= pMorphState->nMorphs) {
        return;
    }
    if (fWeight != pMorphState->afWeights[nMorph]) {
        pMorphState->afWeights[nMorph] = fWeight;
        BitArray_SetBit(pMorphState->aChanged[0], nMorph);
        BitArray_SetBit(pMorphState->aChanged[1], nMorph);
    }
}

// Blends the morph targets that changed for view nView into the skin's morphed meshes (that view's
// override table and memory block, Skin.apOverride and apMemBlock), then clears the view's changed
// bits. SKN_PoseCharacter calls it each frame.
void SkinMorph_Update(Skin* pSkin, int nView) {
    SkinMorphWork* pWork;
    s32 i;
    s32 nEntries;

    if (pSkin->pMorph == NULL) {
        return;
    }
    pWork = SkinMorph_GetWork();
    SkinMorph_SetWorkDesc(pWork, pSkin->pModel->pDesc);
    SkinMorph_SetWorkWeights(pWork, pSkin->pMorph->afWeights, pSkin->pMorph->nMorphs);
    SkinMorph_SetWorkOverrideTable(pWork, pSkin->apOverride[nView]);
    SkinMorph_SetWorkMemBlock(pWork, pSkin->apMemBlock[nView]);
    nEntries = pSkin->pModel->pDesc->n40;
    for (i = 0; i < nEntries; i++) {
        if (SkinMorph_EntryNeedsBlend(pSkin, nView, i)) {
            SkinMorph_BlendEntry(pWork, i);
        }
    }
    SkinMorph_EndWork(pWork);
    BitArray_ClearArray(pSkin->pMorph->aChanged[nView], pSkin->pMorph->nMorphs);
}

// Blends every morph target, at its current weight, into a new override table and memory block for
// the skin's morphed meshes (StaticMem mode 1), and gives both: NULL and NULL without a morph state
// or morphed meshes. SkinBurn_BurnSkin burns them into the skin; SkinMorph_FreeBlended frees them.
void SkinMorph_CreateBlended(Skin* pSkin, HwsMemBlock** ppBlock, HwsOverrideTable** ppTable) {
    HwsMemBlock* pBlock;
    HwsOverrideTable* pTable;
    SkinMorphWork* pWork;
    SkinDesc* pDesc;
    s32 i;
    s32 nEntries;
    s32 nSize;

    nSize = SkinMorph_GetBlendSize(pSkin);
    *ppBlock = NULL;
    *ppTable = NULL;
    if (pSkin->pMorph == NULL || nSize == 0) {
        return;
    }
    pTable = fn_80112A34(pSkin->pModel->pDesc, 0);
    pBlock = fn_801128EC(pSkin->pModel->pDesc, nSize);
    pDesc = pSkin->pModel->pDesc;
    pWork = SkinMorph_GetWork();
    SkinMorph_SetWorkDesc(pWork, pDesc);
    SkinMorph_SetWorkWeights(pWork, pSkin->pMorph->afWeights, pSkin->pMorph->nMorphs);
    SkinMorph_SetWorkOverrideTable(pWork, pTable);
    SkinMorph_SetWorkMemBlock(pWork, pBlock);
    nEntries = pDesc->n40;
    for (i = 0; i < nEntries; i++) {
        SkinMorph_BlendEntry(pWork, i);
    }
    SkinMorph_EndWork(pWork);
    *ppBlock = pBlock;
    *ppTable = pTable;
}

// Frees the memory block and override table SkinMorph_CreateBlended made; pSkin is unused.
void SkinMorph_FreeBlended(Skin* pSkin, HwsMemBlock* pBlock, HwsOverrideTable* pTable) {
    if (pBlock != NULL) {
        fn_80112910(pBlock);
    }
    if (pTable != NULL) {
        fn_80112A58(pTable);
    }
}

// Frees the skin's morph state and its arrays, if it has one.
void SkinMorph_Destroy(Skin* pSkin) {
    if (pSkin->pMorph != NULL) {
        StaticMem_Free(pSkin->pMorph->afWeights);
        StaticMem_Free(pSkin->pMorph->aChanged[0]);
        StaticMem_Free(pSkin->pMorph->aChanged[1]);
        StaticMem_Free(pSkin->pMorph);
        pSkin->pMorph = NULL;
    }
}

// The bytes the skin's morphed meshes take (those with flags 0x100000 and 0x10): the size of a
// memory block for their blended copies. 0 without a skin or description.
s32 SkinMorph_GetBlendSize(Skin* pSkin) {
    SkinDesc* pDesc;
    SkinMesh* pMesh;
    s32 nSize;
    s32 i;

    if (pSkin == NULL || (pDesc = pSkin->pModel->pDesc) == NULL) {
        return 0;
    }
    nSize = 0;
    for (i = 0; i < pDesc->nOverrideMeshes; i++) {
        pMesh = &pDesc->p34[i];
        if ((pMesh->uFlags & 0x100010) == 0x100010) {
            nSize += pMesh->nSize;
        }
    }
    return nSize;
}

// Marks every morph target changed for both views, so each view's next SkinMorph_Update blends them
// all.
void SkinMorph_UpdateAllTargets(Skin* pSkin) {
    if (pSkin == NULL || pSkin->pMorph == NULL) {
        return;
    }
    BitArray_FillArray(pSkin->pMorph->aChanged[0], pSkin->pMorph->nMorphs);
    BitArray_FillArray(pSkin->pMorph->aChanged[1], pSkin->pMorph->nMorphs);
}
