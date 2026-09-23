// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_800111D8();
void fn_800112DC();
void fn_80012EF8();
void fn_80012F18();
void fn_80012F34();
void fn_80012F50();

void fn_80012D88(void);
void fn_80012DCC(void);
void fn_80012D88(void) {
    fn_800111D8();
    fn_80012F50(1, 6, 1);
    fn_80012F34(0);
    fn_80012F18(7);
    fn_80012EF8();
}

void fn_80012DCC(void) {
    fn_800112DC();
    fn_80012F34(1);
    fn_80012F18(3);
    fn_80012EF8();
}
