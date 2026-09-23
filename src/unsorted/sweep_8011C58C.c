// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_8011C58C(s32 p0, s32 p1, s32 p2);
s32 fn_8011C58C(s32 p0, s32 p1, s32 p2) {
    *(s32*)(((u8*)(p0 + 0x10000)) + 0x230) = p1;
    *(s32*)(((u8*)(p0 + 0x10000)) + 0x22C) = p2;
    return (p0 + 0x10000);
}
