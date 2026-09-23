// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


u8 fn_800C5FE4(void* arg0);
u8 fn_800C5FE4(void* arg0) {
    u8 var_r4;

    var_r4 = 0;
    if ((s32) (*(s32*)((u8*)(arg0) + 0x260)) == 0xB) {
        var_r4 = 1;
    }
    if (var_r4 != 0) {
        (*(s32*)((u8*)(arg0) + 0x198)) = 1;
    }
    return var_r4;
}