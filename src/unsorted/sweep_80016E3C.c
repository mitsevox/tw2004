// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_80008370();
void fn_80012EF0();
void fn_800137B0();
s32 fn_80016E28();
void fn_8007644C();
void fn_80076B18();

void fn_80016E3C(void);
void fn_80016E3C(void) {
    s32 t0;
    t0 = fn_80016E28();
    fn_80008370(*(s32*)((u8*)t0));
    fn_8007644C();
    fn_80012EF0(*(s32*)((u8*)t0));
    fn_80076B18();
    fn_800137B0(*(s32*)((u8*)t0));
    *(u8*)(((u8*)t0) + 0x274) = 0;
}
