// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800EFDFC();
void fn_800EFE18();
void strcpy();

void fn_8008A468(u8* p0);
void fn_8008A468(u8* p0) {
    s32 t1;
    fn_800EFE18();
    t1 = fn_800EFDFC();
    strcpy(*(s32*)(((u8*)*(s32*)p0) + 0x8), t1, *(s32*)p0);
}
