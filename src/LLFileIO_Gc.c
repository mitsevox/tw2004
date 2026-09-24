// LLFileIO_Gc.c (EA's name, from its asserts): the disc file reads, run from a queue by a reader
// thread. Only part of it is decompiled so far.
#include "engine.h"
#include "platform.h"
#include "core/startup.h"
#include "core/audtrack.h"

DiscFile lbl_8019EAD0[32];              // the open files
FileQueue lbl_8019E868[2];              // the queued reads, per priority
FileReqPool lbl_8019E880[2];            // the free requests, per priority
OSSemaphore lbl_8019D540;               // signalled when a read ends, or the queue gets its first read
s32 lbl_80281B84;                       // the last read's result: bytes read, or below 0 an error

void fn_80005BE8(const char* szSrc, char* szDst);
void fn_80005C48(s32 nResult, DVDFileInfo* pInfo);
void fn_800060DC(void);
int  fn_80006478(int hFile, void* pDst, u32 uLen, u32 uOffset, void (*pfnDone)(int nBytes, int nError),
                 u8 nPrio, s32 n1C, u8 b20, u8 b21);

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

int fn_80006444(int hFile, void* pDst, u32 uLen, u32 uOffset,
                void (*pfnDone)(int nBytes, int nError)) {
    return fn_80006478(hFile, pDst, uLen, uOffset, pfnDone, 1, 0, 0, 1);
}

// Queues a read (EA's name, from its lock: File_ReadAsyncEx): takes a free request of the
// priority, puts it at the end of that priority's queue and wakes the reader if it is the only
// read queued. Nothing is queued when none is free.
int fn_80006478(int hFile, void* pDst, u32 uLen, u32 uOffset, void (*pfnDone)(int nBytes, int nError),
                u8 nPrio, s32 n1C, u8 b20, u8 b21) {
    FileReqPool* pPool;
    FileQueue* pQueue;
    FileReq* pReq;

    fn_800B7490();
    fn_800B596C("File_ReadAsyncEx");
    pPool = &lbl_8019E880[nPrio];
    pReq = pPool->pNext;
    if (pReq != (FileReq*)pPool) {
        pReq->pPrev->pNext = pReq->pNext;
        pReq->pNext->pPrev = pReq->pPrev;
        pReq->pPrev = pReq;
        pReq->pNext = pReq;
        pQueue = &lbl_8019E868[nPrio];
        if (pQueue != NULL) {
            pReq->pNext = (FileReq*)pQueue;
            pReq->pPrev = pQueue->pPrev;
            if (pQueue->pPrev != NULL) {
                pQueue->pPrev->pNext = pReq;
            }
            pQueue->pPrev = pReq;
        }
        lbl_8019E868[nPrio].nCount++;
        pReq->nFile = hFile;
        pReq->pBuf = pDst;
        pReq->nOffset = uOffset;
        pReq->nLen = uLen;
        pReq->pfnDone = pfnDone;
        pReq->n1C = n1C;
        pReq->b20 = b20;
        pReq->b21 = b21;
        if (lbl_8019E868[0].nCount + lbl_8019E868[1].nCount == 1) {
            OSSignalSemaphore(&lbl_8019D540);
        }
    }
    fn_800B5994("File_ReadAsyncEx");
    return 0;
}

// A file's size in bytes.
u32 fn_800065B0(int hFile) {
    return lbl_8019EAD0[hFile].info.uLength;
}
