// EASportsBio.c (EA's name, from its asserts): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

s32 fn_80009E70();
extern u8* lbl_80281988;
void fn_801250C0(void);
s32 fn_80125354(void);
s32 fn_801253F0(s32 arg0, s32 arg1);
s32 fn_80125194(s32 arg0, s32 arg1);
s32 fn_8012D694();
s32 fn_80124A98();
s32 fn_80125280(void);
s32 fn_8009D74C(s32, s32);
s32 fn_8009DBAC(s32, s32);
s32 fn_8012D5E4(s32, s32);
s32 fn_80124AAC();
s32 fn_801252D0(void);
s32 fn_8012D560();
s32 fn_8012D9B4();
s32 fn_8012D5B0();
s32 fn_8012D6C8();
s32 fn_80125434(s32 arg0, s32 arg1);
s32 fn_8012D7F0();
s32 fn_801254B8(void);
s32 fn_8012D710();
s32 fn_801254EC(void);
s32 fn_80125520(void);
void fn_8009D1D8();
void fn_801255C4(u8* p0);
s32 fn_801255F8(void);
extern u8 lbl_80282568;
extern s32 lbl_8028256C;
extern s32 lbl_80282574;
extern u8 lbl_8028257C;
void fn_80125648(s32 p0, s32 p1, s32 p2);
extern u8 lbl_80282569;
extern u8 lbl_8028256A;
void fn_8012566C(s32 p0);
void fn_80125680(s32 p0);
u8 fn_80125694(void);
void fn_801256A0(s32 v);
s32 fn_801256AC(void);
s32 fn_80106ED8(s32, s32);
s32 fn_80107084(s32, s32);
s32 fn_801258E8();
s32 fn_8012DAB8(u16);
u8 fn_801257A0(void);
void fn_8012D8C4();
void fn_8012D93C();
void fn_8012DD7C();
void fn_80125814(void);
void fn_80125834(void);
void fn_80125854(void);
void fn_80125910(u8 v);
u8 fn_8012591C(void);

void fn_801250C0(void) {
    if ((u32) (*(u32*)((u8*)(lbl_80281988) + 0)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(lbl_80281988) + 0)));
    }
    if ((u32) (*(u32*)((u8*)(lbl_80281988) + 4)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(lbl_80281988) + 4)));
    }
    if ((u32) (*(u32*)((u8*)(lbl_80281988) + 8)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(lbl_80281988) + 8)));
    }
}

s32 fn_80125194(s32 arg0, s32 arg1) {
    s32 temp_r3;

    temp_r3 = fn_80125354();
    if (temp_r3 == 0) {
        fn_801253F0(arg0, arg1);
    }
    return temp_r3;
}

s32 fn_80125280(void) {
    s32 temp_r3;

    if (fn_8012D694() != 0) {
        return fn_80124A98();
    }
    temp_r3 = fn_80124AAC();
    if (temp_r3 != 0) {
        return temp_r3;
    }
    (*(s8*)((u8*)(lbl_80281988) + 0xC)) = 0;
    return 0;
}

s32 fn_801252D0(void) {
    s32 temp_r31;
    s32 temp_r3;

    temp_r31 = fn_8009D74C(0, 0);
    fn_8009DBAC(0, 0);
    if (temp_r31 == -0x19) {
        return -0x19;
    }
    if (fn_8012D5E4((*(s32*)((u8*)(lbl_80281988) + 8)), (*(s32*)((u8*)(lbl_80281988) + 4))) != 0) {
        return fn_80124A98();
    }
    temp_r3 = fn_80124AAC();
    return temp_r3 & ((s32) (-temp_r3 | temp_r3) >> 0x1F);
}

s32 fn_80125354(void) {
    s32 temp_r31;
    s32 temp_r3;

    temp_r31 = fn_8009D74C(0, 0);
    fn_8009DBAC(0, 0);
    if (temp_r31 == -0x19) {
        return -0x19;
    }
    if (fn_8012D560() != 0) {
        return fn_80124A98();
    }
    temp_r3 = fn_80124AAC();
    if (temp_r3 != 0) {
        return temp_r3;
    }
    if ((u8) (*(u8*)((u8*)(lbl_80281988) + 0xC)) == 0) {
        fn_8012D9B4(lbl_80281988 + 0xE);
        (*(u8*)((u8*)(lbl_80281988) + 0xC)) = 1U;
    }
    return 0;
}

s32 fn_801253F0(s32 arg0, s32 arg1) {
    s32 temp_r3;

    if (fn_8012D5B0(arg0, arg1) != 0) {
        return fn_80124A98();
    }
    temp_r3 = fn_80124AAC();
    return temp_r3 & ((s32) (-temp_r3 | temp_r3) >> 0x1F);
}

s32 fn_80125434(s32 arg0, s32 arg1) {
    s32 temp_r31;
    s32 temp_r3;

    temp_r3 = fn_80125354();
    if (temp_r3 != 0) {
        return temp_r3;
    }
    if (fn_8012D6C8() != 0) {
        return fn_80124A98();
    }
    temp_r31 = fn_80124AAC();
    fn_801253F0(arg0, arg1);
    return temp_r31 & ((s32) (-temp_r31 | temp_r31) >> 0x1F);
}

s32 fn_801254B8(void) {
    if (fn_8012D7F0() != 0) {
        return fn_80124A98();
    }
    return 0;
}

s32 fn_801254EC(void) {
    if (fn_8012D710() != 0) {
        return fn_80124A98();
    }
    return 0;
}

s32 fn_80125520(void) {
    return 0;
}

void fn_801255C4(u8* p0) {
    fn_8009D1D8(*(s32*)(p0 + 0x0), *(s32*)(p0 + 0x4), 0, 3);
}

s32 fn_801255F8(void) {
    return 27;
}

void fn_80125648(s32 p0, s32 p1, s32 p2) {
    lbl_8028257C = 1;
    lbl_80282574 = p0;
    lbl_8028256C = p1;
    lbl_80282568 = ((u32)((-p2) | p2) >> 31);
}

void fn_8012566C(s32 p0) {
    lbl_8028256A = ((u32)((-p0) | p0) >> 31);
}

void fn_80125680(s32 p0) {
    lbl_80282569 = ((u32)((-p0) | p0) >> 31);
}

u8 fn_80125694(void) {
    return *(u8*)(lbl_80281988 + 0xC);
}

void fn_801256A0(s32 v) {
    *(s32*)(lbl_80281988 + 0x14) = v;
}

s32 fn_801256AC(void) {
    return *(s32*)(lbl_80281988 + 0x14);
}

u8 fn_801257A0(void) {
    s32 temp_r31;
    s32 temp_r3;
    s32 var_r3;

    temp_r3 = fn_801258E8();
    temp_r31 = fn_80106ED8(0xC, temp_r3);
    var_r3 = fn_80107084(0xC, temp_r3);
    if (var_r3 == -1) {
        var_r3 = 0x4E3;
    }
    fn_8012DAB8((u16) var_r3);
    return temp_r31 > 0;
}

void fn_80125814(void) {
    fn_8012DD7C();
}

void fn_80125834(void) {
    fn_8012D8C4();
}

void fn_80125854(void) {
    fn_8012D93C();
}

void fn_80125910(u8 v) {
    *(u8*)(lbl_80281988 + 0xD) = v;
}

u8 fn_8012591C(void) {
    return *(u8*)(lbl_80281988 + 0xD);
}

// ---- end of sweep code ----
