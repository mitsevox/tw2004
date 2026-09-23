// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_8009D1D8();

void fn_800A26D8(u8* p0);
void fn_800A270C(u8* p0);
void fn_800A2740(u8* p0);
void fn_800A26D8(u8* p0) {
    fn_8009D1D8(*(s32*)(p0 + 0x0), *(s32*)(p0 + 0x4), 0, 0);
}

void fn_800A270C(u8* p0) {
    fn_8009D1D8(*(s32*)(p0 + 0x0), *(s32*)(p0 + 0x4), 0, 1);
}

void fn_800A2740(u8* p0) {
    fn_8009D1D8(*(s32*)(p0 + 0x0), *(s32*)(p0 + 0x4), 0, 2);
}
