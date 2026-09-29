// SitDevTrigger.c (EA file, TW06/TW07): own unit, its .data starts at 0x801912D0 (4 pad bytes at 0x801912CC)

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "sitdev.h"
#include "game.h"
#include "game/modes/pgatoursim.h"
#include "game/modes/pgatour.h"

int SitDev_NumEntries(u16* pList, int nCount);
int SitDev_NumEntriesUnused(u16* pList, int nCount);
u32 SitDev_ChooseRandomResponseNoRepeat(u16* pList, int nCount, int nLeft, u32 nPick);

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
    if (gpSitDevData->abPlayed[pAction->nKind]) return 0;
    nCount = SitDev_NumEntries(pAction->aList, 50);
    nLeft = SitDev_NumEntriesUnused(pAction->aList, nCount);
    nSound = SitDev_ChooseRandomResponseNoRepeat(pAction->aList, nCount, nLeft, Misc_RandFunc(1) % nLeft);
    bNot30 = nEvent != 30;
    if (!GameEffects_SkipOtherCommentary()) {
        fn_800BD83C(nSound, bNot30);
        gpSitDevData->abPlayed[pAction->nKind] = 1;
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
        if (pDo == gpSitDevData->pE8 && nEvent != 30) return 0;
        if (gpSitDevData->abPlayed[pDo->nKind]) return 0;
        fn_800BD580(pDo, nPlayer, nEvent);
        gpSitDevData->abPlayed[pDo->nKind] = 1;
        return 1;
    }
    for (i = 0; i < 14; i++) {
        anCount[i] = 0;
    }
    // Sort the entries not played yet by kind.
    for (i = 0; i < 50; i++) {
        if (pAction->aList[i] != 0xFFF0) {
            nKind = lbl_80282208->p1C[pAction->aList[i] & 0x7FFF].nKind;
            if (!gpSitDevData->abPlayed[nKind]) {
                aaIndex[nKind][anCount[nKind]] = i;
                anCount[(u32)nKind]++;      // fake match: a second spelling of the index, not CSE'd
            }
        }
    }
    for (nKind = 0; nKind < 14; nKind++) {
        if (anCount[nKind] == 1) {
            if (&lbl_80282208->p1C[pAction->aList[aaIndex[nKind][0]]] != gpSitDevData->pE8) {
                fn_800BD580(&lbl_80282208->p1C[pAction->aList[aaIndex[nKind][0]]], nPlayer, nEvent);
                bPlayed = 1;
                gpSitDevData->abPlayed[nKind] = 1;
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
            if (pDo == gpSitDevData->pE8) {
                nPick++;
                if (nPick == anCount[nKind]) {
                    nPick = 0;
                }
                nEntry = aaIndex[nKind][nPick];
                pDo =&lbl_80282208->p1C[pAction->aList[nEntry] & 0x7FFF];
            }
            fn_800BD580(pDo, nPlayer, nEvent);
            gpSitDevData->abPlayed[nKind] = 1;
            pAction->aList[nEntry] |= 0x8000;
            bPlayed = 1;
        }
    }
    return bPlayed;
}

// Whether an action is held back: at situations 21 and 22 in modes with odd holes (gpGame's
// bCustomRound, bRandom18, bDream18 or nRegionalRound), or while PlayNow_IsChallengeRunning is set
// (at 22 only for a ball off the tee); for events 20 and 31 while the GameBreaker is up (not at
// situation 2); and commentary (kinds 1 and 2) during a replay, in modes 6..8, where
// GM_Currently_SkillZoneMode says so, and in mode 11.
u8 fn_800BD3F8(SitDevAction* pAction, int nSit, int nPlayer, u8 nEvent) {
    int nMode = Game_GetMode();
    if (nSit == 22 || nSit == 21) {
        if (gpGame->bCustomRound || gpGame->bRandom18 || gpGame->bDream18 || gpGame->nRegionalRound) return 1;
        if (PlayNow_IsChallengeRunning()) {
            if (nSit == 21) return 1;
            if (nSit == 22 && gPlayers[nPlayer].ball.nLie != 0) return 1;
        }
    }
    if (gGameEffects.bGameBreaker && (nEvent == 20 || nEvent == 31) && nSit != 2) return 1;
    if (pAction->nKind != 1 && pAction->nKind != 2) return 0;
    if (gSession.bReplay || (u32)(nMode - 6) <= 2 || GM_Currently_SkillZoneMode()) return 1;
    if (!PlayNow_IsChallengeRunning() && nMode != 11) return 0;
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
            if (!GameEffects_SkipOtherCommentary()) {
                fn_800BD83C((u16)pDo->n4, bNot30);
            }
            gpSitDevData->pE8 = pDo;
        }
        break;
    case 11:
        if (gSession.options.a0[4] && !GameEffects_SkipOtherCommentary()) {
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
        if (GameEffects_SkipOtherCommentary()) {
            fn_800BD7D0(pDo->n4);
        } else {
            Gaud_InitCrowdReactionSound(pDo->n4, nEvent != 5);
        }
        break;
    case 4:
        if (fn_8002E8B4(nPlayer)) {
            if (pDo->n4 == 0) {
                GameEffects_InFlightGameBreakerTrigger(nPlayer);
            } else {
                GameEffects_ScriptedGameBreakerTrigger(nPlayer, pDo->n4);
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
        gpSitDevData->pD4[i] = 0;
    }
}

// ---- sounds and music ----------------------------------------------------------------------

// Hand GameEffects a commentary line to play later (u48; GameEffects_EndGameBreaker plays it as the
// GameBreaker ends), unless one is waiting already; not in mode 11.
void fn_800BD77C(int nSound) {
    if (Game_GetMode() != 11 && !gGameEffects.b47) {
        gGameEffects.u48 = nSound;
        gGameEffects.b47 = 1;
    }
}

// Hand GameEffects a music to play later (n4F; GameEffects_EndGameBreaker plays it as the GameBreaker ends).
void fn_800BD7D0(u8 nMusic) {
    gGameEffects.bCrowdReactionSet = 1;
    gGameEffects.nCrowdReaction = nMusic;
}

// The same as fn_800BD77C with GameEffects' second slot (u4C).
void fn_800BD7E8(u16 uSound) {
    if (Game_GetMode() != 11 && !gGameEffects.b4A) {
        gGameEffects.u4C = uSound;
        gGameEffects.b4A = 1;
    }
}

void fn_800BD83C(int nSound, int a) {
    Gaud_StartComment(0, nSound, a);
}

void fn_800BD868(int nSound, int a) {
    Gaud_StartComment(2, nSound, a);
}
