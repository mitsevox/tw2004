// MC_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree): the GameCube memory card
// layer under MC.c: mounting, checking and formatting the cards and reading and writing the save
// file through the CARD library, turning its results into the game's error codes, and keeping
// what it learns about each card in lbl_801F1510 (include/core/memcard.h).
//
// port: a PC build replaces this file with file I/O.

#include "core/memcard.h"
#include "frontend/fe.h"
#include "core/goaram.h"

void fn_8006C63C(void);         // } called around every CARD library call (not yet known)
void fn_800A4BDC(void);         // }
void fn_8009DC80(s32 nPort, s32 nSlot, s32 nResult);
void fn_8009DCE8(void);
void fn_8009EB30(UStreamObject* pObject);
void fn_8009EB38(UStreamObject* pObject);
s32  fn_8009ED34(s32 nPort, s32 nSlot, const char* pName, const char* pBackupName);
s32  fn_8009EECC(s32 nPort, s32 nSlot, const char* pName);
s32  fn_80125194(s32 a, s32 b);         // EA Sports Bio (0x80125194)
void GXSetVtxAttrFmt(int nFmt, int nAttr, int nCnt, int nType, u8 uFrac);   // port: GameCube only
u8*  Skalib_ScratchToAram(int n);       // skalib.c
void Skalib_ScratchFromAram(int n);     // skalib.c

// Every operation starts here: note its size and how far the CARD library's transfer count has
// got, from which its progress is measured.
void fn_8009CB9C(s32 nPort, s32 nSlot, s32 nSize) {
    lbl_801F1510[nPort][nSlot].nXferStart = CARDGetXferredBytes(nPort);
    lbl_80281FB4 = nSize;
}

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

// Open file pName on the card into pFile.
s32 fn_8009CEF8(s32 nPort, s32 nSlot, const char* pName, CARDFileInfo* pFile) {
    s32 nResult;
    do {
        fn_800A4BDC();
        nResult = CARDOpen(nPort, pName, pFile);
        fn_8006C63C();
    } while (nResult == CARD_RESULT_BUSY);
    switch (nResult) {
    case CARD_RESULT_FATAL_ERROR:
        return MC_ERR_FATAL;
    case CARD_RESULT_NOCARD:
        lbl_801F1510[nPort][nSlot].uFlags &= ~MC_CARD_PRESENT;
        return MC_ERR_NOCARD;
    case CARD_RESULT_NOPERM:
        return MC_ERR_NOPERM;
    case CARD_RESULT_BROKEN:
        lbl_801F1510[nPort][nSlot].uFlags |= MC_CARD_BROKEN;
        return MC_ERR_BROKEN;
    default:
        return MC_ERR_UNKNOWN;
    case CARD_RESULT_READY:
        return 0;
    case CARD_RESULT_NOFILE:
        return MC_ERR_NOFILE;
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

// Unmount the card if it is mounted, then park the save file images in ARAM (fn_8009EF98).
s32 fn_8009DBAC(s32 nPort, s32 nSlot) {
    s32 nResult;
    if (lbl_801F1510[nPort][nSlot].uFlags & MC_CARD_MOUNTED) {
        lbl_801F1510[nPort][nSlot].uFlags &= ~MC_CARD_MOUNTED;
        do {
            fn_800A4BDC();
            nResult = CARDUnmount(nPort);
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

// Read the directory entry of file nFile into pStat.
s32 fn_8009E280(s32 nPort, s32 nSlot, s32 nFile, CARDStat* pStat) {
    s32 nResult;
    do {
        fn_800A4BDC();
        nResult = CARDGetStatus(nPort, nFile, pStat);
        fn_8006C63C();
    } while (nResult == CARD_RESULT_BUSY);
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
    default:
        return MC_ERR_UNKNOWN;
    case CARD_RESULT_READY:
        return 0;
    }
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

// Load the save file (or its backup) into the first image. When the card's state is bad the card
// is left mounted.
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
        fn_800B67EC(fn_800B6844(lbl_80281FE8, lbl_80281FC4, lbl_80281FC0));
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
        fn_800B67EC(fn_800B68B4(lbl_80281FE8, lbl_80281FC4, lbl_80281FC0));
    }
}

// Read nLen bytes of open file nFile into pBuf, from where the last read stopped. arg3 is not
// used (TibExt's fn_80122744 passes 0).
s32 fn_8009F208(s32 nFile, void* pBuf, s32 nLen, s32 arg3) {
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

// Close open file nFile.
s32 fn_8009F488(s32 nFile) {
    if (lbl_802813D8 == -1) return MC_ERR_NOFILE;
    CARDClose(&lbl_801E3180[nFile]);
    lbl_802813D8 = -1;
    return 0;
}

s32 fn_8009F6A0(s32 nPort, s32 nSlot) {
    CARDStat stat;
    int i;
    for (i = 0; i < 127; i++) {
        if (CARDGetStatus(nPort, i, &stat) == CARD_RESULT_READY &&
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
    if (!(uFlags & 0x08)) return -1;
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
