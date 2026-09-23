// Small functions found by the sweep (sweep.py). Original file: MC_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

s32 fn_8009D74C();
void fn_8009DBAC();
void fn_8009DC80();

void fn_8009DCEC(s32 p0, s32 p1);
void fn_8009DCEC(s32 p0, s32 p1) {
    s32 t0;
    t0 = fn_8009D74C();
    fn_8009DC80(p0, p1, t0);
    fn_8009DBAC(p0, p1);
}
