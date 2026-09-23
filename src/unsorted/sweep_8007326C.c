// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_8007326C(void* arg0);
void fn_8007326C(void* arg0) {
    (*(s32*)((u8*)(arg0) + 4)) = (s32) ((*(s32*)((u8*)(arg0) + 4)) & 0xFFFFFFFC);
}