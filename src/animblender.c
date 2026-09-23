// animblender.c (TW06's golf/animation/animblender.c by its place in the link order; the name is
// not proven, and TW06's SKATime_* functions sit near its end): blends the golfer's animations.
// A character's blend tree (SKABlendNode) has blend nodes with two children and source nodes that
// play one clip; the nodes and their pose buffers come from five pools. The animation player
// (AnimPlayer) functions follow. Only part is decompiled so far.

#include "character.h"

f32  fn_8001F02C(ClipBlend* pBlend, u64 uEvent);   // an event's time (by its 64-bit id)

int  fn_800723E8(SKABlendNode* pNode, SKABlendNode*** pppOldest);
void fn_800725BC(SKABlendNode* pNode, SKABlendFn pfnBlend, f32 fWeight);
int  fn_8007286C(SKABlendNode* pNode, f32 fTime);
f32  fn_800728D8(SKABlendNode* pNode);
f32  fn_80072938(SKABlendNode* pNode);
void fn_8007325C(u8* pAnim);
f32  fn_800732B8(f32 fTime, f32 fNow, f32 fStart, f32 fEnd);
int  fn_800734D0(SKABlendNode* pNode);

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
