// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801D87C0[];

u8* fn_8007DAD4(s32 p0, u8* p1);
u8* fn_8007DAD4(s32 p0, u8* p1) {
    *(s32*)p1 = *(s32*)(lbl_801D87C0 + 0x38);
    return lbl_801D87C0;
}
