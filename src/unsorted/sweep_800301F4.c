// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_800301F4(u8* p0);
s32 fn_800301F4(u8* p0) {
    return (*(s32*)(p0 + 0x60) + (*(s32*)(p0 + 0x64) * *(s32*)(p0 + 0x68)));
}
