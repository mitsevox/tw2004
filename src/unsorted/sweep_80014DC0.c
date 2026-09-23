// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 UStream_Update();
s32 fn_800150B8();
s32 fn_800150E0();
s32 fn_8009198C(s32);

void fn_80014DC0(void);
void fn_80014DC0(void) {
    fn_800150E0();
    do {
        fn_8009198C(0);
    } while (UStream_Update() != 0);
    fn_800150B8();
}