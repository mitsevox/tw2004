// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_8011C46C(void* arg0);
void fn_8011C46C(void* arg0) {
    s32 temp_r4;

    temp_r4 = (*(s32*)((u8*)(arg0) + 0x10020));
    (*(s32*)((u8*)(arg0) + 0x10020)) = (s32) (*(s32*)((u8*)(arg0) + 0x10024));
    (*(s32*)((u8*)(arg0) + 0x10024)) = temp_r4;
}