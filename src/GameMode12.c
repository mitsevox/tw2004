#include "game_types.h"

s32 fn_800FEC78(void);
extern s32 lbl_802823D8;
s32 fn_800FF604(void);
extern u8 lbl_80212418[];
extern u8 lbl_80212468[];
s32 fn_800FF60C(s32 p0, s32 p1);
s32 fn_800FF620(s32 p0, s32 p1);

s32 fn_800FEC78(void) {
    return 0;
}

s32 fn_800FF604(void) {
    return lbl_802823D8;
}

s32 fn_800FF60C(s32 p0, s32 p1) {
    return *(s32*)(lbl_80212468 + (p1 << 2));
}

s32 fn_800FF620(s32 p0, s32 p1) {
    return *(s32*)(lbl_80212418 + (p1 << 2));
}
