// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 fn_800BA080();
extern f32 lbl_802841F8;
extern f32 lbl_802841FC;

f32 fn_800BA40C(void);
f32 fn_800BA40C(void) {
    if (fn_800BA080() != 0) {
        return lbl_802841F8;
    }
    return lbl_802841FC;
}