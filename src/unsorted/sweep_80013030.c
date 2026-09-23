// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u32 SIProbe(s32 nChan);
void fn_8001437C();

void fn_80013030(void);
u32 fn_80013050(s32 nChan);
void fn_80013030(void) {
    fn_8001437C();
}

u32 fn_80013050(s32 nChan) {
    return SIProbe(nChan);
}
