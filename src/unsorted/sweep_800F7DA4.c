// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Rand_Next(s32);
s32 fn_800A624C();
s32 fn_800F263C(s32);

void fn_800F7DA4(void);
void fn_800F7DA4(void) {
    fn_800A624C();
    if (!(Rand_Next(0) & 1)) {
        fn_800F263C(0x15);
        return;
    }
    fn_800F263C(0x25);
}