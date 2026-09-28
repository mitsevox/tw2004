// GameMode6.c (our name): the setup of game mode 6, two-player speed golf at match play (the first
// to hole out wins the hole). The rest of the mode is GameMode8.c's speed golf code
// (SpeedGolfMatch_* and the SpeedGolf_* callbacks modes 6, 7 and 8 share) and GameMode7.c's hole
// start.

#include "golfer.h"
#include "game.h"

// Game mode 6, two-player speed golf at match play, starts: speed golf's shared callbacks
// (GameMode8.c) with mode 6's own hole and game end (SpeedGolfMatch_*). Unlike modes 7 and 8, the
// golfer states go in straight away as a round starts (pfnStartGamePreData
// SpeedGolf_SetGolferStates, so the first-hole run tips stay off), and there is no every-frame
// update (pfnUpdate keeps its default), so no restart from the tee. No gimmes or mulligans; the
// stroke limit and b271, b273, b277, b27E, b282, b283 keep GM_SetModeType's 1, and b279, b27F,
// b280, b281, b285, b286, b288, n290 and n294 are 0; gpGame n10 and nC 2, n4 1 (holes won, as mode
// 8), nDC 0; split screen from lbl_8028227C; two players; the event log starts again.
void SpeedGolfMatch_Init(void) {
    gpGame->pfnInit = SpeedGolfMatch_Init;
    gpGame->pfnShutdown = SpeedGolf_Shutdown;
    gpGame->pfnStartGamePreData = SpeedGolf_SetGolferStates;
    gpGame->pfnSetupNextGolfer = SpeedGolf_SetupNextGolfer;
    gpGame->pfnGetHonors = SpeedGolf_GetHonors;
    gpGame->pfnHoleFinished = SpeedGolfMatch_HoleFinished;
    gpGame->pfnGameFinished = SpeedGolfMatch_GameFinished;
    gpGame->pfnGoToPlayoff = SpeedGolfMatch_GoToPlayoff;
    gpGame->pfnEndHole = SpeedGolfMatch_EndHole;
    gpGame->pfnEndGame = SpeedGolfMatch_EndGame;
    gpGame->pfnLoadHole = SpeedGolf_LoadHole;
    gpGame->pfnRenderBallTarget = SpeedGolf_RenderBallTarget;
    gpGame->pfnCheckControllerPulled = SpeedGolf_CheckControllerPulled;
    gpGame->pfnSetTimer = SpeedGolf_SetHoleTime;
    gpGame->bGimmesAllowed = 0;
    gpGame->b279 = 0;
    gpGame->b27F = 0;
    gpGame->b280 = 0;
    gpGame->b281 = 0;
    gpGame->bAllowGameBreakers = 0;
    gpGame->b286 = 0;
    gpGame->b288 = 0;
    gpGame->n290 = 0;
    gpGame->n294 = 0;
    gpGame->nMulligans = 0;
    gpGame->n10 = 2;
    gpGame->nC = 2;
    gpGame->nDC = 0;
    gpGame->n4 = 1;
    gSession.nSplitScreen = lbl_8028227C;
    Session_SetNumPlayers(2);
    gSpeedGolfUnused = 0;
    gSpeedGolfEventLogCount = 0;
}
