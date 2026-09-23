// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800E3EE0();
s32 fn_800E4C20(s32);
s32 fn_800E4D88();
u8 fn_800ED900();
s32 fn_800ED9A0();

void fn_800EDA34(void);
void fn_800EDA34(void) {
    if (fn_800ED900() != 0) {
        fn_800E4D88();
        fn_800E3EE0();
    } else {
        fn_800E4C20(0);
    }
    fn_800ED9A0();
}