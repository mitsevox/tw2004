// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_80075250(s32 p0);
void fn_8007525C(s32 p0);
void fn_80075268(s32 p0);
void fn_80075274(s32 p0);
void fn_80075250(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}

void fn_8007525C(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}

void fn_80075268(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}

void fn_80075274(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}
