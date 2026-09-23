// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80282088;
void fn_800A8D54();
void fn_800A955C();
s32 fn_800B5BD8();

void fn_800A8D00(void);
s32 fn_800A8D2C(void);
void fn_800A8D00(void) {
    s32 t1;
    fn_800A955C(0);
    t1 = fn_800B5BD8();
    lbl_80282088 = t1;
}

s32 fn_800A8D2C(void) {
    fn_800A8D54();
    fn_800A8D00();
    return 1;
}
