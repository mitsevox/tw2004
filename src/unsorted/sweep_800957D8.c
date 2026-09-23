// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_800957D8(void* arg0);
void fn_800957D8(void* arg0) {
    if (arg0 != NULL) {
        (*(s32*)((u8*)(arg0) + 0x20)) = 0;
        (*(s32*)((u8*)(arg0) + 0x1C)) = 0;
        (*(s32*)((u8*)(arg0) + 0x18)) = (s32) ((*(s32*)((u8*)(arg0) + 0x18)) & 0xFFFFFFFE);
    }
}