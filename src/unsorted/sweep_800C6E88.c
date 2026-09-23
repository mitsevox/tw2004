// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u32 fn_8003A950(s32, s32, s32, s32, s32, s32, s32, s32);

s32 fn_800C6E88(void* arg0, s32 arg1);
s32 fn_800C6E88(void* arg0, s32 arg1) {
    if ((u8) (*(u8*)((u8*)(arg0) + 0x268)) == 1) {
        return 0;
    }
    if (fn_8003A950((*(s32*)((u8*)(arg0) + 0x74)), 0x17, 0, 0, 0, 0, 0, arg1) != 0U) {
        return (*(s32*)((u8*)(arg0) + 0x194)) < 1;
    }
    return 0;
}