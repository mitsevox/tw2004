#include "game_types.h"

s32 fn_800491C4();
s32 fn_800496E0(void* arg0, u8 arg1);
s32 fn_80049514();
void fn_80049A14(void* arg0, void* arg1);

s32 fn_800496E0(void* arg0, u8 arg1) {
    s32 temp_r5;

    temp_r5 = (*(s32*)((u8*)(arg0) + 0x154));
    if (temp_r5 & 0x04000000) {
        return 0;
    }
    (*(s32*)((u8*)(arg0) + 0x154)) = (s32) (temp_r5 | 0x04000000);
    if (arg1 != 0) {
        fn_800491C4();
    }
    return 1;
}

void fn_80049A14(void* arg0, void* arg1) {
    void* temp_r31;

    temp_r31 = (*(void**)((u8*)(arg1) + 8));
    fn_80049514();
    (*(f32*)((u8*)(arg0) + 0x16C)) = (f32) (*(f32*)((u8*)(temp_r31) + 0x1C));
}
