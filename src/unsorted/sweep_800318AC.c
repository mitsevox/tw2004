// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8015929C(s32, s32, s32, s32 (*)(void*, void*));
s32 fn_800318AC(void* arg0, void* arg1);

s32 fn_800318AC(void* arg0, void* arg1);
s32 fn_800318AC(void* arg0, void* arg1) {
    f32 temp_f0;
    f32 temp_f1;

    temp_f0 = (*(f32*)((u8*)(arg0) + 0x10));
    temp_f1 = (*(f32*)((u8*)(arg1) + 0x10));
    if (temp_f0 > temp_f1) {
        return 1;
    }
    if (temp_f0 < temp_f1) {
        return -1;
    }
    return 0;
}