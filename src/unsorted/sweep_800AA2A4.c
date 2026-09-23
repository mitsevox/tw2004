// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800AAEFC();
s32 fn_800ABDB0();

void fn_800AA2A4(void* arg0);
void fn_800AA2A4(void* arg0) {
    (*(f32*)((u8*)(arg0) + 0x4C)) = (f32) ((*(f32*)((u8*)(arg0) + 0x4C)) + (*(f32*)((u8*)(arg0) + 0x50)));
    if (!(*(*(u8**)((u8*)(arg0) + 8)) & 8)) {
        fn_800AAEFC(arg0);
        return;
    }
    fn_800ABDB0();
}