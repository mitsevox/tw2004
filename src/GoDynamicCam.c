// GoDynamicCam.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the dynamic
// cameras. Loads the camera shots, sequences and shot choices from the camera files into
// lbl_80281D88's tables, and picks the sequence and shot that fit a golfer's situation (club,
// shot kind, course and hole, game mode).

#include "golfer.h"
#include "game.h"
#include "dyncam.h"
#include "frontend/fe.h"
#include "endian.h"

DynCamTables* lbl_80281D88;

u8   BitArray_TestBit(u32* pBits, int nBit);         // the bit is set
void DynamicCam_CopySequenceData(u8* pSrc, u8* pDst, int nCount);
void DynamicCam_CopyScriptData(u8* pSrc, CamShot* pDst, u32 nCount);
void DynamicCam_CopyAnimPairData(u8* pSrc, DynCamSet* pDst, u32 nCount);
void DynamicCam_ParseCameraViews(int nSize);
void DynamicCam_ParseCameraViewsFE(int nSize);
void DynamicCam_ParseCameraSeqs(int nSequences);
void DynamicCam_ParseNextSeqs(void);
u8   DynamicCamSearchForPairedSequence(char* szName, CamSequence** ppSeq, CamShot** ppShot);
void fn_8003DC30(f32* pA, f32* pB, f32* pOut);         // a + b
void fn_8003DC54(f32* pA, f32* pB, f32* pOut);
void Quat_RotateVector(f32* pTurn, f32* pVec, f32* pOut);     // the vector turned by it
void DynamicCam_ParseAnimPairs(int nSize);
u8   fn_8003D0EC(CamSequence* pSequence, int nKind);
u8   fn_8003D240(CamShot* pShot, int nKind);
u8   fn_8003D294(CamShot* pShot);
void DynamicCam_LoadFilesFromDisk(void);
void DynamicCam_LoadFilesFromDiskFE(void);
void DynamicCam_LoadCAMSfromStream(UStreamObject* pObject);
void DynamicCam_LoadCAMVfromStream(UStreamObject* pObject);
void DynamicCam_LoadCAMVfromStreamFE(UStreamObject* pObject);
void DynamicCam_LoadCAMAfromStream(UStreamObject* pObject);
void DynamicCam_GetLocation(int nKind, int nPlayer, f32* pOut, CamScript* pScript, CamShot* pShot, f32* pCam,
                            f32* pSub);
void Character_GetBonePos(Character* pChar, int nBone, f32* pPos);   // char.c: a bone's position
void fn_8003D324(f32* pPos, f32* pDir, CamScript* pScript, CamShot* pShot, int nPlayer, f32 fSide, f32 fY);
void DynamicCam_AddHeightOffset(f32* pPos, CamScript* pScript, CamShot* pShot, int nPlayer, f32 fY);
void DynamicCam_TrackPercent(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pCam,
                             f32* pSub);
void DynamicCam_TrackOffset(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pCam, f32* pSub);
void DynamicCam_TrackBallVelocityLag(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pCam,
                                     f32* pSub,
                 f32 f);
void DynamicCam_TrackBallVelocityTight(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pCam,
                                       f32* pSub);
void DynamicCam_TrackFixed(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pCam, f32* pSub);
f32  fn_8003DBA8(f32 f);
void fn_8003D810(f32* pDir, f32* pA, f32* pB);
u8   DynamicCam_IsDefualtSeq(CamSequence* pSequence, int nKind);
s32  DynamicCam_MaterialToCameraLie(u32 n, int nPlayer);
s32  DynamicCam_BallLieToCameraLie(int n, int nPlayer);
u8   DynamicCam_MatchSeqClub(CamSequence* pSequence, int nPlayer);
u8   DynamicCam_MatchHeightDiff(CamSequence* pSequence, int nPlayer, f32 f);
u8   fn_8003CD9C(CamSequence* pSequence, int nPlayer, u8 b);
u8   fn_8003CEEC(CamSequence* pSequence, int nPlayer);
u8   fn_8003D00C(CamSequence* pSequence);
u8   fn_8003D054(CamSequence* pSequence);
u8   fn_8003D0A0(int nMask, int nBit);
u8   fn_8003D140(CamSequence* pSequence);
u8   fn_8003D0BC(CamSequence* pSequence, int nPlayer, f32 f);
u8   DynamicCam_CanUseScriptOnThisHole(CamChoice* pChoice);
u8   DynamicCam_CanUseScriptOnThisModel(CamChoice* pChoice, int nPlayer);
void fn_8003DAC8(CamShot* pShot, int nPlayer, f32* pA, f32* pB);
f32  CamScript_GetBallToPinPercent(int nPlayer, CamScript* pScript);     // gocamscripts.c

// Registers the stream handlers of a round's camera files: 'CAMS' the sequences
// (DynamicCam_LoadCAMSfromStream), 'CAMV' the shots (DynamicCam_LoadCAMVfromStream), 'CAMA' the
// anim pairs (DynamicCam_LoadCAMAfromStream). Called with the other in-game stream clients
// (streammanagerhole.c).
void DynamicCam_RegisterStreamClients(void) {
    Stream_RegisterLoadChunkCallback('CAMS', DynamicCam_LoadCAMSfromStream);
    Stream_RegisterLoadChunkCallback('CAMV', DynamicCam_LoadCAMVfromStream);
    Stream_RegisterLoadChunkCallback('CAMA', DynamicCam_LoadCAMAfromStream);
}

// Unregisters the three handlers DynamicCam_RegisterStreamClients set, then calls
// DynamicCam_LoadFilesFromDisk (empty in this build).
void DynamicCam_UnRegisterStreamClients(void) {
    Stream_UnregisterLoadChunkCallback('CAMS');
    Stream_UnregisterLoadChunkCallback('CAMV');
    Stream_UnregisterLoadChunkCallback('CAMA');
    DynamicCam_LoadFilesFromDisk();
}

// The front end's version: registers only the 'CAMV' shot file, handled by
// DynamicCam_LoadCAMVfromStreamFE. Called with the front end's other stream clients
// (streammanagerhole.c).
void DynamicCam_RegisterStreamClientsFE(void) {
    Stream_RegisterLoadChunkCallback('CAMV', DynamicCam_LoadCAMVfromStreamFE);
}

// Unregisters the front end's 'CAMV' handler, then calls DynamicCam_LoadFilesFromDiskFE (empty in
// this build).
void DynamicCam_UnRegisterStreamClientsFE(void) {
    Stream_UnregisterLoadChunkCallback('CAMV');
    DynamicCam_LoadFilesFromDiskFE();
}

// Empty in this build; DynamicCam_UnRegisterStreamClients calls it. TW07 has
// DynamicCam_LoadFilesFromDisk in this place, a load of the camera files from disk instead of the
// stream.
void DynamicCam_LoadFilesFromDisk(void) {
}

// Empty in this build; DynamicCam_UnRegisterStreamClientsFE calls it. TW07 has
// DynamicCam_LoadFilesFromDiskFE in this place.
void DynamicCam_LoadFilesFromDiskFE(void) {
}

// The 'CAMS' stream handler, the sequence file: two counts (sequences, then shot choices for all of
// them), then the sequences, each followed by its choices (DynamicCam_CopySequenceData). Each
// 'CAMS' or 'CAMV' load counts n1C up (1, 2, then back to 1); the choices get their shots once both
// files are in (DynamicCam_ParseCameraSeqs). Then the sequences' follow-ons become pointers
// (DynamicCam_ParseNextSeqs). Frees the object; ignores it when sequences are loaded already.
void DynamicCam_LoadCAMSfromStream(UStreamObject* pObject) {
    s32 nSequences;
    s32 nChoices;
    u8* pSrc;

    lbl_80281D88->n1C++;
    if (lbl_80281D88->n1C > 2) {
        lbl_80281D88->n1C = 1;
    }
    if (lbl_80281D88->pSequences != NULL) {
        StaticMem_Free(pObject);
        return;
    }
    pSrc = pObject->pData;
    BYTESWAP_SWAPDATA(&pSrc, (u8*)&nSequences, sizeof(nSequences), 4);
    pSrc = pObject->pData + 4;
    BYTESWAP_SWAPDATA(&pSrc, (u8*)&nChoices, sizeof(nChoices), 4);
    lbl_80281D88->pSequences = StaticMem_Alloc(nSequences * sizeof(CamSequence), 2, 0, "GoDynamicCam.c", 403);
    lbl_80281D88->pChoices = StaticMem_Alloc(nChoices * sizeof(CamChoice), 2, 0, "GoDynamicCam.c", 404);
    lbl_80281D88->nSequences = 0;
    lbl_80281D88->nChoicesUsed = 0;
    pSrc = pObject->pData + 8;
    DynamicCam_CopySequenceData(pSrc, (u8*)lbl_80281D88->pSequences, nSequences);
    DynamicCam_ParseCameraSeqs(nSequences);
    DynamicCam_ParseNextSeqs();
    StaticMem_Free(pObject);
}

// The 'CAMV' stream handler in a round, the shot file: copies the shots (DynamicCam_CopyScriptData)
// and sets them up (DynamicCam_ParseCameraViews). Counted in n1C like 'CAMS'. Frees the object;
// ignores it when shots are loaded already.
void DynamicCam_LoadCAMVfromStream(UStreamObject* pObject) {
    lbl_80281D88->n1C++;
    if (lbl_80281D88->n1C > 2) {
        lbl_80281D88->n1C = 1;
    }
    if (lbl_80281D88->pShots != NULL) {
        StaticMem_Free(pObject);
        return;
    }
    lbl_80281D88->pShots = StaticMem_Alloc(pObject->uSize, 2, 0, "GoDynamicCam.c", 454);
    DynamicCam_CopyScriptData(pObject->pData, lbl_80281D88->pShots, pObject->uSize / sizeof(CamShot));
    DynamicCam_ParseCameraViews(pObject->uSize);
    StaticMem_Free(pObject);
}

// The front end's 'CAMV' handler: as DynamicCam_LoadCAMVfromStream, but not counted in n1C, and the
// shots are set up by DynamicCam_ParseCameraViewsFE (no height limits, no sequences to resolve).
void DynamicCam_LoadCAMVfromStreamFE(UStreamObject* pObject) {
    if (lbl_80281D88->pShots != NULL) {
        StaticMem_Free(pObject);
        return;
    }
    lbl_80281D88->pShots = StaticMem_Alloc(pObject->uSize, 2, 0, "GoDynamicCam.c", 495);
    DynamicCam_CopyScriptData(pObject->pData, lbl_80281D88->pShots, pObject->uSize / sizeof(CamShot));
    DynamicCam_ParseCameraViewsFE(pObject->uSize);
    StaticMem_Free(pObject);
}

// The 'CAMA' stream handler, the anim pairs (DynCamSet: a golfer animation's name and the shot or
// sequences to show for it; DynamicCamSearchForPairedSequence): copies them
// (DynamicCam_CopyAnimPairData) and turns their indexes into pointers (DynamicCam_ParseAnimPairs).
// Frees the object; ignores it when pairs are loaded already.
void DynamicCam_LoadCAMAfromStream(UStreamObject* pObject) {
    if (lbl_80281D88->pSets != NULL) {
        StaticMem_Free(pObject);
        return;
    }
    lbl_80281D88->pSets = StaticMem_Alloc(pObject->uSize, 2, 0, "GoDynamicCam.c", 536);
    DynamicCam_CopyAnimPairData(pObject->pData, lbl_80281D88->pSets, pObject->uSize / sizeof(DynCamSet));
    DynamicCam_ParseAnimPairs(pObject->uSize);
    StaticMem_Free(pObject);
}

// Copies nCount sequences from the file (little-endian) into pDst, swapping each value's bytes.
// Each is followed in the file by its shot choices, which go into the choice block in turn; the
// sequence's p4C gets the first of them. The file keeps a word where p4C goes.
void DynamicCam_CopySequenceData(u8* pSrc, u8* pDst, int nCount) {
    SwapField aSequence[] = {
        { 32, 1 },                                          // szName
        { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
        { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 },
    };
    SwapField aChoice[] = {
        { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
        { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 },
        { 48, 4 },                                          // aNoHoles
    };
    CamChoice* pChoice;
    int i;

    for (i = 0; i < nCount; i++) {
        ByteSwap_Records((void**)&pSrc, (void**)&pDst, aSequence, sizeof(aSequence) / sizeof(aSequence[0]),
                         1);
        // pDst is now at the sequence's p4C
        *(CamChoice**)pDst = &lbl_80281D88->pChoices[lbl_80281D88->nChoicesUsed];
        pSrc += sizeof(CamChoice*);
        pDst += sizeof(CamChoice*);
        pChoice = &lbl_80281D88->pChoices[lbl_80281D88->nChoicesUsed];
        ByteSwap_Records((void**)&pSrc, (void**)&pChoice, aChoice, sizeof(aChoice) / sizeof(aChoice[0]),
                    lbl_80281D88->pSequences[i].nChoices);
        lbl_80281D88->nChoicesUsed += lbl_80281D88->pSequences[i].nChoices;
    }
    lbl_80281D88->nSequences = nCount;
}

// Copies nCount shots from the file (little-endian) into pDst, swapping each value's bytes.
void DynamicCam_CopyScriptData(u8* pSrc, CamShot* pDst, u32 nCount) {
    SwapField aFormat[] = {
        { 32, 1 },                                          // szName
        { 16, 4 }, { 16, 4 },                               // v20, v30
        { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
        { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
        { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
        { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 },
        { 1, 1 }, { 1, 1 },
        { 13, 1 },
    };

    ByteSwap_Records((void**)&pSrc, (void**)&pDst, aFormat, sizeof(aFormat) / sizeof(aFormat[0]), nCount);
}

// Copies nCount anim pairs (DynCamSet) from the file into pDst, swapping each value's bytes.
void DynamicCam_CopyAnimPairData(u8* pSrc, DynCamSet* pDst, u32 nCount) {
    SwapField aFormat[] = {
        { 16, 1 },
        { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
        { 1, 1 },
        { 3, 1 },
    };

    ByteSwap_Records((void**)&pSrc, (void**)&pDst, aFormat, sizeof(aFormat) / sizeof(aFormat[0]), nCount);
}

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f (0x80283068), before the 0.0f DynamicCam_ParseCameraViews uses first; its body is unknown.
static f32 GoDynamicCam_StrippedFn(f32 x) {
    return x + 1.0f;
}

// Sets up nSize bytes of shots just loaded in a round: as DynamicCam_ParseCameraViewsFE, and each
// shot's least height f68 is kept at least CamTuning.f168, its most height f6C at least f68, and
// f8C within 0..0.49. When the sequence file is in too (n1C is 2), the sequences' choices are
// resolved (DynamicCam_ParseCameraSeqs).
void DynamicCam_ParseCameraViews(int nSize) {
    int i;

    lbl_80281D88->nShots = 0;
    lbl_80281D88->nShots = nSize / sizeof(CamShot);
    for (i = 0; i < lbl_80281D88->nShots; i++) {
        // port: the file keeps an index in the pointer field
        if (i == (s32)lbl_80281D88->pShots[i].p40) {
            lbl_80281D88->pShots[i].p40 = NULL;
        } else {
            lbl_80281D88->pShots[i].p40 = &lbl_80281D88->pShots[(s32)lbl_80281D88->pShots[i].p40];
            lbl_80281D88->pShots[i].p40->bA9 = 1;
        }
        if (lbl_80281D88->pShots[i].f68 < gpCamTuning->f168) {
            lbl_80281D88->pShots[i].f68 = gpCamTuning->f168;
        }
        if (lbl_80281D88->pShots[i].f6C < lbl_80281D88->pShots[i].f68) {
            lbl_80281D88->pShots[i].f6C = lbl_80281D88->pShots[i].f68;
        }
        lbl_80281D88->pShots[i].f8C = (lbl_80281D88->pShots[i].f8C < 0.0f) ? 0.0f
            : ((lbl_80281D88->pShots[i].f8C > 0.49f) ? 0.49f : lbl_80281D88->pShots[i].f8C);
        lbl_80281D88->pShots[i].f7C = lbl_80281D88->pShots[i].f78;
    }
    if (lbl_80281D88->n1C == 2) {
        DynamicCam_ParseCameraSeqs(lbl_80281D88->nSequences);
    }
}

// Sets up nSize bytes of shots just loaded: turns each shot's follow-on index (p40) into a pointer,
// NULL when it names the shot itself, and marks the follow-on (bA9: not picked on its own by
// DynamicCam_ChooseScript); f6C and f7C start at f68 and f78.
void DynamicCam_ParseCameraViewsFE(int nSize) {
    int i;

    lbl_80281D88->nShots = 0;
    lbl_80281D88->nShots = nSize / sizeof(CamShot);
    for (i = 0; i < lbl_80281D88->nShots; i++) {
        // port: the file keeps an index in the pointer field
        if (i == (s32)lbl_80281D88->pShots[i].p40) {
            lbl_80281D88->pShots[i].p40 = NULL;
        } else {
            lbl_80281D88->pShots[i].p40 = &lbl_80281D88->pShots[(s32)lbl_80281D88->pShots[i].p40];
            lbl_80281D88->pShots[i].p40->bA9 = 1;
        }
        lbl_80281D88->pShots[i].f6C = lbl_80281D88->pShots[i].f68;
        lbl_80281D88->pShots[i].f7C = lbl_80281D88->pShots[i].f78;
    }
}

// Stores the sequence count and, once both the sequence and the shot file are in (n1C is 2),
// resolves every sequence's shot choices: the shot index becomes a pointer, and the time trigger
// (b16) becomes 25 (none) unless it is an event 13..22 that some choice of the same sequence
// answers (its b14).
void DynamicCam_ParseCameraSeqs(int nSequences) {
    int i;
    int j;
    int k;
    u8 bFound;

    lbl_80281D88->nSequences = nSequences;
    if (lbl_80281D88->n1C != 2) {
        return;
    }
    for (i = 0; i < lbl_80281D88->nSequences; i++) {
        for (j = 0; j < lbl_80281D88->pSequences[i].nChoices; j++) {
            // port: the file keeps an index in the pointer field
            lbl_80281D88->pSequences[i].p4C[j].p10 =
                &lbl_80281D88->pShots[(s32)lbl_80281D88->pSequences[i].p4C[j].p10];
            if (lbl_80281D88->pSequences[i].p4C[j].b16 < 13 || lbl_80281D88->pSequences[i].p4C[j].b16 > 22) {
                lbl_80281D88->pSequences[i].p4C[j].b16 = 25;
            }
            if (lbl_80281D88->pSequences[i].p4C[j].b16 >= 13 &&
                lbl_80281D88->pSequences[i].p4C[j].b16 <= 22) {
                bFound = 0;
                for (k = 0; k < lbl_80281D88->pSequences[i].nChoices; k++) {
                    if (lbl_80281D88->pSequences[i].p4C[j].b16 == lbl_80281D88->pSequences[i].p4C[k].b14) {
                        bFound = 1;
                    }
                }
                if (!bFound) {
                    lbl_80281D88->pSequences[i].p4C[j].b16 = 25;
                }
            }
        }
    }
}

// Turns each sequence's follow-on index into a pointer; a sequence whose follow-on has no shot
// choices follows itself.
void DynamicCam_ParseNextSeqs(void) {
    int i;

    for (i = 0; i < lbl_80281D88->nSequences; i++) {
        // port: the file keeps an index in the pointer field
        lbl_80281D88->pSequences[i].p20 = &lbl_80281D88->pSequences[(s32)lbl_80281D88->pSequences[i].p20];
        if (lbl_80281D88->pSequences[i].p20->nChoices <= 0) {
            lbl_80281D88->pSequences[i].p20 = &lbl_80281D88->pSequences[i];
        }
    }
}

// Sets up nSize bytes of anim pairs just loaded: each shot and sequence index becomes a pointer
// into the loaded tables (NULL for a negative index).
void DynamicCam_ParseAnimPairs(int nSize) {
    int i;

    lbl_80281D88->nSets = nSize / sizeof(DynCamSet);
    for (i = 0; i < lbl_80281D88->nSets; i++) {
        // port: the file keeps indexes in the pointer fields
        if ((s32)lbl_80281D88->pSets[i].pShot >= 0) {
            lbl_80281D88->pSets[i].pShot = &lbl_80281D88->pShots[(s32)lbl_80281D88->pSets[i].pShot];
        } else {
            lbl_80281D88->pSets[i].pShot = NULL;
        }
        if ((s32)lbl_80281D88->pSets[i].p14 >= 0) {
            lbl_80281D88->pSets[i].p14 = &lbl_80281D88->pSequences[(s32)lbl_80281D88->pSets[i].p14];
        } else {
            lbl_80281D88->pSets[i].p14 = NULL;
        }
        if ((s32)lbl_80281D88->pSets[i].p18 >= 0) {
            lbl_80281D88->pSets[i].p18 = &lbl_80281D88->pSequences[(s32)lbl_80281D88->pSets[i].p18];
        } else {
            lbl_80281D88->pSets[i].p18 = NULL;
        }
        if ((s32)lbl_80281D88->pSets[i].p1C >= 0) {
            lbl_80281D88->pSets[i].p1C = &lbl_80281D88->pSequences[(s32)lbl_80281D88->pSets[i].p1C];
        } else {
            lbl_80281D88->pSets[i].p1C = NULL;
        }
        if ((s32)lbl_80281D88->pSets[i].p20 >= 0) {
            lbl_80281D88->pSets[i].p20 = &lbl_80281D88->pSequences[(s32)lbl_80281D88->pSets[i].p20];
        } else {
            lbl_80281D88->pSets[i].p20 = NULL;
        }
    }
}

// Allocates the dynamic cameras' tables (gpDynCam), empty: no shots, sequences, anim pairs or
// choices.
void DynamicCam_Init(void) {
    DynCamTables* pTables = StaticMem_Alloc(sizeof(DynCamTables), 2, 0, "GoDynamicCam.c", 938);

    lbl_80281D88 = pTables;
    pTables->pSequences = NULL;
    lbl_80281D88->pShots = NULL;
    lbl_80281D88->pSets = NULL;
    lbl_80281D88->pChoices = NULL;
    lbl_80281D88->nShots = 0;
    lbl_80281D88->nSequences = 0;
    lbl_80281D88->nSets = 0;
    lbl_80281D88->nChoicesUsed = 0;
}

// Frees the loaded shots, sequences, anim pairs and choices and the tables themselves, and resets
// the load count n1C.
void DynamicCam_DeInit(void) {
    lbl_80281D88->nShots = 0;
    lbl_80281D88->nSequences = 0;
    lbl_80281D88->nSets = 0;
    lbl_80281D88->nChoicesUsed = 0;
    if (lbl_80281D88->pSequences != NULL) {
        StaticMem_Free(lbl_80281D88->pSequences);
        lbl_80281D88->pSequences = NULL;
    }
    if (lbl_80281D88->pShots != NULL) {
        StaticMem_Free(lbl_80281D88->pShots);
        lbl_80281D88->pShots = NULL;
    }
    if (lbl_80281D88->pSets != NULL) {
        StaticMem_Free(lbl_80281D88->pSets);
        lbl_80281D88->pSets = NULL;
    }
    if (lbl_80281D88->pChoices != NULL) {
        StaticMem_Free(lbl_80281D88->pChoices);
        lbl_80281D88->pChoices = NULL;
    }
    lbl_80281D88->n1C = 0;
    StaticMem_Free(lbl_80281D88);
}

// Places the camera at pOut for a shot by its tracking mode (bB1): 0 and 1 DynamicCam_TrackPercent,
// 2, 3, 4 and 8 DynamicCam_TrackOffset, 5 and 6 DynamicCam_TrackBallVelocityLag (with the frame
// time f), 7 DynamicCam_TrackBallVelocityTight, 9 DynamicCam_TrackFixed. Then keeps it between the
// shot's least and most height (f68, f6C) over the ground: the ground is measured when
// CameraScript_SnapToScript holds for a shot other than the script's next one (on course 12 with
// Game_GetCurHoleNum 10, fn_8004D5F0's height within 40 of the player's tee; else
// CamScript_GuessBestPlayableHeight, raised for a kind 4 shot that is neither a swing camera nor
// tracks the golfer to the lower of the pin and the tee) and kept in the script's fD8; without
// course data it is 0 (the shot's f68 on the CrAP screen, game type 3). A swing camera or one that
// tracks the golfer takes its heights over the ball instead, at least CamTuning.f168. Too low,
// modes 5 to 7 rise by CamTuning.f194 a call while they were too low already (never under
// CamTuning.f168), the others jump to the least height; above the most height less CamTuning.f174
// over fD8 it eases in softly. Mode 3 then lags: it moves from where it was by the distance beyond
// half its f60, or less for a short move.
void DynamicCam_ProcessScript(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pCam,
                              f32* pSub,
                 f32 f) {
    f32 aOld[4];
    f32 aStep[4];
    f32 aOff[4];
    f32 aDir[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    CourseInfo* pCourse;
    int nPinSet;
    f32 fGround;
    f32 fLow;
    f32 fLo;
    f32 fHi;
    f32 fDiff;
    f32 fTop;
    f32 fEase;
    f32 fAbove;
    f32 fFollow;
    f32 fDist;
    f32 fLimit;
    f32 fStep;
    f32 fOver;

    nPinSet = Game_CurrentPinSet();
    Vec3Copy(pOut, aOld);
    switch (pShot->bB1) {
    case 0:
    case 1:
        DynamicCam_TrackPercent(pShot, nPlayer, pScript, pOut, pCam, pSub);
        break;
    case 2:
    case 3:
    case 4:
    case 8:
        DynamicCam_TrackOffset(pShot, nPlayer, pScript, pOut, pCam, pSub);
        break;
    case 5:
    case 6:
        DynamicCam_TrackBallVelocityLag(pShot, nPlayer, pScript, pOut, pCam, pSub, f);
        break;
    case 7:
        DynamicCam_TrackBallVelocityTight(pShot, nPlayer, pScript, pOut, pCam, pSub);
        break;
    case 9:
        DynamicCam_TrackFixed(pShot, nPlayer, pScript, pOut, pCam, pSub);
        break;
    }
    pCourse = Ter_GetTGD();
    if (pCourse != NULL) {
        if (CameraScript_SnapToScript(pScript, pShot) && pShot != pScript->pNextShot) {
            if (Game_GetCourse() == 12 && Game_GetCurHoleNum() == 10) {
                fn_8003DC54(&pCourse->tee[gSession.nTeeSet[nPlayer]].x, pOut, aOff);
                aOff[1] = 0.0f;
                if ((f32)Math_Sqrt(Vec3_LengthSqClamped(aOff)) < 40.0f) {
                    fGround = fn_8004D5F0(pCourse, pOut);
                } else {
                    fGround = CamScript_GuessBestPlayableHeight(pOut, NULL);
                    if (pShot->bAD == 4 && !DynamicCam_bIsSwingCamera(pShot) && !fn_8003DC78(pShot)) {
                        fLow = (pCourse->pin[nPinSet].y <= pCourse->tee[gSession.nTeeSet[nPlayer]].y)
                                   ? pCourse->pin[nPinSet].y
                                   : pCourse->tee[gSession.nTeeSet[nPlayer]].y;
                        fGround = (fLow <= fGround) ? fGround : fLow;
                    }
                }
                pScript->fD8 = fGround;
                if (pScript->fD8 < -60000.0f) {
                    pScript->fD8 = Ter_GetTGD()->fFloor;
                }
            } else {
                fGround = CamScript_GuessBestPlayableHeight(pOut, NULL);
                if (pShot->bAD == 4 && !DynamicCam_bIsSwingCamera(pShot) && !fn_8003DC78(pShot)) {
                    fLow = (pCourse->pin[nPinSet].y <= pCourse->tee[gSession.nTeeSet[nPlayer]].y)
                               ? pCourse->pin[nPinSet].y
                               : pCourse->tee[gSession.nTeeSet[nPlayer]].y;
                    fGround = (fLow <= fGround) ? fGround : fLow;
                }
                pScript->fD8 = fGround;
                if (pScript->fD8 < -60000.0f) {
                    pScript->fD8 = Ter_GetTGD()->fFloor;
                }
            }
        } else {
            fGround = pScript->fD8;
        }
    } else if (gSession.nGameType == 3) {
        fGround = pShot->f68;
        pOut[1] = fGround;
    } else {
        fGround = 0.0f;
    }
    if (fGround < -60000.0f) {
        fGround = fn_8004D5F0(pCourse, pOut);
    }
    if (DynamicCam_bIsSwingCamera(pShot) || fn_8003DC78(pShot)) {
        fDiff = gPlayers[nPlayer].vBall[1] - fGround;
        fLo = pShot->f68 + fDiff;
        fHi = pShot->f6C + fDiff;
        if (fLo < gpCamTuning->f168) {
            fLo = gpCamTuning->f168;
        }
        if (fHi < gpCamTuning->f168) {
            fHi = gpCamTuning->f168;
        }
    } else {
        fLo = pShot->f68;
        fHi = fn_8003DBA8(pShot->f6C);
    }
    if (pOut[1] - fGround < fLo) {
        if (pShot->bB1 == 5 || pShot->bB1 == 6 || pShot->bB1 == 7) {
            // fake match: a negated >=, where < gives a plain bge
            if (!(aOld[1] - fGround >= fLo)) {
                pOut[1] += gpCamTuning->f194;
                if (pOut[1] - fGround > fLo) {
                    pOut[1] = fGround + fLo;
                }
            }
            if (pOut[1] - fGround < gpCamTuning->f168) {
                pOut[1] = fGround + gpCamTuning->f168;
            }
        } else {
            pOut[1] = fGround + fLo;
        }
    } else {
        fFollow = pScript->fD8;
        fEase = gpCamTuning->f174;
        fAbove = pOut[1] - fFollow;
        fTop = fHi - fEase;
        if (fAbove > fTop) {
            fOver = fAbove - fTop;
            fOver = fEase * (1.0f - fEase / (fOver + fEase));
            pOut[1] = fOver + (fFollow + fTop);
            if (pOut[1] - fGround < fLo) {
                pOut[1] = fGround + fLo;
            }
        }
    }
    if (pShot->bB1 == 3 && !CameraScript_SnapToScript(pScript, pShot) &&
        (pScript->pNextShot != pShot || pScript->fCamTime > 0.0f)) {
        fn_8003DC54(pOut, aOld, aStep);
        fDist = (f32)Math_Sqrt(Vec3_LengthSqClamped(aStep));
        if (aStep[0] != 0.0f || aStep[1] != 0.0f || aStep[2] != 0.0f) {
            LLMath_Normalize3(aStep, aDir);
        } else {
            aDir[0] = 0.0f;
            aDir[1] = 0.0f;
            aDir[2] = 0.0f;
        }
        fLimit = fabsf(pShot->f60) / 2.0f;
        if (fDist > fLimit) {
            fStep = fDist - fLimit;
        } else {
            fStep = fDist * (1.0f - (fLimit - fDist) / fLimit);
            fStep = fStep * fStep;
        }
        Vec3_Scale(fStep, aDir, aStep);
        fn_8003DC30(aOld, aStep, pOut);
    }
}

// The shot is a swing camera: its state kind (bAD) is 1, 3, 13, 28..34 or 40..45.
// DynamicCam_ProcessScript measures such a camera's heights from the ball instead of the ground.
u8 DynamicCam_bIsSwingCamera(CamShot* pShot) {
    u8 nKind = pShot->bAD;

    if (nKind == 3 || nKind == 1 || (nKind >= 29 && nKind <= 33) || (nKind >= 40 && nKind <= 45)
        || nKind == 13 || nKind == 28 || nKind == 34) {
        return 1;
    }
    return 0;
}

// A random shot for state nKind (fn_8003D240), from the first 50 that fn_8003D294 allows, that are
// no other shot's follow-on (bA9) and are not pShot; NULL when there is none. nPlayer is not used.
CamShot* DynamicCam_ChooseScript(int nPlayer, int nKind, CamShot* pShot) {
    int aPick[50];
    int* pPick = aPick;
    int nCount = 0;
    int i;

    for (i = 0; i < lbl_80281D88->nShots; i++) {
        if (nCount >= 50) break;
        if (fn_8003D240(&lbl_80281D88->pShots[i], nKind) && fn_8003D294(&lbl_80281D88->pShots[i])
            && !lbl_80281D88->pShots[i].bA9 && &lbl_80281D88->pShots[i] != pShot) {
            *pPick++ = i;
            nCount++;
        }
    }
    if (nCount == 0) return NULL;
    i = Misc_RandFunc(1) % nCount;
    return &lbl_80281D88->pShots[aPick[i]];
}

// The shot with this name (case ignored), or NULL.
CamShot* DynamicCam_ChooseScriptByName(char* szName) {
    int i;

    for (i = 0; i < lbl_80281D88->nShots; i++) {
        if (stricmp(szName, lbl_80281D88->pShots[i].szName) == 0) {
            return &lbl_80281D88->pShots[i];
        }
    }
    return NULL;
}

// A shot for camera event nKind from the sequence's choices (a choice for event 9 answers any event
// but 23), picked at random from the first 50 that may be used on this hole
// (DynamicCam_CanUseScriptOnThisHole) and by this golfer (DynamicCam_CanUseScriptOnThisModel);
// failing that, from the first 50 for the event at all. The choice's blend goes to the out pointers
// that are not NULL (TW07's names): *pA its interpolation kind (b15), *pF1 its time (f0), *pF2 its
// speed (f4), *pB the event that cuts it short (b16) and *pF3 when (f8). NULL for a NULL sequence
// or when no choice answers the event.
CamShot* DynamicCam_ChooseScriptInSequence(CamSequence* pSequence, int nKind, int* pA, f32* pF1, f32* pF2, int* pB, f32* pF3,
                     int nPlayer) {
    int aPick[50];
    int i;
    int nCount = 0;
    u32 nPick;

    if (pSequence == NULL) return NULL;
    for (i = 0; i < pSequence->nChoices; i++) {
        if (nCount >= 50) break;
        if ((nKind == pSequence->p4C[i].b14 || (nKind != 23 && pSequence->p4C[i].b14 == 9))
            && DynamicCam_CanUseScriptOnThisHole(&pSequence->p4C[i])
                    && DynamicCam_CanUseScriptOnThisModel(&pSequence->p4C[i], nPlayer)) {
            aPick[nCount] = i;
            nCount++;
        }
    }
    if (nCount == 0) {
        for (i = 0; i < pSequence->nChoices; i++) {
            if (nCount >= 50) break;
            if (nKind == pSequence->p4C[i].b14 || (nKind != 23 && pSequence->p4C[i].b14 == 9)) {
                aPick[nCount] = i;
                nCount++;
            }
        }
    }
    if (nCount == 0) return NULL;
    nPick = Misc_RandFunc(1) % nCount;
    if (pA != NULL) {
        *pA = pSequence->p4C[aPick[nPick]].b15;
    }
    if (pF1 != NULL) {
        *pF1 = pSequence->p4C[aPick[nPick]].f0;
    }
    if (pF2 != NULL) {
        *pF2 = pSequence->p4C[aPick[nPick]].f4;
    }
    if (pB != NULL) {
        *pB = pSequence->p4C[aPick[nPick]].b16;
    }
    if (pF3 != NULL) {
        *pF3 = pSequence->p4C[aPick[nPick]].f8;
    }
    return pSequence->p4C[aPick[nPick]].p10;
}

// The choice may be used on the current hole: its aNoHoles bit for Game_GetCourse() * 18 +
// Game_GetCurHoleNum() is clear.
u8 DynamicCam_CanUseScriptOnThisHole(CamChoice* pChoice) {
    if (BitArray_TestBit(pChoice->aNoHoles, Game_GetCourse() * 18 + Game_GetCurHoleNum())) return 0;
    return 1;
}

// The choice suits the player's golfer: a choice marked b17 is only for golfer models 0, 1, 8, 10,
// 12..15 and 17.
u8 DynamicCam_CanUseScriptOnThisModel(CamChoice* pChoice, int nPlayer) {
    int nModel = gPlayers[nPlayer].golfer.nModelID;

    if (pChoice->b17) {
        if (nModel == 0 || nModel == 1 || nModel == 8 || nModel == 10 || nModel == 12 || nModel == 13
            || nModel == 14 || nModel == 15 || nModel == 17) {
            return 1;
        }
        return 0;
    }
    return 1;
}

// Tracking modes 0 and 1: the camera at share f60 of the way from the shot's first point
// (DynamicCam_GetLocation for bAF) to its second (bB0), the way flattened for mode 0, then f64
// sideways (the other way when fn_800453C8 holds for the shot), then its height from
// DynamicCam_AddHeightOffset. A shot with a point of kind 0 (the ball) or 23 waits for a frame in
// which the ball updates (GameEffects_BallUpdatesThisFrame).
void DynamicCam_TrackPercent(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pCam,
                             f32* pSub) {
    f32 vFrom[4];
    f32 vTo[4];
    f32 fY = pOut[1];

    if ((pShot->bAF == 0 || pShot->bB0 == 0 || pShot->bAF == 23 || pShot->bB0 == 23)
        && GameEffects_BallUpdatesThisFrame(nPlayer) < 1) {
        return;
    }
    DynamicCam_GetLocation(pShot->bAF, nPlayer, vFrom, pScript, pShot, pCam, pSub);
    DynamicCam_GetLocation(pShot->bB0, nPlayer, vTo, pScript, pShot, pCam, pSub);
    if (pShot->bB1 == 0) {
        if (CameraScript_FlipCameraForLefty(nPlayer, pShot)) {
            CamUtils_vGetPositionBetweenTwoPoints(vFrom, vTo, 0, 1, pOut, pShot->f60, -pShot->f64);
        } else {
            CamUtils_vGetPositionBetweenTwoPoints(vFrom, vTo, 0, 1, pOut, pShot->f60, pShot->f64);
        }
    } else if (CameraScript_FlipCameraForLefty(nPlayer, pShot)) {
        CamUtils_vGetPositionBetweenTwoPoints(vFrom, vTo, 1, 1, pOut, pShot->f60, -pShot->f64);
    } else {
        CamUtils_vGetPositionBetweenTwoPoints(vFrom, vTo, 1, 1, pOut, pShot->f60, pShot->f64);
    }
    DynamicCam_AddHeightOffset(pOut, pScript, pShot, nPlayer, fY);
}

// Tracking modes 2, 3, 4 and 8: the camera f60 along the direction from the shot's first point
// (bAF) to its second (bB0), level for modes 2 and 3, then f64 sideways (both as fn_8003DAC8 flips
// them), then its height from DynamicCam_AddHeightOffset. For mode 8 its level offset from pSub
// (its height becoming pSub's) is scaled from CamTuning.f240 down to f248 as the distance
// fn_80044F58 gives (never less than the most seen, kept in the script's f100) goes from
// CamTuning.f23C to f244; when CameraScript_SnapToScript holds, f100 just takes the distance.
// Waits, like DynamicCam_TrackPercent, for a ball update when a point is the ball.
void DynamicCam_TrackOffset(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pCam,
                            f32* pSub) {
    f32 vFrom[4];
    f32 vTo[4];
    f32 vOff[4];
    f32 fSide;
    f32 fDist;
    f32 fY = pOut[1];
    f32 fFar;
    f32 fScale;

    if ((pShot->bAF == 0 || pShot->bB0 == 0 || pShot->bAF == 23 || pShot->bB0 == 23)
        && GameEffects_BallUpdatesThisFrame(nPlayer) < 1) {
        return;
    }
    DynamicCam_GetLocation(pShot->bAF, nPlayer, vFrom, pScript, pShot, pCam, pSub);
    DynamicCam_GetLocation(pShot->bB0, nPlayer, vTo, pScript, pShot, pCam, pSub);
    if (pShot->bB1 == 2 || pShot->bB1 == 3) {
        fn_8003DAC8(pShot, nPlayer, &fSide, &fDist);
        CamUtils_vGetPositionBetweenTwoPoints(vFrom, vTo, 0, 0, pOut, fDist, fSide);
    } else {
        fn_8003DAC8(pShot, nPlayer, &fSide, &fDist);
        CamUtils_vGetPositionBetweenTwoPoints(vFrom, vTo, 1, 0, pOut, fDist, fSide);
    }
    if (pShot->bB1 == 8) {
        fFar = CamScript_GetBallToPinPercent(nPlayer, pScript);
        if (!CameraScript_SnapToScript(pScript, pShot)) {
            if (fFar > pScript->f100) {
                pScript->f100 = fFar;
            }
            fFar = pScript->f100;
            fn_8003DC54(pOut, pSub, vOff);
            vOff[1] = 0.0f;
            if (fFar > gpCamTuning->f23C) {
                if (fFar > gpCamTuning->f244) {
                    fScale = gpCamTuning->f248;
                } else {
                    fScale = (fFar - gpCamTuning->f23C) / (gpCamTuning->f244 - gpCamTuning->f23C);
                    fScale = gpCamTuning->f240 - fScale * (gpCamTuning->f240 - gpCamTuning->f248);
                }
                Vec3_Scale(fScale, vOff, vOff);
                fn_8003DC30(vOff, pSub, pOut);
            }
        } else {
            pScript->f100 = fFar;
        }
    }
    DynamicCam_AddHeightOffset(pOut, pScript, pShot, nPlayer, fY);
}

// Tracking modes 5 and 6, the lagging ball-flight camera: its target is f60 along the ball's flight
// direction (level for mode 6, kept within CamTuning.f19C of level by fn_8003D810) from the ball
// (DynamicCam_GetLocation's point 0), plus CamTuning.f18C times (f6C - height) when the ball is
// above its most height over the script's ground fD8 or f190 times (height - f68) below its least,
// then f64 sideways (fn_8003D324); the heights are the current shot's while the next shot has the
// same mode. When CameraScript_SnapToScript holds the camera jumps there. Otherwise it closes the
// distance to the ball (CamTuning.f14C) and turns round the ball towards the target (f150) by a
// share per NTSC frame of the frame time f, scaled while the script's f98 is under CamTuning.f154
// (by ((f154 - fCamTime) / f154) squared over f14C), times the script's f8C when its nBC is 4, less
// for a ball slower than CamTuning.f198 and while the script's second clock f88 is under f158;
// while f88 is under CamTuning.f15C only part of the target's height is taken (the share to the
// power f160).
void DynamicCam_TrackBallVelocityLag(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pCam,
                                     f32* pSub,
                 f32 f) {
    f32 fStep;
    f32 aFrom[4];
    f32 aBallDir[4];
    f32 aTarget[4];
    f32 aCurOff[4];
    f32 aCurDir[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    f32 aTgtOff[4];
    f32 aTgtDir[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    f32 aNew[4];
    f32 aNewDir[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    f32 aAxis[4];
    f32 aTurn[4];
    CamShot* pCur;
    f32 fHi;
    f32 fLo;
    f32 fHeight;
    f32 fDist;
    f32 fMove;
    f32 fTurn;
    f32 fEase;
    f32 fT;
    f32 fCurDist;
    f32 fAngle;
    f32 fFrames;
    f32 fY;
    f32 fGrow;
    f32 fRise;

    fY = pOut[1];
    DynamicCam_GetLocation(0, nPlayer, aFrom, pScript, pShot, pCam, pSub);
    Vec3Copy(gPlayers[nPlayer].ball.vVel, aBallDir);
    if (pShot->bB1 == 6) {
        aBallDir[1] = 0.0f;
    }
    if (aBallDir[0] != 0.0f || aBallDir[1] != 0.0f || aBallDir[2] != 0.0f) {
        LLMath_Normalize3(aBallDir, aBallDir);
    }
    fn_8003D810(aBallDir, pOut, aFrom);
    if (pScript->pNextShot != NULL && pScript->pShot->bB1 == pScript->pNextShot->bB1) {
        pCur = pScript->pShot;
        fHi = pCur->f6C;
        fLo = pCur->f68;
    } else {
        fHi = pShot->f6C;
        fLo = pShot->f68;
    }
    fHeight = gPlayers[nPlayer].ball.vPos[1] - pScript->fD8;
    if (fHeight > fHi) {
        fDist = gpCamTuning->f18C * (fHi - fHeight) + pShot->f60;
    } else if (fHeight < fLo) {
        fDist = gpCamTuning->f190 * (fHeight - fLo) + pShot->f60;
    } else {
        fDist = pShot->f60;
    }
    fn_8000C5D4(aFrom, aBallDir, fDist, aTarget);
    fn_8003D324(aTarget, aBallDir, pScript, pShot, nPlayer, pShot->f64, fY);
    if (CameraScript_SnapToScript(pScript, pShot)) {
        Vec3Copy(aTarget, pOut);
        return;
    }
    // port: NTSC rate
    fFrames = f / (1.0f / 59.94f);
    fMove = gpCamTuning->f14C * fFrames;
    fTurn = gpCamTuning->f150 * fFrames;
    if (pScript->f98 < gpCamTuning->f154) {
        fT = (gpCamTuning->f154 - pScript->fCamTime) / gpCamTuning->f154;
        fT *= fT;
        fEase = (1.0f / gpCamTuning->f14C) * fT;
        fMove *= fEase;
        fTurn *= fEase;
    }
    if (pScript->nBC == 4) {
        fMove *= pScript->f8C;
        fTurn *= pScript->f8C;
    }
    if (gPlayers[nPlayer].ball.fSpeed < gpCamTuning->f198) {
        fGrow = gPlayers[nPlayer].ball.fSpeed / gpCamTuning->f198;
        fGrow = fGrow * fGrow;
        fGrow = fGrow * fGrow;
        fMove *= fGrow;
        fTurn *= fGrow;
    }
    if (pScript->f88 < gpCamTuning->f158) {
        fMove *= pScript->f88 / gpCamTuning->f158;
        fTurn *= pScript->f88 / gpCamTuning->f158;
    }
    if (pScript->f88 < gpCamTuning->f15C) {
        fRise = powf(pScript->f88 / gpCamTuning->f15C, gpCamTuning->f160);
        fRise *= fFrames;
        aTarget[1] = fRise * (aTarget[1] - pOut[1]) + pOut[1];
    }
    fn_8003DC54(pOut, aFrom, aCurOff);
    fn_8003DC54(aTarget, aFrom, aTgtOff);
    fCurDist = (f32)Math_Sqrt(Vec3_LengthSqClamped(aCurOff));
    fStep = (f32)Math_Sqrt(Vec3_LengthSqClamped(aTgtOff)) - fCurDist;
    fStep *= fMove;
    if (aCurOff[0] != 0.0f || aCurOff[1] != 0.0f || aCurOff[2] != 0.0f) {
        LLMath_Normalize3(aCurOff, aCurDir);
    } else {
        aCurDir[0] = 0.0f;
        aCurDir[1] = 0.0f;
        aCurDir[2] = 0.0f;
    }
    if (aTgtOff[0] != 0.0f || aTgtOff[1] != 0.0f || aTgtOff[2] != 0.0f) {
        LLMath_Normalize3(aTgtOff, aTgtDir);
    } else {
        aTgtDir[0] = 0.0f;
        aTgtDir[1] = 0.0f;
        aTgtDir[2] = 0.0f;
    }
    fAngle = Math_Acos(Vec3_Dot(aCurDir, aTgtDir));
    fTurn = fAngle * fTurn;
    vec4flt_CrossProduct(aCurDir, aTgtDir, aAxis);
    if (aAxis[0] != 0.0f || aAxis[1] != 0.0f || aAxis[2] != 0.0f) {
        LLMath_Normalize3(aAxis, aAxis);
    }
    Vec3_Scale(fTurn, aAxis, aAxis);
    Quat_BuildFromVector(aAxis, aTurn);
    aCurDir[3] = 0.0f;
    Quat_RotateVector(aTurn, aCurDir, aNew);
    if (aNew[0] != 0.0f || aNew[1] != 0.0f || aNew[2] != 0.0f) {
        LLMath_Normalize3(aNew, aNewDir);
    } else {
        aNewDir[0] = 0.0f;
        aNewDir[1] = 0.0f;
        aNewDir[2] = 0.0f;
    }
    Vec3_Scale(fCurDist + fStep, aNewDir, aNew);
    fn_8003DC30(aFrom, aNew, pOut);
}

// Tracking mode 7, the tight ball-flight camera: f60 along the ball's flight direction from the
// ball (DynamicCam_GetLocation's point 0), plus CamTuning.f18C times (f6C - height) when the ball's
// height over the script's ground fD8 is above the shot's most height f6C, or f190 times (height -
// f68) when under its least height f68 (the current shot's heights while the next shot has the same
// mode), then f64 sideways (fn_8003D324). Nothing moves while the ball is slower than
// CamTuning.f198.
void DynamicCam_TrackBallVelocityTight(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pCam,
                                       f32* pSub) {
    f32 vFrom[4];
    f32 vDir[4];
    f32 vPos[4];
    f32 fY = pOut[1];
    f32 fHeight;
    f32 fMax;
    f32 fMin;
    f32 fDist;

    DynamicCam_GetLocation(0, nPlayer, vFrom, pScript, pShot, pCam, pSub);
    Vec3Copy(gPlayers[nPlayer].ball.vVel, vDir);
    if (0.0f != vDir[0] || 0.0f != vDir[1] || 0.0f != vDir[2]) {
        LLMath_Normalize3(vDir, vDir);
    }
    if (gPlayers[nPlayer].ball.fSpeed < gpCamTuning->f198) return;
    if (pScript->pNextShot != NULL && pScript->pShot->bB1 == pScript->pNextShot->bB1) {
        fMax = pScript->pShot->f6C;
        fMin = pScript->pShot->f68;
    } else {
        fMax = pShot->f6C;
        fMin = pShot->f68;
    }
    fHeight = gPlayers[nPlayer].ball.vPos[1] - pScript->fD8;
    if (fHeight > fMax) {
        fDist = gpCamTuning->f18C * (fMax - fHeight) + pShot->f60;
    } else if (fHeight < fMin) {
        fDist = gpCamTuning->f190 * (fHeight - fMin) + pShot->f60;
    } else {
        fDist = pShot->f60;
    }
    fn_8000C5D4(vFrom, vDir, fDist, vPos);
    fn_8003D324(vPos, vDir, pScript, pShot, nPlayer, pShot->f64, fY);
    Vec3Copy(vPos, pOut);
}

// Tracking mode 9: the camera at the shot's first point (DynamicCam_GetLocation for bAF).
void DynamicCam_TrackFixed(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pCam, f32* pSub) {
    f32 vPos[4];

    DynamicCam_GetLocation(pShot->bAF, nPlayer, vPos, pScript, pShot, pCam, pSub);
    Vec3Copy(vPos, pOut);
}

// A tracking point of kind nKind for the shot, into pOut (the body parts are TW07's names for these
// bones): 0 the ball (DynamicCam_GetSmoothBallLocation), 1 Player.vBall, 2 between the golfer's
// feet (bones 0x39 and 0x47), 4 the waist (bone 1), 6 the head (bone 10), 8 the chest (bone 7), 9
// Player.vTarget2, 10 the pin, 11 the player's tee, 12 the script's v50, 16 the script's own camera
// point (set on the fairway by CamScript_GetCameraOnFairwayPos for its shot or
// CamScript_PutBackOnFairway for its next shot while CameraScript_SnapToScript holds, else the
// script's v0), 17..19 the head, chest and waist moved one unit along their bone's third matrix
// row, 20 and 21 the shot's other point (the golfer's origin, bone 0, when that is 20 or 21 too)
// moved one unit along bone 0's first or third row, 24 the shot's v20, 25 (0, 0, 100). Any other
// kind leaves pOut as it was.
void DynamicCam_GetLocation(int nKind, int nPlayer, f32* pOut, CamScript* pScript, CamShot* pShot, f32* pCam,
                            f32* pSub) {
    f32 vBone47[4];
    f32 vBone39[4];
    f32 vMid[4];
    f32 vBone10[4];
    f32 vBone1[4];
    f32 vBone7[4];
    f32 vDir7[4];
    f32 vDir10[4];
    f32 vDir1[4];
    f32 vFrom[4];
    f32 vDir[4];
    f32 (*pMatrix)[4];
    int nOther;
    int nPin;
    int nTee;
    CourseInfo* pCourse;

    if (nKind == pShot->bAF) {
        nOther = pShot->bB0;
    } else {
        nOther = pShot->bAF;
    }
    switch (nKind) {
    case 0:
        DynamicCam_GetSmoothBallLocation(pScript, pShot, nPlayer, pOut, 0);
        break;
    case 2:
        Character_GetBonePos(gPlayers[nPlayer].pChar, 0x39, vBone39);
        Character_GetBonePos(gPlayers[nPlayer].pChar, 0x47, vBone47);
        fn_8003DC30(vBone39, vBone47, vMid);
        Vec3_Scale(0.5f, vMid, vMid);
        Vec3Copy(vMid, pOut);
        break;
    case 6:
        Character_GetBonePos(gPlayers[nPlayer].pChar, 10, vBone10);
        Vec3Copy(vBone10, pOut);
        break;
    case 8:
        Character_GetBonePos(gPlayers[nPlayer].pChar, 7, vBone7);
        Vec3Copy(vBone7, pOut);
        break;
    case 4:
        Character_GetBonePos(gPlayers[nPlayer].pChar, 1, vBone1);
        Vec3Copy(vBone1, pOut);
        break;
    case 1:
        Vec3Copy(gPlayers[nPlayer].vBall, pOut);
        break;
    case 9:
        Vec3Copy(gPlayers[nPlayer].vTarget2, pOut);
        break;
    case 12:
        Vec3Copy(pScript->v50, pOut);
        break;
    case 10:
        nPin = Game_CurrentPinSet();
        pCourse = Ter_GetTGD();
        Vec3Copy(&pCourse->pin[nPin].x, pOut);
        break;
    case 11:
        nTee = gSession.nTeeSet[nPlayer];
        pCourse = Ter_GetTGD();
        Vec3Copy(&pCourse->tee[nTee].x, pOut);
        break;
    case 24:
        Vec3Copy(pShot->v20, pOut);
        break;
    case 16:
        if (CameraScript_SnapToScript(pScript, pShot)) {
            if (pShot == pScript->pShot) {
                CamScript_GetCameraOnFairwayPos(pScript, pOut, pCam, nPlayer, pScript->pB4, pSub, NULL);
                Vec3Copy(pOut, pScript->v0);
                pScript->bCF = 1;
            } else if (pShot == pScript->pNextShot) {
                CamScript_PutBackOnFairway(pScript, pOut, pCam, nPlayer, pScript->pB4, pSub);
                Vec3Copy(pOut, pScript->v10);
            }
        } else if (pShot == pScript->pShot) {
            Vec3Copy(pScript->v0, pOut);
        } else if (pShot == pScript->pNextShot) {
            // the script's v0 for the next shot too (v10 is where the fairway branch keeps it)
            Vec3Copy(pScript->v0, pOut);
        }
        break;
    case 17:
        Character_GetBonePos(gPlayers[nPlayer].pChar, 10, vBone10);
        pMatrix = Character_GetBoneMatrix(gPlayers[nPlayer].pChar, 10);
        if (pMatrix == NULL) {
            Vec3Copy(vBone10, pOut);
            break;
        }
        Vec3Copy(pMatrix[2], vDir10);
        if (0.0f != vDir10[0] || 0.0f != vDir10[1] || 0.0f != vDir10[2]) {
            LLMath_Normalize3(vDir10, vDir10);
        }
        fn_8003DC30(vBone10, vDir10, pOut);
        break;
    case 18:
        Character_GetBonePos(gPlayers[nPlayer].pChar, 7, vBone7);
        pMatrix = Character_GetBoneMatrix(gPlayers[nPlayer].pChar, 7);
        if (pMatrix == NULL) {
            Vec3Copy(vBone7, pOut);
            break;
        }
        Vec3Copy(pMatrix[2], vDir7);
        if (0.0f != vDir7[0] || 0.0f != vDir7[1] || 0.0f != vDir7[2]) {
            LLMath_Normalize3(vDir7, vDir7);
        }
        fn_8003DC30(vBone7, vDir7, pOut);
        break;
    case 19:
        Character_GetBonePos(gPlayers[nPlayer].pChar, 1, vBone1);
        pMatrix = Character_GetBoneMatrix(gPlayers[nPlayer].pChar, 1);
        if (pMatrix == NULL) {
            Vec3Copy(vBone1, pOut);
            break;
        }
        Vec3Copy(pMatrix[2], vDir1);
        if (0.0f != vDir1[0] || 0.0f != vDir1[1] || 0.0f != vDir1[2]) {
            LLMath_Normalize3(vDir1, vDir1);
        }
        fn_8003DC30(vBone1, vDir1, pOut);
        break;
    case 20:
        if (nOther != 20 && nOther != 21) {
            DynamicCam_GetLocation(nOther, nPlayer, vFrom, pScript, pShot, pCam, pSub);
        } else {
            Character_GetBonePos(gPlayers[nPlayer].pChar, 0, vFrom);
        }
        pMatrix = Character_GetBoneMatrix(gPlayers[nPlayer].pChar, 0);
        if (pMatrix == NULL) {
            Vec3Copy(vFrom, pOut);
            break;
        }
        Vec3Copy(pMatrix[0], vDir);
        if (0.0f != vDir[0] || 0.0f != vDir[1] || 0.0f != vDir[2]) {
            LLMath_Normalize3(vDir, vDir);
        }
        fn_8003DC30(vFrom, vDir, pOut);
        break;
    case 21:
        if (nOther != 20 && nOther != 21) {
            DynamicCam_GetLocation(nOther, nPlayer, vFrom, pScript, pShot, pCam, pSub);
        } else {
            Character_GetBonePos(gPlayers[nPlayer].pChar, 0, vFrom);
        }
        pMatrix = Character_GetBoneMatrix(gPlayers[nPlayer].pChar, 0);
        if (pMatrix == NULL) {
            Vec3Copy(vFrom, pOut);
            break;
        }
        Vec3Copy(pMatrix[2], vDir);
        if (0.0f != vDir[0] || 0.0f != vDir[1] || 0.0f != vDir[2]) {
            LLMath_Normalize3(vDir, vDir);
        }
        fn_8003DC30(vFrom, vDir, pOut);
        break;
    case 25:
        pOut[0] = 0.0f;
        pOut[1] = 0.0f;
        pOut[2] = 100.0f;
        break;
    }
}

// Picks the camera sequence for state nKind of nPlayer's shot, from the first 50 sequences with
// shot choices whose state, club, shot (a: TW07's allowGenericShotTypes), player type, course, par,
// game mode, start lie (nLie, or the tee on the tee; DynamicCam_BallLieToCameraLie), end lie
// (nClass through DynamicCam_MaterialToCameraLie), pin height over the ball (f2C..f30) and shot
// distance fDist (f24..f28) all fit: one at random by their weights (f34). With none, the last
// default ("DEF") sequence for the state on this course, else the first default one for the state
// that has choices, else NULL. NULL without course data.
CamSequence* DynamicCam_ChooseSequence(int nPlayer, int nLie, int nClass, int nKind, u8 a, f32 fDist) {
    int anPicked[50];
    CourseInfo* pCourse;
    CamSequence* pSeq;
    int i;
    int nPicked;
    int nSum;
    int nDefault;
    int nTee;
    int nClassBit;
    int nPinSet;
    u32 uRand;
    int nRoll;
    f32 fHeight;
    f32 fTotal;

    nPicked = 0;
    nSum = 0;
    nDefault = -1;
    if (Player_OnTee(nPlayer)) {
        nTee = 1;
    } else {
        nTee = DynamicCam_BallLieToCameraLie(nLie, nPlayer);
    }
    nClassBit = DynamicCam_MaterialToCameraLie(nClass, nPlayer);
    nPinSet = Game_CurrentPinSet();
    pCourse = Ter_GetTGD();
    if (pCourse == NULL) {
        return NULL;
    }
    fHeight = pCourse->pin[nPinSet].y - gPlayers[nPlayer].vBall[1];
    for (i = 0; i < lbl_80281D88->nSequences; i++) {
        if (nPicked >= 50) {
            break;
        }
        if (lbl_80281D88->pSequences[i].nChoices > 0) {
            if (DynamicCam_IsDefualtSeq(&lbl_80281D88->pSequences[i], nKind) &&
                fn_8003D00C(&lbl_80281D88->pSequences[i])) {
                nDefault = i;
            } else if (fn_8003D0EC(&lbl_80281D88->pSequences[i], nKind) &&
                       DynamicCam_MatchSeqClub(&lbl_80281D88->pSequences[i], nPlayer) &&
                       fn_8003CD9C(&lbl_80281D88->pSequences[i], nPlayer, a) &&
                       fn_8003CEEC(&lbl_80281D88->pSequences[i], nPlayer) &&
                       fn_8003D00C(&lbl_80281D88->pSequences[i]) &&
                       fn_8003D054(&lbl_80281D88->pSequences[i]) &&
                       fn_8003D0A0(lbl_80281D88->pSequences[i].b49, nTee) &&
                       fn_8003D0A0(lbl_80281D88->pSequences[i].b4A, nClassBit) &&
                       DynamicCam_MatchHeightDiff(&lbl_80281D88->pSequences[i], nPlayer, fHeight) &&
                       fn_8003D140(&lbl_80281D88->pSequences[i])) {
                pSeq = &lbl_80281D88->pSequences[i];
                if (fDist >= pSeq->f24 && fDist <= pSeq->f28 && pSeq->nChoices > 0) {
                    anPicked[nPicked] = i;
                    nPicked++;
                }
            }
        }
    }
    if (nPicked == 0) {
        if (nDefault >= 0) {
            return &lbl_80281D88->pSequences[nDefault];
        }
        for (i = 0; i < lbl_80281D88->nSequences; i++) {
            if (DynamicCam_IsDefualtSeq(&lbl_80281D88->pSequences[i], nKind) &&
                lbl_80281D88->pSequences[i].nChoices > 0) {
                return &lbl_80281D88->pSequences[i];
            }
        }
        return NULL;
    }
    uRand = Misc_RandFunc(1);
    fTotal = 0.0f;
    for (i = 0; i < nPicked; i++) {
        fTotal += lbl_80281D88->pSequences[anPicked[i]].f34;
    }
    nRoll = uRand % (int)(1000.0f * fTotal);
    for (i = 0; i < nPicked; i++) {
        if ((f32)(nRoll - nSum) < 1000.0f * lbl_80281D88->pSequences[anPicked[i]].f34) {
            return &lbl_80281D88->pSequences[anPicked[i]];
        }
        nSum += (int)(1000.0f * lbl_80281D88->pSequences[anPicked[i]].f34);
    }
    return &lbl_80281D88->pSequences[anPicked[0]];
}

// DynamicCam_ChooseSequence's pick for the pre-flight cameras: state nKind (3 becomes 28 when an
// object or hazard is within CamTuning.f16C of the ball, and 3 again for the last fallback), the
// ball's distance to the pin (fn_800D0478) within f24..f28 (fn_8003D0BC) in place of a shot
// distance, no end lie, generic shot types allowed.
CamSequence* DynamicCam_ChoosePreFlightSequence(int nPlayer, int nLie, int nKind) {
    int anPicked[50];
    CourseInfo* pCourse;
    int i;
    int nPicked;
    int nSum;
    int nDefault;
    int nTee;
    int nPinSet;
    u32 uRand;
    int nRoll;
    f32 fPinDist;
    f32 fHeight;
    f32 fTotal;

    nPicked = 0;
    nSum = 0;
    nDefault = -1;
    if (Player_OnTee(nPlayer)) {
        nTee = 1;
    } else {
        nTee = DynamicCam_BallLieToCameraLie(nLie, nPlayer);
    }
    fPinDist = fn_800D0478(nPlayer);
    nPinSet = Game_CurrentPinSet();
    pCourse = Ter_GetTGD();
    if (pCourse == NULL) {
        return NULL;
    }
    fHeight = pCourse->pin[nPinSet].y - gPlayers[nPlayer].vBall[1];
    if (nKind == 3 && Ter_CheckObjectAndHazardObstruction(gPlayers[nPlayer].ball.vPos, gpCamTuning->f16C, 1,
                                                          0, 0.0f, 1, gpCamTuning->f1A4)) {
        nKind = 28;
    }
    for (i = 0; i < lbl_80281D88->nSequences; i++) {
        if (nPicked >= 50) {
            break;
        }
        if (lbl_80281D88->pSequences[i].nChoices > 0) {
            if (DynamicCam_IsDefualtSeq(&lbl_80281D88->pSequences[i], nKind) &&
                fn_8003D00C(&lbl_80281D88->pSequences[i])) {
                nDefault = i;
            } else if (fn_8003D0EC(&lbl_80281D88->pSequences[i], nKind) &&
                       DynamicCam_MatchSeqClub(&lbl_80281D88->pSequences[i], nPlayer) &&
                       fn_8003CD9C(&lbl_80281D88->pSequences[i], nPlayer, 1) &&
                       fn_8003CEEC(&lbl_80281D88->pSequences[i], nPlayer) &&
                       fn_8003D0BC(&lbl_80281D88->pSequences[i], nPlayer, fPinDist) &&
                       fn_8003D00C(&lbl_80281D88->pSequences[i]) &&
                       fn_8003D054(&lbl_80281D88->pSequences[i]) &&
                       DynamicCam_MatchHeightDiff(&lbl_80281D88->pSequences[i], nPlayer, fHeight) &&
                       fn_8003D140(&lbl_80281D88->pSequences[i]) &&
                       fn_8003D0A0(lbl_80281D88->pSequences[i].b49, nTee)) {
                anPicked[nPicked] = i;
                nPicked++;
            }
        }
    }
    if (nPicked == 0) {
        if (nDefault >= 0) {
            return &lbl_80281D88->pSequences[nDefault];
        }
        if (nKind == 28) {
            nKind = 3;
        }
        for (i = 0; i < lbl_80281D88->nSequences; i++) {
            if (DynamicCam_IsDefualtSeq(&lbl_80281D88->pSequences[i], nKind) &&
                lbl_80281D88->pSequences[i].nChoices > 0) {
                return &lbl_80281D88->pSequences[i];
            }
        }
        return NULL;
    }
    uRand = Misc_RandFunc(1);
    fTotal = 0.0f;
    for (i = 0; i < nPicked; i++) {
        fTotal += lbl_80281D88->pSequences[anPicked[i]].f34;
    }
    nRoll = uRand % (int)(1000.0f * fTotal);
    for (i = 0; i < nPicked; i++) {
        if ((f32)(nRoll - nSum) < 1000.0f * lbl_80281D88->pSequences[anPicked[i]].f34) {
            return &lbl_80281D88->pSequences[anPicked[i]];
        }
        nSum += (int)(1000.0f * lbl_80281D88->pSequences[anPicked[i]].f34);
    }
    return &lbl_80281D88->pSequences[anPicked[0]];
}

// The anim pair named szName (an animation's name, case ignored) gives a sequence (*ppSeq) or a
// shot (*ppShot), returning 1: its p20 39 times in 100 when that has shot choices, else by its
// kind: 14 one of p14, p18 and p1C at random (p1C when the pick is missing), 13 its shot. 0 when no
// pair gives one.
u8 DynamicCamSearchForPairedSequence(char* szName, CamSequence** ppSeq, CamShot** ppShot) {
    int i;
    u32 nPick;

    for (i = 0; i < lbl_80281D88->nSets; i++) {
        if (stricmp(lbl_80281D88->pSets[i].szName, szName) != 0) {
            continue;
        }
        if (lbl_80281D88->pSets[i].p20 != NULL && lbl_80281D88->pSets[i].p20->nChoices > 0 &&
            Misc_RandFunc(1) % 100 > 60) {
            *ppSeq = lbl_80281D88->pSets[i].p20;
            return 1;
        }
        if (lbl_80281D88->pSets[i].nKind == 14) {
            nPick = Misc_RandFunc(1) % 3;
            if (nPick == 0 && lbl_80281D88->pSets[i].p14 != NULL) {
                *ppSeq = lbl_80281D88->pSets[i].p14;
                return 1;
            }
            if (nPick == 1 && lbl_80281D88->pSets[i].p18 != NULL) {
                *ppSeq = lbl_80281D88->pSets[i].p18;
                return 1;
            }
            if (lbl_80281D88->pSets[i].p1C != NULL) {
                *ppSeq = lbl_80281D88->pSets[i].p1C;
                return 1;
            }
        } else if (lbl_80281D88->pSets[i].nKind == 13) {
            *ppShot = lbl_80281D88->pSets[i].pShot;
            return 1;
        }
    }
    return 0;
}

// The sequence or shot paired with the golfer's animation (DynamicCamSearchForPairedSequence): with
// b (TW07's checkReaction) the clip in Character.pReactionClip when there is one, else the clip it
// is playing. While the GameBreaker letterbox is up (fn_8003DCAC) the name with "LB" in front is
// tried first. 1 when one is found; *ppSeq and *ppShot are cleared first, and a NULL out pointer
// gives 0.
u8 DynamicCam_ChoosePairedSequenceOrCamera(int nPlayer, u8 b, CamSequence** ppSeq, CamShot** ppShot) {
    char szName[0x18];          // the frame allows 12 to 24 bytes; the true size is unknown
    char* pName = NULL;

    if (ppSeq == NULL || ppShot == NULL) {
        return 0;
    }
    *ppSeq = NULL;
    *ppShot = NULL;
    if (b && gPlayers[nPlayer].pChar->pReactionClip != NULL) {
        pName = gPlayers[nPlayer].pChar->pReactionClip->name;
    }
    if (pName == NULL && gPlayers[nPlayer].pChar->pCurClip != NULL) {
        pName = gPlayers[nPlayer].pChar->pCurClip->name;
    }
    if (pName != NULL) {
        if (fn_8003DCAC()) {
            sprintf(szName, "LB%s", pName);
            if (DynamicCamSearchForPairedSequence(szName, ppSeq, ppShot) == 1) {
                return 1;
            }
        }
        if (DynamicCamSearchForPairedSequence(pName, ppSeq, ppShot) == 1) {
            return 1;
        }
    }
    return 0;
}

// A default sequence for state nKind: one of that state (fn_8003D0EC) whose name starts with "DEF".
// 0 for NULL.
u8 DynamicCam_IsDefualtSeq(CamSequence* pSequence, int nKind) {
    if (pSequence == NULL) return 0;
    if (!fn_8003D0EC(pSequence, nKind)) return 0;
    if (pSequence->szName[0] != 'D') return 0;
    if (pSequence->szName[1] != 'E') return 0;
    if (pSequence->szName[2] != 'F') return 0;
    return 1;
}

s32 lbl_80187988[20] = {0, 1, 2, 6, 2, 3, 4, 5, 3, 3, 3, 3, 6, 3, 3, 3, 5, 3, 6, 7};
s32 lbl_801879D8[17] = {1, 2, 2, 3, 3, 3, 4, 4, 4, 6, 2, 3, 6, 5, 5, 5, 7};

// The camera lie for end lie n (0..19), from gMaterialToCamLies; 0 for any other n. Lie 1 depends
// on the ball: 1 when it lies on lie 0, else 2.
s32 DynamicCam_MaterialToCameraLie(u32 n, int nPlayer) {
    s32 nRet;

    if (n > 19) return 0;
    if (n == 1) {
        nRet = 2;
        if (gPlayers[nPlayer].ball.nLie == 0) {
            nRet = 1;
        }
        return nRet;
    }
    return lbl_80187988[n];
}

// The camera lie for ball lie n, from gBallLieToCamLies (17 entries; n is not checked). nPlayer is
// not used.
s32 DynamicCam_BallLieToCameraLie(int n, int nPlayer) {
    return lbl_801879D8[n];
}

// The sequence suits the player's club: b45 picks the clubs (0 any, 1 woods, 2 5..9 irons,
// 3 1..5 irons, 4 wedges, 5 putter, 6 woods and irons, 7 woods and 1..5 irons, 8 all but the
// putter, 9 5 iron to the wedges, 10 irons and wedges).
u8 DynamicCam_MatchSeqClub(CamSequence* pSequence, int nPlayer) {
    int nClub = gPlayers[nPlayer].nClub;

    switch (pSequence->b45) {
    case 0:
        return 1;
    case 1:
        if (nClub >= CLUB_DRIVER1_e && nClub <= CLUB_7WOOD_e) return 1;
        return 0;
    case 2:
        if (nClub >= CLUB_5IRON_e && nClub <= CLUB_9IRON_e) return 1;
        return 0;
    case 3:
        if (nClub >= CLUB_1IRON_e && nClub <= CLUB_5IRON_e) return 1;
        return 0;
    case 4:
        if (nClub >= CLUB_PITCHINGWEDGE_e && nClub <= CLUB_HIGHLOBWEDGE_e) return 1;
        return 0;
    case 5:
        if (nClub >= CLUB_PUTTER_e && nClub <= CLUB_PUTTER_e) return 1;
        return 0;
    case 6:
        if (nClub >= CLUB_DRIVER1_e && nClub <= CLUB_9IRON_e) return 1;
        return 0;
    case 8:
        if (nClub >= CLUB_DRIVER1_e && nClub <= CLUB_HIGHLOBWEDGE_e) return 1;
        return 0;
    case 9:
        if (nClub >= CLUB_5IRON_e && nClub <= CLUB_HIGHLOBWEDGE_e) return 1;
        return 0;
    case 10:
        if (nClub >= CLUB_1IRON_e && nClub <= CLUB_HIGHLOBWEDGE_e) return 1;
        return 0;
    case 7:
        if (nClub >= CLUB_DRIVER1_e && nClub <= CLUB_5IRON_e) return 1;
        return 0;
    default:
        return 0;
    }
}

// The pin's height over the ball (f) is within the sequence's f2C..f30. nPlayer is not used.
u8 DynamicCam_MatchHeightDiff(CamSequence* pSequence, int nPlayer, f32 f) {
    if (f <= pSequence->f30 && f >= pSequence->f2C) {
        return 1;
    }
    return 0;
}

// The sequence suits the player's shot: b46 picks the shot kind (2..8: kinds 1..7, 10: kind 0,
// 11: kind 1, 12: any but 0 and 1) or asks for b (0 always, 1 on lie 0, 13 on any other lie).
u8 fn_8003CD9C(CamSequence* pSequence, int nPlayer, u8 b) {
    int nKind = gPlayers[nPlayer].nShotKind;

    switch (pSequence->b46) {
    case 0:
        return b != 0;
    case 1:
        if (gPlayers[nPlayer].ball.nLie == 0) {
            return b != 0;
        }
        return 0;
    case 2:
        return nKind == 1;
    case 3:
        return nKind == 2;
    case 4:
        return nKind == 3;
    case 5:
        return nKind == 4;
    case 6:
        return nKind == 5;
    case 7:
        return nKind == 6;
    case 8:
        return nKind == 7;
    case 10:
        return nKind == 0;
    case 11:
        return nKind == 1;
    case 12:
        if (nKind != 1 && nKind != 0) return 1;
        return 0;
    case 13:
        if (gPlayers[nPlayer].ball.nLie == 0) return 0;
        return b != 0;
    default:
        return 0;
    }
}

// The sequence suits who is playing (b47: see CamSequence).
u8 fn_8003CEEC(CamSequence* pSequence, int nPlayer) {
    switch (pSequence->b47) {
    case 0:
        if (!gSession.bReplay && !Player_IsCPU(nPlayer)) return 1;
        return 0;
    case 1:
        if (!gSession.bReplay && Player_IsCPU(nPlayer)) return 1;
        return 0;
    case 2:
        if (gSession.bReplay) return 0;
        return 1;
    case 3:
        return gSession.bReplay != 0;
    case 4:
        if (gSession.bReplay || Player_IsCPU(nPlayer)) return 1;
        return 0;
    default:
        return 0;
    }
}

// The sequence is used on the current course.
u8 fn_8003D00C(CamSequence* pSequence) {
    return (pSequence->uCourses & (1 << Game_GetCourse())) != 0;
}

// The sequence is used for Course_GetCurHolePar's current value.
u8 fn_8003D054(CamSequence* pSequence) {
    return (pSequence->n4B & (1 << Course_GetCurHolePar())) != 0;
}

// Bit nBit of the mask is set.
u8 fn_8003D0A0(int nMask, int nBit) {
    return (nMask & (1 << nBit)) != 0;
}

// The value is within the sequence's f24..f28. nPlayer is not used (every caller passes it).
u8 fn_8003D0BC(CamSequence* pSequence, int nPlayer, f32 f) {
    if (f >= pSequence->f24 && f <= pSequence->f28) {
        return 1;
    }
    return 0;
}

// The sequence is of the kind: kind 14 takes any sequence, kind 10 any of kinds 6..9.
u8 fn_8003D0EC(CamSequence* pSequence, int nKind) {
    if (nKind == 14) return 1;
    if (nKind == 10) {
        switch (pSequence->b44) {
        case 6:
        case 7:
        case 8:
        case 9:
            return 1;
        }
        return 0;
    }
    return pSequence->b44 == nKind;
}

// The sequence suits the game mode and screen (b48: see CamSequence).
u8 fn_8003D140(CamSequence* pSequence) {
    int nMode = Game_GetMode();

    if (pSequence == NULL) return 0;
    if (pSequence->b48 == 0) {
        if (gSession.nSplitScreen) return 0;
        if (nMode == 9) return 0;
        if (nMode == 11) return 0;
        if (GM_Currently_SkillZoneMode()) return 0;
        return 1;
    }
    if (pSequence->b48 == 1) {
        if (gSession.nSplitScreen) return 1;
        if (nMode == 9) return 1;
        return nMode == 11;
    }
    if (pSequence->b48 == 2) {
        return GM_Currently_SkillZoneMode() != 0;
    }
    return 0;
}

// The shot is of the kind: kind 14 takes any shot, kind 10 any of kinds 6..9.
u8 fn_8003D240(CamShot* pShot, int nKind) {
    if (nKind == 14) return 1;
    if (nKind == 10) {
        switch (pShot->bAD) {
        case 6:
        case 7:
        case 8:
        case 9:
            return 1;
        }
        return 0;
    }
    return pShot->bAD == nKind;
}

// The shot may be used: always outside game type 3; there (the CrAP screen) only while a golfer
// is being edited and the shot's u50/u54 bit for that golfer's nGolferId is set.
u8 fn_8003D294(CamShot* pShot) {
    CrAPGolfer* pGolfer;
    int n;

    if (gSession.nGameType != 3) return 1;
    pGolfer = gpCrAPState->pB4;
    if (pGolfer != NULL && pGolfer->pChar != NULL) {
        n = pGolfer->nGolferId;
        if (n <= 32) {
            // EA bug: n == 32 shifts by 32 (undefined in C; the PowerPC gives 0)
            return (pShot->u.bits.u50 & (1 << n)) != 0;
        }
        return (pShot->u.bits.u54 & (1 << (n - 32))) != 0;
    }
    return 0;
}

// Moves pPos fSide sideways, square to pDir on the level, then hands it on to DynamicCam_AddHeightOffset.
void fn_8003D324(f32* pPos, f32* pDir, CamScript* pScript, CamShot* pShot, int nPlayer, f32 fSide, f32 fY) {
    f32 vDir[4];

    Vec3Copy(pDir, vDir);
    vDir[1] = 0.0f;
    if (0.0f != vDir[0] || 0.0f != vDir[1] || 0.0f != vDir[2]) {
        LLMath_Normalize3(vDir, vDir);
    }
    pPos[0] += fSide * -vDir[2];
    pPos[2] += fSide * vDir[0];
    DynamicCam_AddHeightOffset(pPos, pScript, pShot, nPlayer, fY);
}

// Sets the camera's height pPos[1] by the shot's bB2 (0: left alone): 1 halfway between bones 0x39
// and 0x47 of the golfer, 2 bone 1, 3 bone 0xA, 4 fY (the height before), 5 and 7 DynamicCam_GetSmoothBallLocation's
// point, 6 the pin; then the shot's f80 above it. Kind 7 moves on to that height from the script's
// v70 step by step, once per ball update this frame, easing by CamTuning.f22C while the ball rises
// and by f230 to f22C (between the shot's f68, at least 0.15, and the height kept in the script's
// f104) as it comes down; slower early in the script's move (f98 under CamTuning.fD4).
void DynamicCam_AddHeightOffset(f32* pPos, CamScript* pScript, CamShot* pShot, int nPlayer, f32 fY) {
    f32 vBone47[4];
    f32 vBone39[4];
    f32 vMid[4];
    f32 vBone1[4];
    f32 vBoneA[4];
    f32 vPoint[4];
    f32 vStep[4];
    f32 vOff[4];
    int nSteps;
    int nPin;
    CourseInfo* pCourse;
    int i;
    f32 fLow;
    f32 fHeight;
    f32 fEase;
    f64 dSmooth;

    nSteps = GameEffects_BallUpdatesThisFrame(nPlayer);
    if (pShot->bB2 == 0) {
        return;
    }
    switch (pShot->bB2) {
    case 1:
        Character_GetBonePos(gPlayers[nPlayer].pChar, 0x39, vBone39);
        Character_GetBonePos(gPlayers[nPlayer].pChar, 0x47, vBone47);
        fn_8003DC30(vBone39, vBone47, vMid);
        Vec3_Scale(0.5f, vMid, vMid);
        pPos[1] = vMid[1];
        break;
    case 2:
        Character_GetBonePos(gPlayers[nPlayer].pChar, 1, vBone1);
        pPos[1] = vBone1[1];
        break;
    case 3:
        Character_GetBonePos(gPlayers[nPlayer].pChar, 0xA, vBoneA);
        pPos[1] = vBoneA[1];
        break;
    case 4:
        pPos[1] = fY;
        break;
    case 5:
    case 7:
        DynamicCam_GetSmoothBallLocation(pScript, pShot, nPlayer, vPoint, 0);
        pPos[1] = vPoint[1];
        break;
    case 6:
        nPin = Game_CurrentPinSet();
        pCourse = Ter_GetTGD();
        if (pCourse != NULL) {
            pPos[1] = pCourse->pin[nPin].y;
        }
        break;
    }
    pPos[1] += pShot->f80;
    if (pShot->bB2 != 7) {
        return;
    }
    if (CameraScript_SnapToScript(pScript, pShot)) {
        pScript->f104 = pShot->f68;
        return;
    }
    if (nSteps == 0) {
        pPos[1] = fY;
        return;
    }
    for (i = 0; i < nSteps; i++) {
        fn_8003DC54(vPoint, pScript->v70, vOff);
        Vec3_Scale((f32)(i + 1) / (f32)nSteps, vOff, vOff);
        fn_8003DC30(pScript->v70, vOff, vStep);
        pPos[1] = vStep[1];
        pPos[1] += pShot->f80;
        fLow = pShot->f68;
        if (fLow <= 0.15f) {
            fLow = 0.15f;
        }
        if (!gPlayers[nPlayer].ball.bHitTopArc) {
            fEase = gpCamTuning->f22C;
            pScript->f104 = pPos[1] - pScript->fD8;
        } else {
            fHeight = pPos[1] - pScript->fD8;
            if (fHeight <= fLow || pScript->f104 < fLow) {
                fEase = gpCamTuning->f230;
            } else if (fHeight > pScript->f104) {
                fEase = gpCamTuning->f22C;
            } else {
                fEase = (gpCamTuning->f22C - gpCamTuning->f230) * ((fHeight - fLow) / pScript->f104) +
                        gpCamTuning->f230;
            }
        }
        if (pScript->f98 < gpCamTuning->fD4) {
            dSmooth = Math_Sqrt((f32)Math_Sqrt(pScript->f98 / gpCamTuning->fD4));
            fEase = 1.0f - (f32)dSmooth * (1.0f - fEase);
        }
        pPos[1] = fEase * (pPos[1] - fY) + fY;
        fY = pPos[1];
    }
}

// The sequence suits the player's club and shot kind.
u8 fn_8003D7A0(CamSequence* pSequence, int nPlayer) {
    if (pSequence == NULL) return 0;
    if (DynamicCam_MatchSeqClub(pSequence, nPlayer) && fn_8003CD9C(pSequence, nPlayer, 1)) {
        return 1;
    }
    return 0;
}

// Keeps the direction pDir within the tuning's f19C (an angle) of level: with no level part at
// all it becomes the direction from pA to pB; tilted further than f19C, it is turned back to that
// tilt. The result is normalised.
void fn_8003D810(f32* pDir, f32* pA, f32* pB) {
    f32 vLevel[4];
    f32 vAxis[4];
    f32 qTurn[4];

    if (0.0f == pDir[0] && 0.0f == pDir[2]) {
        fn_8003DC54(pB, pA, pDir);
        if (0.0f != pDir[0] || 0.0f != pDir[1] || 0.0f != pDir[2]) {
            LLMath_Normalize3(pDir, pDir);
        }
        return;
    }
    Vec3Copy(pDir, vLevel);
    vLevel[1] = 0.0f;
    if (0.0f != vLevel[0] || 0.0f != vLevel[1] || 0.0f != vLevel[2]) {
        LLMath_Normalize3(vLevel, vLevel);
    }
    if (fabsf(Math_Acos(Vec3_Dot(vLevel, pDir))) > gpCamTuning->f19C) {
        vec4flt_CrossProduct(pDir, vLevel, vAxis);
        if (0.0f != vAxis[0] || 0.0f != vAxis[1] || 0.0f != vAxis[2]) {
            LLMath_Normalize3(vAxis, vAxis);
        }
        Vec3_Scale(gpCamTuning->f19C, vAxis, vAxis);
        Quat_BuildFromVector(vAxis, qTurn);
        vLevel[3] = 0.0f;
        Quat_RotateVector(qTurn, vLevel, pDir);
        if (0.0f != pDir[0] || 0.0f != pDir[1] || 0.0f != pDir[2]) {
            LLMath_Normalize3(pDir, pDir);
        }
    }
}

// The ball's position into pOut; but when the ball is in the cup (lie 12) or on surface 0x62 or
// 0x69, more than 0.005 below the pin and within 2 of the script's v70, v70 instead. With bKeep,
// v70 takes the ball's position whenever it is not used.
void DynamicCam_GetSmoothBallLocation(CamScript* pScript, CamShot* pShot, int nPlayer, f32* pOut, u8 bKeep) {
    u8 bNear = 0;
    CourseInfo* pCourse = Ter_GetTGD();
    int nPin = Game_CurrentPinSet();

    if ((gPlayers[nPlayer].ball.nSurface == 0x62 || gPlayers[nPlayer].ball.nSurface == 0x69
         || gPlayers[nPlayer].ball.nLie == 12)
        && pCourse != NULL) {
        if (pCourse->pin[nPin].y - gPlayers[nPlayer].ball.vPos[1] > 0.005f
            && LLMath_DistanceBetween3(pScript->v70, gPlayers[nPlayer].ball.vPos) < 2.0f) {
            bNear = 1;
        }
    }
    if (bNear) {
        Vec3Copy(pScript->v70, pOut);
    } else {
        Vec3Copy(gPlayers[nPlayer].ball.vPos, pOut);
    }
    if (bKeep && !bNear) {
        Vec3Copy(gPlayers[nPlayer].ball.vPos, pScript->v70);
    }
}

// The shot's f64 and f60 into *pA and *pB; when CameraScript_FlipCameraForLefty holds for the
// player one of them changes sign: (-f64, f60), or (f64, -f60) for a shot with bAF or bB0 set to 21.
void fn_8003DAC8(CamShot* pShot, int nPlayer, f32* pA, f32* pB) {
    if (pShot != NULL) {
        if (CameraScript_FlipCameraForLefty(nPlayer, pShot)) {
            if (pShot->bAF == 21 || pShot->bB0 == 21) {
                if (pA != NULL) {
                    *pA = pShot->f64;
                }
                if (pB != NULL) {
                    *pB = -pShot->f60;
                }
            } else {
                if (pA != NULL) {
                    *pA = -pShot->f64;
                }
                if (pB != NULL) {
                    *pB = pShot->f60;
                }
            }
        } else {
            if (pA != NULL) {
                *pA = pShot->f64;
            }
            if (pB != NULL) {
                *pB = pShot->f60;
            }
        }
    }
}

// Scales the value by 10 on three holes: course 9's hole 12 and course 3's holes 13 and 15.
f32 fn_8003DBA8(f32 f) {
    if (Game_GetCourse() == 9 && Game_GetCurHoleNum() == 12) {
        return 10.0f * f;
    }
    if (Game_GetCourse() == 3 && (Game_GetCurHoleNum() == 13 || Game_GetCurHoleNum() == 15)) {
        return 10.0f * f;
    }
    return f;
}

// a + b into out (three floats)
#ifdef __MWERKS__
asm void fn_8003DC30(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_8003DC30(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
}
#endif

// a - b into out (three floats)
#ifdef __MWERKS__
asm void fn_8003DC54(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_8003DC54(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

u8 fn_8003DC78(CamShot* pShot) {
    u8 nKind = pShot->bAC;

    if (nKind == 1 || (u8)(nKind - 2) <= 4U || nKind == 7) {
        return 1;
    }
    return 0;
}

// The GameBreaker letterbox is up, for a predicted GameBreaker or while b19 is set.
u8 fn_8003DCAC(void) {
    int bResult = 0;

    if (gGameEffects.bGameBreaker && (gGameEffects.nGBType != 0 || gGameEffects.b19 == 1)) {
        bResult = 1;
    }
    return bResult;
}
