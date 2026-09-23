// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_80015624();

f32 fn_80012ED0(u8* p);
f32 fn_80012ED8(u8* p);
f32 fn_80012EE0(u8* p);
f32 fn_80012EE8(u8* p);
s32 fn_80012EF0(u8* p);
void fn_80012EF8(void);
f32 fn_80012ED0(u8* p) {
    return *(f32*)(p + 0xC);
}

f32 fn_80012ED8(u8* p) {
    return *(f32*)(p + 0x8);
}

f32 fn_80012EE0(u8* p) {
    return *(f32*)(p + 0x4);
}

f32 fn_80012EE8(u8* p) {
    return *(f32*)(p + 0x0);
}

s32 fn_80012EF0(u8* p) {
    return *(s32*)(p + 0x14);
}

void fn_80012EF8(void) {
    fn_80015624();
}
