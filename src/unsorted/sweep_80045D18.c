// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281D98;
void fn_80012F34();
void fn_80045E3C();
void fn_80045E40();
void fn_80045E60();
s32 fn_80045E80();

void fn_80045D18(void);
void fn_80045D18(void) {
    s32 t0;
    t0 = fn_80045E80();
    lbl_80281D98 = t0;
    fn_80012F34(1);
    fn_80045E60(lbl_80281D98);
    fn_80045E40(lbl_80281D98);
    fn_80045E3C(0);
}
