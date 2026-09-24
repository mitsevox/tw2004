// char.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the golfer's character
// object (character.h). So far: choosing the clip a character plays (Char_SetClip), its bones and
// matrices, and small setters; the sweep code in the marked block is the other matched small
// functions, not yet cleaned up.

#include "game.h"
#include "charstate.h"
#include "lldyntex.h"
#include "frontend/fe.h"
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
Character* fn_8001A9F4(u8* pData, int nUnused, int nSet, int nId, u8 bLook, SkinChoices* pChoices);
CharSkinSet* fn_8001B208(u8* pData);
Character* fn_8001942C(void);
void  fn_80072D90(void* pAnim);                                 // animblender.c: reset a player
s32   fn_800962F8(Character* pChar);                            // CharAnim.c
void  fn_800184E4(Character* pChar, Skin* pSkin);
void  fn_80018710(Character* pChar);
void  ClipBank_Restore(int nSlot);                  // skalib.c
ClipBank* ClipBank_Get(u32 nSlot);                  // skalib.c
AnimLib* AnimLib_Load(u8* pData, ClipBank* pBank);  // skalib.c
CharModel* fn_80028564(u8* pData, s8 n, CharModelDefs* pDefs, u8 b);    // Skeleton.c
void  fn_80037AB8(Skin* pSkin, CharModel* pModel, int nBone, int nId);   // Skin.c
void  fn_800CC4EC(Character* pChar);                // SkinPart.c
void* CharSlider_CreateDefinitionsFromMem(u8** ppData);
void  fn_8001B58C(CharSkinSet* pSet);
void  fn_8001B878(Character* pChar, int nPlayer);
f32 (*fn_8001EE64(Character* pChar))[4];                        // bone 1's matrix
Character* fn_8001C21C(Character* pChar);
void  Character_UpdateAnimation(Character* pChar, int a, f32 f);
void  Character_UpdateTestPoints(Character* pChar);
void  Character_UpdateFeetTerrainInfo(Character* pChar, int bNormals);
f32   Character_GetTerrainHeightAndNormal(Character* pChar, f32* pPos, f32** ppNormal);
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
void  fn_8001D7EC(Character* pChar);
void  fn_800BBADC(int nValue);         // SitDevFile.c
void  fn_8001EBD8(Character* pChar, int nBone, f32* pPos);
u8    fn_8001EC48(Character* pChar);
f32   fn_8001ED44(Character* pChar, int b);
f32   fn_8001EE00(Character* pChar, int b);
void  fn_80017DDC(Character* pChar);
void  fn_8001899C(Character* pChar, u8 bLegA, u8 bLegB);
void  Character_IKLegToGround(Character* pChar, CourseInfo* pCourse, int nLeg, int nBoneA, int nBoneB,
                              int nBoneC, int nBoneD, int nPoint, int b);
void  fn_8001B644(Character* pChar);
void  fn_8001C860(Character* pChar);
void  fn_80021978(u8 v);                                        // ska_shared.c
void  fn_8002787C(CharModel* pModel);                           // Skeleton.c
void  fn_800279C0(Character* pChar);                            // Skeleton.c
void  SKEL_UpdateState(CharModel* pModel, SkelPose* pPose, u8 bTransform);   // Skeleton.c
void  fn_80037C48(Skin* pSkin, SkelPose* pPose);                // Skin.c
void  fn_8007260C(Character* pChar, SKABlendNode* pNode, CharModel* pModel, f32 fTime);  // animblender.c
void  fn_80072ED8(void* pAnim, SKABlendNode* pNode, f32 fTime);                          // animblender.c
void  fn_80073108(Character* pChar, int nPlayer, void* pAnim, SKABlendNode* pNode, f32 fTime);
void  fn_8009622C(Character* pChar, void* pClip, u8 bKeep, f32 fOffset);                  // CharAnim.c
void  fn_80096F0C(Character* pChar);                            // CharAnim.c
void  fn_8000914C(f32* pQ, f32 (*m)[4]);                        // Quaternion.c: a rotation matrix
int   fn_8001BD18(Character* pChar, Clip* pClip);
void  fn_80008F20(f32* pQ, f32* pOut);                          // Quaternion.c
void  fn_800090E4(f32* pQ, f32* pIn, f32* pOut);                // Quaternion.c: a vector turned by pQ
void  fn_80009410(f32 fAngle, f32* pOut);                       // Quaternion.c
void  fn_8001FCF4(Character* pChar, Clip* pClip, SkelPose* pPose, int n, f32 fTime);
void  fn_800280E8(Character* pChar, f32* pPos, int bPlace);     // Skeleton.c
void  fn_8001EFB4(f32* pA, f32* pB, f32* pOut);
void  fn_8001A14C(Character* pChar);
void  fn_8001D6D8(int n);
void  fn_8010B098(void* pModel);                                // LLDynTex.c
void  fn_800958EC(AnimPlayer* pAnim, s32 n, f32 f);            // CharAnim.c
void  fn_800094D8(f32* pQ, f32* pA, f32* pB, f32* pC);          // Quaternion.c: a rotation as angles
void  fn_80029968(CharModel* pModel, SkelPose* pPose);          // Skeleton.c
void  fn_8000AB40(f32 (*pSrc)[4], f32 (*pDst)[4]);              // UMemPool.c
void  fn_8001EDA8(Character* pChar, int nBone, f32* pPos);
void  fn_8001EF54(f32* pA, f32* pB, f32* pOut);
void  fn_8001EF10(f32* pA, f32* pB, f32* pOut);
void  fn_80095558(void);
void  AnimLib_ApplyOverlays(int nSlot);                         // skalib.c
void  fn_80025478(void);                                        // skalib.c
void  fn_800CA7E0(void);                                        // AnimStream.c
void  fn_800CABA0(void);                                        // AnimStream.c
void  fn_800CB078(void);                                        // AnimStream.c
void  fn_8001DD18(u8* pData, int nBytes);
void  fn_8001DEC8(u8* pData, int nBytes);
s32   fn_800CE8C0(Skin** apSkins, int nSkins, SkinListEntry** ppList);   // SkinPart.c
void  fn_800CEEBC(void);                                        // SkinPart.c: empty
void  fn_800100B0(TexBank* pBank, TexEntry* p8, TexPalette* pC, void* p10, void* p14, s16 nNumTex,
                  s16 nNumPalettes);                            // LLTex.c
void  fn_8001EFD8(f32* pA, f32* pB, f32* pOut);
f32 (*fn_8001EC6C(Character* pChar, int nBone))[4];
f32 (*fn_8001ECA8(Character* pChar, int nBone))[4];
f32   fn_8001EFFC(CamLens* pLens);
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
void  fn_8001744C(void* pChar, void* pModel, SkinChoices* pChoices);   // char_tex_manager.c
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
void  fn_800CCA1C(void);
void  fn_800CCA3C(void);
void  fn_800CEE04(Skin* pSkin, int a, int b);
s32   fn_800CCEA0(Skin* pSkin);         // SkinPart.c: how many choices aSets[3] holds
void  fn_800CEE88(u8 b);
u8    fn_800FCC38(int nPlayer);
void  fn_8010A668(void* p);
void  fn_80008380(void);
void  fn_800106AC(int n);               // LLTexGrp.c
void  fn_800106B8(u8 b);                // LLTexGrp.c
void  fn_8008F310(void);                // uiLoadFile.c: park the UI file's data in ARAM
void* fn_8008F354(void);                // uiLoadFile.c: the UI file's buffer
void  fn_8008F35C(void);                // uiLoadFile.c: bring the UI file's data back
void  fn_800720C8(Character* pChar, SKABlendNode* pNew, SKABlendNode** ppNode, f32* pBlend,
                  SKABlendFn pfnBlend, int b);                                         // animblender.c
void  fn_800724C0(SKABlendNode* pNode, SKABlendNode* pNew, Clip* pClip, f32 fWeight);  // animblender.c
void  fn_800732F4(void* pNode, void* pAnim, f32 fTime);                                // CharAnim.c
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
void fn_8001E8A4(u32* aBits, u32 nBits);
void fn_8001E938(u32* aBits, u32 nBits);
void fn_8001B1DC(Skin* pSkin, CharSkinRef* pRef, s32 n);
void fn_8001B1E8(void* p);
void fn_8001C650(void* arg0, s32 arg1);

void fn_8001B1DC(Skin* pSkin, CharSkinRef* pRef, s32 n) {
    pRef->n0 = n;
    pRef->pSkin = pSkin;
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

// Clear the character's animation events: none set, all at time 2^30 (never).
void fn_80017508(Character* pChar) {
    s32 i;

    for (i = 0; i < 18; i++) {
        pChar->events[i].bSet = 0;
        pChar->events[i].fTime = 1073741824.0f;
    }
}

// Sets the character's animation events from the blend's, fStart later. Events 5..14 are only
// taken when their time is past 0.
void fn_800175B0(Character* pChar, Clip* pBlend, f32 fStart) {
    u32 uId;
    int i;

    fn_80017508(pChar);
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

// A random item of group nGroup of the 'MAL ' bank of the character's slot, or NULL without one.
void* fn_80017678(Character* pChar, int nGroup, int n) {
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
void fn_800177A0(Character* pChar, SkelPose* pPose) {
    int i;
    Skin* pSkin = pChar->pSkin;

    if (pSkin != NULL) {
        fn_8001E938(pPose->a0, 0x80);
        fn_8001E938(pPose->a10, 0x80);
        fn_8001E8A4(pPose->a20, 0x80);
        fn_8001E8A4(pPose->a30, 0x80);
        for (i = 0; i < pChar->pModel->nBones; i++) {
            fn_8001E85C(pSkin->pose.aBones[i].q0, pPose->aBones[i].q0);
            fn_8001E85C(pSkin->pose.aBones[i].v10, pPose->aBones[i].v10);
        }
    }
}

// Only the bit arrays of fn_800177A0 (with a body skin): the last two set, the first two cleared.
void fn_80017864(Character* pChar, SkelPose* pPose) {
    if (pChar->pSkin != NULL) {
        fn_8001E8A4(pPose->a20, 0x80);
        fn_8001E8A4(pPose->a30, 0x80);
        fn_8001E938(pPose->a0, 0x80);
        fn_8001E938(pPose->a10, 0x80);
    }
}

// The ground height (and with bNormals its normal; straight up without ground) under points 0-3.
// n1784 would pick a half of them per call (points 0 and 2, or 1 and 3), but it is set to -1
// first, so every call does all four.
void Character_UpdateFeetTerrainInfo(Character* pChar, int bNormals) {
    f32* pNormal;
    f32 fHeight;
    int i;
    int nLast;
    int nStep;

    if (fn_8000C594() != NULL) {
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
                    Vec_Copy(pNormal, pChar->aGroundNormal[i + pChar->n1784]);
                }
            }
        }
        pChar->n1784++;
        if (pChar->n1784 >= 2) {
            pChar->n1784 = 0;
        }
    }
}

// The ground height at pPos (looked for from 0.055 above it), with *ppNormal pointed at that
// ground's normal; -65536.125 for none, and for surface classes 0xC and 0x12. Of the two heights
// around the point the high one is taken when it is the only one, or the low one is on class 7 or
// 0x13, or the two are less than 0.05 apart, or it is below 1 over the point.
f32 Character_GetTerrainHeightAndNormal(Character* pChar, f32* pPos, f32** ppNormal) {
    f32 vPos[4];
    f32 fLow;
    f32 fHigh;
    SurfaceType* pLowSurface;
    SurfaceType* pHighSurface;
    CourseInfo* pCourse;

    if (pChar != NULL) {
        if ((pCourse = fn_8000C594()) != NULL) {
            Vec_Copy(pPos, vPos);
            vPos[1] += 0.66f / 12.0f;
            Ter_GetEnclosingGroundData(pCourse, vPos, &fLow, &pLowSurface, lbl_801B95D8, &fHigh,
                                       &pHighSurface, lbl_801B95C8);
            if (!(fHigh < -60000.0f)) {
                if (fLow < -60000.0f || pLowSurface->nClass == 7 || pLowSurface->nClass == 0x13 ||
                    fHigh - fLow < 0.05f || fHigh < 1.0f + pPos[1]) {
                    if (pHighSurface->nClass == 0xC || pHighSurface->nClass == 0x12) {
                        return -65536.125f;
                    }
                    *ppNormal = lbl_801B95C8;
                    return fHigh;
                }
            } else if (fLow < -60000.0f) {
                goto none;      // fake match: the original puts this return after the low height
            }
            if (pLowSurface->nClass == 0xC || pLowSurface->nClass == 0x12) {
                return -65536.125f;
            }
            *ppNormal = lbl_801B95D8;
            return fLow;
        none:
            return -65536.125f;
        }
        return -65536.125f;
    }
    return -65536.125f;
}

// Moves a golfer's test points with its bones: points 0-3 from the skin's leg points through the
// leg bones' matrices (or, without them, set out along the bones' axes by the model's fC and f10),
// point 4 from the club class's club point through bone 0x52's matrix.
void Character_UpdateTestPoints(Character* pChar) {
    f32 (*pClubMtx)[4];
    f32 (*pMtx48)[4];
    f32 (*pMtx3A)[4];
    f32 (*pMtx47)[4];
    f32 (*pMtx39)[4];
    f32 fA;
    f32 fB;

    if (!fn_8001EC48(pChar)) {
        return;
    }
    pClubMtx = fn_8001ED08(pChar, 0x52);
    if (pChar->pSkin->b1044) {
        pMtx48 = fn_8001EC6C(pChar, 0x48);
        pMtx3A = fn_8001EC6C(pChar, 0x3A);
        pMtx47 = fn_8001EC6C(pChar, 0x47);
        pMtx39 = fn_8001EC6C(pChar, 0x39);
        fn_800BAD60(pMtx3A, (Vec4*)pChar->pSkin->a1048[0], (Vec4*)pChar->aPoints[0]);
        fn_800BAD60(pMtx48, (Vec4*)pChar->pSkin->a1048[1], (Vec4*)pChar->aPoints[1]);
        fn_800BAD60(pMtx39, (Vec4*)pChar->pSkin->a1048[2], (Vec4*)pChar->aPoints[2]);
        fn_800BAD60(pMtx47, (Vec4*)pChar->pSkin->a1048[3], (Vec4*)pChar->aPoints[3]);
    } else {
        fA = 0.8f * pChar->pModel->f10;
        fB = 0.8f * pChar->pModel->fC;
        pMtx48 = fn_8001EC6C(pChar, 0x48);
        pMtx3A = fn_8001EC6C(pChar, 0x3A);
        pMtx47 = fn_8001EC6C(pChar, 0x47);
        pMtx39 = fn_8001EC6C(pChar, 0x39);
        fn_8000AE6C(pMtx3A[3], pMtx3A[1], pChar->pModel->f10, pChar->aPoints[0]);
        fn_8000AE6C(pMtx48[3], pMtx48[1], pChar->pModel->fC, pChar->aPoints[1]);
        fn_8000AE6C(pMtx39[3], pMtx3A[2], fA, pChar->aPoints[2]);
        fn_8000AE6C(pMtx47[3], pMtx48[2], fB, pChar->aPoints[3]);
        fn_8000AE6C(pChar->aPoints[0], pMtx3A[2], 0.25f * fA, pChar->aPoints[0]);
        fn_8000AE6C(pChar->aPoints[1], pMtx48[2], 0.25f * fB, pChar->aPoints[1]);
    }
    if (pChar->p16D8 != NULL && pClubMtx != NULL) {
        fn_800BAD60(pClubMtx, (Vec4*)pChar->p16D8->a3C[pChar->nClubClass], (Vec4*)pChar->aPoints[4]);
    }
}

// Keeps the club out of the ground: when point 4 is below the terrain and bone 0x52's y axis
// points into the slope, that axis is shortened by how far the point is under, measured against
// the club class's head height (not below 3/4 of it).
void fn_80017DDC(Character* pChar) {
    f32 vNormal[4];
    f32 (*pMtx)[4];
    CourseInfo* pCourse;
    f32 fHeight;
    f32 fUnder;
    f32 fDot;
    f32 fLength;
    f32 fHead;

    if (pChar->p16D8 != NULL && pChar->a179C[1] > 0.9f) {
        pMtx = fn_8001ED08(pChar, 0x52);
        if (pMtx != NULL && (pCourse = fn_8000C594()) != NULL) {
            fHeight = fn_8004D650(pCourse, pChar->aPoints[4], vNormal);
            if (fHeight < -60000.0f || (fUnder = fHeight - pChar->aPoints[4][1]) < 0.0f) {
                return;
            }
            fDot = -fn_8001EEA4(pMtx[1], vNormal);
            if (fDot > 0.707f) {
                fHead = pChar->p16D8->afC[pChar->nClubClass];
                fLength = (fHead - fUnder * vNormal[1] / fDot) / fHead;
                if (fLength > 0.75f) {
                    fn_8001EF34(pMtx[1], fLength, pMtx[1]);
                    fn_80029A90(pChar->pModel, pMtx, 0x52);
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
    int bC860 = 0;
    u8 bLegA = 0;
    u8 bLegB = 0;
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
        if (fn_8001EC48(pChar)) {
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
        fn_8001C860(pChar);
        bC860 = 1;
    }
    if (pChar->n20 == 8 && pChar->nAnim == 8) {
        fn_80073108(pChar, pChar->nPlayer, pChar->anim, &pChar->blend, fTime);
    } else if (pChar->u10 & 0x100) {
        fn_80072ED8(pChar->anim, &pChar->blend, 5.0f * fTime);
    } else {
        fn_80072ED8(pChar->anim, &pChar->blend, fTime);
        if (pChar->uFlags & 0x1000) {
            pChar->uFlags &= ~0x1000;
            if (pChar->pCurClip != NULL && pChar->pCurClip->pF4 != NULL) {
                fn_8009622C(pChar, pChar->pCurClip->pF4, 0, 0.5f);
            }
        }
    }
    fn_80021978(pChar->pModel->bEE);
    if (!gSession.b11) {
        fn_80072ED8(&pChar->anim29C, (SKABlendNode*)pChar->node3E0, fTime);
    }
    if (fn_8001EC48(pChar)) {
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
        if (fn_8001EC48(pChar) && pChar->p16D8 != NULL) {
            pChar->blend.pPose->aBones[pChar->nClubHeadBone].v10[1] = pChar->p16D8->afC[pChar->nClubClass];
        }
        SKEL_UpdateState(pChar->pModel, pChar->blend.pPose, 0);
        if (fn_8001EC48(pChar)) {
            if (fn_8001E9CC(pChar->blend.pPose->a0, fn_8001EEE4(pChar->pModel, 0x36)) ||
                fn_8001E9CC(pChar->blend.pPose->a0, fn_8001EEE4(pChar->pModel, 0x38)) ||
                fn_8001E9CC(pChar->blend.pPose->a0, fn_8001EEE4(pChar->pModel, 0x39)) ||
                fn_8001E9CC(pChar->blend.pPose->a0, fn_8001EEE4(pChar->pModel, 0x3A))) {
                bLegA = 1;
            }
            if (fn_8001E9CC(pChar->blend.pPose->a0, fn_8001EEE4(pChar->pModel, 0x44)) ||
                fn_8001E9CC(pChar->blend.pPose->a0, fn_8001EEE4(pChar->pModel, 0x46)) ||
                fn_8001E9CC(pChar->blend.pPose->a0, fn_8001EEE4(pChar->pModel, 0x47)) ||
                fn_8001E9CC(pChar->blend.pPose->a0, fn_8001EEE4(pChar->pModel, 0x48))) {
                bLegB = 1;
            }
        }
    }
    if (!gSession.b11 && fn_8001EC48(pChar) && ((SKABlendNode*)pChar->node3E0)->pPose != NULL) {
        fn_8007260C(pChar, (SKABlendNode*)pChar->node3E0, pChar->pModel, pChar->anim29C.fTime);
        fn_80037C48(pChar->pSkin, ((SKABlendNode*)pChar->node3E0)->pPose);
    }
    if (pChar->uFlags & 2) {
        pChar->uFlags |= 1;
        return;
    }
    if (pChar->pfn17B0 != NULL) {
        pChar->pfn17B0();
    }
    fn_8001E8A4(auBits, 0x80);
    SKEL_TransformBones(pChar->pModel, auBits);
    if (fn_8001EC48(pChar)) {
        Character_UpdateTestPoints(pChar);
        Character_UpdateFeetTerrainInfo(pChar, bC860 || pChar->n20 != 5 || pChar->n26 == 1);
        if (pChar->n20 == 1 || pChar->n20 == 0 || pChar->n20 == 9 ||
            (pChar->n20 == 11 && !(pChar->u10 & 0x8000)) || pChar->n20 == 5 || pChar->n20 == 12) {
            Character_PlaceFeetOnGround(pChar);
        }
        fn_8001899C(pChar, bLegA, bLegB);
    }
    if (fn_8001EC48(pChar)) {
        if (pChar->pModel->pSkel != NULL) {
            fn_800279C0(pChar);
        }
        fn_80017DDC(pChar);
    }
    pChar->n1698 = 0;
    fn_8001B644(pChar);
    if (fn_8001EC48(pChar)) {
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

// Gives the character its body's skin and poses the model from it; for a golfer, the skin also
// keeps four points of the legs (bones 0x3A, 0x48, 0x39, 0x47, each moved by a small offset that
// depends on the animation slot) in the frame of their bone (through fn_8000AB40's matrix).
void fn_800184E4(Character* pChar, Skin* pSkin) {
    f32 m48[4][4];
    f32 m3A[4][4];
    f32 m47[4][4];
    f32 m39[4][4];
    Vec4 v48;
    Vec4 v3A;
    Vec4 v47;
    Vec4 v39;
    Vec4 vOffsetB;
    Vec4 vOffsetA;
    f32 (*pMtx48)[4];
    f32 (*pMtx3A)[4];
    f32 (*pMtx47)[4];
    f32 (*pMtx39)[4];

    if (pChar != NULL) {
        pChar->pSkin = pSkin;
        fn_80018710(pChar);
        if (fn_8001EC48(pChar)) {
            pMtx48 = fn_8001EC6C(pChar, 0x48);
            pMtx3A = fn_8001EC6C(pChar, 0x3A);
            pMtx47 = fn_8001EC6C(pChar, 0x47);
            pMtx39 = fn_8001EC6C(pChar, 0x39);
            fn_8000AB40(pMtx48, m48);
            fn_8000AB40(pMtx3A, m3A);
            fn_8000AB40(pMtx47, m47);
            fn_8000AB40(pMtx39, m39);
            if (pChar->nSlot == 0) {
                vOffsetA.x = 0.0f;
                vOffsetA.y = -0.031f;
                vOffsetA.z = 0.0f;
                vOffsetA.w = 1.0f;
                vOffsetB.x = 0.0f;
                vOffsetB.y = -0.11f;
                vOffsetB.z = -0.06f;
                vOffsetB.w = 1.0f;
            } else {
                vOffsetA.x = 0.0f;
                vOffsetA.y = -0.025f;
                vOffsetA.z = 0.0f;
                vOffsetA.w = 1.0f;
                vOffsetB.x = 0.0f;
                vOffsetB.y = -0.08f;
                vOffsetB.z = -0.025f;
                vOffsetB.w = 1.0f;
            }
            fn_8001EDA8(pChar, 0x48, &v48.x);
            fn_8001EDA8(pChar, 0x3A, &v3A.x);
            fn_8001EDA8(pChar, 0x47, &v47.x);
            fn_8001EDA8(pChar, 0x39, &v39.x);
            fn_8001EF54(&v48.x, &vOffsetA.x, &v48.x);
            fn_8001EF54(&v3A.x, &vOffsetA.x, &v3A.x);
            fn_8001EF54(&v47.x, &vOffsetB.x, &v47.x);
            fn_8001EF54(&v39.x, &vOffsetB.x, &v39.x);
            fn_800BAD60(m3A, &v3A, (Vec4*)pChar->pSkin->a1048[0]);
            fn_800BAD60(m48, &v48, (Vec4*)pChar->pSkin->a1048[1]);
            fn_800BAD60(m39, &v39, (Vec4*)pChar->pSkin->a1048[2]);
            fn_800BAD60(m47, &v47, (Vec4*)pChar->pSkin->a1048[3]);
            pChar->pSkin->b1044 = 1;
        }
    }
}

// Poses the character's model from its body's skin: the skin's pose (SKEL_UpdateState,
// fn_80029968) and, when the skin has a model, the skin's matrices.
void fn_80018710(Character* pChar) {
    if (pChar != NULL && pChar->pSkin != NULL && pChar->pModel != NULL) {
        SKEL_UpdateState(pChar->pModel, &pChar->pSkin->pose, 1);
        fn_80029968(pChar->pModel, &pChar->pSkin->pose);
        if (pChar->pSkin->pModel != NULL) {
            fn_80037AB8(pChar->pSkin, pChar->pModel, 0, 0);
            fn_80029A74(pChar->pModel, pChar->pSkin->pModel->p34);
            fn_80029A88(pChar->pModel, pChar->pSkin->p1088);
            fn_80029A7C(pChar->pModel, pChar->pSkin->p108C, pChar->pSkin->pModel->n14);
        }
    }
}

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
    f32 fLow;
    f32 fHigh;
    SurfaceType* pSurfLow;
    SurfaceType* pSurfHigh;
    f32 vNormalLow[4];
    f32 vNormalHigh[4];

    if (pChar == NULL) {
        return;
    }
    pCourse = fn_8000C594();
    if (pCourse == NULL) {
        return;
    }
    fLowest = 1073741824.0f;
    if (fn_8001EC48(pChar)) {
        pChar->a179C[0] = 0.0f;
        pChar->a179C[1] = 0.0f;
        pChar->a179C[2] = 0.0f;
        pChar->a179C[3] = 0.0f;
        for (i = 0; i < 4; i++) {
            fn_8001EF54(pChar->aGroundNormal[i], pChar->a179C, pChar->a179C);
            if (pChar->afGroundHeight[i] < fLowest) {
                fLowest = pChar->afGroundHeight[i];
            }
        }
        fn_800BAF04(pChar->a179C, pChar->a179C);
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
    pPos = pChar->pModel->pBones[0].v1C;
    Ter_GetEnclosingGroundData(pCourse, pPos, &fLow, &pSurfLow, vNormalLow, &fHigh, &pSurfHigh,
                               vNormalHigh);
    fY = fHigh;
    if (fHigh < -60000.0f || fHigh > 1.0f + pPos[1]) {
        fY = (fLow < -60000.0f) ? pPos[1] : fLow;
    }
    if (fY > 131072.25f || fY < -131072.25f) {
        return;
    }
    pChar->pModel->pBones[0].v1C[1] = fY;
}

// Puts a golfer's legs on the ground by IK: with bLegA the leg of bones 0x36-0x3A (point 2),
// with bLegB the leg of bones 0x44-0x48 (point 3); the bones they move are then transformed again.
void fn_8001899C(Character* pChar, u8 bLegA, u8 bLegB) {
    CourseInfo* pCourse;
    u32 auBits[4];

    if (pChar == NULL) {
        return;
    }
    fn_8001E938(auBits, 0x80);
    if (!fn_8001EC48(pChar)) {
        return;
    }
    pCourse = fn_8000C594();
    if (pCourse == NULL) {
        return;
    }
    if (bLegA) {
        Character_IKLegToGround(pChar, pCourse, 0, fn_8001EEE4(pChar->pModel, 0x36),
                                fn_8001EEE4(pChar->pModel, 0x38), fn_8001EEE4(pChar->pModel, 0x39),
                                fn_8001EEE4(pChar->pModel, 0x3A), 2, 0);
        fn_8001EA34(auBits, fn_8001EEE4(pChar->pModel, 0x38));
        fn_8001EA34(auBits, fn_8001EEE4(pChar->pModel, 0x36));
        fn_8001EA34(auBits, fn_8001EEE4(pChar->pModel, 0x39));
    }
    if (bLegB) {
        Character_IKLegToGround(pChar, pCourse, 1, fn_8001EEE4(pChar->pModel, 0x44),
                                fn_8001EEE4(pChar->pModel, 0x46), fn_8001EEE4(pChar->pModel, 0x47),
                                fn_8001EEE4(pChar->pModel, 0x48), 3, 1);
        fn_8001EA34(auBits, fn_8001EEE4(pChar->pModel, 0x46));
        fn_8001EA34(auBits, fn_8001EEE4(pChar->pModel, 0x44));
        fn_8001EA34(auBits, fn_8001EEE4(pChar->pModel, 0x47));
    }
    if (fn_8001E9F4(auBits, auBits, 0x80)) {
        SKEL_TransformBones(pChar->pModel, auBits);
    }
}

// Bends leg nLeg (bones A, B, C and D down the leg, test points nPoint and nOther under it) so its
// foot stands on the ground: the knee (B) and hip (A) are turned so bone C reaches the ground height
// under nPoint, then C is tilted towards the ground's slope, more the deeper the foot sat.
void Character_IKLegToGround(Character* pChar, CourseInfo* pCourse, int nLeg, int nBoneA, int nBoneB,
                             int nBoneC, int nBoneD, int nPoint, int nOther) {
    f32 vOld[4];
    f32 vC[4];
    f32 vPoint[4];
    f32 vD[4];
    f32 vA[4];
    f32 vB[4];
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
    f32 fDropA;
    f32 fDropB;
    f32 fDrop;
    f32 fReach;
    f32 fLeg;
    f32 fThigh;
    f32 fShin;
    f32 fDen;
    f32 fSq;
    f32 fCos;
    f32 fAngleA;
    f32 fAngleB;
    f32 fTurn;
    f32 fLen;
    f32 fScale;
    Skeleton* pSkel;
    Bone* pBone;

    fn_8001EBD8(pChar, nBoneC, vC);
    fn_8001EBD8(pChar, nBoneA, vA);
    fn_8001EBD8(pChar, nBoneB, vB);
    fn_8001EBD8(pChar, nBoneD, vD);
    Vec_Copy(pChar->aPoints[nPoint], vPoint);
    // how far each test point sits below the ground (0.165 in, in feet)
    fDropA = 0.165f / 12.0f + (pChar->afGroundHeight[nPoint] - vPoint[1]);
    fDropB = 0.165f / 12.0f + (pChar->afGroundHeight[nOther] - pChar->aPoints[nOther][1]);
    if ((fDropA < 0.0f && fDropB < 0.0f) || fDropA > 1.0f) {
        return;
    }
    fDrop = (fDropA <= fDropB) ? fDropB : fDropA;
    if (fDrop < 0.0f) {
        return;
    }
    fn_8001EF54(pChar->aGroundNormal[nPoint], pChar->aGroundNormal[nOther], vSlope);
    fn_800BAF04(vSlope, vSlope);
    if (fDrop > 0.33f / 12.0f) {
        fDrop = 1.0f;
    } else {
        fDrop = fDrop / (0.33f / 12.0f);
    }
    fDropA -= 0.165f / 12.0f;
    if (fDropA < 0.0f) {
        fDropA = 0.0f;
    }

    // the knee: the angle the thigh and shin must make for the hip to reach bone C raised by fDropA
    Vec3Copy(vC, vOld);
    vC[1] += fDropA;
    fn_8001EF10(vA, vOld, vReach);
    fn_8001EF10(vA, vC, vLeg);
    fn_8001EF10(vB, vA, vThigh);
    fn_8001EF10(vB, vOld, vShin);
    fn_8001EF10(vOld, vD, vFoot);
    fReach = (f32)fn_80009680(fn_80009744(vReach));
    fLeg = (f32)fn_80009680(fn_80009744(vLeg));
    fThigh = (f32)fn_80009680(fn_80009744(vThigh));
    fShin = (f32)fn_80009680(fn_80009744(vShin));
    if (fLeg > fThigh + fShin) {
        fLeg = fThigh + fShin;
    }
    fDen = 2.0f * fThigh * fShin;
    if (0.0f == fDen) {
        fDen = 1.0f;
    }
    fSq = fThigh * fThigh + fShin * fShin;
    fCos = (fSq - fReach * fReach) / fDen;
    fAngleA = fn_80009614((fCos < -1.0f) ? -1.0f : ((fCos > 1.0f) ? 1.0f : fCos));
    fCos = (fSq - fLeg * fLeg) / fDen;
    fAngleB = fn_80009614((fCos < -1.0f) ? -1.0f : ((fCos > 1.0f) ? 1.0f : fCos));
    fTurn = fAngleA - fAngleB;
    vec4flt_CrossProduct(vShin, vThigh, vNormal);
    fLen = fn_800BAFC0(vNormal, vNormal);
    vNormal[3] = 0.0f;
    pSkel = pChar->pModel->pSkel;
    if (pSkel != NULL) {
        // a degenerate bend axis falls back on the last good one
        if (fLen > 0.0001f) {
            Vec_Copy(vNormal, pSkel->a10E8[nLeg]);
        } else {
            Vec_Copy(pSkel->a10E8[nLeg], vNormal);
        }
    }
    if (fabsf(fTurn) > 0.0001f) {
        fn_8001EF34(vNormal, fTurn, vAxis);
        fn_8000923C(vAxis, qTurn);
        fn_80008F20(pChar->pModel->pPoses[nBoneB].q0, qA8);
        fn_800090E4(qA8, qTurn, q98);
        fn_80008FCC(q98, pChar->pModel->pBones[nBoneB].q0C, qB8);
        fn_8001E85C(qB8, pChar->pModel->pBones[nBoneB].q0C);
    }

    // the hip: turned by the change in the angle between the thigh and the hip-to-foot line
    fTurn = fn_8000965C(fShin * fn_800095F0(fAngleA) / fReach) -
            fn_8000965C(fShin * fn_800095F0(fAngleB) / fLeg);
    if (fabsf(fTurn) > 0.0001f) {
        fn_8001EF34(vNormal, fTurn, vAxis);
        vAxis[3] = 0.0f;
        fn_8000923C(vAxis, qTurn);
        fn_80008F20(pChar->pModel->pPoses[nBoneA].q0, qA8);
        fn_800090E4(qA8, qTurn, q98);
        fn_80008FCC(q98, pChar->pModel->pBones[nBoneA].q0C, qB8);
        fn_8001E85C(qB8, pChar->pModel->pBones[nBoneA].q0C);
    }

    // the ankle: tilted about the horizontal axis across the slope, by the slope's angle
    pBone = &pChar->pModel->pBones[nBoneA];
    fn_80008FCC(pBone->q0C, pChar->pModel->pPoses[pBone->nParent].q0, q88);
    fn_80008FCC(pChar->pModel->pBones[nBoneB - 1].q0C, q88, q58);
    fn_80008FCC(pChar->pModel->pBones[nBoneB].q0C, q58, q68);
    fn_80008F20(q68, q78);
    fn_80008FCC(pChar->pModel->pPoses[nBoneC].q0, q78, q48);
    vAxis[0] = vSlope[2];
    vAxis[1] = 0.0f;
    vAxis[2] = -vSlope[0];
    vAxis[3] = 0.0f;
    fLen = (f32)fn_80009680(vAxis[0] * vAxis[0] + vAxis[2] * vAxis[2]);
    if (fLen < 0.01f) {
        return;
    }
    fScale = 1.0f / fLen;
    vAxis[0] *= fScale;
    vAxis[2] *= fScale;
    fCos = vSlope[1];
    fTurn = fn_80009614((fCos < -1.0f) ? -1.0f : ((fCos > 1.0f) ? 1.0f : fCos)) * fDrop;
    if (fabsf(fTurn) > 0.0001f) {
        fn_8001EF34(vAxis, fTurn, vAxis);
        fn_80008FCC(q48, q68, q38);
        fn_80008F20(q38, q28);
        vAxis[3] = 0.0f;
        fn_8000923C(vAxis, qTurn);
        fn_800090E4(q28, qTurn, qB8);
        fn_80008FCC(qB8, q48, q18);
        fn_8001E85C(q18, pChar->pModel->pBones[nBoneC].q0C);
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

// Turns the character to face along pDir (level: up is y), plus fAngle; a direction shorter than
// 0.01 is ignored.
void fn_80019358(Character* pChar, f32* pDir, f32 fAngle) {
    f32 fLen;
    f32 mtx[4][4];              // row 3 is left unset (fn_8000A4E0 reads rows 0..2)
    f32 fYaw;
    f32 fB;
    f32 fC;

    if (pChar != NULL) {
        fLen = fn_80009680(fn_80009744(pDir));
        if (fLen < 0.01f) return;
        fn_8001EF34(pDir, 1.0f / fLen, mtx[0]);
        mtx[0][3] = 0.0f;
        mtx[1][0] = 0.0f;
        mtx[1][1] = 1.0f;
        mtx[1][2] = 0.0f;
        mtx[1][3] = 0.0f;
        vec4flt_CrossProduct(mtx[0], mtx[1], mtx[2]);
        mtx[2][3] = 0.0f;
        fn_8000A4E0(mtx, &fYaw, &fB, &fC);
        fn_800192D4(pChar, fYaw + fAngle);
    }
}

// Makes a character: its four data buffers, both blend trees and animation players, and every
// field that starts at a value.
Character* fn_8001942C(void) {
    Character* pChar;
    SKABlendNode* pNode;
    int i;

    pNode = NULL;
    pChar = fn_80009B34(sizeof(Character), 2, 0x40, "char.c", 0x8A4);
    pChar->pfn17B0 = NULL;
    for (i = 0; i < 4; i++) {
        pChar->buffers[i].n00 = -1;
        pChar->buffers[i].p04 = NULL;
        pChar->buffers[i].p0C = NULL;
        pChar->buffers[i].p10 = NULL;
        pChar->buffers[i].p14 = NULL;
        pChar->buffers[i].pBuf = fn_80009B34(0x890, 2, 0x40, "char.c", 0x8AF);
    }
    pNode = &pChar->blend;
    fn_80072D90(pChar->anim);
    fn_80071C28(&pNode, 1, 0, fn_80072ACC, 1);
    pNode = (SKABlendNode*)pChar->node3E0;
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
    pChar->blend.nGroup = -1;
    pChar->n5CC = -1;
    pChar->p1790 = NULL;
    pChar->n1784 = -1;
    pChar->n17B4 = 0;
    pChar->pRecords = NULL;
    fn_800962F8(pChar);
    pChar->n16DC = 0;
    pChar->f165C = pChar->f1660 = 1073741824.0f;
    pChar->pCurClip = NULL;
    pChar->n178C = 0;
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
    fn_80017508(pChar);
    pChar->p1798 = NULL;
    return pChar;
}

void fn_80019648(void) {
    fn_80095554();
    fn_8001A4BC();
}

// Replays the character's blend at its current time: fAnimTime from f180, the blend's time
// (fn_8001F02C) and v1638[1], then one animation update of no length.
void fn_8001966C(Character* pChar) {
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

// Reads the character's textures from its CHR object (after the slider definitions, p4C) into
// bank78: in the front end (game types 10 and 3) all of them; otherwise, for each name the skins
// use (fn_800CE8C0), the texture of that name (and the one after it when it goes with it) with its
// palette, or an empty one. Then it opens the golfer's texture file.
void fn_80019798(Character* pChar, Skin** apSkins, int nSkins) {
    int nTexBytes;
    int nPalBytes;
    u8* pData;
    SkinListEntry* pList;
    TexEntry* pTexData;
    TexPalette* pPalData;
    u8 bAll;
    u8 bFound;
    int nTex;
    int nPal;
    int nExtra;
    int nNames;
    int nOut;
    int i;
    int j;

    nExtra = 0;
    pList = NULL;
    pData = pChar->p4C;
    fn_80076158(&pData, (u8*)&nTexBytes, 4, 4);
    fn_80076158(&pData, (u8*)&nPalBytes, 4, 4);
    fn_80076158(&pData, (u8*)&pChar->n58, 4, 4);
    fn_80076158(&pData, (u8*)&pChar->n5C, 4, 4);
    if (nTexBytes != 0) {
        pTexData = (TexEntry*)pData;
        fn_8001DD18(pData, nTexBytes);
        pData += nTexBytes;
    }
    if (nPalBytes != 0) {
        pPalData = (TexPalette*)pData;
        fn_8001DEC8(pData, nPalBytes);
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
        pChar->pA8 = fn_80009B34(pChar->nAC * sizeof(TexEntry), 2, 0x10, "char.c", 0x9A9);
        pChar->pB8 = fn_80009B34(pChar->nAC * 64, 2, 0x10, "char.c", 0x9AE);
    }
    if (pChar->nB4 != 0) {
        pChar->pB0 = fn_80009B34(nPalBytes, 2, 0x10, "char.c", 0x9B8);
        pChar->pBC = fn_80009B34(nPal, 2, 0x10, "char.c", 0x9BD);
    }
    if (bAll) {
        if (pChar->nAC != 0) {
            Mem_cpy(pChar->pA8, pTexData, nTexBytes);
        }
        if (pChar->nB4 != 0) {
            Mem_cpy(pChar->pB0, pPalData, nPalBytes);
        }
    } else {
        nOut = 0;
        nNames = pChar->nAC - nExtra;
        for (i = 0; i < nNames; i++) {
            bFound = 0;
            for (j = 0; j < nTex; j++) {
                if (pList[i].uId == pTexData[j].u0) {
                    Mem_cpy(&pChar->pA8[nOut], &pTexData[j], sizeof(TexEntry));
                    if (pTexData[j].nPalette != -1) {
                        Mem_cpy(&pChar->pB0[i], &pPalData[pTexData[j].nPalette], sizeof(TexPalette));
                        pChar->pA8[nOut].nPalette = i;
                    }
                    nOut++;
                    if (pTexData[j].b47 & 1) {
                        Mem_cpy(&pChar->pA8[nOut], &pTexData[j + 1], sizeof(TexEntry));
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
        fn_80009E70(pList);
    }
    fn_800100B0(&pChar->bank78, pChar->pA8, pChar->pB0, pChar->pB8, pChar->pBC, pChar->nAC, pChar->nB4);
    pChar->p50 = &pChar->bank78;
    // port: EA passes arguments fn_800CEEBC (empty) ignores
    ((void (*)(Skin*, TexBank*, int, int))fn_800CEEBC)(pChar->pSkin, &pChar->bank78, 0xBF600, 0xCDA);
    sprintf(pChar->szE1, "%sdata\\CharStrm\\CharTex\\%02dalltex.fxg", "", pChar->nC + 1);
    pChar->hFile = fn_800060E0(pChar->szE1);
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

// Queues a dynamic texture job (LLDynTex.c) for the character with its two functions; the pool's
// last entry points at the character, or at nothing when no job is free.
void fn_80019D64(Character* pChar, void (*pfnA)(Character* pChar), void (*pfnB)(Character* pChar)) {
    DynTexJob* pJob = fn_8010B8EC();

    if (pJob != NULL) {
        lbl_801B95E8.a[6].p = pChar;
        pJob->pfnA = pfnA;
        pJob->pChar = pChar;
        pJob->pfnB = pfnB;
        pJob->p0 = &pChar->p50;
        fn_8010B930(pJob);
    } else {
        lbl_801B95E8.a[6].p = NULL;
    }
}

// Sets up the dynamic textures (LLDynTex.c) for the character's model in use.
void fn_80019DE8(Character* pChar) {
    void* pModel = pChar->a64[pChar->n74];

    fn_8008EAC8(0);
    pChar->p60 = pModel;
    fn_8010BC88(&pChar->p50);
    // port: EA passes an argument fn_8010BEC4 ignores
    ((void (*)(void*))fn_8010BEC4)(pModel);
    fn_80019C1C(pChar);
    fn_800CEB1C(pChar->apSkins, pChar->nSkins, pModel);
    fn_800CEBE8(pChar->apSkins, pChar->nSkins, pModel, NULL, 0);
    // port: EA passes an argument fn_8010BED4 ignores
    ((void (*)(void*))fn_8010BED4)(pModel);
}

// Puts the profile's created golfer's logos on the character's model in use.
void fn_80019E80(Character* pChar) {
    fn_80019C84(pChar);
    fn_80019CEC(pChar);
    fn_8001744C(pChar, pChar->a64[pChar->n74], &fn_80077ACC()->choices);
    fn_8010BA2C(pChar->a64[pChar->n74]);
    fn_8008EA38(1);
}

// Sets up the dynamic textures on the character's other model: the model in use is copied to it
// (fn_8010A6A8) and each skin choice that differs from the skin's current one is put on it.
void fn_80019EF4(Character* pChar) {
    int i;
    int j;
    Skin* pSkin;
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
        for (j = 0; j < fn_800CCEA0(pSkin); j++) {
            if (memcmp(&pSkin->aSets[2][j], &pSkin->aSets[3][j], sizeof(SkinChoice)) != 0) {
                fn_800CECE0(pSkin, j, pSkin->aSets[3][j].nVariant, pSkin->aSets[3][j].nOption, pModel);
            }
        }
    }
    fn_80019C1C(pChar);
    fn_800CEB1C(pChar->apSkins, pChar->nSkins, pModel);
    fn_800CEBE8(pChar->apSkins, pChar->nSkins, pModel, NULL, 0);
    // port: EA passes an argument fn_8010BED4 ignores
    ((void (*)(void*))fn_8010BED4)(pModel);
}

// Once the menu golfer is flagged (fn_8008EAD4): switches the character to its other model and
// puts the profile's logos and the skins on it.
void fn_8001A024(Character* pChar) {
    SaveProfile* pProfile = fn_80077ACC();
    int i;
    void* pModel;

    if (fn_8008EAD4()) {
        fn_8008EAC8(0);
        fn_8008E918(0);
        pChar->n74 = 1 - pChar->n74;
        pModel = pChar->a64[pChar->n74];
        fn_80019CEC(pChar);
        fn_8001744C(pChar, pModel, &pProfile->choices);
        fn_8010BA2C(pModel);
        fn_80008380();
        for (i = 0; i < pChar->nSkins; i++) {
            fn_800CE170(pChar->apSkins[i], pModel);
        }
        fn_8010BC64(pModel);
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

// Sets up the dynamic textures for the character's model in use (fn_8010B098), dresses it
// (fn_8001D4A4) and puts its skins on the model, the "Glove" set first; the last marked player
// (lbl_80281CAC) is dressed again.
void fn_8001A14C(Character* pChar) {
    u64 uGlove;
    void* pModel;

    pModel = pChar->a64[pChar->n74];
    pChar->p60 = pModel;
    fn_8010BC88(&pChar->p50);
    fn_8010B098(pModel);
    // port: EA passes an argument fn_8010BEC4 ignores
    ((void (*)(void*))fn_8010BEC4)(pModel);
    fn_8001D4A4(pChar, pChar->nPlayer);
    fn_80019C1C(pChar);
    fn_800CB700(&uGlove, "Glove");
    fn_800CEBE8(pChar->apSkins, pChar->nSkins, pModel, &uGlove, 1);
    if (lbl_80281CAC >= 0) {
        fn_8001D4A4(gPlayers[lbl_80281CAC].pChar, lbl_80281CAC);
    }
    // port: EA passes an argument fn_8010BED4 ignores
    ((void (*)(void*))fn_8010BED4)(pModel);
}

void fn_8001A20C(Character* pChar) {
    fn_80019C84(pChar);
    fn_80019CEC(pChar);
    fn_8001744C(pChar, pChar->a64[pChar->n74], pChar->pChoices);
    fn_8010BA2C(pChar->a64[pChar->n74]);
    pChar->bE0 = 1;
    lbl_801B95E8.a[6].p = NULL;
}

// Fills the pool with dynamic textures (LLDynTex.c), all free. Every game type gets two.
void fn_8001A288(void) {
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

// With more than two players, only the player with the honor keeps pool entries: every player's
// character gives its back, then that player's takes them and queues its dynamic textures.
void fn_8001A4BC(void) {
    Character* pChar;
    int i;

    lbl_80281CAC = -1;
    if (gSession.nNumPlayers > 2) {
        for (i = 0; i < gSession.nNumPlayers; i++) {
            fn_8001A484(gPlayers[i].pChar);
            fn_8001A3B0(gPlayers[i].pChar);
        }
        i = gpGame->pfnGetHonors(5);
        pChar = gPlayers[i].pChar;
        fn_8001A418(pChar);
        fn_80019D64(pChar, fn_8001A14C, fn_8001A20C);
        fn_8010BF68();
    }
}

// With more than two players, the pool entries go to player nPlayer and the next player
// (fn_800E295C): every other character holding entries (except the one queued last) gives them
// back and gets bit 0x40 of u10; nPlayer's character loses that bit, and unless it has its entries
// already it takes them (the queued character giving its back first) and queues its dynamic
// textures. The next player's character is then queued the same way.
void fn_8001A58C(int nPlayer) {
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
                fn_8001A3B0(gPlayers[i].pChar);
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
                    fn_8001A3B0(pQueued);
                    pQueued->bE0 = 0;
                    pQueued->u10 |= 0x40;
                }
                fn_8001A418(pChar);
                fn_80019D64(pChar, fn_8001A14C, fn_8001A20C);
                fn_8010BF68();
            } else {
                fn_8010BF68();
            }
        }
        i = fn_800E295C();
        if (i < gSession.nNumPlayers) {
            pChar = gPlayers[i].pChar;
            if (!pChar->bE0 && pChar != lbl_801B95E8.a[6].p) {
                fn_8010BF68();
                fn_8001A418(pChar);
                fn_80019D64(pChar, fn_8001A14C, fn_8001A20C);
            }
        }
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

// Reopens every player's character texture file: closes them all, then opens
// "data\CharStrm\CharTex\NNalltex.fxg" for each golfer (NN is its id + 1).
void fn_8001A870(void) {
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

// With two players or fewer, every player's character takes its pool entries and queues its
// dynamic textures (in split screen it is flagged for fn_8001D6D8 too); then the animation slots
// take their overlays and the work copies are freed.
void fn_8001A920(void) {
    int i;
    Character* pChar;

    fn_80095558();
    if (gSession.nNumPlayers <= 2) {
        for (i = 0; i < gSession.nNumPlayers; i++) {
            pChar = gPlayers[i].pChar;
            fn_8001A418(pChar);
            fn_80019D64(pChar, fn_8001A14C, fn_8001A20C);
            fn_8010BF68();
            if (gSession.nSplitScreen) {
                fn_8001D6D8(i);
            }
        }
    }
    AnimLib_ApplyOverlays(0);
    AnimLib_ApplyOverlays(1);
    fn_800CA9DC(-1);
    fn_800CA7E0();
    fn_80025478();
    fn_8001A7F0();
    AnimLib_FreeWorkCopies();
    fn_800CABA0();
    fn_800CB078();
}

// Builds a character from its CHR object: a header (its animation slot and a few values), its
// skin, the p44 entries, its model (fn_80028564), its own animation library, and its slider
// definitions; then a golfer's club skins and, with bLook, its look from pChoices. Every value read
// is followed by 12 bytes it skips.
// port: the object is little-endian on disc and fn_80076158 swaps each value as it reads it: a
//       little-endian port does not swap there.
Character* fn_8001A9F4(u8* pData, int nUnused, int nSet, int nId, u8 bLook, SkinChoices* pChoices) {
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
    CharModelDefs* pDefs = NULL;
    u8* pStart;
    u8* pCopy;
    AnimLib* pLib;
    Skin* pSkin;
    u8 bGolfer;
    u8 bModel;
    int i;

    pChar = fn_8001942C();
    if (pChar == NULL) {
        return NULL;
    }
    pChar->nC = nId;
    pStart = pData;
    fn_80076158(&pData, (u8*)&pChar->nSlot, 4, 4);
    fn_80076158(&pData, (u8*)&fSkin, 4, 4);
    fn_80076158(&pData, (u8*)&nFlag400, 4, 4);
    fn_80076158(&pData, (u8*)&nModel, 4, 4);
    fn_80076158(&pData, (u8*)&f134, 4, 4);
    fn_80076158(&pData, (u8*)&f138, 4, 4);
    fn_80076158(&pData, (u8*)&f13C, 4, 4);
    fn_80076158(&pData, (u8*)&f12C, 4, 4);
    fn_80076158(&pData, (u8*)&f130, 4, 4);
    if (gSession.nGameType == 3) {
        ClipBank_Restore(pChar->nSlot);
    }
    pData += 0xC;
    if (nFlag400 == 1) {
        pChar->u10 |= 0x400;
    }
    bGolfer = fn_8001EC48(pChar);

    // its skin
    fn_80076158(&pData, (u8*)&nSize, 4, 4);
    pData += 0xC;
    if (nSize == 0) {
        pChar->pSkin = NULL;
    } else {
        pChar->pSkin = fn_800377FC(pData, bLook);
        pChar->pSkin->f10D8 = fSkin;
        pChar->pSkin->f10DC = 1.0f;
        if (gSession.nGameType != 10 && gSession.nGameType != 3 && fn_8001EC48(pChar)) {
            fn_80037708(pChar->pSkin);
        }
    }
    pData += nSize;

    // the p44 entries
    fn_80076158(&pData, (u8*)&pChar->n40, 4, 4);
    pData += 0xC;
    pChar->p44 = fn_80009B34(pChar->n40 * sizeof(CharEntry44), 2, 0x40, "char.c", 0xE04);
    for (i = 0; i < pChar->n40; i++) {
        fn_80076158(&pData, (u8*)pChar->p44[i].v0, 0xC, 4);
        pChar->p44[i].fC = 1.0f;
        fn_80076158(&pData, (u8*)&pChar->p44[i].a10[0], 4, 4);
        fn_80076158(&pData, (u8*)&pChar->p44[i].a10[1], 4, 4);
        fn_80076158(&pData, (u8*)&pChar->p44[i].a10[2], 4, 4);
        fn_80076158(&pData, (u8*)&pChar->p44[i].a10[3], 4, 4);
        fn_80076158(&pData, (u8*)&pChar->p44[i].a10[4], 4, 4);
        fn_80076158(&pData, (u8*)&pChar->p44[i].a10[5], 4, 4);
        pData += 0xC;
    }

    // its model
    fn_80076158(&pData, (u8*)&nSize, 4, 4);
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
        bModel = pChoices->n113;
    }
    fn_80018484(pChar, fn_80028564(pData, 1, pDefs, bModel));
    if (pChar->pSkin != NULL) {
        fn_800184E4(pChar, pChar->pSkin);
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
    fn_80076158(&pData, (u8*)&bLib, 4, 4);
    pData += 0xC;
    if (bLib != 0) {
        fn_80076158(&pData, (u8*)&nSize, 4, 4);
        pData += 0xC;
        pPeek = pData + 0x138;
        fn_80076158(&pPeek, (u8*)&nBank, 4, 4);
        pPeek = pData + 0x13C;
        fn_80076158(&pPeek, (u8*)&uLibFlags, 4, 4);
        if (pChar->nSlot == 2 || nBank != 0 || (uLibFlags & 1)) {
            pCopy = fn_80009B34(nSize, 2, 0x40, "char.c", 0xE54);
        } else {
            pCopy = fn_80009B34(nSize, 1, 0x40, "char.c", 0xE57);
        }
        Mem_cpy(pCopy, pData, nSize);
        pLib = AnimLib_Load(pCopy, ClipBank_Get(pChar->nSlot));
        if (pLib->pBank != NULL || (pLib->uFlags & 1)) {
            pChar->pLib = pLib;
        } else {
            lbl_801C6068[pChar->nSlot].overlays[lbl_801C6068[pChar->nSlot].nOverlays].pWork = pLib;
            lbl_801C6068[pChar->nSlot].overlays[lbl_801C6068[pChar->nSlot].nOverlays].pCopy =
                fn_80009B34(nSize, 2, 0x40, "char.c", 0xE68);
            Mem_cpy(lbl_801C6068[pChar->nSlot].overlays[lbl_801C6068[pChar->nSlot].nOverlays].pCopy,
                    pData, nSize);
            lbl_801C6068[pChar->nSlot].overlays[lbl_801C6068[pChar->nSlot].nOverlays].nSize = nSize;
            lbl_801C6068[pChar->nSlot].overlays[lbl_801C6068[pChar->nSlot].nOverlays].pChar = pChar;
            lbl_801C6068[pChar->nSlot].overlays[lbl_801C6068[pChar->nSlot].nOverlays].n10 = nId + 3;
            lbl_801C6068[pChar->nSlot].overlays[lbl_801C6068[pChar->nSlot].nOverlays].bActive = bLook;
            lbl_801C6068[pChar->nSlot].nOverlays++;
            pChar->pLib = fn_80009B34(0x2800, 2, 0x40, "char.c", 0xE75);
        }
        pData += nSize;
    } else {
        pChar->pLib = NULL;
    }

    pChar->p17AC = CharSlider_CreateDefinitionsFromMem(&pData);
    pChar->p4C = pData;
    fn_80018710(pChar);
    if (bGolfer) {
        pChar->p16D8 = lbl_80280E24[nSet];
        pChar->nClubHeadBone = fn_8001EED8(pChar->pModel, 0x53);
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
                fn_80037AB8(pSkin, pChar->pModel, fn_8001EED8(pChar->pModel, 0x52) - 0x52, 0x52);
            }
        }
    }
    pChar->pChoices = pChoices;
    if (bLook) {
        fn_8001DC64(pChar, pChoices);
        fn_800CC4EC(pChar);
    }
    if (gSession.nSplitScreen && fn_8001EC48(pChar)) {
        fn_800375AC(pChar->pSkin, 0);
        fn_80018710(pChar);
    }
    return pChar;
}

// Makes a club skin set from a 'CLB ' object: per entry its club class, that class's afC, the
// entry's size and, 4 bytes on, its skin (fn_800377FC); a3C gets the class's club point.
CharSkinSet* fn_8001B208(u8* pData) {
    CharSkinSet* pSet;
    s32 nSize;
    s32 nClass;
    int i;

    pSet = fn_80009B34(sizeof(CharSkinSet), 2, 0x40, "char.c", 0xF15);
    if (pSet == NULL) {
        return NULL;
    }
    pSet->apSkins[0] = NULL;
    pSet->apSkins[1] = NULL;
    pSet->apSkins[2] = NULL;
    pSet->apSkins[3] = NULL;
    pSet->apSkins[4] = NULL;
    pSet->apSkins[5] = NULL;
    fn_80076158(&pData, (u8*)&pSet->nCount, 4, 4);
    pData += 0xC;
    for (i = 0; i < pSet->nCount; i++) {
        fn_80076158(&pData, (u8*)&nClass, 4, 4);
        fn_80076158(&pData, (u8*)&pSet->afC[nClass], 4, 4);
        fn_80076158(&pData, (u8*)&nSize, 4, 4);
        pData += 4;
        pSet->apSkins[nClass] = fn_800377FC(pData, 0);
        pSet->a9C[nClass] = fn_80009B34(sizeof(CharSkinRef), 2, 0x40, "char.c", 0xF25);
        fn_8001B1DC(pSet->apSkins[nClass], pSet->a9C[nClass], sizeof(CharSkinRef));
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
// not used: fn_8001C468 passes the set it found, but both are freed here.
void fn_8001B58C(CharSkinSet* pSet) {
    int j;
    int i;

    for (i = 0; i < 2; i++) {
        if (lbl_80280E24[i] != NULL) {
            for (j = 0; j < 6; j++) {
                if (lbl_80280E24[i]->apSkins[j] != NULL) {
                    fn_80037CD8(lbl_80280E24[i]->apSkins[j]);
                }
                if (lbl_80280E24[i]->a9C[j] != NULL) {
                    fn_8001B1E8(lbl_80280E24[i]->a9C[j]);
                }
            }
            fn_80009E70(lbl_80280E24[i]);
            lbl_80280E24[i] = NULL;
        }
    }
}

#define MIN(a, b) ((a) <= (b) ? (a) : (b))
#define MAX(a, b) ((a) <= (b) ? (b) : (a))

// The character's bounding box and sphere, from its bones' positions (bone 1 on).
void fn_8001B644(Character* pChar) {
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
    fn_8001EF54(pChar->vMin, pChar->vMax, vCentre);
    fn_8001EF34(vCentre, 0.5f, vCentre);
    pChar->v1668[0] = vCentre[0];
    pChar->v1668[1] = vCentre[1];
    pChar->v1668[2] = vCentre[2];
    fn_8001EF10(pChar->vMax, pChar->vMin, vDiff);
    pChar->f1674 = (f32)fn_80009680(fn_80009744(vDiff)) / 2.0f;
}

// How the camera sees the character: n1654 and n1658 are fn_80007D74's answers for its bounding
// sphere and for a 3-unit one (2 when it is not the view's player or is too far away), f14 the
// nearest it has been, f1664 1 up close fading to 0 between 6 and 15 units deep.
void fn_8001B878(Character* pChar, int nPlayer) {
    f32 (*pMtx)[4];
    f32 fDepth;
    f32 fDist;
    f32 fLen;
    Sphere sphere;
    Vec4 vPos;
    f32 vDir[4];

    pMtx = fn_8001EE64(pChar);
    if (nPlayer != 1000 && nPlayer != fn_8001707C(fn_80016D10())) {
        pChar->n1654 = pChar->n1658 = 2;
        return;
    }
    Vec3Copy(pChar->v1668, &vPos.x);
    vPos.w = 1.0f;
    fn_800BAD60(((Camera*)fn_8001614C())->viewMtx, &vPos, &vPos);
    Vec3Copy(&vPos.x, &sphere.x);
    sphere.radius = pChar->f1674;
    fDepth = sphere.z;
    pChar->n1654 = fn_80007D74(&sphere, fn_8001614C(), 0);
    sphere.radius = 3.0f;
    pChar->n1658 = fn_80007D74(&sphere, fn_8001614C(), 0);
    fn_8001EFB4(pMtx[3], fn_8001F004()->v34, vDir);
    fDist = fn_8000C5FC(fn_8001F004()->v24, vDir);
    fLen = (f32)fn_80009680(fn_80009744(vDir));
    if (fLen < pChar->f14) {
        pChar->f14 = fLen;
    }
    if (fDist > fn_8001ED44(pChar, gSession.nSplitScreen)) {
        pChar->n1654 = 2;
    }
    if (fDist > fn_8001EE00(pChar, gSession.nSplitScreen)) {
        pChar->n1658 = 2;
    }
    fDepth *= fn_8001EFFC(fn_8001F004());
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

// For every character made: bit 0x1000 of u10 cleared; with bit 2, bit 1 follows whether the
// flagstick is out on the current view. Then fn_80035B40 for every character that is not in state
// 2 (fn_8001EE90) or whose n1658 is not 2, is not the camera's player (fn_800636EC), has none of bits
// 0x1000, 0x40 and 1 of u10 set, and has n1698 0.
void fn_8001BA74(void) {
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
            if (fn_80016CF4()->bFlagOut) {
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

// With characters made: fn_80035604, then fn_800358E0 for every character that is not in state 2
// (fn_8001EE90), not the camera's player (fn_800636EC), has neither bit 0x40 nor 1 of u10 set and,
// when uFlags has bit 4, passes fn_8001EC48.
void fn_8001BBD8(u32 uFlags) {
    int i;
    int nPlayer;

    if (lbl_80281CA8 != 0) {
        fn_80035604();
        for (i = 0; i < lbl_80281CA8; i++) {
            nPlayer = fn_800636EC();
            if (fn_8001EE90(lbl_801B9624[i]) != 2 && nPlayer != lbl_801B9624[i]->nPlayer &&
                !(lbl_801B9624[i]->u10 & 0x41) &&
                (fn_8001EC48(lbl_801B9624[i]) || (uFlags & 4) == 0)) {
                fn_800358E0(lbl_801B9624[i], uFlags);
            }
        }
    }
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

// With a clip of flag 0x10 the grip bone goes back to its parent; otherwise it is cut loose
// (parent 0) and its rotation and offset from the root are kept in q16AC and v16BC (mirrored
// while bEE is set). 1 when the state changed, 0 when it already was that way.
int fn_8001BD18(Character* pChar, Clip* pClip) {
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
        fn_80008F20(pModel->pPoses[0].q0, qRoot);
        fn_80008FCC(pModel->pPoses[pChar->nGripBone].q0, qRoot, pChar->q16AC);
        if (fn_8001EDF4(pChar)) {
            fn_80009410(PI, qTurn);
            fn_80008FCC(pChar->q16AC, qTurn, qGrip);
            fn_8001E85C(qGrip, pChar->q16AC);
        }
        fn_8001EFB4(pModel->pPoses[pChar->nGripBone].v10, pModel->pPoses[0].v10, vOffset);
        vOffset[3] = 0.0f;
        fn_800090E4(qRoot, vOffset, pChar->v16BC);
        pChar->v16BC[3] = 0.0f;
        if (fn_8001EDF4(pChar)) {
            pChar->v16BC[2] = -pChar->v16BC[2];
        }
        return 1;
    }
    return 0;
}

// Plays pClip on the character. With bNoBlend the blend tree and the animation player start
// over; otherwise the clip is blended in over its first f18 seconds from the current time.
void fn_8001BE88(Character* pChar, Clip* pClip, int bNoBlend, f32 fTime) {
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
    if (fn_8001EC48(pChar)) {
        fn_8001BD18(pChar, pClip);
    }
    pChar->n178C = 0;
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
        fn_800732F4(&pChar->blend, pChar->anim, pChar->fAnimTime + fTime);
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
    fn_800175B0(pChar, pClip, aBlend[3]);
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
void fn_8001C0E0(Character* pChar) {
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
        pNode = (SKABlendNode*)pChar->node3E0;  // a node without the root's nGroup
        fn_80071F58(&pNode, 0);
        if (pChar->pSkin != NULL) {
            fn_80037CD8(pChar->pSkin);
        }
        if (pChar->pLib != NULL) {
            AnimLib_Free(pChar->pLib);
        }
        fn_8002957C(pChar->pModel);
        pChar->pModel = NULL;
        for (i = 0; i < 4; i++) {
            fn_80009E70(pChar->buffers[i].pBuf);
        }
        if (pChar->p44 != NULL) {
            fn_80009E70(pChar->p44);
        }
        if (pChar->pRecords != NULL) {
            fn_80009E70(pChar->pRecords);
        }
        if (fn_8001EC48(pChar)) {
            fn_8001971C(pChar);
        }
        if (pChar->p17AC != NULL) {
            fn_8010D454(pChar->p17AC);
        }
        fn_80009E70(pChar);
        if (gSession.nGameType == 3) {
            ClipBank_Release(nSlot);
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

// Starts the character system (called once from the main loop): the animation libraries up, no
// club skin sets, the club names read as 64-bit ids, no characters in the menu or player slots,
// and no player marked.
// port: the names are read as big-endian 64-bit words from their strings (FEgolferanim compares
//       them with ids read the same way)
void fn_8001C37C(void) {
    int i;

    Skalib_Init();
    fn_8001F64C();
    fn_80029530();
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

// Frees the club skin sets and every character made, then shuts down the animation libraries.
void fn_8001C468(void) {
    int i;

    for (i = 0; i < 2; i++) {
        if (lbl_80280E24[i] != NULL) {
            fn_8001B58C(lbl_80280E24[i]);
        }
        lbl_80280E24[i] = NULL;
    }
    for (i = 0; i < lbl_80281CA8; i++) {
        fn_8001C0E0(lbl_801B9624[i]);
        lbl_801B9624[i] = NULL;
    }
    lbl_80281CA8 = 0;
    Skalib_Shutdown();
    fn_8001F66C();
    fn_8002955C();
    fn_80071B94();
}

// Frees every character of the menu's golfer slots (lbl_80281EE8).
void fn_8001C518(void) {
    int i;

    for (i = 0; i < CRAP_NUM_GOLFERS; i++) {
        fn_8001C0E0(lbl_80281EE8[i]);
        lbl_80281EE8[i] = NULL;
    }
}

// The model id of the player's golfer.
int fn_8001C558(int nPlayer) {
    return gGolferTable[gSession.nGolfer[nPlayer]].nModelID;
}

// The player's golfer is one of records 30 to 33.
int fn_8001C584(int nPlayer) {
    int b = 0;
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

// Puts the golfer at its player's ball, facing the target (level), and resets its root bone's pose
// and matrix. With u10 bit 8 and a skeleton, it then takes the stance of its clip for the style:
// the clip is started on the skeleton when it changed (at pD4's f24 with bit 0x10000), the root is
// moved by the club class's offset (mirrored when the model is), the pose updated, the feet placed
// and both leg chains moved with the root, and the IK weight set (1 in the swing's states 5 and 7).
// Bits 4, 8, 0x200 and 0x10000 of u10 are cleared; 0x200 also places the feet on the terrain.
void fn_8001C860(Character* pChar) {
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
    pBallPos = pPlayer->ball.vPos;
    Character_SetPosition(pChar, pBallPos, 0);
    pChar->u10 &= ~(0x10000 | 0x200 | 8 | 4);
    // fake match: n2C is compared unsigned here
    if (pChar->p1798 != NULL && (u32)pChar->p1798->n2C == 6 && pChar->n16D4 == 0) {
        pChar->n16D4 = 4;
    }
    fn_8001EFB4(pPlayer->vTarget, pBallPos, vDir);
    vDir[1] = 0.0f;
    fn_80019358(pChar, vDir, 0.0f);
    fn_8001E85C(pModel->pBones[0].q0C, pModel->pPoses[0].q0);
    fn_8001E85C(pModel->pBones[0].v1C, pModel->pPoses[0].v10);
    fn_8000914C(pModel->pPoses[0].q0, pModel->pMatrices[0]);
    fn_8001E880(pModel->pPoses[0].v10, pModel->pMatrices[0][3]);
    if (bStance && (pSkel = pChar->pModel->pSkel) != NULL) {
        pOldClip = pChar->pCurClip;
        fn_8001EED8(pChar->pModel, 1);      // the results are not used
        fn_8001EED8(pChar->pModel, 0x52);
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
        fn_8001BD18(pChar, pSkel->pClip);
        fn_8000AE28(pChar->pModel->pMatrices[0][0], -lbl_80187184[pChar->nClubClass][0], vOffsetX);
        fn_8000AE28(pChar->pModel->pMatrices[0][2], -lbl_80187184[pChar->nClubClass][2], vOffsetZ);
        if (pChar->pModel->bEE) {
            vOffsetZ[0] = -vOffsetZ[0];
            vOffsetZ[2] = -vOffsetZ[2];
        }
        fn_8001EFD8(vOffsetX, pChar->pModel->pBones[0].v1C, pChar->pModel->pBones[0].v1C);
        fn_8001EFD8(vOffsetZ, pChar->pModel->pBones[0].v1C, pChar->pModel->pBones[0].v1C);
        fn_8001E85C(pModel->pBones[0].v1C, pModel->pPoses[0].v10);
        fn_8001E880(pModel->pPoses[0].v10, pModel->pMatrices[0][3]);
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
            fn_8001BD18(pChar, pOldClip);
        }
    }
    if (pChar->pModel->pSkel != NULL) {
        pChar->pModel->pSkel->f1074 = 0.0f;
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
        nGolferModel = fn_8001C558(i);
        if (nGolferModel == uModel && gViewSlots[i].pChar == NULL) {
            fn_800106AC(i);
            nSet = gSession.nSplitScreen ? i : 0;
            gViewSlots[i].pChar = fn_8001C21C(fn_8001A9F4(pObject->pData, 0, nSet, uModel,
                                                          fn_8001C584(i), &gpSaveData[i].choices));
            if (gViewSlots[i].pChar->pSkin != NULL && fn_8001EC48(gViewSlots[i].pChar)) {
                fn_8001D4A4(gViewSlots[i].pChar, i);
            }
            if (gViewSlots[i].pChar->pSkin != NULL && fn_8001EC48(gViewSlots[i].pChar)) {
                pChar = gViewSlots[i].pChar;
                pChar->apSkins[0] = pChar->pSkin;
                pChar->apSkins[1] = pChar->p16D8->apSkins[0];
                pChar->apSkins[2] = pChar->p16D8->apSkins[1];
                pChar->apSkins[3] = pChar->p16D8->apSkins[2];
                pChar->apSkins[4] = pChar->p16D8->apSkins[3];
                pChar->apSkins[5] = pChar->p16D8->apSkins[4];
                pChar->apSkins[6] = pChar->p16D8->apSkins[5];
                pChar->nSkins = 7;
                fn_80019798(pChar, pChar->apSkins, pChar->nSkins);
            }
        }
    }
    fn_800106B8(0);
    fn_80009E70(pObject);
}

// The 'CHR ' stream objects: two handlers for the same type.
void fn_8001CFF0(void) {
    UStream_RegisterHandler('CHR ', fn_8001CE5C);
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

    fn_8000A0BC();
    fn_8000A0C8();
    fn_8008F310();
    pCopy = fn_8008F354();
    Mem_cpy(pCopy, pObject, pObject->uSize + 0x80);
    fn_80009E70(pObject);
    pCopy->pData = (u8*)pCopy + 0x80;
    lbl_80281EE0->pB8->pChar = fn_8001A9F4(pCopy->pData, 0, 0, pCopy->uId, 0, NULL);
    fn_8000A0D4();
    fn_8000A0E0();
    fn_80019798(lbl_80281EE0->pB8->pChar, NULL, 0);
    fn_8008F35C();
    lbl_80281EE0->pB8->pChar->n16C = -1;
    Character_SetPosition(lbl_80281EE0->pB8->pChar, lbl_80189A30, 1);
    fn_800192D4(lbl_80281EE0->pB8->pChar, lbl_80281EE0->f19C);
    if (lbl_80281EE0->pB8->pChar->nC == 7 || lbl_80281EE0->pB8->pChar->nC == 29) {
        fn_8001DC64(lbl_80281EE0->pB8->pChar, &fn_80077ACC()->choices);
    }
    fn_8001C5B4(lbl_80281EE0->pB8->pChar, 5);
    pClip = Char_SetClip(lbl_80281EE0->pB8->pChar, 0, 0, NULL);
    fn_8001BE88(lbl_80281EE0->pB8->pChar, pClip, 1, 0.0f);
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
        fn_8001A418(pChar);
    }
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

// Dresses the character of player slot nSlot: its club skins (in game type 3 golfers 7 and 29 get
// the profile's created golfer's look; otherwise its own look when fn_8001C584 says so), then the
// "shirt" set (only when fn_8001C584 says no) and the "glove" set, as "shirt<n>" / "glove<n>" with
// n from the slot's profile (no number when it is 0 or less).
void fn_8001D4A4(Character* pChar, int nSlot) {
    char szName[32];            // the size is not known

    if (gSession.nGameType == 3) {
        if (pChar->nC == 7 || pChar->nC == 29) {
            Character_SetClubStatesForCharacter(pChar, nSlot, &fn_80077ACC()->choices);
        } else {
            Character_SetClubStatesForCharacter(pChar, nSlot, NULL);
        }
    } else if (fn_8001C584(nSlot)) {
        Character_SetClubStatesForCharacter(pChar, nSlot, pChar->pChoices);
    } else {
        Character_SetClubStatesForCharacter(pChar, nSlot, NULL);
    }
    if (!fn_8001C584(nSlot)) {
        sprintf(szName, "%s", "shirt");
        if (gSession.aProfile[nSlot].n0 > 0) {
            sprintf(szName, "%s%d", szName, gSession.aProfile[nSlot].n0);
        }
        fn_800CC658(pChar, "shirt", szName, NULL);
    }
    sprintf(szName, "%s", "glove");
    if (gSession.aProfile[nSlot].n2 > 0) {
        sprintf(szName, "%s%d", szName, gSession.aProfile[nSlot].n2);
    }
    fn_800CC658(pChar, "glove", szName, NULL);
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

// Every player flagged by fn_8001D6D8 has its character dressed again (fn_8001D4A4) and its
// skins put on its model in use; the flag is cleared.
void fn_8001D6F0(void) {
    int j;
    Character* pChar;
    int i;

    for (i = 0; i < 5; i++) {
        if (gSession.aD28[i]) {
            fn_80008380();
            pChar = gPlayers[i].pChar;
            fn_8001D4A4(pChar, i);
            fn_80019CEC(pChar);
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
    pNode = (SKABlendNode*)pChar->node3E0;
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
    fn_8001A58C(nPlayer);
    if (!gSession.nSplitScreen && lbl_80281CAC != nPlayer) {
        fn_8001D6D8(nPlayer);
        lbl_80281CAC = nPlayer;
    }
}

// Where the ball sits on the hand: bone 0x1A's position, moved 0.05 along the bone's x axis (the
// other way while the model's bEE is set).
void Character_GetBallOnFingerPosition(Character* pChar, f32* pPos) {
    f32 (*pMtx)[4] = fn_8001EC6C(pChar, 0x1A);
    f32 vAxis[3];

    Vec_Copy(pMtx[3], pPos);
    Vec3Copy(pMtx[0], vAxis);
    fn_800BAF04(vAxis, vAxis);
    if (fn_8001EDF4(pChar)) {
        fn_8000C5D4(pPos, vAxis, 0.05f, pPos);
    } else {
        fn_8000C5D4(pPos, vAxis, -0.05f, pPos);
    }
}

// Where the hand holds the ball, for the swing: bone 0x1A's position moved 0.000625 along the
// bone's x axis (the other way while the model's bEE is set), and bone 0x15's pose as three angles
// (fn_800094D8), the second 30 degrees more.
void fn_8001DA04(Character* pChar, f32* pPos, f32* pAngles) {
    f32 (*pMtx)[4];
    f32 vAxis[3];

    if (pChar != NULL && pChar->pModel != NULL) {
        pMtx = fn_8001EC6C(pChar, 0x1A);
        Vec_Copy(pMtx[3], pPos);
        Vec3Copy(pMtx[0], vAxis);
        fn_800BAF04(vAxis, vAxis);
        if (fn_8001EDF4(pChar)) {
            fn_8000C5D4(pPos, vAxis, -0.000625f, pPos);
        } else {
            fn_8000C5D4(pPos, vAxis, 0.000625f, pPos);
        }
        fn_800094D8(pChar->pModel->pPoses[fn_8001EEE4(pChar->pModel, 0x15)].q0, &pAngles[0], &pAngles[1],
                    &pAngles[2]);
        pAngles[1] += 30.0f / 180.0f * PI;
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

// Gives the character the look in pChoices: its skins' choices, then its sliders (the 26 values
// at a9B4). Outside the menu golfer's game type 3 (or on its screens 1 and 4) n113 sets the
// model's bEE.
void fn_8001DC64(Character* pChar, SkinChoices* pChoices) {
    fn_800CC1EC(pChar, pChoices);
    fn_8010E4DC(pChar->p17AC, pChar->pModel, pChar->pSkin, 26, pChoices->a9B4,
                (SKABlendNode*)pChar->node3E0);
    if (gSession.nGameType != 3 || lbl_80281EE0->n0 == 1 || lbl_80281EE0->n0 == 4) {
        if (pChoices->n113 == 0) {
            fn_8001EE98(pChar, 0);
        } else {
            fn_8001EE98(pChar, 1);
        }
    }
    fn_80018484(pChar, pChar->pModel);
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
                fn_8001F08C(&pSrc, &pDst, aRecord, 5, 1);
                pData += 12;
            }
            pDst = pData;
            pSrc = pData;
            fn_8001F08C(&pSrc, &pDst, aTail, 14, 1);
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
        fn_8001F08C(&pSrc, &pDst, aFormat, 5, 1);
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
            fn_800CB700(&uName, lbl_80186EC0[0]);
            fn_800CC710(pChar, 0, uName, gGolferTable[nGolfer].aClubs[0].uPart);
            fn_800CB700(&uName, lbl_80186FB0[0]);
            fn_800CB700(&uVariant, lbl_80187000[0]);
            fn_800CC7DC(pChar, 0, uName, uVariant, gGolferTable[nGolfer].aClubs[0].uModel);
            fn_800CB700(&uName, lbl_80186F10[0]);
            fn_800CB700(&uVariant, lbl_80186F60[0]);
            fn_800CC7DC(pChar, 0, uName, uVariant, gGolferTable[nGolfer].aClubs[0].uShaft);
            fn_800CB700(&uName, lbl_80187050[0]);
            fn_800CB700(&uVariant, lbl_801870A0[0]);
            fn_800CC7DC(pChar, 0, uName, uVariant, gGolferTable[nGolfer].aClubs[0].uGrip);

            fn_800CB700(&uName, lbl_80186EC0[1]);
            fn_800CC710(pChar, 1, uName, gGolferTable[nGolfer].aClubs[1].uPart);
            fn_800CB700(&uName, lbl_80186FB0[1]);
            fn_800CB700(&uVariant, lbl_80187000[1]);
            fn_800CC7DC(pChar, 1, uName, uVariant, gGolferTable[nGolfer].aClubs[1].uModel);
            fn_800CB700(&uName, lbl_80186F10[1]);
            fn_800CB700(&uVariant, lbl_80186F60[1]);
            fn_800CC7DC(pChar, 1, uName, uVariant, gGolferTable[nGolfer].aClubs[1].uShaft);
            fn_800CB700(&uName, lbl_80187050[1]);
            fn_800CB700(&uVariant, lbl_801870A0[1]);
            fn_800CC7DC(pChar, 1, uName, uVariant, gGolferTable[nGolfer].aClubs[1].uGrip);

            fn_800CB700(&uName, lbl_80186EC0[3]);
            fn_800CC710(pChar, 3, uName, gGolferTable[nGolfer].aIronPart[0]);
            fn_800CB700(&uName, lbl_80186FB0[3]);
            fn_800CB700(&uVariant, lbl_80187000[3]);
            fn_800CC7DC(pChar, 3, uName, uVariant, gGolferTable[nGolfer].uIronModel);
            fn_800CB700(&uName, lbl_80186F10[3]);
            fn_800CB700(&uVariant, lbl_80186F60[3]);
            fn_800CC7DC(pChar, 3, uName, uVariant, gGolferTable[nGolfer].uIronShaft);
            fn_800CB700(&uName, lbl_80187050[3]);
            fn_800CB700(&uVariant, lbl_801870A0[3]);
            fn_800CC7DC(pChar, 3, uName, uVariant, gGolferTable[nGolfer].uIronGrip);

            fn_800CB700(&uName, lbl_80186EC0[4]);
            fn_800CC710(pChar, 4, uName, gGolferTable[nGolfer].aIronPart[1]);
            fn_800CB700(&uName, lbl_80186FB0[4]);
            fn_800CB700(&uVariant, lbl_80187000[4]);
            fn_800CC7DC(pChar, 4, uName, uVariant, gGolferTable[nGolfer].uIronModel);
            fn_800CB700(&uName, lbl_80186F10[4]);
            fn_800CB700(&uVariant, lbl_80186F60[4]);
            fn_800CC7DC(pChar, 4, uName, uVariant, gGolferTable[nGolfer].uIronShaft);
            fn_800CB700(&uName, lbl_80187050[4]);
            fn_800CB700(&uVariant, lbl_801870A0[4]);
            fn_800CC7DC(pChar, 4, uName, uVariant, gGolferTable[nGolfer].uIronGrip);

            fn_800CB700(&uName, lbl_80186EC0[5]);
            fn_800CC710(pChar, 5, uName, gGolferTable[nGolfer].wedges.uPart);
            fn_800CB700(&uName, lbl_80186FB0[5]);
            fn_800CB700(&uVariant, lbl_80187000[5]);
            fn_800CC7DC(pChar, 5, uName, uVariant, gGolferTable[nGolfer].wedges.uModel);
            fn_800CB700(&uName, lbl_80186F10[5]);
            fn_800CB700(&uVariant, lbl_80186F60[5]);
            fn_800CC7DC(pChar, 5, uName, uVariant, gGolferTable[nGolfer].wedges.uShaft);
            fn_800CB700(&uName, lbl_80187050[5]);
            fn_800CB700(&uVariant, lbl_801870A0[5]);
            fn_800CC7DC(pChar, 5, uName, uVariant, gGolferTable[nGolfer].wedges.uGrip);

            fn_800CB700(&uName, lbl_80186EC0[2]);
            fn_800CC710(pChar, 2, uName, gGolferTable[nGolfer].aClubs[2].uPart);
            fn_800CB700(&uName, lbl_80186FB0[2]);
            fn_800CB700(&uVariant, lbl_80187000[2]);
            fn_800CC7DC(pChar, 2, uName, uVariant, gGolferTable[nGolfer].aClubs[2].uModel);
            fn_800CB700(&uName, lbl_80186F10[2]);
            fn_800CB700(&uVariant, lbl_80186F60[2]);
            fn_800CC7DC(pChar, 2, uName, uVariant, gGolferTable[nGolfer].aClubs[2].uShaft);
            fn_800CB700(&uName, lbl_80187050[2]);
            fn_800CB700(&uVariant, lbl_801870A0[2]);
            fn_800CC7DC(pChar, 2, uName, uVariant, gGolferTable[nGolfer].aClubs[2].uGrip);
        }
    } else {
        fn_800CC408(pChar, pChoices);
    }
    fn_800CC8BC(pChar, fn_8001EDF4(pChar));
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

// Each bit of aOut is set where both aA and aB have it (bit arrays of nBits bits).
void fn_8001EA54(u32* aA, u32* aB, u32* aOut, u32 nBits) {
    u32 i;

    for (i = 0; i < (nBits + 31) >> 5; i++) {
        aOut[i] = aA[i] & aB[i];
    }
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

f32 (*fn_8001EE64(Character* pChar))[4] {
    return fn_8001ED08(pChar, 1);
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

// Byte-swaps an animation library in place and links it: the header, the records after it, each
// record's entries after those, then each entry's data (each start rounded up to 4 bytes).
// *pnSize gets the library's size.
MtaLib* fn_8001F110(MtaLib* pLib, s32* pnSize) {
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

    pSrc = pDst = pLib;
    fn_8001F08C(&pSrc, &pDst, aHeader, 10, 1);
    pRecords = (MtaRecord*)(pLib + 1);
    pSrc = pDst = pRecords;
    fn_8001F08C(&pSrc, &pDst, aRecord, 5, pLib->nRecords);
    pLib->pRecords = pRecords;
    nOffset = sizeof(MtaLib) + pLib->nRecords * sizeof(MtaRecord);
    for (i = 0; i < pLib->nRecords; i++) {
        pRecord = &pLib->pRecords[i];
        pSrc = pDst = (u8*)pLib + nOffset;
        fn_8001F08C(&pSrc, &pDst, aEntry, 11, pRecord->nEntries);
        pRecord->pEntries = (MtaEntry*)((u8*)pLib + nOffset);
        nOffset += pRecord->nEntries * sizeof(MtaEntry);
    }
    for (i = 0; i < pLib->nRecords; i++) {
        pRecord = &pLib->pRecords[i];
        for (j = 0; j < pRecord->nEntries; j++) {
            pEntry = &pRecord->pEntries[j];
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
