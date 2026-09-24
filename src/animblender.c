// animblender.c (TW06's golf/animation/animblender.c by its place in the link order; the name is
// not proven, and TW06's SKATime_* functions sit near its end): blends the golfer's animations.
// A character's blend tree (SKABlendNode) has blend nodes with two children and source nodes that
// play one clip; the nodes and their pose buffers come from five pools. The animation player
// (AnimPlayer) functions follow. Only part is decompiled so far.

#include "character.h"
#include "charstate.h"
#include "golfer.h"

f32  fn_8001F02C(Clip* pBlend, u64 uEvent);   // an event's time (by its 64-bit id)
void fn_8001F558(void* pItem);          // mtalib.c
void fn_800977CC(void* p);
void fn_800293CC(int nBone, int nCount, SkelPose* pA, SkelPose* pB, SkelPose* pOut, f32 fT);   // Skeleton.c
void fn_80036180(SkelPose1* pA, SkelPose1* pB, SkelPose1* pOut);             // the same, format 1

int  fn_800723E8(SKABlendNode* pNode, SKABlendNode*** pppOldest);
int  fn_8007286C(SKABlendNode* pNode, f32 fTime);
f32  fn_800728D8(SKABlendNode* pNode);
void fn_8007325C(u8* pAnim);
f32  fn_800732B8(f32 fTime, f32 fNow, f32 fStart, f32 fEnd);
int  fn_800734D0(SKABlendNode* pNode);
f32  fn_800737B4(AnimPlayer* pPlayer, f32 fT);

// Create the blend tree pools: 10 of each in game types 3 and 10, else 50.
void fn_80071AD0(void) {
    int nCount;

    if (gSession.nGameType == 3 || gSession.nGameType == 10) {
        nCount = 10;
    } else {
        nCount = 50;
    }
    lbl_80281E98 = fn_8000AFA0(nCount, 0x34, 2, 16);
    lbl_80281E94 = fn_8000AFA0(nCount, 0x2C, 2, 16);
    lbl_80281E90 = fn_8000AFA0(nCount, 0x20, 2, 16);
    lbl_80281E8C = fn_8000AFA0(nCount, sizeof(SkelPose), 2, 16);
    lbl_80281E88 = fn_8000AFA0(nCount, 0x114C, 2, 16);
}

// Destroy the pools fn_80071AD0 made.
void fn_80071B94(void) {
    if (lbl_80281E98 != NULL) {
        fn_8000B058(lbl_80281E98);
        lbl_80281E98 = NULL;
    }
    if (lbl_80281E94 != NULL) {
        fn_8000B058(lbl_80281E94);
        lbl_80281E94 = NULL;
    }
    if (lbl_80281E90 != NULL) {
        fn_8000B058(lbl_80281E90);
        lbl_80281E90 = NULL;
    }
    if (lbl_80281E8C != NULL) {
        fn_8000B058(lbl_80281E8C);
        lbl_80281E8C = NULL;
    }
    if (lbl_80281E88 != NULL) {
        fn_8000B058(lbl_80281E88);
        lbl_80281E88 = NULL;
    }
}

// Give the tree at *ppNode back: each node's pose buffer to its pool, then its children (or, with
// bFreeSources, a source node's clip), then the node itself if it came from a pool (*ppNode is
// then NULL).
void fn_80071F58(SKABlendNode** ppNode, u8 bFreeSources) {
    int i;

    if (ppNode == NULL) return;
    if (*ppNode == NULL) return;
    if ((*ppNode)->nFormat == 0) {
        if ((*ppNode)->pPose != NULL) {
            fn_8000B0D4(lbl_80281E8C, (*ppNode)->pPose);
        }
    } else if ((*ppNode)->nFormat == 1) {
        if ((*ppNode)->pPose != NULL) {
            fn_8000B0D4(lbl_80281E88, (*ppNode)->pPose);
        }
    }
    (*ppNode)->pPose = NULL;
    if ((*ppNode)->nType == 1) {
        for (i = 0; i < 2; i++) {
            fn_80071F58(&(*ppNode)->u.blend.apChild[i], bFreeSources);
        }
    } else if ((*ppNode)->nType == 0) {
        if ((*ppNode)->nFormat == 0) {
            if (bFreeSources) {
                fn_800977CC((*ppNode)->u.src.pSrc);
            }
        } else if ((*ppNode)->nFormat == 1) {
            if (bFreeSources) {
                fn_8001F558((*ppNode)->u.src.pSrc);
            }
        }
    }
    if ((*ppNode)->bPooled == 1) {
        switch ((*ppNode)->nType) {
        case 0:
            fn_8000B0D4(lbl_80281E98, *ppNode);
            break;
        case 1:
            fn_8000B0D4(lbl_80281E94, *ppNode);
            break;
        default:
            fn_8000B0D4(lbl_80281E90, *ppNode);
            break;
        }
        *ppNode = NULL;
    }
}

// How many source nodes the tree under pNode has; *pppOldest gets the slot of the one that ends
// first (left alone when it already holds an earlier one).
int fn_800723E8(SKABlendNode* pNode, SKABlendNode*** pppOldest) {
    int i = 0;
    int nSources = 0;
    SKABlendNode* pChild;

    if (pNode == NULL) return 0;
    do {
        pChild = pNode->u.blend.apChild[i];
        if (pChild != NULL) {
            if (pChild->nType == 1) {
                nSources += fn_800723E8(pChild, pppOldest);
            } else if (pChild->nType == 0) {
                if (*pppOldest == NULL) {
                    *pppOldest = &pNode->u.blend.apChild[i];
                } else if (pChild->fEnd < (**pppOldest)->fEnd) {
                    *pppOldest = &pNode->u.blend.apChild[i];
                }
                nSources++;
            }
        }
        i++;
    } while (i < 2);
    return nSources;
}

// Make pNode a blend node that mixes its children with pfnBlend, and take its times from them.
void fn_800725BC(SKABlendNode* pNode, SKABlendFn pfnBlend, f32 fWeight) {
    if (pNode != NULL) {
        pNode->nType = 1;
        pNode->fWeight = fWeight;
        pNode->u.blend.pfnBlend = pfnBlend;
        pNode->fStart = fn_800728D8(pNode);
        pNode->fEnd = fn_80072938(pNode);
    }
}

// Which of pNode's children play at fTime: -1 neither, 0 or 1 that one, 2 both.
int fn_8007286C(SKABlendNode* pNode, f32 fTime) {
    SKABlendNode* pChild;
    int i = 0;
    int nPlaying = -1;

    if (pNode != NULL) {
        while (i < 2 && nPlaying < 2) {
            pChild = pNode->u.blend.apChild[i];
            if (pChild != NULL && !(pChild->fStart > fTime) && !(pChild->fEnd < fTime)) {
                if (nPlaying == -1) {
                    nPlaying = i;
                } else {
                    nPlaying = 2;
                }
            }
            i++;
        }
    }
    return nPlaying;
}

// The earliest start of pNode's children (0 without children).
f32 fn_800728D8(SKABlendNode* pNode) {
    f32 fStart = 1073741824.0f;
    int bFound = 0;

    if (pNode != NULL) {
        if (pNode->u.blend.apChild[0] != NULL) {
            bFound = 1;
            if (pNode->u.blend.apChild[0]->fStart < fStart) {
                fStart = pNode->u.blend.apChild[0]->fStart;
            }
        }
        if (pNode->u.blend.apChild[1] != NULL) {
            bFound = 1;
            if (pNode->u.blend.apChild[1]->fStart < fStart) {
                fStart = pNode->u.blend.apChild[1]->fStart;
            }
        }
    }
    if (bFound == 0) return 0.0f;
    return fStart;
}

// The latest end of pNode's children (0 without children).
f32 fn_80072938(SKABlendNode* pNode) {
    f32 fEnd = 0.0f;

    if (pNode == NULL) return fEnd;
    if (pNode->u.blend.apChild[0] != NULL && pNode->u.blend.apChild[0]->fEnd > fEnd) {
        fEnd = pNode->u.blend.apChild[0]->fEnd;
    }
    if (pNode->u.blend.apChild[1] != NULL && pNode->u.blend.apChild[1]->fEnd > fEnd) {
        fEnd = pNode->u.blend.apChild[1]->fEnd;
    }
    return fEnd;
}

// The blend weight at fTime across the overlap of two nodes, from the later start to the earlier
// end: it runs from nLater (0 when pA starts later, else 1) to the other end, or back with bOut.
// When one node lies inside the other, it is folded to |2w - 1|.
f32 fn_80072980(SKABlendNode* pA, SKABlendNode* pB, u8 bOut, f32 fTime) {
    f32 fStart;
    f32 fEnd;
    int nLater;
    u8 bInside;
    f32 fDir;
    f32 fLen;
    f32 fWeight;

    if (pA->fStart > pB->fStart) {
        fStart = pA->fStart;
        nLater = 0;
    } else {
        nLater = 1;
        fStart = pB->fStart;
    }
    if (pA->fEnd >= pB->fEnd) {
        fEnd = pB->fEnd;
        bInside = nLater == 1;
    } else {
        fEnd = pA->fEnd;
        bInside = nLater == 0;
    }
    fDir = 2.0f * ((f32)nLater - 0.5f);
    if (!bOut) {
        fLen = fEnd - fStart;
        if (fLen < 0.00001f) {
            fWeight = nLater;
        } else {
            fWeight = nLater - fDir * (fTime - fStart) / fLen;
        }
    } else {
        fLen = fStart - fEnd;
        if (fLen < 0.00001f) {
            fWeight = nLater;
        } else {
            fWeight = nLater - fDir * (fTime - fEnd) / fLen;
        }
    }
    if (bInside) {
        return fabsf(2.0f * fWeight - 1.0f);
    }
    return fWeight;
}

// The blend callback: pose pNode's buffer at fTime from its children. While both play (or neither,
// between them) and fTime is inside their overlap, their weights come from fn_80072980 and the
// poses are blended by format; while only one plays, its pose is copied (a format 1 copy then
// has the child's morph bits cleared).
void fn_80072ACC(SKABlendNode* pNode, int* pn, f32 fTime) {
    int nPlaying;
    u8 bBetween;
    f32 fWeight;
    int i;

    if (pNode == NULL) return;
    if (pNode->nType != 1) return;
    nPlaying = fn_8007286C(pNode, fTime);
    if (nPlaying == 2 || (nPlaying == -1 && pNode->u.blend.apChild[0] != NULL &&
                          pNode->u.blend.apChild[1] != NULL)) {
        bBetween = nPlaying == -1;
        if (fTime >= pNode->u.blend.apChild[0]->fEnd && fTime >= pNode->u.blend.apChild[1]->fEnd) {
            return;
        }
        if (fTime <= pNode->u.blend.apChild[0]->fStart && fTime <= pNode->u.blend.apChild[1]->fStart) {
            return;
        }
        fWeight = fn_80072980(pNode->u.blend.apChild[0], pNode->u.blend.apChild[1], bBetween, fTime);
        pNode->u.blend.apChild[0]->fWeight = fWeight;
        pNode->u.blend.apChild[1]->fWeight = 1.0f - fWeight;
        if (pNode->nFormat == 0) {
            fn_800293CC(1, *pn - 1, pNode->u.blend.apChild[0]->pPose, pNode->u.blend.apChild[1]->pPose,
                        pNode->pPose, fWeight);
        } else if (pNode->nFormat == 1) {
            fn_80036180((SkelPose1*)pNode->u.blend.apChild[0]->pPose,
                        (SkelPose1*)pNode->u.blend.apChild[1]->pPose, (SkelPose1*)pNode->pPose);
        }
    } else if (nPlaying != -1) {
        if (pNode->u.blend.apChild[nPlaying]->nFormat == 0) {
            memcpy(pNode->pPose, pNode->u.blend.apChild[nPlaying]->pPose, sizeof(SkelPose));
        } else if (pNode->u.blend.apChild[nPlaying]->nFormat == 1) {
            memcpy(pNode->pPose, pNode->u.blend.apChild[nPlaying]->pPose, sizeof(SkelPose1));
            for (i = 0; i < 3; i++) {
                fn_8001E938(((SkelPose1*)pNode->u.blend.apChild[nPlaying]->pPose)->aBlocks[i].aBits, 20);
            }
        }
    }
}

// The time of event uEvent in the first source under pNode that has it (0 when none has).
f32 fn_80072CB8(SKABlendNode* pNode, u64 uEvent) {
    f32 fTime = 0.0f;
    int i = 0;
    SKABlendNode* pChild;

    if (pNode == NULL) return fTime;
    while (fTime == 0.0f && i < 2) {
        pChild = pNode->u.blend.apChild[i];
        if (pChild != NULL) {
            if (pChild->nType == 1) {
                fTime = fn_80072CB8(pChild, uEvent);
            } else if (pChild->nType == 0 && pChild->nFormat == 0) {
                fTime = fn_8001F02C(pChild->u.src.pSrc, uEvent);
            }
        }
        i++;
    }
    return fTime;
}

// Resets a player: stopped at time 0, f14 1, and its ten entries chained in order from p44.
void fn_80072D90(AnimPlayer* pPlayer) {
    int i;
    AnimPlayerEntry* pPrev;

    pPlayer->fTime = 0.0f;
    pPlayer->n08 = 0;
    pPlayer->uFlags = 0;
    pPlayer->n00 = 0;
    pPlayer->f14 = 1.0f;
    pPlayer->nC = 0;
    pPlayer->f10 = 0.0f;
    pPlayer->n3C = 0;
    pPlayer->n40 = 0;
    pPlayer->p44 = &pPlayer->a48[0];
    pPrev = NULL;
    for (i = 0; i < 10; i++) {
        pPlayer->a48[i].pNext = (i < 9) ? &pPlayer->a48[i + 1] : NULL;
        pPlayer->a48[i].pPrev = pPrev;
        pPrev = &pPlayer->a48[i];
    }
}

// Advances a player by fT across the times of the tree under pNode: forward, or backward with
// uFlags bit 6. At an end n08 counts the plays down (at 0 the player stops there, bit 2; with bit 8
// it rewinds and clears itself instead); with bit 5 it turns round (bit 6 flips), else it wraps to
// the other end and sets bit 12.
void fn_80072ED8(AnimPlayer* pPlayer, SKABlendNode* pNode, f32 fT) {
    f32 fStep;
    f32 fEnd;
    f32 fStart;

    pPlayer->uFlags &= ~0x1000;
    pPlayer->fStart = fn_800728D8(pNode);
    pPlayer->fEnd = fn_80072938(pNode);
    if (pPlayer->uFlags & 0x80) {
        pPlayer->f30 -= fT;
        if (pPlayer->f30 <= 0.0f) {
            pPlayer->f30 = 0.0f;
            pPlayer->uFlags &= ~0x81;
        }
    }
    pPlayer->uFlags &= ~4;
    fStep = fn_800737B4(pPlayer, fT);
    fStart = pPlayer->fStart;
    fEnd = pPlayer->fEnd;
    if (pPlayer->uFlags & 1) return;
    if (pPlayer->uFlags & 0x40) {
        pPlayer->fTime -= fStep;
        if (pPlayer->fTime < fStart) {
            if (pPlayer->n08 != 0 && pPlayer->n08 > 0) {
                pPlayer->n08--;
            }
            if (pPlayer->n08 == 0) {
                pPlayer->uFlags |= 4;
                pPlayer->fTime = fStart;
                return;
            }
            if (pPlayer->uFlags & 0x20) {
                pPlayer->fTime = fStart;
                pPlayer->uFlags ^= 0x40;
                pPlayer->uFlags |= 4;
                return;
            }
            pPlayer->fTime = fEnd;
            pPlayer->uFlags |= 0x1000;
        }
    } else {
        pPlayer->fTime += fStep;
        if (pPlayer->fTime > fEnd) {
            if (pPlayer->n08 != 0 && pPlayer->n08 > 0) {
                pPlayer->n08--;
            }
            if (pPlayer->n08 == 0) {
                if (pPlayer->uFlags & 0x100) {
                    pPlayer->fTime = 0.0f;
                    pPlayer->uFlags &= ~0x105;
                    pPlayer->n00 = 0;
                    pPlayer->n08 = 1;
                    return;
                }
                pPlayer->uFlags |= 4;
                pPlayer->fTime = fEnd;
                return;
            }
            if (pPlayer->uFlags & 0x20) {
                pPlayer->fTime = fEnd;
                pPlayer->uFlags ^= 0x40;
                pPlayer->uFlags |= 4;
                return;
            }
            pPlayer->fTime = fStart;
            pPlayer->uFlags |= 0x1000;
        }
    }
}

// Sways pPlayer's time around f38: three cosines of the f34 clock (advanced by fT) make a wave
// from 0 to 1, scaled by 0.033 or 0.3 (club 25, by the clip group) or 0.05; the player then runs
// forward or backward (uFlags bit 6) towards that time.
void fn_80073108(Character* pChar, int nPlayer, AnimPlayer* pPlayer, SKABlendNode* pNode, f32 fT) {
    f32 fWave;
    f32 fDelta;

    pPlayer->f34 += fT;
    fWave = 1.0f - (3.0f + (fn_80009638(pPlayer->f34 / 5.0f) +
                            (fn_80009638(5.0f * pPlayer->f34) + fn_80009638(7.0f * pPlayer->f34 / 3.0f)))) /
                       6.0f;
    if (gPlayers[nPlayer].nClub == 25) {
        if (pChar->blend.nGroup == 9) {
            fWave *= 0.033f;
        } else {
            fWave *= 0.3f;
        }
    } else {
        fWave *= 0.05f;
    }
    fDelta = (pPlayer->f38 - fWave) - pPlayer->fTime;
    if (fDelta < 0.0f) {
        fDelta = -fDelta;
        pPlayer->uFlags |= 0x40;
    } else {
        pPlayer->uFlags &= ~0x40;
    }
    fn_80072ED8(pPlayer, pNode, fDelta);
}

// Character.anim is still declared as bytes, so these three take its address as a u8*.
void fn_8007325C(u8* pAnim) {
    ((AnimPlayer*)pAnim)->uFlags |= 2;
}

void fn_8007326C(u8* pAnim) {
    ((AnimPlayer*)pAnim)->uFlags &= ~3;
}

void Anim_SetTime(u8* pAnim, f32 fTime) {
    AnimPlayer* pPlayer = (AnimPlayer*)pAnim;

    pPlayer->fTime = fn_800732B8(fTime, pPlayer->fTime, pPlayer->fStart, pPlayer->fEnd);
}

// A time that may be a code: -10000 is the end, -20000 now, -30000 the start.
f32 fn_800732B8(f32 fTime, f32 fNow, f32 fStart, f32 fEnd) {
    if (-10000.0f == fTime) return fEnd;
    if (-20000.0f == fTime) return fNow;
    if (-30000.0f == fTime) return fStart;
    return fTime;
}

// Cuts the tree at pNode off at fTime: when pPlayer's time is inside it, its end (the player's
// too) and its children's ends come down to fTime (a source's fTo in proportion) and all are
// flagged in bC; otherwise the player goes back to 0 and pNode is freed and taken again as an
// empty blend node of the same format and callback.
void fn_800732F4(SKABlendNode* pNode, AnimPlayer* pPlayer, f32 fTime) {
    s32 nFormat;
    SKABlendFn pfnBlend;
    SKABlendNode* pChild;

    if (pPlayer->fTime < pNode->fStart || fn_8007286C(pNode, pPlayer->fTime) == -1) {
        nFormat = pNode->nFormat;
        pfnBlend = pNode->u.blend.pfnBlend;
        pPlayer->fTime = 0.0f;
        pPlayer->fEnd = 0.0f;
        pPlayer->fStart = 0.0f;
        fn_80071F58(&pNode, 0);
        fn_80071C28(&pNode, 1, nFormat, pfnBlend, 1);
        return;
    }
    if (pNode->fEnd > fTime) {
        pNode->fEnd = fTime;
        pPlayer->fEnd = fTime;
        pChild = pNode->u.blend.apChild[0];
        if (pChild != NULL) {
            if (pChild->fEnd > fTime) {
                if (pChild->nType == 0) {
                    pChild->u.src.fTo = pChild->u.src.fFrom + (fTime - pChild->fStart) *
                        ((pChild->u.src.fTo - pChild->u.src.fFrom) / (pChild->fEnd - pChild->fStart));
                }
                pNode->u.blend.apChild[0]->fEnd = fTime;
            }
            pNode->u.blend.apChild[0]->bC = 1;
        }
        pChild = pNode->u.blend.apChild[1];
        if (pChild != NULL) {
            if (pChild->fEnd > fTime) {
                if (pChild->nType == 0) {
                    pChild->u.src.fTo = pChild->u.src.fFrom + (fTime - pChild->fStart) *
                        ((pChild->u.src.fTo - pChild->u.src.fFrom) / (pChild->fEnd - pChild->fStart));
                }
                pNode->u.blend.apChild[1]->fEnd = fTime;
            }
            pNode->u.blend.apChild[1]->bC = 1;
        }
    }
    pNode->bC = 1;
}

// The tree under pNode plays other than exactly one source.
u8 fn_800734A0(SKABlendNode* pNode) {
    return fn_800734D0(pNode) != 1;
}

// How many source nodes the tree under pNode has.
int fn_800734D0(SKABlendNode* pNode) {
    int i = 0;
    int nSources = 0;
    SKABlendNode* pChild;

    do {
        pChild = pNode->u.blend.apChild[i];
        if (pChild != NULL) {
            if (pChild->nType == 1) {
                nSources += fn_800734D0(pChild);
            } else if (pChild->nType == 0) {
                nSources++;
            }
        }
        i++;
    } while (i < 2);
    return nSources;
}

// A source under pNode plays pSrc (format 0; the format tested is pNode's own).
u8 fn_80073554(SKABlendNode* pNode, void* pSrc) {
    int i;
    SKABlendNode* pChild;

    for (i = 0; i < 2; i++) {
        pChild = pNode->u.blend.apChild[i];
        if (pChild != NULL) {
            if (pChild->nType == 1) {
                if (fn_80073554(pChild, pSrc) == 1) return 1;
            } else if (pChild->nType == 0 && pNode->nFormat == 0 && pChild->u.src.pSrc == pSrc) {
                return 1;
            }
        }
    }
    return 0;
}

// The same for format 1, and never for no source.
u8 fn_80073610(SKABlendNode* pNode, void* pSrc) {
    int i;
    SKABlendNode* pChild;

    if (pSrc == NULL) return 0;
    for (i = 0; i < 2; i++) {
        pChild = pNode->u.blend.apChild[i];
        if (pChild != NULL) {
            if (pChild->nType == 1) {
                if (fn_80073610(pChild, pSrc) == 1) return 1;
            } else if (pChild->nType == 0 && pNode->nFormat == 1 && pChild->u.src.pSrc == pSrc) {
                return 1;
            }
        }
    }
    return 0;
}

// Clear bit nBit in the three blocks of pNode's format 1 pose buffer, and of every source's under
// it (a source's only when its parent is format 1).
void fn_800736D8(SKABlendNode* pNode, s32 nBit) {
    int j;
    SKABlendNode* pChild;
    int i;
    int n = nBit;   // fake match: a copy of the parameter for the pose calls

    for (i = 0; i < 3; i++) {
        fn_8001EB6C(((SkelPose1*)pNode->pPose)->aBlocks[i].aBits, n);
    }
    for (i = 0; i < 2; i++) {
        pChild = pNode->u.blend.apChild[i];
        if (pChild != NULL) {
            if (pChild->nType == 1) {
                fn_800736D8(pChild, nBit);
            } else if (pChild->nType == 0 && pNode->nFormat == 1) {
                for (j = 0; j < 3; j++) {
                    fn_8001EB6C(((SkelPose1*)pChild->pPose)->aBlocks[j].aBits, n);
                }
            }
        }
    }
}

// The player's time step fT scaled by f14, while a blend in (uFlags bit 3) or out (bit 4) runs
// also by f28 / f24; a finished blend in clears bit 3, a finished blend out swaps bit 4 for bit 0.
f32 fn_800737B4(AnimPlayer* pPlayer, f32 fT) {
    f32 fStep = fT * pPlayer->f14;

    if (pPlayer->uFlags & 8) {
        pPlayer->f28 += fStep;
        if (pPlayer->f28 >= pPlayer->f24) {
            pPlayer->uFlags &= ~8;
            pPlayer->f28 = 0.0f;
            return fStep;
        }
        return fStep * (pPlayer->f28 / pPlayer->f24);
    }
    if (pPlayer->uFlags & 0x10) {
        pPlayer->f28 -= fStep;
        if (pPlayer->f28 < pPlayer->f2C) {
            pPlayer->f28 = pPlayer->f2C;
        }
        if (pPlayer->f28 <= 0.0f) {
            pPlayer->uFlags &= ~0x10;
            pPlayer->uFlags |= 1;
            pPlayer->f28 = 0.0f;
            return fStep;
        }
        return fStep * (pPlayer->f28 / pPlayer->f24);
    }
    return fStep;
}
