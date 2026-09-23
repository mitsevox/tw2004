// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80012A78();
u8 fn_800170A0(s32);
s32 fn_800382E0();
s32 fn_8006C8EC(s32);
s32 fn_8006DDA8();
s32 fn_8006DE28();
s32 fn_8006DFA8();
s32 fn_8006E028();
s32 fn_80077168();
s32 fn_8008B864();
s32 fn_8008B9A0();
s32 fn_8008BAB4();
s32 fn_8008C844();
s32 fn_8008C8C4();
u8 fn_8008EB10();
s32 fn_8008F648(s32);
s32 fn_8008FD60(s32);
s32 fn_8009069C();
s32 fn_800A2BA8();
s32 fn_800BA1A4();
s32 fn_801242D0();
extern u8 lbl_80281E50;

void fn_8006D838(void);
void fn_8006D838(void) {
    u8 temp_r3;

    fn_8006E028();
    fn_8006DFA8();
    fn_800A2BA8();
    fn_8008B864();
    if (fn_800170A0(0) != 0) {
        fn_8006C8EC(0);
        temp_r3 = fn_8008EB10();
        if (temp_r3 != 0) {
            fn_8008BAB4();
            fn_8008B9A0();
            fn_8008C844();
        }
        if ((u8) lbl_80281E50 != 0) {
            fn_8008F648(1);
        }
        if (temp_r3 != 0) {
            fn_8008C8C4();
        }
    }
    fn_80012A78();
    fn_800BA1A4();
    fn_8009069C();
    fn_8008FD60(1);
    fn_800382E0();
    fn_80077168();
    fn_8006DE28();
    fn_8006DDA8();
    fn_801242D0();
}