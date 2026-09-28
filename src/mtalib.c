// mtalib.c (EA's name, from its asserts; TW2003 has the file too, and TW2005 puts it at
// c:/dev/Golf_2005/Tigercode/Code/Golf/Animation/mtalib.c): the morph animation libraries
// (MtaLib; MTA and the struct names are our reading). A library has records, each for one block of
// a format 1 pose buffer (SkelPose1), and each record entries: tracks of a byte per frame that
// drive one morph's weight (MtaLib_ApplyToPose), played on a character's second animation player
// through its morph blend tree (AnimBlender.c). The libraries come in 'MAL ' banks from the
// stream files (two slots, gMtaLibBanks), three groups each, one picked at random
// (Character_GetRandomMtaLib); a clip can also carry its own (Clip.pMtaLib, MtaLib_Link). The file
// also holds EA's bone name table (gSkelBoneNames). MtaLib_SwapAndLink and ByteSwap_Records, at the
// end of char.c right before this file, may belong here.

#include "charstate.h"

static MalBank* gMtaLibBanks[2];        // the 'MAL ' bank of each slot (MtaLib_OnLoaded)
static int gMtaLibBytes;                // bytes the banks have allocated (never read)
static u8 lbl_801B9668[200];            // only MtaLib_InitModule touches it (an empty string)

// The bone names by bone id (character.h): 0x15 "rwrst", 0x36-0x3A the right leg ("rhip" to
// "rtoe"), 0x44-0x48 the left leg, 0x52 "IGdriver" (the club bone), 0x53 "clubhead".
char* gSkelBoneNames[90] = {
    "", "root", "ctrgrav", "waist", "s1", "s2", "s3", "s4", "s5", "neck", "head", "tail1", "tail2",
    "tail3", "tail4", "skull", "rcolr", "rshld", "rbictwst", "relb", "r4rm", "rwrst", "rf1", "rf2",
    "rf3", "ri1", "ri2", "ri3", "rt1", "rt2", "rt3", "rslvBjnt", "rslvFjnt", "rslvHjnt", "rshlddef",
    "lcolr", "lshld", "lbictwst", "lelb", "l4rm", "lwrst", "lf1", "lf2", "lf3", "li1", "li2", "li3",
    "lt1", "lt2", "lt3", "lslvBjnt", "lslvFjnt", "lslvHjnt", "lshlddef", "rhip", "rthitwst",
    "rknee", "rankl", "rtoe", "ropnt1", "ropnt2", "ropnt3", "rbpnt1", "rbpnt2", "rbpnt3", "rfpnt1",
    "rfpnt2", "rfpnt3", "lhip", "lthitwst", "lknee", "lankl", "ltoe", "lopnt1", "lopnt2", "lopnt3",
    "lbpnt1", "lbpnt2", "lbpnt3", "lfpnt1", "lfpnt2", "lfpnt3", "IGdriver", "clubhead", "GBall1",
    "tail5", "tail6", "rboob", "lboob", NULL,
};

void MtaLib_FreeBank(MalBank* pBank);

// The entry's values at the two frames around fTime (clamped to its last frame) into *pfA and *pfB;
// returns how far fTime is from the first frame to the second (0 when clamped).
f32 MtaLib_GetEntryFrames(MtaEntry* pEntry, f32* pfA, f32* pfB, f32 fTime) {
    f32 fFrame;
    int nLast;
    int nA;
    int nB;
    f32 fT;
    f32 fRange;

    if (fTime < 0.0f) {
        fTime = 0.0f;
    }
    fFrame = fTime / pEntry->fFrameTime;
    nLast = pEntry->nLastFrame;
    nA = (int)fFrame;
    nB = nA + 1;
    fT = fFrame - (f32)nA;
    if (nA > nLast) {
        fT = 0.0f;
        nB = nLast;
        nA = nLast;
    } else if (nB > nLast) {
        nB = nLast;
        fT = 0.0f;
    }
    fRange = pEntry->fHi - pEntry->fLo;
    if (fRange != 0.0f) {
        *pfA = fRange * (pEntry->pData[nA] / 256.0f) + pEntry->fLo;
        *pfB = fRange * (pEntry->pData[nB] / 256.0f) + pEntry->fLo;
    } else {
        *pfA = pEntry->fLo;
        *pfB = pEntry->fLo;
    }
    return fT;
}

// Sets morph nMorph's weight in pBlock from the entry at fTime and marks the morph set.
void MtaLib_ApplyEntry(MtaEntry* pEntry, SkelPoseBlock* pBlock, int nMorph, SkelPose1* pPose, f32 fTime,
                 f32 fWeight) {
    f32 fA;
    f32 fB;
    f32 fT;

    // pPose and fWeight are unused: MtaLib_ApplyToPose passes them
    fT = MtaLib_GetEntryFrames(pEntry, &fA, &fB, fTime);
    pBlock->af8[nMorph] = fT * (fB - fA) + fA;
    BitArray_SetBit(pBlock->aBits, nMorph);
}

// Sets every morph weight the library pLib drives in pPose at fTime: each entry with a morph
// (nMorph >= 0) sets that morph's weight in its record's block of pPose. SKABlender_Update calls it
// for a format 1 channel; returns 0.
int MtaLib_ApplyToPose(void* pUnused, MtaLib* pLib, SkelPose1* pPose, f32 fTime) {
    int i;
    int j;
    MtaRecord* pRecord;
    SkelPoseBlock* pBlock;

    // pUnused: the caller (animblender.c) passes it; nothing here reads it
    for (i = 0; i < pLib->nRecords; i++) {
        pRecord = &pLib->pRecords[i];
        pBlock = &pPose->aBlocks[pRecord->nBlock];
        for (j = 0; j < pRecord->nEntries; j++) {
            if (pRecord->pEntries[j].nMorph >= 0) {
                MtaLib_ApplyEntry(&pRecord->pEntries[j], pBlock, pRecord->pEntries[j].nMorph, pPose, fTime,
                                  1.0f);
            }
        }
    }
    return 0;
}

void MtaLib_Free(void* pItem) {
    StaticMem_Free(pItem);
}

// Links a library that is already in the machine's byte order (MtaLib_SwapAndLink without the
// swap): the records after the header, each record's entries after those, then each entry's data
// (4-byte aligned). ska_shared.c does this for a clip's own library (Clip.pMtaLib).
void MtaLib_Link(MtaLib* pLib) {
    int i;
    int nPad;
    MtaRecord* pRecord;
    MtaEntry* pEntry;
    int nOffset;
    int j;
    MtaRecord* pRecord2;

    pLib->pRecords = (MtaRecord*)(pLib + 1);
    nOffset = sizeof(MtaLib) + pLib->nRecords * sizeof(MtaRecord);
    for (i = 0; i < pLib->nRecords; i++) {
        pRecord = &pLib->pRecords[i];
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
}

// Starts with no banks: both slots empty, no bytes allocated, lbl_801B9668 an empty string.
void MtaLib_InitModule(void) {
    gMtaLibBanks[0] = NULL;
    gMtaLibBanks[1] = NULL;
    lbl_801B9668[0] = 0;
    gMtaLibBytes = 0;
}

// Frees both banks and clears the byte count.
void MtaLib_CloseModule(void) {
    int i;

    for (i = 0; i < 2; i++) {
        if (gMtaLibBanks[i] != NULL) {
            MtaLib_FreeBank(gMtaLibBanks[i]);
            gMtaLibBanks[i] = NULL;
        }
    }
    gMtaLibBytes = 0;
}

// Frees a bank: the libraries of its three groups, each group's item array, then the bank.
void MtaLib_FreeBank(MalBank* pBank) {
    int i;
    int j;

    for (i = 0; i < 3; i++) {
        if (pBank->aGroup[i].nNum != 0) {
            for (j = 0; j < pBank->aGroup[i].nNum; j++) {
                MtaLib_Free(pBank->aGroup[i].apItem[j]);
            }
            StaticMem_Free(pBank->aGroup[i].apItem);
        }
    }
    StaticMem_Free(pBank);
}

// The bank of slot nBank, NULL outside slots 0 and 1.
// EA bug: nBank is only range-checked; bank 0 is returned either way.
MalBank* MtaLib_GetBank(int nBank) {
    if (nBank < 0 || nBank >= 2) return NULL;
    return gMtaLibBanks[0];
}

// The libraries of the bank's group nGroup; *pnNum gets how many. n is not used.
void** MtaLib_GetGroup(MalBank* pBank, int nGroup, int* pnNum, int n) {
    MalGroup* pGroup;

    pGroup = &pBank->aGroup[nGroup];
    *pnNum = pGroup->nNum;
    return pGroup->apItem;
}

// A random library of the bank's group nGroup, or NULL when the group is empty. n is passed on to
// MtaLib_GetGroup, which does not use it.
void* MtaLib_GetRandom(MalBank* pBank, int nGroup, int n) {
    int nNum;
    void** apItem;

    apItem = MtaLib_GetGroup(pBank, nGroup, &nNum, n);
    if (nNum != 0) {
        return apItem[Misc_RandFunc(1) % nNum];
    }
    return NULL;
}

// Builds a bank from a 'MAL ' object's data (little-endian): its group count, then per group its
// index, its item count and its items, each a library (MtaLib) starting on a 16-byte boundary.
// Each library is copied out, byte-swapped and linked.
MalBank* MtaLib_LoadBank(u8* pData) {
    SwapField aHeader[10] = {
        { 16, 1 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 2, 2 }, { 6, -1 },
        { 4, 4 },
    };
    s32 nSize;
    s32 nGroup;
    void* pA;
    void* pB;
    int i;
    int j;
    MalGroup* pGroup;
    MalBank* pBank;
    MtaLib* pLib;

    pBank = StaticMem_Alloc(sizeof(MalBank), 2, 0x40, "mtalib.c", 474);
    gMtaLibBytes += sizeof(MalBank);
    BYTESWAP_SWAPDATA(&pData, (u8*)&pBank->nNumGroups, 4, 4);
    if ((uptr)pData & 0xF) {
        pData = (u8*)(((uptr)pData & ~0xF) + 0x10);
    }
    for (i = 0; i < pBank->nNumGroups; i++) {
        BYTESWAP_SWAPDATA(&pData, (u8*)&nGroup, 4, 4);
        pGroup = &pBank->aGroup[nGroup];
        BYTESWAP_SWAPDATA(&pData, (u8*)&pGroup->nNum, 4, 4);
        if (pGroup->nNum != 0) {
            pGroup->apItem = StaticMem_Alloc(pGroup->nNum * 4, 2, 0x40, "mtalib.c", 490);
            for (j = 0; j < pGroup->nNum; j++) {
                if ((uptr)pData & 0xF) {
                    pData = (u8*)(((uptr)pData & ~0xF) + 0x10);
                }
                // swap the header in place to read the library's size, then swap it back
                pB = pA = pData;
                ByteSwap_Records(&pA, &pB, aHeader, 10, 1);
                pLib = (MtaLib*)pData;
                pGroup->apItem[j] = StaticMem_Alloc(pLib->nBytes, 2, 0x40, "mtalib.c", 501);
                gMtaLibBytes += pLib->nBytes;
                nSize = pLib->nBytes;
                pB = pA = pData;
                ByteSwap_Records(&pA, &pB, aHeader, 10, 1);
                memcpy(pGroup->apItem[j], pData, nSize);
                pGroup->apItem[j] = MtaLib_SwapAndLink(pGroup->apItem[j], &nSize);
                pData += nSize;
            }
        } else {
            pGroup->apItem = NULL;
        }
    }
    return pBank;
}

// The 'MAL ' stream handler: the object's id is the bank slot; a slot already filled is kept.
void MtaLib_OnLoaded(UStreamObject* pObject) {
    u32 uSlot;

    uSlot = pObject->uId;
    if (uSlot < 2 && gMtaLibBanks[uSlot] == NULL) {
        gMtaLibBanks[uSlot] = MtaLib_LoadBank(pObject->pData);
    }
    StaticMem_Free(pObject);
}

void MtaLib_Register(void) {
    Stream_RegisterLoadChunkCallback('MAL ', MtaLib_OnLoaded);
}

void MtaLib_Unregister(void) {
    Stream_UnregisterLoadChunkCallback('MAL ');
}
