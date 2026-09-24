// AnimStream.c (EA's name, from its asserts): streams animation clips for the reaction groups
// (1 and 5) per player, style and club class, reading them from disc into double buffers. Only
// partly decompiled. The types are in character.h.

#include "game.h"
#include "endian.h"

void fn_8006C63C(void);                 // called while waiting for a read
u8 fn_800C9D14(int nSlot, int nPlayer, int nIndex, int nStyle, int nClub);
void fn_800C9EFC(int nBytes, int nError);
void fn_800C9F14(u8 bForce);
void fn_800CB550(int nBytes, int nError);
void fn_800CA2E4(int nPlayer, AnimLib* pOverlay, AnimLib* pLib);
void fn_800CA194(int nPlayer, u8 b);
void fn_800CA610(int nPlayer, AnimLib* pLib, int nFirst, int nLast, int nStyleFirst, int nStyleLast,
                 int nClubFirst, int nClubLast);
void fn_800CACD4(int nPlayer);
void fn_800CB2B0(int nSlot);
void fn_800CB4E0(int hFile, u32 uFileSize, void* pDst, u32 uLen, u32 uOffset);
AnimLib* fn_80026B34(Character* pChar);  // the library of the character's animation slot
void fn_800CB668(u8 bGlobal, int bFemale, int nPlayer, char* szPath);
int fn_800CB568(int nId);
u8 fn_800CB5B0(int nPlayer, Clip* pClip);

char lbl_80281530[8] = "";              // the folder the stream files' paths start from

AnimStreamGroup lbl_80191490[2] = {
    { 1, 0 },
    { 5, 1 },
};

// Allocates the stream's state with no buffers and no player slots. Streaming is off with a
// controller of type 8, in game mode 11, in one kind of split screen or with two or more players,
// and in the end it is turned off in every case.
void fn_800C937C(void) {
    int i;
    int j;
    int k;
    int m;

    lbl_80282230 = fn_80009B34(sizeof(AnimStream), 2, 0, "AnimStream.c", 158);
    lbl_80282230->p0 = NULL;
    lbl_80282230->p4 = NULL;
    lbl_80282230->pRead = NULL;
    lbl_80282230->nBytes = sizeof(AnimStream);
    lbl_80282230->nState = 0;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            for (k = 0; k < 8; k++) {
                for (m = 0; m < 6; m++) {
                    lbl_80282230->bufs[i][j][k][m].pData = NULL;
                    lbl_80282230->bufs[i][j][k][m].nSize = -1;
                }
            }
        }
    }
    for (i = 0; i < 5; i++) {
        lbl_80282230->players[i].nId = -1;
        for (j = 0; j < 2; j++) {
            for (k = 0; k < 8; k++) {
                for (m = 0; m < 6; m++) {
                    lbl_80282230->players[i].clips[j][k][m].nNext = -1;
                    lbl_80282230->players[i].clips[j][k][m].nMaxSize = -1;
                    lbl_80282230->players[i].clips[j][k][m].b8 = 0;
                }
            }
        }
    }
    lbl_80282230->bOn = 1;
    for (i = 0; i < 5; i++) {
        if (gSession.nController[i] == 8) {
            lbl_80282230->bOn = 0;
        }
    }
    if (Game_GetMode() == 11) {
        lbl_80282230->bOn = 0;
    }
    if (gSession.nSplitScreen == 1) {
        lbl_80282230->bOn = 0;
    }
    if (gSession.nNumPlayers > 2) {
        lbl_80282230->bOn = 0;
    }
    if (gSession.nNumPlayers > 1) {
        lbl_80282230->bOn = 0;
    }
    lbl_80282230->bOn = 0;
}

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

// In game type 6, with streaming on and no read going: finishes a done read, then starts reading
// the next marked clip set (b8 == 1), trying the slots in turn from the one after the first
// player's to play.
void fn_800C9970(void) {
    int nStart;
    int nSlot;
    int nPlayer;
    int i;
    int nStyle;
    int nClub;
    int nFirst;

    if (gSession.nGameType != 6) return;
    if (lbl_80282230 == NULL) return;
    if (lbl_80282230->bOn == 0) return;
    if (lbl_80282230->nState == 2) {
        fn_800C9F14(0);
    }
    if (lbl_80282230->p0 != NULL) return;
    nFirst = fn_800E292C();
    if (nFirst == 5) return;
    nStart = lbl_80282230->players[nFirst].nId;
    if (nStart < 0) {
        nStart = 0;
    }
    nStart = (nStart + 1) % 2;
    for (nSlot = nStart; nSlot < 2; nSlot++) {
        nPlayer = fn_800CB568(nSlot);
        if (nPlayer < 0 || lbl_80282230->p0 != NULL) break;
        for (i = 0; i < 2; i++) {
            for (nStyle = 0; nStyle < 8; nStyle++) {
                for (nClub = 0; nClub < 6; nClub++) {
                    if (lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].b8 == 1) {
                        lbl_80282230->p0 = &lbl_80282230->bufs[nSlot][i][nStyle][nClub];
                        lbl_80282230->p4 = &lbl_80282230->players[nPlayer].clips[i][nStyle][nClub];
                        if (fn_800C9D14(nSlot, nPlayer, i, nStyle, nClub) == 0) {
                            lbl_80282230->p0 = NULL;
                            lbl_80282230->p4 = NULL;
                            continue;
                        }
                        return;
                    }
                }
            }
        }
    }
    if (lbl_80282230->p0 != NULL) return;
    for (nSlot = 0; nSlot < nStart; nSlot++) {
        nPlayer = fn_800CB568(nSlot);
        if (nPlayer < 0 || lbl_80282230->p0 != NULL) return;
        for (i = 0; i < 2; i++) {
            for (nStyle = 0; nStyle < 8; nStyle++) {
                for (nClub = 0; nClub < 6; nClub++) {
                    if (lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].b8 == 1) {
                        lbl_80282230->p0 = &lbl_80282230->bufs[nSlot][i][nStyle][nClub];
                        lbl_80282230->p4 = &lbl_80282230->players[nPlayer].clips[i][nStyle][nClub];
                        if (fn_800C9D14(nSlot, nPlayer, i, nStyle, nClub) == 0) {
                            lbl_80282230->p0 = NULL;
                            lbl_80282230->p4 = NULL;
                            continue;
                        }
                        return;
                    }
                }
            }
        }
    }
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

// Starts reading a player's next clip for a group (by index), style and club class, when the set
// has two or more: the clips take turns. Returns 1 when the read started. The caller has set the
// buffer and the clip set (p0, p4); nSlot is not used.
u8 fn_800C9D14(int nSlot, int nPlayer, int nIndex, int nStyle, int nClub) {
    Character* pChar;
    AnimLib* pLib;
    ClipRecord* pRec;
    u32 uOffset;
    u32 uFileSize;
    u32 uLen;
    s32 nCount;
    u32 uFlags;
    s32 nFirst;
    char szPath[256];

    uFlags = 0;
    pLib = gPlayers[nPlayer].pChar->pLib;
    AnimLib_Find(pLib, fn_800C9928(nIndex), nStyle, nClub, 0, &nCount, &uFlags, NULL, &nFirst);
    if (nCount < 2) return 0;
    lbl_80282230->players[nPlayer].clips[nIndex][nStyle][nClub].nNext++;
    if (lbl_80282230->players[nPlayer].clips[nIndex][nStyle][nClub].nNext >= nCount) {
        lbl_80282230->players[nPlayer].clips[nIndex][nStyle][nClub].nNext = 0;
    }
    pChar = gPlayers[nPlayer].pChar;
    nFirst += lbl_80282230->players[nPlayer].clips[nIndex][nStyle][nClub].nNext;
    pRec = &pChar->pRecords[nFirst];
    fn_800CB668((pRec->n12 >> 1) & 1, pChar->nSlot, nPlayer, szPath);
    uOffset = pRec->n20;
    lbl_80282230->hFile = fn_800060E0(szPath);
    lbl_80282230->nState = 1;
    lbl_80282230->n1CC8 = nPlayer;
    uFileSize = fn_800065B0(lbl_80282230->hFile);
    if (uFileSize < uOffset + lbl_80282230->p0->nSize) {
        uLen = uFileSize - uOffset;
        uLen -= uLen & 0x7FF;
    } else {
        uLen = lbl_80282230->p0->nSize;
    }
    if (fn_80006444(lbl_80282230->hFile, lbl_80282230->pRead, uLen, uOffset, fn_800C9EFC) < 0) {
        fn_8000633C(lbl_80282230->hFile);
        lbl_80282230->hFile = -1;
        return 0;
    }
    return 1;
}

// The read's completion callback: the read is done.
void fn_800C9EFC(int nBytes, int nError) {
    lbl_80282230->nState  = 2;
    lbl_80282230->nResult = nBytes;
}

// Ends the current read: closes the file and, when bForce is set or the clip is free to replace,
// copies what was read into the clip's buffer and marks the request done.
void fn_800C9F14(u8 bForce) {
    if (lbl_80282230->hFile >= 0) {
        fn_8000633C(lbl_80282230->hFile);
        lbl_80282230->hFile = -1;
    }
    if (bForce || fn_800CB5B0(lbl_80282230->n1CC8, lbl_80282230->p0->pData)) {
        if (lbl_80282230->nResult > 0) {
            Mem_cpy(lbl_80282230->p0->pData, lbl_80282230->pRead, lbl_80282230->nResult);
            fn_80020DD4(lbl_80282230->p0->pData, NULL, 16);
        }
        lbl_80282230->p4->b8 = 0;
        lbl_80282230->p4 = NULL;
        lbl_80282230->nState = 0;
        lbl_80282230->p0 = NULL;
    }
}

// With streaming on, finishes any read, then with three or more players makes sure the first two to
// play have a slot each, taking one from a player who holds it.
void fn_800C9FE0(void) {
    int nFirst;
    int nSecond;
    int i;
    int nOther;

    if (lbl_80282230 == NULL) {
        return;
    }
    if (lbl_80282230->bOn == 0) {
        return;
    }
    if (lbl_80282230->nState == 2) {
        fn_800C9F14(1);
    }
    while (lbl_80282230->nState == 1) {
        fn_8006C63C();
    }
    if (lbl_80282230->nState == 2) {
        fn_800C9F14(0);
    }
    if (gSession.nNumPlayers > 2) {
        nFirst = fn_800E292C();
        nSecond = fn_800E295C();
        if (nFirst != 5) {
            if (lbl_80282230->players[nFirst].nId < 0) {
                for (i = 0; i < 2; i++) {
                    nOther = fn_800CB568(i);
                    if (nOther != nSecond) {
                        fn_800CA194(nFirst, 1);
                        lbl_80282230->players[nFirst].nId = i;
                        if (nOther >= 0) {
                            lbl_80282230->players[nOther].nId = -1;
                            fn_800CA194(nOther, 1);
                        }
                    }
                }
            }
            fn_800CACD4(nFirst);
        }
        if (nSecond != 5) {
            if (lbl_80282230->players[nSecond].nId < 0) {
                for (i = 0; i < 2; i++) {
                    nOther = fn_800CB568(i);
                    if (nOther != nSecond && nOther != nFirst) {
                        fn_800CA194(nSecond, 1);
                        lbl_80282230->players[nSecond].nId = i;
                        if (nOther >= 0) {
                            lbl_80282230->players[nOther].nId = -1;
                            fn_800CA194(nOther, 1);
                        }
                    }
                }
            }
        }
    }
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

// With streaming on, works out a player's streamed clip sets: the overlay library's clips where it
// has them, and the base library's (fn_800CA610) for every group, style or club class it lacks or
// whose club node asks for them (flag 1).
void fn_800CA2E4(int nPlayer, AnimLib* pOverlay, AnimLib* pLib) {
    int i;
    int nGroup;
    int nStyle;
    int nClub;
    int nNode;
    s16* pGroup;
    s16* pStyle;
    s16* pIndex;
    int k;
    AnimLeaf* pLeaf;
    int nSize;
    AnimClubNode* pClub;
    ClipRecord* pRec;

    if (lbl_80282230->bOn == 0) return;
    for (i = 0; i < 2; i++) {
        for (nStyle = 0; nStyle < 8; nStyle++) {
            for (nClub = 0; nClub < 6; nClub++) {
                lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].nMaxSize = 0;
            }
        }
    }
    for (i = 0; i < 2; i++) {
        nGroup = lbl_80191490[i].nGroup;
        nNode = pOverlay->groups[nGroup];
        if (nNode < 0) {
            fn_800CA610(nPlayer, pLib, i, i, 0, 7, 0, 5);
            continue;
        }
        pGroup = (s16*)(pOverlay->pTree + nNode);
        for (nStyle = 0; nStyle < 8; nStyle++) {
            if (pGroup[1 + nStyle] < 0) {
                fn_800CA610(nPlayer, pLib, i, i, nStyle, nStyle, 0, 5);
                continue;
            }
            pStyle = (s16*)(pOverlay->pTree + pGroup[1 + nStyle]);
            for (nClub = 0; nClub < 6; nClub++) {
                if (!fn_800C9828(nGroup, nStyle, nClub, -1)) continue;
                if (pStyle[nClub] < 0) {
                    fn_800CA610(nPlayer, pLib, i, i, nStyle, nStyle, nClub, nClub);
                    continue;
                }
                pClub = (AnimClubNode*)(pOverlay->pTree + pStyle[nClub]);
                if (pClub->uFlags & 1) {
                    fn_800CA610(nPlayer, pLib, i, i, nStyle, nStyle, nClub, nClub);
                }
                if (pClub->nDefault >= 0) {
                    pLeaf = (AnimLeaf*)(pOverlay->pTree + pClub->nDefault);
                    pIndex = &pOverlay->pIndex[pLeaf->nFirst];
                    for (k = 0; k < pLeaf->nCount; k++) {
                        pRec = &pOverlay->pRecords[*pIndex];
                        pRec->n12 |= 4;
                        nSize = pRec->n18;
                        if (pRec->n18 > lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].nMaxSize) {
                            if (nSize % 0x800 != 0) {
                                nSize += 0x800 - nSize % 0x800;
                            }
                            lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].nMaxSize = nSize;
                        }
                        pIndex++;
                    }
                }
            }
        }
    }
}

// With streaming on, goes through a player's library over a range of the stream's groups (by
// index), styles and club classes: marks the records of each streamed clip set's default clips
// (flag 4) and keeps the set's largest clip size, rounded up to 0x800 bytes.
void fn_800CA610(int nPlayer, AnimLib* pLib, int nFirst, int nLast, int nStyleFirst, int nStyleLast,
                 int nClubFirst, int nClubLast) {
    int nGroup;
    int i;
    int nStyle;
    int nClub;
    int nNode;
    s16* pGroup;
    s16* pStyle;
    s16* pIndex;
    int k;
    AnimLeaf* pLeaf;
    int nSize;
    AnimClubNode* pClub;
    ClipRecord* pRec;

    if (lbl_80282230->bOn == 0) return;
    for (i = nFirst; i <= nLast; i++) {
        nGroup = lbl_80191490[i].nGroup;
        nNode = pLib->groups[nGroup];
        if (nNode < 0) continue;
        pGroup = (s16*)(pLib->pTree + nNode);
        for (nStyle = nStyleFirst; nStyle <= nStyleLast; nStyle++) {
            if (pGroup[1 + nStyle] < 0) continue;
            pStyle = (s16*)(pLib->pTree + pGroup[1 + nStyle]);
            for (nClub = nClubFirst; nClub <= nClubLast; nClub++) {
                if (!fn_800C9828(nGroup, nStyle, nClub, -1)) continue;
                if (pStyle[nClub] < 0) continue;
                pClub = (AnimClubNode*)(pLib->pTree + pStyle[nClub]);
                if (pClub->nDefault < 0) continue;
                pLeaf = (AnimLeaf*)(pLib->pTree + pClub->nDefault);
                pIndex = &pLib->pIndex[pLeaf->nFirst];
                for (k = 0; k < pLeaf->nCount; k++) {
                    pRec = &pLib->pRecords[*pIndex];
                    pRec->n12 |= 4;
                    nSize = pRec->n18;
                    if (pRec->n18 > lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].nMaxSize) {
                        if (nSize % 0x800 != 0) {
                            nSize += 0x800 - nSize % 0x800;
                        }
                        lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].nMaxSize = nSize;
                    }
                    pIndex++;
                }
            }
        }
    }
}

// With streaming on, adds up the bytes each player's clips need, gives each group, style and club
// class two buffers as big as the largest player's clips, and the read buffer as big as the
// largest of all.
void fn_800CA7E0(void) {
    int nMax = 0;
    int i;
    int nStyle;
    int nClub;
    int nPlayer;
    int nSize;
    int nClip;
    int j;

    if (lbl_80282230->bOn == 0) return;
    for (nPlayer = 0; nPlayer < gSession.nNumPlayers; nPlayer++) {
        lbl_80282230->aPlayerBytes[nPlayer] = 0;
    }
    for (i = 0; i < 2; i++) {
        for (nStyle = 0; nStyle < 8; nStyle++) {
            for (nClub = 0; nClub < 6; nClub++) {
                nSize = 0;
                for (nPlayer = 0; nPlayer < gSession.nNumPlayers; nPlayer++) {
                    lbl_80282230->aPlayerBytes[nPlayer] +=
                        lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].nMaxSize;
                    nClip = lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].nMaxSize;
                    if (nClip > nMax) {
                        nMax = nClip;
                    }
                    if (nClip > nSize) {
                        nSize = nClip;
                    }
                }
                if (nSize > 0) {
                    for (j = 0; j < 2; j++) {
                        lbl_80282230->bufs[j][i][nStyle][nClub].pData =
                            fn_80009B34(nSize, 2, 64, "AnimStream.c", 1005);
                        lbl_80282230->bufs[j][i][nStyle][nClub].nSize = nSize;
                        lbl_80282230->nBytes += nSize;
                    }
                }
            }
        }
    }
    if (nMax > 0) {
        lbl_80282230->pRead = fn_80009B34(nMax, 2, 64, "AnimStream.c", 1018);
        lbl_80282230->nReadSize = nMax;
        lbl_80282230->nBytes += nMax;
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

// The buffer holding a player's clips for a group, style and club class (style 0's when that style
// has none), after giving the player one of the stream's two slots if it has none; NULL when the
// player (2 and up) can not have one.
void* fn_800CAA7C(int nPlayer, int nGroup, int nStyle, int nClub) {
    int nIndex;
    int nOther;
    void* pData;
    int i;

    nIndex = fn_800C98DC(nGroup);
    if (lbl_80282230->players[nPlayer].nId == -1) {
        if (nPlayer < 2) {
            for (i = nPlayer; i < 2; i++) {
                nOther = fn_800CB568(i);
                if (nOther != nPlayer) {
                    lbl_80282230->players[nPlayer].nId = i;
                    // EA bug: when no player has slot i, nOther is -1 and this writes before players[0]
                    lbl_80282230->players[nOther].nId = -1;
                    break;
                }
            }
        } else {
            return NULL;
        }
    }
    pData = lbl_80282230->bufs[lbl_80282230->players[nPlayer].nId][nIndex][nStyle][nClub].pData;
    if (pData == NULL) {
        pData = lbl_80282230->bufs[lbl_80282230->players[nPlayer].nId][nIndex][0][nClub].pData;
        nStyle = 0;
    }
    fn_800CA268(nPlayer, lbl_80282230->players[nPlayer].nId, nGroup, nClub, nStyle);
    fn_8001DB98(gPlayers[nPlayer].pChar);
    return pData;
}

// With streaming on, starts each of every player's streamed clip sets at a random clip.
void fn_800CABA0(void) {
    int i;
    int nIndex;
    int nStyle;
    int nClub;
    Character* pChar;
    s32 nCount;
    u32 uFlags;

    if (lbl_80282230->bOn != 0) {
        for (i = 0; i < gSession.nNumPlayers; i++) {
            pChar = gPlayers[i].pChar;
            for (nIndex = 0; nIndex < 2; nIndex++) {
                for (nStyle = 0; nStyle < 8; nStyle++) {
                    for (nClub = 0; nClub < 6; nClub++) {
                        if (lbl_80282230->players[i].clips[nIndex][nStyle][nClub].nMaxSize > 0) {
                            AnimLib_Find(pChar->pLib, fn_800C9928(nIndex), nStyle, nClub, 0, &nCount, &uFlags,
                                         NULL, NULL);
                            if (nCount > 0) {
                                lbl_80282230->players[i].clips[nIndex][nStyle][nClub].nNext =
                                    Rand_Next(1) % nCount;
                            }
                        }
                    }
                }
            }
        }
    }
}

// With streaming on, reads the current clip of each of a player's marked clip sets (b8) into the
// buffers of the player's slot: first the clips from the golfer's own stream file, then those from
// the shared file of the character's animation slot.
void fn_800CACD4(int nPlayer) {
    Character* pChar;
    AnimLib* pLib;
    ClipRecord* pRecords;
    int nSlot;
    int nAnimSlot;
    u32 uFileSize;
    int i;
    int nStyle;
    int nClub;
    s32 nCount;
    u32 uFlags;
    s32 nFirst;
    char szPath[256];

    uFlags = 0;
    if (lbl_80282230->bOn == 0) return;
    pChar = gPlayers[nPlayer].pChar;
    nAnimSlot = pChar->nSlot;
    pRecords = pChar->pRecords;
    nSlot = lbl_80282230->players[nPlayer].nId;
    pLib = pChar->pLib;
    fn_800CB668(0, nAnimSlot, nPlayer, szPath);
    lbl_80282230->hFile = fn_800060E0(szPath);
    uFileSize = fn_800065B0(lbl_80282230->hFile);
    for (i = 0; i < 2; i++) {
        for (nStyle = 0; nStyle < 8; nStyle++) {
            for (nClub = 0; nClub < 6; nClub++) {
                if (lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].nMaxSize > 0 &&
                    lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].b8 != 0) {
                    AnimLib_Find(pLib, fn_800C9928(i), nStyle, nClub, 0, &nCount, &uFlags, NULL, &nFirst);
                    if (nCount > 0) {
                        nFirst += lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].nNext;
                        if (!(pRecords[nFirst].n12 & 2)) {
                            fn_800CB4E0(lbl_80282230->hFile, uFileSize,
                                        lbl_80282230->bufs[nSlot][i][nStyle][nClub].pData,
                                        lbl_80282230->bufs[nSlot][i][nStyle][nClub].nSize,
                                        pRecords[nFirst].n20);
                            lbl_80282230->bufs[nSlot][i][nStyle][nClub].pData =
                                fn_80020DD4(lbl_80282230->bufs[nSlot][i][nStyle][nClub].pData, NULL, 16);
                            lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].b8 = 0;
                        }
                    }
                }
            }
        }
    }
    fn_8000633C(lbl_80282230->hFile);
    lbl_80282230->hFile = -1;

    fn_800CB668(1, nAnimSlot, 0, szPath);
    lbl_80282230->hFile = fn_800060E0(szPath);
    uFileSize = fn_800065B0(lbl_80282230->hFile);
    for (i = 0; i < 2; i++) {
        for (nStyle = 0; nStyle < 8; nStyle++) {
            for (nClub = 0; nClub < 6; nClub++) {
                if (lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].nMaxSize > 0 &&
                    lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].b8 != 0) {
                    AnimLib_Find(pLib, fn_800C9928(i), nStyle, nClub, 0, &nCount, &uFlags, NULL, &nFirst);
                    if (nCount > 0) {
                        nFirst += lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].nNext;
                        if (pRecords[nFirst].n12 & 2) {
                            fn_800CB4E0(lbl_80282230->hFile, uFileSize,
                                        lbl_80282230->bufs[nSlot][i][nStyle][nClub].pData,
                                        lbl_80282230->bufs[nSlot][i][nStyle][nClub].nSize,
                                        pRecords[nFirst].n20);
                            lbl_80282230->bufs[nSlot][i][nStyle][nClub].pData =
                                fn_80020DD4(lbl_80282230->bufs[nSlot][i][nStyle][nClub].pData, NULL, 16);
                            lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].b8 = 0;
                        }
                    }
                }
            }
        }
    }
    fn_8000633C(lbl_80282230->hFile);
    lbl_80282230->hFile = -1;
}

// With streaming on, gives the first two players a slot each and reads the current clip of each
// of their streamed clip sets from their golfer's own stream file (the clips not in the shared
// files), then from the two shared files.
void fn_800CB078(void) {
    Character* pChar;
    int nPlayer;
    AnimLib* pLib;
    ClipRecord* pRecords;
    int i;
    int nStyle;
    int nClub;
    u32 uFileSize;
    s32 nCount;
    u32 uFlags;
    s32 nFirst;
    char szPath[256];

    uFlags = 0;
    if (lbl_80282230->bOn == 0) return;
    for (nPlayer = 0; nPlayer < gSession.nNumPlayers && nPlayer < 2; nPlayer++) {
        lbl_80282230->players[nPlayer].nId = nPlayer;
        pChar = gPlayers[nPlayer].pChar;
        pLib = pChar->pLib;
        pRecords = pChar->pRecords;
        fn_800CB668(0, pChar->nSlot, nPlayer, szPath);
        lbl_80282230->hFile = fn_800060E0(szPath);
        uFileSize = fn_800065B0(lbl_80282230->hFile);
        for (i = 0; i < 2; i++) {
            for (nStyle = 0; nStyle < 8; nStyle++) {
                for (nClub = 0; nClub < 6; nClub++) {
                    if (lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].nMaxSize > 0) {
                        AnimLib_Find(pLib, fn_800C9928(i), nStyle, nClub, 0, &nCount, &uFlags, NULL, &nFirst);
                        if (nCount > 0) {
                            nFirst += lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].nNext;
                            if (!(pRecords[nFirst].n12 & 2)) {
                                fn_800CB4E0(lbl_80282230->hFile, uFileSize,
                                            lbl_80282230->bufs[nPlayer][i][nStyle][nClub].pData,
                                            lbl_80282230->bufs[nPlayer][i][nStyle][nClub].nSize,
                                            pRecords[nFirst].n20);
                                lbl_80282230->bufs[nPlayer][i][nStyle][nClub].pData =
                                    fn_80020DD4(lbl_80282230->bufs[nPlayer][i][nStyle][nClub].pData,
                                                NULL, 16);
                                lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].b8 = 0;
                            }
                        }
                    }
                }
            }
        }
        fn_8000633C(lbl_80282230->hFile);
        lbl_80282230->hFile = -1;
    }
    fn_800CB2B0(0);
    fn_800CB2B0(1);
}

// With streaming on, reads the current clip of each streamed clip set that sits in shared stream
// file nSlot, for the first two players whose character uses that animation slot.
void fn_800CB2B0(int nSlot) {
    Character* pChar;
    int nPlayer;
    AnimLib* pLib;
    ClipRecord* pRecords;
    int i;
    int nStyle;
    int nClub;
    s32 nCount;
    u32 uFlags;
    s32 nFirst;
    char szPath[256];
    u32 uFileSize;

    uFlags = 0;
    if (lbl_80282230->bOn == 0) return;
    fn_800CB668(1, nSlot, 0, szPath);
    lbl_80282230->hFile = fn_800060E0(szPath);
    uFileSize = fn_800065B0(lbl_80282230->hFile);
    for (nPlayer = 0; nPlayer < gSession.nNumPlayers && nPlayer < 2; nPlayer++) {
        pChar = gPlayers[nPlayer].pChar;
        if (nSlot != pChar->nSlot) continue;
        pLib = pChar->pLib;
        pRecords = pChar->pRecords;
        for (i = 0; i < 2; i++) {
            for (nStyle = 0; nStyle < 8; nStyle++) {
                for (nClub = 0; nClub < 6; nClub++) {
                    if (lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].nMaxSize > 0) {
                        AnimLib_Find(pLib, fn_800C9928(i), nStyle, nClub, 0, &nCount, &uFlags, NULL, &nFirst);
                        if (nCount > 0) {
                            nFirst += lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].nNext;
                            if (pRecords[nFirst].n12 & 2) {
                                fn_800CB4E0(lbl_80282230->hFile, uFileSize,
                                            lbl_80282230->bufs[nPlayer][i][nStyle][nClub].pData,
                                            lbl_80282230->bufs[nPlayer][i][nStyle][nClub].nSize,
                                            pRecords[nFirst].n20);
                                lbl_80282230->bufs[nPlayer][i][nStyle][nClub].pData =
                                    fn_80020DD4(lbl_80282230->bufs[nPlayer][i][nStyle][nClub].pData,
                                                NULL, 16);
                                lbl_80282230->players[nPlayer].clips[i][nStyle][nClub].b8 = 0;
                            }
                        }
                    }
                }
            }
        }
    }
    fn_8000633C(lbl_80282230->hFile);
    lbl_80282230->hFile = -1;
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

// Whether a streamed clip may be replaced: always for players other than the one whose turn it
// is; for that player, only while the golfer is not playing it.
u8 fn_800CB5B0(int nPlayer, Clip* pClip) {
    if (lbl_80282278 != nPlayer) return 1;
    if (fn_80073554(&gPlayers[nPlayer].pChar->blend, pClip)) return 0;
    // node3E0 is still bytes in Character (its type is not known yet); it holds a blend node
    if (fn_80073610((SKABlendNode*)gPlayers[nPlayer].pChar->node3E0, pClip->pF4)) return 0;
    if (gPlayers[nPlayer].pChar->p1790 == pClip || gPlayers[nPlayer].pChar->p1794 == pClip) return 0;
    return 1;
}

// The path of a stream file: the male or female animations every golfer shares, or the ones of
// the player's own golfer model.
void fn_800CB668(u8 bGlobal, int bFemale, int nPlayer, char* szPath) {
    if (bGlobal) {
        if (bFemale == 0) {
            sprintf(szPath, "%sdata\\CharStrm\\AnimGlob\\male.sac", lbl_80281530);
            return;
        }
        sprintf(szPath, "%sdata\\CharStrm\\AnimGlob\\female.sac", lbl_80281530);
        return;
    }
    sprintf(szPath, "%sdata\\CharStrm\\AnimChar\\%02dchr.sac", lbl_80281530, fn_8001C558(nPlayer) + 1);
}

// Packs up to 12 characters of pName into a base-40 code, stored with its bytes reversed. A
// character without a code becomes '_'. Returns 0, 1 when the name is longer than 12 characters,
// or 2 when a character was replaced.
int fn_800CB700(u64* pId, const char* pName) {
    int nResult = 0;
    int i;
    char c;
    int bValid;
    u8 aBytes[8];

    *pId = 0;
    for (i = 0; i < 12; i++) {
        if (*pName != '\0') {
            c = *pName;
            bValid = 0;
            // EA bug: char is signed, so a character above 127 is negative and reads before the table
            if (c < 128 && lbl_80191520[c] != -1) {
                bValid = 1;
            }
            if (!bValid) {
                c = '_';
                nResult = 2;
            }
            pName++;
            *pId *= 40;
            *pId += lbl_80191520[c];
        } else {
            *pId *= 40;
        }
    }
    if (*pName != '\0') {
        nResult = 1;
    }
    for (i = 0; i < 8; i++) {
        aBytes[7 - i] = ((u8*)pId)[i];
    }
    memcpy(pId, aBytes, sizeof(u64));
    return nResult;
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
