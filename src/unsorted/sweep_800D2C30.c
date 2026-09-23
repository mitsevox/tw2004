// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game.h"

s32 fn_800D2B80(int nCourse, int nHole, int nTee);

s32 fn_800D2C30(int nHole, int nTee);
s32 fn_800D2C68(int nTee);
s32 fn_800D2C30(int nHole, int nTee) {
    return fn_800D2B80(gpGame->nHoleCourse[nHole], gpGame->nHoleNum[nHole], nTee);
}

s32 fn_800D2C68(int nTee) {
    return fn_800D2B80(Game_GetCourse(), fn_80015464(), nTee);
}
