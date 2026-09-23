// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_80009E70();
void fn_8001052C();

void fn_8010A668(u8* p0);
void fn_8010A668(u8* p0) {
    fn_8001052C(*(s16*)(p0 + 0x1C));
    fn_80009E70(*(s32*)(p0 + 0x18));
    fn_80009E70(p0);
}
