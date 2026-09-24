// hwsOverride_Gc.c (EA's name, from its asserts): per-mesh overrides of a skin's meshes on the
// GameCube renderer: a table with a pointer per mesh of a SkinDesc, filled from a block of memory
// sized for the meshes that take one (flag 0x100000). Skin.c and SkinMorph.c use them.

#include "engine.h"
#include "platform.h"
#include "charstate.h"

s32 fn_80112964(SkinDesc* pDesc);

// The memory the meshes of pDesc with flag 0x100000 need together.
s32 fn_80112804(SkinDesc* pDesc) {
    s32 nSize = 0;
    int i;

    for (i = 0; i < pDesc->n30; i++) {
        if (pDesc->p34[i].uFlags & 0x100000) {
            nSize += pDesc->p34[i].nSize;
        }
    }
    return nSize;
}

// A memory block of nSize bytes (0: what pDesc's overridden meshes need).
HwsMemBlock* fn_80112848(SkinDesc* pDesc, s32 nSize, int nMode) {
    HwsMemBlock* pBlock;

    if (nSize == 0) {
        nSize = fn_80112804(pDesc);
    }
    if (nSize == 0) {
        return NULL;
    }
    pBlock = fn_80009B34(nSize + sizeof(HwsMemBlock), nMode, 16, "hwsOverride_Gc.c", 60);
    pBlock->nSize = nSize;
    pBlock->nUsed = 0;
    pBlock->pData = (u8*)(pBlock + 1);
    return pBlock;
}

HwsMemBlock* fn_801128C8(SkinDesc* pDesc, s32 nSize) {
    return fn_80112848(pDesc, nSize, 2);
}

HwsMemBlock* fn_801128EC(SkinDesc* pDesc, s32 nSize) {
    return fn_80112848(pDesc, nSize, 1);
}

void fn_80112910(HwsMemBlock* pBlock) {
    if (pBlock != NULL) {
        fn_80009E70(pBlock);
    }
}

// The next nSize bytes of the block, or NULL when it has no more.
void* fn_80112938(HwsMemBlock* pBlock, s32 nSize) {
    s32 nUsed = pBlock->nUsed;
    void* p = NULL;
    s32 nEnd = nUsed + nSize;

    if (nEnd <= pBlock->nSize) {
        p = pBlock->pData + nUsed;
        pBlock->nUsed = nEnd;
    }
    return p;
}

s32 fn_80112964(SkinDesc* pDesc) {
    return pDesc->n30;
}

// An override table for nMeshes meshes (0: all of pDesc's), every entry empty.
HwsOverrideTable* fn_8011296C(SkinDesc* pDesc, s32 nMeshes, int nMode) {
    HwsOverrideTable* pTable;
    s32 n = nMeshes;

    if (n == 0) {
        n = fn_80112964(pDesc);
    }
    if (n == 0) {
        return NULL;
    }
    pTable = fn_80009B34(n * 4 + sizeof(HwsOverrideTable), nMode, 16, "hwsOverride_Gc.c", 129);
    pTable->nMeshes = n;
    pTable->pDesc = pDesc;
    pTable->apMesh = (void**)(pTable + 1);
    memset(pTable->apMesh, 0, n * sizeof(void*));
    return pTable;
}

HwsOverrideTable* fn_80112A10(SkinDesc* pDesc, s32 nMeshes) {
    return fn_8011296C(pDesc, nMeshes, 2);
}

HwsOverrideTable* fn_80112A34(SkinDesc* pDesc, s32 nMeshes) {
    return fn_8011296C(pDesc, nMeshes, 1);
}

void fn_80112A58(HwsOverrideTable* pTable) {
    if (pTable != NULL) {
        fn_80009E70(pTable);
    }
}

// Mesh i's override: its memory from pBlock (kept from before when bKeep is set).
void* fn_80112A80(HwsMemBlock* pBlock, HwsOverrideTable* pTable, int i, u8 bKeep) {
    void* p;

    if (pBlock == NULL || i >= pTable->nMeshes) {
        return NULL;
    }
    if (bKeep && pTable->apMesh[i] != NULL) {
        return pTable->apMesh[i];
    }
    p = fn_80112938(pBlock, pTable->pDesc->p34[i].nSize);
    pTable->apMesh[i] = p;
    return p;
}

void fn_80112B18(HwsOverrideTable* pTable, int i, void* p) {
    if (i < pTable->nMeshes) {
        pTable->apMesh[i] = p;
    }
}

void fn_80112B34(void) {
    HwsRender10* p = &lbl_80223BB0.s10;

    memset(p, 0, 4);
    p->p48 = &p->refs;
    p->p4C = p->a24;
}
