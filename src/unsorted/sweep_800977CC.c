// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "engine.h"

void fn_800977CC(void* arg0);
void fn_800977CC(void* arg0) {
    if ((u32) (*(u32*)((u8*)(arg0) + 0xC0)) != 0U) {
        Circular_MemFreeFunc((void*)(*(u32*)((u8*)(arg0) + 0xC0)));
    }
}