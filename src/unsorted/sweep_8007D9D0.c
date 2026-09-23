// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281ED4;

void fn_8007D9D0(s32 p0, u8* p1);
void fn_8007D9D0(s32 p0, u8* p1) {
    *(s32*)p1 = ((s8)*(u8*)(((u8*)lbl_80281ED4) + 0x2));
}
