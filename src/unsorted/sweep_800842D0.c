// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80125354(s32, s32);
s32 fn_801253F0(s32, s32);
extern void* lbl_80281ED4;

void fn_800842D0(void* arg0, s32* arg1);
void fn_800842D0(void* arg0, s32* arg1) {
    s32 temp_r3;
    s32 temp_r3_2;

    temp_r3 = fn_80125354((*(s32*)((u8*)(arg0) + 0)), (*(s32*)((u8*)(arg0) + 4)));
    (*(s32*)((u8*)(lbl_80281ED4) + 0x11704)) = temp_r3;
    if (temp_r3 != 0) {
        *arg1 = 0;
        return;
    }
    temp_r3_2 = fn_801253F0((*(s32*)((u8*)(arg0) + 0)), (*(s32*)((u8*)(arg0) + 4)));
    (*(s32*)((u8*)(lbl_80281ED4) + 0x11704)) = temp_r3_2;
    *arg1 = temp_r3_2 == 0;
}