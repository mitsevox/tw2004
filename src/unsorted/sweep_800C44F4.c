// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


u8 fn_800C44F4(void* arg0);
u8 fn_800C44F4(void* arg0) {
    s32 temp_r0;

    temp_r0 = (*(s32*)((u8*)(arg0) + 0x260));
    if (temp_r0 == 0xD) {
        return 1U;
    }
    return (u8) (temp_r0 == 0xE);
}