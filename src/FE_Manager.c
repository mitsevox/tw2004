// FE_Manager.c (EA's name, from its asserts; also in EA's 2002 source tree): the front end's
// manager, which runs the menu screens: the movies played from the menus (the intro, the credits,
// the golfers' bios), the profile being worked on, and the created golfer. Partly decompiled.

#include "engine.h"
#include "game.h"
#include "frontend/fe.h"

// Outside this file.
// The movie player (LLVideo.c): plays a movie file until it ends or pfnSkip returns nonzero.
void fn_80075FB8(char* pPath, int (*pfnSkip)(void), int a, int b);
void fn_80037FB4(u8 a, f32* pColor);    // a full-screen colour (GoPostFx.c)
void fn_80010284(void);
void fn_80013400(void);                 // read the controllers
void fn_8008B704(void);
void fn_8008B754(int a);
void fn_8008B7D0(int a);
void fn_8008D8CC(void);
void fn_8008D8F4(void);
void fn_8008DAEC(void);
void fn_8008DBE8(void);
void fn_8008E6D4(int a);
void fn_80079EA8(void);
void fn_80092198(void);
void fn_8009220C(void);
void fn_800A75B4(void);
void fn_800A7644(int a);
void fn_80103B74(int a);
int  fn_80103D14(s16 nSlot);            // the asset worn in an equipment slot, or -1 (FE_CrAPDB.c)
u8   fn_80104020(void* pChoice);        // } a part's choices (FE_CrAPDB.c): whether one may be
int  fn_801048EC(s16 nPart, int a);     // } picked, how many there are, one of them and its
void* fn_80104FA8(s16 nPart, int a, int i);   // } asset
CrAPAsset* fn_80104F68(void* pChoice);  // }
int  fn_801049C8(int a);                // how many choices part a has (FE_CrAPDB.c)
int  fn_80105494(int nAsset);           // } the two attributes an asset raises (-1: none)
int  fn_80105504(int nAsset);           // }
int  fn_801054CC(int nAsset);           // } and the tier it raises each to
int  fn_8010553C(int nAsset);           // }
u8   fn_80105C30(void);                 // the Create-A-Player database is loaded (FE_CrAPDB.c)

// This file, in address order.
void fn_80076F80(UStreamObject* pObject);
int  fn_80076FDC(void);
void fn_8007706C(char* pName, char* pDir, char* pPath);
void fn_800770D4(char* pName, char* pPath);
FEMovie* fn_800770FC(void);
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
void fn_80077780(void);
void fn_80077968(int nSlot);
void fn_800779BC(int a, int b);
GolferRecord* fn_80077A80(int nGolfer);
void fn_80077B78(void);
int  fn_80077BDC(int n);
void fn_80077C1C(int a, int b);
int  fn_80078604(int a, int b, int c);
void fn_80078620(int n, int* pA, int* pB, int* pC);
void fn_8007873C(SaveProfile* pProfile);
u8   FE_CrAP_IsAssetUndesirable(s16 nPart, CrAPAsset* pAsset);
void fn_80078A2C(s16 nPart, int nChance);
u8   fn_80078B84(CrAPAsset* pAsset);
u8   FE_CrAP_IsCrazyHat(CrAPAsset* pAsset);
u8   fn_80078D24(CrAPAsset* pAsset);
void fn_80078E34(SaveProfile* pProfile);
void fn_80079664(SaveProfile* pProfile);
void fn_8007975C(SaveProfile* pProfile, int a, int b);
void fn_800797E0(SaveProfile* pProfile, int a, int nChoice, int b);
void fn_80079974(void);
void fn_80079D30(void);
void fn_80079DAC(void);

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

// Back up every slot's profile, where one is loaded.
void fn_80077780(void) {
    int i;
    for (i = 0; i < 4; i++) {
        if (gpSaveData[i].bActive) {
            Mem_cpy(&lbl_801D7148.p658[i], &gpSaveData[i], sizeof(SaveProfile));
        }
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

// Put a random choice on part nPart: an undesirable one (see FE_CrAP_IsAssetUndesirable) with a
// chance of 100 - nChance percent, else a desirable one; only choices fn_80104020 allows.
void fn_80078A2C(s16 nPart, int nChance) {
    int aChoices[250];
    u32 bDesirable = (int)(Rand_Next(0) % 100) < nChance;
    int nFound = 0;
    int nCount = fn_801048EC(nPart, 0);
    int i;
    void* pChoice;
    CrAPAsset* pAsset;
    for (i = 0; i < nCount; i++) {
        pChoice = fn_80104FA8(nPart, 0, i);
        pAsset = fn_80104F68(pChoice);
        if (bDesirable && !FE_CrAP_IsAssetUndesirable(nPart, pAsset) && fn_80104020(pChoice)) {
            aChoices[nFound++] = i;
        } else if (!bDesirable && FE_CrAP_IsAssetUndesirable(nPart, pAsset) && fn_80104020(pChoice)) {
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
    if (fn_8015F844(pAsset->szName, "White") == 0) {
        return 1;
    }
    if (fn_8015F844(pAsset->szName, "Bright Red") == 0) {
        return 1;
    }
    if (fn_8015F844(pAsset->szName, "Orange") == 0) {
        return 1;
    }
    if (fn_8015F844(pAsset->szName, "Pink") == 0) {
        return 1;
    }
    if (fn_8015F844(pAsset->szName, "Yellow") == 0) {
        return 1;
    }
    if (fn_8015F844(pAsset->szName, "Green") == 0) {
        return 1;
    }
    if (fn_8015F844(pAsset->szName, "Purple") == 0) {
        return 1;
    }
    return fn_8015F844(pAsset->szName, "Blue") == 0;
}

// Anything worn on the head but a plain hat (one worn backwards counts) or a visor.
u8 FE_CrAP_IsCrazyHat(CrAPAsset* pAsset) {
    if (pAsset == NULL) {
        return 0;
    }
    if (fn_8015F844(fn_801064EC(pAsset->nCategory), "Hats") == 0 &&
        strstr(pAsset->szName, "backwards") == NULL) {
        return 0;
    }
    if (fn_8015F844(fn_801064EC(pAsset->nCategory), "Visors") == 0) {
        return 0;
    }
    return 1;
}

// The same test as fn_80078B84.
u8 fn_80078D24(CrAPAsset* pAsset) {
    if (pAsset == NULL) {
        return 0;
    }
    if (fn_8015F844(pAsset->szName, "White") == 0) {
        return 1;
    }
    if (fn_8015F844(pAsset->szName, "Bright Red") == 0) {
        return 1;
    }
    if (fn_8015F844(pAsset->szName, "Orange") == 0) {
        return 1;
    }
    if (fn_8015F844(pAsset->szName, "Pink") == 0) {
        return 1;
    }
    if (fn_8015F844(pAsset->szName, "Yellow") == 0) {
        return 1;
    }
    if (fn_8015F844(pAsset->szName, "Green") == 0) {
        return 1;
    }
    if (fn_8015F844(pAsset->szName, "Purple") == 0) {
        return 1;
    }
    return fn_8015F844(pAsset->szName, "Blue") == 0;
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

// Part a at a random one of its choices.
void fn_8007975C(SaveProfile* pProfile, int a, int b) {
    int nCount = fn_801049C8(a);
    int nPick;
    if (nCount > 0) {
        nPick = Rand_Next(0) % nCount;
    } else {
        nPick = 0;
    }
    fn_800797E0(pProfile, a, nPick, b);
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
