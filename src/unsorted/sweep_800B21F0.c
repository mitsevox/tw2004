// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_8009CD10();
void fn_8009CD7C();
s32 fn_8009D3DC();

void fn_800B21F0(u8* p0, u8* p1);
void fn_800B21F0(u8* p0, u8* p1) {
    s32 t1;
    fn_8009CD10();
    t1 = fn_8009D3DC(*(s32*)p0, *(s32*)(p0 + 0x4));
    *(s32*)p1 = t1;
    fn_8009CD7C();
}
