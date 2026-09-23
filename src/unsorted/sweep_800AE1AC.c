// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32* fn_800AE1AC(void* arg0);
s32* fn_800AE1AC(void* arg0) {
    s32* temp_r5;

    temp_r5 = (*(s32**)((u8*)(arg0) + 0));
    if (temp_r5 == arg0) {
        return NULL;
    }
    (*(s32**)((u8*)(arg0) + 0)) = (s32* ) *temp_r5;
    (*(s32*)((u8*)(arg0) + 4)) = (s32) ((*(s32*)((u8*)(arg0) + 4)) - 1);
    return temp_r5;
}