// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_80012550(s32 p0);
void fn_80012550(s32 p0) {
    *(volatile s32*)0xCC008000 = p0;
}
