// GameModeBattle.c (TW06's and TW07's GameModeBattle): game mode 25, two-player match play for
// clubs. The winner of a hole may take a club out of the other's bag, or put back one of their own
// they have lost (GameModeBattle_CanAddClub); the bags are restored when the mode shuts down. The
// 5-iron, the sand wedge and the putter can never be taken; a player left with one club or fewer
// that can be taken loses as soon as the other wins a hole.

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"

u32 gBattleStartBagMask[5];             // per player: the bag (golfer.uBagMask) at the round's start
s32 gBattleStartClubCount[5];           // per player: how many clubs that bag held
s32 gBattleClubLostThisHole[5] = {26, 26, 26, 26, 26}; // per player: the club taken after the
                                                       // last hole (26 = none)
s32 gBattleHoleWinner = 5;              // the player who won the last hole (5 = nobody)
u8  gBattleShowClubAddRemove;           // the end of the hole offers the winner a club

void GameModeBattle_Shutdown(void);
void GameModeBattle_StartGamePostData(void);
u8   GameModeBattle_GameFinished(u8 bCheck);
void GameModeBattle_EndHole(void);
void GameModeBattle_EndGame(void);
u8   GameModeBattle_ClubIsRequired(int nClub);
int  GameModeBattle_NumRemovableClubsLeft(int nPlayer);
void GameModeBattle_SaveClubSetup(void);
void GameModeBattle_RestoreClubSetup(void);

// Game mode 25's setup (GM_SetModeType, GameRound.c): installs the mode's callbacks (match play's
// next golfer, honors, hole-finished and playoff; Battle's own game-finished, end-hole, end-game,
// shutdown and round start), lets a CPU concede a hole, no mulligans (nMulligans 0) and one view.
void GameModeBattle_Init(void) {
    gpGame->pfnInit = GameModeBattle_Init;
    gpGame->pfnShutdown = GameModeBattle_Shutdown;
    gpGame->pfnSetupNextGolfer = GameModeMatch_SetupNextGolfer;
    gpGame->pfnGetHonors = GameModeMatch_GetHonors;
    gpGame->pfnHoleFinished = GameModeMatch_HoleFinished;
    gpGame->pfnGameFinished = GameModeBattle_GameFinished;
    gpGame->pfnGoToPlayoff = GameModeMatch_GoToPlayoff;
    gpGame->pfnEndHole = GameModeBattle_EndHole;
    gpGame->pfnEndGame = GameModeBattle_EndGame;
    gpGame->pfnStartGamePostData = GameModeBattle_StartGamePostData;
    gpGame->bAIConcedes = 1;
    gpGame->nScoringType = 1;
    gpGame->nMulligans = 0;
    gpGame->nC = 2;
    gpGame->n10 = 2;
    gpGame->nDC = 0;
    gSession.nSplitScreen = 0;
}

// pfnShutdown: every player's bag goes back to how it was at the round's start
// (GameModeBattle_RestoreClubSetup), so the clubs taken in the match are not lost.
void GameModeBattle_Shutdown(void) {
    GameModeBattle_RestoreClubSetup();
}

// pfnStartGamePostData, when the round starts after the players are set up: saves every player's
// bag (GameModeBattle_SaveClubSetup) to restore it at shutdown and to know which clubs can be won
// back.
void GameModeBattle_StartGamePostData(void) {
    GameModeBattle_SaveClubSetup();
}

// The usual match-play end, or a player with at most one club
// left to lose who has just lost a hole.
u8 GameModeBattle_GameFinished(u8 bCheck) {
    if (GameModeMatch_GameFinished(bCheck)) {
        return 1;
    }
    if ((GameModeBattle_NumRemovableClubsLeft(0) <= 1 && gPlayers[1].nModePoints[Game_CurHoleIndex()] != 0) ||
        (GameModeBattle_NumRemovableClubsLeft(1) <= 1 && gPlayers[0].nModePoints[Game_CurHoleIndex()] != 0)) {
        return 1;
    }
    return 0;
}

// pfnEndHole: a player who holed out in fewer strokes than the other wins the hole: 1 in their
// nModePoints for it, one more nHolesWon, and they become the hole's winner (gBattleHoleWinner; 5
// when the hole is halved). A won hole lets the winner take a club (gBattleShowClubAddRemove)
// unless the match is now over. The clubs lost on the hole before are forgotten
// (gBattleClubLostThisHole back to 26).
void GameModeBattle_EndHole(void) {
    gBattleShowClubAddRemove = 0;
    gBattleHoleWinner = 5;
    if (Player_IsHoled(0) &&
        gPlayers[0].nStrokes[Game_CurHoleIndex()] < gPlayers[1].nStrokes[Game_CurHoleIndex()]) {
        gPlayers[0].nModePoints[Game_CurHoleIndex()] = 1;
        gBattleShowClubAddRemove = 1;
        gBattleHoleWinner = 0;
        gPlayers[0].nHolesWon++;
    }
    if (Player_IsHoled(1) &&
        gPlayers[1].nStrokes[Game_CurHoleIndex()] < gPlayers[0].nStrokes[Game_CurHoleIndex()]) {
        gPlayers[1].nModePoints[Game_CurHoleIndex()] = 1;
        gBattleShowClubAddRemove = 1;
        gBattleHoleWinner = 1;
        gPlayers[1].nHolesWon++;
    }
    if (GameModeBattle_GameFinished(1)) {
        gBattleShowClubAddRemove = 0;
    }
    gBattleClubLostThisHole[0] = 26;
    gBattleClubLostThisHole[1] = 26;
    gBattleClubLostThisHole[2] = 26;
    gBattleClubLostThisHole[3] = 26;
    gBattleClubLostThisHole[4] = 26;
}

// pfnEndGame, after a full round that is not a Play Now challenge: the winner is the player whose
// opponent had one club or fewer left to take and just lost a hole (margin 0), else the one with
// more holes won (margin the difference; a tie goes to player 1, margin 0). A human winner with an
// active profile has the game counted as won (EASBio_SetCurrentGameWon) and, when
// GM_Earnings_GetStrokeWinnings pays anything, gets the money (GUI message 0x6B with the base
// prize, GM_Earnings_AwardMoney, and money.n14).
void GameModeBattle_EndGame(void) {
    int nPrize;
    int nWinner;
    int nLoser;
    int nMargin;
    int nMoney;
    int nProfile;
    if (GM_FullRoundOfGolf()) {
        switch (PlayNow_IsChallengeRunning()) {
        case 0:
            break;
        default:
            return;
        }
        if (GameModeBattle_NumRemovableClubsLeft(0) <= 1 &&
            gPlayers[1].nModePoints[Game_CurHoleIndex()] != 0) {
            nWinner = 1;
            nLoser = 0;
            nMargin = 0;
        } else if (GameModeBattle_NumRemovableClubsLeft(1) <= 1 &&
                   gPlayers[0].nModePoints[Game_CurHoleIndex()] != 0) {
            nWinner = 0;
            nLoser = 1;
            nMargin = 0;
        } else if (gPlayers[0].nHolesWon > gPlayers[1].nHolesWon) {
            nMargin = gPlayers[0].nHolesWon - gPlayers[1].nHolesWon;
            nWinner = 0;
            nLoser = 1;
        } else {
            nMargin = gPlayers[1].nHolesWon - gPlayers[0].nHolesWon;
            nWinner = 1;
            nLoser = 0;
        }
        nMoney = GM_Earnings_GetStrokeWinnings(nWinner, nLoser, nMargin, &nPrize);
        if (!Player_IsCPU(nWinner)) {
            nProfile = gPlayers[nWinner].nIndex;
            if (gpSaveData[nProfile].bActive) {
                EASBio_SetCurrentGameWon(1);
                if (nMoney) {
                    GUI_QueueMessage(0, 0x6B, nPrize, nProfile);
                    GM_Earnings_AwardMoney(nWinner, nMoney, 0);
                    gPlayers[nWinner].money.n14 += nMoney;
                }
            }
        }
    }
}

// Whether club nClub can never be taken: the 5-iron (13), the sand wedge (21) and the putter (25).
u8 GameModeBattle_ClubIsRequired(int nClub) {
    u8 bRequired = 0;
    if ((1 << nClub) & 0x2202000) {
        bRequired = 1;
    }
    return bRequired;
}

// How many clubs in nPlayer's bag can still be taken (every club in it but the three
// GameModeBattle_ClubIsRequired keeps).
int GameModeBattle_NumRemovableClubsLeft(int nPlayer) {
    int i;
    int n = 0;
    for (i = 0; i < 26; i++) {
        if (Bag_HasClub(nPlayer, i) && !GameModeBattle_ClubIsRequired(i)) {
            n++;
        }
    }
    return n;
}

// Takes club nClub out of nPlayer's bag and records it as the club they lost this hole
// (gBattleClubLostThisHole, even when it was not in the bag); gives whether it was in the bag. A
// required club (GameModeBattle_ClubIsRequired) is refused: 0 and nothing changes.
u8 GameModeBattle_RemoveClub(int nPlayer, int nClub) {
    u8 bRemoved;
    if (GameModeBattle_ClubIsRequired(nClub)) {
        return 0;
    }
    bRemoved = Bag_RemoveClub(nPlayer, nClub);
    gBattleClubLostThisHole[nPlayer] = nClub;
    return bRemoved;
}

// Puts club nClub in nPlayer's bag (Bag_AddClub): the end-of-hole choice to win back a club instead
// of taking one (GameModeBattle_CanAddClub says which).
void GameModeBattle_AddClub(int nPlayer, int nClub) {
    Bag_AddClub(nPlayer, nClub);
}

// Saves every player's bag (golfer.uBagMask, into gBattleStartBagMask) and how many clubs it holds
// (gBattleStartClubCount), at the round's start.
void GameModeBattle_SaveClubSetup(void) {
    int i;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        *(u32*)((u8*)gBattleStartBagMask + i * sizeof(u32)) = PLAYER(i)->golfer.uBagMask;
        *(s32*)((u8*)gBattleStartClubCount + i * sizeof(s32)) = Bag_CountClubs(i);
    }
}

// Puts every player's bag back as GameModeBattle_SaveClubSetup saved it (gBattleStartBagMask).
void GameModeBattle_RestoreClubSetup(void) {
    int i;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        PLAYER(i)->golfer.uBagMask = *(u32*)((u8*)gBattleStartBagMask + i * sizeof(u32));
    }
}

// How many clubs nPlayer's bag held at the round's start (gBattleStartClubCount; UI command
// IG_vSwapDiscReloadHole shows it).
s32 GameModeBattle_GetNumberStartingClubs(int nPlayer) {
    return gBattleStartClubCount[nPlayer];
}

// Whether club nClub may be put back in nPlayer's bag: they started the round with it
// (gBattleStartBagMask) and no longer have it. 1 or 0.
int GameModeBattle_CanAddClub(int nPlayer, int nClub) {
    u32 uBit = 1 << nClub;
    int bCan = 0;
    if (Bag_HasClub(nPlayer, nClub)) {
        return 0;
    }
    if (uBit & gBattleStartBagMask[nPlayer]) {
        bCan = 1;
    }
    return bCan;
}

// The club taken from nPlayer after the last hole (26 = none; GameModeBattle_EndHole clears it when
// the next hole ends). The situation state (SitDevStateVector.c, value 93) reads it.
s32 GameModeBattle_GetClubLostOnLastHole(int nPlayer) {
    return gBattleClubLostThisHole[nPlayer];
}

// Whether the end of the hole shows the club choice: a player won the hole outright and the match
// goes on (gBattleShowClubAddRemove, set by GameModeBattle_EndHole).
u8 GameModeBattle_ShowEndOfHole_ClubAddRemove_UI(void) {
    return gBattleShowClubAddRemove;
}

// The player who won the last hole outright (0 or 1), 5 when it was halved (gBattleHoleWinner, set
// by GameModeBattle_EndHole).
s32 GameModeBattle_GetHoleWinner(void) {
    return gBattleHoleWinner;
}
