// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_8028255C;
extern s32 lbl_80282560;
s32 DVDGetCurrentDiskID();
s32 OSGetTick();
void fn_801229F8();
void fn_80175FB8();

void fn_80123FF8(void);
void fn_80123FF8(void) {
    s32 t0;
    s32 t1;
    t0 = DVDGetCurrentDiskID();
    lbl_8028255C = t0;
    t1 = OSGetTick();
    lbl_80282560 = t1;
    fn_801229F8();
    fn_80175FB8();
}
