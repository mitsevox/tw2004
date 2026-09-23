// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gReplayData[];

u8* fn_800801C0(s32 p0, u8* p1);
u8* fn_800801C0(s32 p0, u8* p1) {
    *(s32*)p1 = *(s16*)(gReplayData + 0xF04);
    return gReplayData;
}
