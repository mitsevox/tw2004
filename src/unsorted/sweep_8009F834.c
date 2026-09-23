// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801F1510[];

u8* fn_8009F834(s32 p0, s32 p1);
u8* fn_8009F834(s32 p0, s32 p1) {
    return ((lbl_801F1510 + (p0 * 152)) + (p1 * 152));
}
