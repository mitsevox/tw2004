// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_80013268(s32 arg0, s32 arg1, s32 arg2);
s32 fn_80013268(s32 arg0, s32 arg1, s32 arg2) {
    s32 var_r0;

    if (arg0 > arg2) {
        var_r0 = 0xFF;
    } else if (arg0 > arg1) {
        var_r0 = ((s32) ((arg0 - arg1) << 7) / (s32) (arg2 - arg1)) + 0x80;
    } else if (arg0 < (s32) -arg2) {
        var_r0 = 0;
    } else if (arg0 < (s32) -arg1) {
        var_r0 = ((s32) ((arg0 + arg1) << 7) / (s32) (arg2 - arg1)) + 0x80;
    } else {
        var_r0 = 0x80;
    }
    if (var_r0 < 0) {
        return 0;
    }
    if (var_r0 <= 0xFF) {
        return var_r0;
    }
    return 0xFF;
}