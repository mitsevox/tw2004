// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern void* lbl_80282690;

void __ARGetInterruptStatus(void* arg0);
void __ARGetInterruptStatus(void* arg0) {
    (*(void**)((u8*)(arg0) + 8)) = (void* ) lbl_80282690;
    lbl_80282690 = arg0;
}