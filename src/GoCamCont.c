#include "game_types.h"

void fn_80062F1C(u8* p0);
s32 fn_80063C7C(u8* p0);
void Vec_Copy();
void fn_80063CBC(u8* p0, s32 p1);
void fn_800642A4(u8* p0, f32 x0, f32 x1);
void fn_800C6CB0();
void fn_800642B0(void);

void fn_80062F1C(u8* p0) {
    *(s32*)(p0 + 0x78) = 0;
    *(s32*)(p0 + 0x7C) = 0;
    *(s32*)(p0 + 0x80) = 0;
    *(s32*)(p0 + 0x74) = 0;
    *(u8*)(p0 + 0x268) = 0;
}

s32 fn_80063C7C(u8* p0) {
    return (((u32)__cntlzw((4 - *(s32*)(p0 + 0x144))) >> 5) & 0xFF);
}

void fn_80063CBC(u8* p0, s32 p1) {
    *(s32*)(p0 + 0x144) = 3;
    Vec_Copy(p1, (p0 + 0xC4), p0);
}

void fn_800642A4(u8* p0, f32 x0, f32 x1) {
    *(f32*)(p0 + 0x174) = x0;
    *(f32*)(p0 + 0x178) = x1;
}

void fn_800642B0(void) {
    fn_800C6CB0();
}
