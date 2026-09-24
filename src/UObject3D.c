// UObject3D.c (EA's name, from its asserts; also in EA's 2002 source tree): a model made from a
// stream object's data, and a display list of the renderer's state that the file records at
// startup into a block of the buffer pool (engine.h BufferPool). Each display-list step has a
// portable wrapper (fn_80045E3C..fn_80045EA0) over its GameCube version (fn_80045EC0..fn_80045F38).

#include "dynobj.h"
#include "core/startup.h"

void fn_80045E3C(int a);
void fn_80045E40(BufferPoolBlock* pBlock);
void fn_80045E60(BufferPoolBlock* pBlock);
BufferPoolBlock* fn_80045E80(void);
void fn_80045EA0(BufferPoolBlock* pBlock);
void fn_80045EC0(BufferPoolBlock* pBlock);
void fn_80045F14(BufferPoolBlock* pBlock);
BufferPoolBlock* fn_80045F18(void);
void fn_80045F38(BufferPoolBlock* pBlock);

BufferPoolBlock* lbl_80281D98;      // the block holding the display list

// Startup (the game's startup, fn_8006C7A8): takes a free pool block and records into it the
// display list that hands GX the renderer's state, after setting that state's b4 to 1.
void fn_80045D18(void) {
    lbl_80281D98 = fn_80045E80();
    fn_80012F34(1);
    fn_80045E60(lbl_80281D98);
    fn_80045E40(lbl_80281D98);
    fn_80045E3C(0);
}

// Shutdown (fn_8006C854): gives the block back to the pool.
void fn_80045D5C(void) {
    fn_80045EA0(lbl_80281D98);
}

// Builds a model from pData: its mesh tree, and up to three levels of detail from the tree's
// first mesh group.
UObjModel* fn_80045D80(u8* pData) {
    UObjMesh* pGroup;
    UObjModel* pModel;
    int nLods;
    int i;

    pModel = fn_80009B34(sizeof(UObjModel), 2, 0x80, "UObject3D.c", 506);
    pModel->p10 = fn_800073B4(pData, 0);
    pGroup = fn_80045F5C(fn_80045F6C(pModel->p10), 0);
    nLods = fn_80045F50(pGroup);
    for (i = 0; i < 3; i++) {
        if (nLods > i) {
            pModel->apLod[i] = fn_80045F5C(pGroup, i);
        } else {
            pModel->apLod[i] = NULL;
        }
        pModel->apLod[i + 3] = NULL;
    }
    return pModel;
}

// fn_80045D18 passes 0; the GameCube version does nothing here.
void fn_80045E3C(int a) {
}

void fn_80045E40(BufferPoolBlock* pBlock) {
    fn_80045EC0(pBlock);
}

void fn_80045E60(BufferPoolBlock* pBlock) {
    fn_80045F14(pBlock);
}

BufferPoolBlock* fn_80045E80(void) {
    return fn_80045F18();
}

void fn_80045EA0(BufferPoolBlock* pBlock) {
    fn_80045F38(pBlock);
}

// Records fn_80015624's GX commands (the renderer state groups that changed) as a display list in
// the block,
// and keeps the list's size in u1000, which also marks the block as in use.
void fn_80045EC0(BufferPoolBlock* pBlock) {
    DCInvalidateRange(pBlock->unk0, sizeof(pBlock->unk0));
    GXBeginDisplayList(pBlock->unk0, sizeof(pBlock->unk0));
    fn_80015624();
    pBlock->u1000 = GXEndDisplayList();
    DCFlushRange(pBlock->unk0, pBlock->u1000);
}

void fn_80045F14(BufferPoolBlock* pBlock) {
}

BufferPoolBlock* fn_80045F18(void) {
    return fn_800154F4();
}

// Frees the block: a zero size marks it unused.
void fn_80045F38(BufferPoolBlock* pBlock) {
    if (pBlock->u1000 != 0) {
        pBlock->u1000 = 0;
    }
}

int fn_80045F50(UObjMesh* pMesh) {
    return pMesh->pInfo->n0;
}

UObjMesh* fn_80045F5C(UObjMesh* pMesh, int i) {
    return pMesh->p8[i];
}

UObjMesh* fn_80045F6C(UObjModelRoot* pRoot) {
    return pRoot->pMesh;
}
