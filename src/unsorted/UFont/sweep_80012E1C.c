// Small functions found by the sweep (sweep.py). Original file: UFont.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"


void fn_80012E1C(u8* p, s32 v);
void fn_80012E1C(u8* p, s32 v) {
    *(s32*)(p + 0x474) = v;
}
