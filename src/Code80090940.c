// Code80090940.c (our name): the pictures of the movie entries in the UI file table the front end
// shows movies from (lbl_801D87C0.n3C): make an entry's picture, mark it, free it, and free the
// marked ones or all of them. Between uiProcessInterface.c and fe_movies.c; which file it belongs to
// is not known yet (it has no data of its own; lbl_801D87C0 lies in uiProcessInterface.c's .bss
// range, but other files use it too).

#include "game_types.h"
#include "llpict.h"
#include "frontend/fe.h"
#include "game/frontend.h"

void fn_80008380(void);

void fn_800909F4(int nEntry);
void fn_80090A60(void);

// Make the picture of movie entry nEntry from its data.
UIFileEntry* fn_80090940(int nEntry) {
    UIFileEntry* pEntry;
    UIMovieData* pData;

    fn_8008F294();
    pEntry = lbl_80281F1C->pFile->p8->apTables[lbl_801D87C0.n3C]->apEntries[nEntry];
    pData = pEntry->p4;
    pEntry->p8 = (u8*)fn_8002FD00(pData->aData, pData->uSize);
    return pEntry;
}

// Mark movie entry nEntry (flag 0x10) for fn_80090A60 to free.
void fn_800909B4(int nEntry) {
    lbl_80281F1C->pFile->p8->apTables[lbl_801D87C0.n3C]->apEntries[nEntry]->u0 |= 0x10;
}

// Free the picture of movie entry nEntry.
void fn_800909F4(int nEntry) {
    UIFileEntry* pEntry = lbl_80281F1C->pFile->p8->apTables[lbl_801D87C0.n3C]->apEntries[nEntry];

    if (pEntry->p8 != NULL) {
        fn_8002FE70((LLPict*)pEntry->p8);
    }
    pEntry->p8 = NULL;
}

// Free the pictures of the marked movie entries and clear their flags, calling fn_80008380 once
// before the first.
void fn_80090A60(void) {
    u8 bFreed = 0;
    UIColorTable* pTable;
    UIFileEntry* pEntry;
    u32 i;

    if (lbl_80281F1C != NULL) {
        pTable = lbl_80281F1C->pFile->p8->apTables[lbl_801D87C0.n3C];
        for (i = 0; i < pTable->nCount; i++) {
            pEntry = pTable->apEntries[i];
            if (pEntry->u0 & 0x10) {
                if (!bFreed) {
                    fn_80008380();
                }
                if (pEntry->p8 != NULL) {
                    fn_8002FE70((LLPict*)pEntry->p8);
                }
                pEntry->p8 = NULL;
                bFreed = 1;
                pEntry->u0 = 0;
            }
        }
    }
}

// Free the pictures of every movie entry.
void fn_80090B10(void) {
    int nCount;
    int i = 0;

    nCount = lbl_80281F1C->pFile->p8->apTables[lbl_801D87C0.n3C]->nCount;
    for (; i < nCount; i++) {
        fn_800909F4(i);
    }
}
