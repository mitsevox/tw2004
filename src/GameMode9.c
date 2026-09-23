// GameMode9.c (our name): game mode 9, one player playing holes with no opponent and a restart
// option; probably TW06's GameModePractice. It borrows mode 0's GetHonors and GoToPlayoff
// (fn_800FF894, fn_800FFDB0).

#include "golfer.h"
#include "game.h"
#include "engine.h"

extern u8 lbl_80282330;                     // the hole is being restarted

void fn_800ED890(void);
void fn_800ED8B8(void);
void fn_800ED8E0(void);
u8   fn_800ED900(void);
u8   fn_800ED908(int nPlayer, u8 bCheck);
u8   fn_800ED9AC(u8 bCheck);
void fn_800ED9A0(void);
void fn_800EDA08(void);
void fn_800EDA34(int nPlayer);
void fn_800EDA74(void);

// Mode 9 starts: one player, one mulligan, no gimmes, no GameBreakers.
void fn_800ED738(void) {
    gpGame->pfnInit = fn_800ED738;
    gpGame->pfnSetupNextGolfer = fn_800ED8E0;
    gpGame->pfnGetHonors = fn_800FF894;
    gpGame->pfnHoleFinished = fn_800ED908;
    gpGame->pfnGameFinished = fn_800ED9AC;
    gpGame->pfnGoToPlayoff = fn_800FFDB0;
    gpGame->pfnEndGame = fn_800EDA08;
    gpGame->pfn1E4 = fn_800ED890;
    gpGame->pfn224 = fn_800ED8B8;
    gpGame->pfn210 = fn_800EDA34;
    gpGame->bStrokeLimit = 0;
    gpGame->b275 = 0;
    gpGame->bGimmesAllowed = 0;
    gpGame->b27C = 0;
    gpGame->b27D = 0;
    gpGame->b27F = 0;
    gpGame->b280 = 1;
    gpGame->b285 = 0;
    gpGame->n290 = 0;
    gpGame->n294 = 0;
    gpGame->n4 = 0;
    gpGame->nMulligans = 1;
    gpGame->nC = 1;
    gpGame->n10 = 1;
    gpGame->nDC = 0;
    fn_800E1480(0);
    lbl_80282330 = 0;
    gSession.nSplitScreen = 0;
}

// Hole start: the ball is placed, the HUD hides.
void fn_800ED890(void) {
    fn_800EDA74();
    fn_800E3C0C(0);
}

// Hole restart: the same.
void fn_800ED8B8(void) {
    fn_800EDA74();
    fn_800E3C0C(0);
}

void fn_800ED8E0(void) {
    fn_800EDA74();
}

u8 fn_800ED900(void) {
    return lbl_80282330;
}

// Hole finished: on a restart, or once every player has holed out.
u8 fn_800ED908(int nPlayer, u8 bCheck) {
    int i;
    if (fn_800ED900()) {
        return 1;
    }
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (!Player_IsHoled(i)) {
            return 0;
        }
    }
    return 1;
}

// Restart the hole (from the pause menu).
void fn_800ED974(void) {
    lbl_80282330 = 1;
    GM_EndOfGolferTurn(0);
}

void fn_800ED9A0(void) {
    lbl_80282330 = 0;
}

// Game finished: no selected hole is left.
u8 fn_800ED9AC(u8 bCheck) {
    int h;
    for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            return 0;
        }
    }
    return 1;
}

// Game over: the end-of-hole screen.
void fn_800EDA08(void) {
    fn_80125910(1);
    fn_800E4D94(0);
}

// The hole is over: after a restart the game unpauses; otherwise the end-of-round screen.
void fn_800EDA34(int nPlayer) {
    if (fn_800ED900()) {
        fn_800E4D88();
        fn_800E3EE0();
    } else {
        fn_800E4C20(0);
    }
    fn_800ED9A0();
}

// The player goes to "place ball", and the camera moves.
void fn_800EDA74(void) {
    f32 v[4] = {0.0f, 0.0f, 0.0f, 0.5f};
    GOLFERSTATE_Set(GS_PLACE_BALL, 0);
    fn_80063B98(fn_80017028(gPlayers[0].nView[0]), 0.5f, v);
}

// The pad's sticks (beyond the 96..160 dead zone) scaled to -1..1 into the player's fA7C..fA84.
void fn_800EDAE0(int nPlayer) {
    u8* pPad = fn_800136C4(gPlayers[nPlayer].nController);
    if (pPad) {
        if (pPad[3] < 96.0f) {
            gPlayers[nPlayer].fA84 = (96.0f - pPad[3]) / 96.0f;
        }
        if (pPad[3] > 160.0f) {
            gPlayers[nPlayer].fA84 = (160.0f - pPad[3]) / 96.0f;
        }
        if (pPad[2] < 96.0f) {
            gPlayers[nPlayer].fA80 = (96.0f - pPad[2]) / 96.0f;
        }
        if (pPad[2] > 160.0f) {
            gPlayers[nPlayer].fA80 = (160.0f - pPad[2]) / 96.0f;
        }
        if (pPad[0] < 96.0f) {
            gPlayers[nPlayer].fA7C = -(96.0f - pPad[0]) / 96.0f;
        }
        if (pPad[0] > 160.0f) {
            gPlayers[nPlayer].fA7C = -(160.0f - pPad[0]) / 96.0f;
        }
    }
}
