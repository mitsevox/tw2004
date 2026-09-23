// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 UStream_UnregisterHandler(s32);
u8 fn_80112B80();

void fn_8011E4A4(void);
void fn_8011E4A4(void) {
    if (fn_80112B80() != 0) {
        UStream_UnregisterHandler(0x67726173);
    }
}