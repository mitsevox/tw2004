// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8008B990();
s32 fn_8008E420();
s32 fn_8008E44C();

void fn_80109458(s32 p0, u8* p1);
void fn_8010948C(s32 p0, u8* p1);
void fn_801094C0(s32 p0, u8* p1);
void fn_80109458(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_8008E420();
    *(s32*)p1 = (t0 & 0xFF);
}

void fn_8010948C(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_8008E44C();
    *(s32*)p1 = (t0 & 0xFF);
}

void fn_801094C0(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_8008B990();
    *(s32*)p1 = ((u32)__cntlzw((1 - t0)) >> 5);
}
