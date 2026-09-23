// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800D29E8();
s32 fn_800D33F0();
s32 fn_800E3BEC();
s32 fn_800EADD8();
s32 fn_800EDE78();
extern void* gpGame;

void fn_800DCC30(void);
void fn_800DCC30(void) {
    (*(s32 (**)(void*))((u8*)(gpGame) + 0x1CC))(gpGame);
    fn_800E3BEC();
    fn_800EDE78();
    fn_800EADD8();
    fn_800D33F0();
    fn_800D29E8();
}