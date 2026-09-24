// GameAnalysis.c (TW06's name, GameAnalysis_*): round statistics per player (putts, pars, birdies,
// two per-hole flags, probably fairways and greens) and the pick of a tip quoting one of them.

#include "golfer.h"
#include "game.h"

u8  fn_800E6020(int nMode, int nTip);
u8  fn_800E60E4(u32 nTip);
f32 fn_800E6578(int nPlayer, u32 nStat);

// Picks a statistic tip to show: 14 means none. Only in modes 0, 1, 2, 4 and 23 (which shows
// tip 12 three times in four); otherwise a random tip that fits the mode, has a nonzero statistic
// for someone and has not been shown yet.
int fn_800E5E54(void) {
    int i;
    u8 bFound;
    int nTip;
    u8 bAny;
    if (fn_800EC550() || (Game_GetMode() != 0 && Game_GetMode() != 1 && Game_GetMode() != 2 &&
                          Game_GetMode() != 4 && Game_GetMode() != 23)) {
        return 14;
    }
    if (Game_GetMode() == 23 && Rand_Next(1) % 100 < 75) {
        return 12;
    }
    bAny = 0;
    for (i = 0; i < 14; i++) {
        if (fn_800E6020(Game_GetMode(), i) && fn_800E60E4(i) && !lbl_80203138[i]) {
            bAny = 1;
        }
    }
    if (!bAny) {
        return 14;
    }
    bFound = 0;
    nTip = Rand_Next(1) % 14;
    while (!bFound) {
        if (fn_800E6020(Game_GetMode(), nTip) && fn_800E60E4(nTip) && !lbl_80203138[nTip]) {
            bFound = 1;
        } else {
            nTip = (nTip + 1) % 14;
        }
    }
    return nTip;
}

// Whether a tip fits a game mode: never tip 11; mode 0 all but tip 12; modes 1, 2 and 4 only tips
// 0, 1, 5 and 13; mode 23 all; other modes none.
u8 fn_800E6020(int nMode, int nTip) {
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

// Whether any player's statistic for this tip is nonzero (tip 12 needs none).
u8 fn_800E60E4(u32 nTip) {
    int i;
    if ((int)nTip == 12) {
        return 1;
    }
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (0.0f != fn_800E6578(i, nTip)) {
            return 1;
        }
    }
    return 0;
}

// TW06: GameAnalysis_CountTotalPuttsSoFarThisRound (by what it does).
int fn_800E6170(int nPlayer) {
    int n = 0;
    int h;
    for (h = Game_CurHoleIndex() - 1; h >= 0; h--) {
        if (gpGame->bHoleSelected[h] && gPlayers[nPlayer].nPutts[h] < 10) {
            n += gPlayers[nPlayer].nPutts[h];
        }
    }
    return n;
}

// TW06: GameAnalysis_CountTotalHoleScores. The holes played so far scored at par + nRel.
int fn_800E6204(int nPlayer, int nRel) {
    int h;
    int n = 0;
    for (h = Game_CurHoleIndex() - 1; h >= 0; h--) {
        if (gpGame->bHoleSelected[h] && gPlayers[nPlayer].nStrokes[h] == nRel + fn_800D2AD8(h)) {
            n++;
        }
    }
    return n;
}

// TW06: GameAnalysis_CountBogeysOrWorse. The holes played so far scored above par + nRel.
int fn_800E62B4(int nPlayer, int nRel) {
    int h;
    int n = 0;
    for (h = Game_CurHoleIndex() - 1; h >= 0; h--) {
        if (gpGame->bHoleSelected[h] && gPlayers[nPlayer].nStrokes[h] > nRel + fn_800D2AD8(h)) {
            n++;
        }
    }
    return n;
}

// Holes so far with flag b2F6 (probably greens in regulation).
int fn_800E6364(int nPlayer) {
    int n = 0;
    int h;
    for (h = Game_CurHoleIndex() - 1; h >= 0; h--) {
        if (gpGame->bHoleSelected[h] && gPlayers[nPlayer].b2F6[h]) {
            n++;
        }
    }
    return n;
}

// Holes so far with flag b2E4 (probably fairways hit).
int fn_800E63F4(int nPlayer) {
    int n = 0;
    int h;
    for (h = Game_CurHoleIndex() - 1; h >= 0; h--) {
        if (gpGame->bHoleSelected[h] && gPlayers[nPlayer].b2E4[h]) {
            n++;
        }
    }
    return n;
}

// TW06: GameAnalysis_CountCompletedHoles.
int fn_800E6484(void) {
    int n = 0;
    int h;
    for (h = Game_CurHoleIndex() - 1; h >= 0; h--) {
        if (gpGame->bHoleSelected[h]) {
            n++;
        }
    }
    return n;
}

// The holes so far with a putt count (below 10).
int fn_800E64E4(int nPlayer) {
    int n = 0;
    int h;
    for (h = Game_CurHoleIndex() - 1; h >= 0; h--) {
        if (gpGame->bHoleSelected[h] && gPlayers[nPlayer].nPutts[h] < 10) {
            n++;
        }
    }
    return n;
}

// A player's statistic for tip nStat: 0 n2DC, 1 the b2E4 percentage, 2 the b2F6 percentage, 3 putts
// on b2F6 holes, 4 putts per hole, 5 n2E0, 6..9 pars, birdies, eagles, albatrosses, 10 bogeys,
// 11 double bogeys or worse.
f32 fn_800E6578(int nPlayer, u32 nStat) {
    f32 f = 0.0f;
    int nHoles = fn_800E6484();
    int nPuttHoles = fn_800E64E4(nPlayer);
    int n;
    int h;
    switch (nStat) {
    case 0:
        f = gPlayers[nPlayer].n2DC;
        break;
    case 1:
        if (nHoles != 0) {
            f = 100.0f * ((f32)fn_800E63F4(nPlayer) / (f32)nHoles);
        }
        break;
    case 2:
        if (nHoles != 0) {
            f = 100.0f * ((f32)fn_800E6364(nPlayer) / (f32)nHoles);
        }
        break;
    case 3:
        n = 0;
        for (h = Game_CurHoleIndex() - 1; h >= 0; h--) {
            if (gpGame->bHoleSelected[h] && gPlayers[nPlayer].b2F6[h] && gPlayers[nPlayer].nPutts[h] < 10) {
                n += gPlayers[nPlayer].nPutts[h];
            }
        }
        f = n;
        break;
    case 4:
        if (nPuttHoles != 0) {
            f = (f32)fn_800E6170(nPlayer) / (f32)nPuttHoles;
        }
        break;
    case 5:
        f = gPlayers[nPlayer].n2E0;
        break;
    case 6:
        f = fn_800E6204(nPlayer, 0);
        break;
    case 7:
        f = fn_800E6204(nPlayer, -1);
        break;
    case 8:
        f = fn_800E6204(nPlayer, -2);
        break;
    case 9:
        f = fn_800E6204(nPlayer, -3);
        break;
    case 10:
        f = fn_800E6204(nPlayer, 1);
        break;
    case 11:
        f = fn_800E62B4(nPlayer, 1);
        break;
    }
    return f;
}
