// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8008E6D4(s32);

void fn_801097FC(s32* arg0);
void fn_801097FC(s32* arg0) {
    s32 temp_r0;

    temp_r0 = *arg0;
    switch (temp_r0) {
    case 0:
        fn_8008E6D4(0);
        return;
    case 1:
        fn_8008E6D4(1);
        return;
    case 2:
        fn_8008E6D4(4);
        return;
    case 3:
        fn_8008E6D4(5);
        return;
    case 4:
        fn_8008E6D4(2);
        return;
    default:
        fn_8008E6D4(0);
        return;
    }
}