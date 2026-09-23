// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80191990[];
void strcpy();

void fn_8008293C(u8* p0);
void fn_8008293C(u8* p0) {
    strcpy(*(s32*)(((u8*)*(s32*)(p0 + 0x4)) + 0x8), *(s32*)(lbl_80191990 + (*(s32*)p0 << 2)));
}
