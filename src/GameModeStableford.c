// GameModeStableford.c (TW06's GameModeStableford): game mode 18, modified Stableford. Each hole
// scores points by strokes against par from lbl_802816D0 (albatross 8, eagle 5, birdie 2, par 0,
// bogey -1, double bogey or worse -3; the points go in nModePoints). A player who can no longer
// beat double bogey is done with the hole.

#include "golfer.h"
#include "game.h"

int   fn_800E1788(int nPlayer);             // the player's round total
int   fn_800D3C7C(int nPlayer);             // the player's golfer
void  fn_80125854(int a);
extern u8  gNumPlayersSetUp;                // 0x80281D48 (Golfer.c)
extern s32 lbl_80282278;                    // the player whose turn it is
// The points for 3 under par .. 2 or more over. TW06: GameModeStableford::stablefordPointTable.
extern s8  lbl_802816D0[6];
extern u8* gpSaveData;

// Per golfer (gGolferTable's 34): the prize for beating them (lbl_80200538 + 0x1D4, 8 bytes each).
typedef struct GolferPrize {
    s32 nBase;
    s32 nPerStroke;
} GolferPrize;
typedef struct PrizeTable {
    u8          unk0[0x1D4];
    GolferPrize prize[34];      // 0x1D4
} PrizeTable;
extern PrizeTable lbl_80200538;
#define GOLFER_PRIZE(n) lbl_80200538.prize[n]

u8   fn_800FE2B4(int nPlayer);
void fn_800FE344(void);
s32  fn_800FE3FC(int nPlayer);
u8   fn_800FE7EC(int nPlayer, u8 bCheck);
u8   fn_800FE844(u8 bCheck);
u8   fn_800FE8A0(u8 bCheck);
void fn_800FE8A8(void);
void fn_800FE980(void);

// TW06: GameModeStableford::Init. One mulligan per player per round; the CPU may concede.
void fn_800FE1B4(void) {
    gpGame->pfn1C8 = fn_800FE1B4;
    gpGame->pfn1D0 = fn_800FE344;
    gpGame->pfn1D4 = fn_800FE3FC;
    gpGame->pfn1D8 = (u8 (*)(int, int))fn_800FE7EC;
    gpGame->pfn1DC = (u8 (*)(int))fn_800FE844;
    gpGame->pfn1E0 = (s32 (*)(void))fn_800FE8A0;
    gpGame->pfn1E8 = fn_800FE8A8;
    gpGame->pfn1F4 = fn_800FE980;
    gpGame->bAIConcedes = 1;
    gpGame->n4 = 3;
    gpGame->nMulligans = 2;
    gpGame->nC = 4;
    gpGame->n10 = 1;
    gpGame->nDC = 0;
    fn_800E1480(0);
    gSession.nSplitScreen = 0;
}

// TW06: GameModeStableford::PlayerDoneHole. Holed, cut, or already at par + 1 strokes: the best
// left is double bogey, which scores no better than giving up.
u8 fn_800FE2B4(int nPlayer) {
    if (!Player_IsHoled(nPlayer) && !gPlayers[nPlayer].bPlayerCut &&
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] < fn_800D2B08() + 1) {
        return 0;
    }
    return 1;
}

// TW06: GameModeStableford::SetupNextGolfer. In split screen everyone plays at once; otherwise the
// player with the honor gets ready and the others wait.
void fn_800FE344(void) {
    int i;
    if (gSession.nSplitScreen == 1) {
        for (i = 0; i < gNumPlayersSetUp; i++) {
            GOLFERSTATE_Set(GS_PRE_SHOT, i);
        }
    } else {
        lbl_80282278 = gpGame->pfn1D4(5);
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (i == lbl_80282278) {
                GOLFERSTATE_Set(GS_PRE_SHOT, i);
            } else {
                GOLFERSTATE_Set(GS_WAIT, i);
            }
        }
    }
}

// TW06: GameModeStableford::GetHonors. Who plays next after nPlayer (5 = nobody). On the tee the
// honor goes by the scores on the holes played so far (the latest hole first, ties by the hole
// before); otherwise to the player farthest from the pin, off the green first.
s32 fn_800FE3FC(int nPlayer) {
    s32 aOrder[4] = {0, 1, 2, 3};
    s32 aSorted[4];
    int n;
    int h;
    int i;
    int k;
    s32 nLow;
    s32 nScore;
    s32 nHigh;
    CourseInfo* pCourse;
    int nHole;
    f32 fBest;
    int nBest;
    f32 dx;
    f32 dz;
    f32 d;
    for (h = 0; h < Game_CurHoleIndex(); h++) {
        if (gpGame->bHoleSelected[h]) {
            nLow = gPlayers[0].nStrokes[h];
            nHigh = nLow;
            for (i = 0; i < gNumPlayersSetUp; i++) {
                if (gPlayers[(u32)i].nStrokes[h] < nLow) {
                    nLow = gPlayers[(u32)i].nStrokes[h];
                }
                if (gPlayers[(u32)i].nStrokes[h] > nHigh) {
                    nHigh = gPlayers[(u32)i].nStrokes[h];
                }
            }
            n = 0;
            for (nScore = nLow; nScore <= nHigh; nScore++) {
                for (k = 0; k < gNumPlayersSetUp; k++) {
                    AddIfScore(aSorted, &n, aOrder[k], h, nScore);
                }
            }
            for (i = 0; i < gNumPlayersSetUp; i++) {
                aOrder[i] = aSorted[i];
            }
        }
    }
    for (h = 0; h < gNumPlayersSetUp; h++) {
        if (nPlayer != aOrder[h] && Player_OnTee(aOrder[h]) && !fn_800FE2B4(aOrder[h])) {
            return aOrder[h];
        }
    }
    nBest = 5;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i != nPlayer && !fn_800FE2B4(i)) {
            nBest = i;
            break;
        }
    }
    if (nPlayer == 5 && nBest == 5) {
        return 5;
    }
    pCourse = fn_8000C594();
    nHole = Game_CurrentHole();
    fBest = 0.0f;
    nBest = 5;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i != nPlayer && PLAYER(i)->nLie != LIE_GREEN && !fn_800FE2B4(i)) {
            dx = *(f32*)(PLAYER(i)->ball + 0) - pCourse->pin[nHole].x;
            dz = *(f32*)(PLAYER(i)->ball + 8) - pCourse->pin[nHole].z;
            d = fn_80009680(dx * dx + dz * dz);
            if (d > fBest) {
                fBest = d;
                nBest = i;
            }
        }
    }
    if (nBest == 5) {
        fBest = 0.0f;
        nBest = 5;
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (i != nPlayer && !fn_800FE2B4(i)) {
                dx = *(f32*)(PLAYER(i)->ball + 0) - pCourse->pin[nHole].x;
                dz = *(f32*)(PLAYER(i)->ball + 8) - pCourse->pin[nHole].z;
                d = fn_80009680(dx * dx + dz * dz);
                if (d > fBest) {
                    fBest = d;
                    nBest = i;
                }
            }
        }
    }
    if (nBest == nPlayer) {
        return 5;
    }
    return nBest;
}

// TW06: GameModeStableford::HoleFinished. The hole is over when every player is done with it.
u8 fn_800FE7EC(int nPlayer, u8 bCheck) {
    int i;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (!fn_800FE2B4(i)) {
            return 0;
        }
    }
    return 1;
}

// TW06: GameModeStableford::GameFinished. Over when no selected hole is left.
u8 fn_800FE844(u8 bCheck) {
    int h;
    for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            return 0;
        }
    }
    return 1;
}

// No playoff (the GoToPlayoff slot of GameModeMatch.c).
u8 fn_800FE8A0(u8 bCheck) {
    return 0;
}

// TW06: GameModeStableford::EndHole. A player who did not hole out is scored par + 2 (double
// bogey); every player gets the points for the hole.
void fn_800FE8A8(void) {
    int nHole;
    s32 nPar;
    int i;
    int nDiff;
    nHole = Game_CurHoleIndex();
    nPar = fn_800D2B08();
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (!Player_IsHoled(i)) {
            gPlayers[(u32)i].nStrokes[nHole] = nPar + 2;
        }
        nDiff = gPlayers[(u32)i].nStrokes[nHole] - nPar;
        nDiff = (nDiff < -3) ? -3 : ((nDiff > 2) ? 2 : nDiff);
        gPlayers[(u32)i].nModePoints[nHole] = lbl_802816D0[nDiff + 3];
    }
}

// TW06: GameModeStableford::EndGame (empty there). Each human with a profile who finished the round
// in fewer strokes than a CPU player wins money: the prize of the best such CPU golfer, its base
// plus its per-stroke prize for up to 5 strokes of margin.
void fn_800FE980(void) {
    int i;
    int j;
    int nBest;
    int nOurs;
    int nTheirs;
    int nGolfer;
    int nMoney;
    int nMargin;
    int nBase;
    int nProfile;
    u8 bFirst = 1;
    if (fn_800E1BBC()) {
        switch (fn_800EC550()) {
        case 0:
            break;
        default:
            return;
        }
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (!Player_IsCPU(i)) {
                nBest = -1;
                nOurs = fn_800E1788(i);
                for (j = 0; j < gNumPlayersSetUp; j++) {
                    if (i != j && Player_IsCPU(j)) {
                        nTheirs = fn_800E1788(j);
                        if (nOurs < nTheirs) {
                            nGolfer = fn_800D3C7C(j);
                            if (nGolfer > nBest) {
                                nBest = nGolfer;
                                nMargin = nTheirs - nOurs;
                            }
                        }
                    }
                }
                if (nBest > -1) {
                    if (nMargin > 5) {
                        nMargin = 5;
                    }
                    nBase = GOLFER_PRIZE(nBest).nBase;
                    nProfile = PLAYER(i)->nIndex;
                    nMoney = nBase + GOLFER_PRIZE(nBest).nPerStroke * nMargin;
                    if (gpSaveData[nProfile * 0x10600]) {
                        if (bFirst) {
                            fn_80125854(1);
                            bFirst = 0;
                        }
                        if (nBase) {
                            fn_800E4364(0, 0x75, nBase, nProfile);
                        }
                        fn_800D3548(i, nMoney, 0);
                    }
                }
            }
        }
    }
}
