// SitDevTrigger.c (EA file, TW06/TW07): own unit, its .data starts at 0x801912D0 (4 pad bytes at 0x801912CC)

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "sitdev.h"
#include "game.h"
#include "game/modes/pgatoursim.h"
#include "game/modes/pgatour.h"

int fn_800BB218(u16* pList, int nCount);
int fn_800BB248(u16* pList, int nCount);
u32 fn_800BB334(u16* pList, int nCount, int nLeft, u32 nPick);

void fn_800BD77C(int nSound);
void fn_800BD7D0(u8 nMusic);
void fn_800BD7E8(u16 uSound);
void fn_800BD868(int nSound, int a);

// ---- running a script's actions ------------------------------------------------------------

u8   fn_800BCE70(SitDevAction* pAction, u8 nEvent);
u8   fn_800BCF84(SitDevAction* pAction, int nPlayer, u8 nEvent);
u8   fn_800BD3F8(SitDevAction* pAction, int nSit, int nPlayer, u8 nEvent);
void fn_800BD580(SitDevEntry8* pDo, int nPlayer, u8 nEvent);

// Try the entry's actions in order: each fires by its chance unless fn_800BD3F8 holds it back.
void fn_800BCD68(SitDevEntry* pEntry, int nSit, int nPlayer, u8 nEvent) {
    int i;
    SitDevAction* pAction;
    u8 bPlayed = 0;
    for (i = 0; i < 4; i++) {
        if (pEntry->aActions[i] == 0xFFF0) break;
        pAction = &lbl_80282208->p18[pEntry->aActions[i]];
        if (pAction->nChance > Misc_RandFunc(1) % 100 && !fn_800BD3F8(pAction, nSit, nPlayer, nEvent)) {
            if (pAction->bSound) {
                bPlayed = fn_800BCE70(pAction, nEvent);
            } else {
                bPlayed = fn_800BCF84(pAction, nPlayer, nEvent);
            }
        }
    }
    if (lbl_80281E29 && !bPlayed) {
        lbl_80281E28 = 0;
    }
    lbl_80281E29 = 0;
}

// Play a sound drawn from the action's deck, once per kind; not while the GameBreaker holds the
// commentary back (then, for event 8, hand it to GameEffects for later).
u8 fn_800BCE70(SitDevAction* pAction, u8 nEvent) {
    u32 nLeft;
    int nCount;
    u32 nSound;
    int bNot30;
    if (lbl_802811B8->abPlayed[pAction->nKind]) return 0;
    nCount = fn_800BB218(pAction->aList, 50);
    nLeft = fn_800BB248(pAction->aList, nCount);
    nSound = fn_800BB334(pAction->aList, nCount, nLeft, Misc_RandFunc(1) % nLeft);
    bNot30 = nEvent != 30;
    if (!fn_800DC784()) {
        fn_800BD83C(nSound, bNot30);
        lbl_802811B8->abPlayed[pAction->nKind] = 1;
        return 1;
    }
    if (nEvent == 8) {
        fn_800BD77C(nSound);
    }
    return 0;
}

// Run the action's p1C entries: one of each kind not played yet. A lone entry runs unless it is
// the last line played (except for event 30). Otherwise each kind draws one of its entries at
// random, skipping those already drawn (bit 15 of the list entry; when all are, the marks are
// cleared) and the last line played.
u8 fn_800BCF84(SitDevAction* pAction, int nPlayer, u8 nEvent) {
    s32 anCount[14];
    s32 aaIndex[14][50];
    SitDevEntry8* pDo;
    int nKind;
    int nPick;
    int nStart;
    int nEntry;
    int i;
    s32 j;
    u8 bPlayed = 0;

    if (pAction->aList[1] == 0xFFF0) {
        pDo = &lbl_80282208->p1C[pAction->aList[0]];
        if (pDo == lbl_802811B8->pE8 && nEvent != 30) return 0;
        if (lbl_802811B8->abPlayed[pDo->nKind]) return 0;
        fn_800BD580(pDo, nPlayer, nEvent);
        lbl_802811B8->abPlayed[pDo->nKind] = 1;
        return 1;
    }
    for (i = 0; i < 14; i++) {
        anCount[i] = 0;
    }
    // Sort the entries not played yet by kind.
    for (i = 0; i < 50; i++) {
        if (pAction->aList[i] != 0xFFF0) {
            nKind = lbl_80282208->p1C[pAction->aList[i] & 0x7FFF].nKind;
            if (!lbl_802811B8->abPlayed[nKind]) {
                aaIndex[nKind][anCount[nKind]] = i;
                anCount[(u32)nKind]++;      // fake match: a second spelling of the index, not CSE'd
            }
        }
    }
    for (nKind = 0; nKind < 14; nKind++) {
        if (anCount[nKind] == 1) {
            if (&lbl_80282208->p1C[pAction->aList[aaIndex[nKind][0]]] != lbl_802811B8->pE8) {
                fn_800BD580(&lbl_80282208->p1C[pAction->aList[aaIndex[nKind][0]]], nPlayer, nEvent);
                bPlayed = 1;
                lbl_802811B8->abPlayed[nKind] = 1;
            }
        } else if (anCount[nKind] > 1) {
            nPick = Misc_RandFunc(1) % anCount[nKind];
            nEntry = aaIndex[nKind][nPick];
            nStart = nPick;
            while (pAction->aList[nEntry] & 0x8000) {
                nPick++;
                if (nPick == anCount[nKind]) {
                    nPick = 0;
                }
                nEntry = aaIndex[nKind][nPick];
                if (nPick == nStart) {
                    // Every entry has been drawn: start the deck over.
                    for (j = 0; j < anCount[nKind]; j++) {
                        pAction->aList[aaIndex[nKind][j]] &= 0x7FFF;
                    }
                    nPick = nStart;
                    break;
                }
            }
            pDo = &lbl_80282208->p1C[pAction->aList[nEntry]];
            if (pDo == lbl_802811B8->pE8) {
                nPick++;
                if (nPick == anCount[nKind]) {
                    nPick = 0;
                }
                nEntry = aaIndex[nKind][nPick];
                pDo =&lbl_80282208->p1C[pAction->aList[nEntry] & 0x7FFF];
            }
            fn_800BD580(pDo, nPlayer, nEvent);
            lbl_802811B8->abPlayed[nKind] = 1;
            pAction->aList[nEntry] |= 0x8000;
            bPlayed = 1;
        }
    }
    return bPlayed;
}

// Whether an action is held back: at situations 21 and 22 in modes with odd holes (gpGame's
// b136..b139), or while fn_800EC550 is set (at 22 only for a ball off the tee); for events 20 and
// 31 while the GameBreaker is up (not at situation 2); and commentary (kinds 1 and 2) during a
// replay, in modes 6..8, where fn_800E39F0 says so, and in mode 11.
u8 fn_800BD3F8(SitDevAction* pAction, int nSit, int nPlayer, u8 nEvent) {
    int nMode = Game_GetMode();
    if (nSit == 22 || nSit == 21) {
        if (gpGame->b136 || gpGame->b137 || gpGame->b138 || gpGame->b139) return 1;
        if (fn_800EC550()) {
            if (nSit == 21) return 1;
            if (nSit == 22 && gPlayers[nPlayer].ball.nLie != 0) return 1;
        }
    }
    if (lbl_80202898.bGameBreaker && (nEvent == 20 || nEvent == 31) && nSit != 2) return 1;
    if (pAction->nKind != 1 && pAction->nKind != 2) return 0;
    if (gSession.bReplay || (u32)(nMode - 6) <= 2 || fn_800E39F0()) return 1;
    if (!fn_800EC550() && nMode != 11) return 0;
    if (nMode == 11) return 1;
    return 0;
}

// Do one thing: 1 a commentary line (remembered in pE8), 11 and 10 sounds, 7 music, 4 a
// GameBreaker for a human player, 12 and 13 set the player's emotion results.
void fn_800BD580(SitDevEntry8* pDo, int nPlayer, u8 nEvent) {
    int bNot30;
    int nArg;
    switch (pDo->nKind) {
    case 1:
        if (gSession.options.a0[4]) {
            bNot30 = nEvent != 30;
            if (!fn_800DC784()) {
                fn_800BD83C((u16)pDo->n4, bNot30);
            }
            lbl_802811B8->pE8 = pDo;
        }
        break;
    case 11:
        if (gSession.options.a0[4] && !fn_800DC784()) {
            u16 uSound = pDo->n4;
            nArg = 2;
            if (nPlayer == 0) {
                nArg = 0;
            }
            fn_800BD868(uSound, nArg);
        }
        break;
    case 10:
        if (gSession.options.a0[4]) {
            fn_800BD7E8(pDo->n4);
        }
        break;
    case 7:
        if (fn_800DC784()) {
            fn_800BD7D0(pDo->n4);
        } else {
            fn_800A6DCC(pDo->n4, nEvent != 5);
        }
        break;
    case 4:
        if (fn_8002E8B4(nPlayer)) {
            if (pDo->n4 == 0) {
                fn_800DBA50(nPlayer);
            } else {
                fn_800DB30C(nPlayer, pDo->n4);
            }
        }
        break;
    case 12:
        fn_8006AAB4(nPlayer, pDo->n4);
        lbl_801FA198[nPlayer] = 1;
        break;
    case 13:
        fn_8006ACE0(nPlayer, pDo->n4);
        lbl_801FA1AC[nPlayer] = 1;
        break;
    }
}

// Clear the scripts' per-entry bytes.
void fn_800BD74C(void) {
    u32 i;
    for (i = 0; i < lbl_80282208->n10; i++) {
        lbl_802811B8->pD4[i] = 0;
    }
}

// ---- sounds and music ----------------------------------------------------------------------

// Hand GameEffects a commentary line to play later (u48; fn_800DBDA8 plays it as the GameBreaker
// ends), unless one is waiting already; not in mode 11.
void fn_800BD77C(int nSound) {
    if (Game_GetMode() != 11 && !lbl_80202898.b47) {
        lbl_80202898.u48 = nSound;
        lbl_80202898.b47 = 1;
    }
}

// Hand GameEffects a music to play later (n4F; fn_800DBDA8 plays it as the GameBreaker ends).
void fn_800BD7D0(u8 nMusic) {
    lbl_80202898.b4E = 1;
    lbl_80202898.n4F = nMusic;
}

// The same as fn_800BD77C with GameEffects' second slot (u4C).
void fn_800BD7E8(u16 uSound) {
    if (Game_GetMode() != 11 && !lbl_80202898.b4A) {
        lbl_80202898.u4C = uSound;
        lbl_80202898.b4A = 1;
    }
}

void fn_800BD83C(int nSound, int a) {
    fn_800A7664(0, nSound, a);
}

void fn_800BD868(int nSound, int a) {
    fn_800A7664(2, nSound, a);
}
