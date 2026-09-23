// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 fn_800BA080();
extern f32 lbl_802841F0;
extern f32 lbl_802841F4;

f32 fn_800BA3D8(void);
f32 fn_800BA3D8(void) {
    if (fn_800BA080() != 0) {
        return lbl_802841F0;
    }
    return lbl_802841F4;
}