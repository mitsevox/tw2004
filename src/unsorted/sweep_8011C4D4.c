// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_8011C1FC();

void fn_8011C4D4(s32 p0, s32 p1);
void fn_8011C4D4(s32 p0, s32 p1) {
    fn_8011C1FC(p0, p1, (p1 + (*(u16*)(((u8*)(p0 + 0x10000)) + 0x18) << 3)), *(u16*)(((u8*)(p0 + 0x10000)) + 0x18));
}
