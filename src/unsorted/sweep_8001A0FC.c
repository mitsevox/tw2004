// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8008E918(s32);
u8 fn_8008E938();
s32 fn_8008EAC8(s32);
s32 fn_8001A024(s32);

void fn_8001A0FC(s32 arg0);
void fn_8001A0FC(s32 arg0) {
    fn_80019C84();
    fn_8008E918(2);
    fn_8008EAC8(1);
    if (fn_8008E938() == 0) {
        fn_8001A024(arg0);
    }
}