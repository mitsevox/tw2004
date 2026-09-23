// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_800B051C();
void fn_800B65C0();

void fn_800B6844(s32 p0, s32 p1, s32 p2);
void fn_800B68B4(s32 p0, s32 p1, s32 p2);
void fn_800B6844(s32 p0, s32 p1, s32 p2) {
    fn_800B051C(p0, p2, 0);
    fn_800B65C0(p0, p1, p2, 0, 1, 0, 0, 0);
}

void fn_800B68B4(s32 p0, s32 p1, s32 p2) {
    fn_800B051C(p0, p2, 1);
    fn_800B65C0(p1, p0, p2, 1, 1, 0, 0, 0);
}
