// Code80090940.c (our name): the pictures of the UI file's picture table (its kind-2 entries, each a
// named record holding a picture file; uiProcessInterface.c UI_ResolveFileEntries notes the table in
// gUIState.n3C): decode an entry's picture, mark it to be freed, free it, and free the marked
// ones or all of them. The polygon and arc elements load and release them with their screens
// (fe_movies.c UI_LoadEntryPicture, UI_ReleaseEntryPicture).
// Which file it belongs to is not known: it lies between uiProcessInterface.c and fe_movies.c
// (EA's uiProcessPolygon.c), has no data or float constants of its own, and TW2003 has the same
// code in the same place, so no pooled constant or string marks a boundary. Both neighbours use
// gUIState.n3C (fe_movies.c UI_ShowDemoLoadingScreen) and both call into it.

#include "game_types.h"
#include "llpict.h"
#include "frontend/fe.h"
#include "game/frontend.h"

void fn_80008380(void);

void UI_FreeEntryPicture(int nEntry);
void UI_FreeMarkedEntryPictures(void);

// Make the picture of entry nEntry of the UI file's picture table (the table of kind-2 entries,
// uiProcessInterface.c UI_ResolveFileEntries notes it): bring the menus' data back from ARAM if they are out
// (UI_RestoreMenuPictures), decode the entry's picture file (its UIMovieData record) into an LLPict kept in
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
// for a picture entry and uiProcessInterface.c UI_ResolveFileEntries (which tests u0 == 2) does not resolve
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

// Free the pictures of every entry of the UI file's picture table (uiLoadFile.c UI_FreeMenuPictures and the
// UI's shutdown, uiProcessInterface.c UI_CloseInterface, call it).
void UI_FreeAllEntryPictures(void) {
    int nCount;
    int i = 0;

    nCount = gpFrontEnd->pFile->p8->apTables[gUIState.n3C]->nCount;
    for (; i < nCount; i++) {
        UI_FreeEntryPicture(i);
    }
}
