// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_801218DC(s32 p0);
s32 fn_801218EC(s32 p0);
s32 fn_801218DC(s32 p0) {
    return (((u32)__cntlzw((24 - p0)) >> 5) & 0xFF);
}

s32 fn_801218EC(s32 p0) {
    return (((u32)__cntlzw((6 - p0)) >> 5) & 0xFF);
}
