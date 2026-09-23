// UFstPart.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled; the
// sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

s32 fn_80009E70(u8*);
s32 fn_800360A0(u8*);
void fn_80098BDC(u8* arg0);
extern u8 lbl_801DB888[];
void fn_80098C70(void);
void fn_800360D4();
void fn_80099B74(u8* p0);

void fn_80098BDC(u8* arg0) {
    (*(s8*)((u8*)(arg0) + 0x5C)) = 0;
    if ((s32) (*(s32*)((u8*)(arg0) + 0xB4)) < 0) {
        fn_800360A0(arg0 + 0x180);
        fn_80009E70(arg0);
    }
}

void fn_80098C70(void) {
    *(s32*)(((u8*)*(s32*)lbl_801DB888) + 0xB8) = (*(s32*)(((u8*)*(s32*)lbl_801DB888) + 0xB8) | 0x80000000);
    *(s32*)(((u8*)*(s32*)(lbl_801DB888 + 0x4)) + 0xB8) = (*(s32*)(((u8*)*(s32*)(lbl_801DB888 + 0x4)) + 0xB8) | 0x80000000);
    *(s32*)(((u8*)*(s32*)(lbl_801DB888 + 0x8)) + 0xB8) = (*(s32*)(((u8*)*(s32*)(lbl_801DB888 + 0x8)) + 0xB8) | 0x80000000);
    *(s32*)(((u8*)*(s32*)(lbl_801DB888 + 0xC)) + 0xB8) = (*(s32*)(((u8*)*(s32*)(lbl_801DB888 + 0xC)) + 0xB8) | 0x80000000);
    *(s32*)(((u8*)*(s32*)(lbl_801DB888 + 0x10)) + 0xB8) = (*(s32*)(((u8*)*(s32*)(lbl_801DB888 + 0x10)) + 0xB8) | 0x80000000);
    *(s32*)(((u8*)*(s32*)(lbl_801DB888 + 0x14)) + 0xB8) = (*(s32*)(((u8*)*(s32*)(lbl_801DB888 + 0x14)) + 0xB8) | 0x80000000);
}

void fn_80099B74(u8* p0) {
    *(s32*)(p0 + 0x58) = 0;
    fn_800360D4((p0 + 0x180));
}

// ---- end of sweep code ----
