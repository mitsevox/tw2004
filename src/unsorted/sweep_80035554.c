// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_80035554(u8* p0);
f32 fn_80035560(u8* p0);
s32 fn_80035554(u8* p0) {
    return *(u8*)(((u8*)*(s32*)p0) + 0x8B);
}

f32 fn_80035560(u8* p0) {
    return *(f32*)(((u8*)*(s32*)p0) + 0x54);
}
