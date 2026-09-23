// GameRound.c (our name): the round and hole setup that follows GameManager.c - setting up a
// hole, the stroke limit, created-course data, hole names. TW06 has no counterpart file.

#include "golfer.h"

void fn_800E19A4(int nPlayer, int nHoles);
int  fn_800E1CE8(int a, int b);
u8   fn_800588F4(u8* pProfile, int a, int i);

extern u8* gpSaveData;
extern u8  lbl_8028227C;

// The mode's mulligan rule: 0 none, 2 one per player per round.
int fn_800E177C(void) {
    return gpGame->nMulligans;
}

void fn_800E1788(int nPlayer) {
    fn_800E19A4(nPlayer, 18);
}

int fn_800E22E4(int nSlot, int a, int b) {
    int i = fn_800E1CE8(a, b);
    if (i != -1) {
        return fn_800588F4(gpSaveData + nSlot * 0x10600, 0, i);
    }
    return 0;
}

u8 fn_800E234C(int nSlot, int a, int b) {
    int i = fn_800E1CE8(a, b);
    if (i != -1) {
        return fn_800588F4(gpSaveData + nSlot * 0x10600, 1, i);
    }
    return 0;
}

void fn_800E25CC(u8 b) {
    lbl_8028227C = b;
    gSession.nSplitScreen = b;
}

int fn_800E27A8(void) {
    return gpGame->n294 != 0;
}
