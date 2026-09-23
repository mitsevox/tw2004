// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* lbl_80282220;

s32 fn_800C7138(u8* p);
void fn_800C7140(u8 v);
u8 fn_800C714C(void);
void fn_800C7158(u8* p, u8 v);
u8 fn_800C7160(u8* p);
void fn_800C7168(u8* p, u8 v);
u8 fn_800C7170(u8* p);
s32 fn_800C7138(u8* p) {
    return *(s32*)(p + 0x260);
}

void fn_800C7140(u8 v) {
    *(u8*)(lbl_80282220 + 0x55) = v;
}

u8 fn_800C714C(void) {
    return *(u8*)(lbl_80282220 + 0x55);
}

void fn_800C7158(u8* p, u8 v) {
    *(u8*)(p + 0x269) = v;
}

u8 fn_800C7160(u8* p) {
    return *(u8*)(p + 0x269);
}

void fn_800C7168(u8* p, u8 v) {
    *(u8*)(p + 0x26A) = v;
}

u8 fn_800C7170(u8* p) {
    return *(u8*)(p + 0x26A);
}
