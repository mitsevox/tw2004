// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80212B60[];
void fn_8010FC3C();

u8* fn_8010FF5C(s32 p0, s32 p1, s32 p2);
u8* fn_8010FF5C(s32 p0, s32 p1, s32 p2) {
    fn_8010FC3C(lbl_80212B60, p0, 1, p1, p2);
    return lbl_80212B60;
}
