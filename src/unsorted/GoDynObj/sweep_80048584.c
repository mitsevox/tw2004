// Small functions found by the sweep (sweep.py). Original file: GoDynObj.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"


void fn_80048584(u8* p, u8 v);
void fn_80048584(u8* p, u8 v) {
    *(u8*)(p + 0x104) = v;
}
