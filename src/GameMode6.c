// GameMode6.c (our name): game mode 6, a two-player mode on GameMode8.c's callbacks (from mode 8's
// setup 0x800F986C on; pfn1E4 is GameMode7.c's fn_800F9824). Only the setup is here.

#include "golfer.h"
#include "game.h"

void fn_800F944C(void) {
    gpGame->pfnInit = fn_800F944C;
    gpGame->pfnShutdown = fn_800F9A58;
    gpGame->pfn1EC = fn_800F9AB0;
    gpGame->pfnSetupNextGolfer = fn_800F9B34;
    gpGame->pfnGetHonors = fn_800F9BF8;
    gpGame->pfnHoleFinished = fn_800F9C00;
    gpGame->pfnGameFinished = fn_800F9D00;
    gpGame->pfnGoToPlayoff = fn_800F9F04;
    gpGame->pfnEndHole = fn_800F9C48;
    gpGame->pfnEndGame = fn_800F9E00;
    gpGame->pfn1E4 = fn_800F9824;
    gpGame->pfn230 = fn_800FDF58;
    gpGame->pfn234 = fn_800FDF60;
    gpGame->pfn25C = fn_800FDA30;
    gpGame->bGimmesAllowed = 0;
    gpGame->b279 = 0;
    gpGame->b27F = 0;
    gpGame->b280 = 0;
    gpGame->b281 = 0;
    gpGame->b285 = 0;
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
    lbl_802823D4 = 0;
    lbl_802823D0 = 0;
}
