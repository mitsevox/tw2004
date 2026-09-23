// FE_Manager.c (EA's name, from its asserts; also in EA's 2002 source tree): the front end's
// manager, which runs the menu screens: the movies played from the menus (the intro, the credits,
// the golfers' bios), the profile being worked on, the created golfer and its Create-A-Player
// picks and unlocks.

#include "engine.h"
#include "ustream.h"
#include "game.h"
#include "charstate.h"
#include "core/easb.h"
#include "frontend/fe.h"

// Outside this file.
// The movie player (LLVideo.c): plays a movie file until it ends or pfnSkip returns nonzero.
void fn_80075FB8(char* pPath, int (*pfnSkip)(void), int a, int b);
void fn_80037FB4(u8 a, f32* pColor);    // a full-screen colour (GoPostFx.c)
void fn_80010284(void);
void fn_8008B704(void);
void fn_8008B754(int a);
void fn_8008B7D0(int a);
void fn_8008D8CC(void);
void fn_8008D8F4(void);
void fn_8008DBE8(void);
void fn_80092198(void);
void fn_8009220C(void);
void fn_800A7644(int a);
void fn_800A4FD8(void);
void fn_80102AC4(void);
void fn_80103B74(int a);
void fn_801073DC(int nPart);            // FE_CrAPDB.c
void fn_80103B8C(s8 b);                // } FE_CrAPDB.c: set b; an asset's b (2: either),
s8   fn_80103BC0(int nAsset);           // } its kind, fn_80107444's count, and its part and
s16  fn_8010742C(int nAsset);           // } choice
int  fn_80107444(int nAsset);           // }
void fn_80105FF8(int nAsset, s16* pKind, s32* pPart, s32* pChoice);    // }
u8   fn_80056480(int a);
u8   fn_800564AC(int n);
s32  fn_801258E8(void);                 // EASportsBio.c
void fn_8009170C(void);

// This file, in address order.
void fn_80076E48(void);
void fn_80076EEC(void);                 // frees lbl_80281EC8
void fn_80076F20(void);
void fn_80076F24(void);
void fn_80076F54(void);
void fn_80076F58(void);
void fn_80076F80(UStreamObject* pObject);
int  fn_80076FDC(void);
void fn_8007706C(char* pName, char* pDir, char* pPath);
void fn_800770D4(char* pName, char* pPath);
u8   fn_80077148(void);
void FE_GetBIOMovieName(void);
void fn_800772E0(void);
void fn_8007731C(void);
void fn_80077340(void);
void fn_80077344(void);
void fn_80077348(void);
void fn_8007734C(void);
void fn_8007739C(Replay* pReplay);
void fn_800773F8(void);
void fn_80077428(void);
void fn_8007744C(void);
void Golfer_LoadCreatedFromSave(void);
void fn_800779BC(int a, int b);
int  fn_80077BDC(int n);
void fn_80077C1C(int a, int b);
u8   fn_80078008(s32 nAsset, SaveProfile* pProfile);
int  fn_80078604(int a, int b, int c);
void fn_80078620(int n, int* pA, int* pB, int* pC);
void fn_80078680(SaveProfile* pProfile);
void fn_8007873C(SaveProfile* pProfile);
u8   FE_CrAP_IsAssetUndesirable(s16 nPart, CrAPAsset* pAsset);
void fn_80078A2C(s16 nPart, int nChance);
u8   fn_80078B84(CrAPAsset* pAsset);
u8   FE_CrAP_IsCrazyHat(CrAPAsset* pAsset);
u8   fn_80078D24(CrAPAsset* pAsset);
void fn_80078E34(SaveProfile* pProfile);
void fn_80079664(SaveProfile* pProfile);
void fn_8007975C(SaveProfile* pProfile, s16 nPart, int nChance);
int  fn_800797E0(SaveProfile* pProfile, s16 nPart, int b, int nChance);
void fn_80079974(void);
void fn_80079D30(void);
void fn_80079DAC(void);
u8   fn_80079E44(int nAttr);            // a hidden attribute: ATTR_AGGRESSION, ATTR_IQ, ATTR_SPEED

// This file's globals (fe.h), each section in reverse address order as the compiler lays it out.
FEState lbl_801D7148;
FEProfile* lbl_80281ED4;
u32 lbl_80281ED0;
u32 lbl_80281ECC;
u8* lbl_80281EC8;

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f (0x80283AC0), before the 0.0f and 0.05f FE_GetBIOMovieName uses first; its body is unknown.
static f32 FE_Manager_StrippedFn(f32 x) {
    return x + 1.0f;
}

// Set the front end's state up: no profiles loaded, no CPU players, no backups, no movies.
void fn_80076E48(void) {
    int i;
    for (i = 0; i < 5; i++) {
        lbl_801D7148.aLoaded[i] = 0;
        lbl_801D7148.aCPU[i] = 0;
        lbl_801D7148.aBackup[i] = -1;
    }
    lbl_801D7148.b0F = 1;
    lbl_801D7148.b10 = 1;
    lbl_801D7148.nMode = -1;
    lbl_801D7148.b11 = 0;
    lbl_801D7148.b18 = 1;
    lbl_801D7148.n1C = 0;
    lbl_801D7148.nMovieNext = 0;
    lbl_801D7148.nMovieFree = 0;
    lbl_801D7148.p658 = NULL;
    fn_8009170C();
    lbl_801D8858.n30 = 0;
}

// Free the copy of the 'BIO ' stream object's data.
void fn_80076EEC(void) {
    if (lbl_80281EC8 != NULL) {
        fn_80009E70(lbl_80281EC8);
        lbl_80281EC8 = NULL;
    }
}

void fn_80076F20(void) {
}

void fn_80076F24(void) {
    UStream_RegisterHandler(TAG('B', 'I', 'O', ' '), fn_80076F80);
}

void fn_80076F54(void) {
}

void fn_80076F58(void) {
    UStream_UnregisterHandler(TAG('B', 'I', 'O', ' '));
}

// The 'BIO ' stream object's handler: keep a copy of its data.
void fn_80076F80(UStreamObject* pObject) {
    lbl_80281EC8 = fn_80009B34(pObject->uSize, 2, 32, "FE_Manager.c", 285);
    Mem_cpy(lbl_80281EC8, pObject->pData, pObject->uSize);
    fn_80009E70(pObject);
}

// A movie's skip test: any button on any controller.
int fn_80076FDC(void) {
    int i;
    fn_80013400();
    for (i = 0; i < 4; i++) {
        if (fn_800136DC(i)) {
            return 1;
        }
    }
    return 0;
}

// A movie's path on the disc: "data/movies/<name>.NGC".
void FE_MakeMoviePath(char* pName, char* pPath) {
    sprintf(pPath, "data/movies/%s.%s", pName, "NGC");
}

// The same in a subfolder: "data/movies/<dir>/<name>.NGC".
void fn_8007706C(char* pName, char* pDir, char* pPath) {
    sprintf(pPath, "data/movies/%s/%s.%s", pDir, pName, "NGC");
}

void FE_MakeCameoMoviePath(char* pName, char* pPath) {
    fn_8007706C(pName, "cameos", pPath);
}

void fn_800770D4(char* pName, char* pPath) {
    fn_8007706C(pName, "bios", pPath);
}

// ---- the movie queue: the menus queue a movie, the screen fades to black and it plays ----------

// Add a movie to the queue; the caller fills it in.
FEMovie* fn_800770FC(void) {
    FEMovie* pMovie = &lbl_801D7148.aMovies[lbl_801D7148.nMovieFree++];
    if (lbl_801D7148.nMovieFree % FE_NUM_MOVIES == 0) {
        lbl_801D7148.nMovieFree = 0;
    }
    return pMovie;
}

// The queue is empty.
u8 fn_80077148(void) {
    return lbl_801D7148.nMovieFree == lbl_801D7148.nMovieNext;
}

// Once a frame: while a movie is queued, fade the screen to black; once it is black, play the
// movie and take it off the queue.
void FE_GetBIOMovieName(void) {
    char szPath[256];
    char szName[32];
    f32 vColor[4];
    FEMovie* pMovie;
    if (!fn_80077148()) {
        vColor[0] = 0.0f;
        vColor[1] = 0.0f;
        vColor[2] = 0.0f;
        vColor[3] = lbl_801D87C0.fFade;
        fn_80037FB4(1, vColor);
        lbl_801D87C0.fFade += 0.05f;
        pMovie = &lbl_801D7148.aMovies[lbl_801D7148.nMovieNext];
        if (lbl_801D87C0.fFade >= 1.0f) {
            fn_800A75B4();
            fn_800772E0();
            switch (pMovie->nKind) {
            case FE_MOVIE_CREDITS:
                FE_MakeMoviePath("credits", szPath);
                fn_80075FB8(szPath, fn_80076FDC, 0, 0);
                break;
            case FE_MOVIE_BIO:
                sprintf(szName, "bio%02d", pMovie->nBio + 1);
                fn_800770D4(szName, szPath);
                fn_80075FB8(szPath, fn_80076FDC, 0, 0);
                break;
            case 4:                     // fake match: the original never compares with 4; this empty
                break;                  // case only makes the dispatch test 3 before 1
            }
            fn_8007731C();
            fn_800A7644(0);
            lbl_801D87C0.fFade = 0.0f;
            lbl_801D7148.nMovieNext++;
            if (lbl_801D7148.nMovieNext % FE_NUM_MOVIES == 0) {
                lbl_801D7148.nMovieNext = 0;
            }
        }
    }
}

// Before a movie.
void fn_800772E0(void) {
    fn_8008B754(1);
    fn_8008B704();
    fn_8008B7D0(1);
    fn_8008DAEC();
    fn_8008DBE8();
    fn_80092198();
}

// After a movie.
void fn_8007731C(void) {
    fn_8009220C();
    fn_8008D8F4();
}

void fn_80077340(void) {
}

void fn_80077344(void) {
}

void fn_80077348(void) {
}

// The intro movie, unless the session says to skip it.
void fn_8007734C(void) {
    char szPath[64];
    if (!(gSession.uFlags & 0x4000)) {
        FE_MakeMoviePath("intro", szPath);
        fn_80075FB8(szPath, fn_80076FDC, 0, 0);
    }
}

// Play a saved shot: game mode 10 (the replay) with its golfer on its course.
void fn_8007739C(Replay* pReplay) {
    Mem_cpy(&gReplayData, pReplay, sizeof(Replay));
    fn_800E0B38(10);
    Session_SetGolfer(gReplayData.player.golfer.nIndex, 0);
    fn_800E14E0(gReplayData.nCourse);
}

void fn_800773F8(void) {
    fn_80010284();
    fn_80079EA8();
    fn_8007744C();
    fn_8008D8CC();
    fn_80079DAC();
}

void fn_80077428(void) {
    Golfer_LoadCreatedFromSave();
    fn_80079D30();
}

// Set up the profile being worked on, cleared, and the logo textures' hashes.
void fn_8007744C(void) {
    int i;
    lbl_80281ED4 = fn_80009B34(sizeof(FEProfile), 2, 16, "FE_Manager.c", 1035);
    fn_80005AE8(lbl_80281ED4, 0, sizeof(FEProfile));
    lbl_80281ED4->bCopy = 0;
    lbl_80281ED4->b10640 = 0;
    lbl_80281ED4->nSlot = 0;
    lbl_80281ED4->n1 = -1;
    for (i = 0; i < FE_NUM_801D8890; i++) {
        lbl_801D8890[i].b0 = 0;
        lbl_801D8890[i].n4 = 0;
        lbl_801D8890[i].b1 = 0;
    }
    lbl_801D8858.b18 = 0;
    lbl_801D8858.n30 = 0;
    lbl_80281ED4->uSquareHash = fn_8000BEE4("__LogoSquare");
    lbl_80281ED4->uRectHash = fn_8000BEE4("__LogoRect");
    lbl_80281ED4->b11702 = 0;
    lbl_80281ED4->b11703 = 0;
    lbl_80281ED4->n11704 = 0;
}

// Every player on a created golfer gets its slot's saved record in the golfer table, but keeps
// the table's hidden attributes (fn_80079E44). Then the front end's data is freed.
void Golfer_LoadCreatedFromSave(void) {
    int i;
    int j;
    int nGolfer;
    s8 aAttr[NUM_ATTRS];
    s8 aAttrAlt[NUM_ATTRS];
    for (i = 0; i < gSession.nNumPlayers; i++) {
        nGolfer = gSession.nGolfer[i];
        if (nGolfer >= FIRST_CREATED_GOLFER) {
            gSession.nGolfer[i] = nGolfer;      // the original stores it back unchanged
            for (j = 0; j < NUM_ATTRS; j++) {
                aAttr[j] = gGolferTable[nGolfer].attr[j];
                aAttrAlt[j] = gGolferTable[nGolfer].attrAlt[j];
            }
            Mem_cpy(&gGolferTable[gSession.nGolfer[i]], &gpSaveData[i].createdGolfer, sizeof(GolferRecord));
            for (j = 0; j < NUM_ATTRS; j++) {
                if (fn_80079E44(j)) {
                    gGolferTable[gSession.nGolfer[i]].attr[j] = aAttr[j];
                    gGolferTable[gSession.nGolfer[i]].attrAlt[j] = aAttrAlt[j];
                }
            }
        }
    }
    fn_80076EEC();
    fn_80076F20();
    fn_80009E70(lbl_80281ED4);
}

// Back up every slot's profile, where one is loaded.
void fn_80077780(void) {
    int i;
    for (i = 0; i < 4; i++) {
        if (gpSaveData[i].bActive) {
            Mem_cpy(&lbl_801D7148.p658[i], &gpSaveData[i], sizeof(SaveProfile));
        }
    }
}

// Back up one slot's profile. A slot without a backup row takes its own row, first swapping in
// the backup that sits in the first free row.
void fn_80077808(int nSlot) {
    int nFree = -1;
    int i;
    if (lbl_801D7148.aBackup[nSlot] == -1) {
        for (i = 0; i < 4; i++) {
            if (!lbl_801D7148.p658[i].bActive) {
                nFree = i;
                break;
            }
        }
        if (nFree == -1) {
            Mem_cpy(&lbl_801D7148.p658[nSlot], &gpSaveData[nSlot], sizeof(SaveProfile));
            lbl_801D7148.aBackup[nSlot] = nSlot;
        } else if (nSlot == nFree) {
            Mem_cpy(&lbl_801D7148.p658[nSlot], &gpSaveData[nSlot], sizeof(SaveProfile));
            lbl_801D7148.aBackup[nSlot] = nSlot;
        } else {
            fn_800779BC(nFree, nSlot);
            Mem_cpy(&lbl_801D7148.p658[nSlot], &gpSaveData[nSlot], sizeof(SaveProfile));
            lbl_801D7148.aBackup[nSlot] = nSlot;
        }
    } else {
        Mem_cpy(&lbl_801D7148.p658[lbl_801D7148.aBackup[nSlot]], &gpSaveData[nSlot], sizeof(SaveProfile));
    }
}

// Back up one slot's profile into its backup row.
void fn_80077968(int nSlot) {
    Mem_cpy(&lbl_801D7148.p658[lbl_801D7148.aBackup[nSlot]], &gpSaveData[nSlot], sizeof(SaveProfile));
}

// Swap two backup rows.
void fn_800779BC(int a, int b) {
    SaveProfile* pTemp = fn_80009B34(sizeof(SaveProfile), 1, 32, "FE_Manager.c", 1194);
    Mem_cpy(pTemp, &lbl_801D7148.p658[b], sizeof(SaveProfile));
    Mem_cpy(&lbl_801D7148.p658[b], &lbl_801D7148.p658[a], sizeof(SaveProfile));
    Mem_cpy(&lbl_801D7148.p658[a], pTemp, sizeof(SaveProfile));
    fn_80009E70(pTemp);
}

// A golfer's record: a table golfer, or the profile's created golfer.
GolferRecord* fn_80077A80(int nGolfer) {
    SaveProfile* pProfile = fn_80077ACC();
    if (nGolfer < FIRST_CREATED_GOLFER) {
        return &gGolferTable[nGolfer];
    }
    return &pProfile->createdGolfer;
}

// ---- the profile being worked on ---------------------------------------------------------------

SaveProfile* fn_80077ACC(void) {
    if (lbl_80281ED4->bCopy) {
        return &lbl_80281ED4->profile;
    }
    return &gpSaveData[lbl_80281ED4->nSlot];
}

int fn_80077B08(void) {
    return lbl_80281ED4->nSlot;
}

// A yes/no list over 0..28 (Golfer.c asks it); what it marks is not known yet.
u8 fn_80077B18(int n) {
    if (n == 0 || n == 1 || n == 3 || n == 4 || n == 5 || n == 7 || n == 10 || n == 11 || n == 13 ||
        n == 14 || n == 18 || n == 21 || n == 22 || n == 23 || n == 24 || n == 28) {
        return 1;
    }
    return 0;
}

void fn_80077B78(void) {
    fn_80077C1C(-1, 0);
    fn_80077C1C(-1, 1);
    fn_80077C1C(-2, 0);
    fn_80077C1C(-2, 1);
    fn_80077C1C(-3, 0);
    fn_80077C1C(-3, 1);
}

// -1, -2, -3 to 0, 1, 2; anything else to 0.
int fn_80077BDC(int n) {
    switch (n) {
    case -1:
        return 0;
    case -2:
        return 1;
    case -3:
        return 2;
    default:
        return 0;
    }
}

// For b and category a (-1, -2 or -3), seeded by today's date: one of the category's asset kinds
// at random, then up to five different random assets of that kind that fit b (fn_80103BC0 gives
// b or 2) and pass fn_80078008. Then the random stream is seeded from the clock again.
void fn_80077C1C(int a, int b) {
    int aFound[3000];
    int aKinds[88];         // fake match: 4 are used; 88 gives the original's stack frame
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    s32 nHour;
    s32 nMinute;
    s32 nSecond;
    s32 nMsec;
    s32 nPart;
    s32 nChoice;
    s16 nKind;
    int j;
    SaveProfile* pProfile;
    int nCount;
    int nCategory;
    int nSeed;
    int i;
    int nFound;
    int nKinds;
    s8 nB;                  // b as fn_80103B8C and fn_80103BC0 take it
    pProfile = fn_80077ACC();
    nFound = 0;
    nCount = fn_80105C00();
    nKind = 0;
    nPart = 0;
    nChoice = 0;
    nCategory = fn_80077BDC(a);
    switch (a) {
    case -1:
        aKinds[0] = 0;
        aKinds[1] = 1;
        aKinds[2] = 2;
        aKinds[3] = 7;
        nKinds = 4;
        break;
    case -2:
        aKinds[0] = 8;
        aKinds[1] = 19;
        aKinds[2] = 20;
        nKinds = 3;
        break;
    case -3:
        aKinds[0] = 12;
        nKinds = 1;
        break;
    default:
        return;
    }
    fn_8011E020(&nMonth, &nDay, &nYear, &nHour, &nMinute, &nSecond, &nMsec);
    nSeed = fn_80078604(nMonth, nDay, nYear);
    for (j = 0; j < 5; j++) {
        lbl_80281ED4->aPart[b][nCategory][j] = -1;
        lbl_80281ED4->aChoice[b][nCategory][j] = -1;
    }
    lbl_80281ED4->nDateSeed = nSeed;
    if (lbl_80281ED4->nDateSeed == 0) {
        lbl_80281ED4->nDateSeed = 3081979;          // 8/3/1979, packed as fn_80078604 does
    }
    fn_8000B1D4(0, lbl_80281ED4->nDateSeed);
    lbl_80281ED4->aKind[b][nCategory] = aKinds[Rand_Next(0) % nKinds];
    nB = b;
    for (i = 0; i < nCount; i++) {
        fn_80103B8C(fn_80103BC0(i));
        nKind = fn_8010742C(i);
        if (nKind == lbl_80281ED4->aKind[b][nCategory] &&
            (fn_80103BC0(i) == nB || fn_80103BC0(i) == 2) &&
            !fn_80078008(i, pProfile) && fn_80107444(i) > 0) {
            aFound[nFound] = i;
            nFound++;
        }
    }
    fn_80103B8C(nB);
    for (j = 0; j < 5; j++) {
        if (j >= nFound) break;
    retry:
        fn_80105FF8(aFound[Rand_Next(0) % nFound], &nKind, &nPart, &nChoice);
        lbl_80281ED4->aPart[b][nCategory][j] = nPart;
        lbl_80281ED4->aChoice[b][nCategory][j] = nChoice;
        for (i = 0; i < j; i++) {
            if (lbl_80281ED4->aPart[b][nCategory][j] == lbl_80281ED4->aPart[b][nCategory][i] &&
                lbl_80281ED4->aChoice[b][nCategory][j] == lbl_80281ED4->aChoice[b][nCategory][i]) {
                goto retry;                         // fake match: a do-while scores 98.3
            }
        }
    }
    gSession.nSeed = fn_8000B244();
    fn_8000B1D4(0, gSession.nSeed);
}

// Whether a Create-A-Player asset is still locked for the profile (never in the session's 0x4000
// mode, nor while fn_80056480(0) holds). The asset names a lock kind (fn_801055DC) and a number
// for it (fn_80105610): a bit, an award, a tournament won, a count of them to reach, a season...
u8 fn_80078008(s32 nAsset, SaveProfile* pProfile) {
    int aBits[5] = {1, 2, 3, 4, 5};
    int nCount = 0;
    s8 nKind;
    s16 n;
    u8 bLocked;
    int i;
    if (gSession.uFlags & 0x4000) {
        return 0;
    }
    nKind = fn_801055DC(nAsset);
    n = fn_80105610(nAsset);
    if (fn_80056480(0)) {
        return 0;
    }
    switch (nKind) {
    case 0:
        bLocked = fn_8001E9CC(pProfile->aB1CC, n) == 0;
        break;
    case 2:
        bLocked = !fn_80058304(pProfile, 1);
        break;
    case 3:
        bLocked = 1;
        break;
    case 4:
        bLocked = 0;
        break;
    case 6:
        bLocked = fn_8001E9CC(lbl_801D5948, aBits[n]) == 0;
        break;
    case 7:
        bLocked = !pProfile->aC8[n].award.bWon;
        break;
    case 8:
        bLocked = 1;
        for (i = 0; i < 31; i++) {
            if (pProfile->aC8[i].award.bWon) {
                nCount++;
            }
        }
        if (nCount >= n) {
            bLocked = 0;
        }
        break;
    case 9:
        bLocked = pProfile->tour.nSeason < n;
        break;
    case 10:
        bLocked = 1;
        if (fn_800564AC(n)) {
            bLocked = 0;
        }
        for (i = 0; i < 11; i++) {
            if (pProfile->a1054C[i].b && pProfile->a1054C[i].n == n) {
                bLocked = 0;
            }
        }
        break;
    case 11:
        bLocked = 1;
        for (i = 0; i < 11; i++) {
            if (pProfile->a1054C[i].b) {
                nCount++;
            }
        }
        if (nCount >= n) {
            bLocked = 0;
        }
        break;
    case 12:
        if (EASBio_IsBioLoaded() && n <= fn_801258E8()) {
            bLocked = 0;
        } else {
            bLocked = 1;
        }
        break;
    case 13:
        bLocked = 1;
        break;
    case 14:
        bLocked = !pProfile->aLadderAward[n].bWon;
        break;
    case 15:
        bLocked = 1;
        for (i = 0; i < 25; i++) {
            if (pProfile->aLadderAward[i].bWon) {
                nCount++;
            }
        }
        if (nCount >= n) {
            bLocked = 0;
        }
        break;
    case 16:
        bLocked = n > GM_GetGameProgress(pProfile);
        break;
    case 17:
        bLocked = !pProfile->aRTEAward[n].bWon;
        break;
    case 18:
        bLocked = 1;
        for (i = 0; i < 75; i++) {
            if (pProfile->aRTEAward[i].bWon) {
                nCount++;
            }
        }
        if (nCount >= n) {
            bLocked = 0;
        }
        break;
    case 19:
        bLocked = pProfile->aMedal[n] != 0;
        break;
    case 20:
        bLocked = 1;
        if (pProfile->nTourCardLevel >= 1) {
            nCount = 1;
        }
        for (i = 0; i < 29; i++) {
            if (pProfile->aMedal[i]) {
                nCount++;
            }
        }
        if (nCount >= n) {
            bLocked = 0;
        }
        break;
    case 21:
        bLocked = !pProfile->aAward[n].bWon;
        break;
    case 22:
        bLocked = 1;
        for (i = 0; i < 23; i++) {
            if (pProfile->aAward[i].bWon) {
                nCount++;
            }
        }
        if (nCount >= n) {
            bLocked = 0;
        }
        break;
    case 23:
        n += 23;
        bLocked = !pProfile->aAward[n].bWon;
        break;
    case 24:
        bLocked = 1;
        for (i = 23; i < 16; i++) {         // EA bug: never runs (awards 23..38 were meant?)
            if (pProfile->aAward[i].bWon) {
                nCount++;
            }
        }
        if (nCount >= n) {
            bLocked = 0;
        }
        break;
    case 25:
        bLocked = !pProfile->a1C0[n + 12].b;
        break;
    case 26:
        bLocked = 1;
        for (i = 12; i < 16; i++) {
            if (pProfile->a1C0[i].b) {
                nCount++;
            }
        }
        if (nCount >= n) {
            bLocked = 0;
        }
        break;
    case 27:
        bLocked = pProfile->nTourCardLevel < n;
        break;
    case -1:
        bLocked = 0;
        break;
    case 28:
        bLocked = 0;
        break;
    default:
        bLocked = 0;
        break;
    }
    return bLocked;
}

// Pack three numbers into one, b * 1000000 + a * 10000 + c; fn_80078620 unpacks it.
int fn_80078604(int a, int b, int c) {
    int n = c;
    n += a * 10000;
    n += b * 1000000;
    return n;
}

void fn_80078620(int n, int* pA, int* pB, int* pC) {
    *pB = n / 1000000;
    n -= *pB * 1000000;
    *pA = n / 10000;
    n -= *pA * 10000;
    *pC = n;
}

// Note which Create-A-Player assets are locked for the profile (fn_80078008), one bit each.
void fn_80078680(SaveProfile* pProfile) {
    int i;
    int nCount;
    s8 nSaved;
    if (fn_80105C30()) {
        nSaved = fn_80103BB4();
        nCount = fn_80105C00();
        for (i = 0; i < nCount; i++) {
            fn_80103B8C(fn_80103BC0(i));
            if (fn_80078008(i, pProfile)) {
                fn_8001EA34(pProfile->aAssetLocked, i);
            } else {
                fn_8001EB6C(pProfile->aAssetLocked, i);
            }
        }
        fn_80103B8C(nSaved);
    }
}

// ---- the created golfer's parts -----------------------------------------------------------------

// The created golfer's equipment tiers, from the equipment it wears: each of the 53 equipment
// slots can raise up to two attributes' tiers.
void fn_8007873C(SaveProfile* pProfile) {
    s32 i;
    int nAsset;
    s16 nSlot;
    int nAttrA;
    int nAttrB;
    int nTierA;
    int nTierB;
    if (fn_80105C30()) {
        for (i = 0; i < NUM_ATTRS; i++) {
            pProfile->createdGolfer.tier[i] = 0;
        }
        for (nSlot = 0; nSlot < 53; nSlot++) {
            nAsset = fn_80103D14(nSlot);
            if (nAsset >= 0) {
                nAttrA = fn_80105494(nAsset);
                nAttrB = fn_80105504(nAsset);
                nTierA = fn_801054CC(nAsset);
                nTierB = fn_8010553C(nAsset);
                if (nAttrA >= 0 && pProfile->createdGolfer.tier[nAttrA] < nTierA) {
                    pProfile->createdGolfer.tier[nAttrA] = nTierA;
                }
                if (nAttrB >= 0 && pProfile->createdGolfer.tier[nAttrB] < nTierB) {
                    pProfile->createdGolfer.tier[nAttrB] = nTierB;
                }
            }
        }
    }
}

// Only part 10 (the face) has undesirable choices: scars, tattoos, acne, old age and make-up.
u8 FE_CrAP_IsAssetUndesirable(s16 nPart, CrAPAsset* pAsset) {
    switch (nPart) {
    case 10:
        if (stricmp(pAsset->szName, "Cheek Scar & Tat") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Facial Tattoo") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Acne") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Weathered") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Old") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Gold Makeup") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Beauty 4") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Punk") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Punk Makeup") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Alt Punk") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Alt Punk Makeup") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Pink Makeup") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Old Makeup") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Old") == 0) {      // EA's list tests "Old" twice
            return 1;
        }
        break;
    }
    return 0;
}

// Put a random choice on part nPart: an undesirable one (see FE_CrAP_IsAssetUndesirable) with a
// chance of 100 - nChance percent, else a desirable one; only choices fn_80104020 allows.
void fn_80078A2C(s16 nPart, int nChance) {
    int aChoices[250];
    u32 bDesirable = (int)(Rand_Next(0) % 100) < nChance;
    int nFound = 0;
    int nCount = fn_801048EC(nPart, 0);
    int i;
    int nAsset;
    CrAPAsset* pAsset;
    for (i = 0; i < nCount; i++) {
        nAsset = fn_80104FA8(nPart, 0, i);
        pAsset = fn_80104F68(nAsset);
        if (bDesirable && !FE_CrAP_IsAssetUndesirable(nPart, pAsset) && fn_80104020(nAsset)) {
            aChoices[nFound++] = i;
        } else if (!bDesirable && FE_CrAP_IsAssetUndesirable(nPart, pAsset) && fn_80104020(nAsset)) {
            aChoices[nFound++] = i;
        }
    }
    if (nFound) {
        FE_CrAP_TurnOnPart(nPart, 0, aChoices[Rand_Next(0) % nFound]);
    }
}

// The asset is one of the eight plain colours.
u8 fn_80078B84(CrAPAsset* pAsset) {
    if (pAsset == NULL) {
        return 0;
    }
    if (stricmp(pAsset->szName, "White") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Bright Red") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Orange") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Pink") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Yellow") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Green") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Purple") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Blue") == 0) {
        return 1;
    }
    return 0;
}

// Anything worn on the head but a plain hat (one worn backwards counts) or a visor.
u8 FE_CrAP_IsCrazyHat(CrAPAsset* pAsset) {
    if (pAsset == NULL) {
        return 0;
    }
    if (stricmp(fn_801064EC(pAsset->nCategory), "Hats") == 0 &&
        strstr(pAsset->szName, "backwards") == NULL) {
        return 0;
    }
    if (stricmp(fn_801064EC(pAsset->nCategory), "Visors") == 0) {
        return 0;
    }
    return 1;
}

// The same test as fn_80078B84.
u8 fn_80078D24(CrAPAsset* pAsset) {
    if (pAsset == NULL) {
        return 0;
    }
    if (stricmp(pAsset->szName, "White") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Bright Red") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Orange") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Pink") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Yellow") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Green") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Purple") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Blue") == 0) {
        return 1;
    }
    return 0;
}

// A random created golfer: random parts 10, 9 and 16; hair (part 3) with a 10% chance of corn
// rows, an afro or a mohawk; parts 4 to 6 on a random choice now and then; part 14 a plain colour
// one time in five, and part 15 usually the same choice; a hat (part 0) 40% of the time, a crazy
// one one time in five; and a few more parts by chance.
void fn_80078E34(SaveProfile* pProfile) {
    char szDebug[256];
    u8 bPicking;
    u8 bChance;
    int nAsset;
    int nCount;
    int nPick;
    int nPrev;
    CrAPAsset* pAsset;
    fn_8007975C(pProfile, 10, 0);
    fn_8007975C(pProfile, 9, 0);
    fn_8007975C(pProfile, 16, 0);

    bChance = Rand_Next(0) % 100 < 10;
    bPicking = 1;
    nCount = fn_801048EC(3, 0);
    while (bPicking) {
        nPick = Rand_Next(0) % nCount;
        pAsset = fn_80104E84(3, 0, nPick);
        if (bChance && pAsset &&
            (stricmp(pAsset->szName, "Corn Rows") == 0 || stricmp(pAsset->szName, "Afro") == 0 ||
             stricmp(pAsset->szName, "Mohawk") == 0)) {
            FE_CrAP_TurnOnPart(3, 0, nPick);
            bPicking = 0;
        }
        if (!bChance &&
            (pAsset == NULL || (stricmp(pAsset->szName, "Corn Rows") != 0 &&
                                stricmp(pAsset->szName, "Afro") != 0 &&
                                stricmp(pAsset->szName, "Mohawk") != 0))) {
            FE_CrAP_TurnOnPart(3, 0, nPick);
            bPicking = 0;
        }
    }

    bChance = Rand_Next(0) % 100 < 20;
    if (bChance) {
        nCount = fn_801048EC(4, 0);
        nPick = Rand_Next(0) % (nCount - 1);
        nPick++;
        FE_CrAP_TurnOnPart(4, 0, nPick);
    } else {
        FE_CrAP_TurnOnPart(4, 0, 0);
    }
    bChance = Rand_Next(0) % 100 < 10;
    if (bChance) {
        nCount = fn_801048EC(5, 0);
        nPick = Rand_Next(0) % (nCount - 1);
        nPick++;
        FE_CrAP_TurnOnPart(5, 0, nPick);
    } else {
        FE_CrAP_TurnOnPart(5, 0, 0);
    }
    bChance = Rand_Next(0) % 100 < 10;
    if (bChance) {
        nCount = fn_801048EC(6, 0);
        if (nCount != -1) {
            if (nCount > 1) {
                nPick = Rand_Next(0) % (nCount - 1);
            } else {
                nPick = Rand_Next(0) % (nCount - 1);   // EA bug: divides by zero for one choice
                nPick++;
            }
            FE_CrAP_TurnOnPart(6, 0, nPick);
        }
    } else {
        FE_CrAP_TurnOnPart(6, 0, 0);
    }

    bChance = Rand_Next(0) % 100 < 80;
    bPicking = 1;
    nCount = fn_801048EC(14, 0);
    while (bPicking) {
        nPick = Rand_Next(0) % nCount;
        pAsset = fn_80104E84(14, 0, nPick);
        if (bChance && !fn_80078B84(pAsset)) {
            FE_CrAP_TurnOnPart(14, 0, nPick);
            bPicking = 0;
        }
        if (!bChance && fn_80078B84(pAsset)) {
            FE_CrAP_TurnOnPart(14, 0, nPick);
            bPicking = 0;
        }
        nPrev = nPick;
    }
    bChance = Rand_Next(0) % 100 < 90;
    if (bChance) {
        FE_CrAP_TurnOnPart(15, 0, nPrev);
    } else {
        bChance = Rand_Next(0) % 100 < 80;
        bPicking = 1;
        nCount = fn_801048EC(15, 0);
        while (bPicking) {
            nPick = Rand_Next(0) % nCount;
            pAsset = fn_80104E84(15, 0, nPick);
            if (bChance && !fn_80078D24(pAsset)) {
                FE_CrAP_TurnOnPart(15, 0, nPick);
                bPicking = 0;
            }
            if (!bChance && fn_80078D24(pAsset)) {
                FE_CrAP_TurnOnPart(15, 0, nPick);
                bPicking = 0;
            }
        }
    }

    bChance = Rand_Next(0) % 100 < 40;
    if (bChance) {
        bChance = Rand_Next(0) % 100 < 80;
        bPicking = 1;
        nCount = fn_801048EC(0, 0);
        while (bPicking) {
            nPick = Rand_Next(0) % nCount;
            pAsset = fn_80104E84(0, 0, nPick);
            if (bChance && !FE_CrAP_IsCrazyHat(pAsset)) {
                FE_CrAP_TurnOnPart(0, 0, nPick);
                bPicking = 0;
            }
            if (!bChance && FE_CrAP_IsCrazyHat(pAsset)) {
                FE_CrAP_TurnOnPart(0, 0, nPick);
                bPicking = 0;
            }
        }
        nAsset = fn_80104FA8(0, 0, nPick);
        sprintf(szDebug, "I hate everone: %d", nAsset);     // a leftover debug line; never shown
    } else {
        FE_CrAP_TurnOnPart(0, 0, 0);
    }

    fn_801073DC(5);
    fn_801073DC(6);
    fn_801073DC(7);
    fn_801073DC(8);
    fn_801073DC(11);
    fn_801073DC(12);
    fn_801073DC(13);
    fn_801073DC(14);
    if (Rand_Next(0) % 100 < 30) {
        fn_8007975C(pProfile, 19, 0);
    }
    if (Rand_Next(0) % 100 < 30) {
        fn_8007975C(pProfile, 20, 0);
    }
    bChance = Rand_Next(0) % 100 < 20;
    if (bChance) {
        nCount = fn_801048EC(19, 3);
        nPick = Rand_Next(0) % nCount;
        fn_80104E84(19, 3, nPick);
        FE_CrAP_TurnOnPart(19, 0, nPick);
    }
    bChance = Rand_Next(0) % 100 < 5;
    if (bChance) {
        nCount = fn_801048EC(8, 0);
        FE_CrAP_TurnOnPart(8, 0, Rand_Next(0) % nCount);
    } else {
        fn_801073DC(13);
    }
}

void fn_80079664(SaveProfile* pProfile) {
    FE_CrAP_TurnOnPart(0xC, 1, 0);
    FE_CrAP_TurnOnPart(0xC, 2, 0);
    FE_CrAP_TurnOnPart(0xC, 3, 0);
    FE_CrAP_TurnOnPart(0xC, 5, 0);
    FE_CrAP_TurnOnPart(0xC, 6, 0);
    FE_CrAP_TurnOnPart(0xC, 7, 0);
    FE_CrAP_TurnOnPart(0xC, 0, 0);
    fn_8008E6D4(0);
    fn_8007975C(pProfile, 1, 0);
    fn_8007975C(pProfile, 2, 0);
    fn_8007975C(pProfile, 7, 0);
    fn_800797E0(pProfile, 7, (Rand_Next(0) & 1) + 1, 0);
    fn_80078E34(pProfile);
}

// Part nPart at a random b (fn_801049C8 counts them), then at a random choice (fn_800797E0).
void fn_8007975C(SaveProfile* pProfile, s16 nPart, int nChance) {
    int nCount = fn_801049C8(nPart);
    int nPick;
    if (nCount > 0) {
        nPick = Rand_Next(0) % nCount;
    } else {
        nPick = 0;
    }
    fn_800797E0(pProfile, nPart, nPick, nChance);
}

// Part nPart, b at a random choice, which is returned; with a chance of nChance percent, its first
// choice instead. Outside the session's 0x4000 mode (which also skips the intro movie) only
// choices fn_80104020 allows are drawn. -1: nothing was picked.
int fn_800797E0(SaveProfile* pProfile, s16 nPart, int b, int nChance) {
    int aChoices[250];
    int nFound = 0;
    int nCount;
    int i;
    int nPick;
    fn_80105C00();
    if ((int)(Rand_Next(0) % 100) + 1 <= nChance) {
        if (gSession.uFlags & 0x4000) {
            FE_CrAP_TurnOnPart(nPart, b, 0);
            return 0;
        }
        return -1;
    }
    if (gSession.uFlags & 0x4000) {
        nCount = fn_801048EC(nPart, b);
        if (nCount != 0) {
            nPick = Rand_Next(0) % nCount;
            FE_CrAP_TurnOnPart(nPart, b, nPick);
            return nPick;
        }
    } else {
        nCount = fn_801048EC(nPart, b);
        for (i = 0; i < nCount; i++) {
            if (fn_80104020(fn_80104FA8(nPart, b, i))) {
                aChoices[nFound] = i;
                nFound++;
            }
            if (nFound == 250) break;
        }
        if (nFound == 0) {
            FE_CrAP_TurnOnPart(nPart, b, 0);
            return 0;
        }
        nPick = Rand_Next(0) % nFound;
        FE_CrAP_TurnOnPart(nPart, b, aChoices[nPick]);
        return aChoices[nPick];
    }
    return -1;
}

// With a working copy of the profile: a random part 9, and a fixed set of parts turned on.
void fn_80079974(void) {
    SaveProfile* pProfile = fn_80077ACC();
    int i;
    if (lbl_80281ED4->bCopy) {
        fn_80103B74(0);
        fn_8007975C(pProfile, 9, 0);
        FE_CrAP_TurnOnPart(3, 0, 0);
        FE_CrAP_TurnOnPart(0xE, 0, 0);
        FE_CrAP_TurnOnPart(0xF, 0, 0);
        FE_CrAP_TurnOnPart(0x10, 0, 0);
        FE_CrAP_TurnOnPart(0xC, 0, 0);
        FE_CrAP_TurnOnPart(0xC, 1, 0);
        FE_CrAP_TurnOnPart(0xC, 2, 0);
        FE_CrAP_TurnOnPart(0xC, 3, 0);
        FE_CrAP_TurnOnPart(0xC, 5, 0);
        FE_CrAP_TurnOnPart(0xC, 6, 0);
        FE_CrAP_TurnOnPart(0xC, 7, 0);
        FE_CrAP_TurnOnPart(0xC, 4, 0);
        for (i = 0; i < 8; i++) {
            FE_CrAP_TurnOnPart(0xD, 0, i);
            FE_CrAP_TurnOnPart(0xD, 1, i);
        }
        FE_CrAP_TurnOnPart(0xD, 2, 0);
        fn_80103B74(1);
    }
}

// Set up the session for the menus. In game modes 5 and 11 player 1 plays the created golfer when
// slot 1 holds a profile, else golfer 0. Each player slot is a CPU player, a loaded profile (a
// created golfer brings its own bag) or a table golfer with its bag; slots past the players have
// no profile. Then the mode the menus start in.
void fn_80079AD4(void) {
    int i;
    if (Game_GetMode() == 5 || Game_GetMode() == 11) {
        if (gpSaveData[0].bActive) {
            lbl_801D7148.aBackup[0] = 0;
            if (!lbl_80281ED4->b11703) {
                Session_SetGolfer(FIRST_CREATED_GOLFER, 0);
            }
        } else {
            Session_SetGolfer(0, 0);
        }
    }
    for (i = 0; i < 5; i++) {
        if (lbl_801D7148.aCPU[i]) {
            gSession.nController[i] = CONTROLLER_CPU;
            gSession.uBag[i] = 0;
            gpSaveData[i].bActive = 0;
        } else if (lbl_801D7148.aLoaded[i]) {
            gpSaveData[i].bActive = 1;
            if (gSession.nGolfer[i] >= FIRST_CREATED_GOLFER) {
                gSession.uBag[i] = gpSaveData[i].createdGolfer.uBagMask;
            }
        } else if (gSession.nController[i] == CONTROLLER_CPU) {
            gpSaveData[i].bActive = 0;
        } else if (!lbl_801D7148.aLoaded[i]) {
            gpSaveData[i].bActive = 1;
            gSession.uBag[i] = gGolferTable[gSession.nGolfer[i]].uBagMask;
        }
        if (i > gSession.nNumPlayers - 1) {
            gpSaveData[i].bActive = 0;
        }
    }
    lbl_801D7148.nMode = Game_GetMode();
    if (fn_801025F4()) {
        lbl_801D7148.nMode = 4;
    }
    if (fn_800EE470()) {
        lbl_801D7148.nMode = 23;
    }
    if (lbl_80281ED4->b0 && Game_GetMode() == 10) {
        lbl_801D7148.nMode = 27;
    }
    if (lbl_801D7148.nMode == 11 && gpSaveData[lbl_80281ED4->nSlot].nTourCardLevel > 0) {
        lbl_801D7148.nMode = 28;
    }
    lbl_801D7148.b11 = 0;
    lbl_801D87C0.b0 = 1;
    gSession.a8[0] = 0;
    fn_800A4FD8();
    fn_80102AC4();
}

// ---- the profile backups in ARAM ---------------------------------------------------------------

// Move the backups out to ARAM, freeing the main memory they used.
void fn_80079D30(void) {
    if (lbl_801D7148.p658 != NULL) {
        lbl_80281ECC = FE_BACKUP_SIZE;
        if (lbl_80281ED0 == 0) {
            lbl_80281ED0 = fn_800B6564(FE_BACKUP_SIZE);
        }
        fn_800B6844(lbl_801D7148.p658, lbl_80281ED0, lbl_80281ECC);
        fn_800B67EC();
        fn_80009E70(lbl_801D7148.p658);
        lbl_801D7148.p658 = NULL;
    }
}

// Bring the backups back from ARAM (empty ones if there were none).
void fn_80079DAC(void) {
    if (lbl_801D7148.p658 == NULL) {
        lbl_80281ECC = FE_BACKUP_SIZE;
        lbl_801D7148.p658 = fn_80009B34(lbl_80281ECC, 2, 32, "FE_Manager.c", 2778);
        memset(lbl_801D7148.p658, 0, lbl_80281ECC);
        if (lbl_80281ED0 != 0) {
            fn_800B68B4(lbl_801D7148.p658, lbl_80281ED0, lbl_80281ECC);
            fn_800B67EC();
            fn_800B6594(lbl_80281ED0);
            lbl_80281ED0 = 0;
        }
    }
}

// Whether an attribute is one of the hidden ones.
u8 fn_80079E44(int nAttr) {
    int bHidden = 0;
    if (nAttr == ATTR_AGGRESSION || nAttr == ATTR_IQ || nAttr == ATTR_SPEED) {
        bHidden = 1;
    }
    return bHidden;
}
