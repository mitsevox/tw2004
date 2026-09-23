// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_80070B50(s32 p0);
void fn_80070B5C(s32 p0);
void fn_80070B68(s32 p0);
void fn_80070B50(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}

void fn_80070B5C(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}

void fn_80070B68(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}
