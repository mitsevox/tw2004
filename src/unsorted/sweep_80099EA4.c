// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern void* lbl_80281F88;

void fn_80099EA4(void* arg0);
void fn_80099EA4(void* arg0) {
    (*(void**)((u8*)(arg0) + 0x40)) = (void* ) lbl_80281F88;
    lbl_80281F88 = arg0;
}