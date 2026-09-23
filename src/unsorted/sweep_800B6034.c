// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_800B6034(void** arg0, void* arg1);
void fn_800B6034(void** arg0, void* arg1) {
    void* temp_r5;

    if (arg0 != NULL) {
        (*(void**)((u8*)(arg1) + 0)) = (void* ) *arg0;
        (*(void***)((u8*)(arg1) + 4)) = arg0;
        temp_r5 = *arg0;
        if (temp_r5 != NULL) {
            (*(void**)((u8*)(temp_r5) + 4)) = arg1;
        }
        *arg0 = arg1;
    }
}