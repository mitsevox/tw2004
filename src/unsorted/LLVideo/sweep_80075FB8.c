// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 UStream_Close(s32);
s32 UStream_OpenFileByName(s32);
s32 UStream_SetAutoRead(s32);
s32 fn_80006FE8();
u8 fn_80007258();
s32 fn_800755F0(s32);
s32 fn_80075800();
s32 fn_800758B4(s32);
s32 fn_80075904(s32, s32);
s32 fn_800BA734(s32, s32);
s32 fn_80075DEC(s32, s32, s32);
extern u8 lbl_80281EB8;

void fn_80075FB8(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void fn_80075FB8(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 temp_r3_2;
    s32 temp_r3_3;
    u8 temp_r3;

    temp_r3 = fn_80007258();
    lbl_80281EB8 = temp_r3;
    if (temp_r3 != 0) {
        fn_80006FE8();
    }
    fn_800BA734(0, 0);
    fn_800755F0(arg3 | 1);
    temp_r3_2 = UStream_OpenFileByName(arg0);
    if (temp_r3_2 != -1) {
        UStream_SetAutoRead(1);
        temp_r3_3 = fn_80075800();
        fn_80075904(0, temp_r3_3);
        fn_80075DEC(temp_r3_3, arg1, arg2);
        UStream_SetAutoRead(0);
        fn_800758B4(temp_r3_3);
        UStream_Close(temp_r3_2);
    }
}