// GameMode5.c (our name): game mode 5, a list of 83 challenges (lbl_80203554, loaded from the
// 'PLY ' stream object), each an entry of 0x80 bytes.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "game/modes/challenge.h"
#include "game/earnings.h"

extern s32 lbl_802822F8;
void fn_800EAE44(int nId);
s32 fn_800EAE6C(void);
void fn_800EAEB8(void);
void fn_800EAEEC(UStreamObject* pObject);

extern char* lbl_80282310;                  // the challenge text block
extern s32 lbl_80281660;
extern s32 lbl_802822F0;
extern void (*lbl_8028232C)(void);
extern void (*lbl_80282324)(void);
extern void (*lbl_80282318)(int nPlayer);
int   fn_800ED028(int i);
void  fn_80019648(void);
void  fn_800EBEF0(void);
void  fn_800EAD6C(void);
void  fn_800EBD28(void);
void  fn_800EC1E0(void);
void  fn_800ED604(int nPlayer);
void  fn_800EAF18(UStreamObject* pObject);
extern u8  (*lbl_80282320)(u8 bCheck);        // the mode's own GameFinished
extern s32 lbl_80282300;
extern s32 lbl_80282304;
extern s32 lbl_80282308;
extern s32 lbl_8028230C;
extern void (*lbl_80282328)(void);
int   fn_800ECF9C(int i);
void  fn_800EC170(int n);
u8    fn_800EC4F0(int n);
u8    fn_800EBD60(u8 bCheck);
int   fn_800ED508(int nGroup);
extern u8 lbl_802822FC;
u8 fn_800ECA08(void);
extern u8 lbl_80282314;
extern u8 lbl_802822FE;
extern u8 (*lbl_8028231C)(int nPlayer, u8 bCheck);   // and HoleFinished
u8 fn_800ED5C8(int nPlayer, u8 bCheck);
extern u8 lbl_802822FD;

// Game mode 5 starts: its callbacks, gpGame->nC and n10 set to 1, and mode 5's own challenge list.
void fn_800EACD8(void) {
    gpGame->pfnInit = fn_800EACD8;
    gpGame->pfnShutdown = fn_800EAD6C;
    gpGame->pfn1E4 = fn_800EBD28;
    gpGame->pfnEndGame = fn_800EC1E0;
    gpGame->pfnHoleFinished = fn_800ED5C8;
    gpGame->pfn210 = fn_800ED604;
    gpGame->nC = 1;
    gpGame->n10 = 1;
    lbl_802822FE = 0;
    lbl_80281664 = lbl_80203554;
    lbl_80281668 = 83;
}

// The mode ends: its cleanup callback runs once, and the options it changed come back.
void fn_800EAD6C(void) {
    if (lbl_8028232C) {
        lbl_8028232C();
        lbl_8028232C = NULL;
    }
    gpGame->nC = 1;
    gpGame->n10 = 1;
    gSession.options.nC = lbl_80281660;
    gSession.options.nWind = lbl_802822F0;
    lbl_802822FC = 0;
}

void fn_800EADD8(void) {
}

// A 'Cact' object of type 10 (from the 'Cact' handler at 0x80048BDC): one challenge's ball spot.
void fn_800EADDC(void* pObj) {
    // port: the course object is big-endian and read in place through ChallengeSpotRecord; a
    // little-endian port converts v[] (three floats) here
    ChallengeSpotRecord* d = *(ChallengeSpotRecord**)pObj;
    int i = d->nChallenge - 1;
    if (i < 83) {
        lbl_80203170[i].f0 = d->v[0];
        lbl_80203170[i].f4 = d->v[1];
        lbl_80203170[i].f8 = d->v[2];
    }
    fn_80009E70(pObj);
}

void fn_800EAE38(s32 p0) {
    lbl_802822F4 = p0;
    lbl_802822F8 = p0;
}

void fn_800EAE44(int nId) {
    int i = fn_800EAC94(nId);
    lbl_802822F4 = i;
    lbl_802822F8 = i;
}

s32 fn_800EAE6C(void) {
    return 29;
}

void fn_800EAE74(void) {
    UStream_RegisterHandler('PLY ', fn_800EAEEC);
    UStream_RegisterHandler('PLYs', fn_800EAF18);
}

void fn_800EAEB8(void) {
    UStream_UnregisterHandler('PLY ');
    UStream_UnregisterHandler('PLYs');
}

void fn_800EAEEC(UStreamObject* pObject) {
    // port: the 'PLY ' object is copied straight into the challenges (Challenge[83]); it is
    // big-endian on disc, so a little-endian port converts it field by field here
    // (docs/format-byteorder.md)
    fn_8000E790(pObject, sizeof(lbl_80203554), lbl_80203554);
}

// The 'PLYs' object: the challenge text block is copied out.
void fn_800EAF18(UStreamObject* pObject) {
    void* pData;
    u32 nSize = fn_8000E81C(pObject, &pData);
    if (nSize) {
        lbl_80282310 = fn_800951A0(nSize, 0x10, 1);
        Mem_cpy(lbl_80282310, pData, nSize);
        fn_80009E70(pObject);
    }
}

// A challenge starts: the options it changes are saved, the round is set up (mode, course, holes,
// tee set, pins, the CPU opponents), and the holes before the challenge hole get scores so the
// challenge starts from its target (see fn_800ED028); then the challenge's own callbacks go in,
// the mode's own ones kept for fn_800EAD6C and the rest to call.
void fn_800EAF7C(void) {
    int h;
    int nSum;
    int nSum0;
    int nDiff;
    int i;
    int nStrokes;
    int nPar;
    int nHoles;
    u8 bFound;
    lbl_80281660 = gSession.options.nC;
    lbl_802822F0 = gSession.options.nWind;
    fn_800E1074();
    if (gpSaveData[gPlayers[0].nIndex].bActive) {
        gpSaveData[gPlayers[0].nIndex].b70 = 1;
    }
    fn_800E0B38(lbl_80281664[lbl_802822F4].nMode);
    lbl_802822FC = 1;
    fn_800E14E0(lbl_80281664[lbl_802822F4].nCourse);
    fn_800E1260(lbl_80281664[lbl_802822F4].nType);
    if (lbl_80281664[lbl_802822F4].nType == 0) {
        fn_800E1404(lbl_80281664[lbl_802822F4].nHole - 1);
    }
    if (lbl_80281664[lbl_802822F4].nType != 4 && lbl_80281664[lbl_802822F4].nType != 5 &&
        lbl_80281664[lbl_802822F4].nType != 6 && lbl_80281664[lbl_802822F4].nType != 7) {
        fn_800E1480(lbl_80281664[lbl_802822F4].nHole - 1);
    }
    for (h = 0; h < lbl_80281664[lbl_802822F4].nHole - 1; h++) {
        gpGame->bHoleSelected[h] = 0;
    }
    gSession.nTeeSet[0] = lbl_80281664[lbl_802822F4].nTeeSet;
    if (lbl_80281664[lbl_802822F4].n20) {
        gSession.nPinSet = lbl_80281664[lbl_802822F4].n20 - 1;
        for (h = 0; h < 18; h++) {
            gpGame->nPinSet[h] = gSession.nPinSet;
        }
    } else {
        gSession.nPinSet = -1;
        for (h = 0; h < 18; h++) {
            gpGame->nPinSet[h] = 0;
        }
    }
    gNumPlayersSetUp = 1;
    Session_SetNumPlayers(lbl_80281664[lbl_802822F4].nOpponents + 1);
    if (lbl_80281664[lbl_802822F4].nOpponents > 0) {
        Session_SetGolfer(lbl_80281664[lbl_802822F4].aOpponent[0], 1);
        gSession.nController[1] = CONTROLLER_CPU;
        gSession.nTeeSet[1] = lbl_80281664[lbl_802822F4].nTeeSet;
        gNumPlayersSetUp = 2;
        if (gSession.nGolfer[0] == gSession.nGolfer[1] &&
            gSession.aProfile[0].n0 == gSession.aProfile[1].n0) {
            gSession.aProfile[1].n0++;
            if (gSession.aProfile[1].n0 >= 4) {
                gSession.aProfile[1].n0 = 0;
            }
        }
    }
    if (lbl_80281664[lbl_802822F4].nOpponents > 1) {
        Session_SetGolfer(lbl_80281664[lbl_802822F4].aOpponent[1], 2);
        gSession.nController[2] = CONTROLLER_CPU;
        gSession.nTeeSet[2] = lbl_80281664[lbl_802822F4].nTeeSet;
        gNumPlayersSetUp = 3;
        if (gSession.nGolfer[0] == gSession.nGolfer[2] &&
            gSession.aProfile[0].n0 == gSession.aProfile[2].n0) {
            gSession.aProfile[2].n0++;
            if (gSession.aProfile[2].n0 >= 4) {
                gSession.aProfile[2].n0 = 0;
            }
        }
    }
    if (lbl_80281664[lbl_802822F4].nOpponents > 2) {
        Session_SetGolfer(lbl_80281664[lbl_802822F4].aOpponent[2], 3);
        gSession.nController[3] = CONTROLLER_CPU;
        gSession.nTeeSet[3] = lbl_80281664[lbl_802822F4].nTeeSet;
        gNumPlayersSetUp = 4;
        if (gSession.nGolfer[0] == gSession.nGolfer[3] &&
            gSession.aProfile[0].n0 == gSession.aProfile[3].n0) {
            gSession.aProfile[3].n0++;
            if (gSession.aProfile[3].n0 >= 4) {
                gSession.aProfile[3].n0 = 0;
            }
        }
    }
    nStrokes = 0;
    nPar = 0;
    nHoles = 0;
    switch (lbl_80281664[lbl_802822F4].nTargetKind) {
    case 0:
        for (h = 0; h < Game_CurHoleIndex(); h++) {
            gPlayers[0].nStrokes[h] = 0;
        }
        break;
    case 1:
        if (lbl_80281664[lbl_802822F4].nType == 0) {
            for (h = 0; h < Game_CurHoleIndex(); h++) {
                gPlayers[0].nStrokes[h] = 0;
            }
            nPar = 0;
            nHoles = 0;
            nStrokes = lbl_80281664[lbl_802822F4].nTargetBase;
        } else {
            nSum0 = 0;
            for (h = 0; h < Game_CurHoleIndex(); h++) {
                gPlayers[0].nStrokes[h] = fn_800D2AD8(h);
                nSum0 += gPlayers[0].nStrokes[h];
            }
            nDiff = lbl_80281664[lbl_802822F4].nTargetBase - nSum0;
            while (nDiff != 0) {
                h = Rand_Next(0) % (Game_CurHoleIndex() + 1);
                if (nDiff < 0) {
                    if (gPlayers[0].nStrokes[h] > fn_800D2AD8(h) - 1) {
                        gPlayers[0].nStrokes[h]--;
                    }
                } else if (gPlayers[0].nStrokes[h] < fn_800D2AD8(h) + 1) {
                    gPlayers[0].nStrokes[h]++;
                }
                nSum = 0;
                for (h = 0; h < Game_CurHoleIndex(); h++) {
                    nSum += gPlayers[0].nStrokes[h];
                }
                nDiff = lbl_80281664[lbl_802822F4].nTargetBase - nSum;
            }
        }
        break;
    case 2:
        if (lbl_80281664[lbl_802822F4].nType == 0) {
            for (h = 0; h < Game_CurHoleIndex(); h++) {
                gPlayers[0].nStrokes[h] = 0;
            }
            nPar = 0;
            nHoles = 1;
            nStrokes = lbl_80281664[lbl_802822F4].nTargetBase;
        } else {
            nSum0 = 0;
            for (h = 0; h < Game_CurHoleIndex(); h++) {
                gPlayers[0].nStrokes[h] = fn_800D2AD8(h);
                nSum0 += gPlayers[0].nStrokes[h];
            }
            nDiff = lbl_80281664[lbl_802822F4].nTargetBase;
            while (nDiff != 0) {
                h = Rand_Next(0) % (Game_CurHoleIndex() + 1);
                if (nDiff < 0) {
                    if (gPlayers[0].nStrokes[h] > fn_800D2AD8(h) - 1) {
                        gPlayers[0].nStrokes[h]--;
                    }
                } else if (gPlayers[0].nStrokes[h] < fn_800D2AD8(h) + 1) {
                    gPlayers[0].nStrokes[h]++;
                }
                nSum = 0;
                for (h = 0; h < Game_CurHoleIndex(); h++) {
                    nSum += gPlayers[0].nStrokes[h];
                }
                nDiff = lbl_80281664[lbl_802822F4].nTargetBase - (nSum - nSum0);
            }
        }
        break;
    case 3:
        for (h = 0; h < Game_CurHoleIndex(); h++) {
            gPlayers[0].nStrokes[h] = fn_800D2AD8(h) - 1;
        }
        break;
    case 4:
        for (h = 0; h < Game_CurHoleIndex(); h++) {
            gPlayers[0].nStrokes[h] = fn_800D2AD8(h);
        }
        break;
    case 5:
        for (h = 0; h < Game_CurHoleIndex(); h++) {
            gPlayers[0].nStrokes[h] = fn_800D2AD8(h) + 1;
        }
        break;
    case 7:
        if (gpGame->n4 == 1) {
            nDiff = lbl_80281664[lbl_802822F4].nTargetBase;
            while (nDiff != 0) {
                h = Rand_Next(0) % (Game_CurHoleIndex() + 1);
                if (nDiff < 0) {
                    if (gPlayers[0].nModePoints[h] == 0 && gPlayers[1].nModePoints[h] == 0) {
                        nDiff++;
                        gPlayers[1].nModePoints[h] = 1;
                        gPlayers[1].nHolesWon++;
                    }
                } else if (gPlayers[0].nModePoints[h] == 0 && gPlayers[1].nModePoints[h] == 0) {
                    nDiff--;
                    gPlayers[0].nModePoints[h] = 1;
                    gPlayers[0].nHolesWon++;
                }
            }
        }
        if (gpGame->n4 == 0) {
            nSum0 = 0;
            for (h = 0; h < Game_CurHoleIndex(); h++) {
                gPlayers[0].nStrokes[h] = fn_800D2AD8(h);
                nSum0 += gPlayers[0].nStrokes[h];
                for (i = 1; i < gNumPlayersSetUp; i++) {
                    gPlayers[(u32)i].nStrokes[h] = fn_800D2AD8(h);
                }
            }
            nDiff = lbl_80281664[lbl_802822F4].nTargetBase;
            while (nDiff != 0) {
                i = Rand_Next(0) % (Game_CurHoleIndex() + 1);
                if (nDiff < 0) {
                    if (gPlayers[0].nStrokes[i] > fn_800D2AD8(i) - 1) {
                        gPlayers[0].nStrokes[i]--;
                    }
                } else if (gPlayers[0].nStrokes[i] < fn_800D2AD8(i) + 1) {
                    gPlayers[0].nStrokes[i]++;
                }
                nSum = 0;
                for (h = 0; h < Game_CurHoleIndex(); h++) {
                    nSum += gPlayers[0].nStrokes[h];
                }
                nDiff = lbl_80281664[lbl_802822F4].nTargetBase - (nSum - nSum0);
            }
        }
        break;
    }
    switch (lbl_80281664[lbl_802822F4].nHoleKind) {
    case 0:
        gPlayers[0].nStrokes[Game_CurHoleIndex()] = 0;
        break;
    case 1:
        gPlayers[0].nStrokes[Game_CurHoleIndex()] = lbl_80281664[lbl_802822F4].nHoleExtra;
        break;
    case 2:
        gPlayers[0].nStrokes[Game_CurHoleIndex()] = lbl_80281664[lbl_802822F4].nHoleExtra + fn_800D2B08();
        break;
    case 3:
        gPlayers[0].nStrokes[Game_CurHoleIndex()] = fn_800D2B08() - 1;
        break;
    case 4:
        gPlayers[0].nStrokes[Game_CurHoleIndex()] = fn_800D2B08();
        break;
    case 5:
        gPlayers[0].nStrokes[Game_CurHoleIndex()] = fn_800D2B08() + 1;
        break;
    }
    if (lbl_80281664[lbl_802822F4].b4D) {
        gSession.options.nC = 3;
    } else {
        gSession.options.nC = 0;
    }
    gpGame->nMulligans = 0;
    gSession.options.nWind = lbl_80281664[lbl_802822F4].nWind;
    bFound = 0;
    for (i = lbl_802822F4 - 1; i >= 0; i--) {
        if (lbl_80281664[i].nGroup == lbl_80281664[lbl_802822F4].nGroup) {
            bFound = 1;
        }
    }
    if (!bFound) {
        lbl_8028230C = nStrokes;
        lbl_80282308 = nPar;
        lbl_80282304 = nHoles;
        lbl_80282300 = 0;
        lbl_80282314 = 1;
    } else {
        lbl_8028230C += nStrokes;
        lbl_80282308 += nPar;
        lbl_80282304 += nHoles;
    }
    lbl_8028232C = gpGame->pfnShutdown;
    lbl_80282328 = gpGame->pfnEndGame;
    lbl_80282324 = gpGame->pfn1E4;
    lbl_80282320 = gpGame->pfnGameFinished;
    lbl_8028231C = gpGame->pfnHoleFinished;
    lbl_80282318 = gpGame->pfn210;
    gpGame->pfnShutdown = fn_800EAD6C;
    gpGame->pfnEndGame = fn_800EC1E0;
    gpGame->pfn1E4 = fn_800EBD28;
    gpGame->pfnGameFinished = fn_800EBD60;
    gpGame->pfnHoleFinished = fn_800ED5C8;
    gpGame->pfn210 = fn_800ED604;
}

// Hole start (after fn_800EAF7C set the challenge up).
void fn_800EBD28(void) {
    gpGame->b275 = 1;
    lbl_80282324();
    fn_800EBEF0();
}

// The round ends: the round's totals are added up; then the next challenge in the group starts
// (unless only checking), or the group is over (1).
u8 fn_800EBD60(u8 bCheck) {
    int h;
    int nStrokes;
    int i;
    if (lbl_802822FE) {
        return 0;
    }
    if (lbl_80282320(bCheck)) {
        nStrokes = 0;
        for (h = 0; h < 18; h++) {
            if (gpGame->bHoleSelected[h]) {
                nStrokes += gPlayers[0].nStrokes[h];
                lbl_80282308 += fn_800D2AD8(h);
                lbl_80282304++;
                lbl_80282300 += gPlayers[0].n290[h];
            }
        }
        lbl_8028230C += nStrokes;
        fn_800E1260(0);
        for (i = lbl_802822F4 + 1; i < lbl_80281668; i++) {
            if (lbl_80281664[i].nGroup == lbl_80281664[lbl_802822F4].nGroup) {
                if (bCheck) {
                    return 0;
                }
                lbl_802822F4 = i;
                if (fn_800F0818()) {
                    fn_800E0B38(24);
                    fn_800F07C8();
                } else {
                    fn_800E0B38(5);
                    fn_800EAF7C();
                }
                gpGame->b134 = 1;
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

// The challenge's setup: the ball placed at its spot, its bag (bit n of nClubBits gives the n-th club
// of the list below; club 25 is always in), and fn_800ED6F8's value.
void fn_800EBEF0(void) {
    f32 v[4];
    if (lbl_80281664[lbl_802822F4].bPlaceBall) {
        v[0] = lbl_80203170[lbl_802822F4].f0;
        v[1] = lbl_80203170[lbl_802822F4].f4;
        v[2] = lbl_80203170[lbl_802822F4].f8;
        v[3] = 1.0f;
        fn_80055AA8(&gPlayers[0].ball, v, 0);
        Physics_DropBall(&gPlayers[0].ball, v);
        Vec_Copy(v, gPlayers[0].vBall);
    }
    if (lbl_80281664[lbl_802822F4].nClubBits) {
        gPlayers[0].golfer.uBagMask = 0x2000000;
        if (lbl_80281664[lbl_802822F4].nClubBits & 1) {
            gPlayers[0].golfer.uBagMask |= 0x1;
        }
        if (lbl_80281664[lbl_802822F4].nClubBits & 2) {
            gPlayers[0].golfer.uBagMask |= 0x40;
        }
        if (lbl_80281664[lbl_802822F4].nClubBits & 4) {
            gPlayers[0].golfer.uBagMask |= 0x80;
        }
        if (lbl_80281664[lbl_802822F4].nClubBits & 8) {
            gPlayers[0].golfer.uBagMask |= 0x100;
        }
        if (lbl_80281664[lbl_802822F4].nClubBits & 0x10) {
            gPlayers[0].golfer.uBagMask |= 0x200;
        }
        if (lbl_80281664[lbl_802822F4].nClubBits & 0x20) {
            gPlayers[0].golfer.uBagMask |= 0x400;
        }
        if (lbl_80281664[lbl_802822F4].nClubBits & 0x40) {
            gPlayers[0].golfer.uBagMask |= 0x800;
        }
        if (lbl_80281664[lbl_802822F4].nClubBits & 0x80) {
            gPlayers[0].golfer.uBagMask |= 0x1000;
        }
        if (lbl_80281664[lbl_802822F4].nClubBits & 0x100) {
            gPlayers[0].golfer.uBagMask |= 0x2000;
        }
        if (lbl_80281664[lbl_802822F4].nClubBits & 0x200) {
            gPlayers[0].golfer.uBagMask |= 0x4000;
        }
        if (lbl_80281664[lbl_802822F4].nClubBits & 0x400) {
            gPlayers[0].golfer.uBagMask |= 0x8000;
        }
        if (lbl_80281664[lbl_802822F4].nClubBits & 0x800) {
            gPlayers[0].golfer.uBagMask |= 0x10000;
        }
        if (lbl_80281664[lbl_802822F4].nClubBits & 0x1000) {
            gPlayers[0].golfer.uBagMask |= 0x20000;
        }
        if (lbl_80281664[lbl_802822F4].nClubBits & 0x2000) {
            gPlayers[0].golfer.uBagMask |= 0x40000;
        }
        if (lbl_80281664[lbl_802822F4].nClubBits & 0x4000) {
            gPlayers[0].golfer.uBagMask |= 0x80000;
        }
        if (lbl_80281664[lbl_802822F4].nClubBits & 0x8000) {
            gPlayers[0].golfer.uBagMask |= 0x200000;
        }
        if (lbl_80281664[lbl_802822F4].nClubBits & 0x10000) {
            gPlayers[0].golfer.uBagMask |= 0x800000;
        }
    }
    if (lbl_80281664[lbl_802822F4].b4D) {
        fn_800ED6F8(lbl_80281664[lbl_802822F4].f54);
    }
}

// Per medal (0..2): three messages to pick from.
s32 lbl_801925F0[3][3] = {
    {0, 1, 2},
    {3, 4, 5},
    {6, 7, 8},
};

// A message of kind n: one of three at random.
void fn_800EC170(int n) {
    s32 nMsg = lbl_801925F0[n][Rand_Next(0) % 3];
    fn_800E4364(7, nMsg, 0, 0);
}

// The challenge is over: a better medal than the profile's best is saved (with a stamp); the medal's
// message; for a profile, the medal's reward, then two bonuses (0x1C, and 0xC once every challenge
// has a medal).
void fn_800EC1E0(void) {
    s32 aOut[18];               // the payout's breakdown, handed on as a CourseMoneyTracking
    int nReward;
    int nMedal;
    int nProfile;
    int nMoney;
    lbl_80282328();
    if (!fn_800F0818()) {
        nMedal = fn_800EC558();
        if (!fn_801025F4() && nMedal != 3) {
            if (nMedal < gpSaveData[gPlayers[0].nIndex].aMedal[fn_800EAC7C()]) {
                gpSaveData[gPlayers[0].nIndex].aMedal[fn_800EAC7C()] = nMedal;
                gpSaveData[gPlayers[0].nIndex].aMedalDate[fn_800EAC7C()] = fn_800D2994();
            }
            switch (nMedal) {
            case 0:
                nReward = lbl_80281664[lbl_802822F4].aMedal[0].nReward;
                break;
            case 1:
                nReward = lbl_80281664[lbl_802822F4].aMedal[1].nReward;
                break;
            case 2:
                nReward = lbl_80281664[lbl_802822F4].aMedal[2].nReward;
                break;
            }
            fn_800EC170(nMedal);
            nProfile = gPlayers[0].nIndex;
            if (gpSaveData[nProfile].bActive) {
                nMoney = fn_800D7220(nReward, 0, (CourseMoneyTracking*)aOut);
                if (nMoney) {
                    switch (nMedal) {
                    case 0:
                        fn_800E4364(0, 0x6F, nMoney, nProfile);
                        break;
                    case 1:
                        fn_800E4364(0, 0x70, nMoney, nProfile);
                        break;
                    case 2:
                        fn_800E4364(0, 0x71, nMoney, nProfile);
                        break;
                    }
                }
                fn_800D3548(0, nMoney, (CourseMoneyTracking*)aOut);
                if (fn_800ED6F0() && fn_800D9998(0, 0x1C) && fn_800D750C(0, 0x1C)) {
                    fn_800E4364(6, 0x1C, lbl_80200538.nA24, nProfile);
                    fn_800D3548(0, lbl_80200538.nA24, 0);
                    gPlayers[0].money.n8 += lbl_80200538.nA24;
                }
                if (fn_800EC4F0(nProfile) && fn_800D750C(0, 0xC)) {
                    fn_800E4364(2, 0xC, lbl_80200538.n9E4, nProfile);
                    fn_800D3548(0, lbl_80200538.n9E4, 0);
                    gPlayers[0].money.n8 += lbl_80200538.n9E4;
                }
            }
        }
    }
}

// Whether profile n has done every challenge: it has a TOUR card (level 1 or more) and none of the
// 29 best medals is 3 (none).
u8 fn_800EC4F0(int n) {
    SaveProfile* p = &gpSaveData[n];
    int i;
    if (p->nTourCardLevel < 1) {
        return 0;
    }
    for (i = 0; i < 29; i++) {
        if (p->aMedal[i] == 3) {
            return 0;
        }
    }
    return 1;
}

void fn_800EC544(Challenge* p0, s32 p1) {
    lbl_80281664 = p0;
    lbl_80281668 = p1;
}

u8 fn_800EC550(void) {
    return lbl_802822FC;
}

// The medal earned (0 best, 3 none): for each medal its rule against the group's totals (strokes
// plus a number, against par, birdie or par or bogey golf, the n290 total below a mark, a match
// margin or strokes against player 1's, the best n274 with nD8 within the mark, nDD8) or against
// this hole.
int fn_800EC558(void) {
    int m;
    int nRule;
    int nMark;
    int nStrokes;
    int nPen;
    int nSum;
    int nPar;
    int nHoles;
    u8 bBest;
    int i;
    int nPlayoff;
    for (m = 0; m < 3; m++) {
        switch (m) {
        case 0:
            nRule = lbl_80281664[lbl_802822F4].aMedal[0].nRule;
            nMark = lbl_80281664[lbl_802822F4].aMedal[0].nMark;
            break;
        case 1:
            nRule = lbl_80281664[lbl_802822F4].aMedal[1].nRule;
            nMark = lbl_80281664[lbl_802822F4].aMedal[1].nMark;
            break;
        case 2:
            nRule = lbl_80281664[lbl_802822F4].aMedal[2].nRule;
            nMark = lbl_80281664[lbl_802822F4].aMedal[2].nMark;
            break;
        }
        switch (lbl_80281664[lbl_802822F4].nScoring) {
        case 0:
            nStrokes = lbl_8028230C;
            nPar = lbl_80282308;
            nHoles = lbl_80282304;
            nPen = lbl_80282300;
            switch (nRule) {
            case 1:
                if (nStrokes + lbl_80281664[lbl_802822F4].nTargetBase <= nMark) {
                    return m;
                }
                break;
            case 2:
                if (nStrokes - nPar <= nMark) {
                    return m;
                }
                break;
            case 3:
                if (nStrokes <= nPar - nHoles) {
                    return m;
                }
                break;
            case 4:
                if (nStrokes <= nPar) {
                    return m;
                }
                break;
            case 5:
                if (nStrokes <= nPar + nHoles) {
                    return m;
                }
                break;
            case 6:
                if (nPen < nMark) {
                    return m;
                }
                break;
            case 7:
                if (gpGame->n4 == 1) {
                    if (fn_800ED6F0()) {
                        if (gpGame->bD4) {
                            nPlayoff = 2;
                            if (gPlayers[0].nHolesWon > gPlayers[1].nHolesWon) {
                                nPlayoff = 1;
                            }
                            return nPlayoff;
                        }
                        if (gPlayers[0].nHolesWon - gPlayers[1].nHolesWon >= nMark) {
                            return m;
                        }
                    } else if (gpGame->bD4) {
                        if (nMark == 0 && gPlayers[0].nHolesWon > gPlayers[1].nHolesWon) {
                            return m;
                        }
                    } else if (gPlayers[0].nHolesWon - gPlayers[1].nHolesWon >= nMark) {
                        return m;
                    }
                }
                if (gpGame->n4 == 0) {
                    if (gSession.nNumPlayers > 1) {
                        nSum = 0;
                        for (i = 0; i < 18; i++) {
                            if (gpGame->bHoleSelected[i]) {
                                nSum += gPlayers[1].nStrokes[i];
                            }
                        }
                    }
                    // EA bug: with one player nSum is never set, so this compares whatever the
                    // register holds.
                    if (nSum - nStrokes <= nMark) {
                        return m;
                    }
                }
                break;
            case 8:
                bBest = 1;
                for (i = 1; i < gNumPlayersSetUp; i++) {
                    if (gPlayers[0].n274 <= PLAYER(i)->n274) {
                        bBest = 0;
                    }
                }
                if (bBest && gpGame->nD8 <= nMark) {
                    return m;
                }
                break;
            case 9:
                if (fn_800E39F0() && gPlayers[0].nDD8 >= nMark) {
                    return m;
                }
                break;
            }
            break;
        case 1:
            nStrokes = gPlayers[0].nStrokes[Game_CurHoleIndex()];
            nPar = fn_800D2B08();
            switch (nRule) {
            case 1:
                if (nStrokes <= nMark) {
                    return m;
                }
                break;
            case 2:
                if (nStrokes - nPar <= nMark) {
                    return m;
                }
                break;
            case 3:
                if (nStrokes <= nPar - 1) {
                    return m;
                }
                break;
            case 4:
                if (nStrokes <= nPar) {
                    return m;
                }
                break;
            case 5:
                if (nStrokes <= nPar + 1) {
                    return m;
                }
                break;
            case 6:
                // EA bug: nPen is only set when the round's totals are scored (nScoring 0), so
                // this compares whatever the register holds.
                if (nPen < nMark) {
                    return m;
                }
                break;
            case 7:
                if (gpGame->bD4 && gPlayers[0].nHolesWon > gPlayers[1].nHolesWon) {
                    return 2;
                }
                if (gPlayers[0].nHolesWon - gPlayers[1].nHolesWon >= nMark) {
                    return m;
                }
                break;
            case 9:
                if (fn_800E39F0() && gPlayers[0].nDD8 >= nMark) {
                    return m;
                }
                break;
            }
            break;
        }
    }
    return 3;
}

// Whether the game mode is 8.
u8 fn_800ECA08(void) {
    return Game_GetMode() == 8;
}

// The score for a medal over the current group, counted from the lowest (k 0 is aMedal[2]): the
// group's last challenge's mark minus the whole group's target (0 for target kind 1, and kind 7 in
// mode 0; -1 when the current challenge gives no such medal).
s32 fn_800ECA34(int k) {
    int i;
    int nLast = lbl_802822F4;
    int nTarget;
    for (i = 0; i < lbl_80281668; i++) {
        if (lbl_80281664[i].nGroup == lbl_80281664[lbl_802822F4].nGroup && i > nLast) {
            nLast = i;
        }
    }
    nTarget = fn_800ECF9C(lbl_802822F4);
    if ((lbl_80281664[lbl_802822F4].nTargetKind == 7 && Game_GetMode() == 0) ||
        lbl_80281664[lbl_802822F4].nTargetKind == 1) {
        nTarget = 0;
    }
    switch (k) {
    case 0:
        if (lbl_80281664[lbl_802822F4].aMedal[2].nRule == 0) {
            return -1;
        } else {
            return lbl_80281664[nLast].aMedal[2].nMark - nTarget;
        }
    case 1:
        if (lbl_80281664[lbl_802822F4].aMedal[1].nRule == 0) {
            return -1;
        } else {
            return lbl_80281664[nLast].aMedal[1].nMark - nTarget;
        }
    case 2:
        if (lbl_80281664[lbl_802822F4].aMedal[0].nRule == 0) {
            return -1;
        } else {
            return lbl_80281664[nLast].aMedal[0].nMark - nTarget;
        }
    default:
        return 0;
    }
}

void fn_800ECBE4(void) {
    if (Game_GetMode() == 8) {
        fn_800ED710(0);
    }
}

// The number shown against the target: the match margin (mode 1), the n290 total (mode 8), nD8
// (mode 2), nDD8, or the group's strokes so far minus its target so far.
int fn_800ECC14(void) {
    int nTarget;
    int nScore;
    int i;
    int k;
    int h;
    if (Game_GetMode() == 1) {
        if (gpGame->bD4) {
            return 0;
        }
        return gPlayers[0].nHolesWon - gPlayers[1].nHolesWon;
    }
    if (Game_GetMode() == 8) {
        nScore = 0;
        for (h = 0; h < 18; h++) {
            if (gpGame->bHoleSelected[h]) {
                nScore += gPlayers[0].n290[h];
            }
        }
        return nScore;
    }
    i = Game_GetMode();     // i is the mode here, a challenge index below
    if (i == 2) {
        return gpGame->nD8;
    }
    if (fn_800E39F0()) {
        return gPlayers[0].nDD8;
    }
    i = 0;
    nTarget = 0;
    for (; i <= lbl_802822F4; i++) {
        if (lbl_80281664[i].nGroup == lbl_80281664[lbl_802822F4].nGroup) {
            nTarget += fn_800ED028(i);
        }
    }
    k = 0;
    nScore = lbl_80281664[lbl_802822F4].nTargetKind != 1 ? lbl_8028230C : 0;
    if (Game_GetMode() == 0 && lbl_80281664[lbl_802822F4].nTargetKind == 7) {
        if (fn_800E4BF8()) {
            k = 1;
        }
        for (h = 0; h < k + Game_CurHoleIndex(); h++) {
            if (gpGame->bHoleSelected[h]) {
                nScore += gPlayers[0].nStrokes[h];
            }
        }
    } else {
        for (h = 0; h < 18; h++) {
            if (gpGame->bHoleSelected[h] || lbl_80281664[lbl_802822F4].nTargetKind == 1) {
                nScore += gPlayers[0].nStrokes[h];
            }
        }
    }
    if (lbl_80281664[lbl_802822F4].nTargetKind == 1) {
        nTarget = 0;
    }
    return nScore - nTarget;
}

// The total of fn_800ED028 over the current challenge's group.
int fn_800ECF9C(int iUnused) {
    int n = 0;
    int i;
    for (i = 0; i < lbl_80281668; i++) {
        if (lbl_80281664[i].nGroup == lbl_80281664[lbl_802822F4].nGroup) {
            n += fn_800ED028(i);
        }
    }
    return n;
}

// A challenge's target score (in mode 0 only): a fixed number (1); par for the holes before the
// challenge hole plus a number (2); birdie (3), par (4) or bogey (5) on each of them; or the
// opponent's strokes so far (7); then the challenge hole adds a number (1), par plus a number (2),
// birdie (3), par (4) or bogey (5).
int fn_800ED028(int i) {
    int nTarget = 0;
    int nHole = lbl_80281664[i].nHole - 1;
    int k;               // a loop counter, and in case 7 the current hole once it is over
    int h;
    h = Game_GetMode();     // h is the mode here, a hole number below
    if (h == 0) {
        switch (lbl_80281664[i].nTargetKind) {
        case 1:
            nTarget = lbl_80281664[i].nTargetBase;
            break;
        case 2:
            for (k = 0; k < nHole; k++) {
                nTarget += fn_800D2ABC(lbl_80281664[i].nCourse, k);
            }
            nTarget += lbl_80281664[i].nTargetBase;
            break;
        case 3:
            for (h = 0; h < nHole; h++) {
                nTarget += fn_800D2ABC(lbl_80281664[i].nCourse, h) - 1;
            }
            break;
        case 4:
            for (h = 0; h < nHole; h++) {
                nTarget += fn_800D2ABC(lbl_80281664[i].nCourse, h);
            }
            break;
        case 5:
            for (h = 0; h < nHole; h++) {
                nTarget += fn_800D2ABC(lbl_80281664[i].nCourse, h) + 1;
            }
            break;
        case 7:
            k = 0;
            if (fn_800E4BF8()) {
                k = 1;
            }
            for (h = 0; h < k + Game_CurHoleIndex(); h++) {
                if (gpGame->bHoleSelected[h]) {
                    nTarget += gPlayers[1].nStrokes[h];
                }
            }
            break;
        }
        switch (lbl_80281664[i].nHoleKind) {
        case 1:
            nTarget += lbl_80281664[i].nHoleExtra;
            break;
        case 2:
            nTarget += fn_800D2ABC(lbl_80281664[i].nCourse, nHole) + lbl_80281664[i].nHoleExtra;
            break;
        case 3:
            nTarget += fn_800D2ABC(lbl_80281664[i].nCourse, nHole) - 1;
            break;
        case 4:
            nTarget += fn_800D2ABC(lbl_80281664[i].nCourse, nHole);
            break;
        case 5:
            nTarget += fn_800D2ABC(lbl_80281664[i].nCourse, nHole) + 1;
            break;
        }
    }
    return nTarget;
}

// Challenge text lines (offsets into the text block).
char* fn_800ED280(int nId) {
    int i = fn_800EAC94(nId);
    // EA bug: fn_800EAC94 returns 0, never -1, for a group it does not find, so this test never
    // passes and an unknown id gets challenge 0's line.
    if (i == -1) {
        return 0;
    }
    return lbl_80282310 + lbl_80203554[i].n0;
}

char* fn_800ED2C8(int nId) {
    int i = fn_800EAC94(nId);
    // EA bug: never -1, as above.
    if (i == -1) {
        return 0;
    }
    return lbl_80282310 + lbl_80203554[i].n4;
}

// The holes left in the group: the rest of this round, plus each later challenge's holes (one, 18,
// a nine, or the par 5s/4s/3s of its course).
int fn_800ED314(void) {
    int h;
    int i;
    int n = 0;
    for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            n++;
        }
    }
    i = !fn_800ED5C8(0, 1);
    if (fn_800ED508(lbl_80281664[lbl_802822F4].nGroup) > 1) {
        for (i = lbl_802822F4 + i; i < lbl_80281668; i++) {
            if (lbl_80281664[i].nGroup == lbl_80281664[lbl_802822F4].nGroup) {
                switch (lbl_80281664[i].nType) {
                case 0:
                    n += 1;
                    break;
                case 1:
                    n += 18;
                    break;
                case 2:
                    n += 9;
                    break;
                case 3:
                    n += 9;
                    break;
                case 4:
                    for (h = 0; h < 18; h++) {
                        if (fn_800D2ABC(lbl_80281664[i].nCourse, h) == 5) {
                            n++;
                        }
                    }
                    break;
                case 5:
                    for (h = 0; h < 18; h++) {
                        if (fn_800D2ABC(lbl_80281664[i].nCourse, h) == 4) {
                            n++;
                        }
                    }
                    break;
                case 6:
                    for (h = 0; h < 18; h++) {
                        if (fn_800D2ABC(lbl_80281664[i].nCourse, h) == 3) {
                            n++;
                        }
                    }
                    break;
                case 7:
                    break;
                }
            }
        }
    }
    if (fn_800E5110()) {
        return 0;
    }
    // fake match: the binary calls fn_800E4BF8 and branches on its result, but both paths return
    // n (a bare call without the test loses the compare: 99.2%).
    if (fn_800E4BF8()) {
        return n;
    }
    return n;
}

// How many challenges are in a group.
int fn_800ED508(int nGroup) {
    int n = 0;
    int i;
    for (i = 0; i < lbl_80281668; i++) {
        if (nGroup == lbl_80281664[i].nGroup) {
            n++;
        }
    }
    return n;
}

u8 fn_800ED540(void) {
    return lbl_80282314;
}

void fn_800ED548(void) {
    lbl_80282314 = 0;
}

// Restart: back to the challenge it was started on (a UI command, after GM_RestartHole).
void fn_800ED554(void) {
    lbl_802822FE = 1;
    gpGame->b134 = 1;
    lbl_802822F4 = lbl_802822F8;
    GM_EndOfGolferTurn(0);
    gpGame->pfn224();
    fn_800E5714(2);
    fn_800E1074();
    if (fn_800F0818()) {
        GameModeDriverRTE_StartEvent();
    } else {
        fn_800EAF7C();
    }
    fn_80019648();
}

// Hole finished: always after a restart; otherwise the mode's own test.
u8 fn_800ED5C8(int nPlayer, u8 bCheck) {
    if (lbl_802822FE) {
        return 1;
    }
    return lbl_8028231C(nPlayer, bCheck);
}

// The hole is over, the game is not: after a restart fn_800E4D88 runs and the restart flag and b275
// are cleared; otherwise the mode's own callback runs.
void fn_800ED604(int nPlayer) {
    if (lbl_802822FE) {
        fn_800E4D88();
        lbl_802822FE = 0;
        gpGame->b275 = 0;
        return;
    }
    lbl_80282318(nPlayer);
}

// Challenge i's three rewards, the lowest medal's first.
void fn_800ED650(int i, s32* pA, s32* pB, s32* pC) {
    *pA = lbl_80281664[i].aMedal[2].nReward;
    *pB = lbl_80281664[i].aMedal[1].nReward;
    *pC = lbl_80281664[i].aMedal[0].nReward;
}

s32 fn_800ED688(int i) {
    return lbl_80281664[i].nOpponents;
}

s32 fn_800ED69C(int i, int k) {
    if (k == 0) {
        return lbl_80281664[i].aOpponent[0];
    }
    if (k == 1) {
        return lbl_80281664[i].aOpponent[1];
    }
    return lbl_80281664[i].aOpponent[2];
}

void fn_800ED6E8(u8 v) {
    lbl_802822FD = v;
}

u8 fn_800ED6F0(void) {
    return lbl_802822FD;
}

void fn_800ED6F8(f32 x0) {
    lbl_802811F0->b1C = 1;
    lbl_802811F0->f18 = x0;
}

void fn_800ED710(s32 p0) {
    fn_80062D6C(18, p0);
}
