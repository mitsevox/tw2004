// GameMode17.c (our name): game mode 17, around the world. Each player hits the targets in order
// (nNextTarget) with 5 balls; some rings pay extra balls. Hitting every target wins the prize row's
// bonus plus 100 points per ball left.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"

// The prize rows at lbl_80200538 + 0x710 (see GameTargets.c).
typedef struct MiniPrize {
    s32 nId;                    // 0x00  a surface id (0x85..0x90 are the target rings), 999 the prize row
    s32 nMode13;                // 0x04
    s32 nMode16;                // 0x08
    s32 nMode17;                // 0x0C  mode 17's points for the surface
    s32 nMode15;                // 0x10
    u8  unk14[4];
    s32 nMode17Balls;           // 0x18  mode 17's extra balls for the surface
} MiniPrize;
typedef struct PrizeTable {
    u8        unk0[0x710];
    MiniPrize mini[20];         // 0x710
} PrizeTable;
extern PrizeTable lbl_80200538;

extern Replay gReplayData;
extern u8  gNumPlayersSetUp;
extern s32 lbl_80282278;                    // the player whose turn it is
extern s8  lbl_80282360;                    // the number of targets (GameModeReplay.c)
extern s32 lbl_802816A0;                    // the options saved while the game runs
extern s32 lbl_802823A0;
extern s32 lbl_802823A4;                    // the extra balls of the last shot
extern s32 lbl_802823A8;                    // the points of the last shot

void  fn_800F5CC8(void);
void  fn_800F5CE4(void);
s32   fn_800F5D10(void);
s32   fn_800F5D18(int nPlayer);
void  fn_800F5E9C(int nPlayer);
void  fn_800F5F58(int nPlayer);
void  fn_800F66A0(void);
void  fn_800F673C(void);
void  fn_800F6760(void);
void  fn_800F6788(void);
void  fn_800F67E0(int nPlayer);
u8    fn_800F6820(int a);
void  fn_800F6828(int nPlayer);
u8    fn_800F6848(int nPlayer, int a);
void  fn_800F68C4(s32 nSurface, s32* pPoints, s32* pBalls);
u8    fn_800F6990(int nPlayer);
u8    fn_800F69C8(int nPlayer);
s32   fn_800F6A08(int nPlayer, int i);
void  fn_800F6A3C(void);

// Mode 17 starts: one player at a time, no wind, no gimmes, no mulligans.
void fn_800F5AAC(void) {
    gpGame->pfn1C8 = fn_800F5AAC;
    gpGame->pfn1CC = fn_800F5CC8;
    gpGame->pfn1D0 = fn_800F66A0;
    gpGame->pfn1D4 = fn_800F5D18;
    gpGame->pfn1D8 = fn_800F6848;
    gpGame->pfn1DC = fn_800F6820;
    gpGame->pfn1E0 = fn_800F5D10;
    gpGame->pfn248 = fn_800F5E9C;
    gpGame->pfn244 = fn_800F5F58;
    gpGame->pfn1E4 = fn_800F673C;
    gpGame->pfn228 = fn_800F67E0;
    gpGame->pfn224 = fn_800F6760;
    gpGame->pfn1EC = fn_800F5CE4;
    gpGame->pfn250 = fn_800F6828;
    gpGame->pfn264 = fn_800F6990;
    gpGame->pfn258 = fn_800F69C8;
    gpGame->pfn26C = fn_800F6A08;
    gpGame->pfn1F4 = fn_800F6A3C;
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

void fn_800F5CC8(void) {
    SESSION_OPTIONS->unkC = lbl_802816A0;
    SESSION_OPTIONS->nWind = lbl_802823A0;
}

void fn_800F5CE4(void) {
    lbl_802816A0 = SESSION_OPTIONS->unkC;
    lbl_802823A0 = SESSION_OPTIONS->nWind;
    SESSION_OPTIONS->unkC = 4;
    SESSION_OPTIONS->nWind = 0;
}

s32 fn_800F5D10(void) {
    return 0;
}

// Who plays next: player 0 first, then the players with balls left, in turn.
s32 fn_800F5D18(int nPlayer) {
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

// End of a golfer's turn: the ball goes back to the tee, one ball fewer.
void fn_800F5E9C(int nPlayer) {
    if (gReplayData.bF10) {
        Mem_cpy(gPlayers[nPlayer].ball, gReplayData.player.ball, 0xBC);
    } else {
        fn_80055AA8((Ball*)gPlayers[nPlayer].ball,
                    (f32*)((u8*)gPlayers[nPlayer].pBallCourse + gSession.nTeeSet[nPlayer] * 0x10 + 0xB0),
                    nPlayer);
    }
    gPlayers[nPlayer].nDC0--;
}

// The ball stopped: the next target in order moves on; the wrong one scores nothing.
void fn_800F5F58(int nPlayer) {
    s32 nSurface;
    s8 nTarget;
    s32 nMsg;
    s32 bDone;
    f32 fLength;
    s32 nMult;
    u8* pBall;
    nMsg = -1;
    bDone = 0;
    nSurface = gPlayers[nPlayer].nBallSurface;
    fLength = fn_800D0550(nPlayer);
    fn_800F68C4(nSurface, &lbl_802823A8, &lbl_802823A4);
    if (nSurface >= 0x85 && nSurface <= 0x90 && !fn_800F2788(nPlayer, fLength)) {
        nTarget = fn_800F1C74(nPlayer);
        fn_800F1E58(nSurface);
        if (nTarget == gPlayers[nPlayer].nNextTarget) {
            gPlayers[nPlayer].nNextTarget++;
            if (gPlayers[nPlayer].nNextTarget >= lbl_80282360) {
                gPlayers[nPlayer].nNextTarget = 0;
            }
            gPlayers[nPlayer].nE90++;
            if (gPlayers[nPlayer].nE90 > gPlayers[nPlayer].nE8C) {
                gPlayers[nPlayer].nE8C = gPlayers[nPlayer].nE90;
            }
            gPlayers[nPlayer].nDE4[nTarget]++;
            gPlayers[nPlayer].aDC4[3]++;
            if (lbl_80282360 == fn_800F20C0(nPlayer)) {
                lbl_802823A8 = fn_800F2578();
                lbl_802823A8 += gPlayers[nPlayer].nDC0 * 100;
                lbl_802823A8 = fn_800D6A70(lbl_802823A8, nPlayer, 1, 1, 1, 0);
                lbl_802823A8 = fn_800D7220(lbl_802823A8, nPlayer, 0);
                fn_800D3548(nPlayer, lbl_802823A8, 0);
                gPlayers[nPlayer].nD70[Game_CurHoleIndex()]++;
                bDone = 1;
                gPlayers[nPlayer].nDD8 += lbl_802823A8;
                fn_800F3980(0x33, lbl_802823A8, 0, 0, 0xCA, 1);
                lbl_802823A8 = 0;
                if ((s8)gPlayers[nPlayer].bE9E == 0) {
                    gPlayers[nPlayer].bE9E = 1;
                    if (!(Rand_Next(0) & 1)) {
                        nMsg = 0x2B;
                    } else {
                        nMsg = 0x2C;
                    }
                }
            }
            if (nMsg == -1) {
                switch (lbl_80282360 - fn_800F20C0(nPlayer)) {
                case 1:
                    if (Rand_Next(0) & 1) {
                        nMsg = 0x26;
                    } else {
                        nMsg = 0x43;
                    }
                    break;
                case 2:
                    if (Rand_Next(0) & 1) {
                        nMsg = 0x27;
                    } else {
                        nMsg = 0x44;
                    }
                    break;
                case 3:
                    if (Rand_Next(0) & 1) {
                        nMsg = 0x28;
                    } else {
                        nMsg = 0x45;
                    }
                    break;
                case 4:
                    nMsg = 0x29;
                    break;
                case 5:
                    nMsg = 0x2A;
                    break;
                default:
                    if (fn_800F20C0(nPlayer) <= lbl_80282360 / 4) {
                        if (lbl_802823A4 == 1) {
                            if (Rand_Next(0) & 1) {
                                nMsg = 0x36;
                            } else {
                                nMsg = 0x37;
                            }
                        } else {
                            switch (Rand_Next(0) & 3) {
                            case 0:
                                nMsg = 0x4F;
                                break;
                            case 1:
                                nMsg = 0x50;
                                break;
                            case 2:
                                nMsg = 0x51;
                                break;
                            case 3:
                                nMsg = 0x4F;
                                break;
                            }
                        }
                    } else {
                        switch (Rand_Next(0) & 7) {
                        case 0:
                        case 1:
                            nMsg = 0x46;
                            break;
                        case 2:
                        case 3:
                            nMsg = 0x47;
                            break;
                        case 4:
                            nMsg = 0x32;
                            break;
                        case 5:
                            nMsg = 0x31;
                            break;
                        case 6:
                            nMsg = 0x12;
                            break;
                        case 7:
                            nMsg = 0x11;
                            break;
                        }
                    }
                    break;
                }
            }
            if (nSurface == 0x85 || nSurface == 0x88 || nSurface == 0x8C) {
                gPlayers[nPlayer].nDE0++;
                fn_800A62E0();
                pBall = gPlayers[nPlayer].ball;
                fn_800A30E4(8, pBall, nPlayer, 0, 0.0f);
                nMsg = 0x1D;
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
            if (bDone == 0) {
                fn_800F3980(0x33, 0, 0, 0, nSurface, 1);
            }
        } else {
            fn_800F3980(0x33, 0, 0, 0, 0xD0, 1);
            lbl_802823A8 = 0;
            gPlayers[nPlayer].nE90 = 0;
            lbl_802823A4 = 0;
            if (Rand_Next(0) & 1) {
                nMsg = 0xB;
            } else {
                nMsg = 0xD;
            }
        }
    } else {
        gPlayers[nPlayer].nE90 = 0;
    }
    if (nSurface >= 0x85 && nSurface <= 0x90 && fn_800F2788(nPlayer, fLength)) {
        lbl_802823A8 = 0;
        lbl_802823A4 = 0;
    }
    if (lbl_802823A8 != 0) {
        if (lbl_802823A8 > 0) {
            lbl_802823A8 = fn_800D6A70(lbl_802823A8, nPlayer, 1, 1, 1, 0);
            lbl_802823A8 = fn_800D7220(lbl_802823A8, nPlayer, 0);
        } else if (nMsg == -1) {
            if (!(Rand_Next(0) & 1)) {
                nMsg = 0;
            } else {
                nMsg = 0x4E;
            }
        }
        if (lbl_802823A8 + gPlayers[nPlayer].nDD8 < 0) {
            fn_800D3548(nPlayer, -gPlayers[nPlayer].nDD8, 0);
        } else {
            fn_800D3548(nPlayer, lbl_802823A8, 0);
        }
        gPlayers[nPlayer].nDD8 += lbl_802823A8;
        if (gPlayers[nPlayer].nDD8 < 0) {
            gPlayers[nPlayer].nDD8 = 0;
        } else if (!gSession.bReplay) {
            if (fn_800F2788(nPlayer, fLength)) {
                fn_800F3980(0x33, lbl_802823A8, 0, 0, 0xD6, 1);
            } else {
                fn_800F3980(0x33, lbl_802823A8, 0, 0, nSurface, 1);
            }
            if (lbl_802823A8 > 0 && nSurface < 0x85) {
                fn_800A62A4();
            }
        }
    }
    if (lbl_802823A4 != 0) {
        gPlayers[nPlayer].nDC0 += lbl_802823A4;
        gPlayers[nPlayer].aDC4[1] += lbl_802823A4;
    }
    if (nMsg != -1) {
        fn_800F263C(nMsg);
    }
    fn_800F2664(nPlayer);
    fn_800F2668(nPlayer);
}

// Hole start: a player not aiming at their next target is turned to it.
void fn_800F66A0(void) {
    int i;
    fn_800F2030();
    fn_800FF7DC();
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if ((s8)GOLFERSTATE_GetCurrentState(i) == 1) {
            if (PLAYER(i)->nTarget != PLAYER(i)->nNextTarget) {
                fn_800F1B60(i, PLAYER(i)->nNextTarget);
            }
        }
    }
}

void fn_800F673C(void) {
    fn_800F19D4();
    fn_800F6788();
}

void fn_800F6760(void) {
    fn_800F6788();
    AI_DefaultTarget(0);
}

// 5 balls each, starting at the first target.
void fn_800F6788(void) {
    int i;
    fn_800F1EE4();
    for (i = 0; i < 5; i++) {
        gPlayers[i].nNextTarget = 0;
        gPlayers[i].nDC0 = 5;
    }
}

void fn_800F67E0(int nPlayer) {
    if (gPlayers[nPlayer].swing.nState != 0) {
        fn_800E58B4(0x36);
    }
}

u8 fn_800F6820(int a) {
    return 1;
}

void fn_800F6828(int nPlayer) {
    fn_800F5F58(nPlayer);
}

// The game is over when player 0 has hit every target, or nobody has a ball left.
u8 fn_800F6848(int nPlayer, int a) {
    int i;
    if (lbl_80282360 == fn_800F20C0(0)) {
        return 1;
    }
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (PLAYER(i)->nDC0 != 0) {
            return 0;
        }
    }
    return 1;
}

// The points and extra balls for landing on a surface.
void fn_800F68C4(s32 nSurface, s32* pPoints, s32* pBalls) {
    int i;
    *pPoints = 0;
    *pBalls = 0;
    for (i = 0; i < 20; i++) {
        if (nSurface == lbl_80200538.mini[i].nId) {
            *pPoints = lbl_80200538.mini[i].nMode17;
            *pBalls = lbl_80200538.mini[i].nMode17Balls;
        }
    }
}

// Previous / next target: the aim stays on the next target in order.
u8 fn_800F6990(int nPlayer) {
    fn_800F1ABC(nPlayer, gPlayers[nPlayer].nNextTarget);
    return 1;
}

u8 fn_800F69C8(int nPlayer) {
    fn_800F1ABC(nPlayer, gPlayers[nPlayer].nNextTarget);
    return 1;
}

s32 fn_800F6A00(s32 a) {
    return lbl_802823A8;
}

// A target's state for the HUD: 1 for every target but the next one.
s32 fn_800F6A08(int nPlayer, int i) {
    return i != gPlayers[nPlayer].nNextTarget;
}

s32 fn_800F6A34(s32 a) {
    return lbl_802823A4;
}

void fn_800F6A3C(void) {
    fn_80125910(1);
}
