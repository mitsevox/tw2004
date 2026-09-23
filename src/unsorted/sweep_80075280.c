// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 UStream_Update();
s32 fn_80076088(s32);

s32 fn_80075280(s32 arg0, s32* arg1);
s32 fn_80075280(s32 arg0, s32* arg1) {
    s32 temp_r31;

    temp_r31 = UStream_Update();
    if (arg1 != NULL) {
        *arg1 = fn_80076088(arg0 + 8);
    }
    return temp_r31;
}