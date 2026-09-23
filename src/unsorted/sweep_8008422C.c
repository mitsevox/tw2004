// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_80125600();
void fn_80125648();

void fn_8008422C(u8* p0);
void fn_80084258(u8* p0);
void fn_8008422C(u8* p0) {
    fn_80125600(*(s32*)(p0 + 0x0), *(s32*)(p0 + 0x4));
}

void fn_80084258(u8* p0) {
    fn_80125648(*(s32*)(p0 + 0x0), *(s32*)(p0 + 0x4), *(s32*)(p0 + 0x8));
}
