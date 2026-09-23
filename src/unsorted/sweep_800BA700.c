// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 fn_800BA080();
extern f32 lbl_8028420C;
extern f32 lbl_80284234;

f32 fn_800BA700(void);
f32 fn_800BA700(void) {
    if (fn_800BA080() != 0) {
        return lbl_80284234;
    }
    return lbl_8028420C;
}