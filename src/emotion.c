// emotion.c (TW06's emotion.c, golf/ai/emotion.c): the golfers' emotions. After each shot a
// player's PlayerEmotion (golfer.h) records how the shot turned out, and that picks the reaction
// the golfer plays. Only part of the file is decompiled so far.

#include "game.h"
#include "physics.h"

int  fn_8006AA70(int nPlayer);
int  fn_8006AA84(int nPlayer);
void fn_8006ACE0(int nPlayer, int nResult);
void fn_8006B250(int nPlayer);
void fn_8006B57C(int nPlayer);
void fn_8006B6D0(int nPlayer);
void fn_8006B6F0(int nPlayer);
void fn_8006B87C(int nPlayer);
u8   fn_8006BAD8(int nPlayer, s32* pOut);
u8   fn_8006BDC8(int nPlayer, u8 bBefore);
void fn_8006BE80(f32* pA, f32* pB, f32* pOut);
u8   fn_8006BEA4(void);

int fn_8006AA70(int nPlayer) {
    return lbl_801D5F78[nPlayer].n0;
}

int fn_8006AA84(int nPlayer) {
    return lbl_801D5F78[nPlayer].n4;
}

int fn_8006AA9C(int nPlayer) {
    return lbl_801D5F78[nPlayer].n14;
}

// Records how a shot turned out. The mode is asked about the shot with the hole's stroke count
// one lower; if it says so and the ball is in the cup, the outcome becomes 2.
void fn_8006AAB4(int nPlayer, int nResult) {
    gPlayers[nPlayer].nStrokes[gpGame->nCurHole]--;
    if (gpGame->pfn1FC(nPlayer) && gPlayers[nPlayer].ball.nLie == LIE_INCUP_e) {
        nResult = 2;
    }
    gPlayers[nPlayer].nStrokes[gpGame->nCurHole]++;
    lbl_801D5F78[nPlayer].n8 = nResult;

    switch (nResult) {
    case 0:
    case 1:
    case 2:
        lbl_801D5F78[nPlayer].n0 = 0;
        break;
    case 3:
    case 4:
    case 6:
    case 7:
        lbl_801D5F78[nPlayer].n0 = 1;
        break;
    }

    switch (nResult) {
    case 5:
        lbl_801D5F78[nPlayer].n4 = 0;
        break;
    case 0:
        lbl_801D5F78[nPlayer].n4 = 1;
        break;
    case 1:
        lbl_801D5F78[nPlayer].n4 = 2;
        break;
    case 2:
        lbl_801D5F78[nPlayer].n4 = 3;
        break;
    }

    lbl_801D5F78[nPlayer].n14 = lbl_801D5F78[nPlayer].n8;
    lbl_801D5F78[nPlayer].nC = lbl_801D5F78[nPlayer].n0;
    lbl_801D5F78[nPlayer].n10 = lbl_801D5F78[nPlayer].n4;

    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)
            && gPlayers[nPlayer].ball.nLie == LIE_INCUP_e) {
        lbl_801D5F78[nPlayer].n8 = 2;
        lbl_801D5F78[nPlayer].n14 = 2;
        lbl_801D5F78[nPlayer].nC = 0;
        lbl_801D5F78[nPlayer].n10 = 3;
        lbl_801D5F78[nPlayer].n0 = 0;
        lbl_801D5F78[nPlayer].n4 = 3;
    }
}

void fn_8006ACE0(int nPlayer, int nResult) {
    lbl_801D5F78[nPlayer].n14 = nResult;
}

// Sets the reaction to play. Reaction 1 does not replace reaction 2; a change to anything but
// 4 or 5 updates the golfer's emotion at once.
void fn_8006ACF8(int nPlayer, int a) {
    int nOld = lbl_801D5F78[nPlayer].n18;

    if (a == 1) {
        if (lbl_801D5F78[nPlayer].n18 != 2) {
            lbl_801D5F78[nPlayer].n18 = a;
        }
    } else {
        lbl_801D5F78[nPlayer].n18 = a;
    }
    if (a != 4 && a != 5 && nOld != lbl_801D5F78[nPlayer].n18) {
        Emotion_UpdatePlayerEmotion(nPlayer);
    }
}

void fn_8006B250(int nPlayer) {
    switch (lbl_801D5F78[nPlayer].n18) {
    case 0:
        fn_8006B57C(nPlayer);
        break;
    case 1:
        fn_8006B6D0(nPlayer);
        break;
    case 2:
        fn_8006B6F0(nPlayer);
        break;
    case 3:
        fn_8006B87C(nPlayer);
        break;
    }
}

void fn_8006BAA8(int nPlayer) {
    lbl_801D5F78[nPlayer].b1C = 0;
    lbl_801D5F78[nPlayer].b1D = 0;
    lbl_801D5F78[nPlayer].b1E = 0;
    lbl_801D5F78[nPlayer].b1F = 0;
    lbl_801D5F78[nPlayer].n20 = -1;
}

u8 fn_8006BAD8(int nPlayer, s32* pOut) {
    PlayerEmotion* pEmotion = &lbl_801D5F78[nPlayer];
    u8 bResult = 0;
    int n;

    if (!pEmotion->b1C) {
        *pOut = -1;
    } else {
        if (pEmotion->n20 != -1) {
            n = 3 - pEmotion->n20;
        } else {
            n = 0;
        }
        if (pEmotion->b1D) {
            *pOut = n;
            bResult = 1;
        } else if (pEmotion->b1E) {
            *pOut = n;
            pEmotion->b1E = 0;
        } else {
            *pOut = -1;
        }
    }
    return bResult;
}

// With bBefore, whether the shot about to be taken would count for the mode (fn_800E23B0 with one
// more stroke), unless the ball is already in the cup; without it, the player's bC2D.
u8 fn_8006BDC8(int nPlayer, u8 bBefore) {
    if (bBefore) {
        if (gPlayers[nPlayer].ballBefore.nLie != LIE_INCUP_e) {
            return fn_800E23B0(nPlayer, gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1) != 0;
        }
        return 0;
    }
    return gPlayers[nPlayer].bC2D != 0;
}

// a - b into out (three floats)
#ifdef __MWERKS__
asm void fn_8006BE80(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_8006BE80(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// A scripted GameBreaker is on screen.
u8 fn_8006BEA4(void) {
    return lbl_80202898.bGameBreaker && lbl_80202898.nGBType == 0;
}
