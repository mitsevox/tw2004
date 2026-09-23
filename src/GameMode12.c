// GameMode12.c (our name): game mode 12, stroke play with points. A shot that stops on a special
// surface (a row of the prize table at lbl_80200538 + 0x710) scores points, times the shot's
// multiplier; each hole's points (nD28) are then scaled by the score against par, and a human
// player is paid the round's total. The honors, hole-finished and game-finished callbacks are
// mode 0's.

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "game/earnings.h"

extern u8  gNumPlayersSetUp;                // 0x80281D48 (Golfer.c)
extern s32 lbl_802823DC;                    // the surface the ball stopped on (-1: none)

// The surfaces a player has scored on, for the HUD (fn_800FF634): lbl_802823D8 entries.
extern s32 lbl_802823D8;
extern s32 lbl_80212468[20];                // the surface
extern s32 lbl_80212418[20];                // how many times

s32  fn_800FEC78(void);
void fn_800FEF00(s32 nSurface, s32* pPoints, s32* pMeter, s32* pMult);
s32  fn_800FEFF8(int nPlayer, s32 nSurface);
void fn_800FEC80(int nPlayer);
s32  fn_800FF038(int nPlayer);
void fn_800FF114(void);
void fn_800FF288(void);
void fn_800FF3D4(void);
void fn_800FF3F8(void);
void fn_800FF5B4(int nPlayer);
void fn_800FF6C0(int nPlayer);

// Mode 12 starts: stroke play with mode 0's turn order, one mulligan per player per round.
void fn_800FEAFC(void) {
    gpGame->pfn1C8 = fn_800FEAFC;
    gpGame->pfn1D4 = fn_800FF894;
    gpGame->pfn1D8 = fn_800FFCCC;
    gpGame->pfn1DC = fn_800FFD54;
    gpGame->pfn1E0 = fn_800FEC78;
    gpGame->pfn1F4 = fn_800FF288;
    gpGame->pfn23C = fn_800FEC80;
    gpGame->pfn240 = (s32 (*)(void))fn_800FF038;
    gpGame->pfn1E8 = fn_800FF114;
    gpGame->pfn1E4 = fn_800FF3D4;
    gpGame->pfn1D0 = fn_800FF3F8;
    gpGame->pfn244 = fn_800FF5B4;
    gpGame->pfn228 = fn_800FF6C0;
    gpGame->b271 = 0;
    gpGame->b281 = 0;
    gpGame->bStrokeLimit = 0;
    gpGame->b288 = 0;
    gpGame->n4 = 0;
    gpGame->nMulligans = 1;
    gpGame->nC = 4;
    gpGame->n10 = 1;
    gpGame->nDC = 0;
    fn_800E1480(0);
    lbl_802823DC = -1;
    gSession.nSplitScreen = 0;
    SESSION_OPTIONS->unkC = 0;
}

s32 fn_800FEC78(void) {
    return 0;
}

// A surface is used up once it has scored 5 times (1 time for one that costs points).
static inline u8 SurfaceUsedUp(s32* pPoints, s32 nHits) {
    u8 bUsed = 1;
    s32 nPoints = *pPoints;
    if (nPoints < 0 && nHits < 1) {
        bUsed = 0;
    }
    if (nPoints > 0 && nHits < 5) {
        bUsed = 0;
    }
    return bUsed;
}

// The ball stopped on a surface that still scores: a higher multiplier is taken, the points are
// added (more for each earlier time on the same surface), the bonus meter fills; each shows a
// message at the ball's place on screen.
void fn_800FEC80(int nPlayer) {
    s32 nHits;
    s32 nScore;
    s32 nPoints;
    s32 nMeter;
    s32 nMult;
    f32 x;
    f32 y;
    if (lbl_802823DC >= 0) {
        fn_800FEF00(lbl_802823DC, &nPoints, &nMeter, &nMult);
        nHits = fn_800FEFF8(nPlayer, lbl_802823DC);
        if (!SurfaceUsedUp(&nPoints, nHits)) {
            fn_8006434C(fn_80017004(gPlayers[nPlayer].nView0), gPlayers[nPlayer].ball.vPrev,
                        &x, &y, 0);
            fn_8006A8D4(fn_80017004(gPlayers[nPlayer].nView0), &x, &y);
            if (nMult > gPlayers[nPlayer].nDBC) {
                gPlayers[nPlayer].nDBC = nMult;
                fn_800E53F0(0x35, nMult, 512.0f * x, 448.0f * y);
            }
            if (nPoints != 0) {
                gPlayers[nPlayer].aCD4[gPlayers[nPlayer].nCD0] = lbl_802823DC;
                gPlayers[nPlayer].nCD0++;
                gPlayers[nPlayer].nD70[Game_CurHoleIndex()]++;
                nScore = nPoints * (nHits + 1);
                gPlayers[nPlayer].nDB8 += nScore * gPlayers[nPlayer].nDBC;
                if (!gSession.bReplay) {
                    fn_800F3980(0x33, nScore, 512.0f * x, 448.0f * y, lbl_802823DC, nHits + 1);
                }
            }
            if (nMeter != 0) {
                gPlayers[nPlayer].nD24 += nMeter;
                if (gPlayers[nPlayer].nD24 > 100) {
                    gPlayers[nPlayer].nD24 = 100;
                }
                fn_800E53F0(0x34, nMeter, 512.0f * x, 448.0f * y);
            }
        }
    }
}

// fake match: stands in for a function the original linker stripped. Its constants (0.0, then 0.5)
// are still in this file's pool; its body is unknown.
static f32 GameMode12_StrippedFn(f32 x) {
    if (x > 0.5f) {
        return 0.0f;
    }
    return x;
}

// A surface's row in the prize table: its points, bonus-meter points and shot multiplier (all 0
// when it has none).
void fn_800FEF00(s32 nSurface, s32* pPoints, s32* pMeter, s32* pMult) {
    int i;
    *pPoints = 0;
    *pMeter = 0;
    *pMult = 0;
    for (i = 0; i < 20; i++) {
        if (nSurface == lbl_80200538.aMini[i].nId) {
            *pPoints = lbl_80200538.aMini[i].n4;
            *pMeter = lbl_80200538.aMini[i].n14;
            *pMult = lbl_80200538.aMini[i].n18;
        }
    }
}

// How many times the player has scored on the surface nSurface.
s32 fn_800FEFF8(int nPlayer, s32 nSurface) {
    s32 n = 0;
    int i;
    for (i = 0; i < gPlayers[nPlayer].nCD0; i++) {
        if (nSurface == gPlayers[nPlayer].aCD4[i]) {
            n++;
        }
    }
    return n;
}

// Where the ball stopped (lbl_802823DC). A surface with points scores up to 5 times, one that costs
// points once. Always returns 0.
s32 fn_800FF038(int nPlayer) {
    SurfaceType* pSurface;
    s32 nHits;
    s32 nPoints;
    s32 nMeter;
    s32 nMult;
    pSurface = PLAYER(nPlayer)->ball.pHitSurface;
    lbl_802823DC = -1;
    if (pSurface) {
        lbl_802823DC = pSurface - gSurfaceTypes;
        nHits = fn_800FEFF8(nPlayer, lbl_802823DC);
        fn_800FEF00(lbl_802823DC, &nPoints, &nMeter, &nMult);
        if (!SurfaceUsedUp(&nPoints, nHits) && nPoints != 0) {
            return 0;
        }
    }
    return 0;
}

// Hole finished: each player's points for the hole are multiplied by the score: 32 for a hole in
// one, then 16 for 3 under par down to 0.33 for 3 over, nothing worse than that.
void fn_800FF114(void) {
    int i;
    f32 fMult;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (PLAYER(i)->nStrokes[Game_CurHoleIndex()] == 1) {
            fMult = 32.0f;
        } else {
            switch (PLAYER(i)->nStrokes[Game_CurHoleIndex()] - fn_800D2B08()) {
            case -3:
                fMult = 16.0f;
                break;
            case -2:
                fMult = 8.0f;
                break;
            case -1:
                fMult = 4.0f;
                break;
            case 0:
                fMult = 2.0f;
                break;
            case 1:
                fMult = 0.66f;
                break;
            case 2:
                fMult = 0.5f;
                break;
            case 3:
                fMult = 0.33f;
                break;
            default:
                fMult = 0.0f;
                break;
            }
        }
        PLAYER(i)->nD28[Game_CurHoleIndex()] = fMult * PLAYER(i)->nD28[Game_CurHoleIndex()];
        fn_800E4364(0, 0x73, PLAYER(i)->nD28[Game_CurHoleIndex()], PLAYER(i)->nIndex);
    }
}

// Game finished: each human with a profile is paid the round's points.
void fn_800FF288(void) {
    int i;
    int h;
    s32 nMoney;
    if (fn_800E1BBC()) {
        for (i = 0; i < gNumPlayersSetUp; i++) {
            nMoney = 0;
            if (!Player_IsCPU(i)) {
                for (h = 0; h < 18; h++) {
                    nMoney += PLAYER(i)->nD28[h];
                }
            }
            if (gpSaveData[PLAYER(i)->nIndex].bActive) {
                if (nMoney) {
                    fn_800E4364(0, 0x6A, nMoney, PLAYER(i)->nIndex);
                }
                fn_800D3548(i, nMoney, 0);
            }
        }
    }
}

// Hole start: the bonus meters empty.
void fn_800FF3D4(void) {
    int i;
    i = 0;
    while (i < 5) {
        gPlayers[i++].nD24 = 0;
    }
}

// Round setup: every player's surfaces scored, points and multiplier reset.
void fn_800FF3F8(void) {
    int i;
    int j;
    for (i = 0; i < 5; i++) {
        PLAYER(i)->nCD0 = 0;
        PLAYER(i)->nDBC = 1;
        PLAYER(i)->nDB8 = 0;
        for (j = 0; j < 20; j++) {
            PLAYER(i)->aCD4[j] = 0;
        }
    }
    fn_800FF7DC();
}

s32 fn_800FF49C(int nPlayer) {
    return gPlayers[nPlayer].nDBC;
}

s32 fn_800FF4B4(int nPlayer) {
    return gPlayers[nPlayer].nD24;
}

// The player's points on this hole.
s32 fn_800FF4CC(int nPlayer) {
    return gPlayers[nPlayer].nD28[Game_CurHoleIndex()];
}

// The player's points for the round.
s32 fn_800FF514(int nPlayer) {
    s32 n = 0;
    int h;
    for (h = 0; h < 18; h++) {
        n += gPlayers[nPlayer].nD28[h];
    }
    return n;
}

// End of a turn: the shot's points go to the hole.
void fn_800FF5B4(int nPlayer) {
    gPlayers[nPlayer].nD28[Game_CurHoleIndex()] += gPlayers[nPlayer].nDB8;
}

s32 fn_800FF604(void) {
    return lbl_802823D8;
}

s32 fn_800FF60C(int nPlayer, int i) {
    return lbl_80212468[i];
}

s32 fn_800FF620(int nPlayer, int i) {
    return lbl_80212418[i];
}

// The HUD's list of the surfaces the player has scored on, and how many times.
void fn_800FF634(int nPlayer) {
    int i;
    s32 n;
    lbl_802823D8 = 0;
    for (i = 0; i < 20; i++) {
        n = fn_800FEFF8(nPlayer, lbl_80200538.aMini[i].nId);
        if (n != 0) {
            lbl_80212468[lbl_802823D8] = lbl_80200538.aMini[i].nId;
            lbl_80212418[lbl_802823D8] = n;
            lbl_802823D8++;
        }
    }
}

void fn_800FF6C0(int nPlayer) {
    if (gPlayers[nPlayer].swing.nState != 0) {
        fn_800E58B4(0x36);
    }
}
