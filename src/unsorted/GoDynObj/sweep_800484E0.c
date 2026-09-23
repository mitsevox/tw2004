// Small functions found by the sweep (sweep.py). Original file: GoDynObj.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

extern u8 lbl_80187B98[];

u8* fn_800484E0(s32 p0);
u8* fn_800484E0(s32 p0) {
    return (lbl_80187B98 + (p0 * 13));
}
