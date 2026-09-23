// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 OSDisableInterrupts();

void MWEnterCriticalSection(u8* p0);
void MWEnterCriticalSection(u8* p0) {
    s32 t0;
    t0 = OSDisableInterrupts();
    *(s32*)p0 = t0;
}
