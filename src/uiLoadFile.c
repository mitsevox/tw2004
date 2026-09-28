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
void* gpUIFileData;             // the UI file's data, copied out of its stream object (fn_8008ED80)
u32*  gpUIFonts;             // the 'FONS' data: a count, then that many UIFont offsets, turned
                                // into pointers the same way
UINamedList* gpUIPictureList;      // the 'GRPS'/'MPCS' data: a count, then that many offsets that
                                // fn_8008EFC0 turns into pointers
u8    gbUIFileInAram;             // it is in ARAM (fn_8008F310), not in main memory
u32   gUIFileAramSize;             // the next multiple of 32 above its size
u32   gUIFileAram;             // the UI file's ARAM address while it is parked there
u32   gUIPicturesAramSize;             // the next multiple of 32 above its size
u32   gUIPicturesAram;             // the menus' 'GRPS'/'MPCS' data's ARAM address

UILoaded gUITextureBanks;

void fn_8008ED28(void);
void fn_8008ED80(UStreamObject* pObject);
void fn_8008EE1C(UStreamObject* pObject);
void fn_8008EEB8(UStreamObject* pObject);
void fn_8008EFC0(UINamedList* pList);
void fn_8008EFFC(UStreamObject* pObject);
u8 fn_8008F204(int nKind);
void fn_80090898(void);                                 // uiProcessInterface.c
void fn_80010028(void* pBank);                          // LLTex.c: free a texture bank
int  UFont_FindFreeSlot(void);                                 // UFont.c: a free font slot
void UFont_LoadFont(int nSlot, void* pFont, int n);        // UFont.c: load a font into a slot
void UFont_FreeFont(int nSlot);                            // UFont.c: free a font slot

// Start with nothing loaded.
void fn_8008EC30(void) {
    int i;

    gUITextureBanks.nCount = 0;
    gpUIFileData = NULL;
    for (i = 0; i < UI_NUM_LOADED; i++) {
        gUITextureBanks.ap4[i] = NULL;
    }
    gpUIFonts = 0;
    gpUIPictureList = 0;
}

void fn_8008EC60(char* szSet) {
    gUIInterfaceName = szSet;
}

// Pick the UI set by the game type (a round, start-up or the menus) and register the handlers
// for its stream objects.
void fn_8008EC68(void) {
    if (gSession.nGameType == 4) {
        gUIInterfaceName = "ingame";
    } else if (gSession.nGameType == 0) {
        gUIInterfaceName = "startup";
    } else {
        gUIInterfaceName = "frontend";
    }
    Stream_RegisterLoadChunkCallback('DATS', fn_8008ED80);
    Stream_RegisterLoadChunkCallback('TXFS', fn_8008EE1C);
    Stream_RegisterLoadChunkCallback('FONS', fn_8008EFFC);
    Stream_RegisterLoadChunkCallback('GRPS', fn_8008EEB8);
    Stream_RegisterLoadChunkCallback('MPCS', fn_8008EEB8);
}

void fn_8008ED28(void) {
    Stream_UnregisterLoadChunkCallback('DATS');
    Stream_UnregisterLoadChunkCallback('TXFS');
    Stream_UnregisterLoadChunkCallback('FONS');
    Stream_UnregisterLoadChunkCallback('GRPS');
    Stream_UnregisterLoadChunkCallback('MPCS');
}

// 'DATS': keep a copy of the UI file. With game type 10 its size is noted for parking it in ARAM.
void fn_8008ED80(UStreamObject* pObject) {
    void* pData;

    if (gUIFileAram == 0 && gSession.nGameType == 10) {
        gUIFileAramSize = ((pObject->uSize >> 5) + 1) << 5;
    }
    pData = StaticMem_Alloc(pObject->uSize, 2, 32, "uiLoadFile.c", 165);
    Mem_cpy(pData, pObject->pData, pObject->uSize);
    gpUIFileData = pData;
    StaticMem_Free(pObject);
}

// 'TXFS': a texture bank, kept when fn_8008F204 wants the object's kind.
void fn_8008EE1C(UStreamObject* pObject) {
    if (fn_8008F204((int)pObject->uId % 100000)) {
        gUITextureBanks.ap4[gUITextureBanks.nCount] = fn_8000FB88(pObject, NULL, 0);
        gUITextureBanks.nCount++;
    }
    StaticMem_Free(pObject);
}

// .sdata order: defined here, after fn_8008EC68's "ingame" and "startup", as in the original.
u8    gbUIPicturesToAram = 1;         // the menus' 'GRPS'/'MPCS' data has not been copied to ARAM yet

// 'GRPS' and 'MPCS'. The menus' copy goes to ARAM the first time it comes in, and later ones
// are dropped: fn_8008F294 brings it back from there.
void fn_8008EEB8(UStreamObject* pObject) {
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
    fn_8008EFC0(pData);
    StaticMem_Free(pObject);
}

// Turn the table's offsets into pointers.
// port: the data stores 32-bit offsets where the code expects pointers, as the GameCube's are.
void fn_8008EFC0(UINamedList* pList) {
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

// 'FONS': load each font into a free font slot, and keep the slot in its place.
// port: the data stores 32-bit offsets where the code expects pointers, as the GameCube's are.
void fn_8008EFFC(UStreamObject* pObject) {
    u32* pData;
    u32 i;
    int nSlot;
    u32 uFont;

    pData = StaticMem_Alloc(pObject->uSize, 2, 32, "uiLoadFile.c", 348);
    Mem_cpy(pData, pObject->pData, pObject->uSize);
    gpUIFonts = pData;
    for (i = 0; i < gpUIFonts[0]; i++) {
        uFont = gpUIFonts[1 + i];        // fake match: through a local, as in fn_8008EFC0
        uFont += (uptr)pData;
        gpUIFonts[1 + i] = uFont;
        nSlot = UFont_FindFreeSlot();
        UFont_LoadFont(nSlot, &((UIFont*)gpUIFonts[1 + i])->nSlot, 0);
        ((UIFont*)gpUIFonts[1 + i])->nSlot = nSlot;
    }
    StaticMem_Free(pObject);
}

// szUnused: EA passes the UI set's name (fn_8009005C) to this getter and the three below.
void* fn_8008F0C0(char* szUnused) {
    return gpUIFileData;
}

void fn_8008F0C8(void* p) {
    if (p != NULL) {
        StaticMem_Free(p);
    }
}

UILoaded* fn_8008F0F0(char* szUnused) {
    return &gUITextureBanks;
}

// Free the texture banks the 'TXFS' handler kept.
void fn_8008F0FC(UILoaded* pLoaded) {
    int i;

    for (i = 0; i < UI_NUM_LOADED; i++) {
        if (pLoaded->ap4[i] != NULL) {
            fn_80010028(pLoaded->ap4[i]);
        }
        pLoaded->ap4[i] = NULL;
    }
}

UINamedList* fn_8008F15C(char* szUnused) {
    return gpUIPictureList;
}

void fn_8008F164(void* p) {
    if (p != NULL) {
        StaticMem_Free(p);
    }
}

u32* fn_8008F18C(char* szUnused) {
    return gpUIFonts;
}

// Free the fonts' slots and the 'FONS' data.
void fn_8008F194(u32* pTable) {
    u32 i;

    if (pTable != NULL) {
        for (i = 0; i < pTable[0]; i++) {
            UFont_FreeFont(((UIFont*)pTable[1 + i])->nSlot);
        }
        StaticMem_Free(pTable);
    }
}

// Whether the 'TXFS' handler keeps an object of this kind (its id modulo 100000): kind 3 only in
// a lesson.
u8 fn_8008F204(int nKind) {
    if (nKind == 0 || nKind == 1) return 1;
    if (nKind == 3) return Lessons_IsRunning();
    return 1;
}

void fn_8008F24C(void) {
    if (gpFrontEnd->pC != NULL) {
        fn_80090B10();
        fn_8008F164(gpFrontEnd->pC);
        gpFrontEnd->pC = NULL;
    }
}

// Bring the menus' 'GRPS'/'MPCS' data back from ARAM if the front end has none.
void fn_8008F294(void) {
    UINamedList* pData;

    if (gpFrontEnd->pC == NULL) {
        pData = StaticMem_Alloc(gUIPicturesAramSize, 1, 32, "uiLoadFile.c", 585);
        GoARAM_WaitTransfer(GoARAM_CopyFromAram(pData, gUIPicturesAram, gUIPicturesAramSize));
        fn_8008EFC0(pData);
        gpFrontEnd->pC = gpUIPictureList;
        fn_80090898();
    }
}

// Park the UI file's data in ARAM (the main memory copy stays allocated).
void fn_8008F310(void) {
    gUIFileAram = GoARAM_Alloc(gUIFileAramSize);
    GoARAM_WaitTransfer(GoARAM_CopyToAram(gpUIFileData, gUIFileAram, gUIFileAramSize));
    gbUIFileInAram = 1;
}

void* fn_8008F354(void) {
    return gpUIFileData;
}

// Bring the UI file's data back from ARAM and free the ARAM.
void fn_8008F35C(void) {
    gbUIFileInAram = 0;
    GoARAM_WaitTransfer(GoARAM_CopyFromAram(gpUIFileData, gUIFileAram, gUIFileAramSize));
    GoARAM_Free(gUIFileAram);
}
