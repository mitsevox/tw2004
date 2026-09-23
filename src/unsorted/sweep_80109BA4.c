// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Rand_Next(s32);
s32 fn_80077ACC();
s32 fn_8007873C(s32);
s32 fn_8007975C(s32, s32, s32);
s32 fn_800797E0(s32, s32, s32, s32);
s32 fn_80103B74(s32);
s32 fn_801073DC(s32);

void fn_80109BA4(void);
void fn_80109BA4(void) {
    s32 temp_r31;

    temp_r31 = fn_80077ACC();
    fn_80103B74(0);
    fn_801073DC(2);
    fn_801073DC(5);
    fn_801073DC(6);
    fn_801073DC(7);
    fn_801073DC(8);
    fn_801073DC(0xB);
    fn_801073DC(0xC);
    fn_801073DC(0xD);
    fn_801073DC(0xE);
    fn_8007975C(temp_r31, 0, 0x50);
    fn_8007975C(temp_r31, 1, 0);
    fn_8007975C(temp_r31, 2, 0);
    fn_8007975C(temp_r31, 7, 0);
    fn_8007975C(temp_r31, 0x13, 0x46);
    fn_8007975C(temp_r31, 0x14, 0x46);
    fn_8007975C(temp_r31, 8, 0x46);
    fn_800797E0(temp_r31, 7, (Rand_Next(0) & 1) + 1, 0);
    fn_8007873C(temp_r31);
}