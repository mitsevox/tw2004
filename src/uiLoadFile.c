// uiLoadFile.c (EA's name, from its asserts; also in EA's 2002 source tree): loads the menu UI's
// files. Registers the stream handlers for the UI's data, keeps the loaded file's data (and can
// park it in ARAM while char.c borrows its buffer to load a menu golfer), and frees what was
// loaded.

#include "game.h"
#include "frontend/fe.h"
#include "core/goaram.h"
#include "game/frontend.h"

// .sbss: defined in reverse address order (CodeWarrior lays them out last-defined-first).
char* gUIInterfaceName;             // the name of the UI set: "frontend", "ingame" or "startup"
void* gpUIFileData;             // the UI file's data, copied out of its stream object (UI_StreamLoadFile)
u32*  gpUIFonts;             // the 'FONS' data: a count, then that many UIFont offsets, turned
                                // into pointers the same way
UINamedList* gpUIPictureList;      // the 'GRPS'/'MPCS' data: a count, then that many offsets that
                                // UI_RelocatePictureList turns into pointers
u8    gbUIFileInAram;             // it is in ARAM (UI_ParkFileInAram), not in main memory
u32   gUIFileAramSize;             // the next multiple of 32 above its size
u32   gUIFileAram;             // the UI file's ARAM address while it is parked there
u32   gUIPicturesAramSize;             // the next multiple of 32 above its size
u32   gUIPicturesAram;             // the menus' 'GRPS'/'MPCS' data's ARAM address

UILoaded gUITextureBanks;

void UI_UnregisterStreamClients(void);
void UI_StreamLoadFile(UStreamObject* pObject);
void UI_StreamLoadTextures(UStreamObject* pObject);
void UI_StreamLoadPictures(UStreamObject* pObject);
void UI_RelocatePictureList(UINamedList* pList);
void UI_StreamLoadFonts(UStreamObject* pObject);
u8 UI_TestTextureGroupChkRef(int nKind);
void UI_RefreshFileEntries(void);                                 // uiProcessInterface.c
void fn_80010028(void* pBank);                          // LLTex.c: free a texture bank
int  UFont_FindFreeSlot(void);                                 // UFont.c: a free font slot
void UFont_LoadFont(int nSlot, void* pFont, int n);        // UFont.c: load a font into a slot
void UFont_FreeFont(int nSlot);                            // UFont.c: free a font slot

// Start with nothing loaded (UI_vInitModule): no UI file, texture banks, fonts or picture list. The
// ARAM copies' addresses and sizes are kept.
void UI_ResetLoadedFiles(void) {
    int i;

    gUITextureBanks.nCount = 0;
    gpUIFileData = NULL;
    for (i = 0; i < UI_NUM_LOADED; i++) {
        gUITextureBanks.ap4[i] = NULL;
    }
    gpUIFonts = 0;
    gpUIPictureList = 0;
}

// The UI set being opened ("startup", "frontend" or "ingame"; UI_OpenInterface); the 'GRPS'/'MPCS'
// handler looks for "frontend".
void UI_SetInterfaceName(char* szSet) {
    gUIInterfaceName = szSet;
}

// Pick the UI set by the game type (4, a round starting: "ingame"; 0, boot: "startup"; any other:
// "frontend") and register the handlers of the UI's stream objects: 'DATS' the UI file, 'TXFS'
// texture banks, 'FONS' fonts, 'GRPS' and 'MPCS' the picture list. streammanagerhole.c calls it
// before the menus', a round's and start-up's files are streamed.
void UI_RegisterStreamClients(void) {
    if (gSession.nGameType == 4) {
        gUIInterfaceName = "ingame";
    } else if (gSession.nGameType == 0) {
        gUIInterfaceName = "startup";
    } else {
        gUIInterfaceName = "frontend";
    }
    Stream_RegisterLoadChunkCallback('DATS', UI_StreamLoadFile);
    Stream_RegisterLoadChunkCallback('TXFS', UI_StreamLoadTextures);
    Stream_RegisterLoadChunkCallback('FONS', UI_StreamLoadFonts);
    Stream_RegisterLoadChunkCallback('GRPS', UI_StreamLoadPictures);
    Stream_RegisterLoadChunkCallback('MPCS', UI_StreamLoadPictures);
}

// Take the UI's stream object handlers off again ('DATS', 'TXFS', 'FONS', 'GRPS', 'MPCS';
// streammanagerhole.c).
void UI_UnregisterStreamClients(void) {
    Stream_UnregisterLoadChunkCallback('DATS');
    Stream_UnregisterLoadChunkCallback('TXFS');
    Stream_UnregisterLoadChunkCallback('FONS');
    Stream_UnregisterLoadChunkCallback('GRPS');
    Stream_UnregisterLoadChunkCallback('MPCS');
}

// 'DATS' handler: keep a copy of the UI file (gpUIFileData) and free the stream object. While the
// menus are being started (game type 10) and the file has never been parked in ARAM (gUIFileAram
// 0), its size is noted for UI_ParkFileInAram, rounded up past the next multiple of 32.
void UI_StreamLoadFile(UStreamObject* pObject) {
    void* pData;

    if (gUIFileAram == 0 && gSession.nGameType == 10) {
        gUIFileAramSize = ((pObject->uSize >> 5) + 1) << 5;
    }
    pData = StaticMem_Alloc(pObject->uSize, 2, 32, "uiLoadFile.c", 165);
    Mem_cpy(pData, pObject->pData, pObject->uSize);
    gpUIFileData = pData;
    StaticMem_Free(pObject);
}

// 'TXFS' handler: when UI_TestTextureGroupChkRef wants the texture group's kind (its id modulo
// 100000), build its texture bank (fn_8000FB88) into the next of gUITextureBanks' five slots
// (nothing checks the count); the stream object is freed either way.
void UI_StreamLoadTextures(UStreamObject* pObject) {
    if (UI_TestTextureGroupChkRef((int)pObject->uId % 100000)) {
        gUITextureBanks.ap4[gUITextureBanks.nCount] = fn_8000FB88(pObject, NULL, 0);
        gUITextureBanks.nCount++;
    }
    StaticMem_Free(pObject);
}

// .sdata order: defined here, after UI_RegisterStreamClients's "ingame" and "startup", as in the original.
u8    gbUIPicturesToAram = 1;         // the menus' 'GRPS'/'MPCS' data has not been copied to ARAM yet

// 'GRPS' and 'MPCS' handler: the picture list. In the menus ("frontend") the first copy that comes
// in is also copied to ARAM (its ARAM space allocated once, the first time) and kept; later copies
// are dropped, and the list stays NULL until UI_RestoreMenuPictures brings it back from ARAM.
// Outside the menus every copy is kept. A kept copy is relocated (UI_RelocatePictureList); the
// stream object is freed.
void UI_StreamLoadPictures(UStreamObject* pObject) {
    UINamedList* pData;

    if (strcmp(gUIInterfaceName, "frontend") == 0) {
        if (gUIPicturesAramSize == 0) {
            gUIPicturesAramSize = ((pObject->uSize >> 5) + 1) << 5;
            gUIPicturesAram = GoARAM_Alloc(gUIPicturesAramSize);
        }
        if (gbUIPicturesToAram) {
            GoARAM_WaitTransfer(GoARAM_CopyToAram(pObject->pData, gUIPicturesAram, gUIPicturesAramSize));
            pData = StaticMem_Alloc(pObject->uSize, 1, 32, "uiLoadFile.c", 247);
            gbUIPicturesToAram = 0;
        } else {
            StaticMem_Free(pObject);
            return;
        }
    } else {
        pData = StaticMem_Alloc(pObject->uSize, 2, 32, "uiLoadFile.c", 257);
    }
    Mem_cpy(pData, pObject->pData, pObject->uSize);
    UI_RelocatePictureList(pData);
    StaticMem_Free(pObject);
}

// Make pList the picture list (gpUIPictureList) and turn its record offsets into pointers.
// port: the data stores 32-bit offsets where the code expects pointers, as the GameCube's are.
void UI_RelocatePictureList(UINamedList* pList) {
    u32 i;
    char* pName;

    gpUIPictureList = pList;
    for (i = 0; i < gpUIPictureList->nCount; i++) {
        // fake match: the add goes through a local (in one expression the value and the offset
        // come out in each other's registers)
        pName = gpUIPictureList->apNames[i];
        pName += (uptr)pList;
        gpUIPictureList->apNames[i] = pName;
    }
}

// 'FONS' handler: keep a copy of the fonts (gpUIFonts: a count, then that many UIFont offsets),
// turn the offsets into pointers, load each font into a free font slot and note the slot in the
// font's nSlot; the stream object is freed.
// port: the data stores 32-bit offsets where the code expects pointers, as the GameCube's are.
void UI_StreamLoadFonts(UStreamObject* pObject) {
    u32* pData;
    u32 i;
    int nSlot;
    u32 uFont;

    pData = StaticMem_Alloc(pObject->uSize, 2, 32, "uiLoadFile.c", 348);
    Mem_cpy(pData, pObject->pData, pObject->uSize);
    gpUIFonts = pData;
    for (i = 0; i < gpUIFonts[0]; i++) {
        uFont = gpUIFonts[1 + i];        // fake match: through a local, as in UI_RelocatePictureList
        uFont += (uptr)pData;
        gpUIFonts[1 + i] = uFont;
        nSlot = UFont_FindFreeSlot();
        UFont_LoadFont(nSlot, &((UIFont*)gpUIFonts[1 + i])->nSlot, 0);
        ((UIFont*)gpUIFonts[1 + i])->nSlot = nSlot;
    }
    StaticMem_Free(pObject);
}

// The UI file's data (gpUIFileData) for UI_OpenInterface. szUnused: EA passes the UI set's name to
// this getter and the three like it.
void* UI_GetFileData(char* szUnused) {
    return gpUIFileData;
}

// Free the UI file's data p (UI_CloseInterface) unless it is NULL.
void UI_FreeFileData(void* p) {
    if (p != NULL) {
        StaticMem_Free(p);
    }
}

// The texture banks the 'TXFS' handler kept (gUITextureBanks), for UI_OpenInterface.
UILoaded* UI_GetTextureBanks(char* szUnused) {
    return &gUITextureBanks;
}

// Free the texture banks the 'TXFS' handler kept and clear their slots (UI_CloseInterface); nCount
// is left as it is until UI_ResetLoadedFiles.
void UI_FreeTextureBanks(UILoaded* pLoaded) {
    int i;

    for (i = 0; i < UI_NUM_LOADED; i++) {
        if (pLoaded->ap4[i] != NULL) {
            fn_80010028(pLoaded->ap4[i]);
        }
        pLoaded->ap4[i] = NULL;
    }
}

// The picture list (gpUIPictureList; NULL in the menus once it is parked in ARAM), for
// UI_OpenInterface.
UINamedList* UI_GetPictureList(char* szUnused) {
    return gpUIPictureList;
}

// Free the picture list p unless it is NULL (UI_CloseInterface, UI_FreeMenuPictures).
void UI_FreePictureList(void* p) {
    if (p != NULL) {
        StaticMem_Free(p);
    }
}

// The 'FONS' fonts (gpUIFonts), for UI_OpenInterface.
u32* UI_GetFonts(char* szUnused) {
    return gpUIFonts;
}

// Free the fonts' slots and the 'FONS' data pTable (UI_CloseInterface); NULL: nothing to do.
void UI_FreeFonts(u32* pTable) {
    u32 i;

    if (pTable != NULL) {
        for (i = 0; i < pTable[0]; i++) {
            UFont_FreeFont(((UIFont*)pTable[1 + i])->nSlot);
        }
        StaticMem_Free(pTable);
    }
}

// Whether the 'TXFS' handler keeps a texture group of kind nKind (its stream id modulo 100000):
// kind 3 only while a lesson runs, every other kind always.
u8 UI_TestTextureGroupChkRef(int nKind) {
    if (nKind == 0 || nKind == 1) return 1;
    if (nKind == 3) return Lessons_IsRunning();
    return 1;
}

// Free the menus' picture data if the front end has it: the movie entries' decoded pictures
// (fn_80090B10) and the picture list (FrontEnd.pC, NULL after); UI_RestoreMenuPictures brings it
// back from ARAM. FEgolferanim.c's FE_vUpdateGolferAll and UI_CloseInterface call it.
void UI_FreeMenuPictures(void) {
    if (gpFrontEnd->pC != NULL) {
        fn_80090B10();
        UI_FreePictureList(gpFrontEnd->pC);
        gpFrontEnd->pC = NULL;
    }
}

// If the front end has no picture list, bring the menus' copy back from ARAM (gUIPicturesAram),
// relocate it, hand it to the front end (FrontEnd.pC) and resolve the UI file's entries again
// (UI_RefreshFileEntries). Code80090940.c calls it before decoding a movie entry's picture.
void UI_RestoreMenuPictures(void) {
    UINamedList* pData;

    if (gpFrontEnd->pC == NULL) {
        pData = StaticMem_Alloc(gUIPicturesAramSize, 1, 32, "uiLoadFile.c", 585);
        GoARAM_WaitTransfer(GoARAM_CopyFromAram(pData, gUIPicturesAram, gUIPicturesAramSize));
        UI_RelocatePictureList(pData);
        gpFrontEnd->pC = gpUIPictureList;
        UI_RefreshFileEntries();
    }
}

// Copy the UI file's data to ARAM (gUIFileAram) and mark it parked; the main-memory copy stays
// allocated, and char.c's Character_GolferStreamCallbackFE borrows it (UI_GetFileBuffer) to load a
// menu golfer until UI_RestoreFileFromAram.
void UI_ParkFileInAram(void) {
    gUIFileAram = GoARAM_Alloc(gUIFileAramSize);
    GoARAM_WaitTransfer(GoARAM_CopyToAram(gpUIFileData, gUIFileAram, gUIFileAramSize));
    gbUIFileInAram = 1;
}

// The UI file's buffer (gpUIFileData), which char.c fills with a menu golfer while the file is
// parked in ARAM (UI_ParkFileInAram).
void* UI_GetFileBuffer(void) {
    return gpUIFileData;
}

// Bring the UI file's data back from ARAM and free the ARAM (gUIFileAram keeps its old value).
void UI_RestoreFileFromAram(void) {
    gbUIFileInAram = 0;
    GoARAM_WaitTransfer(GoARAM_CopyFromAram(gpUIFileData, gUIFileAram, gUIFileAramSize));
    GoARAM_Free(gUIFileAram);
}
