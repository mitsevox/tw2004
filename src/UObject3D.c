// UObject3D.c (EA's name, from its asserts; also in EA's 2002 source tree): a model made from a
// stream object's data.

#include "dynobj.h"
#include "core/startup.h"

// ---- sweep code (not yet cleaned up) ----

extern BufferPoolBlock* lbl_80281D98;
void fn_80045E3C(int a);
void fn_80045E40(BufferPoolBlock* pBlock);
void fn_80045E60(BufferPoolBlock* pBlock);
BufferPoolBlock* fn_80045E80(void);
void fn_80045D18(void);
void fn_80045EA0(BufferPoolBlock* pBlock);
void fn_80045D5C(void);
void fn_80045EC0(BufferPoolBlock* pBlock);
void fn_80045F14(BufferPoolBlock* pBlock);
BufferPoolBlock* fn_80045F18(void);
void fn_80045F38(BufferPoolBlock* pBlock);

void fn_80045D18(void) {
    lbl_80281D98 = fn_80045E80();
    fn_80012F34(1);
    fn_80045E60(lbl_80281D98);
    fn_80045E40(lbl_80281D98);
    fn_80045E3C(0);
}

void fn_80045D5C(void) {
    fn_80045EA0(lbl_80281D98);
}

// ---- end of sweep code ----

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

// ---- sweep code (not yet cleaned up) ----

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

// ---- end of sweep code ----
