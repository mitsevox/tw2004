// GameMode6.c: the setup of game mode 6, two-player speed golf at match play, where the first to
// hole out wins the hole. The rest of the mode is speed golf's code in GameMode8.c and the hole start
// in GameMode7.c.

#include "golfer.h"
#include "game.h"

// Game mode 6 starts (GM_SetModeType): speed golf's shared callbacks with mode 6's own hole and
// game end (SpeedGolfMatch_*). Unlike modes 7 and 8, speed golf's golfer states go straight in as
// a round starts, without switching on the first hole's run tips, and there is no every-frame
// update, so no starting a hole again from the tee. The stroke limit stays on; no gimmes,
// mulligans, hole restart, flyovers, caddie tips, GameBreakers, flight speed buttons, course
// sounds or golfer animations before or after the shot. Holes won are the score; two players,
// with split screen as last chosen; the event log starts again.
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
    gpGame->bAllowHoleRestart = 0;
    gpGame->bDoFlyBy = 0;
    gpGame->bDoMidholeFlyBy = 0;
    gpGame->bAllowCaddieTips = 0;
    gpGame->bAllowGameBreakers = 0;
    gpGame->bAllowSpeedSlowButtons = 0;
    gpGame->bAllowEnvironmentSounds = 0;
    gpGame->nPreShotAnimationsMode = GM_ANIMS_OFF;
    gpGame->nPostShotAnimationsMode = GM_ANIMS_OFF;
    gpGame->nMulligans = GM_MULLIGANS_NONE;
    gpGame->nMinGolfers = 2;
    gpGame->nMaxGolfers = 2;
    gpGame->nCurrentRound = 0;
    gpGame->nScoringType = GM_SCORING_HOLES_WON;
    gSession.nSplitScreen = gSplitScreen;
    Session_SetNumPlayers(2);
    gSpeedGolfUnused = 0;
    gSpeedGolfEventLogCount = 0;
}
