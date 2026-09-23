// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8003532C();
void fn_8006E5A8();
void fn_8006F144();
void fn_8006F400();
void fn_8006F430();

void fn_80093900(u8* p0);
void fn_80093900(u8* p0) {
    s32 t0;
    t0 = fn_8003532C();
    fn_8006F430(*(f32*)(p0 + 0x80));
    fn_8006F400(t0, 0, p0, *(f32*)(p0 + 0x10));
    fn_8006F400(t0, 1, (p0 + 0x20), *(f32*)(p0 + 0x30));
    fn_8006F400(t0, 2, (p0 + 0x40), *(f32*)(p0 + 0x50));
    fn_8006F400(t0, 3, (p0 + 0x60), *(f32*)(p0 + 0x70));
}
