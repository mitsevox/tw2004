// uiLoadFile.c (EA's name, from its asserts; also in EA's 2002 source tree): loads the menu UI's
// files. Registers the stream handlers for the UI's data, keeps the loaded file's data (and can
// park it in ARAM while char.c borrows its buffer to load a menu golfer), and frees what was
// loaded.

#include "game.h"
#include "frontend/fe.h"
#include "core/goaram.h"
#include "game/frontend.h"

u8    lbl_80281360 = 1;         // the menus' 'GRPS'/'MPCS' data has not been copied to ARAM yet

u32   lbl_80281EF0;             // the menus' 'GRPS'/'MPCS' data's ARAM address
u32   lbl_80281EF4;             // the next multiple of 32 above its size
u32   lbl_80281EF8;             // the UI file's ARAM address while it is parked there
u32   lbl_80281EFC;             // the next multiple of 32 above its size
u8    lbl_80281F00;             // it is in ARAM (fn_8008F310), not in main memory
UINamedList* lbl_80281F04;      // the 'GRPS'/'MPCS' data: a count, then that many offsets that
                                // fn_8008EFC0 turns into pointers
u32*  lbl_80281F08;             // the 'FONS' data: a count, then that many UIFont offsets, turned
                                // into pointers the same way
void* lbl_80281F0C;             // the UI file's data, copied out of its stream object (fn_8008ED80)
char* lbl_80281F10;             // the name of the UI set: "frontend", "ingame" or "startup"

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

    lbl_801D87A8.nCount = 0;
    lbl_80281F0C = NULL;
    for (i = 0; i < UI_NUM_LOADED; i++) {
        lbl_801D87A8.ap4[i] = NULL;
    }
    lbl_80281F08 = 0;
    lbl_80281F04 = 0;
}

void fn_8008EC60(char* szSet) {
    lbl_80281F10 = szSet;
}

// Pick the UI set by the game type (a round, start-up or the menus) and register the handlers
// for its stream objects.
void fn_8008EC68(void) {
    if (gSession.nGameType == 4) {
        lbl_80281F10 = "ingame";
    } else if (gSession.nGameType == 0) {
        lbl_80281F10 = "startup";
    } else {
        lbl_80281F10 = "frontend";
    }
    UStream_RegisterHandler('DATS', fn_8008ED80);
    UStream_RegisterHandler('TXFS', fn_8008EE1C);
    UStream_RegisterHandler('FONS', fn_8008EFFC);
    UStream_RegisterHandler('GRPS', fn_8008EEB8);
    UStream_RegisterHandler('MPCS', fn_8008EEB8);
}

void fn_8008ED28(void) {
    UStream_UnregisterHandler('DATS');
    UStream_UnregisterHandler('TXFS');
    UStream_UnregisterHandler('FONS');
    UStream_UnregisterHandler('GRPS');
    UStream_UnregisterHandler('MPCS');
}

// 'DATS': keep a copy of the UI file. With game type 10 its size is noted for parking it in ARAM.
void fn_8008ED80(UStreamObject* pObject) {
    void* pData;

    if (lbl_80281EF8 == 0 && gSession.nGameType == 10) {
        lbl_80281EFC = ((pObject->uSize >> 5) + 1) << 5;
    }
    pData = fn_80009B34(pObject->uSize, 2, 32, "uiLoadFile.c", 165);
    Mem_cpy(pData, pObject->pData, pObject->uSize);
    lbl_80281F0C = pData;
    fn_80009E70(pObject);
}

// 'TXFS': a texture bank, kept when fn_8008F204 wants the object's kind.
void fn_8008EE1C(UStreamObject* pObject) {
    if (fn_8008F204((int)pObject->uId % 100000)) {
        lbl_801D87A8.ap4[lbl_801D87A8.nCount] = fn_8000FB88(pObject, NULL, 0);
        lbl_801D87A8.nCount++;
    }
    fn_80009E70(pObject);
}

// 'GRPS' and 'MPCS'. The menus' copy goes to ARAM the first time it comes in, and later ones
// are dropped: fn_8008F294 brings it back from there.
void fn_8008EEB8(UStreamObject* pObject) {
    UINamedList* pData;

    if (strcmp(lbl_80281F10, "frontend") == 0) {
        if (lbl_80281EF4 == 0) {
            lbl_80281EF4 = ((pObject->uSize >> 5) + 1) << 5;
            lbl_80281EF0 = GoARAM_Alloc(lbl_80281EF4);
        }
        if (lbl_80281360) {
            GoARAM_WaitTransfer(GoARAM_CopyToAram(pObject->pData, lbl_80281EF0, lbl_80281EF4));
            pData = fn_80009B34(pObject->uSize, 1, 32, "uiLoadFile.c", 247);
            lbl_80281360 = 0;
        } else {
            fn_80009E70(pObject);
            return;
        }
    } else {
        pData = fn_80009B34(pObject->uSize, 2, 32, "uiLoadFile.c", 257);
    }
    Mem_cpy(pData, pObject->pData, pObject->uSize);
    fn_8008EFC0(pData);
    fn_80009E70(pObject);
}

// Turn the table's offsets into pointers.
// port: the data stores 32-bit offsets where the code expects pointers, as the GameCube's are.
void fn_8008EFC0(UINamedList* pList) {
    u32 i;
    char** ppName;

    lbl_80281F04 = pList;
    for (i = 0; i < lbl_80281F04->nCount; i++) {
        ppName = &lbl_80281F04->apNames[i];
        *ppName = (char*)((uptr)*ppName + (uptr)pList);
    }
}

// 'FONS': load each font into a free font slot, and keep the slot in its place.
// port: the data stores 32-bit offsets where the code expects pointers, as the GameCube's are.
void fn_8008EFFC(UStreamObject* pObject) {
    u32* pData;
    u32 i;
    int nSlot;

    pData = fn_80009B34(pObject->uSize, 2, 32, "uiLoadFile.c", 348);
    Mem_cpy(pData, pObject->pData, pObject->uSize);
    lbl_80281F08 = pData;
    for (i = 0; i < lbl_80281F08[0]; i++) {
        lbl_80281F08[1 + i] = (uptr)((u8*)lbl_80281F08[1 + i] + (uptr)pData);
        nSlot = UFont_FindFreeSlot();
        UFont_LoadFont(nSlot, &((UIFont*)lbl_80281F08[1 + i])->nSlot, 0);
        ((UIFont*)lbl_80281F08[1 + i])->nSlot = nSlot;
    }
    fn_80009E70(pObject);
}

// szUnused: EA passes the UI set's name (fn_8009005C) to this getter and the three below.
void* fn_8008F0C0(char* szUnused) {
    return lbl_80281F0C;
}

void fn_8008F0C8(void* p) {
    if (p != NULL) {
        fn_80009E70(p);
    }
}

UILoaded* fn_8008F0F0(char* szUnused) {
    return &lbl_801D87A8;
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
    return lbl_80281F04;
}

void fn_8008F164(void* p) {
    if (p != NULL) {
        fn_80009E70(p);
    }
}

u32* fn_8008F18C(char* szUnused) {
    return lbl_80281F08;
}

// Free the fonts' slots and the 'FONS' data.
void fn_8008F194(u32* pTable) {
    u32 i;

    if (pTable != NULL) {
        for (i = 0; i < pTable[0]; i++) {
            UFont_FreeFont(((UIFont*)pTable[1 + i])->nSlot);
        }
        fn_80009E70(pTable);
    }
}

// Whether the 'TXFS' handler keeps an object of this kind (its id modulo 100000): kind 3 only in
// a lesson.
u8 fn_8008F204(int nKind) {
    if (nKind == 0 || nKind == 1) return 1;
    if (nKind == 3) return fn_80100294();
    return 1;
}

void fn_8008F24C(void) {
    if (lbl_80281F1C->pC != NULL) {
        fn_80090B10();
        fn_8008F164(lbl_80281F1C->pC);
        lbl_80281F1C->pC = NULL;
    }
}

// Bring the menus' 'GRPS'/'MPCS' data back from ARAM if the front end has none.
void fn_8008F294(void) {
    UINamedList* pData;

    if (lbl_80281F1C->pC == NULL) {
        pData = fn_80009B34(lbl_80281EF4, 1, 32, "uiLoadFile.c", 585);
        GoARAM_WaitTransfer(GoARAM_CopyFromAram(pData, lbl_80281EF0, lbl_80281EF4));
        fn_8008EFC0(pData);
        lbl_80281F1C->pC = lbl_80281F04;
        fn_80090898();
    }
}

// Park the UI file's data in ARAM (the main memory copy stays allocated).
void fn_8008F310(void) {
    lbl_80281EF8 = GoARAM_Alloc(lbl_80281EFC);
    GoARAM_WaitTransfer(GoARAM_CopyToAram(lbl_80281F0C, lbl_80281EF8, lbl_80281EFC));
    lbl_80281F00 = 1;
}

void* fn_8008F354(void) {
    return lbl_80281F0C;
}

// Bring the UI file's data back from ARAM and free the ARAM.
void fn_8008F35C(void) {
    lbl_80281F00 = 0;
    GoARAM_WaitTransfer(GoARAM_CopyFromAram(lbl_80281F0C, lbl_80281EF8, lbl_80281EFC));
    GoARAM_Free(lbl_80281EF8);
}
