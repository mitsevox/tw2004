// EASportsBio.c (EA's name, from its asserts): not yet decompiled; the sweep code below is the
// matched small functions.

#include "engine.h"
#include "core/easb.h"

// ---- sweep code (not yet cleaned up) ----

void fn_800A27BC(const char* szSrc, u16* szDst, s32 nMax);
void fn_801250C0(void);
s32 fn_80125354(s32 arg0, s32 arg1);
s32 fn_801253F0(s32 arg0, s32 arg1);
s32 fn_80125194(s32 arg0, s32 arg1);
s32 fn_80124A98();
s32 fn_80125280(void);
s32 fn_8009D74C(s32, s32);
s32 fn_8009DBAC(s32, s32);
s32 fn_80124AAC();
s32 fn_801252D0(void);
s32 fn_80125434(s32 arg0, s32 arg1);
s32 fn_801254B8(void);
s32 fn_801254EC(void);
s32 fn_80125520(u8 b);
void fn_8009D1D8();
void fn_801255C4(u8* p0);
s32 fn_801255F8(void);
void fn_80125648(s32 p0, s32 p1, s32 p2);
void fn_8012566C(s32 p0);
void fn_80125680(s32 p0);
u8 fn_80125694(void);
void fn_801256A0(s32 v);
s32 fn_801256AC(void);
s32 fn_80106ED8(s32, s32);
s32 fn_80107084(s32, s32);
s32 fn_801258E8(void);
u8 fn_801257A0(void);
void fn_80125814(u8 bFlag);
void fn_80125834(u32 uCount);
u8 fn_8012591C(void);

// The Bio's level progress as a percentage.
f32 fn_80124BDC(void) {
    u16 uLevel;
    f32 fProgress;

    fn_8012DF4C(&uLevel, &fProgress);
    return 100.0f * fProgress;
}

void fn_801250C0(void) {
    if (lbl_80281988->pCurrentGameImage != NULL) {
        fn_80009E70(lbl_80281988->pCurrentGameImage);
    }
    if (lbl_80281988->pOurGameImage != NULL) {
        fn_80009E70(lbl_80281988->pOurGameImage);
    }
    if (lbl_80281988->pIcon != NULL) {
        fn_80009E70(lbl_80281988->pIcon);
    }
}

s32 fn_80125194(s32 arg0, s32 arg1) {
    s32 temp_r3;

    temp_r3 = fn_80125354(arg0, arg1);
    if (temp_r3 == 0) {
        fn_801253F0(arg0, arg1);
    }
    return temp_r3;
}

s32 fn_80125280(void) {
    s32 temp_r3;

    if (fn_8012D694() != EASB_ERROR_NONE) {
        return fn_80124A98();
    }
    temp_r3 = fn_80124AAC();
    if (temp_r3 != 0) {
        return temp_r3;
    }
    lbl_80281988->bBioLoaded = 0;
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
    if (fn_8012D5E4(lbl_80281988->pIcon, lbl_80281988->pOurGameImage) != EASB_ERROR_NONE) {
        return fn_80124A98();
    }
    temp_r3 = fn_80124AAC();
    return temp_r3 & ((s32) (-temp_r3 | temp_r3) >> 0x1F);
}

// arg0 and arg1 are unused; its callers hand on their own two arguments.
s32 fn_80125354(s32 arg0, s32 arg1) {
    s32 temp_r31;
    s32 temp_r3;

    temp_r31 = fn_8009D74C(0, 0);
    fn_8009DBAC(0, 0);
    if (temp_r31 == -0x19) {
        return -0x19;
    }
    if (fn_8012D560() != EASB_ERROR_NONE) {
        return fn_80124A98();
    }
    temp_r3 = fn_80124AAC();
    if (temp_r3 != 0) {
        return temp_r3;
    }
    if (!lbl_80281988->bBioLoaded) {
        fn_8012D9B4(&lbl_80281988->uCurLevel);
        lbl_80281988->bBioLoaded = 1;
    }
    return 0;
}

s32 fn_801253F0(s32 arg0, s32 arg1) {
    s32 temp_r3;

    if (fn_8012D5B0() != EASB_ERROR_NONE) {
        return fn_80124A98();
    }
    temp_r3 = fn_80124AAC();
    return temp_r3 & ((s32) (-temp_r3 | temp_r3) >> 0x1F);
}

s32 fn_80125434(s32 arg0, s32 arg1) {
    s32 temp_r31;
    s32 temp_r3;

    temp_r3 = fn_80125354(arg0, arg1);
    if (temp_r3 != 0) {
        return temp_r3;
    }
    if (fn_8012D6C8() != EASB_ERROR_NONE) {
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
    if (fn_8012D710() != EASB_ERROR_NONE) {
        return fn_80124A98();
    }
    return 0;
}

s32 fn_80125520(u8 b) {
    return 0;
}

// TRUE when the Bio's file status (fn_8012D744) is 1.
u8 fn_80125528(void) {
    u32 uStatus;

    // EA bug: the error is ignored, so uStatus is read unset when the library is not running
    fn_8012D744(&uStatus);
    return uStatus == 1;
}

s32 fn_8012555C(s32* pArgs) {
    s32 nError;
    s32 a0;
    s32 a1;

    a1 = pArgs[1];
    a0 = pArgs[0];
    nError = fn_80125354(a0, a1);
    if (nError == -18) return 1;
    if (nError == 0) {
        fn_801253F0(a0, a1);
    }
    return 0;
}

void fn_801255C4(u8* p0) {
    fn_8009D1D8(*(s32*)(p0 + 0x0), *(s32*)(p0 + 0x4), 0, 3);
}

s32 fn_801255F8(void) {
    return 27;
}

void fn_80125600(s32 a0, s32 a1) {
    lbl_80282578 = a0;
    lbl_8028257D = 1;
    lbl_80282570 = a1;
    fn_8012E818(a1, lbl_80281988->pCurrentGameImage);
    fn_80125520(lbl_80282570);
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
    return lbl_80281988->bBioLoaded;
}

void fn_801256A0(s32 v) {
    lbl_80281988->eCurrentRewardMessage = v;
}

s32 fn_801256AC(void) {
    return lbl_80281988->eCurrentRewardMessage;
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

void fn_80125814(u8 bFlag) {
    fn_8012DD7C(bFlag);
}

void fn_80125834(u32 uCount) {
    fn_8012D8C4(uCount);
}

void fn_80125854(int nCount) {
    fn_8012D93C(nCount);
}

// Records an accomplishment by name; the next reward check reports it.
void fn_80125874(const char* szName, s32 arg1) {
    u16 szWide[0x40];

    if (fn_80125694()) {
        fn_800A27BC(szName, szWide, 0x3F);
        if (fn_8012DD24(szWide, arg1, 'en') == EASB_ERROR_NONE) {
            lbl_80281988->bNewAccomplishment = 1;
        }
    }
}

s32 fn_801258E8(void) {
    u16 uLevel;

    fn_8012D9B4(&uLevel);
    return uLevel;
}

void fn_80125910(u8 b) {
    lbl_80281988->bGameWon = b;
}

u8 fn_8012591C(void) {
    return lbl_80281988->bGameWon;
}

// The Bio's level, or 0 when no Bio is loaded.
u16 fn_80125928(void) {
    u16 uLevel = 0;
    f32 fProgress = 0.0f;

    if (lbl_80281988->bBioLoaded) {
        fn_8012DF4C(&uLevel, &fProgress);
        return uLevel;
    }
    return 0;
}

// ---- end of sweep code ----
