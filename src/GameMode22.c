// GameMode22.c (our name): game mode 22 (GameRound.c starts it with fn_80125E68), and before it the
// trophy case's text (Rookie of the Year, Player of the Year, the money and scoring leaders; "Earned
// on %s"). One file: both halves share its .data, .sdata and .sbss blocks. Not yet decompiled; the
// sweep code below is the matched small functions.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/modes/mode22.h"

void fn_80125BB8(const s32* pA, s32* pOut);
void fn_801260B8(void);
void fn_801260BC(void);
void fn_80126130(void);
void fn_80126150(void);
void fn_801262C4(int nPlayer);
s32 fn_8012632C(void);
u8   fn_801263C4(int nPlayer, int n);
void fn_80126698();
u8   fn_80126FB0(s32* pn8);
u8   fn_80126418(int n);
void fn_8012643C(void);
void fn_8012645C(int nPlayer);
s32  fn_80126640(int n);
void fn_80126EC0();
void fn_80126E68(void);
void fn_80126E88(void);
void fn_80126F7C(void);
void fn_80126F80(void);
void fn_80126F84(s32 p0);
void fn_80126F94(s32 n);
s32 fn_80126FA0(void);
s32 fn_80126FD8(void);
u8   fn_80127004(void);
void fn_80127034(int nPlayer);
s32 fn_80127098(s32 arg0);

// A value mod 4 (a callback FE_MessageTable.c installs).
void fn_80125BB8(const s32* pA, s32* pOut) {
    *pOut = *pA % 4;
}

void fn_801260B8(void) {
}

void fn_801260BC(void) {
}

void fn_80126130(void) {
    GameModeStroke_SetupNextGolfer();
}

// Pays the player in fn_80126FB0's n8 5000.
void fn_80126150(void) {
    s32 nPlayer;

    fn_80126FB0(&nPlayer);
    fn_800D3548(nPlayer, 5000, NULL);
}

void fn_801262C4(int nPlayer) {
    fn_80126F80();
    fn_80127034(nPlayer);
    fn_800E5CA4(0, gPlayers[nPlayer].nEBC, 0, 0, 0, 0, 0, 0.0f);
}

s32 fn_8012632C(void) {
    return 0;
}

u8 fn_801263C4(int nPlayer, int n) {
    s32 bRet = 0;

    if (fn_80126418(n) && lbl_80195498.n18 < 0) {
        bRet = 1;
    }
    return bRet;
}

// n is not read (fn_801263C4 passes one).
u8 fn_80126418(int n) {
    return fn_80126FB0(NULL);
}

void fn_8012643C(void) {
    fn_80126698();
}

// The ball back on the player's tee.
void fn_8012645C(int nPlayer) {
    fn_80055AA8(&gPlayers[nPlayer].ball,
                &gPlayers[nPlayer].ball.pCourse->tee[gSession.nTeeSet[nPlayer]].x, nPlayer);
}

s32 fn_80126640(int n) {
    switch (n) {
    case 0:
        return 0;
    case 1:
    case 2:
    case 10:
        return 1;
    case 3:
    case 4:
    case 5:
    case 14:
    case 15:
    case 17:
        return 2;
    case 6:
    case 7:
    case 8:
        return 3;
    case 9:
        return 4;
    case 12:
        return 5;
    default:
        return 1;
    }
}

void fn_80126E68(void) {
    fn_80126EC0();
}

void fn_80126E88(void) {
    fn_80126EC0();
    lbl_80195498.n8 = 5;
    lbl_80195498.bC = 0;
}

void fn_80126F7C(void) {
}

void fn_80126F80(void) {
}

void fn_80126F84(s32 p0) {
    lbl_80195498.n4 = p0;
}

void fn_80126F94(s32 n) {
    lbl_80195498.n0 = n;
}

s32 fn_80126FA0(void) {
    return lbl_80195498.n0;
}

// n8 (when pn8 is not NULL) and bC.
u8 fn_80126FB0(s32* pn8) {
    if (pn8 != NULL) {
        *pn8 = lbl_80195498.n8;
    }
    return lbl_80195498.bC;
}

s32 fn_80126FD8(void) {
    s32 t0;
    t0 = Game_GetMode();
    return (((u32)__cntlzw((22 - t0)) >> 5) & 0xFF);
}

u8 fn_80127004(void) {
    if (lbl_80195498.n8 != 5 && lbl_80195498.n18 > 0) {
        return 1;
    }
    return 0;
}

// Shows n4 less the current player's nEA0 as a message (nPlayer is not read; every caller passes
// one).
void fn_80127034(int nPlayer) {
    sprintf(lbl_80195488, "%d", lbl_80195498.n4 - gPlayers[lbl_80282278].nEA0);
    fn_800E5D68(lbl_80195488);
}

s32 fn_80127098(s32 arg0) {
    switch (arg0) {
    case 6:
        return 0;
    case 7:
        return 1;
    case 5:
        return 2;
    case 3:
        return 3;
    case 4:
        return 4;
    default:
        return 0;
    }
}
