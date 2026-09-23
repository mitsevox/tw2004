// LLPict_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled; the
// sweep code below is the matched small functions.

#include "engine.h"

// ---- sweep code (not yet cleaned up) ----

extern u8 lbl_801876C8[];
extern s32 lbl_802810C0;
void fn_80056204();
void fn_80056208();
void fn_8002F4FC(void);
void fn_8002F540(void);
void fn_8002FE70(void* arg0);
void fn_8002FEAC(void);
void fn_800B90F4();
void fn_800B91B8();
void fn_8002FEB0(u8* p0, u8* p1, s32 p2, s32 p3);
s32 fn_800B920C();
s32 fn_800B9808();
void fn_8002FF38(void* arg0, void* arg1);
void fn_8002FF94(void);

void fn_8002F4FC(void) {
    void* t1;
    fn_80056204();
    t1 = fn_80009B34(2048, 2, 32, (const char*)lbl_801876C8, 68);
    *(void**)((u8*)lbl_802810C0) = t1;
}

void fn_8002F540(void) {
    fn_80056208();
    fn_80009E70((void*)*(s32*)((u8*)lbl_802810C0));
}

void fn_8002FE70(void* arg0) {
    if (arg0 != NULL) {
        fn_80009E70((*(void**)((u8*)(arg0) + 0x60)));
        fn_80009E70(arg0);
    }
}

void fn_8002FEAC(void) {
}

void fn_8002FEB0(u8* p0, u8* p1, s32 p2, s32 p3) {
    void* t0;
    *(s32*)(p0 + 0x60) = 0;
    t0 = fn_80009B34(80, 1, 32, (const char*)lbl_801876C8, 278);
    *(void**)p1 = t0;
    *(s32*)(p1 + 0x4) = 0;
    fn_800B90F4(p2, p3);
    fn_800B91B8(*(s32*)p1);
}

void fn_8002FF38(void* arg0, void* arg1) {
    if ((u32) (*(u32*)((u8*)(arg0) + 0x60)) != 0U) {
        fn_80009E70((void*)(*(u32*)((u8*)(arg0) + 0x60)));
    }
    if ((u32) (*(u32*)((u8*)(arg1) + 4)) != 0U) {
        fn_800B9808((*(s32*)((u8*)(arg1) + 0)), (*(u32*)((u8*)(arg1) + 4)));
    }
    fn_800B920C((*(s32*)((u8*)(arg1) + 0)));
    fn_80009E70((void*)(*(s32*)((u8*)(arg1) + 0)));
}

void fn_8002FF94(void) {
}

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

void fn_800B9930();
void fn_8003001C(s32 p0, u8* p1);
void fn_8003009C(void);
s32 fn_800301D0(void* arg0);
s32 fn_800301F4(u8* p0);
s32 fn_8003020C(u8* p);
s32 fn_80030214(u8* p0);
s32 fn_80030234(u8* p0);
s32 fn_8003024C(u8* p);

void fn_8003001C(s32 p0, u8* p1) {
    fn_800B9930(*(s32*)(p1 + 0x0));
}

void fn_8003009C(void) {
}

s32 fn_800301D0(void* arg0) {
    return (*(s32*)((u8*)(arg0) + 0x60)) + ((s32) ((*(s32*)((u8*)(arg0) + 0x64)) * (*(s32*)((u8*)(arg0) + 0x68)) * 5) / 4);
}

s32 fn_800301F4(u8* p0) {
    return (*(s32*)(p0 + 0x60) + (*(s32*)(p0 + 0x64) * *(s32*)(p0 + 0x68)));
}

s32 fn_8003020C(u8* p) {
    return *(s32*)(p + 0x60);
}

s32 fn_80030214(u8* p0) {
    return (*(s32*)(p0 + 0x8) + ((u32)((*(s32*)p0 * *(s32*)(p0 + 0x4)) * 5) >> 2));
}

s32 fn_80030234(u8* p0) {
    return (*(s32*)(p0 + 0x8) + (*(s32*)p0 * *(s32*)(p0 + 0x4)));
}

s32 fn_8003024C(u8* p) {
    return *(s32*)(p + 0x8);
}

// ---- end of sweep code ----
