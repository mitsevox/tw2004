// GoARAM.c (EA's name, from its asserts): the audio RAM (ARAM): its set-up (fn_800B62DC), its heap
// and the queue of DMA transfers between main memory and ARAM (the state at lbl_801F6680). The
// front end, the memory card and the animation code keep data here while it is not needed.

#include "core/goaram.h"
#include "core/startup.h"

void       fn_800B5D08(ARAMHeap* pHeap);
void       fn_800B5F28(ARAMHeap* pHeap, ARAMBlock* pBlock);
ARAMBlock* fn_800B5F4C(ARAMHeap* pHeap);
ARAMBlock* fn_800B5F8C(ARAMHeap* pHeap, ARAMBlock* pHead, ARAMBlock* pBlock);
void       fn_800B6034(ARAMBlock* pAt, ARAMBlock* pBlock);
void       fn_800B6060(ARAMBlock* pAt, ARAMBlock* pBlock);
void       fn_800B608C(ARAMBlock* pBlock);
ARAMBlock* fn_800B60B0(ARAMBlock* pHead, ARAMBlock* pBlock);
ARAMBlock* fn_800B60F0(ARAMBlock* pHead, ARAMBlock* pBlock);
void       fn_800B6188(ARAMHeap* pHeap);
void       fn_800B61B4(ARQRequest* pRequest);
void       fn_800B6214(ARAMTransfer* pTransfer);
void       fn_800B62DC(void);
void       fn_800B64D8(void);
void       fn_800B655C(void);
void       fn_800B6560(void);
s32        fn_800B67B4(ARAMTransfer* pTransfer);
void       fn_80007328(void);
void       fn_80007368(void);

ARAMState lbl_801F6680;
ARAMState* lbl_802814C8 = &lbl_801F6680;

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

// The ARQ library calls this when a transfer's DMA is done.
void fn_800B61B4(ARQRequest* pRequest) {
    ARAMTransfer* pTransfer = (ARAMTransfer*)pRequest;

    if (pTransfer->pfnDone != NULL) {
        pTransfer->pfnDone(pTransfer->uOwner);
    }
    if (pTransfer->uFlags & 1) {
        fn_800B6214(pTransfer);
        return;
    }
    pTransfer->nState = 2;
}

// Moves a transfer from the queued list back to the unused list.
void fn_800B6214(ARAMTransfer* pTransfer) {
    pTransfer->nState = 0;
    if (pTransfer == pTransfer->pNext) {
        lbl_802814C8->pQueued = NULL;
    } else {
        if (pTransfer == lbl_802814C8->pQueued) {
            lbl_802814C8->pQueued = pTransfer->pNext;
        }
        pTransfer->pPrev->pNext = pTransfer->pNext;
        pTransfer->pNext->pPrev = pTransfer->pPrev;
        pTransfer->pPrev = pTransfer;
        pTransfer->pNext = pTransfer;
    }
    lbl_802814C8->nQueued--;
    if (lbl_802814C8->pUnused != NULL) {
        pTransfer->pNext = lbl_802814C8->pUnused;
        pTransfer->pPrev = lbl_802814C8->pUnused->pPrev;
        if (lbl_802814C8->pUnused->pPrev != NULL) {
            lbl_802814C8->pUnused->pPrev->pNext = pTransfer;
        }
        lbl_802814C8->pUnused->pPrev = pTransfer;
    }
    lbl_802814C8->pUnused = pTransfer;
    lbl_802814C8->nUnused++;
}

// Sets up the ARAM: the heap over all of it and ARAM_NUM_TRANSFERS unused transfers.
void fn_800B62DC(void) {
    ARAMTransfer* pTransfer;
    u32 uBase;
    u32 uEnd;
    int i;

    ARInit(NULL, 0);
    uBase = ARGetBaseAddress();
    uEnd = fn_80133B18();
    lbl_802814C8->pHeapMem = fn_800951A0(sizeof(ARAMHeap) + ARAM_NUM_BLOCKS * sizeof(ARAMBlock), 16, 1);
    lbl_802814C8->pHeap = fn_800B5C40(uEnd - uBase, uBase, ARAM_NUM_BLOCKS, lbl_802814C8->pHeapMem);
    lbl_802814C8->p8 = fn_800951A0(ARAM_NUM_TRANSFERS * sizeof(ARAMTransfer), 16, 1);
    lbl_802814C8->nTransfers = ARAM_NUM_TRANSFERS;
    if (lbl_802814C8->p8 == NULL) {
        lbl_802814C8->p18 = fn_80009B34(ARAM_NUM_TRANSFERS * sizeof(ARAMTransfer), 2, 16, "GoARAM.c", 192);
        lbl_802814C8->pTransfers = lbl_802814C8->p18;
    } else {
        lbl_802814C8->p18 = NULL;
        lbl_802814C8->pTransfers = lbl_802814C8->p8;
    }
    lbl_802814C8->pQueued = NULL;
    lbl_802814C8->pUnused = NULL;
    pTransfer = lbl_802814C8->pTransfers;
    for (i = 0; i < ARAM_NUM_TRANSFERS; i++) {
        pTransfer->pNext = pTransfer;
        pTransfer->pPrev = pTransfer;
        if (lbl_802814C8->pUnused != NULL) {
            pTransfer->pNext = lbl_802814C8->pUnused;
            pTransfer->pPrev = lbl_802814C8->pUnused->pPrev;
            if (lbl_802814C8->pUnused->pPrev != NULL) {
                lbl_802814C8->pUnused->pPrev->pNext = pTransfer;
            }
            lbl_802814C8->pUnused->pPrev = pTransfer;
        }
        lbl_802814C8->pUnused = pTransfer;
        pTransfer++;
    }
    lbl_802814C8->nUnused = ARAM_NUM_TRANSFERS;
    lbl_802814C8->nQueued = 0;
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
    fn_8009527C(lbl_802814C8->pHeapMem);
    lbl_802814C8->pHeapMem = NULL;
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

// Queues a DMA of uLength bytes from uSource to uDest (nType as ARQRequest.type) and returns its
// transfer. pfnDone(uOwner) is called when it is done.
ARAMTransfer* fn_800B65C0(u32 uSource, u32 uDest, u32 uLength, int nType, u32 uPriority,
                          void (*pfnDone)(u32 uOwner), u32 uOwner, u8 uFlags) {
    ARAMTransfer* pTransfer;

    fn_80007368();
    pTransfer = lbl_802814C8->pUnused;
    if (pTransfer == pTransfer->pNext) {
        lbl_802814C8->pUnused = NULL;
    } else {
        if (pTransfer == lbl_802814C8->pUnused) {
            lbl_802814C8->pUnused = pTransfer->pNext;
        }
        pTransfer->pPrev->pNext = pTransfer->pNext;
        pTransfer->pNext->pPrev = pTransfer->pPrev;
        pTransfer->pPrev = pTransfer;
        pTransfer->pNext = pTransfer;
    }
    lbl_802814C8->nUnused--;
    if (lbl_802814C8->pQueued != NULL) {
        pTransfer->pNext = lbl_802814C8->pQueued;
        pTransfer->pPrev = lbl_802814C8->pQueued->pPrev;
        if (lbl_802814C8->pQueued->pPrev != NULL) {
            lbl_802814C8->pQueued->pPrev->pNext = pTransfer;
        }
        lbl_802814C8->pQueued->pPrev = pTransfer;
    }
    lbl_802814C8->pQueued = pTransfer;
    lbl_802814C8->nQueued++;
    fn_80007328();
    pTransfer->pfnDone = pfnDone;
    pTransfer->uOwner = uOwner;
    pTransfer->uFlags = uFlags;
    pTransfer->nType = nType;
    pTransfer->uMain = (nType == 0) ? uSource : uDest;
    pTransfer->uLength = uLength;
    pTransfer->nState = 1;
    ARQPostRequest(&pTransfer->request, uOwner, nType, uPriority, uSource, uDest, uLength, fn_800B61B4);
    return pTransfer;
}

// Cancels the queued transfers of uOwner that allow it (flag 2) and returns how many.
int fn_800B6728(u32 uOwner) {
    ARAMTransfer* pTransfer;
    ARAMTransfer* pNext;
    int nQueued;
    int i;
    int nCancelled = 0;

    fn_80007368();
    i = 0;
    pTransfer = lbl_802814C8->pQueued;
    nQueued = lbl_802814C8->nQueued;
    for (; i < nQueued; i++) {
        pNext = pTransfer->pNext;
        if ((pTransfer->uFlags & 2) && pTransfer->uOwner == uOwner) {
            ARQRemoveRequest(&pTransfer->request);
            fn_800B6214(pTransfer);
            nCancelled++;
        }
        pTransfer = pNext;
    }
    fn_80007328();
    return nCancelled;
}

s32 fn_800B67B4(ARAMTransfer* pTransfer) {
    s32 nState;

    fn_80007368();
    nState = pTransfer->nState;
    fn_80007328();
    return nState;
}

// Waits for a transfer to finish, then gives it back.
void fn_800B67EC(ARAMTransfer* pTransfer) {
    do {
    } while (fn_800B67B4(pTransfer) != 2);
    // port: the main-memory address is kept as a u32, as the ARQ library takes it
    fn_800B04EC((void*)pTransfer->uMain, pTransfer->uLength, pTransfer->nType);
    fn_80007368();
    fn_800B6214(pTransfer);
    fn_80007328();
}

// Starts copying uSize bytes from pSrc to ARAM address uAram; fn_800B67EC waits for it.
ARAMTransfer* fn_800B6844(void* pSrc, u32 uAram, u32 uSize) {
    fn_800B051C(pSrc, uSize, 0);
    // port: the ARQ library takes addresses as u32
    return fn_800B65C0((u32)pSrc, uAram, uSize, 0, 1, NULL, 0, 0);
}

// Starts copying uSize bytes from ARAM address uAram to pDst; fn_800B67EC waits for it.
ARAMTransfer* fn_800B68B4(void* pDst, u32 uAram, u32 uSize) {
    fn_800B051C(pDst, uSize, 1);
    // port: the ARQ library takes addresses as u32
    return fn_800B65C0(uAram, (u32)pDst, uSize, 1, 1, NULL, 0, 0);
}
