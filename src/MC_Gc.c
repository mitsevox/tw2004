// MC_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree): the GameCube memory card
// layer under MC.c: mounting, checking and formatting the cards and reading and writing the save
// file through the CARD library, turning its results into the game's error codes, and keeping
// what it learns about each card in lbl_801F1510 (include/core/memcard.h).
//
// port: a PC build replaces this file with file I/O.
//
// The CARD library calls take the port through an int copy, nChan (and fn_8009E280 its file
// number, nFileNo). The copies do nothing, but EA's code evidently had them: the int-to-long copy
// is what the original's register use shows (fake match: nothing else reproduces it).

#include "core/memcard.h"
#include "frontend/fe.h"
#include "core/goaram.h"

void fn_8006C63C(void);         // called in every wait for a CARD library result (not yet known)
void fn_8009DC80(s32 nPort, s32 nSlot, s32 nResult);
void fn_8009DCE8(void);
void fn_8009EB30(UStreamObject* pObject);
void fn_8009EB38(UStreamObject* pObject);
void fn_8009EB40(s32 nPort, s32 nSlot);
s32  fn_8009E280(s32 nPort, s32 nSlot, s32 nFile, CARDStat* pStat);
s32  fn_8009ED34(s32 nPort, s32 nSlot, const char* pName, const char* pBackupName);
s32  fn_8009EECC(s32 nPort, s32 nSlot, const char* pName);
s32  fn_8009EF68(s32 nPort, s32 nSlot);
s32  fn_8009F4D8(const void* pA, const void* pB);
s32  fn_80125194(s32 a, s32 b);         // EA Sports Bio (0x80125194)
s32  fn_801255C4(s32* pPos);            // EASportsBio.c
s32  fn_801255F8(void);                 // EASportsBio.c: the EA Sports Bio file's size
void fn_8012CCCC(int uHandle);          // EASBStorage.c
u8*  Skalib_ScratchToAram(int n);       // skalib.c
void Skalib_ScratchFromAram(int n);     // skalib.c

void* lbl_802813D0 = lbl_801E7100;
u8    lbl_802813D4 = 1;
s32   lbl_802813D8 = -1;

// Before a mount, read, write, delete or format: note the operation's size (lbl_80281FB4) and the
// CARD library's transfer count so far (nXferStart).
void fn_8009CB9C(s32 nPort, s32 nSlot, s32 nSize) {
    lbl_801F1510[nPort][nSlot].nXferStart = CARDGetXferredBytes(nPort);
    lbl_80281FB4 = nSize;
}

// Park the save file images: take the ARAM once, allocate both images, then let fn_8009EF98 copy
// the first to ARAM and free both.
void fn_8009CC00(void) {
    lbl_80281FC0 = MC_BUFFER_SIZE + 0x20;
    if (lbl_80281FC4 == 0) {
        lbl_80281FC4 = GoARAM_Alloc(MC_BUFFER_SIZE + 0x20);
    }
    lbl_80281FE8 = fn_80009B34(MC_BUFFER_SIZE, 1, 0x20, "MC_Gc.c", 129);
    lbl_80281FE4 = lbl_80281FE8;
    lbl_80281FEC = lbl_80281FE8;
    lbl_80281FDC = fn_80009B34(MC_BUFFER_SIZE, 1, 0x20, "MC_Gc.c", 133);
    lbl_80281FD8 = lbl_80281FDC;
    lbl_80281FE0 = lbl_80281FDC;
    fn_8009EF98();
}

// Free the icon and banner objects, MC.c's 'eagm' list (fn_800A1BE0), the first image and the
// ARAM. The second image is not freed.
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
        GoARAM_Free(lbl_80281FC4);
        lbl_80281FC4 = 0;
    }
}

// Reset: lbl_80282008 0 and lbl_80282000 1 for both ports, no I/O error noted (lbl_80281FD0); then
// mount slot 0 of each port once and unmount it again, which notes what is in it.
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

// Rename file pOldName on the card to pNewName.
s32 fn_8009CDA0(s32 nPort, s32 nSlot, const char* pOldName, const char* pNewName) {
    int nChan = nPort;
    s32 nResult;
    CARDRenameAsync(nChan, pOldName, pNewName, NULL);
    while ((nResult = CARDGetResultCode(nChan)) == CARD_RESULT_BUSY) {
        fn_8009EB40(nPort, nSlot);
        fn_8006C63C();
        fn_800A4BDC();
    }
    switch (nResult) {
    case CARD_RESULT_FATAL_ERROR:
        return MC_ERR_FATAL;
    case CARD_RESULT_NOCARD:
        lbl_801F1510[nPort][nSlot].uFlags &= ~MC_CARD_PRESENT;
        return MC_ERR_NOCARD;
    case CARD_RESULT_NOFILE:
        return MC_ERR_NOFILE;
    case CARD_RESULT_NOPERM:
        return MC_ERR_NOPERM;
    case CARD_RESULT_EXIST:
        return MC_ERR_EXIST;
    case CARD_RESULT_NAMETOOLONG:
        return MC_ERR_NAMETOOLONG;
    case CARD_RESULT_IOERROR:
        lbl_801F1510[nPort][nSlot].uFlags |= MC_CARD_IOERROR;
        return MC_ERR_IOERROR;
    default:
        return MC_ERR_UNKNOWN;
    case CARD_RESULT_READY:
        return 0;
    }
}

// Open file pName on the card into pFile.
s32 fn_8009CEF8(int nPort, s32 nSlot, const char* pName, CARDFileInfo* pFile) {
    s32 nResult;
    int nChan = nPort;
    do {
        fn_800A4BDC();
        nResult = CARDOpen(nChan, pName, pFile);
        fn_8006C63C();
    } while (nResult == CARD_RESULT_BUSY);
    switch (nResult) {
    case CARD_RESULT_FATAL_ERROR:
        return MC_ERR_FATAL;
    case CARD_RESULT_NOCARD:
        lbl_801F1510[nChan][nSlot].uFlags &= ~MC_CARD_PRESENT;
        return MC_ERR_NOCARD;
    case CARD_RESULT_NOFILE:
        return MC_ERR_NOFILE;
    case CARD_RESULT_NOPERM:
        return MC_ERR_NOPERM;
    case CARD_RESULT_BROKEN:
        lbl_801F1510[nChan][nSlot].uFlags |= MC_CARD_BROKEN;
        return MC_ERR_BROKEN;
    default:
        return MC_ERR_UNKNOWN;
    case CARD_RESULT_READY:
        return 0;
    }
}

// Close pFile.
s32 fn_8009D010(s32 nPort, s32 nSlot, CARDFileInfo* pFile) {
    s32 nResult;
    do {
        fn_800A4BDC();
        nResult = CARDClose(pFile);
        fn_8006C63C();
    } while (nResult == CARD_RESULT_BUSY);
    switch (nResult) {
    case CARD_RESULT_FATAL_ERROR:
        return MC_ERR_FATAL;
    case CARD_RESULT_NOCARD:
        lbl_801F1510[nPort][nSlot].uFlags &= ~MC_CARD_PRESENT;
        return MC_ERR_NOCARD;
    default:
        return MC_ERR_UNKNOWN;
    case CARD_RESULT_READY:
        return 0;
    }
}

// See whether there is a card in the port, and note its size and sector size.
s32 fn_8009D0D4(s32 nPort, s32 nSlot) {
    s32 nMemSize;
    s32 nSectorSize;
    s32 nResult;
    lbl_801F1510[nPort][nSlot].nSectorSize = 0;
    lbl_801F1510[nPort][nSlot].nMemSize = 0;
    lbl_801F1510[nPort][nSlot].uFlags |= MC_CARD_PRESENT;
    do {
        fn_800A4BDC();
        nResult = CARDProbeEx(nPort, &nMemSize, &nSectorSize);
        fn_8006C63C();
    } while (nResult == CARD_RESULT_BUSY);
    switch (nResult) {
    case CARD_RESULT_FATAL_ERROR:
        return MC_ERR_FATAL;
    case CARD_RESULT_NOCARD:
        lbl_801F1510[nPort][nSlot].uFlags &= ~MC_CARD_PRESENT;
        return MC_ERR_NOCARD;
    case CARD_RESULT_WRONGDEVICE:
        lbl_801F1510[nPort][nSlot].uFlags |= MC_CARD_WRONGDEVICE;
        return MC_ERR_WRONGDEVICE;
    default:
        return MC_ERR_UNKNOWN;
    case CARD_RESULT_READY:
        lbl_801F1510[nPort][nSlot].nSectorSize = nSectorSize;
        lbl_801F1510[nPort][nSlot].nMemSize = nMemSize;
        return 0;
    }
}

// The space a save of kind nKind needs: 40 when its file is not on the card yet (the EA Sports
// Bio's own size for kind 3), else 0; 40 also when there is no card. Mounts the card for the look
// if needed. The EA bug of fn_8009D50C is here too.
s32 fn_8009D1D8(s32 nPort, s32 nSlot, s32 arg2, s32 nKind) {
    u32 uFlags = lbl_801F1510[nPort][nSlot].uFlags;
    u32 bMounted = (uFlags >> 2) & 1;   // MC_CARD_MOUNTED
    s32 nResult;
    if (nPort == -1 && nSlot == -1) return 40;
    if (!(uFlags & MC_CARD_PRESENT)) return 40;
    if (fn_8009D0D4(nPort, nSlot) != 0) return 0;
    if (!bMounted && fn_8009D74C(nPort, nSlot) != 0) return 0;
    switch (nKind) {
    case 0:
    case 1:
    case 2:
        nResult = fn_8009D614(nPort, nSlot, MC_FILE_NAME);
        break;
    case 3:
        nResult = fn_8009D614(nPort, nSlot, "EASB");
        break;
    default:
        nResult = MC_ERR_NOFILE;
        break;
    }
    if (nResult != 0) {
        switch (nKind) {
        case 0:
        case 1:
        case 2:
            return 40;
        case 3:
            return fn_801255F8();
        default:
            return 0;
        }
    }
    switch (nKind) {
    case 0:
    case 1:
    case 2:
        if (fn_8009D614(nPort, nSlot, MC_BACKUP_NAME) != 0) return 0;
        break;
    case 3:
        return 0;
    }
    if (!bMounted && fn_8009DBAC(nPort, nSlot) != 0) return 0;
    return 0;
}

// fn_8009D1D8's count for the game's save, plus the EA Sports Bio's (fn_801255C4).
s32 fn_8009D390(s32 nPort, s32 nSlot) {
    s32 aPos[2];
    s32 nCount = 0;
    aPos[0] = nPort;
    aPos[1] = nSlot;
    nCount += fn_8009D1D8(nPort, nSlot, 0, 0);
    nCount += fn_801255C4(aPos);
    return nCount;
}

// How many new files a save of the game needs: 1 when the save file or its backup is not on the
// card yet, else 0 (also 0 when the card cannot be read). Mounts the card for the look if needed.
// The EA bug of fn_8009D50C is here too.
s32 fn_8009D3DC(s32 nPort, s32 nSlot) {
    u32 uFlags = lbl_801F1510[nPort][nSlot].uFlags;
    u32 bMounted = (uFlags >> 2) & 1;   // MC_CARD_MOUNTED
    if (nPort == -1 && nSlot == -1) return 1;
    if (!(uFlags & MC_CARD_PRESENT)) return 1;
    if (fn_8009D0D4(nPort, nSlot) != 0) return 0;
    if (!bMounted && fn_8009D74C(nPort, nSlot) != 0) return 0;
    if (fn_8009D614(nPort, nSlot, MC_FILE_NAME) != 0) return 1;
    if (fn_8009D614(nPort, nSlot, MC_BACKUP_NAME) != 0) return 1;
    if (!bMounted && fn_8009DBAC(nPort, nSlot) != 0) return 0;
    return 0;
}

// How many new files a save of the EA Sports Bio needs on the card: 1 when there is no "EASB" file
// yet, else 0 (also 0 when the card cannot be read). Mounts the card for the look if needed.
s32 fn_8009D50C(s32 nPort, s32 nSlot) {
    // EA bug: the flags are read before the test for port and slot -1, which reads 0x130 bytes
    // before the table.
    u32 uFlags = lbl_801F1510[nPort][nSlot].uFlags;
    u32 bMounted = (uFlags >> 2) & 1;   // MC_CARD_MOUNTED
    if (nPort == -1 && nSlot == -1) return 1;
    if (!(uFlags & MC_CARD_PRESENT)) return 1;
    if (fn_8009D0D4(nPort, nSlot) != 0) return 0;
    if (!bMounted && fn_8009D74C(nPort, nSlot) != 0) return 0;
    if (fn_8009D614(nPort, nSlot, "EASB") != 0) return 1;
    if (!bMounted && fn_8009DBAC(nPort, nSlot) != 0) return 0;
    return 0;
}

// Whether file pName is on the card (0), tried by opening and closing it. The name "EASB" means
// the EA Sports Bio file, asked of its own code (fn_80125194), whose -44, -43 and -18 count as there.
s32 fn_8009D614(s32 nPort, s32 nSlot, const char* pName) {
    CARDFileInfo file;
    s32 nResult;
    nResult = fn_8009F734(nPort, nSlot);
    if (nResult != 0) return nResult;
    if (strcmp("EASB", pName) == 0) {
        nResult = fn_80125194(0, 0);
        if (nResult == -44 || nResult == -43 || nResult == -18) {
            nResult = 0;
        }
        return nResult;
    }
    nResult = fn_8009CEF8(nPort, nSlot, pName, &file);
    if (nResult == 0) {
        nResult = fn_8009D010(nPort, nSlot, &file);
    }
    return nResult;
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
    if (nResult == CARD_RESULT_NOCARD) {
        lbl_801F1510[nPort][0].uFlags &= ~MC_CARD_MOUNTED;
    }
}

// Mount the card (unmounting it first if it was mounted: the answer is then MC_ERR_MOUNTED), check
// it and note what it holds: its encoding, free space and free directory entries. The flags start
// again from present and formatted; b94 is cleared, but put back when the card is broken. A card
// with an I/O error is not touched. Brings the save file images back (fn_8009F02C).
s32 fn_8009D74C(s32 nPort, s32 nSlot) {
    int nChan;
    s32 nFreeBytes;
    s32 nFreeFiles;
    u16 uEncoding;
    u32 bWasMounted = 0;
    u8 b94;
    s32 nResult;
    if (lbl_80281FD0[nPort] != 0) return MC_ERR_IOERROR;
    nChan = nPort;
    b94 = lbl_801F1510[nPort][nSlot].b94;
    lbl_801F1510[nPort][nSlot].b94 = 0;
    nResult = fn_8009D0D4(nChan, nSlot);
    if (nResult != 0) return nResult;
    if (lbl_801F1510[nPort][nSlot].uFlags & MC_CARD_MOUNTED) {
        nResult = fn_8009DBAC(nPort, nSlot);
        if (nResult != 0) return nResult;
        bWasMounted = 1;
    }
    fn_8009CB9C(nPort, nSlot, 0xA000);
    lbl_801F1510[nPort][nSlot].nFreeBlocks = 0;
    lbl_801F1510[nPort][nSlot].uFlags = 0;
    lbl_801F1510[nPort][nSlot].uFlags |= MC_CARD_PRESENT | MC_CARD_FORMATTED;
    CARDMountAsync(nChan, lbl_802813D0, fn_8009D728, NULL);
    while ((nResult = CARDGetResultCode(nChan)) == CARD_RESULT_BUSY) {
        fn_800A4BDC();
        fn_8009EB40(nPort, nSlot);
        fn_8006C63C();
    }
    switch (nResult) {
    case CARD_RESULT_FATAL_ERROR:
        return MC_ERR_FATAL;
    case CARD_RESULT_WRONGDEVICE:
        lbl_801F1510[nPort][nSlot].uFlags |= MC_CARD_WRONGDEVICE;
        return MC_ERR_WRONGDEVICE;
    case CARD_RESULT_NOCARD:
        lbl_801F1510[nPort][nSlot].uFlags &= ~MC_CARD_PRESENT;
        return MC_ERR_NOCARD;
    case CARD_RESULT_IOERROR:
        lbl_801F1510[nPort][nSlot].uFlags |= MC_CARD_IOERROR;
        return MC_ERR_IOERROR;
    case CARD_RESULT_ENCODING:
        lbl_801F1510[nPort][nSlot].uFlags |= MC_CARD_MOUNTED;
        lbl_801F1510[nPort][nSlot].uFlags |= MC_CARD_ENCODING;
        lbl_801F1510[nPort][nSlot].uFlags &= ~MC_CARD_FORMATTED;
        return MC_ERR_ENCODING;
    default:
        return MC_ERR_UNKNOWN;
    case CARD_RESULT_READY:
    case CARD_RESULT_BROKEN:        // the check below finds out
        break;
    }
    CARDCheckAsync(nChan, NULL);
    while ((nResult = CARDGetResultCode(nChan)) == CARD_RESULT_BUSY) {
        fn_800A4BDC();
        fn_8009EB40(nPort, nSlot);
        fn_8006C63C();
    }
    switch (nResult) {
    case CARD_RESULT_FATAL_ERROR:
        return MC_ERR_FATAL;
    case CARD_RESULT_NOCARD:
        lbl_801F1510[nPort][nSlot].uFlags &= ~MC_CARD_PRESENT;
        return MC_ERR_NOCARD;
    case CARD_RESULT_IOERROR:
        lbl_801F1510[nPort][nSlot].uFlags |= MC_CARD_IOERROR;
        return MC_ERR_IOERROR;
    case CARD_RESULT_ENCODING:
        lbl_801F1510[nPort][nSlot].uFlags |= MC_CARD_MOUNTED;
        lbl_801F1510[nPort][nSlot].uFlags |= MC_CARD_ENCODING;
        return MC_ERR_ENCODING;
    case CARD_RESULT_BROKEN:
        lbl_801F1510[nPort][nSlot].uFlags |= MC_CARD_MOUNTED;
        lbl_801F1510[nPort][nSlot].uFlags &= ~MC_CARD_FORMATTED;
        lbl_801F1510[nPort][nSlot].uFlags |= MC_CARD_BROKEN;
        lbl_801F1510[nPort][nSlot].b94 = b94;
        return MC_ERR_BROKEN;
    default:
        return MC_ERR_UNKNOWN;
    case CARD_RESULT_READY:
        break;
    }
    switch (CARDGetEncoding(nChan, &uEncoding)) {
    case CARD_RESULT_FATAL_ERROR:
        return MC_ERR_FATAL;
    case CARD_RESULT_NOCARD:
        lbl_801F1510[nPort][nSlot].uFlags &= ~MC_CARD_PRESENT;
        return MC_ERR_NOCARD;
    case CARD_RESULT_READY:
        if (uEncoding != 0) {
            lbl_801F1510[nPort][nSlot].uFlags |= MC_CARD_ENCODING;
        }
        break;
    case CARD_RESULT_WRONGDEVICE:
    default:
        return MC_ERR_UNKNOWN;
    }
    switch (CARDFreeBlocks(nChan, &nFreeBytes, &nFreeFiles)) {
    case CARD_RESULT_FATAL_ERROR:
        return MC_ERR_FATAL;
    case CARD_RESULT_NOCARD:
        lbl_801F1510[nPort][nSlot].uFlags &= ~MC_CARD_PRESENT;
        return MC_ERR_NOCARD;
    case CARD_RESULT_BROKEN:
        lbl_801F1510[nPort][nSlot].uFlags |= MC_CARD_BROKEN;
        lbl_801F1510[nPort][nSlot].b94 = b94;
        return MC_ERR_BROKEN;
    case CARD_RESULT_READY:
        // the free bytes in whole sectors, rounded up
        lbl_801F1510[nPort][nSlot].nFreeBlocks = (lbl_801F1510[nPort][nSlot].nSectorSize - 1 + nFreeBytes)
                                               / lbl_801F1510[nPort][nSlot].nSectorSize;
        lbl_801F1510[nPort][nSlot].nFreeFiles = nFreeFiles;
        break;
    default:
        return MC_ERR_UNKNOWN;
    }
    fn_8009EB40(nPort, nSlot);
    lbl_801F1510[nPort][nSlot].uFlags |= MC_CARD_MOUNTED;
    fn_8009F02C();
    return bWasMounted ? MC_ERR_MOUNTED : 0;
}

// Unmount the card if it is mounted, then park the save file images in ARAM (fn_8009EF98).
s32 fn_8009DBAC(s32 nPort, s32 nSlot) {
    s32 nResult;
    int nChan;
    if (lbl_801F1510[nPort][nSlot].uFlags & MC_CARD_MOUNTED) {
        lbl_801F1510[nPort][nSlot].uFlags &= ~MC_CARD_MOUNTED;
        nChan = nPort;
        do {
            fn_800A4BDC();
            nResult = CARDUnmount(nChan);
            fn_8006C63C();
        } while (nResult == CARD_RESULT_BUSY);
        switch (nResult) {
        case CARD_RESULT_FATAL_ERROR:
            return MC_ERR_FATAL;
        case CARD_RESULT_NOCARD:
            lbl_801F1510[nPort][nSlot].uFlags &= ~MC_CARD_PRESENT;
            return MC_ERR_NOCARD;
        case CARD_RESULT_READY:
            break;
        default:
            return MC_ERR_UNKNOWN;
        }
    }
    fn_8009EF98();
    return 0;
}

// Note a mount's result: an I/O error marks the card damaged (lbl_80281FD0).
void fn_8009DC80(s32 nPort, s32 nSlot, s32 nResult) {
    fn_8009DCE8();
    switch (nResult) {
    case MC_ERR_LIMIT:
    case MC_ERR_FATAL:
    case MC_ERR_EXIST:
    case MC_ERR_WRONGDEVICE:
    case MC_ERR_ENCODING:
    case MC_ERR_UNKNOWN:
    case MC_ERR_CANCELED:
    case MC_ERR_NOENT:
    case MC_ERR_BROKEN:
    case MC_ERR_NOPERM:
    case MC_ERR_MOUNTED:
    case MC_ERR_BADDATA:
    case MC_ERR_NOFILE:
    case MC_ERR_NAMETOOLONG:
    case MC_ERR_INSSPACE:
    case MC_ERR_NOCARD:
        break;
    case MC_ERR_IOERROR:
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

// Read the whole of file pName (nLen bytes) into pBuf, then check that it is one of the game's
// saves: its banner and icon at 0x40 and its comments at the start, else MC_ERR_BADDATA. A card
// with an I/O error is not touched.
s32 fn_8009DD94(s32 nPort, s32 nSlot, const char* pName, void* pBuf, s32 nLen) {
    CARDFileInfo file;
    CARDStat stat;
    int nChan;
    s32 nResult;
    if (lbl_80281FD0[nPort] != 0) return MC_ERR_IOERROR;
    nResult = fn_8009F734(nPort, nSlot);
    if (nResult != 0) return nResult;
    nResult = fn_8009D0D4(nPort, nSlot);
    if (nResult != 0) return nResult;
    nResult = fn_8009CEF8(nPort, nSlot, pName, &file);
    if (nResult != 0) return nResult;
    fn_8009CB9C(nPort, nSlot, nLen);
    CARDReadAsync(&file, pBuf, nLen, 0, NULL);
    nChan = nPort;
    while ((nResult = CARDGetResultCode(nChan)) == CARD_RESULT_BUSY) {
        fn_800A4BDC();
        fn_8009EB40(nPort, nSlot);
        fn_8006C63C();
    }
    switch (nResult) {
    case CARD_RESULT_FATAL_ERROR:
        fn_8009D010(nPort, nSlot, &file);
        return MC_ERR_FATAL;
    case CARD_RESULT_NOCARD:
        fn_8009D010(nPort, nSlot, &file);
        lbl_801F1510[nPort][nSlot].uFlags &= ~MC_CARD_PRESENT;
        return MC_ERR_NOCARD;
    case CARD_RESULT_NOFILE:
        fn_8009D010(nPort, nSlot, &file);
        return MC_ERR_NOFILE;
    case CARD_RESULT_LIMIT:
        fn_8009D010(nPort, nSlot, &file);
        return MC_ERR_LIMIT;
    case CARD_RESULT_CANCELED:
        fn_8009D010(nPort, nSlot, &file);
        return MC_ERR_CANCELED;
    default:
        return MC_ERR_UNKNOWN;
    case CARD_RESULT_READY:
        nResult = fn_8009E280(nPort, nSlot, file.fileNo, &stat);
        if (nResult != 0) return nResult;
        if (stat.iconAddr != 0x40 || stat.commentAddr != 0) {
            fn_8009D010(nPort, nSlot, &file);
            return MC_ERR_BADDATA;
        }
        nResult = fn_8009D010(nPort, nSlot, &file);
        if (nResult != 0) return nResult;
        return 0;
    }
}

// Create file pName of uSize bytes on the card, open in pFile.
s32 fn_8009DFD8(s32 nPort, s32 nSlot, const char* pName, u32 uSize, CARDFileInfo* pFile) {
    int nChan = nPort;
    s32 nResult;
    CARDCreateAsync(nChan, pName, uSize, pFile, NULL);
    while ((nResult = CARDGetResultCode(nChan)) == CARD_RESULT_BUSY) {
        fn_800A4BDC();
        fn_8009EB40(nPort, nSlot);
        fn_8006C63C();
    }
    switch (nResult) {
    case CARD_RESULT_FATAL_ERROR:
        return MC_ERR_FATAL;
    case CARD_RESULT_NOCARD:
        lbl_801F1510[nPort][nSlot].uFlags &= ~MC_CARD_PRESENT;
        return MC_ERR_NOCARD;
    case CARD_RESULT_EXIST:
        return MC_ERR_EXIST;
    case CARD_RESULT_NOENT:
        return MC_ERR_NOENT;
    case CARD_RESULT_INSSPACE:
        return MC_ERR_INSSPACE;
    case CARD_RESULT_NAMETOOLONG:
        return MC_ERR_NAMETOOLONG;
    case CARD_RESULT_IOERROR:
        lbl_801F1510[nPort][nSlot].uFlags |= MC_CARD_IOERROR;
        return MC_ERR_IOERROR;
    default:
        return MC_ERR_UNKNOWN;
    case CARD_RESULT_READY:
        return 0;
    }
}

// Write nLen bytes from pBuf to open file pFile at nOffset.
s32 fn_8009E130(s32 nPort, s32 nSlot, CARDFileInfo* pFile, const void* pBuf, s32 nLen, s32 nOffset) {
    int nChan = nPort;
    s32 nResult;
    CARDWriteAsync(pFile, pBuf, nLen, nOffset, NULL);
    while ((nResult = CARDGetResultCode(nChan)) == CARD_RESULT_BUSY) {
        fn_800A4BDC();
        fn_8009EB40(nPort, nSlot);
        fn_8006C63C();
    }
    switch (nResult) {
    case CARD_RESULT_FATAL_ERROR:
        return MC_ERR_FATAL;
    case CARD_RESULT_NOCARD:
        lbl_801F1510[nPort][nSlot].uFlags &= ~MC_CARD_PRESENT;
        return MC_ERR_NOCARD;
    case CARD_RESULT_NOFILE:
        return MC_ERR_NOFILE;
    case CARD_RESULT_NOPERM:
        return MC_ERR_NOPERM;
    case CARD_RESULT_IOERROR:
        lbl_801F1510[nPort][nSlot].uFlags |= MC_CARD_IOERROR;
        return MC_ERR_IOERROR;
    case CARD_RESULT_LIMIT:
        return MC_ERR_LIMIT;
    case CARD_RESULT_CANCELED:
        return MC_ERR_CANCELED;
    default:
        return MC_ERR_UNKNOWN;
    case CARD_RESULT_READY:
        return 0;
    }
}

// Read the directory entry of file nFile into pStat.
s32 fn_8009E280(s32 nPort, s32 nSlot, s32 nFile, CARDStat* pStat) {
    s32 nResult;
    int nChan = nPort;
    int nFileNo = nFile;
    do {
        fn_800A4BDC();
        nResult = CARDGetStatus(nChan, nFileNo, pStat);
        fn_8006C63C();
    } while (nResult == CARD_RESULT_BUSY);
    switch (nResult) {
    case CARD_RESULT_FATAL_ERROR:
        return MC_ERR_FATAL;
    case CARD_RESULT_NOCARD:
        lbl_801F1510[nChan][nSlot].uFlags &= ~MC_CARD_PRESENT;
        return MC_ERR_NOCARD;
    case CARD_RESULT_NOFILE:
        return MC_ERR_NOFILE;
    case CARD_RESULT_NOPERM:
        return MC_ERR_NOPERM;
    default:
        return MC_ERR_UNKNOWN;
    case CARD_RESULT_READY:
        return 0;
    }
}

// Write pStat back as the directory entry of file nFile.
s32 fn_8009E360(s32 nPort, s32 nSlot, s32 nFile, CARDStat* pStat) {
    int nChan = nPort;
    s32 nResult;
    CARDSetStatus(nChan, nFile, pStat);
    while ((nResult = CARDGetResultCode(nChan)) == CARD_RESULT_BUSY) {
        fn_800A4BDC();
        fn_8006C63C();
    }
    switch (nResult) {
    case CARD_RESULT_FATAL_ERROR:
        return MC_ERR_FATAL;
    case CARD_RESULT_NOCARD:
        lbl_801F1510[nPort][nSlot].uFlags &= ~MC_CARD_PRESENT;
        return MC_ERR_NOCARD;
    case CARD_RESULT_NOFILE:
        return MC_ERR_NOFILE;
    case CARD_RESULT_NOPERM:
        return MC_ERR_NOPERM;
    case CARD_RESULT_IOERROR:
        lbl_801F1510[nPort][nSlot].uFlags |= MC_CARD_IOERROR;
        return MC_ERR_IOERROR;
    default:
        return MC_ERR_UNKNOWN;
    case CARD_RESULT_READY:
        return 0;
    }
}

// Describe the open file's banner and icon in its directory entry: the banner and icon images
// start at 0x40 (SaveImage.aBanner), both in format 2, the icon shown at speed 3 and looping, and
// the comment strings at the start.
s32 fn_8009E47C(s32 nPort, s32 nSlot, CARDFileInfo* pFile) {
    CARDStat stat;
    s32 nFile = pFile->fileNo;
    s32 nResult = fn_8009E280(nPort, nSlot, nFile, &stat);
    if (nResult != 0) return nResult;
    stat.commentAddr = 0;
    stat.iconAddr = 0x40;
    stat.iconFormat = (stat.iconFormat & ~3) | 2;
    stat.bannerFormat = (stat.bannerFormat & ~3) | 2;
    stat.iconSpeed = (stat.iconSpeed & ~3) | 3;
    stat.bannerFormat &= ~4;
    nResult = fn_8009E360(nPort, nSlot, nFile, &stat);
    if (nResult != 0) return nResult;
    return 0;
}

// Fill in the save file's comment strings (the game's name and "Users and Options") and copy the
// icon ('MCI ') and banner ('MCB ') images when asked for.
void fn_8009E544(char* pGameName, char* pComment, u8* pIcon, u8* pBanner) {
    memset(pGameName, 0, 32);
    memset(pComment, 0, 32);
    strcpy(pGameName, "Tiger Woods PGA Tour 2004");
    strcpy(pComment, "Users and Options");
    if (pIcon != NULL) {
        memcpy(pIcon, lbl_80281FB8->pData, lbl_80281FB8->uSize);
    }
    if (pBanner != NULL) {
        memcpy(pBanner, lbl_80281FBC->pData, lbl_80281FBC->uSize);
    }
}

// Write nLen bytes from pBuf as file pName, creating it when it is not there, set its banner and
// icon (fn_8009E47C), then load it back (fn_8009ED34) to check it.
s32 fn_8009E604(s32 nPort, s32 nSlot, const char* pName, void* pBuf, s32 nLen,
                const char* pBackupName) {
    CARDFileInfo file;
    s32 nResult;
    nResult = fn_8009F734(nPort, nSlot);
    if (nResult != 0) return nResult;
    nResult = fn_8009D0D4(nPort, nSlot);
    if (nResult != 0) return nResult;
    fn_8009CB9C(nPort, nSlot, nLen + 0x6000);
    nResult = fn_8009CEF8(nPort, nSlot, pName, &file);
    if (nResult == MC_ERR_NOFILE) {
        nResult = fn_8009DFD8(nPort, nSlot, pName, nLen, &file);
        if (nResult != 0) return nResult;
    } else if (nResult != 0) {
        return nResult;
    }
    nResult = fn_8009E130(nPort, nSlot, &file, pBuf, nLen, 0);
    if (nResult != 0) {
        fn_8009D010(nPort, nSlot, &file);
        return nResult;
    }
    nResult = fn_8009E47C(nPort, nSlot, &file);
    if (nResult != 0) return nResult;
    nResult = fn_8009D010(nPort, nSlot, &file);
    if (nResult != 0) return nResult;
    nResult = fn_8009ED34(nPort, nSlot, pName, NULL);
    if (nResult != 0) return nResult;
    return 0;
}

// Delete file pName from the card ("EASB": the EA Sports Bio's file, by its number). Nothing to do
// without a name; a card with an I/O error is not touched.
s32 fn_8009E758(s32 nPort, s32 nSlot, const char* pName) {
    int nChan;
    s32 nResult;
    if (pName == NULL) return 0;
    if (lbl_80281FD0[nPort] != 0) return MC_ERR_IOERROR;
    nResult = fn_8009F734(nPort, nSlot);
    if (nResult != 0) return nResult;
    nChan = nPort;
    nResult = fn_8009D0D4(nPort, nSlot);
    if (nResult != 0) return nResult;
    fn_8009CB9C(nPort, nSlot, 0x54000);
    if (strcmp("EASB", pName) == 0) {
        CARDFastDeleteAsync(nChan, lbl_80281FCC, NULL);
    } else {
        CARDDeleteAsync(nChan, pName, NULL);
    }
    while ((nResult = CARDGetResultCode(nChan)) == CARD_RESULT_BUSY) {
        fn_800A4BDC();
        fn_8009EB40(nPort, nSlot);
        fn_8006C63C();
    }
    switch (nResult) {
    case CARD_RESULT_FATAL_ERROR:
        return MC_ERR_FATAL;
    case CARD_RESULT_NOCARD:
        lbl_801F1510[nPort][nSlot].uFlags &= ~MC_CARD_PRESENT;
        return MC_ERR_NOCARD;
    case CARD_RESULT_NOFILE:
        return MC_ERR_NOFILE;
    case CARD_RESULT_NOPERM:
        return MC_ERR_NOPERM;
    case CARD_RESULT_IOERROR:
        lbl_801F1510[nPort][nSlot].uFlags |= MC_CARD_IOERROR;
        return MC_ERR_IOERROR;
    default:
        return MC_ERR_UNKNOWN;
    case CARD_RESULT_READY:
        return 0;
    }
}

// Format the card (it must be there and mounted). An I/O error also sets b94.
s32 fn_8009E918(s32 nPort, s32 nSlot) {
    s32 nResult;
    int nChan;
    if (!(lbl_801F1510[nPort][nSlot].uFlags & MC_CARD_PRESENT)) return -4;
    if (!(lbl_801F1510[nPort][nSlot].uFlags & MC_CARD_MOUNTED)) return MC_ERR_NOTMOUNTED;
    nChan = nPort;
    nResult = fn_8009D0D4(nChan, nSlot);
    if (nResult != 0) return nResult;
    lbl_801F1510[nPort][nSlot].uFlags &= ~MC_CARD_FORMATTED;
    fn_8009CB9C(nPort, nSlot, lbl_801F1510[nPort][nSlot].nMemSize + 0xA000);
    CARDFormatAsync(nChan, NULL);
    while ((nResult = CARDGetResultCode(nChan)) == CARD_RESULT_BUSY) {
        fn_800A4BDC();
        fn_8009EB40(nPort, nSlot);
        fn_8006C63C();
    }
    switch (nResult) {
    case CARD_RESULT_FATAL_ERROR:
        return MC_ERR_FATAL;
    case CARD_RESULT_NOCARD:
        lbl_801F1510[nPort][nSlot].uFlags &= ~MC_CARD_PRESENT;
        return MC_ERR_NOCARD;
    case CARD_RESULT_IOERROR:
        lbl_801F1510[nPort][nSlot].uFlags |= MC_CARD_IOERROR;
        lbl_801F1510[nPort][nSlot].b94 = 1;
        return MC_ERR_IOERROR;
    case CARD_RESULT_READY:
        lbl_801F1510[nPort][nSlot].uFlags |= MC_CARD_FORMATTED;
        break;
    default:
        return MC_ERR_UNKNOWN;
    }
    return 0;
}

// Take the stream objects: the save file's icon ('MCI ') and banner ('MCB '), and MC.c's 'eagm'.
void fn_8009EA98(void) {
    Stream_RegisterLoadChunkCallback('MCI ', fn_8009EB30);
    Stream_RegisterLoadChunkCallback('MCB ', fn_8009EB38);
    Stream_RegisterLoadChunkCallback('eagm', fn_800A1D4C);
}

void fn_8009EAF0(void) {
    Stream_UnregisterLoadChunkCallback('MCI ');
    Stream_UnregisterLoadChunkCallback('MCB ');
    Stream_UnregisterLoadChunkCallback('eagm');
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

// Look at every file on the card and mark the 'eagm' entries (MC.c) whose names match the file's
// game code; return how many are marked. Mounts the card for the look if needed.
s32 fn_8009EB44(s32 nPort, s32 nSlot) {
    CARDStat stat;
    char szGameCode[5];     // size unknown (at most 8)
    s32 nMount;
    int i;
    s32 nResult;
    nMount = fn_8009D74C(nPort, nSlot);
    if (nMount != 0 && nMount != MC_ERR_MOUNTED) return nMount;
    nResult = fn_8009F734(nPort, nSlot);
    if (nResult == 0) {
        nResult = fn_8009D0D4(nPort, nSlot);
        if (nResult == 0) {
            for (i = 0; i < 127; i++) {
                nResult = fn_8009E280(nPort, nSlot, i, &stat);
                if (nResult == 0) {
                    strncpy(szGameCode, stat.gameName, 4);
                    szGameCode[4] = '\0';
                    fn_800A1F6C(szGameCode);
                }
            }
        }
    }
    if (nMount == 0) {
        fn_8009DBAC(nPort, nSlot);
    }
    if (nResult == 0 || nResult == MC_ERR_NOFILE) return fn_800A2030();
    return nResult;
}

// Make the backup the save file: when both are on the card, delete pName first; then load the
// backup, and if it is good rename it to pName. A bad backup is deleted (-38). No backup: -37.
s32 fn_8009EC30(s32 nPort, s32 nSlot, const char* pName, const char* pBackupName) {
    s32 nResult = fn_8009D614(nPort, nSlot, pBackupName);
    if (nResult == 0) {
        if (fn_8009D614(nPort, nSlot, pName) == 0) {
            nResult = fn_8009E758(nPort, nSlot, pName);
            if (nResult != 0) return nResult;
        }
        nResult = fn_8009EF68(nPort, nSlot);
        if (nResult == 0) return fn_8009CDA0(nPort, nSlot, pBackupName, pName);
        if (nResult == MC_ERR_BADDATA) {
            nResult = fn_8009E758(nPort, nSlot, pBackupName);
            return (nResult != 0) ? nResult : -38;
        }
        return nResult;
    }
    return (nResult != MC_ERR_NOFILE) ? nResult : -37;
}

// Load and check the save file, then its backup, each into the first image (a backup on the card is
// read last, so the image holds it after). A good save file makes the backup redundant (deleted), a
// good backup alone becomes the save file (fn_8009EC30). Both bad, or one bad and the other
// missing: MC_ERR_BADDATA. pName and pBackupName are not used; the names are always MC_FILE_NAME
// and MC_BACKUP_NAME.
s32 fn_8009ED34(s32 nPort, s32 nSlot, const char* pName, const char* pBackupName) {
    s32 nMain = fn_8009EECC(nPort, nSlot, MC_FILE_NAME);
    s32 nBackup = fn_8009EECC(nPort, nSlot, MC_BACKUP_NAME);
    if ((nMain == MC_ERR_BADDATA && nBackup == MC_ERR_BADDATA) ||
        (nMain == MC_ERR_NOFILE && nBackup == MC_ERR_BADDATA) ||
        (nMain == MC_ERR_BADDATA && nBackup == MC_ERR_NOFILE)) {
        return MC_ERR_BADDATA;
    }
    if (nMain == 0) {
        if (nBackup != MC_ERR_NOFILE) {
            fn_8009E758(nPort, nSlot, MC_BACKUP_NAME);
        }
        return 0;
    }
    if (nBackup == 0) {
        nMain = fn_8009EC30(nPort, nSlot, MC_FILE_NAME, MC_BACKUP_NAME);
    }
    return nMain;
}

// Mount the card, load and check the save file and its backup (fn_8009ED34), and unmount it again
// if this mounted it. When the card's state is bad (fn_8009F734) the card is left mounted.
s32 fn_8009EE28(s32 nPort, s32 nSlot) {
    s32 nMount;
    s32 nResult;
    nMount = fn_8009D74C(nPort, nSlot);
    if (nMount != 0 && nMount != MC_ERR_MOUNTED) return nMount;
    nResult = fn_8009F734(nPort, nSlot);
    if (nResult != 0) return nResult;
    nResult = fn_8009ED34(nPort, nSlot, MC_FILE_NAME, MC_BACKUP_NAME);
    if (nMount == 0) {
        fn_8009DBAC(nPort, nSlot);
    }
    return nResult;
}

// Load the save file pName into the first image and check it (fn_800A233C): MC_ERR_BADDATA when it
// is not a good save.
s32 fn_8009EECC(s32 nPort, s32 nSlot, const char* pName) {
    s32 nResult = fn_8009DD44(nPort, nSlot, MC_DIR_NAME);
    if (nResult == 0) {
        nResult = fn_8009DD94(nPort, nSlot, pName, lbl_80281FE8, MC_BUFFER_SIZE);
        if (nResult == 0) {
            if (fn_800A233C(lbl_80281FE8, &lbl_80281FE8->trailer)) return 0;
            return MC_ERR_BADDATA;
        }
    }
    return nResult;
}

// Read the backup copy of the save file into the first image.
s32 fn_8009EF68(s32 nPort, s32 nSlot) {
    return fn_8009EECC(nPort, nSlot, MC_BACKUP_NAME);
}

u32 fn_8009EF90(void) {
    return lbl_80281FC0;
}

// Park the save file images: copy the first to ARAM, then give the memory back. In game type 6
// the images are skalib's two scratch areas, which get their own contents back from ARAM.
void fn_8009EF98(void) {
    if (lbl_80281FE8 != NULL) {
        lbl_80281FC0 = MC_BUFFER_SIZE + 0x20;
        GoARAM_WaitTransfer(GoARAM_CopyToAram(lbl_80281FE8, lbl_80281FC4, lbl_80281FC0));
        if (gSession.nGameType == 6) {
            Skalib_ScratchFromAram(0);
            Skalib_ScratchFromAram(1);
        } else {
            fn_80009E70(lbl_80281FE8);
            fn_80009E70(lbl_80281FDC);
        }
        lbl_80281FE8 = NULL;
        lbl_80281FEC = NULL;
        lbl_80281FE4 = NULL;
        lbl_80281FDC = NULL;
        lbl_80281FE0 = NULL;
        lbl_80281FD8 = NULL;
    }
}

// Bring the save file images back: take the memory (skalib's scratch areas in game type 6, whose
// contents go to ARAM meanwhile) and copy the first image back from ARAM.
void fn_8009F02C(void) {
    if (lbl_80281FE8 == NULL) {
        if (gSession.nGameType == 6) {
            lbl_80281FE8 = lbl_80281FEC = lbl_80281FE4 = (SaveImage*)Skalib_ScratchToAram(0);
            lbl_80281FDC = lbl_80281FE0 = lbl_80281FD8 = (SaveImage*)Skalib_ScratchToAram(1);
        } else {
            lbl_80281FE8 = lbl_80281FEC = lbl_80281FE4 =
                fn_80009B34(lbl_80281FC0, 1, 0x20, "MC_Gc.c", 2805);
            lbl_80281FDC = lbl_80281FE0 = lbl_80281FD8 =
                fn_80009B34(lbl_80281FC0, 1, 0x20, "MC_Gc.c", 2810);
        }
        GoARAM_WaitTransfer(GoARAM_CopyFromAram(lbl_80281FE8, lbl_80281FC4, lbl_80281FC0));
    }
}

// List the files on the card whose names hold pPattern, sorted by fn_8009F4D8: copy up to nMax
// names into apName and the number found into pnFound. The last one found is noted as the file to
// open (lbl_802813D8, lbl_80281FCC). nSlot is not used.
s32 fn_8009F0F0(s32 nPort, s32 nSlot, const char* pPattern, char** apName, s32 nMax, s32* pnFound) {
    CARDStat stat;
    int nChan = nPort;
    int i;
    lbl_80281FB0 = 0;
    for (i = 0; i < 127; i++) {
        if (CARDGetStatus(nChan, i, &stat) == CARD_RESULT_READY &&
            strstr(stat.fileName, pPattern) != NULL) {
            memcpy(&lbl_801E3B6C[lbl_80281FB0], &stat, sizeof(CARDStat));
            lbl_802813D8 = i;
            lbl_80281FB0++;
            lbl_80281FCC = i;
        }
    }
    qsort(lbl_801E3B6C, lbl_80281FB0, sizeof(CARDStat), fn_8009F4D8);
    for (i = 0; i < lbl_80281FB0 && i < nMax; i++) {
        strcpy(apName[i], lbl_801E3B6C[i].fileName);
    }
    *pnFound = lbl_80281FB0;
    lbl_80281FB0 = 0;
    return 0;
}

// Read nLen bytes of open file nFile into pBuf at the file position (lbl_80281FC8) and move the
// position on; CARDRead's result is ignored. arg3 is not used (TibExt's SFIO_vReadCallback passes 0).
s32 fn_8009F208(s32 nFile, void* pBuf, s32 nLen, s32 arg3) {
    CARDRead(&lbl_801E3180[nFile], pBuf, nLen, lbl_80281FC8);
    lbl_80281FC8 += nLen;
    return 0;
}

// Write nLen bytes from pBuf to open file nFile on port 0 at the file position (lbl_80281FC8) and
// move the position on. An I/O error marks the card in port 0 damaged; any other result clears that
// and gives 0.
s32 fn_8009F258(s32 nFile, void* pBuf, s32 nLen) {
    s32 nResult = fn_8009E130(0, 0, &lbl_801E3180[nFile], pBuf, nLen, lbl_80281FC8);
    lbl_80281FC8 += nLen;
    if (nResult == MC_ERR_IOERROR) {
        lbl_80281FD0[0] = 1;
        return MC_ERR_IOERROR;
    }
    lbl_80281FD0[0] = 0;
    return 0;
}

// Move the file position (lbl_80281FC8) to nOffset, from the start or from where it is. An nOffset
// above 0x76000, or below 0 from the start, gives MC_ERR_BADDATA and puts the position back at the
// start. nFile is not used.
s32 fn_8009F2D8(s32 nFile, s32 nOffset, u8 bFromStart) {
    if (bFromStart) {
        if (nOffset < 0) {
            lbl_80281FC8 = 0;
            return MC_ERR_BADDATA;
        }
        if (nOffset > 0x76000) {
            lbl_80281FC8 = 0;
            return MC_ERR_BADDATA;
        }
    } else if (nOffset > 0x76000) {
        lbl_80281FC8 = 0;
        return MC_ERR_BADDATA;
    }
    lbl_80281FC8 = bFromStart ? nOffset : lbl_80281FC8 + nOffset;
    lbl_802813D4 = bFromStart;
    return 0;
}

s32 fn_8009F35C(void) {
    return 0;
}

s32 fn_8009F364(void) {
    return 0;
}

// The card's free space in bytes.
s32 fn_8009F36C(s32 nPort, s32 nSlot, s32* pnFreeBytes) {
    s32 nFreeFiles = 0;
    CARDFreeBlocks(nPort, pnFreeBytes, &nFreeFiles);
    return 0;
}

s32 fn_8009F3A0(s32 nPort, s32 nSlot, const char* pName, s32* pnFreeFiles) {
    s32 nFreeBytes = 0;
    CARDFreeBlocks(nPort, &nFreeBytes, pnFreeFiles);
    return 0;
}

// Open the file fn_8009F0F0 or fn_8009F514 noted last (lbl_802813D8) and start at its beginning.
// pName "EASB" also hands it to the EA Sports Bio's code (fn_8012CCCC). uFlags is not used.
s32 fn_8009F3D4(s32 nPort, s32 nSlot, const char* pName, u32 uFlags, s32* pnFile) {
    CARDFileInfo file;
    s32 nResult;
    lbl_80281FC8 = 0;
    if (lbl_802813D8 == -1) return MC_ERR_NOFILE;
    nResult = CARDFastOpen(nPort, lbl_802813D8, &file);
    if (nResult != 0) return nResult;
    memcpy(&lbl_801E3180[file.fileNo], &file, sizeof(CARDFileInfo));
    *pnFile = file.fileNo;
    if (strcmp("EASB", pName) == 0) {
        fn_8012CCCC(file.fileNo);
        lbl_80281FCC = file.fileNo;
    }
    return 0;
}

// Close open file nFile.
s32 fn_8009F488(s32 nFile) {
    if (lbl_802813D8 == -1) return MC_ERR_NOFILE;
    CARDClose(&lbl_801E3180[nFile]);
    lbl_802813D8 = -1;
    return 0;
}

// fn_8009F0F0's qsort order, meant as newest first by the entries' time stamps (see the EA bug).
s32 fn_8009F4D8(const void* pA, const void* pB) {
    // EA bug: the time stamps are read through the addresses of the two parameters, not through
    // the entries they point to, so this compares whatever is on the stack there and the order is
    // arbitrary. A port should read ((const CARDStat*)pA)->time.
    u32 uTimeA = ((const CARDStat*)&pA)->time;
    u32 uTimeB = ((const CARDStat*)&pB)->time;
    if (uTimeA > uTimeB) return -1;
    return uTimeA < uTimeB;
}

// Create pName with nLen bytes (only the EA Sports Bio's "EASB"), set its attribute bit 0x40 and
// note it as the file to open (lbl_802813D8). An I/O error marks the card damaged.
s32 fn_8009F514(s32 nPort, s32 nSlot, const char* pName, s32 nLen) {
    CARDFileInfo file;
    u8 uAttr;
    s32 nResult;
    if (strcmp("EASB", pName) == 0) {
        nResult = fn_8009DFD8(nPort, nSlot, pName, nLen, &file);
        CARDGetAttributes(nPort, file.fileNo, &uAttr);
        uAttr |= 0x40;
        CARDSetAttributes(nPort, file.fileNo, uAttr);
        lbl_802813D8 = file.fileNo;
        if (nResult == MC_ERR_IOERROR) {
            lbl_80281FD0[nPort] = 1;
        }
        return nResult;
    }
    return 0;
}

// Delete the save file, when pName is its name or the save directory's. No file is not an error.
s32 fn_8009F5E4(s32 nPort, s32 nSlot, const char* pName) {
    s32 nResult = fn_8009F734(nPort, nSlot);
    if (nResult != 0) return nResult;
    if (strcmp(MC_FILE_NAME, pName) == 0 || strcmp(MC_DIR_NAME, pName) == 0) {
        nResult = fn_8009E758(nPort, nSlot, MC_FILE_NAME);
    }
    if (nResult != 0 && nResult != MC_ERR_NOFILE) return nResult;
    return 0;
}

s32 fn_8009F6A0(s32 nPort, s32 nSlot) {
    CARDStat stat;
    int nChan = nPort;
    int i;
    for (i = 0; i < 127; i++) {
        if (CARDGetStatus(nChan, i, &stat) == CARD_RESULT_READY &&
            strstr(stat.fileName, "BASLUS-20572") != NULL) {
            return 0;
        }
    }
    return MC_ERR_NOFILE;
}

// Whether the card in this port had an I/O error.
s32 fn_8009F728(int nPort) {
    return lbl_80281FD0[nPort];
}

s32 fn_8009F734(s32 nPort, s32 nSlot) {
    u32 uFlags = lbl_801F1510[nPort][nSlot].uFlags;
    if (!(uFlags & MC_CARD_PRESENT)) return -4;
    if (!(uFlags & MC_CARD_FORMATTED)) return -1;
    if (!(uFlags & MC_CARD_MOUNTED)) return MC_ERR_NOTMOUNTED;
    return 0;
}

// Vertex format 2: position (x, y, z floats), colour (RGBA8) and a texture coordinate (s, t floats).
// port: GameCube only.
void fn_8009F780(void) {
    GXSetVtxAttrFmt(2, 9, 1, 4, 0);
    GXSetVtxAttrFmt(2, 11, 1, 5, 0);
    GXSetVtxAttrFmt(2, 13, 1, 4, 0);
}

void fn_8009F7E4(void) {
}

u8 fn_8009F7E8(int nPort) {
    return lbl_80282008[nPort];
}

void fn_8009F7F4(MCCardState* pState, int nPort, int nSlot) {
    *pState = lbl_801F1510[nPort][nSlot];
}

MCCardState* fn_8009F834(s32 nPort, s32 nSlot) {
    return &lbl_801F1510[nPort][nSlot];
}
