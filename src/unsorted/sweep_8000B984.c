// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281C04;
extern s32 lbl_80281C08;
extern s32 lbl_80281C0C;
s32 fn_800951A0();

void fn_8000B984(void);
void fn_8000B984(void) {
    s32 t0;
    s32 t1;
    s32 t2;
    t0 = fn_800951A0(147700, 16, 1);
    lbl_80281C04 = t0;
    t1 = fn_800951A0(52, 16, 1);
    lbl_80281C0C = t1;
    t2 = fn_800951A0(32768, 16, 1);
    lbl_80281C08 = t2;
}
