// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281980;
extern s32 lbl_80282518;

void fn_8012408C(s32 v);
s32 fn_80124094(void);
void fn_8012409C(void);
void fn_8012408C(s32 v) {
    lbl_80281980 = v;
}

s32 fn_80124094(void) {
    return lbl_80281980;
}

void fn_8012409C(void) {
    lbl_80282518 = 1;
}
