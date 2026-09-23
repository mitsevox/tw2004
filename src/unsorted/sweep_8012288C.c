// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281970;

s32 fn_8012288C(u8* p0, u8* p1);
s32 fn_8012288C(u8* p0, u8* p1) {
    *(s32*)p1 = *(s32*)(((u8*)lbl_80281970) + 0x48);
    *(s32*)p0 = 2;
    return *(s32*)(((u8*)lbl_80281970) + 0x44);
}
