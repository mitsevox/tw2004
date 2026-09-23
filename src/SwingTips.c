// SwingTips.c (our name): the tips shown as a swing starts (fn_800D1DAC, from
// STATEFUNC_SwingInit). Each test (wind, lie, slope, the golfer's attributes, par and score) picks
// a tip; the first time a save profile meets one it gets the full tip (and a flag in the save),
// later a short random one. Not yet decompiled beyond the two tests below.

#include "game.h"

u8 fn_800D1698(int nPlayer);
u8 fn_800D19F8(int nPlayer);
u8 fn_800D1A34(int nPlayer);
u8 fn_800D1A70(int nPlayer);
u8 fn_800D1AA8(int nPlayer);
u8 fn_800D1AE0(void);
u8 fn_800D1C38(int nPlayer);
u8 fn_800D1D30(void);

// A tip test: a shot other than a putt, with the wind's speed over 6.
u8 fn_800D1698(int nPlayer) {
    if (gPlayers[nPlayer].nShotKind == 0) return 0;
    return Wind_Get(NULL) > 6.0f;
}

// A tip test: the ball lies in lie 3, 4 or 5.
u8 fn_800D19F8(int nPlayer) {
    if (gPlayers[nPlayer].ball.nLie == 3 || gPlayers[nPlayer].ball.nLie == 4 ||
        gPlayers[nPlayer].ball.nLie == 5) {
        return 1;
    }
    return 0;
}

// A tip test: the ball lies in lie 6, 7 or 8.
u8 fn_800D1A34(int nPlayer) {
    if (gPlayers[nPlayer].ball.nLie == 6 || gPlayers[nPlayer].ball.nLie == 7 ||
        gPlayers[nPlayer].ball.nLie == 8) {
        return 1;
    }
    return 0;
}

// A tip test: the target is more than 17 feet below the ball.
u8 fn_800D1A70(int nPlayer) {
    return 3.0f * (gPlayers[nPlayer].vTargetCopy[1] - gPlayers[nPlayer].vBall[1]) < -17.0f;
}

// A tip test: the target is more than 17 feet above the ball.
u8 fn_800D1AA8(int nPlayer) {
    return 3.0f * (gPlayers[nPlayer].vTargetCopy[1] - gPlayers[nPlayer].vBall[1]) > 17.0f;
}

// A tip test: lbl_802811F0's flag 0x2 is set.
u8 fn_800D1AE0(void) {
    return fn_80035574() != 0;
}

// A tip test: the club reaches past the pin (the longest the player can hit it is more than the
// ball's distance from the pin).
u8 fn_800D1C38(int nPlayer) {
    f32 fMax = AI_MaxDistance(nPlayer, gPlayers[nPlayer].nShotKind, gPlayers[nPlayer].nClub);
    return fMax > fn_800D0478(nPlayer);
}

// A tip test that never fires.
u8 fn_800D1D30(void) {
    return 0;
}
