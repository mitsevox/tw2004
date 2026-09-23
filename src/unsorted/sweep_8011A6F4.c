// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80117628();

s32 fn_8011A6F4(s32 p0, s32 p1);
s32 fn_8011A6F4(s32 p0, s32 p1) {
    s32 t0;
    t0 = fn_80117628(p1);
    return (*(s32*)(((u8*)t0) + 0x4C) & 0xFF);
}
