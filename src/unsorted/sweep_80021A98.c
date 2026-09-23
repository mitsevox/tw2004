// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

f32 fn_800205F8();
extern f32 lbl_80282CC0;

f32 fn_80021A98(void* arg0);
f32 fn_80021A98(void* arg0) {
    void* temp_r31;

    temp_r31 = (*(void**)((u8*)(arg0) + 0xD8));
    if (temp_r31 != NULL) {
        return fn_800205F8() / (*(f32*)((u8*)(temp_r31) + 0x1EC));
    }
    return lbl_80282CC0;
}