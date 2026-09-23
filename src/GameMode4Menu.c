#include "game_types.h"

extern u8 lbl_801946B0[];
extern u8 lbl_80260CB8[];
s32 fn_80102A44();
void fn_8012172C(u8* p0, u8* p1);
extern u8 gSession[];
s32 fn_80077B08();
void fn_801022BC();
void fn_80102468();
void fn_801217C4(void);
f32 fn_8000AD78(f32, f32);
extern f32 lbl_80284B00;
void fn_80121808(void* arg0, f32* arg1);
void fn_80102A58();
s32 fn_80121A38();
void fn_8012185C(u8* p0, u8* p1);
void fn_80121890(u8* p0);
s32 fn_801218BC(s32 arg0);
s32 fn_801218DC(s32 p0);
s32 fn_801218EC(s32 p0);

void fn_8012172C(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_80102A44(*(s32*)(lbl_801946B0 + (*(s32*)p0 << 2)));
    *(s32*)p1 = t0;
}

void fn_801217C4(void) {
    s32 t0;
    t0 = fn_80077B08();
    fn_801022BC(t0, *(s32*)(lbl_80260CB8 + 0x4));
    *(s32*)(gSession + 0x44) = 30;
    fn_80102468(gSession);
}

void fn_80121808(void* arg0, f32* arg1) {
    *arg1 = lbl_80284B00 * fn_8000AD78((*(f32*)((u8*)(arg0) + 8)) - (*(f32*)((u8*)(arg0) + 0)), -((*(f32*)((u8*)(arg0) + 0xC)) - (*(f32*)((u8*)(arg0) + 4))));
}

void fn_8012185C(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_80121A38(*(s32*)p0);
    *(s32*)p1 = t0;
}

void fn_80121890(u8* p0) {
    fn_80102A58(*(s32*)p0, *(s32*)(((u8*)*(s32*)(p0 + 0x4)) + 0x8));
}

s32 fn_801218BC(s32 arg0) {
    u8 var_r0;

    var_r0 = 0;
    if ((arg0 >= 0x12) && (arg0 < 0x18)) {
        var_r0 = 1;
    }
    return var_r0;
}

s32 fn_801218DC(s32 p0) {
    return (((u32)__cntlzw((24 - p0)) >> 5) & 0xFF);
}

s32 fn_801218EC(s32 p0) {
    return (((u32)__cntlzw((6 - p0)) >> 5) & 0xFF);
}
