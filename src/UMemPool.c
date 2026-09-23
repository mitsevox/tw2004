// UMemPool.c (EA's name, from its asserts; also in EA's 2002 source tree): pools of fixed-size
// nodes carved from one allocation (the file streamer keeps its object nodes in one), and before
// them a block of math helpers: 4x4 matrices, paired-single vector operations, atan2f, fabsf and
// the natural logarithm with its log2 lookup table. Only part is decompiled so far.

#include "engine.h"

f32* lbl_80281BD8;                      // the log2 table: 1024 entries over the mantissa of [1, 2)

void fn_8000AE9C(void);

// ---- sweep code (not yet cleaned up) ----

void Vec_Copy();
void fn_8000A0E8(u8* p0, u8* p1);
void fn_8000A144(u8* p0, u8* p1);

void fn_8000A0E8(u8* p0, u8* p1) {
    Vec_Copy();
    Vec_Copy((p0 + 0x10), (p1 + 0x10));
    Vec_Copy((p0 + 0x20), (p1 + 0x20));
    Vec_Copy((p0 + 0x30), (p1 + 0x30));
}

void fn_8000A144(u8* p0, u8* p1) {
    Vec_Copy();
    Vec_Copy((p0 + 0x10), (p1 + 0x10));
    Vec_Copy((p0 + 0x20), (p1 + 0x20));
}

// ---- end of sweep code ----

// Swaps two 4-float vectors.
void fn_8000AD34(f32* pA, f32* pB) {
    f32 f;

    f = pA[0];
    pA[0] = pB[0];
    pB[0] = f;
    f = pA[1];
    pA[1] = pB[1];
    pB[1] = f;
    f = pA[2];
    pA[2] = pB[2];
    pB[2] = f;
    f = pA[3];
    pA[3] = pB[3];
    pB[3] = f;
}

f32 fn_8000AD78(f32 y, f32 x) {
    return atan2(y, x);
}

f32 fabsf(f32 x) {
    return fabs(x);
}

// 0x8000AE94: the absolute value of a double (fabsf, which rounds its result to a float, is
// the float version).
double fabs(double x) {
    return __fabs(x);
}

// Fills the log2 table: entry i is log2(1 + i / 1024).
void fn_8000AE9C(void) {
    u32 uMantissa;
    f32* pEntry;
    u32 i;
    f32 f;

    pEntry = lbl_80281BD8;
    i = 0;
    uMantissa = 0;
    do {
        // port: builds the float from its bits through a u32 pointer (see Rand_Float).
        *(u32*)&f = uMantissa | 0x3F800000;
        *pEntry = 1.442695f * fn_8000AF7C(f);
        i++;
        uMantissa += 0x2000;
        pEntry++;
    } while (i < 0x400);
}

void fn_8000AF1C(void) {
}

void fn_8000AF20(void) {
    fn_8000AF1C();
    lbl_80281BD8 = fn_800951A0(0x400 * sizeof(f32), 16, 1);
    fn_8000AE9C();
}

void fn_8000AF58(void) {
    fn_8009527C(lbl_80281BD8);
}

f32 fn_8000AF7C(f32 x) {
    return log(x);
}

// Makes a pool of nNodes nodes of uNodeSize bytes each, every node aligned to uAlign (a power of
// two). The nodes are filled with 0xDD and chained into the free list, the last one first.
UMemPool* fn_8000AFA0(int nNodes, u32 uNodeSize, u32 uFlags, u32 uAlign) {
    UMemPool* pPool;
    u32 uSize;
    u32 uTotal;
    u8* pNode;
    UMemPoolNode* pPrev;

    uSize = (uAlign + uNodeSize - 1) & ~(uAlign - 1);
    uTotal = uAlign + nNodes * uSize;
    pPool = fn_80009B34(uTotal, uFlags, uAlign, "UMemPool.c", 82);
    if (pPool != NULL) {
        pPool->nNodes = nNodes;
        pPool->nFree = nNodes;
        pPool->uNodeSize = uSize;
        pPool->pEnd = (u8*)pPool + uTotal;
        pNode = (u8*)pPool + uAlign;
        pPrev = NULL;
        while (nNodes-- > 0) {
            fn_80005AE8(pNode, 0xDD, uSize);
            ((UMemPoolNode*)pNode)->pNext = pPrev;
            pPrev = (UMemPoolNode*)pNode;
            pNode += uSize;
        }
        pPool->pFree = pPrev;
    }
    return pPool;
}

void fn_8000B058(UMemPool* pPool) {
    fn_80009E70(pPool);
}

// Takes a node off the free list and fills it with 0xBB; NULL when the pool is empty.
void* fn_8000B078(UMemPool* pPool) {
    UMemPoolNode* pNode;

    pNode = pPool->pFree;
    if (pNode != NULL) {
        pPool->pFree = pNode->pNext;
        pPool->nFree--;
        fn_80005AE8(pNode, 0xBB, pPool->uNodeSize);
    }
    return pNode;
}

// Fills a node with 0x99 and puts it back on the free list.
void fn_8000B0D4(UMemPool* pPool, void* p) {
    UMemPoolNode* pNode;

    pNode = p;
    fn_80005AE8(pNode, 0x99, pPool->uNodeSize);
    pNode->pNext = pPool->pFree;
    pPool->pFree = pNode;
    pPool->nFree++;
}
