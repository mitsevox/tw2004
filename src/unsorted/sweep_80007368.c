// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 OSDisableInterrupts();
extern s32* lbl_80281510;

void fn_80007368(void);
void fn_80007368(void) {
    if ((s32) *lbl_80281510 == 0) {
        OSDisableInterrupts();
    }
    *lbl_80281510 += 1;
}