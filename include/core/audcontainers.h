// audcontainers.h (our name): the small containers the sound engine keeps its voices, tracks and
// requests in (UAudContainers.c): a doubly linked list, a ring-buffer queue of fixed-size items and
// a pool of fixed-size blocks. None of them allocates: the caller hands in the memory.

#ifndef CORE_AUDCONTAINERS_H
#define CORE_AUDCONTAINERS_H

#include "engine.h"

// A link in a UList. It sits at the start of whatever the list holds.
typedef struct UListNode {
    struct UListNode* pPrev;    // 0x0    towards the head, NULL at the head
    struct UListNode* pNext;    // 0x4    towards the tail, NULL at the tail
} UListNode;
LAYOUT_ASSERT(UListNode, 0x8);

typedef struct UList {
    UListNode* pHead;           // 0x0
    UListNode* pTail;           // 0x4
    u16        n8;              // 0x8    set by fn_800ADE70, never read here
    u16        nCount;          // 0xA    links in the list
} UList;
LAYOUT_ASSERT(UList, 0xC);

// A ring buffer of nMax items of nItemSize bytes each, from pBase to pEnd.
typedef struct UQueue {
    u8*  pBase;                 // 0x0    the buffer
    u8*  pEnd;                  // 0x4    one past its end
    u8*  pRead;                 // 0x8    the oldest item, taken by fn_800AE084
    u8*  pWrite;                // 0xC    where fn_800AE03C puts the next item
    u16  nMax;                  // 0x10   items the buffer holds
    u16  nCount;                // 0x12   items in the queue
    s32  nItemSize;             // 0x14
} UQueue;
LAYOUT_ASSERT(UQueue, 0x18);

// A free block in a UPool: its first word links to the next free block.
typedef struct UPoolNode {
    struct UPoolNode* pNext;    // 0x0
} UPoolNode;

// A pool of fixed-size blocks. The free blocks form a chain from head.pNext that ends at the pool
// itself (&head), not at NULL.
typedef struct UPool {
    UPoolNode head;             // 0x0    head.pNext: the first free block
    s32       nFree;            // 0x4    free blocks
} UPool;
LAYOUT_ASSERT(UPool, 0x8);

void       fn_800ADE70(UList* pList, u16 n8);
void       fn_800ADE88(UList* pList, UListNode* pNode);
void       fn_800ADEC8(UList* pList, UListNode* pNode);
void       fn_800ADF08(UList* pList, UListNode* pNode, UListNode* pAt);
void       fn_800ADF6C(UList* pList, UListNode* pNode);
void       fn_800AE00C(UQueue* pQueue, void* pBuffer, u16 nMax, s32 nItemSize);
void*      fn_800AE03C(UQueue* pQueue);
void*      fn_800AE084(UQueue* pQueue);
void       fn_800AE0DC(UPool* pPool, void* pMem, u32 nBlocks, u32 nBlockSize);
void*      fn_800AE1AC(UPool* pPool);
void       fn_800AE1DC(UPool* pPool, void* pBlock);

#endif
