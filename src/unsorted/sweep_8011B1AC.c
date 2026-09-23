// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_8011B1DC();

void fn_8011B1AC(u8* p0, s32 p1);
void fn_8011B1AC(u8* p0, s32 p1) {
    fn_8011B1DC(*(s32*)(p0 + 0x8), *(u16*)(p0 + 0x6), p1, p0);
}
