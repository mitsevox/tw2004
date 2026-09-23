// Small functions found by the sweep (sweep.py). Original file: rcmp_mad_codec.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"


s32 fn_800B9930(u8* p0);
s32 fn_800B9930(u8* p0) {
    return (((u32)__cntlzw((2 - *(u8*)(p0 + 0x14))) >> 5) & 0xFF);
}
