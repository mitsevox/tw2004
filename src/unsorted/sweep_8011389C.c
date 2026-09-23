// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80223BB0[];
void fn_80113840();

u8* fn_8011389C(void);
u8* fn_8011389C(void) {
    fn_80113840();
    *(s32*)(lbl_80223BB0 + 0x60) = 0;
    return lbl_80223BB0;
}
