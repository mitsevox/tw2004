// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_800ABC54(u8* p0);
void fn_800ABC54(u8* p0) {
    *(s32*)(p0 + 0x64) = 0;
    *(s32*)(p0 + 0x68) = 0;
    *(s32*)(p0 + 0x6C) = 0;
    *(s32*)(p0 + 0x70) = 0;
    *(s32*)(p0 + 0x74) = 0;
    *(s32*)(p0 + 0x78) = 0;
    *(s32*)(p0 + 0x7C) = 0;
    *(s32*)(p0 + 0x80) = 0;
    *(u16*)(p0 + 0x84) = (65536 - 1);
    *(u16*)(p0 + 0x86) = (65536 - 1);
    *(u8*)(p0 + 0x88) = 255;
    *(u8*)(p0 + 0x8A) = 0;
    *(u8*)(p0 + 0x89) = 0;
}
