// uiLoadFile.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled;
// the sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void UStream_UnregisterHandler();
void fn_8008ED28(void);
extern s32 lbl_80281F0C;
s32 fn_8008F0C0(void);
s32 fn_80009E70();
void fn_8008F0C8(u32 arg0);
extern u8 lbl_801D87A8[];
u8* fn_8008F0F0(void);
extern s32 lbl_80281F04;
s32 fn_8008F15C(void);
void fn_8008F164(u32 arg0);
extern s32 lbl_80281F08;
s32 fn_8008F18C(void);
s32 fn_80090B10();
extern void* lbl_80281F1C;
void fn_8008F24C(void);

void fn_8008ED28(void) {
    UStream_UnregisterHandler(1145132115);
    UStream_UnregisterHandler(1415071315);
    UStream_UnregisterHandler(1179602515);
    UStream_UnregisterHandler(1196576851);
    UStream_UnregisterHandler(1297105747);
}

s32 fn_8008F0C0(void) {
    return lbl_80281F0C;
}

void fn_8008F0C8(u32 arg0) {
    if (arg0 != 0U) {
        fn_80009E70();
    }
}

u8* fn_8008F0F0(void) {
    return lbl_801D87A8;
}

s32 fn_8008F15C(void) {
    return lbl_80281F04;
}

void fn_8008F164(u32 arg0) {
    if (arg0 != 0U) {
        fn_80009E70();
    }
}

s32 fn_8008F18C(void) {
    return lbl_80281F08;
}

void fn_8008F24C(void) {
    if ((u32) (*(u32*)((u8*)(lbl_80281F1C) + 0xC)) != 0U) {
        fn_80090B10();
        fn_8008F164((*(u32*)((u8*)(lbl_80281F1C) + 0xC)));
        (*(u32*)((u8*)(lbl_80281F1C) + 0xC)) = 0U;
    }
}

// ---- end of sweep code ----
