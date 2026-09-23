// Small functions found by the sweep (sweep.py). Original file: GoTerrain.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

extern u8 lbl_801B8980[];
double floor();

f32 fn_80035074(f32 x0);
f32 fn_80035074(f32 x0) {
    f32 t0;
    t0 = floor(x0);
    return t0;
}
