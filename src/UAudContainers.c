// UAudContainers.c (our name, after TW06's golf/audio/engine/utils/uaudcontainers.c, whose list,
// queue and pool functions these match in order and shape): the sound engine's containers, a
// doubly linked list (UList), a ring-buffer queue (UQueue, the stream read requests) and a pool of
// fixed-size blocks (UPool, the voices and tracks). See core/audcontainers.h.

#include "core/audcontainers.h"

// Empties a list that holds up to nSlots links (the n8 given; stored, never checked).
void UList_Reset(UList* pList, u16 n8) {
    pList->pHead = NULL;
    pList->pTail = NULL;
    pList->n8 = n8;
    pList->nCount = 0;
}

// Adds a link at the head of a list.
void UList_PushHead(UList* pList, UListNode* pNode) {
    if (pList->nCount != 0) {
        pList->pHead->pPrev = pNode;
    } else {
        pList->pTail = pNode;
    }
    pNode->pNext = pList->pHead;
    pNode->pPrev = NULL;
    pList->pHead = pNode;
    pList->nCount++;
}

// Adds a link at the tail of a list.
void UList_PushTail(UList* pList, UListNode* pNode) {
    if (pList->nCount != 0) {
        pList->pTail->pNext = pNode;
    } else {
        pList->pHead = pNode;
    }
    pNode->pPrev = pList->pTail;
    pNode->pNext = NULL;
    pList->pTail = pNode;
    pList->nCount++;
}

// Puts a link in front of pAt.
// EA bug: when pAt is the tail (and not also the head) the link goes after it (UList_PushTail)
// instead: InsertSortWorldPerf (hlaudtrack.c), inserting a track in front of the first one of lower
// priority, then leaves the higher-priority track at the tail, where Trk_AllocPerf steals first.
void UList_InsertAt(UList* pList, UListNode* pNode, UListNode* pAt) {
    UListNode* pPrev;

    if (pAt == pList->pHead) {
        UList_PushHead(pList, pNode);
    } else if (pAt == pList->pTail) {
        UList_PushTail(pList, pNode);
    } else {
        pPrev = pAt->pPrev;
        pNode->pPrev = pPrev;
        pNode->pNext = pAt;
        pPrev->pNext = pNode;
        pAt->pPrev = pNode;
        pList->nCount++;
    }
}

// Takes a link out of a list.
void UList_DeleteAt(UList* pList, UListNode* pNode) {
    if (pNode == pList->pTail) {
        if (pNode->pPrev != NULL) {
            pList->pTail = pNode->pPrev;
            pNode->pPrev->pNext = NULL;
        } else {
            pList->pTail = NULL;
            pList->pHead = NULL;
        }
    } else if (pNode == pList->pHead) {
        if (pNode->pNext != NULL) {
            pList->pHead = pNode->pNext;
            pNode->pNext->pPrev = NULL;
        } else {
            pList->pTail = NULL;
            pList->pHead = NULL;
        }
    } else {
        pNode->pPrev->pNext = pNode->pNext;
        pNode->pNext->pPrev = pNode->pPrev;
    }
    pList->nCount--;
}

// Sets up an empty queue over a buffer of nMax items.
void UQueue_Reset(UQueue* pQueue, void* pBuffer, u16 nMax, s32 nItemSize) {
    pQueue->pBase = pBuffer;
    pQueue->pEnd = (u8*)pBuffer + nMax * nItemSize;
    pQueue->pWrite = pBuffer;
    pQueue->pRead = pBuffer;
    pQueue->nMax = nMax;
    pQueue->nCount = 0;
    pQueue->nItemSize = nItemSize;
}

// Adds an item and returns its slot for the caller to fill. It does not check for a full queue.
void* UQueue_Push(UQueue* pQueue) {
    u8* pItem;

    pItem = pQueue->pWrite;
    pQueue->nCount++;
    pQueue->pWrite += pQueue->nItemSize;
    if (pQueue->pWrite >= pQueue->pEnd) {
        pQueue->pWrite -= pQueue->pEnd - pQueue->pBase;
    }
    return pItem;
}

// Takes the oldest item, or returns NULL when the queue is empty.
void* UQueue_Pop(UQueue* pQueue) {
    u8* pItem;

    if (pQueue->nCount - 1 < 0) return NULL;
    pItem = pQueue->pRead;
    pQueue->nCount--;
    pQueue->pRead += pQueue->nItemSize;
    if (pQueue->pRead >= pQueue->pEnd) {
        pQueue->pRead -= pQueue->pEnd - pQueue->pBase;
    }
    return pItem;
}

// Cuts pMem into nBlocks blocks of nBlockSize bytes and chains them all as free.
void UPool_Init(UPool* pPool, void* pMem, u32 nBlocks, u32 nBlockSize) {
    UPoolNode* pNode;
    u32 i;
    u8* pNext;

    pNode = pMem;
    pPool->head.pNext = pNode;
    pNext = (u8*)pNode;
    for (i = 0; i < nBlocks - 1; i++) {
        pNext += nBlockSize;
        pNode->pNext = (UPoolNode*)pNext;
        pNode = (UPoolNode*)pNext;
    }
    pNode->pNext = &pPool->head;
    pPool->nFree = nBlocks;
}

// Takes a free block, or returns NULL when none is left.
void* UPool_Alloc(UPool* pPool) {
    UPoolNode* pNode;

    pNode = pPool->head.pNext;
    if (pNode == &pPool->head) return NULL;
    pPool->head.pNext = pNode->pNext;
    pPool->nFree--;
    return pNode;
}

// Gives a block back to the pool.
void UPool_Free(UPool* pPool, void* pBlock) {
    ((UPoolNode*)pBlock)->pNext = pPool->head.pNext;
    pPool->head.pNext = pBlock;
    pPool->nFree++;
}
