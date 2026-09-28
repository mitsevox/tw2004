// GameMode7.c (our name): game mode 7, a two-player mode on the shared head-to-head code (see
// GameMode6.c); it sets more of the callbacks than mode 6.

#include "golfer.h"
#include "game.h"

void fn_800F9610(void) {
    gpGame->pfnInit = fn_800F9610;
    gpGame->pfnShutdown = SpeedGolf_Shutdown;
    gpGame->pfn1EC = fn_800F9844;
    gpGame->pfnSetupNextGolfer = SpeedGolf_SetupNextGolfer;
    gpGame->pfnGetHonors = SpeedGolf_GetHonors;
    gpGame->pfnHoleFinished = SpeedGolfPoints_HoleFinished;
    gpGame->pfnGameFinished = SpeedGolfPoints_GameFinished;
    gpGame->pfnGoToPlayoff = SpeedGolf_GoToPlayoff;
    gpGame->pfnEndHole = SpeedGolfPoints_EndHole;
    gpGame->pfnEndGame = SpeedGolf_EndGame;
    gpGame->pfn1E4 = fn_800F9824;
    gpGame->pfn220 = fn_800FDF38;
    gpGame->pfn228 = SpeedGolfPoints_ClearStartFlags;
    gpGame->pfn230 = fn_800FDF58;
    gpGame->pfn234 = fn_800FDF60;
    gpGame->pfn25C = fn_800FDA30;
    gpGame->b271 = 0;
    gpGame->bStrokeLimit = 0;
    gpGame->b273 = 0;
    gpGame->b277 = 0;
    gpGame->bGimmesAllowed = 0;
    gpGame->b279 = 0;
    gpGame->b27E = 0;
    gpGame->b27F = 0;
    gpGame->b280 = 0;
    gpGame->b281 = 0;
    gpGame->b282 = 0;
    gpGame->b283 = 0;
    gpGame->b285 = 0;
    gpGame->b286 = 0;
    gpGame->b288 = 0;
    gpGame->n290 = 0;
    gpGame->n294 = 0;
    gpGame->nMulligans = 0;
    gpGame->n10 = 2;
    gpGame->nC = 2;
    gpGame->nDC = 0;
    gpGame->n4 = 0;
    gSpeedGolfUnused = 0;
    gSession.nSplitScreen = lbl_8028227C;
    gSpeedGolfEventLogCount = 0;
    Session_SetNumPlayers(2);
}

void fn_800F9824(void) {
    SpeedGolf_StartHole();
}

void fn_800F9844(void) {
    gSpeedGolfFirstHoleTips = 1;
    SpeedGolf_SetGolferStates();
}
