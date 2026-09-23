// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 OSEnableInterrupts();
extern s32* lbl_80281510;

void fn_80007328(void);
void fn_80007328(void) {
    *lbl_80281510 -= 1;
    if ((s32) *lbl_80281510 == 0) {
        OSEnableInterrupts();
    }
}