// Small functions found by the sweep (sweep.py). Original file: GoPostFx.c (EA's name, from its asserts; also in TW06; placed by the file map).

#include "game_types.h"

u8 fn_8003944C();
s32 fn_80038314();
s32 fn_800383A8();
s32 fn_80038438();

void fn_800382E0(void);
void fn_800382E0(void) {
    if (fn_8003944C() == 0) {
        fn_80038314();
        fn_800383A8();
        fn_80038438();
    }
}