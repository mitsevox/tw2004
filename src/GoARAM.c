// GoARAM.c (EA's name, from its asserts): the audio RAM (ARAM): its set-up (fn_800B62DC), its heap
// and the queue of DMA transfers between main memory and ARAM (the state at lbl_802814C8). The
// heap (fn_800B5C40-fn_800B6188) is written out; the rest is still the sweep's code.

#include "core/goaram.h"

ARAMHeap*  fn_800B5C40(u32 uSize, u32 uBase, u32 nBlocks, ARAMHeap* pHeap);
void       fn_800B5D08(ARAMHeap* pHeap);
u32        fn_800B5D34(ARAMHeap* pHeap, u32 uSize, u32 uAlign);
void       fn_800B5E88(ARAMHeap* pHeap, u32 uAram);
void       fn_800B5F28(ARAMHeap* pHeap, ARAMBlock* pBlock);
ARAMBlock* fn_800B5F4C(ARAMHeap* pHeap);
ARAMBlock* fn_800B5F8C(ARAMHeap* pHeap, ARAMBlock* pHead, ARAMBlock* pBlock);
void       fn_800B6034(ARAMBlock* pAt, ARAMBlock* pBlock);
void       fn_800B6060(ARAMBlock* pAt, ARAMBlock* pBlock);
void       fn_800B608C(ARAMBlock* pBlock);
ARAMBlock* fn_800B60B0(ARAMBlock* pHead, ARAMBlock* pBlock);
ARAMBlock* fn_800B60F0(ARAMBlock* pHead, ARAMBlock* pBlock);
void       fn_800B6188(ARAMHeap* pHeap);

// Sets up a heap of uSize bytes at ARAM address uBase, with nBlocks block records after pHeap:
// one record holds the whole heap as free, the others go to the spare list.
ARAMHeap* fn_800B5C40(u32 uSize, u32 uBase, u32 nBlocks, ARAMHeap* pHeap) {
    ARAMBlock* pBlock;
    u32 i;

    pHeap->p20 = NULL;
    pHeap->pBlocks = (ARAMBlock*)(pHeap + 1);
    pHeap->uBase = uBase;
    pHeap->uFree = uSize;
    pHeap->uSize = uSize;
    pHeap->nBlocks = nBlocks;
    pHeap->pSpare = pHeap->pBlocks;
    pHeap->pBlocks->pPrev = pHeap->pBlocks->pNext = pHeap->pBlocks;
    for (i = 1; i < pHeap->nBlocks; i++) {
        fn_800B6034(pHeap->pSpare, &pHeap->pBlocks[i]);
    }
    pHeap->pUsed = NULL;
    pBlock = fn_800B5F4C(pHeap);
    pBlock->uSize = pHeap->uFree;
    pBlock->uOffset = 0;
    pHeap->pFree = pBlock;
    return pHeap;
}

void fn_800B5D08(ARAMHeap* pHeap) {
    if (pHeap->p20 != NULL) {
        fn_80009E70(pHeap->p20);
    }
}

// Hands out uSize bytes aligned to uAlign and returns their ARAM address. It takes the free block
// that fits exactly, else the largest, and cuts the piece from its low end.
u32 fn_800B5D34(ARAMHeap* pHeap, u32 uSize, u32 uAlign) {
    ARAMBlock* pHead = pHeap->pFree;
    u32 uBase = pHeap->uBase;
    ARAMBlock* pBest = pHead;
    ARAMBlock* pBlock = pHead;
    ARAMBlock* pUse;
    u32 uPad;
    u32 uNeed;

    do {
        if (pBlock->uSize == uSize + (uAlign - (pBlock->uOffset + uBase) % uAlign) % uAlign) {
            pBest = pBlock;
            break;
        }
        if (pBlock->uSize > pBest->uSize) {
            pBest = pBlock;
        }
        pBlock = pBlock->pNext;
    } while (pBlock != pHead);
    uPad = (uAlign - (pBest->uOffset + uBase) % uAlign) % uAlign;
    uNeed = uSize + uPad;
    if (pBest->uSize < uNeed) {
        fn_800B6188(pHeap);
    }
    if (pBest->uSize == uNeed) {
        pHeap->pFree = fn_800B60B0(pHeap->pFree, pBest);
        pUse = pBest;
    } else {
        pUse = fn_800B5F4C(pHeap);
        pUse->uSize = uNeed;
        pUse->uOffset = pBest->uOffset;
        pBest->uSize -= uNeed;
        pBest->uOffset += uNeed;
    }
    pUse->uStart = pUse->uOffset + uPad;
    pHeap->pUsed = fn_800B60F0(pHeap->pUsed, pUse);
    pHeap->uFree -= uNeed;
    return pUse->uStart + pHeap->uBase;
}

// Gives back the block handed out at ARAM address uAram, merging it with its free neighbours.
void fn_800B5E88(ARAMHeap* pHeap, u32 uAram) {
    ARAMBlock* pHead;
    ARAMBlock* pBlock;
    u32 uStart;

    uStart = uAram - pHeap->uBase;
    pHead = pHeap->pUsed;
    pBlock = pHead;
    do {
        if (pBlock->uStart == uStart) {
            pHeap->uFree += pBlock->uSize;
            pHeap->pUsed = fn_800B60B0(pHeap->pUsed, pBlock);
            pHeap->pFree = fn_800B60F0(pHeap->pFree, pBlock);
            pHeap->pFree = fn_800B5F8C(pHeap, pHeap->pFree, pBlock);
            return;
        }
        pBlock = pBlock->pNext;
    } while (pBlock != pHead);
}

// Puts a block record back in the spare list.
void fn_800B5F28(ARAMHeap* pHeap, ARAMBlock* pBlock) {
    fn_800B6034(pHeap->pSpare, pBlock);
}

// Takes a record from the spare list.
ARAMBlock* fn_800B5F4C(ARAMHeap* pHeap) {
    ARAMBlock* pBlock = pHeap->pSpare;

    pHeap->pSpare = pHeap->pSpare->pNext;
    fn_800B608C(pBlock);
    return pBlock;
}

// Merges the free block pBlock with the free blocks either side of it when they touch.
ARAMBlock* fn_800B5F8C(ARAMHeap* pHeap, ARAMBlock* pHead, ARAMBlock* pBlock) {
    ARAMBlock* pPrev = pBlock->pPrev;
    ARAMBlock* pNext;
    int i;

    for (i = 0; i < 2; i++) {
        if (pHead->pNext == pHead) return pHead;
        pNext = pPrev->pNext;
        if (pNext->uOffset == pPrev->uOffset + pPrev->uSize) {
            pPrev->uSize += pNext->uSize;
            pHead = fn_800B60B0(pHead, pNext);
            fn_800B5F28(pHeap, pNext);
        } else {
            pPrev = pNext;
        }
    }
    return pHead;
}

// Links pBlock in after pAt.
void fn_800B6034(ARAMBlock* pAt, ARAMBlock* pBlock) {
    if (pAt != NULL) {
        pBlock->pNext = pAt->pNext;
        pBlock->pPrev = pAt;
        if (pAt->pNext != NULL) {
            pAt->pNext->pPrev = pBlock;
        }
        pAt->pNext = pBlock;
    }
}

// Links pBlock in before pAt.
void fn_800B6060(ARAMBlock* pAt, ARAMBlock* pBlock) {
    if (pAt != NULL) {
        pBlock->pNext = pAt;
        pBlock->pPrev = pAt->pPrev;
        if (pAt->pPrev != NULL) {
            pAt->pPrev->pNext = pBlock;
        }
        pAt->pPrev = pBlock;
    }
}

// Unlinks pBlock, leaving it a list of its own.
void fn_800B608C(ARAMBlock* pBlock) {
    pBlock->pPrev->pNext = pBlock->pNext;
    pBlock->pNext->pPrev = pBlock->pPrev;
    pBlock->pPrev = pBlock;
    pBlock->pNext = pBlock;
}

// Removes pBlock from the list at pHead and returns the list's new head (NULL once it is empty).
ARAMBlock* fn_800B60B0(ARAMBlock* pHead, ARAMBlock* pBlock) {
    if (pBlock == pBlock->pNext) return NULL;
    if (pBlock == pHead) {
        pHead = pBlock->pNext;
    }
    pBlock->pPrev->pNext = pBlock->pNext;
    pBlock->pNext->pPrev = pBlock->pPrev;
    pBlock->pPrev = pBlock;
    pBlock->pNext = pBlock;
    return pHead;
}

// Adds pBlock to the list at pHead, keeping it sorted by offset, and returns the new head.
ARAMBlock* fn_800B60F0(ARAMBlock* pHead, ARAMBlock* pBlock) {
    ARAMBlock* pAt;

    if (pHead == NULL) return pBlock;
    pAt = pHead;
    do {
        if (pAt->uOffset > pBlock->uOffset) {
            fn_800B6060(pAt, pBlock);
            if (pAt == pHead) {
                pHead = pBlock;
            }
            return pHead;
        }
        pAt = pAt->pNext;
    } while (pAt != pHead);
    fn_800B6060(pAt, pBlock);
    return pHead;
}

// Walks both lists and does nothing with them: whatever it checked or printed is compiled out.
void fn_800B6188(ARAMHeap* pHeap) {
    ARAMBlock* pHead;
    ARAMBlock* pBlock;

    pHead = pHeap->pUsed;
    pBlock = pHead;
    do {
        pBlock = pBlock->pNext;
    } while (pBlock != pHead);
    pHead = pHeap->pFree;
    pBlock = pHead;
    do {
        pBlock = pBlock->pNext;
    } while (pBlock != pHead);
}

// ---- sweep code (not yet cleaned up) ----

void fn_800B6214(void* arg0);
s32 fn_80007328();
s32 fn_80007368();
s32 fn_800B04EC(s32, s32, s32);
void fn_800B051C();
void fn_800B65C0();
void fn_800B61B4(void* arg0);
void fn_800B64D8(void);
void fn_800B655C(void);
void fn_800B6560(void);
u32 fn_800B6564(u32 uSize);
void fn_800B6594(u32 uAram);
s32 fn_800B67B4(void* arg0);
void fn_800B67EC(void* arg0);
void fn_800B6844(s32 p0, s32 p1, s32 p2);
void fn_800B68B4(s32 p0, s32 p1, s32 p2);

void fn_800B61B4(void* arg0) {
    s32 (*temp_r12)(s32);

    temp_r12 = (*(s32 (**)(s32))((u8*)(arg0) + 0x20));
    if (temp_r12 != NULL) {
        temp_r12((*(s32*)((u8*)(arg0) + 0x24)));
    }
    if ((*(u8*)((u8*)(arg0) + 0x38)) & 1) {
        fn_800B6214(arg0);
        return;
    }
    (*(s32*)((u8*)(arg0) + 0x28)) = 2;
}

void fn_800B6214(void* arg0) {
    void* temp_r0;
    void* temp_r4;
    void* temp_r5;

    (*(s32*)((u8*)(arg0) + 0x28)) = 0;
    temp_r5 = (*(void**)((u8*)(arg0) + 0x3C));
    if (arg0 == temp_r5) {
        (*(void**)((u8*)(lbl_802814C8) + 0x10)) = NULL;
    } else {
        if (arg0 == (void* ) (*(void**)((u8*)(lbl_802814C8) + 0x10))) {
            (*(void**)((u8*)(lbl_802814C8) + 0x10)) = temp_r5;
        }
        (*(void**)((u8*)((*(void**)((u8*)(arg0) + 0x40))) + 0x3C)) = (void* ) (*(void**)((u8*)(arg0) + 0x3C));
        (*(void**)((u8*)((*(void**)((u8*)(arg0) + 0x3C))) + 0x40)) = (void* ) (*(void**)((u8*)(arg0) + 0x40));
        (*(void**)((u8*)(arg0) + 0x40)) = arg0;
        (*(void**)((u8*)(arg0) + 0x3C)) = arg0;
    }
    (*(s32*)((u8*)(lbl_802814C8) + 0x20)) = (s32) ((*(s32*)((u8*)(lbl_802814C8) + 0x20)) - 1);
    temp_r0 = (*(void**)((u8*)(lbl_802814C8) + 0x14));
    if (temp_r0 != NULL) {
        (*(void**)((u8*)(arg0) + 0x3C)) = temp_r0;
        (*(void**)((u8*)(arg0) + 0x40)) = (void* ) (*(void**)((u8*)((*(void**)((u8*)(lbl_802814C8) + 0x14))) + 0x40));
        temp_r4 = (*(void**)((u8*)((*(void**)((u8*)(lbl_802814C8) + 0x14))) + 0x40));
        if (temp_r4 != NULL) {
            (*(void**)((u8*)(temp_r4) + 0x3C)) = arg0;
        }
        (*(void**)((u8*)((*(void**)((u8*)(lbl_802814C8) + 0x14))) + 0x40)) = arg0;
    }
    (*(void**)((u8*)(lbl_802814C8) + 0x14)) = arg0;
    (*(s32*)((u8*)(lbl_802814C8) + 0x24)) = (s32) ((*(s32*)((u8*)(lbl_802814C8) + 0x24)) + 1);
}

void fn_800B64D8(void) {
    if (lbl_802814C8->p18 != NULL) {
        fn_80009E70(lbl_802814C8->p18);
        lbl_802814C8->p18 = NULL;
    }
    fn_8009527C(lbl_802814C8->p8);
    lbl_802814C8->p8 = NULL;
    fn_800B5D08(lbl_802814C8->pHeap);
    lbl_802814C8->pHeap = NULL;
    fn_8009527C(lbl_802814C8->p4);
    lbl_802814C8->p4 = NULL;
}

void fn_800B655C(void) {
}

void fn_800B6560(void) {
}

// Hands out uSize bytes of ARAM, 32-byte aligned, and returns their address.
u32 fn_800B6564(u32 uSize) {
    return fn_800B5D34(lbl_802814C8->pHeap, uSize, 32);
}

void fn_800B6594(u32 uAram) {
    fn_800B5E88(lbl_802814C8->pHeap, uAram);
}

s32 fn_800B67B4(void* arg0) {
    s32 temp_r31;

    fn_80007368();
    temp_r31 = (*(s32*)((u8*)(arg0) + 0x28));
    fn_80007328();
    return temp_r31;
}

void fn_800B67EC(void* arg0) {
    do {
    } while (fn_800B67B4(arg0) != 2);
    fn_800B04EC((*(s32*)((u8*)(arg0) + 0x30)), (*(s32*)((u8*)(arg0) + 0x34)), (*(s32*)((u8*)(arg0) + 0x2C)));
    fn_80007368();
    fn_800B6214(arg0);
    fn_80007328();
}

void fn_800B6844(s32 p0, s32 p1, s32 p2) {
    fn_800B051C(p0, p2, 0);
    fn_800B65C0(p0, p1, p2, 0, 1, 0, 0, 0);
}

void fn_800B68B4(s32 p0, s32 p1, s32 p2) {
    fn_800B051C(p0, p2, 1);
    fn_800B65C0(p1, p0, p2, 1, 1, 0, 0, 0);
}

// ---- end of sweep code ----
