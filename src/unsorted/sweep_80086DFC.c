// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800F9254();
s32 fn_800F9308();

void fn_80086DFC(s32 p0, u8* p1);
void fn_80086E2C(s32 p0, u8* p1);
void fn_80086DFC(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_800F9254();
    *(s32*)p1 = t0;
}

void fn_80086E2C(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_800F9308();
    *(s32*)p1 = t0;
}
