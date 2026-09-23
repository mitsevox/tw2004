// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281BD8;
void fn_8000AE9C();
void fn_8000AF1C();
s32 fn_800951A0();

void fn_8000AF20(void);
void fn_8000AF20(void) {
    s32 t1;
    fn_8000AF1C();
    t1 = fn_800951A0(4096, 16, 1);
    lbl_80281BD8 = t1;
    fn_8000AE9C();
}
