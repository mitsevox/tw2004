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
void fn_800399E0(u8* pSrc, CamShot* pDst, u32 nCount);
void fn_80039A48(u8* pSrc, DynCamSet* pDst, u32 nCount);
void fn_80039B14(int nSize);
void fn_80039C5C(int nSize);
void fn_80039D0C(int nSequences);
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
    i = Rand_Next(1) % nCount;
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

// The value is within the sequence's f2C..f30.
u8 fn_8003CD6C(CamSequence* pSequence, f32 f) {
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

// The value is within the sequence's f24..f28.
u8 fn_8003D0BC(CamSequence* pSequence, f32 f) {
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
            return (pShot->u50 & (1 << n)) != 0;
        }
        return (pShot->u54 & (1 << (n - 32))) != 0;
    }
    return 0;
}

// The sequence suits the player's club and shot kind.
u8 fn_8003D7A0(CamSequence* pSequence, int nPlayer) {
    if (pSequence == NULL) return 0;
    if (fn_8003CBE8(pSequence, nPlayer) && fn_8003CD9C(pSequence, nPlayer, 1)) {
        return 1;
    }
    return 0;
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
