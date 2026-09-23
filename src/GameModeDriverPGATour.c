// GameModeDriverPGATour.c (TW06's GameModeDriverPGATour): game mode 23, a PGA Tour season of 31
// tournaments (gPgaData, loaded from the 'PGA' stream objects), with the player's results kept in
// the save profile (TourSeason): "Did Not Play", "Cut", a finishing place, "Tied (%d players)".

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "game/modes/pgatour.h"

Pga80205F30* fn_800EE8B8(void);
void fn_800EDEE8(void);
void fn_800EDF34(UStreamObject* pObject);
void fn_800EDF60(UStreamObject* pObject);
void fn_800EDF90(UStreamObject* pObject);
void fn_800EE064(void);
s32 fn_800EE8B0(int nPlayer);
void fn_80119934(int a);
void fn_800EF294(void);
s32 fn_800EF834(void);
s32 fn_800EFB88(void);
s32 fn_800EFBAC(void);
char* fn_800EFDFC(s32 i);
Tournament* fn_800EFA70(s32 i);
s32 fn_800EFE3C(s32 i);
char* fn_800EFE60(s32 i);
void fn_800F009C(void);
u8 fn_8011908C(s32, s32);
void fn_800F018C(void);

void fn_800EE0A0(s32 i);
void fn_80117DE8(s32 a, s32 b);
s32  fn_801190D8(s32 a, s32 n);
s32  fn_800EFBD0(s32 i);
void fn_800D27CC(u16* pDate, s32 nDays);
void fn_800907AC(s32 nMoney, char* pDst);
void fn_80117C50(s32 a, s32 b);
void fn_800EF130(s32 nPlayer, u8 bQuick);
void fn_800EEB94(s32 a);
void fn_80117860(TourSeason* pTour);
s32  fn_8008AC00(void);
void fn_8011A720(s32 a, s32 nHole);
void fn_8011A5F8(s32 a);
u8   fn_800EF720(u8 bCheck);
s32  fn_8011A684(s32 a);
u8   fn_8011A6F4(s32 a, s32 b);
s32  fn_80119A04(s32 a, s32 b);
u8   fn_801197A4(s32 nPlayer, s32 b);
u8   fn_80117DE0(void);
void fn_80117B58(s32 a);
s32  fn_801191D0(s32 a, s32 b, s32 c);
void fn_800EEA3C(s32 nPlayer);
void fn_80117DF0(s32 nPlayer);
void fn_80117AF8(s32 nPlayer);
void fn_800EED0C(s32 nPlayer);
void fn_8011A538(s32 nPlayer);
void fn_80117D80(s32 nPlayer);
u8   fn_800EF83C(u16 nDate, s32* pId, s32* pRound);
s32  fn_8011A7C8(s32 nPlayer, s32 nHole);
s32  fn_80119588(s32 nPlayer, s32 a);
s32   fn_80118684(s32 a);
char* fn_80118E30(s32 a, s32 b);
s32   fn_80119118(s32 a, s32 b);
s32   fn_801197CC(s32 a, s32 b);
void  fn_800EDFC0(UStreamObject* pObject);
s32  fn_800F02A8(void);
s32  fn_800EFA9C(s32 i);
void fn_800EE02C(void);
u8   fn_800EF64C(u8 bCheck);
void fn_800EF2B8(void);
void fn_800EE478(void);
u8   fn_800EE5B4(int nPlayer);
u8   fn_800EE6A0(s32 nPlayer);
s32  fn_800EE778(int nPlayer);
s32  fn_800EE810(int nPlayer);
void fn_800EE8C4(void);
s32  fn_800EF0E0(s32 nPlayer);
void fn_80117E98(s32 nPlayer);
s32  fn_80119A2C(s32 nPlayer, s32 a);
void fn_801178C8(s32 nPlayer, SeasonEvent* pEvent, s32 nRound, s32 n, s32 k);

// TW06: GameModeDriverPGATour::Init. Stroke play's hole and honors rules, the tour's own round and
// playoff handling; no mulligans, one player.
void fn_800EDD18(void) {
    gpGame->pfnInit = fn_800EDD18;
    gpGame->pfnShutdown = fn_800EE02C;
    gpGame->pfn1E4 = fn_800EF294;
    gpGame->pfnSetupNextGolfer = fn_800FF7DC;
    gpGame->pfnGetHonors = fn_800FF894;
    gpGame->pfnHoleFinished = fn_800FFCCC;
    gpGame->pfnGameFinished = fn_800EF64C;
    gpGame->pfnGoToPlayoff = fn_800EF720;
    gpGame->pfn1EC = fn_800EE064;
    gpGame->pfnEndHole = fn_800EF2B8;
    gpGame->pfnEndGame = fn_800EE478;
    gpGame->pfn1F8 = fn_800EE5B4;
    // IsPuttForWin's player is an s32 (long): as an int its profile index compiles differently
    gpGame->pfn1FC = (u8 (*)(int))fn_800EE6A0;
    gpGame->pfn200 = fn_800EE778;
    gpGame->pfn204 = fn_800EE810;
    gpGame->pfn208 = fn_800EE8B0;
    gpGame->b274 = 0;
    gpGame->n4 = 0;
    gpGame->nMulligans = 0;
    gpGame->nC = 1;
    gpGame->n10 = 1;
    gpGame->nDC = 0;
    gpGame->nE0 = 1;
    gSession.nSplitScreen = 0;
}

void fn_800EDE78(void) {
}

// TW06: GameModeDriverPGATour::RegisterStreamClients.
void fn_800EDE7C(void) {
    UStream_RegisterHandler('PGAc', fn_800EDF34);
    UStream_RegisterHandler('PGAt', fn_800EDF60);
    UStream_RegisterHandler('PGAp', fn_800EDF90);
    UStream_RegisterHandler('PGAn', fn_800EDFC0);
}

// TW06: GameModeDriverPGATour::UnregisterStreamClients.
void fn_800EDEE8(void) {
    UStream_UnregisterHandler('PGAc');
    UStream_UnregisterHandler('PGAt');
    UStream_UnregisterHandler('PGAp');
    UStream_UnregisterHandler('PGAn');
}

// TW06: GameModeDriverPGATour::LoadPGAcFromStream.
void fn_800EDF34(UStreamObject* pObject) {
    // port: the 'PGAc' object is copied straight into gPgaData.aTournament (Tournament[31]); it is
    // big-endian on disc, so a little-endian port converts it field by field here
    // (docs/format-byteorder.md)
    fn_8000E790(pObject, sizeof(gPgaData.aTournament), gPgaData.aTournament);
}

// TW06: GameModeDriverPGATour::LoadPGAtFromStream.
void fn_800EDF60(UStreamObject* pObject) {
    // port: the 'PGAt' object is copied straight into gPgaData.aTourEvent (TourEvent[31]); it is
    // big-endian on disc, so a little-endian port converts it field by field here
    // (docs/format-byteorder.md)
    fn_8000E790(pObject, sizeof(gPgaData.aTourEvent), gPgaData.aTourEvent);
}

// TW06: GameModeDriverPGATour::LoadPGApFromStream.
void fn_800EDF90(UStreamObject* pObject) {
    // port: the 'PGAp' object is copied straight into gPgaData.aTriple (PgaTriple[11]); it is
    // big-endian on disc, so a little-endian port converts it field by field here
    // (docs/format-byteorder.md)
    fn_8000E790(pObject, sizeof(gPgaData.aTriple), gPgaData.aTriple);
}

// TW06: GameModeDriverPGATour::Locale_PgaTourMode_LoadPGAnFromStream. The 'PGAn' object: the names
// block is copied out.
void fn_800EDFC0(UStreamObject* pObject) {
    void* pData;
    u32 nSize = fn_8000E81C(pObject, &pData);
    if (nSize) {
        gPgaData.pNames = fn_800951A0(nSize, 0x10, 1);
        Mem_cpy(gPgaData.pNames, pData, nSize);
        fn_80009E70(pObject);
    }
}

// The mode ends: one player back, and the options it changed come back.
void fn_800EE02C(void) {
    gpGame->nC = 1;
    gpGame->n10 = 1;
    gSession.options.nC = lbl_80281670;
    gSession.options.nWind = lbl_80282338;
    lbl_8028233C = 0;
}

// The current tournament's number of rounds goes into the game state.
void fn_800EE064(void) {
    s32 nTourEvent = gPgaData.aTournament[gpSaveData->tour.nEvent].nTourEvent - 1;
    gpGame->nE0 = gPgaData.aTourEvent[nTourEvent].nRounds;
}

// The course of the tournament format i's current round: everyone plays its tee set, every hole its
// pin position, and its GameOptions.n18 replaces the player's (kept in lbl_80281674).
// Not exact yet (only gPgaData's and gSession's address loads come out in the other order); the
// tee set's store back to the tournament is in the original.
void fn_800EE0A0(s32 i) {
    PlayerNumber_t nPlayer = PLR_1_e;
    TourEvent* pEvent = &gPgaData.aTourEvent[i];
    int k;
    int h;
    for (k = 0; k < 5; k++) {
        gSession.nTeeSet[k] = pEvent->nTeeSet;
    }
    pEvent->nTeeSet = gSession.nTeeSet[4];
    fn_800E14E0(gPgaData.aTourEvent[i].aRound[gpGame->nDC].nCourse);
    gSession.nPinSet = gPgaData.aTourEvent[i].aRound[gpSaveData[nPlayer].tour.nRound].nPinSet - 1;
    for (h = 0; h < 18; h++) {
        gpGame->nPinSet[h] = gPgaData.aTourEvent[i].aRound[gpSaveData[nPlayer].tour.nRound].nPinSet - 1;
    }
    lbl_80281674 = gSession.options.n18;
    gSession.options.n18 = (u8)gPgaData.aTourEvent[i].aRound[gpSaveData[nPlayer].tour.nRound].n8;
    fn_80055C40(gSession.options.n18);
}

// A round of the current tournament starts: the options nC and nWind are kept (fn_800EE02C puts
// them back) and set to 4 and calm, one player plays the round's course, and the round's field is
// set up.
void fn_800EE2C8(void) {
    PlayerNumber_t nPlayer = PLR_1_e;
    s32 nEvent = gpSaveData[nPlayer].tour.nEvent;
    TourStats* pRec = &lbl_80205ED8;
    s32 nFormat;
    lbl_80281670 = gSession.options.nC;
    lbl_80282338 = gSession.options.nWind;
    gSession.options.nC = 4;
    gSession.options.nWind = 0;
    lbl_8028233C = 1;
    lbl_80282340 = 16;
    lbl_80205F30.b0 = 0;
    if (gPgaData.aTournament[nEvent].nTourEvent) {
        gSession.nNumPlayers = 1;
        gpGame->nDC = gpSaveData[nPlayer].tour.nRound;
        nFormat = gPgaData.aTournament[nEvent].nTourEvent - 1;
        gpGame->nE0 = gPgaData.aTourEvent[nFormat].nRounds;
        fn_800EE0A0(gPgaData.aTournament[nEvent].nTourEvent - 1);
        fn_80117DE8(0, 0);
        fn_801178C8(0, &gpSaveData[nPlayer].tour.aEvent[gpSaveData[nPlayer].tour.nEvent],
                    gpSaveData[nPlayer].tour.nRound, gPgaData.aTourEvent[nFormat].a40[fn_800EF0E0(0)], 5);
        fn_80005AE8(pRec, 0, sizeof(*pRec));
        pRec->n2++;
        if (gpSaveData[nPlayer].tour.nRound == 0) {
            pRec->n0++;
        }
        fn_800E1260(1);
    }
}

u8 fn_800EE470(void) {
    return lbl_8028233C;
}

// A profile, and its current tournament. fake match: fn_800EE478 reaches the profile through these
// in two statements, where the original adds the profile's offset to gpSaveData last (indexed
// load/store); gpSaveData[nPlayer] written out adds it first.
static inline SaveProfile* Tour_Profile(PlayerNumber_t nPlayer) {
    return &gpSaveData[nPlayer];
}

static inline SeasonEvent* Tour_CurrentEvent(PlayerNumber_t nPlayer) {
    return &gpSaveData[nPlayer].tour.aEvent[gpSaveData[nPlayer].tour.nEvent];
}

// A round of the current tournament is over for profile 0: after the second round of a
// tournament of four or more the cut is checked, the round's score is kept, and after the last
// round the tournament ends.
void fn_800EE478(void) {
    PlayerNumber_t nPlayer = PLR_1_e;
    if (gpSaveData[nPlayer].tour.nRound == 0) {
        Tour_Profile(nPlayer)->n104C8++;
    }
    if (fn_800EFA9C(gpSaveData[nPlayer].tour.nEvent) >= 4 && gpSaveData[nPlayer].tour.nRound == 1) {
        fn_80117B58(0);
        if (fn_801197A4(0, 0)) {
            Tour_CurrentEvent(nPlayer)->nUserRankType = 1;
        }
    }
    gpSaveData[nPlayer].tour.aEvent[gpSaveData[nPlayer].tour.nEvent].nUserScore = fn_801191D0(0, 0, 1);
    if (gpSaveData[nPlayer].tour.nRound + 1 >= fn_800EFA9C(gpSaveData[nPlayer].tour.nEvent)) {
        fn_800EEA3C(0);
    }
}

// TW06: GameModeDriverPGATour::IsPuttForLead. Whether holing this putt puts the player in the lead:
// in a playoff, beating the best score on this hole; otherwise, not ahead now and ahead with it.
u8 fn_800EE5B4(int nPlayer) {
    int bLead;
    if (gpGame->bD4) {
        return gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1 <
               fn_8011A7C8(nPlayer, Game_CurHoleIndex());
    }
    bLead = 0;
    if (fn_800E1904(nPlayer, 0) >= fn_80119588(nPlayer, 1) &&
        fn_800E1904(nPlayer, 1) + 1 < fn_80119588(nPlayer, 1)) {
        bLead = 1;
    }
    return bLead;
}

// TW06: GameModeDriverPGATour::IsPuttForWin. In a playoff, a putt for the lead; otherwise on the
// last round, a putt that would put the player ahead.
u8 fn_800EE6A0(s32 nPlayer) {
    s32 nRounds;
    if (gpGame->bD4) {
        return fn_800EE5B4(nPlayer);
    }
    nRounds = fn_800EFA9C(gpSaveData[nPlayer].tour.nEvent);
    return fn_8008AC00() == 1 && gpSaveData[nPlayer].tour.nRound + 1 >= nRounds &&
           fn_800E1904(nPlayer, 1) + 1 < fn_80119588(nPlayer, 1);
}

// TW06: GameModeDriverPGATour::GetCurrentLead. Strokes behind the leader (in a playoff, on this hole).
s32 fn_800EE778(int nPlayer) {
    if (gpGame->bD4) {
        return fn_8011A7C8(nPlayer, Game_CurHoleIndex()) - gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
    }
    return fn_80119588(nPlayer, 1) - fn_800E1904(nPlayer, 0);
}

// TW06: GameModeDriverPGATour::GetPotentialLead.
s32 fn_800EE810(int nPlayer) {
    if (gpGame->bD4) {
        return fn_8011A7C8(nPlayer, Game_CurHoleIndex()) -
               (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1);
    }
    return fn_80119588(nPlayer, 1) - (fn_800E1904(nPlayer, 1) + 1);
}

// The mode's pfn208 answer for every player (GameRound.c's default works it out).
s32 fn_800EE8B0(int nPlayer) {
    return 3;
}

Pga80205F30* fn_800EE8B8(void) {
    return &lbl_80205F30;
}

// The message after a tournament the player won: the first win, then either three wins of the
// tournaments marked nC, one of four random ones, or for tournaments 9 and 8 their own.
void fn_800EE8C4(void) {
    PlayerNumber_t nPlayer = PLR_1_e;
    Tournament* p = fn_800EFA70(gpSaveData[nPlayer].tour.nEvent);
    int i;
    int nWins;
    if (lbl_80205F30.b0 == 1) {
        if (fn_800F02A8() == 0) {
            fn_800E4364(5, 31, 0, 0);
        }
        if (p->nC) {
            nWins = 0;
            for (i = 0; i <= gpSaveData[nPlayer].tour.nEvent; i++) {
                if (gPgaData.aTournament[i].nC && gpSaveData[nPlayer].tour.aEvent[i].nUserRank == 1) {
                    nWins++;
                }
            }
            if (nWins >= 3) {
                fn_800E4364(5, 8, 0, 0);
            } else {
                fn_800E4364(5, (Rand_Next(1) & 3) + 27, 0, 0);
            }
        } else if (gpSaveData[nPlayer].tour.nEvent == 9) {
            fn_800E4364(5, 9, 0, 0);
        } else if (gpSaveData[nPlayer].tour.nEvent == 8) {
            fn_800E4364(5, 10, 0, 0);
        }
    }
}

// The last round is over: a win is recorded in the profile (with its score and the tournament's
// aPrize[bracket][1]) and its message queued, and the prize money is paid.
void fn_800EEA3C(s32 nPlayer) {
    s32 nBracket = fn_800EF0E0(nPlayer);
    Tournament* p = fn_800EFA70(gpSaveData[nPlayer].tour.nEvent);
    s32 nMoney;
    fn_80117E98(nPlayer);
    if (fn_801190D8(nPlayer, 0) == 1) {
        fn_800EE8C4();
        if (fn_800D7770(nPlayer, &gpSaveData[nPlayer].aC8[gpSaveData[nPlayer].tour.nEvent].award)) {
            gpSaveData[nPlayer].aC8[gpSaveData[nPlayer].tour.nEvent].nScore = fn_801191D0(nPlayer, 0, 1);
            gpSaveData[nPlayer].aC8[gpSaveData[nPlayer].tour.nEvent].n6 = p->aPrize[nBracket][1];
        }
    }
    nMoney = fn_80119A2C(nPlayer, 0);
    if (nMoney) {
        fn_800D3548(0, nMoney, NULL);
        gPlayers[nPlayer].money.n4 += nMoney;
    }
}

// The tournament is over for the player: its champion and winning score are kept with the
// player's result (cut, a place, or did not play), and the season moves on to the next tournament.
void fn_800EEB94(s32 nPlayer) {
    SeasonEvent* p = &gpSaveData[nPlayer].tour.aEvent[gpSaveData[nPlayer].tour.nEvent];
    s32 nLeader = fn_801197CC(nPlayer, 0);
    s32 nGolfer = fn_80119118(nPlayer, nLeader);
    strcpy(p->szChampName, fn_80118E30(nPlayer, nGolfer));
    p->nChampScore = fn_8011937C(nPlayer, nLeader, 1);
    if (fn_8011908C(nPlayer, 0)) {
        if (fn_801197A4(nPlayer, 0)) {
            p->nUserRank = 0;
            p->nUserScore = 0;
            p->nUserRankType = 1;
        } else {
            p->nUserRank = fn_801190D8(nPlayer, 0);
            p->nUserScore = fn_8011937C(nPlayer, 0, 1);
            p->nUserRankType = 2;
        }
    } else {
        p->nUserRank = 0;
        p->nUserScore = 0;
        p->nUserRankType = 0;
    }
    gpSaveData[nPlayer].tour.nRound = 0;
    gpSaveData[nPlayer].tour.nEvent = fn_800EFBD0(gpSaveData[nPlayer].tour.nEvent + 1);
}

// The round's statistics go into the player's career totals: most are added, n4 and nC keep the
// higher value.
void fn_800EED0C(s32 nPlayer) {
    TourStats* pRound = &lbl_80205ED8;
    TourStats* pTotal = &gpSaveData[nPlayer].tourStats;
    pTotal->n0 += pRound->n0;
    pTotal->n2 += pRound->n2;
    pTotal->n4 = pTotal->n4 <= pRound->n4 ? pRound->n4 : pTotal->n4;
    pTotal->n6 += pRound->n6;
    pTotal->n8 += pRound->n8;
    pTotal->nC = pTotal->nC <= pRound->nC ? pRound->nC : pTotal->nC;
    pTotal->nE += pRound->nE;
    pTotal->n10 += pRound->n10;
    pTotal->n12 += pRound->n12;
    pTotal->n14 += pRound->n14;
    pTotal->n16 += pRound->n16;
    pTotal->n18 += pRound->n18;
    pTotal->n1A += pRound->n1A;
    pTotal->n1C += pRound->n1C;
    pTotal->n1E += pRound->n1E;
    pTotal->n20 += pRound->n20;
    pTotal->n22 += pRound->n22;
    pTotal->n24 += pRound->n24;
    pTotal->n26 += pRound->n26;
    pTotal->n28 += pRound->n28;
    pTotal->n2A += pRound->n2A;
    pTotal->n2C += pRound->n2C;
    pTotal->n2E += pRound->n2E;
    pTotal->n30 += pRound->n30;
    pTotal->n32 += pRound->n32;
    pTotal->n34 += pRound->n34;
    pTotal->n36 += pRound->n36;
    pTotal->n38 += pRound->n38;
    pTotal->n3A += pRound->n3A;
    pTotal->n3C += pRound->n3C;
    pTotal->n40 += pRound->n40;
    pTotal->n44 += pRound->n44;
    pTotal->n48 += pRound->n48;
    pTotal->n4A += pRound->n4A;
    pTotal->n50 += pRound->n50;
    pTotal->n54 += pRound->n54;
}

// A round is over. The round count goes up and a player who missed the cut is out; after the last
// round the tournament ends.
void fn_800EEF88(s32 nPlayer) {
    if (fn_80117DE0()) {
        fn_80117DF0(nPlayer);
        if (gpSaveData[nPlayer].tour.nRound == 0) {
            fn_80117AF8(nPlayer);
        }
    } else {
        fn_800EED0C(nPlayer);
        fn_8011A538(nPlayer);
        fn_80117D80(nPlayer);
        fn_80117DF0(nPlayer);
        gpSaveData[nPlayer].tour.nRound++;
        if (fn_801197A4(nPlayer, 0)) {
            fn_800EF130(nPlayer, 0);
        }
        if (gpSaveData[nPlayer].tour.nRound >= fn_800EFA9C(gpSaveData[nPlayer].tour.nEvent)) {
            fn_800EEB94(nPlayer);
        }
    }
}

void fn_800EF094(s32 a, s32 n) {
    lbl_80205F30.b0 = 1;
    lbl_80205F30.n4 = fn_801190D8(a, 0);
    lbl_80205F30.n8 = n;
}

// The player's bracket, 0..9: tournaments won x 10 / 31 (profile 0's awards; nPlayer is not read).
s32 fn_800EF0E0(s32 nPlayer) {
    s32 n = fn_800F02A8() * 10 / 31;
    return n > 9 ? 9 : n;
}

// The rounds of the current tournament not played yet are played out for the player: each round's
// course is loaded and the round simulated (k 3 when bQuick is set). fn_800EF9D0 skips ahead with it.
void fn_800EF130(s32 nPlayer, u8 bQuick) {
    s32 nRounds = fn_800EFA9C(gpSaveData[nPlayer].tour.nEvent);
    Tournament* p = fn_800EFA70(gpSaveData[nPlayer].tour.nEvent);
    s32 k;
    s32 nTourEvent;
    while (gpSaveData[nPlayer].tour.nRound < nRounds) {
        if (p->nTourEvent) {
            fn_800E14E0(
                gPgaData.aTourEvent[p->nTourEvent - 1].aRound[gpSaveData[nPlayer].tour.nRound].nCourse);
            k = 0;
            nTourEvent = gPgaData.aTournament[gpSaveData[nPlayer].tour.nEvent].nTourEvent - 1;
            if (bQuick) {
                k = 3;
            }
            fn_801178C8(nPlayer, &gpSaveData[nPlayer].tour.aEvent[gpSaveData[nPlayer].tour.nEvent],
                        gpSaveData[nPlayer].tour.nRound,
                        gPgaData.aTourEvent[nTourEvent].a40[fn_800EF0E0(nPlayer)], k);
        }
        gpSaveData[nPlayer].tour.nRound++;
    }
    fn_8011A5F8(nPlayer);
    fn_80117E98(nPlayer);
}

void fn_800EF294(void) {
    fn_80119934(0);
}

// TW06: GameModeDriverPGATour::GameFinished (by its slot). Whether the round is over: no selected
// hole is left and it was the last round, and no playoff follows (in a playoff, after every hole).
u8 fn_800EF64C(u8 bCheck) {
    s32 i;
    if (gpGame->bD4) {
        fn_8011A720(0, Game_CurHoleIndex());
        return fn_800EF720(bCheck) == 0;
    }
    for (i = Game_CurHoleIndex() + 1; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            return 0;
        }
    }
    if (gpGame->nDC + 1 >= gpGame->nE0) {
        fn_8011A5F8(0);
        return fn_800EF720(bCheck) == 0;
    }
    return 1;
}

// TW06: GameModeDriverPGATour::GoToPlayoff (by its slot). A tie for the lead after the last round
// goes to a playoff: the scores are cleared and the playoff holes (16..18 of the course, looping)
// are set up. bCheck is not read.
u8 fn_800EF720(u8 bCheck) {
    u8 bPlayoff = 0;
    s32 i;
    int h;
    if (fn_8011A684(0) > 1 && fn_8011A6F4(0, 0)) {
        bPlayoff = 1;
    }
    if (bPlayoff) {
        gpGame->bD5 = 1;
        for (i = 0; i < gNumPlayersSetUp; i++) {
            for (h = 0; h < 18; h++) {
                PLAYER(i)->nStrokes[h] = 0;
                PLAYER(i)->nModePoints[h] = 0;
            }
        }
        lbl_80282340++;
        if (lbl_80282340 > 17) {
            lbl_80282340 = 15;
        }
        fn_800E1260(0);
        fn_800E1404(lbl_80282340);
        gpGame->bD4 = 1;
        fn_800E45C0();
    }
    return bPlayoff;
}

s32 fn_800EF834(void) {
    return 31;
}

// TW06: GameModeDriverPGATour::GetEventByDate.
// Which tournament (and which of its rounds) is played on a date: each tournament starts on a
// date per season (aStartDate, seasons from 2004).
u8 fn_800EF83C(u16 nDate, s32* pId, s32* pRound) {
    s32 nMonth;
    s32 nDay;
    s32 i;
    s32 d;
    s32 nYear;
    s32 nSeason;
    u8 bFound;
    fn_800D2714(&nDate, &nMonth, &nDay, &nYear);
    bFound = 0;
    nSeason = nYear - 2004;
    if (nSeason >= 0 && nSeason < 10) {
        for (i = 0; i < 31; i++) {
            d = nDate - gPgaData.aTournament[i].aStartDate[nSeason];
            if (d >= 0 && d < fn_800EFA9C(i)) {
                *pId = i;
                bFound = 1;
                *pRound = d;
                break;
            }
        }
    }
    if (!bFound) {
        *pId = -1;
        *pRound = 0;
    }
    return bFound;
}

// TW06: GameModeDriverPGATour::GetSelectedEvent. The tournament profile 0 is on, and its round.
s32 fn_800EF908(s32* pRound) {
    PlayerNumber_t nPlayer = PLR_1_e;
    *pRound = gpSaveData[nPlayer].tour.nRound;
    return gpSaveData[nPlayer].tour.nEvent;
}

// TW06: GameModeDriverPGATour::GetNextEvent. The tournament after the current one.
s32 fn_800EF940(void) {
    PlayerNumber_t nPlayer = PLR_1_e;
    return fn_800EFBD0(gpSaveData[nPlayer].tour.nEvent + 1);
}

// TW06: GameModeDriverPGATour::GetFinalEventOfSeason. The last tournament there is.
s32 fn_800EF984(void) {
    s32 nLast = 0;
    s32 i = fn_800EFBD0(1);
    while (i != -1) {
        nLast = i;
        i = fn_800EFBD0(i + 1);
    }
    return nLast;
}

// Skips ahead to tournament nEvent: a tournament under way is abandoned, the ones before are
// played out.
void fn_800EF9D0(s32 nEvent) {
    PlayerNumber_t nPlayer = PLR_1_e;
    if (nEvent != gpSaveData[nPlayer].tour.nEvent && gpSaveData[nPlayer].tour.nRound > 0) {
        fn_80117C50(0, 0);
    }
    while (gpSaveData[nPlayer].tour.nEvent < nEvent) {
        fn_800EF130(0, 0);
        fn_800EEB94(0);
    }
}

// Tournament i (0..30), or none.
Tournament* fn_800EFA70(s32 i) {
    if (i != -1 && i < 31) {
        return &gPgaData.aTournament[i];
    }
    return 0;
}

// TW06: GameModeDriverPGATour::GetRounds. Tournament i's number of rounds (from its format; 1 without one).
s32 fn_800EFA9C(s32 i) {
    if (gPgaData.aTournament[i].nTourEvent) {
        return gPgaData.aTourEvent[gPgaData.aTournament[i].nTourEvent - 1].nRounds;
    }
    return 1;
}

// The next season: 0 after the tenth, else 1 and the season starts at its first tournament.
s32 fn_800EFAD0(void) {
    PlayerNumber_t nPlayer = PLR_1_e;
    gpSaveData[nPlayer].tour.nSeason++;
    if (gpSaveData[nPlayer].tour.nSeason >= 10) {
        gpSaveData[nPlayer].tour.nSeason = 10;
        gpSaveData[nPlayer].tour.nEvent = 0;
        return 0;
    }
    gpSaveData[nPlayer].tour.nEvent = fn_800EFBD0(0);
    fn_80117860(&gpSaveData[nPlayer].tour);
    return 1;
}

// TW06: GameModeDriverPGATour::GetCurrentSeason. Profile 0's season, 0 = 2004.
s32 fn_800EFB88(void) {
    PlayerNumber_t nPlayer = PLR_1_e;
    return gpSaveData[nPlayer].tour.nSeason;
}

s32 fn_800EFBAC(void) {
    return fn_800EFB88() + 2004;
}

// TW06: GameModeDriverPGATour::GetEventOnOrAfter. The first tournament from i on that is held this
// season, or -1.
s32 fn_800EFBD0(s32 i) {
    PlayerNumber_t nPlayer = PLR_1_e;
    s32 nEvent;
    u8 bFound = 0;
    while (!bFound) {
        Tournament* p = fn_800EFA70(i);
        if (p != NULL) {
            if (p->aStartDate[gpSaveData[nPlayer].tour.nSeason] != 0) {
                nEvent = i;
                bFound = 1;
            }
        } else {
            nEvent = -1;
            bFound = 1;
        }
        i++;
    }
    return nEvent;
}

// The tournament being played on a date.
Tournament* fn_800EFC80(u16 nDate) {
    s32 nId;
    s32 nRound;
    if (fn_800EF83C(nDate, &nId, &nRound)) {
        return fn_800EFA70(nId);
    }
    return 0;
}

// Tournament i's first prize in bracket k. TW06: GameModeDriverPGATour::ComputeFirstPrizeForBracket
// (by shape).
s32 fn_800EFCC0(s32 i, s32 k) {
    Tournament* p = fn_800EFA70(i);
    return p->aPrize[k][0] * 1000;
}

// Tournament i's purse in bracket k. TW06: GameModeDriverPGATour::ComputePurseForBracket (by shape).
s32 fn_800EFCFC(s32 i, s32 k) {
    Tournament* p = fn_800EFA70(i);
    return p->aPrize[k][1] * 1000;
}

u16 fn_800EFD38(s32 i) {
    Tournament* p = fn_800EFA70(i);
    if (p == NULL) {
        return 0xFFFF;
    }
    return p->aStartDate[fn_800EFB88()];
}

// TW06: GameModeDriverPGATour::GetEndDate. The last day of tournament i this season.
u16 fn_800EFD84(s32 i) {
    u16 nDate;
    Tournament* p = fn_800EFA70(i);
    if (p == NULL) {
        return 0xFFFF;
    }
    nDate = p->aStartDate[fn_800EFB88()];
    fn_800D27CC(&nDate, fn_800EFA9C(i) - 1);
    return nDate;
}

// TW06: GameModeDriverPGATour::GetName.
char* fn_800EFDFC(s32 i) {
    return gPgaData.pNames + gPgaData.aTournament[i].nName;
}

// TW06: GameModeDriverPGATour::GetCurrentEventID. Profile 0's current tournament.
s32 fn_800EFE18(void) {
    PlayerNumber_t nPlayer = PLR_1_e;
    return gpSaveData[nPlayer].tour.nEvent;
}

s32 fn_800EFE3C(s32 i) {
    return fn_800EFA70(i)->n10;
}

// TW06: GameModeDriverPGATour::GetInitialChampName.
char* fn_800EFE60(s32 i) {
    return gPgaData.aTournament[i].szChampName;
}

// TW06: GameModeDriverPGATour::GetInitialChampScore.
s32 fn_800EFE78(s32 i) {
    return gPgaData.aTournament[i].nChampScore;
}

// TW06: GameModeDriverPGATour::GetCourses. The course of each round of a tournament; returns the
// number of rounds.
s32 fn_800EFE90(Tournament* p, s32* pCourses) {
    TourEvent* pEvent = &gPgaData.aTourEvent[p->nTourEvent - 1];
    s32 nRounds;
    s32 i;
    TourRound* pRound;
    if (pEvent != NULL) {
        nRounds = pEvent->nRounds;
        pRound = pEvent->aRound;
        for (i = 0; i < nRounds; i++) {
            pCourses[i] = pRound[i].nCourse;
        }
        return nRounds;
    }
    return 0;
}

// TW06: GameModeDriverPGATour::GetWinnerEarningsString. Tournament i's first prize as text: in the
// player's bracket when it was played, else in the current one.
void fn_800EFF7C(s32 i, char* pDst) {
    PlayerNumber_t nPlayer = PLR_1_e;
    SeasonEvent* p = &gpSaveData[nPlayer].tour.aEvent[i];
    s32 nBracket;
    if (i < fn_800EFE18()) {
        nBracket = p->nUserBracket;
    } else {
        nBracket = fn_800EF0E0(nPlayer);
    }
    fn_800907AC(fn_800EFCC0(i, nBracket), pDst);
}

// TW06: GameModeDriverPGATour::GetCurrentEventLeader. The leader's name, or "Tied (%d players)".
void fn_800F0010(char* pDst) {
    s32 n = fn_80118684(0);
    if (n > 1) {
        sprintf(pDst, "Tied (%d players)", n);
    } else {
        s32 nLeader = fn_801197CC(0, 0);
        s32 nGolfer = fn_80119118(0, nLeader);
        strcpy(pDst, fn_80118E30(0, nGolfer));
    }
}

void fn_800F009C(void) {
    s32 nLeader = fn_801197CC(0, 0);
    fn_8011937C(0, nLeader, fn_8011908C(0, nLeader) == 0);
}

// TW06: GameModeDriverPGATour::GetPurseString. The same for the purse.
void fn_800F00F8(s32 i, char* pDst) {
    PlayerNumber_t nPlayer = PLR_1_e;
    SeasonEvent* p = &gpSaveData[nPlayer].tour.aEvent[i];
    s32 nBracket;
    if (i < fn_800EFE18()) {
        nBracket = p->nUserBracket;
    } else {
        nBracket = fn_800EF0E0(nPlayer);
    }
    fn_800907AC(fn_800EFCFC(i, nBracket), pDst);
}

void fn_800F018C(void) {
    fn_8011937C(0, 0, fn_8011908C(0, 0) == 0);
}

// TW06: GameModeDriverPGATour::GetUserFinishString. A tournament's result for the season screen:
// "Did Not Play", "Cut", or the place.
void fn_800F01CC(s32 i, char* pDst) {
    switch (gpSaveData->tour.aEvent[i].nUserRankType) {
    case 0:
        strcpy(pDst, "Did Not Play");
        return;
    case 1:
        strcpy(pDst, "Cut");
        return;
    case 2:
        sprintf(pDst, "%d", gpSaveData->tour.aEvent[i].nUserRank);
        return;
    }
}

// TW06: GameModeDriverPGATour::GetChamp.
void fn_800F0258(s32 i, char* pDst) {
    strcpy(pDst, gpSaveData->tour.aEvent[i].szChampName);
}

// TW06: GameModeDriverPGATour::GetChampScore.
s32 fn_800F0290(s32 i) {
    return gpSaveData->tour.aEvent[i].nChampScore;
}

// How many tournaments profile 0 has won.
s32 fn_800F02A8(void) {
    s32 n = 0;
    s32 i;
    for (i = 0; i < 31; i++) {
        if (gpSaveData->aC8[i].award.bWon == 1) {
            n++;
        }
    }
    return n;
}

s32 fn_800F02D4(s32 i) {
    return gPgaData.aTriple[i].n0;
}

s32 fn_800F02EC(s32 i) {
    return gPgaData.aTriple[i].n4;
}

s32 fn_800F0304(s32 i) {
    return gPgaData.aTriple[i].n8;
}

// The message after a round, if there is one: after the second round whether the player made the
// cut, and in a playoff the score to beat.
s32 fn_800F031C(char* pDst) {
    PlayerNumber_t nPlayer = PLR_1_e;
    if (gpSaveData[nPlayer].tour.nRound == 1 && fn_80119A04(0, 0) == 18) {
        if (fn_801197A4(0, 0)) {
            strcpy(pDst, "TOURNAMENT CUT\n\nYou did not place in the top 70 after two\n"
                         "rounds. You have been cut from the tournament.");
            return 1;
        }
        strcpy(pDst, "TOURNAMENT CUT\n\nCongratulations! You placed in the top 70\n"
                     "after two rounds. You made the cut!");
        return 1;
    }
    if (gpGame->bD4 && fn_8011A6F4(0, 0) && fn_8011A684(0) > 1) {
        sprintf(pDst, "TOURNAMENT PLAYOFF\n\nYou're tied for first place. You must beat\n"
                      "your opponent's score of %d on the playoff\nhole to win.",
                fn_8011A7C8(0, lbl_80282340));
        return 1;
    }
    return 0;
}

s32 fn_800F0428(s32 nPlayer) {
    return gpSaveData[nPlayer].tour.nEvent;
}
