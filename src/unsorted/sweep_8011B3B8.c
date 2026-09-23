// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_8011B1DC();
void fn_8011B264();

void fn_8011B3B8(u8* p0, s32 p1);
void fn_8011B3E8(u8* p0, s32 p1);
void fn_8011B418(u8* p0, s32 p1);
void fn_8011B448(u8* p0, s32 p1);
void fn_8011B478(u8* p0, s32 p1);
void fn_8011B4A8(u8* p0, s32 p1);
void fn_8011B4D8(u8* p0, s32 p1);
void fn_8011B508(u8* p0, s32 p1);
void fn_8011B3B8(u8* p0, s32 p1) {
    fn_8011B264(*(u16*)(p0 + 0x20), *(u16*)(p0 + 0x22), p1, p0);
}

void fn_8011B3E8(u8* p0, s32 p1) {
    fn_8011B1DC(*(u16*)(p0 + 0x14), *(u16*)(p0 + 0x24), p1, p0);
}

void fn_8011B418(u8* p0, s32 p1) {
    fn_8011B1DC(*(u16*)(p0 + 0x26), *(u16*)(p0 + 0x2), p1, p0);
}

void fn_8011B448(u8* p0, s32 p1) {
    fn_8011B264(*(u16*)(p0 + 0x28), *(u16*)(p0 + 0x2A), p1, p0);
}

void fn_8011B478(u8* p0, s32 p1) {
    fn_8011B264(*(u16*)(p0 + 0x2C), *(u16*)(p0 + 0x2E), p1, p0);
}

void fn_8011B4A8(u8* p0, s32 p1) {
    fn_8011B264(*(u16*)(p0 + 0x30), *(u16*)(p0 + 0x32), p1, p0);
}

void fn_8011B4D8(u8* p0, s32 p1) {
    fn_8011B264(*(u16*)(p0 + 0x34), *(u16*)(p0 + 0x12), p1, p0);
}

void fn_8011B508(u8* p0, s32 p1) {
    fn_8011B1DC(*(u16*)(p0 + 0x36), *(u16*)(p0 + 0x2), p1, p0);
}
