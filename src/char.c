// char.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the golfer's character
// object (character.h). So far: choosing the clip a character plays (Char_SetClip), its bones and
// matrices, and small setters; the sweep code in the marked block is the other matched small
// functions, not yet cleaned up.

#include "golfer.h"

void  fn_8001D7EC(void);
u8    fn_8001EC48(void);
f32 (*fn_8001EC6C(Character* pChar, int nBone))[4];
f32 (*fn_8001ECA8(Character* pChar, int nBone))[4];

// ---- sweep code (not yet cleaned up) ----
s32 fn_8001E8A4(s32, s32);
s32 fn_8001E938(s32, s32);
void fn_80017864(void* arg0, s32 arg1);
void fn_80019C84(void);
void fn_8001A4BC();
void fn_80095554();
void fn_80019648(void);
s32 fn_8008E918(s32);
u8 fn_8008E938();
s32 fn_8008EAC8(s32);
s32 fn_8001A024(s32);
void fn_8001A0FC(s32 arg0);
void fn_8001A484(void);
void fn_8010BF68();
void fn_8001A73C(void);
void AnimLib_MergeOverlay();
void fn_8001A75C(u8* p0);
void fn_8001A7C8(void);
void fn_80014BB4();
void fn_80014DC0();
void fn_8001A798();
void fn_8001A7F0(void);
void fn_8001B1DC(s32 p0, u8* p1, s32 p2);
void fn_8001B1E8(void* p);
void fn_8001A33C();
void fn_80035CC0();
void fn_80095560();
void fn_800C9764();
void fn_800CCA3C();
void fn_8001C2B4(void);
void fn_80095564();
void fn_8001C2E4(void);
extern s32 lbl_80280E20;
void fn_8001A288();
void fn_80027738();
void fn_80036460();
void fn_80036464();
void fn_800CCA1C();
void fn_800CEE88();
void fn_80112C64();
void fn_80112CEC();
void fn_8001C304(void);
void fn_8001C350(void);
void fn_8001C650(void* arg0, s32 arg1);
void fn_8001CE34(void);
void fn_8001D268(void);
void fn_8001A9F4();
s32 fn_8001C21C();
void fn_8001D3EC(u8* p0);
void fn_8001D47C(void);
void fn_8001DB98(u8* p0);
void fn_8001E85C(u8* p0, u8* p1);
f32 fn_8001EFFC(u8* p);
void* fn_8001F004(void);
extern f32 lbl_80282BC0;
extern f32 lbl_80282BF8;
s32 fn_8001EE88(u8* p);
void fn_8001EE98(u8* p0, s32 p1);
f32 fn_8001EEA4(u8* p0, u8* p1);
extern s32 lbl_80280DF0;
void* fn_80008370();

void fn_80017864(void* arg0, s32 arg1) {
    if ((u32) (*(u32*)((u8*)(arg0) + 0x3C)) != 0U) {
        fn_8001E8A4(arg1 + 0x20, 0x80);
        fn_8001E8A4(arg1 + 0x30, 0x80);
        fn_8001E938(arg1, 0x80);
        fn_8001E938(arg1 + 0x10, 0x80);
    }
}

void fn_80019648(void) {
    fn_80095554();
    fn_8001A4BC();
}

void fn_8001A0FC(s32 arg0) {
    fn_80019C84();
    fn_8008E918(2);
    fn_8008EAC8(1);
    if (fn_8008E938() == 0) {
        fn_8001A024(arg0);
    }
}

void fn_8001A484(void) {
}

void fn_8001A73C(void) {
    fn_8010BF68();
}

// port: the 'SAC ' handler; the overlay library is little-endian on disc and AnimLib_MergeOverlay
//       swaps it (fn_80020BC8 > fn_80076158): a little-endian port does not swap there.
void fn_8001A75C(u8* p0) {
    AnimLib_MergeOverlay(*(s32*)p0, *(s32*)(p0 + 0x20));
    fn_80009E70(p0);
}

void fn_8001A7C8(void) {
    UStream_UnregisterHandler(1396785952);
}

void fn_8001A7F0(void) {
    fn_8001A798();
    fn_80014BB4();
    fn_80014DC0();
    fn_8001A7C8();
}

void fn_8001B1DC(s32 p0, u8* p1, s32 p2) {
    *(s32*)p1 = p2;
    *(s32*)(p1 + 0x4) = p0;
}

void fn_8001B1E8(void* p) {
    fn_80009E70(p);
}

void fn_8001C2B4(void) {
    fn_80035CC0();
    fn_800CCA3C();
    fn_800C9764();
    fn_80095560();
    fn_8001A33C();
}

void fn_8001C2E4(void) {
    fn_80095564();
}

void fn_8001C304(void) {
    fn_8001A288();
    fn_80027738(0);
    lbl_80280E20 = 3;
    fn_800CCA1C();
    fn_800CEE88(0);
    fn_80036460(1800);
    fn_80112C64(1);
}

void fn_8001C350(void) {
    fn_8001A33C();
    fn_800CCA3C();
    fn_80036464();
    fn_80112CEC();
}

void fn_8001C650(void* arg0, s32 arg1) {
    void* temp_r5;

    temp_r5 = (*(void**)((u8*)(arg0) + 0x1798));
    if ((temp_r5 != NULL) && ((u32) (*(u32*)((u8*)(temp_r5) + 0x2C)) == 6U) && (arg1 == 0)) {
        (*(s32*)((u8*)(arg0) + 0x16D4)) = 4;
    }
    (*(s32*)((u8*)(arg0) + 0x16D4)) = arg1;
}

void fn_8001CE34(void) {
    UStream_UnregisterHandler(1129071136);
}

void fn_8001D268(void) {
    UStream_UnregisterHandler(1128813088);
}

// port: the 'SKLO' handler; the skeleton is little-endian on disc and fn_8001A9F4 swaps it
//       (fn_80076158): a little-endian port does not swap there.
void fn_8001D3EC(u8* p0) {
    s32 t1;
    fn_8001A9F4(*(s32*)p0, 0, 0, *(s32*)(p0 + 0x20), 0, 0);
    t1 = fn_8001C21C();
    *(s32*)(((u8*)t1) + 0x4) = 1000;
    *(s32*)(((u8*)t1) + 0x8) = *(s32*)(p0 + 0x20);
    fn_80009E70(p0);
}

void fn_8001D47C(void) {
    UStream_UnregisterHandler(1397443663);
}

void fn_8001DB98(u8* p0) {
    *(s32*)(p0 + 0x43C) = -1;
    *(s32*)(p0 + 0x440) = 0;
    *(s32*)(p0 + 0x448) = 0;
    *(s32*)(p0 + 0x44C) = 0;
    *(s32*)(p0 + 0x450) = 0;
    *(s32*)(p0 + 0x458) = -1;
    *(s32*)(p0 + 0x45C) = 0;
    *(s32*)(p0 + 0x464) = 0;
    *(s32*)(p0 + 0x468) = 0;
    *(s32*)(p0 + 0x46C) = 0;
    *(s32*)(p0 + 0x474) = -1;
    *(s32*)(p0 + 0x478) = 0;
    *(s32*)(p0 + 0x480) = 0;
    *(s32*)(p0 + 0x484) = 0;
    *(s32*)(p0 + 0x488) = 0;
    *(s32*)(p0 + 0x490) = -1;
    *(s32*)(p0 + 0x494) = 0;
    *(s32*)(p0 + 0x49C) = 0;
    *(s32*)(p0 + 0x4A0) = 0;
    *(s32*)(p0 + 0x4A4) = 0;
}

void fn_8001E85C(u8* p0, u8* p1) {
    *(f32*)(p1 + 0xC) = *(f32*)(p0 + 0xC);
    *(f32*)p1 = *(f32*)p0;
    *(f32*)(p1 + 0x4) = *(f32*)(p0 + 0x4);
    *(f32*)(p1 + 0x8) = *(f32*)(p0 + 0x8);
}

s32 fn_8001EE88(u8* p) {
    return *(s32*)(p + 0x1658);
}

void fn_8001EE98(u8* p0, s32 p1) {
    *(u8*)(((u8*)*(s32*)(p0 + 0x38)) + 0xEE) = p1;
}

f32 fn_8001EEA4(u8* p0, u8* p1) {
    return (*(f32*)(p0 + 0xC) * *(f32*)(p1 + 0xC) + (*(f32*)(p0 + 0x8) * *(f32*)(p1 + 0x8) + (*(f32*)p0 * *(f32*)p1 + (*(f32*)(p0 + 0x4) * *(f32*)(p1 + 0x4)))));
}

f32 fn_8001EFFC(u8* p) {
    return *(f32*)(p + 0xB0);
}

void* fn_8001F004(void) {
    return fn_80008370(*(s32*)((u8*)lbl_80280DF0));
}

// ---- end of sweep code ----

// Pick the character's clip for an animation group and style from its animation library, keyed
// also by the character's club class (class 1 looks up as 0) and n16D4. The lookup's fallback flags
// go to bits 0x200 / 0x400 of uFlags; the clip is kept in pCurClip.
void* Char_SetClip(Character* pChar, int nGroup, int nStyle, const char* pName) {
    u32   uFlags = 0;
    int   nClub  = pChar->nClubClass;
    void* pClip;
    if (nClub == 1) {
        nClub = 0;
    }
    pClip = AnimLib_Pick(pChar->nPlayer, pChar->pLib, nGroup, nStyle, nClub, pChar->n16D4, &uFlags, pName);
    if (uFlags & 1) {
        pChar->uFlags |= 0x200;
    } else {
        pChar->uFlags &= ~0x200;
    }
    if (uFlags & 2) {
        pChar->uFlags |= 0x400;
    } else {
        pChar->uFlags &= ~0x400;
    }
    pChar->pCurClip = pClip;
    return pClip;
}

// Give the character its model and look up the bones the swing needs: the club head (0x53), the
// grip (0x52) and bone 0x15.
void fn_80018484(Character* pChar, CharModel* pModel) {
    if (pChar != NULL) {
        pChar->pModel        = pModel;
        pChar->nClubHeadBone = fn_8001EED8(pChar->pModel, 0x53);
        pChar->nGripBone     = fn_8001EED8(pChar->pModel, 0x52);
        pChar->n16A8         = fn_8001EEE4(pChar->pModel, 0x15);
    }
}

void fn_8001C7FC(Character* pChar, int nStyle) {
    pChar->nStyle = nStyle;
}

void fn_8001D624(int n) {
    gSession.aD2D[n] = 1;
}

void fn_8001D6D8(int n) {
    gSession.aD28[n] = 1;
}

void fn_8001D7A4(Character* pChar) {
    fn_8001D7EC();
    pChar->u10 = pChar->u10 & ~0x20C;
    pChar->u10 = pChar->u10 | 0x40;
}

// The ball is in the golfer's hand: the current clip has more than bone 0x54's index of something
// (its n1C), and the model has that bone.
u8 fn_8001DBF4(Character* pChar) {
    if (pChar->pCurClip != NULL && fn_8001EED8(pChar->pModel, 0x54) != 0xFF &&
        pChar->pCurClip->n1C > fn_8001EED8(pChar->pModel, 0x54)) {
        return 1;
    }
    return 0;
}

f32 (*fn_8001EC6C(Character* pChar, int nBone))[4] {
    return fn_8001ECA8(pChar, fn_8001EEE4(pChar->pModel, nBone));
}

// Bone n's matrix (bone 1's while fn_8001EC48 says no); NULL without a character.
f32 (*fn_8001ECA8(Character* pChar, int nBone))[4] {
    int n = nBone;
    if (pChar == NULL) {
        return NULL;
    }
    if (fn_8001EC48() == 0) {
        n = 1;
    }
    return pChar->pModel->pMatrices[n];
}

// A bone's matrix, by bone id.
f32 (*fn_8001ED08(Character* pChar, int nBone))[4] {
    return fn_8001ECA8(pChar, fn_8001EED8(pChar->pModel, nBone));
}

f32 fn_8001ED44(Character* pChar, int b) {
    if (b != 0) {
        return pChar->f1660 * (lbl_80282BF8 / fn_8001EFFC(fn_8001F004()));
    }
    return pChar->f1660 * (lbl_80282BC0 / fn_8001EFFC(fn_8001F004()));
}

u8 fn_8001EDF4(Character* pChar) {
    return pChar->pModel->bEE;
}

f32 fn_8001EE00(Character* pChar, int b) {
    if (b != 0) {
        return pChar->f165C * (lbl_80282BF8 / fn_8001EFFC(fn_8001F004()));
    }
    return pChar->f165C * (lbl_80282BC0 / fn_8001EFFC(fn_8001F004()));
}

void fn_8001EE64(Character* pChar) {
    fn_8001ED08(pChar, 1);
}

int fn_8001EE90(Character* pChar) {
    return pChar->n1654;
}
