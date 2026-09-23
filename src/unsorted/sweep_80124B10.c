// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281988;
void Mem_cpy();
void fn_80009E70();

void fn_80124B10(u8* p0);
void fn_80124B10(u8* p0) {
    Mem_cpy(*(s32*)(((u8*)lbl_80281988) + 0x8), *(s32*)p0, *(s32*)(p0 + 0x24));
    fn_80009E70(p0);
}
