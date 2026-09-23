// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_8001D7EC();

void fn_8001D7A4(u8* p0);
void fn_8001D7A4(u8* p0) {
    fn_8001D7EC();
    *(s32*)(p0 + 0x10) = (*(s32*)(p0 + 0x10) & -525);
    *(s32*)(p0 + 0x10) = (*(s32*)(p0 + 0x10) | 64);
}
