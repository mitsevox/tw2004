// MC.c (EA's name, from its asserts; also in EA's 2002 source tree): the game side of the memory
// card: finding the save file on either card, loading and saving it, and the results the menus turn
// into messages. The GameCube calls are in MC_Gc.c (include/core/memcard.h).
//
// Each operation here mounts the card if it is not mounted yet (fn_8009D74C) and unmounts it
// again after only in that case.

#include "core/memcard.h"
#include "frontend/fe.h"
#include "game/earnings.h"
#include "game/frontend.h"

s32 fn_800A13E8(s32 nPort, s32 nSlot, s32 n);
s32 fn_800A2248(s32 nPort, s32 nSlot);

// At boot: no created golfer yet; try fn_800A13E8 on each card, and at the first that succeeds mark
// player slot 0's profile loaded and return 1.
u8 fn_8009F850(void) {
    int i;
    gCurGolferRecord.bAvailable = 0;
    for (i = 0; i < MC_NUM_PORTS; i++) {
        if (fn_800A13E8(i, 0, 0) == 0) {
            lbl_801D7148.aLoaded[0] = 1;
            return 1;
        }
    }
    return 0;
}

// Merge a save's record tables into the game's: every entry the game's tables do not already hold
// is offered to them (fn_800D8750) as nobody's. The course and the mode are switched to reach each
// table, then put back.
void fn_8009F8C8(SaveRecords* pRecords) {
    int nMode;
    int nCourse;
    int nHoleNum;
    int i;
    int k;
    int j;

    nMode = Game_GetMode();
    nHoleNum = gpGame->nCurHoleNum;
    nCourse = Game_GetCourse();
    for (i = 0; i < NUM_COURSE_RECORDS; i++) {
        fn_800E14E0(i);
        for (j = 0; j < 8; j++) {
            for (k = 0; k < 5; k++) {
                if (!fn_800D8458(j, pRecords->aCourseRecord[(u32)i].aRecord[(u32)j][k].nValue,
                                 pRecords->aCourseRecord[(u32)i].aRecord[(u32)j][k].szName, i)) {
                    fn_800D8750(j, pRecords->aCourseRecord[(u32)i].aRecord[(u32)j][k].nValue, 1,
                                pRecords->aCourseRecord[(u32)i].aRecord[(u32)j][k].szName, 5);
                }
            }
        }
    }
    gpGame->nMode = 16;
    for (i = 0; i < 3; i++) {
        gpGame->nCurHoleNum = i;
        for (j = 0; j < 3; j++) {
            for (k = 0; k < 5; k++) {
                if (!fn_800D853C(j, pRecords->recB[(u32)i][(u32)j][k].nValue,
                                 pRecords->recB[(u32)i][(u32)j][k].szName, i)) {
                    fn_800D8750(j, pRecords->recB[(u32)i][(u32)j][k].nValue, 1,
                                pRecords->recB[(u32)i][(u32)j][k].szName, 5);
                }
            }
        }
    }
    gpGame->nMode = 22;
    for (i = 0; i < 5; i++) {
        gpGame->nCurHoleNum = i;
        for (j = 0; j < 2; j++) {
            for (k = 0; k < 5; k++) {
                if (!fn_800D85DC(j, pRecords->recC[(u32)i][(u32)j][k].nValue,
                                 pRecords->recC[(u32)i][(u32)j][k].szName, i)) {
                    fn_800D8750(j, pRecords->recC[(u32)i][(u32)j][k].nValue, 1,
                                pRecords->recC[(u32)i][(u32)j][k].szName, 5);
                }
            }
        }
    }
    gpGame->nMode = nMode;
    gpGame->nCurHoleNum = nHoleNum;
    fn_800E14E0(nCourse);
}

// At boot: find the save file on a card and read it into the second image; if it is good, take its
// options and records into the game. Message 0x85 tells the front end which card is being read,
// 0x86 that none was found.
s32 fn_8009FAD0(void) {
    s32 nMount;
    u8 bFound;
    int nPort;
    int nSlot;
    s32 nResult;
    MsgArg args[2];
    MsgArg arg;

    bFound = 0;
    for (nPort = 0; nPort < MC_NUM_PORTS && !bFound; nPort++) {
        for (nSlot = 0; nSlot < MC_NUM_SLOTS && !bFound; nSlot++) {
            if (fn_8009EE28(nPort, nSlot) != 0) continue;
            nMount = fn_8009D74C(nPort, nSlot);
            if (nMount != 0 && nMount != MC_ERR_MOUNTED) continue;
            if (fn_8009F734(nPort, nSlot) == 0) {
                nResult = fn_8009DD44(nPort, nSlot, MC_DIR_NAME);
                if (nResult == 0) {
                    if (lbl_80281F1C != NULL) {
                        fn_80005AE8(args, 0, sizeof(args));
                        args[0].i = nPort;
                        args[1].i = nSlot;
                        fn_8016B0F8(lbl_80281F1C->pHandler, 0x85, 2, args);
                    }
                    nResult = fn_8009DD94(nPort, nSlot, MC_FILE_NAME, lbl_80281FDC, MC_BUFFER_SIZE);
                    if (nResult == 0) {
                        if (fn_800A233C(lbl_80281FDC, &lbl_80281FDC->trailer)) {
                            bFound = 1;
                        } else {
                            lbl_80281FDC->uFlags = 0;
                        }
                    } else if (nResult == MC_ERR_BADDATA) {
                        lbl_80281FDC->uFlags = 0;
                    }
                }
            }
            if (nMount == 0) {
                fn_8009DBAC(nPort, nSlot);
            }
        }
    }
    if (bFound) {
        fn_8009F02C();
        Mem_cpy(lbl_80281FDC, lbl_80281FE8, MC_BUFFER_SIZE);
        if (lbl_80281FDC->uFlags & MC_SAVE_OPTIONS) {
            Mem_cpy(&gSession.options, &lbl_80281FDC->options, sizeof(GameOptions));
            fn_8002EBA4((u8*)&gSession.options, gSession.options.a7[0]);
        }
        if (lbl_80281FDC->uFlags & MC_SAVE_RECORDS) {
            fn_8009F8C8(&lbl_80281FDC->records);
        }
        fn_8009EF98();
    } else if (lbl_80281F1C != NULL) {
        fn_80005AE8(&arg, 0, sizeof(arg));
        fn_8016B0F8(lbl_80281F1C->pHandler, 0x86, 1, &arg);
    }
    // EA bug: nResult is never set when no card gets as far as the file check
    return nResult;
}

// Load the save file from the card at pPos into the first image and, if it is good, make it the
// game's copy (the second image) and take its options and records into the game.
s32 fn_8009FCFC(MCCardPos* pPos) {
    u8 bLoaded;
    s32 nPort;
    s32 nMount;
    s32 nResult;
    s32 nSlot;

    bLoaded = 0;
    nSlot = pPos->nSlot;
    nPort = pPos->nPort;
    nMount = fn_8009D74C(nPort, nSlot);
    if (nMount != 0 && nMount != MC_ERR_MOUNTED) return nMount;
    nResult = fn_8009F734(nPort, nSlot);
    if (nResult != 0) return nResult;
    nResult = fn_8009DD44(nPort, nSlot, MC_DIR_NAME);
    if (nResult != 0) {
        nResult = fn_8009DD94(nPort, nSlot, MC_FILE_NAME, lbl_80281FE8, MC_BUFFER_SIZE);
        if (nResult == 0) {
            if (fn_800A233C(lbl_80281FE8, &lbl_80281FE8->trailer)) {
                bLoaded = 1;
                Mem_cpy(lbl_80281FDC, lbl_80281FE8, MC_BUFFER_SIZE);
            } else {
                lbl_80281FDC->uFlags = 0;
            }
        } else if (nResult == MC_ERR_BADDATA) {
            lbl_80281FDC->uFlags = 0;
        }
    }
    if (nMount == 0) {
        fn_8009DBAC(nPort, nSlot);
    }
    if (bLoaded) {
        if (lbl_80281FDC->uFlags & MC_SAVE_4D0C0) {
            lbl_80281FF8 = lbl_80281FDC->n4D0C0;
        } else {
            lbl_80281FF8 = 0;
        }
        if (lbl_80281FDC->uFlags & MC_SAVE_OPTIONS) {
            Mem_cpy(&gSession.options, &lbl_80281FDC->options, sizeof(GameOptions));
            fn_8002EBA4((u8*)&gSession.options, gSession.options.a7[0]);
        }
        if (lbl_80281FDC->uFlags & MC_SAVE_RECORDS) {
            fn_8009F8C8(&lbl_80281FDC->records);
        }
    }
    return nResult;
}

// Delete the save file from the card.
s32 fn_800A0A7C(s32 nPort, s32 nSlot) {
    s32 nMount;
    s32 nResult;
    nMount = fn_8009D74C(nPort, nSlot);
    if (nMount != 0 && nMount != MC_ERR_MOUNTED) return nMount;
    nResult = fn_8009F734(nPort, nSlot);
    if (nResult != 0) return nResult;
    nResult = fn_8009F5E4(nPort, nSlot, MC_DIR_NAME);
    if (nMount == 0) {
        fn_8009DBAC(nPort, nSlot);
    }
    return nResult;
}

// Whether the card at pPos holds the save file (one of the file functions in lbl_8018C7D8).
int fn_800A1758(MCCardPos* pPos) {
    return fn_800A2248(pPos->nPort, pPos->nSlot) == 0;
}

void fn_800A19F4(void) {
}

// Format the card. A card that is broken or has the wrong encoding can still be formatted.
s32 fn_800A2100(s32 nPort, s32 nSlot) {
    s32 nMount;
    s32 nResult;
    nMount = fn_8009D74C(nPort, nSlot);
    if (nMount != 0 && nMount != MC_ERR_MOUNTED && nMount != MC_ERR_ENCODING &&
        nMount != MC_ERR_BROKEN) {
        return nMount;
    }
    nResult = fn_8009E918(nPort, nSlot);
    if (nMount == 0) {
        fn_8009DBAC(nPort, nSlot);
    }
    return nResult;
}

s32 fn_800A218C(void) {
    return MC_ERR_NOFILE;
}

// Look on the card for a file whose name holds "BASLUS-20572" (fn_8009F6A0): 0 when there is one.
s32 fn_800A2194(s32 nPort, s32 nSlot) {
    s32 nMount;
    s32 nResult;
    nMount = fn_8009D74C(nPort, nSlot);
    if (nMount != 0 && nMount != MC_ERR_MOUNTED) return nMount;
    nResult = fn_8009F734(nPort, nSlot);
    if (nResult != 0) {
        if (nMount == 0) {
            fn_8009DBAC(nPort, nSlot);
        }
        return nResult;
    }
    nResult = fn_8009F6A0(nPort, nSlot);
    if (nMount == 0) {
        fn_8009DBAC(nPort, nSlot);
    }
    return nResult;
}

// Whether the card holds the save file (0), or why not.
s32 fn_800A2248(s32 nPort, s32 nSlot) {
    s32 nMount;
    s32 nResult;
    nMount = fn_8009D74C(nPort, nSlot);
    if (nMount != 0 && nMount != MC_ERR_MOUNTED) return nMount;
    nResult = fn_8009F734(nPort, nSlot);
    if (nResult != 0) {
        if (nMount == 0) {
            fn_8009DBAC(nPort, nSlot);
        }
        return nResult;
    }
    nResult = fn_8009DD44(nPort, nSlot, MC_DIR_NAME);
    if (nResult != 0) {
        if (nMount == 0) {
            fn_8009DBAC(nPort, nSlot);
        }
        return nResult;
    }
    nResult = fn_8009D614(nPort, nSlot, MC_FILE_NAME);
    if (nMount == 0) {
        fn_8009DBAC(nPort, nSlot);
    }
    return nResult;
}
