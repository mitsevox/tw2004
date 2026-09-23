// GameMode12.c (our name): game mode 12, stroke play with points. A shot that stops on a special
// surface (a row of the prize table at lbl_80200538 + 0x710) scores points, times the shot's
// multiplier; each hole's points (nD28) are then scaled by the score against par, and a human
// player is paid the round's total. The honors, hole-finished and game-finished callbacks are
// mode 0's.

#include "golfer.h"
#include "game.h"

s32   fn_800FF894(int nPlayer);
u8    fn_800FFCCC(int nPlayer, u8 bCheck);
u8    fn_800FFD54(u8 bCheck);
extern u8  gNumPlayersSetUp;                // 0x80281D48 (Golfer.c)
extern u8* gpSaveData;
extern s32 lbl_802823D8;
extern s32 lbl_802823DC;                    // the surface the ball stopped on (-1: none)
extern u8  lbl_80212418[];
extern u8  lbl_80212468[];

s32  fn_800FEC78(void);
void fn_800FEC80(int nPlayer);
s32  fn_800FF038(int nPlayer);
void fn_800FF114(void);
void fn_800FF288(void);
void fn_800FF3D4(void);
void fn_800FF3F8(void);
void fn_800FF5B4(int nPlayer);
void fn_800FF6C0(int nPlayer);

// Mode 12 starts: stroke play with mode 0's turn order, one mulligan per player per round.
void fn_800FEAFC(void) {
    gpGame->pfn1C8 = fn_800FEAFC;
    gpGame->pfn1D4 = fn_800FF894;
    gpGame->pfn1D8 = (u8 (*)(int, int))fn_800FFCCC;
    gpGame->pfn1DC = (u8 (*)(int))fn_800FFD54;
    gpGame->pfn1E0 = fn_800FEC78;
    gpGame->pfn1F4 = fn_800FF288;
    gpGame->pfn23C = fn_800FEC80;
    gpGame->pfn240 = (s32 (*)(void))fn_800FF038;
    gpGame->pfn1E8 = fn_800FF114;
    gpGame->pfn1E4 = fn_800FF3D4;
    gpGame->pfn1D0 = fn_800FF3F8;
    gpGame->pfn244 = fn_800FF5B4;
    gpGame->pfn228 = fn_800FF6C0;
    gpGame->b271 = 0;
    gpGame->b281 = 0;
    gpGame->bStrokeLimit = 0;
    gpGame->b288 = 0;
    gpGame->n4 = 0;
    gpGame->nMulligans = 1;
    gpGame->nC = 4;
    gpGame->n10 = 1;
    gpGame->nDC = 0;
    fn_800E1480(0);
    lbl_802823DC = -1;
    gSession.nSplitScreen = 0;
    SESSION_OPTIONS->unkC = 0;
}

s32 fn_800FEC78(void) {
    return 0;
}

// How many times the player has scored on the surface nSurface.
s32 fn_800FEFF8(int nPlayer, s32 nSurface) {
    s32 n = 0;
    int i;
    for (i = 0; i < gPlayers[nPlayer].nCD0; i++) {
        if (nSurface == gPlayers[nPlayer].aCD4[i]) {
            n++;
        }
    }
    return n;
}

// Game finished: each human with a profile is paid the round's points.
void fn_800FF288(void) {
    int i;
    int h;
    s32 nMoney;
    if (fn_800E1BBC()) {
        for (i = 0; i < gNumPlayersSetUp; i++) {
            nMoney = 0;
            if (!Player_IsCPU(i)) {
                for (h = 0; h < 18; h++) {
                    nMoney += PLAYER(i)->nD28[h];
                }
            }
            if (gpSaveData[PLAYER(i)->nIndex * 0x10600]) {
                if (nMoney) {
                    fn_800E4364(0, 0x6A, nMoney, PLAYER(i)->nIndex);
                }
                fn_800D3548(i, nMoney, 0);
            }
        }
    }
}

// Round setup: every player's surfaces scored, points and multiplier reset.
void fn_800FF3F8(void) {
    int i;
    int j;
    for (i = 0; i < 5; i++) {
        PLAYER(i)->nCD0 = 0;
        PLAYER(i)->nDBC = 1;
        PLAYER(i)->nDB8 = 0;
        for (j = 0; j < 20; j++) {
            PLAYER(i)->aCD4[j] = 0;
        }
    }
    fn_800FF7DC();
}

// The player's points for the round.
s32 fn_800FF514(int nPlayer) {
    s32 n = 0;
    int h;
    for (h = 0; h < 18; h++) {
        n += gPlayers[nPlayer].nD28[h];
    }
    return n;
}

s32 fn_800FF604(void) {
    return lbl_802823D8;
}

s32 fn_800FF60C(s32 p0, s32 p1) {
    return *(s32*)(lbl_80212468 + (p1 << 2));
}

s32 fn_800FF620(s32 p0, s32 p1) {
    return *(s32*)(lbl_80212418 + (p1 << 2));
}
