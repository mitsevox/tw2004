// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80205F3C[];

u8* fn_800EFE60(s32 p0);
u8* fn_800EFE60(s32 p0) {
    return ((lbl_80205F3C + (p0 * 100)) + 0x14);
}
