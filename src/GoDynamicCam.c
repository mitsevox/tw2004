// GoDynamicCam.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the dynamic
// cameras. Loads the camera shots, sequences and shot choices from the camera files into
// lbl_80281D88's tables, and picks the sequence and shot that fit a golfer's situation (club,
// shot kind, course and hole, game mode).

#include "golfer.h"
#include "game.h"
#include "dyncam.h"
#include "frontend/fe.h"
#include "endian.h"

u8   fn_8001E9CC(u32* pBits, int nBit);         // the bit is set
void fn_80039884(u8* pSrc, u8* pDst, int nCount);
void fn_800399E0(u8* pSrc, CamShot* pDst, u32 nCount);
void fn_80039A48(u8* pSrc, DynCamSet* pDst, u32 nCount);
void fn_80039B14(int nSize);
void fn_80039C5C(int nSize);
void fn_80039D0C(int nSequences);
void fn_80039E58(void);
u8   fn_8003C800(char* szName, CamSequence** ppSeq, CamShot** ppShot);
void fn_8003DC30(f32* pA, f32* pB, f32* pOut);         // a + b
void fn_8003DC54(f32* pA, f32* pB, f32* pOut);
void fn_800090E4(f32* pTurn, f32* pVec, f32* pOut);     // the vector turned by it
void fn_80039EB8(int nSize);
u8   fn_8003D0EC(CamSequence* pSequence, int nKind);
u8   fn_8003D240(CamShot* pShot, int nKind);
u8   fn_8003D294(CamShot* pShot);
void fn_8003954C(void);
void fn_80039550(void);
void fn_80039554(UStreamObject* pObject);
void fn_80039690(UStreamObject* pObject);
void fn_80039754(UStreamObject* pObject);
void fn_800397EC(UStreamObject* pObject);
void DynamicCam_GetLocation(int nKind, int nPlayer, f32* pOut, CamScript* pScript, CamShot* pShot, f32* pCam,
                            f32* pSub);
void fn_8001EB8C(Character* pChar, int nBone, f32* pPos);   // char.c: a bone's position
void fn_8003D324(f32* pPos, f32* pDir, CamScript* pScript, CamShot* pShot, int nPlayer, f32 fSide, f32 fY);
void fn_8003D414(f32* pPos, CamScript* pScript, CamShot* pShot, int nPlayer, f32 fY);
void fn_8003AC50(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pCam, f32* pSub);
void fn_8003ADF8(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pCam, f32* pSub);
void fn_8003B028(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pCam, f32* pSub,
                 f32 f);
void fn_8003B534(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pCam, f32* pSub);
void fn_8003B6D0(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pCam, f32* pSub);
f32  fn_8003DBA8(f32 f);
void fn_8003D810(f32* pDir, f32* pA, f32* pB);
u8   fn_8003CAFC(CamSequence* pSequence, int nKind);
s32  fn_8003CB80(u32 n, int nPlayer);
s32  fn_8003CBD4(int n, int nPlayer);
u8   fn_8003CBE8(CamSequence* pSequence, int nPlayer);
u8   fn_8003CD6C(CamSequence* pSequence, int nPlayer, f32 f);
u8   fn_8003CD9C(CamSequence* pSequence, int nPlayer, u8 b);
u8   fn_8003CEEC(CamSequence* pSequence, int nPlayer);
u8   fn_8003D00C(CamSequence* pSequence);
u8   fn_8003D054(CamSequence* pSequence);
u8   fn_8003D0A0(int nMask, int nBit);
u8   fn_8003D140(CamSequence* pSequence);
u8   fn_8003D0BC(CamSequence* pSequence, int nPlayer, f32 f);
u8   fn_8003AB94(CamChoice* pChoice);
u8   fn_8003ABEC(CamChoice* pChoice, int nPlayer);
void fn_8003DAC8(CamShot* pShot, int nPlayer, f32* pA, f32* pB);
f32  fn_80044F58(int nPlayer, CamScript* pScript);     // gocamscripts.c

// Registers the handlers of the camera files ('CAMS', 'CAMV', 'CAMA').
void fn_80039454(void) {
    UStream_RegisterHandler('CAMS', fn_80039554);
    UStream_RegisterHandler('CAMV', fn_80039690);
    UStream_RegisterHandler('CAMA', fn_800397EC);
}

void fn_800394AC(void) {
    UStream_UnregisterHandler('CAMS');
    UStream_UnregisterHandler('CAMV');
    UStream_UnregisterHandler('CAMA');
    fn_8003954C();
}

// The same for the other 'CAMV' handler alone.
void fn_800394F0(void) {
    UStream_RegisterHandler('CAMV', fn_80039754);
}

void fn_80039520(void) {
    UStream_UnregisterHandler('CAMV');
    fn_80039550();
}

void fn_8003954C(void) {
}

void fn_80039550(void) {
}

// The stream handler for the sequence file: two counts (sequences, then choices), then the
// sequences; takes them unless some are loaded already, and makes room for the choices.
void fn_80039554(UStreamObject* pObject) {
    s32 nSequences;
    s32 nChoices;
    u8* pSrc;

    lbl_80281D88->n1C++;
    if (lbl_80281D88->n1C > 2) {
        lbl_80281D88->n1C = 1;
    }
    if (lbl_80281D88->pSequences != NULL) {
        fn_80009E70(pObject);
        return;
    }
    pSrc = pObject->pData;
    fn_80076158(&pSrc, (u8*)&nSequences, sizeof(nSequences), 4);
    pSrc = pObject->pData + 4;
    fn_80076158(&pSrc, (u8*)&nChoices, sizeof(nChoices), 4);
    lbl_80281D88->pSequences = fn_80009B34(nSequences * sizeof(CamSequence), 2, 0, "GoDynamicCam.c", 403);
    lbl_80281D88->pChoices = fn_80009B34(nChoices * sizeof(CamChoice), 2, 0, "GoDynamicCam.c", 404);
    lbl_80281D88->nSequences = 0;
    lbl_80281D88->nChoicesUsed = 0;
    pSrc = pObject->pData + 8;
    fn_80039884(pSrc, (u8*)lbl_80281D88->pSequences, nSequences);
    fn_80039D0C(nSequences);
    fn_80039E58();
    fn_80009E70(pObject);
}

// The stream handler for the shot file: takes the shots unless some are loaded already.
void fn_80039690(UStreamObject* pObject) {
    lbl_80281D88->n1C++;
    if (lbl_80281D88->n1C > 2) {
        lbl_80281D88->n1C = 1;
    }
    if (lbl_80281D88->pShots != NULL) {
        fn_80009E70(pObject);
        return;
    }
    lbl_80281D88->pShots = fn_80009B34(pObject->uSize, 2, 0, "GoDynamicCam.c", 454);
    fn_800399E0(pObject->pData, lbl_80281D88->pShots, pObject->uSize / sizeof(CamShot));
    fn_80039B14(pObject->uSize);
    fn_80009E70(pObject);
}

// The stream handler for another shot file: the same, but the load is not counted and the shots
// are not clamped (fn_80039C5C).
void fn_80039754(UStreamObject* pObject) {
    if (lbl_80281D88->pShots != NULL) {
        fn_80009E70(pObject);
        return;
    }
    lbl_80281D88->pShots = fn_80009B34(pObject->uSize, 2, 0, "GoDynamicCam.c", 495);
    fn_800399E0(pObject->pData, lbl_80281D88->pShots, pObject->uSize / sizeof(CamShot));
    fn_80039C5C(pObject->uSize);
    fn_80009E70(pObject);
}

// The stream handler for the file of sets (a shot and four sequences each).
void fn_800397EC(UStreamObject* pObject) {
    if (lbl_80281D88->pSets != NULL) {
        fn_80009E70(pObject);
        return;
    }
    lbl_80281D88->pSets = fn_80009B34(pObject->uSize, 2, 0, "GoDynamicCam.c", 536);
    fn_80039A48(pObject->pData, lbl_80281D88->pSets, pObject->uSize / sizeof(DynCamSet));
    fn_80039EB8(pObject->uSize);
    fn_80009E70(pObject);
}

// Copies nCount sequences from the file (little-endian) into pDst, swapping each value's bytes.
// Each is followed in the file by its shot choices, which go into the choice block in turn; the
// sequence's p4C gets the first of them. The file keeps a word where p4C goes.
void fn_80039884(u8* pSrc, u8* pDst, int nCount) {
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
        fn_8001F08C((void**)&pSrc, (void**)&pDst, aSequence, sizeof(aSequence) / sizeof(aSequence[0]), 1);
        // pDst is now at the sequence's p4C
        *(CamChoice**)pDst = &lbl_80281D88->pChoices[lbl_80281D88->nChoicesUsed];
        pSrc += sizeof(CamChoice*);
        pDst += sizeof(CamChoice*);
        pChoice = &lbl_80281D88->pChoices[lbl_80281D88->nChoicesUsed];
        fn_8001F08C((void**)&pSrc, (void**)&pChoice, aChoice, sizeof(aChoice) / sizeof(aChoice[0]),
                    lbl_80281D88->pSequences[i].nChoices);
        lbl_80281D88->nChoicesUsed += lbl_80281D88->pSequences[i].nChoices;
    }
    lbl_80281D88->nSequences = nCount;
}

// Copies nCount shots from the file (little-endian) into pDst, swapping each value's bytes.
void fn_800399E0(u8* pSrc, CamShot* pDst, u32 nCount) {
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

    fn_8001F08C((void**)&pSrc, (void**)&pDst, aFormat, sizeof(aFormat) / sizeof(aFormat[0]), nCount);
}

// The same for nCount shot sets.
void fn_80039A48(u8* pSrc, DynCamSet* pDst, u32 nCount) {
    SwapField aFormat[] = {
        { 16, 1 },
        { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
        { 1, 1 },
        { 3, 1 },
    };

    fn_8001F08C((void**)&pSrc, (void**)&pDst, aFormat, sizeof(aFormat) / sizeof(aFormat[0]), nCount);
}

// Sets up nSize bytes of freshly loaded shots, as fn_80039C5C does, and first keeps f68 at least
// the ground clearance, f6C at least f68 and f8C within 0..0.49. Once the sequences are loaded
// too, their choices are checked (fn_80039D0C).
void fn_80039B14(int nSize) {
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
        if (lbl_80281D88->pShots[i].f68 < lbl_80281F78->f168) {
            lbl_80281D88->pShots[i].f68 = lbl_80281F78->f168;
        }
        if (lbl_80281D88->pShots[i].f6C < lbl_80281D88->pShots[i].f68) {
            lbl_80281D88->pShots[i].f6C = lbl_80281D88->pShots[i].f68;
        }
        lbl_80281D88->pShots[i].f8C = (lbl_80281D88->pShots[i].f8C < 0.0f) ? 0.0f
            : ((lbl_80281D88->pShots[i].f8C > 0.49f) ? 0.49f : lbl_80281D88->pShots[i].f8C);
        lbl_80281D88->pShots[i].f7C = lbl_80281D88->pShots[i].f78;
    }
    if (lbl_80281D88->n1C == 2) {
        fn_80039D0C(lbl_80281D88->nSequences);
    }
}

// Sets up nSize bytes of freshly loaded shots: turns each shot's follow-on index (p40) into a
// pointer, NULL when it names the shot itself, and marks the follow-on; f6C and f7C start at f68
// and f78.
void fn_80039C5C(int nSize) {
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

// Once both camera files are in (n1C is 2): turns each shot choice's shot index into a pointer, and
// sets its b16 to 25 unless it is 13..22 and some choice of the same sequence is for that shot kind.
void fn_80039D0C(int nSequences) {
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
void fn_80039E58(void) {
    int i;

    for (i = 0; i < lbl_80281D88->nSequences; i++) {
        // port: the file keeps an index in the pointer field
        lbl_80281D88->pSequences[i].p20 = &lbl_80281D88->pSequences[(s32)lbl_80281D88->pSequences[i].p20];
        if (lbl_80281D88->pSequences[i].p20->nChoices <= 0) {
            lbl_80281D88->pSequences[i].p20 = &lbl_80281D88->pSequences[i];
        }
    }
}

// Sets up nSize bytes of freshly loaded sets: each shot and sequence index becomes a pointer
// (NULL for a negative index).
void fn_80039EB8(int nSize) {
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

// Allocates the dynamic cameras' tables, empty.
void fn_80039FF8(void) {
    DynCamTables* pTables = fn_80009B34(sizeof(DynCamTables), 2, 0, "GoDynamicCam.c", 938);

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

// Frees the dynamic cameras' tables.
void fn_8003A074(void) {
    lbl_80281D88->nShots = 0;
    lbl_80281D88->nSequences = 0;
    lbl_80281D88->nSets = 0;
    lbl_80281D88->nChoicesUsed = 0;
    if (lbl_80281D88->pSequences != NULL) {
        fn_80009E70(lbl_80281D88->pSequences);
        lbl_80281D88->pSequences = NULL;
    }
    if (lbl_80281D88->pShots != NULL) {
        fn_80009E70(lbl_80281D88->pShots);
        lbl_80281D88->pShots = NULL;
    }
    if (lbl_80281D88->pSets != NULL) {
        fn_80009E70(lbl_80281D88->pSets);
        lbl_80281D88->pSets = NULL;
    }
    if (lbl_80281D88->pChoices != NULL) {
        fn_80009E70(lbl_80281D88->pChoices);
        lbl_80281D88->pChoices = NULL;
    }
    lbl_80281D88->n1C = 0;
    fn_80009E70(lbl_80281D88);
}

// Places a shot's camera at pOut by its placement kind (bB1), then keeps it above the ground under
// it (on course 12 when fn_80015464 is 10, within 40 of the tee, fn_8004D5F0's height): at least
// f68 above it, eased in under f6C. A kind 3 camera then moves a limited step from where it was.
void fn_8003A148(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pCam, f32* pSub,
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
        fn_8003AC50(pShot, nPlayer, pScript, pOut, pCam, pSub);
        break;
    case 2:
    case 3:
    case 4:
    case 8:
        fn_8003ADF8(pShot, nPlayer, pScript, pOut, pCam, pSub);
        break;
    case 5:
    case 6:
        fn_8003B028(pShot, nPlayer, pScript, pOut, pCam, pSub, f);
        break;
    case 7:
        fn_8003B534(pShot, nPlayer, pScript, pOut, pCam, pSub);
        break;
    case 9:
        fn_8003B6D0(pShot, nPlayer, pScript, pOut, pCam, pSub);
        break;
    }
    pCourse = fn_8000C594();
    if (pCourse != NULL) {
        if (fn_80043388(pScript, pShot) && pShot != pScript->pNextShot) {
            if (Game_GetCourse() == 12 && fn_80015464() == 10) {
                fn_8003DC54(&pCourse->tee[gSession.nTeeSet[nPlayer]].x, pOut, aOff);
                aOff[1] = 0.0f;
                if ((f32)fn_80009680(fn_80009744(aOff)) < 40.0f) {
                    fGround = fn_8004D5F0(pCourse, pOut);
                } else {
                    fGround = Terrain_HeightAt(pOut, NULL);
                    if (pShot->bAD == 4 && !fn_8003A76C(pShot) && !fn_8003DC78(pShot)) {
                        fLow = (pCourse->pin[nPinSet].y <= pCourse->tee[gSession.nTeeSet[nPlayer]].y)
                                   ? pCourse->pin[nPinSet].y
                                   : pCourse->tee[gSession.nTeeSet[nPlayer]].y;
                        fGround = (fLow <= fGround) ? fGround : fLow;
                    }
                }
                pScript->fD8 = fGround;
                if (pScript->fD8 < -60000.0f) {
                    pScript->fD8 = fn_8000C594()->fFloor;
                }
            } else {
                fGround = Terrain_HeightAt(pOut, NULL);
                if (pShot->bAD == 4 && !fn_8003A76C(pShot) && !fn_8003DC78(pShot)) {
                    fLow = (pCourse->pin[nPinSet].y <= pCourse->tee[gSession.nTeeSet[nPlayer]].y)
                               ? pCourse->pin[nPinSet].y
                               : pCourse->tee[gSession.nTeeSet[nPlayer]].y;
                    fGround = (fLow <= fGround) ? fGround : fLow;
                }
                pScript->fD8 = fGround;
                if (pScript->fD8 < -60000.0f) {
                    pScript->fD8 = fn_8000C594()->fFloor;
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
    if (fn_8003A76C(pShot) || fn_8003DC78(pShot)) {
        fDiff = gPlayers[nPlayer].vBall[1] - fGround;
        fLo = pShot->f68 + fDiff;
        fHi = pShot->f6C + fDiff;
        if (fLo < lbl_80281F78->f168) {
            fLo = lbl_80281F78->f168;
        }
        if (fHi < lbl_80281F78->f168) {
            fHi = lbl_80281F78->f168;
        }
    } else {
        fLo = pShot->f68;
        fHi = fn_8003DBA8(pShot->f6C);
    }
    if (pOut[1] - fGround < fLo) {
        if (pShot->bB1 == 5 || pShot->bB1 == 6 || pShot->bB1 == 7) {
            // fake match: a negated >=, where < gives a plain bge
            if (!(aOld[1] - fGround >= fLo)) {
                pOut[1] += lbl_80281F78->f194;
                if (pOut[1] - fGround > fLo) {
                    pOut[1] = fGround + fLo;
                }
            }
            if (pOut[1] - fGround < lbl_80281F78->f168) {
                pOut[1] = fGround + lbl_80281F78->f168;
            }
        } else {
            pOut[1] = fGround + fLo;
        }
    } else {
        fFollow = pScript->fD8;
        fEase = lbl_80281F78->f174;
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
    if (pShot->bB1 == 3 && !fn_80043388(pScript, pShot) &&
        (pScript->pNextShot != pShot || pScript->fCamTime > 0.0f)) {
        fn_8003DC54(pOut, aOld, aStep);
        fDist = (f32)fn_80009680(fn_80009744(aStep));
        if (aStep[0] != 0.0f || aStep[1] != 0.0f || aStep[2] != 0.0f) {
            fn_800BAF04(aStep, aDir);
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
        fn_8001EF34(aDir, fStep, aStep);
        fn_8003DC30(aOld, aStep, pOut);
    }
}

// The shot is one of kinds 1, 3, 13, 28..34 or 40..45.
u8 fn_8003A76C(CamShot* pShot) {
    u8 nKind = pShot->bAD;

    if (nKind == 3 || nKind == 1 || (nKind >= 29 && nKind <= 33) || (nKind >= 40 && nKind <= 45)
        || nKind == 13 || nKind == 28 || nKind == 34) {
        return 1;
    }
    return 0;
}

// A shot of the kind, picked at random from the first 50 that may be used now, are no other
// shot's follow-on and are not pShot; NULL when there is none.
CamShot* fn_8003A7C8(int nPlayer, int nKind, CamShot* pShot) {
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
CamShot* fn_8003A8C4(char* szName) {
    int i;

    for (i = 0; i < lbl_80281D88->nShots; i++) {
        if (stricmp(szName, lbl_80281D88->pShots[i].szName) == 0) {
            return &lbl_80281D88->pShots[i];
        }
    }
    return NULL;
}

// A shot of kind nKind from the sequence's choices (a choice for kind 9 fits any kind but 23),
// picked at random from the first 50 that may be used on this hole by this golfer; failing that,
// from the first 50 of the kind at all. Its blend kinds and times go to the out pointers that are
// not NULL. NULL when the sequence has none.
CamShot* fn_8003A950(CamSequence* pSequence, int nKind, int* pA, f32* pF1, f32* pF2, int* pB, f32* pF3,
                     int nPlayer) {
    int aPick[50];
    int i;
    int nCount = 0;
    u32 nPick;

    if (pSequence == NULL) return NULL;
    for (i = 0; i < pSequence->nChoices; i++) {
        if (nCount >= 50) break;
        if ((nKind == pSequence->p4C[i].b14 || (nKind != 23 && pSequence->p4C[i].b14 == 9))
            && fn_8003AB94(&pSequence->p4C[i]) && fn_8003ABEC(&pSequence->p4C[i], nPlayer)) {
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

// The choice may be used on the current hole.
u8 fn_8003AB94(CamChoice* pChoice) {
    if (fn_8001E9CC(pChoice->aNoHoles, Game_GetCourse() * 18 + fn_80015464())) return 0;
    return 1;
}

// The choice suits the player's golfer: a choice marked b17 is only for golfer models 0, 1, 8, 10,
// 12..15 and 17.
u8 fn_8003ABEC(CamChoice* pChoice, int nPlayer) {
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

// The camera's position for a shot between two points: DynamicCam_GetLocation's points for the
// shot's bAF and bB0, turned into a position by the shot's f60 and f64 (f64 the other way when
// fn_800453C8 holds; level unless bB1), then fn_8003D414. A shot with a point of kind 0 or 23
// waits for a frame in which the ball moves.
void fn_8003AC50(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pCam, f32* pSub) {
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
        if (fn_800453C8(nPlayer, pShot)) {
            fn_800C7D14(vFrom, vTo, 0, 1, pOut, pShot->f60, -pShot->f64);
        } else {
            fn_800C7D14(vFrom, vTo, 0, 1, pOut, pShot->f60, pShot->f64);
        }
    } else if (fn_800453C8(nPlayer, pShot)) {
        fn_800C7D14(vFrom, vTo, 1, 1, pOut, pShot->f60, -pShot->f64);
    } else {
        fn_800C7D14(vFrom, vTo, 1, 1, pOut, pShot->f60, pShot->f64);
    }
    fn_8003D414(pOut, pScript, pShot, nPlayer, fY);
}

// As fn_8003AC50, with the distance and side from fn_8003DAC8 (level for bB1 2 and 3). For bB1 8,
// the camera's offset from pSub is shortened as fn_80044F58's distance (never less than the most
// seen, pScript->f100) goes from CamTuning.f23C to f244, unless fn_80043388 holds.
void fn_8003ADF8(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pCam, f32* pSub) {
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
        fn_800C7D14(vFrom, vTo, 0, 0, pOut, fDist, fSide);
    } else {
        fn_8003DAC8(pShot, nPlayer, &fSide, &fDist);
        fn_800C7D14(vFrom, vTo, 1, 0, pOut, fDist, fSide);
    }
    if (pShot->bB1 == 8) {
        fFar = fn_80044F58(nPlayer, pScript);
        if (!fn_80043388(pScript, pShot)) {
            if (fFar > pScript->f100) {
                pScript->f100 = fFar;
            }
            fFar = pScript->f100;
            fn_8003DC54(pOut, pSub, vOff);
            vOff[1] = 0.0f;
            if (fFar > lbl_80281F78->f23C) {
                if (fFar > lbl_80281F78->f244) {
                    fScale = lbl_80281F78->f248;
                } else {
                    fScale = (fFar - lbl_80281F78->f23C) / (lbl_80281F78->f244 - lbl_80281F78->f23C);
                    fScale = lbl_80281F78->f240 - fScale * (lbl_80281F78->f240 - lbl_80281F78->f248);
                }
                fn_8001EF34(vOff, fScale, vOff);
                fn_8003DC30(vOff, pSub, pOut);
            }
        } else {
            pScript->f100 = fFar;
        }
    }
    fn_8003D414(pOut, pScript, pShot, nPlayer, fY);
}

// A camera that follows the ball (placement kinds 5 and 6; 6 stays level): its target is along the
// ball's direction from DynamicCam_GetLocation's kind 0 point, by the shot's f60 (changed as in
// fn_8003B534), moved sideways by f64. Unless fn_80043388 holds, pOut moves towards it by a share
// of the distance and the angle between them per frame, scaled while f98 is under CamTuning.f154,
// by f8C when nBC is 4, for a ball slower than f198 and early on the second clock (f88).
void fn_8003B028(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pCam, f32* pSub,
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
    f32 fY;
    f32 fHi;
    f32 fLo;
    f32 fHeight;
    f32 fDist;
    f32 fFrames;
    f32 fMove;
    f32 fTurn;
    f32 fEase;
    f32 fT;
    f32 fCurDist;
    f32 fGrow;
    f32 fRise;

    fY = pOut[1];
    DynamicCam_GetLocation(0, nPlayer, aFrom, pScript, pShot, pCam, pSub);
    Vec3Copy(gPlayers[nPlayer].ball.vVel, aBallDir);
    if (pShot->bB1 == 6) {
        aBallDir[1] = 0.0f;
    }
    if (aBallDir[0] != 0.0f || aBallDir[1] != 0.0f || aBallDir[2] != 0.0f) {
        fn_800BAF04(aBallDir, aBallDir);
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
        fDist = lbl_80281F78->f18C * (fHi - fHeight) + pShot->f60;
    } else if (fHeight < fLo) {
        fDist = lbl_80281F78->f190 * (fHeight - fLo) + pShot->f60;
    } else {
        fDist = pShot->f60;
    }
    fn_8000C5D4(aFrom, aBallDir, fDist, aTarget);
    fn_8003D324(aTarget, aBallDir, pScript, pShot, nPlayer, pShot->f64, fY);
    if (fn_80043388(pScript, pShot)) {
        Vec3Copy(aTarget, pOut);
        return;
    }
    // port: NTSC rate
    fFrames = f / (1.0f / 59.94f);
    fMove = lbl_80281F78->f14C * fFrames;
    fTurn = lbl_80281F78->f150 * fFrames;
    if (pScript->f98 < lbl_80281F78->f154) {
        fT = (lbl_80281F78->f154 - pScript->fCamTime) / lbl_80281F78->f154;
        fEase = (1.0f / lbl_80281F78->f14C) * (fT * fT);
        fMove *= fEase;
        fTurn *= fEase;
    }
    if (pScript->nBC == 4) {
        fMove *= pScript->f8C;
        fTurn *= pScript->f8C;
    }
    if (gPlayers[nPlayer].ball.fSpeed < lbl_80281F78->f198) {
        fGrow = gPlayers[nPlayer].ball.fSpeed / lbl_80281F78->f198;
        fGrow = fGrow * fGrow;
        fGrow = fGrow * fGrow;
        fMove *= fGrow;
        fTurn *= fGrow;
    }
    if (pScript->f88 < lbl_80281F78->f158) {
        fMove *= pScript->f88 / lbl_80281F78->f158;
        fTurn *= pScript->f88 / lbl_80281F78->f158;
    }
    if (pScript->f88 < lbl_80281F78->f15C) {
        fRise = powf(pScript->f88 / lbl_80281F78->f15C, lbl_80281F78->f160) * fFrames;
        aTarget[1] = fRise * (aTarget[1] - pOut[1]) + pOut[1];
    }
    fn_8003DC54(pOut, aFrom, aCurOff);
    fn_8003DC54(aTarget, aFrom, aTgtOff);
    fCurDist = (f32)fn_80009680(fn_80009744(aCurOff));
    fStep = ((f32)fn_80009680(fn_80009744(aTgtOff)) - fCurDist) * fMove;
    if (aCurOff[0] != 0.0f || aCurOff[1] != 0.0f || aCurOff[2] != 0.0f) {
        fn_800BAF04(aCurOff, aCurDir);
    } else {
        aCurDir[0] = 0.0f;
        aCurDir[1] = 0.0f;
        aCurDir[2] = 0.0f;
    }
    if (aTgtOff[0] != 0.0f || aTgtOff[1] != 0.0f || aTgtOff[2] != 0.0f) {
        fn_800BAF04(aTgtOff, aTgtDir);
    } else {
        aTgtDir[0] = 0.0f;
        aTgtDir[1] = 0.0f;
        aTgtDir[2] = 0.0f;
    }
    fTurn = fn_80009614(fn_8000C5FC(aCurDir, aTgtDir)) * fTurn;
    vec4flt_CrossProduct(aCurDir, aTgtDir, aAxis);
    if (aAxis[0] != 0.0f || aAxis[1] != 0.0f || aAxis[2] != 0.0f) {
        fn_800BAF04(aAxis, aAxis);
    }
    fn_8001EF34(aAxis, fTurn, aAxis);
    fn_8000923C(aAxis, aTurn);
    aCurDir[3] = 0.0f;
    fn_800090E4(aTurn, aCurDir, aNew);
    if (aNew[0] != 0.0f || aNew[1] != 0.0f || aNew[2] != 0.0f) {
        fn_800BAF04(aNew, aNewDir);
    } else {
        aNewDir[0] = 0.0f;
        aNewDir[1] = 0.0f;
        aNewDir[2] = 0.0f;
    }
    fn_8001EF34(aNewDir, fCurDist + fStep, aNew);
    fn_8003DC30(aFrom, aNew, pOut);
}

// The ball-flight camera's position for the shot: from the point DynamicCam_GetLocation gives for
// kind 0, along the ball's flight by the shot's f60 (changed by CamTuning.f18C times how far the
// ball is above the shot's f6C over the script's ground height fD8, or f190 times how far below its
// f68), then
// moved sideways by its f64 (fn_8003D324). Nothing happens while the ball is slower than
// CamTuning.f198.
void fn_8003B534(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pCam, f32* pSub) {
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
        fn_800BAF04(vDir, vDir);
    }
    if (gPlayers[nPlayer].ball.fSpeed < lbl_80281F78->f198) return;
    if (pScript->pNextShot != NULL && pScript->pShot->bB1 == pScript->pNextShot->bB1) {
        fMax = pScript->pShot->f6C;
        fMin = pScript->pShot->f68;
    } else {
        fMax = pShot->f6C;
        fMin = pShot->f68;
    }
    fHeight = gPlayers[nPlayer].ball.vPos[1] - pScript->fD8;
    if (fHeight > fMax) {
        fDist = lbl_80281F78->f18C * (fMax - fHeight) + pShot->f60;
    } else if (fHeight < fMin) {
        fDist = lbl_80281F78->f190 * (fHeight - fMin) + pShot->f60;
    } else {
        fDist = pShot->f60;
    }
    fn_8000C5D4(vFrom, vDir, fDist, vPos);
    fn_8003D324(vPos, vDir, pScript, pShot, nPlayer, pShot->f64, fY);
    Vec3Copy(vPos, pOut);
}

// The point DynamicCam_GetLocation gives for the shot's bAF.
void fn_8003B6D0(CamShot* pShot, int nPlayer, CamScript* pScript, f32* pOut, f32* pCam, f32* pSub) {
    f32 vPos[4];

    DynamicCam_GetLocation(pShot->bAF, nPlayer, vPos, pScript, pShot, pCam, pSub);
    Vec3Copy(vPos, pOut);
}

// A point of kind nKind for the shot into pOut: 0 the ball (or the script's v70 near the pin,
// fn_8003D9AC), 1 Player.vBall, 2 halfway between bones 0x39 and 0x47 of the golfer, 4, 6 and 8
// bones 1, 10 and 7, 9 Player.vTarget2, 10 the pin, 11 the player's tee, 12 the script's v50,
// 16 the script's own points (kept on the fairway when fn_80043388 says so), 17..19 bones 10, 7
// and 1 moved along their matrix's third row, 20 and 21 the shot's other point (bone 0 when that
// is 20 or 21 too) moved along bone 0's first or third row, 24 the shot's v20, 25 (0, 0, 100).
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
        fn_8003D9AC(pScript, pShot, nPlayer, pOut, 0);
        break;
    case 2:
        fn_8001EB8C(gPlayers[nPlayer].pChar, 0x39, vBone39);
        fn_8001EB8C(gPlayers[nPlayer].pChar, 0x47, vBone47);
        fn_8003DC30(vBone39, vBone47, vMid);
        fn_8001EF34(vMid, 0.5f, vMid);
        Vec3Copy(vMid, pOut);
        break;
    case 6:
        fn_8001EB8C(gPlayers[nPlayer].pChar, 10, vBone10);
        Vec3Copy(vBone10, pOut);
        break;
    case 8:
        fn_8001EB8C(gPlayers[nPlayer].pChar, 7, vBone7);
        Vec3Copy(vBone7, pOut);
        break;
    case 4:
        fn_8001EB8C(gPlayers[nPlayer].pChar, 1, vBone1);
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
        pCourse = fn_8000C594();
        Vec3Copy(&pCourse->pin[nPin].x, pOut);
        break;
    case 11:
        nTee = gSession.nTeeSet[nPlayer];
        pCourse = fn_8000C594();
        Vec3Copy(&pCourse->tee[nTee].x, pOut);
        break;
    case 24:
        Vec3Copy(pShot->v20, pOut);
        break;
    case 16:
        if (fn_80043388(pScript, pShot)) {
            if (pShot == pScript->pShot) {
                fn_80043C74(pScript, pOut, pCam, nPlayer, pScript->pB4, pSub, NULL);
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
        fn_8001EB8C(gPlayers[nPlayer].pChar, 10, vBone10);
        pMatrix = fn_8001ED08(gPlayers[nPlayer].pChar, 10);
        if (pMatrix == NULL) {
            Vec3Copy(vBone10, pOut);
            break;
        }
        Vec3Copy(pMatrix[2], vDir10);
        if (0.0f != vDir10[0] || 0.0f != vDir10[1] || 0.0f != vDir10[2]) {
            fn_800BAF04(vDir10, vDir10);
        }
        fn_8003DC30(vBone10, vDir10, pOut);
        break;
    case 18:
        fn_8001EB8C(gPlayers[nPlayer].pChar, 7, vBone7);
        pMatrix = fn_8001ED08(gPlayers[nPlayer].pChar, 7);
        if (pMatrix == NULL) {
            Vec3Copy(vBone7, pOut);
            break;
        }
        Vec3Copy(pMatrix[2], vDir7);
        if (0.0f != vDir7[0] || 0.0f != vDir7[1] || 0.0f != vDir7[2]) {
            fn_800BAF04(vDir7, vDir7);
        }
        fn_8003DC30(vBone7, vDir7, pOut);
        break;
    case 19:
        fn_8001EB8C(gPlayers[nPlayer].pChar, 1, vBone1);
        pMatrix = fn_8001ED08(gPlayers[nPlayer].pChar, 1);
        if (pMatrix == NULL) {
            Vec3Copy(vBone1, pOut);
            break;
        }
        Vec3Copy(pMatrix[2], vDir1);
        if (0.0f != vDir1[0] || 0.0f != vDir1[1] || 0.0f != vDir1[2]) {
            fn_800BAF04(vDir1, vDir1);
        }
        fn_8003DC30(vBone1, vDir1, pOut);
        break;
    case 20:
        if (nOther != 20 && nOther != 21) {
            DynamicCam_GetLocation(nOther, nPlayer, vFrom, pScript, pShot, pCam, pSub);
        } else {
            fn_8001EB8C(gPlayers[nPlayer].pChar, 0, vFrom);
        }
        pMatrix = fn_8001ED08(gPlayers[nPlayer].pChar, 0);
        if (pMatrix == NULL) {
            Vec3Copy(vFrom, pOut);
            break;
        }
        Vec3Copy(pMatrix[0], vDir);
        if (0.0f != vDir[0] || 0.0f != vDir[1] || 0.0f != vDir[2]) {
            fn_800BAF04(vDir, vDir);
        }
        fn_8003DC30(vFrom, vDir, pOut);
        break;
    case 21:
        if (nOther != 20 && nOther != 21) {
            DynamicCam_GetLocation(nOther, nPlayer, vFrom, pScript, pShot, pCam, pSub);
        } else {
            fn_8001EB8C(gPlayers[nPlayer].pChar, 0, vFrom);
        }
        pMatrix = fn_8001ED08(gPlayers[nPlayer].pChar, 0);
        if (pMatrix == NULL) {
            Vec3Copy(vFrom, pOut);
            break;
        }
        Vec3Copy(pMatrix[2], vDir);
        if (0.0f != vDir[0] || 0.0f != vDir[1] || 0.0f != vDir[2]) {
            fn_800BAF04(vDir, vDir);
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

// Picks a camera sequence of kind nKind for nPlayer's shot: one of those (up to 50) that fit the
// player, the club, the lie (nLie) and nClass bits, the pin's height over the ball and fDist
// (f24..f28), at random by their weights (f34). With none, the default sequence that fits, or the
// first default one with shot choices; NULL without a course.
CamSequence* fn_8003BDBC(int nPlayer, int nLie, int nClass, int nKind, u8 a, f32 fDist) {
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
    f32 fHeight;
    f32 fTotal;
    f32 fWeight;

    nPicked = 0;
    nSum = 0;
    nDefault = -1;
    if (Player_OnTee(nPlayer)) {
        nTee = 1;
    } else {
        nTee = fn_8003CBD4(nLie, nPlayer);
    }
    nClassBit = fn_8003CB80(nClass, nPlayer);
    nPinSet = Game_CurrentPinSet();
    pCourse = fn_8000C594();
    if (pCourse == NULL) {
        return NULL;
    }
    fHeight = pCourse->pin[nPinSet].y - gPlayers[nPlayer].vBall[1];
    for (i = 0; i < lbl_80281D88->nSequences; i++) {
        if (nPicked >= 50) {
            break;
        }
        if (lbl_80281D88->pSequences[i].nChoices > 0) {
            if (fn_8003CAFC(&lbl_80281D88->pSequences[i], nKind) &&
                fn_8003D00C(&lbl_80281D88->pSequences[i])) {
                nDefault = i;
            } else if (fn_8003D0EC(&lbl_80281D88->pSequences[i], nKind) &&
                       fn_8003CBE8(&lbl_80281D88->pSequences[i], nPlayer) &&
                       fn_8003CD9C(&lbl_80281D88->pSequences[i], nPlayer, a) &&
                       fn_8003CEEC(&lbl_80281D88->pSequences[i], nPlayer) &&
                       fn_8003D00C(&lbl_80281D88->pSequences[i]) &&
                       fn_8003D054(&lbl_80281D88->pSequences[i]) &&
                       fn_8003D0A0(lbl_80281D88->pSequences[i].b49, nTee) &&
                       fn_8003D0A0(lbl_80281D88->pSequences[i].b4A, nClassBit) &&
                       fn_8003CD6C(&lbl_80281D88->pSequences[i], nPlayer, fHeight) &&
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
            if (fn_8003CAFC(&lbl_80281D88->pSequences[i], nKind) &&
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
    for (i = 0; i < nPicked; i++) {
        fWeight = 1000.0f * lbl_80281D88->pSequences[anPicked[i]].f34;
        if ((f32)(int)(uRand % (int)(1000.0f * fTotal) - nSum) < fWeight) {
            return &lbl_80281D88->pSequences[anPicked[i]];
        }
        nSum += (int)fWeight;
    }
    return &lbl_80281D88->pSequences[anPicked[0]];
}

// The same pick for the pre-flight cameras: kind nKind (3 becomes 28 while something stands
// around the ball), by the ball's distance from the pin (fn_8003D0BC) instead of fDist and without
// the nClass bits.
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
    f32 fPinDist;
    f32 fHeight;
    f32 fTotal;
    f32 fWeight;

    nPicked = 0;
    nSum = 0;
    nDefault = -1;
    if (Player_OnTee(nPlayer)) {
        nTee = 1;
    } else {
        nTee = fn_8003CBD4(nLie, nPlayer);
    }
    fPinDist = fn_800D0478(nPlayer);
    nPinSet = Game_CurrentPinSet();
    pCourse = fn_8000C594();
    if (pCourse == NULL) {
        return NULL;
    }
    fHeight = pCourse->pin[nPinSet].y - gPlayers[nPlayer].vBall[1];
    if (nKind == 3 && Ter_CheckObjectAndHazardObstruction(gPlayers[nPlayer].ball.vPos, lbl_80281F78->f16C, 1,
                                                          0, 0.0f, 1, lbl_80281F78->f1A4)) {
        nKind = 28;
    }
    for (i = 0; i < lbl_80281D88->nSequences; i++) {
        if (nPicked >= 50) {
            break;
        }
        if (lbl_80281D88->pSequences[i].nChoices > 0) {
            if (fn_8003CAFC(&lbl_80281D88->pSequences[i], nKind) &&
                fn_8003D00C(&lbl_80281D88->pSequences[i])) {
                nDefault = i;
            } else if (fn_8003D0EC(&lbl_80281D88->pSequences[i], nKind) &&
                       fn_8003CBE8(&lbl_80281D88->pSequences[i], nPlayer) &&
                       fn_8003CD9C(&lbl_80281D88->pSequences[i], nPlayer, 1) &&
                       fn_8003CEEC(&lbl_80281D88->pSequences[i], nPlayer) &&
                       fn_8003D0BC(&lbl_80281D88->pSequences[i], nPlayer, fPinDist) &&
                       fn_8003D00C(&lbl_80281D88->pSequences[i]) &&
                       fn_8003D054(&lbl_80281D88->pSequences[i]) &&
                       fn_8003CD6C(&lbl_80281D88->pSequences[i], nPlayer, fHeight) &&
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
            if (fn_8003CAFC(&lbl_80281D88->pSequences[i], nKind) &&
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
    for (i = 0; i < nPicked; i++) {
        fWeight = 1000.0f * lbl_80281D88->pSequences[anPicked[i]].f34;
        if ((f32)(int)(uRand % (int)(1000.0f * fTotal) - nSum) < fWeight) {
            return &lbl_80281D88->pSequences[anPicked[i]];
        }
        nSum += (int)fWeight;
    }
    return &lbl_80281D88->pSequences[anPicked[0]];
}

// The set named szName (case ignored) gives a sequence (*ppSeq) or a shot (*ppShot): its p20
// 39 times in 100 when that has shot choices, else by its kind: 14 one of p14, p18 and p1C at
// random (the next one when the pick is missing), 13 its shot. 0: no set gave one.
u8 fn_8003C800(char* szName, CamSequence** ppSeq, CamShot** ppShot) {
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

// The sequence and shot named after the golfer's clip: with b, the clip in Character.p1790 when
// there is one, else the clip it is playing. While the GameBreaker letterbox is up (fn_8003DCAC)
// the "LB" version of the name is tried first.
u8 fn_8003C9D0(int nPlayer, u8 b, CamSequence** ppSeq, CamShot** ppShot) {
    char szName[0x18];          // the frame allows 12 to 24 bytes; the true size is unknown
    char* pName = NULL;

    if (ppSeq == NULL || ppShot == NULL) {
        return 0;
    }
    *ppSeq = NULL;
    *ppShot = NULL;
    if (b && gPlayers[nPlayer].pChar->p1790 != NULL) {
        pName = gPlayers[nPlayer].pChar->p1790->name;
    }
    if (pName == NULL && gPlayers[nPlayer].pChar->pCurClip != NULL) {
        pName = gPlayers[nPlayer].pChar->pCurClip->name;
    }
    if (pName != NULL) {
        if (fn_8003DCAC()) {
            sprintf(szName, "LB%s", pName);
            if (fn_8003C800(szName, ppSeq, ppShot) == 1) {
                return 1;
            }
        }
        if (fn_8003C800(pName, ppSeq, ppShot) == 1) {
            return 1;
        }
    }
    return 0;
}

// A default sequence of the kind: one whose name starts with "DEF".
u8 fn_8003CAFC(CamSequence* pSequence, int nKind) {
    if (pSequence == NULL) return 0;
    if (!fn_8003D0EC(pSequence, nKind)) return 0;
    if (pSequence->szName[0] != 'D') return 0;
    if (pSequence->szName[1] != 'E') return 0;
    if (pSequence->szName[2] != 'F') return 0;
    return 1;
}

// Maps n (0..19) through lbl_80187988; 1 depends on the ball's lie (1 on lie 0, else 2).
s32 fn_8003CB80(u32 n, int nPlayer) {
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

// Maps n through lbl_801879D8.
s32 fn_8003CBD4(int n, int nPlayer) {
    return lbl_801879D8[n];
}

// The sequence suits the player's club: b45 picks the clubs (0 any, 1 woods, 2 5..9 irons,
// 3 1..5 irons, 4 wedges, 5 putter, 6 woods and irons, 7 woods and 1..5 irons, 8 all but the
// putter, 9 5 iron to the wedges, 10 irons and wedges).
u8 fn_8003CBE8(CamSequence* pSequence, int nPlayer) {
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

// The value is within the sequence's f2C..f30. nPlayer is not used (every caller passes it).
u8 fn_8003CD6C(CamSequence* pSequence, int nPlayer, f32 f) {
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

// The sequence is used for fn_800D2B08's current value.
u8 fn_8003D054(CamSequence* pSequence) {
    return (pSequence->n4B & (1 << fn_800D2B08())) != 0;
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
        if (fn_800E39F0()) return 0;
        return 1;
    }
    if (pSequence->b48 == 1) {
        if (gSession.nSplitScreen) return 1;
        if (nMode == 9) return 1;
        return nMode == 11;
    }
    if (pSequence->b48 == 2) {
        return fn_800E39F0() != 0;
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
// is being edited and the shot's u50/u54 bit for the CrAP camera is set.
u8 fn_8003D294(CamShot* pShot) {
    CrAPGolfer* pGolfer;
    int n;

    if (gSession.nGameType != 3) return 1;
    pGolfer = lbl_80281EE0->pB4;
    if (pGolfer != NULL && pGolfer->pChar != NULL) {
        n = pGolfer->nC;
        if (n <= 32) {
            // EA bug: n == 32 shifts by 32 (undefined in C; the PowerPC gives 0)
            return (pShot->u.bits.u50 & (1 << n)) != 0;
        }
        return (pShot->u.bits.u54 & (1 << (n - 32))) != 0;
    }
    return 0;
}

// Moves pPos fSide sideways, square to pDir on the level, then hands it on to fn_8003D414.
void fn_8003D324(f32* pPos, f32* pDir, CamScript* pScript, CamShot* pShot, int nPlayer, f32 fSide, f32 fY) {
    f32 vDir[4];

    Vec3Copy(pDir, vDir);
    vDir[1] = 0.0f;
    if (0.0f != vDir[0] || 0.0f != vDir[1] || 0.0f != vDir[2]) {
        fn_800BAF04(vDir, vDir);
    }
    pPos[0] += fSide * -vDir[2];
    pPos[2] += fSide * vDir[0];
    fn_8003D414(pPos, pScript, pShot, nPlayer, fY);
}

// Sets the camera's height pPos[1] by the shot's bB2 (0: left alone): 1 halfway between bones 0x39
// and 0x47 of the golfer, 2 bone 1, 3 bone 0xA, 4 fY (the height before), 5 and 7 fn_8003D9AC's
// point, 6 the pin; then the shot's f80 above it. Kind 7 moves on to that height from the script's
// v70 step by step, once per ball update this frame, easing by CamTuning.f22C while the ball rises
// and by f230 to f22C (between the shot's f68, at least 0.15, and the height kept in the script's
// f104) as it comes down; slower early in the script's move (f98 under CamTuning.fD4).
void fn_8003D414(f32* pPos, CamScript* pScript, CamShot* pShot, int nPlayer, f32 fY) {
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

    nSteps = GameEffects_BallUpdatesThisFrame(nPlayer);
    if (pShot->bB2 == 0) {
        return;
    }
    switch (pShot->bB2) {
    case 1:
        fn_8001EB8C(gPlayers[nPlayer].pChar, 0x39, vBone39);
        fn_8001EB8C(gPlayers[nPlayer].pChar, 0x47, vBone47);
        fn_8003DC30(vBone39, vBone47, vMid);
        fn_8001EF34(vMid, 0.5f, vMid);
        pPos[1] = vMid[1];
        break;
    case 2:
        fn_8001EB8C(gPlayers[nPlayer].pChar, 1, vBone1);
        pPos[1] = vBone1[1];
        break;
    case 3:
        fn_8001EB8C(gPlayers[nPlayer].pChar, 0xA, vBoneA);
        pPos[1] = vBoneA[1];
        break;
    case 4:
        pPos[1] = fY;
        break;
    case 5:
    case 7:
        fn_8003D9AC(pScript, pShot, nPlayer, vPoint, 0);
        pPos[1] = vPoint[1];
        break;
    case 6:
        nPin = Game_CurrentPinSet();
        pCourse = fn_8000C594();
        if (pCourse != NULL) {
            pPos[1] = pCourse->pin[nPin].y;
        }
        break;
    }
    pPos[1] += pShot->f80;
    if (pShot->bB2 != 7) {
        return;
    }
    if (fn_80043388(pScript, pShot)) {
        pScript->f104 = pShot->f68;
        return;
    }
    if (nSteps == 0) {
        pPos[1] = fY;
        return;
    }
    for (i = 0; i < nSteps; i++) {
        fn_8003DC54(vPoint, pScript->v70, vOff);
        fn_8001EF34(vOff, (f32)(i + 1) / (f32)nSteps, vOff);
        fn_8003DC30(pScript->v70, vOff, vStep);
        pPos[1] = vStep[1];
        pPos[1] += pShot->f80;
        fLow = pShot->f68;
        if (fLow <= 0.15f) {
            fLow = 0.15f;
        }
        if (!gPlayers[nPlayer].ball.bHitTopArc) {
            fEase = lbl_80281F78->f22C;
            pScript->f104 = pPos[1] - pScript->fD8;
        } else {
            fHeight = pPos[1] - pScript->fD8;
            if (fHeight <= fLow || pScript->f104 < fLow) {
                fEase = lbl_80281F78->f230;
            } else if (fHeight > pScript->f104) {
                fEase = lbl_80281F78->f22C;
            } else {
                fEase = (lbl_80281F78->f22C - lbl_80281F78->f230) * ((fHeight - fLow) / pScript->f104) +
                        lbl_80281F78->f230;
            }
        }
        if (pScript->f98 < lbl_80281F78->fD4) {
            fEase = 1.0f -
                    (f32)fn_80009680((f32)fn_80009680(pScript->f98 / lbl_80281F78->fD4)) * (1.0f - fEase);
        }
        pPos[1] = fEase * (pPos[1] - fY) + fY;
        fY = pPos[1];
    }
}

// The sequence suits the player's club and shot kind.
u8 fn_8003D7A0(CamSequence* pSequence, int nPlayer) {
    if (pSequence == NULL) return 0;
    if (fn_8003CBE8(pSequence, nPlayer) && fn_8003CD9C(pSequence, nPlayer, 1)) {
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
            fn_800BAF04(pDir, pDir);
        }
        return;
    }
    Vec3Copy(pDir, vLevel);
    vLevel[1] = 0.0f;
    if (0.0f != vLevel[0] || 0.0f != vLevel[1] || 0.0f != vLevel[2]) {
        fn_800BAF04(vLevel, vLevel);
    }
    if (fabsf(fn_80009614(fn_8000C5FC(vLevel, pDir))) > lbl_80281F78->f19C) {
        vec4flt_CrossProduct(pDir, vLevel, vAxis);
        if (0.0f != vAxis[0] || 0.0f != vAxis[1] || 0.0f != vAxis[2]) {
            fn_800BAF04(vAxis, vAxis);
        }
        fn_8001EF34(vAxis, lbl_80281F78->f19C, vAxis);
        fn_8000923C(vAxis, qTurn);
        vLevel[3] = 0.0f;
        fn_800090E4(qTurn, vLevel, pDir);
        if (0.0f != pDir[0] || 0.0f != pDir[1] || 0.0f != pDir[2]) {
            fn_800BAF04(pDir, pDir);
        }
    }
}

// The ball's position into pOut; but when the ball is in the cup (lie 12) or on surface 0x62 or
// 0x69, more than 0.005 below the pin and within 2 of the script's v70, v70 instead. With bKeep,
// v70 takes the ball's position whenever it is not used.
void fn_8003D9AC(CamScript* pScript, CamShot* pShot, int nPlayer, f32* pOut, u8 bKeep) {
    u8 bNear = 0;
    CourseInfo* pCourse = fn_8000C594();
    int nPin = Game_CurrentPinSet();

    if ((gPlayers[nPlayer].ball.nSurface == 0x62 || gPlayers[nPlayer].ball.nSurface == 0x69
         || gPlayers[nPlayer].ball.nLie == 12)
        && pCourse != NULL) {
        if (pCourse->pin[nPin].y - gPlayers[nPlayer].ball.vPos[1] > 0.005f
            && Vec_Distance(pScript->v70, gPlayers[nPlayer].ball.vPos) < 2.0f) {
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

// The shot's f64 and f60 into *pA and *pB; when fn_800453C8 holds for the player they are
// turned a quarter: (-f64, f60), or (f64, -f60) for a shot with bAF or bB0 set to 21.
void fn_8003DAC8(CamShot* pShot, int nPlayer, f32* pA, f32* pB) {
    if (pShot != NULL) {
        if (fn_800453C8(nPlayer, pShot)) {
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
    if (Game_GetCourse() == 9 && fn_80015464() == 12) {
        return 10.0f * f;
    }
    if (Game_GetCourse() == 3 && (fn_80015464() == 13 || fn_80015464() == 15)) {
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

    if (lbl_80202898.bGameBreaker && (lbl_80202898.nGBType != 0 || lbl_80202898.b19 == 1)) {
        bResult = 1;
    }
    return bResult;
}
