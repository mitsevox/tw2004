// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern void* lbl_80281EE0;

s32 fn_8008E420(void);
s32 fn_8008E420(void) {
    void* temp_r3;

    temp_r3 = (*(void**)((u8*)(lbl_80281EE0) + 0xB4));
    if ((temp_r3 != NULL) && ((u32) (*(u32*)((u8*)(temp_r3) + 8)) != 0U)) {
        return 1;
    }
    return 0;
}