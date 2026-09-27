// Golfer.c: the golfers' luck: the "1 in gLuckOdds[n]" odds per player (Luck_*) and the roll
// against them (Golfer_IsLucky). No assert names this file; "Golfer.c" is our name, kept from
// before the split. CodeWarrior GC/2.5, -O4,p.
// Split 2026-09-27: the code before 0x8002D8A8 is ai_brain.c, Code8002BBB0.c and
// Code8002C984.c; each part, compiled alone, reproduces its own .sdata2 pool byte for byte.
// This part (0x8002D8A8-0x8002DB80) owns .sdata 0x802810B0-0x802810B8 (gLuckOdds) and .sdata2
// 0x80282EE8-0x80282EF0.

#include "golfer.h"
#include "endian.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "frontend/fe.h"

void Luck_ResetAllOdds(void);
void Luck_ResetOdds(int nPlayer);

s8 gLuckOdds[8] = {12, 12, 12, 12, 12}; // 0x802810B0  "1 in n" per player: 12 12 12 12

// ---- luck -------------------------------------------------------------------------------------

void Luck_ResetAllOdds2(void) {
    Luck_ResetAllOdds();
}

void Luck_ResetAllOdds(void) {
    int i;
    for (i = 0; i < 5; i++) {
        Luck_ResetOdds((u8)i);
    }
}

// The luck odds: "1 in gLuckOdds[n]". 12 at the start of a round, and every hole transition
// takes one off any player still above 9 - so 1 in 12, 11, 10, then 1 in 9 for the rest.
void Luck_ResetOdds(int nPlayer) {
    gLuckOdds[nPlayer] = 12;
}

void Luck_TightenOdds(void) {
    int i;
    for (i = 0; i < 5; i++) {
        if (gLuckOdds[i] > 9) {
            gLuckOdds[i]--;
        }
    }
}

// Is this shot the perfect (lucky) one? Humans only, not in split screen (fn_80101D4C can force
// it). One chance in the player's odds (12), halved when player 0 is 5+ holes down to player 1 in
// game mode 4 or when fn_800DA234 says so, then cut by LUCK/2 percent. Only off the green, never
// a putt, and only on a par 3, for pitches, or from lies 1/2 under 250 (fn_800D0478).
u8 Golfer_IsLucky(int nPlayer) {
    u8      bLucky = 0;
    u32     uOdds;
    u32     uRoll;
    if (fn_80101D4C(nPlayer)) {
        return 1;
    }
    if (Player_IsCPU(nPlayer) || gSession.nSplitScreen) {
        return 0;
    }
    uOdds = gLuckOdds[nPlayer];
    if (Game_GetMode() == 4 && gPlayers[1].nHolesWon - gPlayers[0].nHolesWon > 4) {
        uOdds >>= 1;
    } else if (fn_800DA234()) {
        uOdds >>= 1;
    }
    if (nPlayer >= 0 && nPlayer <= 3) {
        int nLuck = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_LUCK, ATTR_TOTAL);
        nLuck = nLuck < 0 ? 0 : (nLuck > 110 ? 110 : nLuck);
        uOdds -= uOdds * (nLuck >> 1) / 100;
        if (uOdds < 1) {
            uOdds = 1;
        }
    }
    uRoll = Misc_RandFunc(0) % uOdds;
    if (gPlayers[nPlayer].ball.nLie != LIE_GREEN_e && gPlayers[nPlayer].nShotKind != SHOT_TYPE_PUTT_e &&
        !gSession.nSplitScreen) {
        Game_CurrentPinSet();
        Ter_GetTGD();
        if (Course_GetCurHolePar() == 3) {
            bLucky = 1;
        } else if (gPlayers[nPlayer].nShotKind == SHOT_TYPE_PITCH_e) {
            bLucky = 1;
        } else if ((gPlayers[nPlayer].ball.nLie == 1 || gPlayers[nPlayer].ball.nLie == 2) &&
                   fn_800D0478(nPlayer) < 250.0f) {
            bLucky = 1;
        }
    }
    if (bLucky && uRoll != 0) {
        bLucky = 0;
    }
    return bLucky;
}
void  fn_8005CE70(int nPlayer);
