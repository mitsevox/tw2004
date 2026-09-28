// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_80005520();
void fn_80005590();
void fn_800083A4();
void StaticMem_InitOnce();

s32 main(s32 p0, s32 p1);
s32 main(s32 p0, s32 p1) {
    StaticMem_InitOnce();
    fn_80005520();
    fn_800083A4(p0, p1);
    fn_80005590();
    return 0;
}
