// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281ED4;

void fn_8007EE80(u8* p0);
void fn_8007EE80(u8* p0) {
    *(u8*)(((u8*)lbl_80281ED4) + 0x5391) = *(s32*)(p0 + 0x4);
}
