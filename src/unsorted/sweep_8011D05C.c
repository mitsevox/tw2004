// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80077ACC(void*);
s32 strcpy(s32, s32);

void fn_8011D05C(void** arg0);
void fn_8011D05C(void** arg0) {
    s32 temp_r31;
    void* temp_r3;

    temp_r3 = *arg0;
    temp_r31 = (*(s32*)((u8*)(temp_r3) + 8));
    strcpy(temp_r31, fn_80077ACC(temp_r3) + 1);
}