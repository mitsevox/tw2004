// TibExt.c (EA's name, from its asserts; TW06): the glue between EA's libraries (the EA Sports
// Bio's EASB.c, the shared file library, TagFile, CRC32) and the game: memory, the real-time clock,
// and the memory-card callbacks (lbl_80281970) the shared file library calls. The callbacks that
// take a device use its port (eDevice / 4) and slot (eDevice % 4); the last card call's error and
// result are kept for SFIO_eProcessCallback, the library's update call.

#include "engine.h"
#include "game.h"
#include "core/easb.h"
#include "Common/SharedFileIO.h"
#include "core/memcard.h"

#define TIBEXT_MAX_FOUND 10     // save files one probe looks at

void SFIO_vSetCurrentError(s32 nCardError);
void SFIO_vSetCurrentResult(s32 n);
void SFIO_vFindCallback(char* pSearchName, int eDevice);
void SFIO_vFreeSpaceCallback(int eDevice);
void SFIO_vFreeEntryCallback(int eDevice);
void SFIO_vMountCallback(int eDevice);
void SFIO_vUnMountCallback(int eDevice);
void SFIO_vOpenCallback(const char* pDirName, char* pFileName, int eDevice, u32 uFlags);
void SFIO_vCloseCallback(int uHandle);
void SFIO_vCreateCallback(char* pFileName, u32 uSize, int eDevice);
void SFIO_vDeleteCallback(char* pDirName, char* pFileName, int eDevice);
void SFIO_vDestroyCallback(char* pDirName, int eDevice);
void SFIO_vReadCallback(int uHandle, void* pBuffer, u32 uSize);
void SFIO_vWriteCallback(int uHandle, void* pBuffer, u32 uSize);
void SFIO_vSeekCallback(int uHandle, u32 uOffset, u32 uWhence);
void fn_80122834(int uHandle);
void fn_80122868(int uHandle, u32 uValue);
int  SFIO_eProcessCallback(int* pProcess, int* pResult);
void SFIO_vGetFileCallback(char* pFileName);

TibExtCard lbl_80260D88;
TibExtCard* lbl_80281970 = &lbl_80260D88;

void* TibExtMemAlloc(u32 uHeapID, u32 uSize, u32 uAlign) {
    return StaticMem_Alloc(uSize, StaticMem_GetMode(), uAlign, "TibExt.c", 42);
}

void TibExtMemFree(u32 uHeapID, void* p, u32 uSize, u32 uAlign) {
    StaticMem_Free(p);
}

// The library's assert handler: does nothing in the retail game.
void TibExt_AssertHandler(const char* pFile, int uLine, const char* pExpr, int unused) {
}

// The clock in seconds since 1970-01-01.
u32 TibExtCurrentTimeGet(void) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    s32 nHour;
    s32 nMinute;
    s32 nSecond;
    s32 nMsec;
    u16 uEpoch;
    u16 uToday;
    int nDays;
    int nSeconds;

    fn_8011E020(&nMonth, &nDay, &nYear, &nHour, &nMinute, &nSecond, &nMsec);
    CalDate_SetMDY(&uEpoch, 0, 0, 1970);
    CalDate_SetMDY(&uToday, nMonth, nDay, nYear);
    nDays = uToday - uEpoch - 1;
    nSeconds = 86400 * nDays;
    nSeconds += nMinute * 60 + nHour * 3600 + nSecond;
    return nSeconds;
}

SFIOFuncTable* SFIO_spGetCallbacks(void) {
    lbl_80281970->fn.pfnProbe = SFIO_vFindCallback;
    lbl_80281970->fn.pfn08 = SFIO_vFreeSpaceCallback;
    lbl_80281970->fn.pfn0C = SFIO_vFreeEntryCallback;
    lbl_80281970->fn.pfnStartProbe = SFIO_vMountCallback;
    lbl_80281970->fn.pfnSelectDevice = SFIO_vUnMountCallback;
    lbl_80281970->fn.pfnMount = SFIO_vOpenCallback;
    lbl_80281970->fn.pfnOp19 = SFIO_vCloseCallback;
    lbl_80281970->fn.pfn20 = SFIO_vCreateCallback;
    lbl_80281970->fn.pfn24 = SFIO_vDeleteCallback;
    lbl_80281970->fn.pfn28 = SFIO_vDestroyCallback;
    lbl_80281970->fn.pfnRead = SFIO_vReadCallback;
    lbl_80281970->fn.pfnWrite = SFIO_vWriteCallback;
    lbl_80281970->fn.pfnSeek = SFIO_vSeekCallback;
    lbl_80281970->fn.pfnOp18 = fn_80122834;
    lbl_80281970->fn.pfn3C = fn_80122868;
    lbl_80281970->fn.pfnUpdate = SFIO_eProcessCallback;
    lbl_80281970->fn.pfn44 = SFIO_vGetFileCallback;
    return &lbl_80281970->fn;
}

// Looks for the save file: the first of the card's files named like pSearchName that the library
// takes as a save file name is kept in szFileName, with a result of 1.
void SFIO_vFindCallback(char* pSearchName, int eDevice) {
    char aszName[TIBEXT_MAX_FOUND][64];
    char* apName[TIBEXT_MAX_FOUND];
    s32 nFound;
    s32 nErr;
    int i;

    SFIO_vSetCurrentResult(0);
    for (i = 0; i < TIBEXT_MAX_FOUND; i++) {
        apName[i] = aszName[i];
    }
    nErr = MC_FindFiles(eDevice / 4, eDevice % 4, pSearchName, apName, TIBEXT_MAX_FOUND, &nFound);
    if (nErr != 0) {
        SFIO_vSetCurrentError(nErr);
        return;
    }
    for (i = 0; i < nFound; i++) {
        if (SFIOValidateFilename(apName[i])) {
            strcpy(lbl_80281970->szFileName, apName[i]);
            SFIO_vSetCurrentError(0);
            SFIO_vSetCurrentResult(1);
            return;
        }
    }
    SFIO_vSetCurrentError(0);
}

// The file library's error code for each card error, by -error: 0 for none, 17 for most.
// fake match: EA's table starts 8-aligned after the 9-byte "TibExt.c"; plain s32 data is only
// 4-aligned, and the cause is not known.
s32 lbl_80194758[46] __attribute__((aligned(8))) = {
    0,  17, 17, 3,  17, 17, 17, 17, 17, 17, 17, 17, 4,  17, 4,  17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 14, 3,  14, 17, 17, 17, 17, 17, 17, 17, 17, 17,
};

void SFIO_vSetCurrentError(s32 nCardError) {
    lbl_80281970->nError = lbl_80194758[-nCardError];
}

void SFIO_vSetCurrentResult(s32 n) {
    lbl_80281970->n48 = n;
}

// The card's free space.
void SFIO_vFreeSpaceCallback(int eDevice) {
    s32 nFree;

    SFIO_vSetCurrentError(MC_GetFreeSpace(eDevice / 4, eDevice % 4, &nFree));
    SFIO_vSetCurrentResult(nFree);
}

// The card's free directory entries.
void SFIO_vFreeEntryCallback(int eDevice) {
    s32 nFree;

    SFIO_vSetCurrentError(MC_GetNumFreeEntries(eDevice / 4, eDevice % 4, ".", &nFree));
    SFIO_vSetCurrentResult(nFree);
}

// Mounts the card.
void SFIO_vMountCallback(int eDevice) {
    s32 nErr = MC_MountCard(eDevice / 4, eDevice % 4);

    SFIO_vSetCurrentError(nErr);
    if (nErr == 0) {
        SFIO_vSetCurrentError(nErr);
    }
}

// Unmounts the card.
void SFIO_vUnMountCallback(int eDevice) {
    SFIO_vSetCurrentError(MC_UnmountCard(eDevice / 4, eDevice % 4));
}

// Opens the save file; the result is its file number.
void SFIO_vOpenCallback(const char* pDirName, char* pFileName, int eDevice, u32 uFlags) {
    s32 nFile;
    s32 nErr = MC_OpenFile(eDevice / 4, eDevice % 4, pFileName, uFlags, &nFile);

    SFIO_vSetCurrentError(nErr);
    if (nErr == 0) {
        SFIO_vSetCurrentResult(nFile);
    }
}

// Closes it.
void SFIO_vCloseCallback(int uHandle) {
    SFIO_vSetCurrentError(MC_CloseFile(uHandle));
}

// Creates it with uSize bytes.
void SFIO_vCreateCallback(char* pFileName, u32 uSize, int eDevice) {
    s32 nErr = MC_CreateDirectory(eDevice / 4, eDevice % 4, pFileName, uSize);

    SFIO_vSetCurrentError(nErr);
    if (nErr != 0) return;
}

void SFIO_vDeleteCallback(char* pDirName, char* pFileName, int eDevice) {
    s32 nErr = MC_DeleteFile(eDevice / 4, eDevice % 4, pFileName);

    SFIO_vSetCurrentError(nErr);
    if (nErr != 0) return;
}

// Deletes the save file.
void SFIO_vDestroyCallback(char* pDirName, int eDevice) {
    SFIO_vSetCurrentError(MC_DeleteDirectory(eDevice / 4, eDevice % 4, pDirName));
}

// Reads from the open file; the result is the bytes read.
void SFIO_vReadCallback(int uHandle, void* pBuffer, u32 uSize) {
    s32 nErr = MC_ReadFile(uHandle, pBuffer, uSize, 0);

    SFIO_vSetCurrentError(nErr);
    if (nErr != 0) {
        SFIO_vSetCurrentResult(0);
    } else {
        SFIO_vSetCurrentResult(uSize);
    }
}

// Writes to the open file; the result is the bytes written.
void SFIO_vWriteCallback(int uHandle, void* pBuffer, u32 uSize) {
    s32 nErr = MC_WriteFile(uHandle, pBuffer, uSize);

    SFIO_vSetCurrentError(nErr);
    if (nErr != 0) {
        SFIO_vSetCurrentResult(0);
    } else {
        SFIO_vSetCurrentResult(uSize);
    }
}

void SFIO_vSeekCallback(int uHandle, u32 uOffset, u32 uWhence) {
    s32 nErr = MC_SeekFile(uHandle, uOffset, uWhence == 0);

    SFIO_vSetCurrentError(nErr);
    if (nErr != 0) return;
}

void fn_80122834(int uHandle) {
    s32 nErr = MC_FlushFile();

    SFIO_vSetCurrentError(nErr);
    if (nErr != 0) return;
}

void fn_80122868(int uHandle, u32 uValue) {
    SFIO_vSetCurrentError(MC_SetAttributesOnFile());
}

// The library's update call: every card call here finishes at once, so the process is always
// done (2); the error and result are the last call's.
int SFIO_eProcessCallback(int* pProcess, int* pResult) {
    *pResult = lbl_80281970->n48;
    *pProcess = 2;
    return lbl_80281970->nError;
}

// The save file's name, as the probe found it.
void SFIO_vGetFileCallback(char* pFileName) {
    snprintf(pFileName, 64, "%s", lbl_80281970->szFileName);
}
