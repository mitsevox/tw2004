// FETextureMgr.c: the front end's texture banks, which 'txf2' stream objects bring in and which are
// searched by a texture's name; the loading bar's texture object; the loading screen's picture
// ('load'), picked for the round about to start. Also the game mode getter and the name packer.

#include "game.h"
#include "ustream.h"

UStreamObject* gpFEtmLoadingBarObject;  // a copy of the 'txf2' object FETM_LOADING_BAR_ID
u8* gpFEtmLoadingBarData;               // the first FETM_LOADING_BAR_DATA_SIZE bytes of its data
u8* gpFEtmLoadScreenData;               // the 'load' object's data (FETM_LOAD_SCREEN_DATA_SIZE)
int gFEtmNumBanks;                      // how many 'txf2' objects became banks
TexBank* gFEtmBanks[FETM_NUM_BANKS];    // the banks, each at its object's id (modulo 100000)
u8 gFEtmBankLoaded[FETM_NUM_BANKS];     // 1: the slot holds a bank from a 'txf2' object
LoadObjInfo gFEtmLoadScreenInfo;

static void FEtm_vRegisterLoadScreenStreamClient(void);
static void FEtm_vUnregisterLoadScreenStreamClient(void);
static void FEtm_vLoadScreenStreamCallback(UStreamObject* xpStreamData_p);
static void FEtm_vStreamCallback(UStreamObject* xpStreamData_p);
static void FEtm_ClearLoadScreenList(void);

// As the front end starts: no banks.
void FEtm_InitFETextureMgr(void) {
    int i;

    gFEtmNumBanks = 0;
    for (i = 0; i < FETM_NUM_BANKS; i++) {
        gFEtmBanks[i] = NULL;
        gFEtmBankLoaded[i] = 0;
    }
}

// As the front end shuts down: frees the banks in the first gFEtmNumBanks slots, with their pixel
// and palette data when the slot is marked loaded.
void FEtm_CloseFETextureMgr(void) {
    int i;

    for (i = 0; i < gFEtmNumBanks; i++) {
        if (gFEtmBankLoaded[i]) {
            TX_vFreeTextureDataInTextureGroup(gFEtmBanks[i]);
        }
        Circular_MemFreeFunc(gFEtmBanks[i]);
        gFEtmBanks[i] = NULL;
    }
}

// At boot: the loading screen's data buffer and the loading bar's object and data, from the system
// heap.
void FEtm_AllocBuffers(void) {
    gpFEtmLoadScreenData = Mem_AllocFunc(FETM_LOAD_SCREEN_DATA_SIZE, 16, 1);
    gpFEtmLoadingBarObject = Mem_AllocFunc(sizeof(UStreamObject), 16, 1);
    gpFEtmLoadingBarData = Mem_AllocFunc(FETM_LOADING_BAR_DATA_SIZE, 16, 1);
}

// With the front end's other stream handlers: 'txf2' objects go to FEtm_vStreamCallback.
void FEtm_vRegisterStreamClients(void) {
    Stream_RegisterLoadChunkCallback(TAG('t', 'x', 'f', '2'), FEtm_vStreamCallback);
}

void FEtm_vUnregisterStreamClients(void) {
    Stream_UnregisterLoadChunkCallback(TAG('t', 'x', 'f', '2'));
}

static void FEtm_vRegisterLoadScreenStreamClient(void) {
    Stream_RegisterLoadChunkCallback(TAG('l', 'o', 'a', 'd'), FEtm_vLoadScreenStreamCallback);
}

static void FEtm_vUnregisterLoadScreenStreamClient(void) {
    Stream_UnregisterLoadChunkCallback(TAG('l', 'o', 'a', 'd'));
}

// The 'load' object (the loading screen's picture): its size is kept and its data copied into
// gpFEtmLoadScreenData, then the object is freed.
static void FEtm_vLoadScreenStreamCallback(UStreamObject* xpStreamData_p) {
    gFEtmLoadScreenInfo.uSize = xpStreamData_p->uSize;
    Mem_cpy(gpFEtmLoadScreenData, xpStreamData_p->pData, xpStreamData_p->uSize);
    Circular_MemFreeFunc(xpStreamData_p);
}

// When the front end ends for a round, before the loading screen shows: streams its picture, file
// nFile of data/Load/Load%d.gcb. File 2 while the calendar screen's flag is set; 0 for the lessons,
// the Play Now challenges, four of the five target games, and Random 18, custom and regional
// rounds; 21 for Tiger's Dream 18; otherwise the course's own (0 for a course without one).
void FEtm_StreamLoadScreen(void) {
    int nFile;

    if (PlayNow_GetCalendarFlag()) {
        nFile = 2;
    } else if (GM_GetModeType() == GM_Lessons_mode || GM_GetModeType() == GM_PlayNow_mode ||
               PlayNow_IsChallengeRunning()) {
        nFile = 0;
    } else if (GM_GetModeType() == GM_SkillZoneTimed_mode ||
               GM_GetModeType() == GM_SkillZoneCapture_mode ||
               GM_GetModeType() == GM_SkillZoneTarget_mode ||
               GM_GetModeType() == GM_SkillZoneTargetToTarget_mode) {
        nFile = 0;
    } else if (gpGame->bRandom18 || gpGame->bCustomRound || gpGame->nRegionalRound) {
        nFile = 0;
    } else if (gpGame->bDream18) {
        nFile = 21;
    } else {
        switch (Game_GetCourse()) {
        case 0:  nFile = 1;  break;     // Pebble Beach
        case 1:  nFile = 3;  break;     // Princeville Resort
        case 2:  nFile = 4;  break;     // TPC at Sawgrass
        case 3:  nFile = 5;  break;     // Black Rock Cove
        case 6:  nFile = 6;  break;     // Royal Birkdale
        case 10: nFile = 7;  break;     // Spyglass Hill
        case 11: nFile = 8;  break;     // Poppy Hills
        case 12: nFile = 9;  break;     // The Highlands
        case 13: nFile = 10; break;     // TPC of Scottsdale
        case 15: nFile = 11; break;     // St Andrews
        case 14: nFile = 12; break;     // Torrey Pines
        case 9:  nFile = 13; break;     // The Predator
        case 4:  nFile = 20; break;     // Penguin Falls
        case 5:  nFile = 22; break;     // Bethpage Black
        case 8:  nFile = 14; break;     // Bay Hill Club
        case 16: nFile = 16; break;     // Sahalee CC
        case 17: nFile = 19; break;     // Emerald Dragon
        case 18: nFile = 15; break;     // Wallaby Creek
        case 19: nFile = 18; break;     // Kapalua Plantation
        case 20: nFile = 17; break;     // Pinehurst No. 2
        default: nFile = 0;  break;
        }
    }
    FEtm_vRegisterLoadScreenStreamClient();
    StreamManager_AddLoadScreenFile(nFile);
    StreamManagerLoadScreen_StreamFiles();
    FEtm_ClearLoadScreenList();
    FEtm_vUnregisterLoadScreenStreamClient();
}

// A 'txf2' object. The one with id FETM_LOADING_BAR_ID (modulo 100000) is copied, with the first
// FETM_LOADING_BAR_DATA_SIZE bytes of its data, into the loading bar's buffers; any other becomes a
// new bank with its own copy of the pixels and palettes, in the slot of its id. The object is freed.
static void FEtm_vStreamCallback(UStreamObject* xpStreamData_p) {
    int iAssocChkRef = (int)xpStreamData_p->uId % 100000;

    if (iAssocChkRef == FETM_LOADING_BAR_ID) {
        Mem_cpy(gpFEtmLoadingBarObject, xpStreamData_p, sizeof(UStreamObject));
        Mem_cpy(gpFEtmLoadingBarData, xpStreamData_p->pData, FETM_LOADING_BAR_DATA_SIZE);
        gpFEtmLoadingBarObject->pData = gpFEtmLoadingBarData;
        Circular_MemFreeFunc(xpStreamData_p);
        return;
    }
    gFEtmBanks[iAssocChkRef] = TX_spDownloadData(xpStreamData_p, NULL, 0);
    gFEtmBankLoaded[iAssocChkRef] = 1;
    Circular_MemFreeFunc(xpStreamData_p);
    gFEtmNumBanks++;
}

// The pixels of the first level of the texture named pName, in any bank: 1 and *ppPixels set, or 0
// when either pointer is NULL or no bank has the texture.
u8 FEtm_GetTexturePixels(char* pName, u8** ppPixels) {
    TexBank* pBank;
    TexEntry* pTex;

    if (pName == NULL || ppPixels == NULL) {
        return 0;
    }
    FEtm_FindTexture(pName, &pBank, &pTex);
    if (pTex == NULL) {
        return 0;
    }
    *ppPixels = pBank->pPixels + pTex->aMips[0].uPixels;
    return 1;
}

// The texture named pName in the first slot (from 0) whose bank has it: its bank and entry, and the
// slot is returned. -1 and both NULL when none has it.
int FEtm_FindTexture(char* pName, TexBank** ppBank, TexEntry** ppTex) {
    int i;
    u64 uName = FEtm_PackName(pName);
    u64 nTex;
    TexBank* pBank;

    for (i = 0; i < FETM_NUM_BANKS; i++) {
        if (gFEtmBankLoaded[i]) {
            pBank = gFEtmBanks[i];
            if (pBank != NULL) {
                nTex = (u32)TX_uiFindTextureIndexFromID(pBank, uName);
                if (nTex != TX_NO_TEXTURE_INDEX) {
                    *ppBank = pBank;
                    *ppTex = &pBank->pTextures[nTex];
                    return i;
                }
            }
        }
    }
    *ppBank = NULL;
    *ppTex = NULL;
    return -1;
}

// Empties the stream manager's loading-screen list once its file has streamed.
static void FEtm_ClearLoadScreenList(void) {
    STREAM_LIST(STREAM_FILE_LOADSCREEN).nNumFiles = 0;
}

GM_ModeType_t GM_GetModeType(void) {
    return gpGame->nMode;
}

// pName packed into a 64-bit name code (SKA_PackName), the key textures and other named data are
// looked up by.
u64 FEtm_PackName(char* pName) {
    u64 uName = 0;

    SKA_PackName(&uName, pName);
    return uName;
}
