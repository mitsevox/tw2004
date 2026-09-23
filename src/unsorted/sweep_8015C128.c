// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 InitializeUART(s32);
s32 OSGetConsoleType();
s32 WriteUARTN(s32, s32);
s32 __TRK_write_console(s32, s32, s32*, s32);
extern s32 lbl_80282A00;

s32 fn_8015C128(s32 arg0, s32 arg1, s32* arg2, s32 arg3);
s32 fn_8015C128(s32 arg0, s32 arg1, s32* arg2, s32 arg3) {
    s32 var_r3;

    if (!(OSGetConsoleType() & 0x20000000)) {
        var_r3 = 0;
        if ((s32) lbl_80282A00 == 0) {
            var_r3 = InitializeUART(0xE100);
            if (var_r3 == 0) {
                lbl_80282A00 = 1;
            }
        }
        if (var_r3 != 0) {
            return 1;
        }
        if (WriteUARTN(arg1, *arg2) != 0) {
            *arg2 = 0;
            return 1;
        }
        goto block_8;
    }
block_8:
    __TRK_write_console(arg0, arg1, arg2, arg3);
    return 0;
}