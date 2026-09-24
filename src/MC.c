// MC.c (EA's name, from its asserts; also in EA's 2002 source tree): the game side of the memory
// card: finding the save file on either card, loading and saving it, and the results the menus turn
// into messages. The GameCube calls are in MC_Gc.c (include/core/memcard.h).
//
// Each operation here mounts the card if it is not mounted yet (fn_8009D74C) and unmounts it
// again after only in that case.

#include "charstate.h"
#include "core/memcard.h"
#include "frontend/fe.h"
#include "game/earnings.h"
#include "game/frontend.h"
#include "frontend/uistudio.h"

s32 fn_800A13E8(s32 nPort, s32 nSlot, s32 nProfile);
s32 fn_800A2248(s32 nPort, s32 nSlot);
s32 fn_800A0B18(s32 nPort, s32 nSlot, const char* szName, SaveImage* pImage);
s32 fn_800A0BC8(s32 nPort, s32 nSlot, const char* szName, SaveImage* pImage);
void fn_800A253C(void);

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
                        fn_8016B0F8(lbl_80281F1C->pHandler, 0x85, 2, (const s32*)args);
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
        fn_8016B0F8(lbl_80281F1C->pHandler, 0x86, 1, (const s32*)&arg);
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

// Save the options and the records to the card at pPos: a new save file when there is none (if the
// card has room), else into the save already there.
s32 fn_8009FE90(MCCardPos* pPos) {
    s32 nMount;
    s32 nResult;
    s32 nPort;
    s32 nSlot;

    nSlot = pPos->nSlot;
    nPort = pPos->nPort;
    nMount = fn_8009D74C(nPort, nSlot);
    if (nMount != 0 && nMount != MC_ERR_MOUNTED) return nMount;
    // EA bug: this return and the failed writes below leave a card it mounted mounted
    nResult = fn_8009F734(nPort, nSlot);
    if (nResult != 0) return nResult;
    if (fn_8009DD44(nPort, nSlot, MC_DIR_NAME) != 0) {
        if (lbl_801F1510[nPort][nSlot].nFreeBlocks < fn_8009D1D8(nPort, nSlot, 0, 0)) {
            return MC_ERR_INSSPACE;
        }
        nResult = fn_8009F514(nPort, nSlot, MC_DIR_NAME, 0);
        if (nResult != 0) return nResult;
        fn_8009DD44(nPort, nSlot, MC_DIR_NAME);
        fn_8009E544(lbl_80281FDC->szGameName, lbl_80281FDC->szComment, lbl_80281FDC->aIcon,
                    lbl_80281FDC->aBanner);
        lbl_80281FDC->uFlags = 0;
        lbl_80281FDC->uFlags |= MC_SAVE_OPTIONS;
        lbl_80281FDC->uFlags |= MC_SAVE_RECORDS;
        Mem_cpy(&lbl_80281FDC->options, &gSession.options, sizeof(GameOptions));
        Mem_cpy(&lbl_80281FDC->records, gSession.aCourseRecord, sizeof(SaveRecords));
        lbl_80281FDC->trailer.aMagic[0] = '@';
        lbl_80281FDC->trailer.aMagic[1] = 'B';
        lbl_80281FDC->trailer.aMagic[2] = 'E';
        lbl_80281FDC->trailer.uChecksum = fn_800A23BC(lbl_80281FDC, &lbl_80281FDC->trailer);
        nResult = fn_8009E604(nPort, nSlot, MC_FILE_NAME, lbl_80281FDC, MC_BUFFER_SIZE, NULL);
        if (nResult != 0) {
            fn_8009F5E4(nPort, nSlot, MC_DIR_NAME);
            return nResult;
        }
    } else {
        if (fn_8009DD94(nPort, nSlot, MC_FILE_NAME, lbl_80281FE8, MC_BUFFER_SIZE) != 0) {
            lbl_80281FE8->uFlags = 0;
            if (nMount == 0) {
                fn_8009DBAC(nPort, nSlot);
            }
            return MC_ERR_BADDATA;
        }
        if (!fn_800A233C(lbl_80281FE8, &lbl_80281FE8->trailer)) {
            lbl_80281FE8->uFlags = 0;
            if (nMount == 0) {
                fn_8009DBAC(nPort, nSlot);
            }
            return MC_ERR_BADDATA;
        }
        Mem_cpy(lbl_80281FDC, lbl_80281FE8, MC_BUFFER_SIZE);
        lbl_80281FDC->uFlags |= MC_SAVE_OPTIONS;
        lbl_80281FDC->uFlags |= MC_SAVE_RECORDS;
        Mem_cpy(&lbl_80281FDC->options, &gSession.options, sizeof(GameOptions));
        Mem_cpy(&lbl_80281FDC->records, gSession.aCourseRecord, sizeof(SaveRecords));
        lbl_80281FDC->trailer.aMagic[0] = '@';
        lbl_80281FDC->trailer.aMagic[1] = 'B';
        lbl_80281FDC->trailer.aMagic[2] = 'E';
        lbl_80281FDC->trailer.uChecksum = fn_800A23BC(lbl_80281FDC, &lbl_80281FDC->trailer);
        nResult = fn_8009E604(nPort, nSlot, MC_FILE_NAME, lbl_80281FDC, MC_BUFFER_SIZE,
                              MC_BACKUP_NAME);
        if (nResult != 0) return nResult;
    }
    if (nMount == 0) {
        fn_8009DBAC(nPort, nSlot);
    }
    return 0;
}

// Load replay pPos->n8 of the save on the card into gReplayData: -16 when the save has none there.
s32 fn_800A0230(MCCardPos* pPos) {
    s32 nMount;
    s32 nResult;
    s32 nPort;
    s32 nSlot;
    s32 nReplay;

    nSlot = pPos->nSlot;
    nPort = pPos->nPort;
    nReplay = pPos->n8;
    nMount = fn_8009D74C(nPort, nSlot);
    if (nMount != 0 && nMount != MC_ERR_MOUNTED) return nMount;
    // EA bug: this return leaves a card it mounted mounted
    nResult = fn_8009F734(nPort, nSlot);
    if (nResult != 0) return nResult;
    nResult = fn_8009DD44(nPort, nSlot, MC_DIR_NAME);
    if (nResult != 0) {
        if (nMount == 0) {
            fn_8009DBAC(nPort, nSlot);
        }
        return nResult;
    }
    nResult = fn_8009DD94(nPort, nSlot, MC_FILE_NAME, lbl_80281FEC, MC_BUFFER_SIZE);
    if (nResult == 0) {
        if (!fn_800A233C(lbl_80281FEC, &lbl_80281FEC->trailer)) {
            nResult = MC_ERR_BADDATA;
        } else if (lbl_80281FEC->uFlags & MC_SAVE_REPLAY(nReplay)) {
            Mem_cpy(&gReplayData, &lbl_80281FEC->aReplay[nReplay], sizeof(Replay));
            nResult = 0;
        } else {
            nResult = -16;
        }
    }
    if (nMount == 0) {
        fn_8009DBAC(nPort, nSlot);
    }
    return nResult;
}

// Save gReplayData as replay pPos->n8 of the save on the card (-1: the first free one; -17 when none
// is). With no save file on the card, one is made first (fn_8009FE90).
s32 fn_800A036C(MCCardPos* pPos) {
    s32 nMount;
    s32 nResult;
    u8 bNewFile;
    s32 nPort;
    s32 nSlot;
    s32 nReplay;
    int i;
    u8 bFree;

    nSlot = pPos->nSlot;
    nPort = pPos->nPort;
    nReplay = pPos->n8;
    nMount = fn_8009D74C(nPort, nSlot);
    if (nMount != 0 && nMount != MC_ERR_MOUNTED) return nMount;
    // EA bug: this return and the ones below leave a card it mounted mounted
    nResult = fn_8009F734(nPort, nSlot);
    if (nResult != 0) return nResult;
    bNewFile = 0;
    if (fn_8009DD44(nPort, nSlot, MC_DIR_NAME) != 0) {
        bNewFile = 1;
        if (lbl_801F1510[nPort][nSlot].nFreeBlocks < fn_8009D1D8(nPort, nSlot, 0, 0)) {
            return MC_ERR_INSSPACE;
        }
        nResult = fn_8009FE90(pPos);
        if (nResult != 0) return nResult;
    }
    if (fn_8009DD94(nPort, nSlot, MC_FILE_NAME, lbl_80281FEC, MC_BUFFER_SIZE) != 0) {
        lbl_80281FEC->uFlags = 0;
        return MC_ERR_BADDATA;
    }
    if (!fn_800A233C(lbl_80281FEC, &lbl_80281FEC->trailer)) {
        lbl_80281FEC->uFlags = 0;
        return MC_ERR_BADDATA;
    }
    Mem_cpy(lbl_80281FE0, lbl_80281FEC, MC_BUFFER_SIZE);
    if (nReplay == -1) {
        i = 0;
        bFree = 0;
        while (i < NUM_SAVE_REPLAYS && !bFree) {
            if (!(lbl_80281FE0->uFlags & MC_SAVE_REPLAY(i))) {
                bFree = 1;
            } else {
                i++;
            }
        }
        if (i == NUM_SAVE_REPLAYS) return -17;
        nReplay = i;
    }
    Mem_cpy(&lbl_80281FE0->aReplay[nReplay], &gReplayData, sizeof(Replay));
    lbl_80281FE0->uFlags |= MC_SAVE_REPLAY(nReplay);
    lbl_80281FE0->trailer.aMagic[0] = '@';
    lbl_80281FE0->trailer.aMagic[1] = 'B';
    lbl_80281FE0->trailer.aMagic[2] = 'E';
    lbl_80281FE0->trailer.uChecksum = fn_800A23BC(lbl_80281FE0, &lbl_80281FE0->trailer);
    if (bNewFile) {
        nResult = fn_8009E604(nPort, nSlot, MC_FILE_NAME, lbl_80281FE0, MC_BUFFER_SIZE, NULL);
    } else {
        nResult = fn_8009E604(nPort, nSlot, MC_FILE_NAME, lbl_80281FE0, MC_BUFFER_SIZE,
                              MC_BACKUP_NAME);
    }
    if (nMount == 0) {
        fn_8009DBAC(nPort, nSlot);
    }
    return nResult;
}

// Save gReplayData over replay nReplay of the save on the card (-1: into the first free one; -17
// when none is). -16 when the card has no save file.
s32 fn_800A0610(s32 nPort, s32 nSlot, s32 nReplay) {
    s32 nMount;
    s32 nResult;
    int i;
    u8 bFree;

    nMount = fn_8009D74C(nPort, nSlot);
    if (nMount != 0 && nMount != MC_ERR_MOUNTED) return nMount;
    // EA bug: this return and the ones below leave a card it mounted mounted
    nResult = fn_8009F734(nPort, nSlot);
    if (nResult != 0) return nResult;
    if (fn_8009DD44(nPort, nSlot, MC_DIR_NAME) != 0) {
        if (nMount == 0) {
            fn_8009DBAC(nPort, nSlot);
        }
        return -16;
    }
    if (fn_8009DD94(nPort, nSlot, MC_FILE_NAME, lbl_80281FEC, MC_BUFFER_SIZE) != 0) {
        lbl_80281FEC->uFlags = 0;
        return MC_ERR_BADDATA;
    }
    if (!fn_800A233C(lbl_80281FEC, &lbl_80281FEC->trailer)) {
        lbl_80281FEC->uFlags = 0;
        return MC_ERR_BADDATA;
    }
    Mem_cpy(lbl_80281FE0, lbl_80281FEC, MC_BUFFER_SIZE);
    // EA bug: with nReplay -1 this shifts by -1 (undefined in C; the GameCube clears no bit)
    lbl_80281FE0->uFlags &= ~MC_SAVE_REPLAY(nReplay);
    if (nReplay == -1) {
        i = 0;
        bFree = 0;
        while (i < NUM_SAVE_REPLAYS && !bFree) {
            if (!(lbl_80281FEC->uFlags & MC_SAVE_REPLAY(i))) {
                bFree = 1;
            } else {
                i++;
            }
        }
        if (i == NUM_SAVE_REPLAYS) return -17;
        nReplay = i;
    }
    Mem_cpy(&lbl_80281FE0->aReplay[nReplay], &gReplayData, sizeof(Replay));
    lbl_80281FE0->uFlags |= MC_SAVE_REPLAY(nReplay);
    lbl_80281FE0->trailer.aMagic[0] = '@';
    lbl_80281FE0->trailer.aMagic[1] = 'B';
    lbl_80281FE0->trailer.aMagic[2] = 'E';
    lbl_80281FE0->trailer.uChecksum = fn_800A23BC(lbl_80281FE0, &lbl_80281FE0->trailer);
    nResult = fn_8009E604(nPort, nSlot, MC_FILE_NAME, lbl_80281FE0, MC_BUFFER_SIZE, MC_BACKUP_NAME);
    if (nMount == 0) {
        fn_8009DBAC(nPort, nSlot);
    }
    return nResult;
}

// Note in the card's MCCardState which replays the save on it holds.
void fn_800A0868(s32 nPort, s32 nSlot) {
    MCCardState* pState;
    s32 nMount;
    int i;

    pState = fn_8009F834(nPort, nSlot);
    // EA bug: clears 4 bits, but there are NUM_SAVE_REPLAYS (5) replays
    fn_8001E938(pState->aReplayUsed, 4);
    if (nPort >= MC_NUM_PORTS || nSlot >= MC_NUM_SLOTS) return;
    nMount = fn_8009D74C(nPort, nSlot);
    if (nMount != 0 && nMount != MC_ERR_MOUNTED) return;
    if (fn_8009F734(nPort, nSlot) != 0) {
        if (nMount == 0) {
            fn_8009DBAC(nPort, nSlot);
        }
        return;
    }
    if (fn_8009DD44(nPort, nSlot, MC_DIR_NAME) != 0) {
        if (nMount == 0) {
            fn_8009DBAC(nPort, nSlot);
        }
        return;
    }
    if (fn_8009DD94(nPort, nSlot, MC_FILE_NAME, lbl_80281FEC, MC_BUFFER_SIZE) != 0) {
        if (nMount == 0) {
            fn_8009DBAC(nPort, nSlot);
        }
        return;
    }
    if (!fn_800A233C(lbl_80281FEC, &lbl_80281FEC->trailer)) {
        if (nMount == 0) {
            fn_8009DBAC(nPort, nSlot);
        }
        return;
    }
    for (i = 0; i < NUM_SAVE_REPLAYS; i++) {
        if (lbl_80281FEC->uFlags & MC_SAVE_REPLAY(i)) {
            fn_8001EA34(pState->aReplayUsed, i);
        }
    }
    if (nMount == 0) {
        fn_8009DBAC(nPort, nSlot);
    }
}

// How many replays the save on the card at pPos holds.
s32 fn_800A09EC(MCCardPos* pPos) {
    MCCardState* pState;
    s32 nPort;
    s32 nSlot;
    int i;
    s32 nCount = 0;
    nSlot = pPos->nSlot;
    nPort = pPos->nPort;
    pState = fn_8009F834(nPort, nSlot);
    fn_800A0868(nPort, nSlot);
    for (i = 0; i < NUM_SAVE_REPLAYS; i++) {
        if (fn_8001E9CC(pState->aReplayUsed, i)) {
            nCount++;
        }
    }
    return nCount;
}

// Load the profile named pPos->szC from the save on the card into profile pPos->pos.n8, and the
// save's records into the game. A save marked "@BD" gets the profile's CrAP info reset and its
// created golfer's model set to 7 (as fn_800A13E8).
s32 fn_800A0C6C(MCCardPosStr* pPos) {
    s32 nMount;
    s32 nResult;
    s32 nRead;
    s32 nPort;
    s32 nSlot;
    s32 nProfile;
    s32 nFound;
    char* szName;

    nSlot = pPos->pos.nSlot;
    nPort = pPos->pos.nPort;
    nProfile = pPos->pos.n8;
    szName = pPos->szC;
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
    nRead = fn_8009DD94(nPort, nSlot, MC_FILE_NAME, lbl_80281FE4, MC_BUFFER_SIZE);
    // EA bug: this return and the one after fn_800A0B18 leave a card it mounted mounted
    if (nRead == MC_ERR_BADDATA) return MC_ERR_BADDATA;
    if (nRead != 0) {
        if (nMount == 0) {
            fn_8009DBAC(nPort, nSlot);
        }
        return -15;
    }
    if (!fn_800A233C(lbl_80281FE4, &lbl_80281FE4->trailer)) {
        if (nMount == 0) {
            fn_8009DBAC(nPort, nSlot);
        }
        return MC_ERR_BADDATA;
    }
    Mem_cpy(lbl_80281FD8, lbl_80281FE4, MC_BUFFER_SIZE);
    nFound = fn_800A0B18(nPort, nSlot, szName, lbl_80281FD8);
    if (nFound < 0) return nFound;
    if (lbl_80281FD8->uFlags & MC_SAVE_RECORDS) {
        fn_8009F8C8(&lbl_80281FD8->records);
    }
    Mem_cpy(&gpSaveData[nProfile], &lbl_80281FD8->aProfile[nFound], sizeof(SaveProfile));
    if (lbl_80281FE4->trailer.aMagic[2] != 'E') {
        FE_CrAP_InitCrAPInfo(&gpSaveData[nProfile]);
        gpSaveData[nProfile].createdGolfer.nModelID = 7;
    }
    if (nMount == 0) {
        fn_8009DBAC(nPort, nSlot);
    }
    return nRead;
}

// Save profile pPos->n8 (gpSaveData) to the card, over the saved profile of the same name or into a
// free slot, with the options and the records; the save then remembers it as the last one saved
// (n4D0C0). With no save file on the card, one is made first (fn_8009FE90).
s32 fn_800A0E6C(MCCardPos* pPos) {
    s32 nProfile;
    s32 nMount;
    u8 bNewFile;
    s32 nPort;
    s32 nSlot;
    s32 nResult;

    nSlot = pPos->nSlot;
    nPort = pPos->nPort;
    nProfile = pPos->n8;
    nMount = fn_8009D74C(nPort, nSlot);
    if (nMount != 0 && nMount != MC_ERR_MOUNTED) return nMount;
    // EA bug: this return and the ones below leave a card it mounted mounted
    nResult = fn_8009F734(nPort, nSlot);
    if (nResult != 0) return nResult;
    bNewFile = 0;
    if (fn_8009DD44(nPort, nSlot, MC_DIR_NAME) != 0) {
        bNewFile = 1;
        if (lbl_801F1510[nPort][nSlot].nFreeBlocks < fn_8009D1D8(nPort, nSlot, 0, 0)) {
            return MC_ERR_INSSPACE;
        }
        nResult = fn_8009FE90(pPos);
        if (nResult != 0) return nResult;
    }
    if (fn_8009DD94(nPort, nSlot, MC_FILE_NAME, lbl_80281FE8, MC_BUFFER_SIZE) != 0) {
        lbl_80281FE8->uFlags = 0;
        return MC_ERR_BADDATA;
    }
    if (!fn_800A233C(lbl_80281FE8, &lbl_80281FE8->trailer)) {
        lbl_80281FE8->uFlags = 0;
        return MC_ERR_BADDATA;
    }
    Mem_cpy(lbl_80281FDC, lbl_80281FE8, MC_BUFFER_SIZE);
    lbl_80281FDC->uFlags |= MC_SAVE_OPTIONS;
    lbl_80281FDC->uFlags |= MC_SAVE_RECORDS;
    Mem_cpy(&lbl_80281FDC->options, &gSession.options, sizeof(GameOptions));
    Mem_cpy(&lbl_80281FDC->records, gSession.aCourseRecord, sizeof(SaveRecords));
    nResult = fn_800A0BC8(nPort, nSlot, gpSaveData[nProfile].szName, lbl_80281FDC);
    if (nResult < 0) return nResult;
    lbl_80281FDC->uFlags |= MC_SAVE_PROFILE(nResult);
    Mem_cpy(&lbl_80281FDC->aProfile[nResult], &gpSaveData[nProfile], sizeof(SaveProfile));
    lbl_80281FDC->n4D0C0 = nResult;
    lbl_80281FDC->uFlags |= MC_SAVE_4D0C0;
    lbl_80281FDC->trailer.aMagic[0] = '@';
    lbl_80281FDC->trailer.aMagic[1] = 'B';
    lbl_80281FDC->trailer.aMagic[2] = 'E';
    lbl_80281FDC->trailer.uChecksum = fn_800A23BC(lbl_80281FDC, &lbl_80281FDC->trailer);
    if (bNewFile) {
        nResult = fn_8009E604(nPort, nSlot, MC_FILE_NAME, lbl_80281FDC, MC_BUFFER_SIZE, NULL);
    } else {
        nResult = fn_8009E604(nPort, nSlot, MC_FILE_NAME, lbl_80281FDC, MC_BUFFER_SIZE,
                              MC_BACKUP_NAME);
    }
    if (nMount == 0) {
        fn_8009DBAC(nPort, nSlot);
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

// The profile in pImage named szName (any case), or -15. nPort and nSlot are not used.
s32 fn_800A0B18(s32 nPort, s32 nSlot, const char* szName, SaveImage* pImage) {
    int i;
    int bFound;
    if (szName == NULL) return -15;
    i = 0;
    bFound = 0;
    while (i < NUM_SAVE_PROFILES && !bFound) {
        if ((pImage->uFlags & MC_SAVE_PROFILE(i)) && stricmp(pImage->aProfile[i].szName, szName) == 0) {
            bFound = 1;
        }
        if (!bFound) {
            i++;
        }
    }
    if (!bFound) return -15;
    return i;
}

// Where to save the profile named szName in pImage: its own slot, else the first free one, else -36.
s32 fn_800A0BC8(s32 nPort, s32 nSlot, const char* szName, SaveImage* pImage) {
    s32 nProfile;
    int i;
    nProfile = fn_800A0B18(nPort, nSlot, szName, pImage);
    if (nProfile >= 0) return nProfile;
    for (i = 0; i < NUM_SAVE_PROFILES; i++) {
        if (!(pImage->uFlags & MC_SAVE_PROFILE(i))) return i;
    }
    return -36;
}

// Whether the card at pPos holds the save file (one of the file functions in lbl_8018C7D8).
int fn_800A1758(MCCardPos* pPos) {
    return fn_800A2248(pPos->nPort, pPos->nSlot) == 0;
}

// Load the save on the card and take the profile it was last saved from (n4D0C0) into profile
// nProfile, and the save's records into the game. A save marked "@BD" (older than "@BE") gets its
// CrAP info reset and its created golfer's model set to 7.
s32 fn_800A13E8(s32 nPort, s32 nSlot, s32 nProfile) {
    s32 nMount;
    s32 nResult;

    nMount = fn_8009D74C(nPort, nSlot);
    if (nMount != 0 && nMount != MC_ERR_MOUNTED) return nMount;
    // EA bug: this return and the ones below leave a card it mounted mounted
    nResult = fn_8009F734(nPort, nSlot);
    if (nResult != 0) return nResult;
    nResult = fn_8009DD44(nPort, nSlot, MC_DIR_NAME);
    if (nResult == 0) {
        nResult = fn_8009DD94(nPort, nSlot, MC_FILE_NAME, lbl_80281FE8, MC_BUFFER_SIZE);
        if (nResult != 0) {
            return (nResult == MC_ERR_BADDATA) ? MC_ERR_BADDATA : -15;
        }
        if (!fn_800A233C(lbl_80281FE8, &lbl_80281FE8->trailer)) return MC_ERR_BADDATA;
        Mem_cpy(lbl_80281FDC, lbl_80281FE8, MC_BUFFER_SIZE);
        if (!(lbl_80281FDC->uFlags & MC_SAVE_4D0C0)) return -15;
        if (!(lbl_80281FDC->uFlags & MC_SAVE_PROFILE(lbl_80281FDC->n4D0C0))) return -15;
        if (lbl_80281FDC->uFlags & MC_SAVE_RECORDS) {
            fn_8009F8C8(&lbl_80281FDC->records);
        }
        Mem_cpy(&gpSaveData[nProfile], &lbl_80281FDC->aProfile[lbl_80281FDC->n4D0C0],
                sizeof(SaveProfile));
        if (lbl_80281FE4->trailer.aMagic[2] != 'E') {
            FE_CrAP_InitCrAPInfo(&gpSaveData[nProfile]);
            gpSaveData[nProfile].createdGolfer.nModelID = 7;
        }
    }
    if (nMount == 0) {
        fn_8009DBAC(nPort, nSlot);
    }
    return nResult;
}

// The name of profile nProfile in the save on the card, into szName ("" and -15 when there is none).
s32 fn_800A1590(s32 nPort, s32 nSlot, s32 nProfile, char* szName) {
    s32 nMount;
    s32 nResult;

    if (nPort >= MC_NUM_PORTS || nSlot >= MC_NUM_SLOTS) return -15;
    szName[0] = 0;
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
    nResult = fn_8009DD94(nPort, nSlot, MC_FILE_NAME, lbl_80281FE4, MC_BUFFER_SIZE);
    if (nResult != 0) {
        if (nMount == 0) {
            fn_8009DBAC(nPort, nSlot);
        }
        return nResult;
    }
    if (!fn_800A233C(lbl_80281FE4, &lbl_80281FE4->trailer)) {
        if (nMount == 0) {
            fn_8009DBAC(nPort, nSlot);
        }
        return MC_ERR_BADDATA;
    }
    Mem_cpy(lbl_80281FD8, lbl_80281FE4, MC_BUFFER_SIZE);
    if (lbl_80281FD8->uFlags & MC_SAVE_PROFILE(nProfile)) {
        strncpy(szName, lbl_80281FD8->aProfile[nProfile].szName, 0x1D);
        if (nMount == 0) {
            fn_8009DBAC(nPort, nSlot);
        }
        return 0;
    }
    if (nMount == 0) {
        fn_8009DBAC(nPort, nSlot);
    }
    return -15;
}

// Save profile nProfile (gpSaveData) over the saved profile named szName, with the options and the
// records; the save then remembers it as the last one saved (n4D0C0).
s32 fn_800A1164(s32 nPort, s32 nSlot, const char* szName, s32 nProfile) {
    s32 nMount;
    s32 nResult;
    s32 nFound;

    nMount = fn_8009D74C(nPort, nSlot);
    if (nMount != 0 && nMount != MC_ERR_MOUNTED) return nMount;
    // EA bug: this return and the ones below leave a card it mounted mounted
    nResult = fn_8009F734(nPort, nSlot);
    if (nResult != 0) return nResult;
    if (fn_8009DD44(nPort, nSlot, MC_DIR_NAME) != 0) {
        if (nMount == 0) {
            fn_8009DBAC(nPort, nSlot);
        }
        return -15;
    }
    if (fn_8009DD94(nPort, nSlot, MC_FILE_NAME, lbl_80281FE8, MC_BUFFER_SIZE) != 0) {
        lbl_80281FE8->uFlags = 0;
        return MC_ERR_BADDATA;
    }
    if (!fn_800A233C(lbl_80281FE8, &lbl_80281FE8->trailer)) {
        lbl_80281FE8->uFlags = 0;
        return MC_ERR_BADDATA;
    }
    Mem_cpy(lbl_80281FDC, lbl_80281FE8, MC_BUFFER_SIZE);
    nFound = fn_800A0B18(nPort, nSlot, szName, lbl_80281FDC);
    if (nFound < 0) return nFound;
    lbl_80281FD8->uFlags &= ~MC_SAVE_PROFILE(nFound);
    lbl_80281FD8->uFlags |= MC_SAVE_PROFILE(nFound);
    Mem_cpy(&lbl_80281FD8->aProfile[nFound], &gpSaveData[nProfile], sizeof(SaveProfile));
    lbl_80281FDC->n4D0C0 = nFound;
    lbl_80281FDC->uFlags |= MC_SAVE_4D0C0;
    lbl_80281FDC->uFlags |= MC_SAVE_OPTIONS;
    lbl_80281FDC->uFlags |= MC_SAVE_RECORDS;
    Mem_cpy(&lbl_80281FDC->options, &gSession.options, sizeof(GameOptions));
    Mem_cpy(&lbl_80281FDC->records, gSession.aCourseRecord, sizeof(SaveRecords));
    lbl_80281FDC->trailer.aMagic[0] = '@';
    lbl_80281FDC->trailer.aMagic[1] = 'B';
    lbl_80281FDC->trailer.aMagic[2] = 'E';
    lbl_80281FDC->trailer.uChecksum = fn_800A23BC(lbl_80281FDC, &lbl_80281FDC->trailer);
    nResult = fn_8009E604(nPort, nSlot, MC_FILE_NAME, lbl_80281FDC, MC_BUFFER_SIZE, MC_BACKUP_NAME);
    if (nMount == 0) {
        fn_8009DBAC(nPort, nSlot);
    }
    return nResult;
}

// Read the names of the profiles saved on the card into its MCCardState, for the menus.
void fn_800A178C(s32 nPort, s32 nSlot) {
    MCCardState* pState;
    s32 nMount;
    int i;

    pState = fn_8009F834(nPort, nSlot);
    fn_8001E938(pState->aNameUsed, 4);
    pState->aszName[0][0] = 0;
    pState->aszName[1][0] = 0;
    pState->aszName[2][0] = 0;
    pState->aszName[3][0] = 0;
    if (nPort >= MC_NUM_PORTS || nSlot >= MC_NUM_SLOTS) return;
    nMount = fn_8009D74C(nPort, nSlot);
    if (nMount != 0 && nMount != MC_ERR_MOUNTED) return;
    if (fn_8009F734(nPort, nSlot) != 0) {
        if (nMount == 0) {
            fn_8009DBAC(nPort, nSlot);
        }
        return;
    }
    if (fn_8009DD44(nPort, nSlot, MC_DIR_NAME) != 0) {
        if (nMount == 0) {
            fn_8009DBAC(nPort, nSlot);
        }
        return;
    }
    if (fn_8009DD94(nPort, nSlot, MC_FILE_NAME, lbl_80281FE4, MC_BUFFER_SIZE) != 0) {
        if (nMount == 0) {
            fn_8009DBAC(nPort, nSlot);
        }
        return;
    }
    if (!fn_800A233C(lbl_80281FE4, &lbl_80281FE4->trailer)) {
        if (nMount == 0) {
            fn_8009DBAC(nPort, nSlot);
        }
        return;
    }
    Mem_cpy(lbl_80281FD8, lbl_80281FE4, MC_BUFFER_SIZE);
    for (i = 0; i < NUM_SAVE_PROFILES; i++) {
        if (lbl_80281FD8->uFlags & MC_SAVE_PROFILE(i)) {
            fn_8001EA34(pState->aNameUsed, i);
            strncpy(pState->aszName[i], lbl_80281FD8->aProfile[i].szName, 0x1D);
        }
    }
    if (nMount == 0) {
        fn_8009DBAC(nPort, nSlot);
    }
}

// How many profiles the card at pPos holds (fn_800A178C reads their names).
s32 fn_800A1964(MCCardPos* pPos) {
    MCCardState* pState;
    s32 nPort;
    s32 nSlot;
    int i;
    s32 nCount = 0;
    nSlot = pPos->nSlot;
    nPort = pPos->nPort;
    pState = fn_8009F834(nPort, nSlot);
    fn_800A178C(nPort, nSlot);
    for (i = 0; i < 4; i++) {
        if (fn_8001E9CC(pState->aNameUsed, i)) {
            nCount++;
        }
    }
    return nCount;
}

void fn_800A19F4(void) {
}

// ---- reading the 'eagm' text: lines end in '\r\n', the buffer ends in 0xFF (-1 as a char) ----

// The length of the line at p.
s32 fn_800A19F8(const char* p) {
    s32 nLen = 0;
    while (*p != '\r' && *p != -1) {
        nLen++;
        p++;
    }
    return nLen;
}

// Skip white space; NULL at the end of the buffer.
char* fn_800A1A28(char* p) {
    while (*p != -1 && isspace(*p)) {
        p++;
    }
    if (*p == -1) {
        return NULL;
    }
    return p;
}

// The next line's text, past its white space; NULL at the end of the buffer or at the line
// "END_OF_FILE".
char* fn_800A1A90(char* p) {
    char* pLine;
    while (*p != '\n' && *p != -1) {
        p++;
    }
    if (*p == -1) {
        return NULL;
    }
    pLine = fn_800A1A28(p);
    if (strncmp("END_OF_FILE", pLine, strlen("END_OF_FILE")) != 0) {
        return pLine;
    }
    return NULL;
}

// How many lines follow p before the next one that starts with a quote.
s32 fn_800A1B28(char* p) {
    s32 nLines = 0;
    u8 bFound;
    p = fn_800A1A90(p);
    bFound = 0;
    while (!bFound && p != NULL) {
        if (*p != '"') {
            nLines++;
            p = fn_800A1A90(p);
        } else {
            bFound = 1;
        }
    }
    return nLines;
}

// How many lines from p on start with a quote.
s32 fn_800A1B94(char* p) {
    s32 nQuoted = 0;
    while (p != NULL) {
        if (*p == '"') {
            nQuoted++;
        }
        p = fn_800A1A90(p);
    }
    return nQuoted;
}

// Free the 'eagm' list.
void fn_800A1BE0(void) {
    int i;
    if (lbl_80281FF0 != NULL) {
        for (i = 0; i < lbl_80281FF4; i++) {
            fn_80009E70(lbl_80281FF0[i].p4);
        }
        fn_80009E70(lbl_80281FF0);
        lbl_80281FF0 = NULL;
    }
}

// Trim szText in place: the white space at both ends, a final 0xFF, and every quote.
void fn_800A1C58(char* szText) {
    char aBuf[0x40]; // size unknown: the frame leaves room for 0x40 to 0x4C bytes
    s32 nLen;
    char* pStart;
    char* pEnd;
    char* pDst;
    s32 n;

    nLen = strlen(szText);
    pStart = szText;
    while (isspace(*pStart)) {
        pStart++;
    }
    pEnd = &szText[nLen];
    if (*--pEnd == -1) {
        pEnd--;
    }
    while (isspace(*pEnd)) {
        pEnd--;
    }
    pDst = aBuf;
    n = 0;
    for (; *pStart != 0 && pStart != pEnd + 1 && *pStart != -1; pStart++) {
        if (*pStart != '"') {
            *pDst = *pStart;
            n++;
            pDst++;
        }
    }
    aBuf[n] = 0;
    strcpy(szText, aBuf);
}

// The 'eagm' handler: build the 'eagm' list from the object's text, then free the object. The text
// is a list of entries, each a quoted line with the entry's name followed by its names, one a line.
void fn_800A1D4C(UStreamObject* pObject) {
    char* p;
    int i;
    int j;
    s32 nLen;

    if (lbl_80281FF0 != NULL) {
        fn_80009E70(pObject);
        return;
    }
    p = (char*)pObject->pData;
    lbl_80281FF4 = fn_800A1B94(p);
    if (lbl_80281FF4 == 0) {
        fn_80009E70(pObject);
        return;
    }
    lbl_80281FF0 = fn_80009B34(lbl_80281FF4 * sizeof(MCEagmEntry), 2, 0x10, "MC.c", 3336);
    for (i = 0; i < lbl_80281FF4; i++) {
        lbl_80281FF0[i].n8 = fn_800A1B28(p);
        lbl_80281FF0[i].p4 = fn_80009B34(lbl_80281FF0[i].n8 * 16, 2, 0x10, "MC.c", 3349);
        for (j = 0; j < lbl_80281FF0[i].n8 + 1; j++) {
            p = fn_800A1A90(p);
        }
    }
    p = (char*)pObject->pData;
    for (i = 0; i < lbl_80281FF4; i++) {
        lbl_80281FF0[i].b0 = 0;
        nLen = fn_800A19F8(p);
        strncpy(lbl_80281FF0[i].szName, p, nLen);
        lbl_80281FF0[i].szName[nLen] = 0;
        fn_800A1C58(lbl_80281FF0[i].szName);
        p = fn_800A1A90(p);
        for (j = 0; j < lbl_80281FF0[i].n8; j++) {
            nLen = fn_800A19F8(p);
            strncpy(lbl_80281FF0[i].p4 + j * 16, p, nLen);
            (lbl_80281FF0[i].p4 + j * 16)[nLen] = 0;
            fn_800A1C58(lbl_80281FF0[i].p4 + j * 16);
            p = fn_800A1A90(p);
        }
    }
    fn_80009E70(pObject);
}

// Find the first 'eagm' entry holding a name that szGameCode starts with: mark it and set its bit
// in lbl_801F1100.
void fn_800A1F6C(const char* szGameCode) {
    int i;
    int j;
    for (i = 0; i < lbl_80281FF4; i++) {
        for (j = 0; j < lbl_80281FF0[i].n8; j++) {
            if (strncmp(lbl_80281FF0[i].p4 + j * 16, szGameCode,
                        strlen(lbl_80281FF0[i].p4 + j * 16)) == 0) {
                lbl_80281FF0[i].b0 = 1;
                fn_8001EA34(lbl_801F1100, i);
                return;
            }
        }
    }
}

// How many 'eagm' entries are marked.
s32 fn_800A2030(void) {
    s32 nMarked = 0;
    MCEagmEntry* pEntry = lbl_80281FF0;
    int i;
    for (i = 0; i < lbl_80281FF4; i++) {
        if (pEntry->b0) {
            nMarked++;
        }
        pEntry++;
    }
    return nMarked;
}

// At start-up: point the images' spare pointers at the second image, build the CRC table, start the
// CARD library and clear the per-card format failures and the 'eagm' bits.
void fn_800A2064(void) {
    int nPort;
    int nSlot;
    fn_800A19F4();
    if (lbl_80281FDC == NULL) {
        lbl_80281FDC = NULL;
        lbl_80281FD8 = NULL;
        lbl_80281FE0 = NULL;
    }
    if (lbl_80281FD8 == NULL) {
        lbl_80281FD8 = lbl_80281FDC;
    }
    if (lbl_80281FE0 == NULL) {
        lbl_80281FE0 = lbl_80281FDC;
    }
    fn_800A253C();
    CARDInit();
    for (nPort = 0; nPort < MC_NUM_PORTS; nPort++) {
        // EA bug: the slot loop runs to 2 where a port has one slot, so [0][1] is [1][0] and
        // [1][1] writes past the array, into lbl_801F1640's texture names
        for (nSlot = 0; nSlot < 2; nSlot++) {
            lbl_801F1510[nPort][nSlot].b94 = 0;
        }
    }
    fn_8001E938(lbl_801F1100, 0x80);
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

s32 fn_800A218C(s32 nPort, s32 nSlot) {
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

// Whether the data from pData up to pTrailer is a good save: the mark "@BD" or "@BE", then the
// checksum.
u8 fn_800A233C(void* pData, SaveTrailer* pTrailer) {
    u32 uSum;
    if (pTrailer->aMagic[0] != '@' || pTrailer->aMagic[1] != 'B'
        || (pTrailer->aMagic[2] != 'D' && pTrailer->aMagic[2] != 'E')) {
        return 0;
    }
    uSum = fn_800A23BC(pData, pTrailer);
    if (uSum != pTrailer->uChecksum) {
        return 0;
    }
    return 1;
}

// The save's CRC-32, over the table fn_800A253C builds.
u32 fn_800A23BC(void* pData, SaveTrailer* pTrailer) {
    u32 i;
    u8* p = pData;
    u32 nLen = (u8*)&pTrailer->uChecksum - p;
    u32 uCrc = 0xFFFFFFFF;
    u32 uSaved = pTrailer->uChecksum;
    pTrailer->uChecksum = 0;
    for (i = 0; i < nLen; i++) {
        uCrc = (uCrc >> 8) ^ lbl_801F1110[(uCrc ^ *p++) & 0xFF];
    }
    pTrailer->uChecksum = uSaved;
    return uCrc ^ 0xFFFFFFFF;
}

// Build the CRC table, at start-up (fn_800A2064). EA shifts right but uses the polynomial's
// unreflected form 0x04C11DB7 (the usual table takes 0xEDB88320), so this is not the standard
// CRC-32; the game only ever checks its own sums with it.
void fn_800A253C(void) {
    u32 i;
    u32 uCrc;
    int k;
    for (i = 0; i < 256; i++) {
        uCrc = i;
        for (k = 0; k < 8; k++) {
            if (uCrc & 1) {
                uCrc = (uCrc >> 1) ^ 0x04C11DB7;
            } else {
                uCrc >>= 1;
            }
        }
        lbl_801F1110[i] = uCrc;
    }
}

u8 fn_800A2604(s32 nEntry) {
    return lbl_80281FF0[nEntry].b0;
}

char* fn_800A2614(s32 nEntry) {
    return lbl_80281FF0[nEntry].szName;
}

s32 fn_800A2628(void) {
    return lbl_80281FF4;
}

// Three callbacks of a table in .data (0x80189CB0 area), one per save kind, each paired with one
// of fn_800A26D8/fn_800A270C/fn_800A2740: whether the file on the card was rejected as bad data.
s32 fn_800A2630(MCCardPos* pPos) {
    return fn_8009EE28(pPos->nPort, pPos->nSlot) == MC_ERR_BADDATA;
}

s32 fn_800A2668(MCCardPos* pPos) {
    return fn_8009EE28(pPos->nPort, pPos->nSlot) == MC_ERR_BADDATA;
}

s32 fn_800A26A0(MCCardPos* pPos) {
    return fn_8009EE28(pPos->nPort, pPos->nSlot) == MC_ERR_BADDATA;
}

// The space the three save kinds need (fn_8009D1D8's last argument).
void fn_800A26D8(MCCardPos* pPos) {
    fn_8009D1D8(pPos->nPort, pPos->nSlot, 0, 0);
}

void fn_800A270C(MCCardPos* pPos) {
    fn_8009D1D8(pPos->nPort, pPos->nSlot, 0, 1);
}

void fn_800A2740(MCCardPos* pPos) {
    fn_8009D1D8(pPos->nPort, pPos->nSlot, 0, 2);
}

void fn_800A2774(const u16* szSrc, char* szDst, s32 nMax) {
    while (*szSrc != 0 && nMax > 1) {
        if (*szSrc > 0xFF) {
            *szDst = '\xAC';
        } else {
            *szDst = *szSrc;
        }
        szSrc++;
        szDst++;
        nMax--;
    }
    *szDst = 0;
}

void fn_800A27BC(const char* szSrc, u16* szDst, s32 nMax) {
    while (*szSrc != 0 && nMax > 1) {
        *szDst = *szSrc;
        szSrc++;
        nMax--;
        szDst++;
    }
    *szDst = 0;
}

s32 fn_800A27F4(void) {
    return lbl_80281FF8;
}
