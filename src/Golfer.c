// Golfer.c: the golfers' luck, the lucky (perfect) shot. Each player has odds of "1 in
// gLuckOdds[n]" (Luck_*: 1 in 12 at the start of a round, one better after each hole down to 1 in
// 9, back to 12 after a lucky shot), and Golfer_IsLucky rolls against them when a shot is planned;
// a won roll lets the caddie's rehearsed shot replace the player's (Luck_TakePerfectShot,
// Code8002DB80.c). No assert names this file; "Golfer.c" is our name, kept from before the split.
// CodeWarrior GC/2.5, -O4,p.
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

s8 gLuckOdds[8] = {12, 12, 12, 12, 12}; // 0x802810B0  lucky-shot odds, "1 in n", per player slot
                                        // (slot 4: the caddie's copy; 5-7 never used)

// ---- luck -------------------------------------------------------------------------------------

// The round's set-up (GO_vInitIG): every player's lucky-shot odds back to 1 in 12.
void Luck_InitIG(void) {
    Luck_ResetAllOdds();
}

// Every player's lucky-shot odds back to 1 in 12, all five player slots (slot 4 is the caddie's
// copy).
void Luck_ResetAllOdds(void) {
    int i;
    for (i = 0; i < 5; i++) {
        Luck_ResetOdds((u8)i);
    }
}

// A player's lucky-shot odds back to 1 in 12 (gLuckOdds): every player at the start of a round
// (Luck_ResetAllOdds), and a player who has just taken a lucky shot (Luck_TakePerfectShot).
// Luck_TightenOdds improves them hole by hole.
void Luck_ResetOdds(int nPlayer) {
    gLuckOdds[nPlayer] = 12;
}

// After each hole (the game loop's hole exit): every player's lucky-shot odds improve by one, 1 in
// 12, 11, 10, then 1 in 9 for the rest of the round.
void Luck_TightenOdds(void) {
    int i;
    for (i = 0; i < 5; i++) {
        if (gLuckOdds[i] > 9) {
            gLuckOdds[i]--;
        }
    }
}

// Rolls whether this shot is the perfect (lucky) one, stored in Player.bPerfect when the shot is
// planned. Always for a CPU in game mode 11 (fn_80101D4C); never for a CPU or in split screen. The
// chance is 1 in the player's odds (gLuckOdds, 12 to 9), halved in game mode 4 when player 0 is 5
// or more holes down to player 1, or else when the hole-in-one prize is on this hole (fn_800DA234);
// for players 0-3 then cut by LUCK/2 percent (LUCK clamped to 0..110, odds at least 1 in 1). Only
// off the green and never on a putt, and only on a par 3, for a pitch, or from the fairway (lies 1
// and 2) within 250 yards of the pin.
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
