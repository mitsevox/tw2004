// Small functions found by the sweep (sweep.py). Original file: GoTerrain.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

extern u8 lbl_801D3CB0[];

f32 fn_800336E4(void);
f32 fn_800336F4(void);
f32 fn_800336E4(void) {
    return *(f32*)(lbl_801D3CB0 + 0x1178);
}

f32 fn_800336F4(void) {
    return *(f32*)(lbl_801D3CB0 + 0x1170);
}
