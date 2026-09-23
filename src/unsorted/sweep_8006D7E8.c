// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80012A78();
u8 fn_800170A0(s32);
s32 fn_800382E0();
s32 fn_8006C8EC(s32);
s32 fn_8008F648(s32);
s32 fn_8008FD60(s32);
s32 fn_8009069C();

void fn_8006D7E8(void);
void fn_8006D7E8(void) {
    if (fn_800170A0(0) != 0) {
        fn_8006C8EC(0);
        fn_8008F648(1);
    }
    fn_80012A78();
    fn_8008FD60(1);
    fn_8009069C();
    fn_800382E0();
}