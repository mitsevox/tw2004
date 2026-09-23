// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 Bag_HasClub(s32, s32, void*);

void fn_80089ED0(void* arg0, s32* arg1);
void fn_80089ED0(void* arg0, s32* arg1) {
    if (Bag_HasClub((*(s32*)((u8*)(arg0) + 0)), (*(s32*)((u8*)(arg0) + 4)), arg0) != 0) {
        *arg1 = 1;
        return;
    }
    *arg1 = 0;
}