// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


f32 fn_80008EF0(u8* p0);
f32 fn_80008EF0(u8* p0) {
    return ((*(f32*)(p0 + 0x8) * *(f32*)(p0 + 0x8)) + ((*(f32*)(p0 + 0x4) * *(f32*)(p0 + 0x4)) + ((*(f32*)(p0 + 0xC) * *(f32*)(p0 + 0xC)) + (*(f32*)p0 * *(f32*)p0))));
}
