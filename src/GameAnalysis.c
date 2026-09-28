// GameAnalysis.c (TW06's name, GameAnalysis_*; TW07 keeps them in AnalysisUtilities.c): a
// player's statistics over the round's holes played so far (putts, pars, birdies and the like,
// fairways hit, greens in regulation, the longest drive and putt) and the pick of the statistic
// tip the HUD shows.

#include "golfer.h"
#include "game.h"

u8  GameAnalysis_IsTipForMode(int nMode, int nTip);
u8  GameAnalysis_AnyoneHasTipStat(u32 nTip);
f32 GameAnalysis_GetTipStat(int nPlayer, u32 nStat);

// Picks the statistic tip the HUD shows next (a UI command queues it with GUI_QueueTip); 14 means
// none. None during a GameMode5 challenge or outside modes 0, 1, 2, 4 and 23; mode 23 shows tip 12
// three times in four. Otherwise a random tip that fits the mode (GameAnalysis_IsTipForMode), has a
// nonzero statistic for some player (GameAnalysis_AnyoneHasTipStat) and has not been shown this
// hole (gTipShown), counting up from a random start; none when no tip qualifies.
int GameAnalysis_PickTip(void) {
    int i;
    u8 bFound;
    int nTip;
    u8 bAny;
    if (PlayNow_IsChallengeRunning() || (Game_GetMode() != 0 && Game_GetMode() != 1 && Game_GetMode() != 2 &&
                          Game_GetMode() != 4 && Game_GetMode() != 23)) {
        return 14;
    }
    if (Game_GetMode() == 23 && Misc_RandFunc(1) % 100 < 75) {
        return 12;
    }
    bAny = 0;
    for (i = 0; i < 14; i++) {
        if (GameAnalysis_IsTipForMode(Game_GetMode(), i) && GameAnalysis_AnyoneHasTipStat(i)
            && !gTipShown[i]) {
            bAny = 1;
        }
    }
    if (!bAny) {
        return 14;
    }
    bFound = 0;
    nTip = Misc_RandFunc(1) % 14;
    while (!bFound) {
        if (GameAnalysis_IsTipForMode(Game_GetMode(), nTip) && GameAnalysis_AnyoneHasTipStat(nTip)
            && !gTipShown[nTip]) {
            bFound = 1;
        } else {
            nTip = (nTip + 1) % 14;
        }
    }
    return nTip;
}

// Whether a statistic tip (GameAnalysis_GetTipStat) fits the game mode: never tip 11 (double bogeys
// or worse); mode 0 all but tip 12; modes 1, 2 and 4 only tips 0 (longest drive), 1 (fairways), 5
// (longest putt) and 13; mode 23 all; other modes none.
u8 GameAnalysis_IsTipForMode(int nMode, int nTip) {
    if (nTip == 11) {
        return 0;
    }
    switch (nMode) {
    case 0:
        return nTip != 12;
    case 1:
    case 2:
    case 4:
        if (nTip != 12 && nTip != 2 && nTip != 3 && nTip != 4 && nTip != 6 && nTip != 7 && nTip != 8 &&
            nTip != 9 && nTip != 10 && nTip != 11) {
            return 1;
        }
        return 0;
    case 23:
        return 1;
    default:
        return 0;
    }
}

// Whether any player in the round has a nonzero statistic for the tip (GameAnalysis_GetTipStat).
// Tip 12 needs none and is always allowed; tip 13 has no statistic, so it never is.
u8 GameAnalysis_AnyoneHasTipStat(u32 nTip) {
    int i;
    if ((int)nTip == 12) {
        return 1;
    }
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (0.0f != GameAnalysis_GetTipStat(i, nTip)) {
            return 1;
        }
    }
    return 0;
}

// The player's putts on the round's holes before the current one; a hole whose putt count is 10 or
// more is skipped.
int GameAnalysis_CountTotalPuttsSoFarThisRound(int nPlayer) {
    int n = 0;
    int h;
    for (h = Game_CurHoleIndex() - 1; h >= 0; h--) {
        if (gpGame->bHoleSelected[h] && gPlayers[nPlayer].nPutts[h] < 10) {
            n += gPlayers[nPlayer].nPutts[h];
        }
    }
    return n;
}

// How many of the round's holes before the current one the player scored at exactly par + nRel (0
// pars, -1 birdies, -2 eagles, 1 bogeys).
int GameAnalysis_CountTotalHoleScores(int nPlayer, int nRel) {
    int h;
    int n = 0;
    for (h = Game_CurHoleIndex() - 1; h >= 0; h--) {
        if (gpGame->bHoleSelected[h] && gPlayers[nPlayer].nStrokes[h] == nRel + Course_GetHolePar(h)) {
            n++;
        }
    }
    return n;
}

// How many of the round's holes before the current one the player scored above par + nRel;
// GameAnalysis_GetTipStat passes 1 (double bogeys or worse).
int GameAnalysis_CountBogeysOrWorse(int nPlayer, int nRel) {
    int h;
    int n = 0;
    for (h = Game_CurHoleIndex() - 1; h >= 0; h--) {
        if (gpGame->bHoleSelected[h] && gPlayers[nPlayer].nStrokes[h] > nRel + Course_GetHolePar(h)) {
            n++;
        }
    }
    return n;
}

// The round's holes before the current one where the player hit the green in regulation
// (Player.bGreenInReg).
int GameAnalysis_CountTotalGIRsSoFarThisRound(int nPlayer) {
    int n = 0;
    int h;
    for (h = Game_CurHoleIndex() - 1; h >= 0; h--) {
        if (gpGame->bHoleSelected[h] && gPlayers[nPlayer].bGreenInReg[h]) {
            n++;
        }
    }
    return n;
}

// The round's holes before the current one where the player hit the fairway (Player.bFairwayHit).
int GameAnalysis_CountTotalFairwaysSoFarThisRound(int nPlayer) {
    int n = 0;
    int h;
    for (h = Game_CurHoleIndex() - 1; h >= 0; h--) {
        if (gpGame->bHoleSelected[h] && gPlayers[nPlayer].bFairwayHit[h]) {
            n++;
        }
    }
    return n;
}

// How many of the round's holes come before the current one (no player: the holes everyone has
// played).
int GameAnalysis_CountCompletedHoles(void) {
    int n = 0;
    int h;
    for (h = Game_CurHoleIndex() - 1; h >= 0; h--) {
        if (gpGame->bHoleSelected[h]) {
            n++;
        }
    }
    return n;
}

// The round's holes before the current one with a putt count below 10 for the player: the holes the
// putts-per-hole statistic divides by.
int GameAnalysis_CountHolesWithPuttsSoFarThisRound(int nPlayer) {
    int n = 0;
    int h;
    for (h = Game_CurHoleIndex() - 1; h >= 0; h--) {
        if (gpGame->bHoleSelected[h] && gPlayers[nPlayer].nPutts[h] < 10) {
            n++;
        }
    }
    return n;
}

// A player's statistic for tip nStat over the round's holes before the current one: 0 the longest
// drive (Player.nLongestDrive), 1 fairways hit as a percentage of the holes played (par 3s count in
// the total: see the EA bug), 2 greens in regulation as a percentage, 3 putts on greens hit in
// regulation, 4 putts per hole, 5 the longest putt (Player.nLongestPutt), 6 pars, 7 birdies, 8
// eagles, 9 albatrosses, 10 bogeys, 11 double bogeys or worse; 0 for any other tip (12, 13).
f32 GameAnalysis_GetTipStat(int nPlayer, u32 nStat) {
    f32 f = 0.0f;
    int nHoles = GameAnalysis_CountCompletedHoles();
    int nPuttHoles = GameAnalysis_CountHolesWithPuttsSoFarThisRound(nPlayer);
    int n;
    int h;
    switch (nStat) {
    case 0:
        f = gPlayers[nPlayer].nLongestDrive;
        break;
    case 1:
        if (nHoles != 0) {
            // EA bug: divides by every hole played, par 3s too, though only a par 4 or 5 can have
            // its fairway hit (Earnings.c sets bFairwayHit only there; the PGA TOUR round statistics
            // count par 4s and 5s only), so after a par 3 hitting every fairway shows below 100%
            f = 100.0f * ((f32)GameAnalysis_CountTotalFairwaysSoFarThisRound(nPlayer) / (f32)nHoles);
        }
        break;
    case 2:
        if (nHoles != 0) {
            f = 100.0f * ((f32)GameAnalysis_CountTotalGIRsSoFarThisRound(nPlayer) / (f32)nHoles);
        }
        break;
    case 3:
        n = 0;
        for (h = Game_CurHoleIndex() - 1; h >= 0; h--) {
            if (gpGame->bHoleSelected[h] && gPlayers[nPlayer].bGreenInReg[h] && gPlayers[nPlayer].nPutts[h]
                < 10) {
                n += gPlayers[nPlayer].nPutts[h];
            }
        }
        f = n;
        break;
    case 4:
        if (nPuttHoles != 0) {
            f = (f32)GameAnalysis_CountTotalPuttsSoFarThisRound(nPlayer) / (f32)nPuttHoles;
        }
        break;
    case 5:
        f = gPlayers[nPlayer].nLongestPutt;
        break;
    case 6:
        f = GameAnalysis_CountTotalHoleScores(nPlayer, 0);
        break;
    case 7:
        f = GameAnalysis_CountTotalHoleScores(nPlayer, -1);
        break;
    case 8:
        f = GameAnalysis_CountTotalHoleScores(nPlayer, -2);
        break;
    case 9:
        f = GameAnalysis_CountTotalHoleScores(nPlayer, -3);
        break;
    case 10:
        f = GameAnalysis_CountTotalHoleScores(nPlayer, 1);
        break;
    case 11:
        f = GameAnalysis_CountBogeysOrWorse(nPlayer, 1);
        break;
    }
    return f;
}
