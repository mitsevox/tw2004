// char.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the character object
// (character.h): the golfers and the other skinned characters ('SKLO', player 1000). Building one
// from its 'CHR ' object, its animation, bones, ground placement and leg IK, the set-up for a
// shot, its streamed textures and clothes, and the stream handlers that make them. Not all of it
// matches yet; the code in the two marked sweep blocks is matched but not yet cleaned up.

#include "game.h"
#include "charstate.h"
#include "lldyntex.h"
#include "frontend/fe.h"
#include "unsorted/cull.h"
#include "game_types.h"
#include "endian.h"

// data order: these lead the unity's .data (0x80186CC8), ahead of char_tex_manager.c's
// "_usrtextr", so they are defined before its #include.
IKLinkDef lbl_80186CC8[7] = {
    {1, 0.0f, -1, 0.0f, 0.0f},
    {3, 1.0f, -1, 0.0f, 0.0f},
    {7, 0.5f, -1, 0.0f, 0.0f},
    {17, 1.0f, -1, 0.0f, 0.0f},
    {21, 0.25f, -1, 0.0f, 0.0f},
    {82, 0.0f, -1, 0.0f, 0.0f},
    {83, 0.0f, -1, 0.0f, 0.0f},
};
IKLinkDef lbl_80186D54[10] = {
    {1, 0.0f, -1, 0.0f, 0.0f},
    {3, 1.0f, -1, 0.0f, 0.0f},
    {4, 0.5f, -1, 0.0f, 0.0f},
    {7, 0.1f, -1, 0.0f, 0.0f},
    {16, 0.25f, -1, 0.0f, 0.0f},
    {17, 0.5f, -1, 0.0f, 0.0f},
    {20, 0.5f, -1, 0.0f, 0.0f},
    {21, 0.25f, -1, 0.0f, 0.0f},
    {82, 0.0f, -1, 0.0f, 0.0f},
    {83, 0.0f, -1, 0.0f, 0.0f},
};
IKLinkDef lbl_80186E1C[5] = {
    {7, 0.0f, -1, 0.0f, 0.0f},
    {35, 0.25f, -1, 0.0f, 0.0f},
    {36, 0.5f, -1, 0.0f, 0.0f},
    {39, 0.0f, -1, 0.0f, 0.0f},
    {40, 0.0f, -1, 0.0f, 0.0f},
};
IKChainDef lbl_80186E80[2] = {
    {lbl_80186D54, 10, 20, 0.01f},
    {lbl_80186E1C, 5, 20, 0.01f},
};
IKChainDef lbl_80186EA0[2] = {
    {lbl_80186CC8, 7, 10, 0.01f},
    {lbl_80186E1C, 5, 20, 0.01f},
};
char lbl_80186EC0[6][13] = {"Drivers", "Fairwaywoods", "Putters", "3Irons", "7Irons", "Wedges"};
char lbl_80186F10[6][13] = {"fwd_shaft", "fwd_shaft", "pwi_shaft", "pwi_shaft", "pwi_shaft", "pwi_shaft"};
char lbl_80186F60[6][13] = {"Defaults", "Defaults", "Defaults", "Defaults", "Defaults", "Defaults"};
char lbl_80186FB0[6][13] = {"EA_Driver", "EA_Fairway", "EA_Putter", "EA_3Iron", "EA_7Iron", "EA_Wedge"};
char lbl_80187000[6][13] = {"Defaults", "Defaults", "Defaults", "Defaults", "Defaults", "Defaults"};
char lbl_80187050[6][13] = {"fwd_grip", "fwd_grip", "pwi_grip", "pwi_grip", "pwi_grip", "pwi_grip"};
char lbl_801870A0[6][13] = {"Defaults", "Defaults", "Defaults", "Defaults", "Defaults", "Defaults"};

// char_tex_manager.c and char.c were one translation unit (a unity build: TW07 compiles both in
// golf2_unity.cpp): the .data of both is one 8-aligned block from 0x801870F0 ("_usrtextr", then
// char.c's first string at 0x801870FC, which a separate char.o could not start at).
#include "../src/char_tex_manager.c"

void  fn_80014BB4(void);
void  fn_80014C9C(void);
void  fn_80014DC0(void);
void  Character_ExecuteTextureSwapFE(Character* pChar);
void  CharacterTex_Init(void);
void  CharacterTex_Close(void);
void  CharacterTex_ReleasePoolEntries(Character* pChar);
void  CharacterTex_PreHoleInit(void);
void  CharacterTex_StartStreamingPlayers(int nPlayer);
void  Character_LoadSacFromStream(UStreamObject* pObject);
void  Character_RegisterSacStreamClient(void);
void  Character_UnregisterSacStreamClient(void);
Character* Character_CreateFromMem(u8* pData, int nUnused, int nSet, int nId, u8 bLook,
                                   SkinChoices* pChoices);
CharSkinSet* Character_CreateClubSkinSet(u8* pData);
Character* Character_Create(void);
s32   fn_800962F8(Character* pChar);                            // CharAnim.c
void  Character_SetSkin(Character* pChar, Skin* pSkin);
void  Character_SetPreferedPos(Character* pChar);
void  ClipBank_Restore(int nSlot);                  // skalib.c
ClipBank* ClipBank_Get(u32 nSlot);                  // skalib.c
AnimLib* AnimLib_Load(u8* pData, ClipBank* pBank);  // skalib.c
CharModel* SKEL_LoadFromMem(u8* pData, s8 n, CharModelDefs* pDefs, int b);   // Skeleton.c
void  fn_80037AB8(Skin* pSkin, CharModel* pModel, int nBone, int nId);   // Skin.c
void  SkinPart_BurnBodySkin(Character* pChar);                // SkinPart.c
void* CharSlider_CreateDefinitionsFromMem(u8** ppData);
void  Character_FreeClubSkinSets(CharSkinSet* pSet);
void  Character_ClipTest(Character* pChar, int nPlayer);
f32 (*fn_8001EE64(Character* pChar))[4];                        // bone 1's matrix
Character* Character_Add(Character* pChar);
void  Character_UpdateAnimation(Character* pChar, int a, f32 f);
void  Character_UpdateTestPoints(Character* pChar);
void  Character_UpdateFeetTerrainInfo(Character* pChar, int bNormals);
f32   Character_GetTerrainHeightAndNormal(Character* pChar, f32* pPos, f32** ppNormal);
void  Character_PlaceFeetOnGround(Character* pChar);
void  SKEL_TransformBones(CharModel* pModel, u32* auBits);
void  fn_800B28D4(Character* pChar, int a, int b);
void  fn_800B2FB0(Character* pChar, int a, int b);
void  LLMath_mat44fltMultiply(f32 mtx[4][4], Vec4* src, Vec4* dst);    // VecMath.c: a point through a matrix
void  Character_GetBonePos(Character* pChar, int nBone, f32* pPos);
void  fn_8001CCF8(UStreamObject* pObject);
void  fn_8001CD80(UStreamObject* pObject);
void  fn_8001CE5C(UStreamObject* pObject);
void  fn_8001D020(UStreamObject* pObject);
void  fn_8001D3EC(UStreamObject* pObject);
void  fn_8001D7EC(Character* pChar);
void  fn_800BBADC(int nValue);         // SitDevFile.c
void  Character_GetBonePos_FromIndex(Character* pChar, int nBone, f32* pPos);
u8    Character_IsGolfer(Character* pChar);
f32   fn_8001ED44(Character* pChar, int b);
f32   fn_8001EE00(Character* pChar, int b);
void  Character_KeepClubOutOfGround(Character* pChar);
void  Character_IKLegsToGround(Character* pChar, u8 bRightLeg, u8 bLeftLeg);
void  Character_IKLegToGround(Character* pChar, CourseInfo* pCourse, int nLeg, int nHip, int nKnee,
                              int nAnkle, int nToe, int nAnklePoint, int nToePoint);
void  Character_CalculateClipPoints(Character* pChar);
void  Character_SetupForShot(Character* pChar);
void  fn_80021978(u8 v);                                        // ska_shared.c
void  fn_8002787C(CharModel* pModel);                           // Skeleton.c
void  fn_800279C0(Character* pChar);                            // Skeleton.c
void  SKEL_UpdateState(CharModel* pModel, SkelPose* pPose, u8 bTransform);   // Skeleton.c
void  fn_80037C48(Skin* pSkin, SkelPose* pPose);                // Skin.c
void  fn_8009622C(Character* pChar, void* pClip, u8 bKeep, f32 fOffset);                  // CharAnim.c
void  fn_80096F0C(Character* pChar);                            // CharAnim.c
void  Quat_QuatToMatrix(f32* pQ, f32 (*m)[4]);                        // Quaternion.c: a rotation matrix
int   Character_UpdateClubAttachment(Character* pChar, Clip* pClip);
void  Quat_Invert(f32* pQ, f32* pOut);                          // Quaternion.c
void  Quat_RotateVector(f32* pQ, f32* pIn, f32* pOut);                // Quaternion.c: a vector turned by pQ
void  fn_800280E8(Character* pChar, f32* pPos, int bPlace);     // Skeleton.c
void  fn_8001EFB4(f32* pA, f32* pB, f32* pOut);
void  Character_BeginLoadTexturesCallbackIG(Character* pChar);
void  fn_8001D6D8(int n);
void  fn_8010B098(void* pModel);                                // LLDynTex.c
void  fn_800958EC(AnimPlayer* pAnim, s32 n, f32 f);            // CharAnim.c
void  Quat_ExtractEulerAngles(f32* pQ, f32* pA, f32* pB, f32* pC);          // Quaternion.c: a rotation as angles
void  fn_80029968(CharModel* pModel, SkelPose* pPose);          // Skeleton.c
void  mat44flt_Invert(f32 (*pSrc)[4], f32 (*pDst)[4]);              // UMemPool.c
void  Character_GetBonePosSwapIfLefty(Character* pChar, int nBone, f32* pPos);
void  Char_Vec3Add(f32* pA, f32* pB, f32* pOut);
void  Char_Vec3Sub(f32* pA, f32* pB, f32* pOut);
void  fn_80095558(void);
void  fn_800253E0_ApplySavedChoices(int nSlot);                         // skalib.c
void  fn_80025478(void);                                        // skalib.c
void  fn_800CA7E0(void);                                        // AnimStream.c
void  fn_800CABA0(void);                                        // AnimStream.c
void  fn_800CB078(void);                                        // AnimStream.c
void  fn_8001DD18(u8* pData, int nBytes);
void  fn_8001DEC8(u8* pData, int nBytes);
s32   fn_800CE8C0(Skin** apSkins, int nSkins, SkinListEntry** ppList);   // SkinPart.c
void  fn_800CEEBC(void);                                        // SkinPart.c: empty
void  fn_800100B0(TexBank* pBank, TexEntry* p8, TexPalette* pC, void* p10, void* p14, int nNumTex,
                  int nNumPalettes);                            // LLTex.c
void  fn_8001EFD8(f32* pA, f32* pB, f32* pOut);
f32 (*Character_GetBoneMatrixSwapIfLefty(Character* pChar, int nBone))[4];
f32 (*Character_GetBoneMatrix_FromIndex(Character* pChar, int nBone))[4];
f32   Camera_GetLensFovScale(CamLens* pLens);
void  SKEL_EnableIK(u8 bOn);
void  fn_80035C58(void);
void  fn_80035CC0(void);
void  fn_80036460(int n);
void  fn_80036464(void);
void  fn_80095554(void);
void  fn_8009555C(void);
void  fn_80095560(void);
void  fn_80095564(void);
void  fn_800955F0(int nPlayer);
void  sApplyUserLogos(void* pChar, void* pModel, SkinChoices* pChoices);   // char_tex_manager.c
void  fn_8010BA2C(void* p);
void  fn_8008B704(void);               // FEgolferanim.c
void  fn_8008B754(int nNext);           // FEgolferanim.c
u8    fn_8008E924(void);                // FEgolferanim.c
void  fn_8008E918(s32 v);
u8    fn_8008E938(void);
void  fn_8008EAC8(u8 v);
void  fn_800C937C(void);
void  fn_800C9764(void);
void  fn_800C9FE0(void);
void  SkinPart_Init(void);
void  SkinPart_Shutdown(void);
void  fn_800CEE04(Skin* pSkin, int a, int b);
s32   SkinPart_GetNumSets(Skin* pSkin);         // SkinPart.c: how many choices aSets[3] holds
void  SkinPart_SetChangeAllCopies(u8 b);
u8    fn_800FCC38(int nPlayer);
void  fn_8010A668(void* p);
void  fn_80008380(void);
void  fn_800106AC(int n);               // LLTexGrp.c
void  fn_800106B8(u8 b);                // LLTexGrp.c
void  fn_8008F310(void);                // uiLoadFile.c: park the UI file's data in ARAM
void* fn_8008F354(void);                // uiLoadFile.c: the UI file's buffer
void  fn_8008F35C(void);                // uiLoadFile.c: bring the UI file's data back
void  fn_801141F8(struct DynChain* pChain, CharModel* pModel);                         // DynChain.c
void  fn_80035600(void);                // GoTerrain.c
void  fn_80035604(void);                // GoTerrain.c
void  fn_800358E0(Character* pChar, u32 uFlags);
void  fn_80035B40(Character* pChar, int n);
void  fn_800364A0(void);                // Skin.c
int   fn_800636EC(void);                // GoCamCont.c
void  fn_8010BF68(void);
void  fn_8010BFE0(void);
void  fn_80112C64(int n);
void  fn_80112CEC(void);

// ---- sweep code (not yet cleaned up) ----
void BitArray_SetAll(u32* aBits, u32 nBits);
void BitArray_ClearAll(u32* aBits, u32 nBits);
void CharSkinRef_Init(Skin* pSkin, CharSkinRef* pRef, s32 n);
void CharSkinRef_Free(void* p);
void fn_8001C650(void* arg0, s32 arg1);

// ---- end of sweep code ----

// This file's .sbss (charstate.h), in reverse address order as the compiler lays it out.
s32 lbl_80281CAC;
s32 lbl_80281CA8;

CharModelDefs lbl_80280E10 = { lbl_80186E80, 2 };
CharModelDefs lbl_80280E18 = { lbl_80186EA0, 2 };
s32 lbl_80280E20 = 3;
CharSkinSet* lbl_80280E24[2] = { NULL, NULL };

u64 lbl_801B9638[6];
Character* lbl_801B9624[5];
CharPool lbl_801B95E8;
f32 lbl_801B95D8[4];
f32 lbl_801B95C8[4];

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f, 2^30, 0.0f, -60000.0f and 3.0f (0x80282BC0), 1.0f and 3.0f before their first users
// below; its body is unknown, this one only reproduces the order.
static void char_StrippedFn(void) {
    Math_Sin(1.0f);
    Math_Sin(1073741824.0f);
    Math_Sin(0.0f);
    Math_Sin(-60000.0f);
    Math_Sin(3.0f);
}

// Clears the character's SKA tags (its animation events, Character.events): none set, each at time
// 2^30 (never).
void Character_ResetSKATags(Character* pChar) {
    s32 i;

    for (i = 0; i < 18; i++) {
        pChar->events[i].bSet = 0;
        pChar->events[i].fTime = 1073741824.0f;
    }
}

// Sets the character's SKA tags (animation events) for clip pBlend started at time fStart: all
// reset, then each of the clip's events set at fStart plus its time in the clip. Tags 5..14 are
// only taken when their time is past 0.
void Character_InitSKATags(Character* pChar, Clip* pBlend, f32 fStart) {
    u32 uId;
    int i;

    Character_ResetSKATags(pChar);
    if (pBlend->pEvents != NULL) {
        for (i = 0; i < pBlend->nEvents; i++) {
            uId = pBlend->pEvents[i].uId;
            if (uId < 5 || uId > 14 || pBlend->pEvents[i].fTime > 0.0f) {
                pChar->events[uId].bSet = 1;
                pChar->events[uId].fTime = fStart + pBlend->pEvents[i].fTime;
            }
        }
    }
}

// A random animation library (MtaLib) of group nGroup of the 'MAL ' bank of the character's slot
// (CharAnim.c plays it on the second animation player), or NULL when there is no bank or the group
// is empty.
void* Character_GetRandomMtaLib(Character* pChar, int nGroup, int n) {
    void* pItem = NULL;
    int nNum = 0;
    MalBank* pBank;

    if ((pBank = fn_8001F760(pChar->nSlot)) != NULL) {
        // port: EA passes fn_8001F780's arguments (with the count's address) to fn_8001F79C, which
        // takes three: the count's address arrives as its unused n, and n is ignored
        pItem = ((void* (*)(MalBank*, int, int*, int))fn_8001F79C)(pBank, nGroup, &nNum, n);
    }
    return pItem;
}

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

// Fill a blend node's pose from the character's body skin (none: the pose is left alone): the
// first two bit arrays cleared, the next two set, and every bone's rotation and position copied.
void Character_InitBoneState(Character* pChar, SkelPose* pPose) {
    int i;
    Skin* pSkin = pChar->pSkin;

    if (pSkin != NULL) {
        BitArray_ClearAll(pPose->a0, 0x80);
        BitArray_ClearAll(pPose->a10, 0x80);
        BitArray_SetAll(pPose->a20, 0x80);
        BitArray_SetAll(pPose->a30, 0x80);
        for (i = 0; i < pChar->pModel->nBones; i++) {
            Quat_Copy(pSkin->pose.aBones[i].q0, pPose->aBones[i].q0);
            Quat_Copy(pSkin->pose.aBones[i].v10, pPose->aBones[i].v10);
        }
    }
}

// Only the bit arrays of Character_InitBoneState (with a body skin): the last two set, the first
// two cleared.
void Character_InitBoneStateBits(Character* pChar, SkelPose* pPose) {
    if (pChar->pSkin != NULL) {
        BitArray_SetAll(pPose->a20, 0x80);
        BitArray_SetAll(pPose->a30, 0x80);
        BitArray_ClearAll(pPose->a0, 0x80);
        BitArray_ClearAll(pPose->a10, 0x80);
    }
}

// Reads the ground height under the four foot test points (0 right toe, 1 left toe, 2 right ankle,
// 3 left ankle) into afGroundHeight, where there is ground, and with bNormals the ground's normal
// into aGroundNormal (straight up where there is none). n1784 would pick half of the points per
// call (0 and 2, or 1 and 3), but it is set to -1 first, so every call does all four.
void Character_UpdateFeetTerrainInfo(Character* pChar, int bNormals) {
    f32* pNormal;
    f32 fHeight;
    int i;
    int nLast;
    int nStep;

    if (Ter_GetTGD() != NULL) {
        pChar->n1784 = -1;
        if (pChar->n1784 < 0) {
            pChar->n1784 = 0;
            nLast = 3;
            nStep = 1;
        } else {
            nLast = 2;
            nStep = 2;
        }
        for (i = 0; i <= nLast; i += nStep) {
            fHeight = Character_GetTerrainHeightAndNormal(pChar, pChar->aPoints[i + pChar->n1784], &pNormal);
            if (!(fHeight < -60000.0f)) {
                pChar->afGroundHeight[i + pChar->n1784] = fHeight;
            }
            if (bNormals) {
                if (fHeight < -60000.0f) {
                    pChar->aGroundNormal[i + pChar->n1784][0] = 0.0f;
                    pChar->aGroundNormal[i + pChar->n1784][1] = 1.0f;
                    pChar->aGroundNormal[i + pChar->n1784][2] = 0.0f;
                    pChar->aGroundNormal[i + pChar->n1784][3] = 0.0f;
                } else {
                    LLMath_CopyVec(pNormal, pChar->aGroundNormal[i + pChar->n1784]);
                }
            }
        }
        pChar->n1784++;
        if (pChar->n1784 >= 2) {
            pChar->n1784 = 0;
        }
    }
}

// The ground height at pPos (searched from 0.055 above it), with *ppNormal pointed at that ground's
// normal (a static copy); -65536.125 for none, and for surface classes 0xC and 0x12. Of the
// supporting ground (just below the point) and the covering ground (just above it), the covering
// one is taken when it is the only one, or the supporting one is on class 7 or 0x13, or the two are
// less than 0.05 apart, or it is less than 1 above the point.
f32 Character_GetTerrainHeightAndNormal(Character* pChar, f32* pPos, f32** ppNormal) {
    f32 vPos[4];
    f32 fSupportingHeight;
    f32 fCoveringHeight;
    SurfaceType* pSupportingSurface;
    SurfaceType* pCoveringSurface;
    CourseInfo* pCourse;

    if (pChar != NULL) {
        if ((pCourse = Ter_GetTGD()) != NULL) {
            LLMath_CopyVec(pPos, vPos);
            vPos[1] += 0.66f / 12.0f;
            Ter_GetEnclosingGroundData(pCourse, vPos, &fSupportingHeight, &pSupportingSurface, lbl_801B95D8,
                                       &fCoveringHeight, &pCoveringSurface, lbl_801B95C8);
            if (!(fCoveringHeight < -60000.0f)) {
                if (fSupportingHeight < -60000.0f || pSupportingSurface->nClass == 7 ||
                    pSupportingSurface->nClass == 0x13 || fCoveringHeight - fSupportingHeight < 0.05f ||
                    fCoveringHeight < 1.0f + pPos[1]) {
                    if (pCoveringSurface->nClass == 0xC || pCoveringSurface->nClass == 0x12) {
                        return -65536.125f;
                    }
                    *ppNormal = lbl_801B95C8;
                    return fCoveringHeight;
                }
            } else if (fSupportingHeight < -60000.0f) {
                goto none;      // fake match: the original puts this return after the supporting height
            }
            if (pSupportingSurface->nClass == 0xC || pSupportingSurface->nClass == 0x12) {
                return -65536.125f;
            }
            *ppNormal = lbl_801B95D8;
            return fSupportingHeight;
        none:
            return -65536.125f;
        }
        return -65536.125f;
    }
    return -65536.125f;
}

// Moves a golfer's five test points with its bones (left and right swapped for a left-hander): 0-3
// the right toe, left toe, right ankle and left ankle (bones 0x3A, 0x48, 0x39, 0x47), from the
// points Character_SetSkin kept in the skin, or before that set out along the bones' axes by the
// model's leg sizes (f10 right, fC left); 4 the club class's club point through the club bone 0x52
// (IGdriver).
void Character_UpdateTestPoints(Character* pChar) {
    f32 (*pClubMtx)[4];
    f32 (*pMtxLToe)[4];
    f32 (*pMtxRToe)[4];
    f32 (*pMtxLFoot)[4];
    f32 (*pMtxRFoot)[4];
    f32 fRight;
    f32 fLeft;

    if (!Character_IsGolfer(pChar)) {
        return;
    }
    pClubMtx = Character_GetBoneMatrix(pChar, 0x52);
    if (pChar->pSkin->b1044) {
        pMtxLToe = Character_GetBoneMatrixSwapIfLefty(pChar, 0x48);
        pMtxRToe = Character_GetBoneMatrixSwapIfLefty(pChar, 0x3A);
        pMtxLFoot = Character_GetBoneMatrixSwapIfLefty(pChar, 0x47);
        pMtxRFoot = Character_GetBoneMatrixSwapIfLefty(pChar, 0x39);
        LLMath_mat44fltMultiply(pMtxRToe, (Vec4*)pChar->pSkin->a1048[0], (Vec4*)pChar->aPoints[0]);
        LLMath_mat44fltMultiply(pMtxLToe, (Vec4*)pChar->pSkin->a1048[1], (Vec4*)pChar->aPoints[1]);
        LLMath_mat44fltMultiply(pMtxRFoot, (Vec4*)pChar->pSkin->a1048[2], (Vec4*)pChar->aPoints[2]);
        LLMath_mat44fltMultiply(pMtxLFoot, (Vec4*)pChar->pSkin->a1048[3], (Vec4*)pChar->aPoints[3]);
    } else {
        fRight = 0.8f * pChar->pModel->f10;
        fLeft = 0.8f * pChar->pModel->fC;
        pMtxLToe = Character_GetBoneMatrixSwapIfLefty(pChar, 0x48);
        pMtxRToe = Character_GetBoneMatrixSwapIfLefty(pChar, 0x3A);
        pMtxLFoot = Character_GetBoneMatrixSwapIfLefty(pChar, 0x47);
        pMtxRFoot = Character_GetBoneMatrixSwapIfLefty(pChar, 0x39);
        LLMath_AddScale(pMtxRToe[3], pMtxRToe[1], pChar->pModel->f10, pChar->aPoints[0]);
        LLMath_AddScale(pMtxLToe[3], pMtxLToe[1], pChar->pModel->fC, pChar->aPoints[1]);
        LLMath_AddScale(pMtxRFoot[3], pMtxRToe[2], fRight, pChar->aPoints[2]);
        LLMath_AddScale(pMtxLFoot[3], pMtxLToe[2], fLeft, pChar->aPoints[3]);
        LLMath_AddScale(pChar->aPoints[0], pMtxRToe[2], 0.25f * fRight, pChar->aPoints[0]);
        LLMath_AddScale(pChar->aPoints[1], pMtxLToe[2], 0.25f * fLeft, pChar->aPoints[1]);
    }
    if (pChar->p16D8 != NULL && pClubMtx != NULL) {
        LLMath_mat44fltMultiply(pClubMtx, (Vec4*)pChar->p16D8->a3C[pChar->nClubClass],
                                (Vec4*)pChar->aPoints[4]);
    }
}

// Keeps the club out of the ground: on fairly level ground (the feet's average ground normal a179C
// has y above 0.9), when the club point (test point 4) is below the terrain and the club bone
// 0x52's y axis points into the slope, that axis is shortened by how far the point is under,
// measured against the club class's head height (not below 3/4 of it).
void Character_KeepClubOutOfGround(Character* pChar) {
    f32 vNormal[4];
    f32 (*pMtx)[4];
    CourseInfo* pCourse;
    f32 fHeight;
    f32 fUnder;
    f32 fDot;
    f32 fLength;
    f32 fHead;

    if (pChar->p16D8 != NULL && pChar->a179C[1] > 0.9f) {
        pMtx = Character_GetBoneMatrix(pChar, 0x52);
        if (pMtx != NULL && (pCourse = Ter_GetTGD()) != NULL) {
            fHeight = fn_8004D650(pCourse, pChar->aPoints[4], vNormal);
            // the else's return is the dead second `b` after the fUnder return; !(<) keeps the
            // NaN case of `fHeight < -60000.0f`
            if (!(fHeight < -60000.0f)) {
                fUnder = fHeight - pChar->aPoints[4][1];
                if (fUnder < 0.0f) {
                    return;
                }
            } else {
                return;
            }
            fDot = -fn_8001EEA4(pMtx[1], vNormal);
            if (fDot > 0.707f) {
                fHead = pChar->p16D8->afC[pChar->nClubClass];
                fLength = (fHead - fUnder * vNormal[1] / fDot) / fHead;
                if (fLength > 0.75f) {
                    Vec3_Scale(fLength, pMtx[1], pMtx[1]);
                    SKEL_UpdateSkinningMatrix(pChar->pModel, pMtx, 0x52);
                }
            }
        }
    }
}

// Advances the character's animation by fTime: both animation players and their blend trees, the
// pose (the club head at the club class's height), the bones, the feet on the ground and the
// model's dynamic chains. Unless bForce, it waits while a golfer's state is 0x13 and while f14 of
// any other character is above fn_8001ED44.
void Character_UpdateAnimation(Character* pChar, int bForce, f32 fTime) {
    u32 auBits[4];
    int bSetupForShot = 0;
    u8 bRightLegIK = 0;
    u8 bLeftLegIK = 0;
    int i;

    if (pChar == NULL) {
        return;
    }
    if (pChar->uFlags & 1) {
        pChar->n1698 = 0;
        return;
    }
    if (pChar->u10 & 1) {
        return;
    }
    if (!bForce) {
        if (Character_IsGolfer(pChar)) {
            // fake match: the state is compared as an s8 (see GOLFERSTATE_GetCurrentState)
            if ((s8)GOLFERSTATE_GetCurrentState(pChar->nPlayer) == 0x13) {
                return;
            }
        } else if (pChar->f14 > fn_8001ED44(pChar, gSession.nSplitScreen)) {
            pChar->f14 = 1073741824.0f;
            pChar->n1698 = 0;
            return;
        }
    }
    pChar->f14 = 1073741824.0f;
    if (pChar->u10 & 4) {
        Character_SetupForShot(pChar);
        bSetupForShot = 1;
    }
    if (pChar->n20 == 8 && pChar->nAnim == 8) {
        SKATime_Idle(pChar, pChar->nPlayer, (AnimPlayer*)pChar->anim, &pChar->blend, fTime);
    } else if (pChar->u10 & 0x100) {
        SKATime_Update((AnimPlayer*)pChar->anim, &pChar->blend, 5.0f * fTime);
    } else {
        SKATime_Update((AnimPlayer*)pChar->anim, &pChar->blend, fTime);
        if (pChar->uFlags & 0x1000) {
            pChar->uFlags &= ~0x1000;
            if (pChar->pCurClip != NULL && pChar->pCurClip->pF4 != NULL) {
                fn_8009622C(pChar, pChar->pCurClip->pF4, 0, 0.5f);
            }
        }
    }
    fn_80021978(pChar->pModel->bEE);
    if (!gSession.b11) {
        SKATime_Update(&pChar->anim29C, &pChar->node3E0, fTime);
    }
    if (Character_IsGolfer(pChar)) {
        CharacterState_UpdateSKAState(pChar);
        if (!gSession.b11) {
            fn_80096F0C(pChar);
        }
    }
    if (pChar->pModel->pSkel != NULL) {
        fn_8002787C(pChar->pModel);
    }
    if (pChar->blend.pPose != NULL) {
        fn_8007260C(pChar, &pChar->blend, pChar->pModel, pChar->fAnimTime);
        if (Character_IsGolfer(pChar) && pChar->p16D8 != NULL) {
            // fake match: aBones[nClubHeadBone].v10[1] written as a flat float index (0x40 / 4 + 8 per
            // bone + 5), which gives the original's indexed store; the pose is all 4-byte words
            ((f32*)pChar->blend.pPose)[pChar->nClubHeadBone * 8 + 21] = pChar->p16D8->afC[pChar->nClubClass];
        }
        SKEL_UpdateState(pChar->pModel, pChar->blend.pPose, 0);
        if (Character_IsGolfer(pChar)) {
            if (BitArray_Test(pChar->blend.pPose->a0, CharModel_GetBoneIndexMapped(pChar->pModel, 0x36)) ||
                BitArray_Test(pChar->blend.pPose->a0, CharModel_GetBoneIndexMapped(pChar->pModel, 0x38)) ||
                BitArray_Test(pChar->blend.pPose->a0, CharModel_GetBoneIndexMapped(pChar->pModel, 0x39)) ||
                BitArray_Test(pChar->blend.pPose->a0, CharModel_GetBoneIndexMapped(pChar->pModel, 0x3A))) {
                bRightLegIK = 1;
            }
            if (BitArray_Test(pChar->blend.pPose->a0, CharModel_GetBoneIndexMapped(pChar->pModel, 0x44)) ||
                BitArray_Test(pChar->blend.pPose->a0, CharModel_GetBoneIndexMapped(pChar->pModel, 0x46)) ||
                BitArray_Test(pChar->blend.pPose->a0, CharModel_GetBoneIndexMapped(pChar->pModel, 0x47)) ||
                BitArray_Test(pChar->blend.pPose->a0, CharModel_GetBoneIndexMapped(pChar->pModel, 0x48))) {
                bLeftLegIK = 1;
            }
        }
    }
    if (!gSession.b11 && Character_IsGolfer(pChar) && pChar->node3E0.pPose != NULL) {
        fn_8007260C(pChar, &pChar->node3E0, pChar->pModel, pChar->anim29C.fTime);
        fn_80037C48(pChar->pSkin, pChar->node3E0.pPose);
    }
    if (pChar->uFlags & 2) {
        pChar->uFlags |= 1;
        return;
    }
    if (pChar->pfn17B0 != NULL) {
        pChar->pfn17B0();
    }
    BitArray_SetAll(auBits, 0x80);
    SKEL_TransformBones(pChar->pModel, auBits);
    if (Character_IsGolfer(pChar)) {
        Character_UpdateTestPoints(pChar);
        Character_UpdateFeetTerrainInfo(pChar, bSetupForShot || pChar->n20 != 5 || pChar->n26 == 1);
        if (pChar->n20 == 1 || pChar->n20 == 0 || pChar->n20 == 9 ||
            (pChar->n20 == 11 && !(pChar->u10 & 0x8000)) || pChar->n20 == 5 || pChar->n20 == 12) {
            Character_PlaceFeetOnGround(pChar);
        }
        Character_IKLegsToGround(pChar, bRightLegIK, bLeftLegIK);
    }
    if (Character_IsGolfer(pChar)) {
        if (pChar->pModel->pSkel != NULL) {
            fn_800279C0(pChar);
        }
        Character_KeepClubOutOfGround(pChar);
    }
    pChar->n1698 = 0;
    Character_CalculateClipPoints(pChar);
    if (Character_IsGolfer(pChar)) {
        fn_80029948(pChar->pModel, pChar->pModel->pF0, fTime);
        fn_80029948(pChar->pModel, pChar->pModel->pF4, fTime);
        fn_80029948(pChar->pModel, pChar->pModel->pF8, fTime);
        for (i = 0; i < 6; i++) {
            fn_80029948(pChar->pModel, pChar->pModel->apFC[i], fTime);
        }
        for (i = 0; i < 6; i++) {
            fn_80029948(pChar->pModel, pChar->pModel->ap114[i], fTime);
        }
    }
}

// Gives the character its skeleton (model) and looks up the bones the swing needs: the club head
// (0x53), the club bone (0x52, IGdriver: the trail's grip end) and the right wrist (0x15).
void Character_SetSkeleton(Character* pChar, CharModel* pModel) {
    if (pChar != NULL) {
        pChar->pModel        = pModel;
        pChar->nClubHeadBone = CharModel_GetBoneIndex(pChar->pModel, 0x53);
        pChar->nGripBone     = CharModel_GetBoneIndex(pChar->pModel, 0x52);
        pChar->n16A8         = CharModel_GetBoneIndexMapped(pChar->pModel, 0x15);
    }
}

// Gives the character its body's skin and poses the skeleton from it (Character_SetPreferedPos).
// For a golfer it then keeps the four foot test points in the skin (TW07 has this part as
// Character_SetTestPointsFeet): each toe bone's position moved by vBoneToToe and each ankle bone's
// by vBoneToHeel (larger offsets for animation slot 0), taken into the frame of its bone (through
// its inverted matrix) into Skin.a1048, and b1044 set.
void Character_SetSkin(Character* pChar, Skin* pSkin) {
    f32 mLToeInv[4][4];
    f32 mRToeInv[4][4];
    f32 mLFootInv[4][4];
    f32 mRFootInv[4][4];
    Vec4 vLToe;
    Vec4 vRToe;
    Vec4 vLFoot;
    Vec4 vRFoot;
    Vec4 vBoneToHeel;
    Vec4 vBoneToToe;
    f32 (*pMtxLToe)[4];
    f32 (*pMtxRToe)[4];
    f32 (*pMtxLFoot)[4];
    f32 (*pMtxRFoot)[4];

    if (pChar != NULL) {
        pChar->pSkin = pSkin;
        Character_SetPreferedPos(pChar);
        if (Character_IsGolfer(pChar)) {
            pMtxLToe = Character_GetBoneMatrixSwapIfLefty(pChar, 0x48);
            pMtxRToe = Character_GetBoneMatrixSwapIfLefty(pChar, 0x3A);
            pMtxLFoot = Character_GetBoneMatrixSwapIfLefty(pChar, 0x47);
            pMtxRFoot = Character_GetBoneMatrixSwapIfLefty(pChar, 0x39);
            mat44flt_Invert(pMtxLToe, mLToeInv);
            mat44flt_Invert(pMtxRToe, mRToeInv);
            mat44flt_Invert(pMtxLFoot, mLFootInv);
            mat44flt_Invert(pMtxRFoot, mRFootInv);
            if (pChar->nSlot == 0) {
                vBoneToToe.x = 0.0f;
                vBoneToToe.y = -0.031f;
                vBoneToToe.z = 0.0f;
                vBoneToToe.w = 1.0f;
                vBoneToHeel.x = 0.0f;
                vBoneToHeel.y = -0.11f;
                vBoneToHeel.z = -0.06f;
                vBoneToHeel.w = 1.0f;
            } else {
                vBoneToToe.x = 0.0f;
                vBoneToToe.y = -0.025f;
                vBoneToToe.z = 0.0f;
                vBoneToToe.w = 1.0f;
                vBoneToHeel.x = 0.0f;
                vBoneToHeel.y = -0.08f;
                vBoneToHeel.z = -0.025f;
                vBoneToHeel.w = 1.0f;
            }
            Character_GetBonePosSwapIfLefty(pChar, 0x48, &vLToe.x);
            Character_GetBonePosSwapIfLefty(pChar, 0x3A, &vRToe.x);
            Character_GetBonePosSwapIfLefty(pChar, 0x47, &vLFoot.x);
            Character_GetBonePosSwapIfLefty(pChar, 0x39, &vRFoot.x);
            Char_Vec3Add(&vLToe.x, &vBoneToToe.x, &vLToe.x);
            Char_Vec3Add(&vRToe.x, &vBoneToToe.x, &vRToe.x);
            Char_Vec3Add(&vLFoot.x, &vBoneToHeel.x, &vLFoot.x);
            Char_Vec3Add(&vRFoot.x, &vBoneToHeel.x, &vRFoot.x);
            LLMath_mat44fltMultiply(mRToeInv, &vRToe, (Vec4*)pChar->pSkin->a1048[0]);
            LLMath_mat44fltMultiply(mLToeInv, &vLToe, (Vec4*)pChar->pSkin->a1048[1]);
            LLMath_mat44fltMultiply(mRFootInv, &vRFoot, (Vec4*)pChar->pSkin->a1048[2]);
            LLMath_mat44fltMultiply(mLFootInv, &vLFoot, (Vec4*)pChar->pSkin->a1048[3]);
            pChar->pSkin->b1044 = 1;
        }
    }
}

// Poses the character's skeleton in its body skin's rest pose (SKEL_UpdateState, fn_80029968) and,
// when the skin has a model, hands the skeleton the skin's matrices. Needs a character, a skin and
// a model.
void Character_SetPreferedPos(Character* pChar) {
    if (pChar != NULL && pChar->pSkin != NULL && pChar->pModel != NULL) {
        SKEL_UpdateState(pChar->pModel, &pChar->pSkin->pose, 1);
        fn_80029968(pChar->pModel, &pChar->pSkin->pose);
        if (pChar->pSkin->pModel != NULL) {
            fn_80037AB8(pChar->pSkin, pChar->pModel, 0, 0);
            fn_80029A74(pChar->pModel, pChar->pSkin->pModel->p34);
            SKEL_SetDefaultWorld2BoneMatrices(pChar->pModel, pChar->pSkin->p1088);
            fn_80029A7C(pChar->pModel, pChar->pSkin->p108C, pChar->pSkin->pModel->n14);
        }
    }
}

// fake match: an identity read; it gives EA's register order.
static inline f32* fn_800187CC_Read(f32* p) { return p; }

// A golfer is dropped to 0.01 below the lowest of its four ground heights (the test points move
// with it) and a179C set to the average ground normal; any other character stands on the ground
// under its root bone (the ground below it, if the one found is more than 1 above).
void Character_PlaceFeetOnGround(Character* pChar) {
    CourseInfo* pCourse;
    f32 fLowest;
    f32 fY;
    f32 fDelta;
    f32* pPos;
    int i;
    f32 fH;
    f32 fSupportingHeight;
    f32 fCoveringHeight;
    SurfaceType* pSupportingSurface;
    SurfaceType* pCoveringSurface;
    f32 vSupportingNormal[4];
    f32 vCoveringNormal[4];

    if (pChar == NULL) {
        return;
    }
    pCourse = Ter_GetTGD();
    if (pCourse == NULL) {
        return;
    }
    fLowest = 1073741824.0f;
    if (Character_IsGolfer(pChar)) {
        pChar->a179C[0] = 0.0f;
        pChar->a179C[1] = 0.0f;
        pChar->a179C[2] = 0.0f;
        pChar->a179C[3] = 0.0f;
        for (i = 0; i < 4; i++) {
            Char_Vec3Add(pChar->aGroundNormal[i], pChar->a179C, pChar->a179C);
            if (pChar->afGroundHeight[i] < fLowest) {
                fLowest = pChar->afGroundHeight[i];
            }
        }
        LLMath_Normalize3(pChar->a179C, pChar->a179C);
        if (fLowest < -60000.0f) {
            return;
        }
        fY = fLowest - 0.01f;
        fDelta = fY - pChar->pModel->pBones[0].v1C[1];
        pChar->pModel->pBones[0].v1C[1] = fY;
        pChar->aPoints[0][1] += fDelta;
        pChar->aPoints[1][1] += fDelta;
        pChar->aPoints[2][1] += fDelta;
        pChar->aPoints[3][1] += fDelta;
        pChar->aPoints[4][1] += fDelta;
        return;
    }
    pPos = fn_800187CC_Read(pChar->pModel->pBones[0].v1C);
    Ter_GetEnclosingGroundData(pCourse, pPos, &fSupportingHeight, &pSupportingSurface, vSupportingNormal,
                               &fCoveringHeight, &pCoveringSurface, vCoveringNormal);
    fH = fCoveringHeight;
    if (fCoveringHeight < -60000.0f || fCoveringHeight > 1.0f + pPos[1]) {
        fH = fSupportingHeight;
        if (fH < -60000.0f) {
            fH = pPos[1];
        }
    }
    fY = fH;
    if (fY > 131072.25f || fY < -131072.25f) {
        return;
    }
    pChar->pModel->pBones[0].v1C[1] = fY;
}

// Puts a golfer's legs on the ground by IK: with bRightLeg the right leg (hip 0x36, knee 0x38,
// ankle 0x39, toe 0x3A), with bLeftLeg the left (0x44-0x48); the bones they move are then
// transformed again.
void Character_IKLegsToGround(Character* pChar, u8 bRightLeg, u8 bLeftLeg) {
    CourseInfo* pCourse;
    u32 auBits[4];

    if (pChar == NULL) {
        return;
    }
    BitArray_ClearAll(auBits, 0x80);
    if (!Character_IsGolfer(pChar)) {
        return;
    }
    pCourse = Ter_GetTGD();
    if (pCourse == NULL) {
        return;
    }
    if (bRightLeg) {
        Character_IKLegToGround(pChar, pCourse, 0, CharModel_GetBoneIndexMapped(pChar->pModel, 0x36),
                                CharModel_GetBoneIndexMapped(pChar->pModel,
                                        0x38), CharModel_GetBoneIndexMapped(pChar->pModel, 0x39),
                                CharModel_GetBoneIndexMapped(pChar->pModel, 0x3A), 2, 0);
        BitArray_Set(auBits, CharModel_GetBoneIndexMapped(pChar->pModel, 0x38));
        BitArray_Set(auBits, CharModel_GetBoneIndexMapped(pChar->pModel, 0x36));
        BitArray_Set(auBits, CharModel_GetBoneIndexMapped(pChar->pModel, 0x39));
    }
    if (bLeftLeg) {
        Character_IKLegToGround(pChar, pCourse, 1, CharModel_GetBoneIndexMapped(pChar->pModel, 0x44),
                                CharModel_GetBoneIndexMapped(pChar->pModel,
                                        0x46), CharModel_GetBoneIndexMapped(pChar->pModel, 0x47),
                                CharModel_GetBoneIndexMapped(pChar->pModel, 0x48), 3, 1);
        BitArray_Set(auBits, CharModel_GetBoneIndexMapped(pChar->pModel, 0x46));
        BitArray_Set(auBits, CharModel_GetBoneIndexMapped(pChar->pModel, 0x44));
        BitArray_Set(auBits, CharModel_GetBoneIndexMapped(pChar->pModel, 0x47));
    }
    if (BitArray_Intersects(auBits, auBits, 0x80)) {
        SKEL_TransformBones(pChar->pModel, auBits);
    }
}

// Bends leg nLeg (0 right, 1 left; its hip, knee, ankle and toe bones, and its ankle and toe test
// points) so its foot stands on the ground: the knee and hip are turned so the ankle reaches the
// ground height under the ankle point, then the ankle is tilted towards the ground's slope, more
// the deeper the foot sat.
void Character_IKLegToGround(Character* pChar, CourseInfo* pCourse, int nLeg, int nHip, int nKnee,
                             int nAnkle, int nToe, int nAnklePoint, int nToePoint) {
    f32 vOld[4];
    f32 vAnkle[4];
    f32 vPoint[4];
    f32 vToe[4];
    f32 vHip[4];
    f32 vKnee[4];
    f32 vReach[4];
    f32 vLeg[4];
    f32 vThigh[4];
    f32 vShin[4];
    f32 vFoot[4];
    f32 vAxis[4];
    f32 vSlope[4];
    f32 qTurn[4];
    f32 qB8[4];
    f32 qA8[4];
    f32 q98[4];
    f32 q88[4];
    f32 q78[4];
    f32 q68[4];
    f32 q58[4];
    f32 q48[4];
    f32 q38[4];
    f32 q28[4];
    f32 q18[4];
    f32 vNormal[4];
    f32 fDropB;
    f32 fReach;
    f32 fLeg;
    f32 fThigh;
    f32 fShin;
    f32 fDrop;
    f32 fSq;
    f32 fCos;
    f32 fAngleA;
    f32 fAngleB;
    f32 fTurn;
    f32 fTurn2;
    f32 fTurn3;
    f32 fDen;
    f32 fDropA;
    f32 fLen;
    Skeleton* pSkel;
    Bone* pBone;

    Character_GetBonePos_FromIndex(pChar, nAnkle, vAnkle);
    Character_GetBonePos_FromIndex(pChar, nHip, vHip);
    Character_GetBonePos_FromIndex(pChar, nKnee, vKnee);
    Character_GetBonePos_FromIndex(pChar, nToe, vToe);
    LLMath_CopyVec(pChar->aPoints[nAnklePoint], vPoint);
    // how far each test point sits below the ground (0.165 in, in feet)
    fDropA = 0.165f / 12.0f + (pChar->afGroundHeight[nAnklePoint] - vPoint[1]);
    fDropB = 0.165f / 12.0f + (pChar->afGroundHeight[nToePoint] - pChar->aPoints[nToePoint][1]);
    if ((fDropA < 0.0f && fDropB < 0.0f) || fDropA > 1.0f) {
        return;
    }
    fDrop = (fDropA <= fDropB) ? fDropB : fDropA;
    if (fDrop < 0.0f) {
        return;
    }
    Char_Vec3Add(pChar->aGroundNormal[nAnklePoint], pChar->aGroundNormal[nToePoint], vSlope);
    LLMath_Normalize3(vSlope, vSlope);
    if (fDrop > 0.33f / 12.0f) {
        fDrop = 1.0f;
    } else {
        fDrop = fDrop / (0.33f / 12.0f);
    }
    fDropA -= 0.165f / 12.0f;
    if (fDropA < 0.0f) {
        fDropA = 0.0f;
    }

    // the knee: the angle the thigh and shin must make for the hip to reach the ankle raised by fDropA
    Vec3Copy(vAnkle, vOld);
    vAnkle[1] += fDropA;
    Char_Vec3Sub(vHip, vOld, vReach);
    Char_Vec3Sub(vHip, vAnkle, vLeg);
    Char_Vec3Sub(vKnee, vHip, vThigh);
    Char_Vec3Sub(vKnee, vOld, vShin);
    Char_Vec3Sub(vOld, vToe, vFoot);
    fReach = (f32)Math_Sqrt(Vec3_LengthSqClamped(vReach));
    fLeg = (f32)Math_Sqrt(Vec3_LengthSqClamped(vLeg));
    fThigh = (f32)Math_Sqrt(Vec3_LengthSqClamped(vThigh));
    fShin = (f32)Math_Sqrt(Vec3_LengthSqClamped(vShin));
    if (fLeg > fThigh + fShin) {
        fLeg = fThigh + fShin;
    }
    fDen = 2.0f * fThigh * fShin;
    if (0.0f == fDen) {
        fDen = 1.0f;
    }
    fSq = fThigh * fThigh + fShin * fShin;
    fCos = (fSq - fReach * fReach) / fDen;
    fAngleA = Math_Acos((fCos < -1.0f) ? -1.0f : ((fCos > 1.0f) ? 1.0f : fCos));
    fCos = (fSq - fLeg * fLeg) / fDen;
    fAngleB = Math_Acos((fCos < -1.0f) ? -1.0f : ((fCos > 1.0f) ? 1.0f : fCos));
    fTurn = fAngleA - fAngleB;
    vec4flt_CrossProduct(vShin, vThigh, vNormal);
    fLen = LLMath_NormalizeReturnLength3(vNormal, vNormal);
    vNormal[3] = 0.0f;
    pSkel = pChar->pModel->pSkel;
    if (pSkel != NULL) {
        // a degenerate bend axis falls back on the last good one
        if (fLen > 0.0001f) {
            LLMath_CopyVec(vNormal, pSkel->a10E8[nLeg]);
        } else {
            LLMath_CopyVec(pSkel->a10E8[nLeg], vNormal);
        }
    }
    if (fabsf(fTurn) > 0.0001f) {
        Vec3_Scale(fTurn, vNormal, vAxis);
        Quat_BuildFromVector(vAxis, qTurn);
        Quat_Invert(pChar->pModel->pPoses[nKnee].q0, qA8);
        Quat_RotateVector(qA8, qTurn, q98);
        Quat_Multiply(q98, pChar->pModel->pBones[nKnee].q0C, qB8);
        Quat_Copy(qB8, pChar->pModel->pBones[nKnee].q0C);
    }

    // the hip: turned by the change in the angle between the thigh and the hip-to-foot line
    fTurn2 = Math_Asin(fShin * Math_Sin(fAngleA) / fReach);
    fTurn2 -= Math_Asin(fShin * Math_Sin(fAngleB) / fLeg);
    if (fabsf(fTurn2) > 0.0001f) {
        Vec3_Scale(fTurn2, vNormal, vAxis);
        vAxis[3] = 0.0f;
        Quat_BuildFromVector(vAxis, qTurn);
        Quat_Invert(pChar->pModel->pPoses[nHip].q0, qA8);
        Quat_RotateVector(qA8, qTurn, q98);
        Quat_Multiply(q98, pChar->pModel->pBones[nHip].q0C, qB8);
        Quat_Copy(qB8, pChar->pModel->pBones[nHip].q0C);
    }

    // the ankle: tilted about the horizontal axis across the slope, by the slope's angle
    pBone = &pChar->pModel->pBones[nHip];
    Quat_Multiply(pBone->q0C, pChar->pModel->pPoses[pBone->nParent].q0, q88);
    Quat_Multiply(pChar->pModel->pBones[nKnee - 1].q0C, q88, q58);
    Quat_Multiply(pChar->pModel->pBones[nKnee].q0C, q58, q68);
    Quat_Invert(q68, q78);
    Quat_Multiply(pChar->pModel->pPoses[nAnkle].q0, q78, q48);
    vAxis[0] = vSlope[2];
    vAxis[1] = 0.0f;
    vAxis[2] = -vSlope[0];
    vAxis[3] = 0.0f;
    fLen = (f32)Math_Sqrt(vAxis[0] * vAxis[0] + vAxis[2] * vAxis[2]);
    if (fLen < 0.01f) {
        return;
    }
    vAxis[0] *= 1.0f / fLen;
    vAxis[2] *= 1.0f / fLen;
    fCos = vSlope[1];
    fTurn3 = Math_Acos((fCos < -1.0f) ? -1.0f : ((fCos > 1.0f) ? 1.0f : fCos));
    fTurn3 *= fDrop;
    if (fabsf(fTurn3) > 0.0001f) {
        Vec3_Scale(fTurn3, vAxis, vAxis);
        Quat_Multiply(q48, q68, q38);
        Quat_Invert(q38, q28);
        vAxis[3] = 0.0f;
        Quat_BuildFromVector(vAxis, qTurn);
        Quat_RotateVector(q28, qTurn, qB8);
        Quat_Multiply(qB8, q48, q18);
        Quat_Copy(q18, pChar->pModel->pBones[nAnkle].q0C);
    }
}

// Moves the character to pPos (its root bone's position); with bPlace, the bones are transformed
// again and the feet put back on the ground.
void Character_SetPosition(Character* pChar, f32* pPos, u8 bPlace) {
    u32 auBits[4];

    BitArray_SetAll(auBits, 0x80);
    if (pChar != NULL) {
        LLMath_CopyVec(pPos, pChar->pModel->pBones->v1C);
        if (bPlace) {
            SKEL_TransformBones(pChar->pModel, auBits);
            Character_UpdateTestPoints(pChar);
            pChar->n1784 = -1;
            Character_UpdateFeetTerrainInfo(pChar, 1);
            Character_PlaceFeetOnGround(pChar);
        }
    }
}

// Turns the character's root bone to yaw fAngle (radians about y). A left-handed golfer is turned
// half a turn more in game type 3 (a front-end type; TW07 asks GM_IsFrontEnd here).
void Character_SetOrientation(Character* pChar, f32 fAngle) {
    if (pChar != NULL) {
        if (gSession.nGameType == 3 && Character_IsLeftHanded(pChar)) {
            fAngle += PI;
        }
        Quat_EulerAngles(0.0f, fAngle, 0.0f, pChar->pModel->pBones->q0C);
    }
}

// Turns the character to face along pDir, levelled (up is y), plus fAngle of extra yaw; a direction
// shorter than 0.01 is ignored.
void Character_SetOrientationVec(Character* pChar, f32* pDir, f32 fAngle) {
    f32 fLen;
    f32 mtx[4][4];              // row 3 is left unset (mat44flt_ExtractEulerAngles reads rows 0..2)
    f32 fYaw;
    f32 fPitch;
    f32 fRoll;

    if (pChar != NULL) {
        fLen = Math_Sqrt(Vec3_LengthSqClamped(pDir));
        if (fLen < 0.01f) return;
        Vec3_Scale(1.0f / fLen, pDir, mtx[0]);
        mtx[0][3] = 0.0f;
        mtx[1][0] = 0.0f;
        mtx[1][1] = 1.0f;
        mtx[1][2] = 0.0f;
        mtx[1][3] = 0.0f;
        vec4flt_CrossProduct(mtx[0], mtx[1], mtx[2]);
        mtx[2][3] = 0.0f;
        mat44flt_ExtractEulerAngles(mtx, &fYaw, &fPitch, &fRoll);
        Character_SetOrientation(pChar, fYaw + fAngle);
    }
}

// Allocates a character (static memory) and sets it up: its four data buffers, both blend trees and
// animation players, its SKA tags reset, and every field that starts at a value.
Character* Character_Create(void) {
    Character* pChar;
    SKABlendNode* pNode;
    int i;

    pNode = NULL;
    pChar = StaticMem_Alloc(sizeof(Character), 2, 0x40, "char.c", 0x8A4);
    pChar->pfn17B0 = NULL;
    for (i = 0; i < 4; i++) {
        pChar->buffers[i].n00 = -1;
        pChar->buffers[i].p04 = NULL;
        pChar->buffers[i].p0C = NULL;
        pChar->buffers[i].p10 = NULL;
        pChar->buffers[i].p14 = NULL;
        pChar->buffers[i].pBuf = StaticMem_Alloc(0x890, 2, 0x40, "char.c", 0x8AF);
    }
    pNode = &pChar->blend;
    fn_80072D90((AnimPlayer*)pChar->anim);
    fn_80071C28(&pNode, 1, 0, fn_80072ACC, 1);
    pNode = &pChar->node3E0;
    fn_80072D90(&pChar->anim29C);
    fn_80071C28(&pNode, 1, 1, fn_80072ACC, 1);
    pChar->pLib = NULL;
    pChar->n3D4 = 0;
    pChar->p44 = NULL;
    pChar->f14 = 0.0f;
    pChar->nAnim = 0;
    pChar->n20 = 0;
    pChar->n18 = 0;
    pChar->u10 = 0x4000;
    pChar->f162C = 1.0f;
    pChar->f1630 = 0.5f;
    pChar->f1634 = 0.2f;
    pChar->n1650 = 0;
    pChar->n1654 = 2;
    pChar->n1658 = 2;
    pChar->n1698 = 0;
    pChar->p16D8 = NULL;
    pChar->nClubClass = 0;
    pChar->n16D4 = 0;
    pChar->nSlot = 0;
    pChar->n48 = -1;
    pChar->f1664 = 1.0f;
    pChar->nStyle = 0;
    pChar->nPlayer = -1;
    pChar->uId = 0;
    pChar->pBlend = NULL;
    pChar->fBackswing = 0.0f;
    pChar->nGroup = -1;
    pChar->n5CC = -1;
    pChar->p1790 = NULL;
    pChar->n1784 = -1;
    pChar->n17B4 = 0;
    pChar->pRecords = NULL;
    fn_800962F8(pChar);
    pChar->n16DC = 0;
    pChar->f165C = pChar->f1660 = 1073741824.0f;
    pChar->pCurClip = NULL;
    pChar->p178C = NULL;
    pChar->a6C[0] = -1;
    pChar->a64[0] = NULL;
    pChar->a6C[1] = -1;
    pChar->a64[1] = NULL;
    if (gSession.nGameType == 10 || gSession.nGameType == 3) {
        pChar->n70 = 2;
    } else {
        pChar->n70 = 1;
    }
    pChar->n74 = 0;
    pChar->bE0 = 0;
    Character_ResetSKATags(pChar);
    pChar->p1798 = NULL;
    return pChar;
}

// The characters' set-up before a hole (and on a restart): with more than two players, the dynamic
// textures go to the player with the honor (CharacterTex_PreHoleInit).
void Character_PreHoleInit(void) {
    fn_80095554();
    CharacterTex_PreHoleInit();
}

// Poses the character at the moment of impact: the blend's time set to its event 2 (the ball hit;
// fAnimTime from f180, that event's time in the blend and v1638[1]), u10 bits 0x10000, 8 and 4 set
// (4: the update sets it up for the shot), then one animation update of no length with the
// skeleton's clip cleared.
void Character_AlignCharacterForShotImpact(Character* pChar) {
    if (pChar != NULL && pChar->pBlend != NULL) {
        pChar->u10 |= 0x10000;
        pChar->u10 |= 8;
        pChar->u10 |= 4;
        pChar->pModel->pSkel->pClip = NULL;
        pChar->fAnimTime = pChar->f180 + fn_8001F02C(pChar->pBlend, 2) - pChar->v1638[1];
        Character_UpdateAnimation(pChar, 0, 0.0f);
        pChar->pModel->pSkel->pClip = NULL;
    }
}

// Frees the character's textures: gives back its dynamic texture pool entries, frees the texture
// and palette tables Character_LoadTextures made, and closes its texture file.
void Character_FreeTextures(Character* pChar) {
    CharacterTex_ReleasePoolEntries(pChar);
    if (pChar->pA8 != NULL) {
        StaticMem_Free(pChar->pA8);
    }
    if (pChar->pB0 != NULL) {
        StaticMem_Free(pChar->pB0);
    }
    if (pChar->pB8 != NULL) {
        StaticMem_Free(pChar->pB8);
    }
    if (pChar->pBC != NULL) {
        StaticMem_Free(pChar->pBC);
    }
    if (pChar->hFile >= 0) {
        fn_8000633C(pChar->hFile);
    }
}

// Reads the character's textures from its CHR object (after the slider definitions, p4C) into
// bank78: in the front end (game types 10 and 3) all of them; otherwise, for each name the skins
// use (fn_800CE8C0), the texture of that name (and the one after it when it goes with it) with its
// palette, or an empty one. Then it opens the golfer's texture file.
void Character_LoadTextures(Character* pChar, Skin** apSkins, int nSkins) {
    TexEntry* pTexData;
    TexPalette* pPalData;
    int nTex;
    int nTexBytes;
    int nPalBytes;
    u8* pData;
    SkinListEntry* pList;
    u8 bFound;
    int nPal;
    u8 bAll;
    int nExtra;
    int nOut;
    int i;
    int j;
    int k;
    int m;
    int nNames;

    nExtra = 0;
    pList = NULL;
    pData = pChar->p4C;
    BYTESWAP_SWAPDATA(&pData, (u8*)&nTexBytes, 4, 4);
    BYTESWAP_SWAPDATA(&pData, (u8*)&nPalBytes, 4, 4);
    BYTESWAP_SWAPDATA(&pData, (u8*)&pChar->n58, 4, 4);
    BYTESWAP_SWAPDATA(&pData, (u8*)&pChar->n5C, 4, 4);
    if (nTexBytes != 0) {
        pTexData = (TexEntry*)pData;
        // fake match: pTexData and pPalData (the same pointers as pData) go to the swaps: EA's
        // registers
        fn_8001DD18((u8*)pTexData, nTexBytes);
        pData += nTexBytes;
    }
    if (nPalBytes != 0) {
        pPalData = (TexPalette*)pData;
        fn_8001DEC8((u8*)pPalData, nPalBytes);
    } else {
        pChar->n5C = 0;
    }
    bAll = gSession.nGameType == 10 || gSession.nGameType == 3;
    nTex = nTexBytes / (int)sizeof(TexEntry);
    nPal = nPalBytes / (int)sizeof(TexPalette);
    if (bAll) {
        pChar->nAC = nTex;
        pChar->nB4 = nPal;
    } else {
        pChar->nAC = fn_800CE8C0(apSkins, nSkins, &pList);
        nExtra = 0;
        pChar->nB4 = pChar->nAC;
        nPalBytes = pChar->nB4 * sizeof(TexPalette);
        for (i = 0; i < pChar->nAC; i++) {
            for (j = 0; j < nTex; j++) {
                if (pList[i].uId == pTexData[j].u0 && (pTexData[j].b47 & 1)) {
                    nExtra++;
                    break;
                }
            }
        }
        pChar->nAC += nExtra;
    }
    pChar->pA8 = NULL;
    pChar->pB8 = NULL;
    pChar->pB0 = NULL;
    pChar->pBC = NULL;
    if (pChar->nAC != 0) {
        pChar->pA8 = StaticMem_Alloc(pChar->nAC * sizeof(TexEntry), 2, 0x10, "char.c", 0x9A9);
        pChar->pB8 = StaticMem_Alloc(pChar->nAC * 64, 2, 0x10, "char.c", 0x9AE);
    }
    if (pChar->nB4 != 0) {
        pChar->pB0 = StaticMem_Alloc(nPalBytes, 2, 0x10, "char.c", 0x9B8);
        pChar->pBC = StaticMem_Alloc(nPal, 2, 0x10, "char.c", 0x9BD);
    }
    if (bAll) {
        if (pChar->nAC != 0) {
            Mem_cpy(pChar->pA8, pTexData, nTexBytes);
        }
        if (pChar->nB4 != 0) {
            Mem_cpy(pChar->pB0, pPalData, nPalBytes);
        }
    } else {
        nNames = pChar->nAC - nExtra;
        for (k = nOut = 0; k < nNames; k++) {
            bFound = 0;
            for (m = 0; m < nTex; m++) {
                if (pList[k].uId == pTexData[m].u0) {
                    Mem_cpy(&pChar->pA8[nOut], &pTexData[m], sizeof(TexEntry));
                    if (pTexData[m].nPalette != -1) {
                        Mem_cpy(&pChar->pB0[k], &pPalData[pTexData[m].nPalette], sizeof(TexPalette));
                        pChar->pA8[nOut].nPalette = k;
                    }
                    nOut++;
                    if (pTexData[m].b47 & 1) {
                        Mem_cpy(&pChar->pA8[nOut], &pTexData[m + 1], sizeof(TexEntry));
                        nOut++;
                    }
                    bFound = 1;
                    break;
                }
            }
            if (!bFound) {
                pChar->pA8[nOut].u0 = 0;
                pChar->pA8[nOut].nPalette = -1;
                nOut++;
            }
        }
    }
    if (pList != NULL) {
        StaticMem_Free(pList);
    }
    fn_800100B0(&pChar->bank78, pChar->pA8, pChar->pB0, pChar->pB8, pChar->pBC, pChar->nAC, pChar->nB4);
    pChar->p50 = &pChar->bank78;
    // port: EA passes arguments fn_800CEEBC (empty) ignores
    ((void (*)(Skin*, TexBank*, int, int))fn_800CEEBC)(pChar->pSkin, &pChar->bank78, 0xBF600, 0xCDA);
    sprintf(pChar->szE1, "%sdata\\CharStrm\\CharTex\\%02dalltex.fxg", "", pChar->nC + 1);
    pChar->hFile = fn_800060E0(pChar->szE1);
}

// Copies every skin's newest choices (copy 3, see SkinPart_SetChangeAllCopies) down to copy 2, as a
// texture load starts.
void Character_CopySkinChoices3To2(Character* pChar) {
    int i;
    for (i = 0; i < pChar->nSkins; i++) {
        fn_800CEE04(pChar->apSkins[i], 3, 2);
    }
}

// Copies every skin's choices from copy 2 down to copy 1, as a texture load ends.
void Character_CopySkinChoices2To1(Character* pChar) {
    int i;
    for (i = 0; i < pChar->nSkins; i++) {
        fn_800CEE04(pChar->apSkins[i], 2, 1);
    }
}

// Copies every skin's choices from copy 1 down to copy 0, the one shown, and flags each skin's
// choices changed (bit 0x1 of u10D4).
void Character_CopySkinChoices1To0(Character* pChar) {
    int i;
    for (i = 0; i < pChar->nSkins; i++) {
        fn_800CEE04(pChar->apSkins[i], 1, 0);
        pChar->apSkins[i]->u10D4 |= 1;
    }
}

// Queues a dynamic texture load (LLDynTex.c) for the character, with the callbacks run as it begins
// and as it ends (the Begin/End ...Callback functions). The pool's last entry points at the
// character queued, or at nothing when no job is free.
void Character_AddTextureLoadRequest(Character* pChar, void (*pfnBegin)(Character* pChar),
                                     void (*pfnEnd)(Character* pChar)) {
    DynTexJob* pJob = fn_8010B8EC();

    if (pJob != NULL) {
        lbl_801B95E8.a[6].p = pChar;
        pJob->pfnA = pfnBegin;
        pJob->pChar = pChar;
        pJob->pfnB = pfnEnd;
        pJob->p0 = &pChar->p50;
        fn_8010B930(pJob);
    } else {
        lbl_801B95E8.a[6].p = NULL;
    }
}

// The front end's begin callback of a texture load (the golfer that came in): sets up the dynamic
// textures of the character's model in use for the skins' newest choices
// (Character_CopySkinChoices3To2): the name codes the choices do not use and the ones they need go
// to its dynamic textures (fn_800CEB1C, fn_800CEBE8).
void Character_BeginLoadTexturesCallbackFE(Character* pArg) {
    // fake match: a copy of the parameter through void* (a plain copy is merged into it)
    Character* pChar = (Character*)(void*)pArg;
    void* pModel = pChar->a64[pChar->n74];

    fn_8008EAC8(0);
    pChar->p60 = pModel;
    fn_8010BC88(&pChar->p50);
    // port: EA passes an argument fn_8010BEC4 ignores
    ((void (*)(void*))fn_8010BEC4)(pModel);
    Character_CopySkinChoices3To2(pChar);
    fn_800CEB1C(pChar->apSkins, pChar->nSkins, pModel);
    fn_800CEBE8(pChar->apSkins, pChar->nSkins, pModel, NULL, 0);
    // port: EA passes an argument fn_8010BED4 ignores
    ((void (*)(void*))fn_8010BED4)(pModel);
}

// The front end's end callback of a texture load (the golfer that came in): passes the skins'
// choices down to copy 0, puts the profile's logos on the model in use, and sets the menu's b81
// (fn_8008EA38).
void Character_EndLoadTexturesCallbackFE(Character* pChar) {
    Character_CopySkinChoices2To1(pChar);
    Character_CopySkinChoices1To0(pChar);
    sApplyUserLogos(pChar, pChar->a64[pChar->n74], &FE_GetCurrentProfile()->choices);
    fn_8010BA2C(pChar->a64[pChar->n74]);
    fn_8008EA38(1);
}

// The front end's begin callback of a texture swap (the golfer shown): copies the model in use to
// the character's other model (fn_8010A6A8), puts on it each skin choice that differs from the
// skin's current one, and sets up its dynamic textures for the newest choices.
// Character_ExecuteTextureSwapFE switches to it.
void Character_BeginSwapTexturesCallbackFE(Character* pArg) {
    int i;
    int j;
    Skin* pSkin;
    // fake match: a copy of the parameter through void* (a plain copy is merged into it)
    Character* pChar = (Character*)(void*)pArg;
    void* pModel;

    fn_8008E918(1);
    pModel = pChar->a64[1 - pChar->n74];
    fn_8010A6A8(pChar->a64[pChar->n74], pModel);
    pChar->p60 = pModel;
    fn_8010BC88(&pChar->p50);
    // port: EA passes an argument fn_8010BEC4 ignores
    ((void (*)(void*))fn_8010BEC4)(pModel);
    for (i = 0; i < pChar->nSkins; i++) {
        pSkin = pChar->apSkins[i];
        for (j = 0; j < SkinPart_GetNumSets(pSkin); j++) {
            if (memcmp(&pSkin->aSets[2][j], &pSkin->aSets[3][j], sizeof(SkinChoice)) != 0) {
                fn_800CECE0(pSkin, j, pSkin->aSets[3][j].nVariant, pSkin->aSets[3][j].nOption, pModel);
            }
        }
    }
    Character_CopySkinChoices3To2(pChar);
    fn_800CEB1C(pChar->apSkins, pChar->nSkins, pModel);
    fn_800CEBE8(pChar->apSkins, pChar->nSkins, pModel, NULL, 0);
    // port: EA passes an argument fn_8010BED4 ignores
    ((void (*)(void*))fn_8010BED4)(pModel);
}

// Once a swap is due (fn_8008EAD4, set by Character_EndSwapTexturesCallbackFE): switches the
// character to its other model, the one the swap loaded, passes the skins' choices down to copy 0
// and puts the profile's logos and the skins on it.
void Character_ExecuteTextureSwapFE(Character* pChar) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    int i;
    void* pModel;

    if (fn_8008EAD4()) {
        fn_8008EAC8(0);
        fn_8008E918(0);
        pChar->n74 = 1 - pChar->n74;
        pModel = pChar->a64[pChar->n74];
        Character_CopySkinChoices1To0(pChar);
        sApplyUserLogos(pChar, pModel, &pProfile->choices);
        fn_8010BA2C(pModel);
        fn_80008380();
        for (i = 0; i < pChar->nSkins; i++) {
            fn_800CE170(pChar->apSkins[i], pModel);
        }
        fn_8010BC64(pModel);
    }
}

// The front end's end callback of a texture swap: passes the skins' choices down to copy 1, flags
// the swap due (fn_8008EAC8) and, unless the menu delays it (fn_8008E938), swaps now.
void Character_EndSwapTexturesCallbackFE(Character* pChar) {
    Character_CopySkinChoices2To1(pChar);
    fn_8008E918(2);
    fn_8008EAC8(1);
    if (fn_8008E938() == 0) {
        Character_ExecuteTextureSwapFE(pChar);
    }
}

// The in-game begin callback of a texture load: sets up the dynamic textures of the character's
// model in use (fn_8010B098), dresses it (Character_SetClubsAndClothes) and hands its dynamic
// textures the name codes the skins' newest choices need (fn_800CEBE8, given the "Glove" part's
// id); the last marked player (lbl_80281CAC) is dressed again.
void Character_BeginLoadTexturesCallbackIG(Character* pArg) {
    // fake match: a copy of the parameter through void* (a plain copy is merged into it)
    Character* pChar = (Character*)(void*)pArg;
    u64 uGlove;
    void* pModel;

    pModel = pChar->a64[pChar->n74];
    pChar->p60 = pModel;
    fn_8010BC88(&pChar->p50);
    fn_8010B098(pModel);
    // port: EA passes an argument fn_8010BEC4 ignores
    ((void (*)(void*))fn_8010BEC4)(pModel);
    Character_SetClubsAndClothes(pChar, pChar->nPlayer);
    Character_CopySkinChoices3To2(pChar);
    SKA_PackName(&uGlove, "Glove");
    fn_800CEBE8(pChar->apSkins, pChar->nSkins, pModel, &uGlove, 1);
    if (lbl_80281CAC >= 0) {
        Character_SetClubsAndClothes(gPlayers[lbl_80281CAC].pChar, lbl_80281CAC);
    }
    // port: EA passes an argument fn_8010BED4 ignores
    ((void (*)(void*))fn_8010BED4)(pModel);
}

// The in-game end callback of a texture load: passes the skins' choices down to copy 0, puts the
// created golfer's logos on the model in use, marks the character's textures loaded (bE0) and
// clears the pool's queued character.
void Character_EndLoadTexturesCallbackIG(Character* pChar) {
    Character_CopySkinChoices2To1(pChar);
    Character_CopySkinChoices1To0(pChar);
    sApplyUserLogos(pChar, pChar->a64[pChar->n74], pChar->pChoices);
    fn_8010BA2C(pChar->a64[pChar->n74]);
    pChar->bE0 = 1;
    lbl_801B95E8.a[6].p = NULL;
}

// Fills the characters' dynamic texture pool: two entries (the game-type test gives two either
// way), each a dynamic texture (LLDynTex.c) for 0x46 textures with a 0x87000-byte pixel buffer, all
// free. Called when a round or the front end starts.
void CharacterTex_Init(void) {
    int i;

    if (gSession.nGameType == 10 || gSession.nGameType == 3) {
        lbl_801B95E8.nEntries = 2;
    } else {
        lbl_801B95E8.nEntries = 2;
    }
    for (i = 0; i < lbl_801B95E8.nEntries; i++) {
        lbl_801B95E8.a[i].p = fn_8010A520(0x46, 0x87000, 0, 0x870, 4);
        lbl_801B95E8.a[i].bUsed = 0;
    }
}

// Free every pool entry's dynamic texture (fn_8010A668) and mark the entry free.
void CharacterTex_Close(void) {
    int i;
    for (i = 0; i < lbl_801B95E8.nEntries; i++) {
        fn_8010A668(lbl_801B95E8.a[i].p);
        lbl_801B95E8.a[i].bUsed = 0;
    }
}

// Gives the character's dynamic texture pool entries back (a64 and a6C cleared) and marks its
// textures not loaded (bE0).
void CharacterTex_ReleasePoolEntries(Character* pChar) {
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

// Takes free dynamic texture pool entries for the character until it has n70 of them: a64 gets each
// entry's dynamic texture, a6C its index. It takes fewer when the pool runs out.
void CharacterTex_TakePoolEntries(Character* pChar) {
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

// Empty in this build: the pool code calls it with each character just before the character gives
// its pool entries back. Left unnamed: there is nothing in it to read a name from.
void fn_8001A484(Character* pChar) {
}

// Runs one step of the dynamic texture loader (fn_8010BFE0) each frame from the main loop, with
// more than two players (with two or fewer every character's textures are loaded at the start).
void CharacterTex_TextureLoader(void) {
    if (gSession.nNumPlayers > 2) {
        fn_8010BFE0();
    }
}

// Before a hole: no player marked (lbl_80281CAC -1); with more than two players only the player
// with the honor keeps pool entries: every player's character gives its back, then that player's
// takes them and its dynamic textures are loaded now (fn_8010BF68 runs the loader to the end).
void CharacterTex_PreHoleInit(void) {
    Character* pChar;
    int i;

    lbl_80281CAC = -1;
    if (gSession.nNumPlayers > 2) {
        for (i = 0; i < gSession.nNumPlayers; i++) {
            fn_8001A484(gPlayers[i].pChar);
            CharacterTex_ReleasePoolEntries(gPlayers[i].pChar);
        }
        i = gpGame->pfnGetHonors(5);
        pChar = gPlayers[i].pChar;
        CharacterTex_TakePoolEntries(pChar);
        Character_AddTextureLoadRequest(pChar, Character_BeginLoadTexturesCallbackIG,
                                        Character_EndLoadTexturesCallbackIG);
        fn_8010BF68();
    }
}

// With more than two players, the dynamic textures go to player nPlayer (up now) and the next to
// play (GM_GetSecondHonors). nPlayer's character loses bit 0x40 of u10; after the display finishes
// drawing (fn_80008380), every other character holding pool entries (except the one being loaded,
// the pool's a[6].p) gives them back and gets bit 0x40 (not drawn). Unless nPlayer's textures are
// loaded (bE0), it takes the entries (the character being loaded giving its back first) and its
// textures are loaded now (fn_8010BF68); when it is the one being loaded, the loader is just run to
// the end. The next player's character is then queued the same way, its textures left to
// CharacterTex_TextureLoader.
void CharacterTex_StartStreamingPlayers(int nPlayer) {
    int i;
    Character* pChar;
    Character* pQueued;

    if (gSession.nNumPlayers > 2) {
        gPlayers[nPlayer].pChar->u10 &= ~0x40;
        fn_80008380();
        pChar = gPlayers[nPlayer].pChar;
        for (i = 0; i < gSession.nNumPlayers; i++) {
            if (i != nPlayer && gPlayers[i].pChar->a64[gPlayers[i].pChar->n74] != NULL &&
                gPlayers[i].pChar != lbl_801B95E8.a[6].p) {
                fn_8001A484(gPlayers[i].pChar);
                CharacterTex_ReleasePoolEntries(gPlayers[i].pChar);
                gPlayers[i].pChar->bE0 = 0;
                gPlayers[i].pChar->u10 |= 0x40;
            }
        }
        if (!pChar->bE0) {
            pQueued = lbl_801B95E8.a[6].p;
            if (pChar != pQueued) {
                fn_8010BF68();
                if (pQueued != NULL) {
                    fn_8001A484(pQueued);
                    CharacterTex_ReleasePoolEntries(pQueued);
                    pQueued->bE0 = 0;
                    pQueued->u10 |= 0x40;
                }
                CharacterTex_TakePoolEntries(pChar);
                Character_AddTextureLoadRequest(pChar, Character_BeginLoadTexturesCallbackIG,
                                                Character_EndLoadTexturesCallbackIG);
                fn_8010BF68();
            } else {
                fn_8010BF68();
            }
        }
        i = GM_GetSecondHonors();
        if (i < gSession.nNumPlayers) {
            pChar = gPlayers[i].pChar;
            if (!pChar->bE0 && pChar != lbl_801B95E8.a[6].p) {
                fn_8010BF68();
                CharacterTex_TakePoolEntries(pChar);
                Character_AddTextureLoadRequest(pChar, Character_BeginLoadTexturesCallbackIG,
                                                Character_EndLoadTexturesCallbackIG);
            }
        }
    }
}

// Runs the dynamic texture loader until nothing is left to load (fn_8010BF68). Called at the end of
// each hole.
void CharacterTex_WaitEndOfTextureLoader(void) {
    fn_8010BF68();
}

// The 'SAC ' handler: an animation library merged over the one of the slot the object's id names.
// port: the overlay library is little-endian on disc and AnimLib_MergeOverlay swaps it
//       (fn_80020BC8 > BYTESWAP_SWAPDATA): a little-endian port does not swap there.
void Character_LoadSacFromStream(UStreamObject* pObject) {
    AnimLib_MergeOverlay(pObject->pData, pObject->uId);
    StaticMem_Free(pObject);
}

void Character_RegisterSacStreamClient(void) {
    Stream_RegisterLoadChunkCallback('SAC ', Character_LoadSacFromStream);
}

void Character_UnregisterSacStreamClient(void) {
    Stream_UnregisterLoadChunkCallback('SAC ');
}

// Loads the round's sac animation files, merging each as it arrives (Character_LoadSacFromStream):
// fn_80014BB4 fills stream list 0 with the slots' malesac / femsac and every player's CharSac file,
// fn_80014DC0 loads until done.
void Character_LoadSacFiles(void) {
    Character_RegisterSacStreamClient();
    fn_80014BB4();
    fn_80014DC0();
    Character_UnregisterSacStreamClient();
}

// Before each hole after the first, with more than one player: the animation slot's libraries are
// rebuilt from their copies (AnimLib_ReloadSlot), its sac files are streamed in and merged
// (fn_80014C9C, Character_LoadSacFromStream), and the current slot's work copies freed
// (lbl_80281CE4 set). Then fn_800C9FE0 gives the first two players to play an animation stream slot
// each.
void Character_ReloadSacFiles(void) {
    if (gSession.nNumPlayers > 1) {
        lbl_80281CE4 = 1;
        AnimLib_ReloadSlot();
        Character_RegisterSacStreamClient();
        fn_80014C9C();
        fn_80014DC0();
        Character_UnregisterSacStreamClient();
        AnimLib_FreeWorkCopies();
    }
    fn_800C9FE0();
}

// Reopens every player's character texture file: closes them all, then opens
// "data\CharStrm\CharTex\NNalltex.fxg" for each golfer (NN is its id + 1).
void Character_ReopenTextureFiles(void) {
    int i;
    Character* pChar;

    for (i = 0; i < gSession.nNumPlayers; i++) {
        fn_8000633C(gPlayers[i].pChar->hFile);
    }
    for (i = 0; i < gSession.nNumPlayers; i++) {
        pChar = gPlayers[i].pChar;
        sprintf(pChar->szE1, "%sdata\\CharStrm\\CharTex\\%02dalltex.fxg", "", pChar->nC + 1);
        pChar->hFile = fn_800060E0(pChar->szE1);
    }
}

// The characters' set-up late in a round's start (GO_vInitIG, after the players are set up): with
// two players or fewer, every player's character takes its pool entries and its dynamic textures
// are loaded now (fn_8010BF68; in split screen fn_8001D6D8 for its player too). Then the saved
// choices of animation slots 0 and 1 are applied, the animation stream is set up (fn_800CA9DC,
// fn_800CA7E0, fn_800CABA0, fn_800CB078), the sac files are loaded (Character_LoadSacFiles) and the
// work copies freed.
void Character_PostInit(void) {
    int i;
    Character* pChar;

    fn_80095558();
    if (gSession.nNumPlayers <= 2) {
        for (i = 0; i < gSession.nNumPlayers; i++) {
            pChar = gPlayers[i].pChar;
            CharacterTex_TakePoolEntries(pChar);
            Character_AddTextureLoadRequest(pChar, Character_BeginLoadTexturesCallbackIG,
                                            Character_EndLoadTexturesCallbackIG);
            fn_8010BF68();
            if (gSession.nSplitScreen) {
                fn_8001D6D8(i);
            }
        }
    }
    fn_800253E0_ApplySavedChoices(0);
    fn_800253E0_ApplySavedChoices(1);
    fn_800CA9DC(-1);
    fn_800CA7E0();
    fn_80025478();
    Character_LoadSacFiles();
    AnimLib_FreeWorkCopies();
    fn_800CABA0();
    fn_800CB078();
}

// Builds a character from its 'CHR ' object at pData: a header (its animation slot, a skin value, a
// flag for bit 0x400 of u10, a model flag and five model values), its skin, the p44 entries, its
// model (SKEL_LoadFromMem; a golfer outside the front end gets the golfer model definitions), its
// own animation library (kept when its clips are its own or in a bank, else queued as an overlay of
// its slot), and its slider definitions. A golfer then gets club skin set nSet (lbl_80280E24), and
// with bLook its look from pChoices. nId: the golfer id (nC). nUnused: not read. NULL when no
// character could be made.
// port: the object is little-endian on disc and BYTESWAP_SWAPDATA swaps each value as it reads it:
// a little-endian port does not swap there.
Character* Character_CreateFromMem(u8* pData, int nUnused, int nSet, int nId, u8 bLook,
                                   SkinChoices* pChoices) {
    int nSize;
    int bLib;
    int nFlag400;
    int nModel;
    f32 fSkin;
    f32 f12C;
    f32 f130;
    f32 f134;
    f32 f138;
    f32 f13C;
    int nBank;
    u32 uLibFlags;
    u8* pPeek;
    Character* pChar;
    int i;
    CharModelDefs* pDefs = NULL;
    u8* pStart;
    u8* pCopy;
    AnimLib* pLib;
    Skin* pSkin;
    u8 bGolfer;
    int bModel;

    pChar = Character_Create();
    if (pChar == NULL) {
        return NULL;
    }
    pChar->nC = nId;
    pStart = pData;
    BYTESWAP_SWAPDATA(&pData, (u8*)&pChar->nSlot, 4, 4);
    BYTESWAP_SWAPDATA(&pData, (u8*)&fSkin, 4, 4);
    BYTESWAP_SWAPDATA(&pData, (u8*)&nFlag400, 4, 4);
    BYTESWAP_SWAPDATA(&pData, (u8*)&nModel, 4, 4);
    BYTESWAP_SWAPDATA(&pData, (u8*)&f134, 4, 4);
    BYTESWAP_SWAPDATA(&pData, (u8*)&f138, 4, 4);
    BYTESWAP_SWAPDATA(&pData, (u8*)&f13C, 4, 4);
    BYTESWAP_SWAPDATA(&pData, (u8*)&f12C, 4, 4);
    BYTESWAP_SWAPDATA(&pData, (u8*)&f130, 4, 4);
    if (gSession.nGameType == 3) {
        ClipBank_Restore(pChar->nSlot);
    }
    pData += 0xC;
    if (nFlag400 == 1) {
        pChar->u10 |= 0x400;
    }
    bGolfer = Character_IsGolfer(pChar);

    // its skin
    BYTESWAP_SWAPDATA(&pData, (u8*)&nSize, 4, 4);
    pData += 0xC;
    if (nSize == 0) {
        pChar->pSkin = NULL;
    } else {
        pChar->pSkin = fn_800377FC(pData, bLook);
        pChar->pSkin->f10D8 = fSkin;
        pChar->pSkin->f10DC = 1.0f;
        if (gSession.nGameType != 10 && gSession.nGameType != 3 && Character_IsGolfer(pChar)) {
            fn_80037708(pChar->pSkin);
        }
    }
    pData += nSize;

    // the p44 entries
    BYTESWAP_SWAPDATA(&pData, (u8*)&pChar->n40, 4, 4);
    pData += 0xC;
    pChar->p44 = StaticMem_Alloc(pChar->n40 * sizeof(CharEntry44), 2, 0x40, "char.c", 0xE04);
    for (i = 0; i < pChar->n40; i++) {
        BYTESWAP_SWAPDATA(&pData, (u8*)pChar->p44[i].v0, 0xC, 4);
        pChar->p44[i].fC = 1.0f;
        BYTESWAP_SWAPDATA(&pData, (u8*)&pChar->p44[i].a10[0], 4, 4);
        BYTESWAP_SWAPDATA(&pData, (u8*)&pChar->p44[i].a10[1], 4, 4);
        BYTESWAP_SWAPDATA(&pData, (u8*)&pChar->p44[i].a10[2], 4, 4);
        BYTESWAP_SWAPDATA(&pData, (u8*)&pChar->p44[i].a10[3], 4, 4);
        BYTESWAP_SWAPDATA(&pData, (u8*)&pChar->p44[i].a10[4], 4, 4);
        BYTESWAP_SWAPDATA(&pData, (u8*)&pChar->p44[i].a10[5], 4, 4);
        pData += 0xC;
    }

    // its model
    BYTESWAP_SWAPDATA(&pData, (u8*)&nSize, 4, 4);
    pData += 0xC;
    if (bGolfer && gSession.nGameType != 10 && gSession.nGameType != 3) {
        if (gSession.nSplitScreen) {
            pDefs = &lbl_80280E18;
        } else {
            pDefs = &lbl_80280E10;
        }
    }
    bModel = nModel == 1;
    if (bLook && pChoices != NULL) {
        bModel = (u8)pChoices->n113;
    }
    Character_SetSkeleton(pChar, SKEL_LoadFromMem(pData, 1, pDefs, bModel));
    if (pChar->pSkin != NULL) {
        Character_SetSkin(pChar, pChar->pSkin);
    }
    pChar->pModel->f12C = f12C;
    pChar->pModel->f130 = f130;
    pChar->pModel->f134 = f134;
    pChar->pModel->f138 = f138;
    pChar->pModel->f13C = f13C;
    pData += nSize;
    pChar->n3D4 = pData - pStart;

    // its own animation library: kept when its clips are its own or in a bank, otherwise merged over
    // its slot's library as an overlay
    BYTESWAP_SWAPDATA(&pData, (u8*)&bLib, 4, 4);
    pData += 0xC;
    if (bLib != 0) {
        BYTESWAP_SWAPDATA(&pData, (u8*)&nSize, 4, 4);
        pData += 0xC;
        pPeek = pData + 0x138;
        BYTESWAP_SWAPDATA(&pPeek, (u8*)&nBank, 4, 4);
        pPeek = pData + 0x13C;
        BYTESWAP_SWAPDATA(&pPeek, (u8*)&uLibFlags, 4, 4);
        if (pChar->nSlot == 2 || nBank != 0 || (uLibFlags & 1)) {
            pCopy = StaticMem_Alloc(nSize, 2, 0x40, "char.c", 0xE54);
        } else {
            pCopy = StaticMem_Alloc(nSize, 1, 0x40, "char.c", 0xE57);
        }
        Mem_cpy(pCopy, pData, nSize);
        pLib = AnimLib_Load(pCopy, ClipBank_Get(pChar->nSlot));
        if (pLib->pBank != NULL || (pLib->uFlags & 1)) {
            pChar->pLib = pLib;
        } else {
            lbl_801C6068[pChar->nSlot].overlays[lbl_801C6068[pChar->nSlot].nOverlays].pWork = pLib;
            lbl_801C6068[pChar->nSlot].overlays[lbl_801C6068[pChar->nSlot].nOverlays].pCopy =
                StaticMem_Alloc(nSize, 2, 0x40, "char.c", 0xE68);
            Mem_cpy(lbl_801C6068[pChar->nSlot].overlays[lbl_801C6068[pChar->nSlot].nOverlays].pCopy,
                    pData, nSize);
            lbl_801C6068[pChar->nSlot].overlays[lbl_801C6068[pChar->nSlot].nOverlays].nSize = nSize;
            lbl_801C6068[pChar->nSlot].overlays[lbl_801C6068[pChar->nSlot].nOverlays].pChar = pChar;
            lbl_801C6068[pChar->nSlot].overlays[lbl_801C6068[pChar->nSlot].nOverlays].n10 = nId + 3;
            lbl_801C6068[pChar->nSlot].overlays[lbl_801C6068[pChar->nSlot].nOverlays].bActive = bLook;
            lbl_801C6068[pChar->nSlot].nOverlays++;
            pChar->pLib = StaticMem_Alloc(0x2800, 2, 0x40, "char.c", 0xE75);
        }
        pData += nSize;
    } else {
        pChar->pLib = NULL;
    }

    pChar->p17AC = CharSlider_CreateDefinitionsFromMem(&pData);
    pChar->p4C = pData;
    Character_SetPreferedPos(pChar);
    if (bGolfer) {
        pChar->p16D8 = lbl_80280E24[nSet];
        pChar->nClubHeadBone = CharModel_GetBoneIndex(pChar->pModel, 0x53);
        pChar->f165C = 100.0f;
        pChar->f1660 = 200.0f;
    } else {
        pChar->nClubHeadBone = 0;
        pChar->f165C = 50.0f;
        pChar->f1660 = 100.0f;
    }
    if (pChar->p16D8 != NULL) {
        for (i = 0; i < 6; i++) {
            pSkin = pChar->p16D8->apSkins[i];
            if (pSkin != NULL) {
                fn_80037AB8(pSkin, pChar->pModel, CharModel_GetBoneIndex(pChar->pModel, 0x52) - 0x52, 0x52);
            }
        }
    }
    pChar->pChoices = pChoices;
    if (bLook) {
        fn_8001DC64(pChar, pChoices);
        SkinPart_BurnBodySkin(pChar);
    }
    if (gSession.nSplitScreen && Character_IsGolfer(pChar)) {
        fn_800375AC(pChar->pSkin, 0);
        Character_SetPreferedPos(pChar);
    }
    return pChar;
}

// Fills a club skin's record: n0 = n (its caller passes the record's size, 8) and the skin.
void CharSkinRef_Init(Skin* pSkin, CharSkinRef* pRef, s32 n) {
    pRef->n0 = n;
    pRef->pSkin = pSkin;
}

void CharSkinRef_Free(void* p) {
    StaticMem_Free(p);
}
// ---- end of sweep code ----

// Makes a club skin set from a 'CLB ' object: per entry its club class, that class's afC (the club
// head bone's height), the entry's size and, 4 bytes on, its skin (fn_800377FC) and a CharSkinRef
// for it; a3C gets the class's fixed club point (one for drivers and fairway woods, one each for
// putters, 3 irons, 7 irons and wedges). NULL when out of memory.
CharSkinSet* Character_CreateClubSkinSet(u8* pData) {
    CharSkinSet* pSet;
    s32 nSize;
    s32 nClass;
    int i;

    pSet = StaticMem_Alloc(sizeof(CharSkinSet), 2, 0x40, "char.c", 0xF15);
    if (pSet == NULL) {
        return NULL;
    }
    pSet->apSkins[0] = NULL;
    pSet->apSkins[1] = NULL;
    pSet->apSkins[2] = NULL;
    pSet->apSkins[3] = NULL;
    pSet->apSkins[4] = NULL;
    pSet->apSkins[5] = NULL;
    BYTESWAP_SWAPDATA(&pData, (u8*)&pSet->nCount, 4, 4);
    pData += 0xC;
    for (i = 0; i < pSet->nCount; i++) {
        BYTESWAP_SWAPDATA(&pData, (u8*)&nClass, 4, 4);
        BYTESWAP_SWAPDATA(&pData, (u8*)&pSet->afC[nClass], 4, 4);
        BYTESWAP_SWAPDATA(&pData, (u8*)&nSize, 4, 4);
        pData += 4;
        pSet->apSkins[nClass] = fn_800377FC(pData, 0);
        pSet->a9C[nClass] = StaticMem_Alloc(sizeof(CharSkinRef), 2, 0x40, "char.c", 0xF25);
        CharSkinRef_Init(pSet->apSkins[nClass], pSet->a9C[nClass], sizeof(CharSkinRef));
        if (nClass == 0 || nClass == 1) {
            pSet->a3C[nClass][0] = 0.065f;
            pSet->a3C[nClass][1] = 1.117f;
            pSet->a3C[nClass][2] = -0.013f;
            pSet->a3C[nClass][3] = 1.0f;
        } else if (nClass == 3) {
            pSet->a3C[nClass][0] = 0.087f;
            pSet->a3C[nClass][1] = 0.986f;
            pSet->a3C[nClass][2] = 0.02f;
            pSet->a3C[nClass][3] = 1.0f;
        } else if (nClass == 4) {
            pSet->a3C[nClass][0] = 0.089f;
            pSet->a3C[nClass][1] = 0.926f;
            pSet->a3C[nClass][2] = 0.017f;
            pSet->a3C[nClass][3] = 1.0f;
        } else if (nClass == 2) {
            pSet->a3C[nClass][0] = 0.115f;
            pSet->a3C[nClass][1] = 0.852f;
            pSet->a3C[nClass][2] = -0.015f;
            pSet->a3C[nClass][3] = 1.0f;
        } else if (nClass == 5) {
            pSet->a3C[nClass][0] = 0.099f;
            pSet->a3C[nClass][1] = 0.94f;
            pSet->a3C[nClass][2] = 0.031f;
            pSet->a3C[nClass][3] = 1.0f;
        } else {
            pSet->a3C[nClass][0] = 0.0f;
            pSet->a3C[nClass][1] = 0.0f;
            pSet->a3C[nClass][2] = 0.0f;
            pSet->a3C[nClass][3] = 1.0f;
        }
        pData += nSize;
    }
    pSet->n0 = 0;
    return pSet;
}

// Frees the club skin sets (lbl_80280E24): each one's skins and a9C blocks, then the set. pSet is
// not used: Legacy_Character_CloseModule passes the set it found, but both are freed here.
void Character_FreeClubSkinSets(CharSkinSet* pSet) {
    int j;
    int i;

    for (i = 0; i < 2; i++) {
        if (lbl_80280E24[i] != NULL) {
            for (j = 0; j < 6; j++) {
                if (lbl_80280E24[i]->apSkins[j] != NULL) {
                    fn_80037CD8(lbl_80280E24[i]->apSkins[j]);
                }
                if (lbl_80280E24[i]->a9C[j] != NULL) {
                    CharSkinRef_Free(lbl_80280E24[i]->a9C[j]);
                }
            }
            StaticMem_Free(lbl_80280E24[i]);
            lbl_80280E24[i] = NULL;
        }
    }
}

#define MIN(a, b) ((a) <= (b) ? (a) : (b))
#define MAX(a, b) ((a) <= (b) ? (b) : (a))

// The character's bounding box (vMin / vMax: its bones from bone 1 on, grown by 0.33 each way) and
// the sphere around it that Character_ClipTest tests (v1668 its centre, f1674 half its diagonal).
// Nothing for a model of fewer than two bones.
void Character_CalculateClipPoints(Character* pChar) {
    f32 vCentre[4];
    f32 vDiff[4];
    int i;

    if (pChar->pModel->nBones < 2) {
        return;
    }
    Vec3Copy(pChar->pModel->pMatrices[1][3], pChar->vMin);
    Vec3Copy(pChar->pModel->pMatrices[1][3], pChar->vMax);
    for (i = 2; i < pChar->pModel->nBones; i++) {
        pChar->vMin[0] = MIN(pChar->pModel->pMatrices[i][3][0], pChar->vMin[0]);
        pChar->vMin[1] = MIN(pChar->pModel->pMatrices[i][3][1], pChar->vMin[1]);
        pChar->vMin[2] = MIN(pChar->pModel->pMatrices[i][3][2], pChar->vMin[2]);
        pChar->vMax[0] = MAX(pChar->pModel->pMatrices[i][3][0], pChar->vMax[0]);
        pChar->vMax[1] = MAX(pChar->pModel->pMatrices[i][3][1], pChar->vMax[1]);
        pChar->vMax[2] = MAX(pChar->pModel->pMatrices[i][3][2], pChar->vMax[2]);
    }
    pChar->vMin[0] -= 0.33f;
    pChar->vMin[1] -= 0.33f;
    pChar->vMin[2] -= 0.33f;
    pChar->vMax[0] += 0.33f;
    pChar->vMax[1] += 0.33f;
    pChar->vMax[2] += 0.33f;
    Char_Vec3Add(pChar->vMin, pChar->vMax, vCentre);
    Vec3_Scale(0.5f, vCentre, vCentre);
    pChar->v1668[0] = vCentre[0];
    pChar->v1668[1] = vCentre[1];
    pChar->v1668[2] = vCentre[2];
    Char_Vec3Sub(pChar->vMax, pChar->vMin, vDiff);
    pChar->f1674 = (f32)Math_Sqrt(Vec3_LengthSqClamped(vDiff)) / 2.0f;
}

// Tests the character against the current camera. Only for the active player of the current view
// (nPlayer 1000: any character); for another player both answers are 2 (out of view). n1654 is
// fn_80007D74's answer for its bounding sphere (v1668, f1674; 2 = out of view) and n1658 the same
// for a 3-unit sphere (its shadow); each is 2 as well past its distance along the lens
// (fn_8001ED44, fn_8001EE00; halved in split screen). f14 keeps the nearest bone 1 has been to the
// camera; f1664 is 1 up close, fading to 0 between 6 and 15 units deep (scaled by the lens's field
// of view).
void Character_ClipTest(Character* pChar, int nPlayer) {
    f32 (*pMtx)[4];
    f32 fDepth;
    f32 fDist;
    f32 fLen;
    Sphere sphere;
    Vec4 vPos;
    f32 vDir[4];

    pMtx = fn_8001EE64(pChar);
    if (nPlayer != 1000 && nPlayer
        != ViewController_GetActivePlayerNumber(ViewController_GetCurrentViewControllerID())) {
        pChar->n1654 = pChar->n1658 = 2;
        return;
    }
    Vec3Copy(pChar->v1668, &vPos.x);
    vPos.w = 1.0f;
    LLMath_mat44fltMultiply(((Camera*)RC_spGetCurrentRenderCtx())->viewMtx, &vPos, &vPos);
    Vec3Copy(&vPos.x, &sphere.x);
    sphere.radius = pChar->f1674;
    fDepth = sphere.z;
    pChar->n1654 = fn_80007D74(&sphere, RC_spGetCurrentRenderCtx(), 0);
    sphere.radius = 3.0f;
    pChar->n1658 = fn_80007D74(&sphere, RC_spGetCurrentRenderCtx(), 0);
    fn_8001EFB4(pMtx[3], Camera_GetCurrentLens()->m4[3], vDir);
    fDist = Vec3_Dot(Camera_GetCurrentLens()->m4[2], vDir);
    fLen = (f32)Math_Sqrt(Vec3_LengthSqClamped(vDir));
    if (fLen < pChar->f14) {
        pChar->f14 = fLen;
    }
    if (fDist > fn_8001ED44(pChar, gSession.nSplitScreen)) {
        pChar->n1654 = 2;
    }
    if (fDist > fn_8001EE00(pChar, gSession.nSplitScreen)) {
        pChar->n1658 = 2;
    }
    fDepth *= Camera_GetLensFovScale(Camera_GetCurrentLens());
    if (pChar->n1654 == 2) {
        pChar->f1664 = 0.0f;
    } else if (fDepth > 15.0f) {
        pChar->f1664 = 0.0f;
    } else if (fDepth < 6.0f) {
        pChar->f1664 = 1.0f;
    } else {
        pChar->f1664 = 1.0f - (fDepth - 6.0f) / 9.0f;
    }
}

// Each frame before drawing: every character loses bit 0x1000 of u10, and one with bit 2 (the
// flagstick, GoDynObj.c) gets bit 1 (hidden) while the current view has the flag out. Then every
// character whose body or shadow is in view (n1654 or n1658 not 2), that is not the player
// fn_800636EC names, has none of bits 0x1000, 0x40 and 1 of u10 and is not posed yet (n1698 0) gets
// its skins posed on its model (fn_80035B40).
void Character_PreRenderAll(void) {
    int i;
    int nPlayer;
    u8 bDo;
    int bState;

    fn_80035600();
    fn_800364A0();
    for (i = 0; i < lbl_80281CA8; i++) {
        nPlayer = fn_800636EC();
        lbl_801B9624[i]->u10 &= ~0x1000;
        if (lbl_801B9624[i]->u10 & 2) {
            if (ViewController_GetCurrentViewController()->bFlagOut) {
                lbl_801B9624[i]->u10 |= 1;
            } else {
                lbl_801B9624[i]->u10 &= ~1;
            }
        }
        bState = fn_8001EE90(lbl_801B9624[i]) != 2;
        bDo = bState || fn_8001EE88(lbl_801B9624[i]) != 2;
        bDo = bDo && nPlayer != lbl_801B9624[i]->nPlayer;
        bDo = bDo && !(lbl_801B9624[i]->u10 & 0x1041);
        bDo = bDo != 0;     // fake match: the original turns bDo into 0/1 again (neg; or; srwi)
        if (bDo && lbl_801B9624[i]->n1698 == 0) {
            fn_80035B40(lbl_801B9624[i], 0);
        }
    }
}

// Draws every character (fn_800358E0 with uFlags) that is in view (n1654 not 2), is not the player
// fn_800636EC names, is neither hidden (bit 1 of u10) nor without its textures (bit 0x40), and,
// when uFlags has bit 4, is a golfer. fn_80035604 first; nothing when no character is made.
void Character_RenderAll(u32 uFlags) {
    int i;
    int nPlayer;

    if (lbl_80281CA8 != 0) {
        fn_80035604();
        for (i = 0; i < lbl_80281CA8; i++) {
            nPlayer = fn_800636EC();
            if (fn_8001EE90(lbl_801B9624[i]) != 2 && nPlayer != lbl_801B9624[i]->nPlayer &&
                !(lbl_801B9624[i]->u10 & 0x41) &&
                (Character_IsGolfer(lbl_801B9624[i]) || (uFlags & 4) == 0)) {
                fn_800358E0(lbl_801B9624[i], uFlags);
            }
        }
    }
}

// Advances every character's animation by fTime, except in game type 6 while GUI_IsPauseMenuOpen holds.
void Character_UpdateAll(f32 fTime) {
    int i;

    if (gSession.nGameType != 6 || !GUI_IsPauseMenuOpen()) {
        for (i = 0; i < lbl_80281CA8; i++) {
            Character_UpdateAnimation(lbl_801B9624[i], 0, fTime);
        }
    }
}

// Hangs the club from the hand or from the root, as the clip says. With clip flag 0x10 the club
// bone (nGripBone, bone 0x52) goes back to its parent, the right wrist (n16A8); otherwise it is
// parented to the root (bit 0x4000 of u10) and its rotation and offset from the root are kept in
// q16AC and v16BC (for a left-hander turned half round and mirrored in z). Returns 1 when the
// attachment changed, 0 when it already was that way.
int Character_UpdateClubAttachment(Character* pChar, Clip* pClip) {
    CharModel* pModel;
    f32 qRoot[4];
    f32 vOffset[4];
    f32 qGrip[4];
    f32 qTurn[4];

    if (pClip->uFlags & 0x10) {
        if (pChar->u10 & 0x4000) {
            pChar->u10 &= ~0x4000;
            pChar->pModel->pBones[pChar->nGripBone].nParent = pChar->n16A8;
            return 1;
        }
    } else if (!(pChar->u10 & 0x4000)) {
        pModel = pChar->pModel;
        pChar->u10 |= 0x4000;
        pChar->pModel->pBones[pChar->nGripBone].nParent = 0;
        Quat_Invert(pModel->pPoses[0].q0, qRoot);
        Quat_Multiply(pModel->pPoses[pChar->nGripBone].q0, qRoot, pChar->q16AC);
        if (Character_IsLeftHanded(pChar)) {
            Legacy_Quat_BuildFromPitch(PI, qTurn);
            Quat_Multiply(pChar->q16AC, qTurn, qGrip);
            Quat_Copy(qGrip, pChar->q16AC);
        }
        fn_8001EFB4(pModel->pPoses[pChar->nGripBone].v10, pModel->pPoses[0].v10, vOffset);
        vOffset[3] = 0.0f;
        Quat_RotateVector(qRoot, vOffset, pChar->v16BC);
        pChar->v16BC[3] = 0.0f;
        if (Character_IsLeftHanded(pChar)) {
            pChar->v16BC[2] = -pChar->v16BC[2];
        }
        return 1;
    }
    return 0;
}

// Plays pClip on the character (nothing for NULL); a golfer's club first follows the clip
// (Character_UpdateClubAttachment). With bNoBlend the blend tree and the animation player start
// over; otherwise the clip is blended in over its first f18 seconds from the current time plus
// fTime. Its SKA tags then start at the blend's start, and the clip's pF4 animation plays too
// (fn_8009622C).
void Character_PlayClip(Character* pChar, Clip* pClip, int bNoBlend, f32 fTime) {
    f32 aBlend[6];
    SKABlendNode* pNode;
    SKABlendNode* pNew;
    AnimPlayer* pAnim;
    f32 fLen;

    pNode = &pChar->blend;
    pNew = NULL;
    pAnim = (AnimPlayer*)pChar->anim;
    if (pClip == NULL) {
        return;
    }
    if (Character_IsGolfer(pChar)) {
        Character_UpdateClubAttachment(pChar, pClip);
    }
    pChar->p178C = NULL;
    if (bNoBlend) {
        fn_80071F58(&pNode, 0);
        fn_80071C28(&pNode, 1, 0, fn_80072ACC, 0);
        fn_800725BC(pNode, fn_80072ACC, 0.5f);
        Anim_SetRate((u8*)pAnim, 1.0f);
        pAnim->n00 = 0;
        pAnim->uFlags = 0;
        pAnim->n08 = -1;
        pAnim->fTime = 0.0f;
    }
    fn_80071C28(&pNew, 0, 0, fn_80072ACC, 1);
    fn_800724C0(&pChar->blend, pNew, pClip, 1.0f);
    if (!bNoBlend) {
        fn_800732F4(&pChar->blend, (AnimPlayer*)pChar->anim, pChar->fAnimTime + fTime);
        aBlend[5] = 0.0f;
        aBlend[0] = 0.0f;
        fLen = pClip->f18;
        aBlend[1] = fLen;
        aBlend[2] = -1.0f;
        if (fLen > aBlend[1]) {
            aBlend[1] = fLen;
        }
        aBlend[3] = pAnim->fTime + aBlend[5];
        aBlend[4] = aBlend[3] + (aBlend[1] - aBlend[0]);
        fn_800720C8(pChar, pNew, &pNode, aBlend, fn_80072ACC, 1);
    } else {
        fn_800720C8(pChar, pNew, &pNode, NULL, fn_80072ACC, 0);
        aBlend[3] = 0.0f;
    }
    pAnim->fStart = pNode->fStart;
    pAnim->fEnd = pNode->fEnd;
    Character_InitSKATags(pChar, pClip, aBlend[3]);
    if (pClip != NULL && pClip->pF4 != NULL) {
        fn_8009622C(pChar, pClip->pF4, bNoBlend, fTime);
    }
    pChar->pCurClip = pClip;
    fn_801141F8(pChar->pModel->pF0, pChar->pModel);
    fn_801141F8(pChar->pModel->pF4, pChar->pModel);
    fn_801141F8(pChar->pModel->pF8, pChar->pModel);
}

// Frees a character: its texture bank slot, both blend trees, its skin, library, model, buffers
// and the rest; in game type 3 its slot's clip bank is released too.
void Character_Free(Character* pChar) {
    SKABlendNode* pNode;
    int i;
    int nSlot;

    if (pChar != NULL) {
        nSlot = pChar->nSlot;
        if (pChar->n48 >= 0) {
            fn_80010544(pChar->n48);
        }
        pNode = &pChar->blend;
        fn_80071F58(&pNode, 0);
        pNode = &pChar->node3E0;  // a node without the root's nGroup
        fn_80071F58(&pNode, 0);
        if (pChar->pSkin != NULL) {
            fn_80037CD8(pChar->pSkin);
        }
        if (pChar->pLib != NULL) {
            AnimLib_Free(pChar->pLib);
        }
        SKEL_Free(pChar->pModel);
        pChar->pModel = NULL;
        for (i = 0; i < 4; i++) {
            StaticMem_Free(pChar->buffers[i].pBuf);
        }
        if (pChar->p44 != NULL) {
            StaticMem_Free(pChar->p44);
        }
        if (pChar->pRecords != NULL) {
            StaticMem_Free(pChar->pRecords);
        }
        if (Character_IsGolfer(pChar)) {
            Character_FreeTextures(pChar);
        }
        if (pChar->p17AC != NULL) {
            CharSlider_Free(pChar->p17AC);
        }
        StaticMem_Free(pChar);
        if (gSession.nGameType == 3) {
            ClipBank_Release(nSlot);
        }
    }
}

// Add a character to the table of characters (up to five); NULL when it is full.
Character* Character_Add(Character* pChar) {
    if (lbl_80281CA8 >= 5) {
        return NULL;
    }
    lbl_801B9624[lbl_80281CA8] = pChar;
    pChar->nIndex = lbl_80281CA8;
    lbl_80281CA8++;
    return pChar;
}

// Starts the characters for a round (GO_vInitIG): the dynamic texture pool, IK on, blend trees of
// up to six clips (four in split screen, lbl_80280E20), the animation stream, the skin parts (every
// copy changed together) and the skin meshes of each view (fn_80035C58).
void Character_InitIG(void) {
    int n;
    fn_8009555C();
    CharacterTex_Init();
    SKEL_EnableIK(1);
    n = 6;
    if (gSession.nSplitScreen) {
        n = 4;
    }
    lbl_80280E20 = n;
    fn_800C937C();
    SkinPart_Init();
    SkinPart_SetChangeAllCopies(1);
    fn_80035C58();
}

// Shuts down what Character_InitIG started: the skin meshes, the skin parts, the animation stream
// and the dynamic texture pool.
void Character_CloseIG(void) {
    fn_80035CC0();
    SkinPart_Shutdown();
    fn_800C9764();
    fn_80095560();
    CharacterTex_Close();
}

// The characters' step at the end of each hole (fn_80095564: outside split screen, every player's
// skin frees what loading it allocated).
void Character_ExitHole(void) {
    fn_80095564();
}

// Starts the characters for the front end (GO_vInitFE): the dynamic texture pool, IK off, blend
// trees of up to three clips (lbl_80280E20), the skin parts (each copy changed on its own) and the
// skinned-vertex buffer (fn_80112C64).
void Character_InitFE(void) {
    CharacterTex_Init();
    SKEL_EnableIK(0);
    lbl_80280E20 = 3;
    SkinPart_Init();
    SkinPart_SetChangeAllCopies(0);
    fn_80036460(1800);
    fn_80112C64(1);
}

// Shuts down what Character_InitFE started: the dynamic texture pool and the skin parts go, and
// fn_80036464 and fn_80112CEC free their buffers.
void Character_CloseFE(void) {
    CharacterTex_Close();
    SkinPart_Shutdown();
    fn_80036464();
    fn_80112CEC();
}

ViewSlot gViewSlots[5] = { 0 };

// Starts the character module (fn_8006C7A8, when the front end or a round starts): the animation
// libraries, the 'MAL ' banks and the skeleton module up, the blend tree pools made, no club skin
// sets, the club names read as 64-bit ids, no characters in the front end's or the players' slots,
// and no player marked (lbl_80281CAC).
// port: the names are read as big-endian 64-bit words from their strings (FEgolferanim compares
// them with ids read the same way)
void Legacy_Character_InitModule(void) {
    int i;

    Skalib_Init();
    fn_8001F64C();
    SKEL_InitModule();
    fn_80071AD0();
    for (i = 0; i < 2; i++) {
        lbl_80280E24[i] = NULL;
    }
    lbl_801B9638[0] = *(u64*)"IGdriver";
    lbl_801B9638[2] = *(u64*)"IGputter";
    lbl_801B9638[3] = *(u64*)"IGiron3";
    lbl_801B9638[4] = *(u64*)"IGiron7";
    lbl_801B9638[5] = *(u64*)"IGwedge";
    for (i = 0; i < CRAP_NUM_GOLFERS; i++) {
        lbl_80281EE8[i] = NULL;
    }
    for (i = 0; i < 5; i++) {
        gViewSlots[i].pChar = NULL;
    }
    lbl_80281CAC = -1;
}

// Shuts the character module down (fn_8006C854): frees the club skin sets and every character made,
// then shuts down the animation libraries, the 'MAL ' banks, the skeleton module and the blend tree
// pools.
void Legacy_Character_CloseModule(void) {
    int i;

    for (i = 0; i < 2; i++) {
        if (lbl_80280E24[i] != NULL) {
            Character_FreeClubSkinSets(lbl_80280E24[i]);
        }
        lbl_80280E24[i] = NULL;
    }
    for (i = 0; i < lbl_80281CA8; i++) {
        Character_Free(lbl_801B9624[i]);
        lbl_801B9624[i] = NULL;
    }
    lbl_80281CA8 = 0;
    Skalib_Shutdown();
    fn_8001F66C();
    SKEL_CloseModule();
    fn_80071B94();
}

// Frees the front end's golfer characters (lbl_80281EE8, one per CrAP golfer slot) and clears the
// slots. FEgolferanim.c only ever sets them to NULL, so there is nothing to free in practice.
void Character_FreeFEGolfers(void) {
    int i;

    for (i = 0; i < CRAP_NUM_GOLFERS; i++) {
        Character_Free(lbl_80281EE8[i]);
        lbl_80281EE8[i] = NULL;
    }
}

// The model id of the player's golfer.
int Character_GetGolferModelID(int nPlayer) {
    return gGolferTable[gSession.nGolfer[nPlayer]].nModelID;
}

// Whether the player's golfer is a created one (golfer table slots 30 to 33, FIRST_CREATED_GOLFER
// on).
int Character_IsCrAPGolfer(int nPlayer) {
    int b = 0;
    if (gSession.nGolfer[nPlayer] >= 30 && gSession.nGolfer[nPlayer] <= 33) {
        b = 1;
    }
    return b;
}

// Set the club class, and put the club head bone at the class's height.
void Character_SelectClub(Character* pChar, int n) {
    int nBone;

    if (pChar == NULL || pChar->pModel == NULL || pChar->nSlot < 0 || pChar->nSlot >= 3) {
        return;
    }
    nBone = CharModel_GetBoneIndex(pChar->pModel, 0x53);
    if (pChar->p16D8 != NULL) {
        pChar->nClubClass = n;
        pChar->pModel->pBones[nBone].v1C[1] = pChar->p16D8->afC[pChar->nClubClass];
    }
}

// ---- sweep code (not yet cleaned up) ----
void fn_8001C650(void* arg0, s32 arg1) {
    void* temp_r5;

    temp_r5 = (*(void**)((u8*)(arg0) + 0x1798));
    if ((temp_r5 != NULL) && ((u32) (*(u32*)((u8*)(temp_r5) + 0x2C)) == 6U) && (arg1 == 0)) {
        (*(s32*)((u8*)(arg0) + 0x16D4)) = 4;
    }
    (*(s32*)((u8*)(arg0) + 0x16D4)) = arg1;
}
// ---- end of sweep code ----

// The player's golfer takes the player's shot kind and club; when either changed, it goes back
// to animation 5.
void fn_8001C680(int nPlayer) {
    Player* pPlayer = &gPlayers[nPlayer];
    Character* pChar = pPlayer->pChar;
    int nKind = pChar->nShotKind;
    int nClub;

    Character_SelectGameShotType(pChar, pPlayer->nShotKind);
    nClub = pChar->nClub;
    Character_SelectGameClub(pChar, pPlayer->nClub);
    if (nClub != pPlayer->nClub || nKind != pPlayer->nShotKind) {
        pChar->nAnim = 0;
        pChar->u10 |= 0x80;
        fn_80095744(pChar, 5);
        Character_AlignShotWithTarget(nPlayer, 1, 1);
    }
}

s32 lbl_80187164[8] = { 8, 0, 6, 3, 2, 1, 10, 4 };

f32 lbl_80187184[6][3] = {
    { 0.058f, 0.0f, 0.025f },
    { 0.058f, 0.0f, 0.025f },
    { 0.045f, -0.024f, 0.05f },
    { 0.04f, 0.0f, 0.075f },
    { 0.04f, 0.0f, 0.075f },
    { 0.045f, -0.0f, 0.075f },
};

// Set the character's shot kind and the clip key that goes with it.
void Character_SelectGameShotType(Character* pChar, int nKind) {
    if (pChar != NULL) {
        fn_8001C650(pChar, lbl_80187164[nKind]);
        pChar->nShotKind = nKind;
    }
}

void Character_SelectGameClub(Character* pChar, int nClub) {
    // per club: what Character_SelectClub gets
    int aKind[26] = {0, 0, 0, 0, 0, 0, 1, 1, 1, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 2};

    if (pChar != NULL) {
        fn_800BBADC(nClub);
        pChar->nClub = nClub;
        Character_SelectClub(pChar, aKind[nClub]);
    }
}

// Sets the emotion (nStyle) the character's clips are picked with (Char_SetClip's style);
// CharacterState_UpdateGameEmotionState and CharacterState_SetTapInState set it.
void Character_SetEmotion(Character* pChar, int nStyle) {
    pChar->nStyle = nStyle;
}

// Flags on the player's character: bit 4 always, bit 8 set or cleared by b, bit 0x200 set by a.
void Character_AlignShotWithTarget(int nPlayer, u8 a, u8 b) {
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

// fake match: the index passed through an inline's parameter numbers the nPlayer read below the
// shared u10 read, which gives EA's registers.
static inline f32* fn_8001C860_Get(int nPlayer) { return gPlayers[nPlayer].ball.vPos; }

// Puts the golfer at its player's ball, facing the target (level), and resets its root bone's pose
// and matrix. With u10 bit 8 and a skeleton, it then takes the stance of its clip for the style:
// the clip is started on the skeleton when it changed (at its event 2's time with bit 0x10000), the
// root is moved by the club class's offset (mirrored when the model is), the pose updated, the
// feet placed and both leg chains moved with the root, and the IK weight set (1 in the swing's
// states 5 and 7). Bits 4, 8, 0x200 and 0x10000 of u10 are cleared; with 0x200 the ground under
// the feet is sampled again first.
void Character_SetupForShot(Character* pChar) {
    f32 vDir[4];
    f32 vOffsetX[4];
    f32 vOffsetZ[4];
    f32 vPos[4];
    int bStance;
    int bPlace;
    int bClipTime;
    CharModel* pModel;
    f32* pBallPos;
    Player* pPlayer;
    Skeleton* pSkel;
    Clip* pOldClip;
    Clip* pClip;
    f32 fY;

    pPlayer = &gPlayers[pChar->nPlayer];
    pModel = pChar->pModel;
    bPlace = pChar->u10 & 0x200;
    bStance = pChar->u10 & 8;
    bClipTime = pChar->u10 & 0x10000;
    pBallPos = fn_8001C860_Get(pChar->nPlayer);
    Character_SetPosition(pChar, pBallPos, 0);
    pChar->u10 &= ~(0x10000 | 0x200 | 8 | 4);
    // fake match: n2C is compared unsigned here
    if (pChar->p1798 != NULL && (u32)pChar->p1798->n2C == 6 && pChar->n16D4 == 0) {
        pChar->n16D4 = 4;
    }
    fn_8001EFB4(pPlayer->vTarget, pBallPos, vDir);
    vDir[1] = 0.0f;
    Character_SetOrientationVec(pChar, vDir, 0.0f);
    Quat_Copy(pModel->pBones[0].q0C, pModel->pPoses[0].q0);
    Quat_Copy(pModel->pBones[0].v1C, pModel->pPoses[0].v10);
    Quat_QuatToMatrix(pModel->pPoses[0].q0, pModel->pMatrices[0]);
    Vec4_CopyPoint(pModel->pPoses[0].v10, pModel->pMatrices[0][3]);
    if (bStance && (pSkel = pChar->pModel->pSkel) != NULL) {
        pOldClip = pChar->pCurClip;
        CharModel_GetBoneIndex(pChar->pModel, 1);      // the results are not used
        CharModel_GetBoneIndex(pChar->pModel, 0x52);
        pClip = Char_SetClip(pChar, 0, pChar->nStyle, NULL);
        if (pClip->pD8 == NULL) {
            fn_80027108(pSkel);
            SKEL_SetIKSolutionWeight(pChar->pModel->pSkel, 0.0f);
            pChar->pModel->pSkel->f1074 = 0.0f;
            return;
        }
        if (pSkel->pClip != pClip) {
            pSkel->pClip = pClip;
            fn_80021978(pChar->pModel->bEE);
            if (bClipTime) {
                fn_8001FCF4(pChar, pClip, &pSkel->pose, 0, pClip->pEvents[2].fTime);
            } else {
                fn_8001FCF4(pChar, pClip, &pSkel->pose, 0, 0.0f);
            }
        }
        Character_UpdateClubAttachment(pChar, pSkel->pClip);
        LLMath_Scale(-lbl_80187184[pChar->nClubClass][0], pChar->pModel->pMatrices[0][0], vOffsetX);
        LLMath_Scale(-lbl_80187184[pChar->nClubClass][2], pChar->pModel->pMatrices[0][2], vOffsetZ);
        if (pChar->pModel->bEE) {
            vOffsetZ[0] = -vOffsetZ[0];
            vOffsetZ[2] = -vOffsetZ[2];
        }
        fn_8001EFD8(vOffsetX, pChar->pModel->pBones[0].v1C, pChar->pModel->pBones[0].v1C);
        fn_8001EFD8(vOffsetZ, pChar->pModel->pBones[0].v1C, pChar->pModel->pBones[0].v1C);
        Quat_Copy(pModel->pBones[0].v1C, pModel->pPoses[0].v10);
        Vec4_CopyPoint(pModel->pPoses[0].v10, pModel->pMatrices[0][3]);
        fn_80027108(pSkel);
        if (pChar->p16D8 != NULL) {
            pSkel->pose.aBones[pChar->nClubHeadBone].v10[1] = pChar->p16D8->afC[pChar->nClubClass];
        }
        SKEL_UpdateState(pChar->pModel, &pSkel->pose, 1);
        Character_UpdateTestPoints(pChar);
        if (bPlace) {
            pChar->n1784 = -1;
            Character_UpdateFeetTerrainInfo(pChar, 1);
        }
        fY = pChar->pModel->pBones[0].v1C[1];
        Character_PlaceFeetOnGround(pChar);
        fY = pChar->pModel->pBones[0].v1C[1] - fY;
        SKEL_TranslateIKChainY(pChar->pModel, &pChar->pModel->pSkel->pChains[0], fY);
        SKEL_TranslateIKChainY(pChar->pModel, &pChar->pModel->pSkel->pChains[1], fY);
        fn_8001EFD8(gPlayers[pChar->nPlayer].ball.vPos, vOffsetX, vPos);
        fn_8001EFD8(vPos, vOffsetZ, vPos);
        vPos[1] += lbl_80187184[pChar->nClubClass][1];
        fn_800280E8(pChar, vPos, bPlace);
        pChar->pModel->pSkel->n1130 = pChar->n16D4;
        pChar->pModel->pSkel->n112C = pChar->nClubClass;
        if (((pChar->n20 == 5 || pChar->nAnim == 5) && fn_8009637C(pChar)) || pChar->n20 == 7) {
            SKEL_SetIKSolutionWeight(pChar->pModel->pSkel, 1.0f);
        } else {
            SKEL_SetIKSolutionWeight(pChar->pModel->pSkel, 0.0f);
        }
        if (pOldClip != NULL) {
            Character_UpdateClubAttachment(pChar, pOldClip);
        }
    }
    if (pChar->pModel->pSkel != NULL) {
        pChar->pModel->pSkel->f1074 = 0.0f;
    }
}

// The 'CLB ' handlers: what Character_CreateClubSkinSet makes of the object is kept unless there
// already is one; the first handler makes a second one for split screen.
void fn_8001CCF8(UStreamObject* pObject) {
    if (lbl_80280E24[0] == NULL) {
        if (gSession.nSplitScreen) {
            lbl_80280E24[0] = Character_CreateClubSkinSet(pObject->pData);
            lbl_80280E24[1] = Character_CreateClubSkinSet(pObject->pData);
        } else {
            lbl_80280E24[0] = Character_CreateClubSkinSet(pObject->pData);
            lbl_80280E24[1] = NULL;
        }
    }
    StaticMem_Free(pObject);
}

void fn_8001CD80(UStreamObject* pObject) {
    if (lbl_80280E24[0] == NULL) {
        lbl_80280E24[0] = Character_CreateClubSkinSet(pObject->pData);
        lbl_80280E24[1] = NULL;
    }
    StaticMem_Free(pObject);
}

// The 'CLB ' stream objects: two handlers for the same type.
void fn_8001CDD4(void) {
    Stream_RegisterLoadChunkCallback('CLB ', fn_8001CCF8);
}

void fn_8001CE04(void) {
    Stream_RegisterLoadChunkCallback('CLB ', fn_8001CD80);
}

void fn_8001CE34(void) {
    Stream_UnregisterLoadChunkCallback('CLB ');
}

// A 'CHR ' object: a character for every player whose golfer has this model and who has none yet
// (in split screen with the player's own set), dressed from the player's profile, with its body
// skin and the club skin set's six skins put on the model.
void fn_8001CE5C(UStreamObject* pObject) {
    u32 uModel = pObject->uId;
    int i;
    int nSet;
    int nGolferModel;
    Character* pChar;

    fn_800106B8(1);
    for (i = 0; i < gSession.nNumPlayers; i++) {
        nGolferModel = Character_GetGolferModelID(i);
        if (nGolferModel == uModel && gViewSlots[i].pChar == NULL) {
            fn_800106AC(i);
            nSet = gSession.nSplitScreen ? i : 0;
            gViewSlots[i].pChar = Character_Add(Character_CreateFromMem(pObject->pData, 0, nSet, uModel,
                                                          Character_IsCrAPGolfer(i), &gpSaveData[i].choices));
            if (gViewSlots[i].pChar->pSkin != NULL && Character_IsGolfer(gViewSlots[i].pChar)) {
                Character_SetClubsAndClothes(gViewSlots[i].pChar, i);
            }
            if (gViewSlots[i].pChar->pSkin != NULL && Character_IsGolfer(gViewSlots[i].pChar)) {
                pChar = gViewSlots[i].pChar;
                pChar->apSkins[0] = pChar->pSkin;
                pChar->apSkins[1] = pChar->p16D8->apSkins[0];
                pChar->apSkins[2] = pChar->p16D8->apSkins[1];
                pChar->apSkins[3] = pChar->p16D8->apSkins[2];
                pChar->apSkins[4] = pChar->p16D8->apSkins[3];
                pChar->apSkins[5] = pChar->p16D8->apSkins[4];
                pChar->apSkins[6] = pChar->p16D8->apSkins[5];
                pChar->nSkins = 7;
                Character_LoadTextures(pChar, pChar->apSkins, pChar->nSkins);
            }
        }
    }
    fn_800106B8(0);
    StaticMem_Free(pObject);
}

// The 'CHR ' stream objects: two handlers for the same type.
void fn_8001CFF0(void) {
    Stream_RegisterLoadChunkCallback('CHR ', fn_8001CE5C);
}

// A 'CHR ' object for the golfer the menu is loading (lbl_80281EE0->pB8): the UI file is parked in
// ARAM and its buffer takes a copy of the object (header and data), from which the character is
// made while the static heap counts what it takes. The character is placed at the origin facing
// f19C, dressed as the profile's created golfer for golfers 7 and 29, set to club class 5 and its
// first clip. When it is still the golfer the menu wants, its body and club skins go on the model
// (created golfers with one pool entry take two) and it takes its pool entries.
void fn_8001D020(UStreamObject* pObject) {
    UStreamObject* pCopy;
    Character* pChar;
    Clip* pClip;

    StaticMem_ResetCount();
    StaticMem_StartCount();
    fn_8008F310();
    pCopy = fn_8008F354();
    Mem_cpy(pCopy, pObject, pObject->uSize + 0x80);
    StaticMem_Free(pObject);
    pCopy->pData = (u8*)pCopy + 0x80;
    lbl_80281EE0->pB8->pChar = Character_CreateFromMem(pCopy->pData, 0, 0, pCopy->uId, 0, NULL);
    StaticMem_StopCount();
    StaticMem_GetCount();
    Character_LoadTextures(lbl_80281EE0->pB8->pChar, NULL, 0);
    fn_8008F35C();
    lbl_80281EE0->pB8->pChar->n16C = -1;
    Character_SetPosition(lbl_80281EE0->pB8->pChar, lbl_80189A30, 1);
    Character_SetOrientation(lbl_80281EE0->pB8->pChar, lbl_80281EE0->f19C);
    if (lbl_80281EE0->pB8->pChar->nC == 7 || lbl_80281EE0->pB8->pChar->nC == 29) {
        fn_8001DC64(lbl_80281EE0->pB8->pChar, &FE_GetCurrentProfile()->choices);
    }
    Character_SelectClub(lbl_80281EE0->pB8->pChar, 5);
    pClip = Char_SetClip(lbl_80281EE0->pB8->pChar, 0, 0, NULL);
    Character_PlayClip(lbl_80281EE0->pB8->pChar, pClip, 1, 0.0f);
    if (lbl_80281EE0->pB8->nC == lbl_80281EE0->n8C) {
        pChar = lbl_80281EE0->pB8->pChar;
        pChar->nSkins = 7;
        pChar->apSkins[0] = pChar->pSkin;
        pChar->apSkins[1] = pChar->p16D8->apSkins[0];
        pChar->apSkins[2] = pChar->p16D8->apSkins[1];
        pChar->apSkins[3] = pChar->p16D8->apSkins[2];
        pChar->apSkins[4] = pChar->p16D8->apSkins[3];
        pChar->apSkins[5] = pChar->p16D8->apSkins[4];
        pChar->apSkins[6] = pChar->p16D8->apSkins[5];
        if ((pChar->nC == 7 || pChar->nC == 29) && pChar->n70 == 1) {
            pChar->n70 = 2;
        }
        CharacterTex_TakePoolEntries(pChar);
    }
}

void fn_8001D238(void) {
    Stream_RegisterLoadChunkCallback('CHR ', fn_8001D020);
}

void fn_8001D268(void) {
    Stream_UnregisterLoadChunkCallback('CHR ');
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

// Run Character_ClipTest on every character with no player (the 'SKLO' ones).
void fn_8001D384(void) {
    int i;
    for (i = 0; i < lbl_80281CA8; i++) {
        if (lbl_801B9624[i]->nPlayer == 1000) {
            Character_ClipTest(lbl_801B9624[i], 1000);
        }
    }
}

// The 'SKLO' handler: a character built from the object with no player (1000), keyed by the
// object's id.
// port: the skeleton is little-endian on disc and Character_CreateFromMem swaps it (BYTESWAP_SWAPDATA): a
//       little-endian port does not swap there.
void fn_8001D3EC(UStreamObject* pObject) {
    Character* pChar = Character_Add(Character_CreateFromMem(pObject->pData, 0, 0, pObject->uId, 0, NULL));
    pChar->nPlayer = 1000;
    pChar->uId     = pObject->uId;
    StaticMem_Free(pObject);
}

void fn_8001D44C(void) {
    Stream_RegisterLoadChunkCallback('SKLO', fn_8001D3EC);
}

void fn_8001D47C(void) {
    Stream_UnregisterLoadChunkCallback('SKLO');
}

// Dresses the character of player slot nSlot: its club skins (in game type 3 golfers 7 and 29 get
// the profile's created golfer's look; otherwise its own look when Character_IsCrAPGolfer says so), then the
// "shirt" set (only when Character_IsCrAPGolfer says no) and the "glove" set, as "shirt<n>" / "glove<n>" with
// n from the slot's profile (no number when it is 0 or less).
void Character_SetClubsAndClothes(Character* pChar, int nSlot) {
    char szName[32];            // the size is not known

    if (gSession.nGameType == 3) {
        if (pChar->nC == 7 || pChar->nC == 29) {
            Character_SetClubStatesForCharacter(pChar, nSlot, &FE_GetCurrentProfile()->choices);
        } else {
            Character_SetClubStatesForCharacter(pChar, nSlot, NULL);
        }
    } else if (Character_IsCrAPGolfer(nSlot)) {
        Character_SetClubStatesForCharacter(pChar, nSlot, pChar->pChoices);
    } else {
        Character_SetClubStatesForCharacter(pChar, nSlot, NULL);
    }
    if (!Character_IsCrAPGolfer(nSlot)) {
        sprintf(szName, "%s", "shirt");
        if (gSession.aProfile[nSlot].n0 > 0) {
            sprintf(szName, "%s%d", szName, gSession.aProfile[nSlot].n0);
        }
        SkinPart_ChooseBodySetByName(pChar, "shirt", szName, NULL);
    }
    sprintf(szName, "%s", "glove");
    if (gSession.aProfile[nSlot].n2 > 0) {
        sprintf(szName, "%s%d", szName, gSession.aProfile[nSlot].n2);
    }
    SkinPart_ChooseBodySetByName(pChar, "glove", szName, NULL);
}

void fn_8001D624(int n) {
    gSession.aD2D[n] = 1;
}

// Each index set by fn_8001D624 is taken once the CrAP camera's golfer runs its script, the front
// end is not in state 4 and fn_8008E924 agrees: the front end is aborted into state 4.
void fn_8001D63C(void) {
    int i;

    for (i = 0; i < 5; i++) {
        if (gSession.aD2D[i] && lbl_80281EE0->pB4 != NULL && lbl_80281EE0->pB4->b18 &&
            fn_8008B990() != 4 && fn_8008E924()) {
            gSession.aD2D[i] = 0;
            fn_8008B704();
            fn_8008B754(4);
        }
    }
}

void fn_8001D6D8(int n) {
    gSession.aD28[n] = 1;
}

// Every player flagged by fn_8001D6D8 has its character dressed again (Character_SetClubsAndClothes) and its
// skins put on its model in use; the flag is cleared.
void fn_8001D6F0(void) {
    int j;
    Character* pChar;
    int i;

    for (i = 0; i < 5; i++) {
        if (gSession.aD28[i]) {
            fn_80008380();
            pChar = gPlayers[i].pChar;
            Character_SetClubsAndClothes(pChar, i);
            Character_CopySkinChoices1To0(pChar);
            for (j = 0; j < pChar->nSkins; j++) {
                fn_800CE170(pChar->apSkins[j], pChar->a64[pChar->n74]);
            }
            gSession.aD28[i] = 0;
        }
    }
}

void fn_8001D7A4(Character* pChar) {
    fn_8001D7EC(pChar);
    pChar->u10 = pChar->u10 & ~0x20C;
    pChar->u10 = pChar->u10 | 0x40;
}

// Resets the character's animation: both blend trees are given back and rebuilt as one node
// blending with fn_80072ACC (fn_800725BC, 0.5), both animation players are reset and the state
// cleared.
void fn_8001D7EC(Character* pChar) {
    SKABlendNode* pNode;

    pNode = &pChar->blend;
    fn_80071F58(&pNode, 0);
    fn_80071C28(&pNode, 1, 0, fn_80072ACC, 0);
    fn_800725BC(pNode, fn_80072ACC, 0.5f);
    pNode = &pChar->node3E0;
    fn_80071F58(&pNode, 0);
    fn_80071C28(&pNode, 1, 1, fn_80072ACC, 1);
    fn_800725BC(pNode, fn_80072ACC, 0.5f);
    fn_800958EC(&pChar->anim29C, 0, 0.0f);
    pChar->n2C = 0;
    pChar->n30 = 0;
    fn_800958EC((AnimPlayer*)pChar->anim, 0, 0.0f);
    pChar->nAnim = 0;
    pChar->n20 = 0;
    pChar->n18 = 0;
    pChar->u28 = 0;
}

void fn_8001D8DC(int nPlayer) {
    gPlayers[nPlayer].pChar->u10 &= ~0x40;
    fn_800955F0(nPlayer);
    CharacterTex_StartStreamingPlayers(nPlayer);
    if (!gSession.nSplitScreen && lbl_80281CAC != nPlayer) {
        fn_8001D6D8(nPlayer);
        lbl_80281CAC = nPlayer;
    }
}

// Where the ball sits on the hand: bone 0x1A's position, moved 0.05 along the bone's x axis (the
// other way while the model's bEE is set).
void Character_GetBallOnFingerPosition(Character* pChar, f32* pPos) {
    f32 (*pMtx)[4] = Character_GetBoneMatrixSwapIfLefty(pChar, 0x1A);
    f32 vAxis[3];

    LLMath_CopyVec(pMtx[3], pPos);
    Vec3Copy(pMtx[0], vAxis);
    LLMath_Normalize3(vAxis, vAxis);
    if (Character_IsLeftHanded(pChar)) {
        fn_8000C5D4(pPos, vAxis, 0.05f, pPos);
    } else {
        fn_8000C5D4(pPos, vAxis, -0.05f, pPos);
    }
}

// A point at the hand and three angles: bone 0x1A's position moved 0.000625 along the bone's x
// axis (the other way while the model's bEE is set), and bone 0x15's pose as three angles
// (Quat_ExtractEulerAngles), the second 30 degrees more.
void fn_8001DA04(Character* pChar, f32* pPos, f32* pAngles) {
    f32 (*pMtx)[4];
    f32 vAxis[3];

    if (pChar != NULL && pChar->pModel != NULL) {
        pMtx = Character_GetBoneMatrixSwapIfLefty(pChar, 0x1A);
        LLMath_CopyVec(pMtx[3], pPos);
        Vec3Copy(pMtx[0], vAxis);
        LLMath_Normalize3(vAxis, vAxis);
        if (Character_IsLeftHanded(pChar)) {
            fn_8000C5D4(pPos, vAxis, -0.000625f, pPos);
        } else {
            fn_8000C5D4(pPos, vAxis, 0.000625f, pPos);
        }
        Quat_ExtractEulerAngles(pChar->pModel->pPoses[CharModel_GetBoneIndexMapped(pChar->pModel, 0x15)].q0,
                                &pAngles[0], &pAngles[1],
                    &pAngles[2]);
        pAngles[1] += 30.0f / 180.0f * PI;
    }
}

// The clip's point v80 through bone 0's matrix (Character_GetBoneMatrix) into pOut; without a clip, bone 0's
// position (Character_GetBonePos).
void fn_8001DB04(Character* pChar, f32* pOut) {
    Vec4 vPos;
    f32 (*pMtx)[4];

    if (pChar->pCurClip != NULL) {
        CharModel_GetBoneIndex(pChar->pModel, 1);  // the result is not used
        pMtx = Character_GetBoneMatrix(pChar, 0);
        Vec3Copy(pChar->pCurClip->v80, &vPos.x);
        vPos.w = 1.0f;
        LLMath_mat44fltMultiply(pMtx, &vPos, (Vec4*)pOut);
        return;
    }
    Character_GetBonePos(pChar, 0, pOut);
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

// 1 when there is a current clip, the model has bone 0x54 and the clip's n1C is above that bone's
// index (Swing.c then puts the ball on bone 0x54).
u8 fn_8001DBF4(Character* pChar) {
    if (pChar->pCurClip != NULL && CharModel_GetBoneIndex(pChar->pModel, 0x54) != 0xFF &&
        pChar->pCurClip->n1C > CharModel_GetBoneIndex(pChar->pModel, 0x54)) {
        return 1;
    }
    return 0;
}

// Gives the character the look in pChoices: its skins' choices, then its sliders (the 26 values
// at a9B4). Outside the menu golfer's game type 3 (or on its screens 1 and 4) n113 sets the
// model's bEE.
void fn_8001DC64(Character* pChar, SkinChoices* pChoices) {
    SkinPart_ApplyBodyChoices(pChar, pChoices);
    CharSlider_UpdateCharacterBasedOnSliderValues(pChar->p17AC, pChar->pModel, pChar->pSkin, 26, pChoices->a9B4,
                &pChar->node3E0);
    if (gSession.nGameType != 3 || lbl_80281EE0->n0 == 1 || lbl_80281EE0->n0 == 4) {
        if (pChoices->n113 == 0) {
            fn_8001EE98(pChar, 0);
        } else {
            fn_8001EE98(pChar, 1);
        }
    }
    Character_SetSkeleton(pChar, pChar->pModel);
}

// Byte-swaps nBytes of 0x50-byte texture entries in place, once (bit 0x40 of b47 marks it done):
// from 0x08 four 12-byte records (a 4-byte field, four 2-byte ones), then four 2-byte fields,
// nine bytes and seven bytes. An entry with b40 0 has its name decoded (not used) and goes with
// the entry before when that one has the same name and goes with its next.
void fn_8001DD18(u8* pData, int nBytes) {
    SwapField aRecord[5] = { { 4, 4 }, { 2, 2 }, { 2, 2 }, { 2, 2 }, { 2, 2 } };
    SwapField aTail[14] = { { 2, 2 }, { 2, 2 }, { 2, 2 }, { 2, 2 }, { 1, 1 }, { 1, 1 }, { 1, 1 },
                            { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 7, 7 } };
    char szName[64];            // the size is not known
    void* pSrc;
    void* pDst;
    TexEntry* pEntry;
    int nEntries;
    int i;
    int j;

    if (!(((TexEntry*)pData)->b47 & 0x40)) {
        pEntry = (TexEntry*)pData;
        nEntries = nBytes / (int)sizeof(TexEntry);
        for (i = 0; i < nEntries; i++) {
            pDst = pEntry;
            pSrc = pEntry;
            pData = (u8*)pEntry->aMips;
            for (j = 0; j < 4; j++) {
                pDst = pData;
                pSrc = pData;
                ByteSwap_Records(&pSrc, &pDst, aRecord, 5, 1);
                pData += 12;
            }
            pDst = pData;
            pSrc = pData;
            ByteSwap_Records(&pSrc, &pDst, aTail, 14, 1);
            if (pEntry->b40 == 0) {
                fn_800CB868(&pEntry->u0, szName);
                if (i != 0 && pEntry->u0 == pEntry[-1].u0 && (pEntry[-1].b47 & 1)) {
                    pEntry->b47 |= 1;
                }
            }
            pEntry->b47 |= 0x40;
            pEntry++;
        }
    }
}

// Byte-swaps nBytes of 12-byte records in place: a 4-byte field, then four 2-byte ones.
void fn_8001DEC8(u8* pData, int nBytes) {
    SwapField aFormat[5] = { { 4, 4 }, { 2, 2 }, { 2, 2 }, { 2, 2 }, { 2, 2 } };
    void* pSrc;
    void* pDst;
    int i;

    for (i = 0; i < nBytes / 12; i++) {
        pDst = pData;
        pSrc = pData;
        ByteSwap_Records(&pSrc, &pDst, aFormat, 5, 1);
        pData += 12;
    }
}

// Dresses the character's six club skins: from pChoices when it is given, else from the golfer's
// gGolferTable row (golfer 7's own row in the 0x4000 session mode). The front end's golfer is
// left until fn_8008EAB0 allows it.
void Character_SetClubStatesForCharacter(Character* pChar, int nSlot, SkinChoices* pChoices) {
    int nGolfer;
    u64 uName;
    u64 uVariant;

    if (pChar == NULL || pChar->p16D8 == NULL) return;
    if (gSession.nGameType == 3 && !fn_8008EAB0()) {
        fn_8008EABC(1);
        return;
    }
    if (pChoices == NULL) {
        nGolfer = Golfer_FindById(pChar->nC);
        if ((gSession.uFlags & 0x4000) && pChar->nC == 7) {
            nGolfer = 7;
        }
        if (nGolfer >= 0) {
            SKA_PackName(&uName, lbl_80186EC0[0]);
            SkinPart_ChooseClubPartVariant(pChar, 0, uName, gGolferTable[nGolfer].aClubs[0].uPart);
            SKA_PackName(&uName, lbl_80186FB0[0]);
            SKA_PackName(&uVariant, lbl_80187000[0]);
            SkinPart_ChooseClubSet(pChar, 0, uName, uVariant, gGolferTable[nGolfer].aClubs[0].uModel);
            SKA_PackName(&uName, lbl_80186F10[0]);
            SKA_PackName(&uVariant, lbl_80186F60[0]);
            SkinPart_ChooseClubSet(pChar, 0, uName, uVariant, gGolferTable[nGolfer].aClubs[0].uShaft);
            SKA_PackName(&uName, lbl_80187050[0]);
            SKA_PackName(&uVariant, lbl_801870A0[0]);
            SkinPart_ChooseClubSet(pChar, 0, uName, uVariant, gGolferTable[nGolfer].aClubs[0].uGrip);

            SKA_PackName(&uName, lbl_80186EC0[1]);
            SkinPart_ChooseClubPartVariant(pChar, 1, uName, gGolferTable[nGolfer].aClubs[1].uPart);
            SKA_PackName(&uName, lbl_80186FB0[1]);
            SKA_PackName(&uVariant, lbl_80187000[1]);
            SkinPart_ChooseClubSet(pChar, 1, uName, uVariant, gGolferTable[nGolfer].aClubs[1].uModel);
            SKA_PackName(&uName, lbl_80186F10[1]);
            SKA_PackName(&uVariant, lbl_80186F60[1]);
            SkinPart_ChooseClubSet(pChar, 1, uName, uVariant, gGolferTable[nGolfer].aClubs[1].uShaft);
            SKA_PackName(&uName, lbl_80187050[1]);
            SKA_PackName(&uVariant, lbl_801870A0[1]);
            SkinPart_ChooseClubSet(pChar, 1, uName, uVariant, gGolferTable[nGolfer].aClubs[1].uGrip);

            SKA_PackName(&uName, lbl_80186EC0[3]);
            SkinPart_ChooseClubPartVariant(pChar, 3, uName, gGolferTable[nGolfer].aIronPart[0]);
            SKA_PackName(&uName, lbl_80186FB0[3]);
            SKA_PackName(&uVariant, lbl_80187000[3]);
            SkinPart_ChooseClubSet(pChar, 3, uName, uVariant, gGolferTable[nGolfer].uIronModel);
            SKA_PackName(&uName, lbl_80186F10[3]);
            SKA_PackName(&uVariant, lbl_80186F60[3]);
            SkinPart_ChooseClubSet(pChar, 3, uName, uVariant, gGolferTable[nGolfer].uIronShaft);
            SKA_PackName(&uName, lbl_80187050[3]);
            SKA_PackName(&uVariant, lbl_801870A0[3]);
            SkinPart_ChooseClubSet(pChar, 3, uName, uVariant, gGolferTable[nGolfer].uIronGrip);

            SKA_PackName(&uName, lbl_80186EC0[4]);
            SkinPart_ChooseClubPartVariant(pChar, 4, uName, gGolferTable[nGolfer].aIronPart[1]);
            SKA_PackName(&uName, lbl_80186FB0[4]);
            SKA_PackName(&uVariant, lbl_80187000[4]);
            SkinPart_ChooseClubSet(pChar, 4, uName, uVariant, gGolferTable[nGolfer].uIronModel);
            SKA_PackName(&uName, lbl_80186F10[4]);
            SKA_PackName(&uVariant, lbl_80186F60[4]);
            SkinPart_ChooseClubSet(pChar, 4, uName, uVariant, gGolferTable[nGolfer].uIronShaft);
            SKA_PackName(&uName, lbl_80187050[4]);
            SKA_PackName(&uVariant, lbl_801870A0[4]);
            SkinPart_ChooseClubSet(pChar, 4, uName, uVariant, gGolferTable[nGolfer].uIronGrip);

            SKA_PackName(&uName, lbl_80186EC0[5]);
            SkinPart_ChooseClubPartVariant(pChar, 5, uName, gGolferTable[nGolfer].wedges.uPart);
            SKA_PackName(&uName, lbl_80186FB0[5]);
            SKA_PackName(&uVariant, lbl_80187000[5]);
            SkinPart_ChooseClubSet(pChar, 5, uName, uVariant, gGolferTable[nGolfer].wedges.uModel);
            SKA_PackName(&uName, lbl_80186F10[5]);
            SKA_PackName(&uVariant, lbl_80186F60[5]);
            SkinPart_ChooseClubSet(pChar, 5, uName, uVariant, gGolferTable[nGolfer].wedges.uShaft);
            SKA_PackName(&uName, lbl_80187050[5]);
            SKA_PackName(&uVariant, lbl_801870A0[5]);
            SkinPart_ChooseClubSet(pChar, 5, uName, uVariant, gGolferTable[nGolfer].wedges.uGrip);

            SKA_PackName(&uName, lbl_80186EC0[2]);
            SkinPart_ChooseClubPartVariant(pChar, 2, uName, gGolferTable[nGolfer].aClubs[2].uPart);
            SKA_PackName(&uName, lbl_80186FB0[2]);
            SKA_PackName(&uVariant, lbl_80187000[2]);
            SkinPart_ChooseClubSet(pChar, 2, uName, uVariant, gGolferTable[nGolfer].aClubs[2].uModel);
            SKA_PackName(&uName, lbl_80186F10[2]);
            SKA_PackName(&uVariant, lbl_80186F60[2]);
            SkinPart_ChooseClubSet(pChar, 2, uName, uVariant, gGolferTable[nGolfer].aClubs[2].uShaft);
            SKA_PackName(&uName, lbl_80187050[2]);
            SKA_PackName(&uVariant, lbl_801870A0[2]);
            SkinPart_ChooseClubSet(pChar, 2, uName, uVariant, gGolferTable[nGolfer].aClubs[2].uGrip);
        }
    } else {
        SkinPart_ApplyClubChoices(pChar, pChoices);
    }
    SkinPart_SetClubsLeftHanded(pChar, Character_IsLeftHanded(pChar));
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
void Quat_Copy(f32* pSrc, f32* pDst) {
    pDst[3] = pSrc[3];
    pDst[0] = pSrc[0];
    pDst[1] = pSrc[1];
    pDst[2] = pSrc[2];
}

void Vec4_CopyPoint(f32* pSrc, f32* pDst) {
    pDst[0] = pSrc[0];
    pDst[1] = pSrc[1];
    pDst[2] = pSrc[2];
    pDst[3] = 1.0f;
}

// Sets every bit of a bit array of nBits bits.
void BitArray_SetAll(u32* aBits, u32 nBits) {
    u32 i;

    for (i = 0; i < (nBits + 31) >> 5; i++) {
        aBits[i] = 0xFFFFFFFF;
    }
}

// Clears every bit of a bit array of nBits bits.
void BitArray_ClearAll(u32* aBits, u32 nBits) {
    u32 i;

    for (i = 0; i < (nBits + 31) >> 5; i++) {
        aBits[i] = 0;
    }
}

u8 BitArray_Test(u32* aBits, u32 n) {
    return (aBits[n >> 5] & (1 << (n & 31))) != 0;
}

// True when the two bit arrays of nBits bits have any bit set in both.
u8 BitArray_Intersects(u32* aA, u32* aB, u32 nBits) {
    u32 i;
    for (i = 0; i < (nBits + 31) >> 5; i++) {
        if (aA[i] & aB[i]) {
            return 1;
        }
    }
    return 0;
}

void BitArray_Set(u32* aBits, u32 n) {
    aBits[n >> 5] |= 1 << (n & 31);
}

// Each bit of aOut is set where both aA and aB have it (bit arrays of nBits bits).
void fn_8001EA54(u32* aA, u32* aB, u32* aOut, u32 nBits) {
    u32 i;

    for (i = 0; i < (nBits + 31) >> 5; i++) {
        aOut[i] = aA[i] & aB[i];
    }
}

void BitArray_Clear(u32* aBits, u32 n) {
    aBits[n >> 5] &= ~(1 << (n & 31));
}

// A bone's position, by bone id.
void Character_GetBonePos(Character* pChar, int nBone, f32* pPos) {
    Character_GetBonePos_FromIndex(pChar, CharModel_GetBoneIndex(pChar->pModel, nBone), pPos);
}

// Bone n's position (bone 1's without an animation slot); nothing without a character.
void Character_GetBonePos_FromIndex(Character* pChar, int nBone, f32* pPos) {
    if (pChar != NULL) {
        if (Character_IsGolfer(pChar) == 0) {
            nBone = 1;
        }
        LLMath_CopyVec(pChar->pModel->pMatrices[nBone][3], pPos);
    }
}

// The character plays from animation slot 0 or 1.
u8 Character_IsGolfer(Character* pChar) {
    if (pChar->nSlot >= 0 && pChar->nSlot < 2) {
        return 1;
    }
    return 0;
}

f32 (*Character_GetBoneMatrixSwapIfLefty(Character* pChar, int nBone))[4] {
    return Character_GetBoneMatrix_FromIndex(pChar, CharModel_GetBoneIndexMapped(pChar->pModel, nBone));
}

// Bone n's matrix (bone 1's without an animation slot); NULL without a character.
f32 (*Character_GetBoneMatrix_FromIndex(Character* pChar, int nBone))[4] {
    int n = nBone;
    if (pChar == NULL) {
        return NULL;
    }
    if (Character_IsGolfer(pChar) == 0) {
        n = 1;
    }
    return pChar->pModel->pMatrices[n];
}

// A bone's matrix, by bone id.
f32 (*Character_GetBoneMatrix(Character* pChar, int nBone))[4] {
    return Character_GetBoneMatrix_FromIndex(pChar, CharModel_GetBoneIndex(pChar->pModel, nBone));
}

f32 fn_8001ED44(Character* pChar, int b) {
    if (b != 0) {
        return pChar->f1660 * (0.5f / Camera_GetLensFovScale(Camera_GetCurrentLens()));
    }
    return pChar->f1660 * (1.0f / Camera_GetLensFovScale(Camera_GetCurrentLens()));
}

// A bone's position, by bone id through CharModel_GetBoneIndexMapped.
void Character_GetBonePosSwapIfLefty(Character* pChar, int nBone, f32* pPos) {
    Character_GetBonePos_FromIndex(pChar, CharModel_GetBoneIndexMapped(pChar->pModel, nBone), pPos);
}

// True for a left-handed golfer: its model is mirrored, and bone ids looked up through
// CharModel_GetBoneIndexMapped go through its second bone table.
u8 Character_IsLeftHanded(Character* pChar) {
    return pChar->pModel->bEE;
}

f32 fn_8001EE00(Character* pChar, int b) {
    if (b != 0) {
        return pChar->f165C * (0.5f / Camera_GetLensFovScale(Camera_GetCurrentLens()));
    }
    return pChar->f165C * (1.0f / Camera_GetLensFovScale(Camera_GetCurrentLens()));
}

f32 (*fn_8001EE64(Character* pChar))[4] {
    return Character_GetBoneMatrix(pChar, 1);
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

// A bone id's index in the model's skeleton, without the second table
// (CharModel_GetBoneIndexMapped).
int CharModel_GetBoneIndex(CharModel* pModel, int nBone) {
    return pModel->aBone[nBone];
}

// A bone's index, through the second table while the model's bEE is set.
int CharModel_GetBoneIndexMapped(CharModel* pModel, int nBone) {
    if (pModel->bEE) {
        return pModel->aBone2[pModel->aBone[nBone]];
    }
    return pModel->aBone[nBone];
}

// a - b into out (three floats)
#ifdef __MWERKS__
asm void Char_Vec3Sub(register f32* pA, register f32* pB, register f32* pOut) {
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
void Char_Vec3Sub(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// in scaled by f into out (three floats)
#ifdef __MWERKS__
asm void Vec3_Scale(register f32 f, register f32* pIn, register f32* pOut) {
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
void Vec3_Scale(f32 f, f32* pIn, f32* pOut) {
    pOut[0] = pIn[0] * f;
    pOut[1] = pIn[1] * f;
    pOut[2] = pIn[2] * f;
}
#endif

// b + a into out (three floats)
#ifdef __MWERKS__
asm void Char_Vec3Add(register f32* pA, register f32* pB, register f32* pOut) {
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
void Char_Vec3Add(f32* pA, f32* pB, f32* pOut) {
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

// The lens's field-of-view scale, tan(fov / 2) / tan(30 degrees) (GoCamera.c): 1 for a 60-degree
// lens, smaller when zoomed in. Visibility and camera distances are scaled by it.
f32 Camera_GetLensFovScale(CamLens* pLens) {
    return pLens->fB0;
}

// The current render camera's lens.
CamLens* Camera_GetCurrentLens(void) {
    return Camera_GetLens(*lbl_80280DF0);
}

// The time of the blend's event uEvent, 0 when it has none.
f32 fn_8001F02C(Clip* pBlend, u64 uEvent) {
    int i;

    for (i = 0; i < pBlend->nEvents; i++) {
        if (pBlend->pEvents[i].uId == uEvent) {
            return pBlend->pEvents[i].fTime;
        }
    }
    return 0.0f;
}

// ---- sweep code (not yet cleaned up) ----

void Anim_SetRate(u8* p, f32 v);

void Anim_SetRate(u8* p, f32 v) {
    *(f32*)(p + 0x14) = v;
}

// ---- end of sweep code ----

// Byte-swaps nCount records laid out as pFormat's nFields fields from *ppSrc to *ppDst; both
// pointers are left after the last record.
void ByteSwap_Records(void** ppSrc, void** ppDst, SwapField* pFormat, int nFields, int nCount) {
    SwapField* pField;
    int i;

    if (nCount > 0) {
        do {
            pField = pFormat;
            for (i = 0; i < nFields; i++) {
                // port: *ppSrc is read and advanced as a u8* (BYTESWAP_SWAPDATA's parameter)
                BYTESWAP_SWAPDATA((u8**)ppSrc, *ppDst, pField->nBytes, pField->nSize);
                *ppDst = (u8*)*ppDst + pField->nBytes;
                pField++;
            }
        } while (--nCount > 0);
    }
}

// Byte-swaps an animation library in place and links it: the header, the records after it, each
// record's entries after those, then each entry's data (each start rounded up to 4 bytes).
// *pnSize gets the library's size.
MtaLib* fn_8001F110(MtaLib* pArg, s32* pnSize) {
    MtaLib* pLib;
    SwapField aHeader[10] = {
        { 16, 1 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 2, 2 }, { 6, 1 },
        { 4, 4 },
    };
    SwapField aRecord[5] = {
        { 16, 1 }, { 16, 1 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
    };
    SwapField aEntry[11] = {
        { 16, 1 }, { 16, 1 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
        { 4, 4 }, { 8, 4 },
    };
    void* pSrc;
    void* pDst;
    s32 nOffset;
    MtaRecord* pRecords;
    int i;
    MtaRecord* pRecord;
    int j;
    MtaEntry* pEntry;
    int nPad;
    MtaRecord* pRecord2;

    pSrc = pDst = pArg;
    ByteSwap_Records(&pSrc, &pDst, aHeader, 10, 1);
    pRecords = (MtaRecord*)(pArg + 1);
    pSrc = pDst = pRecords;
    ByteSwap_Records(&pSrc, &pDst, aRecord, 5, pArg->nRecords);
    pArg->pRecords = pRecords;
    nOffset = sizeof(MtaLib) + pArg->nRecords * sizeof(MtaRecord);
    // fake match: pLib is set from the parameter (through void*) in the first loop's condition, the
    // same value every time; the two then share EA's r31 and the copy disappears. A copy before
    // the loop schedules the entry block differently, a plain copy is merged into pArg.
    for (i = 0; i < (pLib = (MtaLib*)(void*)pArg)->nRecords; i++) {
        pRecord = &pLib->pRecords[i];
        pSrc = pDst = (u8*)pLib + nOffset;
        ByteSwap_Records(&pSrc, &pDst, aEntry, 11, pRecord->nEntries);
        pRecord->pEntries = (MtaEntry*)((u8*)pLib + nOffset);
        nOffset += pRecord->nEntries * sizeof(MtaEntry);
    }
    for (i = 0; i < pLib->nRecords; i++) {
        pRecord2 = &pLib->pRecords[i];
        for (j = 0; j < pRecord2->nEntries; j++) {
            pEntry = &pRecord2->pEntries[j];
            pEntry->pData = (u8*)pLib + nOffset;
            nOffset += pEntry->nBytes;
            nPad = nOffset % 4;
            if (nPad != 0) {
                nOffset += 4 - nPad;
            }
        }
    }
    if (pnSize != NULL) {
        *pnSize = nOffset;
    }
    return pLib;
}
