// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801D7148[];

u8* fn_8007FED8(s32 p0, u8* p1);
u8* fn_8007FED8(s32 p0, u8* p1) {
    *(s32*)p1 = *(u8*)(lbl_801D7148 + 0x18);
    return lbl_801D7148;
}
