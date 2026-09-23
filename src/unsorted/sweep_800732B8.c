// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern f32 lbl_80283A24;
extern f32 lbl_80283A28;
extern f32 lbl_80283A2C;

f32 fn_800732B8(f32 farg0, f32 farg1, f32 farg2, f32 farg3);
f32 fn_800732B8(f32 farg0, f32 farg1, f32 farg2, f32 farg3) {
    if (lbl_80283A24 == farg0) {
        return farg3;
    }
    if (lbl_80283A28 == farg0) {
        return farg1;
    }
    if (lbl_80283A2C == farg0) {
        return farg2;
    }
    return farg0;
}