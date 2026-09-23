// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gSession[];
s32 strlen();

void fn_80082CDC(u8* p0, u8* p1);
void fn_80082CDC(u8* p0, u8* p1) {
    s32 t0;
    t0 = strlen(*(s32*)(((u8*)*(s32*)p0) + 0x8));
    *(s32*)p1 = t0;
}
