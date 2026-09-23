// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8000A0E8(s32, s32);
s32 fn_8000A798(s32, s32);
s32 fn_8000ADC0(s32);
s32 fn_8001728C(s32);

void fn_80076954(s32 arg0, s32 arg1);
void fn_80076954(s32 arg0, s32 arg1) {
    if (arg1 == 0) {
        fn_8000ADC0(arg0 + 4);
        fn_8000ADC0(arg0 + 0x44);
    } else {
        fn_8000A0E8(arg1, arg0 + 4);
        fn_8000A798(arg1, arg0 + 0x44);
    }
    fn_8001728C(arg0);
}