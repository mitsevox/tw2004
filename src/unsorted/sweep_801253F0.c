// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8012D5B0();
s32 fn_80124A98();
s32 fn_801253F0();

s32 fn_801253F0(void);
s32 fn_801253F0(void) {
    s32 temp_r3;

    if (fn_8012D5B0() != 0) {
        return fn_80124A98();
    }
    temp_r3 = fn_80124AAC();
    return temp_r3 & ((s32) (-temp_r3 | temp_r3) >> 0x1F);
}