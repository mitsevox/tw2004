// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_800D27E0(u16* arg0);
s32 fn_800D27E0(u16* arg0) {
    return ((*arg0 - 1) % 7) + 1;
}