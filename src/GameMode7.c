// GameMode7.c (our name): game mode 7, a two-player mode on the shared head-to-head code (see
// GameMode6.c); it sets more of the callbacks than mode 6.

#include "golfer.h"
#include "game.h"

void fn_800F9610(void) {
    gpGame->pfnInit = fn_800F9610;
    gpGame->pfnShutdown = fn_800F9A58;
    gpGame->pfn1EC = fn_800F9844;
    gpGame->pfnSetupNextGolfer = fn_800F9B34;
    gpGame->pfnGetHonors = fn_800F9BF8;
    gpGame->pfnHoleFinished = fn_800FA118;
    gpGame->pfnGameFinished = fn_800FA148;
    gpGame->pfnGoToPlayoff = fn_800FA2C8;
    gpGame->pfnEndHole = fn_800FA3AC;
    gpGame->pfnEndGame = fn_800FA410;
    gpGame->pfn1E4 = fn_800F9824;
    gpGame->pfn220 = fn_800FDF38;
    gpGame->pfn228 = fn_800FA998;
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
    lbl_802823D4 = 0;
    gSession.nSplitScreen = lbl_8028227C;
    lbl_802823D0 = 0;
    Session_SetNumPlayers(2);
}

void fn_800F9824(void) {
    fn_800FA570();
}

void fn_800F9844(void) {
    lbl_802823C9 = 1;
    fn_800F9AB0();
}
