#include "game_types.h"

extern u8 lbl_801F8458[];
u8 fn_800BA080(void);
extern u8 lbl_801F846C[];
void UStream_UnregisterHandler();
void Mem_cpy();
void fn_80009E70();
s32 fn_800107C0();
void fn_800BA0E4(void);
void fn_800BA118(u8* p0);
void fn_800BA15C(s32 p0);
extern f32 lbl_802841EC;
f32 fn_800BA3A4(void);
extern f32 lbl_802841F0;
extern f32 lbl_802841F4;
f32 fn_800BA3D8(void);
extern f32 lbl_802841F8;
extern f32 lbl_802841FC;
f32 fn_800BA40C(void);
extern f32 lbl_80284230;
f32 fn_800BA6CC(void);
extern f32 lbl_8028420C;
extern f32 lbl_80284234;
f32 fn_800BA700(void);

u8 fn_800BA080(void) {
    return ((u32)((-1 - *(s32*)(lbl_801F8458 + 0x10)) | (*(s32*)(lbl_801F8458 + 0x10) + 1)) >> 31);
}

void fn_800BA0E4(void) {
    UStream_UnregisterHandler(1414676824);
    UStream_UnregisterHandler(1414682708);
}

void fn_800BA118(u8* p0) {
    Mem_cpy(lbl_801F846C, *(s32*)p0, 7296);
    fn_80009E70(p0);
}

void fn_800BA15C(s32 p0) {
    s32 t0;
    t0 = fn_800107C0(p0, 0, 0);
    *(s32*)(lbl_801F8458 + 0x10) = t0;
    fn_80009E70(p0, lbl_801F8458);
}

f32 fn_800BA3A4(void) {
    if (fn_800BA080() != 0) {
        return lbl_802841EC;
    }
    return lbl_802841EC;
}

f32 fn_800BA3D8(void) {
    if (fn_800BA080() != 0) {
        return lbl_802841F0;
    }
    return lbl_802841F4;
}

f32 fn_800BA40C(void) {
    if (fn_800BA080() != 0) {
        return lbl_802841F8;
    }
    return lbl_802841FC;
}

f32 fn_800BA6CC(void) {
    if (fn_800BA080() != 0) {
        return lbl_80284230;
    }
    return lbl_80284230;
}

f32 fn_800BA700(void) {
    if (fn_800BA080() != 0) {
        return lbl_80284234;
    }
    return lbl_8028420C;
}
