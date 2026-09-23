// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_8011B1DC();

void fn_8011B56C(u8* p0, s32 p1);
void fn_8011B59C(u8* p0, s32 p1);
void fn_8011B5CC(u8* p0, s32 p1);
void fn_8011B56C(u8* p0, s32 p1) {
    fn_8011B1DC(*(u16*)(p0 + 0x38), *(u16*)(p0 + 0x2A), p1, p0);
}

void fn_8011B59C(u8* p0, s32 p1) {
    fn_8011B1DC(*(u16*)(p0 + 0x3A), *(u16*)(p0 + 0x2E), p1, p0);
}

void fn_8011B5CC(u8* p0, s32 p1) {
    fn_8011B1DC(*(u16*)(p0 + 0x3C), *(u16*)(p0 + 0x32), p1, p0);
}
