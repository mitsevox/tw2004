// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Rand_Next(s32);

s32 fn_800AB32C(u32 arg0);
s32 fn_800AB32C(u32 arg0) {
    if (arg0 != 0) {
        return Rand_Next(1) % arg0;
    }
    return 0;
}