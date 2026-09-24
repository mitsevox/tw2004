// CharAnim.c (our name; TW06's char_state.c, its functions CharacterState_*): the golfer's
// animation state - the blend data added for each clip group, the idle and emotion updates, and
// the queued state changes played out on the second animation player (Character.anim29C).

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "charstate.h"
#include "frontend/fe.h"
#include "game/save.h"

f32   fn_8001F02C(Clip* pBlend, u64 uEvent);   // an event's time (by its 64-bit id)

void  fn_800958EC(AnimPlayer* pAnim, s32 n, f32 f);
void  fn_80095FD0(Character* pChar, MtaLib* pLib, u8 bReset, int nGroup, SKABlendFn pfnBlend, int nC,
                  int nAnim, f32 fStart, f32 fFrom, f32 fTo, f32 fOffset, f32 fTime);
void  fn_8009622C(Character* pChar, void* pClip, u8 bKeep, f32 fOffset);
s8    fn_80096338(void);
s32   fn_800962F8(Character* pChar);
s32   fn_80096508(void);
int   fn_80096530(Character* pChar);
f32   fn_800971B8(Character* pChar);
u8    fn_8001EC48(Character* pChar);                        // char.c
void  Character_PlaceFeetOnGround(Character* pChar);        // char.c
int   fn_8001BD18(Character* pChar, Clip* pClip);           // char.c
void  fn_800175B0(Character* pChar, Clip* pBlend, f32 fStart);   // char.c
void  fn_801141F8(struct DynChain* pChain, CharModel* pModel);   // DynChain.c
char* fn_801008A8(void);                                    // GameMode11.c
void  fn_8000A144(f32 (*pSrc)[4], f32 (*pDst)[4]);          // UMemPool.c: copies three rows
void  fn_800089D4(f32 (*m)[4], f32* pQ);                    // Quaternion.c: a rotation matrix's quaternion

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f (0x80283CD8), before the 0.0f fn_800957FC uses first; its body is unknown.
static f32 CharAnim_StrippedFn(f32 x) {
    return x + 1.0f;
}

// Clear the second player's state and its queued state change; with bReset, rebuild its blend
// node as a half-and-half blend. In game type 3 the created golfer's sliders are applied again.
void fn_800957FC(Character* pChar, u8 bReset) {
    SKABlendNode* pNode;

    if (pChar == NULL) return;
    pChar->n30 = 0;
    pChar->n2C = 0;
    pChar->u28 &= ~1;
    fn_800958EC(&pChar->anim29C, 0, 0.0f);
    if (bReset) {
        pNode = &pChar->node3E0;
        fn_80071F58(&pNode, 0);
        fn_80071C28(&pNode, 1, 1, fn_80072ACC, 1);
        fn_800725BC(pNode, fn_80072ACC, 0.5f);
    }
    if (gSession.nGameType == 3 && pChar->p17AC != NULL) {
        fn_8010E4DC(pChar->p17AC, pChar->pModel, pChar->pSkin, 26, fn_80077ACC()->choices.a9B4,
                    &pChar->node3E0);
    }
}

void fn_800958EC(AnimPlayer* pAnim, s32 n, f32 f) {
    pAnim->nC  = n;
    pAnim->f10 = f;
}

// Work out a clip's blend window aBlend: [0] its start and [1] its end in the clip, [2] its event
// 2's time (fFrom -70000 only, else [1]), [3] and [4] the start and end on the player's clock,
// [5] fOffset. fFrom and fTo may be markers: -40000 and -50000 take the clip's pD8 times, -90000
// (fFrom) fn_800971B8, -70000 (fFrom) v1638[1], resyncing the first player; other negatives take
// 0 and the clip's length. fStart -10000 starts the window at the blend tree's end. With aPrev,
// the window lines up with the previous one's. The result is the window's length on the clock.
f32 fn_800958F8(Character* pChar, f32* aPrev, Clip* pClip, f32* aBlend, f32 fFrom, f32 fTo, f32 fStart,
                f32 fOffset) {
    aBlend[2] = -1.0f;
    if (pClip->pD8 != NULL) {
        if (-40000.0f == fFrom) {
            aBlend[0] = pClip->pD8->f0C;
        } else if (-50000.0f == fFrom) {
            aBlend[0] = pClip->pD8->f08;
        } else if (-90000.0f == fFrom) {
            aBlend[0] = fn_800971B8(pChar);
        } else if (-70000.0f == fFrom) {
            aBlend[0] = pChar->v1638[1];
            aBlend[2] = fn_8001F02C(pClip, 2);
            fn_800732F4(&pChar->blend, (AnimPlayer*)pChar->anim, pChar->fAnimTime);
        } else if (fFrom < 0.0f) {
            aBlend[0] = 0.0f;
        }
        if (-40000.0f == fTo) {
            aBlend[1] = pClip->pD8->f0C;
        } else if (-50000.0f == fTo) {
            aBlend[1] = pClip->pD8->f08;
        } else if (fTo < 0.0f) {
            aBlend[1] = pClip->f18;
        }
    } else {
        if (fFrom < 0.0f) {
            aBlend[0] = 0.0f;
        }
        if (fTo < 0.0f) {
            aBlend[1] = pClip->f18;
        }
    }
    if (aBlend[2] < 0.0f) {
        aBlend[2] = aBlend[1];
    }
    if (-10000.0f == fStart) {
        aBlend[3] = fOffset + fn_80072938(&pChar->blend);
    } else {
        aBlend[3] = fStart + fOffset;
    }
    aBlend[5] = fOffset;
    if (aPrev != NULL) {
        f32 fRatio = (aPrev[2] - aPrev[0]) / (aBlend[2] - aBlend[0]);
        aBlend[3] = aPrev[3];
        aBlend[4] = fRatio * (aBlend[1] - aBlend[0]) + aBlend[3];
    } else {
        aBlend[4] = aBlend[3] + (aBlend[1] - aBlend[0]);
    }
    return aBlend[4] - aBlend[3];
}

// Play clip group nGroup on the golfer (nothing at style -1): the clip is the blend clip for
// fFrom -70000, the one kept for groups 5, 6, 10 and 9, or else Char_SetClip's pick (in a lesson,
// group 1 by the lesson's name). The putting clip "gplptt12" first turns the root bone square to
// the ground under the golfer. With bReset the blend tree starts over. The rest is as in
// fn_80095FD0, on the first player; a clip with a pF4 library also starts it on the second.
void CharacterState_AddSKABlendData(Character* pChar, u8 bReset, int nGroup, SKABlendFn pfnBlend, int nC,
                                    int nAnim, f32 fStart, f32 fFrom, f32 fTo, f32 fOffset, f32 fTime) {
    SKABlendNode* pNode;
    SKABlendNode* pNew;
    f32 aBlend[18];             // only [0]-[5] are used; EA's frame has room for 18 (true size unknown)
    f32 m[4][4];
    f32 vNormal[4];
    Clip* pClip;
    char* pName;
    CourseInfo* pCourse;
    f32 fDelay;

    if (pChar->nStyle == -1) return;
    pNode = &pChar->blend;
    pNew = NULL;
    pChar->u10 &= ~0x8000;
    pChar->nGroup = nGroup;
    pChar->uFlags &= 0x818;
    if (-70000.0f == fFrom && pChar->pBlend != NULL) {
        pClip = pChar->pBlend;
    } else {
        if ((nGroup == 5 || nGroup == 10 || nGroup == 6) && pChar->p1790 != NULL) {
            pClip = pChar->p1790;
        } else if (nGroup == 9 && pChar->p1794 != NULL) {
            pClip = pChar->p1794;
        } else {
            pName = NULL;
            if (fn_80100294() && nGroup == 1) {
                pName = fn_801008A8();
            }
            pClip = Char_SetClip(pChar, nGroup, pChar->nStyle, pName);
            if (nGroup == 5 || nGroup == 10 || nGroup == 6) {
                pChar->p1790 = pClip;
            } else if (nGroup == 9) {
                pChar->p1794 = pClip;
            }
        }
        if (strcmp(pClip->name, "gplptt12") == 0 && (pCourse = fn_8000C594()) != NULL &&
            Ter_GetSupportingGroundNormal(pCourse, pChar->pModel->pMatrices[0][3], vNormal)) {
            Vec_Copy(pChar->a179C, m[1]);
            vec4flt_CrossProduct(pChar->pModel->pMatrices[0][0], pChar->a179C, m[2]);
            fn_800BAF04(m[2], m[2]);
            vec4flt_CrossProduct(pChar->a179C, m[2], m[0]);
            m[0][3] = 0.0f;
            m[1][3] = 0.0f;
            m[2][3] = 0.0f;
            m[3][3] = 1.0f;
            fn_8000A144(m, pChar->pModel->pMatrices[0]);
            fn_800089D4(m, pChar->pModel->pBones->q0C);
            pChar->u10 |= 0x8000;
        }
        strcpy(pChar->sz1614, pClip->name);
        EVENT_Trigger(pChar->nPlayer, 0x48, NULL, nGroup);
        if (pClip->pD8 != NULL) {
            pChar->pBlend = pClip;
        } else {
            pChar->pBlend = NULL;
        }
    }
    if (fn_8001EC48(pChar)) {
        fn_8001BD18(pChar, pClip);
    }
    if (bReset) {
        pChar->fAnimTime = 0.0f;
        fn_80071F58(&pNode, 0);
        fn_80071C28(&pNode, 1, pNode->nFormat, pfnBlend, nC);
        fn_801141F8(pChar->pModel->pF0, pChar->pModel);
        fn_801141F8(pChar->pModel->pF4, pChar->pModel);
        fn_801141F8(pChar->pModel->pF8, pChar->pModel);
    }
    if (-20000.0f == fStart) {
        fStart = pChar->fAnimTime;
    }
    fn_800958F8(pChar, NULL, pClip, aBlend, fFrom, fTo, fStart, fOffset);
    fDelay = fOffset + (pChar->fAnimEnd - pChar->fAnimTime);
    fn_800175B0(pChar, pClip, aBlend[3] - aBlend[0]);
    pNew = NULL;
    fn_80071C28(&pNew, 0, pNode->nFormat, pfnBlend, nC);
    fn_800724C0(&pChar->blend, pNew, pClip, 1.0f);
    fn_800720C8(pChar, pNew, &pNode, aBlend, pfnBlend, nC);
    ((AnimPlayer*)pChar->anim)->n00 = 0;
    pChar->n16C = 1;
    pChar->f180 = pNode->fStart;
    pChar->fAnimEnd = pNode->fEnd;
    if (-20000.0f == fTime) {
        fTime = pChar->fAnimTime;
    } else if (-10000.0f == fTime) {
        fTime = pNode->fEnd;
    } else if (-30000.0f == fTime) {
        fTime = pNode->fStart;
    }
    if (fOffset < 0.0f) {
        fTime += fOffset;
    }
    fn_800958EC((AnimPlayer*)pChar->anim, nAnim, fTime);
    pChar->p178C = NULL;
    if (pClip != NULL && pClip->pF4 != NULL) {
        fn_8009622C(pChar, pClip->pF4, bReset, fDelay);
    }
}

// Play pLib (a MAL bank's library) on the second player from fFrom to fTo, starting at fStart on
// the player's clock; with bReset its blend node is rebuilt first. fStart -20000 means the
// player's time; negative fFrom and fTo mean 0 and the library's end. fTime (-20000 the player's
// time, -10000 the node's end, -30000 its start) plus a negative fOffset is when the queued
// state nAnim runs (fn_800958EC). In game type 3 the created golfer's sliders are applied again.
// nGroup is not used; every caller passes it (fn_80096F0C: the MAL group pLib came from).
void fn_80095FD0(Character* pChar, MtaLib* pLib, u8 bReset, int nGroup, SKABlendFn pfnBlend, int nC,
                 int nAnim, f32 fStart, f32 fFrom, f32 fTo, f32 fOffset, f32 fTime) {
    SKABlendNode* pNode = &pChar->node3E0;
    SKABlendNode* pNew = NULL;
    f32 aBlend[6];

    if (pLib == NULL) return;
    pChar->anim29C.uFlags = 0;
    if (bReset) {
        pChar->anim29C.fTime = 0.0f;
        fn_80071F58(&pNode, 0);
        fn_80071C28(&pNode, 1, pNode->nFormat, pfnBlend, nC);
    }
    if (-20000.0f == fStart) {
        fStart = pChar->anim29C.fTime;
    }
    if (fFrom < 0.0f) {
        fFrom = 0.0f;
    }
    if (fTo < 0.0f) {
        fTo = pLib->f1C;
    }
    pNew = NULL;
    fn_80071C28(&pNew, 0, pNode->nFormat, pfnBlend, nC);
    fn_800724C0(&pChar->node3E0, pNew, pLib, 1.0f);
    aBlend[3] = fStart;
    aBlend[5] = fOffset;
    aBlend[0] = fFrom;
    aBlend[1] = fTo;
    aBlend[4] = fStart + (fTo - fFrom);
    fn_800720C8(pChar, pNew, &pNode, aBlend, pfnBlend, nC);
    pChar->anim29C.n00 = 0;
    pChar->anim29C.n08 = 1;
    pChar->anim29C.fStart = pNode->fStart;
    pChar->anim29C.fEnd = pNode->fEnd;
    if (-20000.0f == fTime) {
        fTime = pChar->anim29C.fTime;
    } else if (-10000.0f == fTime) {
        fTime = pNode->fEnd;
    } else if (-30000.0f == fTime) {
        fTime = pNode->fStart;
    }
    if (fOffset < 0.0f) {
        fTime += fOffset;
    }
    fn_800958EC(&pChar->anim29C, nAnim, fTime);
    pChar->p178C = pLib;
    if (gSession.nGameType == 3 && pChar->p17AC != NULL) {
        fn_8010E4DC(pChar->p17AC, pChar->pModel, pChar->pSkin, 26, fn_80077ACC()->choices.a9B4,
                    &pChar->node3E0);
    }
}

// Start pClip on the second player (fn_80095FD0) and queue state 4; unless bKeep, its blend node
// first moves to fOffset past the player's time.
void fn_8009622C(Character* pChar, void* pClip, u8 bKeep, f32 fOffset) {
    if (pClip == NULL) return;
    if (!bKeep) {
        fn_800732F4(&pChar->node3E0, &pChar->anim29C, pChar->anim29C.fTime + fOffset);
    }
    fn_80095FD0(pChar, pClip, bKeep, 0, fn_80072ACC, 1, 5, -20000.0f, -30000.0f, -10000.0f, 0.0f, -10000.0f);
    pChar->n2C = 4;
    pChar->n30 = 4;
    pChar->u28 |= 1;
    if (pChar->n20 == 5) {
        pChar->anim29C.f10 -= 0.95f;
    }
}

// Start the idle wait over: a new count (fn_80096338) and the other two counters cleared.
s32 fn_800962F8(Character* pChar) {
    pChar->n24 = fn_80096338();
    pChar->n25 = 0;
    pChar->n26 = 0;
    return 2;
}

// A random wait of 8 to 10.
s8 fn_80096338(void) {
    return Rand_Next(1) % 3 + 8;
}

u8 fn_8009637C(Character* pChar) {
    return pChar->n26 != 1;
}

// The idle update, in game type 6 with n20 at 5: count n24 down, then n25; when n25 runs out, play
// an animation group 4 clip (state 4, n26 set) if the library has one that is not flagged, else
// start the wait over. The result is the state to go to.
s32 fn_80096398(Character* pChar) {
    s32 nState;
    u32 uFlags;
    s32 nCount;
    int nClub;

    nState = 2;
    if (gSession.nGameType != 6) return 2;
    switch (pChar->n20) {
    case 5:
        if (pChar->n24 > 0) {
            if (--pChar->n24 <= 0) {
                pChar->n24 = 0;
                pChar->n25 = fn_80096508();
                nState = 3;
            } else {
                nState = 2;
            }
        } else if (pChar->n25 > 0) {
            if (--pChar->n25 <= 0) {
                uFlags = 0;
                nCount = 0;
                pChar->n25 = 0;
                if (pChar->pLib->groups[4] >= 0) {
                    nClub = pChar->nClubClass;
                    if (nClub == 1) {
                        nClub = 0;
                    }
                    AnimLib_Find(pChar->pLib, 4, pChar->nStyle, nClub, pChar->n16D4, &nCount, &uFlags,
                                 NULL, NULL);
                    if (!(uFlags & 1)) {
                        pChar->n26 = 1;
                        nState = 4;
                        break;
                    }
                }
                pChar->n26 = 0;
                pChar->n24 = fn_80096338();
                nState = 2;
            } else {
                nState = 3;
            }
        } else if (pChar->n26 > 0) {
            pChar->n26 = 0;
            pChar->n24 = fn_80096338();
            nState = 2;
        }
        break;
    default:
        nState = fn_800962F8(pChar);
    }
    return nState;
}

s32 fn_80096508(void) {
    Rand_Next(1);
    return 1;
}

// TW06: CharacterState_UpdateGameEmotionState. The animation style from how the shot turned out.
int fn_80096530(Character* pChar) {
    int aStyle[10] = {5, 6, 7, 2, 1, 0, 3, 4, 5, 2};
    int nResult    = fn_8006AA9C(pChar->nPlayer);
    fn_8001C7FC(pChar, aStyle[nResult]);
    return nResult;
}

// TW06: CharacterState_SetTapInState. Animation 11, the gimme tap-in: the style is the score the
// tap-in will give (under par 6, par 5, over par 2; the style-0 branch repeats the par test and can
// never be taken), then clip group 9 plays. Which clip that is comes from the golfer's animation
// library (AnimLib_Pick): the two standard tap-ins, plus the pool-cue tap-in for a few golfers.
void CharacterState_SetTapInState(Character* pChar) {
    int nScore;
    if (pChar == NULL) return;
    nScore = Hole_ScoreAfterTapIn(pChar->nPlayer);
    if (nScore < 0) {
        fn_8001C7FC(pChar, 6);
    } else if (nScore == 0) {
        fn_8001C7FC(pChar, 5);
    } else if (nScore == 0) {
        fn_8001C7FC(pChar, 0);
    } else {
        fn_8001C7FC(pChar, 2);
    }
    CharacterState_AddSKABlendData(pChar, 1, 9, fn_80072ACC, 1, 8, -10000.0f, -30000.0f, -10000.0f, 0.0f,
                                   -10000.0f);
}

// The first player's state change: once its time reaches f10, the state nC queued becomes nAnim
// and its clip group is added. Afterwards the skeleton's IK is blended in or out.
void CharacterState_UpdateSKAState(Character* pChar) {
    f32 fIK = 0.25f;
    u8 bNoIK = 0;
    u8 bTransition = 0;
    u8 bReset;
    int nSaved;
    u8 bOther;
    int nGroup;
    int nResult;
    int i;
    f32 fOffset;
    f32 fLag;

    if (pChar->n170 != 0 && pChar->fAnimTime >= pChar->f174) {
        pChar->nAnim = pChar->n170;
        pChar->n18 |= 1;
    }
    if (!(pChar->n18 & 1)) return;
    switch (pChar->nAnim) {
    case 1:
        nGroup = 1;
        if (gPlayers[pChar->nPlayer].ball.nLie == 0 && gSession.options.a24[6] == 0 &&
            gSession.options.nWind >= 1 && pChar->nSlot == 0) {
            nGroup = 16;
        } else if (gPlayers[pChar->nPlayer].ball.nLie == 0 && gPlayers[pChar->nPlayer].nClub >= 0 &&
                   gPlayers[pChar->nPlayer].nClub <= 5 && Rand_Next(1) % 100 < 10 && pChar->nSlot == 0) {
            nGroup = 16;
        }
        CharacterState_AddSKABlendData(pChar, 1, nGroup, fn_80072ACC, 1, 2, -10000.0f, -30000.0f, -10000.0f,
                                       0.0f, -10000.0f);
        break;
    case 10:
        CharacterState_AddSKABlendData(pChar, 1, 14, fn_80072ACC, 1, 0, -10000.0f, -30000.0f, -10000.0f, 0.0f,
                                       -10000.0f);
        Character_PlaceFeetOnGround(pChar);
        break;
    case 2:
        switch (pChar->n20) {
        case 4:
        case 1:
            Anim_SetTime(pChar->anim, -10000.0f);
            fn_800958EC((AnimPlayer*)pChar->anim, 0, -10000.0f);
            break;
        case 3:
            Anim_SetTime(pChar->anim, -30000.0f);
            fn_800958EC((AnimPlayer*)pChar->anim, 0, -10000.0f);
            break;
        default:
            CharacterState_AddSKABlendData(pChar, 1, 1, fn_80072ACC, 1, 0, -10000.0f, -30000.0f, -10000.0f,
                                           0.0f, -10000.0f);
            Anim_SetTime(pChar->anim, -10000.0f);
            break;
        }
        break;
    case 3:
        bReset = 0;
        if (pChar->n20 != 2) {
            bReset = 1;
        }
        CharacterState_AddSKABlendData(pChar, bReset, 8, fn_80072ACC, 1, 5, -10000.0f, -30000.0f, -10000.0f,
                                       0.0f, -10000.0f);
        break;
    case 4:
        bReset = 0;
        if (pChar->n20 != 5) {
            bReset = 1;
        }
        CharacterState_AddSKABlendData(pChar, bReset, 7, fn_80072ACC, 1, 2, -20000.0f, -30000.0f, -10000.0f,
                                       0.0f, -10000.0f);
        break;
    case 5:
        bReset = 0;
        fOffset = 0.0f;
        if (pChar->u10 & 0x80) {
            pChar->u10 &= ~0x80;
            switch (pChar->n20) {
            case 6:
                fn_800732F4(&pChar->blend, (AnimPlayer*)pChar->anim, pChar->fAnimTime);
                fOffset = 0.35f;
                break;
            case 5:
                fn_800732F4(&pChar->blend, (AnimPlayer*)pChar->anim, 0.25f + pChar->fAnimTime);
                fOffset = 0.0f;
                break;
            default:
                bReset = 1;
                break;
            }
            nGroup = fn_800962F8(pChar);
        } else {
            switch (pChar->n20) {
            case 6:
                fn_800732F4(&pChar->blend, (AnimPlayer*)pChar->anim, pChar->fAnimTime);
                fOffset = 0.35f;
                break;
            case 1:
            case 3:
            case 5:
                break;
            case 2:
            case 4:
            default:
                bReset = 1;
                break;
            }
            nGroup = fn_80096398(pChar);
        }
        CharacterState_AddSKABlendData(pChar, bReset, nGroup, fn_80072ACC, 1, 5, -20000.0f, -30000.0f,
                                       -10000.0f, fOffset, -10000.0f);
        bNoIK = fn_8009637C(pChar);
        bTransition = 1;
        pChar->f174 -= 1.0f;
        break;
    case 6:
        bReset = 0;
        switch (pChar->n20) {
        case 5:
            fn_800732F4(&pChar->blend, (AnimPlayer*)pChar->anim, 0.25f + pChar->fAnimTime);
            break;
        default:
            bReset = 1;
            break;
        }
        CharacterState_AddSKABlendData(pChar, bReset, 0, fn_80072ACC, 1, 0, -20000.0f, -50000.0f, -40000.0f,
                                       0.0f, -10000.0f);
        bNoIK = 1;
        bTransition = 1;
        fn_800958EC(&pChar->anim29C, 0, 0.0f);
        if (gSession.nGameType == 6) {
            pChar->n30 = 0;
            pChar->n2C = 0;
            fn_800957B0(pChar, 1);
        }
        break;
    case 7:
        bReset = 0;
        switch (pChar->n20) {
        case 6:
            fLag = pChar->f1644;
            break;
        default:
            bReset = 1;
            fLag = 0.0f;
            break;
        }
        pChar->uFlags &= ~0x40;
        fn_800732F4(&pChar->blend, (AnimPlayer*)pChar->anim, pChar->fAnimTime);
        CharacterState_AddSKABlendData(pChar, bReset, 0, fn_80072ACC, 1, 8, -20000.0f, -70000.0f, -10000.0f,
                                       fLag, -10000.0f);
        bNoIK = 1;
        break;
    case 14:
        nSaved = pChar->n16D4;
        if (gPlayers[pChar->nPlayer].ball.nLie == 0) {
            pChar->n16D4 = 6;
        } else {
            pChar->n16D4 = 0;
        }
        // EA's code resets the blend tree either way
        switch (pChar->n20) {
        case 7:
            bReset = 1;
            break;
        default:
            bReset = 1;
            break;
        }
        CharacterState_AddSKABlendData(pChar, bReset, 11, fn_80072ACC, 1, 8, -20000.0f, -30000.0f, -10000.0f,
                                       0.0f, -10000.0f);
        pChar->n16D4 = nSaved;
        bNoIK = 0;
        break;
    case 8:
        pChar->uFlags &= ~1;
        pChar->uFlags |= 0x40;
        pChar->f198 = 0.0f;
        pChar->f19C = pChar->fAnimTime;
        fn_800958EC((AnimPlayer*)pChar->anim, 0, -10000.0f);
        if (pChar->nClubClass == 2) {
            bTransition = 1;
            fIK = 0.5f;
        }
        for (i = 0; i < 5; i++) {
            if (gPlayers[i].pChar == pChar) {
                EVENT_Trigger(i, 0xB, NULL, 0);
                break;
            }
        }
        break;
    case 9:
        bOther = 0;
        nResult = fn_80096530(pChar);
        switch (nResult) {
        case 8:
        case 9:
            nGroup = 10;
            break;
        default:
            nGroup = 5;
            break;
        }
        switch (pChar->n20) {
        case 7:
        case 8:
        case 14:
            break;
        default:
            bOther = 1;
            break;
        }
        pChar->uFlags &= ~0x40;
        if (bOther || pChar->nClubClass != 2) {
            CharacterState_AddSKABlendData(pChar, 1, 5, fn_80072ACC, 1, 0, -20000.0f, -30000.0f, -10000.0f,
                                           0.0f, -10000.0f);
        } else {
            fn_800732F4(&pChar->blend, (AnimPlayer*)pChar->anim, pChar->fAnimTime);
            CharacterState_AddSKABlendData(pChar, bOther, nGroup, fn_80072ACC, 1, 0, -20000.0f, -30000.0f,
                                           -10000.0f, 0.5f, -10000.0f);
            bTransition = 1;
            fIK = 0.1f;
            pChar->fAnimTime += 0.0001f;
        }
        break;
    case 11:
        CharacterState_SetTapInState(pChar);
        bTransition = 1;
        break;
    case 12:
        fn_80096530(pChar);
        CharacterState_AddSKABlendData(pChar, 1, 6, fn_80072ACC, 1, 0, -10000.0f, -30000.0f, -10000.0f, 0.0f,
                                       -10000.0f);
        break;
    }
    if (bTransition) {
        SKEL_TransitionIK(pChar->pModel->pSkel, !bNoIK, fIK);
    } else if (bNoIK) {
        SKEL_SetIKSolutionWeight(pChar->pModel->pSkel, 1.0f);
    } else {
        SKEL_SetIKSolutionWeight(pChar->pModel->pSkel, 0.0f);
    }
    pChar->n20 = pChar->nAnim;
    pChar->n18 &= ~1;
    if (pChar->nGroup != 4) {
        pChar->n26 = 0;
        pChar->u10 &= ~0x100;
    }
}

// The second player's state change: once its time reaches f10, the state nC queued runs. State 5
// clears the queue (in game type 6 it signals event 0x2A and stays waiting); states 1 to 3 play a
// clip from MAL group 0 to 2 on it when n30 is not 4 and n20 is 6 or 7.
void fn_80096F0C(Character* pChar) {
    MtaLib* pLib;

    if (pChar->anim29C.nC != 0 && pChar->anim29C.fTime >= pChar->anim29C.f10) {
        pChar->n2C = pChar->anim29C.nC;
        pChar->u28 |= 1;
    }
    if (!(pChar->u28 & 1)) return;
    switch (pChar->n2C) {
    case 5:
        fn_800958EC(&pChar->anim29C, 0, 0.0f);
        if (gSession.nGameType == 6) {
            EVENT_Trigger(pChar->nPlayer, 0x2A, NULL, -1);
            pChar->n30 = 5;
            return;
        }
        break;
    case 2:
        if (pChar->n30 == 4 || (pChar->n20 != 6 && pChar->n20 != 7)) {
            goto skip;  // fake match: past the n30 update to the shared clear (a copy here: 90.3%)
        }
        pLib = fn_80017678(pChar, 1, -1);
        fn_800732F4(&pChar->node3E0, &pChar->anim29C, 0.5f + pChar->anim29C.fTime);
        fn_80095FD0(pChar, pLib, 0, 1, fn_80072ACC, 1, 2, -20000.0f, -30000.0f, -10000.0f, 0.0f, -10000.0f);
        pChar->anim29C.f10 -= 0.5f;
        break;
    case 3:
        if (pChar->n30 == 4 || (pChar->n20 != 6 && pChar->n20 != 7)) {
            goto skip;  // fake match: as in case 2
        }
        fn_800732F4(&pChar->node3E0, &pChar->anim29C, 0.5f + pChar->anim29C.fTime);
        pLib = fn_80017678(pChar, 2, -1);
        fn_80095FD0(pChar, pLib, 0, 2, fn_80072ACC, 1, 3, -20000.0f, -30000.0f, -10000.0f, 0.0f, -10000.0f);
        pChar->anim29C.f10 -= 0.5f;
        break;
    case 1:
        if (pChar->n30 == 4 || (pChar->n20 != 6 && pChar->n20 != 7)) {
            goto skip;  // fake match: as in case 2
        }
        fn_800732F4(&pChar->node3E0, &pChar->anim29C, 0.5f + pChar->anim29C.fTime);
        pLib = fn_80017678(pChar, 0, -1);
        fn_80095FD0(pChar, pLib, 0, 0, fn_80072ACC, 1, 1, -20000.0f, -30000.0f, -10000.0f, 0.0f, -10000.0f);
        pChar->anim29C.f10 -= 0.5f;
        break;
    }
    pChar->n30 = pChar->n2C;
skip:
    pChar->u28 &= ~1;
}

// Halfway between the blend's second clip's f0C and the time of the blend's event 2.
f32 fn_800971B8(Character* pChar) {
    f32 fTime  = fn_8001F02C(pChar->pBlend, 2);
    f32 fStart = pChar->pBlend->pD8->f0C;
    fTime = (fTime - fStart) / 2.0f + fStart;
    return fTime;
}
