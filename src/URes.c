// URes.c: the resource list, the stream objects the game keeps once UStream.c has delivered them
// (models, networks, 'RPNS' objects), found by type and id, with a hole's kept apart so they can
// be freed when it unloads. Also the search for a chunk in a buffer of chunks.

#include "game_types.h"
#include "engine.h"
#include "ustream.h"

UStreamObject* gpResHead;               // the newest default resource (the divider when none)
UStreamObject* gpResDivider;            // gResDivider while the list is set up, else NULL
UStreamObject* gpResTail;               // the newest of a hole's resources (the divider when none)
Resources_EResourceClass gResCurrentClass;  // the class Resources_Add adds under
UStreamObject gResDivider;              // between the two classes; never freed

// Empties the resource list: only the divider is in it, as both its head and its tail.
void Resources_InitModule(void) {
    gpResHead = NULL;
    gpResDivider = NULL;
    gpResTail = NULL;
    gResDivider.uType = TAG('X', 'X', 'X', 'X');
    gResDivider.uId = 0;
    gResDivider.pfnFree = NULL;
    gResDivider.pNext = NULL;
    gResDivider.pPrev = NULL;
    gpResDivider = &gResDivider;
    gpResHead = &gResDivider;
    gpResTail = &gResDivider;
}

void Resources_vSetCurrentResourceClass(Resources_EResourceClass eClass) {
    gResCurrentClass = eClass;
}

// Adds a resource at the current class's end of the list: a default one at the head, a hole's at
// the tail.
void Resources_Add(UStreamObject* pRes) {
    pRes->pPrev = NULL;
    pRes->pNext = NULL;
    if (gResCurrentClass == RESOURCES_CLASS_DEFAULT) {
        pRes->pPrev = gpResHead;
        gpResHead->pNext = pRes;
        gpResHead = pRes;
    } else if (gResCurrentClass == RESOURCES_CLASS_HOLE) {
        pRes->pNext = gpResTail;
        gpResTail->pPrev = pRes;
        gpResTail = pRes;
    }
}

// Whether a resource of the same type and id is in the list.
u8 Resources_IsAlreadyThere(UStreamObject* pRes) {
    UStreamObject* pCurrentRes = gpResHead;

    while (pCurrentRes != NULL) {
        if (pCurrentRes->uType == pRes->uType && pCurrentRes->uId == pRes->uId) {
            break;
        }
        pCurrentRes = pCurrentRes->pPrev;
    }
    return pCurrentRes != NULL;
}

// Whether a resource of this type and id is in the list.
u8 Resources_IsIDRefAlreadyThere(u32 uType, u32 uId) {
    UStreamObject* pRes = gpResHead;

    while (pRes != NULL) {
        if (pRes->uType == uType && pRes->uId == uId) {
            break;
        }
        pRes = pRes->pPrev;
    }
    return pRes != NULL;
}

// Takes a resource out of the list and frees it, calling its own free function first. The divider
// is unlinked but never freed.
void Resources_Del(UStreamObject* pRes) {
    if (pRes->pfnFree != NULL) {
        pRes->pfnFree(pRes);
    }
    if (pRes->pNext != NULL) {
        pRes->pNext->pPrev = pRes->pPrev;
    } else {
        gpResHead = pRes->pPrev;
        if (gpResHead != NULL) {
            gpResHead->pNext = NULL;
        }
    }
    if (pRes->pPrev != NULL) {
        pRes->pPrev->pNext = pRes->pNext;
    } else {
        gpResTail = pRes->pNext;
        if (gpResTail != NULL) {
            gpResTail->pPrev = NULL;
        }
    }
    if (pRes != &gResDivider) {
        Circular_MemFreeFunc(pRes);
    }
}

// Frees every resource of both classes and unlinks the divider: the list is empty until
// Resources_InitModule sets it up again.
void Resources_CloseModule(void) {
    UStreamObject* pRes = gpResHead;
    UStreamObject* pDelRes;

    while (pRes != NULL) {
        pDelRes = pRes;
        pRes = pRes->pPrev;
        Resources_Del(pDelRes);
    }
    gpResHead = NULL;
    gpResDivider = NULL;
    gpResTail = NULL;
}

// Frees every resource of a class: when a hole unloads, the hole's ones past the divider. The
// divider stays. The branch for the default class has no caller.
void Resources_DelAllClassResources(Resources_EResourceClass eClass) {
    UStreamObject* pRes;
    UStreamObject* pDelRes;

    if (eClass == RESOURCES_CLASS_DEFAULT) {
        pRes = gpResHead;
        while (pRes != gpResDivider) {
            pDelRes = pRes;
            pRes = pRes->pPrev;
            Resources_Del(pDelRes);
        }
    } else if (eClass == RESOURCES_CLASS_HOLE) {
        if (gpResDivider != NULL) {
            pRes = gpResDivider->pPrev;
            while (pRes != NULL) {
                pDelRes = pRes;
                pRes = pRes->pPrev;
                Resources_Del(pDelRes);
            }
        }
    }
}

// The resource of this type and id; NULL when there is none.
UStreamObject* Resources_Get(u32 uType, u32 uId) {
    UStreamObject* pRes;

    for (pRes = gpResHead; pRes != NULL; pRes = pRes->pPrev) {
        if (pRes->uType == uType && pRes->uId == uId) {
            return pRes;
        }
    }
    return NULL;
}

// The data of the chunk with this tag and id in a buffer of chunks (each a TChunkR header, its data
// and padding to 4 bytes). When there is none, this points past the buffer.
void* Resources_GetDataInBuffer(u8* pBuffer, u32 uBuffSize, u32 uTag, u32 uId) {
    u8 bFound = 0;
    u8* pPos = pBuffer;

    while (!bFound && (u32)(pPos - pBuffer) < uBuffSize) {
        bFound = ((TChunkR*)pPos)->uId == uId && ((TChunkR*)pPos)->uTag == uTag;
        if (!bFound) {
            pPos += ((TChunkR*)pPos)->uSize;
            while ((u32)pPos & 3) {
                pPos++;
            }
        }
    }
    return pPos + sizeof(TChunkR);
}

// The chunk with this tag and id in a buffer of chunks; NULL when there is none.
TChunkR* Resources_GetChunkInBuffer(u8* pBuffer, u32 uBuffSize, u32 uTag, u32 uId) {
    u8* pPos = pBuffer;
    TChunkR* pChunk = NULL;
    u8 bFound = 0;

    while (!bFound && (u32)(pPos - pBuffer) < uBuffSize) {
        pChunk = (TChunkR*)pPos;
        bFound = pChunk->uId == uId && pChunk->uTag == uTag;
        if (!bFound) {
            pPos += pChunk->uSize;
            while ((u32)pPos & 3) {
                pPos++;
            }
        }
    }
    if (bFound) {
        return pChunk;
    }
    return NULL;
}

// For a resource that streamed in again while its old copy is in the list: frees the new copy. It
// looks the old one up first, but does nothing with it.
void Resources_UpdateOldResourceTime(UStreamObject* pRes) {
    UStreamObject* pOldRes = gpResHead;

    while (pOldRes != NULL) {
        if (pOldRes->uType == pRes->uType && pOldRes->uId == pRes->uId) {
            break;
        }
        pOldRes = pOldRes->pPrev;
    }
    Circular_MemFreeFunc(pRes);
}
