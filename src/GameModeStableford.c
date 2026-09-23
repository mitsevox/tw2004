// GameModeStableford.c (TW06's GameModeStableford): game mode 18, modified Stableford. Each hole
// scores points by strokes against par from lbl_802816D0 (albatross 8, eagle 5, birdie 2, par 0,
// bogey -1, double bogey or worse -3; the points go in nModePoints). A player who can no longer
// beat double bogey is done with the hole.

#include "golfer.h"

int   Game_CurHoleIndex(void);
u8    Player_IsHoled(int nPlayer);
void  GOLFERSTATE_Set(int nState, int nPlayer);
void  fn_800E1480(int nHole);
extern u8  gNumPlayersSetUp;                // 0x80281D48 (Golfer.c)
extern s32 lbl_80282278;                    // the player whose turn it is
// The points for 3 under par .. 2 or more over. TW06: GameModeStableford::stablefordPointTable.
extern s8  lbl_802816D0[6];

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
    if (!Player_IsHoled(nPlayer) && !gPlayers[nPlayer].unk28C &&
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
