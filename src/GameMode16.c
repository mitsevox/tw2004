// GameMode16.c (our name): game mode 16, target practice. Each player has 20 balls (nDC0) to hit
// the targets; a target pays up to 3 times, hitting every target pays the prize row's bonus, and
// the bullseyes, streaks and the longest carry are counted. Bonuses picked up on the way
// (pfn268) raise a points multiplier.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"

// The prize rows at lbl_80200538 + 0x710 (see GameTargets.c); nMode16 is the points for a surface.
typedef struct MiniPrize {
    s32 nId;                    // 0x00  a surface id (0x85..0x90 are the target rings), 999 the prize row
    s32 nMode13;                // 0x04
    s32 nMode16;                // 0x08
    s32 nMode17;                // 0x0C
    s32 nMode15;                // 0x10
    u8  unk14[8];
} MiniPrize;
typedef struct PrizeTable {
    u8        unk0[0x710];
    MiniPrize mini[20];         // 0x710
} PrizeTable;
extern PrizeTable lbl_80200538;

extern Replay gReplayData;
extern u8  gNumPlayersSetUp;
extern s32 lbl_80282278;                    // the player whose turn it is
extern s8  lbl_80282360;                    // the number of targets (GameMode10.c)
extern s32 lbl_80281698;                    // the options saved while the game runs
extern s32 lbl_80282390;
extern s32 lbl_80282394;                    // the points multiplier from bonuses
extern s32 lbl_80282398;                    // the points of the last shot

void  fn_800F4D6C(void);
void  fn_800F4D88(void);
s32   fn_800F4DB4(void);
s32   fn_800F4DBC(int nPlayer);
void  fn_800F4F40(int nPlayer);
void  fn_800F5014(int nPlayer);
void  fn_800F56D4(void);
void  fn_800F577C(void);
void  fn_800F57A0(void);
void  fn_800F57C8(void);
void  fn_800F5808(int nPlayer);
u8    fn_800F5848(int a);
void  fn_800F5850(int nPlayer);
u8    fn_800F5870(int nPlayer, int a);
void  fn_800F58B4(s32 nSurface, s32* pPoints);
s32   fn_800F59CC(void);
s32   fn_800F59D4(void);
s32   fn_800F59DC(int nPlayer, int i);
void  fn_800F5A14(int nPlayer, int nId);
void  fn_800F5A88(void);

// Mode 16 starts: one player at a time, no wind, no gimmes, no mulligans.
void fn_800F4B40(void) {
    gpGame->pfn1C8 = fn_800F4B40;
    gpGame->pfn1CC = fn_800F4D6C;
    gpGame->pfn1D0 = fn_800F56D4;
    gpGame->pfn1D4 = fn_800F4DBC;
    gpGame->pfn1D8 = fn_800F5870;
    gpGame->pfn1DC = fn_800F5848;
    gpGame->pfn1E0 = fn_800F4DB4;
    gpGame->pfn248 = fn_800F4F40;
    gpGame->pfn244 = fn_800F5014;
    gpGame->pfn1E4 = fn_800F577C;
    gpGame->pfn228 = fn_800F5808;
    gpGame->pfn224 = fn_800F57A0;
    gpGame->pfn1EC = fn_800F4D88;
    gpGame->pfn250 = fn_800F5850;
    gpGame->pfn264 = fn_800F1BD8;
    gpGame->pfn258 = fn_800F1C34;
    gpGame->pfn26C = fn_800F59DC;
    gpGame->pfn268 = fn_800F5A14;
    gpGame->pfn1F4 = fn_800F5A88;
    gpGame->b276 = 0;
    gpGame->bGimmesAllowed = 0;
    gpGame->b280 = 0;
    gpGame->b271 = 0;
    gpGame->b281 = 0;
    gpGame->bStrokeLimit = 0;
    gpGame->b285 = 0;
    gpGame->b274 = 0;
    gpGame->b286 = 1;
    gpGame->b287 = 0;
    gpGame->b288 = 0;
    gpGame->b289 = 0;
    gpGame->b28A = 0;
    gpGame->bNoWind = 1;
    gpGame->bBumpObstructions = 0;
    gpGame->b28D = 1;
    gpGame->n4 = 0;
    gpGame->nMulligans = 0;
    gpGame->n10 = 1;
    gpGame->nC = 1;
    gpGame->n290 = 0;
    gpGame->n294 = 0;
    gpGame->nDC = 0;
    lbl_80282360 = 0;
    gSession.unk5B38 = 0;
    fn_800E1480(0);
}

void fn_800F4D6C(void) {
    SESSION_OPTIONS->unkC = lbl_80281698;
    SESSION_OPTIONS->nWind = lbl_80282390;
}

void fn_800F4D88(void) {
    lbl_80281698 = SESSION_OPTIONS->unkC;
    lbl_80282390 = SESSION_OPTIONS->nWind;
    SESSION_OPTIONS->unkC = 4;
    SESSION_OPTIONS->nWind = 0;
}

s32 fn_800F4DB4(void) {
    return 0;
}

// Who plays next: player 0 first, then the players with balls left, in turn.
s32 fn_800F4DBC(int nPlayer) {
    int i;
    int n;
    u8 bFirst = 1;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (PLAYER(i)->nStrokes[Game_CurHoleIndex()] != 0) {
            bFirst = 0;
        }
    }
    if (bFirst) {
        return 0;
    }
    n = lbl_80282278;
    for (i = 0; i < 5; i++) {
        n++;
        if (n >= gNumPlayersSetUp) {
            n = 0;
        }
        if (n != nPlayer && gPlayers[n].nDC0 != 0) {
            return n;
        }
    }
    return 5;
}

// End of a golfer's turn: the ball goes back to the tee, one ball fewer; count multiplied shots.
void fn_800F4F40(int nPlayer) {
    if (gReplayData.bF10) {
        Mem_cpy(gPlayers[nPlayer].ball, gReplayData.player.ball, 0xBC);
    } else {
        fn_80055AA8((Ball*)gPlayers[nPlayer].ball,
                    (f32*)((u8*)gPlayers[nPlayer].pBallCourse + gSession.nTeeSet[nPlayer] * 0x10 + 0xB0), nPlayer);
    }
    gPlayers[nPlayer].nDC0--;
    if (gPlayers[nPlayer].nDBC > 1) {
        gPlayers[nPlayer].aDC4[0]++;
    }
}

// The ball stopped: score the target, the all-targets bonus, and the longest carry.
void fn_800F5014(int nPlayer) {
    s32 nSurface;
    s8 nTarget;
    s32 nMsg;
    f32 fLength;
    s32 nMult;
    u8* pBall;
    nMsg = -1;
    nSurface = gPlayers[nPlayer].nBallSurface;
    fLength = fn_800D0550(nPlayer);
    fn_800F58B4(nSurface, &lbl_80282398);
    if (nSurface >= 0x85 && nSurface <= 0x90 && !fn_800F2788(nPlayer, fLength)) {
        nTarget = fn_800F1C74(nPlayer);
        fn_800F1E58(nSurface);
        gPlayers[nPlayer].nE90++;
        if (gPlayers[nPlayer].nE90 > gPlayers[nPlayer].nE8C) {
            gPlayers[nPlayer].nE8C = gPlayers[nPlayer].nE90;
        }
        if (gPlayers[nPlayer].nDE4[nTarget] > 3) {
            lbl_80282398 = 0;
            fn_800F3980(0x33, 0, 0, 0, 0xC8, 1);
            fn_800A63D0();
            nMsg = 2;
        } else {
            gPlayers[nPlayer].nDE4[nTarget]++;
            gPlayers[nPlayer].aDC4[3]++;
            if (lbl_80282360 == fn_800F20C0(nPlayer)) {
                lbl_80282398 = fn_800F2578();
                lbl_80282398 = fn_800D6A70(lbl_80282398, nPlayer, 1, 1, 1, 0);
                lbl_80282398 = fn_800D7220(lbl_80282398, nPlayer, 0);
                fn_800D3548(nPlayer, lbl_80282398, 0);
                gPlayers[nPlayer].nD70[Game_CurHoleIndex()]++;
                gPlayers[nPlayer].nDD8 += lbl_80282398;
                fn_800F3980(0x33, lbl_80282398, 0, 0, 0xCA, 1);
                lbl_80282398 = 0;
                if ((s8)gPlayers[nPlayer].bE9E == 0) {
                    gPlayers[nPlayer].bE9E = 1;
                    if (!(Rand_Next(0) & 1)) {
                        nMsg = 0x2B;
                    } else {
                        nMsg = 0x2C;
                    }
                }
            }
            if (nSurface == 0x85 || nSurface == 0x88 || nSurface == 0x8C) {
                gPlayers[nPlayer].nDE0++;
                fn_800A62E0();
                pBall = gPlayers[nPlayer].ball;
                fn_800A30E4(8, pBall, nPlayer, 0, 0.0f);
                nMult = gPlayers[nPlayer].nDBC;
                if (nMult > 1) {
                    fn_800A30E4(nMult + 7, pBall, nPlayer, 0, 0.0f);
                }
            } else {
                fn_800A6358();
                nMult = gPlayers[nPlayer].nDBC;
                if (nMult > 1) {
                    fn_800A30E4(nMult + 7, gPlayers[nPlayer].ball, nPlayer, 0, 0.0f);
                }
            }
            if (nMsg == -1) {
                switch (gPlayers[nPlayer].nDBC) {
                case 2:
                    nMsg = 0x32;
                    break;
                case 3:
                    nMsg = 0x33;
                    break;
                case 5:
                    nMsg = 0x31;
                    break;
                default:
                    switch (fn_800F1E58(nSurface)) {
                    case 0:
                        nMsg = 0x1D;
                        break;
                    case 1:
                        nMsg = 0x1E;
                        break;
                    case 2:
                        nMsg = 0x1F;
                        break;
                    case 3:
                        nMsg = 0x21;
                        break;
                    case 4:
                        nMsg = 0x20;
                        break;
                    }
                    break;
                }
                fn_800F3980(0x33, 0, 0, 0, nSurface, 1);
            }
        }
    } else {
        gPlayers[nPlayer].nE90 = 0;
    }
    if (nSurface >= 0x85 && nSurface <= 0x90 && fn_800F2788(nPlayer, fLength)) {
        if (fLength > gPlayers[nPlayer].nDDC) {
            gPlayers[nPlayer].nDDC = fLength;
            if (nMsg == -1) {
                switch (Rand_Next(0) & 3) {
                case 0:
                    nMsg = 0x31;
                    break;
                case 1:
                    nMsg = 0x32;
                    break;
                default:
                    nMsg = 0x33;
                    break;
                }
            }
        } else {
            lbl_80282398 = 0;
            if (nMsg == -1) {
                fn_800F3980(0x33, 0, 0, 0, 0xCB, 1);
                switch (Rand_Next(0) & 3) {
                case 0:
                    nMsg = 0x2F;
                    break;
                case 1:
                    nMsg = 1;
                    break;
                default:
                    nMsg = 0x30;
                    break;
                }
            }
        }
    }
    if (lbl_80282398 != 0) {
        if (lbl_80282398 > 0) {
            lbl_80282398 = (f32)lbl_80282398 * gPlayers[nPlayer].nDBC;
            lbl_80282398 = (f32)(lbl_80282398 * lbl_80282394);
            lbl_80282398 = fn_800D6A70(lbl_80282398, nPlayer, 1, 1, 1, 0);
            lbl_80282398 = fn_800D7220(lbl_80282398, nPlayer, 0);
        } else if (!(Rand_Next(0) & 1)) {
            fn_800F263C(0);
        } else {
            fn_800F263C(0x4E);
        }
        if (lbl_80282398 + gPlayers[nPlayer].nDD8 < 0) {
            fn_800D3548(nPlayer, -gPlayers[nPlayer].nDD8, 0);
        } else {
            fn_800D3548(nPlayer, lbl_80282398, 0);
        }
        gPlayers[nPlayer].nDD8 += lbl_80282398;
        if (gPlayers[nPlayer].nDD8 < 0) {
            gPlayers[nPlayer].nDD8 = 0;
        } else if (!gSession.bReplay) {
            if (fn_800F2788(nPlayer, fLength)) {
                fn_800F3980(0x33, lbl_80282398, 0, 0, 0xD6, 1);
            } else {
                fn_800F3980(0x33, lbl_80282398, 0, 0, nSurface, 1);
            }
            if (lbl_80282398 > 0 && nSurface < 0x85) {
                fn_800A62A4();
            }
        }
    }
    if (nMsg != -1) {
        fn_800F263C(nMsg);
    }
    fn_800F2664(nPlayer);
    fn_800F2668(nPlayer);
}

// Hole start: maybe a multiplier, and a player who has not shot aims at their target.
void fn_800F56D4(void) {
    int i;
    fn_800F2030();
    fn_800FF7DC();
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if ((s8)GOLFERSTATE_GetCurrentState(i) == 1) {
            fn_800F21B4(i);
            if (PLAYER(i)->nDC0 == 0) {
                fn_800F1B60(i, (s8)PLAYER(i)->nTarget);
            }
        }
    }
    lbl_80282394 = 1;
}

void fn_800F577C(void) {
    fn_800F19D4();
    fn_800F57C8();
}

void fn_800F57A0(void) {
    fn_800F57C8();
    AI_DefaultTarget(0);
}

// 20 balls each.
void fn_800F57C8(void) {
    int i;
    fn_800F1EE4();
    i = 0;
    while (i < 5) {
        gPlayers[i++].nDC0 = 20;
    }
}

void fn_800F5808(int nPlayer) {
    if (gPlayers[nPlayer].swing.nState != 0) {
        fn_800E58B4(0x36);
    }
}

u8 fn_800F5848(int a) {
    return 1;
}

void fn_800F5850(int nPlayer) {
    fn_800F5014(nPlayer);
}

// The game is over when nobody has a ball left.
u8 fn_800F5870(int nPlayer, int a) {
    int i;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (PLAYER(i)->nDC0 != 0) {
            return 0;
        }
    }
    return 1;
}

// The points for landing on a surface.
void fn_800F58B4(s32 nSurface, s32* pPoints) {
    int i;
    *pPoints = 0;
    for (i = 0; i < 20; i++) {
        if (nSurface == lbl_80200538.mini[i].nId) {
            *pPoints = lbl_80200538.mini[i].nMode16;
        }
    }
}

s32 fn_800F59CC(void) {
    return lbl_80282398;
}

s32 fn_800F59D4(void) {
    return lbl_80282394;
}

// A target's state for the HUD: 1 when it has paid out 3 times.
s32 fn_800F59DC(int nPlayer, int i) {
    if (gPlayers[nPlayer].nDE4[i] > 3) {
        return 1;
    }
    return 0;
}

// A bonus was collected: the multiplier goes up.
void fn_800F5A14(int nPlayer, int nId) {
    s32 n = fn_800F2810(nId);
    fn_800A30E4(8, gPlayers[nPlayer].ball, nPlayer, 0, 0.0f);
    lbl_80282394 += n + 2;
}

void fn_800F5A88(void) {
    fn_80125910(1);
}
