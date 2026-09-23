// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game.h"

int fn_800D30B4(int nRound);

int fn_800D3118(int nRound, int nHole) {
    return lbl_801FA1F8[fn_800D30B4(nRound)].aHoles[nHole].nCourse;
}
