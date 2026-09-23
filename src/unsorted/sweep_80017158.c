// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_80012EF0();
s32 fn_80016E28();
void fn_80017004();
void fn_800171D8();

void fn_80017158(s32 p0);
void fn_80017158(s32 p0) {
    s32 t0;
    t0 = fn_80016E28();
    fn_80017004(p0);
    fn_80012EF0();
    fn_800171D8(*(f32*)(((u8*)t0) + 0x284), *(f32*)(((u8*)t0) + 0x280), *(f32*)(((u8*)t0) + 0x27C), *(f32*)(((u8*)t0) + 0x278));
}
