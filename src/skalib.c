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
    u64    uId;                 // 0x100  must match the clip bank's when the clips live there
    s32    n108;                // 0x108
    s32    nDefault;            // 0x10C  the leaf used when a group has nothing
    s32    nTreeSize;           // 0x110
    s32    nClips;              // 0x114  entries in ppClips
    s32    nRecords;            // 0x118  36-byte clip records (a 16-character name first)
    u8*    pTree;               // 0x11C
    void** ppClips;             // 0x120  clip pointers; a clip's name is at +0xA0
    s16*   pIndex;              // 0x124  record of each clip
    void*  pFile;               // 0x128  the loaded file this library sits in
    s32    n12C;                // 0x12C
    struct ClipRecord* pRecords;  // 0x130
    u8*    pClipData;           // 0x134  the library's own clips
    struct ClipBank* pBank;     // 0x138  set in the file: the clips are in a bank instead
    u32    uFlags;              // 0x13C  1: the library carries its own clips
    s32    n140;                // 0x140
    s16    n144;                // 0x144
    s16    nClips2;             // 0x146
} AnimLib;

typedef struct ClipRecord {
    char   name[16];            // 0x00
    s16    n10;                 // 0x10
    s16    n12;                 // 0x12
    s32    n14;                 // 0x14
    s32    n18;                 // 0x18
    void*  pClip;               // 0x1C  offset into the clip data on disc, pointer once loaded
    s32    n20;                 // 0x20
} ClipRecord;

// A clip bank: clips shared by several libraries (a 0x20-byte header, the clip offsets, then the
// clips, each 16-aligned).
typedef struct ClipBank {
    u64    uId;                 // 0x00
    s32    nClips;              // 0x08
    s32    n0C;                 // 0x0C
    void** ppClips;             // 0x10
    void*  pFile;               // 0x14  the loaded file, while the bank owns it
} ClipBank;

// A loaded file as the streaming code hands it over: the data, which of the three slots it is
// for (0x20) and its size (0x24).
typedef struct LoadedFile {
    u8*    pData;               // 0x00
    u8     unk04[0x1C];
    u32    nSlot;               // 0x20
    u32    nSize;               // 0x24
} LoadedFile;

// One field of a byte-swap description: nBytes bytes made of nSize-byte values (negative: not swapped).
typedef struct SwapField {
    s32 nBytes;
    s32 nSize;
} SwapField;

void  fn_8001F08C(void** ppSrc, void** ppDst, SwapField* pFormat, int nFields, int nCount);  // byte-swap by format
void  fn_80076158(void** ppSrc, void* pDst, int nBytes, int nSize);                           // byte-swap a run
void* fn_80020DD4(void* pClip, void* pOut, int nAlign);
void* fn_80009B34(u32 uSize, u32 uFlags, u32 uAlign, const char* pFile, int nLine);  // alloc
void  fn_80009E70(void* p);                                                           // free
void  fn_80005628(void* pDst, void* pSrc, int nBytes);                                // memcpy
ClipBank* ClipBank_Get(u32 nSlot);
u32   fn_800B6564(u32 uSize);                          // ARAM alloc
void  fn_800B6594(u32 uAram);                          // ARAM free
void  fn_800B6844(void* pSrc, u32 uAram, u32 uSize);   // copy to ARAM
void  fn_800B68B4(void* pDst, u32 uAram, u32 uSize);   // copy from ARAM
void  fn_800B67EC(void);                               // wait for the ARAM copy
void  AnimLib_Free(AnimLib* pLib);
void  fn_800269E4(struct LibOverlay* pOv, int nSlot, s32 n);
void  fn_800CA9DC(int nSlot);
u32   Skalib_NextSlot(void);
void  fn_80022828(void);
int   fn_80023F7C(int nSlot);
AnimLib* AnimLib_Load(u8* pData, ClipBank* pBank);
u32   fn_8009EF90(void);
u8    fn_800C9828(int nGroup, int nStyle, int nClub, int nKey);
void  AnimLib_FreeCopies(void);
void  ClipBank_FreeAram(void);
int   strcmp(const char* pA, const char* pB);

// A library that can be layered over a slot's own (0x20 bytes).
typedef struct LibOverlay {
    AnimLib* pWork;             // 0x00  the loaded (swapped) copy
    void*  pCopy;               // 0x04  the file as it came off the disc
    u32    nSize;               // 0x08
    struct { s32 n0; s32 n4; }* p0C;  // 0x0C
    s32    n10;                 // 0x10
    s32    n14;                 // 0x14
    u8     bActive;             // 0x18
    u8     pad19[7];
} LibOverlay;

// One of the three animation slots (0x158 bytes).
typedef struct LibSlot {
    AnimLib*   pLib;            // 0x000  the loaded (swapped) library
    void*      pCopy;           // 0x004  the file as it came off the disc
    u32        nSize;           // 0x008
    LibOverlay overlays[10];    // 0x00C
    s32        nOverlays;       // 0x14C
    s32        n150;            // 0x150
    u8         unk154[4];
} LibSlot;

// The start of a clip (only the fields read here).
typedef struct Clip {
    u32    uFlags;              // 0x00  4: its data is in ARAM
    u8     unk04[0x9C];
    char   name[0x3C];          // 0xA0
    u32    uAram;               // 0xDC
} Clip;

extern ClipBank*   lbl_801C6050[3];   // the clip bank of each slot
extern AnimLib*    lbl_801C605C[3];   // the library of each slot, when its clips are in the bank
extern LibSlot     lbl_801C6068[3];
extern u32         lbl_801C6470[3];   // ARAM copy of each slot's bank file
extern u32         lbl_801C647C[3];   // its size
extern LoadedFile* lbl_801C6488[3];   // each slot's bank file, while it is in main memory
extern LoadedFile* lbl_80281CE0;      // the buffer banks are brought back from ARAM into
extern u8          lbl_80281CE4;
extern u32         lbl_80281078;      // the current slot
extern u32         lbl_80281D04[2];   // ARAM copy of each scratch area
extern u32         lbl_80281D0C[2];   // its size
extern u8          lbl_801D9908[0xC8];
extern u32         lbl_80281D18;
extern u8          lbl_80281CD8;      // double buffering: libraries alternate between slots 0 and 1
extern s32         lbl_80281CE8;      // group, style, club and key being merged
extern s32         lbl_80281CEC;
extern s32         lbl_80281CF0;
extern s32         lbl_80281CF4;
extern u8*         lbl_80281CC4;      // staging buffers (32-aligned), see Skalib_Init
extern u8*         lbl_80281CC8;
extern u8*         lbl_80281CCC;
extern u8*         lbl_80281CD0;
extern u8          lbl_801C5E2C[0x1DC];
extern u8          lbl_801C5C50[0x1DC];
extern u8          lbl_801BF9C0[0x6290];
extern u8          lbl_801B9730[0x6290];
void               fn_80005AE8(void* p, int c, int n);   // memset


extern char (*lbl_80281D14)[2][8][6][16];   // the last clip name played: [player][reaction kind][style][club]

// The clip with this name (at +0xA0 in each clip), or NULL.
void* AnimLib_FindByName(AnimLib* pLib, const char* pName) {
    int i;
    for (i = 0; i < pLib->nClips; i++) {
        if (pLib->ppClips[i] != NULL && strcmp((char*)pLib->ppClips[i] + 0xA0, pName) == 0) return pLib->ppClips[i];
    }
    return NULL;
}

// Sets everything up: no libraries or banks, the last-played table allocated and cleared, and the
// four 32-aligned staging buffers used when overlay clips are merged in.
void Skalib_Init(void) {
    int i;
    int j;

    lbl_801C605C[0] = NULL;
    lbl_801C6050[0] = NULL;
    lbl_801C605C[1] = NULL;
    lbl_801C6050[1] = NULL;
    lbl_801C605C[2] = NULL;
    lbl_801C6050[2] = NULL;
    lbl_801D9908[0] = 0;
    for (i = 0; i < 3; i++) {
        lbl_801C6068[i].n150      = 0;
        lbl_801C6068[i].nOverlays = 0;
        lbl_801C6068[i].pLib      = NULL;
        lbl_801C6068[i].pCopy     = NULL;
        for (j = 0; j < 10; j++) {
            lbl_801C6068[i].overlays[j].bActive = 0;
            lbl_801C6068[i].overlays[j].n10     = -1;
            lbl_801C6068[i].overlays[j].n14     = -1;
            lbl_801C6068[i].overlays[j].pCopy   = NULL;
            lbl_801C6068[i].overlays[j].pWork   = NULL;
        }
    }
    if (gSession.nGameType == 3 || gSession.nGameType == 10) lbl_80281D18 = 0;
    lbl_80281D14 = fn_80009B34(0x1800, 2, 0, "skalib.c", 508);
    fn_80005AE8(lbl_80281D14, 0, 0x1800);
    lbl_80281CC4 = lbl_801C5E2C;
    lbl_80281CC8 = lbl_801C5C50;
    lbl_80281CCC = lbl_801BF9C0;
    lbl_80281CD0 = lbl_801B9730;
    lbl_80281CC4 = (u8*)((((u32)lbl_80281CC4 >> 5) + 1) << 5);
    lbl_80281CC8 = (u8*)((((u32)lbl_80281CC8 >> 5) + 1) << 5);
    lbl_80281CCC = (u8*)((((u32)lbl_80281CCC >> 5) + 1) << 5);
    lbl_80281CD0 = (u8*)((((u32)lbl_80281CD0 >> 5) + 1) << 5);
}

void AnimLib_Free(AnimLib* pLib);
void ClipBank_Free(ClipBank* pBank);

// Frees every library and bank.
void Skalib_Shutdown(void) {
    int i;

    AnimLib_FreeCopies();
    fn_80009E70(lbl_80281D14);
    lbl_80281D14 = NULL;
    for (i = 0; i < 3; i++) {
        if (lbl_801C605C[i] != NULL) {
            AnimLib_Free(lbl_801C605C[i]);
            lbl_801C605C[i] = NULL;
        }
        if (lbl_801C6050[i] != NULL) {
            if (lbl_801C6050[i]->pFile == lbl_80281CE0) lbl_80281CE0 = NULL;
            ClipBank_Free(lbl_801C6050[i]);
            lbl_801C6050[i] = NULL;
        }
    }
    ClipBank_FreeAram();
}

// Frees a library: just its file when it was loaded from one (plus the clip table it allocated
// for its own clips), otherwise each part it was built from.
void AnimLib_Free(AnimLib* pLib) {
    if (pLib->pFile != NULL) {
        if (pLib->pClipData != NULL) fn_80009E70(pLib->ppClips);
        fn_80009E70(pLib->pFile);
    } else {
        if (pLib->ppClips != NULL) fn_80009E70(pLib->ppClips);
        if (pLib->pIndex != NULL) fn_80009E70(pLib->pIndex);
        if (pLib->pTree != NULL) fn_80009E70(pLib->pTree);
        if (pLib->pRecords != NULL) fn_80009E70(pLib->pRecords);
        fn_80009E70(pLib);
    }
}

// Frees a bank, and the ARAM of any of its clips that live there.
void ClipBank_Free(ClipBank* pBank) {
    u32   i;
    Clip* pClip;

    for (i = 0; i < pBank->nClips; i++) {
        pClip = pBank->ppClips[i];
        if (pClip != NULL && (pClip->uFlags & 4)) fn_800B6594(pClip->uAram);
    }
    if (pBank->pFile != NULL) {
        fn_80009E70(pBank->pFile);
    } else {
        fn_80009E70(pBank);
    }
}

// A slot's clip bank (NULL past the three slots).
ClipBank* ClipBank_Get(u32 nSlot) {
    if (nSlot >= 3) return NULL;
    return lbl_801C6050[nSlot];
}

// A leaf of the clip tree (see AnimLib); while two trees are merged its mask holds flags
// instead: 1 keep this one, 2 replace it.
typedef struct AnimLeaf {
    s16 nCount;                 // 0x0
    s16 nFirst;                 // 0x2
    u32 uMask;                  // 0x4
} AnimLeaf;

// What the merge adds up as it walks.
typedef struct MergeCtx {
    u32  n0;
    s32* pCount;                // 0x4  clips still in use
    s32  nBytes;                // 0x8  bytes of clip data still in use
} MergeCtx;

// Merge walk, sizing pass: counts the tree bytes the merged library needs (a leaf, then the
// node for this level) and decides for each leaf pair which side wins.
int AnimLib_MergeSizeCb(AnimLib* pLibA, AnimLib* pLib, AnimLeaf* pLeaf, AnimLeaf* pOver, MergeCtx* pCtx, int nLevel) {
    int bAny = 0;

    if (pLeaf != NULL || pOver != NULL) bAny = 1;
    if (bAny) pLib->nTreeSize += 8;
    switch (nLevel) {
    case 0:
        break;
    case 1:
        pLib->nTreeSize += 0x14;
        break;
    case 2:
        pLib->nTreeSize += 0xC;
        break;
    case 3:
        pLib->nTreeSize += 0x20;
        break;
    case 4:
        break;
    }
    if (pLeaf != NULL) {
        if (pOver != NULL) {
            if (fn_800C9828(lbl_80281CE8, lbl_80281CEC, lbl_80281CF0, lbl_80281CF4)) {
                pOver->uMask |= 2;
                pLeaf->uMask |= 2;
            } else if (lbl_80281CE8 == 20) {
                pOver->uMask |= 2;
            } else {
                if (!(pOver->uMask & 1)) pLib->nClips -= pLeaf->nCount;
                if (!(pOver->uMask & 1) && !(pLeaf->uMask & 1)) {
                    pLeaf->uMask |= 2;
                } else {
                    pLeaf->uMask &= ~2;
                    pLeaf->uMask |= 1;
                }
            }
        } else if (fn_800C9828(lbl_80281CE8, lbl_80281CEC, lbl_80281CF0, lbl_80281CF4)) {
            pLeaf->uMask |= 2;
        } else {
            pLeaf->uMask &= ~2;
            pLeaf->uMask |= 1;
        }
    }
    return 0;
}

// Merge walk, release pass: every clip of a replaced leaf loses a user; the ones nobody uses any
// more come off the totals.
int AnimLib_MergeReleaseCb(AnimLib* pLibA, AnimLib* pLibB, AnimLeaf* pLeafA, AnimLeaf* pLeafB, MergeCtx* pCtx, int nLevel) {
    ClipRecord* pRec;
    int         i;
    s16*        pIdx;

    if (pLeafA != NULL && (pLeafA->uMask & 2) && pLeafA->nCount != 0) {
        pIdx = pLibA->pIndex + pLeafA->nFirst;
        for (i = 0; i < pLeafA->nCount; pIdx++, i++) {
            pRec = &pLibA->pRecords[*pIdx];
            pRec->n10--;
            if (pRec->n10 == 0) {
                (*pCtx->pCount)--;
                pCtx->nBytes -= pRec->n14;
            }
        }
    }
    if (pLeafB != NULL && (pLeafB->uMask & 2) && pLeafB->nCount != 0) {
        pIdx = pLibB->pIndex + pLeafB->nFirst;
        for (i = 0; i < pLeafB->nCount; pIdx++, i++) {
            pRec = &pLibB->pRecords[*pIdx];
            pRec->n10--;
            if (pRec->n10 == 0) {
                (*pCtx->pCount)--;
                pCtx->nBytes -= pRec->n14;
            }
        }
    }
    return 0;
}

u8 Skalib_IsDoubleBuffered(void) {
    return lbl_80281CD8;
}

int fn_80023CE8(int n);

// The slot the next library loads into: with double buffering it flips between 0 and 1.
u32 Skalib_NextSlot(void) {
    if (Skalib_IsDoubleBuffered()) {
        lbl_80281078 = (lbl_80281078 == 0);
    } else if (fn_80023CE8(0)) {
        lbl_80281078 = 0;
    } else {
        lbl_80281078 = 1;
    }
    return lbl_80281078;
}

u32 Skalib_CurSlot(void) {
    return lbl_80281078;
}

// The scratch area for slot n, uSize bytes: while only one of the first two slots has a bank,
// it is carved out of that bank; otherwise it is the slot's own bank.
static inline u8* Skalib_Scratch(int n, u32 uSize) {
    if (lbl_801C6050[0] != NULL && lbl_801C6050[1] == NULL) return (u8*)lbl_801C6050[0] + n * uSize;
    if (lbl_801C6050[0] == NULL && lbl_801C6050[1] != NULL) return (u8*)lbl_801C6050[1] + n * uSize;
    return (u8*)lbl_801C6050[n];
}

// Copies slot n's scratch area to ARAM.
u8* Skalib_ScratchToAram(int n) {
    u32 uSize = fn_8009EF90();
    u8* p;
    if (lbl_801C6050[0] != NULL && lbl_801C6050[1] == NULL) {
        p = (u8*)lbl_801C6050[0] + n * uSize;
    } else if (lbl_801C6050[0] == NULL && lbl_801C6050[1] != NULL) {
        p = (u8*)lbl_801C6050[1] + n * uSize;
    } else {
        p = (u8*)lbl_801C6050[n];
    }
    lbl_80281D0C[n] = uSize;
    if (lbl_80281D04[n] == 0) lbl_80281D04[n] = fn_800B6564(lbl_80281D0C[n]);
    fn_800B6844(p, lbl_80281D04[n], lbl_80281D0C[n]);
    fn_800B67EC();
    return p;
}

// Brings slot n's scratch area back from ARAM and frees the ARAM.
void Skalib_ScratchFromAram(int n) {
    u32 uSize = fn_8009EF90();
    fn_800B68B4(Skalib_Scratch(n, uSize), lbl_80281D04[n], lbl_80281D0C[n]);
    fn_800B67EC();
    if (lbl_80281D04[n] != 0) {
        fn_800B6594(lbl_80281D04[n]);
        lbl_80281D04[n] = 0;
    }
}

// Frees the working copies of every slot's libraries (only the current slot's while
// lbl_80281CE4 is set).
void AnimLib_FreeWorkCopies(void) {
    LibSlot*    pSlot;
    int         j;
    int         k;
    u32         i;
    LibOverlay* pOv;

    pSlot = lbl_801C6068;
    for (i = 0; i < 3; i++, pSlot++) {
        if (lbl_80281CE4 != 0 && i != lbl_80281078) continue;
        if (pSlot->nOverlays != 0) {
            for (k = 0; k < pSlot->nOverlays; k++) {
            }
            for (j = 0; j < pSlot->nOverlays; j++) {
                pOv = &pSlot->overlays[j];
                AnimLib_Free(pOv->pWork);
                pOv->pWork = NULL;
                pOv->n10   = -1;
            }
            pSlot->n150 = 0;
        }
        if (pSlot->pLib != NULL) AnimLib_Free(pSlot->pLib);
        pSlot->pLib = NULL;
    }
}

// Frees the pristine copies of every slot's libraries.
void AnimLib_FreeCopies(void) {
    LibSlot*    pSlot;
    LibOverlay* pOv;
    u32         i;
    int         j;

    pSlot = lbl_801C6068;
    for (i = 0; i < 3; i++, pSlot++) {
        if (pSlot->nOverlays != 0) {
            for (j = 0; j < pSlot->nOverlays; j++) {
                pOv = &pSlot->overlays[j];
                fn_80009E70(pOv->pCopy);
                pOv->pCopy = NULL;
            }
        }
        pSlot->nOverlays = 0;
        if (pSlot->pCopy != NULL) fn_80009E70(pSlot->pCopy);
        pSlot->pCopy = NULL;
    }
}

// Re-applies a slot's active overlay libraries.
void AnimLib_ApplyOverlays(int nSlot) {
    LibSlot*    pSlot = &lbl_801C6068[nSlot];
    LibOverlay* pOv;
    int         n     = pSlot->nOverlays;
    int         i;

    if (n != 0) {
        pOv = pSlot->overlays;
        for (i = 0; i < n; pOv++, i++) {
            if (pOv->bActive) fn_800269E4(pOv, nSlot, pOv->p0C->n4);
        }
    }
}

void fn_80025478(void) {
    u32 i;
    int nTotal = 0;
    fn_80022828();
    for (i = 0; i < 3; i++) nTotal += fn_80023F7C(i);
}

// Rebuilds the current slot's libraries from their pristine copies (they are swapped in place
// when loaded, so a reload starts from the copy), first freeing the bank clips kept in ARAM.
void AnimLib_ReloadSlot(void) {
    u32         nSlot = Skalib_NextSlot();
    u32         i;
    LibSlot*    pSlot;
    int         j;
    LibOverlay* pOv;
    Clip*       pClip;
    AnimLib*    pLib;

    for (i = 0; i < lbl_801C6050[nSlot]->nClips; i++) {
        pClip = lbl_801C6050[nSlot]->ppClips[i];
        if (pClip != NULL && (pClip->uFlags & 4)) fn_800B6594(pClip->uAram);
    }
    pSlot = &lbl_801C6068[nSlot];
    if (pSlot->nOverlays != 0) {
        pSlot->pLib = fn_80009B34(pSlot->nSize, 1, 0x40, "skalib.c", 3193);
        fn_80005628(pSlot->pLib, pSlot->pCopy, pSlot->nSize);
        pLib = AnimLib_Load((u8*)pSlot->pLib, ClipBank_Get(nSlot));
        pLib->pFile = pSlot->pLib;
        for (j = 0; j < pSlot->nOverlays; j++) {
            pOv        = &pSlot->overlays[j];
            pOv->pWork = fn_80009B34(pOv->nSize, 1, 0x40, "skalib.c", 3207);
            fn_80005628(pOv->pWork, pOv->pCopy, pOv->nSize);
            AnimLib_Load((u8*)pOv->pWork, ClipBank_Get(nSlot));
            pOv->n10 = pOv->n14 + 3;
        }
    }
    AnimLib_ApplyOverlays(nSlot);
    fn_800CA9DC(nSlot);
    fn_80023F7C(nSlot);
}

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
void* fn_800CAA7C(int nPlayer, int nGroup, int nStyle, int nClub);

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
            return AnimLib_FindByName(pLib, pName);
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

// Swaps a clip bank's header.
void ClipBank_SwapHeader(void* p) {
    SwapField fmt[5] = {{8, -8}, {4, 4}, {4, 4}, {4, 4}, {4, 4}};
    void*     pSrc = p;
    void*     pDst = p;
    fn_8001F08C(&pSrc, &pDst, fmt, 5, 1);
}

// Swap one node of the clip tree (see AnimLib).
void AnimLib_SwapGroupNode(void* pSrc, void* pDst) {
    SwapField fmt[3] = {{2, 2}, {0x10, 2}, {2, 1}};
    fn_8001F08C(&pSrc, &pDst, fmt, 3, 1);
}

void AnimLib_SwapStyleNode(void* pSrc, void* pDst) {
    SwapField fmt[2] = {{12, 2}, {2, 1}};
    fn_8001F08C(&pSrc, &pDst, fmt, 2, 1);
}

void AnimLib_SwapClubNode(void* pSrc, void* pDst) {
    SwapField fmt[5] = {{2, 2}, {2, 2}, {0x16, 2}, {2, 2}, {4, 4}};
    fn_8001F08C(&pSrc, &pDst, fmt, 5, 1);
}

void AnimLib_SwapLeaf(void* pSrc, void* pDst) {
    SwapField fmt[3] = {{2, 2}, {2, 2}, {4, 4}};
    fn_8001F08C(&pSrc, &pDst, fmt, 3, 1);
}

// Swaps the whole clip tree, walking it the way AnimLib_Find does.
void AnimLib_SwapTree(AnimLib* pLib, u8* pSrc, u8* pDst) {
    int  nGroup;
    int  nStyle;
    int  nClub;
    int  nKey;
    s32  nOff;
    s16* pNode;
    s16* pStyle;
    s16* pClub;

    if (pLib->nDefault >= 0) AnimLib_SwapLeaf(pSrc + pLib->nDefault, pDst + pLib->nDefault);
    for (nGroup = 0; nGroup < 21; nGroup++) {
        nOff = pLib->groups[nGroup];
        if (nOff < 0) continue;
        pNode = (s16*)(pDst + nOff);
        AnimLib_SwapGroupNode(pSrc + nOff, pNode);
        if (pNode[0] >= 0) AnimLib_SwapLeaf(pSrc + pNode[0], pDst + pNode[0]);
        for (nStyle = 0; nStyle < 8; nStyle++) {
            nOff = pNode[1 + nStyle];
            if (nOff <= 0) continue;
            pStyle = (s16*)(pDst + nOff);
            AnimLib_SwapStyleNode(pSrc + nOff, pStyle);
            for (nClub = 0; nClub < 6; nClub++) {
                nOff = pStyle[nClub];
                if (nOff <= 0) continue;
                pClub = (s16*)(pDst + nOff);
                AnimLib_SwapClubNode(pSrc + nOff, pClub);
                if (pClub[1] >= 0) AnimLib_SwapLeaf(pSrc + pClub[1], pDst + pClub[1]);
                for (nKey = 0; nKey < 11; nKey++) {
                    nOff = pClub[2 + nKey];
                    if (nOff > 0) AnimLib_SwapLeaf(pSrc + nOff, pDst + nOff);
                }
            }
        }
    }
}

// Sets up an animation library loaded at pData (swapping it in place). Its clips come either from
// the clip bank pBank, or (flag 1) from the library itself; a library whose id does not match the
// bank's plays the bank's first clip for everything. NULL when it needs a bank and there is none.
AnimLib* AnimLib_Load(u8* pData, ClipBank* pBank) {


    SwapField hdrFmt[19] = {{0x100, 4}, {8, -8}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, -4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {2, 2}, {2, 2}};
    SwapField recFmt[7]  = {{0x10, -1}, {2, 2}, {2, 2}, {4, 4}, {4, 4}, {4, 4}, {4, 4}};
    void*     pDst;
    void*     pSrc;
    u32       uPad;
    AnimLib*  pLib;
    u8*       p;
    int       i;

    uPad = 16 - ((u32)pData & 15);
    if (uPad == 16) uPad = 0;
    pLib        = (AnimLib*)(pData + uPad);
    pLib->pFile = pData;
    p           = (u8*)pLib + sizeof(AnimLib);
    pDst = pSrc = pLib;
    fn_8001F08C(&pSrc, &pDst, hdrFmt, 19, 1);
    if (pLib->pBank != NULL) {
        if (pBank == NULL) return NULL;
        pLib->pClipData = NULL;
        pLib->ppClips   = (void**)p;
        p += pLib->nClips * 4;
        pLib->nClips2   = pLib->nClips;
        pLib->pTree = p;
        pDst = pSrc = pLib->ppClips;
        fn_80076158(&pSrc, pDst, pLib->nClips * 4, 4);
        if (pLib->uId != pBank->uId) {
            for (i = 0; i < pLib->nClips; i++) pLib->ppClips[i] = pBank->ppClips[0];
        } else {
            for (i = 0; i < pLib->nClips; i++) pLib->ppClips[i] = pBank->ppClips[(u32)pLib->ppClips[i]];
        }
        pLib->pBank = pBank;
    } else {
        pLib->pIndex  = (s16*)p;
        p += pLib->nClips * 2;
        pLib->nClips2 = pLib->nClips;
        if ((u32)p & 15) p = (u8*)((((u32)p >> 4) + 1) << 4);
        pLib->pRecords = (ClipRecord*)p;
        p += pLib->nRecords * sizeof(ClipRecord);
        pLib->pTree = p;
        p += pLib->nTreeSize;
        pDst = pSrc = pLib->pIndex;
        fn_80076158(&pSrc, pDst, pLib->nClips * 2, 2);
        pDst = pSrc = pLib->pRecords;
        fn_8001F08C(&pSrc, &pDst, recFmt, 7, pLib->nRecords);
        if (pLib->uFlags & 1) {
            if ((u32)p & 15) p = (u8*)((((u32)p >> 4) + 1) << 4);
            pLib->pClipData = p;
            pLib->ppClips   = fn_80009B34(pLib->nClips * 4, 2, 0x40, "skalib.c", 4164);
            for (i = 0; i < pLib->nRecords; i++) {
                pLib->pRecords[i].pClip = fn_80020DD4(pLib->pClipData + (u32)pLib->pRecords[i].pClip, NULL, 16);
            }
            for (i = 0; i < pLib->nClips; i++) pLib->ppClips[i] = pLib->pRecords[pLib->pIndex[i]].pClip;
        } else {
            pLib->pClipData = NULL;
            pLib->ppClips   = NULL;
        }
        pLib->pBank = NULL;
    }
    pDst = pSrc = pLib->pTree;
    AnimLib_SwapTree(pLib, pSrc, pDst);
    return pLib;
}

// Sets up a clip bank loaded at pData, aligned to uAlign.
ClipBank* ClipBank_Load(u8* pFile, u32 uAlign) {
    u32       uUnused;
    void*     pSrc;
    u32       uPad;
    ClipBank* pBank;
    int       i;
    u8*       pData;

    pData = pFile;
    uPad = uAlign - ((u32)pData & (uAlign - 1));
    if (uPad == uAlign) uPad = 0;
    pBank = (ClipBank*)(pData + uPad);
    ClipBank_SwapHeader(pBank);
    pBank->pFile   = NULL;
    pData += 0x20;
    pBank->ppClips = (void**)pData;
    pSrc = pBank->ppClips;
    fn_80076158(&pSrc, pBank->ppClips, pBank->nClips * 4, 4);
    pData += pBank->nClips * 4;
    uPad = 16 - ((u32)pData & 15);
    if (uPad == 16) uPad = 0;
    pData += uPad;
    for (i = 0; i < pBank->nClips; i++) {
        pBank->ppClips[i] = fn_80020DD4(pData + (u32)pBank->ppClips[i], &uUnused, 16);
    }
    return pBank;
}

// A slot's animation library has loaded: keep a copy and set it up against the slot's bank.
void AnimLib_OnLoaded(LoadedFile* pFile) {
    u8       bFree = 1;
    u32      nSlot = pFile->nSlot;
    AnimLib* pLib;

    if (nSlot < 3 && lbl_801C605C[nSlot] == NULL) {
        lbl_801C6068[nSlot].pCopy = fn_80009B34(pFile->nSize, 2, 0x40, "skalib.c", 4520);
        fn_80005628(lbl_801C6068[nSlot].pCopy, pFile->pData, pFile->nSize);
        lbl_801C6068[nSlot].nSize = pFile->nSize;
        pLib = AnimLib_Load(pFile->pData, ClipBank_Get(nSlot));
        pLib->pFile = pFile;
        if (pLib->pBank != NULL) {
            lbl_801C605C[nSlot] = pLib;
        } else {
            lbl_801C6068[nSlot].pLib = pLib;
        }
        bFree = 0;
    }
    if (bFree) fn_80009E70(pFile);
}

void ClipBank_Stash(int nSlot);

// A slot's clip bank file has loaded: park it in ARAM.
void ClipBank_OnLoaded(LoadedFile* pFile) {
    u32 nSlot = pFile->nSlot;
    lbl_801C6488[nSlot] = pFile;
    ClipBank_Stash(nSlot);
}

// Makes a loaded clip bank file the slot's bank.
void ClipBank_Install(LoadedFile* pFile) {
    u8  bFree = 1;
    u32 nSlot = pFile->nSlot;

    if (nSlot < 3 && lbl_801C6050[nSlot] == NULL) {
        lbl_801C6050[nSlot]        = ClipBank_Load(pFile->pData, 16);
        bFree                      = 0;
        lbl_801C6050[nSlot]->pFile = pFile;
    }
    if (bFree) fn_80009E70(pFile);
}

// Forgets a slot's bank.
void ClipBank_Release(int nSlot) {
    if (lbl_801C6050[nSlot] != NULL) {
        if (lbl_801C6050[nSlot]->pFile != NULL) {
            lbl_801C6050[nSlot]->pFile = NULL;
            lbl_801C6050[nSlot]        = NULL;
            lbl_801C6488[nSlot]        = NULL;
        } else {
            lbl_801C6050[nSlot] = NULL;
        }
    }
    if (lbl_801C6488[nSlot] != NULL) lbl_801C6488[nSlot] = NULL;
}

// Copies a slot's bank file to ARAM and frees it; the first time, allocates the buffer it is
// brought back into.
void ClipBank_Stash(int nSlot) {
    if (lbl_801C6488[nSlot] != NULL) {
        lbl_801C647C[nSlot] = ((lbl_801C6488[nSlot]->nSize + 0x80) / 32 + 1) * 32;
        if (lbl_801C6470[nSlot] == 0) lbl_801C6470[nSlot] = fn_800B6564(lbl_801C647C[nSlot]);
        fn_800B6844(lbl_801C6488[nSlot], lbl_801C6470[nSlot], lbl_801C647C[nSlot]);
        fn_800B67EC();
        if (lbl_801C6488[nSlot] != lbl_80281CE0) fn_80009E70(lbl_801C6488[nSlot]);
        lbl_801C6488[nSlot] = NULL;
        if (nSlot == 0 && lbl_80281CE0 == NULL) {
            lbl_80281CE0 = fn_80009B34(lbl_801C647C[nSlot], 2, 0x20, "skalib.c", 4671);
        }
    }
}

// Brings a slot's bank back from ARAM and installs it.
void ClipBank_Restore(int nSlot) {
    if (lbl_801C6488[nSlot] == NULL) {
        lbl_801C6488[nSlot] = lbl_80281CE0;
        fn_800B68B4(lbl_801C6488[nSlot], lbl_801C6470[nSlot], lbl_801C647C[nSlot]);
        fn_800B67EC();
        lbl_801C6488[nSlot]->pData = (u8*)lbl_801C6488[nSlot] + 0x80;
        ClipBank_Install(lbl_801C6488[nSlot]);
    }
}

// Frees the banks' ARAM and the restore buffer.
void ClipBank_FreeAram(void) {
    int i;
    for (i = 0; i < 3; i++) {
        if (lbl_801C6470[i] != 0) {
            fn_800B6594(lbl_801C6470[i]);
            lbl_801C6470[i] = 0;
        }
    }
    if (lbl_80281CE0 != NULL) {
        fn_80009E70(lbl_80281CE0);
        lbl_80281CE0 = NULL;
    }
}

int UStream_RegisterHandler(u32 uType, void (*pfn)(LoadedFile*));
int UStream_UnregisterHandler(u32 uType);

// Hooks the loaders up to the file streamer: 'SAL ' animation libraries and 'BNK ' clip banks.
void Skalib_Register(void) {
    UStream_RegisterHandler('SAL ', AnimLib_OnLoaded);
    UStream_RegisterHandler('BNK ', ClipBank_OnLoaded);
}

void Skalib_Unregister(void) {
    UStream_UnregisterHandler('SAL ');
    UStream_UnregisterHandler('BNK ');
}
