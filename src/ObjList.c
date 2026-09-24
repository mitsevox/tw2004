// ObjList.c (our name): the list of stream objects the game keeps after UStream.c delivered them
// (the hole's networks, models, 'RPNS' objects), found by type and id. New objects go at the head
// or the tail; a fixed mark object between the two sides lets the loader release one side at once.
// Also a search through a block of tagged records. Its data ends at 0x801A25EC, padded to the
// next file's 0x801A25F0.

#include "game_types.h"
#include "engine.h"
#include "ustream.h"

// The globals, in reverse address order (CodeWarrior lays them out last-defined-first).
UStreamObject* lbl_80281BFC;            // the head: the newest object added there
UStreamObject* lbl_80281BF8;            // the mark (lbl_801A25B8 once the list is set up)
UStreamObject* lbl_80281BF4;            // the tail
int lbl_80281BF0;                       // where fn_8000B4B8 adds: 0 the head, 1 the tail
UStreamObject lbl_801A25B8;             // the mark object, never freed

void fn_8000B46C(void) {
    lbl_80281BFC = NULL;
    lbl_80281BF8 = NULL;
    lbl_80281BF4 = NULL;
    lbl_801A25B8.uType = TAG('X', 'X', 'X', 'X');
    lbl_801A25B8.uId = 0;
    lbl_801A25B8.pfn8 = NULL;
    lbl_801A25B8.pNext = NULL;
    lbl_801A25B8.pPrev = NULL;
    lbl_80281BF8 = &lbl_801A25B8;
    lbl_80281BFC = &lbl_801A25B8;
    lbl_80281BF4 = &lbl_801A25B8;
}

void fn_8000B4B0(int nEnd) {
    lbl_80281BF0 = nEnd;
}

void fn_8000B4B8(UStreamObject* pObject) {
    pObject->pPrev = NULL;
    pObject->pNext = NULL;
    if (lbl_80281BF0 == 0) {
        pObject->pPrev = lbl_80281BFC;
        lbl_80281BFC->pNext = pObject;
        lbl_80281BFC = pObject;
    } else if (lbl_80281BF0 == 1) {
        pObject->pNext = lbl_80281BF4;
        lbl_80281BF4->pPrev = pObject;
        lbl_80281BF4 = pObject;
    }
}

u8 fn_8000B508(UStreamObject* pObject) {
    UStreamObject* p = lbl_80281BFC;

    while (p != NULL) {
        if (p->uType == pObject->uType && p->uId == pObject->uId) {
            break;
        }
        p = p->pPrev;
    }
    return p != NULL;
}

u8 fn_8000B54C(u32 uType, u32 uId) {
    UStreamObject* p = lbl_80281BFC;

    while (p != NULL) {
        if (p->uType == uType && p->uId == uId) {
            break;
        }
        p = p->pPrev;
    }
    return p != NULL;
}

void fn_8000B588(UStreamObject* pObject) {
    if (pObject->pfn8 != NULL) {
        pObject->pfn8(pObject);
    }
    if (pObject->pNext != NULL) {
        pObject->pNext->pPrev = pObject->pPrev;
    } else {
        lbl_80281BFC = pObject->pPrev;
        if (lbl_80281BFC != NULL) {
            lbl_80281BFC->pNext = NULL;
        }
    }
    if (pObject->pPrev != NULL) {
        pObject->pPrev->pNext = pObject->pNext;
    } else {
        lbl_80281BF4 = pObject->pNext;
        if (lbl_80281BF4 != NULL) {
            lbl_80281BF4->pPrev = NULL;
        }
    }
    if (pObject != &lbl_801A25B8) {
        fn_80009E70(pObject);
    }
}

void fn_8000B63C(void) {
    UStreamObject* p = lbl_80281BFC;
    UStreamObject* pObject;

    while (p != NULL) {
        pObject = p;
        p = p->pPrev;
        fn_8000B588(pObject);
    }
    lbl_80281BFC = NULL;
    lbl_80281BF8 = NULL;
    lbl_80281BF4 = NULL;
}

void fn_8000B68C(int nEnd) {
    UStreamObject* p;
    UStreamObject* pObject;

    if (nEnd == 0) {
        p = lbl_80281BFC;
        while (p != lbl_80281BF8) {
            pObject = p;
            p = p->pPrev;
            fn_8000B588(pObject);
        }
    } else if (nEnd == 1) {
        if (lbl_80281BF8 != NULL) {
            p = lbl_80281BF8->pPrev;
            while (p != NULL) {
                pObject = p;
                p = p->pPrev;
                fn_8000B588(pObject);
            }
        }
    }
}

UStreamObject* fn_8000B70C(u32 uType, u32 uId) {
    UStreamObject* p;

    for (p = lbl_80281BFC; p != NULL; p = p->pPrev) {
        if (p->uType == uType && p->uId == uId) {
            return p;
        }
    }
    return NULL;
}

// The data of the record with this tag and id. When there is none, this points past the block.
void* fn_8000B748(u8* pBlock, u32 uLen, u32 uTag, u32 uId) {
    u8 bFound = 0;
    u8* p = pBlock;

    while (!bFound && (u32)(p - pBlock) < uLen) {
        bFound = ((TagRecord*)p)->uId == uId && ((TagRecord*)p)->uTag == uTag;
        if (!bFound) {
            p += ((TagRecord*)p)->uSize;
            while ((u32)p & 3) {
                p++;
            }
        }
    }
    return p + sizeof(TagRecord);
}

TagRecord* fn_8000B7B0(u8* pBlock, u32 uLen, u32 uTag, u32 uId) {
    u8* p = pBlock;
    TagRecord* pRec = NULL;
    u8 bFound = 0;

    while (!bFound && (u32)(p - pBlock) < uLen) {
        pRec = (TagRecord*)p;
        bFound = pRec->uId == uId && pRec->uTag == uTag;
        if (!bFound) {
            p += pRec->uSize;
            while ((u32)p & 3) {
                p++;
            }
        }
    }
    if (bFound) {
        return pRec;
    }
    return NULL;
}

// Frees the object. It first looks for an object of the same type and id in the list, but does
// nothing with the answer.
void fn_8000B830(UStreamObject* pObject) {
    UStreamObject* p = lbl_80281BFC;

    while (p != NULL) {
        if (p->uType == pObject->uType && p->uId == pObject->uId) {
            break;
        }
        p = p->pPrev;
    }
    fn_80009E70(pObject);
}
