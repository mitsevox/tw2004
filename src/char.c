// char.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the golfer's character
// object (character.h). So far: choosing the clip a character plays (Char_SetClip), its bones and
// matrices, and small setters; the sweep code in the marked block is the other matched small
// functions, not yet cleaned up.

#include "game.h"
#include "charstate.h"
#include "unsorted/cull.h"
#include "game_types.h"
#include "endian.h"

void  fn_80014BB4(void);
void  fn_80014C9C(void);
void  fn_80014DC0(void);
void  fn_8001A024(Character* pChar);
void  fn_8001A288(void);
void  fn_8001A33C(void);
void  fn_8001A3B0(Character* pChar);
void  fn_8001A4BC(void);
void  fn_8001A58C(int nPlayer);
void  fn_8001A75C(UStreamObject* pObject);
void  fn_8001A798(void);
void  fn_8001A7C8(void);
Character* fn_8001A9F4(u8* pData, int a, int nPlayer, u32 uId, u8 b, void* p);
void* fn_8001B208(u8* pData);
void  fn_8001B878(Character* pChar, int n);
void  fn_8001C0E0(Character* pChar);
Character* fn_8001C21C(Character* pChar);
void  Character_UpdateAnimation(Character* pChar, int a, f32 f);
void  Character_UpdateTestPoints(Character* pChar);
void  Character_UpdateFeetTerrainInfo(Character* pChar, int a);
void  Character_PlaceFeetOnGround(Character* pChar);
void  SKEL_TransformBones(CharModel* pModel, u32* auBits);
void  fn_800B28D4(Character* pChar, int a, int b);
void  fn_800B2FB0(Character* pChar, int a, int b);
void  fn_800BAD60(f32 mtx[4][4], Vec4* src, Vec4* dst);    // VecMath.c: a point through a matrix
void  fn_8001EB8C(Character* pChar, int nBone, f32* pPos);
void  fn_8001CCF8(UStreamObject* pObject);
void  fn_8001CD80(UStreamObject* pObject);
void  fn_8001CE5C(UStreamObject* pObject);
void  fn_8001D020(UStreamObject* pObject);
void  fn_8001D3EC(UStreamObject* pObject);
void  fn_8001D7EC(void);
void  fn_8001C5B4(Character* pChar, int n);
void  fn_800BBADC(int nValue);         // SitDevFile.c
void  fn_8001EBD8(Character* pChar, int nBone, f32* pPos);
u8    fn_8001EC48(Character* pChar);
f32 (*fn_8001EC6C(Character* pChar, int nBone))[4];
f32 (*fn_8001ECA8(Character* pChar, int nBone))[4];
f32   fn_8001EFFC(CamLens* pLens);
CamLens* fn_8001F004(void);
void  fn_80027738(u8 bOn);
void  fn_80035C58(void);
void  fn_80035CC0(void);
void  fn_80036460(int n);
void  fn_80036464(void);
void  fn_80095554(void);
void  fn_8009555C(void);
void  fn_80095560(void);
void  fn_80095564(void);
void  fn_800955F0(int nPlayer);
void  fn_8008E918(s32 v);
u8    fn_8008E938(void);
void  fn_8008EAC8(u8 v);
void  fn_800C937C(void);
void  fn_800C9764(void);
void  fn_800C9FE0(void);
void  fn_800CCA1C(void);
void  fn_800CCA3C(void);
void  fn_800CEE04(Skin* pSkin, int a, int b);
void  fn_800CEE88(u8 b);
u8    fn_800FCC38(int nPlayer);
void  fn_8010A668(void* p);
void  fn_8010BF68(void);
void  fn_8010BFE0(void);
void  fn_80112C64(int n);
void  fn_80112CEC(void);

// ---- sweep code (not yet cleaned up) ----
void fn_8001E8A4(u32* aBits, u32 nBits);
void fn_8001E938(u32* aBits, u32 nBits);
void fn_80017864(void* arg0, u32 (*arg1)[4]);
void fn_8001B1DC(s32 p0, u8* p1, s32 p2);
void fn_8001B1E8(void* p);
void fn_8001C650(void* arg0, s32 arg1);

void fn_80017864(void* arg0, u32 (*arg1)[4]) {
    if ((u32) (*(u32*)((u8*)(arg0) + 0x3C)) != 0U) {
        fn_8001E8A4(arg1[2], 0x80);
        fn_8001E8A4(arg1[3], 0x80);
        fn_8001E938(arg1[0], 0x80);
        fn_8001E938(arg1[1], 0x80);
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

// Moves the character to pPos (its root bone's position); with bPlace, the bones are transformed
// again and the feet put back on the ground.
void Character_SetPosition(Character* pChar, f32* pPos, u8 bPlace) {
    u32 auBits[4];

    fn_8001E8A4(auBits, 0x80);
    if (pChar != NULL) {
        Vec_Copy(pPos, pChar->pModel->pBones->v1C);
        if (bPlace) {
            SKEL_TransformBones(pChar->pModel, auBits);
            Character_UpdateTestPoints(pChar);
            pChar->n1784 = -1;
            Character_UpdateFeetTerrainInfo(pChar, 1);
            Character_PlaceFeetOnGround(pChar);
        }
    }
}

// Turns the character's root bone to fAngle about y (half a turn more in game type 3 while the
// model's bEE is set).
void fn_800192D4(Character* pChar, f32 fAngle) {
    if (pChar != NULL) {
        if (gSession.nGameType == 3 && fn_8001EDF4(pChar)) {
            fAngle += PI;
        }
        fn_80008BB8(pChar->pModel->pBones->q0C, 0.0f, fAngle, 0.0f);
    }
}

void fn_80019648(void) {
    fn_80095554();
    fn_8001A4BC();
}

// Give back the character's pool entries and free what it holds.
void fn_8001971C(Character* pChar) {
    fn_8001A3B0(pChar);
    if (pChar->pA8 != NULL) {
        fn_80009E70(pChar->pA8);
    }
    if (pChar->pB0 != NULL) {
        fn_80009E70(pChar->pB0);
    }
    if (pChar->pB8 != NULL) {
        fn_80009E70(pChar->pB8);
    }
    if (pChar->pBC != NULL) {
        fn_80009E70(pChar->pBC);
    }
    if (pChar->hFile >= 0) {
        fn_8000633C(pChar->hFile);
    }
}

void fn_80019C1C(Character* pChar) {
    int i;
    for (i = 0; i < pChar->nSkins; i++) {
        fn_800CEE04(pChar->apSkins[i], 3, 2);
    }
}

void fn_80019C84(Character* pChar) {
    int i;
    for (i = 0; i < pChar->nSkins; i++) {
        fn_800CEE04(pChar->apSkins[i], 2, 1);
    }
}

void fn_80019CEC(Character* pChar) {
    int i;
    for (i = 0; i < pChar->nSkins; i++) {
        fn_800CEE04(pChar->apSkins[i], 1, 0);
        pChar->apSkins[i]->u10D4 |= 1;
    }
}

void fn_8001A0FC(Character* pChar) {
    fn_80019C84(pChar);
    fn_8008E918(2);
    fn_8008EAC8(1);
    if (fn_8008E938() == 0) {
        fn_8001A024(pChar);
    }
}

// Reset every pool entry and mark it free.
void fn_8001A33C(void) {
    int i;
    for (i = 0; i < lbl_801B95E8.nEntries; i++) {
        fn_8010A668(lbl_801B95E8.a[i].p);
        lbl_801B95E8.a[i].bUsed = 0;
    }
}

// Give the character's pool entries back.
void fn_8001A3B0(Character* pChar) {
    int i;
    for (i = 0; i < pChar->n70; i++) {
        if (pChar->a64[i] != NULL) {
            lbl_801B95E8.a[pChar->a6C[i]].bUsed = 0;
            pChar->a6C[i] = -1;
            pChar->a64[i] = NULL;
        }
    }
    pChar->bE0 = 0;
}

// Take n70 free pool entries for the character.
void fn_8001A418(Character* pChar) {
    int i;
    int n = 0;
    for (i = 0; i < lbl_801B95E8.nEntries; i++) {
        if (lbl_801B95E8.a[i].bUsed == 0) {
            pChar->a6C[n] = i;
            pChar->a64[n] = lbl_801B95E8.a[i].p;
            n++;
            lbl_801B95E8.a[i].bUsed = 1;
            if (n == pChar->n70) {
                return;
            }
        }
    }
}

// Empty; fn_8001A4BC calls it with a player's character.
void fn_8001A484(Character* pChar) {
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

// Advances every character's animation by fTime, except in game type 6 while fn_800E415C holds.
void fn_8001BC8C(f32 fTime) {
    int i;

    if (gSession.nGameType != 6 || !fn_800E415C()) {
        for (i = 0; i < lbl_80281CA8; i++) {
            Character_UpdateAnimation(lbl_801B9624[i], 0, fTime);
        }
    }
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

// Set the club class, and put the club head bone at the class's height.
void fn_8001C5B4(Character* pChar, int n) {
    int nBone;

    if (pChar == NULL || pChar->pModel == NULL || pChar->nSlot < 0 || pChar->nSlot >= 3) {
        return;
    }
    nBone = fn_8001EED8(pChar->pModel, 0x53);
    if (pChar->p16D8 != NULL) {
        pChar->nClubClass = n;
        pChar->pModel->pBones[nBone].v1C[1] = pChar->p16D8->afC[pChar->nClubClass];
    }
}

// The player's golfer takes the player's shot kind and club; when either changed, it goes back
// to animation 5.
void fn_8001C680(int nPlayer) {
    Player* pPlayer = &gPlayers[nPlayer];
    Character* pChar = pPlayer->pChar;
    int nKind = pChar->nShotKind;
    int nClub;

    fn_8001C724(pChar, pPlayer->nShotKind);
    nClub = pChar->nClub;
    fn_8001C774(pChar, pPlayer->nClub);
    if (nClub != pPlayer->nClub || nKind != pPlayer->nShotKind) {
        pChar->nAnim = 0;
        pChar->u10 |= 0x80;
        fn_80095744(pChar, 5);
        fn_8001C804(nPlayer, 1, 1);
    }
}

// Set the character's shot kind and the clip key that goes with it.
void fn_8001C724(Character* pChar, int nKind) {
    if (pChar != NULL) {
        fn_8001C650(pChar, lbl_80187164[nKind]);
        pChar->nShotKind = nKind;
    }
}

void fn_8001C774(Character* pChar, int nClub) {
    // per club: what fn_8001C5B4 gets
    int aKind[26] = {0, 0, 0, 0, 0, 0, 1, 1, 1, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 2};

    if (pChar != NULL) {
        fn_800BBADC(nClub);
        pChar->nClub = nClub;
        fn_8001C5B4(pChar, aKind[nClub]);
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

// Runs fn_800B28D4 and fn_800B2FB0 on each character found by id (nPlayer 1000) whose n1658 is not
// 2 and that has neither bit 0x01 nor 0x40 of u10 set.
void fn_8001D290(void) {
    int i;

    for (i = 0; i < lbl_80281CA8; i++) {
        if (lbl_801B9624[i]->nPlayer == 1000 && lbl_801B9624[i]->n1658 != 2 &&
            !(lbl_801B9624[i]->u10 & 0x41)) {
            fn_800B28D4(lbl_801B9624[i], 1, 0);
            fn_800B2FB0(lbl_801B9624[i], 1, 0);
        }
    }
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

// Run fn_8001B878 on every character with no player (the 'SKLO' ones).
void fn_8001D384(void) {
    int i;
    for (i = 0; i < lbl_80281CA8; i++) {
        if (lbl_801B9624[i]->nPlayer == 1000) {
            fn_8001B878(lbl_801B9624[i], 1000);
        }
    }
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

void fn_8001D8DC(int nPlayer) {
    gPlayers[nPlayer].pChar->u10 &= ~0x40;
    fn_800955F0(nPlayer);
    fn_8001A58C(nPlayer);
    if (!gSession.nSplitScreen && lbl_80281CAC != nPlayer) {
        fn_8001D6D8(nPlayer);
        lbl_80281CAC = nPlayer;
    }
}

// Empty the character's four data buffers (their memory is kept).
// The clip's point v80 through bone 0's matrix (fn_8001ED08) into pOut; without a clip, bone 0's
// position (fn_8001EB8C).
void fn_8001DB04(Character* pChar, f32* pOut) {
    Vec4 vPos;
    f32 (*pMtx)[4];

    if (pChar->pCurClip != NULL) {
        fn_8001EED8(pChar->pModel, 1);  // the result is not used
        pMtx = fn_8001ED08(pChar, 0);
        Vec3Copy(pChar->pCurClip->v80, &vPos.x);
        vPos.w = 1.0f;
        fn_800BAD60(pMtx, &vPos, (Vec4*)pOut);
        return;
    }
    fn_8001EB8C(pChar, 0, pOut);
}

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

// Every player for whom fn_800FCC38 says so has its animation played at normal speed.
void fn_8001E7DC(void) {
    int i;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        if (fn_800FCC38(i)) {
            Anim_SetRate(gPlayers[i].pChar->anim, 1.0f);
        }
    }
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

// Sets every bit of a bit array of nBits bits.
void fn_8001E8A4(u32* aBits, u32 nBits) {
    u32 i;

    for (i = 0; i < (nBits + 31) >> 5; i++) {
        aBits[i] = 0xFFFFFFFF;
    }
}

// Clears every bit of a bit array of nBits bits.
void fn_8001E938(u32* aBits, u32 nBits) {
    u32 i;

    for (i = 0; i < (nBits + 31) >> 5; i++) {
        aBits[i] = 0;
    }
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
        return pChar->f1660 * (0.5f / fn_8001EFFC(fn_8001F004()));
    }
    return pChar->f1660 * (1.0f / fn_8001EFFC(fn_8001F004()));
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
        return pChar->f165C * (0.5f / fn_8001EFFC(fn_8001F004()));
    }
    return pChar->f165C * (1.0f / fn_8001EFFC(fn_8001F004()));
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

// a - b into out (three floats)
#ifdef __MWERKS__
asm void fn_8001EF10(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_8001EF10(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// in scaled by f into out (three floats)
#ifdef __MWERKS__
asm void fn_8001EF34(register f32* pIn, register f32 f, register f32* pOut) {
    nofralloc
    fmr      f2, f
    psq_l    f0, 0(pIn), 0, 0
    psq_l    f1, 8(pIn), 1, 0
    ps_muls0 f0, f0, f2
    ps_muls0 f1, f1, f2
    psq_st   f0, 0(pOut), 0, 0
    psq_st   f1, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_8001EF34(f32* pIn, f32 f, f32* pOut) {
    pOut[0] = pIn[0] * f;
    pOut[1] = pIn[1] * f;
    pOut[2] = pIn[2] * f;
}
#endif

// b + a into out (three floats)
#ifdef __MWERKS__
asm void fn_8001EF54(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_add f2, f2, f0
    ps_add f3, f3, f1
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_8001EF54(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
}
#endif

// a x b into out; out's fourth float is set to 0.
#ifdef __MWERKS__
asm void vec4flt_CrossProduct(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l      f0, 0(pA), 0, 0
    psq_l      f1, 4(pA), 0, 0
    psq_l      f3, 0(pB), 0, 0
    psq_l      f5, 4(pB), 0, 0
    ps_merge10 f2, f1, f0
    ps_merge10 f4, f3, f3
    ps_merge10 f3, f5, f3
    ps_mul     f4, f0, f4
    ps_mul     f0, f2, f5
    ps_merge11 f2, f4, f4
    ps_msub    f0, f1, f3, f0
    ps_sub     f4, f4, f2
    psq_st     f0, 0(pOut), 0, 0
    psq_st     f4, 8(pOut), 0, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void vec4flt_CrossProduct(f32* pA, f32* pB, f32* pOut) {
    f32 fX = pA[1] * pB[2] - pA[2] * pB[1];
    f32 fY = pA[2] * pB[0] - pA[0] * pB[2];
    f32 fZ = pA[0] * pB[1] - pA[1] * pB[0];

    pOut[0] = fX;
    pOut[1] = fY;
    pOut[2] = fZ;
    pOut[3] = 0.0f;
}
#endif

// a - b into out (four floats)
#ifdef __MWERKS__
asm void fn_8001EFB4(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 0, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 0, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 0, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_8001EFB4(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
    pOut[3] = pA[3] - pB[3];
}
#endif

// b + a into out (four floats)
#ifdef __MWERKS__
asm void fn_8001EFD8(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 0, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 0, 0
    ps_add f2, f2, f0
    ps_add f3, f3, f1
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 0, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_8001EFD8(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
    pOut[3] = pB[3] + pA[3];
}
#endif

f32 fn_8001EFFC(CamLens* pLens) {
    return pLens->fB0;
}

// The current render camera's lens.
CamLens* fn_8001F004(void) {
    return fn_80008370(*lbl_80280DF0);
}

// ---- sweep code (not yet cleaned up) ----

void Anim_SetRate(u8* p, f32 v);

void Anim_SetRate(u8* p, f32 v) {
    *(f32*)(p + 0x14) = v;
}

// ---- end of sweep code ----

// Byte-swaps nCount records laid out as pFormat's nFields fields from *ppSrc to *ppDst; both
// pointers are left after the last record.
void fn_8001F08C(void** ppSrc, void** ppDst, SwapField* pFormat, int nFields, int nCount) {
    SwapField* pField;
    int i;

    if (nCount > 0) {
        do {
            pField = pFormat;
            for (i = 0; i < nFields; i++) {
                // port: *ppSrc is read and advanced as a u8* (fn_80076158's parameter)
                fn_80076158((u8**)ppSrc, *ppDst, pField->nBytes, pField->nSize);
                *ppDst = (u8*)*ppDst + pField->nBytes;
                pField++;
            }
        } while (--nCount > 0);
    }
}
