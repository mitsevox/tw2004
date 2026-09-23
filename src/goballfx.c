#include "game_types.h"

s32 fn_8001F004();
s32 fn_8003532C();
s32 fn_80093A04(s32, s32);
void fn_80093824(void);
void fn_8006F400();
void fn_8006F430();
void fn_80093900(u8* p0);
s32 fn_8006E5A8(s32, s32);
s32 fn_8006F144(s32);
void fn_80093990(s32 arg0);
s32 fn_8006E62C(s32);
s32 fn_8006F148(s32);
void fn_800939CC(s32 arg0);
extern u8 lbl_80281F40;
void fn_80093AD4(void);
extern u8 lbl_801D94B0[];
void fn_800360A0();
void fn_80093D14(void);

void fn_80093824(void) {
    fn_8003532C();
    fn_80093A04(0, fn_8001F004());
}

void fn_80093900(u8* p0) {
    s32 t0;
    t0 = fn_8003532C();
    fn_8006F430(*(f32*)(p0 + 0x80));
    fn_8006F400(t0, 0, p0, *(f32*)(p0 + 0x10));
    fn_8006F400(t0, 1, (p0 + 0x20), *(f32*)(p0 + 0x30));
    fn_8006F400(t0, 2, (p0 + 0x40), *(f32*)(p0 + 0x50));
    fn_8006F400(t0, 3, (p0 + 0x60), *(f32*)(p0 + 0x70));
}

void fn_80093990(s32 arg0) {
    fn_8006E5A8(arg0 + 0x54, 5);
    fn_8006F144(arg0);
}

void fn_800939CC(s32 arg0) {
    fn_8006E62C(arg0 + 0x54);
    fn_8006F148(arg0);
}

void fn_80093AD4(void) {
    lbl_80281F40 = 1;
}

void fn_80093D14(void) {
    fn_800360A0(lbl_801D94B0);
}
