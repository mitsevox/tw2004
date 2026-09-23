// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_800A44A0();
void fn_800A47A0();
void fn_800A49A4(u8 arg0);
void fn_800A75B4(void);
s32 fn_800A6660(s32);
extern u8 lbl_8028202E;

void fn_800A72EC(u8 arg0, u8 arg1);
void fn_800A72EC(u8 arg0, u8 arg1) {
    if ((lbl_8028202E ^ arg0) != 0) {
        lbl_8028202E = arg0;
        if (arg0 != 0) {
            fn_800A49A4(0);
            fn_800A75B4();
            fn_800A6EC8();
            fn_800A6660(0);
            return;
        }
        if (arg1 != 0) {
            fn_800A44A0();
            fn_800A47A0();
        }
    }
}