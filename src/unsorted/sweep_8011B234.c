// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_8011B264();

void fn_8011B234(u8* p0, s32 p1);
void fn_8011B234(u8* p0, s32 p1) {
    fn_8011B264(*(u16*)(p0 + 0xE), *(u16*)(p0 + 0x10), p1, p0);
}
