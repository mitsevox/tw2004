// LLFileIO_Gc.c (EA's name, from its asserts): the disc file reads, run from a queue by a reader
// thread. Only part of it is decompiled so far.
#include "engine.h"
#include "platform.h"
#include "core/startup.h"

DiscFile lbl_8019EAD0[32];              // the open files
OSSemaphore lbl_8019D540;               // signalled when a read ends
s32 lbl_80281B84;                       // the last read's result: bytes read, or below 0 an error

void fn_80005BE8(const char* szSrc, char* szDst);
void fn_80005C48(s32 nResult, DVDFileInfo* pInfo);
void fn_800060DC(void);

// Copies a file path in the disc's form: letters upper case, backslashes as slashes.
void fn_80005BE8(const char* szSrc, char* szDst) {
    while (*szSrc != 0) {
        if (*szSrc >= 'a' && *szSrc <= 'z') {
            *szDst = *szSrc - ('a' - 'A');
        } else if (*szSrc == '\\') {
            *szDst = '/';
        } else {
            *szDst = *szSrc;
        }
        szSrc++;
        szDst++;
    }
    *szDst = 0;
}

// The read's DVD callback: drops the CPU cache over what was read and wakes the reader.
void fn_80005C48(s32 nResult, DVDFileInfo* pInfo) {
    if (nResult > 0) {
        DCInvalidateRange(pInfo->pAddr, nResult);
    }
    lbl_80281B84 = nResult;
    OSSignalSemaphore(&lbl_8019D540);
}

// Starts a queued read, retrying (with the disc-error screen) until the drive takes it.
void fn_80005C90(FileReq* pReq) {
    lbl_80281B84 = 0;
    do {
        fn_800B7490();
    } while (!DVDReadAsyncPrio(&lbl_8019EAD0[pReq->nFile].info, pReq->pBuf, pReq->nLen, pReq->nOffset,
                               fn_80005C48, 2));
}

void fn_800060DC(void) {
}

// A file's size in bytes.
u32 fn_800065B0(int hFile) {
    return lbl_8019EAD0[hFile].info.uLength;
}
