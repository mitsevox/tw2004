// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8012411C(s32*);
s32 fn_80124138(s32);

void fn_8008410C(s32** arg0, s32* arg1);
void fn_8008410C(s32** arg0, s32* arg1) {
    s32 temp_r31;
    s32* temp_r3;

    temp_r3 = *arg0;
    temp_r31 = *temp_r3;
    *arg1 = temp_r31 + fn_8012411C(temp_r3);
    fn_80124138(*arg1);
}