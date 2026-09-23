// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game.h"

int fn_800D2ABC(int nCourse, int nHole) {
    return lbl_801FA2F4[nCourse].aHoles[nHole].nPar;
}

int fn_800D2AD8(int nHole) {
    return lbl_801FA2F4[gpGame->nHoleCourse[nHole]].aHoles[gpGame->nHoleNum[nHole]].nPar;
}
