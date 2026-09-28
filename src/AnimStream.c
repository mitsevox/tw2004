// AnimStream.c (EA's name, from its asserts): the golfer animation stream. Instead of keeping
// every clip of animation groups 1 and 5 (the reactions) in memory, it keeps one clip per player,
// style and club class in one of two buffer slots (the first two players to play hold one each)
// and reads the next clip from disc (data\CharStrm\*.sac) in the background while the current one
// plays, so the clips take turns. char.c sets it up (AnimStream_Init, AnimStream_SizeSlotClips,
// AnimStream_AllocBuffers, AnimStream_ReadFirstClips), skalib.c's AnimLib_Pick plays from it
// (AnimStream_GetClip) and the main loop drives the reads (AnimStream_Update). In this build
// AnimStream_Init turns streaming off in every case, so no clip is streamed. The file also holds the
// base-40 name codes (SKA_PackName, SKA_UnpackName, SKA_UnpackSwappedName). The types are in
// character.h.

#include "game.h"
#include "endian.h"

void fn_8006C63C(void);                 // called while waiting for a read
u8 AnimStream_StartRead(int nSlot, int nPlayer, int nIndex, int nStyle, int nClub);
void AnimStream_OnReadDone(int nBytes, int nError);
void AnimStream_EndRead(u8 bForce);
void AnimStream_OnReadNowDone(int nBytes, int nError);
void AnimStream_SizePlayerClips(int nPlayer, AnimLib* pOverlay, AnimLib* pLib);
void AnimStream_MarkPlayerClips(int nPlayer, u8 bMark);
void AnimStream_SizeLibClips(int nPlayer, AnimLib* pLib, int nFirst, int nLast, int nStyleFirst,
                             int nStyleLast, int nClubFirst, int nClubLast);
void AnimStream_ReadPlayerClips(int nPlayer);
void AnimStream_ReadSharedClips(int nSlot);
void AnimStream_ReadNow(int hFile, u32 uFileSize, void* pDst, u32 uLen, u32 uOffset);
void AnimStream_GetFilePath(u8 bGlobal, int bFemale, int nPlayer, char* szPath);
int AnimStream_FindSlotPlayer(int nId);
u8 AnimStream_CanReplaceClip(int nPlayer, Clip* pClip);

char gAnimStreamRoot[8] = "";          // the folder the stream files' paths start from (empty)

// The stream's state (character.h), allocated by AnimStream_Init; the file's .sbss.
AnimStream* gpAnimStream;

// The animation groups the stream handles and each one's index in its tables.
AnimStreamGroup gAnimStreamGroups[2] = {
    { 1, 0 },
    { 5, 1 },
};

// Allocates the stream's state with no buffers and no player slots. Streaming is off with a
// controller of type 8, in game mode 11, in one kind of split screen or with two or more players,
// and in the end it is turned off in every case.
void AnimStream_Init(void) {
    int i;
    int nPlayer;
    int j;
    int k;
    int m;

    gpAnimStream = StaticMem_Alloc(sizeof(AnimStream), 2, 0, "AnimStream.c", 158);
    gpAnimStream->p0 = NULL;
    gpAnimStream->p4 = NULL;
    gpAnimStream->pRead = NULL;
    gpAnimStream->nBytes = sizeof(AnimStream);
    gpAnimStream->nState = 0;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            for (k = 0; k < 8; k++) {
                for (m = 0; m < 6; m++) {
                    gpAnimStream->bufs[i][j][k][m].pData = NULL;
                    gpAnimStream->bufs[i][j][k][m].nSize = -1;
                }
            }
        }
    }
    for (nPlayer = 0; nPlayer < 5; nPlayer++) {
        gpAnimStream->players[nPlayer].nId = -1;
        for (j = 0; j < 2; j++) {
            for (k = 0; k < 8; k++) {
                for (m = 0; m < 6; m++) {
                    gpAnimStream->players[nPlayer].clips[j][k][m].nNext = -1;
                    gpAnimStream->players[nPlayer].clips[j][k][m].nMaxSize = -1;
                    gpAnimStream->players[nPlayer].clips[j][k][m].b8 = 0;
                }
            }
        }
    }
    gpAnimStream->bOn = 1;
    for (i = 0; i < 5; i++) {
        if (gSession.nController[i] == 8) {
            gpAnimStream->bOn = 0;
        }
    }
    if (Game_GetMode() == 11) {
        gpAnimStream->bOn = 0;
    }
    if (gSession.nSplitScreen == 1) {
        gpAnimStream->bOn = 0;
    }
    if (gSession.nNumPlayers > 2) {
        gpAnimStream->bOn = 0;
    }
    if (gSession.nNumPlayers > 1) {
        gpAnimStream->bOn = 0;
    }
    gpAnimStream->bOn = 0;
}

// Frees the stream: the read buffer, every clip buffer, then the state itself.
void AnimStream_Close(void) {
    int i;
    int j;
    int k;
    int m;

    if (gpAnimStream->pRead != NULL) {
        StaticMem_Free(gpAnimStream->pRead);
    }
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            for (k = 0; k < 8; k++) {
                for (m = 0; m < 6; m++) {
                    if (gpAnimStream->bufs[i][j][k][m].pData != NULL) {
                        StaticMem_Free(gpAnimStream->bufs[i][j][k][m].pData);
                    }
                }
            }
        }
    }
    StaticMem_Free(gpAnimStream);
    gpAnimStream = NULL;
}

// Whether the clips of a group, style and club class are streamed: only with streaming on, only
// for the default key, and group 5 only for club class 2 or style 7.
u8 AnimStream_IsStreamed(int nGroup, int nStyle, int nClub, int nKey) {
    int i;

    if (gpAnimStream == NULL) return 0;
    if (gpAnimStream->bOn == 0) return 0;
    if (nStyle < 0 || nClub < 0) return 0;
    if (nKey >= 0) return 0;
    if (nGroup == 5 && nClub != 2 && nStyle != 7) return 0;
    if (nGroup == 1) return 1;
    for (i = 0; i < 2; i++) {
        if (nGroup == gAnimStreamGroups[i].nGroup) return 1;
    }
    return 0;
}

// The index of an animation group in the stream's tables, -1 when it is not streamed.
int AnimStream_GetGroupIndex(int nGroup) {
    int i;
    for (i = 0; i < 2; i++) {
        if (nGroup == gAnimStreamGroups[i].nGroup) {
            return gAnimStreamGroups[i].nIndex;
        }
    }
    return -1;
}

// The animation group at an index of the stream's tables, -1 for none.
int AnimStream_GetGroup(int nIndex) {
    int i;
    for (i = 0; i < 2; i++) {
        if (nIndex == gAnimStreamGroups[i].nIndex) {
            return gAnimStreamGroups[i].nGroup;
        }
    }
    return -1;
}

// Once a frame from the main loop, in game type 6 with streaming on: ends a read that is done
// (AnimStream_EndRead, only if its clip is free to replace), and when no read is going (p0 NULL)
// starts the next one: the first clip set marked for reading (b8 == 1) of the player holding a
// slot, trying the slot after the one the first player to play (GM_GetHonors) holds first, then the
// slots before it. A set whose read does not start (AnimStream_StartRead returns 0) is passed over.
void AnimStream_Update(void) {
    int nStart;
    int nSlot;
    int i;
    int nStyle;
    int nClub;
    int nPlayer;
    int nFirst;

    if (gSession.nGameType != 6) return;
    if (gpAnimStream == NULL) return;
    if (gpAnimStream->bOn == 0) return;
    if (gpAnimStream->nState == 2) {
        AnimStream_EndRead(0);
    }
    if (gpAnimStream->p0 != NULL) return;
    nFirst = GM_GetHonors();
    if (nFirst == 5) return;
    if ((nStart = gpAnimStream->players[nFirst].nId) < 0) {
        nStart = 0;
    }
    nStart++;
    nStart %= 2;
    for (nSlot = nStart; nSlot < 2; nSlot++) {
        nPlayer = AnimStream_FindSlotPlayer(nSlot);
        if (nPlayer < 0 || gpAnimStream->p0 != NULL) break;
        for (i = 0; i < 2; i++) {
            for (nStyle = 0; nStyle < 8; nStyle++) {
                for (nClub = 0; nClub < 6; nClub++) {
                    if (gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].b8 == 1) {
                        gpAnimStream->p0 = &gpAnimStream->bufs[nSlot][i][nStyle][nClub];
                        gpAnimStream->p4 = &gpAnimStream->players[nPlayer].clips[i][nStyle][nClub];
                        if (AnimStream_StartRead(nSlot, nPlayer, i, nStyle, nClub) == 0) {
                            gpAnimStream->p0 = NULL;
                            gpAnimStream->p4 = NULL;
                            continue;
                        }
                        return;
                    }
                }
            }
        }
    }
    if (gpAnimStream->p0 != NULL) return;
    for (nSlot = 0; nSlot < nStart; nSlot++) {
        nPlayer = AnimStream_FindSlotPlayer(nSlot);
        if (nPlayer < 0) return;
        if (gpAnimStream->p0 != NULL) return;
        for (i = 0; i < 2; i++) {
            for (nStyle = 0; nStyle < 8; nStyle++) {
                for (nClub = 0; nClub < 6; nClub++) {
                    if (gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].b8 == 1) {
                        gpAnimStream->p0 = &gpAnimStream->bufs[nSlot][i][nStyle][nClub];
                        gpAnimStream->p4 = &gpAnimStream->players[nPlayer].clips[i][nStyle][nClub];
                        if (AnimStream_StartRead(nSlot, nPlayer, i, nStyle, nClub) == 0) {
                            gpAnimStream->p0 = NULL;
                            gpAnimStream->p4 = NULL;
                            continue;
                        }
                        return;
                    }
                }
            }
        }
    }
}

// In game type 6 with streaming on: waits (calling fn_8006C63C) while a read is in progress, then
// ends it with AnimStream_EndRead(1), so its clip is stored even if the golfer is playing the old
// one. Called on the way out of a hole.
void AnimStream_WaitForRead(void) {
    if (gSession.nGameType != 6 || gpAnimStream == NULL || gpAnimStream->bOn == 0) return;
    while (gpAnimStream->nState == 1) {
        fn_8006C63C();
    }
    if (gpAnimStream->nState == 2) {
        AnimStream_EndRead(1);
    }
}

// Starts reading a player's next clip of a group (by index), style and club class into the read
// buffer, when the set has two or more clips (they take turns: nNext advances and wraps). The clip
// comes from the shared stream file or the golfer's own (its record's flag 2); the read runs in the
// background and AnimStream_OnReadDone ends it. Returns 1 when the read started, 0 when the set has
// fewer than two clips or the read could not start (the file is closed again; nState stays 1). The
// caller has set the buffer and the clip set (p0, p4); nSlot is not used.
u8 AnimStream_StartRead(int nSlot, int nPlayer, int nIndex, int nStyle, int nClub) {
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
    AnimLib_Find(pLib, AnimStream_GetGroup(nIndex), nStyle, nClub, 0, &nCount, &uFlags, NULL, &nFirst);
    if (nCount < 2) return 0;
    gpAnimStream->players[nPlayer].clips[nIndex][nStyle][nClub].nNext++;
    if (gpAnimStream->players[nPlayer].clips[nIndex][nStyle][nClub].nNext >= nCount) {
        gpAnimStream->players[nPlayer].clips[nIndex][nStyle][nClub].nNext = 0;
    }
    pChar = gPlayers[nPlayer].pChar;
    nFirst += gpAnimStream->players[nPlayer].clips[nIndex][nStyle][nClub].nNext;
    pRec = &pChar->pRecords[nFirst];
    AnimStream_GetFilePath((pRec->n12 >> 1) & 1, pChar->nSlot, nPlayer, szPath);
    uOffset = pRec->n20;
    gpAnimStream->hFile = fn_800060E0(szPath);
    gpAnimStream->nState = 1;
    gpAnimStream->n1CC8 = nPlayer;
    uFileSize = fn_800065B0(gpAnimStream->hFile);
    if (uFileSize < uOffset + gpAnimStream->p0->nSize) {
        uLen = uFileSize - uOffset;
        uLen -= uLen & 0x7FF;
    } else {
        uLen = gpAnimStream->p0->nSize;
    }
    if (fn_80006444(gpAnimStream->hFile, gpAnimStream->pRead, uLen, uOffset, AnimStream_OnReadDone) < 0) {
        fn_8000633C(gpAnimStream->hFile);
        gpAnimStream->hFile = -1;
        return 0;
    }
    return 1;
}

// AnimStream_StartRead's completion callback: the read is done (nState 2) and got nBytes bytes.
// nError is not looked at.
void AnimStream_OnReadDone(int nBytes, int nError) {
    gpAnimStream->nState  = 2;
    gpAnimStream->nResult = nBytes;
}

// Ends the current read: closes the file and, when bForce is set or the clip in the set's buffer is
// free to replace (AnimStream_CanReplaceClip), copies what was read (if anything) into that buffer,
// prepares it (SKA_LoadFromMem), clears the set's read mark (b8) and goes idle (nState 0).
// Otherwise the read stays done (nState 2) and a later call stores it.
void AnimStream_EndRead(u8 bForce) {
    if (gpAnimStream->hFile >= 0) {
        fn_8000633C(gpAnimStream->hFile);
        gpAnimStream->hFile = -1;
    }
    if (bForce || AnimStream_CanReplaceClip(gpAnimStream->n1CC8, gpAnimStream->p0->pData)) {
        if (gpAnimStream->nResult > 0) {
            Mem_cpy(gpAnimStream->p0->pData, gpAnimStream->pRead, gpAnimStream->nResult);
            SKA_LoadFromMem(gpAnimStream->p0->pData, NULL, 16);
        }
        gpAnimStream->p4->b8 = 0;
        gpAnimStream->p4 = NULL;
        gpAnimStream->nState = 0;
        gpAnimStream->p0 = NULL;
    }
}

// With streaming on: ends a done read (stored at once), waits for one in progress and ends it
// (stored if its clip is free). Then, with three or more players, gives the first two to play
// (GM_GetHonors, GM_GetSecondHonors) one of the stream's two slots each when they have none, taking
// it from its holder; both the new holder and the old one get all their clip sets marked for
// reading, and the first player's marked clips are read at once (AnimStream_ReadPlayerClips). The
// slot loops have no break: when neither slot is held by the other of the two, the player takes
// slot 0 and then slot 1, and slot 0 is left with nobody. Called by Character_ReloadSacFiles and an
// event.c handler.
void AnimStream_AssignSlots(void) {
    int nFirst;
    int nSecond;
    int i;
    int nOther;

    if (gpAnimStream == NULL) {
        return;
    }
    if (gpAnimStream->bOn == 0) {
        return;
    }
    if (gpAnimStream->nState == 2) {
        AnimStream_EndRead(1);
    }
    while (gpAnimStream->nState == 1) {
        fn_8006C63C();
    }
    if (gpAnimStream->nState == 2) {
        AnimStream_EndRead(0);
    }
    if (gSession.nNumPlayers > 2) {
        nFirst = GM_GetHonors();
        nSecond = GM_GetSecondHonors();
        if (nFirst != 5) {
            if (gpAnimStream->players[nFirst].nId < 0) {
                for (i = 0; i < 2; i++) {
                    nOther = AnimStream_FindSlotPlayer(i);
                    if (nOther != nSecond) {
                        AnimStream_MarkPlayerClips(nFirst, 1);
                        gpAnimStream->players[nFirst].nId = i;
                        if (nOther >= 0) {
                            gpAnimStream->players[nOther].nId = -1;
                            AnimStream_MarkPlayerClips(nOther, 1);
                        }
                    }
                }
            }
            AnimStream_ReadPlayerClips(nFirst);
        }
        if (nSecond != 5) {
            if (gpAnimStream->players[nSecond].nId < 0) {
                for (i = 0; i < 2; i++) {
                    nOther = AnimStream_FindSlotPlayer(i);
                    if (nOther != nSecond && nOther != nFirst) {
                        AnimStream_MarkPlayerClips(nSecond, 1);
                        gpAnimStream->players[nSecond].nId = i;
                        if (nOther >= 0) {
                            gpAnimStream->players[nOther].nId = -1;
                            AnimStream_MarkPlayerClips(nOther, 1);
                        }
                    }
                }
            }
        }
    }
}

// Sets the read mark (b8) of each of a player's streamed clip sets that has buffers to bMark.
// AnimStream_AssignSlots marks them all (bMark = 1) when a player's slot changes hands, so the new
// slot's buffers get the player's clips.
void AnimStream_MarkPlayerClips(int nPlayer, u8 bMark) {
    int i;
    int nStyle;
    int nClub;
    int nGroup;

    for (i = 0; i < 2; i++) {
        nGroup = AnimStream_GetGroup(i);
        for (nStyle = 0; nStyle < 8; nStyle++) {
            for (nClub = 0; nClub < 6; nClub++) {
                if (AnimStream_IsStreamed(nGroup, nStyle, nClub, -1) &&
                    gpAnimStream->bufs[0][i][nStyle][nClub].nSize > 0) {
                    gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].b8 = bMark;
                }
            }
        }
    }
}

// Marks a player's clip set for a group, style and club class to have its next clip read (b8 = 1);
// nothing for a group that is not streamed. nSlot (the player's slot) is not used, and the club
// class comes before the style.
void AnimStream_MarkClips(int nPlayer, int nSlot, int nGroup, int nClub, int nStyle) {
    int nIndex = AnimStream_GetGroupIndex(nGroup);
    if (nIndex >= 0) {
        gpAnimStream->players[nPlayer].clips[nIndex][nStyle][nClub].b8 = 1;
    }
}

// With streaming on, works out the largest clip (rounded up to 2 KB) of each of a player's streamed
// clip sets, flagging the clips' records as streamed (flag 4): from the overlay library's default
// clips where it has the set, and from the base library's (AnimStream_SizeLibClips) for every
// group, style or club class the overlay lacks or whose club node asks for them too (flag 1).
void AnimStream_SizePlayerClips(int nPlayer, AnimLib* pOverlay, AnimLib* pLib) {
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

    if (gpAnimStream->bOn == 0) return;
    for (i = 0; i < 2; i++) {
        for (nStyle = 0; nStyle < 8; nStyle++) {
            for (nClub = 0; nClub < 6; nClub++) {
                gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].nMaxSize = 0;
            }
        }
    }
    for (i = 0; i < 2; i++) {
        nGroup = gAnimStreamGroups[i].nGroup;
        nNode = pOverlay->groups[nGroup];
        if (nNode < 0) {
            AnimStream_SizeLibClips(nPlayer, pLib, i, i, 0, 7, 0, 5);
            continue;
        }
        pGroup = (s16*)(pOverlay->pTree + nNode);
        for (nStyle = 0; nStyle < 8; nStyle++) {
            if (pGroup[1 + nStyle] < 0) {
                AnimStream_SizeLibClips(nPlayer, pLib, i, i, nStyle, nStyle, 0, 5);
                continue;
            }
            pStyle = (s16*)(pOverlay->pTree + pGroup[1 + nStyle]);
            for (nClub = 0; nClub < 6; nClub++) {
                if (!AnimStream_IsStreamed(nGroup, nStyle, nClub, -1)) continue;
                if (pStyle[nClub] < 0) {
                    AnimStream_SizeLibClips(nPlayer, pLib, i, i, nStyle, nStyle, nClub, nClub);
                    continue;
                }
                pClub = (AnimClubNode*)(pOverlay->pTree + pStyle[nClub]);
                if (pClub->uFlags & 1) {
                    AnimStream_SizeLibClips(nPlayer, pLib, i, i, nStyle, nStyle, nClub, nClub);
                }
                if (pClub->nDefault >= 0) {
                    pLeaf = (AnimLeaf*)(pOverlay->pTree + pClub->nDefault);
                    pIndex = &pOverlay->pIndex[pLeaf->nFirst];
                    for (k = 0; k < pLeaf->nCount; k++) {
                        pRec = &pOverlay->pRecords[*pIndex];
                        pRec->n12 |= 4;
                        nSize = pRec->n18;
                        if (pRec->n18 > gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].nMaxSize) {
                            if (nSize % 0x800 != 0) {
                                nSize += 0x800 - nSize % 0x800;
                            }
                            gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].nMaxSize = nSize;
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
void AnimStream_SizeLibClips(int nPlayer, AnimLib* pLib, int nFirst, int nLast, int nStyleFirst,
                             int nStyleLast, int nClubFirst, int nClubLast) {
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

    if (gpAnimStream->bOn == 0) return;
    for (i = nFirst; i <= nLast; i++) {
        nGroup = gAnimStreamGroups[i].nGroup;
        nNode = pLib->groups[nGroup];
        if (nNode < 0) continue;
        pGroup = (s16*)(pLib->pTree + nNode);
        for (nStyle = nStyleFirst; nStyle <= nStyleLast; nStyle++) {
            if (pGroup[1 + nStyle] < 0) continue;
            pStyle = (s16*)(pLib->pTree + pGroup[1 + nStyle]);
            for (nClub = nClubFirst; nClub <= nClubLast; nClub++) {
                if (!AnimStream_IsStreamed(nGroup, nStyle, nClub, -1)) continue;
                if (pStyle[nClub] < 0) continue;
                pClub = (AnimClubNode*)(pLib->pTree + pStyle[nClub]);
                if (pClub->nDefault < 0) continue;
                pLeaf = (AnimLeaf*)(pLib->pTree + pClub->nDefault);
                pIndex = &pLib->pIndex[pLeaf->nFirst];
                for (k = 0; k < pLeaf->nCount; k++) {
                    pRec = &pLib->pRecords[*pIndex];
                    pRec->n12 |= 4;
                    nSize = pRec->n18;
                    if (pRec->n18 > gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].nMaxSize) {
                        if (nSize % 0x800 != 0) {
                            nSize += 0x800 - nSize % 0x800;
                        }
                        gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].nMaxSize = nSize;
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
void AnimStream_AllocBuffers(void) {
    int nMax = 0;
    int i;
    int nStyle;
    int nClub;
    int nPlayer;
    int nSize;
    int nClip;
    int j;

    if (gpAnimStream->bOn == 0) return;
    for (nPlayer = 0; nPlayer < gSession.nNumPlayers; nPlayer++) {
        gpAnimStream->aPlayerBytes[nPlayer] = 0;
    }
    for (i = 0; i < 2; i++) {
        for (nStyle = 0; nStyle < 8; nStyle++) {
            for (nClub = 0; nClub < 6; nClub++) {
                nSize = 0;
                for (nPlayer = 0; nPlayer < gSession.nNumPlayers; nPlayer++) {
                    gpAnimStream->aPlayerBytes[nPlayer] +=
                        gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].nMaxSize;
                    nClip = gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].nMaxSize;
                    if (nClip > nMax) {
                        nMax = nClip;
                    }
                    if (nClip > nSize) {
                        nSize = nClip;
                    }
                }
                if (nSize > 0) {
                    for (j = 0; j < 2; j++) {
                        gpAnimStream->bufs[j][i][nStyle][nClub].pData =
                            StaticMem_Alloc(nSize, 2, 64, "AnimStream.c", 1005);
                        gpAnimStream->bufs[j][i][nStyle][nClub].nSize = nSize;
                        gpAnimStream->nBytes += nSize;
                    }
                }
            }
        }
    }
    if (nMax > 0) {
        gpAnimStream->pRead = StaticMem_Alloc(nMax, 2, 64, "AnimStream.c", 1018);
        gpAnimStream->nReadSize = nMax;
        gpAnimStream->nBytes += nMax;
    }
}

// With streaming on, sets up the streamed clips of each player whose character uses this
// animation slot (-1: every player), from the character's overlay library and its slot's library.
void AnimStream_SizeSlotClips(int nSlot) {
    int i;

    if (gpAnimStream->bOn == 0) return;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        if (nSlot == -1 || nSlot == gPlayers[i].pChar->nSlot) {
            AnimStream_SizePlayerClips(i, fn_80026AC0(gPlayers[i].pChar), fn_80026B34(gPlayers[i].pChar));
        }
    }
}

// The clip AnimLib_Pick plays when AnimStream_IsStreamed says the position is streamed: the buffer
// of the player's slot for a group, style and club class (style 0's when that style has none). A
// player without a slot first takes slot nPlayer from whoever holds it; players 2 and up can not,
// and get NULL. The set is then marked for its next clip to be read, and the character's key frame
// buffers are emptied.
void* AnimStream_GetClip(int nPlayer, int nGroup, int nStyle, int nClub) {
    int nIndex;
    int nOther;
    void* pData;
    int i;
    int nUseStyle = nStyle;

    nIndex = AnimStream_GetGroupIndex(nGroup);
    if (gpAnimStream->players[nPlayer].nId == -1) {
        if (nPlayer < 2) {
            for (i = nPlayer; i < 2; i++) {
                nOther = AnimStream_FindSlotPlayer(i);
                if (nOther != nPlayer) {
                    gpAnimStream->players[nPlayer].nId = i;
                    // EA bug: when no player has slot i, nOther is -1 and this writes before players[0]
                    gpAnimStream->players[nOther].nId = -1;
                    break;
                }
            }
        } else {
            return NULL;
        }
    }
    pData = gpAnimStream->bufs[gpAnimStream->players[nPlayer].nId][nIndex][nStyle][nClub].pData;
    if (pData == NULL) {
        pData = gpAnimStream->bufs[gpAnimStream->players[nPlayer].nId][nIndex][0][nClub].pData;
        nUseStyle = 0;
    }
    AnimStream_MarkClips(nPlayer, gpAnimStream->players[nPlayer].nId, nGroup, nClub, nUseStyle);
    Character_ClearKeyFrameBuffers(gPlayers[nPlayer].pChar);
    return pData;
}

// With streaming on, starts each of every player's streamed clip sets at a random clip.
void AnimStream_RandomizeClips(void) {
    int i;
    int nIndex;
    int nStyle;
    int nClub;
    Character* pChar;
    s32 nCount;
    u32 uFlags;

    if (gpAnimStream->bOn != 0) {
        for (i = 0; i < gSession.nNumPlayers; i++) {
            pChar = gPlayers[i].pChar;
            for (nIndex = 0; nIndex < 2; nIndex++) {
                for (nStyle = 0; nStyle < 8; nStyle++) {
                    for (nClub = 0; nClub < 6; nClub++) {
                        if (gpAnimStream->players[i].clips[nIndex][nStyle][nClub].nMaxSize > 0) {
                            AnimLib_Find(pChar->pLib, AnimStream_GetGroup(nIndex), nStyle, nClub, 0, &nCount,
                                         &uFlags,
                                         NULL, NULL);
                            if (nCount > 0) {
                                gpAnimStream->players[i].clips[nIndex][nStyle][nClub].nNext =
                                    Misc_RandFunc(1) % nCount;
                            }
                        }
                    }
                }
            }
        }
    }
}

// With streaming on, reads the current clip of each of a player's marked clip sets (b8) into the
// buffers of the player's slot and clears the mark, waiting for each read: first the clips from the
// golfer's own stream file, then those from the shared file of the character's animation slot.
void AnimStream_ReadPlayerClips(int nPlayer) {
    Character* pChar;
    AnimLib* pLib;
    ClipRecord* pRecords;
    int nSlot;
    int i;
    int nStyle;
    int nClub;
    int nAnimSlot;
    u32 uFileSize;
    s32 nCount;
    u32 uFlags;
    s32 nFirst;
    char szPath[256];

    uFlags = 0;
    if (gpAnimStream->bOn == 0) return;
    pChar = gPlayers[nPlayer].pChar;
    nAnimSlot = pChar->nSlot;
    pRecords = pChar->pRecords;
    nSlot = gpAnimStream->players[nPlayer].nId;
    pLib = pChar->pLib;
    AnimStream_GetFilePath(0, pChar->nSlot, nPlayer, szPath);
    gpAnimStream->hFile = fn_800060E0(szPath);
    uFileSize = fn_800065B0(gpAnimStream->hFile);
    for (i = 0; i < 2; i++) {
        for (nStyle = 0; nStyle < 8; nStyle++) {
            for (nClub = 0; nClub < 6; nClub++) {
                if (gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].nMaxSize > 0 &&
                    gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].b8 != 0) {
                    AnimLib_Find(pLib, AnimStream_GetGroup(i), nStyle, nClub, 0, &nCount, &uFlags, NULL,
                                 &nFirst);
                    if (nCount > 0) {
                        nFirst += gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].nNext;
                        if (!(pRecords[nFirst].n12 & 2)) {
                            AnimStream_ReadNow(gpAnimStream->hFile, uFileSize,
                                        gpAnimStream->bufs[nSlot][i][nStyle][nClub].pData,
                                        gpAnimStream->bufs[nSlot][i][nStyle][nClub].nSize,
                                        pRecords[nFirst].n20);
                            gpAnimStream->bufs[nSlot][i][nStyle][nClub].pData =
                                SKA_LoadFromMem(gpAnimStream->bufs[nSlot][i][nStyle][nClub].pData, NULL, 16);
                            gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].b8 = 0;
                        }
                    }
                }
            }
        }
    }
    fn_8000633C(gpAnimStream->hFile);
    gpAnimStream->hFile = -1;

    AnimStream_GetFilePath(1, nAnimSlot, 0, szPath);
    gpAnimStream->hFile = fn_800060E0(szPath);
    uFileSize = fn_800065B0(gpAnimStream->hFile);
    for (i = 0; i < 2; i++) {
        for (nStyle = 0; nStyle < 8; nStyle++) {
            for (nClub = 0; nClub < 6; nClub++) {
                if (gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].nMaxSize > 0 &&
                    gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].b8 != 0) {
                    AnimLib_Find(pLib, AnimStream_GetGroup(i), nStyle, nClub, 0, &nCount, &uFlags, NULL,
                                 &nFirst);
                    if (nCount > 0) {
                        nFirst += gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].nNext;
                        if (pRecords[nFirst].n12 & 2) {
                            AnimStream_ReadNow(gpAnimStream->hFile, uFileSize,
                                        gpAnimStream->bufs[nSlot][i][nStyle][nClub].pData,
                                        gpAnimStream->bufs[nSlot][i][nStyle][nClub].nSize,
                                        pRecords[nFirst].n20);
                            gpAnimStream->bufs[nSlot][i][nStyle][nClub].pData =
                                SKA_LoadFromMem(gpAnimStream->bufs[nSlot][i][nStyle][nClub].pData, NULL, 16);
                            gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].b8 = 0;
                        }
                    }
                }
            }
        }
    }
    fn_8000633C(gpAnimStream->hFile);
    gpAnimStream->hFile = -1;
}

// With streaming on, gives the first two players a slot each and reads the current clip of each
// of their streamed clip sets from their golfer's own stream file (the clips not in the shared
// files), then from the two shared files.
void AnimStream_ReadFirstClips(void) {
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
    if (gpAnimStream->bOn == 0) return;
    for (nPlayer = 0; nPlayer < gSession.nNumPlayers && nPlayer < 2; nPlayer++) {
        gpAnimStream->players[nPlayer].nId = nPlayer;
        pChar = gPlayers[nPlayer].pChar;
        pLib = pChar->pLib;
        pRecords = pChar->pRecords;
        AnimStream_GetFilePath(0, pChar->nSlot, nPlayer, szPath);
        gpAnimStream->hFile = fn_800060E0(szPath);
        uFileSize = fn_800065B0(gpAnimStream->hFile);
        for (i = 0; i < 2; i++) {
            for (nStyle = 0; nStyle < 8; nStyle++) {
                for (nClub = 0; nClub < 6; nClub++) {
                    if (gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].nMaxSize > 0) {
                        AnimLib_Find(pLib, AnimStream_GetGroup(i), nStyle, nClub, 0, &nCount, &uFlags, NULL,
                                     &nFirst);
                        if (nCount > 0) {
                            nFirst += gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].nNext;
                            if (!(pRecords[nFirst].n12 & 2)) {
                                AnimStream_ReadNow(gpAnimStream->hFile, uFileSize,
                                            gpAnimStream->bufs[nPlayer][i][nStyle][nClub].pData,
                                            gpAnimStream->bufs[nPlayer][i][nStyle][nClub].nSize,
                                            pRecords[nFirst].n20);
                                gpAnimStream->bufs[nPlayer][i][nStyle][nClub].pData =
                                    SKA_LoadFromMem(gpAnimStream->bufs[nPlayer][i][nStyle][nClub].pData,
                                                    NULL, 16);
                                gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].b8 = 0;
                            }
                        }
                    }
                }
            }
        }
        fn_8000633C(gpAnimStream->hFile);
        gpAnimStream->hFile = -1;
    }
    AnimStream_ReadSharedClips(0);
    AnimStream_ReadSharedClips(1);
}

// With streaming on, reads the current clip of each streamed clip set that sits in shared stream
// file nSlot, for the first two players whose character uses that animation slot.
void AnimStream_ReadSharedClips(int nSlot) {
    Character* pChar;
    int nPlayer;
    AnimLib* pLib;
    ClipRecord* pRecords;
    ClipRecord* pRec;
    int i;
    int nStyle;
    int nClub;
    s32 nCount;
    u32 uFlags;
    s32 nFirst;
    char szPath[256];
    u32 uFileSize;

    uFlags = 0;
    if (gpAnimStream->bOn == 0) return;
    AnimStream_GetFilePath(1, nSlot, 0, szPath);
    gpAnimStream->hFile = fn_800060E0(szPath);
    uFileSize = fn_800065B0(gpAnimStream->hFile);
    for (nPlayer = 0; nPlayer < gSession.nNumPlayers && nPlayer < 2; nPlayer++) {
        pChar = gPlayers[nPlayer].pChar;
        if (nSlot != pChar->nSlot) continue;
        pLib = pChar->pLib;
        pRecords = pChar->pRecords;
        for (i = 0; i < 2; i++) {
            for (nStyle = 0; nStyle < 8; nStyle++) {
                for (nClub = 0; nClub < 6; nClub++) {
                    if (gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].nMaxSize > 0) {
                        AnimLib_Find(pLib, AnimStream_GetGroup(i), nStyle, nClub, 0, &nCount, &uFlags, NULL,
                                     &nFirst);
                        if (nCount > 0) {
                            nFirst += gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].nNext;
                            pRec = &pRecords[nFirst];
                            if (pRec->n12 & 2) {
                                AnimStream_ReadNow(gpAnimStream->hFile, uFileSize,
                                            gpAnimStream->bufs[nPlayer][i][nStyle][nClub].pData,
                                            gpAnimStream->bufs[nPlayer][i][nStyle][nClub].nSize,
                                            pRec->n20);
                                gpAnimStream->bufs[nPlayer][i][nStyle][nClub].pData =
                                    SKA_LoadFromMem(gpAnimStream->bufs[nPlayer][i][nStyle][nClub].pData,
                                                    NULL, 16);
                                gpAnimStream->players[nPlayer].clips[i][nStyle][nClub].b8 = 0;
                            }
                        }
                    }
                }
            }
        }
    }
    fn_8000633C(gpAnimStream->hFile);
    gpAnimStream->hFile = -1;
}

// Reads from a file and waits for it. A read past the end of the file is cut to what is left,
// rounded down to 2 KB.
void AnimStream_ReadNow(int hFile, u32 uFileSize, void* pDst, u32 uLen, u32 uOffset) {
    if (uFileSize < uOffset + uLen) {
        uLen = uFileSize - uOffset;
        uLen -= uLen & 0x7FF;
    }
    gpAnimStream->bReadDone = 0;
    fn_80006444(hFile, pDst, uLen, uOffset, AnimStream_OnReadNowDone);
    do {
        fn_8006C63C();
    } while (gpAnimStream->bReadDone == 0);
}

// AnimStream_ReadNow's completion callback: sets bReadDone and keeps the byte count. nError is not
// looked at.
void AnimStream_OnReadNowDone(int nBytes, int nError) {
    gpAnimStream->bReadDone = 1;
    gpAnimStream->nResult   = nBytes;
}

// The player holding stream slot nId (0 or 1), -1 for none.
int AnimStream_FindSlotPlayer(int nId) {
    int i;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        if (gpAnimStream->players[i].nId == nId) {
            return i;
        }
    }
    return -1;
}

// Whether a streamed clip may be replaced: always for players other than the one whose turn it
// is; for that player, only while the golfer is not playing it.
u8 AnimStream_CanReplaceClip(int nPlayer, Clip* pClip) {
    if (lbl_80282278 != nPlayer) return 1;
    if (SKABlender_HasClip(&gPlayers[nPlayer].pChar->blend, pClip)) return 0;
    if (SKABlender_HasMtaLib(&gPlayers[nPlayer].pChar->node3E0, pClip->pF4)) return 0;
    if (gPlayers[nPlayer].pChar->p1790 == pClip || gPlayers[nPlayer].pChar->p1794 == pClip) return 0;
    return 1;
}

// The path of a stream file: the male or female animations every golfer shares, or the ones of
// the player's own golfer model.
void AnimStream_GetFilePath(u8 bGlobal, int bFemale, int nPlayer, char* szPath) {
    if (bGlobal) {
        if (bFemale == 0) {
            sprintf(szPath, "%sdata\\CharStrm\\AnimGlob\\male.sac", gAnimStreamRoot);
            return;
        }
        sprintf(szPath, "%sdata\\CharStrm\\AnimGlob\\female.sac", gAnimStreamRoot);
        return;
    }
    sprintf(szPath, "%sdata\\CharStrm\\AnimChar\\%02dchr.sac", gAnimStreamRoot,
            Character_GetGolferModelID(nPlayer) + 1);
}

// The base-40 digit of each character for SKA_PackName, -1 for none: '\0' 0, '+' 1, '-' 2, '0'-'9'
// 3-12, letters 13-38 (upper and lower case alike), '_' 39.
s32 gSKANameCodes[128] = {
    0,  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1,  -1, 2,  -1, -1,
    3,  4,  5,  6,  7,  8,  9,  10, 11, 12, -1, -1, -1, -1, -1, -1,
    -1, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
    28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, -1, -1, -1, -1, 39,
    -1, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
    28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, -1, -1, -1, -1, -1,
};

// The character of each base-40 digit, for SKA_UnpackName (letters come back in lower case).
char gSKANameChars[40] = "\0+-0123456789abcdefghijklmnopqrstuvwxyz_";

// Packs up to 12 characters of pName into a base-40 code (a shorter name is padded with code 0),
// stored with its bytes reversed (SKA_UnpackSwappedName reads it back). Upper and lower case
// letters pack the same; a character without a code becomes '_'. Returns 0, 1 when the name is
// longer than 12 characters, or 2 when a character was replaced.
int SKA_PackName(u64* pId, const char* pName) {
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
            if (c < 128 && gSKANameCodes[c] != -1) {
                bValid = 1;
            }
            if (!bValid) {
                c = '_';
                nResult = 2;
            }
            pName++;
            *pId *= 40;
            *pId += gSKANameCodes[c];
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

// Unpacks a name code into its 12 characters (short names come back padded with '\0'); szName takes
// 13 bytes. The code must be in native byte order: SKA_PackName's own output is reversed and goes
// through SKA_UnpackSwappedName.
void SKA_UnpackName(u64* pId, char* szName) {
    int i;
    u64 uId = *pId;

    szName[12] = '\0';
    for (i = 11; i >= 0; i--) {
        szName[i] = gSKANameChars[uId % 40];
        uId /= 40;
    }
}

// SKA_UnpackName for a code stored with its bytes reversed, as SKA_PackName stores it.
void SKA_UnpackSwappedName(u64* pId, char* szName) {
    u64 uId = *pId;
    u8* p = (u8*)&uId;

    BYTESWAP_SWAPDATA(&p, (u8*)&uId, sizeof(u64), sizeof(u64));
    SKA_UnpackName(&uId, szName);
}
