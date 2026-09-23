// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281ED4;

void fn_80081F98(u8* p0);
void fn_80081FA8(s32 p0, u8* p1);
void fn_80081F98(u8* p0) {
    *(u8*)(((u8*)lbl_80281ED4) + 0x1) = *(s32*)p0;
}

void fn_80081FA8(s32 p0, u8* p1) {
    *(s32*)p1 = ((s8)*(u8*)(((u8*)lbl_80281ED4) + 0x1));
}
