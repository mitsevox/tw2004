// Code80090940.c (our name): the pictures of the movie entries in the UI file table the front end
// shows movies from (gUIState.n3C): make an entry's picture, mark it, free it, and free the
// marked ones or all of them. Between uiProcessInterface.c and fe_movies.c; which file it belongs to
// is not known yet (it has no data of its own; gUIState lies in uiProcessInterface.c's .bss
// range, but other files use it too).

#include "game_types.h"
#include "llpict.h"
#include "frontend/fe.h"
#include "game/frontend.h"

void fn_80008380(void);

void UI_FreeEntryPicture(int nEntry);
void UI_FreeMarkedEntryPictures(void);

// Make the picture of entry nEntry of the UI file's picture table (the table of kind-2 entries,
// uiProcessInterface.c fn_8008FE88 notes it): bring the menus' data back from ARAM if they are out
// (fn_8008F294), decode the entry's picture file (its UIMovieData record) into an LLPict kept in
// the entry's p8. Returns the entry.
UIFileEntry* UI_DecodeEntryPicture(int nEntry) {
    UIFileEntry* pEntry;
    UIMovieData* pData;

    UI_RestoreMenuPictures();
    pEntry = gpFrontEnd->pFile->p8->apTables[gUIState.n3C]->apEntries[nEntry];
    pData = pEntry->p4;
    pEntry->p8 = (u8*)fn_8002FD00(pData->aData, pData->uSize);
    return pEntry;
}

// Mark entry nEntry of the UI file's picture table (flag 0x10) for UI_FreeMarkedEntryPictures to
// free its picture.
void UI_MarkEntryPictureForFree(int nEntry) {
    gpFrontEnd->pFile->p8->apTables[gUIState.n3C]->apEntries[nEntry]->u0 |= 0x10;
}

// Free the picture of entry nEntry of the UI file's picture table, if it has one.
void UI_FreeEntryPicture(int nEntry) {
    UIFileEntry* pEntry = gpFrontEnd->pFile->p8->apTables[gUIState.n3C]->apEntries[nEntry];

    if (pEntry->p8 != NULL) {
        fn_8002FE70((LLPict*)pEntry->p8);
    }
    pEntry->p8 = NULL;
}

// With a front end, free the pictures of the picture table's entries marked by
// UI_MarkEntryPictureForFree (flag 0x10), waiting for the GPU (fn_80008380) before the first. Each
// such entry's u0 is then cleared whole, kind 2 included, so UI_LoadEntryPicture no longer takes it
// for a picture entry and uiProcessInterface.c fn_8008FE88 (which tests u0 == 2) does not resolve
// it again. gomainloop.c calls it once a frame.
void UI_FreeMarkedEntryPictures(void) {
    u8 bFreed = 0;
    UIColorTable* pTable;
    UIFileEntry* pEntry;
    u32 i;

    if (gpFrontEnd != NULL) {
        pTable = gpFrontEnd->pFile->p8->apTables[gUIState.n3C];
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

// Free the pictures of every entry of the UI file's picture table (uiLoadFile.c fn_8008F24C and the
// UI's shutdown, uiProcessInterface.c fn_80090400, call it).
void UI_FreeAllEntryPictures(void) {
    int nCount;
    int i = 0;

    nCount = gpFrontEnd->pFile->p8->apTables[gUIState.n3C]->nCount;
    for (; i < nCount; i++) {
        UI_FreeEntryPicture(i);
    }
}
