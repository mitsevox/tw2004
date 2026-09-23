// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80193AD4[];
extern s32 lbl_80281EE0;
void fn_80016D10();
s32 fn_80017028();
s32 fn_80077ACC();
void fn_80078E34();
void fn_8008E244();
void fn_8008E364();
void fn_800C3C40();
void fn_80103B74();

void fn_80109D5C(void);
void fn_80109D5C(void) {
    s32 t0;
    s32 t2;
    t0 = fn_80077ACC();
    fn_80016D10();
    t2 = fn_80017028();
    fn_8008E244();
    fn_8008E364(1);
    fn_800C3C40(t2, lbl_80193AD4, *(s32*)(((u8*)lbl_80281EE0) + 0x4), 0, 0, 0);
    fn_80103B74(0);
    fn_80078E34(t0);
}
