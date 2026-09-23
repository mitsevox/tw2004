// TibExt.c (EA's name, from its asserts; TW06): the glue between EA's Tiburon SDK libraries (the
// EA Sports Bio's EASB.c, the shared file library) and the game: memory, the real-time clock, and
// the memory-card callbacks (lbl_80281970) the shared file library calls. Each callback runs one
// card call on the device's port (eDevice / 4) and slot (eDevice % 4) and keeps its error and
// result for fn_8012288C, the library's update call.

#include "engine.h"
#include "game.h"
#include "core/easb.h"
#include "Common/SharedFileIO.h"
#include "core/memcard.h"

#define TIBEXT_MAX_FOUND 10     // save files one probe looks at

void fn_80122468(s32 nCardError);
void fn_80122488(s32 n);
void fn_80122330(char* pSearchName, int eDevice);
void fn_80122494(int eDevice);
void fn_801224E0(int eDevice);
void fn_80122530(int eDevice);
void fn_8012258C(int eDevice);
void fn_801225CC(const char* pDirName, char* pFileName, int eDevice, u32 uFlags);
void fn_80122630(int uHandle);
void fn_80122654(char* pFileName, u32 uSize, int eDevice);
void fn_801226AC(char* pDirName, char* pFileName, int eDevice);
void fn_80122700(char* pDirName, int eDevice);
void fn_80122744(int uHandle, void* pBuffer, u32 uSize);
void fn_801227A0(int uHandle, void* pBuffer, u32 uSize);
void fn_801227F8(int uHandle, u32 uOffset, u32 uWhence);
void fn_80122834(int uHandle);
void fn_80122868(int uHandle, u32 uValue);
int  fn_8012288C(int* pProcess, int* pResult);
void fn_801228AC(char* pFileName);

void* TibExtMemAlloc(u32 uHeapID, u32 uSize, u32 uAlign) {
    return fn_80009B34(uSize, fn_8000A0B4(), uAlign, "TibExt.c", 42);
}

void TibExtMemFree(u32 uHeapID, void* p, u32 uSize, u32 uAlign) {
    fn_80009E70(p);
}

// The library's assert handler: does nothing in the retail game.
void fn_8012214C(const char* pFile, int uLine, const char* pExpr, int unused) {
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

    fn_8011E020(&nMonth, &nDay, &nYear, &nHour, &nMinute, &nSecond, &nMsec);
    fn_800D2678(&uEpoch, 0, 0, 1970);
    fn_800D2678(&uToday, nMonth, nDay, nYear);
    return nMinute * 60 + nHour * 3600 + nSecond + (uToday - uEpoch - 1) * 86400;
}

SFIOFuncTable* fn_801221F0(void) {
    lbl_80281970->fn.pfnProbe = fn_80122330;
    lbl_80281970->fn.pfn08 = fn_80122494;
    lbl_80281970->fn.pfn0C = fn_801224E0;
    lbl_80281970->fn.pfnStartProbe = fn_80122530;
    lbl_80281970->fn.pfnSelectDevice = fn_8012258C;
    lbl_80281970->fn.pfnMount = fn_801225CC;
    lbl_80281970->fn.pfnOp19 = fn_80122630;
    lbl_80281970->fn.pfn20 = fn_80122654;
    lbl_80281970->fn.pfn24 = fn_801226AC;
    lbl_80281970->fn.pfn28 = fn_80122700;
    lbl_80281970->fn.pfnRead = fn_80122744;
    lbl_80281970->fn.pfnWrite = fn_801227A0;
    lbl_80281970->fn.pfnSeek = fn_801227F8;
    lbl_80281970->fn.pfnOp18 = fn_80122834;
    lbl_80281970->fn.pfn3C = fn_80122868;
    lbl_80281970->fn.pfnUpdate = fn_8012288C;
    lbl_80281970->fn.pfn44 = fn_801228AC;
    return &lbl_80281970->fn;
}

// Looks for the save file: the first of the card's files named like pSearchName that the library
// takes as a save file name is kept in szFileName, with a result of 1.
void fn_80122330(char* pSearchName, int eDevice) {
    char aszName[TIBEXT_MAX_FOUND][64];
    char* apName[TIBEXT_MAX_FOUND];
    s32 nFound;
    s32 nErr;
    int i;

    fn_80122488(0);
    for (i = 0; i < TIBEXT_MAX_FOUND; i++) {
        apName[i] = aszName[i];
    }
    nErr = fn_8009F0F0(eDevice / 4, eDevice % 4, pSearchName, apName, TIBEXT_MAX_FOUND, &nFound);
    if (nErr != 0) {
        fn_80122468(nErr);
        return;
    }
    for (i = 0; i < nFound; i++) {
        if (SFIOValidateFilename(apName[i])) {
            strcpy(lbl_80281970->szFileName, apName[i]);
            fn_80122468(0);
            fn_80122488(1);
            return;
        }
    }
    fn_80122468(0);
}

void fn_80122468(s32 nCardError) {
    lbl_80281970->nError = lbl_80194758[-nCardError];
}

void fn_80122488(s32 n) {
    lbl_80281970->n48 = n;
}

// The card's free space.
void fn_80122494(int eDevice) {
    s32 nFree;

    fn_80122468(fn_8009F36C(eDevice / 4, eDevice % 4, &nFree));
    fn_80122488(nFree);
}

// The card's free directory entries.
void fn_801224E0(int eDevice) {
    s32 nFree;

    fn_80122468(fn_8009F3A0(eDevice / 4, eDevice % 4, ".", &nFree));
    fn_80122488(nFree);
}

// Mounts the card.
void fn_80122530(int eDevice) {
    s32 nErr = fn_8009D74C(eDevice / 4, eDevice % 4);

    fn_80122468(nErr);
    if (nErr == 0) {
        fn_80122468(nErr);
    }
}

// Unmounts the card.
void fn_8012258C(int eDevice) {
    fn_80122468(fn_8009DBAC(eDevice / 4, eDevice % 4));
}

// Opens the save file; the result is its file number.
void fn_801225CC(const char* pDirName, char* pFileName, int eDevice, u32 uFlags) {
    s32 nFile;
    s32 nErr = fn_8009F3D4(eDevice / 4, eDevice % 4, pFileName, uFlags, &nFile);

    fn_80122468(nErr);
    if (nErr == 0) {
        fn_80122488(nFile);
    }
}

// Closes it.
void fn_80122630(int uHandle) {
    fn_80122468(fn_8009F488(uHandle));
}

// Creates it with uSize bytes.
void fn_80122654(char* pFileName, u32 uSize, int eDevice) {
    s32 nErr = fn_8009F514(eDevice / 4, eDevice % 4, pFileName, uSize);

    fn_80122468(nErr);
    if (nErr != 0) return;
}

void fn_801226AC(char* pDirName, char* pFileName, int eDevice) {
    s32 nErr = fn_8009E758(eDevice / 4, eDevice % 4, pFileName);

    fn_80122468(nErr);
    if (nErr != 0) return;
}

// Deletes the save file.
void fn_80122700(char* pDirName, int eDevice) {
    fn_80122468(fn_8009F5E4(eDevice / 4, eDevice % 4, pDirName));
}

// Reads from the open file; the result is the bytes read.
void fn_80122744(int uHandle, void* pBuffer, u32 uSize) {
    s32 nErr = fn_8009F208(uHandle, pBuffer, uSize);

    fn_80122468(nErr);
    if (nErr != 0) {
        fn_80122488(0);
    } else {
        fn_80122488(uSize);
    }
}

// Writes to the open file; the result is the bytes written.
void fn_801227A0(int uHandle, void* pBuffer, u32 uSize) {
    s32 nErr = fn_8009F258(uHandle, pBuffer, uSize);

    fn_80122468(nErr);
    if (nErr != 0) {
        fn_80122488(0);
    } else {
        fn_80122488(uSize);
    }
}

void fn_801227F8(int uHandle, u32 uOffset, u32 uWhence) {
    s32 nErr = fn_8009F2D8(uHandle, uOffset, uWhence == 0);

    fn_80122468(nErr);
    if (nErr != 0) return;
}

void fn_80122834(int uHandle) {
    s32 nErr = fn_8009F35C();

    fn_80122468(nErr);
    if (nErr != 0) return;
}

void fn_80122868(int uHandle, u32 uValue) {
    fn_80122468(fn_8009F364());
}

// The library's update call: every card call here finishes at once, so the process is always
// done (2); the error and result are the last call's.
int fn_8012288C(int* pProcess, int* pResult) {
    *pResult = lbl_80281970->n48;
    *pProcess = 2;
    return lbl_80281970->nError;
}

// The save file's name, as the probe found it.
void fn_801228AC(char* pFileName) {
    snprintf(pFileName, 64, "%s", lbl_80281970->szFileName);
}
