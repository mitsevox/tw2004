// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 __AXFXAlloc;
extern s32 __AXFXFree;

void fn_801393C4(s32 p0, s32 p1);
void fn_801393C4(s32 p0, s32 p1) {
    __AXFXAlloc = p0;
    __AXFXFree = p1;
}
