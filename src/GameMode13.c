// GameMode13.c (our name): game mode 13, the timed target game. Each player starts with 90 seconds
// (5400 frames, in n290 for the hole). Hitting a target pays points and adds time; each earlier hit
// on the same target scales both by 0.75, and a target pays at most 3 times. Bonuses picked up on
// the way raise a points multiplier. The game ends when everyone's time is up.

#include "golfer.h"

// The prize rows at lbl_80200538 + 0x710 (see GameMode10.c).
typedef struct MiniPrize {
    s32 nId;                    // 0x00  a surface id (0x85..0x90 are the target rings), 999 the prize row
    s32 nMode13;                // 0x04  mode 13's points for the surface
    s32 nMode16;                // 0x08
    s32 nMode17;                // 0x0C
    s32 nMode15;                // 0x10
    s32 nMode13Time;            // 0x14  mode 13's seconds added for the surface
    s32 nMode17Balls;           // 0x18
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
extern s32 lbl_802816A8;                    // the options saved while the game runs
extern s32 lbl_802823B0;
extern s32 lbl_802823B4;                    // the points multiplier from bonuses
extern s32 lbl_802823B8;                    // the seconds added by the last shot
extern s32 lbl_802823BC;                    // the points of the last shot

void  fn_800E1480(int nHole);
u32   Rand_Next(int nStream);
int   Game_CurHoleIndex(void);
void  Mem_cpy(void* pDst, void* pSrc, int nBytes);
void  fn_80055AA8(u8* pBall, f32* pPos, int nPlayer);
f32   fn_800D0550(int nPlayer);             // the shot's length
int   GOLFERSTATE_GetCurrentState(int nPlayer);
void  GOLFERSTATE_Switch(int nState, int nPlayer);
void  AI_DefaultTarget(int nPlayer);
void  fn_800A624C(void);
void  fn_800A6278(void);
void  fn_800A62A4(void);
void  fn_800A62E0(void);
void  fn_800A6358(void);
void  fn_800A63D0(void);
void  fn_800A30E4(int nKind, u8* pBall, int nPlayer, int a, f32 f);
s32   fn_800D6A70(s32 nPoints, int nPlayer, int a, int b, int c, int d);
int   fn_800D7220(int nReward, int a, s32* pOut);
void  fn_800D3548(int nPlayer, int nMoney, s32* p);
u8    fn_800E2B40(int nPlayer, u8* pBall);
void  fn_800E3C0C(int a);
void  fn_800E53F0(int nMsg, int a, int b, int c);
void  fn_800E58B4(int a);
void  fn_800ED710(s32 p0);
void  fn_80062D6C(int a, int b);
void  fn_800FF7DC(void);
void  fn_80125910(int a);
void  fn_800F19D4(void);
void  fn_800F1B60(int nPlayer, int n);
u8    fn_800F1BD8(int nPlayer);
u8    fn_800F1C34(int nPlayer);
s8    fn_800F1C74(int nPlayer);
s32   fn_800F1E58(s32 n);
void  fn_800F1EE4(void);
void  fn_800F2030(void);
s32   fn_800F20C0(int nPlayer);
void  fn_800F21B4(int nPlayer);
s32   fn_800F2578(void);
void  fn_800F263C(s32 p0);
void  fn_800F2664(int nPlayer);
void  fn_800F2668(int nPlayer);
s32   fn_800F266C(s32 n, int i);
u8    fn_800F2788(int nPlayer, f32 f);
s32   fn_800F2810(s32 n);
void  fn_800F3980(int nMsg, s32 a, s32 b, s32 c, s32 d, s32 e);

void  fn_800F6CC4(void);
void  fn_800F6CE0(void);
s32   fn_800F6D0C(void);
s32   fn_800F6D14(int nPlayer);
void  fn_800F6DFC(int nPlayer);
void  fn_800F6ED4(int nPlayer);
void  fn_800F7A4C(s32 nSurface, s32* pPoints, s32* pTime, s32* pBalls);
void  fn_800F7B44(void);
void  fn_800F7C00(void);
void  fn_800F7C24(int nPlayer);
void  fn_800F7CA0(void);
void  fn_800F7CD4(void);
void  fn_800F7D4C(int nPlayer);
u8    fn_800F7D8C(int a);
s32   fn_800F7D94(void);
s32   fn_800F7D9C(void);
void  fn_800F7DA4(void);
void  fn_800F7DE8(void);
void  fn_800F7ED8(int nPlayer);
void  fn_800F7EF8(int nPlayer);
void  fn_800F7F1C(int nPlayer, int nTime);
u8    fn_800F7F70(int nPlayer, int a);
void  fn_800F7FF4(int nPlayer, int nId);
s32   fn_800F8068(int nPlayer, int i);
s32   fn_800F80A0(void);
void  fn_800F80A8(void);
void  fn_800F80D4(s32 p0);

// Mode 13 starts: one player at a time, no wind, no gimmes, one mulligan.
void fn_800F6A60(void) {
    gpGame->pfn1C8 = fn_800F6A60;
    gpGame->pfn1CC = fn_800F6CC4;
    gpGame->pfn1D0 = fn_800F7B44;
    gpGame->pfn1D4 = fn_800F6D14;
    gpGame->pfn1D8 = fn_800F7F70;
    gpGame->pfn1DC = fn_800F7D8C;
    gpGame->pfn1E0 = fn_800F6D0C;
    gpGame->pfn248 = fn_800F6DFC;
    gpGame->pfn244 = fn_800F6ED4;
    gpGame->pfn1E4 = fn_800F7C00;
    gpGame->pfn228 = fn_800F7D4C;
    gpGame->pfn24C = fn_800F7C24;
    gpGame->pfn224 = fn_800F7CA0;
    gpGame->pfn1EC = fn_800F6CE0;
    gpGame->pfn250 = fn_800F7ED8;
    gpGame->pfn254 = fn_800F7EF8;
    gpGame->pfn264 = fn_800F1BD8;
    gpGame->pfn258 = fn_800F1C34;
    gpGame->pfn25C = fn_800F7F1C;
    gpGame->pfn268 = fn_800F7FF4;
    gpGame->pfn26C = fn_800F8068;
    gpGame->pfn1F4 = fn_800F80A8;
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
    gpGame->nMulligans = 1;
    gpGame->n10 = 1;
    gpGame->nC = 1;
    gpGame->n290 = 0;
    gpGame->n294 = 0;
    gpGame->nDC = 0;
    fn_800E1480(0);
    lbl_80282360 = 0;
    gSession.nSplitScreen = 0;
    gSession.unk5B38 = 0;
}

void fn_800F6CC4(void) {
    SESSION_OPTIONS->unkC = lbl_802816A8;
    SESSION_OPTIONS->nWind = lbl_802823B0;
}

void fn_800F6CE0(void) {
    lbl_802816A8 = SESSION_OPTIONS->unkC;
    lbl_802823B0 = SESSION_OPTIONS->nWind;
    SESSION_OPTIONS->unkC = 4;
    SESSION_OPTIONS->nWind = 0;
}

s32 fn_800F6D0C(void) {
    return 0;
}

// Who plays next: player 0 first, then the players with time left, in turn.
s32 fn_800F6D14(int nPlayer) {
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
        if (n != nPlayer && gPlayers[n].n290[Game_CurHoleIndex()] != 0) {
            return n;
        }
    }
    return 5;
}

// End of a golfer's turn: the clock stops, the ball goes back to the tee; count the shots.
void fn_800F6DFC(int nPlayer) {
    fn_800ED710(nPlayer);
    if (gReplayData.bF10) {
        Mem_cpy(gPlayers[nPlayer].ball, gReplayData.player.ball, 0xBC);
    } else {
        fn_80055AA8(gPlayers[nPlayer].ball,
                    (f32*)((u8*)gPlayers[nPlayer].pBallCourse + gSession.nTeeSet[nPlayer] * 0x10 + 0xB0), nPlayer);
    }
    gPlayers[nPlayer].nDC0++;
    if (gPlayers[nPlayer].nDBC > 1) {
        gPlayers[nPlayer].aDC4[0]++;
    }
}

// The ball stopped: points and time for the target, less for a target hit before.
void fn_800F6ED4(int nPlayer) {
    s32 nBalls;
    s32 nSurface;
    s8 nTarget;
    s32 nMsg;
    s32 bTime;
    s32 nAdded;
    s32 nAddedFrames;
    f32 fLength;
    f32 fScale;
    s32 nMult;
    u8* pBall;
    int j;
    nMsg = -1;
    nSurface = gPlayers[nPlayer].nBallSurface;
    fn_800F7A4C(nSurface, &lbl_802823BC, &lbl_802823B8, &nBalls);
    fScale = 1.0f;
    nAdded = 0;
    bTime = lbl_802823B8 != 0;
    fLength = fn_800D0550(nPlayer);
    if (nSurface >= 0x85 && nSurface <= 0x90 && !fn_800F2788(nPlayer, fLength)) {
        nTarget = fn_800F1C74(nPlayer);
        gPlayers[nPlayer].nE90++;
        if (gPlayers[nPlayer].nE90 > gPlayers[nPlayer].nE8C) {
            gPlayers[nPlayer].nE8C = gPlayers[nPlayer].nE90;
        }
        for (j = 0; j < gPlayers[nPlayer].nDE4[nTarget]; j++) {
            fScale *= 0.75f;
        }
        if (gPlayers[nPlayer].nDE4[nTarget] == 0) {
            if (lbl_80282360 - 1 == fn_800F20C0(nPlayer)) {
                fScale = 1.0f;
                lbl_802823BC = fn_800F2578();
                lbl_802823BC = fn_800D6A70(lbl_802823BC, nPlayer, 1, 1, 1, 0);
                lbl_802823BC = fn_800D7220(lbl_802823BC, nPlayer, 0);
                fn_800D3548(nPlayer, lbl_802823BC, 0);
                lbl_802823B8 = 0;
                gPlayers[nPlayer].nD70[Game_CurHoleIndex()]++;
                gPlayers[nPlayer].nDD8 += lbl_802823BC;
                fn_800F3980(0x33, lbl_802823BC, 0, 0, 0xCA, 1);
                lbl_802823BC = 0;
                lbl_802823B8 = 0;
                if ((s8)gPlayers[nPlayer].bE9E == 0) {
                    gPlayers[nPlayer].bE9E = 1;
                    if (!(Rand_Next(0) & 1)) {
                        nMsg = 0x2B;
                    } else {
                        nMsg = 0x2C;
                    }
                }
            } else {
                fScale = 1.0f;
                lbl_802823BC += 100;
                lbl_802823BC *= gPlayers[nPlayer].nDBC;
                lbl_802823BC = fn_800F266C(lbl_802823BC, nTarget);
                lbl_802823BC = fn_800D6A70(lbl_802823BC, nPlayer, 1, 1, 1, 0);
                lbl_802823BC = fn_800D7220(lbl_802823BC, nPlayer, 0);
                fn_800D3548(nPlayer, lbl_802823BC, 0);
                gPlayers[nPlayer].aCD4[gPlayers[nPlayer].nCD0] = nSurface;
                gPlayers[nPlayer].nCD0++;
                lbl_802823B8 += 5;
                gPlayers[nPlayer].nD70[Game_CurHoleIndex()]++;
                gPlayers[nPlayer].nDD8 += lbl_802823BC;
                gPlayers[nPlayer].aDC4[4] += lbl_802823B8 * 60;
                fn_800ED710(nPlayer);
                fn_800F80D4(gPlayers[nPlayer].n290[Game_CurHoleIndex()] + lbl_802823B8 * 60);
                nAdded = lbl_802823B8;
                fn_800F3980(0x33, lbl_802823BC, 0, 0, 0xC9, 1);
                fn_800E53F0(0x34, lbl_802823B8 * 60, 0, 0);
                lbl_802823BC = 0;
                lbl_802823B8 = 0;
                if (gPlayers[nPlayer].nDBC > 1) {
                    nMsg = 0x31;
                } else {
                    nMsg = 0x22;
                }
            }
        } else {
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
        if (gPlayers[nPlayer].nDE4[nTarget] > 3) {
            fScale = 0.0f;
            lbl_802823BC = 0;
            lbl_802823B8 = 0;
            fn_800F3980(0x33, 0, 0, 0, 0xC8, 1);
            fn_800A63D0();
            nMsg = 2;
        } else {
            gPlayers[nPlayer].nDE4[nTarget]++;
            gPlayers[nPlayer].aDC4[3]++;
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
        }
    } else {
        gPlayers[nPlayer].nE90 = 0;
    }
    if (nSurface >= 0x85 && nSurface <= 0x90 && fn_800F2788(nPlayer, fLength)) {
        if (fLength > gPlayers[nPlayer].nDDC) {
            gPlayers[nPlayer].nDDC = fLength;
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
        } else {
            fScale = 0.0f;
            lbl_802823BC = 0;
            lbl_802823B8 = 0;
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
    if (lbl_802823B8 != 0) {
        lbl_802823B8 = lbl_802823B8 * fScale;
        gPlayers[nPlayer].aDC4[4] += lbl_802823B8 * 60;
        fn_800ED710(nPlayer);
        fn_800F80D4(gPlayers[nPlayer].n290[Game_CurHoleIndex()] + lbl_802823B8 * 60);
        nAdded = lbl_802823B8;
        if (!gSession.bReplay) {
            fn_800E53F0(0x34, lbl_802823B8 * 60, 0, 0);
        }
    }
    if (lbl_802823BC != 0) {
        if (lbl_802823BC > 0) {
            lbl_802823BC = (f32)lbl_802823BC * gPlayers[nPlayer].nDBC;
            lbl_802823BC = lbl_802823BC * fScale;
            if (nSurface >= 0x85 && nSurface <= 0x90 && !fn_800F2788(nPlayer, fLength)) {
                lbl_802823BC = fn_800F266C(lbl_802823BC, nTarget);
                switch (gPlayers[nPlayer].nDBC) {
                case 2:
                    nMsg = 0x32;
                    break;
                case 3:
                    nMsg = 0x33;
                    break;
                case 4:
                    break;
                default:
                    nMsg = 0x31;
                    break;
                }
            }
            lbl_802823BC = (f32)(lbl_802823BC * lbl_802823B4);
            lbl_802823BC = fn_800D6A70(lbl_802823BC, nPlayer, 1, 1, 1, 0);
            lbl_802823BC = fn_800D7220(lbl_802823BC, nPlayer, 0);
        } else if (!(Rand_Next(0) & 1)) {
            fn_800F263C(0);
        } else {
            fn_800F263C(0x4E);
        }
        gPlayers[nPlayer].aCD4[gPlayers[nPlayer].nCD0] = nSurface;
        gPlayers[nPlayer].nCD0++;
        gPlayers[nPlayer].nD70[Game_CurHoleIndex()]++;
        if (lbl_802823BC + gPlayers[nPlayer].nDD8 < 0) {
            fn_800D3548(nPlayer, -gPlayers[nPlayer].nDD8, 0);
        } else {
            fn_800D3548(nPlayer, lbl_802823BC, 0);
        }
        gPlayers[nPlayer].nDD8 += lbl_802823BC;
        if (gPlayers[nPlayer].nDD8 < 0) {
            gPlayers[nPlayer].nDD8 = 0;
        } else if (!gSession.bReplay) {
            if (fn_800F2788(nPlayer, fLength)) {
                fn_800F3980(0x33, lbl_802823BC, 0, 0, 0xD6, 1);
            } else {
                fn_800F3980(0x33, lbl_802823BC, 0, 0, nSurface, 1);
            }
            if (lbl_802823BC > 0 && nSurface < 0x85) {
                fn_800A62A4();
            }
        }
    }
    if ((s8)gPlayers[nPlayer].bE9D) {
        if (bTime) {
            gPlayers[nPlayer].bE9D = 0;
            switch (Rand_Next(0) & 3) {
            case 0:
                nMsg = 0x38;
                break;
            case 1:
                nMsg = 0x39;
                break;
            default:
                nMsg = 0x34;
                break;
            }
        } else {
            nMsg = 0x14;
        }
    }
    if (fLength > gPlayers[nPlayer].nDDC && !fn_800E2B40(nPlayer, gPlayers[nPlayer].ball)) {
        gPlayers[nPlayer].nDDC = fLength;
    }
    if (nMsg != -1) {
        fn_800F263C(nMsg);
    }
    nAddedFrames = nAdded * 60;
    if (gPlayers[nPlayer].n290[Game_CurHoleIndex()] + nAddedFrames > 600 &&
        gPlayers[nPlayer].n290[Game_CurHoleIndex()] <= 600) {
        fn_800A6278();
    }
    if (gPlayers[nPlayer].n290[Game_CurHoleIndex()] <= 0 && nAddedFrames <= 600 && nAdded > 0) {
        fn_800A624C();
    }
    fn_800F2664(nPlayer);
    fn_800F2668(nPlayer);
}

// The points, seconds and balls for landing on a surface.
void fn_800F7A4C(s32 nSurface, s32* pPoints, s32* pTime, s32* pBalls) {
    int i;
    *pPoints = 0;
    *pTime = 0;
    *pBalls = 0;
    for (i = 0; i < 20; i++) {
        if (nSurface == lbl_80200538.mini[i].nId) {
            *pPoints = lbl_80200538.mini[i].nMode13;
            *pTime = lbl_80200538.mini[i].nMode13Time;
            *pBalls = lbl_80200538.mini[i].nMode17Balls;
        }
    }
}

// Hole start: maybe a multiplier, the clock shows the time left, and a player who has not shot
// aims at their target.
void fn_800F7B44(void) {
    int i;
    fn_800F2030();
    fn_800FF7DC();
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if ((s8)GOLFERSTATE_GetCurrentState(i) == 1) {
            fn_800F21B4(i);
            fn_800F80D4(PLAYER(i)->n290[Game_CurHoleIndex()]);
            if (PLAYER(i)->nDC0 == 0) {
                fn_800F1B60(i, (s8)PLAYER(i)->nTarget);
            }
        }
    }
    lbl_802823B4 = 1;
}

void fn_800F7C00(void) {
    fn_800F19D4();
    fn_800F7CD4();
}

// The clock is reset to 90 seconds for everyone.
void fn_800F7C24(int nPlayer) {
    int i;
    fn_800F80D4(5400);
    for (i = 0; i < 5; i++) {
        PLAYER(i)->n290[Game_CurHoleIndex()] = 5400;
    }
}

void fn_800F7CA0(void) {
    fn_800F7CD4();
    AI_DefaultTarget(0);
    fn_800F80D4(-1);
    fn_800A6278();
}

void fn_800F7CD4(void) {
    int i;
    fn_800F1EE4();
    for (i = 0; i < 5; i++) {
        PLAYER(i)->n290[Game_CurHoleIndex()] = 5400;
    }
}

void fn_800F7D4C(int nPlayer) {
    if (gPlayers[nPlayer].swing.nState != 0) {
        fn_800E58B4(0x36);
    }
}

u8 fn_800F7D8C(int a) {
    return 1;
}

s32 fn_800F7D94(void) {
    return lbl_802823BC;
}

s32 fn_800F7D9C(void) {
    return lbl_802823B8;
}

void fn_800F7DA4(void) {
    fn_800A624C();
    if (!(Rand_Next(0) & 1)) {
        fn_800F263C(0x15);
        return;
    }
    fn_800F263C(0x25);
}

// The time ran out: a shot not yet taken ends the player's game; one in flight finishes first.
void fn_800F7DE8(void) {
    fn_800A6278();
    if ((s8)GOLFERSTATE_GetCurrentState(lbl_80282278) == 1 || (s8)GOLFERSTATE_GetCurrentState(lbl_80282278) == 2 ||
        (s8)GOLFERSTATE_GetCurrentState(lbl_80282278) == 3 || (s8)GOLFERSTATE_GetCurrentState(lbl_80282278) == 4 ||
        (s8)GOLFERSTATE_GetCurrentState(lbl_80282278) == 10) {
        gPlayers[lbl_80282278].nLie = 12;
        gPlayers[lbl_80282278].nC3C |= 0x04000000;
        fn_800F80D4(0);
        fn_800ED710(lbl_80282278);
        GOLFERSTATE_Switch(13, lbl_80282278);
        fn_800F263C(0x14);
        return;
    }
    gPlayers[lbl_80282278].bE9D = 1;
}

void fn_800F7ED8(int nPlayer) {
    fn_800F6ED4(nPlayer);
}

// A mulligan: the multiplier and the streak are lost.
void fn_800F7EF8(int nPlayer) {
    gPlayers[nPlayer].nDBC = 1;
    gPlayers[nPlayer].nE90 = 0;
}

void fn_800F7F1C(int nPlayer, int nTime) {
    gPlayers[nPlayer].n290[Game_CurHoleIndex()] = nTime;
}

// The game is over when everyone's time is up.
u8 fn_800F7F70(int nPlayer, int a) {
    int i;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (PLAYER(i)->n290[Game_CurHoleIndex()] != 0) {
            return 0;
        }
    }
    return 1;
}

// A bonus was collected: the multiplier goes up.
void fn_800F7FF4(int nPlayer, int nId) {
    s32 n = fn_800F2810(nId);
    fn_800A30E4(8, gPlayers[nPlayer].ball, nPlayer, 0, 0.0f);
    lbl_802823B4 += n + 2;
}

// A target's state for the HUD: 1 when it has paid out 3 times.
s32 fn_800F8068(int nPlayer, int i) {
    if (gPlayers[nPlayer].nDE4[i] > 3) {
        return 1;
    }
    return 0;
}

s32 fn_800F80A0(void) {
    return lbl_802823B4;
}

void fn_800F80A8(void) {
    fn_80125910(1);
    fn_800E3C0C(0);
}

void fn_800F80D4(s32 p0) {
    fn_80062D6C(17, p0);
}
