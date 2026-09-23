// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 fn_8001E9CC(s32, s32);
s32 fn_8001EA34(s32, s32);
s32 fn_80077ACC();
s32 fn_80104FA8(s16, s32, s32);

void fn_80108B84(void* arg0);
void fn_80108B84(void* arg0) {
    s32 temp_r31;
    s32 temp_r3;

    temp_r31 = fn_80077ACC();
    temp_r3 = fn_80104FA8((s16) (*(s32*)((u8*)(arg0) + 0)), (*(s32*)((u8*)(arg0) + 4)), (*(s32*)((u8*)(arg0) + 8)));
    if (fn_8001E9CC(temp_r31 + 0xB344, temp_r3) != 0) {
        fn_8001EA34(temp_r31 + 0xB4BC, temp_r3);
    }
}