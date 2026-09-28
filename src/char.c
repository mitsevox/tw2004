// char.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the character object
// (character.h): the golfers and the other skinned characters ('SKLO', player 1000). Building one
// from its 'CHR ' object, its animation, bones, ground placement and leg IK, the set-up for a
// shot, clipping and drawing every character, its clubs, clothes and streamed textures, and the
// stream handlers that make them; TW07 keeps the same file (golf/animation/char.c). At the end, the
// small helpers EA's headers define (bit arrays as in TW07's UBitArray.h, the bone getters of
// char.h, paired-single vector sums) and the animation library byte-swap (MtaLib_SwapAndLink).
// char_tex_manager.c is compiled inside it (#include below).

#include "game.h"
#include "charstate.h"
#include "lldyntex.h"
#include "frontend/fe.h"
#include "unsorted/cull.h"
#include "game_types.h"
#include "endian.h"

// data order: these lead the unity's .data (0x80186CC8), ahead of char_tex_manager.c's
// "_usrtextr", so they are defined before its #include.
// The golfer's IK chains (bone ids as in mtalib.c's names): the spine and right arm down to the
// club head (root, waist, s4, rshld, rwrst, IGdriver, clubhead; gIKRightArmLinks adds s1, rcolr
// and r4rm) and the left arm (s4, lcolr, lshld, l4rm, lwrst). The short right arm is split
// screen's (gIKChainDefsSplit).
IKLinkDef gIKRightArmLinksSplit[7] = {
    {1, 0.0f, -1, 0.0f, 0.0f},
    {3, 1.0f, -1, 0.0f, 0.0f},
    {7, 0.5f, -1, 0.0f, 0.0f},
    {17, 1.0f, -1, 0.0f, 0.0f},
    {21, 0.25f, -1, 0.0f, 0.0f},
    {82, 0.0f, -1, 0.0f, 0.0f},
    {83, 0.0f, -1, 0.0f, 0.0f},
};
IKLinkDef gIKRightArmLinks[10] = {
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
IKLinkDef gIKLeftArmLinks[5] = {
    {7, 0.0f, -1, 0.0f, 0.0f},
    {35, 0.25f, -1, 0.0f, 0.0f},
    {36, 0.5f, -1, 0.0f, 0.0f},
    {39, 0.0f, -1, 0.0f, 0.0f},
    {40, 0.0f, -1, 0.0f, 0.0f},
};
IKChainDef gIKChainDefs[2] = {
    {gIKRightArmLinks, 10, 20, 0.01f},
    {gIKLeftArmLinks, 5, 20, 0.01f},
};
IKChainDef gIKChainDefsSplit[2] = {
    {gIKRightArmLinksSplit, 7, 10, 0.01f},
    {gIKLeftArmLinks, 5, 20, 0.01f},
};
// Per club class, the names Character_SetClubStatesForCharacter dresses the clubs by: the part, then
// the head, shaft and grip sets, each with its variant.
char gClubPartNames[6][13] = {"Drivers", "Fairwaywoods", "Putters", "3Irons", "7Irons", "Wedges"};
char gClubShaftSetNames[6][13] = {"fwd_shaft", "fwd_shaft", "pwi_shaft", "pwi_shaft", "pwi_shaft",
                                  "pwi_shaft"};
char gClubShaftVariantNames[6][13] = {"Defaults", "Defaults", "Defaults", "Defaults", "Defaults", "Defaults"};
char gClubHeadSetNames[6][13] = {"EA_Driver", "EA_Fairway", "EA_Putter", "EA_3Iron", "EA_7Iron", "EA_Wedge"};
char gClubHeadVariantNames[6][13] = {"Defaults", "Defaults", "Defaults", "Defaults", "Defaults", "Defaults"};
char gClubGripSetNames[6][13] = {"fwd_grip", "fwd_grip", "pwi_grip", "pwi_grip", "pwi_grip", "pwi_grip"};
char gClubGripVariantNames[6][13] = {"Defaults", "Defaults", "Defaults", "Defaults", "Defaults", "Defaults"};

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
s32   CharacterState_ResetFidgetState(Character* pChar);                            // CharAnim.c
void  Character_SetSkin(Character* pChar, Skin* pSkin);
void  Character_SetPreferedPos(Character* pChar);
void  ClipBank_Restore(int nSlot);                  // skalib.c
ClipBank* ClipBank_Get(u32 nSlot);                  // skalib.c
AnimLib* AnimLib_Load(u8* pData, ClipBank* pBank);  // skalib.c
CharModel* SKEL_LoadFromMem(u8* pData, s8 n, CharModelDefs* pDefs, int b);   // Skeleton.c
void  fn_80037AB8(Skin* pSkin, CharModel* pModel, int nBone, int nId);   // Skin.c
void  SkinPart_BurnBodySkin(Character* pChar);                // SkinPart.c
CharSliderDefs* CharSlider_CreateDefinitionsFromMem(u8** ppData);
void  Character_FreeClubSkinSets(CharSkinSet* pSet);
void  Character_ClipTest(Character* pChar, int nPlayer);
f32 (*Character_GetRootMatrix(Character* pChar))[4];                        // bone 1's matrix
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
void  Character_ClubStreamCallbackIG(UStreamObject* pObject);
void  Character_ClubStreamCallbackFE(UStreamObject* pObject);
void  Character_GolferStreamCallbackIG(UStreamObject* pObject);
void  Character_GolferStreamCallbackFE(UStreamObject* pObject);
void  SkeletalObject_StreamCallback(UStreamObject* pObject);
void  Character_ResetBlenders(Character* pChar);
void  fn_800BBADC(int nValue);         // SitDevFile.c
void  Character_GetBonePos_FromIndex(Character* pChar, int nBone, f32* pPos);
u8    Character_IsGolfer(Character* pChar);
f32   Character_ComputeMaxVisableDistance(Character* pChar, int bSplitScreen);
f32   Character_ComputeMaxVisableShadowDistance(Character* pChar, int bSplitScreen);
void  Character_KeepClubOutOfGround(Character* pChar);
void  Character_IKLegsToGround(Character* pChar, u8 bRightLeg, u8 bLeftLeg);
void  Character_IKLegToGround(Character* pChar, CourseInfo* pCourse, int nLeg, int nHip, int nKnee,
                              int nAnkle, int nToe, int nAnklePoint, int nToePoint);
void  Character_CalculateClipPoints(Character* pChar);
void  Character_SetupForShot(Character* pChar);
void  SKA_SetLeftHanded(u8 v);                                        // ska_shared.c
void  SKEL_PreTransformIKSkeleton(CharModel* pModel);                           // Skeleton.c
void  SKEL_PostTransformIKSkeleton(Character* pChar);                            // Skeleton.c
void  SKEL_UpdateState(CharModel* pModel, SkelPose* pPose, u8 bTransform);   // Skeleton.c
void  fn_80037C48(Skin* pSkin, SkelPose* pPose);                // Skin.c
void  CharacterState_PlayClipMorphs(Character* pChar, void* pLib, u8 bReset, f32 fOffset);   // CharAnim.c
void  CharacterState_UpdateMorphState(Character* pChar);                            // CharAnim.c
void  Quat_QuatToMatrix(f32* pQ, f32 (*m)[4]);                        // Quaternion.c: a rotation matrix
int   Character_UpdateClubAttachment(Character* pChar, Clip* pClip);
void  Quat_Invert(f32* pQ, f32* pOut);                          // Quaternion.c
void  Quat_RotateVector(f32* pQ, f32* pIn, f32* pOut);                // Quaternion.c: a vector turned by pQ
f32   SKEL_InitIKSkeleton(Character* pChar, f32* pTarget, int bNormals);  // Skeleton.c
void  Char_Vec4Sub(f32* pA, f32* pB, f32* pOut);
void  Character_BeginLoadTexturesCallbackIG(Character* pChar);
void  Character_RequestClothesUpdateIG(int n);
void  fn_8010B098(void* pModel);                                // LLDynTex.c
void  CharacterState_SetTransition(TSKATime* pTime, s32 nState, f32 fTime);     // CharAnim.c
void  Quat_ExtractEulerAngles(f32* pQ, f32* pA, f32* pB, f32* pC);          // Quaternion.c: a rotation as angles
void  SKEL_InitHalfJoints(CharModel* pModel, SkelPose* pPose);          // Skeleton.c
void  mat44flt_Invert(f32 (*pSrc)[4], f32 (*pDst)[4]);              // UMemPool.c
void  Character_GetBonePosSwapIfLefty(Character* pChar, int nBone, f32* pPos);
void  Char_Vec3Add(f32* pA, f32* pB, f32* pOut);
void  Char_Vec3Sub(f32* pA, f32* pB, f32* pOut);
void  fn_80095558(void);
void  AnimLib_ApplySlotCustomAnims(int nSlot);                         // skalib.c
void  Skalib_PlanBanks(void);                                        // skalib.c
void  AnimStream_AllocBuffers(void);                                        // AnimStream.c
void  AnimStream_RandomizeClips(void);                                        // AnimStream.c
void  AnimStream_ReadFirstClips(void);                                        // AnimStream.c
void  Character_SwapTexEntries(u8* pData, int nBytes);
void  Character_SwapTexPalettes(u8* pData, int nBytes);
s32   SkinPart_ListAllTextures(Skin** apSkins, int nSkins, SkinListEntry** ppList);   // SkinPart.c
void  SkinPart_InitTextures(void);                                        // SkinPart.c: empty
void  fn_800100B0(TexBank* pBank, TexEntry* p8, TexPalette* pC, void* p10, void* p14, int nNumTex,
                  int nNumPalettes);                            // LLTex.c
void  Char_Vec4Add(f32* pA, f32* pB, f32* pOut);
f32 (*Character_GetBoneMatrixSwapIfLefty(Character* pChar, int nBone))[4];
f32 (*Character_GetBoneMatrix_FromIndex(Character* pChar, int nBone))[4];
f32   Camera_GetLensFovScale(CamLens* pLens);
void  SKEL_EnableIK(u8 bOn);
void  SKN_InitTris(void);
void  SKN_FreeTris(void);
void  SKN_InitModule(int n);
void  SKN_CloseModule(void);
void  fn_80095554(void);
void  fn_8009555C(void);
void  fn_80095560(void);
void  fn_80095564(void);
void  fn_800955F0(int nPlayer);
void  sApplyUserLogos(void* pChar, void* pModel, SkinChoices* pChoices);   // char_tex_manager.c
void  fn_8010BA2C(void* p);
void  FE_StreamInterruptState(void);               // FEgolferanim.c
void  FE_StreamSetNextState(int nNext);           // FEgolferanim.c
u8    FE_IsTextureSwapDone(void);                // FEgolferanim.c
void  FE_SetTextureSwapState(int nState);
u8    FE_GetDelayTextureSwap(void);
void  FE_SetTextureSwapDue(u8 bDue);
void  AnimStream_Init(void);
void  AnimStream_Close(void);
void  AnimStream_AssignSlots(void);
void  SkinPart_Init(void);
void  SkinPart_Shutdown(void);
void  SkinPart_CopyChoices(Skin* pSkin, int nFrom, int nTo);
s32   SkinPart_GetNumSets(Skin* pSkin);         // SkinPart.c: how many choices aSets[3] holds
void  SkinPart_SetChangeAllCopies(u8 b);
u8    SpeedGolf_IsStartXYHeld(int nPlayer);
void  fn_8010A668(void* p);
void  fn_80008380(void);
void  fn_800106AC(int n);               // LLTexGrp.c
void  fn_800106B8(u8 b);                // LLTexGrp.c
void  UI_ParkFileInAram(void);                // uiLoadFile.c: park the UI file's data in ARAM
void* UI_GetFileBuffer(void);                // uiLoadFile.c: the UI file's buffer
void  UI_RestoreFileFromAram(void);                // uiLoadFile.c: bring the UI file's data back
void  fn_801141F8(struct DynChain* pChain, CharModel* pModel);                         // DynChain.c
void  fn_80035600(void);                // GoTerrain.c
void  fn_80035604(void);                // GoTerrain.c
void  SKN_DrawCharacter(Character* pChar, u32 uFlags);
void  SKN_PoseCharacter(Character* pChar, int n);
void  SKN_BeginFrame(void);                // Skin.c
int   fn_800636EC(void);                // GoCamCont.c
void  fn_8010BF68(void);
void  fn_8010BFE0(void);
void  fn_80112C64(int n);
void  fn_80112CEC(void);
void  CharSkinRef_Init(Skin* pSkin, CharSkinRef* pRef, s32 n);
void  CharSkinRef_Free(void* p);
void  Character_SelectShotType(Character* pChar, int nShotType);

// This file's .sbss (charstate.h), in reverse address order as the compiler lays it out.
// the player whose textures Character_PrepareForRendering streamed last; -1 none
s32 gCharTexStreamedPlayer;
s32 gNumCharacters;             // how many characters gCharacters holds

CharModelDefs gCharModelDefs = { gIKChainDefs, 2 };             // a golfer's IK chains
CharModelDefs gCharModelDefsSplit = { gIKChainDefsSplit, 2 };   // the same in split screen
s32 gMaxBlendClips = 3;         // the most clips a blend node mixes (6, 4 in split screen, in game)
CharSkinSet* gClubSkinSets[2] = { NULL, NULL };  // the clubs: one set, or one per view in split screen

u64 gClubBoneIds[6];            // the club bones' packed names ("IGdriver", -, "IGputter", ...)
Character* gCharacters[5];      // every character made (Character_Add)
CharPool gCharDynTexPool;       // the characters' dynamic textures (CharacterTex_TakePoolEntries)
f32 gCharSupportingNormal[4];   // } the ground normals below and above a foot's test point
f32 gCharCoveringNormal[4];     // } (Character_GetTerrainHeightAndNormal)

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

// Clears the character's SKA tags (its animation events, Character.aTags): none set, each at time
// 2^30 (never).
void Character_ResetSKATags(Character* pChar) {
    s32 i;

    for (i = 0; i < 18; i++) {
        pChar->aTags[i].bSet = 0;
        pChar->aTags[i].fTime = 1073741824.0f;
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
                pChar->aTags[uId].bSet = 1;
                pChar->aTags[uId].fTime = fStart + pBlend->pEvents[i].fTime;
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

    if ((pBank = MtaLib_GetBank(pChar->nSlot)) != NULL) {
        // port: EA passes MtaLib_GetGroup's arguments (with the count's address) to MtaLib_GetRandom, which
        //       takes three: the count's address arrives as its unused n, and n is ignored
        pItem = ((void* (*)(MalBank*, int, int*, int))MtaLib_GetRandom)(pBank, nGroup, &nNum, n);
    }
    return pItem;
}

// Pick the character's clip for an animation group and style from its animation library, keyed
// also by the character's club class (class 1 looks up as 0) and nClipKey. The lookup's fallback flags
// go to bits 0x200 / 0x400 of uFlags; the clip is kept in pCurClip.
void* Char_SetClip(Character* pChar, int nGroup, int nStyle, const char* pName) {
    u32   uFlags = 0;
    int   nClub  = pChar->nClubClass;
    void* pClip;
    if (nClub == 1) {
        nClub = 0;
    }
    pClip = AnimLib_Pick(pChar->nPlayer, pChar->pLib, nGroup, nStyle, nClub, pChar->nClipKey, &uFlags, pName);
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
        BitArray_ClearArray(pPose->a0, 0x80);
        BitArray_ClearArray(pPose->a10, 0x80);
        BitArray_FillArray(pPose->a20, 0x80);
        BitArray_FillArray(pPose->a30, 0x80);
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
        BitArray_FillArray(pPose->a20, 0x80);
        BitArray_FillArray(pPose->a30, 0x80);
        BitArray_ClearArray(pPose->a0, 0x80);
        BitArray_ClearArray(pPose->a10, 0x80);
    }
}

// Reads the ground height under the four foot test points (0 right toe, 1 left toe, 2 right ankle,
// 3 left ankle) into afGroundHeight, where there is ground, and with bNormals the ground's normal
// into aGroundNormal (straight up where there is none). nFootPointStart would pick half of the
// points per call (0 and 2, or 1 and 3), but it is set to -1 first, so every call does all four.
void Character_UpdateFeetTerrainInfo(Character* pChar, int bNormals) {
    f32* pNormal;
    f32 fHeight;
    int i;
    int nLast;
    int nStep;

    if (Ter_GetTGD() != NULL) {
        pChar->nFootPointStart = -1;
        if (pChar->nFootPointStart < 0) {
            pChar->nFootPointStart = 0;
            nLast = 3;
            nStep = 1;
        } else {
            nLast = 2;
            nStep = 2;
        }
        for (i = 0; i <= nLast; i += nStep) {
            fHeight = Character_GetTerrainHeightAndNormal(pChar,
                                                          pChar->aTestPoints[i + pChar->nFootPointStart],
                                                          &pNormal);
            if (!(fHeight < -60000.0f)) {
                pChar->afGroundHeight[i + pChar->nFootPointStart] = fHeight;
            }
            if (bNormals) {
                if (fHeight < -60000.0f) {
                    pChar->aGroundNormal[i + pChar->nFootPointStart][0] = 0.0f;
                    pChar->aGroundNormal[i + pChar->nFootPointStart][1] = 1.0f;
                    pChar->aGroundNormal[i + pChar->nFootPointStart][2] = 0.0f;
                    pChar->aGroundNormal[i + pChar->nFootPointStart][3] = 0.0f;
                } else {
                    LLMath_CopyVec(pNormal, pChar->aGroundNormal[i + pChar->nFootPointStart]);
                }
            }
        }
        pChar->nFootPointStart++;
        if (pChar->nFootPointStart >= 2) {
            pChar->nFootPointStart = 0;
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
            Ter_GetEnclosingGroundData(pCourse, vPos, &fSupportingHeight, &pSupportingSurface,
                                       gCharSupportingNormal, &fCoveringHeight, &pCoveringSurface,
                                       gCharCoveringNormal);
            if (!(fCoveringHeight < -60000.0f)) {
                if (fSupportingHeight < -60000.0f || pSupportingSurface->nClass == 7 ||
                    pSupportingSurface->nClass == 0x13 || fCoveringHeight - fSupportingHeight < 0.05f ||
                    fCoveringHeight < 1.0f + pPos[1]) {
                    if (pCoveringSurface->nClass == 0xC || pCoveringSurface->nClass == 0x12) {
                        return -65536.125f;
                    }
                    *ppNormal = gCharCoveringNormal;
                    return fCoveringHeight;
                }
            } else if (fSupportingHeight < -60000.0f) {
                goto none;      // fake match: the original puts this return after the supporting height
            }
            if (pSupportingSurface->nClass == 0xC || pSupportingSurface->nClass == 0x12) {
                return -65536.125f;
            }
            *ppNormal = gCharSupportingNormal;
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
// model's leg sizes (fRightFootLen right, fLeftFootLen left); 4 the club class's club point through
// the club bone 0x52 (IGdriver).
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
    if (pChar->pSkin->bFootPoints) {
        pMtxLToe = Character_GetBoneMatrixSwapIfLefty(pChar, 0x48);
        pMtxRToe = Character_GetBoneMatrixSwapIfLefty(pChar, 0x3A);
        pMtxLFoot = Character_GetBoneMatrixSwapIfLefty(pChar, 0x47);
        pMtxRFoot = Character_GetBoneMatrixSwapIfLefty(pChar, 0x39);
        LLMath_mat44fltMultiply(pMtxRToe, (Vec4*)pChar->pSkin->aFootPoints[0], (Vec4*)pChar->aTestPoints[0]);
        LLMath_mat44fltMultiply(pMtxLToe, (Vec4*)pChar->pSkin->aFootPoints[1], (Vec4*)pChar->aTestPoints[1]);
        LLMath_mat44fltMultiply(pMtxRFoot, (Vec4*)pChar->pSkin->aFootPoints[2], (Vec4*)pChar->aTestPoints[2]);
        LLMath_mat44fltMultiply(pMtxLFoot, (Vec4*)pChar->pSkin->aFootPoints[3], (Vec4*)pChar->aTestPoints[3]);
    } else {
        fRight = 0.8f * pChar->pModel->fRightFootLen;
        fLeft = 0.8f * pChar->pModel->fLeftFootLen;
        pMtxLToe = Character_GetBoneMatrixSwapIfLefty(pChar, 0x48);
        pMtxRToe = Character_GetBoneMatrixSwapIfLefty(pChar, 0x3A);
        pMtxLFoot = Character_GetBoneMatrixSwapIfLefty(pChar, 0x47);
        pMtxRFoot = Character_GetBoneMatrixSwapIfLefty(pChar, 0x39);
        LLMath_AddScale(pMtxRToe[3], pMtxRToe[1], pChar->pModel->fRightFootLen, pChar->aTestPoints[0]);
        LLMath_AddScale(pMtxLToe[3], pMtxLToe[1], pChar->pModel->fLeftFootLen, pChar->aTestPoints[1]);
        LLMath_AddScale(pMtxRFoot[3], pMtxRToe[2], fRight, pChar->aTestPoints[2]);
        LLMath_AddScale(pMtxLFoot[3], pMtxLToe[2], fLeft, pChar->aTestPoints[3]);
        LLMath_AddScale(pChar->aTestPoints[0], pMtxRToe[2], 0.25f * fRight, pChar->aTestPoints[0]);
        LLMath_AddScale(pChar->aTestPoints[1], pMtxLToe[2], 0.25f * fLeft, pChar->aTestPoints[1]);
    }
    if (pChar->pClubSet != NULL && pClubMtx != NULL) {
        LLMath_mat44fltMultiply(pClubMtx, (Vec4*)pChar->pClubSet->a3C[pChar->nClubClass],
                                (Vec4*)pChar->aTestPoints[4]);
    }
}

// Keeps the club out of the ground: on fairly level ground (the feet's average ground normal vAvgGroundNormal
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

    if (pChar->pClubSet != NULL && pChar->vAvgGroundNormal[1] > 0.9f) {
        pMtx = Character_GetBoneMatrix(pChar, 0x52);
        if (pMtx != NULL && (pCourse = Ter_GetTGD()) != NULL) {
            fHeight = fn_8004D650(pCourse, pChar->aTestPoints[4], vNormal);
            // the else's return is the dead second `b` after the fUnder return; !(<) keeps the
            // NaN case of `fHeight < -60000.0f`
            if (!(fHeight < -60000.0f)) {
                fUnder = fHeight - pChar->aTestPoints[4][1];
                if (fUnder < 0.0f) {
                    return;
                }
            } else {
                return;
            }
            fDot = -Vec4_Dot(pMtx[1], vNormal);
            if (fDot > 0.707f) {
                fHead = pChar->pClubSet->afC[pChar->nClubClass];
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
// model's dynamic chains. Unless bForce, it waits while a golfer's state is 0x13 and while
// fNearestCamDist of any other character is above Character_ComputeMaxVisableDistance.
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
        pChar->bPosed = 0;
        return;
    }
    if (pChar->uCharFlags & 1) {
        return;
    }
    if (!bForce) {
        if (Character_IsGolfer(pChar)) {
            // fake match: the state is compared as an s8 (see GOLFERSTATE_GetCurrentState)
            if ((s8)GOLFERSTATE_GetCurrentState(pChar->nPlayer) == 0x13) {
                return;
            }
        } else if (pChar->fNearestCamDist
                   > Character_ComputeMaxVisableDistance(pChar, gSession.nSplitScreen)) {
            pChar->fNearestCamDist = 1073741824.0f;
            pChar->bPosed = 0;
            return;
        }
    }
    pChar->fNearestCamDist = 1073741824.0f;
    if (pChar->uCharFlags & 4) {
        Character_SetupForShot(pChar);
        bSetupForShot = 1;
    }
    if (pChar->nCurState == 8 && pChar->nTargetState == 8) {
        SKATime_Idle(pChar, pChar->nPlayer, (TSKATime*)pChar->anim, &pChar->blend, fTime);
    } else if (pChar->uCharFlags & 0x100) {
        SKATime_Update((TSKATime*)pChar->anim, &pChar->blend, 5.0f * fTime);
    } else {
        SKATime_Update((TSKATime*)pChar->anim, &pChar->blend, fTime);
        if (pChar->uFlags & 0x1000) {
            pChar->uFlags &= ~0x1000;
            if (pChar->pCurClip != NULL && pChar->pCurClip->pMtaLib != NULL) {
                CharacterState_PlayClipMorphs(pChar, pChar->pCurClip->pMtaLib, 0, 0.5f);
            }
        }
    }
    SKA_SetLeftHanded(pChar->pModel->bLeftHanded);
    if (!gSession.b11) {
        SKATime_Update(&pChar->morphAnim, &pChar->morphBlend, fTime);
    }
    if (Character_IsGolfer(pChar)) {
        CharacterState_UpdateSKAState(pChar);
        if (!gSession.b11) {
            CharacterState_UpdateMorphState(pChar);
        }
    }
    if (pChar->pModel->pSkel != NULL) {
        SKEL_PreTransformIKSkeleton(pChar->pModel);
    }
    if (pChar->blend.pPose != NULL) {
        SKABlender_Update(pChar, &pChar->blend, pChar->pModel, pChar->fAnimTime);
        if (Character_IsGolfer(pChar) && pChar->pClubSet != NULL) {
            // fake match: aBones[nClubHeadBone].v10[1] written as a flat float index (0x40 / 4 + 8 per
            // bone + 5), which gives the original's indexed store; the pose is all 4-byte words
            ((f32*)pChar->blend.pPose)[pChar->nClubHeadBone * 8 + 21]
                    = pChar->pClubSet->afC[pChar->nClubClass];
        }
        SKEL_UpdateState(pChar->pModel, pChar->blend.pPose, 0);
        if (Character_IsGolfer(pChar)) {
            if (BitArray_TestBit(pChar->blend.pPose->a0, CharModel_GetBoneIndexMapped(pChar->pModel, 0x36)) ||
                BitArray_TestBit(pChar->blend.pPose->a0, CharModel_GetBoneIndexMapped(pChar->pModel, 0x38)) ||
                BitArray_TestBit(pChar->blend.pPose->a0, CharModel_GetBoneIndexMapped(pChar->pModel, 0x39)) ||
                BitArray_TestBit(pChar->blend.pPose->a0, CharModel_GetBoneIndexMapped(pChar->pModel, 0x3A))) {
                bRightLegIK = 1;
            }
            if (BitArray_TestBit(pChar->blend.pPose->a0, CharModel_GetBoneIndexMapped(pChar->pModel, 0x44)) ||
                BitArray_TestBit(pChar->blend.pPose->a0, CharModel_GetBoneIndexMapped(pChar->pModel, 0x46)) ||
                BitArray_TestBit(pChar->blend.pPose->a0, CharModel_GetBoneIndexMapped(pChar->pModel, 0x47)) ||
                BitArray_TestBit(pChar->blend.pPose->a0, CharModel_GetBoneIndexMapped(pChar->pModel, 0x48))) {
                bLeftLegIK = 1;
            }
        }
    }
    if (!gSession.b11 && Character_IsGolfer(pChar) && pChar->morphBlend.pPose != NULL) {
        SKABlender_Update(pChar, &pChar->morphBlend, pChar->pModel, pChar->morphAnim.fTime);
        fn_80037C48(pChar->pSkin, pChar->morphBlend.pPose);
    }
    if (pChar->uFlags & 2) {
        pChar->uFlags |= 1;
        return;
    }
    if (pChar->pfnPreBones != NULL) {
        pChar->pfnPreBones();
    }
    BitArray_FillArray(auBits, 0x80);
    SKEL_TransformBones(pChar->pModel, auBits);
    if (Character_IsGolfer(pChar)) {
        Character_UpdateTestPoints(pChar);
        Character_UpdateFeetTerrainInfo(pChar, bSetupForShot || pChar->nCurState != 5 || pChar->bFidgeting
                                        == 1);
        if (pChar->nCurState == 1 || pChar->nCurState == 0 || pChar->nCurState == 9 ||
            (pChar->nCurState == 11 && !(pChar->uCharFlags & 0x8000)) || pChar->nCurState == 5
                    || pChar->nCurState == 12) {
            Character_PlaceFeetOnGround(pChar);
        }
        Character_IKLegsToGround(pChar, bRightLegIK, bLeftLegIK);
    }
    if (Character_IsGolfer(pChar)) {
        if (pChar->pModel->pSkel != NULL) {
            SKEL_PostTransformIKSkeleton(pChar);
        }
        Character_KeepClubOutOfGround(pChar);
    }
    pChar->bPosed = 0;
    Character_CalculateClipPoints(pChar);
    if (Character_IsGolfer(pChar)) {
        SKEL_UpdateDynChain(pChar->pModel, pChar->pModel->pF0, fTime);
        SKEL_UpdateDynChain(pChar->pModel, pChar->pModel->pF4, fTime);
        SKEL_UpdateDynChain(pChar->pModel, pChar->pModel->pF8, fTime);
        for (i = 0; i < 6; i++) {
            SKEL_UpdateDynChain(pChar->pModel, pChar->pModel->apFC[i], fTime);
        }
        for (i = 0; i < 6; i++) {
            SKEL_UpdateDynChain(pChar->pModel, pChar->pModel->ap114[i], fTime);
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
        pChar->nWristBone    = CharModel_GetBoneIndexMapped(pChar->pModel, 0x15);
    }
}

// Gives the character its body's skin and poses the skeleton from it (Character_SetPreferedPos).
// For a golfer it then keeps the four foot test points in the skin (TW07 has this part as
// Character_SetTestPointsFeet): each toe bone's position moved by vBoneToToe and each ankle bone's
// by vBoneToHeel (larger offsets for animation slot 0), taken into the frame of its bone (through
// its inverted matrix) into Skin.aFootPoints, and bFootPoints set.
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
            LLMath_mat44fltMultiply(mRToeInv, &vRToe, (Vec4*)pChar->pSkin->aFootPoints[0]);
            LLMath_mat44fltMultiply(mLToeInv, &vLToe, (Vec4*)pChar->pSkin->aFootPoints[1]);
            LLMath_mat44fltMultiply(mRFootInv, &vRFoot, (Vec4*)pChar->pSkin->aFootPoints[2]);
            LLMath_mat44fltMultiply(mLFootInv, &vLFoot, (Vec4*)pChar->pSkin->aFootPoints[3]);
            pChar->pSkin->bFootPoints = 1;
        }
    }
}

// Poses the character's skeleton in its body skin's rest pose (SKEL_UpdateState, SKEL_InitHalfJoints) and,
// when the skin has a model, hands the skeleton the skin's matrices. Needs a character, a skin and
// a model.
void Character_SetPreferedPos(Character* pChar) {
    if (pChar != NULL && pChar->pSkin != NULL && pChar->pModel != NULL) {
        SKEL_UpdateState(pChar->pModel, &pChar->pSkin->pose, 1);
        SKEL_InitHalfJoints(pChar->pModel, &pChar->pSkin->pose);
        if (pChar->pSkin->pModel != NULL) {
            fn_80037AB8(pChar->pSkin, pChar->pModel, 0, 0);
            SKEL_SetSkinBonePoses(pChar->pModel, pChar->pSkin->pModel->p34);
            SKEL_SetDefaultWorld2BoneMatrices(pChar->pModel, pChar->pSkin->pWorld2Bone);
            SKEL_SetSkinningMatrices(pChar->pModel, pChar->pSkin->pSkinMtx, pChar->pSkin->pModel->n14);
        }
    }
}

// fake match: an identity read; it gives EA's register order.
static inline f32* fn_800187CC_Read(f32* p) { return p; }

// A golfer is dropped to 0.01 below the lowest of its four ground heights (the test points move
// with it) and vAvgGroundNormal set to the average ground normal; any other character stands on the ground
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
        pChar->vAvgGroundNormal[0] = 0.0f;
        pChar->vAvgGroundNormal[1] = 0.0f;
        pChar->vAvgGroundNormal[2] = 0.0f;
        pChar->vAvgGroundNormal[3] = 0.0f;
        for (i = 0; i < 4; i++) {
            Char_Vec3Add(pChar->aGroundNormal[i], pChar->vAvgGroundNormal, pChar->vAvgGroundNormal);
            if (pChar->afGroundHeight[i] < fLowest) {
                fLowest = pChar->afGroundHeight[i];
            }
        }
        LLMath_Normalize3(pChar->vAvgGroundNormal, pChar->vAvgGroundNormal);
        if (fLowest < -60000.0f) {
            return;
        }
        fY = fLowest - 0.01f;
        fDelta = fY - pChar->pModel->pBones[0].v1C[1];
        pChar->pModel->pBones[0].v1C[1] = fY;
        pChar->aTestPoints[0][1] += fDelta;
        pChar->aTestPoints[1][1] += fDelta;
        pChar->aTestPoints[2][1] += fDelta;
        pChar->aTestPoints[3][1] += fDelta;
        pChar->aTestPoints[4][1] += fDelta;
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
    BitArray_ClearArray(auBits, 0x80);
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
        BitArray_SetBit(auBits, CharModel_GetBoneIndexMapped(pChar->pModel, 0x38));
        BitArray_SetBit(auBits, CharModel_GetBoneIndexMapped(pChar->pModel, 0x36));
        BitArray_SetBit(auBits, CharModel_GetBoneIndexMapped(pChar->pModel, 0x39));
    }
    if (bLeftLeg) {
        Character_IKLegToGround(pChar, pCourse, 1, CharModel_GetBoneIndexMapped(pChar->pModel, 0x44),
                                CharModel_GetBoneIndexMapped(pChar->pModel,
                                        0x46), CharModel_GetBoneIndexMapped(pChar->pModel, 0x47),
                                CharModel_GetBoneIndexMapped(pChar->pModel, 0x48), 3, 1);
        BitArray_SetBit(auBits, CharModel_GetBoneIndexMapped(pChar->pModel, 0x46));
        BitArray_SetBit(auBits, CharModel_GetBoneIndexMapped(pChar->pModel, 0x44));
        BitArray_SetBit(auBits, CharModel_GetBoneIndexMapped(pChar->pModel, 0x47));
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
    LLMath_CopyVec(pChar->aTestPoints[nAnklePoint], vPoint);
    // how far each test point sits below the ground (0.165 in, in feet)
    fDropA = 0.165f / 12.0f + (pChar->afGroundHeight[nAnklePoint] - vPoint[1]);
    fDropB = 0.165f / 12.0f + (pChar->afGroundHeight[nToePoint] - pChar->aTestPoints[nToePoint][1]);
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

    BitArray_FillArray(auBits, 0x80);
    if (pChar != NULL) {
        LLMath_CopyVec(pPos, pChar->pModel->pBones->v1C);
        if (bPlace) {
            SKEL_TransformBones(pChar->pModel, auBits);
            Character_UpdateTestPoints(pChar);
            pChar->nFootPointStart = -1;
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
    pChar->pfnPreBones = NULL;
    for (i = 0; i < 4; i++) {
        pChar->buffers[i].n00 = -1;
        pChar->buffers[i].p04 = NULL;
        pChar->buffers[i].p0C = NULL;
        pChar->buffers[i].p10 = NULL;
        pChar->buffers[i].p14 = NULL;
        pChar->buffers[i].pBuf = StaticMem_Alloc(0x890, 2, 0x40, "char.c", 0x8AF);
    }
    pNode = &pChar->blend;
    SKATime_Init((TSKATime*)pChar->anim);
    SKABlendData_Init(&pNode, 1, 0, SKABlender_BlendLinear, 1);
    pNode = &pChar->morphBlend;
    SKATime_Init(&pChar->morphAnim);
    SKABlendData_Init(&pNode, 1, 1, SKABlender_BlendLinear, 1);
    pChar->pLib = NULL;
    pChar->n3D4 = 0;
    pChar->p44 = NULL;
    pChar->fNearestCamDist = 0.0f;
    pChar->nTargetState = 0;
    pChar->nCurState = 0;
    pChar->uSKAFlags = 0;
    pChar->uCharFlags = 0x4000;
    pChar->f162C = 1.0f;
    pChar->f1630 = 0.5f;
    pChar->f1634 = 0.2f;
    pChar->n1650 = 0;
    pChar->nClipResult = 2;
    pChar->nShadowClipResult = 2;
    pChar->bPosed = 0;
    pChar->pClubSet = NULL;
    pChar->nClubClass = 0;
    pChar->nClipKey = 0;
    pChar->nSlot = 0;
    pChar->n48 = -1;
    pChar->fNearFade = 1.0f;
    pChar->nStyle = 0;
    pChar->nPlayer = -1;
    pChar->uId = 0;
    pChar->pBlend = NULL;
    pChar->fBackswing = 0.0f;
    pChar->nGroup = -1;
    pChar->nClampEvent = -1;
    pChar->pReactionClip = NULL;
    pChar->nFootPointStart = -1;
    pChar->nView = 0;
    pChar->pRecords = NULL;
    CharacterState_ResetFidgetState(pChar);
    pChar->n16DC = 0;
    pChar->fMaxShadowDist = pChar->fMaxVisibleDist = 1073741824.0f;
    pChar->pCurClip = NULL;
    pChar->pMorphLib = NULL;
    pChar->aDynTexSlot[0] = -1;
    pChar->apDynTex[0] = NULL;
    pChar->aDynTexSlot[1] = -1;
    pChar->apDynTex[1] = NULL;
    if (gSession.nGameType == 10 || gSession.nGameType == 3) {
        pChar->nDynTex = 2;
    } else {
        pChar->nDynTex = 1;
    }
    pChar->nCurDynTex = 0;
    pChar->bTexLoaded = 0;
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
// fAnimTime from fAnimStart, that event's time in the blend and afSwingTop[1]), uCharFlags bits
// 0x10000, 8 and 4 set (4: the update sets it up for the shot), then one animation update of no
// length with the skeleton's clip cleared.
void Character_AlignCharacterForShotImpact(Character* pChar) {
    if (pChar != NULL && pChar->pBlend != NULL) {
        pChar->uCharFlags |= 0x10000;
        pChar->uCharFlags |= 8;
        pChar->uCharFlags |= 4;
        pChar->pModel->pSkel->pClip = NULL;
        pChar->fAnimTime = pChar->fAnimStart + SKA_GetTagTime(pChar->pBlend, 2) - pChar->afSwingTop[1];
        Character_UpdateAnimation(pChar, 0, 0.0f);
        pChar->pModel->pSkel->pClip = NULL;
    }
}

// Frees the character's textures: gives back its dynamic texture pool entries, frees the texture
// and palette tables Character_LoadTextures made, and closes its texture file.
void Character_FreeTextures(Character* pChar) {
    CharacterTex_ReleasePoolEntries(pChar);
    if (pChar->pTexEntries != NULL) {
        StaticMem_Free(pChar->pTexEntries);
    }
    if (pChar->pPalettes != NULL) {
        StaticMem_Free(pChar->pPalettes);
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
// use (SkinPart_ListAllTextures), the texture of that name (and the one after it when it goes with
// it) with its palette, or an empty one. Then it opens the golfer's texture file.
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
    BYTESWAP_SWAPDATA(&pData, (u8*)&pChar->nTexFileBase, 4, 4);
    BYTESWAP_SWAPDATA(&pData, (u8*)&pChar->n5C, 4, 4);
    if (nTexBytes != 0) {
        pTexData = (TexEntry*)pData;
        // fake match: pTexData and pPalData (the same pointers as pData) go to the swaps: EA's
        // registers
        Character_SwapTexEntries((u8*)pTexData, nTexBytes);
        pData += nTexBytes;
    }
    if (nPalBytes != 0) {
        pPalData = (TexPalette*)pData;
        Character_SwapTexPalettes((u8*)pPalData, nPalBytes);
    } else {
        pChar->n5C = 0;
    }
    bAll = gSession.nGameType == 10 || gSession.nGameType == 3;
    nTex = nTexBytes / (int)sizeof(TexEntry);
    nPal = nPalBytes / (int)sizeof(TexPalette);
    if (bAll) {
        pChar->nTexEntries = nTex;
        pChar->nPalettes = nPal;
    } else {
        pChar->nTexEntries = SkinPart_ListAllTextures(apSkins, nSkins, &pList);
        nExtra = 0;
        pChar->nPalettes = pChar->nTexEntries;
        nPalBytes = pChar->nPalettes * sizeof(TexPalette);
        for (i = 0; i < pChar->nTexEntries; i++) {
            for (j = 0; j < nTex; j++) {
                if (pList[i].uId == pTexData[j].u0 && (pTexData[j].b47 & 1)) {
                    nExtra++;
                    break;
                }
            }
        }
        pChar->nTexEntries += nExtra;
    }
    pChar->pTexEntries = NULL;
    pChar->pB8 = NULL;
    pChar->pPalettes = NULL;
    pChar->pBC = NULL;
    if (pChar->nTexEntries != 0) {
        pChar->pTexEntries = StaticMem_Alloc(pChar->nTexEntries * sizeof(TexEntry), 2, 0x10, "char.c", 0x9A9);
        pChar->pB8 = StaticMem_Alloc(pChar->nTexEntries * 64, 2, 0x10, "char.c", 0x9AE);
    }
    if (pChar->nPalettes != 0) {
        pChar->pPalettes = StaticMem_Alloc(nPalBytes, 2, 0x10, "char.c", 0x9B8);
        pChar->pBC = StaticMem_Alloc(nPal, 2, 0x10, "char.c", 0x9BD);
    }
    if (bAll) {
        if (pChar->nTexEntries != 0) {
            Mem_cpy(pChar->pTexEntries, pTexData, nTexBytes);
        }
        if (pChar->nPalettes != 0) {
            Mem_cpy(pChar->pPalettes, pPalData, nPalBytes);
        }
    } else {
        nNames = pChar->nTexEntries - nExtra;
        for (k = nOut = 0; k < nNames; k++) {
            bFound = 0;
            for (m = 0; m < nTex; m++) {
                if (pList[k].uId == pTexData[m].u0) {
                    Mem_cpy(&pChar->pTexEntries[nOut], &pTexData[m], sizeof(TexEntry));
                    if (pTexData[m].nPalette != -1) {
                        Mem_cpy(&pChar->pPalettes[k], &pPalData[pTexData[m].nPalette], sizeof(TexPalette));
                        pChar->pTexEntries[nOut].nPalette = k;
                    }
                    nOut++;
                    if (pTexData[m].b47 & 1) {
                        Mem_cpy(&pChar->pTexEntries[nOut], &pTexData[m + 1], sizeof(TexEntry));
                        nOut++;
                    }
                    bFound = 1;
                    break;
                }
            }
            if (!bFound) {
                pChar->pTexEntries[nOut].u0 = 0;
                pChar->pTexEntries[nOut].nPalette = -1;
                nOut++;
            }
        }
    }
    if (pList != NULL) {
        StaticMem_Free(pList);
    }
    fn_800100B0(&pChar->bank78, pChar->pTexEntries, pChar->pPalettes, pChar->pB8, pChar->pBC,
                pChar->nTexEntries, pChar->nPalettes);
    pChar->pBank = &pChar->bank78;
    // port: EA passes arguments SkinPart_InitTextures (empty) ignores
    ((void (*)(Skin*, TexBank*, int, int))SkinPart_InitTextures)(pChar->pSkin, &pChar->bank78, 0xBF600,
            0xCDA);
    sprintf(pChar->szTexFile, "%sdata\\CharStrm\\CharTex\\%02dalltex.fxg", "", pChar->nGolferId + 1);
    pChar->hFile = fn_800060E0(pChar->szTexFile);
}

// Copies every skin's newest choices (copy 3, see SkinPart_SetChangeAllCopies) down to copy 2, as a
// texture load starts.
void Character_CopySkinChoices3To2(Character* pChar) {
    int i;
    for (i = 0; i < pChar->nSkins; i++) {
        SkinPart_CopyChoices(pChar->apSkins[i], 3, 2);
    }
}

// Copies every skin's choices from copy 2 down to copy 1, as a texture load ends.
void Character_CopySkinChoices2To1(Character* pChar) {
    int i;
    for (i = 0; i < pChar->nSkins; i++) {
        SkinPart_CopyChoices(pChar->apSkins[i], 2, 1);
    }
}

// Copies every skin's choices from copy 1 down to copy 0, the one shown, and flags each skin's
// choices changed (bit 0x1 of uFlags).
void Character_CopySkinChoices1To0(Character* pChar) {
    int i;
    for (i = 0; i < pChar->nSkins; i++) {
        SkinPart_CopyChoices(pChar->apSkins[i], 1, 0);
        pChar->apSkins[i]->uFlags |= 1;
    }
}

// Queues a dynamic texture load (LLDynTex.c) for the character, with the callbacks run as it begins
// and as it ends (the Begin/End ...Callback functions). The pool's pLoadingChar is set to the
// character queued, or to NULL when no job is free.
void Character_AddTextureLoadRequest(Character* pChar, void (*pfnBegin)(Character* pChar),
                                     void (*pfnEnd)(Character* pChar)) {
    DynTexJob* pJob = fn_8010B8EC();

    if (pJob != NULL) {
        gCharDynTexPool.pLoadingChar = pChar;
        pJob->pfnBegin = pfnBegin;
        pJob->pChar = pChar;
        pJob->pfnEnd = pfnEnd;
        pJob->ppBank = &pChar->pBank;
        fn_8010B930(pJob);
    } else {
        gCharDynTexPool.pLoadingChar = NULL;
    }
}

// The front end's begin callback of a texture load (the golfer that came in): sets up the dynamic
// textures of the character's model in use for the skins' newest choices
// (Character_CopySkinChoices3To2): the name codes the choices do not use and the ones they need go
// to its dynamic textures (SkinPart_DropUnusedTextures, SkinPart_QueueMissingTextures).
void Character_BeginLoadTexturesCallbackFE(Character* pArg) {
    // fake match: a copy of the parameter through void* (a plain copy is merged into it)
    Character* pChar = (Character*)(void*)pArg;
    void* pModel = pChar->apDynTex[pChar->nCurDynTex];

    FE_SetTextureSwapDue(0);
    pChar->pDynTex = pModel;
    fn_8010BC88(&pChar->pBank);
    // port: EA passes an argument fn_8010BEC4 ignores
    ((void (*)(void*))fn_8010BEC4)(pModel);
    Character_CopySkinChoices3To2(pChar);
    SkinPart_DropUnusedTextures(pChar->apSkins, pChar->nSkins, pModel);
    SkinPart_QueueMissingTextures(pChar->apSkins, pChar->nSkins, pModel, NULL, 0);
    // port: EA passes an argument fn_8010BED4 ignores
    ((void (*)(void*))fn_8010BED4)(pModel);
}

// The front end's end callback of a texture load (the golfer that came in): passes the skins'
// choices down to copy 0, puts the profile's logos on the model in use, and sets the menu's b81
// (FE_SetNewTexturesFlag).
void Character_EndLoadTexturesCallbackFE(Character* pChar) {
    Character_CopySkinChoices2To1(pChar);
    Character_CopySkinChoices1To0(pChar);
    sApplyUserLogos(pChar, pChar->apDynTex[pChar->nCurDynTex], &FE_GetCurrentProfile()->choices);
    fn_8010BA2C(pChar->apDynTex[pChar->nCurDynTex]);
    FE_SetNewTexturesFlag(1);
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

    FE_SetTextureSwapState(1);
    pModel = pChar->apDynTex[1 - pChar->nCurDynTex];
    fn_8010A6A8(pChar->apDynTex[pChar->nCurDynTex], pModel);
    pChar->pDynTex = pModel;
    fn_8010BC88(&pChar->pBank);
    // port: EA passes an argument fn_8010BEC4 ignores
    ((void (*)(void*))fn_8010BEC4)(pModel);
    for (i = 0; i < pChar->nSkins; i++) {
        pSkin = pChar->apSkins[i];
        for (j = 0; j < SkinPart_GetNumSets(pSkin); j++) {
            if (memcmp(&pSkin->aSets[2][j], &pSkin->aSets[3][j], sizeof(SkinChoice)) != 0) {
                SkinPart_DropSetVariantTextures(pSkin, j, pSkin->aSets[3][j].nVariant,
                                                pSkin->aSets[3][j].nOption, pModel);
            }
        }
    }
    Character_CopySkinChoices3To2(pChar);
    SkinPart_DropUnusedTextures(pChar->apSkins, pChar->nSkins, pModel);
    SkinPart_QueueMissingTextures(pChar->apSkins, pChar->nSkins, pModel, NULL, 0);
    // port: EA passes an argument fn_8010BED4 ignores
    ((void (*)(void*))fn_8010BED4)(pModel);
}

// Once a swap is due (FE_IsTextureSwapDue, set by Character_EndSwapTexturesCallbackFE): switches the
// character to its other model, the one the swap loaded, passes the skins' choices down to copy 0
// and puts the profile's logos and the skins on it.
void Character_ExecuteTextureSwapFE(Character* pChar) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    int i;
    void* pModel;

    if (FE_IsTextureSwapDue()) {
        FE_SetTextureSwapDue(0);
        FE_SetTextureSwapState(0);
        pChar->nCurDynTex = 1 - pChar->nCurDynTex;
        pModel = pChar->apDynTex[pChar->nCurDynTex];
        Character_CopySkinChoices1To0(pChar);
        sApplyUserLogos(pChar, pModel, &pProfile->choices);
        fn_8010BA2C(pModel);
        fn_80008380();
        for (i = 0; i < pChar->nSkins; i++) {
            SkinPart_SetupMaterials(pChar->apSkins[i], pModel);
        }
        fn_8010BC64(pModel);
    }
}

// The front end's end callback of a texture swap: passes the skins' choices down to copy 1, flags
// the swap due (FE_SetTextureSwapDue) and, unless the menu delays it (FE_GetDelayTextureSwap), swaps now.
void Character_EndSwapTexturesCallbackFE(Character* pChar) {
    Character_CopySkinChoices2To1(pChar);
    FE_SetTextureSwapState(2);
    FE_SetTextureSwapDue(1);
    if (FE_GetDelayTextureSwap() == 0) {
        Character_ExecuteTextureSwapFE(pChar);
    }
}

// The in-game begin callback of a texture load: sets up the dynamic textures of the character's
// model in use (fn_8010B098), dresses it (Character_SetClubsAndClothes) and hands its dynamic
// textures the name codes the skins' newest choices need (SkinPart_QueueMissingTextures, given the
// "Glove" part's id); the last marked player (gCharTexStreamedPlayer) is dressed again.
void Character_BeginLoadTexturesCallbackIG(Character* pArg) {
    // fake match: a copy of the parameter through void* (a plain copy is merged into it)
    Character* pChar = (Character*)(void*)pArg;
    u64 uGlove;
    void* pModel;

    pModel = pChar->apDynTex[pChar->nCurDynTex];
    pChar->pDynTex = pModel;
    fn_8010BC88(&pChar->pBank);
    fn_8010B098(pModel);
    // port: EA passes an argument fn_8010BEC4 ignores
    ((void (*)(void*))fn_8010BEC4)(pModel);
    Character_SetClubsAndClothes(pChar, pChar->nPlayer);
    Character_CopySkinChoices3To2(pChar);
    SKA_PackName(&uGlove, "Glove");
    SkinPart_QueueMissingTextures(pChar->apSkins, pChar->nSkins, pModel, &uGlove, 1);
    if (gCharTexStreamedPlayer >= 0) {
        Character_SetClubsAndClothes(gPlayers[gCharTexStreamedPlayer].pChar, gCharTexStreamedPlayer);
    }
    // port: EA passes an argument fn_8010BED4 ignores
    ((void (*)(void*))fn_8010BED4)(pModel);
}

// The in-game end callback of a texture load: passes the skins' choices down to copy 0, puts the
// created golfer's logos on the model in use, marks the character's textures loaded (bTexLoaded) and
// clears the pool's queued character.
void Character_EndLoadTexturesCallbackIG(Character* pChar) {
    Character_CopySkinChoices2To1(pChar);
    Character_CopySkinChoices1To0(pChar);
    sApplyUserLogos(pChar, pChar->apDynTex[pChar->nCurDynTex], pChar->pChoices);
    fn_8010BA2C(pChar->apDynTex[pChar->nCurDynTex]);
    pChar->bTexLoaded = 1;
    gCharDynTexPool.pLoadingChar = NULL;
}

// Fills the characters' dynamic texture pool: two entries (the game-type test gives two either
// way), each a dynamic texture (LLDynTex.c) for 0x46 textures with a 0x87000-byte pixel buffer, all
// free. Called when a round or the front end starts.
void CharacterTex_Init(void) {
    int i;

    if (gSession.nGameType == 10 || gSession.nGameType == 3) {
        gCharDynTexPool.nEntries = 2;
    } else {
        gCharDynTexPool.nEntries = 2;
    }
    for (i = 0; i < gCharDynTexPool.nEntries; i++) {
        gCharDynTexPool.a[i].pDynTex = fn_8010A520(0x46, 0x87000, 0, 0x870, 4);
        gCharDynTexPool.a[i].bUsed = 0;
    }
}

// Free every pool entry's dynamic texture (fn_8010A668) and mark the entry free.
void CharacterTex_Close(void) {
    int i;
    for (i = 0; i < gCharDynTexPool.nEntries; i++) {
        fn_8010A668(gCharDynTexPool.a[i].pDynTex);
        gCharDynTexPool.a[i].bUsed = 0;
    }
}

// Gives the character's dynamic texture pool entries back (a64 and a6C cleared) and marks its
// textures not loaded (bTexLoaded).
void CharacterTex_ReleasePoolEntries(Character* pChar) {
    int i;
    for (i = 0; i < pChar->nDynTex; i++) {
        if (pChar->apDynTex[i] != NULL) {
            gCharDynTexPool.a[pChar->aDynTexSlot[i]].bUsed = 0;
            pChar->aDynTexSlot[i] = -1;
            pChar->apDynTex[i] = NULL;
        }
    }
    pChar->bTexLoaded = 0;
}

// Takes free dynamic texture pool entries for the character until it has nDynTex of them: a64 gets each
// entry's dynamic texture, a6C its index. It takes fewer when the pool runs out.
void CharacterTex_TakePoolEntries(Character* pChar) {
    int i;
    int n = 0;
    for (i = 0; i < gCharDynTexPool.nEntries; i++) {
        if (gCharDynTexPool.a[i].bUsed == 0) {
            pChar->aDynTexSlot[n] = i;
            pChar->apDynTex[n] = gCharDynTexPool.a[i].pDynTex;
            n++;
            gCharDynTexPool.a[i].bUsed = 1;
            if (n == pChar->nDynTex) {
                return;
            }
        }
    }
}

// Empty in this build: CharacterTex_PreHoleInit and CharacterTex_StartStreamingPlayers call it with
// a character just before that character gives its dynamic-texture pool entries back
// (CharacterTex_ReleasePoolEntries).
void CharacterTex_PreReleasePoolEntries(Character* pChar) {
}

// Runs one step of the dynamic texture loader (fn_8010BFE0) each frame from the main loop, with
// more than two players (with two or fewer every character's textures are loaded at the start).
void CharacterTex_TextureLoader(void) {
    if (gSession.nNumPlayers > 2) {
        fn_8010BFE0();
    }
}

// Before a hole: no player marked (gCharTexStreamedPlayer -1); with more than two players only the player
// with the honor keeps pool entries: every player's character gives its back, then that player's
// takes them and its dynamic textures are loaded now (fn_8010BF68 runs the loader to the end).
void CharacterTex_PreHoleInit(void) {
    Character* pChar;
    int i;

    gCharTexStreamedPlayer = -1;
    if (gSession.nNumPlayers > 2) {
        for (i = 0; i < gSession.nNumPlayers; i++) {
            CharacterTex_PreReleasePoolEntries(gPlayers[i].pChar);
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
// play (GM_GetSecondHonors). nPlayer's character loses bit 0x40 of uCharFlags; after the display
// finishes drawing (fn_80008380), every other character holding pool entries (except the one being
// loaded, the pool's pLoadingChar) gives them back and gets bit 0x40 (not drawn). Unless nPlayer's
// textures are loaded (bTexLoaded), it takes the entries (the character being loaded giving its
// back first) and its textures are loaded now (fn_8010BF68); when it is the one being loaded, the
// loader is just run to the end. The next player's character is then queued the same way, its
// textures left to CharacterTex_TextureLoader.
void CharacterTex_StartStreamingPlayers(int nPlayer) {
    int i;
    Character* pChar;
    Character* pQueued;

    if (gSession.nNumPlayers > 2) {
        gPlayers[nPlayer].pChar->uCharFlags &= ~0x40;
        fn_80008380();
        pChar = gPlayers[nPlayer].pChar;
        for (i = 0; i < gSession.nNumPlayers; i++) {
            if (i != nPlayer && gPlayers[i].pChar->apDynTex[gPlayers[i].pChar->nCurDynTex] != NULL &&
                gPlayers[i].pChar != gCharDynTexPool.pLoadingChar) {
                CharacterTex_PreReleasePoolEntries(gPlayers[i].pChar);
                CharacterTex_ReleasePoolEntries(gPlayers[i].pChar);
                gPlayers[i].pChar->bTexLoaded = 0;
                gPlayers[i].pChar->uCharFlags |= 0x40;
            }
        }
        if (!pChar->bTexLoaded) {
            pQueued = gCharDynTexPool.pLoadingChar;
            if (pChar != pQueued) {
                fn_8010BF68();
                if (pQueued != NULL) {
                    CharacterTex_PreReleasePoolEntries(pQueued);
                    CharacterTex_ReleasePoolEntries(pQueued);
                    pQueued->bTexLoaded = 0;
                    pQueued->uCharFlags |= 0x40;
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
            if (!pChar->bTexLoaded && pChar != gCharDynTexPool.pLoadingChar) {
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
//       (SKA_SwapClip > BYTESWAP_SWAPDATA): a little-endian port does not swap there.
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
// (gSacReloading set). Then AnimStream_AssignSlots gives the first two players to play an animation
// stream slot each.
void Character_ReloadSacFiles(void) {
    if (gSession.nNumPlayers > 1) {
        gSacReloading = 1;
        AnimLib_ReloadSlot();
        Character_RegisterSacStreamClient();
        fn_80014C9C();
        fn_80014DC0();
        Character_UnregisterSacStreamClient();
        AnimLib_FreeWorkCopies();
    }
    AnimStream_AssignSlots();
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
        sprintf(pChar->szTexFile, "%sdata\\CharStrm\\CharTex\\%02dalltex.fxg", "", pChar->nGolferId + 1);
        pChar->hFile = fn_800060E0(pChar->szTexFile);
    }
}

// The characters' set-up late in a round's start (GO_vInitIG, after the players are set up): with
// two players or fewer, every player's character takes its pool entries and its dynamic textures
// are loaded now (fn_8010BF68; in split screen Character_RequestClothesUpdateIG for its player
// too). Then the saved choices of animation slots 0 and 1 are applied, the animation stream is set
// up (AnimStream_SizeSlotClips, AnimStream_AllocBuffers, AnimStream_RandomizeClips,
// AnimStream_ReadFirstClips), the sac files are loaded (Character_LoadSacFiles) and the work copies freed.
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
                Character_RequestClothesUpdateIG(i);
            }
        }
    }
    AnimLib_ApplySlotCustomAnims(0);
    AnimLib_ApplySlotCustomAnims(1);
    AnimStream_SizeSlotClips(-1);
    AnimStream_AllocBuffers();
    Skalib_PlanBanks();
    Character_LoadSacFiles();
    AnimLib_FreeWorkCopies();
    AnimStream_RandomizeClips();
    AnimStream_ReadFirstClips();
}

// Builds a character from its 'CHR ' object at pData: a header (its animation slot, a skin value, a
// flag for bit 0x400 of uCharFlags, a model flag and five model values), its skin, the p44 entries,
// its model (SKEL_LoadFromMem; a golfer outside the front end gets the golfer model definitions),
// its own animation library (kept when its clips are its own or in a bank, else queued as an
// overlay of its slot), and its slider definitions. A golfer then gets club skin set nSet
// (gClubSkinSets), and with bLook its look from pChoices. nId: the golfer id (nGolferId). nUnused:
// not read. NULL when no character could be made.
// port: the object is little-endian on disc and BYTESWAP_SWAPDATA swaps each value as it reads it:
//       a little-endian port does not swap there.
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
    pChar->nGolferId = nId;
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
        pChar->uCharFlags |= 0x400;
    }
    bGolfer = Character_IsGolfer(pChar);

    // its skin
    BYTESWAP_SWAPDATA(&pData, (u8*)&nSize, 4, 4);
    pData += 0xC;
    if (nSize == 0) {
        pChar->pSkin = NULL;
    } else {
        pChar->pSkin = SKN_Create(pData, bLook);
        pChar->pSkin->f10D8 = fSkin;
        pChar->pSkin->f10DC = 1.0f;
        if (gSession.nGameType != 10 && gSession.nGameType != 3 && Character_IsGolfer(pChar)) {
            SKN_FreeRenderData(pChar->pSkin);
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
            pDefs = &gCharModelDefsSplit;
        } else {
            pDefs = &gCharModelDefs;
        }
    }
    bModel = nModel == 1;
    if (bLook && pChoices != NULL) {
        bModel = (u8)pChoices->bLeftHanded;
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
            gLibSlots[pChar->nSlot].overlays[gLibSlots[pChar->nSlot].nOverlays].pWork = pLib;
            gLibSlots[pChar->nSlot].overlays[gLibSlots[pChar->nSlot].nOverlays].pCopy =
                StaticMem_Alloc(nSize, 2, 0x40, "char.c", 0xE68);
            Mem_cpy(gLibSlots[pChar->nSlot].overlays[gLibSlots[pChar->nSlot].nOverlays].pCopy,
                    pData, nSize);
            gLibSlots[pChar->nSlot].overlays[gLibSlots[pChar->nSlot].nOverlays].nSize = nSize;
            gLibSlots[pChar->nSlot].overlays[gLibSlots[pChar->nSlot].nOverlays].pChar = pChar;
            gLibSlots[pChar->nSlot].overlays[gLibSlots[pChar->nSlot].nOverlays].nStreamId = nId + 3;
            gLibSlots[pChar->nSlot].overlays[gLibSlots[pChar->nSlot].nOverlays].bActive = bLook;
            gLibSlots[pChar->nSlot].nOverlays++;
            pChar->pLib = StaticMem_Alloc(0x2800, 2, 0x40, "char.c", 0xE75);
        }
        pData += nSize;
    } else {
        pChar->pLib = NULL;
    }

    pChar->pSliderDefs = CharSlider_CreateDefinitionsFromMem(&pData);
    pChar->p4C = pData;
    Character_SetPreferedPos(pChar);
    if (bGolfer) {
        pChar->pClubSet = gClubSkinSets[nSet];
        pChar->nClubHeadBone = CharModel_GetBoneIndex(pChar->pModel, 0x53);
        pChar->fMaxShadowDist = 100.0f;
        pChar->fMaxVisibleDist = 200.0f;
    } else {
        pChar->nClubHeadBone = 0;
        pChar->fMaxShadowDist = 50.0f;
        pChar->fMaxVisibleDist = 100.0f;
    }
    if (pChar->pClubSet != NULL) {
        for (i = 0; i < 6; i++) {
            pSkin = pChar->pClubSet->apSkins[i];
            if (pSkin != NULL) {
                fn_80037AB8(pSkin, pChar->pModel, CharModel_GetBoneIndex(pChar->pModel, 0x52) - 0x52, 0x52);
            }
        }
    }
    pChar->pChoices = pChoices;
    if (bLook) {
        Character_ApplyCrAPSettings(pChar, pChoices);
        SkinPart_BurnBodySkin(pChar);
    }
    if (gSession.nSplitScreen && Character_IsGolfer(pChar)) {
        SKN_AllocRenderData(pChar->pSkin, 0);
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

// Makes a club skin set from a 'CLB ' object: per entry its club class, that class's afC (the club
// head bone's height), the entry's size and, 4 bytes on, its skin (SKN_Create) and a CharSkinRef
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
        pSet->apSkins[nClass] = SKN_Create(pData, 0);
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

// Frees the club skin sets (gClubSkinSets): each one's skins and a9C blocks, then the set. pSet is
// not used: Legacy_Character_CloseModule passes the set it found, but both are freed here.
void Character_FreeClubSkinSets(CharSkinSet* pSet) {
    int j;
    int i;

    for (i = 0; i < 2; i++) {
        if (gClubSkinSets[i] != NULL) {
            for (j = 0; j < 6; j++) {
                if (gClubSkinSets[i]->apSkins[j] != NULL) {
                    fn_80037CD8(gClubSkinSets[i]->apSkins[j]);
                }
                if (gClubSkinSets[i]->a9C[j] != NULL) {
                    CharSkinRef_Free(gClubSkinSets[i]->a9C[j]);
                }
            }
            StaticMem_Free(gClubSkinSets[i]);
            gClubSkinSets[i] = NULL;
        }
    }
}

#define MIN(a, b) ((a) <= (b) ? (a) : (b))
#define MAX(a, b) ((a) <= (b) ? (b) : (a))

// The character's bounding box (vMin / vMax: its bones from bone 1 on, grown by 0.33 each way) and
// the sphere around it that Character_ClipTest tests (vSphereCentre its centre, fSphereRadius half
// its diagonal). Nothing for a model of fewer than two bones.
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
    pChar->vSphereCentre[0] = vCentre[0];
    pChar->vSphereCentre[1] = vCentre[1];
    pChar->vSphereCentre[2] = vCentre[2];
    Char_Vec3Sub(pChar->vMax, pChar->vMin, vDiff);
    pChar->fSphereRadius = (f32)Math_Sqrt(Vec3_LengthSqClamped(vDiff)) / 2.0f;
}

// Tests the character against the current camera. Only for the active player of the current view
// (nPlayer 1000: any character); for another player both answers are 2 (out of view). nClipResult
// is fn_80007D74's answer for its bounding sphere (vSphereCentre, fSphereRadius; 2 = out of view)
// and nShadowClipResult the same for a 3-unit sphere (its shadow); each is 2 as well past its
// distance along the lens (Character_ComputeMaxVisableDistance,
// Character_ComputeMaxVisableShadowDistance; halved in split screen). fNearestCamDist keeps the
// nearest bone 1 has been to the camera; fNearFade is 1 up close, fading to 0 between 6 and 15
// units deep (scaled by the lens's field of view).
void Character_ClipTest(Character* pChar, int nPlayer) {
    f32 (*pMat)[4];
    f32 fDistInViewSpace;
    f32 fDot;
    f32 fDist;
    Sphere sSphereInCamSpace;
    Vec4 xSpherePosInCamSpace;
    f32 vDir[4];

    pMat = Character_GetRootMatrix(pChar);
    if (nPlayer != 1000 && nPlayer
        != ViewController_GetActivePlayerNumber(ViewController_GetCurrentViewControllerID())) {
        pChar->nClipResult = pChar->nShadowClipResult = 2;
        return;
    }
    Vec3Copy(pChar->vSphereCentre, &xSpherePosInCamSpace.x);
    xSpherePosInCamSpace.w = 1.0f;
    LLMath_mat44fltMultiply(((Camera*)RC_spGetCurrentRenderCtx())->viewMtx, &xSpherePosInCamSpace,
                            &xSpherePosInCamSpace);
    Vec3Copy(&xSpherePosInCamSpace.x, &sSphereInCamSpace.x);
    sSphereInCamSpace.radius = pChar->fSphereRadius;
    fDistInViewSpace = sSphereInCamSpace.z;
    pChar->nClipResult = fn_80007D74(&sSphereInCamSpace, RC_spGetCurrentRenderCtx(), 0);
    sSphereInCamSpace.radius = 3.0f;
    pChar->nShadowClipResult = fn_80007D74(&sSphereInCamSpace, RC_spGetCurrentRenderCtx(), 0);
    Char_Vec4Sub(pMat[3], Camera_GetCurrentLens()->m4[3], vDir);
    fDot = Vec3_Dot(Camera_GetCurrentLens()->m4[2], vDir);
    fDist = (f32)Math_Sqrt(Vec3_LengthSqClamped(vDir));
    if (fDist < pChar->fNearestCamDist) {
        pChar->fNearestCamDist = fDist;
    }
    if (fDot > Character_ComputeMaxVisableDistance(pChar, gSession.nSplitScreen)) {
        pChar->nClipResult = 2;
    }
    if (fDot > Character_ComputeMaxVisableShadowDistance(pChar, gSession.nSplitScreen)) {
        pChar->nShadowClipResult = 2;
    }
    fDistInViewSpace *= Camera_GetLensFovScale(Camera_GetCurrentLens());
    if (pChar->nClipResult == 2) {
        pChar->fNearFade = 0.0f;
    } else if (fDistInViewSpace > 15.0f) {
        pChar->fNearFade = 0.0f;
    } else if (fDistInViewSpace < 6.0f) {
        pChar->fNearFade = 1.0f;
    } else {
        pChar->fNearFade = 1.0f - (fDistInViewSpace - 6.0f) / 9.0f;
    }
}

// Each frame before drawing: every character loses bit 0x1000 of uCharFlags, and one with bit 2
// (the flagstick, GoDynObj.c) gets bit 1 (hidden) while the current view has the flag out. Then
// every character whose body or shadow is in view (nClipResult or nShadowClipResult not 2), that is
// not the player fn_800636EC names, has none of bits 0x1000, 0x40 and 1 of uCharFlags and is not
// posed yet (bPosed 0) gets its skins posed on its model (SKN_PoseCharacter).
void Character_PreRenderAll(void) {
    int i;
    int iPlayer2Clip;
    u8 bPreRender;
    int bState;

    fn_80035600();
    SKN_BeginFrame();
    for (i = 0; i < gNumCharacters; i++) {
        iPlayer2Clip = fn_800636EC();
        gCharacters[i]->uCharFlags &= ~0x1000;
        if (gCharacters[i]->uCharFlags & 2) {
            if (ViewController_GetCurrentViewController()->bFlagOut) {
                gCharacters[i]->uCharFlags |= 1;
            } else {
                gCharacters[i]->uCharFlags &= ~1;
            }
        }
        bState = Character_GetClipResult(gCharacters[i]) != 2;
        bPreRender = bState || Character_GetShadowClipResult(gCharacters[i]) != 2;
        bPreRender = bPreRender && iPlayer2Clip != gCharacters[i]->nPlayer;
        bPreRender = bPreRender && !(gCharacters[i]->uCharFlags & 0x1041);
        // fake match: the original turns bPreRender into 0/1 again (neg; or; srwi)
        bPreRender = bPreRender != 0;
        if (bPreRender && gCharacters[i]->bPosed == 0) {
            SKN_PoseCharacter(gCharacters[i], 0);
        }
    }
}

// Draws every character (SKN_DrawCharacter with uFlags) that is in view (nClipResult not 2), is not
// the player fn_800636EC names, is neither hidden (bit 1 of uCharFlags) nor without its textures
// (bit 0x40), and, when uFlags has bit 4, is a golfer. fn_80035604 first; nothing when no character
// is made.
void Character_RenderAll(u32 uFlags) {
    int i;
    int iPlayer2Clip;

    if (gNumCharacters != 0) {
        fn_80035604();
        for (i = 0; i < gNumCharacters; i++) {
            iPlayer2Clip = fn_800636EC();
            if (Character_GetClipResult(gCharacters[i]) != 2 && iPlayer2Clip != gCharacters[i]->nPlayer &&
                !(gCharacters[i]->uCharFlags & 0x41) &&
                (Character_IsGolfer(gCharacters[i]) || (uFlags & 4) == 0)) {
                SKN_DrawCharacter(gCharacters[i], uFlags);
            }
        }
    }
}

// Advances every character's animation by fTime, except in game type 6 while GUI_IsPauseMenuOpen holds.
void Character_UpdateAll(f32 fTime) {
    int i;

    if (gSession.nGameType != 6 || !GUI_IsPauseMenuOpen()) {
        for (i = 0; i < gNumCharacters; i++) {
            Character_UpdateAnimation(gCharacters[i], 0, fTime);
        }
    }
}

// Hangs the club from the hand or from the root, as the clip says. With clip flag 0x10 the club
// bone (nGripBone, bone 0x52) goes back to its parent, the right wrist (nWristBone); otherwise it is
// parented to the root (bit 0x4000 of uCharFlags) and its rotation and offset from the root are
// kept in qGripFromRoot and vGripFromRoot (for a left-hander turned half round and mirrored in z).
// Returns 1 when the attachment changed, 0 when it already was that way.
int Character_UpdateClubAttachment(Character* pChar, Clip* pClip) {
    CharModel* pModel;
    f32 qRoot[4];
    f32 vOffset[4];
    f32 qGrip[4];
    f32 qTurn[4];

    if (pClip->uFlags & 0x10) {
        if (pChar->uCharFlags & 0x4000) {
            pChar->uCharFlags &= ~0x4000;
            pChar->pModel->pBones[pChar->nGripBone].nParent = pChar->nWristBone;
            return 1;
        }
    } else if (!(pChar->uCharFlags & 0x4000)) {
        pModel = pChar->pModel;
        pChar->uCharFlags |= 0x4000;
        pChar->pModel->pBones[pChar->nGripBone].nParent = 0;
        Quat_Invert(pModel->pPoses[0].q0, qRoot);
        Quat_Multiply(pModel->pPoses[pChar->nGripBone].q0, qRoot, pChar->qGripFromRoot);
        if (Character_IsLeftHanded(pChar)) {
            Legacy_Quat_BuildFromPitch(PI, qTurn);
            Quat_Multiply(pChar->qGripFromRoot, qTurn, qGrip);
            Quat_Copy(qGrip, pChar->qGripFromRoot);
        }
        Char_Vec4Sub(pModel->pPoses[pChar->nGripBone].v10, pModel->pPoses[0].v10, vOffset);
        vOffset[3] = 0.0f;
        Quat_RotateVector(qRoot, vOffset, pChar->vGripFromRoot);
        pChar->vGripFromRoot[3] = 0.0f;
        if (Character_IsLeftHanded(pChar)) {
            pChar->vGripFromRoot[2] = -pChar->vGripFromRoot[2];
        }
        return 1;
    }
    return 0;
}

// Plays pClip on the character (nothing for NULL); a golfer's club first follows the clip
// (Character_UpdateClubAttachment). With bNoBlend the blend tree and the animation player start
// over; otherwise the clip is blended in over its first f18 seconds from the current time plus
// fTime. Its SKA tags then start at the blend's start, and the clip's pMtaLib animation plays too
// (CharacterState_PlayClipMorphs).
void Character_PlayClip(Character* pChar, Clip* pClip, int bNoBlend, f32 fTime) {
    f32 aBlend[6];
    SKABlendNode* pNode;
    SKABlendNode* pNew;
    TSKATime* pAnim;
    f32 fLen;

    pNode = &pChar->blend;
    pNew = NULL;
    pAnim = (TSKATime*)pChar->anim;
    if (pClip == NULL) {
        return;
    }
    if (Character_IsGolfer(pChar)) {
        Character_UpdateClubAttachment(pChar, pClip);
    }
    pChar->pMorphLib = NULL;
    if (bNoBlend) {
        SKABlendData_Shutdown(&pNode, 0);
        SKABlendData_Init(&pNode, 1, 0, SKABlender_BlendLinear, 0);
        SKABlender_SetBlender(pNode, SKABlender_BlendLinear, 0.5f);
        SKATime_SetTimeScale((u8*)pAnim, 1.0f);
        pAnim->n00 = 0;
        pAnim->uFlags = 0;
        pAnim->nPlays = -1;
        pAnim->fTime = 0.0f;
    }
    SKABlendData_Init(&pNew, 0, 0, SKABlender_BlendLinear, 1);
    SKAChannel_SetChannel(&pChar->blend, pNew, pClip, 1.0f);
    if (!bNoBlend) {
        SKABlender_ClampT1(&pChar->blend, (TSKATime*)pChar->anim, pChar->fAnimTime + fTime);
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
        SKABlender_AddBlenderData(pChar, pNew, &pNode, aBlend, SKABlender_BlendLinear, 1);
    } else {
        SKABlender_AddBlenderData(pChar, pNew, &pNode, NULL, SKABlender_BlendLinear, 0);
        aBlend[3] = 0.0f;
    }
    pAnim->fStart = pNode->fStart;
    pAnim->fEnd = pNode->fEnd;
    Character_InitSKATags(pChar, pClip, aBlend[3]);
    if (pClip != NULL && pClip->pMtaLib != NULL) {
        CharacterState_PlayClipMorphs(pChar, pClip->pMtaLib, bNoBlend, fTime);
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
        SKABlendData_Shutdown(&pNode, 0);
        pNode = &pChar->morphBlend;  // a node without the root's nGroup
        SKABlendData_Shutdown(&pNode, 0);
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
        if (pChar->pSliderDefs != NULL) {
            CharSlider_Free(pChar->pSliderDefs);
        }
        StaticMem_Free(pChar);
        if (gSession.nGameType == 3) {
            ClipBank_Release(nSlot);
        }
    }
}

// Add a character to the table of characters (up to five); NULL when it is full.
Character* Character_Add(Character* pChar) {
    if (gNumCharacters >= 5) {
        return NULL;
    }
    gCharacters[gNumCharacters] = pChar;
    pChar->nIndex = gNumCharacters;
    gNumCharacters++;
    return pChar;
}

// Starts the characters for a round (GO_vInitIG): the dynamic texture pool, IK on, blend trees of
// up to six clips (four in split screen, gMaxBlendClips), the animation stream, the skin parts (every
// copy changed together) and the skin meshes of each view (SKN_InitTris).
void Character_InitIG(void) {
    int n;
    fn_8009555C();
    CharacterTex_Init();
    SKEL_EnableIK(1);
    n = 6;
    if (gSession.nSplitScreen) {
        n = 4;
    }
    gMaxBlendClips = n;
    AnimStream_Init();
    SkinPart_Init();
    SkinPart_SetChangeAllCopies(1);
    SKN_InitTris();
}

// Shuts down what Character_InitIG started: the skin meshes, the skin parts, the animation stream
// and the dynamic texture pool.
void Character_CloseIG(void) {
    SKN_FreeTris();
    SkinPart_Shutdown();
    AnimStream_Close();
    fn_80095560();
    CharacterTex_Close();
}

// The characters' step at the end of each hole (fn_80095564: outside split screen, every player's
// skin frees what loading it allocated).
void Character_ExitHole(void) {
    fn_80095564();
}

// Starts the characters for the front end (GO_vInitFE): the dynamic texture pool, IK off, blend
// trees of up to three clips (gMaxBlendClips), the skin parts (each copy changed on its own) and the
// skinned-vertex buffer (fn_80112C64).
void Character_InitFE(void) {
    CharacterTex_Init();
    SKEL_EnableIK(0);
    gMaxBlendClips = 3;
    SkinPart_Init();
    SkinPart_SetChangeAllCopies(0);
    SKN_InitModule(1800);
    fn_80112C64(1);
}

// Shuts down what Character_InitFE started: the dynamic texture pool and the skin parts go, and
// SKN_CloseModule and fn_80112CEC free their buffers.
void Character_CloseFE(void) {
    CharacterTex_Close();
    SkinPart_Shutdown();
    SKN_CloseModule();
    fn_80112CEC();
}

ViewSlot gViewSlots[5] = { 0 };

// Starts the character module (fn_8006C7A8, when the front end or a round starts): the animation
// libraries, the 'MAL ' banks and the skeleton module up, the blend tree pools made, no club skin
// sets, the club names read as 64-bit ids, no characters in the front end's or the players' slots,
// and no player marked (gCharTexStreamedPlayer).
// port: the names are read as big-endian 64-bit words from their strings (FEgolferanim compares
//       them with ids read the same way)
void Legacy_Character_InitModule(void) {
    int i;

    SKALIB_InitModule();
    MtaLib_InitModule();
    SKEL_InitModule();
    AnimBlender_InitModule();
    for (i = 0; i < 2; i++) {
        gClubSkinSets[i] = NULL;
    }
    gClubBoneIds[0] = *(u64*)"IGdriver";
    gClubBoneIds[2] = *(u64*)"IGputter";
    gClubBoneIds[3] = *(u64*)"IGiron3";
    gClubBoneIds[4] = *(u64*)"IGiron7";
    gClubBoneIds[5] = *(u64*)"IGwedge";
    for (i = 0; i < CRAP_NUM_GOLFERS; i++) {
        gFEGolferChars[i] = NULL;
    }
    for (i = 0; i < 5; i++) {
        gViewSlots[i].pChar = NULL;
    }
    gCharTexStreamedPlayer = -1;
}

// Shuts the character module down (fn_8006C854): frees the club skin sets and every character made,
// then shuts down the animation libraries, the 'MAL ' banks, the skeleton module and the blend tree
// pools.
void Legacy_Character_CloseModule(void) {
    int i;

    for (i = 0; i < 2; i++) {
        if (gClubSkinSets[i] != NULL) {
            Character_FreeClubSkinSets(gClubSkinSets[i]);
        }
        gClubSkinSets[i] = NULL;
    }
    for (i = 0; i < gNumCharacters; i++) {
        Character_Free(gCharacters[i]);
        gCharacters[i] = NULL;
    }
    gNumCharacters = 0;
    SKALIB_CloseModule();
    MtaLib_CloseModule();
    SKEL_CloseModule();
    AnimBlender_CloseModule();
}

// Frees the front end's golfer characters (gFEGolferChars, one per CrAP golfer slot) and clears the
// slots. FEgolferanim.c only ever sets them to NULL, so there is nothing to free in practice.
void Character_FreeFEGolfers(void) {
    int i;

    for (i = 0; i < CRAP_NUM_GOLFERS; i++) {
        Character_Free(gFEGolferChars[i]);
        gFEGolferChars[i] = NULL;
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

// With a club set, sets the character's club class (0 the drivers, 1 the fairway woods, 2 the
// putter, 3 irons 1-5, 4 irons 6-9, 5 the wedges; Character_SelectGameClub maps the clubs) and
// moves the club head bone (0x53) to the club's length for that class (the set's afC, the bone's y
// offset). Nothing without a model or club set, or for a slot outside 0-2.
void Character_SelectClub(Character* pChar, int nClass) {
    int nClubHeadBone;

    if (pChar == NULL || pChar->pModel == NULL || pChar->nSlot < 0 || pChar->nSlot >= 3) {
        return;
    }
    nClubHeadBone = CharModel_GetBoneIndex(pChar->pModel, 0x53);
    if (pChar->pClubSet != NULL) {
        pChar->nClubClass = nClass;
        pChar->pModel->pBones[nClubHeadBone].v1C[1] = pChar->pClubSet->afC[pChar->nClubClass];
    }
}

// Sets the key the character's clips are looked up by (nClipKey, Char_SetClip) to nShotType. With
// p1798's mode (n2C) 6 and nShotType 0 it first stores 4, which the next line overwrites at once:
// the key always ends as nShotType (Character_SetupForShot's same test keeps its 4).
void Character_SelectShotType(Character* pChar, int nShotType) {
    // fake match: n2C is compared unsigned here
    if (pChar->p1798 != NULL && (u32)pChar->p1798->n2C == 6 && nShotType == 0) {
        pChar->nClipKey = 4;
    }
    pChar->nClipKey = nShotType;
}

// The player's golfer takes the player's shot kind and club (Character_SelectGameShotType,
// Character_SelectGameClub). When either changed, its animation restarts: nTargetState 0,
// uCharFlags bit 0x80 set, animation 5 asked for (fn_80095744), and a full set-up for the shot
// requested (Character_AlignShotWithTarget with both flags).
void Character_InitNewClubAndShotType(int nPlayer) {
    Player* pPlayer = &gPlayers[nPlayer];
    Character* pChar = pPlayer->pChar;
    int nPrevShotKind = pChar->nShotKind;
    int nPrevClub;

    Character_SelectGameShotType(pChar, pPlayer->nShotKind);
    nPrevClub = pChar->nClub;
    Character_SelectGameClub(pChar, pPlayer->nClub);
    if (nPrevClub != pPlayer->nClub || nPrevShotKind != pPlayer->nShotKind) {
        pChar->nTargetState = 0;
        pChar->uCharFlags |= 0x80;
        fn_80095744(pChar, 5);
        Character_AlignShotWithTarget(nPlayer, 1, 1);
    }
}

// Per shot kind, the clip key Character_SelectGameShotType sets with it.
s32 gShotKindClipKeys[8] = { 8, 0, 6, 3, 2, 1, 10, 4 };

// Per club class, where the golfer stands from the ball (Character_SetupForShot): x back along
// its root's x axis, y up, z back along its z axis (that offset turned round for a left-hander).
f32 gClubStanceOffsets[6][3] = {
    { 0.058f, 0.0f, 0.025f },
    { 0.058f, 0.0f, 0.025f },
    { 0.045f, -0.024f, 0.05f },
    { 0.04f, 0.0f, 0.075f },
    { 0.04f, 0.0f, 0.075f },
    { 0.045f, -0.0f, 0.075f },
};

// Sets the character's shot kind (the player's nShotKind) and the clip key it maps to
// (gShotKindClipKeys, through Character_SelectShotType). Nothing for NULL.
void Character_SelectGameShotType(Character* pChar, int nKind) {
    if (pChar != NULL) {
        Character_SelectShotType(pChar, gShotKindClipKeys[nKind]);
        pChar->nShotKind = nKind;
    }
}

// Sets the character's club (Player.nClub, CLUB_DRIVER1_e to CLUB_PUTTER_e) and, through
// Character_SelectClub, its club class: 0 the drivers, 1 the fairway woods, 3 irons 1-5, 4 irons
// 6-9, 5 the wedges, 2 the putter. The club also goes to the situation device's state
// (fn_800BBADC). Nothing for NULL.
void Character_SelectGameClub(Character* pChar, int nClub) {
    // per club: what Character_SelectClub gets
    int aClass[26] = {0, 0, 0, 0, 0, 0, 1, 1, 1, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 2};

    if (pChar != NULL) {
        fn_800BBADC(nClub);
        pChar->nClub = nClub;
        Character_SelectClub(pChar, aClass[nClub]);
    }
}

// Sets the emotion (nStyle) the character's clips are picked with (Char_SetClip's style);
// CharacterState_UpdateGameEmotionState and CharacterState_SetTapInState set it.
void Character_SetEmotion(Character* pChar, int nStyle) {
    pChar->nStyle = nStyle;
}

// Asks for the player's golfer to be set up for its shot on its next animation update (uCharFlags
// bit 4: Character_UpdateAnimation runs Character_SetupForShot): at its ball, facing the target.
// bInitIK (bit 8, set or cleared) has it also take its stance and leg IK; bResetPos (bit 0x200,
// only set) has the ground under its feet sampled again.
void Character_AlignShotWithTarget(int nPlayer, u8 bResetPos, u8 bInitIK) {
    Character* pChar = gPlayers[nPlayer].pChar;
    pChar->uCharFlags |= 4;
    if (bInitIK) {
        pChar->uCharFlags |= 8;
    } else {
        pChar->uCharFlags &= ~8;
    }
    if (bResetPos) {
        pChar->uCharFlags |= 0x200;
    }
}

// fake match: the index passed through an inline's parameter numbers the nPlayer read below the
// shared uCharFlags read, which gives EA's registers.
static inline f32* fn_8001C860_Get(int nPlayer) { return gPlayers[nPlayer].ball.vPos; }

// Puts the golfer at its player's ball, facing the target (level), and resets its root bone's pose
// and matrix. With uCharFlags bit 8 and a skeleton, it then takes the stance of its clip for the
// style: the clip is started on the skeleton when it changed (at its event 2's time with bit
// 0x10000), the root is moved by the club class's offset (mirrored when the model is), the pose
// updated, the feet placed and both leg chains moved with the root, and the IK weight set (1 in the
// swing's states 5 and 7). Bits 4, 8, 0x200 and 0x10000 of uCharFlags are cleared; with 0x200 the
// ground under the feet is sampled again first.
void Character_SetupForShot(Character* pChar) {
    f32 vDir[4];
    f32 vOffsetX[4];
    f32 vOffsetZ[4];
    f32 vPos[4];
    int bInitIK;
    int bResetPos;
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
    bResetPos = pChar->uCharFlags & 0x200;
    bInitIK = pChar->uCharFlags & 8;
    bClipTime = pChar->uCharFlags & 0x10000;
    pBallPos = fn_8001C860_Get(pChar->nPlayer);
    Character_SetPosition(pChar, pBallPos, 0);
    pChar->uCharFlags &= ~(0x10000 | 0x200 | 8 | 4);
    // fake match: n2C is compared unsigned here
    if (pChar->p1798 != NULL && (u32)pChar->p1798->n2C == 6 && pChar->nClipKey == 0) {
        pChar->nClipKey = 4;
    }
    Char_Vec4Sub(pPlayer->vTarget, pBallPos, vDir);
    vDir[1] = 0.0f;
    Character_SetOrientationVec(pChar, vDir, 0.0f);
    Quat_Copy(pModel->pBones[0].q0C, pModel->pPoses[0].q0);
    Quat_Copy(pModel->pBones[0].v1C, pModel->pPoses[0].v10);
    Quat_QuatToMatrix(pModel->pPoses[0].q0, pModel->pMatrices[0]);
    Vec4_CopyPoint(pModel->pPoses[0].v10, pModel->pMatrices[0][3]);
    if (bInitIK && (pSkel = pChar->pModel->pSkel) != NULL) {
        pOldClip = pChar->pCurClip;
        CharModel_GetBoneIndex(pChar->pModel, 1);      // the results are not used
        CharModel_GetBoneIndex(pChar->pModel, 0x52);
        pClip = Char_SetClip(pChar, 0, pChar->nStyle, NULL);
        if (pClip->pD8 == NULL) {
            SKEL_ResetIKSkeleton(pSkel);
            SKEL_SetIKSolutionWeight(pChar->pModel->pSkel, 0.0f);
            pChar->pModel->pSkel->fIKBlendLeft = 0.0f;
            return;
        }
        if (pSkel->pClip != pClip) {
            pSkel->pClip = pClip;
            SKA_SetLeftHanded(pChar->pModel->bLeftHanded);
            if (bClipTime) {
                SKA_Update(pChar, pClip, &pSkel->pose, 0, pClip->pEvents[2].fTime);
            } else {
                SKA_Update(pChar, pClip, &pSkel->pose, 0, 0.0f);
            }
        }
        Character_UpdateClubAttachment(pChar, pSkel->pClip);
        LLMath_Scale(-gClubStanceOffsets[pChar->nClubClass][0], pChar->pModel->pMatrices[0][0], vOffsetX);
        LLMath_Scale(-gClubStanceOffsets[pChar->nClubClass][2], pChar->pModel->pMatrices[0][2], vOffsetZ);
        if (pChar->pModel->bLeftHanded) {
            vOffsetZ[0] = -vOffsetZ[0];
            vOffsetZ[2] = -vOffsetZ[2];
        }
        Char_Vec4Add(vOffsetX, pChar->pModel->pBones[0].v1C, pChar->pModel->pBones[0].v1C);
        Char_Vec4Add(vOffsetZ, pChar->pModel->pBones[0].v1C, pChar->pModel->pBones[0].v1C);
        Quat_Copy(pModel->pBones[0].v1C, pModel->pPoses[0].v10);
        Vec4_CopyPoint(pModel->pPoses[0].v10, pModel->pMatrices[0][3]);
        SKEL_ResetIKSkeleton(pSkel);
        if (pChar->pClubSet != NULL) {
            pSkel->pose.aBones[pChar->nClubHeadBone].v10[1] = pChar->pClubSet->afC[pChar->nClubClass];
        }
        SKEL_UpdateState(pChar->pModel, &pSkel->pose, 1);
        Character_UpdateTestPoints(pChar);
        if (bResetPos) {
            pChar->nFootPointStart = -1;
            Character_UpdateFeetTerrainInfo(pChar, 1);
        }
        fY = pChar->pModel->pBones[0].v1C[1];
        Character_PlaceFeetOnGround(pChar);
        fY = pChar->pModel->pBones[0].v1C[1] - fY;
        SKEL_TranslateIKChainY(pChar->pModel, &pChar->pModel->pSkel->pChains[0], fY);
        SKEL_TranslateIKChainY(pChar->pModel, &pChar->pModel->pSkel->pChains[1], fY);
        Char_Vec4Add(gPlayers[pChar->nPlayer].ball.vPos, vOffsetX, vPos);
        Char_Vec4Add(vPos, vOffsetZ, vPos);
        vPos[1] += gClubStanceOffsets[pChar->nClubClass][1];
        SKEL_InitIKSkeleton(pChar, vPos, bResetPos);
        pChar->pModel->pSkel->nIKClipKey = pChar->nClipKey;
        pChar->pModel->pSkel->nIKClubClass = pChar->nClubClass;
        if (((pChar->nCurState == 5 || pChar->nTargetState == 5) && CharacterState_IsNotFidgeting(pChar)) ||
            pChar->nCurState == 7) {
            SKEL_SetIKSolutionWeight(pChar->pModel->pSkel, 1.0f);
        } else {
            SKEL_SetIKSolutionWeight(pChar->pModel->pSkel, 0.0f);
        }
        if (pOldClip != NULL) {
            Character_UpdateClubAttachment(pChar, pOldClip);
        }
    }
    if (pChar->pModel->pSkel != NULL) {
        pChar->pModel->pSkel->fIKBlendLeft = 0.0f;
    }
}

// The in-game 'CLB ' (club models) stream handler: unless a club skin set is loaded already
// (gClubSkinSets[0]), makes one from the object (Character_CreateClubSkinSet), and in split screen a
// second one for the second view (gClubSkinSets[1], else NULL). The object is freed either way.
void Character_ClubStreamCallbackIG(UStreamObject* pObject) {
    if (gClubSkinSets[0] == NULL) {
        if (gSession.nSplitScreen) {
            gClubSkinSets[0] = Character_CreateClubSkinSet(pObject->pData);
            gClubSkinSets[1] = Character_CreateClubSkinSet(pObject->pData);
        } else {
            gClubSkinSets[0] = Character_CreateClubSkinSet(pObject->pData);
            gClubSkinSets[1] = NULL;
        }
    }
    StaticMem_Free(pObject);
}

// The front end's 'CLB ' stream handler: as Character_ClubStreamCallbackIG, with one set only.
void Character_ClubStreamCallbackFE(UStreamObject* pObject) {
    if (gClubSkinSets[0] == NULL) {
        gClubSkinSets[0] = Character_CreateClubSkinSet(pObject->pData);
        gClubSkinSets[1] = NULL;
    }
    StaticMem_Free(pObject);
}

// Registers Character_ClubStreamCallbackIG for 'CLB ' objects (streammanagerhole's in-game list).
void Character_RegisterClubStreamClientIG(void) {
    Stream_RegisterLoadChunkCallback('CLB ', Character_ClubStreamCallbackIG);
}

void Character_RegisterClubStreamClientFE(void) {
    Stream_RegisterLoadChunkCallback('CLB ', Character_ClubStreamCallbackFE);
}

void Character_UnregisterClubStreamClient(void) {
    Stream_UnregisterLoadChunkCallback('CLB ');
}

// The in-game 'CHR ' (golfer model) stream handler: for every player whose golfer has this model
// (the object's id) and who has no character yet, makes one (Character_CreateFromMem, with texture
// set i in split screen, else 0, and the player's saved look), texture lookups kept to the player's
// group meanwhile (fn_800106AC, fn_800106B8). A golfer with a body skin is dressed
// (Character_SetClubsAndClothes), its seven skins (the body, then the club set's six) listed and
// their textures loaded. The object is freed.
void Character_GolferStreamCallbackIG(UStreamObject* pObject) {
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
            gViewSlots[i].pChar =
                Character_Add(Character_CreateFromMem(pObject->pData, 0, nSet, uModel,
                                                      Character_IsCrAPGolfer(i), &gpSaveData[i].choices));
            if (gViewSlots[i].pChar->pSkin != NULL && Character_IsGolfer(gViewSlots[i].pChar)) {
                Character_SetClubsAndClothes(gViewSlots[i].pChar, i);
            }
            if (gViewSlots[i].pChar->pSkin != NULL && Character_IsGolfer(gViewSlots[i].pChar)) {
                pChar = gViewSlots[i].pChar;
                pChar->apSkins[0] = pChar->pSkin;
                pChar->apSkins[1] = pChar->pClubSet->apSkins[0];
                pChar->apSkins[2] = pChar->pClubSet->apSkins[1];
                pChar->apSkins[3] = pChar->pClubSet->apSkins[2];
                pChar->apSkins[4] = pChar->pClubSet->apSkins[3];
                pChar->apSkins[5] = pChar->pClubSet->apSkins[4];
                pChar->apSkins[6] = pChar->pClubSet->apSkins[5];
                pChar->nSkins = 7;
                Character_LoadTextures(pChar, pChar->apSkins, pChar->nSkins);
            }
        }
    }
    fn_800106B8(0);
    StaticMem_Free(pObject);
}

// Registers Character_GolferStreamCallbackIG for 'CHR ' objects (streammanagerhole's in-game list).
void Character_RegisterGolferStreamClientIG(void) {
    Stream_RegisterLoadChunkCallback('CHR ', Character_GolferStreamCallbackIG);
}

// The front end's 'CHR ' stream handler, for the golfer the create-a-player menu is loading
// (gpCrAPState->pB8): the UI file is parked in ARAM so its buffer can take a copy of the object
// (header and data), and the object is freed; the character is made from the copy (the static heap
// counting what it takes) and its textures loaded, then the UI file is brought back. The character
// is placed at gFEGolferPos facing f19C, given the profile's created-golfer look for golfers 7 and
// 29 (Character_ApplyCrAPSettings), set to club class 5 (the wedges) and its first clip. When it
// is still the golfer the menu wants (pB8->nGolferId is n8C), its body and six club skins are
// listed on it (created golfers with one texture pool entry get two) and it takes its pool entries.
void Character_GolferStreamCallbackFE(UStreamObject* pObject) {
    UStreamObject* pCopy;
    Character* pChar;
    Clip* pClip;

    StaticMem_ResetCount();
    StaticMem_StartCount();
    UI_ParkFileInAram();
    pCopy = UI_GetFileBuffer();
    Mem_cpy(pCopy, pObject, pObject->uSize + 0x80);
    StaticMem_Free(pObject);
    pCopy->pData = (u8*)pCopy + 0x80;
    gpCrAPState->pB8->pChar = Character_CreateFromMem(pCopy->pData, 0, 0, pCopy->uId, 0, NULL);
    StaticMem_StopCount();
    StaticMem_GetCount();
    Character_LoadTextures(gpCrAPState->pB8->pChar, NULL, 0);
    UI_RestoreFileFromAram();
    gpCrAPState->pB8->pChar->nPlays = -1;
    Character_SetPosition(gpCrAPState->pB8->pChar, gFEGolferPos, 1);
    Character_SetOrientation(gpCrAPState->pB8->pChar, gpCrAPState->fFacing);
    if (gpCrAPState->pB8->pChar->nGolferId == 7 || gpCrAPState->pB8->pChar->nGolferId == 29) {
        Character_ApplyCrAPSettings(gpCrAPState->pB8->pChar, &FE_GetCurrentProfile()->choices);
    }
    Character_SelectClub(gpCrAPState->pB8->pChar, 5);
    pClip = Char_SetClip(gpCrAPState->pB8->pChar, 0, 0, NULL);
    Character_PlayClip(gpCrAPState->pB8->pChar, pClip, 1, 0.0f);
    if (gpCrAPState->pB8->nGolferId == gpCrAPState->n8C) {
        pChar = gpCrAPState->pB8->pChar;
        pChar->nSkins = 7;
        pChar->apSkins[0] = pChar->pSkin;
        pChar->apSkins[1] = pChar->pClubSet->apSkins[0];
        pChar->apSkins[2] = pChar->pClubSet->apSkins[1];
        pChar->apSkins[3] = pChar->pClubSet->apSkins[2];
        pChar->apSkins[4] = pChar->pClubSet->apSkins[3];
        pChar->apSkins[5] = pChar->pClubSet->apSkins[4];
        pChar->apSkins[6] = pChar->pClubSet->apSkins[5];
        if ((pChar->nGolferId == 7 || pChar->nGolferId == 29) && pChar->nDynTex == 1) {
            pChar->nDynTex = 2;
        }
        CharacterTex_TakePoolEntries(pChar);
    }
}

void Character_RegisterGolferStreamClientFE(void) {
    Stream_RegisterLoadChunkCallback('CHR ', Character_GolferStreamCallbackFE);
}

void Character_UnregisterGolferStreamClient(void) {
    Stream_UnregisterLoadChunkCallback('CHR ');
}

// Draws the shadow of every skeletal object (a character with no player, nPlayer 1000, from a
// 'SKLO' object, such as the flag) that is not hidden (uCharFlags bits 0x01 and 0x40) and whose
// shadow is in view (nShadowClipResult not 2): into the shadow texture (fn_800B28D4) and onto the
// ground (fn_800B2FB0). gomainloop calls it in single view only.
void SkeletalObject_RenderShadowsAll(void) {
    int i;

    for (i = 0; i < gNumCharacters; i++) {
        if (gCharacters[i]->nPlayer == 1000 && gCharacters[i]->nShadowClipResult != 2 &&
            !(gCharacters[i]->uCharFlags & 0x41)) {
            fn_800B28D4(gCharacters[i], 1, 0);
            fn_800B2FB0(gCharacters[i], 1, 0);
        }
    }
}

// The skeletal object (a character with no player, nPlayer 1000) built from the 'SKLO' object with
// this id, or NULL. Id 100 is the flag (GoDynObj, GoTerrain, GameTargets).
Character* SkeletalObject_FindObject(int nId) {
    int i;
    for (i = 0; i < gNumCharacters; i++) {
        if (gCharacters[i]->nPlayer == 1000 && gCharacters[i]->uId == nId) {
            return gCharacters[i];
        }
    }
    return NULL;
}

// Run Character_ClipTest on every character with no player (the 'SKLO' ones).
void SkeletalObject_ClipTestAll(void) {
    int i;
    for (i = 0; i < gNumCharacters; i++) {
        if (gCharacters[i]->nPlayer == 1000) {
            Character_ClipTest(gCharacters[i], 1000);
        }
    }
}

// The 'SKLO' handler: a character built from the object with no player (1000), keyed by the
// object's id.
// port: the skeleton is little-endian on disc and Character_CreateFromMem swaps it
//       (BYTESWAP_SWAPDATA): a little-endian port does not swap there.
void SkeletalObject_StreamCallback(UStreamObject* pObject) {
    Character* pChar =
        Character_Add(Character_CreateFromMem(pObject->pData, 0, 0, pObject->uId, 0, NULL));
    pChar->nPlayer = 1000;
    pChar->uId     = pObject->uId;
    StaticMem_Free(pObject);
}

void SkeletalObject_RegisterStreamClient(void) {
    Stream_RegisterLoadChunkCallback('SKLO', SkeletalObject_StreamCallback);
}

void SkeletalObject_UnregisterStreamClient(void) {
    Stream_UnregisterLoadChunkCallback('SKLO');
}

// Dresses the character of player slot nSlot: its club skins (in game type 3 golfers 7 and 29 get
// the profile's created golfer's look; otherwise its own look when Character_IsCrAPGolfer says
// so), then the "shirt" set (only when Character_IsCrAPGolfer says no) and the "glove" set, as
// "shirt<n>" / "glove<n>" with n from the slot's profile (no number when it is 0 or less).
void Character_SetClubsAndClothes(Character* pChar, int nSlot) {
    char szName[32];            // the size is not known

    if (gSession.nGameType == 3) {
        if (pChar->nGolferId == 7 || pChar->nGolferId == 29) {
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

// Asks for the create-a-player menu's golfer to be dressed again (gSession.aD2D[n];
// Character_UpdateClothesFE does it). FE_CrAPDB calls it when an asset or logo is turned on or off.
void Character_RequestClothesUpdateFE(int n) {
    gSession.aD2D[n] = 1;
}

// Each frame in the create-a-player mode (game type 3): for every flag
// Character_RequestClothesUpdateFE set, once the shown golfer (gpCrAPState->pB4) is ready
// (bLoaded), the menu golfer's state machine is not in state 4 and nothing holds it
// (FE_IsTextureSwapDone), the flag is cleared and the running state aborted for state 4, which
// dresses the shown golfer again and swaps its textures (FE_StreamFunc_SwapTexturesInit).
void Character_UpdateClothesFE(void) {
    int i;

    for (i = 0; i < 5; i++) {
        if (gSession.aD2D[i] && gpCrAPState->pB4 != NULL && gpCrAPState->pB4->bLoaded &&
            FE_StreamGetCurrentState() != 4 && FE_IsTextureSwapDone()) {
            gSession.aD2D[i] = 0;
            FE_StreamInterruptState();
            FE_StreamSetNextState(4);
        }
    }
}

// Asks for player n's golfer to be dressed again (gSession.aD28[n]; Character_UpdateClothesIG does
// it).
void Character_RequestClothesUpdateIG(int n) {
    gSession.aD28[n] = 1;
}

// Each frame in game (gomainloop, outside game types 1 and 3): every player
// Character_RequestClothesUpdateIG flagged has its golfer dressed again once the GPU is idle
// (fn_80008380, Character_SetClubsAndClothes), its skin choices copied from set 1 to set 0
// (Character_CopySkinChoices1To0) and its skins' materials set up again for its current dynamic
// texture (SkinPart_SetupMaterials); the flag is cleared.
void Character_UpdateClothesIG(void) {
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
                SkinPart_SetupMaterials(pChar->apSkins[j], pChar->apDynTex[pChar->nCurDynTex]);
            }
            gSession.aD28[i] = 0;
        }
    }
}

// Puts a golfer to sleep at the end of its turn (GM_EndOfGolferTurn): its animation blending reset
// (Character_ResetBlenders), the shot set-up requests (uCharFlags bits 4, 8 and 0x200) dropped, and
// bit 0x40 set, so neither it nor its shadow is drawn until Character_PrepareForRendering wakes it.
void Character_Sleep(Character* pChar) {
    Character_ResetBlenders(pChar);
    pChar->uCharFlags = pChar->uCharFlags & ~0x20C;
    pChar->uCharFlags = pChar->uCharFlags | 0x40;
}

// Resets the character's animation blending: each of its two blend trees (blend, morphBlend) is
// given back and rebuilt as one empty node mixing its children with SKABlender_BlendLinear at
// weight 0.5 (pose format 0 for the first, 1 for the second), both animation players are set back
// to animation 0 at time 0, and the animation state (nMorphTargetState, nMorphCurState,
// nTargetState, nCurState, uSKAFlags, uMorphFlags) is cleared.
void Character_ResetBlenders(Character* pChar) {
    SKABlendNode* pNode;

    pNode = &pChar->blend;
    SKABlendData_Shutdown(&pNode, 0);
    SKABlendData_Init(&pNode, 1, 0, SKABlender_BlendLinear, 0);
    SKABlender_SetBlender(pNode, SKABlender_BlendLinear, 0.5f);
    pNode = &pChar->morphBlend;
    SKABlendData_Shutdown(&pNode, 0);
    SKABlendData_Init(&pNode, 1, 1, SKABlender_BlendLinear, 1);
    SKABlender_SetBlender(pNode, SKABlender_BlendLinear, 0.5f);
    CharacterState_SetTransition(&pChar->morphAnim, 0, 0.0f);
    pChar->nMorphTargetState = 0;
    pChar->nMorphCurState = 0;
    CharacterState_SetTransition((TSKATime*)pChar->anim, 0, 0.0f);
    pChar->nTargetState = 0;
    pChar->nCurState = 0;
    pChar->uSKAFlags = 0;
    pChar->uMorphFlags = 0;
}

// Wakes the player's golfer for its turn (stateFunc's PreShot, InitialFlyBy and PlaceBall):
// uCharFlags bit 0x40 cleared so it is drawn again (Character_Sleep set it), its body skin made the
// loaded one in single view (fn_800955F0) and its textures streamed
// (CharacterTex_StartStreamingPlayers). In single view, when it is not the golfer last prepared
// (gCharTexStreamedPlayer), it is also flagged to be dressed again
// (Character_RequestClothesUpdateIG).
void Character_PrepareForRendering(int nPlayer) {
    gPlayers[nPlayer].pChar->uCharFlags &= ~0x40;
    fn_800955F0(nPlayer);
    CharacterTex_StartStreamingPlayers(nPlayer);
    if (!gSession.nSplitScreen && gCharTexStreamedPlayer != nPlayer) {
        Character_RequestClothesUpdateIG(nPlayer);
        gCharTexStreamedPlayer = nPlayer;
    }
}

// Where the ball sits in the golfer's fingers (pPos): the index finger bone's position (0x1A,
// "ri2"; the left hand's for a left-hander, Character_GetBoneMatrixSwapIfLefty) moved 0.05 against
// the bone's x axis (along it for a left-hander).
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

// Where the tee held in the golfer's hand goes (pPos) and its rotation (pAngles, three Euler angles
// in radians; stateFunc's PreShot update places the tee with them): the index finger bone's
// position (0x1A, the left hand's for a left-hander) moved 0.000625 along the bone's x axis
// (against it for a left-hander), and the wrist bone's pose (0x15, CharModel_GetBoneIndexMapped) as
// angles (Quat_ExtractEulerAngles), the second turned 30 degrees more. Nothing without a character
// or model.
void Character_GetTeeInHandPositionAndRot(Character* pChar, f32* pPos, f32* pAngles) {
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

// Where the golfer will stand when its current clip ends (pOut): the clip's end offset (v80)
// through bone id 0's matrix; without a clip, bone id 0's position. GM_ShowPostShotAnimation checks
// the ground there.
void Character_GetEndOfAnimationPosition(Character* pChar, f32* pOut) {
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

// Empties the character's four key-frame buffers (ska_shared.c's cache of a clip's decoded keys:
// n00 -1, no clip), so the next pose decodes its keys again; AnimStream calls it when it hands out
// the player's streamed clip data. The buffers' memory is kept.
void Character_ClearKeyFrameBuffers(Character* pChar) {
    int i;
    for (i = 0; i < 4; i++) {
        pChar->buffers[i].n00 = -1;
        pChar->buffers[i].p04 = NULL;
        pChar->buffers[i].p0C = NULL;
        pChar->buffers[i].p10 = NULL;
        pChar->buffers[i].p14 = NULL;
    }
}

// 1 when the golfer's current clip animates the ball bone (0x54, "GBall1"): the model has that bone
// and the clip's track count (n1C) reaches past its index. Its callers then put the ball at that
// bone (stateFunc's ball removal, the create-a-player golfer's ball in hand).
u8 Character_IsHoldingBall(Character* pChar) {
    if (pChar->pCurClip != NULL && CharModel_GetBoneIndex(pChar->pModel, 0x54) != 0xFF &&
        pChar->pCurClip->n1C > CharModel_GetBoneIndex(pChar->pModel, 0x54)) {
        return 1;
    }
    return 0;
}

// Gives the character a created golfer's look from pChoices: its skins' choices
// (SkinPart_ApplyBodyChoices), its 26 body sliders (a9B4) and its handedness (bLeftHanded non-zero:
// left-handed, the model's bLeftHanded), the handedness except in the create-a-player mode (game
// type 3) off its screens 1 and 4 (gpCrAPState->nScreenKind); then the skeleton is set up again
// from the model (Character_SetSkeleton).
void Character_ApplyCrAPSettings(Character* pChar, SkinChoices* pChoices) {
    SkinPart_ApplyBodyChoices(pChar, pChoices);
    CharSlider_UpdateCharacterBasedOnSliderValues(pChar->pSliderDefs, pChar->pModel, pChar->pSkin, 26,
                                                  pChoices->a9B4,
                &pChar->morphBlend);
    if (gSession.nGameType != 3 || gpCrAPState->nScreenKind == 1 || gpCrAPState->nScreenKind == 4) {
        if (pChoices->bLeftHanded == 0) {
            Character_SetLeftHanded(pChar, 0);
        } else {
            Character_SetLeftHanded(pChar, 1);
        }
    }
    Character_SetSkeleton(pChar, pChar->pModel);
}

// Byte-swaps the character file's texture table in place (nBytes of 0x50-byte TexEntry rows), once:
// bit 0x40 of the first row's b47 marks it done. In each row the 8-byte name hash stays; the four
// mip records (a 4-byte field, four 2-byte ones), nWidth, nHeight, nPalette and n3E are swapped,
// the nine single bytes after them stay and the last seven bytes are reversed as one field. A row
// with b40 0 has its name unpacked (into a buffer nothing reads) and, when the row before has the
// same name and b47 bit 0 (the next texture goes with it), takes that bit too.
void Character_SwapTexEntries(u8* pData, int nBytes) {
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
                SKA_UnpackName(&pEntry->u0, szName);
                if (i != 0 && pEntry->u0 == pEntry[-1].u0 && (pEntry[-1].b47 & 1)) {
                    pEntry->b47 |= 1;
                }
            }
            pEntry->b47 |= 0x40;
            pEntry++;
        }
    }
}

// Byte-swaps the character file's palette table in place (nBytes of 12-byte TexPalette rows): a
// 4-byte field, then four 2-byte ones.
void Character_SwapTexPalettes(u8* pData, int nBytes) {
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

// Dresses the character's six club skins (driver, fairway wood, putter, 3 and 7 irons, wedge: the
// gClubPartNames classes): from pChoices when given (SkinPart_ApplyClubChoices), else from the
// golfer's gGolferTable row found by its id nGolferId (row 7 itself for golfer 7 while
// gSession.uFlags has 0x4000), each class's part variant and its head, shaft and grip sets; then
// the clubs are mirrored for a left-handed golfer. In the front end (game type 3) a call while the
// menu golfer's b1D1 is clear only sets it (FE_SetClubStatesAllowed) and returns. nSlot is not
// used; nothing without a character or its club set.
void Character_SetClubStatesForCharacter(Character* pChar, int nSlot, SkinChoices* pChoices) {
    int nGolfer;
    u64 uName;
    u64 uVariant;

    if (pChar == NULL || pChar->pClubSet == NULL) return;
    if (gSession.nGameType == 3 && !FE_GetClubStatesAllowed()) {
        FE_SetClubStatesAllowed(1);
        return;
    }
    if (pChoices == NULL) {
        nGolfer = Golfer_FindById(pChar->nGolferId);
        if ((gSession.uFlags & 0x4000) && pChar->nGolferId == 7) {
            nGolfer = 7;
        }
        if (nGolfer >= 0) {
            SKA_PackName(&uName, gClubPartNames[0]);
            SkinPart_ChooseClubPartVariant(pChar, 0, uName, gGolferTable[nGolfer].aClubs[0].uPart);
            SKA_PackName(&uName, gClubHeadSetNames[0]);
            SKA_PackName(&uVariant, gClubHeadVariantNames[0]);
            SkinPart_ChooseClubSet(pChar, 0, uName, uVariant, gGolferTable[nGolfer].aClubs[0].uModel);
            SKA_PackName(&uName, gClubShaftSetNames[0]);
            SKA_PackName(&uVariant, gClubShaftVariantNames[0]);
            SkinPart_ChooseClubSet(pChar, 0, uName, uVariant, gGolferTable[nGolfer].aClubs[0].uShaft);
            SKA_PackName(&uName, gClubGripSetNames[0]);
            SKA_PackName(&uVariant, gClubGripVariantNames[0]);
            SkinPart_ChooseClubSet(pChar, 0, uName, uVariant, gGolferTable[nGolfer].aClubs[0].uGrip);

            SKA_PackName(&uName, gClubPartNames[1]);
            SkinPart_ChooseClubPartVariant(pChar, 1, uName, gGolferTable[nGolfer].aClubs[1].uPart);
            SKA_PackName(&uName, gClubHeadSetNames[1]);
            SKA_PackName(&uVariant, gClubHeadVariantNames[1]);
            SkinPart_ChooseClubSet(pChar, 1, uName, uVariant, gGolferTable[nGolfer].aClubs[1].uModel);
            SKA_PackName(&uName, gClubShaftSetNames[1]);
            SKA_PackName(&uVariant, gClubShaftVariantNames[1]);
            SkinPart_ChooseClubSet(pChar, 1, uName, uVariant, gGolferTable[nGolfer].aClubs[1].uShaft);
            SKA_PackName(&uName, gClubGripSetNames[1]);
            SKA_PackName(&uVariant, gClubGripVariantNames[1]);
            SkinPart_ChooseClubSet(pChar, 1, uName, uVariant, gGolferTable[nGolfer].aClubs[1].uGrip);

            SKA_PackName(&uName, gClubPartNames[3]);
            SkinPart_ChooseClubPartVariant(pChar, 3, uName, gGolferTable[nGolfer].aIronPart[0]);
            SKA_PackName(&uName, gClubHeadSetNames[3]);
            SKA_PackName(&uVariant, gClubHeadVariantNames[3]);
            SkinPart_ChooseClubSet(pChar, 3, uName, uVariant, gGolferTable[nGolfer].uIronModel);
            SKA_PackName(&uName, gClubShaftSetNames[3]);
            SKA_PackName(&uVariant, gClubShaftVariantNames[3]);
            SkinPart_ChooseClubSet(pChar, 3, uName, uVariant, gGolferTable[nGolfer].uIronShaft);
            SKA_PackName(&uName, gClubGripSetNames[3]);
            SKA_PackName(&uVariant, gClubGripVariantNames[3]);
            SkinPart_ChooseClubSet(pChar, 3, uName, uVariant, gGolferTable[nGolfer].uIronGrip);

            SKA_PackName(&uName, gClubPartNames[4]);
            SkinPart_ChooseClubPartVariant(pChar, 4, uName, gGolferTable[nGolfer].aIronPart[1]);
            SKA_PackName(&uName, gClubHeadSetNames[4]);
            SKA_PackName(&uVariant, gClubHeadVariantNames[4]);
            SkinPart_ChooseClubSet(pChar, 4, uName, uVariant, gGolferTable[nGolfer].uIronModel);
            SKA_PackName(&uName, gClubShaftSetNames[4]);
            SKA_PackName(&uVariant, gClubShaftVariantNames[4]);
            SkinPart_ChooseClubSet(pChar, 4, uName, uVariant, gGolferTable[nGolfer].uIronShaft);
            SKA_PackName(&uName, gClubGripSetNames[4]);
            SKA_PackName(&uVariant, gClubGripVariantNames[4]);
            SkinPart_ChooseClubSet(pChar, 4, uName, uVariant, gGolferTable[nGolfer].uIronGrip);

            SKA_PackName(&uName, gClubPartNames[5]);
            SkinPart_ChooseClubPartVariant(pChar, 5, uName, gGolferTable[nGolfer].wedges.uPart);
            SKA_PackName(&uName, gClubHeadSetNames[5]);
            SKA_PackName(&uVariant, gClubHeadVariantNames[5]);
            SkinPart_ChooseClubSet(pChar, 5, uName, uVariant, gGolferTable[nGolfer].wedges.uModel);
            SKA_PackName(&uName, gClubShaftSetNames[5]);
            SKA_PackName(&uVariant, gClubShaftVariantNames[5]);
            SkinPart_ChooseClubSet(pChar, 5, uName, uVariant, gGolferTable[nGolfer].wedges.uShaft);
            SKA_PackName(&uName, gClubGripSetNames[5]);
            SKA_PackName(&uVariant, gClubGripVariantNames[5]);
            SkinPart_ChooseClubSet(pChar, 5, uName, uVariant, gGolferTable[nGolfer].wedges.uGrip);

            SKA_PackName(&uName, gClubPartNames[2]);
            SkinPart_ChooseClubPartVariant(pChar, 2, uName, gGolferTable[nGolfer].aClubs[2].uPart);
            SKA_PackName(&uName, gClubHeadSetNames[2]);
            SKA_PackName(&uVariant, gClubHeadVariantNames[2]);
            SkinPart_ChooseClubSet(pChar, 2, uName, uVariant, gGolferTable[nGolfer].aClubs[2].uModel);
            SKA_PackName(&uName, gClubShaftSetNames[2]);
            SKA_PackName(&uVariant, gClubShaftVariantNames[2]);
            SkinPart_ChooseClubSet(pChar, 2, uName, uVariant, gGolferTable[nGolfer].aClubs[2].uShaft);
            SKA_PackName(&uName, gClubGripSetNames[2]);
            SKA_PackName(&uVariant, gClubGripVariantNames[2]);
            SkinPart_ChooseClubSet(pChar, 2, uName, uVariant, gGolferTable[nGolfer].aClubs[2].uGrip);
        }
    } else {
        SkinPart_ApplyClubChoices(pChar, pChoices);
    }
    SkinPart_SetClubsLeftHanded(pChar, Character_IsLeftHanded(pChar));
}

// Every frame in game type 6 (the main loop): each player whose pad holds Start, X and Y together
// (SpeedGolf_IsStartXYHeld) gets its golfer's animation back to normal speed (time scale 1), undoing a slowed
// or held animation such as the swing's hold at the top (0.008).
void Character_ResetTimeScalesOnCombo(void) {
    int i;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        if (SpeedGolf_IsStartXYHeld(i)) {
            SKATime_SetTimeScale(gPlayers[i].pChar->anim, 1.0f);
        }
    }
}

// Copies a quaternion (four floats; w is copied first).
void Quat_Copy(f32* pSrc, f32* pDst) {
    pDst[3] = pSrc[3];
    pDst[0] = pSrc[0];
    pDst[1] = pSrc[1];
    pDst[2] = pSrc[2];
}

// Copies the point pSrc (x, y, z) into the four-float pDst, with w = 1.
void Vec4_CopyPoint(f32* pSrc, f32* pDst) {
    pDst[0] = pSrc[0];
    pDst[1] = pSrc[1];
    pDst[2] = pSrc[2];
    pDst[3] = 1.0f;
}

// Sets every bit of a bit array of nBits bits.
void BitArray_FillArray(u32* aBits, u32 nBits) {
    u32 i;

    for (i = 0; i < (nBits + 31) >> 5; i++) {
        aBits[i] = 0xFFFFFFFF;
    }
}

// Clears every bit of a bit array of nBits bits.
void BitArray_ClearArray(u32* aBits, u32 nBits) {
    u32 i;

    for (i = 0; i < (nBits + 31) >> 5; i++) {
        aBits[i] = 0;
    }
}

u8 BitArray_TestBit(u32* aBits, u32 n) {
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

void BitArray_SetBit(u32* aBits, u32 n) {
    aBits[n >> 5] |= 1 << (n & 31);
}

// Each bit of aOut is set where both aA and aB have it (bit arrays of nBits bits).
void BitArray_MergeArrayWithAnd(u32* aA, u32* aB, u32* aOut, u32 nBits) {
    u32 i;

    for (i = 0; i < (nBits + 31) >> 5; i++) {
        aOut[i] = aA[i] & aB[i];
    }
}

void BitArray_ClearBit(u32* aBits, u32 n) {
    aBits[n >> 5] &= ~(1 << (n & 31));
}

// A bone's position, by bone id.
void Character_GetBonePos(Character* pChar, int nBone, f32* pPos) {
    Character_GetBonePos_FromIndex(pChar, CharModel_GetBoneIndex(pChar->pModel, nBone), pPos);
}

// The position of the bone at index nBone of the model (row 3 of its matrix) into pPos; a character
// that is not a golfer (Character_IsGolfer) gives index 1's instead. Nothing without a character.
void Character_GetBonePos_FromIndex(Character* pChar, int nBone, f32* pPos) {
    if (pChar != NULL) {
        if (Character_IsGolfer(pChar) == 0) {
            nBone = 1;
        }
        LLMath_CopyVec(pChar->pModel->pMatrices[nBone][3], pPos);
    }
}

// True for a golfer: a character playing from animation slot 0 or 1 (skalib's clip banks). For the
// other characters (the flagstick and the like) the bone getters give bone index 1's matrix
// whatever bone is asked for (Character_GetBoneMatrix_FromIndex).
u8 Character_IsGolfer(Character* pChar) {
    if (pChar->nSlot >= 0 && pChar->nSlot < 2) {
        return 1;
    }
    return 0;
}

// A bone's matrix by bone id; for a left-handed golfer the bone of the other side
// (CharModel_GetBoneIndexMapped).
f32 (*Character_GetBoneMatrixSwapIfLefty(Character* pChar, int nBone))[4] {
    return Character_GetBoneMatrix_FromIndex(pChar, CharModel_GetBoneIndexMapped(pChar->pModel, nBone));
}

// The matrix of the bone at index nBone of the model; a character that is not a golfer
// (Character_IsGolfer) gives index 1's instead. NULL without a character.
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

// How far along the camera's view the character is still drawn: fMaxVisibleDist (200, or 100 for a
// non-golfer) over the lens's field-of-view scale, so farther when zoomed in, and half that when
// bSplitScreen. Past it Character_ClipTest marks the body out of view, and
// Character_UpdateAnimation stops animating a character that is not a golfer.
f32 Character_ComputeMaxVisableDistance(Character* pChar, int bSplitScreen) {
    if (bSplitScreen != 0) {
        return pChar->fMaxVisibleDist * (0.5f / Camera_GetLensFovScale(Camera_GetCurrentLens()));
    }
    return pChar->fMaxVisibleDist * (1.0f / Camera_GetLensFovScale(Camera_GetCurrentLens()));
}

// A bone's position by bone id; for a left-handed golfer the bone of the other side
// (CharModel_GetBoneIndexMapped).
void Character_GetBonePosSwapIfLefty(Character* pChar, int nBone, f32* pPos) {
    Character_GetBonePos_FromIndex(pChar, CharModel_GetBoneIndexMapped(pChar->pModel, nBone), pPos);
}

// True for a left-handed golfer: its model is mirrored, and bone ids looked up through
// CharModel_GetBoneIndexMapped go through its second bone table.
u8 Character_IsLeftHanded(Character* pChar) {
    return pChar->pModel->bLeftHanded;
}

// How far along the camera's view the character's shadow is still drawn: fMaxShadowDist (100, or 50
// for a non-golfer) over the lens's field-of-view scale, and half that when bSplitScreen. Past it
// Character_ClipTest marks the shadow out of view.
f32 Character_ComputeMaxVisableShadowDistance(Character* pChar, int bSplitScreen) {
    if (bSplitScreen != 0) {
        return pChar->fMaxShadowDist * (0.5f / Camera_GetLensFovScale(Camera_GetCurrentLens()));
    }
    return pChar->fMaxShadowDist * (1.0f / Camera_GetLensFovScale(Camera_GetCurrentLens()));
}

f32 (*Character_GetRootMatrix(Character* pChar))[4] {
    return Character_GetBoneMatrix(pChar, 1);
}

// Character_ClipTest's answer for the character's shadow (a 3-unit sphere): 2 = out of view.
int Character_GetShadowClipResult(Character* pChar) {
    return pChar->nShadowClipResult;
}

// Character_ClipTest's answer for the character's body (its bounding sphere): 2 = out of view.
int Character_GetClipResult(Character* pChar) {
    return pChar->nClipResult;
}

// Makes the golfer left-handed (bLeftHanded 1) or right-handed; the callers set the skeleton again
// after it (Character_SetSkeleton).
void Character_SetLeftHanded(Character* pChar, u8 bLeftHanded) {
    pChar->pModel->bLeftHanded = bLeftHanded;
}

// The dot product of two 4-vectors.
f32 Vec4_Dot(f32* pA, f32* pB) {
    return pA[0] * pB[0] + pA[1] * pB[1] + pA[2] * pB[2] + pA[3] * pB[3];
}

// A bone id's index in the model's skeleton (0xFF: the model has no such bone).
// CharModel_GetBoneIndexMapped is the form that swaps sides for a left-hander.
int CharModel_GetBoneIndex(CharModel* pModel, int nBone) {
    return pModel->aBone[nBone];
}

// A bone id's index in the model's skeleton; while the model is left-handed (bLeftHanded) it goes
// through aBone2 to the bone of the other side, so right-hand bone ids find the left hand's bones.
int CharModel_GetBoneIndexMapped(CharModel* pModel, int nBone) {
    if (pModel->bLeftHanded) {
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
asm void Char_Vec4Sub(register f32* pA, register f32* pB, register f32* pOut) {
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
void Char_Vec4Sub(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
    pOut[3] = pA[3] - pB[3];
}
#endif

// b + a into out (four floats)
#ifdef __MWERKS__
asm void Char_Vec4Add(register f32* pA, register f32* pB, register f32* pOut) {
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
void Char_Vec4Add(f32* pA, f32* pB, f32* pOut) {
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

// The time into the clip of its SKA tag (timed event) uEvent, 0 when the clip has none. Tag 2 is
// the ball-hit time the swing measures.
f32 SKA_GetTagTime(Clip* pBlend, u64 uEvent) {
    int i;

    for (i = 0; i < pBlend->nEvents; i++) {
        if (pBlend->pEvents[i].uId == uEvent) {
            return pBlend->pEvents[i].fTime;
        }
    }
    return 0.0f;
}

// Sets an animation player's time scale (TSKATime.fTimeScale): 1 plays at normal speed, the
// swing's hold at the top uses 0.008.
void SKATime_SetTimeScale(u8* pAnim, f32 fRate) {
    ((TSKATime*)pAnim)->fTimeScale = fRate;
}

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
MtaLib* MtaLib_SwapAndLink(MtaLib* pArg, s32* pnSize) {
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
