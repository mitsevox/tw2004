// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800AB1B8();

void fn_800AA3D4(u8* arg0);
void fn_800AA3D4(u8* arg0) {
    if (!(*arg0 & 8)) {
        fn_800AB1B8();
    }
}