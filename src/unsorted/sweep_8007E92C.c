// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281EE0;

void fn_8007E92C(u8* p0);
void fn_8007E92C(u8* p0) {
    *(u8*)(((u8*)lbl_80281EE0) + 0x83) = *(s32*)p0;
}
