// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800AAE90();
s32 fn_800ABD0C();

void fn_800AA0D8(void* arg0);
void fn_800AA0D8(void* arg0) {
    (*(s8*)((u8*)(arg0) + 0x3E)) = 0;
    if (!(*(*(u8**)((u8*)(arg0) + 8)) & 8)) {
        fn_800AAE90(arg0);
        return;
    }
    fn_800ABD0C();
}