#include "game_types.h"

void fn_800EADD8(void);
extern s32 lbl_802822F4;
extern s32 lbl_802822F8;
s32 fn_800EAC94();
void fn_800EAE38(s32 p0);
void fn_800EAE44(void);
s32 fn_800EAE6C(void);
extern u8 lbl_80203554[];
void UStream_UnregisterHandler();
void fn_8000E790();
void fn_800EAEB8(void);
void fn_800EAEEC(s32 p0);
extern s32 lbl_80281664;
extern s32 lbl_80281668;
void fn_800EC544(s32 p0, s32 p1);
extern u8 lbl_802822FC;
u8 fn_800EC550(void);
s32 Game_GetMode();
s32 fn_800ECA08(void);
void fn_800ECBE4(void);
extern u8 lbl_80282314;
u8 fn_800ED540(void);
void fn_800ED548(void);
extern u8 lbl_802822FE;
extern s32 (*lbl_8028231C)();
s32 fn_800ED5C8(void);
extern u8 lbl_802822FD;
void fn_800ED6E8(u8 v);
u8 fn_800ED6F0(void);
extern s32 lbl_802811F0;
void fn_800ED6F8(f32 x0);
void fn_80062D6C();
void fn_800ED710(s32 p0);

void fn_800EADD8(void) {
}

void fn_800EAE38(s32 p0) {
    lbl_802822F4 = p0;
    lbl_802822F8 = p0;
}

void fn_800EAE44(void) {
    s32 t0;
    t0 = fn_800EAC94();
    lbl_802822F4 = t0;
    lbl_802822F8 = t0;
}

s32 fn_800EAE6C(void) {
    return 29;
}

void fn_800EAEB8(void) {
    UStream_UnregisterHandler(1347180832);
    UStream_UnregisterHandler(1347180915);
}

void fn_800EAEEC(s32 p0) {
    fn_8000E790(p0, 10624, lbl_80203554);
}

void fn_800EC544(s32 p0, s32 p1) {
    lbl_80281664 = p0;
    lbl_80281668 = p1;
}

u8 fn_800EC550(void) {
    return lbl_802822FC;
}

s32 fn_800ECA08(void) {
    s32 t0;
    t0 = Game_GetMode();
    return (((u32)__cntlzw((8 - t0)) >> 5) & 0xFF);
}

void fn_800ECBE4(void) {
    if (Game_GetMode() == 8) {
        fn_800ED710(0);
    }
}

u8 fn_800ED540(void) {
    return lbl_80282314;
}

void fn_800ED548(void) {
    lbl_80282314 = 0;
}

s32 fn_800ED5C8(void) {
    if ((u8) lbl_802822FE != 0) {
        return 1;
    }
    return lbl_8028231C();
}

void fn_800ED6E8(u8 v) {
    lbl_802822FD = v;
}

u8 fn_800ED6F0(void) {
    return lbl_802822FD;
}

void fn_800ED6F8(f32 x0) {
    *(u8*)(((u8*)lbl_802811F0) + 0x1C) = 1;
    *(f32*)(((u8*)lbl_802811F0) + 0x18) = x0;
}

void fn_800ED710(s32 p0) {
    fn_80062D6C(18, p0);
}
