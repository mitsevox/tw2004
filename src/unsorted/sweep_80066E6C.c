// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_800A64A8();
void fn_800A6660();

void fn_80066E6C(s32 p0);
void fn_80066E90(s32 p0);
void fn_80066EB8(s32 p0);
void fn_80066E6C(s32 p0) {
    fn_800A6660((p0 & 0xFF));
}

void fn_80066E90(s32 p0) {
    fn_800A64A8((p0 & 0xFF), 1);
}

void fn_80066EB8(s32 p0) {
    fn_800A6660((p0 & 0xFF));
}
