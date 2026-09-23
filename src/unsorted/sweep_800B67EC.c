// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80007328();
s32 fn_80007368();
s32 fn_800B04EC(s32, s32, s32);
s32 fn_800B6214(void*);

void fn_800B67EC(void* arg0);
void fn_800B67EC(void* arg0) {
    do {

    } while (fn_800B67B4(arg0) != 2);
    fn_800B04EC((*(s32*)((u8*)(arg0) + 0x30)), (*(s32*)((u8*)(arg0) + 0x34)), (*(s32*)((u8*)(arg0) + 0x2C)));
    fn_80007368();
    fn_800B6214(arg0);
    fn_80007328();
}