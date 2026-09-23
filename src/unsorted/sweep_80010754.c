// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* lbl_80280DD8;

void fn_80010754(u8 v);
void fn_80010754(u8 v) {
    *(u8*)(lbl_80280DD8 + 0xC) = v;
}
