// MC.c (EA's name, from its asserts; also in EA's 2002 source tree): the game side of the memory
// card: finding the save file on either card, loading and saving it, and the results the menus turn
// into messages. The GameCube calls are in MC_Gc.c (include/core/memcard.h).
//
// Each operation here mounts the card if it is not mounted yet (fn_8009D74C) and unmounts it
// again after only in that case.

#include "core/memcard.h"
#include "frontend/fe.h"

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
