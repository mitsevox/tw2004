// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801879D8[];

s32 fn_8003CBD4(s32 p0);
s32 fn_8003CBD4(s32 p0) {
    return *(s32*)(lbl_801879D8 + (p0 << 2));
}
