// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80077A80();
void fn_800E1260();
void fn_800E1404();
void fn_800E14E0();

void fn_8007BD1C(u8* p0);
void fn_8007BD44(u8* p0);
void fn_8007BD84(u8* p0);
void fn_8007BD1C(u8* p0) {
    fn_800E14E0((*(s32*)p0 & 0xFF));
}

void fn_8007BD44(u8* p0) {
    fn_800E1260(0);
    fn_800E1404(((*(s32*)p0 & 0xFF) - 1));
}

void fn_8007BD84(u8* p0) {
    fn_800E1260((*(s32*)p0 & 0xFF));
}
