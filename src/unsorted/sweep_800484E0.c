// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80187B98[];

u8* fn_800484E0(s32 p0);
u8* fn_800484E0(s32 p0) {
    return (lbl_80187B98 + (p0 * 13));
}
