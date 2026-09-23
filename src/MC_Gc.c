// MC_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree): the GameCube memory card
// layer under MC.c: mounting, checking and formatting the cards and reading and writing the save
// file through the CARD library, turning its results into the game's error codes, and keeping
// what it learns about each card in lbl_801F1510 (include/core/memcard.h).
//
// port: a PC build replaces this file with file I/O.

#include "core/memcard.h"
#include "frontend/fe.h"

void fn_8009DC80(s32 nPort, s32 nSlot, s32 nResult);
void fn_8009DCE8(void);
void fn_8009EB30(UStreamObject* pObject);
void fn_8009EB38(UStreamObject* pObject);
s32  fn_8009EECC(s32 nPort, s32 nSlot, const char* pName);

// Park the save file images: take the ARAM for them once, allocate both, then let fn_8009EF98 copy
// them to ARAM and free them.
void fn_8009CC00(void) {
    lbl_80281FC0 = MC_BUFFER_SIZE + 0x20;
    if (lbl_80281FC4 == 0) {
        lbl_80281FC4 = fn_800B6564(MC_BUFFER_SIZE + 0x20);
    }
    lbl_80281FE8 = fn_80009B34(MC_BUFFER_SIZE, 1, 0x20, "MC_Gc.c", 129);
    lbl_80281FE4 = lbl_80281FE8;
    lbl_80281FEC = lbl_80281FE8;
    lbl_80281FDC = fn_80009B34(MC_BUFFER_SIZE, 1, 0x20, "MC_Gc.c", 133);
    lbl_80281FD8 = lbl_80281FDC;
    lbl_80281FE0 = lbl_80281FDC;
    fn_8009EF98();
}

// Free everything: the icon and banner objects, MC.c's data, the first image and the ARAM.
void fn_8009CC88(void) {
    if (lbl_80281FB8 != NULL) {
        fn_80009E70(lbl_80281FB8);
        lbl_80281FB8 = NULL;
    }
    if (lbl_80281FBC != NULL) {
        fn_80009E70(lbl_80281FBC);
        lbl_80281FBC = NULL;
    }
    fn_800A1BE0();
    if (lbl_80281FE8 != NULL) {
        fn_80009E70(lbl_80281FE8);
        lbl_80281FEC = NULL;
        lbl_80281FE4 = NULL;
        lbl_80281FE8 = NULL;
    }
    if (lbl_80281FC4 != 0) {
        fn_800B6594(lbl_80281FC4);
        lbl_80281FC4 = 0;
    }
}

// Start up: no multitaps, one slot per port, no card damaged yet; then mount each card once and
// unmount it again, which notes what is in each port.
void fn_8009CD10(void) {
    int i;
    lbl_80282008[0] = 0;
    lbl_80282008[1] = 0;
    lbl_80282000[0] = 1;
    lbl_80282000[1] = 1;
    lbl_80281FD0[0] = 0;
    lbl_80281FD0[1] = 0;
    for (i = 0; i < MC_NUM_PORTS; i++) {
        fn_8009DCEC(i, 0);
    }
}

void fn_8009CD7C(void) {
}

void fn_8009CD80(s32 nPort, s32 nSlot) {
    fn_8009DCEC(nPort, nSlot);
}

// Look for the save file, then for its backup: 0 when one is there.
s32 fn_8009D6CC(s32 nPort, s32 nSlot) {
    s32 nResult = fn_8009D614(nPort, nSlot, MC_FILE_NAME);
    if (nResult != 0) {
        nResult = fn_8009D614(nPort, nSlot, MC_BACKUP_NAME);
    }
    return nResult;
}

// The detach callback given to CARDMountAsync: the card was pulled out, so it is no longer mounted.
// port: GameCube only.
void fn_8009D728(s32 nPort, s32 nResult) {
    if (nResult == -3) {
        lbl_801F1510[nPort][0].uFlags &= ~MC_CARD_MOUNTED;
    }
}

// Note a mount's result: an I/O error marks the card damaged (lbl_80281FD0).
void fn_8009DC80(s32 nPort, s32 nSlot, s32 nResult) {
    fn_8009DCE8();
    switch (nResult) {
    case -34:
    case -33:
    case -32:
    case -31:
    case -29:
    case -28:
    case -27:
    case -26:
    case -24:
    case -23:
    case -22:
    case -18:
    case -12:
    case -6:
    case -5:
    case -3:
        break;
    case -25:
        lbl_80281FD0[nPort] = 1;
        break;
    }
}

void fn_8009DCE8(void) {
}

// Mount the card to see what is there, note the result, and unmount it.
void fn_8009DCEC(s32 nPort, s32 nSlot) {
    s32 nResult = fn_8009D74C(nPort, nSlot);
    fn_8009DC80(nPort, nSlot, nResult);
    fn_8009DBAC(nPort, nSlot);
}

// Whether the card holds a save file (0) or its backup. pName is not used.
s32 fn_8009DD44(s32 nPort, s32 nSlot, const char* pName) {
    s32 nResult = fn_8009F734(nPort, nSlot);
    if (nResult != 0) return nResult;
    return fn_8009D6CC(nPort, nSlot);
}

// Take the stream objects: the save file's icon ('MCI ') and banner ('MCB '), and MC.c's 'eagm'.
void fn_8009EA98(void) {
    UStream_RegisterHandler('MCI ', fn_8009EB30);
    UStream_RegisterHandler('MCB ', fn_8009EB38);
    UStream_RegisterHandler('eagm', fn_800A1D4C);
}

void fn_8009EAF0(void) {
    UStream_UnregisterHandler('MCI ');
    UStream_UnregisterHandler('MCB ');
    UStream_UnregisterHandler('eagm');
}

void fn_8009EB30(UStreamObject* pObject) {
    lbl_80281FB8 = pObject;
}

void fn_8009EB38(UStreamObject* pObject) {
    lbl_80281FBC = pObject;
}

// Called while waiting for the CARD library.
void fn_8009EB40(s32 nPort, s32 nSlot) {
}

// Read the backup copy of the save file into the first image.
s32 fn_8009EF68(s32 nPort, s32 nSlot) {
    return fn_8009EECC(nPort, nSlot, MC_BACKUP_NAME);
}

u32 fn_8009EF90(void) {
    return lbl_80281FC0;
}

// Read nLen bytes of open file nFile into pBuf, from where the last read stopped.
s32 fn_8009F208(s32 nFile, void* pBuf, s32 nLen) {
    CARDRead(&lbl_801E3180[nFile], pBuf, nLen, lbl_80281FC8);
    lbl_80281FC8 += nLen;
    return 0;
}

s32 fn_8009F35C(void) {
    return 0;
}

s32 fn_8009F364(void) {
    return 0;
}

// Close open file nFile (-12 when none is open).
s32 fn_8009F488(s32 nFile) {
    if (lbl_802813D8 == -1) return -12;
    CARDClose(&lbl_801E3180[nFile]);
    lbl_802813D8 = -1;
    return 0;
}
