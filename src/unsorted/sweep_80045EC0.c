// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void DCFlushRange();
void DCInvalidateRange();
void fn_80015624();
void fn_8014489C();
s32 fn_80144964();

void fn_80045EC0(u8* p0);
void fn_80045EC0(u8* p0) {
    s32 t3;
    DCInvalidateRange(p0, 4096);
    fn_8014489C(p0, 4096);
    fn_80015624();
    t3 = fn_80144964();
    *(s32*)(p0 + 0x1000) = t3;
    DCFlushRange(p0, *(s32*)(p0 + 0x1000));
}
