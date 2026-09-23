// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void OSRestoreInterrupts();

void fn_801654D0(u8* p);
void fn_801654D0(u8* p) {
    OSRestoreInterrupts(*(s32*)(p + 0x0));
}
