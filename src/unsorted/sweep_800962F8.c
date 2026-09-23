// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s8 fn_80096338();

s32 fn_800962F8(void* arg0);
s32 fn_800962F8(void* arg0) {
    (*(s8*)((u8*)(arg0) + 0x24)) = fn_80096338();
    (*(s8*)((u8*)(arg0) + 0x25)) = 0;
    (*(s8*)((u8*)(arg0) + 0x26)) = 0;
    return 2;
}