// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* lbl_80280E08;

void fn_8001425C(s32 v);
f32 fn_80014268(u8* p);
f32 fn_80014270(u8* p);
f32 fn_80014278(u8* p);
void fn_8001425C(s32 v) {
    *(s32*)(lbl_80280E08 + 0xD0) = v;
}

f32 fn_80014268(u8* p) {
    return *(f32*)(p + 0xAC);
}

f32 fn_80014270(u8* p) {
    return *(f32*)(p + 0xA8);
}

f32 fn_80014278(u8* p) {
    return *(f32*)(p + 0xA4);
}
