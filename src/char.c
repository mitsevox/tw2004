// char.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the golfer's character
// object (character.h). So far: choosing the clip a character plays (Char_SetClip), its bones and
// matrices, and small setters; the sweep code in the marked block is the other matched small
// functions, not yet cleaned up.

#include "golfer.h"
#include "charstate.h"

void  fn_80014BB4(void);
void  fn_80014C9C(void);
void  fn_80014DC0(void);
void  fn_8001A288(void);
void  fn_8001A33C(void);
void  fn_8001A4BC(void);
void  fn_8001A75C(UStreamObject* pObject);
void  fn_8001A798(void);
void  fn_8001A7C8(void);
Character* fn_8001A9F4(u8* pData, int a, int nPlayer, u32 uId, u8 b, void* p);
void* fn_8001B208(u8* pData);
void  fn_8001C0E0(Character* pChar);
Character* fn_8001C21C(Character* pChar);
void  fn_8001CCF8(UStreamObject* pObject);
void  fn_8001CD80(UStreamObject* pObject);
void  fn_8001CE5C(UStreamObject* pObject);
void  fn_8001D020(UStreamObject* pObject);
void  fn_8001D3EC(UStreamObject* pObject);
void  fn_8001D7EC(void);
void  fn_8001EBD8(Character* pChar, int nBone, f32* pPos);
u8    fn_8001EC48(Character* pChar);
f32 (*fn_8001EC6C(Character* pChar, int nBone))[4];
f32 (*fn_8001ECA8(Character* pChar, int nBone))[4];
void  fn_80027738(u8 bOn);
void  fn_80035C58(void);
void  fn_80035CC0(void);
void  fn_80036460(int n);
void  fn_80036464(void);
void  fn_80095554(void);
void  fn_8009555C(void);
void  fn_80095560(void);
void  fn_80095564(void);
void  fn_800C937C(void);
void  fn_800C9764(void);
void  fn_800C9FE0(void);
void  fn_800CCA1C(void);
void  fn_800CCA3C(void);
void  fn_800CEE88(u8 b);
void  fn_8010BF68(void);
void  fn_8010BFE0(void);
void  fn_80112C64(int n);
void  fn_80112CEC(void);

// ---- sweep code (not yet cleaned up) ----
s32 fn_8001E8A4(s32, s32);
s32 fn_8001E938(s32, s32);
void fn_80017864(void* arg0, s32 arg1);
void fn_80019C84(void);
s32 fn_8008E918(s32);
u8 fn_8008E938();
s32 fn_8008EAC8(s32);
s32 fn_8001A024(s32);
void fn_8001A0FC(s32 arg0);
void fn_8001B1DC(s32 p0, u8* p1, s32 p2);
void fn_8001B1E8(void* p);
void fn_8001C650(void* arg0, s32 arg1);
f32 fn_8001EFFC(u8* p);
void* fn_8001F004(void);
extern f32 lbl_80282BC0;
extern f32 lbl_80282BF8;
void* fn_80008370();

void fn_80017864(void* arg0, s32 arg1) {
    if ((u32) (*(u32*)((u8*)(arg0) + 0x3C)) != 0U) {
        fn_8001E8A4(arg1 + 0x20, 0x80);
        fn_8001E8A4(arg1 + 0x30, 0x80);
        fn_8001E938(arg1, 0x80);
        fn_8001E938(arg1 + 0x10, 0x80);
    }
}

void fn_8001A0FC(s32 arg0) {
    fn_80019C84();
    fn_8008E918(2);
    fn_8008EAC8(1);
    if (fn_8008E938() == 0) {
        fn_8001A024(arg0);
    }
}

void fn_8001B1DC(s32 p0, u8* p1, s32 p2) {
    *(s32*)p1 = p2;
    *(s32*)(p1 + 0x4) = p0;
}

void fn_8001B1E8(void* p) {
    fn_80009E70(p);
}

void fn_8001C650(void* arg0, s32 arg1) {
    void* temp_r5;

    temp_r5 = (*(void**)((u8*)(arg0) + 0x1798));
    if ((temp_r5 != NULL) && ((u32) (*(u32*)((u8*)(temp_r5) + 0x2C)) == 6U) && (arg1 == 0)) {
        (*(s32*)((u8*)(arg0) + 0x16D4)) = 4;
    }
    (*(s32*)((u8*)(arg0) + 0x16D4)) = arg1;
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

void fn_80019648(void) {
    fn_80095554();
    fn_8001A4BC();
}

void fn_8001A484(void) {
}

void fn_8001A488(void) {
    if (gSession.nNumPlayers > 2) {
        fn_8010BFE0();
    }
}

void fn_8001A73C(void) {
    fn_8010BF68();
}

// The 'SAC ' handler: an animation library merged over the one of the slot the object's id names.
// port: the overlay library is little-endian on disc and AnimLib_MergeOverlay swaps it
//       (fn_80020BC8 > fn_80076158): a little-endian port does not swap there.
void fn_8001A75C(UStreamObject* pObject) {
    AnimLib_MergeOverlay(pObject->pData, pObject->uId);
    fn_80009E70(pObject);
}

void fn_8001A798(void) {
    UStream_RegisterHandler('SAC ', fn_8001A75C);
}

void fn_8001A7C8(void) {
    UStream_UnregisterHandler('SAC ');
}

// Handle the 'SAC ' overlays while fn_80014BB4 and fn_80014DC0 run.
void fn_8001A7F0(void) {
    fn_8001A798();
    fn_80014BB4();
    fn_80014DC0();
    fn_8001A7C8();
}

// With more than one player: reload the animation slot with 'SAC ' overlays handled, then free the
// work copies.
void fn_8001A81C(void) {
    if (gSession.nNumPlayers > 1) {
        lbl_80281CE4 = 1;
        AnimLib_ReloadSlot();
        fn_8001A798();
        fn_80014C9C();
        fn_80014DC0();
        fn_8001A7C8();
        AnimLib_FreeWorkCopies();
    }
    fn_800C9FE0();
}

// Add a character to the table of characters (up to five); NULL when it is full.
Character* fn_8001C21C(Character* pChar) {
    if (lbl_80281CA8 >= 5) {
        return NULL;
    }
    lbl_801B9624[lbl_80281CA8] = pChar;
    pChar->nIndex = lbl_80281CA8;
    lbl_80281CA8++;
    return pChar;
}

void fn_8001C254(void) {
    int n;
    fn_8009555C();
    fn_8001A288();
    fn_80027738(1);
    n = 6;
    if (gSession.nSplitScreen) {
        n = 4;
    }
    lbl_80280E20 = n;
    fn_800C937C();
    fn_800CCA1C();
    fn_800CEE88(1);
    fn_80035C58();
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

void fn_8001C518(void) {
    fn_8001C0E0(lbl_80281EE8);
    lbl_80281EE8 = NULL;
}

// The model id of the player's golfer.
u8 fn_8001C558(int nPlayer) {
    return gGolferTable[gSession.nGolfer[nPlayer]].nModelID;
}

// The player's golfer is one of records 30 to 33.
u8 fn_8001C584(int nPlayer) {
    u8 b = 0;
    if (gSession.nGolfer[nPlayer] >= 30 && gSession.nGolfer[nPlayer] <= 33) {
        b = 1;
    }
    return b;
}

// Set the character's shot kind and the clip key that goes with it.
void fn_8001C724(Character* pChar, int nKind) {
    if (pChar != NULL) {
        fn_8001C650(pChar, lbl_80187164[nKind]);
        pChar->nShotKind = nKind;
    }
}

void fn_8001C7FC(Character* pChar, int nStyle) {
    pChar->nStyle = nStyle;
}

// Flags on the player's character: bit 4 always, bit 8 set or cleared by b, bit 0x200 set by a.
void fn_8001C804(int nPlayer, u8 a, u8 b) {
    Character* pChar = gPlayers[nPlayer].pChar;
    pChar->u10 |= 4;
    if (b) {
        pChar->u10 |= 8;
    } else {
        pChar->u10 &= ~8;
    }
    if (a) {
        pChar->u10 |= 0x200;
    }
}

// The 'CLB ' handlers: what fn_8001B208 makes of the object is kept unless there already is one;
// the first handler makes a second one for split screen.
void fn_8001CCF8(UStreamObject* pObject) {
    if (lbl_80280E24[0] == NULL) {
        if (gSession.nSplitScreen) {
            lbl_80280E24[0] = fn_8001B208(pObject->pData);
            lbl_80280E24[1] = fn_8001B208(pObject->pData);
        } else {
            lbl_80280E24[0] = fn_8001B208(pObject->pData);
            lbl_80280E24[1] = NULL;
        }
    }
    fn_80009E70(pObject);
}

void fn_8001CD80(UStreamObject* pObject) {
    if (lbl_80280E24[0] == NULL) {
        lbl_80280E24[0] = fn_8001B208(pObject->pData);
        lbl_80280E24[1] = NULL;
    }
    fn_80009E70(pObject);
}

// The 'CLB ' stream objects: two handlers for the same type.
void fn_8001CDD4(void) {
    UStream_RegisterHandler('CLB ', fn_8001CCF8);
}

void fn_8001CE04(void) {
    UStream_RegisterHandler('CLB ', fn_8001CD80);
}

void fn_8001CE34(void) {
    UStream_UnregisterHandler('CLB ');
}

// The 'CHR ' stream objects: two handlers for the same type.
void fn_8001CFF0(void) {
    UStream_RegisterHandler('CHR ', fn_8001CE5C);
}

void fn_8001D238(void) {
    UStream_RegisterHandler('CHR ', fn_8001D020);
}

void fn_8001D268(void) {
    UStream_UnregisterHandler('CHR ');
}

// The character built from the 'SKLO' object with this id (fn_8001D3EC), or NULL.
Character* fn_8001D324(int nId) {
    int i;
    for (i = 0; i < lbl_80281CA8; i++) {
        if (lbl_801B9624[i]->nPlayer == 1000 && lbl_801B9624[i]->uId == nId) {
            return lbl_801B9624[i];
        }
    }
    return NULL;
}

// The 'SKLO' handler: a character built from the object with no player (1000), keyed by the
// object's id.
// port: the skeleton is little-endian on disc and fn_8001A9F4 swaps it (fn_80076158): a
//       little-endian port does not swap there.
void fn_8001D3EC(UStreamObject* pObject) {
    Character* pChar = fn_8001C21C(fn_8001A9F4(pObject->pData, 0, 0, pObject->uId, 0, NULL));
    pChar->nPlayer = 1000;
    pChar->uId     = pObject->uId;
    fn_80009E70(pObject);
}

void fn_8001D44C(void) {
    UStream_RegisterHandler('SKLO', fn_8001D3EC);
}

void fn_8001D47C(void) {
    UStream_UnregisterHandler('SKLO');
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

// Empty the character's four data buffers (their memory is kept).
void fn_8001DB98(Character* pChar) {
    int i;
    for (i = 0; i < 4; i++) {
        pChar->buffers[i].n00 = -1;
        pChar->buffers[i].p04 = NULL;
        pChar->buffers[i].p0C = NULL;
        pChar->buffers[i].p10 = NULL;
        pChar->buffers[i].p14 = NULL;
    }
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

// Copy a quaternion (Skeleton.c's use).
void fn_8001E85C(f32* pSrc, f32* pDst) {
    pDst[3] = pSrc[3];
    pDst[0] = pSrc[0];
    pDst[1] = pSrc[1];
    pDst[2] = pSrc[2];
}

void fn_8001E880(f32* pSrc, f32* pDst) {
    pDst[0] = pSrc[0];
    pDst[1] = pSrc[1];
    pDst[2] = pSrc[2];
    pDst[3] = 1.0f;
}

u8 fn_8001E9CC(u32* aBits, u32 n) {
    return (aBits[n >> 5] & (1 << (n & 31))) != 0;
}

u8 fn_8001E9F4(u32* aA, u32* aB, u32 nBits) {
    u32 i;
    for (i = 0; i < (nBits + 31) >> 5; i++) {
        if (aA[i] & aB[i]) {
            return 1;
        }
    }
    return 0;
}

void fn_8001EA34(u32* aBits, u32 n) {
    aBits[n >> 5] |= 1 << (n & 31);
}

void fn_8001EB6C(u32* aBits, u32 n) {
    aBits[n >> 5] &= ~(1 << (n & 31));
}

// A bone's position, by bone id.
void fn_8001EB8C(Character* pChar, int nBone, f32* pPos) {
    fn_8001EBD8(pChar, fn_8001EED8(pChar->pModel, nBone), pPos);
}

// Bone n's position (bone 1's without an animation slot); nothing without a character.
void fn_8001EBD8(Character* pChar, int nBone, f32* pPos) {
    if (pChar != NULL) {
        if (fn_8001EC48(pChar) == 0) {
            nBone = 1;
        }
        Vec_Copy(pChar->pModel->pMatrices[nBone][3], pPos);
    }
}

// The character plays from animation slot 0 or 1.
u8 fn_8001EC48(Character* pChar) {
    if (pChar->nSlot >= 0 && pChar->nSlot < 2) {
        return 1;
    }
    return 0;
}

f32 (*fn_8001EC6C(Character* pChar, int nBone))[4] {
    return fn_8001ECA8(pChar, fn_8001EEE4(pChar->pModel, nBone));
}

// Bone n's matrix (bone 1's without an animation slot); NULL without a character.
f32 (*fn_8001ECA8(Character* pChar, int nBone))[4] {
    int n = nBone;
    if (pChar == NULL) {
        return NULL;
    }
    if (fn_8001EC48(pChar) == 0) {
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

// A bone's position, by bone id through fn_8001EEE4.
void fn_8001EDA8(Character* pChar, int nBone, f32* pPos) {
    fn_8001EBD8(pChar, fn_8001EEE4(pChar->pModel, nBone), pPos);
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

int fn_8001EE88(Character* pChar) {
    return pChar->n1658;
}

int fn_8001EE90(Character* pChar) {
    return pChar->n1654;
}

void fn_8001EE98(Character* pChar, u8 b) {
    pChar->pModel->bEE = b;
}

// The dot product of two 4-vectors.
f32 fn_8001EEA4(f32* pA, f32* pB) {
    return pA[0] * pB[0] + pA[1] * pB[1] + pA[2] * pB[2] + pA[3] * pB[3];
}

int fn_8001EED8(CharModel* pModel, int nBone) {
    return pModel->aBone[nBone];
}

// A bone's index, through the second table while the model's bEE is set.
int fn_8001EEE4(CharModel* pModel, int nBone) {
    if (pModel->bEE) {
        return pModel->aBone2[pModel->aBone[nBone]];
    }
    return pModel->aBone[nBone];
}
