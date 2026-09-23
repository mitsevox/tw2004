// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8010F7E4(s32);
void* fn_8010FB70();

void fn_8010969C(void* arg0);
void fn_8010969C(void* arg0) {
    s32 temp_r4;
    void* temp_r3;

    temp_r3 = fn_8010FB70();
    temp_r4 = (*(s32*)((u8*)(arg0) + 4));
    switch (temp_r4) {
    case 0:
        (*(s8*)((u8*)(temp_r3) + 0x1021)) = 0;
        break;
    case 1:
        (*(s8*)((u8*)(temp_r3) + 0x1021)) = 1;
        break;
    }
    fn_8010F7E4(temp_r4);
}