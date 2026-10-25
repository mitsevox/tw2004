// GoARAM.c: the auxiliary RAM (ARAM): its set-up, its heap and the queue of DMA transfers between
// main memory and ARAM. The sound engine, the front end, the memory card and the animation code
// keep data there.

#include "core/goaram.h"
#include "core/startup.h"

ARAMState gARAMState;
ARAMState* gpARAMState = &gARAMState;

static void       GoARAM_HeapClose(ARAMHeap* pHeap);
static void       GoARAM_HeapPutBlockRecord(ARAMHeap* pHeap, ARAMBlock* pBlock);
static ARAMBlock* GoARAM_HeapTakeBlockRecord(ARAMHeap* pHeap);
static ARAMBlock* GoARAM_HeapMergeFreeBlock(ARAMHeap* pHeap, ARAMBlock* pHead, ARAMBlock* pBlock);
static void       GoARAM_BlockInsertAfter(ARAMBlock* pAt, ARAMBlock* pBlock);
static void       GoARAM_BlockInsertBefore(ARAMBlock* pAt, ARAMBlock* pBlock);
static void       GoARAM_BlockUnlink(ARAMBlock* pBlock);
static ARAMBlock* GoARAM_BlockRemove(ARAMBlock* pHead, ARAMBlock* pBlock);
static ARAMBlock* GoARAM_BlockInsertSorted(ARAMBlock* pHead, ARAMBlock* pBlock);
static void       GoARAM_HeapWalk(ARAMHeap* pHeap);
static void       GoARAM_HandleTransferDone(ARQRequest* pRequest);
static void       GoARAM_ReleaseTransfer(ARAMTransfer* pTransfer);
static ARAMTransferState GoARAM_GetTransferState(ARAMTransfer* pTransfer);

// Sets up a heap of uSize bytes at ARAM address uBase, with nBlocks block records after pHeap:
// one record holds the whole heap as free, the others go to the spare list.
ARAMHeap* GoARAM_HeapInit(u32 uSize, u32 uBase, u32 nBlocks, ARAMHeap* pHeap) {
    ARAMBlock* pBlock;
    u32 i;

    pHeap->pCircularMem = NULL;
    pHeap->pBlocks = (ARAMBlock*)(pHeap + 1);
    pHeap->uBase = uBase;
    pHeap->uFree = uSize;
    pHeap->uSize = uSize;
    pHeap->nBlocks = nBlocks;
    pHeap->pSpare = pHeap->pBlocks;
    pHeap->pBlocks->pPrev = pHeap->pBlocks->pNext = pHeap->pBlocks;
    for (i = 1; i < pHeap->nBlocks; i++) {
        GoARAM_BlockInsertAfter(pHeap->pSpare, &pHeap->pBlocks[i]);
    }
    pHeap->pUsed = NULL;
    pBlock = GoARAM_HeapTakeBlockRecord(pHeap);
    pBlock->uSize = pHeap->uFree;
    pBlock->uOffset = 0;
    pHeap->pFree = pBlock;
    return pHeap;
}

// Frees the heap's block of the game's heap, if it has one; nothing in this build gives it one.
static void GoARAM_HeapClose(ARAMHeap* pHeap) {
    if (pHeap->pCircularMem != NULL) {
        Circular_MemFreeFunc(pHeap->pCircularMem);
    }
}

// Hands out uSize bytes aligned to uAlign and returns their ARAM address. It takes the free block
// that fits exactly, else the largest, and cuts the piece from its low end.
u32 GoARAM_HeapAlloc(ARAMHeap* pHeap, u32 uSize, u32 uAlign) {
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
        GoARAM_HeapWalk(pHeap);
    }
    if (pBest->uSize == uNeed) {
        pHeap->pFree = GoARAM_BlockRemove(pHeap->pFree, pBest);
        pUse = pBest;
    } else {
        pUse = GoARAM_HeapTakeBlockRecord(pHeap);
        pUse->uSize = uNeed;
        pUse->uOffset = pBest->uOffset;
        pBest->uSize -= uNeed;
        pBest->uOffset += uNeed;
    }
    pUse->uStart = pUse->uOffset + uPad;
    pHeap->pUsed = GoARAM_BlockInsertSorted(pHeap->pUsed, pUse);
    pHeap->uFree -= uNeed;
    return pUse->uStart + pHeap->uBase;
}

// Gives back the block handed out at ARAM address uAram, merging it with its free neighbours.
void GoARAM_HeapFree(ARAMHeap* pHeap, u32 uAram) {
    ARAMBlock* pHead;
    ARAMBlock* pBlock;
    u32 uStart;

    uStart = uAram - pHeap->uBase;
    pHead = pHeap->pUsed;
    pBlock = pHead;
    do {
        if (pBlock->uStart == uStart) {
            pHeap->uFree += pBlock->uSize;
            pHeap->pUsed = GoARAM_BlockRemove(pHeap->pUsed, pBlock);
            pHeap->pFree = GoARAM_BlockInsertSorted(pHeap->pFree, pBlock);
            pHeap->pFree = GoARAM_HeapMergeFreeBlock(pHeap, pHeap->pFree, pBlock);
            return;
        }
        pBlock = pBlock->pNext;
    } while (pBlock != pHead);
}

// Puts a block record back in the spare list.
static void GoARAM_HeapPutBlockRecord(ARAMHeap* pHeap, ARAMBlock* pBlock) {
    GoARAM_BlockInsertAfter(pHeap->pSpare, pBlock);
}

// Takes a record from the spare list.
static ARAMBlock* GoARAM_HeapTakeBlockRecord(ARAMHeap* pHeap) {
    ARAMBlock* pBlock = pHeap->pSpare;

    pHeap->pSpare = pHeap->pSpare->pNext;
    GoARAM_BlockUnlink(pBlock);
    return pBlock;
}

// Merges the free block pBlock with the free blocks either side of it when they touch.
static ARAMBlock* GoARAM_HeapMergeFreeBlock(ARAMHeap* pHeap, ARAMBlock* pHead, ARAMBlock* pBlock) {
    ARAMBlock* pPrev = pBlock->pPrev;
    ARAMBlock* pNext;
    int i;

    for (i = 0; i < 2; i++) {
        if (pHead->pNext == pHead) return pHead;
        pNext = pPrev->pNext;
        if (pNext->uOffset == pPrev->uOffset + pPrev->uSize) {
            pPrev->uSize += pNext->uSize;
            pHead = GoARAM_BlockRemove(pHead, pNext);
            GoARAM_HeapPutBlockRecord(pHeap, pNext);
        } else {
            pPrev = pNext;
        }
    }
    return pHead;
}

// Links pBlock in after pAt.
static void GoARAM_BlockInsertAfter(ARAMBlock* pAt, ARAMBlock* pBlock) {
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
static void GoARAM_BlockInsertBefore(ARAMBlock* pAt, ARAMBlock* pBlock) {
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
static void GoARAM_BlockUnlink(ARAMBlock* pBlock) {
    pBlock->pPrev->pNext = pBlock->pNext;
    pBlock->pNext->pPrev = pBlock->pPrev;
    pBlock->pPrev = pBlock;
    pBlock->pNext = pBlock;
}

// Removes pBlock from the list at pHead and returns the list's new head (NULL once it is empty).
static ARAMBlock* GoARAM_BlockRemove(ARAMBlock* pHead, ARAMBlock* pBlock) {
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
static ARAMBlock* GoARAM_BlockInsertSorted(ARAMBlock* pHead, ARAMBlock* pBlock) {
    ARAMBlock* pAt;

    if (pHead == NULL) return pBlock;
    pAt = pHead;
    do {
        if (pAt->uOffset > pBlock->uOffset) {
            GoARAM_BlockInsertBefore(pAt, pBlock);
            if (pAt == pHead) {
                pHead = pBlock;
            }
            return pHead;
        }
        pAt = pAt->pNext;
    } while (pAt != pHead);
    GoARAM_BlockInsertBefore(pAt, pBlock);
    return pHead;
}

// Called when no free block is big enough: walks the used list, then the free list, and does
// nothing else.
static void GoARAM_HeapWalk(ARAMHeap* pHeap) {
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

// The ARQ library calls this when a transfer's DMA is done: the transfer's pfnDone runs, then the
// transfer is given back or marked done.
static void GoARAM_HandleTransferDone(ARQRequest* pRequest) {
    ARAMTransfer* pTransfer = (ARAMTransfer*)pRequest;

    if (pTransfer->pfnDone != NULL) {
        pTransfer->pfnDone(pTransfer->uOwner);
    }
    if (pTransfer->uFlags & ARAM_TRANSFER_RELEASE_WHEN_DONE) {
        GoARAM_ReleaseTransfer(pTransfer);
        return;
    }
    pTransfer->nState = ARAM_TRANSFER_DONE;
}

// Moves a transfer from the queued list back to the unused list.
static void GoARAM_ReleaseTransfer(ARAMTransfer* pTransfer) {
    pTransfer->nState = ARAM_TRANSFER_UNUSED;
    if (pTransfer == pTransfer->pNext) {
        gpARAMState->pQueued = NULL;
    } else {
        if (pTransfer == gpARAMState->pQueued) {
            gpARAMState->pQueued = pTransfer->pNext;
        }
        pTransfer->pPrev->pNext = pTransfer->pNext;
        pTransfer->pNext->pPrev = pTransfer->pPrev;
        pTransfer->pPrev = pTransfer;
        pTransfer->pNext = pTransfer;
    }
    gpARAMState->nQueued--;
    if (gpARAMState->pUnused != NULL) {
        pTransfer->pNext = gpARAMState->pUnused;
        pTransfer->pPrev = gpARAMState->pUnused->pPrev;
        if (gpARAMState->pUnused->pPrev != NULL) {
            gpARAMState->pUnused->pPrev->pNext = pTransfer;
        }
        gpARAMState->pUnused->pPrev = pTransfer;
    }
    gpARAMState->pUnused = pTransfer;
    gpARAMState->nUnused++;
}

// Sets up the ARAM at boot: the heap over all of it past what the ARAM library keeps, and
// ARAM_NUM_TRANSFERS unused transfers (from the system heap, else from the game's heap).
void GoARAM_InitOnce(void) {
    ARAMTransfer* pTransfer;
    u32 uBase;
    u32 uEnd;
    int i;

    ARInit(NULL, 0);
    uBase = ARGetBaseAddress();
    uEnd = ARGetSize();
    gpARAMState->pHeapMem = Mem_AllocFunc(sizeof(ARAMHeap) + ARAM_NUM_BLOCKS * sizeof(ARAMBlock), 16, 1);
    gpARAMState->pHeap = GoARAM_HeapInit(uEnd - uBase, uBase, ARAM_NUM_BLOCKS, gpARAMState->pHeapMem);
    gpARAMState->pTransfersMem = Mem_AllocFunc(ARAM_NUM_TRANSFERS * sizeof(ARAMTransfer), 16, 1);
    gpARAMState->nTransfers = ARAM_NUM_TRANSFERS;
    if (gpARAMState->pTransfersMem == NULL) {
        gpARAMState->pTransfersCircular = Circular_MemAllocFunc(ARAM_NUM_TRANSFERS * sizeof(ARAMTransfer),
                                                                MEMTYPE_LOW, 16, "GoARAM.c", 192);
        gpARAMState->pTransfers = gpARAMState->pTransfersCircular;
    } else {
        gpARAMState->pTransfersCircular = NULL;
        gpARAMState->pTransfers = gpARAMState->pTransfersMem;
    }
    gpARAMState->pQueued = NULL;
    gpARAMState->pUnused = NULL;
    pTransfer = gpARAMState->pTransfers;
    for (i = 0; i < ARAM_NUM_TRANSFERS; i++) {
        pTransfer->pNext = pTransfer;
        pTransfer->pPrev = pTransfer;
        if (gpARAMState->pUnused != NULL) {
            pTransfer->pNext = gpARAMState->pUnused;
            pTransfer->pPrev = gpARAMState->pUnused->pPrev;
            if (gpARAMState->pUnused->pPrev != NULL) {
                gpARAMState->pUnused->pPrev->pNext = pTransfer;
            }
            gpARAMState->pUnused->pPrev = pTransfer;
        }
        gpARAMState->pUnused = pTransfer;
        pTransfer++;
    }
    gpARAMState->nUnused = ARAM_NUM_TRANSFERS;
    gpARAMState->nQueued = 0;
}

// Frees the transfers, then the heap: its block of the game's heap (GoARAM_HeapClose), then its
// header and block records.
void GoARAM_CloseOnce(void) {
    if (gpARAMState->pTransfersCircular != NULL) {
        Circular_MemFreeFunc(gpARAMState->pTransfersCircular);
        gpARAMState->pTransfersCircular = NULL;
    }
    Mem_FreeFunc(gpARAMState->pTransfersMem);
    gpARAMState->pTransfersMem = NULL;
    GoARAM_HeapClose(gpARAMState->pHeap);
    gpARAMState->pHeap = NULL;
    Mem_FreeFunc(gpARAMState->pHeapMem);
    gpARAMState->pHeapMem = NULL;
}

// Empty in this build: the main loop's start-up list for the systems each game mode needs calls it.
void GoARAM_InitModule(void) {
}

// Empty in this build: the matching shut-down list calls it.
void GoARAM_CloseModule(void) {
}

// Hands out uSize bytes of ARAM, 32-byte aligned, and returns their address.
u32 GoARAM_Alloc(u32 uSize) {
    return GoARAM_HeapAlloc(gpARAMState->pHeap, uSize, 32);
}

void GoARAM_Free(u32 uAram) {
    GoARAM_HeapFree(gpARAMState->pHeap, uAram);
}

// Queues a DMA of uLength bytes from uSource to uDest (nType an ARQ_TYPE_*, uPriority an
// ARQ_PRIORITY_*) and returns its transfer, with interrupts off while it moves the transfer to
// the queued list. pfnDone(uOwner) is called when it is done; uFlags are ARAM_TRANSFER_* bits.
ARAMTransfer* GoARAM_QueueTransfer(u32 uSource, u32 uDest, u32 uLength, int nType, u32 uPriority,
                                   void (*pfnDone)(u32 uOwner), u32 uOwner, u8 uFlags) {
    ARAMTransfer* pTransfer;

    Displ_DisableInterrupts();
    pTransfer = gpARAMState->pUnused;
    if (pTransfer == pTransfer->pNext) {
        gpARAMState->pUnused = NULL;
    } else {
        if (pTransfer == gpARAMState->pUnused) {
            gpARAMState->pUnused = pTransfer->pNext;
        }
        pTransfer->pPrev->pNext = pTransfer->pNext;
        pTransfer->pNext->pPrev = pTransfer->pPrev;
        pTransfer->pPrev = pTransfer;
        pTransfer->pNext = pTransfer;
    }
    gpARAMState->nUnused--;
    if (gpARAMState->pQueued != NULL) {
        pTransfer->pNext = gpARAMState->pQueued;
        pTransfer->pPrev = gpARAMState->pQueued->pPrev;
        if (gpARAMState->pQueued->pPrev != NULL) {
            gpARAMState->pQueued->pPrev->pNext = pTransfer;
        }
        gpARAMState->pQueued->pPrev = pTransfer;
    }
    gpARAMState->pQueued = pTransfer;
    gpARAMState->nQueued++;
    Displ_RestoreInterrupts();
    pTransfer->pfnDone = pfnDone;
    pTransfer->uOwner = uOwner;
    pTransfer->uFlags = uFlags;
    pTransfer->nType = nType;
    pTransfer->uMain = (nType == ARQ_TYPE_MRAM_TO_ARAM) ? uSource : uDest;
    pTransfer->uLength = uLength;
    pTransfer->nState = ARAM_TRANSFER_QUEUED;
    ARQPostRequest(&pTransfer->request, uOwner, nType, uPriority, uSource, uDest, uLength,
                   GoARAM_HandleTransferDone);
    return pTransfer;
}

// Takes back every transfer of uOwner still in the queued list that is ARAM_TRANSFER_CANCELLABLE,
// its request taken off the ARQ queue, and returns how many.
int GoARAM_CancelTransfers(u32 uOwner) {
    ARAMTransfer* pTransfer;
    ARAMTransfer* pNext;
    int nQueued;
    int i;
    int nCancelled = 0;

    Displ_DisableInterrupts();
    i = 0;
    pTransfer = gpARAMState->pQueued;
    nQueued = gpARAMState->nQueued;
    for (; i < nQueued; i++) {
        pNext = pTransfer->pNext;
        if ((pTransfer->uFlags & ARAM_TRANSFER_CANCELLABLE) && pTransfer->uOwner == uOwner) {
            ARQRemoveRequest(&pTransfer->request);
            GoARAM_ReleaseTransfer(pTransfer);
            nCancelled++;
        }
        pTransfer = pNext;
    }
    Displ_RestoreInterrupts();
    return nCancelled;
}

// Reads a transfer's state with interrupts off.
static ARAMTransferState GoARAM_GetTransferState(ARAMTransfer* pTransfer) {
    ARAMTransferState nState;

    Displ_DisableInterrupts();
    nState = pTransfer->nState;
    Displ_RestoreInterrupts();
    return nState;
}

// Waits for a transfer's DMA to be done, drops the CPU cache over what came into main memory, then
// gives the transfer back.
void GoARAM_WaitTransfer(ARAMTransfer* pTransfer) {
    do {
    } while (GoARAM_GetTransferState(pTransfer) != ARAM_TRANSFER_DONE);
    AudDma_CacheAfterTransfer((void*)pTransfer->uMain, pTransfer->uLength, pTransfer->nType);
    Displ_DisableInterrupts();
    GoARAM_ReleaseTransfer(pTransfer);
    Displ_RestoreInterrupts();
}

// Starts copying uSize bytes from pSrc to ARAM address uAram, the CPU cache written back first;
// GoARAM_WaitTransfer waits for it.
ARAMTransfer* GoARAM_CopyToAram(void* pSrc, u32 uAram, u32 uSize) {
    AudDma_CacheBeforeTransfer(pSrc, uSize, ARQ_TYPE_MRAM_TO_ARAM);
    return GoARAM_QueueTransfer((u32)pSrc, uAram, uSize, ARQ_TYPE_MRAM_TO_ARAM, ARQ_PRIORITY_HIGH,
                                NULL, 0, 0);
}

// Starts copying uSize bytes from ARAM address uAram to pDst, the CPU cache over pDst dropped
// first; GoARAM_WaitTransfer waits for it.
ARAMTransfer* GoARAM_CopyFromAram(void* pDst, u32 uAram, u32 uSize) {
    AudDma_CacheBeforeTransfer(pDst, uSize, ARQ_TYPE_ARAM_TO_MRAM);
    return GoARAM_QueueTransfer(uAram, (u32)pDst, uSize, ARQ_TYPE_ARAM_TO_MRAM, ARQ_PRIORITY_HIGH,
                                NULL, 0, 0);
}
