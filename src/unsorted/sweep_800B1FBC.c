// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

int fn_800B13FC(s32* pnPort, s32* pnSlot);
int fn_800B14E4(s32* pnPort, s32* pnSlot);

void fn_800B1FBC(s32** p0, s32* p1);
void fn_800B1FFC(s32** p0, s32* p1);
void fn_800B1FBC(s32** p0, s32* p1) {
    *p1 = (u8)fn_800B14E4(p0[0], p0[1]);
}

void fn_800B1FFC(s32** p0, s32* p1) {
    *p1 = (u8)fn_800B13FC(p0[0], p0[1]);
}
