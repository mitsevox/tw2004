// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 fn_8003CBE8();
u8 fn_8003CD9C(s32, s32, s32);

s32 fn_8003D7A0(s32 arg0, s32 arg1);
s32 fn_8003D7A0(s32 arg0, s32 arg1) {
    if (arg0 == 0) {
        return 0;
    }
    if ((fn_8003CBE8() != 0) && (fn_8003CD9C(arg0, arg1, 1) != 0)) {
        return 1;
    }
    return 0;
}