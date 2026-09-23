// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void Session_SetNumPlayers();
void fn_800E0B38();
s32 fn_800E2520();
void fn_800E25E0();

void fn_8007BBA0(u8* p0, u8* p1);
void fn_8007BBD8(u8* p0, u8* p1);
void fn_8007BC10(u8* p0, u8* p1);
void fn_8007BC48(u8* p0);
void fn_8007BC74(u8* p0);
void fn_8007BBA0(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800E2520((*(s32*)p0 & 0xFF));
    *(s32*)p1 = t0;
}

void fn_8007BBD8(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800E2520((*(s32*)p0 & 0xFF));
    *(s32*)p1 = t0;
}

void fn_8007BC10(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800E2520((*(s32*)p0 & 0xFF));
    *(s32*)p1 = t0;
}

void fn_8007BC48(u8* p0) {
    fn_800E0B38((*(s32*)p0 & 0xFF));
    fn_800E25E0();
}

void fn_8007BC74(u8* p0) {
    Session_SetNumPlayers((*(s32*)p0 & 0xFF));
    fn_800E25E0();
}
