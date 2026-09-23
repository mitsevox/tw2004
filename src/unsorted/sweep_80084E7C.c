// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281ED4;

void fn_80084E7C(s32 p0, u8* p1);
void fn_80084E7C(s32 p0, u8* p1) {
    *(s32*)p1 = *(s32*)(((u8*)(lbl_80281ED4 + 0x10000)) + 0x1704);
}
