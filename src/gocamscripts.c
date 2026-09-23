#include "game_types.h"

s32 GameEffects_BallUpdatesThisFrame(s32);
s32 fn_80043920(void* arg0, s32 arg1);
s32 fn_80044AA8(void* arg0);
s32 Game_GetCourse();
s32 fn_80015464();
s32 fn_80044E2C(s32 arg0);
s32 fn_80044E74(void* arg0);

s32 fn_80043920(void* arg0, s32 arg1) {
    u8 temp_r0;
    u8 temp_r0_2;
    void* temp_r3;
    void* temp_r3_2;

    temp_r3 = (*(void**)((u8*)(arg0) + 0xAC));
    if (temp_r3 == NULL) {
        return 0;
    }
    temp_r0 = (*(u8*)((u8*)(temp_r3) + 0xAC));
    if (((temp_r0 == 0) || (temp_r0 == 0xD)) && (GameEffects_BallUpdatesThisFrame(arg1) < 1)) {
        return 1;
    }
    temp_r3_2 = (*(void**)((u8*)(arg0) + 0xB0));
    if ((temp_r3_2 == NULL) || ((s32) (*(s32*)((u8*)(arg0) + 0xBC)) == 5)) {
        return 0;
    }
    temp_r0_2 = (*(u8*)((u8*)(temp_r3_2) + 0xAC));
    if (((temp_r0_2 == 0) || (temp_r0_2 == 0xD)) && (GameEffects_BallUpdatesThisFrame(arg1) < 1)) {
        return 1;
    }
    return 0;
}

s32 fn_80044AA8(void* arg0) {
    u32 temp_r0;

    if (arg0 == NULL) {
        return 0;
    }
    temp_r0 = (*(u32*)((u8*)(arg0) + 0x2C));
    if (temp_r0 == 0xAU) {
        return 0;
    }
    if (temp_r0 == 9U) {
        return 0;
    }
    if ((temp_r0 >= 1U) && (temp_r0 <= 0xCU)) {
        return 1;
    }
    if (temp_r0 == 0x12U) {
        return 1;
    }
    return 0;
}

s32 fn_80044E2C(s32 arg0) {
    if ((arg0 == 0x95) && (Game_GetCourse() == 7) && (fn_80015464() == 2)) {
        return 1;
    }
    return 0;
}

s32 fn_80044E74(void* arg0) {
    u8 temp_r0;

    if (arg0 == NULL) {
        return 0;
    }
    temp_r0 = (*(u8*)((u8*)(arg0) + 0xAD));
    if ((temp_r0 >= 6U) && (temp_r0 <= 0xAU)) {
        return 1;
    }
    return 0;
}
