// Small functions found by the sweep (sweep.py). Original file: UFstPart.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

void fn_800360D4();

void fn_80099B74(u8* p0);
void fn_80099B74(u8* p0) {
    *(s32*)(p0 + 0x58) = 0;
    fn_800360D4((p0 + 0x180));
}
