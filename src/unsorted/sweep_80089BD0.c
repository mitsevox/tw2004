// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_80082DBC();
void fn_80082E10();

void fn_80089BD0(void);
void fn_80089BD4(s32 a, u8* p);
void fn_80089BE0(void);
void fn_80089C00(void);
void fn_80089BD0(void) {
}

void fn_80089BD4(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 0;
}

void fn_80089BE0(void) {
    fn_80082DBC();
}

void fn_80089C00(void) {
    fn_80082E10();
}
