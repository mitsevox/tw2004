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

void GameEffects_SetPostGBNegativeCommentary(int nSound);
void GameEffects_SetPostGBCrowdLevel(u8 nMusic);
void GameEffects_SetPostGBCommentary(u16 uSound);
void Gaud_StartPlaylist2Comment(int nSound, int a);

// ---- running a script's actions ------------------------------------------------------------

u8   SitDev_InvokeCommentaryBank(SitDevAction* pAction, u8 nEvent);
u8   SitDev_InvokeAction(SitDevAction* pAction, int nPlayer, u8 nEvent);
u8   SitDev_SuppressAction(SitDevAction* pAction, int nSit, int nPlayer, u8 nEvent);
void SitDev_TriggerResponse(SitDevEntry8* pDo, int nPlayer, u8 nEvent);

// Runs a matched situation's actions for the player (up to four, to the first 0xFFF0): each fires
// by its percent chance unless SitDev_SuppressAction holds it back, a sound action through
// SitDev_InvokeCommentaryBank, any other through SitDev_InvokeAction. nSit is the situation's
// script file (the top five bits of b2). Afterwards, when the prediction flags were set for this
// run (gSitDevPredictionPending) and the last action tried played nothing, the prediction counts as not voiced
// (gSitDevPredictionVoiced cleared); gSitDevPredictionPending is cleared either way.
void SitDev_InvokeMultipleActions(SitDevEntry* pEntry, int nSit, int nPlayer, u8 nEvent) {
    int i;
    SitDevAction* pAction;
    u8 bPlayed = 0;
    for (i = 0; i < 4; i++) {
        if (pEntry->aActions[i] == 0xFFF0) break;
        pAction = &gpSitDevScripts->p18[pEntry->aActions[i]];
        if (pAction->nChance > Misc_RandFunc(1) % 100
            && !SitDev_SuppressAction(pAction, nSit, nPlayer, nEvent)) {
            if (pAction->bSound) {
                bPlayed = SitDev_InvokeCommentaryBank(pAction, nEvent);
            } else {
                bPlayed = SitDev_InvokeAction(pAction, nPlayer, nEvent);
            }
        }
    }
    if (gSitDevPredictionPending && !bPlayed) {
        gSitDevPredictionVoiced = 0;
    }
    gSitDevPredictionPending = 0;
}

// A sound action: unless one of its kind (nKind) has played, draws a line from its list
// (SitDev_NumEntries, SitDev_NumEntriesUnused, SitDev_ChooseRandomResponseNoRepeat) and plays it as
// regular commentary (play mode 1; 0 at situation event 30, the flyover). While the GameBreaker
// holds commentary back (GameEffects_SkipOtherCommentary) the drawn line is not played; at
// situation event 8 it is kept for the end of a scripted GameBreaker that fails. Returns whether it
// played.
u8 SitDev_InvokeCommentaryBank(SitDevAction* pAction, u8 nEvent) {
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
        Gaud_StartRegularComment(nSound, bNot30);
        gpSitDevData->abPlayed[pAction->nKind] = 1;
        return 1;
    }
    if (nEvent == 8) {
        GameEffects_SetPostGBNegativeCommentary(nSound);
    }
    return 0;
}

// A response action (bSound clear): its list names responses (p1C), and each plays through
// SitDev_TriggerResponse. A single response plays unless its kind has played or it is the last line
// played (pE8; allowed at situation event 30). Otherwise, for each kind not played yet: a kind with
// one response plays it unless it is the last line played; a kind with more draws one at random,
// skipping those already drawn (bit 15; when every one is, the marks are cleared) and stepping one
// on past the last line played, and marks it drawn. Returns whether anything played.
u8 SitDev_InvokeAction(SitDevAction* pAction, int nPlayer, u8 nEvent) {
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
        pDo = &gpSitDevScripts->p1C[pAction->aList[0]];
        if (pDo == gpSitDevData->pE8 && nEvent != 30) return 0;
        if (gpSitDevData->abPlayed[pDo->nKind]) return 0;
        SitDev_TriggerResponse(pDo, nPlayer, nEvent);
        gpSitDevData->abPlayed[pDo->nKind] = 1;
        return 1;
    }
    for (i = 0; i < 14; i++) {
        anCount[i] = 0;
    }
    // Sort the entries not played yet by kind.
    for (i = 0; i < 50; i++) {
        if (pAction->aList[i] != 0xFFF0) {
            nKind = gpSitDevScripts->p1C[pAction->aList[i] & 0x7FFF].nKind;
            if (!gpSitDevData->abPlayed[nKind]) {
                aaIndex[nKind][anCount[nKind]] = i;
                anCount[(u32)nKind]++;      // fake match: a second spelling of the index, not CSE'd
            }
        }
    }
    for (nKind = 0; nKind < 14; nKind++) {
        if (anCount[nKind] == 1) {
            if (&gpSitDevScripts->p1C[pAction->aList[aaIndex[nKind][0]]] != gpSitDevData->pE8) {
                SitDev_TriggerResponse(&gpSitDevScripts->p1C[pAction->aList[aaIndex[nKind][0]]], nPlayer,
                                       nEvent);
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
            pDo = &gpSitDevScripts->p1C[pAction->aList[nEntry]];
            if (pDo == gpSitDevData->pE8) {
                nPick++;
                if (nPick == anCount[nKind]) {
                    nPick = 0;
                }
                nEntry = aaIndex[nKind][nPick];
                pDo =&gpSitDevScripts->p1C[pAction->aList[nEntry] & 0x7FFF];
            }
            SitDev_TriggerResponse(pDo, nPlayer, nEvent);
            gpSitDevData->abPlayed[nKind] = 1;
            pAction->aList[nEntry] |= 0x8000;
            bPlayed = 1;
        }
    }
    return bPlayed;
}

// Whether an action is held back. Script files 21 and 22: in rounds with custom holes (gpGame
// bCustomRound, bRandom18, bDream18, nRegionalRound), and during a challenge
// (PlayNow_IsChallengeRunning; file 22 only for a ball not on lie 0). Situation events 20 and 31
// while a GameBreaker is up, except script file 2. Commentary actions (kinds 1 and 2) during a
// replay, in modes 6 to 8, when GM_Currently_SkillZoneMode says so, and in mode 11.
u8 SitDev_SuppressAction(SitDevAction* pAction, int nSit, int nPlayer, u8 nEvent) {
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

// Does one response, by its kind: 1 a line of regular commentary when commentary is on (options
// a0[4]): played unless the GameBreaker holds commentary back, and remembered as the last line
// (pE8) either way; 11 a line of playlist 2 (play mode 0 for player 0, else 2) when commentary is
// on and not held back; 10 a line kept for a GameBreaker that works, when commentary is on; 7 a
// crowd reaction (Gaud_InitCrowdReactionSound), kept for the GameBreaker's end while it holds
// commentary back; 4 a GameBreaker for a player whose controller is 8 or lower (fn_8002E8B4): in
// flight for argument 0, else scripted with it; 12 and 13 set the player's shot emotion
// (fn_8006AAB4) or its predicted emotion (fn_8006ACE0) and note that the scripts did.
void SitDev_TriggerResponse(SitDevEntry8* pDo, int nPlayer, u8 nEvent) {
    int bNot30;
    int nArg;
    switch (pDo->nKind) {
    case 1:
        if (gSession.options.a0[4]) {
            bNot30 = nEvent != 30;
            if (!GameEffects_SkipOtherCommentary()) {
                Gaud_StartRegularComment((u16)pDo->n4, bNot30);
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
            Gaud_StartPlaylist2Comment(uSound, nArg);
        }
        break;
    case 10:
        if (gSession.options.a0[4]) {
            GameEffects_SetPostGBCommentary(pDo->n4);
        }
        break;
    case 7:
        if (GameEffects_SkipOtherCommentary()) {
            GameEffects_SetPostGBCrowdLevel(pDo->n4);
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
        gSitDevEmotionSet[nPlayer] = 1;
        break;
    case 13:
        fn_8006ACE0(nPlayer, pDo->n4);
        gSitDevPredictedEmotionSet[nPlayer] = 1;
        break;
    }
}

// Clears the group flags (SitDevData.pD4, one byte per group, n10 of them), so every situation
// group may fire again: at each shot set-up (situation event 3) and when the scripts load.
void SitDev_ClearGroupFlags(void) {
    u32 i;
    for (i = 0; i < gpSitDevScripts->n10; i++) {
        gpSitDevData->pD4[i] = 0;
    }
}

// ---- sounds and music ----------------------------------------------------------------------

// Keeps a commentary line for the end of a scripted GameBreaker the shot did not achieve
// (gGameEffects.u48; GameEffects_EndGameBreaker plays it), unless one is kept already; not in mode
// 11.
void GameEffects_SetPostGBNegativeCommentary(int nSound) {
    if (Game_GetMode() != 11 && !gGameEffects.b47) {
        gGameEffects.u48 = nSound;
        gGameEffects.b47 = 1;
    }
}

// Keeps a crowd reaction (gGameEffects.nCrowdReaction) for the end of a scripted GameBreaker the
// shot did not achieve (GameEffects_EndGameBreaker).
void GameEffects_SetPostGBCrowdLevel(u8 nMusic) {
    gGameEffects.bCrowdReactionSet = 1;
    gGameEffects.nCrowdReaction = nMusic;
}

// Keeps a commentary line for the end of a GameBreaker that works (gGameEffects.u4C: a predicted
// one, or a scripted one the shot achieved; GameEffects_EndGameBreaker plays it), unless one is
// kept already; not in mode 11.
void GameEffects_SetPostGBCommentary(u16 uSound) {
    if (Game_GetMode() != 11 && !gGameEffects.b4A) {
        gGameEffects.u4C = uSound;
        gGameEffects.b4A = 1;
    }
}

// Plays commentary line nSound of playlist 0, the regular commentary (Gaud_StartComment; a is its
// play mode).
void Gaud_StartRegularComment(int nSound, int a) {
    Gaud_StartComment(0, nSound, a);
}

// Plays commentary line nSound of playlist 2 (Gaud_StartComment; a is its play mode): response kind
// 11.
void Gaud_StartPlaylist2Comment(int nSound, int a) {
    Gaud_StartComment(2, nSound, a);
}
