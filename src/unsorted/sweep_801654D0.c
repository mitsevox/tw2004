// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void OSRestoreInterrupts();

void MWExitCriticalSection(u8* p);
void MWExitCriticalSection(u8* p) {
    OSRestoreInterrupts(*(s32*)(p + 0x0));
}
