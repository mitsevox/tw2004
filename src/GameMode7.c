// GameMode7.c (our name): game mode 7, a two-player mode on the shared head-to-head code (see
// GameMode6.c); it sets more of the callbacks than mode 6.

#include "golfer.h"
#include "game.h"

extern u8  lbl_8028227C;
extern u8  lbl_802823C9;
extern s32 lbl_802823D0;
extern s32 lbl_802823D4;

void  fn_800F9844(void);
u8    fn_800FA148(int a);
s32   fn_800FA2C8(void);
void  fn_800FA3AC(void);
void  fn_800FA410(void);
void  fn_800FA570();
void  fn_800FDF38(void);
void  fn_800FA998(int nPlayer);

void fn_800F9610(void) {
    gpGame->pfn1C8 = fn_800F9610;
    gpGame->pfn1CC = fn_800F9A58;
    gpGame->pfn1EC = fn_800F9844;
    gpGame->pfn1D0 = fn_800F9B34;
    gpGame->pfn1D4 = fn_800F9BF8;
    gpGame->pfn1D8 = fn_800FA118;
    gpGame->pfn1DC = fn_800FA148;
    gpGame->pfn1E0 = fn_800FA2C8;
    gpGame->pfn1E8 = fn_800FA3AC;
    gpGame->pfn1F4 = fn_800FA410;
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
