// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80193AC8[];
extern s32 lbl_80281EE0;
s32 Rand_Next();
void fn_80016D10();
s32 fn_80017028();
s32 fn_80077ACC();
void fn_8007873C();
void fn_80079664();
void fn_8008E244();
void fn_8008E364();
void fn_800A73F0();
void fn_800C3C40();
void fn_80103B74();

void fn_80109CBC(void);
void fn_80109CBC(void) {
    s32 t0;
    s32 t2;
    s32 t3;
    t0 = fn_80077ACC();
    fn_80016D10();
    t2 = fn_80017028();
    t3 = Rand_Next(0);
    fn_800A73F0(((t3 & 0x7) + 11));
    fn_8008E244();
    fn_8008E364(0);
    fn_800C3C40(t2, lbl_80193AC8, *(s32*)(((u8*)lbl_80281EE0) + 0x4), 0, 0, 0);
    fn_80103B74(0);
    fn_80079664(t0);
    fn_8007873C(t0);
}
