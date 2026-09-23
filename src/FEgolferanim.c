// FEgolferanim.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): not yet
// decompiled; the sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern s32 lbl_80281EE0;
void fn_80009E70();
void fn_8008B820(void);
void fn_8008DBE8();
void fn_8008DC10();
void fn_8008B00C(void);
void fn_8008B204(void);
void fn_8008B208(void);
void fn_8008B3C8(void);
void fn_80014DFC();
void fn_8008EBB4();
void fn_8008B3CC(void);
void fn_8008EBE4();
void fn_8008B430(void);
void UStream_Stop();
void fn_8008B4A0(void);
void fn_8008B570(void);
void fn_8010B9BC();
void fn_8008B5FC(void);
void fn_8010BEC4();
void fn_8008B674(void);
void fn_8008B6E4(void);
extern u8 lbl_80189AA0[];
extern u8 lbl_801D8708[];
void fn_8008B704();
void fn_8008B754();
void fn_8008B7D0();
void fn_8008B760(void);
u8* fn_8008B850(void);
s32 fn_8008B990(void);
void fn_8008C938(void);
s32 fn_8008C93C();
s32 fn_8008CC30();
void fn_8008CE2C(void);
s32 fn_8008CE88(s32);
s32 fn_800B9CF0(s32);
extern s32 lbl_80281330;
void fn_8008C844(void);
s32 fn_8008CA88();
void fn_8008C8C4(void);
void GXCopyTex();
void GXInvalidateTexAll();
void GXPixModeSync();
void GXSetTexCopyDst();
void GXSetTexCopySrc();
s32 fn_8002A624();
void fn_8001D238();
void fn_8008AD80();
void fn_8008D8F4();
void fn_8008D8CC(void);
s32 Char_SetClip(s32, s32, s32, s32);
s32 fn_8008E02C(void);
void fn_8008E244(void);
void fn_8008E354(void);
void fn_8008E358(s32 p0);
void fn_8008E724(u32 arg0, u32 arg1, s8 arg2, u8 arg3);
u8 fn_8008E944(u8 arg0, f32 farg0);
s32 strncpy();
extern f32 lbl_80283B40;
void fn_8008E364(s32 arg0);
s32 fn_8008E420(void);
s32 fn_8008E44C(void);
s32 fn_8001C5B4(s32, s32);
void fn_8008E6D4(s32 p0);
void fn_8008E718(s32 v);
extern f32 lbl_80283B3C;
void fn_8008E818(s32 v);
s32 fn_8008DD50(s32);
void fn_8008E824(void);
u8 fn_80103B80();
void fn_8008E860(s32 arg0);
void fn_8008E8D0(s32 arg0);
void fn_8008E918(s32 v);
s32 fn_8008E924(void);
u8 fn_8008E938(void);
void fn_8008E960(u32 arg0);
s32 fn_8008E9A8(void);
s32 fn_8001BE88(s32, u32, s32, void*, f32);
void fn_8008E9B4(void);
void fn_8008EA38(u8 v);
u8 fn_8008EAB0(void);
void fn_8008EABC(u8 v);
void fn_8008EAC8(u8 v);
u8 fn_8008EAD4(void);
void fn_8008EAE0(s32 v);
s32 fn_8008EAEC(void);
void fn_8008EAF8(s32 v);
s32 fn_8008EB04(void);
void fn_8008EB70(void);

void fn_8008B00C(void) {
    fn_8008B820();
    fn_8008DBE8();
    fn_8008DC10();
    fn_80009E70(lbl_80281EE0);
    lbl_80281EE0 = 0;
}

void fn_8008B204(void) {
}

void fn_8008B208(void) {
}

void fn_8008B3C8(void) {
}

void fn_8008B3CC(void) {
    fn_80014DFC(*(s32*)(((u8*)*(s32*)(((u8*)lbl_80281EE0) + 0xB8)) + 0xC), *(s32*)(((u8*)*(s32*)(((u8*)lbl_80281EE0) + 0xB8)) + 0x10));
    fn_8008EBB4();
    *(s32*)(((u8*)*(s32*)(((u8*)lbl_80281EE0) + 0xB8)) + 0x14) = *(s32*)(((u8*)*(s32*)(((u8*)lbl_80281EE0) + 0xB8)) + 0xC);
    *(s32*)(((u8*)lbl_80281EE0) + 0x8C) = *(s32*)(((u8*)*(s32*)(((u8*)lbl_80281EE0) + 0xB8)) + 0xC);
    *(s32*)(((u8*)lbl_80281EE0) + 0x190) = (*(s32*)(((u8*)lbl_80281EE0) + 0x190) + 1);
}

void fn_8008B430(void) {
    fn_8008EBE4();
}

void fn_8008B4A0(void) {
    UStream_Stop();
}

void fn_8008B570(void) {
}

void fn_8008B5FC(void) {
    fn_8010B9BC();
}

void fn_8008B674(void) {
    fn_8010BEC4();
}

void fn_8008B6E4(void) {
    fn_8010B9BC();
}

void fn_8008B760(void) {
    fn_8008B754(1);
    fn_8008B704();
    fn_8008B7D0(1);
}

void fn_8008B820(void) {
    fn_8008B754(1);
    fn_8008B704();
    fn_8008B7D0(1);
}

u8* fn_8008B850(void) {
    *(u8*)(lbl_801D8708 + 0x8) = 1;
    return lbl_801D8708;
}

s32 fn_8008B990(void) {
    return *(s32*)(lbl_801D8708 + 0x4);
}

void fn_8008C844(void) {
    if (((u8) (*(u8*)((u8*)((*(void**)((u8*)(lbl_80281EE0) + 0xB4))) + 0x18)) != 0) && ((u8) (*(u8*)((u8*)(lbl_80281EE0) + 0x86)) == 0) && ((u8) (*(u8*)((u8*)(lbl_80281EE0) + 0x88)) == 0) && ((s32) lbl_80281330 != 0)) {
        (*(s8*)((u8*)(lbl_80281EE0) + 0x18C)) = 1;
        fn_8008C938();
        fn_8008C93C();
        fn_8008CE88(0);
        fn_800B9CF0(0);
        fn_8008CC30();
        fn_8008CE2C();
        fn_8008C93C();
    }
}

void fn_8008C8C4(void) {
    if (((u8) (*(u8*)((u8*)((*(void**)((u8*)(lbl_80281EE0) + 0xB4))) + 0x18)) != 0) && ((u8) (*(u8*)((u8*)(lbl_80281EE0) + 0x86)) == 0) && ((u8) (*(u8*)((u8*)(lbl_80281EE0) + 0x88)) == 0)) {
        if ((s32) lbl_80281330 != 0) {
            fn_8008CA88();
            return;
        }
        (*(s8*)((u8*)(lbl_80281EE0) + 0x18C)) = 0;
        fn_8008CE88(0);
        fn_800B9CF0(0);
    }
}

void fn_8008C938(void) {
}

void fn_8008CE2C(void) {
    s32 t3;
    GXPixModeSync();
    GXSetTexCopySrc(128, 0, 384, 448);
    GXSetTexCopyDst(384, 448, 6, 0);
    t3 = fn_8002A624();
    GXCopyTex(t3, 0);
    GXPixModeSync();
    GXInvalidateTexAll();
}

void fn_8008D8CC(void) {
    fn_8001D238();
    fn_8008AD80();
    fn_8008D8F4();
}

s32 fn_8008E02C(void) {
    s32 temp_r0;

    temp_r0 = (*(s32*)((u8*)(lbl_80281EE0) + 4));
    if (temp_r0 == 1) {
        return Char_SetClip((*(s32*)((u8*)((*(void**)((u8*)(lbl_80281EE0) + 0xB4))) + 8)), 0xB, 0, 0);
    }
    if (temp_r0 == 2) {
        return Char_SetClip((*(s32*)((u8*)((*(void**)((u8*)(lbl_80281EE0) + 0xB4))) + 8)), 7, 0, 0);
    }
    return Char_SetClip((*(s32*)((u8*)((*(void**)((u8*)(lbl_80281EE0) + 0xB4))) + 8)), 1, 0, 0);
}

void fn_8008E244(void) {
    *(u8*)(((u8*)lbl_80281EE0) + 0x1DC) = 0;
}

void fn_8008E354(void) {
}

void fn_8008E358(s32 p0) {
    lbl_80281330 = (p0 & 0xFF);
}

void fn_8008E364(s32 arg0) {
    s32 temp_r0;
    s32 temp_r3;

    if (arg0 != (s32) (*(s32*)((u8*)(lbl_80281EE0) + 4))) {
        temp_r3 = fn_8008E02C();
        if (temp_r3 != 0) {
            temp_r0 = (*(s32*)((u8*)(lbl_80281EE0) + 0x1C0));
            if ((temp_r0 != 1) && (temp_r0 != 0)) {
                fn_8008E724(temp_r3 + 0xA0, 0, 0, 0);
                fn_8008E944(0, lbl_80283B40);
                (*(s32*)((u8*)(lbl_80281EE0) + 0x1C4)) = 2;
            } else {
                strncpy(lbl_80281EE0 + 0x20, temp_r3 + 0xA0, 0x10);
                (*(s32*)((u8*)(lbl_80281EE0) + 0x1C4)) = 2;
            }
        }
        (*(s8*)((u8*)(lbl_80281EE0) + 0x30)) = 0;
    }
    (*(s32*)((u8*)(lbl_80281EE0) + 4)) = arg0;
}

s32 fn_8008E420(void) {
    void* temp_r3;

    temp_r3 = (*(void**)((u8*)(lbl_80281EE0) + 0xB4));
    if ((temp_r3 != NULL) && ((u32) (*(u32*)((u8*)(temp_r3) + 8)) != 0U)) {
        return 1;
    }
    return 0;
}

s32 fn_8008E44C(void) {
    return ((u32)((-*(u8*)(((u8*)*(s32*)(((u8*)lbl_80281EE0) + 0xB4)) + 0x18)) | *(u8*)(((u8*)*(s32*)(((u8*)lbl_80281EE0) + 0xB4)) + 0x18)) >> 31);
}

void fn_8008E6D4(s32 p0) {
    *(s32*)(((u8*)lbl_80281EE0) + 0x1B8) = p0;
    *(s32*)(((u8*)lbl_80281EE0) + 0x1BC) = -1;
    fn_8001C5B4(*(s32*)(((u8*)*(s32*)(((u8*)lbl_80281EE0) + 0xB4)) + 0x8), *(s32*)(((u8*)lbl_80281EE0) + 0x1B8));
}

void fn_8008E718(s32 v) {
    *(s32*)(lbl_80281EE0 + 0x1BC) = v;
}

void fn_8008E724(u32 arg0, u32 arg1, s8 arg2, u8 arg3) {
    if (arg0 == 0U) {
        (*(s8*)((u8*)(lbl_80281EE0) + 0x20)) = 0;
        (*(s32*)((u8*)(lbl_80281EE0) + 0x1C0)) = 4;
    } else {
        strncpy(lbl_80281EE0 + 0x20, arg0, 0x10, lbl_80281EE0);
        (*(f32*)((u8*)(lbl_80281EE0) + 0x1CC)) = (f32) lbl_80283B3C;
        if (arg3 != 0) {
            (*(s32*)((u8*)(lbl_80281EE0) + 0x1C0)) = 0;
        } else {
            (*(s32*)((u8*)(lbl_80281EE0) + 0x1C0)) = 1;
        }
        (*(s8*)((u8*)(lbl_80281EE0) + 0x1C8)) = 1;
        (*(s32*)((u8*)(lbl_80281EE0) + 0x1C4)) = 3;
    }
    if (arg1 == 0U) {
        (*(s8*)((u8*)(lbl_80281EE0) + 0x30)) = 0;
    } else {
        strncpy(lbl_80281EE0 + 0x30, arg1, 0x20);
    }
    (*(s8*)((u8*)(lbl_80281EE0) + 0x1D0)) = arg2;
}

void fn_8008E818(s32 v) {
    *(s32*)(lbl_80281EE0 + 0x50) = v;
}

void fn_8008E824(void) {
    fn_8008DD50(0);
    *(s32*)(((u8*)lbl_80281EE0) + 0x50) = 0;
    *(s32*)(((u8*)lbl_80281EE0) + 0x1C0) = 4;
}

void fn_8008E860(s32 arg0) {
    s32 temp_r4;

    if (fn_80103B80() != 0) {
        temp_r4 = (*(s32*)((u8*)(lbl_80281EE0) + 8));
        (*(s32*)((u8*)(lbl_80281EE0) + 8)) = arg0;
        (*(s32*)((u8*)(lbl_80281EE0) + 0xC)) = 0;
        (*(s8*)((u8*)(lbl_80281EE0) + 0x80)) = 0;
        if (temp_r4 != (s32) (*(s32*)((u8*)(lbl_80281EE0) + 8))) {
            fn_8008E244();
            fn_8008DD50(0);
        }
    }
}

void fn_8008E8D0(s32 arg0) {
    if (fn_80103B80() != 0) {
        (*(s32*)((u8*)(lbl_80281EE0) + 0xC)) = arg0;
        (*(s8*)((u8*)(lbl_80281EE0) + 0x80)) = 1;
    }
}

void fn_8008E918(s32 v) {
    *(s32*)(lbl_80281EE0 + 0x74) = v;
}

s32 fn_8008E924(void) {
    return (((u32)__cntlzw(*(s32*)(((u8*)lbl_80281EE0) + 0x74)) >> 5) & 0xFF);
}

u8 fn_8008E938(void) {
    return *(u8*)(lbl_80281EE0 + 0x78);
}

u8 fn_8008E944(u8 arg0, f32 farg0) {
    u8 temp_r0;

    temp_r0 = (*(u8*)((u8*)(lbl_80281EE0) + 0x78));
    (*(u8*)((u8*)(lbl_80281EE0) + 0x78)) = arg0;
    (*(f32*)((u8*)(lbl_80281EE0) + 0x7C)) = farg0;
    return temp_r0;
}

void fn_8008E960(u32 arg0) {
    if (arg0 == 0U) {
        (*(s8*)((u8*)(lbl_80281EE0) + 0x54)) = 0;
        return;
    }
    strncpy(lbl_80281EE0 + 0x54, arg0, 0x20, lbl_80281EE0);
}

s32 fn_8008E9A8(void) {
    return *(s32*)(lbl_80281EE0 + 0x8);
}

void fn_8008E9B4(void) {
    u32 temp_r3;

    fn_8001C5B4((*(s32*)((u8*)((*(void**)((u8*)(lbl_80281EE0) + 0xB4))) + 8)), 5);
    temp_r3 = Char_SetClip((*(s32*)((u8*)((*(void**)((u8*)(lbl_80281EE0) + 0xB4))) + 8)), 8, 0, 0);
    if (temp_r3 != 0U) {
        fn_8001BE88((*(s32*)((u8*)((*(void**)((u8*)(lbl_80281EE0) + 0xB4))) + 8)), temp_r3, 0, (void*)lbl_80281EE0, lbl_80283B3C);
    }
    fn_8001C5B4((*(s32*)((u8*)((*(void**)((u8*)(lbl_80281EE0) + 0xB4))) + 8)), (*(s32*)((u8*)(lbl_80281EE0) + 0x1B8)));
}

void fn_8008EA38(u8 v) {
    *(u8*)(lbl_80281EE0 + 0x81) = v;
}

u8 fn_8008EAB0(void) {
    return *(u8*)(lbl_80281EE0 + 0x1D1);
}

void fn_8008EABC(u8 v) {
    *(u8*)(lbl_80281EE0 + 0x1D1) = v;
}

void fn_8008EAC8(u8 v) {
    *(u8*)(lbl_80281EE0 + 0x1D2) = v;
}

u8 fn_8008EAD4(void) {
    return *(u8*)(lbl_80281EE0 + 0x1D2);
}

void fn_8008EAE0(s32 v) {
    *(s32*)(lbl_80281EE0 + 0x1D4) = v;
}

s32 fn_8008EAEC(void) {
    return *(s32*)(lbl_80281EE0 + 0x1D4);
}

void fn_8008EAF8(s32 v) {
    *(s32*)(lbl_80281EE0 + 0x1D8) = v;
}

s32 fn_8008EB04(void) {
    return *(s32*)(lbl_80281EE0 + 0x1D8);
}

void fn_8008EB70(void) {
    fn_8008E724(0, 0, 0, 0);
    fn_8008DD50(0);
    fn_8008E944(0, lbl_80283B40);
}

// ---- end of sweep code ----
