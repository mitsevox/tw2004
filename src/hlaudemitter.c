#include "game_types.h"

void fn_800AD1C4(void);
s32 fn_800A8248();
void* fn_800AD674();
void fn_800AD950(s32 arg0, s32 arg1, s32 arg2);
s32 fn_800A82CC();
void fn_800AD9AC(s32 arg0, s32 arg1, s32 arg2);
void fn_800A834C();
void fn_800ADA08(void);
s32 fn_800A8394(s32, s32, s32, s32);
void fn_800ADA28(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32 fn_800A8424(u8, s32, f32);
void fn_800ADA94(u8 arg0, s32 arg1, f32 farg0);
s32 fn_800A84A4(s32, s32, f32);
void fn_800ADAF0(s32 arg0, s32 arg1, f32 farg0);
void fn_800ADDC8(s32 arg0, u8 arg1, s32 arg2);

void fn_800AD1C4(void) {
}

void fn_800AD950(s32 arg0, s32 arg1, s32 arg2) {
    if (fn_800AD674() != NULL) {
        fn_800A8248(arg0, arg1, arg2);
    }
}

void fn_800AD9AC(s32 arg0, s32 arg1, s32 arg2) {
    if (fn_800AD674() != NULL) {
        fn_800A82CC(arg0, arg1, arg2);
    }
}

void fn_800ADA08(void) {
    fn_800A834C();
}

void fn_800ADA28(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    if (fn_800AD674() != 0U) {
        fn_800A8394(arg0, arg1, arg2, arg3);
    }
}

void fn_800ADA94(u8 arg0, s32 arg1, f32 farg0) {
    if (fn_800AD674() != 0U) {
        fn_800A8424(arg0, arg1, farg0);
    }
}

void fn_800ADAF0(s32 arg0, s32 arg1, f32 farg0) {
    if (fn_800AD674() != 0U) {
        fn_800A84A4(arg0, arg1, farg0);
    }
}

void fn_800ADDC8(s32 arg0, u8 arg1, s32 arg2) {
    s32 (*temp_r12)(s32, u8, s32);
    s32 temp_r0;
    void* temp_r3;

    temp_r3 = fn_800AD674();
    if (temp_r3 != NULL) {
        temp_r0 = 1 << arg1;
        (*(u8*)((u8*)(temp_r3) + 0x22)) = (u8) ((*(u8*)((u8*)(temp_r3) + 0x22)) & (u8) ~(temp_r0 | temp_r0));
        temp_r12 = (*(s32 (**)(s32, u8, s32))((u8*)(temp_r3) + 0x30));
        if (temp_r12 != NULL) {
            temp_r12(arg0, arg1, arg2);
        }
    }
}
