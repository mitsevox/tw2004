// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_8008F80C();

void fn_8007C48C(u8* p0);
void fn_8007C48C(u8* p0) {
    fn_8008F80C(*(s32*)p0, (*(s32*)(p0 + 0x4) & 0xFF));
}
