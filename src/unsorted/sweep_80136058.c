// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 __AXCallbackAuxA;
extern s32 __AXContextAuxA;

void fn_80136058(s32 p0, s32 p1);
void fn_80136058(s32 p0, s32 p1) {
    __AXCallbackAuxA = p0;
    __AXContextAuxA = p1;
}
