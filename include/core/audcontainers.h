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
    u32       nFree;            // 0x4    free blocks
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

// UAudMemStack.c: a stack allocator. Blocks are cut from the top of one buffer, and each one is
// recorded in a table so it can be given back by its address.
typedef struct UAudMemStackBlock {
    u8*  pMem;                  // 0x0    the block's start
    u32  uSize;                 // 0x4    its size, rounded up to the stack's alignment; 0 once freed
    u32  unk8;                  // 0x8
} UAudMemStackBlock;
LAYOUT_ASSERT(UAudMemStackBlock, 0xC);

typedef struct UAudMemStack {
    u8*                pTop;        // 0x0    where the next block starts
    u8*                pEnd;        // 0x4    one past the buffer's end
    u32                nMaxBlocks;  // 0x8    entries in pBlocks
    UAudMemStackBlock* pBlocks;     // 0xC
    u32                nBlocks;     // 0x10   blocks handed out
    u32                n14;         // 0x14   cleared at set-up, counted against nMaxBlocks
    u32                nAlign;      // 0x18   a power of two
    u8                 bOwnBlocks;  // 0x1C   pBlocks was allocated by the stack itself
} UAudMemStack;
LAYOUT_ASSERT(UAudMemStack, 0x20);

void*      fn_800B5BD8(u32 uSize);      // the sound engine's allocator
void       fn_800B5C04(void* p);        // gives back what fn_800B5BD8 handed out

#endif
