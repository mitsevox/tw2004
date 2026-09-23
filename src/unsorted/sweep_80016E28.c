// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801B8BA8[];

u8* fn_80016E28(s32 p0);
u8* fn_80016E28(s32 p0) {
    return (lbl_801B8BA8 + (p0 * 648));
}
