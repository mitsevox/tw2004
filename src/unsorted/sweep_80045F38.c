// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_80045F38(void* arg0);
void fn_80045F38(void* arg0) {
    if ((u32) (*(u32*)((u8*)(arg0) + 0x1000)) != 0U) {
        (*(u32*)((u8*)(arg0) + 0x1000)) = 0U;
    }
}