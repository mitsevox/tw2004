// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80280E20;
void fn_8001A288();
void fn_8001A33C();
void fn_80027738();
void fn_80036460();
void fn_80036464();
void fn_800CCA1C();
void fn_800CCA3C();
void fn_800CEE88();
void fn_80112C64();
void fn_80112CEC();

void fn_8001C304(void);
void fn_8001C350(void);
void fn_8001C304(void) {
    fn_8001A288();
    fn_80027738(0);
    lbl_80280E20 = 3;
    fn_800CCA1C();
    fn_800CEE88(0);
    fn_80036460(1800);
    fn_80112C64(1);
}

void fn_8001C350(void) {
    fn_8001A33C();
    fn_800CCA3C();
    fn_80036464();
    fn_80112CEC();
}
