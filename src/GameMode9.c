// GameMode9.c (our name; TW06/TW07's GameModePractice, TW07's GameMode_Practice.cpp: TW06's
// GM_ModeType_t has GM_Practice_mode = 9): game mode 9, practice. The chosen holes
// (bHoleSelected) are played with the ball placed by hand before every shot (player 0 goes to
// GS_PLACE_BALL, InitCamera), any number of mulligans, and no GameBreakers, profile statistics or
// flyover at the hole start. The player can end a hole early (FinishHole, from a UI command). It
// borrows mode 0's GetHonors and GoToPlayoff (GameModeStroke_GetHonors, fn_800FFDB0). The pad
// reading for placing the ball in every mode (GameModePractice_ReadPlaceBallSticks) is here too.

#include "golfer.h"
#include "game.h"
#include "engine.h"

u8 gPracticeHoleEndedEarly;     // the player ended the hole early (FinishHole); cleared once the hole
                                //   is over (EndTurnEndHoleNotGame) and by Init

void GameModePractice_LoadHole(void);
void GameModePractice_RestartHole(void);
void GameModePractice_SetupNextGolfer(void);
u8   GameModePractice_IsHoleEndedEarly(void);
u8   GameModePractice_HoleFinished(int nPlayer, u8 bCheck);
u8   GameModePractice_GameFinished(u8 bCheck);
void GameModePractice_ClearHoleEndedEarly(void);
void GameModePractice_EndGame(void);
void GameModePractice_EndTurnEndHoleNotGame(int nPlayer);
void GameModePractice_InitCamera(void);

// Game mode 9's (practice) setup (pfnInit, from GM_SetModeType): its hooks (stroke play's GetHonors
// and fn_800FFDB0 as GoToPlayoff), any number of mulligans (nMulligans 1), no gimmes, no stroke
// limit, no GameBreakers (b285), no scorecards from the game manager (b275; EndGame and
// EndTurnEndHoleNotGame show their own), nothing counted in the profile's statistics (b27C, b27D),
// no flyover at the hole start (b27F) but the mid-hole flyover button on (b280), one view, the
// current hole 0 and the hole not ended early.
void GameModePractice_Init(void) {
    gpGame->pfnInit = GameModePractice_Init;
    gpGame->pfnSetupNextGolfer = GameModePractice_SetupNextGolfer;
    gpGame->pfnGetHonors = GameModeStroke_GetHonors;
    gpGame->pfnHoleFinished = GameModePractice_HoleFinished;
    gpGame->pfnGameFinished = GameModePractice_GameFinished;
    gpGame->pfnGoToPlayoff = fn_800FFDB0;
    gpGame->pfnEndGame = GameModePractice_EndGame;
    gpGame->pfn1E4 = GameModePractice_LoadHole;
    gpGame->pfn224 = GameModePractice_RestartHole;
    gpGame->pfn210 = GameModePractice_EndTurnEndHoleNotGame;
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
    GM_SetCurrentHole(0);
    gPracticeHoleEndedEarly = 0;
    gSession.nSplitScreen = 0;
}

// Hole start (pfn1E4): player 0 goes to placing the ball with the camera faded in (InitCamera) and
// the HUD is hidden.
void GameModePractice_LoadHole(void) {
    GameModePractice_InitCamera();
    GUI_ShowToggleFullScreenUI(0);
}

// The hole restarts (pfn224, GM_RestartHole): as at the hole start, player 0 goes to placing the
// ball (InitCamera) and the HUD is hidden.
void GameModePractice_RestartHole(void) {
    GameModePractice_InitCamera();
    GUI_ShowToggleFullScreenUI(0);
}

// Before each shot (pfnSetupNextGolfer): player 0 goes to placing the ball again (InitCamera), so
// every shot is played from where the player puts the ball.
void GameModePractice_SetupNextGolfer(void) {
    GameModePractice_InitCamera();
}

u8 GameModePractice_IsHoleEndedEarly(void) {
    return gPracticeHoleEndedEarly;
}

// The hole is over when the player ended it early (FinishHole) or once every player has holed out;
// nPlayer and bCheck are not used.
u8 GameModePractice_HoleFinished(int nPlayer, u8 bCheck) {
    int i;
    if (GameModePractice_IsHoleEndedEarly()) {
        return 1;
    }
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (!Player_IsHoled(i)) {
            return 0;
        }
    }
    return 1;
}

// The player ends the hole early (a UI command, fn_800880AC): the ended-early flag is set
// (HoleFinished then says the hole is over) and player 0's turn ends (GM_EndOfGolferTurn).
void GameModePractice_FinishHole(void) {
    gPracticeHoleEndedEarly = 1;
    GM_EndOfGolferTurn(0);
}

void GameModePractice_ClearHoleEndedEarly(void) {
    gPracticeHoleEndedEarly = 0;
}

// The game is over when no hole after the current one is among the chosen holes
// (gpGame->bHoleSelected); bCheck is not used.
u8 GameModePractice_GameFinished(u8 bCheck) {
    int h;
    for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            return 0;
        }
    }
    return 1;
}

// End of the game (pfnEndGame): the game counts as won in the bio (EASBio_SetCurrentGameWon), then
// the end-of-game scorecard (GUI_EndOfGameScorecard).
void GameModePractice_EndGame(void) {
    EASBio_SetCurrentGameWon(1);
    GUI_EndOfGameScorecard(0);
}

// The hole is over and the game is not (pfn210): after an early end (IsHoleEndedEarly) the
// end-of-hole screen is marked up without showing (GUI_SetEndOfHolePending) and the pause menu
// closed, which moves on to the next hole; otherwise the end-of-hole scorecard. The ended-early
// flag is cleared.
void GameModePractice_EndTurnEndHoleNotGame(int nPlayer) {
    if (GameModePractice_IsHoleEndedEarly()) {
        GUI_SetEndOfHolePending();
        GUI_PauseMenuClosed();
    } else {
        GUI_BetweenHolesScorecard(0);
    }
    GameModePractice_ClearHoleEndedEarly();
}

// Player 0's golfer state is set to placing the ball (GS_PLACE_BALL) and their view's camera fades
// in (CameraController_FadeIn, time 0.5, fade colour {0, 0, 0, 0.5}). LoadHole, RestartHole and
// SetupNextGolfer call it.
void GameModePractice_InitCamera(void) {
    f32 vFadeColor[4] = {0.0f, 0.0f, 0.0f, 0.5f};
    GOLFERSTATE_Set(GS_PLACE_BALL, 0);
    CameraController_FadeIn(ViewController_GetCameraControl(gPlayers[0].nView[0]), 0.5f, vFadeColor);
}

// The pad's sticks while the ball is being placed (STATEFUNC_PlaceBallUpdate, in every mode): stick
// bytes 3, 2 and 0 (Input_sGetStickInfo), beyond a 96..160 dead zone and scaled to about -1..1, go
// into fA84, fA80 and fA7C (the last negated), which PlaceBall_UpdateMomentums uses to move the
// cursor and turn its heading. Inside the dead zone a value is left as it was
// (PlaceBall_UpdateMomentums eases it back to 0).
void GameModePractice_ReadPlaceBallSticks(int nPlayer) {
    u8* pPad = Input_sGetStickInfo(gPlayers[nPlayer].nController);
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
