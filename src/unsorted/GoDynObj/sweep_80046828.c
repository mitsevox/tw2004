// Small functions found by the sweep (sweep.py). Original file: GoDynObj.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

s32 fn_80012EF8();
s32 fn_80012F18(s32);
s32 fn_80012F50(s32, s32, s32);
s32 fn_80035118(s32, s32);
s32 fn_800352E4();
s32 fn_80035308();
s32 fn_80035338(s32);
s32 fn_80048F68(s32, s32, s32);
u8 fn_800E39F0();
void fn_80046FDC(s32 arg0);
s32 fn_800470B0(s32);

void fn_80046828(s32 arg0);
void fn_80046828(s32 arg0) {
    fn_80035118(4, 5);
    fn_80012F50(1, 6, 0x80);
    fn_80012F18(3);
    fn_80035338(1);
    fn_80035308();
    fn_800352E4();
    fn_80012EF8();
    fn_80048F68(3, 0, 0);
    if (fn_800E39F0() != 0) {
        fn_80046FDC(arg0);
        fn_800470B0(arg0);
    }
}