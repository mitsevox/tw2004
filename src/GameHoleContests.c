#include "game_types.h"

s32 Game_CurHoleIndex();
u8 fn_800ED6F0();
extern void* gpGame;
extern s32 lbl_80281568;
u8 fn_800DA174(void);
extern s32 lbl_8028156C;
u8 fn_800DA1D4(void);
extern u8 gPlayers[];
extern s32 lbl_80281570;
extern s32 lbl_80282278;
s32 fn_800DA234(void);
extern u8 lbl_80202828[];
extern u8 lbl_80202870[];
u8* fn_800DAD1C(s32 p0);
s32 fn_800DAD30(s32 p0);
extern u8 lbl_80282260;
extern s32 lbl_80282268;
u8 fn_800DAD44(void);
s32 fn_800DAD4C(void);

u8 fn_800DA174(void) {
    if ((fn_800ED6F0() != 0) && (Game_CurHoleIndex() == 0x11) && ((u8) (*(u8*)((u8*)(gpGame) + 0xD4)) == 0)) {
        return 1U;
    }
    return (u8) (lbl_80281568 == Game_CurHoleIndex());
}

u8 fn_800DA1D4(void) {
    if ((fn_800ED6F0() != 0) && (Game_CurHoleIndex() == 0x10) && ((u8) (*(u8*)((u8*)(gpGame) + 0xD4)) == 0)) {
        return 1U;
    }
    return (u8) (lbl_8028156C == Game_CurHoleIndex());
}

s32 fn_800DA234(void) {
    s32 t0;
    t0 = Game_CurHoleIndex();
    return (((u32)__cntlzw((t0 - lbl_80281570)) >> 5) & 0xFF);
}

u8* fn_800DAD1C(s32 p0) {
    return (lbl_80202828 + (p0 * 14));
}

s32 fn_800DAD30(s32 p0) {
    return *(s32*)(lbl_80202870 + (p0 << 2));
}

u8 fn_800DAD44(void) {
    return lbl_80282260;
}

s32 fn_800DAD4C(void) {
    return lbl_80282268;
}
