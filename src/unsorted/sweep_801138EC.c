// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_801138EC(s32 p0);
void fn_801138F8(s32 p0);
void fn_80113904(s32 p0);
void fn_801138EC(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}

void fn_801138F8(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}

void fn_80113904(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}
