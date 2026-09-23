// GoDynObj.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled; the
// sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern u8 lbl_80187D2C[];
extern s32 lbl_80281DA0;
s32 fn_80009B34();
void fn_800461A8(void);
void fn_80009E70();
void fn_80046264(void);
s32 fn_80012EF8();
s32 fn_80012F18(s32);
s32 fn_80012F50(s32, s32, s32);
s32 fn_80035118(s32, s32);
s32 fn_800352E4();
s32 fn_80035308();
s32 fn_80035338(s32);
s32 fn_80048F68(s32, s32, s32);
u8 fn_800E39F0();
void fn_80046FDC(s32 arg0);
s32 fn_800470B0(s32);
void fn_80046828(s32 arg0);
extern u8 lbl_80187B98[];
u8* fn_800484E0(s32 p0);
void fn_80048584(u8* p, u8 v);

// ---- sweep code (not yet cleaned up) ----

s32 UStream_RegisterHandler(s32, void (*)(void*), s32);
s32 fn_8000B4B8(void*);
u8 fn_8000B508();
s32 fn_80045D80(s32);
void fn_800460F8(void* arg0);
void fn_80045F74(void* arg0);
s32 fn_800075CC(s32);
void UStream_UnregisterHandler();
void fn_80046174(void);

void fn_80045F74(void* arg0) {
    if (fn_8000B508() == 0) {
        (*(s32*)((u8*)(arg0) + 4)) = fn_80045D80((*(s32*)((u8*)(arg0) + 0)));
        (*(void (**)(void*))((u8*)(arg0) + 8)) = fn_800460F8;
        fn_8000B4B8(arg0);
    }
}

void fn_800460F8(void* arg0) {
    void* temp_r31;

    temp_r31 = (*(void**)((u8*)(arg0) + 4));
    fn_800075CC((*(s32*)((u8*)(temp_r31) + 0x10)));
    fn_80009E70(temp_r31);
}

void fn_80046174(void) {
    UStream_UnregisterHandler(1413828384);
    UStream_UnregisterHandler(1111575628);
}

// ---- end of sweep code ----

void fn_800461A8(void) {
    s32 t0;
    t0 = fn_80009B34(2736, 2, 16, lbl_80187D2C, 283);
    lbl_80281DA0 = t0;
    *(s32*)(((u8*)t0) + 0xA54) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA58) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA5C) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA60) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA64) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA68) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA6C) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA70) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA74) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA78) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA7C) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA80) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA84) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA88) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA8C) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA90) = 0;
}

void fn_80046264(void) {
    fn_80009E70(lbl_80281DA0);
}

void fn_80046828(s32 arg0) {
    fn_80035118(4, 5);
    fn_80012F50(1, 6, 0x80);
    fn_80012F18(3);
    fn_80035338(1);
    fn_80035308();
    fn_800352E4();
    fn_80012EF8();
    fn_80048F68(3, 0, 0);
    if (fn_800E39F0() != 0) {
        fn_80046FDC(arg0);
        fn_800470B0(arg0);
    }
}

u8* fn_800484E0(s32 p0) {
    return (lbl_80187B98 + (p0 * 13));
}

void fn_80048584(u8* p, u8 v) {
    *(u8*)(p + 0x104) = v;
}

// ---- end of sweep code ----
