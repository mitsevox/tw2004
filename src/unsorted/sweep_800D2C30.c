// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 gpGame;
extern u8 lbl_801FA2F4[];
s32 Game_GetCourse();
s32 fn_80015464();
void fn_800D2B80();

void fn_800D2C30(s32 p0, s32 p1);
void fn_800D2C68(s32 p0);
void fn_800D2C30(s32 p0, s32 p1) {
    fn_800D2B80(*(s32*)(((u8*)(gpGame + (p0 << 2))) + 0x18), *(s32*)(((u8*)(gpGame + (p0 << 2))) + 0x68), p1, (gpGame + (p0 << 2)));
}

void fn_800D2C68(s32 p0) {
    s32 t0;
    s32 t1;
    t0 = fn_80015464();
    t1 = Game_GetCourse();
    fn_800D2B80(t1, t0, p0);
}
