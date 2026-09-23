// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void DCFlushRange();
void DCInvalidateRange();
void fn_80015624();
void GXBeginDisplayList();
s32 GXEndDisplayList();

void fn_80045EC0(u8* p0);
void fn_80045EC0(u8* p0) {
    s32 t3;
    DCInvalidateRange(p0, 4096);
    GXBeginDisplayList(p0, 4096);
    fn_80015624();
    t3 = GXEndDisplayList();
    *(s32*)(p0 + 0x1000) = t3;
    DCFlushRange(p0, *(s32*)(p0 + 0x1000));
}
