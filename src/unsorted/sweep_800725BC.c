// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

f32 fn_800728D8();
f32 fn_80072938();

void fn_800725BC(void* arg0, s32 arg1, f32 farg0);
void fn_800725BC(void* arg0, s32 arg1, f32 farg0) {
    if (arg0 != NULL) {
        (*(s32*)((u8*)(arg0) + 4)) = 1;
        (*(f32*)((u8*)(arg0) + 0x18)) = farg0;
        (*(s32*)((u8*)(arg0) + 0x20)) = arg1;
        (*(f32*)((u8*)(arg0) + 0x10)) = fn_800728D8();
        (*(f32*)((u8*)(arg0) + 0x14)) = fn_80072938(arg0);
    }
}