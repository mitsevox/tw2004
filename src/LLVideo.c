// LLVideo.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled; the
// sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern s32 lbl_80281200;
void fn_800757B8(void);
void fn_80007254();
void fn_80075C48(void);
void fn_80075C68(void);
s32 UStream_Close(s32);
s32 UStream_OpenFileByName(s32);
s32 UStream_SetAutoRead(s32);
s32 fn_80006FE8();
u8 fn_80007258();
s32 fn_800755F0(s32);
s32 fn_80075800();
s32 fn_800758B4(s32);
s32 fn_80075904(s32, s32);
s32 fn_800BA734(s32, s32);
s32 fn_80075DEC(s32, s32, s32);
extern u8 lbl_80281EB8;
void fn_80075FB8(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32 fn_80076088(u8* p);
s32 fn_80076090(u8* p0);
s32 fn_800760A0(u8* p);
u8 fn_800760A8(u8* p);

void fn_800757B8(void) {
    *(s32*)((u8*)lbl_80281200) = 0;
    *(s32*)(((u8*)lbl_80281200) + 0x4) = 0;
    *(s32*)(((u8*)lbl_80281200) + 0x8) = 0;
    *(s32*)(((u8*)lbl_80281200) + 0xC) = 0;
    *(s32*)(((u8*)lbl_80281200) + 0x10) = 0;
    *(s32*)(((u8*)lbl_80281200) + 0x14) = 0;
    *(s32*)(((u8*)lbl_80281200) + 0x18) = 0;
    *(s32*)(((u8*)lbl_80281200) + 0x1C) = 0;
}

void fn_80075C48(void) {
    fn_80007254();
}

void fn_80075C68(void) {
    fn_80007254();
}

void fn_80075FB8(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 temp_r3_2;
    s32 temp_r3_3;
    u8 temp_r3;

    temp_r3 = fn_80007258();
    lbl_80281EB8 = temp_r3;
    if (temp_r3 != 0) {
        fn_80006FE8();
    }
    fn_800BA734(0, 0);
    fn_800755F0(arg3 | 1);
    temp_r3_2 = UStream_OpenFileByName(arg0);
    if (temp_r3_2 != -1) {
        UStream_SetAutoRead(1);
        temp_r3_3 = fn_80075800();
        fn_80075904(0, temp_r3_3);
        fn_80075DEC(temp_r3_3, arg1, arg2);
        UStream_SetAutoRead(0);
        fn_800758B4(temp_r3_3);
        UStream_Close(temp_r3_2);
    }
}

s32 fn_80076088(u8* p) {
    return *(s32*)(p + 0x1008);
}

s32 fn_80076090(u8* p0) {
    return (((u32)__cntlzw(*(s32*)(p0 + 0x1008)) >> 5) & 0xFF);
}

s32 fn_800760A0(u8* p) {
    return *(s32*)(p + 0x10A4);
}

u8 fn_800760A8(u8* p) {
    return *(u8*)(p + 0x1022);
}

// ---- end of sweep code ----
