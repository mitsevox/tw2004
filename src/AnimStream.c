// AnimStream.c (EA's name, from its asserts): streams animation clips for the reaction groups
// (1 and 5) per player, style and club class, reading them from disc into double buffers. Only
// partly decompiled. The types are in character.h.

#include "golfer.h"

int  fn_80006444(int hFile, void* pDst, u32 uLen, u32 uOffset, void (*pfnDone)(int, int));   // UStream.c:
                                                                                               // async read
void fn_8006C63C(void);                 // called while waiting for a read
void fn_800C9F14(u8 bForce);
void fn_800CB550(int nResult, int n);

AnimStreamGroup lbl_80191490[2] = {
    { 1, 0 },
    { 5, 1 },
};

// The index of an animation group in the stream's tables, -1 when it is not streamed.
int fn_800C98DC(int nGroup) {
    int i;
    for (i = 0; i < 2; i++) {
        if (nGroup == lbl_80191490[i].nGroup) {
            return lbl_80191490[i].nIndex;
        }
    }
    return -1;
}

// The animation group at an index of the stream's tables, -1 for none.
int fn_800C9928(int nIndex) {
    int i;
    for (i = 0; i < 2; i++) {
        if (nIndex == lbl_80191490[i].nIndex) {
            return lbl_80191490[i].nGroup;
        }
    }
    return -1;
}

// In game type 6, with streaming on: waits for a read in progress, then finishes it.
void fn_800C9CA0(void) {
    if (gSession.nGameType != 6 || lbl_80282230 == NULL || lbl_80282230->bOn == 0) return;
    while (lbl_80282230->nState == 1) {
        fn_8006C63C();
    }
    if (lbl_80282230->nState == 2) {
        fn_800C9F14(1);
    }
}

// The read's completion callback: the read is done.
void fn_800C9EFC(int nResult, int n) {
    lbl_80282230->nState  = 2;
    lbl_80282230->nResult = nResult;
}

// Marks a player's clips for a group, style and club class.
void fn_800CA268(int nPlayer, int a, int nGroup, int nClub, int nStyle) {
    int nIndex = fn_800C98DC(nGroup);
    if (nIndex >= 0) {
        lbl_80282230->players[nPlayer].clips[nIndex][nStyle][nClub].b8 = 1;
    }
}

// Reads from a file and waits for it. A read past the end of the file is cut to what is left,
// rounded down to 2 KB.
void fn_800CB4E0(int hFile, u32 uFileSize, void* pDst, u32 uLen, u32 uOffset) {
    if (uFileSize < uOffset + uLen) {
        uLen = uFileSize - uOffset;
        uLen -= uLen & 0x7FF;
    }
    lbl_80282230->bReadDone = 0;
    fn_80006444(hFile, pDst, uLen, uOffset, fn_800CB550);
    do {
        fn_8006C63C();
    } while (lbl_80282230->bReadDone == 0);
}

// fn_800CB4E0's completion callback.
void fn_800CB550(int nResult, int n) {
    lbl_80282230->bReadDone = 1;
    lbl_80282230->nResult   = nResult;
}

// The player whose part of the stream has this id, -1 for none.
int fn_800CB568(int nId) {
    int i;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        if (lbl_80282230->players[i].nId == nId) {
            return i;
        }
    }
    return -1;
}
