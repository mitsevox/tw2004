// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801F6338[];
void OSSignalSemaphore();

void fn_800B59EC(void);
void fn_800B59EC(void) {
    OSSignalSemaphore(lbl_801F6338);
}
