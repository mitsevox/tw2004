// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Game_GetMode();
s32 fn_800E1788(s32);
s32 fn_800E1904(s32, s32);
s32 fn_800E8CA8(s32, s32);
s32 fn_8011937C(s32, s32, s32);

void fn_80086F0C(s32* arg0, s32* arg1);
void fn_80086F0C(s32* arg0, s32* arg1) {
    s32 temp_r3;

    temp_r3 = Game_GetMode();
    switch (temp_r3) {
    case 18:
        *arg1 = fn_800E1788(*arg0);
        return;
    case 23:
        *arg1 = fn_8011937C(*arg0, 0, 0);
        return;
    case 19:
        *arg1 = fn_800E8CA8(*arg0, 1);
        return;
    default:
        *arg1 = fn_800E1904(*arg0, 1);
        return;
    }
}