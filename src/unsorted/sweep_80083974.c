// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_80083974(void* arg0);
void fn_80083974(void* arg0) {
    s32 temp_r0;
    s32 temp_r5;

    temp_r0 = (*(s32*)((u8*)(arg0) + 8)) % 3;
    temp_r5 = (((*(s32*)((u8*)(arg0) + 0xC)) % 10) + 1) * 0x19;
    switch (temp_r0) {
    case 0:
        *(*(s32**)((u8*)(arg0) + 0x10)) = temp_r5;
        *(*(s32**)((u8*)(arg0) + 0x14)) = 0x32;
        *(*(s32**)((u8*)(arg0) + 0x18)) = 0x32;
        return;
    case 1:
        *(*(s32**)((u8*)(arg0) + 0x10)) = 0x32;
        *(*(s32**)((u8*)(arg0) + 0x14)) = temp_r5;
        *(*(s32**)((u8*)(arg0) + 0x18)) = 0x32;
        return;
    case 2:
        *(*(s32**)((u8*)(arg0) + 0x10)) = 0x32;
        *(*(s32**)((u8*)(arg0) + 0x14)) = 0x32;
        *(*(s32**)((u8*)(arg0) + 0x18)) = temp_r5;
        return;
    }
}