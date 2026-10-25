// SitDevSignedValues.c: which of the state values the situation scripts test are signed:
// SitDev_CompareConditions compares a "greater" or "less" test on these as signed 16-bit numbers
// (relative scores, leads, angles).

#include "game_types.h"
#include "sitdev.h"

u8 gSitDevValueIsSigned[SITDEV_NUM_SIGNED_FLAGS] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 1, 0, 1,
    1, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};
