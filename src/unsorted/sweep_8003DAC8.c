// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 fn_800453C8(s32, void*);

void fn_8003DAC8(void* arg0, s32 arg1, f32* arg2, f32* arg3);
void fn_8003DAC8(void* arg0, s32 arg1, f32* arg2, f32* arg3) {
    if (arg0 != NULL) {
        if (fn_800453C8(arg1, arg0) != 0) {
            if (((u8) (*(u8*)((u8*)(arg0) + 0xAF)) == 0x15) || ((u8) (*(u8*)((u8*)(arg0) + 0xB0)) == 0x15)) {
                if (arg2 != NULL) {
                    *arg2 = (*(f32*)((u8*)(arg0) + 0x64));
                }
                if (arg3 != NULL) {
                    *arg3 = -(*(f32*)((u8*)(arg0) + 0x60));
                }
            } else {
                if (arg2 != NULL) {
                    *arg2 = -(*(f32*)((u8*)(arg0) + 0x64));
                }
                if (arg3 != NULL) {
                    *arg3 = (*(f32*)((u8*)(arg0) + 0x60));
                }
            }
        } else {
            if (arg2 != NULL) {
                *arg2 = (*(f32*)((u8*)(arg0) + 0x64));
            }
            if (arg3 != NULL) {
                *arg3 = (*(f32*)((u8*)(arg0) + 0x60));
            }
        }
    }
}