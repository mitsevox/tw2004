// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80016E28();

s32 fn_80017028(void);
void fn_8001704C(s32 p0, s32 p1);
s32 fn_80017028(void) {
    s32 t0;
    t0 = fn_80016E28();
    return (t0 + 4);
}

void fn_8001704C(s32 p0, s32 p1) {
    s32 t0;
    t0 = fn_80016E28();
    *(s32*)(((u8*)t0) + 0x270) = p1;
}
