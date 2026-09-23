#include "game_types.h"

void fn_800F18C8();
void fn_800F1404(void);
s32 fn_800F193C(void);
s32 fn_800F1944(void);
extern u8 gSession[];
u8* fn_800F194C(void);
s32 Game_GetMode();
s32 fn_800F7DE8();
void fn_800F1DF0(void);
s32 fn_800F3828();
s32 fn_800F48C4();
void fn_800F1E1C(void);
s32 fn_800F37F8(s32);
s32 fn_800F59CC(s32);
s32 fn_800F6A00(s32);
s32 fn_800F7D94(s32);
s32 fn_800F2408(s32 arg0);
s32 fn_800F7D9C(s32);
s32 fn_800F2494(s32 arg0);
s32 fn_800F59D4(s32);
s32 fn_800F80A0(s32);
s32 fn_800F24D8(s32 arg0);
s32 fn_800F6A34(s32);
s32 fn_800F2534(s32 arg0);
void fn_800F263C(s32 p0);
void fn_800F2664(void);
void fn_800F2668(void);
void fn_800A7664();
void fn_800F2958(s32 p0, s32 p1);

void fn_800F1404(void) {
    fn_800F18C8();
}

s32 fn_800F193C(void) {
    return 1;
}

s32 fn_800F1944(void) {
    return 1;
}

u8* fn_800F194C(void) {
    *(u8*)(gSession + 0x12) = 1;
    return gSession;
}

void fn_800F1DF0(void) {
    if (Game_GetMode() == 0xD) {
        fn_800F7DE8();
    }
}

void fn_800F1E1C(void) {
    if (Game_GetMode() == 0xE) {
        fn_800F3828();
    }
    if (Game_GetMode() == 0xF) {
        fn_800F48C4();
    }
}

s32 fn_800F2408(s32 arg0) {
    if (Game_GetMode() == 0xD) {
        return fn_800F7D94(arg0);
    }
    if (Game_GetMode() == 0xE) {
        return fn_800F37F8(arg0);
    }
    if (Game_GetMode() == 0x10) {
        return fn_800F59CC(arg0);
    }
    if (Game_GetMode() == 0x11) {
        return fn_800F6A00(arg0);
    }
    return 0;
}

s32 fn_800F2494(s32 arg0) {
    if (Game_GetMode() == 0xD) {
        return fn_800F7D9C(arg0);
    }
    return 0;
}

s32 fn_800F24D8(s32 arg0) {
    if (Game_GetMode() == 0xD) {
        return fn_800F80A0(arg0);
    }
    if (Game_GetMode() == 0x10) {
        return fn_800F59D4(arg0);
    }
    return 0;
}

s32 fn_800F2534(s32 arg0) {
    if (Game_GetMode() == 0x11) {
        return fn_800F6A34(arg0);
    }
    return 0;
}

void fn_800F263C(s32 p0) {
    fn_800F2958((p0 & 0xFFFF), 1);
}

void fn_800F2664(void) {
}

void fn_800F2668(void) {
}

void fn_800F2958(s32 p0, s32 p1) {
    fn_800A7664(7, p0, p1);
}
