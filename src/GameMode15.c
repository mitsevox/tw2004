// GameMode15.c (our name): game mode 15, HORSE on the targets. A leader lands on a target; the
// others must land on the same target at least as close, or take a letter (nE88). Closer still
// makes you the leader. Five letters and you are out; the last player in wins.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/earnings.h"

extern Replay gReplayData;
extern u8  gNumPlayersSetUp;
extern s32 lbl_80282278;                    // the player whose turn it is
extern s8  lbl_80282360;                    // the number of targets (GameModeReplay.c)
extern s32 lbl_80281690;                    // the options saved while the game runs
extern s32 lbl_80282378;
extern u8  lbl_8028237C;                    // a new leader was just set
extern u8  lbl_8028237D;                    // the round was ended
extern s32 lbl_80282380;                    // the points of the last shot
extern s8  lbl_80282384;                    // the leader's target
extern s32 lbl_80282388;                    // the leader's rank (0 best .. 4)
extern s32 lbl_8028238C;                    // the leader (5 = none)

void  fn_800A6394(void);
void  fn_800A640C(void);

void  fn_800F3C2C(void);
void  fn_800F3C48(void);
s32   fn_800F3C74(void);
s32   fn_800F3C7C(int nPlayer);
void  fn_800F3E00(int nPlayer);
void  fn_800F3EBC(int nPlayer);
void  fn_800F4584(void);
void  fn_800F4698(void);
void  fn_800F46BC(void);
void  fn_800F46E4(void);
void  fn_800F471C(int nPlayer);
u8    fn_800F475C(int a);
void  fn_800F4764(int nPlayer);
s32   fn_800F4784(int nPlayer, int a);
u8    fn_800F47D8(int nPlayer);
u8    fn_800F4828(int nPlayer);
s8    fn_800F4878(void);
void  fn_800F4894(int nPlayer);
void  fn_800F4950(void);
void  fn_800F49E8(s32 nSurface, s32* pPoints);
s32   fn_800F4B00(void);
s32   fn_800F4B14(int a, int i);

// Mode 15 starts: the same setup as mode 14.
void fn_800F39F4(void) {
    gpGame->pfn1C8 = fn_800F39F4;
    gpGame->pfn1CC = fn_800F3C2C;
    gpGame->pfn1D0 = fn_800F4584;
    gpGame->pfn1D4 = fn_800F3C7C;
    gpGame->pfn1D8 = (u8 (*)(int, int))fn_800F4784;
    gpGame->pfn1DC = fn_800F475C;
    gpGame->pfn1E0 = fn_800F3C74;
    gpGame->pfn248 = fn_800F3E00;
    gpGame->pfn244 = fn_800F3EBC;
    gpGame->pfn1E4 = fn_800F4698;
    gpGame->pfn228 = fn_800F471C;
    gpGame->pfn224 = fn_800F46BC;
    gpGame->pfn1EC = fn_800F3C48;
    gpGame->pfn250 = fn_800F4764;
    gpGame->pfn264 = fn_800F47D8;
    gpGame->pfn258 = fn_800F4828;
    gpGame->pfn260 = fn_800F4894;
    gpGame->pfn1F4 = fn_800F4950;
    gpGame->pfn26C = fn_800F4B14;
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
    gpGame->n10 = 2;
    gpGame->nC = 2;
    gpGame->n290 = 0;
    gpGame->n294 = 0;
    gpGame->nDC = 0;
    fn_800E1480(0);
    lbl_80282360 = 0;
    gSession.nSplitScreen = 0;
    gSession.nPinSet = 0;
}

void fn_800F3C2C(void) {
    SESSION_OPTIONS->unkC = lbl_80281690;
    SESSION_OPTIONS->nWind = lbl_80282378;
}

void fn_800F3C48(void) {
    lbl_80281690 = SESSION_OPTIONS->unkC;
    lbl_80282378 = SESSION_OPTIONS->nWind;
    SESSION_OPTIONS->unkC = 4;
    SESSION_OPTIONS->nWind = 0;
}

s32 fn_800F3C74(void) {
    return 0;
}

// Who plays next: player 0 first, then the players still in, in turn.
s32 fn_800F3C7C(int nPlayer) {
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
        if (n != nPlayer && gPlayers[n].nE88 < 5) {
            return n;
        }
    }
    return 5;
}

// End of a golfer's turn: the ball goes back to the tee (or to the replay's ball).
void fn_800F3E00(int nPlayer) {
    if (gReplayData.bF10) {
        Mem_cpy(&gPlayers[nPlayer].ball, &gReplayData.player.ball, sizeof(Ball));
    } else {
        fn_80055AA8(&gPlayers[nPlayer].ball,
                    &gPlayers[nPlayer].ball.pCourse->tee[gSession.nTeeSet[nPlayer]].x, nPlayer);
    }
    gPlayers[nPlayer].nDC0++;
}

// The ball stopped: match the leader, beat them, or take a letter.
void fn_800F3EBC(int nPlayer) {
    s32 nSurface;
    s8 nTarget;
    s32 nRank;
    s32 nMsg;
    f32 fLength;
    s32 nMult;
    Ball* pBall;
    f32 x;
    f32 y;
    nMsg = -1;
    if (lbl_8028237D) {
        fn_800F3980(0x33, 0, 0, 0, 0xD1, 1);
        nMsg = 0x14;
    } else {
        nSurface = gPlayers[nPlayer].ball.nSurface;
        fLength = fn_800D0550(nPlayer);
        fn_800F49E8(nSurface, &lbl_80282380);
        if (nSurface >= 0x85 && nSurface <= 0x90 && !fn_800F2788(nPlayer, fLength)) {
            nTarget = fn_800F1C74(nPlayer);
            nRank = fn_800F1E58(nSurface);
            gPlayers[nPlayer].nDE4[nTarget]++;
            gPlayers[nPlayer].aDC4[3]++;
            if (lbl_80282380 != 0) {
                if (lbl_80282380 > 0) {
                    lbl_80282380 = fn_800D6A70(lbl_80282380, nPlayer, 1, 1, 1, 0);
                    lbl_80282380 = fn_800D7220(lbl_80282380, nPlayer, 0);
                }
                gPlayers[nPlayer].nDD8 += lbl_80282380;
                if (gPlayers[nPlayer].nDD8 < 0) {
                    gPlayers[nPlayer].nDD8 = 0;
                }
                if (!gSession.bReplay) {
                    fn_8006434C(fn_80017004(gPlayers[nPlayer].nView[0]), gPlayers[nPlayer].ball.vPrev, &x, &y,
                                0);
                    fn_8006A8D4(fn_80017004(gPlayers[nPlayer].nView[0]), &x, &y);
                    fn_800F3980(0x33, lbl_80282380, 512.0f * x, 448.0f * y, nSurface, 1);
                }
            }
            if (lbl_8028238C != 5) {
                if (nTarget == lbl_80282384 && nRank == lbl_80282388) {
                    fn_800F3980(0x33, 0, 0, 0, nSurface, 1);
                    if (nRank == 0) {
                        fn_800A62E0();
                        nMsg = 0x32;
                        pBall = &gPlayers[nPlayer].ball;
                        fn_800A30E4(8, pBall, nPlayer, 0, 0.0f);
                        nMult = gPlayers[nPlayer].nDBC;
                        if (nMult > 1) {
                            fn_800A30E4(nMult + 7, pBall, nPlayer, 0, 0.0f);
                        }
                    } else {
                        fn_800A6358();
                        nMsg = 0x11;
                        nMult = gPlayers[nPlayer].nDBC;
                        if (nMult > 1) {
                            fn_800A30E4(nMult + 7, &gPlayers[nPlayer].ball, nPlayer, 0, 0.0f);
                        }
                    }
                } else if (nTarget == lbl_80282384 && nRank < lbl_80282388) {
                    lbl_8028238C = nPlayer;
                    lbl_80282388 = nRank;
                    lbl_80282384 = nTarget;
                    fn_800F3980(0x33, 0, 0, 0, 0xCF, 1);
                    lbl_8028237C = 1;
                    if (nRank == 0) {
                        fn_800A62E0();
                        nMsg = 0x31;
                        pBall = &gPlayers[nPlayer].ball;
                        fn_800A30E4(8, pBall, nPlayer, 0, 0.0f);
                        nMult = gPlayers[nPlayer].nDBC;
                        if (nMult > 1) {
                            fn_800A30E4(nMult + 7, pBall, nPlayer, 0, 0.0f);
                        }
                    } else {
                        fn_800A6358();
                        nMsg = 0x33;
                        nMult = gPlayers[nPlayer].nDBC;
                        if (nMult > 1) {
                            fn_800A30E4(nMult + 7, &gPlayers[nPlayer].ball, nPlayer, 0, 0.0f);
                        }
                    }
                } else {
                    lbl_8028238C = 5;
                    gPlayers[nPlayer].nE88++;
                    fn_80062D6C(0x38, gPlayers[nPlayer].nE88);
                    fn_800A640C();
                    if (nTarget == lbl_80282384) {
                        fn_800F3980(0x33, 0, 0, 0, 0xCE, 1);
                        if (!(Rand_Next(0) & 1)) {
                            nMsg = 0xE;
                        } else {
                            nMsg = 0x10;
                        }
                    } else {
                        fn_800F3980(0x33, 0, 0, 0, 0xD0, 1);
                        if (!(Rand_Next(0) & 1)) {
                            nMsg = 0xA;
                        } else {
                            nMsg = 0xC;
                        }
                    }
                }
            } else {
                lbl_8028238C = nPlayer;
                lbl_80282388 = nRank;
                lbl_80282384 = nTarget;
                fn_800F3980(0x33, 0, 0, 0, nSurface, 1);
                if (nRank == 0) {
                    fn_800A62E0();
                    pBall = &gPlayers[nPlayer].ball;
                    fn_800A30E4(8, pBall, nPlayer, 0, 0.0f);
                    nMult = gPlayers[nPlayer].nDBC;
                    if (nMult > 1) {
                        fn_800A30E4(nMult + 7, pBall, nPlayer, 0, 0.0f);
                    }
                } else {
                    fn_800A6358();
                    nMult = gPlayers[nPlayer].nDBC;
                    if (nMult > 1) {
                        fn_800A30E4(nMult + 7, &gPlayers[nPlayer].ball, nPlayer, 0, 0.0f);
                    }
                }
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
            }
        } else if (lbl_8028238C != 5) {
            lbl_8028238C = 5;
            gPlayers[nPlayer].nE88++;
            fn_800F3980(0x33, 0, 0, 0, 0xCE, 1);
            fn_80062D6C(0x38, gPlayers[nPlayer].nE88);
            fn_800A640C();
            switch (gPlayers[nPlayer].nE88) {
            case 1:
                if (!(Rand_Next(0) & 1)) {
                    nMsg = 3;
                } else {
                    nMsg = 4;
                }
                break;
            case 2:
                nMsg = 5;
                break;
            case 3:
                nMsg = 6;
                break;
            case 4:
                if (!(Rand_Next(0) & 1)) {
                    nMsg = 7;
                } else {
                    nMsg = 8;
                }
                break;
            case 5:
                nMsg = 9;
                break;
            }
        } else {
            lbl_8028238C = 5;
        }
    }
    if (nMsg != -1) {
        fn_800F263C(nMsg);
    }
}

// Hole start: when the turn comes back to the leader, the lead is lost. While there is a leader,
// everyone aims at the leader's target.
void fn_800F4584(void) {
    int i;
    lbl_8028237D = 0;
    fn_800F2030();
    fn_800FF7DC();
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if ((s8)GOLFERSTATE_GetCurrentState(i) == 1) {
            fn_800F39CC(900);
            if (lbl_8028238C == i) {
                lbl_8028238C = 5;
                if (!(Rand_Next(0) & 1)) {
                    fn_800F2958(0x23, 0);
                } else {
                    fn_800F2958(0x24, 0);
                }
            }
            if (lbl_8028238C != 5 || PLAYER(i)->nDC0 == 0) {
                if (lbl_8028238C != 5) {
                    PLAYER(i)->nTarget = lbl_80282384;
                }
                fn_800F1B60(i, (s8)PLAYER(i)->nTarget);
            }
        }
    }
}

void fn_800F4698(void) {
    fn_800F19D4();
    fn_800F46E4();
}

void fn_800F46BC(void) {
    fn_800F46E4();
    AI_DefaultTarget(0);
}

void fn_800F46E4(void) {
    fn_800F1EE4();
    lbl_8028238C = 5;
    lbl_80282388 = 5;
    lbl_80282384 = 0;
    lbl_8028237C = 0;
}

void fn_800F471C(int nPlayer) {
    if (gPlayers[nPlayer].swing.nState != 0) {
        fn_800E58B4(0x36);
    }
}

u8 fn_800F475C(int a) {
    return 1;
}

void fn_800F4764(int nPlayer) {
    fn_800F3EBC(nPlayer);
}

// The game is over when at most one player is still in.
s32 fn_800F4784(int nPlayer, int a) {
    int i;
    s32 n = 0;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (PLAYER(i)->nE88 < 5) {
            n++;
        }
    }
    return n <= 1;
}

// Previous / next target: only without a leader; with one, the leader's target stays.
u8 fn_800F47D8(int nPlayer) {
    if (lbl_8028238C == 5) {
        fn_800F1BD8(nPlayer);
    } else {
        fn_800F1ABC(nPlayer, (s8)gPlayers[nPlayer].nTarget);
    }
    return 1;
}

u8 fn_800F4828(int nPlayer) {
    if (lbl_8028238C == 5) {
        fn_800F1C34(nPlayer);
    } else {
        fn_800F1ABC(nPlayer, (s8)gPlayers[nPlayer].nTarget);
    }
    return 1;
}

s8 fn_800F4878(void) {
    if ((s32) lbl_8028238C != 5) {
        return (s8) lbl_80282388;
    }
    return -1;
}

void fn_800F4894(int nPlayer) {
    fn_800F39CC(-1);
    fn_800A6278();
    lbl_8028237C = 0;
}

// End the round now (from the pause menu): with a leader, the other player takes a letter.
void fn_800F48C4(void) {
    GOLFERSTATE_Switch(12, lbl_80282278);   // GS_SIMULATE
    fn_800E3D90();
    fn_800A6278();
    lbl_8028237D = 1;
    if (lbl_8028238C != 5) {
        if (lbl_8028238C == 0) {
            gPlayers[1].nE88++;
            fn_80062D6C(0x38, gPlayers[1].nE88);
        } else {
            gPlayers[0].nE88++;
            fn_80062D6C(0x38, gPlayers[0].nE88);
        }
        fn_800A6394();
    }
}

// Game finished: the players still in are paid their points.
void fn_800F4950(void) {
    int i;
    fn_80125910(1);
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (PLAYER(i)->nE88 < 5) {
            fn_800D3548(i, PLAYER(i)->nDD8, 0);
        } else {
            PLAYER(i)->nDD8 = 0;
        }
    }
}

// The points for landing on a surface.
void fn_800F49E8(s32 nSurface, s32* pPoints) {
    int i;
    *pPoints = 0;
    for (i = 0; i < 20; i++) {
        if (nSurface == lbl_80200538.aMini[i].nId) {
            *pPoints = lbl_80200538.aMini[i].n10;
        }
    }
}

s32 fn_800F4B00(void) {
    return ((u32)((-lbl_8028237C) | lbl_8028237C) >> 31);
}

// A target's state for the HUD: 1 for the leader's target.
s32 fn_800F4B14(int a, int i) {
    if (lbl_8028238C == 5 || i == lbl_80282384) {
        return 0;
    }
    return 1;
}
