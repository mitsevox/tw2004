// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_8011C59C(s32 p0, s32 p1);
void fn_8011C5A8(s32 p0, s32 p1);
void fn_8011C59C(s32 p0, s32 p1) {
    *(s32*)(((u8*)(p0 + 0x10000)) + 0x238) = p1;
}

void fn_8011C5A8(s32 p0, s32 p1) {
    *(s32*)(((u8*)(p0 + 0x10000)) + 0x234) = p1;
}
