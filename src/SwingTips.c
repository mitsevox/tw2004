// SwingTips.c (our name): the tips shown as a swing starts (fn_800D1DAC, from
// STATEFUNC_SwingInit). Each test (wind, lie, slope, the golfer's attributes, par and score) picks
// a tip; the first time a save profile meets one it gets the full tip (and a flag in the save),
// later a short random one. Not yet decompiled beyond the two tests below.

#include "game.h"

u8 fn_800D1AE0(void);
u8 fn_800D1D30(void);

// A tip test: lbl_802811F0's flag 0x2 is set.
u8 fn_800D1AE0(void) {
    return fn_80035574() != 0;
}

// A tip test that never fires.
u8 fn_800D1D30(void) {
    return 0;
}
