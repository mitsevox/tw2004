// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80282330;
void GM_EndOfGolferTurn();

void fn_800ED974(void);
void fn_800ED974(void) {
    lbl_80282330 = 1;
    GM_EndOfGolferTurn(0);
}
