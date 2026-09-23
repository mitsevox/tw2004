// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80124174();
s32 fn_801241CC();

void fn_80084160(s32 p0, u8* p1);
void fn_80084190(s32 p0, u8* p1);
void fn_80084160(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_801241CC();
    *(s32*)p1 = t0;
}

void fn_80084190(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_80124174();
    *(s32*)p1 = t0;
}
