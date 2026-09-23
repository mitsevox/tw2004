// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


f32 fn_8000C244(u8* p0, u8* p1, u8* p2);
f32 fn_8000C244(u8* p0, u8* p1, u8* p2) {
    return ((*(f32*)p1 - *(f32*)p0) * (*(f32*)(p2 + 0x8) - *(f32*)(p0 + 0x8)) - ((*(f32*)p2 - *(f32*)p0) * (*(f32*)(p1 + 0x8) - *(f32*)(p0 + 0x8))));
}
