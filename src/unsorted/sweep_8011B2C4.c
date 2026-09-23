// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_8011B1DC();
void fn_8011B264();

void fn_8011B2C4(u8* p0, s32 p1);
void fn_8011B2F4(u8* p0, s32 p1);
void fn_8011B324(u8* p0, s32 p1);
void fn_8011B354(u8* p0, s32 p1);
void fn_8011B2C4(u8* p0, s32 p1) {
    fn_8011B264(*(u16*)(p0 + 0x12), *(u16*)(p0 + 0x14), p1, p0);
}

void fn_8011B2F4(u8* p0, s32 p1) {
    fn_8011B1DC(*(u16*)(p0 + 0x16), *(u16*)(p0 + 0x2), p1, p0);
}

void fn_8011B324(u8* p0, s32 p1) {
    fn_8011B1DC(*(u16*)(p0 + 0x18), *(u16*)(p0 + 0x12), p1, p0);
}

void fn_8011B354(u8* p0, s32 p1) {
    fn_8011B264(*(u16*)(p0 + 0x1A), *(u16*)(p0 + 0x1C), p1, p0);
}
