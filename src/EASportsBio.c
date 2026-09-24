// EASportsBio.c (EA's name, from its asserts): the game's side of the EA Sports Bio, EA's
// cross-game player profile. It starts the library with this game's name, turns the memory-card
// screens' requests into library calls (EASB.c), and reports level-ups and accomplishments to the
// front end. Its state is the manager lbl_80281988 (TW06's EASBio_MgrT).

#include "engine.h"
#include "core/memcard.h"
#include "core/easb.h"

s32 fn_80106ED8(s32 arg0, s32 nLevel);
s32 fn_80107084(s32 arg0, s32 nLevel);
s32 EASBio_ConvertError(EASBErrorE eError);
s32 EASBio_WaitForAsyncProcessToComplete(void);
s32 fn_80125520(u8 b);
s32 fn_801258E8(void);
u8 fn_801257A0(void);
void fn_80124B10(UStreamObject* pObject);

// Starts the Bio library once, under this game's name, with the memory-card glue from TibExt.c.
void EASBio_InitOnce(void) {
    lbl_80261040.szProductName = lbl_80195308;
    lbl_80261040.szGamesPlayedType = lbl_80195324;
    lbl_80261040.pCallbacks = fn_801221F0();
    lbl_80261040.uHeapID = 0;
    lbl_80261040.uGamesPlayedTypeLanguage = 'en';
    fn_8000A0AC(0);
    fn_8012D394(&lbl_80261040);
    fn_8000A0AC(2);
    lbl_80281988->bNewAccomplishment = 0;
    lbl_80281988->bBioLoaded = 0;
    fn_8012DAB8(2);
    EASBio_SetCurrentRewardMessage(EASBio_eReward_None);
}

// The 'EASI' stream object is the Bio icon.
void fn_80124A40(void) {
    UStream_RegisterHandler('EASI', fn_80124B10);
}

void fn_80124A70(void) {
    UStream_UnregisterHandler('EASI');
}

// The memory-card screens' error code for a library error.
s32 EASBio_ConvertError(EASBErrorE eError) {
    return EASB_gErrorMap[eError];
}

// Runs the library's memory-card operation step by step until it is complete.
s32 EASBio_WaitForAsyncProcessToComplete(void) {
    EASBProcessE eProcess;
    EASBErrorE eError;

    eProcess = EASB_PROCESS_NONE;
    while (eProcess != EASB_PROCESS_COMPLETE) {
        eError = fn_8012D7F8(&eProcess);
        if (eError != EASB_ERROR_NONE) return EASBio_ConvertError(eError);
        if (eProcess == EASB_PROCESS_NONE) return 0;
    }
    return 0;
}

// Copies the 'EASI' icon into the manager's buffer.
void fn_80124B10(UStreamObject* pObject) {
    Mem_cpy(lbl_80281988->pIcon, pObject->pData, pObject->uSize);
    fn_80009E70(pObject);
}

// Sets up the manager for the front end: no pictures yet, room for the icon, nothing pending.
void fn_80124B54(void) {
    lbl_80281988->pCurrentGameImage = NULL;
    lbl_80281988->pOurGameImage = NULL;
    lbl_80281988->pIcon = fn_80009B34(0x2000, 2, 32, "EASportsBio.c", 454);
    lbl_8028257D = 0;
    lbl_8028257C = 0;
    lbl_80282578 = 0;
    lbl_80282574 = 0;
    lbl_80282570 = 0;
    lbl_8028256C = 0;
    lbl_8028256A = 0;
    lbl_80282569 = 0;
    lbl_80282568 = 1;
    EASBio_SetGamePlayState(0);
}

// The Bio's level progress as a percentage.
f32 fn_80124BDC(void) {
    u16 uLevel;
    f32 fProgress;

    fn_8012DF4C(&uLevel, &fProgress);
    return 100.0f * fProgress;
}

void fn_801250C0(void) {
    if (lbl_80281988->pCurrentGameImage != NULL) {
        fn_80009E70(lbl_80281988->pCurrentGameImage);
    }
    if (lbl_80281988->pOurGameImage != NULL) {
        fn_80009E70(lbl_80281988->pOurGameImage);
    }
    if (lbl_80281988->pIcon != NULL) {
        fn_80009E70(lbl_80281988->pIcon);
    }
}

// TRUE when the card could be opened (or reported -18).
int fn_80125118(s32* pArgs) {
    s32 nPort;
    s32 nSlot;
    s32 nError;
    u8 bOk;

    nSlot = pArgs[1];
    nPort = pArgs[0];
    nError = fn_80125354(nPort, nSlot);
    if (nError == 0 || nError == -18) {
        bOk = 1;
    } else {
        bOk = 0;
    }
    if (nError == 0) {
        fn_801253F0(nPort, nSlot);
    }
    return bOk;
}

s32 fn_80125194(s32 arg0, s32 arg1) {
    s32 nError;

    nError = fn_80125354(arg0, arg1);
    if (nError == 0) {
        fn_801253F0(arg0, arg1);
    }
    return nError;
}

s32 fn_801251EC(s32* pArgs) {
    s32 nPort;
    s32 nSlot;
    s32 nError;
    EASBErrorE eError;

    nSlot = pArgs[1];
    nPort = pArgs[0];
    nError = fn_80125354(nPort, nSlot);
    if (nError != 0) return nError;
    eError = fn_8012D794(lbl_80281988->pOurGameImage);
    if (eError != EASB_ERROR_NONE) return EASBio_ConvertError(eError);
    nError = EASBio_WaitForAsyncProcessToComplete();
    fn_801253F0(nPort, nSlot);
    return nError & ((-nError | nError) >> 31);
}

// nPort and nSlot are unused, as in the ones below: the menus pass the card they picked.
s32 fn_80125280(s32 nPort, s32 nSlot) {
    s32 nError;
    EASBErrorE eError;

    eError = fn_8012D694();
    if (eError != EASB_ERROR_NONE) return EASBio_ConvertError(eError);
    nError = EASBio_WaitForAsyncProcessToComplete();
    if (nError != 0) return nError;
    lbl_80281988->bBioLoaded = 0;
    return 0;
}

s32 fn_801252D0(s32 nPort, s32 nSlot) {
    s32 nMount;
    s32 nError;
    EASBErrorE eError;

    nMount = fn_8009D74C(0, 0);
    fn_8009DBAC(0, 0);
    if (nMount == -25) return -25;
    eError = fn_8012D5E4(lbl_80281988->pIcon, lbl_80281988->pOurGameImage);
    if (eError != EASB_ERROR_NONE) return EASBio_ConvertError(eError);
    nError = EASBio_WaitForAsyncProcessToComplete();
    return nError & ((-nError | nError) >> 31);
}

// arg0 and arg1 are unused; its callers hand on their own two arguments.
s32 fn_80125354(s32 arg0, s32 arg1) {
    s32 nMount;
    s32 nError;
    EASBErrorE eError;

    nMount = fn_8009D74C(0, 0);
    fn_8009DBAC(0, 0);
    if (nMount == -25) return -25;
    eError = fn_8012D560();
    if (eError != EASB_ERROR_NONE) return EASBio_ConvertError(eError);
    nError = EASBio_WaitForAsyncProcessToComplete();
    if (nError != 0) return nError;
    if (!lbl_80281988->bBioLoaded) {
        fn_8012D9B4(&lbl_80281988->uCurLevel);
        lbl_80281988->bBioLoaded = 1;
    }
    return 0;
}

s32 fn_801253F0(s32 arg0, s32 arg1) {
    s32 nError;
    EASBErrorE eError;

    eError = fn_8012D5B0();
    if (eError != EASB_ERROR_NONE) return EASBio_ConvertError(eError);
    nError = EASBio_WaitForAsyncProcessToComplete();
    return nError & ((-nError | nError) >> 31);
}

s32 fn_80125434(s32 arg0, s32 arg1) {
    s32 nWait;
    s32 nError;
    EASBErrorE eError;

    nError = fn_80125354(arg0, arg1);
    if (nError != 0) return nError;
    eError = fn_8012D6C8();
    if (eError != EASB_ERROR_NONE) return EASBio_ConvertError(eError);
    nWait = EASBio_WaitForAsyncProcessToComplete();
    fn_801253F0(arg0, arg1);
    return nWait & ((-nWait | nWait) >> 31);
}

s32 fn_801254B8(void) {
    EASBErrorE eError;

    eError = fn_8012D7F0();
    if (eError != EASB_ERROR_NONE) return EASBio_ConvertError(eError);
    return 0;
}

s32 fn_801254EC(void) {
    EASBErrorE eError;

    eError = fn_8012D710();
    if (eError != EASB_ERROR_NONE) return EASBio_ConvertError(eError);
    return 0;
}

s32 fn_80125520(u8 b) {
    return 0;
}

// TRUE when the Bio's file status (fn_8012D744) is 1.
u8 fn_80125528(s32 nPort, s32 nSlot) {
    u32 uStatus;

    // EA bug: the error is ignored, so uStatus is read unset when the library is not running
    fn_8012D744(&uStatus);
    return uStatus == 1;
}

s32 fn_8012555C(s32* pArgs) {
    s32 nError;
    s32 nPort;
    s32 nSlot;

    nSlot = pArgs[1];
    nPort = pArgs[0];
    nError = fn_80125354(nPort, nSlot);
    if (nError == -18) return 1;
    if (nError == 0) {
        fn_801253F0(nPort, nSlot);
    }
    return 0;
}

s32 fn_801255C4(s32* pArgs) {
    return fn_8009D1D8(pArgs[0], pArgs[1], 0, 3);
}

s32 fn_801255F8(void) {
    return 27;
}

void fn_80125600(s32 a0, s32 a1) {
    lbl_80282578 = a0;
    lbl_8028257D = 1;
    lbl_80282570 = a1;
    fn_8012E818(a1, lbl_80281988->pCurrentGameImage);
    fn_80125520(lbl_80282570);
}

void fn_80125648(s32 p0, s32 p1, s32 p2) {
    lbl_8028257C = 1;
    lbl_80282574 = p0;
    lbl_8028256C = p1;
    lbl_80282568 = (u32)(-p2 | p2) >> 31;
}

void fn_8012566C(s32 p0) {
    lbl_8028256A = (u32)(-p0 | p0) >> 31;
}

void fn_80125680(s32 p0) {
    lbl_80282569 = (u32)(-p0 | p0) >> 31;
}

u8 EASBio_IsBioLoaded(void) {
    return lbl_80281988->bBioLoaded;
}

void EASBio_SetCurrentRewardMessage(s32 eReward) {
    lbl_80281988->eCurrentRewardMessage = eReward;
}

s32 EASBio_GetCurrentRewardMessage(void) {
    return lbl_80281988->eCurrentRewardMessage;
}

// What to tell the player after a round: a new accomplishment, a level-up, or a level-up that
// unlocked something (TW06's EASBio_eReward values).
EASBio_eReward fn_801256B8(void) {
    EASBio_eReward eReward;
    u16 uLevel;
    u16 uNextLevel;

    eReward = EASBio_eReward_None;
    if (!EASBio_IsBioLoaded()) return EASBio_eReward_None;
    if (lbl_80281988->bNewAccomplishment) {
        lbl_80281988->bNewAccomplishment = 0;
        eReward = EASBio_eReward_NewAccomplishment;
    }
    if (fn_8012D9B4(&uLevel) != EASB_ERROR_NONE) return eReward;
    if (fn_8012DA38(&uNextLevel) != EASB_ERROR_NONE) return eReward;
    if (uLevel > lbl_80281988->uCurLevel || uLevel >= uNextLevel) {
        lbl_80281988->uCurLevel = uLevel;
        eReward = EASBio_eReward_LeveledUp;
        if (uNextLevel == EASB_MAX_LEVEL + 1 || lbl_80281988->uCurLevel < uNextLevel) {
            return EASBio_eReward_LeveledUp;
        }
        if (fn_801257A0()) {
            eReward = EASBio_eReward_LeveledUpAndUnlocked;
        }
    }
    return eReward;
}

u8 fn_801257A0(void) {
    s32 nLevel;
    s32 nUnlocked;
    s32 nNext;

    nLevel = fn_801258E8();
    nUnlocked = fn_80106ED8(12, nLevel);
    nNext = fn_80107084(12, nLevel);
    if (nNext == -1) {
        nNext = EASB_MAX_LEVEL + 1;
    }
    fn_8012DAB8((u16)nNext);
    return nUnlocked > 0;
}

void EASBio_SetGamePlayState(u8 bFlag) {
    fn_8012DD7C(bFlag);
}

void EASBio_IncrementGamesPlayed(u32 uCount) {
    fn_8012D8C4(uCount);
}

void EASBio_IncrementGamesWon(u32 uCount) {
    fn_8012D93C(uCount);
}

// Records an accomplishment by name; the next reward check reports it.
void EASBio_SetAccomplishment(const char* szName, s32 arg1) {
    u16 szWide[0x40];

    if (EASBio_IsBioLoaded()) {
        fn_800A27BC(szName, szWide, 0x3F);
        if (fn_8012DD24(szWide, arg1, 'en') == EASB_ERROR_NONE) {
            lbl_80281988->bNewAccomplishment = 1;
        }
    }
}

// The level as the library reports it; returned as an int (fn_801257A0 does not re-extend it).
s32 fn_801258E8(void) {
    u16 uLevel;

    fn_8012D9B4(&uLevel);
    return uLevel;
}

void EASBio_SetCurrentGameWon(u8 bWon) {
    lbl_80281988->bGameWon = bWon;
}

u8 EASBio_IsCurrentGameWon(void) {
    return lbl_80281988->bGameWon;
}

// The Bio's level, or 0 when no Bio is loaded.
s32 fn_80125928(void) {
    u16 uLevel = 0;
    f32 fProgress = 0.0f;

    if (lbl_80281988->bBioLoaded) {
        fn_8012DF4C(&uLevel, &fProgress);
        return uLevel;
    }
    return 0;
}
