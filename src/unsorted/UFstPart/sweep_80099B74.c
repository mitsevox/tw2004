// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_800360D4();

void fn_80099B74(u8* p0);
void fn_80099B74(u8* p0) {
    *(s32*)(p0 + 0x58) = 0;
    fn_800360D4((p0 + 0x180));
}
