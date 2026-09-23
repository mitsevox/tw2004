// GameMode14.c (our name): game mode 14, a two-player target game. The targets are 40 spots on the
// hole (the list in GameMode10.c). Landing on a target claims it for you, unless the other player
// has one there that is at least as close. The first to hold 5 targets wins. Each target is worth
// points by how close the claiming shot was.

#include "golfer.h"
#include "ball.h"
#include "game.h"

// One target's claim: how close the claiming shot was (0 best .. 4, 5 = unclaimed) and who holds it
// (5 = nobody).
typedef struct Claim {
    s32 nRank;                  // 0x0
    s32 nOwner;                 // 0x4
} Claim;
extern Claim lbl_80211FB8[40];
#define CLAIMS lbl_80211FB8

extern Replay gReplayData;
extern u8  gNumPlayersSetUp;
extern s32 lbl_80282278;                    // the player whose turn it is
extern s8  lbl_80282360;                    // the number of targets (GameMode10.c)
extern s32 lbl_80281688;                    // the options saved while the game runs
extern s32 lbl_80282368;
extern s32 lbl_8028236C;                    // who starts: 0 or 1, at random
extern u8  lbl_80282370;                    // the round was ended
extern s32 lbl_80282374;                    // the points of the last claim
extern s32 lbl_801928F0[];                  // points per rank

void  fn_800E1480(int nHole);
u32   Rand_Next(int nStream);
void  Mem_cpy(void* pDst, void* pSrc, int nBytes);
f32   fn_800D0550(int nPlayer);          // the shot's length
void  AI_DefaultTarget(int nPlayer);
void  fn_800A6278(void);
void  fn_800A62E0(void);
void  fn_800A6358(void);
void  fn_800A63D0(void);
void  fn_800A30E4(int nKind, u8* pBall, int nPlayer, int a, f32 f);
s32   fn_800D6A70(s32 nPoints, int nPlayer, int a, int b, int c, int d);
int   fn_800D7220(int nReward, int a, s32* pOut);
void  fn_800D3548(int nPlayer, int nMoney, s32* p);
void  fn_800E3D90(void);
void  fn_800E58B4(int a);
void  fn_800E5B0C(int nMsg, u32 uFloats, void* p0, void* p1, void* p2, void* p3, void* p4);
void  fn_800FF7DC(void);
void  fn_80125910(int a);
void  fn_800F19D4(void);
void  fn_800F1B60(int nPlayer, int n);
u8    fn_800F1BD8(int nPlayer);
u8    fn_800F1C34(int nPlayer);
s8    fn_800F1C74(int nPlayer);
int   fn_800F1D34(int nPlayer);
s32   fn_800F1E58(s32 n);
void  fn_800F1EE4(void);
void  fn_800F2030(void);
void  fn_800F263C(s32 p0);
s32   fn_800F266C(s32 n, int i);
u8    fn_800F2788(int nPlayer, f32 f);
void  fn_800F2958(s32 p0, s32 p1);

void  fn_800F2BBC(void);
void  fn_800F2BD8(void);
s32   fn_800F2C2C(void);
s32   fn_800F2C34(int nPlayer);
void  fn_800F2D4C(int nPlayer);
void  fn_800F2E08(int nPlayer);
void  fn_800F31E0(void);
void  fn_800F330C(void);
void  fn_800F3330(void);
void  fn_800F3358(void);
void  fn_800F33D0(int nPlayer);
u8    fn_800F3410(int a);
void  fn_800F3418(int nPlayer);
u8    fn_800F3438(int nPlayer, int a);
s32   fn_800F354C(int nPlayer);
s32   fn_800F3668(int n);
void  fn_800F36A4(void);
void  fn_800F3800(int nPlayer);
void  fn_800F3860(void);
s32   fn_800F392C(int a, int i);
void  fn_800F3980(int nMsg, s32 a, s32 b, s32 c, s32 d, s32 e);
void  fn_800F39CC(s32 p0);

// Mode 14 starts: two players, no wind, no gimmes, no mulligans.
void fn_800F2984(void) {
    gpGame->pfn1C8 = fn_800F2984;
    gpGame->pfn1CC = fn_800F2BBC;
    gpGame->pfn1D0 = fn_800F31E0;
    gpGame->pfn1D4 = fn_800F2C34;
    gpGame->pfn1D8 = fn_800F3438;
    gpGame->pfn1DC = fn_800F3410;
    gpGame->pfn1E0 = fn_800F2C2C;
    gpGame->pfn248 = fn_800F2D4C;
    gpGame->pfn244 = fn_800F2E08;
    gpGame->pfn1E4 = fn_800F330C;
    gpGame->pfn228 = fn_800F33D0;
    gpGame->pfn224 = fn_800F3330;
    gpGame->pfn1EC = fn_800F2BD8;
    gpGame->pfn250 = fn_800F3418;
    gpGame->pfn264 = fn_800F1BD8;
    gpGame->pfn258 = fn_800F1C34;
    gpGame->pfn260 = fn_800F3800;
    gpGame->pfn1F4 = fn_800F3860;
    gpGame->pfn26C = fn_800F392C;
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
    gSession.unk5B38 = 0;
}

// Game finished: the saved options go back.
void fn_800F2BBC(void) {
    SESSION_OPTIONS->unkC = lbl_80281688;
    SESSION_OPTIONS->nWind = lbl_80282368;
}

// Round setup: options saved and replaced, and a random first player.
void fn_800F2BD8(void) {
    lbl_80281688 = SESSION_OPTIONS->unkC;
    lbl_80282368 = SESSION_OPTIONS->nWind;
    SESSION_OPTIONS->unkC = 4;
    SESSION_OPTIONS->nWind = 0;
    lbl_8028236C = Rand_Next(0) & 1;
}

s32 fn_800F2C2C(void) {
    return 0;
}

// Who plays next: the random starter before anyone has hit, then the players in turn.
s32 fn_800F2C34(int nPlayer) {
    int i;
    int n;
    u8 bFirst = 1;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (PLAYER(i)->nStrokes[Game_CurHoleIndex()] != 0) {
            bFirst = 0;
        }
    }
    if (bFirst) {
        return lbl_8028236C;
    }
    n = lbl_80282278;
    for (i = 0; i < 5; i++) {
        n++;
        if (n >= gNumPlayersSetUp) {
            n = 0;
        }
        if (n != nPlayer) {
            return n;
        }
    }
    return 5;
}

// End of a golfer's turn: the ball goes back to the tee (or to the replay's ball).
void fn_800F2D4C(int nPlayer) {
    if (gReplayData.bF10) {
        Mem_cpy(gPlayers[nPlayer].ball, gReplayData.player.ball, 0xBC);
    } else {
        fn_80055AA8((Ball*)gPlayers[nPlayer].ball,
                    (f32*)((u8*)gPlayers[nPlayer].pBallCourse + gSession.nTeeSet[nPlayer] * 0x10 + 0xB0), nPlayer);
    }
    gPlayers[nPlayer].nDC0++;
}

// The ball stopped: on a target, maybe claim it.
void fn_800F2E08(int nPlayer) {
    s32 nSurface;
    int nTarget;
    s32 nRank;
    s32 nMsg = -1;
    s32 nText;
    s32 nKind = 0;
    f32 fLength;
    s32 nMult;
    if (lbl_80282370) {
        fn_800F3980(0x33, 0, 0, 0, 0xD1, 1);
        nMsg = 0x14;
    } else {
        nSurface = gPlayers[nPlayer].nBallSurface;
        fLength = fn_800D0550(nPlayer);
        if (nSurface >= 0x85 && nSurface <= 0x90 && !fn_800F2788(nPlayer, fLength)) {
            nTarget = fn_800F1C74(nPlayer);
            nRank = fn_800F1E58(nSurface);
            if (CLAIMS[nTarget].nRank == 0) {
                fn_800F3980(0x33, 0, 0, 0, 0xCD, 1);
                fn_800A63D0();
                nMsg = 2;
            } else if (nRank >= CLAIMS[nTarget].nRank) {
                fn_800F3980(0x33, 0, 0, 0, 0xCC, 1);
                nMsg = 0x10;
            } else {
                nText = 0;
                if (CLAIMS[nTarget].nRank == 5) {
                    if (!(Rand_Next(0) & 1)) {
                        nMsg = 0x1A;
                    } else {
                        nMsg = 0x52;
                    }
                } else if (nPlayer == CLAIMS[nTarget].nOwner) {
                    nMsg = 0x42;
                } else {
                    nText = 0xD4;
                    nKind = 1;
                    gPlayers[nPlayer].nE94++;
                    if (!(Rand_Next(0) & 1)) {
                        nMsg = 0xF;
                    } else {
                        nMsg = 0x1B;
                    }
                }
                if (nRank == 0) {
                    gPlayers[nPlayer].nDE0++;
                    if (nKind == 0) {
                        nText = 0xD2;
                        if (!(Rand_Next(0) & 1)) {
                            nMsg = 0x16;
                        } else {
                            nMsg = 0x53;
                        }
                    } else {
                        nText = 0xD5;
                        if (!(Rand_Next(0) & 1)) {
                            nMsg = 0x17;
                        } else {
                            nMsg = 0x1C;
                        }
                    }
                    nKind = 2;
                }
                CLAIMS[nTarget].nRank = nRank;
                CLAIMS[nTarget].nOwner = nPlayer;
                gPlayers[nPlayer].aCD4[gPlayers[nPlayer].nCD0] = nSurface;
                gPlayers[nPlayer].nCD0++;
                gPlayers[nPlayer].nD70[Game_CurHoleIndex()]++;
                lbl_80282374 = fn_800F3668(nTarget);
                lbl_80282374 = fn_800F266C(lbl_80282374, nTarget);
                lbl_80282374 = fn_800D6A70(lbl_80282374, nPlayer, 1, 1, 1, 0);
                lbl_80282374 = fn_800D7220(lbl_80282374, nPlayer, 0);
                fn_800F36A4();
                if (nText != 0) {
                    fn_800F3980(0x33, lbl_80282374, 0, 0, nText, 1);
                }
                if (!gSession.bReplay) {
                    fn_800F3980(0x33, lbl_80282374, 0, 0, nSurface, 1);
                    if (nKind == 0) {
                        fn_800F3980(0x33, lbl_80282374, 0, 0, 0xD3, 1);
                    }
                    if (nRank == 0) {
                        u8* pBall;
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
            }
        }
    }
    if (nMsg != -1) {
        fn_800F263C(nMsg);
    }
}

// Hole start: the players' targets reset. A player 3 or more targets behind gets a comment.
void fn_800F31E0(void) {
    int i;
    s32 n0;
    s32 n1;
    s32 nMsg = -1;
    lbl_80282370 = 0;
    fn_800F2030();
    fn_800FF7DC();
    n0 = fn_800F354C(0);
    n1 = fn_800F354C(1);
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if ((s8)GOLFERSTATE_GetCurrentState(i) == 1) {
            fn_800F39CC(900);
            if (PLAYER(i)->nDC0 == 0) {
                fn_800F1B60(i, (s8)PLAYER(i)->nTarget);
            }
            if (i == 0 && n1 >= n0 + 3) {
                if (!(Rand_Next(0) & 1)) {
                    nMsg = 0x4A;
                } else {
                    nMsg = 0x4C;
                }
            } else if (i == 1 && n0 >= n1 + 3) {
                if (!(Rand_Next(0) & 1)) {
                    nMsg = 0x4B;
                } else {
                    nMsg = 0x4D;
                }
            }
        }
    }
    if (nMsg != -1) {
        fn_800F2958((u16)nMsg, 0);
    }
}

void fn_800F330C(void) {
    fn_800F19D4();
    fn_800F3358();
}

void fn_800F3330(void) {
    fn_800F3358();
    AI_DefaultTarget(0);
}

// Every target unclaimed.
void fn_800F3358(void) {
    int i;
    fn_800F1EE4();
    for (i = 0; i < 40; i++) {
        CLAIMS[i].nRank = 5;
        CLAIMS[i].nOwner = 5;
    }
}

void fn_800F33D0(int nPlayer) {
    if (gPlayers[nPlayer].swing.nState != 0) {
        fn_800E58B4(0x36);
    }
}

u8 fn_800F3410(int a) {
    return 1;
}

void fn_800F3418(int nPlayer) {
    fn_800F2E08(nPlayer);
}

// The game is over when someone holds 5 targets.
u8 fn_800F3438(int nPlayer, int a) {
    int i;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (fn_800F354C(i) >= 5) {
            return 1;
        }
    }
    return 0;
}

static inline int CurrentTarget(int nPlayer) {
    return fn_800F1D34(nPlayer);
}

// Who holds the target the player is aiming at (-1: not aiming at one).
s32 fn_800F3490(int nPlayer) {
    if (gPlayers[nPlayer].nSurface < 0x85 || gPlayers[nPlayer].nSurface > 0x90) {
        return -1;
    }
    return CLAIMS[CurrentTarget(nPlayer)].nOwner;
}

s32 fn_800F34F0(int nPlayer) {
    if (gPlayers[nPlayer].nSurface < 0x85 || gPlayers[nPlayer].nSurface > 0x90) {
        return -1;
    }
    return CLAIMS[fn_800F1D34(nPlayer)].nRank;
}

// How many targets the player holds.
s32 fn_800F354C(int nPlayer) {
    s32 n = 0;
    int i;
    for (i = 0; i < 40; i++) {
        if (nPlayer == CLAIMS[i].nOwner) {
            n++;
        }
    }
    return n;
}

s32 fn_800F363C(int i) {
    return CLAIMS[i].nOwner;
}

s32 fn_800F3654(s32 p0) {
    return CLAIMS[p0].nRank;
}

// A claimed target's points.
s32 fn_800F3668(int n) {
    if (CLAIMS[n].nOwner != 5) {
        return lbl_801928F0[CLAIMS[n].nRank];
    }
    return 0;
}

// Each player's points: the sum over the targets they hold.
void fn_800F36A4(void) {
    int i;
    s32 n;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        PLAYER(i)->nDD8 = 0;
    }
    for (i = 0; i < 40; i++) {
        if (CLAIMS[i].nOwner != 5) {
            n = fn_800F3668(i);
            n = fn_800F266C(n, i);
            n = fn_800D6A70(n, CLAIMS[i].nOwner, 1, 1, 1, 0);
            n = fn_800D7220(n, CLAIMS[i].nOwner, 0);
            gPlayers[CLAIMS[i].nOwner].nDD8 += n;
        }
    }
}

s32 fn_800F37F8(void) {
    return lbl_80282374;
}

void fn_800F3800(int nPlayer) {
    fn_800F39CC(-1);
    fn_800A6278();
}

// End the round now (from the pause menu).
void fn_800F3828(void) {
    GOLFERSTATE_Switch(12, lbl_80282278);   // GS_SIMULATE
    fn_800E3D90();
    fn_800A6278();
    lbl_80282370 = 1;
}

// Game finished: the winner is paid their points; a loser with no targets gets a comment.
void fn_800F3860(void) {
    int i;
    s32 nMsg = -1;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (fn_800F354C(i) < 5) {
            PLAYER(i)->nDD8 = 0;
            if (fn_800F354C(i) == 0) {
                nMsg = 0x48;
            }
        } else {
            fn_80125910(1);
            fn_800D3548(i, PLAYER(i)->nDD8, 0);
        }
    }
    if (nMsg != -1) {
        fn_800F2958((u16)nMsg, 0);
    }
}

// A target's state for the HUD: 1 claimed with a hole-out, 2 player 0's, 3 player 1's, 0 free.
s32 fn_800F392C(int a, int i) {
    if (CLAIMS[i].nRank == 0) {
        return 1;
    }
    if (CLAIMS[i].nOwner == 0) {
        return 2;
    }
    return CLAIMS[i].nOwner == 1 ? 3 : 0;
}

void fn_800F3980(int nMsg, s32 a, s32 b, s32 c, s32 d, s32 e) {
    fn_800E5B0C(nMsg, 0, &a, &b, &c, &d, &e);
}

void fn_800F39CC(s32 p0) {
    fn_80062D6C(55, p0);
}
