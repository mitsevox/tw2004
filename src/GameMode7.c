// GameMode7.c (our name): game mode 7, a two-player mode on the shared head-to-head code (see
// GameMode6.c); it sets more of the callbacks than mode 6.

#include "golfer.h"
#include "game.h"

// Game mode 7, two-player speed golf on event points, starts: speed golf's shared callbacks
// (GameMode8.c) with mode 7's own hole and game end (SpeedGolfPoints_*), the every-frame update
// (SpeedGolf_Update) and SpeedGolfPoints_ClearStartFlags each frame of the swing (pfn228); the
// golfer states go in through SpeedGolf_StartGamePreData. No stroke limit, gimmes or mulligans;
// b271, b273, b277, b279, b27E to b283, b285, b286, b288, n290 and n294 0; gpGame n10 and nC 2, n4
// 0 (points, not holes won, in SpeedGolf_EndGame), nDC 0; split screen from lbl_8028227C; two
// players; the event log starts again.
void SpeedGolfPoints_Init(void) {
    gpGame->pfnInit = SpeedGolfPoints_Init;
    gpGame->pfnShutdown = SpeedGolf_Shutdown;
    gpGame->pfn1EC = SpeedGolf_StartGamePreData;
    gpGame->pfnSetupNextGolfer = SpeedGolf_SetupNextGolfer;
    gpGame->pfnGetHonors = SpeedGolf_GetHonors;
    gpGame->pfnHoleFinished = SpeedGolfPoints_HoleFinished;
    gpGame->pfnGameFinished = SpeedGolfPoints_GameFinished;
    gpGame->pfnGoToPlayoff = SpeedGolf_GoToPlayoff;
    gpGame->pfnEndHole = SpeedGolfPoints_EndHole;
    gpGame->pfnEndGame = SpeedGolf_EndGame;
    gpGame->pfn1E4 = SpeedGolf_LoadHole;
    gpGame->pfn220 = SpeedGolf_Update;
    gpGame->pfn228 = SpeedGolfPoints_ClearStartFlags;
    gpGame->pfn230 = SpeedGolf_RenderBallTarget;
    gpGame->pfn234 = SpeedGolf_CheckControllerPulled;
    gpGame->pfn25C = SpeedGolf_SetHoleTime;
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

// The hole-start hook (pfn1E4) of modes 6, 7 and 8: SpeedGolf_StartHole.
void SpeedGolf_LoadHole(void) {
    SpeedGolf_StartHole();
}

// As a round starts (pfn1EC of modes 7 and 8; mode 6 calls SpeedGolf_SetGolferStates alone): the
// first hole's run tips are switched on (gSpeedGolfFirstHoleTips) and speed golf's golfer states go
// in.
void SpeedGolf_StartGamePreData(void) {
    gSpeedGolfFirstHoleTips = 1;
    SpeedGolf_SetGolferStates();
}
