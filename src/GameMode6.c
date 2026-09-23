// GameMode6.c (our name): game mode 6, a two-player mode that runs on the shared head-to-head code
// after mode 8's setup (0x800F986C on). Only the setup is here.

#include "golfer.h"

void  Session_SetNumPlayers(int n);
extern u8  lbl_8028227C;
extern s32 lbl_802823D0;
extern s32 lbl_802823D4;

void  fn_800F9824(void);
void  fn_800F9A58(void);
void  fn_800F9AB0(void);
void  fn_800F9B34(void);
s32   fn_800F9BF8(int nPlayer);
u8    fn_800F9C00(int nPlayer, int a);
void  fn_800F9C48(void);
u8    fn_800F9D00(int a);
void  fn_800F9E00(void);
s32   fn_800F9F04(void);
u8    fn_800FDF58(int nPlayer);
u8    fn_800FDF60(void);
void  fn_800FDA30(int nPlayer, int a);

void fn_800F944C(void) {
    gpGame->pfn1C8 = fn_800F944C;
    gpGame->pfn1CC = fn_800F9A58;
    gpGame->pfn1EC = fn_800F9AB0;
    gpGame->pfn1D0 = fn_800F9B34;
    gpGame->pfn1D4 = fn_800F9BF8;
    gpGame->pfn1D8 = fn_800F9C00;
    gpGame->pfn1DC = fn_800F9D00;
    gpGame->pfn1E0 = fn_800F9F04;
    gpGame->pfn1E8 = fn_800F9C48;
    gpGame->pfn1F4 = fn_800F9E00;
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
