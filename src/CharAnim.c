// CharAnim.c (our name; EA's char_State.c in TW06 and TW07, golf/animation; its functions are
// CharacterState_*): the golfer's animation states. Each body state (Character.nAnim) plays a
// clip group on the first animation player (CharacterState_UpdateSKAState,
// CharacterState_AddSKABlendData: the swing, the idle and its fidgets, the reactions, the tap-in);
// the second player (Character.anim29C) plays morph libraries (MtaLib, each track a morph weight)
// the same way, from a clip's own library or at random from a 'MAL ' bank while the golfer swings.

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "charstate.h"
#include "frontend/fe.h"
#include "game/save.h"

f32   SKA_GetTagTime(Clip* pBlend, u64 uEvent);   // an event's time (by its 64-bit id)

void  CharacterState_SetTransition(AnimPlayer* pTime, s32 nState, f32 fTime);
void  CharacterState_AddMorphBlendData(Character* pChar, MtaLib* pLib, u8 bReset, int nGroup,
                                       SKABlendFn pfnBlend, int nC, int nTransitionState, f32 fStart,
                                       f32 fFrom, f32 fTo, f32 fOffset, f32 fTransitionTime);
void  CharacterState_PlayClipMorphs(Character* pChar, void* pLib, u8 bReset, f32 fOffset);
s8    GET_AMBIENT_FIDGET_COUNT(void);
s32   CharacterState_ResetFidgetState(Character* pChar);
s32   GET_IDLE_FIDGET_COUNT(void);
int   CharacterState_UpdateGameEmotionState(Character* pChar);
f32   CharacterState_GetLastSKAStateID(Character* pChar);
u8    Character_IsGolfer(Character* pChar);                        // char.c
void  Character_PlaceFeetOnGround(Character* pChar);        // char.c
int   Character_UpdateClubAttachment(Character* pChar, Clip* pClip);           // char.c
void  Character_InitSKATags(Character* pChar, Clip* pBlend, f32 fStart);   // char.c
void  fn_801141F8(struct DynChain* pChain, CharModel* pModel);   // DynChain.c
char* fn_801008A8(void);                                    // GameMode11.c
void  LLMath_CopyMat34(f32 (*pSrc)[4], f32 (*pDst)[4]);          // UMemPool.c: copies three rows
void  Quat_BuildFromMatrix(f32 (*m)[4], f32* pQ);                    // Quaternion.c: a rotation matrix's quaternion

// .bss (character.h). Section note: owner by link order only. Nothing here uses it (only skalib.c's
// SKALIB_InitModule clears [0]); it lies between LLTime.c's .bss and GoShaderObject_Glows_Gc.c's, and of
// the units between them this is the one from skalib.c's source directory (TW06/TW07
// golf/animation) and the one with a stripped function (below).
u8 lbl_801D9908[0xC8];

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f (0x80283CD8), before the 0.0f CharacterState_ResetMorphState uses first; its body is unknown.
static f32 CharAnim_StrippedFn(f32 x) {
    return x + 1.0f;
}

// Stop the morph animations (the second player, anim29C, and its tree node3E0): its current and
// target states, its waiting change and its queued transition are cleared. With bReset the tree is
// given back and set up again as an empty blend node. In the menus (game type 3) the created
// golfer's slider values are applied to the tree again.
void CharacterState_ResetMorphState(Character* pChar, u8 bReset) {
    SKABlendNode* pNode;

    if (pChar == NULL) return;
    pChar->n30 = 0;
    pChar->n2C = 0;
    pChar->u28 &= ~1;
    CharacterState_SetTransition(&pChar->anim29C, 0, 0.0f);
    if (bReset) {
        pNode = &pChar->node3E0;
        SKABlendData_Shutdown(&pNode, 0);
        SKABlendData_Init(&pNode, 1, 1, SKABlender_BlendLinear, 1);
        SKABlender_SetBlender(pNode, SKABlender_BlendLinear, 0.5f);
    }
    if (gSession.nGameType == 3 && pChar->pSliderDefs != NULL) {
        CharSlider_UpdateCharacterBasedOnSliderValues(pChar->pSliderDefs, pChar->pModel, pChar->pSkin, 26,
                                                      FE_GetCurrentProfile()->choices.a9B4,
                    &pChar->node3E0);
    }
}

// Queue the player's transition: state nState starts once the player's time reaches fTime ((0,
// 0.0): none). CharacterState_UpdateSKAState (the body) and CharacterState_UpdateMorphState (the
// morph player) make the change.
void CharacterState_SetTransition(AnimPlayer* pTime, s32 nState, f32 fTime) {
    pTime->nC  = nState;
    pTime->f10 = fTime;
}

// Work out a clip's blend window aBlend (EA's TSKABlendInfo): [0] and [1] its start and end in the
// clip, [2] its event 2 (ball hit) time (fFrom -70000 only, else [1]), [3] and [4] the start and
// end on the player's clock, [5] fOffset. fFrom and fTo may be markers: -40000 and -50000 take the
// clip's pD8 last and first key times, -90000 (fFrom) CharacterState_GetLastSKAStateID, -70000
// (fFrom) v1638[1], first cutting the body's tree off at its time (SKABlender_ClampT1); other negatives
// take 0 and the clip's length (f18). fStart -10000 starts the window at the tree's end. With
// aPrev, the window starts with the previous one and is scaled so its span up to event 2 matches
// the previous one's. The result is the window's length on the clock.
f32 SKABlend_CalculateBlendInfo(Character* pChar, f32* aPrev, Clip* pClip, f32* aBlend, f32 fFrom, f32 fTo,
                                f32 fStart, f32 fOffset) {
    aBlend[2] = -1.0f;
    if (pClip->pD8 != NULL) {
        if (-40000.0f == fFrom) {
            aBlend[0] = pClip->pD8->f0C;
        } else if (-50000.0f == fFrom) {
            aBlend[0] = pClip->pD8->f08;
        } else if (-90000.0f == fFrom) {
            aBlend[0] = CharacterState_GetLastSKAStateID(pChar);
        } else if (-70000.0f == fFrom) {
            aBlend[0] = pChar->v1638[1];
            aBlend[2] = SKA_GetTagTime(pClip, 2);
            SKABlender_ClampT1(&pChar->blend, (AnimPlayer*)pChar->anim, pChar->fAnimTime);
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
        aBlend[3] = fOffset + SKABlender_GetEndTime(&pChar->blend);
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

// Play clip group nGroup (EA's eIGSkaStates) on the golfer's body; nothing at style -1. The clip is
// pBlend again for fFrom -70000, the one kept for groups 5, 6, 10 and 9, or else Char_SetClip's
// pick (in a lesson, group 1 by the lesson's name). The putting clip "gplptt12" first turns the
// root bone square to the ground under the golfer. With bReset the blend tree starts over. The
// window comes from SKABlend_CalculateBlendInfo (fStart -20000: the player's time now). The
// transition to nTransitionState comes at fTransitionTime (-20000 the player's time, -10000 the
// tree's end, -30000 its start) plus a negative fOffset. A clip with its own morph library (pF4)
// starts that too (CharacterState_PlayClipMorphs).
void CharacterState_AddSKABlendData(Character* pChar, u8 bReset, int nGroup, SKABlendFn pfnBlend, int nC,
                                    int nTransitionState, f32 fStart, f32 fFrom, f32 fTo, f32 fOffset,
                                    f32 fTransitionTime) {
    SKABlendNode* pNode;
    SKABlendNode* pNew;
    f32 aBlend[18];            // only [0]-[5] are used; EA's frame has room for 18 (true size unknown)
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
        if (strcmp(pClip->name, "gplptt12") == 0 && (pCourse = Ter_GetTGD()) != NULL &&
            Ter_GetSupportingGroundNormal(pCourse, pChar->pModel->pMatrices[0][3], vNormal)) {
            LLMath_CopyVec(pChar->vAvgGroundNormal, m[1]);
            vec4flt_CrossProduct(pChar->pModel->pMatrices[0][0], pChar->vAvgGroundNormal, m[2]);
            LLMath_Normalize3(m[2], m[2]);
            vec4flt_CrossProduct(pChar->vAvgGroundNormal, m[2], m[0]);
            m[0][3] = 0.0f;
            m[1][3] = 0.0f;
            m[2][3] = 0.0f;
            m[3][3] = 1.0f;
            LLMath_CopyMat34(m, pChar->pModel->pMatrices[0]);
            Quat_BuildFromMatrix(m, pChar->pModel->pBones->q0C);
            pChar->u10 |= 0x8000;
        }
        strcpy(pChar->szLastClip, pClip->name);
        EVENT_Trigger(pChar->nPlayer, 0x48, NULL, nGroup);
        if (pClip->pD8 != NULL) {
            pChar->pBlend = pClip;
        } else {
            pChar->pBlend = NULL;
        }
    }
    if (Character_IsGolfer(pChar)) {
        Character_UpdateClubAttachment(pChar, pClip);
    }
    if (bReset) {
        pChar->fAnimTime = 0.0f;
        SKABlendData_Shutdown(&pNode, 0);
        SKABlendData_Init(&pNode, 1, pNode->nFormat, pfnBlend, nC);
        fn_801141F8(pChar->pModel->pF0, pChar->pModel);
        fn_801141F8(pChar->pModel->pF4, pChar->pModel);
        fn_801141F8(pChar->pModel->pF8, pChar->pModel);
    }
    if (-20000.0f == fStart) {
        fStart = pChar->fAnimTime;
    }
    SKABlend_CalculateBlendInfo(pChar, NULL, pClip, aBlend, fFrom, fTo, fStart, fOffset);
    fDelay = fOffset + (pChar->fAnimEnd - pChar->fAnimTime);
    Character_InitSKATags(pChar, pClip, aBlend[3] - aBlend[0]);
    pNew = NULL;
    SKABlendData_Init(&pNew, 0, pNode->nFormat, pfnBlend, nC);
    SKAChannel_SetChannel(&pChar->blend, pNew, pClip, 1.0f);
    SKABlender_AddBlenderData(pChar, pNew, &pNode, aBlend, pfnBlend, nC);
    ((AnimPlayer*)pChar->anim)->n00 = 0;
    pChar->n16C = 1;
    pChar->f180 = pNode->fStart;
    pChar->fAnimEnd = pNode->fEnd;
    if (-20000.0f == fTransitionTime) {
        fTransitionTime = pChar->fAnimTime;
    } else if (-10000.0f == fTransitionTime) {
        fTransitionTime = pNode->fEnd;
    } else if (-30000.0f == fTransitionTime) {
        fTransitionTime = pNode->fStart;
    }
    if (fOffset < 0.0f) {
        fTransitionTime += fOffset;
    }
    CharacterState_SetTransition((AnimPlayer*)pChar->anim, nTransitionState, fTransitionTime);
    pChar->p178C = NULL;
    if (pClip != NULL && pClip->pF4 != NULL) {
        CharacterState_PlayClipMorphs(pChar, pClip->pF4, bReset, fDelay);
    }
}

// Play pLib (a morph library: an item of a 'MAL ' bank, or a clip's pF4) on the morph player
// (anim29C, tree node3E0) from fFrom to fTo, starting at fStart on the player's clock (-20000: its
// time now); negative fFrom and fTo mean 0 and the library's end. With bReset the tree starts over.
// The transition to nTransitionState comes at fTransitionTime (-20000 the player's time, -10000 the
// tree's end, -30000 its start) plus a negative fOffset. In the menus (game type 3) the created
// golfer's slider values are applied again. nGroup is not used; every caller passes it
// (CharacterState_UpdateMorphState: the MAL group pLib came from).
void CharacterState_AddMorphBlendData(Character* pChar, MtaLib* pLib, u8 bReset, int nGroup,
                                      SKABlendFn pfnBlend, int nC, int nTransitionState, f32 fStart,
                                      f32 fFrom, f32 fTo, f32 fOffset, f32 fTransitionTime) {
    SKABlendNode* pNode = &pChar->node3E0;
    SKABlendNode* pNew = NULL;
    f32 aBlend[6];

    if (pLib == NULL) return;
    pChar->anim29C.uFlags = 0;
    if (bReset) {
        pChar->anim29C.fTime = 0.0f;
        SKABlendData_Shutdown(&pNode, 0);
        SKABlendData_Init(&pNode, 1, pNode->nFormat, pfnBlend, nC);
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
    SKABlendData_Init(&pNew, 0, pNode->nFormat, pfnBlend, nC);
    SKAChannel_SetChannel(&pChar->node3E0, pNew, pLib, 1.0f);
    aBlend[3] = fStart;
    aBlend[5] = fOffset;
    aBlend[0] = fFrom;
    aBlend[1] = fTo;
    aBlend[4] = fStart + (fTo - fFrom);
    SKABlender_AddBlenderData(pChar, pNew, &pNode, aBlend, pfnBlend, nC);
    pChar->anim29C.n00 = 0;
    pChar->anim29C.n08 = 1;
    pChar->anim29C.fStart = pNode->fStart;
    pChar->anim29C.fEnd = pNode->fEnd;
    if (-20000.0f == fTransitionTime) {
        fTransitionTime = pChar->anim29C.fTime;
    } else if (-10000.0f == fTransitionTime) {
        fTransitionTime = pNode->fEnd;
    } else if (-30000.0f == fTransitionTime) {
        fTransitionTime = pNode->fStart;
    }
    if (fOffset < 0.0f) {
        fTransitionTime += fOffset;
    }
    CharacterState_SetTransition(&pChar->anim29C, nTransitionState, fTransitionTime);
    pChar->p178C = pLib;
    if (gSession.nGameType == 3 && pChar->pSliderDefs != NULL) {
        CharSlider_UpdateCharacterBasedOnSliderValues(pChar->pSliderDefs, pChar->pModel, pChar->pSkin, 26,
                                                      FE_GetCurrentProfile()->choices.a9B4,
                    &pChar->node3E0);
    }
}

// Play a body clip's own morph library pLib (Clip.pF4) on the morph player, to its end, in morph
// state 4; state 5 follows at the end (0.95 earlier while the body is in state 5). Without bReset
// the morph tree is first cut off at fOffset past the player's time (SKABlender_ClampT1) so the library
// blends in; with it the tree starts over.
void CharacterState_PlayClipMorphs(Character* pChar, void* pLib, u8 bReset, f32 fOffset) {
    if (pLib == NULL) return;
    if (!bReset) {
        SKABlender_ClampT1(&pChar->node3E0, &pChar->anim29C, pChar->anim29C.fTime + fOffset);
    }
    CharacterState_AddMorphBlendData(pChar, pLib, bReset, 0, SKABlender_BlendLinear, 1, 5, -20000.0f,
                                     -30000.0f,
                                     -10000.0f, 0.0f, -10000.0f);
    pChar->n2C = 4;
    pChar->n30 = 4;
    pChar->u28 |= 1;
    if (pChar->n20 == 5) {
        pChar->anim29C.f10 -= 0.95f;
    }
}

// Start the idle's fidget count over: GET_AMBIENT_FIDGET_COUNT ambient idles before the idle clip,
// and no idle or fidget pending. Returns clip group 2, the ambient idle.
s32 CharacterState_ResetFidgetState(Character* pChar) {
    pChar->n24 = GET_AMBIENT_FIDGET_COUNT();
    pChar->n25 = 0;
    pChar->n26 = 0;
    return 2;
}

// A random count of 8 to 10: how many times the idle picks the ambient idle (group 2) before the
// idle clip (group 3).
s8 GET_AMBIENT_FIDGET_COUNT(void) {
    return Misc_RandFunc(1) % 3 + 8;
}

// Whether the golfer is not playing a fidget (group 4, n26 set): the idle (state 5) keeps its IK on
// only then (CharacterState_UpdateSKAState, Character_SetupForShot).
u8 CharacterState_IsNotFidgeting(Character* pChar) {
    return pChar->n26 != 1;
}

// The idle's clip group (EA's fidget cycle), picked each time state 5 starts in a round (game type
// 6) with the body already in state 5: the ambient idle (group 2) until the ambient count n24 runs
// out, then the idle (group 3) for the idle count n25 (GET_IDLE_FIDGET_COUNT), then a fidget (group
// 4, n26 set) if the library has one for this style, club class and clip key that is not flagged 1;
// after the fidget, or without one, the count starts over. From another state the count starts over
// (CharacterState_ResetFidgetState); outside a round it is always group 2.
s32 CharacterState_UpdateFidgetState(Character* pChar) {
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
                pChar->n25 = GET_IDLE_FIDGET_COUNT();
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
                    AnimLib_Find(pChar->pLib, 4, pChar->nStyle, nClub, pChar->nClipKey, &nCount, &uFlags,
                                 NULL, NULL);
                    if (!(uFlags & 1)) {
                        pChar->n26 = 1;
                        nState = 4;
                        break;
                    }
                }
                pChar->n26 = 0;
                pChar->n24 = GET_AMBIENT_FIDGET_COUNT();
                nState = 2;
            } else {
                nState = 3;
            }
        } else if (pChar->n26 > 0) {
            pChar->n26 = 0;
            pChar->n24 = GET_AMBIENT_FIDGET_COUNT();
            nState = 2;
        }
        break;
    default:
        nState = CharacterState_ResetFidgetState(pChar);
    }
    return nState;
}

// Draws a random number it does not use and returns 1: how many times the idle clip (group 3) plays
// before a fidget (group 4).
s32 GET_IDLE_FIDGET_COUNT(void) {
    Misc_RandFunc(1);
    return 1;
}

// TW06: CharacterState_UpdateGameEmotionState. Set the golfer's emotion (the style his clips are
// picked with, Character_SetEmotion) from his player's reaction type (emotion.c, 0..9) through a
// table, and return the type.
int CharacterState_UpdateGameEmotionState(Character* pChar) {
    int aEmotion[10] = {5, 6, 7, 2, 1, 0, 3, 4, 5, 2};
    int nType = fn_8006AA9C(pChar->nPlayer);
    Character_SetEmotion(pChar, aEmotion[nType]);
    return nType;
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
        Character_SetEmotion(pChar, 6);
    } else if (nScore == 0) {
        Character_SetEmotion(pChar, 5);
    } else if (nScore == 0) {
        Character_SetEmotion(pChar, 0);
    } else {
        Character_SetEmotion(pChar, 2);
    }
    CharacterState_AddSKABlendData(pChar, 1, 9, SKABlender_BlendLinear, 1, 8, -10000.0f, -30000.0f,
                                   -10000.0f, 0.0f,
                                   -10000.0f);
}

// The body's state change (EA's CharacterAnimStateE: nAnim the target, n20 the current state). Once
// the player's time reaches the queued transition (n170 at f174) that becomes the target; while a
// change is waiting (n18 bit 0) the target's clip group plays: 1 group 1 (16 on the tee with
// options nWind >= 1 and a24[6] off, or 1 in 10 with club 0-5; slot 0 only), 2 the clip playing
// jumps to its end (or start), 3 group 8, 4 group 7, 5 the idle (CharacterState_UpdateFidgetState),
// 6 the backswing and 7 the downswing (group 0), 8 no new clip (flag 0x40, event 0xB), 9 the
// reaction (group 5, or 10 for reaction types 8 and 9), 10 group 14, 11 the tap-in, 12 group 6, 14
// group 11 (clip key 6 on the tee, else 0). Then the skeleton's IK is turned on or off (blended
// over fIKTransitionTime where the state asks) and the target becomes the current state.
void CharacterState_UpdateSKAState(Character* pChar) {
    f32 fIKTransitionTime = 0.25f;
    u8 bEnableIK = 0;
    u8 bTransitionIK = 0;
    u8 bReset;
    int nOldClipKey;
    u8 bFromOtherState;
    int nGroup;
    int nType;
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
                   gPlayers[pChar->nPlayer].nClub <= 5 && Misc_RandFunc(1) % 100 < 10 && pChar->nSlot == 0) {
            nGroup = 16;
        }
        CharacterState_AddSKABlendData(pChar, 1, nGroup, SKABlender_BlendLinear, 1, 2, -10000.0f, -30000.0f,
                                       -10000.0f,
                                       0.0f, -10000.0f);
        break;
    case 10:
        CharacterState_AddSKABlendData(pChar, 1, 14, SKABlender_BlendLinear, 1, 0, -10000.0f, -30000.0f,
                                       -10000.0f, 0.0f,
                                       -10000.0f);
        Character_PlaceFeetOnGround(pChar);
        break;
    case 2:
        switch (pChar->n20) {
        case 4:
        case 1:
            SKATime_SetTime(pChar->anim, -10000.0f);
            CharacterState_SetTransition((AnimPlayer*)pChar->anim, 0, -10000.0f);
            break;
        case 3:
            SKATime_SetTime(pChar->anim, -30000.0f);
            CharacterState_SetTransition((AnimPlayer*)pChar->anim, 0, -10000.0f);
            break;
        default:
            CharacterState_AddSKABlendData(pChar, 1, 1, SKABlender_BlendLinear, 1, 0, -10000.0f, -30000.0f,
                                           -10000.0f,
                                           0.0f, -10000.0f);
            SKATime_SetTime(pChar->anim, -10000.0f);
            break;
        }
        break;
    case 3:
        bReset = 0;
        if (pChar->n20 != 2) {
            bReset = 1;
        }
        CharacterState_AddSKABlendData(pChar, bReset, 8, SKABlender_BlendLinear, 1, 5, -10000.0f, -30000.0f,
                                       -10000.0f,
                                       0.0f, -10000.0f);
        break;
    case 4:
        bReset = 0;
        if (pChar->n20 != 5) {
            bReset = 1;
        }
        CharacterState_AddSKABlendData(pChar, bReset, 7, SKABlender_BlendLinear, 1, 2, -20000.0f, -30000.0f,
                                       -10000.0f,
                                       0.0f, -10000.0f);
        break;
    case 5:
        bReset = 0;
        fOffset = 0.0f;
        if (pChar->u10 & 0x80) {
            pChar->u10 &= ~0x80;
            switch (pChar->n20) {
            case 6:
                SKABlender_ClampT1(&pChar->blend, (AnimPlayer*)pChar->anim, pChar->fAnimTime);
                fOffset = 0.35f;
                break;
            case 5:
                SKABlender_ClampT1(&pChar->blend, (AnimPlayer*)pChar->anim, 0.25f + pChar->fAnimTime);
                fOffset = 0.0f;
                break;
            default:
                bReset = 1;
                break;
            }
            nGroup = CharacterState_ResetFidgetState(pChar);
        } else {
            switch (pChar->n20) {
            case 6:
                SKABlender_ClampT1(&pChar->blend, (AnimPlayer*)pChar->anim, pChar->fAnimTime);
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
            nGroup = CharacterState_UpdateFidgetState(pChar);
        }
        CharacterState_AddSKABlendData(pChar, bReset, nGroup, SKABlender_BlendLinear, 1, 5, -20000.0f,
                                       -30000.0f,
                                       -10000.0f, fOffset, -10000.0f);
        bEnableIK = CharacterState_IsNotFidgeting(pChar);
        bTransitionIK = 1;
        pChar->f174 -= 1.0f;
        break;
    case 6:
        bReset = 0;
        switch (pChar->n20) {
        case 5:
            SKABlender_ClampT1(&pChar->blend, (AnimPlayer*)pChar->anim, 0.25f + pChar->fAnimTime);
            break;
        default:
            bReset = 1;
            break;
        }
        CharacterState_AddSKABlendData(pChar, bReset, 0, SKABlender_BlendLinear, 1, 0, -20000.0f, -50000.0f,
                                       -40000.0f,
                                       0.0f, -10000.0f);
        bEnableIK = 1;
        bTransitionIK = 1;
        CharacterState_SetTransition(&pChar->anim29C, 0, 0.0f);
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
        SKABlender_ClampT1(&pChar->blend, (AnimPlayer*)pChar->anim, pChar->fAnimTime);
        CharacterState_AddSKABlendData(pChar, bReset, 0, SKABlender_BlendLinear, 1, 8, -20000.0f, -70000.0f,
                                       -10000.0f,
                                       fLag, -10000.0f);
        bEnableIK = 1;
        break;
    case 14:
        nOldClipKey = pChar->nClipKey;
        if (gPlayers[pChar->nPlayer].ball.nLie == 0) {
            pChar->nClipKey = 6;
        } else {
            pChar->nClipKey = 0;
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
        CharacterState_AddSKABlendData(pChar, bReset, 11, SKABlender_BlendLinear, 1, 8, -20000.0f, -30000.0f,
                                       -10000.0f,
                                       0.0f, -10000.0f);
        pChar->nClipKey = nOldClipKey;
        bEnableIK = 0;
        break;
    case 8:
        pChar->uFlags &= ~1;
        pChar->uFlags |= 0x40;
        pChar->f198 = 0.0f;
        pChar->f19C = pChar->fAnimTime;
        CharacterState_SetTransition((AnimPlayer*)pChar->anim, 0, -10000.0f);
        if (pChar->nClubClass == 2) {
            bTransitionIK = 1;
            fIKTransitionTime = 0.5f;
        }
        for (i = 0; i < 5; i++) {
            if (gPlayers[i].pChar == pChar) {
                EVENT_Trigger(i, 0xB, NULL, 0);
                break;
            }
        }
        break;
    case 9:
        bFromOtherState = 0;
        nType = CharacterState_UpdateGameEmotionState(pChar);
        switch (nType) {
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
            bFromOtherState = 1;
            break;
        }
        pChar->uFlags &= ~0x40;
        if (bFromOtherState || pChar->nClubClass != 2) {
            CharacterState_AddSKABlendData(pChar, 1, 5, SKABlender_BlendLinear, 1, 0, -20000.0f, -30000.0f,
                                           -10000.0f,
                                           0.0f, -10000.0f);
        } else {
            SKABlender_ClampT1(&pChar->blend, (AnimPlayer*)pChar->anim, pChar->fAnimTime);
            CharacterState_AddSKABlendData(pChar, bFromOtherState, nGroup, SKABlender_BlendLinear, 1, 0,
                                           -20000.0f,
                                           -30000.0f, -10000.0f, 0.5f, -10000.0f);
            bTransitionIK = 1;
            fIKTransitionTime = 0.1f;
            pChar->fAnimTime += 0.0001f;
        }
        break;
    case 11:
        CharacterState_SetTapInState(pChar);
        bTransitionIK = 1;
        break;
    case 12:
        CharacterState_UpdateGameEmotionState(pChar);
        CharacterState_AddSKABlendData(pChar, 1, 6, SKABlender_BlendLinear, 1, 0, -10000.0f, -30000.0f,
                                       -10000.0f, 0.0f,
                                       -10000.0f);
        break;
    }
    if (bTransitionIK) {
        SKEL_TransitionIK(pChar->pModel->pSkel, !bEnableIK, fIKTransitionTime);
    } else if (bEnableIK) {
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

// The morph player's state change, as CharacterState_UpdateSKAState does for the body. Once its
// time reaches the queued transition that becomes the target (n2C); while a change is waiting (u28
// bit 0): state 5 clears the queue (in a round it signals event 0x2A and stays waiting, current
// state 5); states 1 to 3 play a random library of MAL group 0 to 2, blended in over 0.5 and
// repeating 0.5 before its end, but only while the body swings (state 6 or 7) and the morph player
// is not playing a clip's own library (state 4); otherwise the change is dropped. The target then
// becomes the current state (n30).
void CharacterState_UpdateMorphState(Character* pChar) {
    MtaLib* pLib;

    if (pChar->anim29C.nC != 0 && pChar->anim29C.fTime >= pChar->anim29C.f10) {
        pChar->n2C = pChar->anim29C.nC;
        pChar->u28 |= 1;
    }
    if (!(pChar->u28 & 1)) return;
    switch (pChar->n2C) {
    case 5:
        CharacterState_SetTransition(&pChar->anim29C, 0, 0.0f);
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
        pLib = Character_GetRandomMtaLib(pChar, 1, -1);
        SKABlender_ClampT1(&pChar->node3E0, &pChar->anim29C, 0.5f + pChar->anim29C.fTime);
        CharacterState_AddMorphBlendData(pChar, pLib, 0, 1, SKABlender_BlendLinear, 1, 2, -20000.0f,
                                         -30000.0f,
                                         -10000.0f, 0.0f, -10000.0f);
        pChar->anim29C.f10 -= 0.5f;
        break;
    case 3:
        if (pChar->n30 == 4 || (pChar->n20 != 6 && pChar->n20 != 7)) {
            goto skip;  // fake match: as in case 2
        }
        SKABlender_ClampT1(&pChar->node3E0, &pChar->anim29C, 0.5f + pChar->anim29C.fTime);
        pLib = Character_GetRandomMtaLib(pChar, 2, -1);
        CharacterState_AddMorphBlendData(pChar, pLib, 0, 2, SKABlender_BlendLinear, 1, 3, -20000.0f,
                                         -30000.0f,
                                         -10000.0f, 0.0f, -10000.0f);
        pChar->anim29C.f10 -= 0.5f;
        break;
    case 1:
        if (pChar->n30 == 4 || (pChar->n20 != 6 && pChar->n20 != 7)) {
            goto skip;  // fake match: as in case 2
        }
        SKABlender_ClampT1(&pChar->node3E0, &pChar->anim29C, 0.5f + pChar->anim29C.fTime);
        pLib = Character_GetRandomMtaLib(pChar, 0, -1);
        CharacterState_AddMorphBlendData(pChar, pLib, 0, 0, SKABlender_BlendLinear, 1, 1, -20000.0f,
                                         -30000.0f,
                                         -10000.0f, 0.0f, -10000.0f);
        pChar->anim29C.f10 -= 0.5f;
        break;
    }
    pChar->n30 = pChar->n2C;
skip:
    pChar->u28 &= ~1;
}

// EA's name, though it returns a time: halfway between pBlend's last key time (pD8's f0C) and its
// event 2 (the ball hit). SKABlend_CalculateBlendInfo starts a window there for fFrom -90000.
f32 CharacterState_GetLastSKAStateID(Character* pChar) {
    f32 fTime  = SKA_GetTagTime(pChar->pBlend, 2);
    f32 fStart = pChar->pBlend->pD8->f0C;
    fTime = (fTime - fStart) / 2.0f + fStart;
    return fTime;
}
