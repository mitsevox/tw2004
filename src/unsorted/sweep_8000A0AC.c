// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80281BB0;
extern s32 lbl_80281BB4;
extern s32 lbl_80281BB8;

void fn_8000A0AC(s32 v);
s32 fn_8000A0B4(void);
void fn_8000A0BC(void);
void fn_8000A0C8(void);
void fn_8000A0D4(void);
s32 fn_8000A0E0(void);
void fn_8000A0AC(s32 v) {
    lbl_80281BB8 = v;
}

s32 fn_8000A0B4(void) {
    return lbl_80281BB8;
}

void fn_8000A0BC(void) {
    lbl_80281BB4 = 0;
}

void fn_8000A0C8(void) {
    lbl_80281BB0 = 1;
}

void fn_8000A0D4(void) {
    lbl_80281BB0 = 0;
}

s32 fn_8000A0E0(void) {
    return lbl_80281BB4;
}
