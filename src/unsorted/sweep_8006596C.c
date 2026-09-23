// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_8006596C(void* arg0, void** arg1, void** arg2, void** arg3);
void fn_8006596C(void* arg0, void** arg1, void** arg2, void** arg3) {
    void* temp_r0;
    void* temp_r4;
    void* temp_r5;
    void* temp_r7;

    temp_r0 = (*(void**)((u8*)(arg0) + 0x44));
    if (temp_r0 != NULL) {
        *arg1 = temp_r0;
    } else {
        *arg1 = arg0;
    }
    temp_r7 = (*(void**)((u8*)(arg0) + 0x40));
    if (temp_r7 != NULL) {
        if ((s32) (*(s32*)((u8*)(arg0) + 0xA4)) == (s32) (*(s32*)((u8*)(temp_r7) + 0xA4))) {
            *arg2 = temp_r7;
        } else {
            *arg2 = arg0;
        }
    } else {
        *arg2 = arg0;
    }
    temp_r4 = *arg2;
    if (temp_r4 != NULL) {
        temp_r5 = (*(void**)((u8*)(temp_r4) + 0x40));
        if (temp_r5 != NULL) {
            if ((s32) (*(s32*)((u8*)(arg0) + 0xA4)) == (s32) (*(s32*)((u8*)(temp_r5) + 0xA4))) {
                *arg3 = temp_r5;
                return;
            }
            *arg3 = temp_r4;
            return;
        }
    }
    *arg3 = temp_r4;
}