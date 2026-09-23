// Small functions found by the sweep (sweep.py). Original file: FEgolferanim.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

extern s32 lbl_80281EE0;
void fn_80009E70();
void fn_8008B820();
void fn_8008DBE8();
void fn_8008DC10();

void fn_8008B00C(void);
void fn_8008B00C(void) {
    fn_8008B820();
    fn_8008DBE8();
    fn_8008DC10();
    fn_80009E70(lbl_80281EE0);
    lbl_80281EE0 = 0;
}
