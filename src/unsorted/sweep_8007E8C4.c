// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801D7148[];

u8* fn_8007E8C4(u8* p0, u8* p1);
u8* fn_8007E8DC(s32 p0, u8* p1);
u8* fn_8007E8F0(u8* p0);
u8* fn_8007E8C4(u8* p0, u8* p1) {
    *(s32*)p1 = *(u8*)(lbl_801D7148 + *(s32*)p0);
    return lbl_801D7148;
}

u8* fn_8007E8DC(s32 p0, u8* p1) {
    *(s32*)p1 = *(u8*)(lbl_801D7148 + 0xF);
    return lbl_801D7148;
}

u8* fn_8007E8F0(u8* p0) {
    *(u8*)(lbl_801D7148 + 0xF) = *(s32*)p0;
    return lbl_801D7148;
}
