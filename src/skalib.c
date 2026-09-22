// skalib.c (EA's name, from its asserts): the skeletal animation library. So far only the clip
// lookup and pick, the part that chooses which clip a golfer plays.

#include "golfer.h"

char* strcpy(char* pDst, const char* pSrc);
int   strcmp(const char* pA, const char* pB);

// An animation library (a SAL object: glbchar.gcb holds the shared male/female ones, and each
// character's CHR object embeds its own). On disc it is little-endian; the loader swaps it and
// fills in the pointers. Clips are found through a tree of s16 byte offsets into pTree:
//   group node:  [0] default leaf, [1 + style] style node (8 styles)
//   style node:  [club] club node (6 club classes)
//   club node:   [1] default leaf, [2 + key] leaf (11 keys)
//   leaf:        [0] clip count, [1] first entry in ppClips, then a u32 "played" mask
typedef struct AnimLib {
    s32    groups[64];          // 0x000  byte offset of each group's node, -1 none (21 used)
    u32    id[2];               // 0x100
    s32    n108;                // 0x108
    s32    nDefault;            // 0x10C  the leaf used when a group has nothing
    s32    nTreeSize;           // 0x110
    s32    nClips;              // 0x114  entries in ppClips
    s32    nRecords;            // 0x118  36-byte clip records (a 16-character name first)
    u8*    pTree;               // 0x11C
    void** ppClips;             // 0x120  clip pointers; a clip's name is at +0xA0
} AnimLib;

extern char (*lbl_80281D14)[2][8][6][16];   // the last clip name played: [player][reaction kind][style][club]

// The clips for an animation group, style, club class and key: each level falls back to its
// default (flag 1 when the group, style or club level did, flag 2 when only the key did), and
// the library default is the last resort. Returns the leaf's clip pointers.
void** AnimLib_Find(AnimLib* pLib, int nGroup, int nStyle, int nClub, int nKey, s32* pCount,
                    u32* pFlags, u32** ppUsed, s32* pFirst) {
    s16* pNode;
    s16* pClub;
    s32  nOff;
    s16* pLeaf;

    if (ppUsed != NULL) *ppUsed = NULL;
    if (nGroup >= 0 && nGroup < 21 && nStyle >= 0 && nStyle < 8 && nClub >= 0 && nClub < 6 && nKey >= 0 &&
        nKey < 11) {
        *pCount = 1;
        nOff = pLib->groups[nGroup];
        if (nOff < 0) {
            *pFlags |= 1;
            nOff = pLib->nDefault;
            goto leaf;
        }
        pNode = (s16*)(pLib->pTree + nOff);
        nOff  = pNode[1 + nStyle];
        if (nOff < 0 && nStyle != 0) nOff = pNode[1];
        if (nOff < 0) {
            *pFlags |= 1;
            nOff = pNode[0];
            if (nOff >= 0) goto leaf;
            nOff = pLib->nDefault;
            if (nOff >= 0) goto leaf;
            return NULL;
        }
        nOff = *(s16*)(pLib->pTree + nOff + nClub * 2);
        if (nOff < 0) {
            *pFlags |= 1;
            nOff = pNode[0];
            if (nOff >= 0) goto leaf;
            nOff = pLib->nDefault;
            if (nOff >= 0) goto leaf;
            return NULL;
        }
        pClub = (s16*)(pLib->pTree + nOff);
        nOff  = pClub[2 + nKey];
        if (nOff < 0) {
            *pFlags |= 2;
            nOff = pClub[1];
            if (nOff >= 0) goto leaf;
            nOff = pNode[0];
            if (nOff >= 0) goto leaf;
            nOff = pLib->nDefault;
            if (nOff >= 0) goto leaf;
            return NULL;
        }
leaf:
        pLeaf   = (s16*)(pLib->pTree + nOff);
        *pCount = pLeaf[0];
        if (ppUsed != NULL) *ppUsed = (u32*)(pLeaf + 2);
        if (pFirst != NULL) *pFirst = pLeaf[1];
        return pLib->ppClips + pLeaf[1];
    }
    return NULL;
}

// For reactions (groups 1 and 5): whether this clip is the one the player last played for this
// group, style and club class; *ppSlot gets that slot so the caller can record the new one.
u8 AnimLib_WasLastPlayed(int nPlayer, const char* pName, char** ppSlot, int nGroup, int nStyle, int nClub) {
    int nKind;
    *ppSlot = NULL;
    switch (nGroup) {
    case 5:
        nKind = 0;
        break;
    case 1:
        nKind = 1;
        break;
    default:
        return 0;
    }
    if (nPlayer < 0 || nPlayer >= 4) return 0;
    *ppSlot = lbl_80281D14[nPlayer][nKind][nStyle][nClub];
    return strcmp(pName, lbl_80281D14[nPlayer][nKind][nStyle][nClub]) == 0;
}

int   AnimLib_RandomIndex(u32 uUsed, int nCount);
u8    fn_800C9828(int nGroup, int nStyle, int nClub, int nKey);
void* fn_800CAA7C(int nPlayer, int nGroup, int nStyle, int nClub);
void* fn_80021ADC(AnimLib* pLib, const char* pName);

// The clip a player plays for a group and style. A named clip (lessons) is looked up by name.
// Otherwise one of the leaf's clips at random, never the reaction played last time, and - through
// the leaf's played mask - none again until all of them have been played.
void* AnimLib_Pick(int nPlayer, AnimLib* pLib, int nGroup, int nStyle, int nClub, int nKey, u32* pFlags,
                   const char* pName) {
    s32    nCount;
    u32*   pUsed;
    char*  pSlot;
    void** ppClips;
    int    nPick;
    u32    uUsed;
    u32    uBit;
    u32    uAll;
    void*  pClip;

    AnimLib_Find(pLib, nGroup, nStyle, nClub, nKey, &nCount, pFlags, &pUsed, NULL);
    if (fn_800C9828(nGroup, nStyle, nClub, (*pFlags & 2) ? -1 : nKey)) {
        pClip = fn_800CAA7C(nPlayer, nGroup, nStyle, nClub);
        if (pClip != NULL) return pClip;
    }
    if (nGroup >= 0 && nGroup < 21) {
        ppClips = AnimLib_Find(pLib, nGroup, nStyle, nClub, nKey, &nCount, pFlags, &pUsed, NULL);
        if (ppClips != NULL && nCount > 0) {
            if (pName == NULL) {
                pSlot = NULL;
                nPick = AnimLib_RandomIndex(*pUsed, nCount);
                if (nCount > 1 && AnimLib_WasLastPlayed(nPlayer, (char*)ppClips[nPick] + 0xA0, &pSlot, nGroup, nStyle, nClub)) {
                    if (++nPick >= nCount) nPick = 0;
                }
                if (nPick < 32 && pUsed != NULL) {
                    uUsed = *pUsed;
                    uAll  = (1 << nCount) - 1;
                    if ((uAll & uUsed) == uAll) {
                        uUsed = 1 << nPick;
                    } else {
                        uBit = 1 << nPick;
                        while (uUsed & uBit) {
                            if (++nPick >= nCount) nPick = 0;
                            uBit = 1 << nPick;
                        }
                        uUsed |= uBit;
                        if ((uAll & uUsed) == uAll) uUsed = 1 << nPick;
                    }
                    if (pSlot != NULL) strcpy(pSlot, (char*)ppClips[nPick] + 0xA0);
                    *pUsed |= uUsed;
                }
                return ppClips[nPick];
            }
            return fn_80021ADC(pLib, pName);
        }
    }
    return NULL;
}

// A random clip index under nCount, re-rolled up to twice when it has been played already.
int AnimLib_RandomIndex(u32 uUsed, int nCount) {
    int nTries = 0;
    u32 nPick;
    do {
        nPick = Rand_Next(1) % nCount;
        if (!((1 << nPick) & uUsed)) break;
    } while (++nTries < 3);
    return nPick;
}
