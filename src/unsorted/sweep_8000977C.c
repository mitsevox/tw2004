// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8009527C();
extern u32 lbl_80281BC0;
extern u32 lbl_80281BD4;

void fn_8000977C(void);
void fn_8000977C(void) {
    if ((u32) lbl_80281BC0 != 0U) {
        fn_8009527C(lbl_80281BC0);
    }
    if ((u32) lbl_80281BD4 != 0U) {
        fn_8009527C(lbl_80281BD4);
    }
    lbl_80281BC0 = 0U;
    lbl_80281BD4 = 0U;
}