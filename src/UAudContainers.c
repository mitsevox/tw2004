// UAudContainers.c: the sound engine's containers. A doubly linked list (the tracks and voices in
// use), a ring-buffer queue (the stream read requests) and a pool of fixed-size blocks (the free
// tracks and voices). None of them allocates: the caller hands in the memory.

#include "core/audcontainers.h"

// Empties a list. nslots is stored but never checked.
void UList_Reset(TListDbl* plist, u16 nslots) {
    plist->pHead = NULL;
    plist->pTail = NULL;
    plist->nSlots = nslots;
    plist->nCount = 0;
}

// Adds a link at the head of a list.
void UList_PushHead(TListDbl* plist, TListDblLink* plink) {
    if (plist->nCount != 0) {
        plist->pHead->pPrev = plink;
    } else {
        plist->pTail = plink;
    }
    plink->pNext = plist->pHead;
    plink->pPrev = NULL;
    plist->pHead = plink;
    plist->nCount++;
}

// Adds a link at the tail of a list.
void UList_PushTail(TListDbl* plist, TListDblLink* plink) {
    if (plist->nCount != 0) {
        plist->pTail->pNext = plink;
    } else {
        plist->pHead = plink;
    }
    plink->pPrev = plist->pTail;
    plink->pNext = NULL;
    plist->pTail = plink;
    plist->nCount++;
}

// Puts a link in front of pdestlink. When pdestlink is the tail (and not also the head), the link
// goes after it instead, at the tail.
void UList_InsertAt(TListDbl* plist, TListDblLink* plink, TListDblLink* pdestlink) {
    TListDblLink* pprev;

    if (pdestlink == plist->pHead) {
        UList_PushHead(plist, plink);
    } else if (pdestlink == plist->pTail) {
        UList_PushTail(plist, plink);
    } else {
        pprev = pdestlink->pPrev;
        plink->pPrev = pprev;
        plink->pNext = pdestlink;
        pprev->pNext = plink;
        pdestlink->pPrev = plink;
        plist->nCount++;
    }
}

// Takes a link out of a list.
void UList_DeleteAt(TListDbl* plist, TListDblLink* plink) {
    if (plink == plist->pTail) {
        if (plink->pPrev != NULL) {
            plist->pTail = plink->pPrev;
            plink->pPrev->pNext = NULL;
        } else {
            plist->pTail = NULL;
            plist->pHead = NULL;
        }
    } else if (plink == plist->pHead) {
        if (plink->pNext != NULL) {
            plist->pHead = plink->pNext;
            plink->pNext->pPrev = NULL;
        } else {
            plist->pTail = NULL;
            plist->pHead = NULL;
        }
    } else {
        plink->pPrev->pNext = plink->pNext;
        plink->pNext->pPrev = plink->pPrev;
    }
    plist->nCount--;
}

// Sets up an empty queue over buff, nslots items of itemSize bytes.
void UQueue_Reset(TQueue* pq, void* buff, u16 nslots, u32 itemSize) {
    pq->pBase = buff;
    pq->pEnd = (u8*)buff + nslots * itemSize;
    pq->pWrite = buff;
    pq->pRead = buff;
    pq->nMax = nslots;
    pq->nCount = 0;
    pq->nItemSize = itemSize;
}

// Adds an item and returns its slot for the caller to fill. It does not check for a full queue:
// the callers do, against nMax.
u8* UQueue_Push(TQueue* pq) {
    u8* pret;

    pret = pq->pWrite;
    pq->nCount++;
    pq->pWrite += pq->nItemSize;
    if (pq->pWrite >= pq->pEnd) {
        pq->pWrite -= pq->pEnd - pq->pBase;
    }
    return pret;
}

// Takes the oldest item, or returns NULL when the queue is empty.
u8* UQueue_Pop(TQueue* pq) {
    u8* pret;

    if (pq->nCount - 1 < 0) return NULL;
    pret = pq->pRead;
    pq->nCount--;
    pq->pRead += pq->nItemSize;
    if (pq->pRead >= pq->pEnd) {
        pq->pRead -= pq->pEnd - pq->pBase;
    }
    return pret;
}

// Cuts pBuffer into nEntries blocks of entrySizeBytes bytes and chains them all as free.
void UPool_Init(TPool* pPool, void* pBuffer, u32 nEntries, u32 entrySizeBytes) {
    TListSglLink* pNode;
    u32 iEntry;
    char* pEntry;

    pNode = pBuffer;
    pPool->head.pNext = pNode;
    pEntry = (char*)pNode;
    for (iEntry = 0; iEntry < nEntries - 1; iEntry++) {
        pEntry += entrySizeBytes;
        pNode->pNext = (TListSglLink*)pEntry;
        pNode = (TListSglLink*)pEntry;
    }
    pNode->pNext = &pPool->head;
    pPool->nFree = nEntries;
}

// Takes a free block, or returns NULL when none is left.
TListSglLink* UPool_Alloc(TPool* pPool) {
    TListSglLink* pNode;

    pNode = pPool->head.pNext;
    if (pNode == &pPool->head) return NULL;
    pPool->head.pNext = pNode->pNext;
    pPool->nFree--;
    return pNode;
}

// Gives a block back to the pool.
void UPool_Free(TPool* pPool, TListSglLink* pNode) {
    pNode->pNext = pPool->head.pNext;
    pPool->head.pNext = pNode;
    pPool->nFree++;
}
