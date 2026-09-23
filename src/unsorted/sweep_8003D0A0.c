// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_8003D0A0(s32 p0, s32 p1);
s32 fn_8003D0A0(s32 p0, s32 p1) {
    return ((u32)((-(p0 & (1 << p1))) | (p0 & (1 << p1))) >> 31);
}
