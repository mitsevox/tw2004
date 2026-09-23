// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800A402C(s32 p0);
void fn_800A4038();
void fn_800A8D88();
void fn_800A8DC8();
void fn_800AD0C4();
void fn_800AD1C4();

s32 fn_800A7A34(s32 p0, s32 p1, s32 p2);
void fn_800A7A98(s32 p0);
s32 fn_800A7A34(s32 p0, s32 p1, s32 p2) {
    fn_800AD0C4();
    fn_800A402C(p2);
    fn_800A8DC8(p0, p1, p2, 0);
    return 1;
}

void fn_800A7A98(s32 p0) {
    fn_800AD1C4();
    fn_800A4038();
    fn_800A8D88(p0);
}
