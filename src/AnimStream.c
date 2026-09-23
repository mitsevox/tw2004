// AnimStream.c (EA's name, from its asserts): streams animation clips for the reaction groups
// (1 and 5) per player, style and club class, reading them from disc into double buffers. Only
// partly decompiled. The types are in character.h.

#include "golfer.h"
#include "endian.h"

void fn_8006C63C(void);                 // called while waiting for a read
void fn_800C9F14(u8 bForce);
void fn_800CB550(int nBytes, int nError);
void fn_800CA2E4(int nPlayer, AnimLib* pOverlay, AnimLib* pLib);
AnimLib* fn_80026AC0(Character* pChar);  // the overlay library loaded for the character (slots 0 and 1)
AnimLib* fn_80026B34(Character* pChar);  // the library of the character's animation slot

AnimStreamGroup lbl_80191490[2] = {
    { 1, 0 },
    { 5, 1 },
};

// Frees the stream: the read buffer, every clip buffer, then the state itself.
void fn_800C9764(void) {
    int i;
    int j;
    int k;
    int m;

    if (lbl_80282230->pRead != NULL) {
        fn_80009E70(lbl_80282230->pRead);
    }
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            for (k = 0; k < 8; k++) {
                for (m = 0; m < 6; m++) {
                    if (lbl_80282230->bufs[i][j][k][m].pData != NULL) {
                        fn_80009E70(lbl_80282230->bufs[i][j][k][m].pData);
                    }
                }
            }
        }
    }
    fn_80009E70(lbl_80282230);
    lbl_80282230 = NULL;
}

// Whether the clips of a group, style and club class are streamed: only with streaming on, only
// for the default key, and group 5 only for club class 2 or style 7.
u8 fn_800C9828(int nGroup, int nStyle, int nClub, int nKey) {
    int i;

    if (lbl_80282230 == NULL) return 0;
    if (lbl_80282230->bOn == 0) return 0;
    if (nStyle < 0 || nClub < 0) return 0;
    if (nKey >= 0) return 0;
    if (nGroup == 5 && nClub != 2 && nStyle != 7) return 0;
    if (nGroup == 1) return 1;
    for (i = 0; i < 2; i++) {
        if (nGroup == lbl_80191490[i].nGroup) return 1;
    }
    return 0;
}

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
void fn_800C9EFC(int nBytes, int nError) {
    lbl_80282230->nState  = 2;
    lbl_80282230->nResult = nBytes;
}

// Sets b8 of each of a player's streamed clip sets that has a buffer.
void fn_800CA194(int nPlayer, u8 b) {
    int i;
    int nStyle;
    int nClub;
    int nGroup;

    for (i = 0; i < 2; i++) {
        nGroup = fn_800C9928(i);
        for (nStyle = 0; nStyle < 8; nStyle++) {
            for (nClub = 0; nClub < 6; nClub++) {
                if (fn_800C9828(nGroup, nStyle, nClub, -1) &&
                    lbl_80282230->bufs[0][i][nStyle][nClub].nSize > 0) {
                    lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].b8 = b;
                }
            }
        }
    }
}

// Marks a player's clips for a group, style and club class.
void fn_800CA268(int nPlayer, int a, int nGroup, int nClub, int nStyle) {
    int nIndex = fn_800C98DC(nGroup);
    if (nIndex >= 0) {
        lbl_80282230->players[nPlayer].clips[nIndex][nStyle][nClub].b8 = 1;
    }
}

// With streaming on, sets up the streamed clips of each player whose character uses this
// animation slot (-1: every player), from the character's overlay library and its slot's library.
void fn_800CA9DC(int nSlot) {
    int i;

    if (lbl_80282230->bOn == 0) return;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        if (nSlot == -1 || nSlot == gPlayers[i].pChar->nSlot) {
            fn_800CA2E4(i, fn_80026AC0(gPlayers[i].pChar), fn_80026B34(gPlayers[i].pChar));
        }
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
void fn_800CB550(int nBytes, int nError) {
    lbl_80282230->bReadDone = 1;
    lbl_80282230->nResult   = nBytes;
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

// Unpacks a name code (fn_800CB700) into its 12 characters.
void fn_800CB868(u64* pId, char* szName) {
    int i;
    u64 uId = *pId;

    szName[12] = '\0';
    for (i = 11; i >= 0; i--) {
        szName[i] = lbl_80191720[uId % 40];
        uId /= 40;
    }
}

// The same for a code stored with its bytes reversed.
void fn_800CB8F0(u64* pId, char* szName) {
    u64 uId = *pId;
    u8* p = (u8*)&uId;

    fn_80076158(&p, (u8*)&uId, sizeof(u64), sizeof(u64));
    fn_800CB868(&uId, szName);
}
