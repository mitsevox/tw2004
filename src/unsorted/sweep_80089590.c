// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 fn_800DAD44();
s32 fn_800DAD4C();

void fn_80089590(s32* arg0, s32* arg1);
void fn_80089590(s32* arg0, s32* arg1) {
    s32 temp_r0;

    temp_r0 = *arg0;
    switch (temp_r0) {
    case 0:
        if (fn_800DAD44() != 0) {
            *arg1 = 1;
            return;
        }
        *arg1 = 0;
        return;
    case 1:
        *arg1 = fn_800DAD4C();
        return;
    }
}