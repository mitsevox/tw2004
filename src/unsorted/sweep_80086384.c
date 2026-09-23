// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_800E4238();
void fn_800E42F4();

void fn_80086384(u8* p);
void fn_800863A8(u8* p);
void fn_80086384(u8* p) {
    fn_800E42F4(*(s32*)(p + 0x0));
}

void fn_800863A8(u8* p) {
    fn_800E4238(*(s32*)(p + 0x0));
}
