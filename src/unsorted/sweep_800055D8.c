// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "engine.h"

void fn_80005520();
void fn_80005590();
void fn_800083A4();

s32 main(s32 p0, s32 p1);
s32 main(s32 p0, s32 p1) {
    Circular_InitOnce();
    fn_80005520();
    fn_800083A4(p0, p1);
    fn_80005590();
    return 0;
}
