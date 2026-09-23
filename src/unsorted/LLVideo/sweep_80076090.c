// Small functions found by the sweep (sweep.py). Original file: LLVideo.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"


s32 fn_80076090(u8* p0);
s32 fn_80076090(u8* p0) {
    return (((u32)__cntlzw(*(s32*)(p0 + 0x1008)) >> 5) & 0xFF);
}
