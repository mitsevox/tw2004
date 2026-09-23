// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_8009CD10();
void fn_8009CD7C();
s32 fn_800B120C();
s32 fn_800B12FC();

void fn_800B20B8(u8* p0, u8* p1);
void fn_800B2104(u8* p0, u8* p1);
void fn_800B20B8(u8* p0, u8* p1) {
    s32 t1;
    fn_8009CD10();
    t1 = fn_800B12FC(*(s32*)p0, *(s32*)(p0 + 0x4));
    *(s32*)p1 = t1;
    fn_8009CD7C();
}

void fn_800B2104(u8* p0, u8* p1) {
    s32 t1;
    fn_8009CD10();
    t1 = fn_800B120C(*(s32*)p0, *(s32*)(p0 + 0x4));
    *(s32*)p1 = t1;
    fn_8009CD7C();
}
