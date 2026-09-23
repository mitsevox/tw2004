// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80224070[];

u8* fn_80117628(s32 p0);
u8* fn_80117628(s32 p0) {
    return (lbl_80224070 + (p0 * 80));
}
