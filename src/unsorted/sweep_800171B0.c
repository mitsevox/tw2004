// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80280DF0;
void fn_80013E40();
void fn_80076A54();

void fn_800171B0(void);
void fn_800171D8(u8* p0, f32 x0, f32 x1, f32 x2, f32 x3);
void fn_800171B0(void) {
    fn_80013E40(*(s32*)((u8*)lbl_80280DF0));
}

void fn_800171D8(u8* p0, f32 x0, f32 x1, f32 x2, f32 x3) {
    *(f32*)p0 = x0;
    *(f32*)(p0 + 0x4) = x1;
    *(f32*)(p0 + 0x8) = x2;
    *(f32*)(p0 + 0xC) = x3;
    fn_80076A54();
}
