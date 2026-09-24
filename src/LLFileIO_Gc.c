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
u8 lbl_8019D54C[0x1000];                // the reader thread's stack
OSThread lbl_8019E550;                  // the reader thread (fn_80005D10)
s32 lbl_80281B84;                       // the last read's result: bytes read, or below 0 an error
// The open files' paths, by slot. The symbol is 0x2000 bytes; the code only reaches the first 0x1000.
char lbl_801A0350[32][0x80];
s32 lbl_80281B80;                       // how many files are open

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

// The reader thread (EA's name, from its lock: file_RequestDaemon). Each time the semaphore wakes it,
// it finishes the read it started (the callback, then the request back to its priority's free
// list), and starts the first queued read of the most urgent priority.
void* fn_80005D10(void* pArg) {
    int nPrio;                  // set whenever pReq is
    FileReq* pReq;
    FileReqPool* pPool;

    pReq = NULL;
    for (;;) {
        OSWaitSemaphore(&lbl_8019D540);
        fn_800B596C("file_RequestDaemon");
        if (pReq != NULL && lbl_80281B84 >= 0) {
            if (pReq->pfnDone != NULL) {
                if (pReq->b21) {
                    // port: EA passes two arguments more than the callback takes
                    ((void (*)(int, int, int, int))pReq->pfnDone)(pReq->nLen, 0, 0, 0);
                } else {
                    // a callback of the other kind: the buffer, the length, n1C and b20
                    ((void (*)(void*, s32, s32, u8))pReq->pfnDone)(pReq->pBuf, pReq->nLen, pReq->n1C,
                                                                  pReq->b20);
                }
                OSCheckActiveThreads();
                pReq->pfnDone = NULL;
                pReq->n1C = 0;
                pReq->b20 = 0;
                pReq->b21 = 0;
            }
            pReq->nFile = -1;
            pReq->pBuf = NULL;
            pReq->nLen = 0;
            pPool = &lbl_8019E880[nPrio];
            pReq->nOffset = 0;
            pReq->pPrev->pNext = pReq->pNext;
            pReq->pNext->pPrev = pReq->pPrev;
            pReq->pPrev = pReq;
            pReq->pNext = pReq;
            if (pPool != NULL) {
                pReq->pNext = pPool->pNext;
                pReq->pPrev = (FileReq*)pPool;
                if (pPool->pNext != NULL) {
                    pPool->pNext->pPrev = pReq;
                }
                pPool->pNext = pReq;
            }
            pReq = NULL;
            lbl_8019E868[nPrio].nCount--;
        } else {
            fn_800B7490();
        }
        if (pReq == NULL) {
            for (nPrio = 0; nPrio < 2; nPrio++) {
                if (lbl_8019E868[nPrio].nCount > 0) {
                    pReq = lbl_8019E868[nPrio].pNext;
                    break;
                }
            }
        }
        if (pReq != NULL) {
            fn_80005C90(pReq);
        }
        fn_800B5994("file_RequestDaemon");
    }
}

// Sets the file reads up: the drive, no files open, empty queues with eight free requests per
// priority, and the reader thread.
int fn_80005EC0(void) {
    FileQueue* pQueue;
    FileReqPool* pPool;
    int i;
    s32 j;

    DVDInit();
    OSInitSemaphore(&lbl_8019D540, 0);
    fn_80005AE8(lbl_8019EAD0, 0, sizeof(lbl_8019EAD0));
    for (j = 0; j < 32; j++) {
        lbl_8019EAD0[j].nEntry = -1;
        lbl_8019EAD0[j].szPath[0] = 0;
        lbl_801A0350[j][0] = 0;
    }
    lbl_80281B80 = i = 0;
    for (; i < 2; i++) {
        pQueue = &lbl_8019E868[i];
        pPool = &lbl_8019E880[i];
        fn_80005AE8(pQueue, 0, sizeof(FileQueue));
        fn_80005AE8(pPool, 0, sizeof(FileReqPool));
        pQueue->pNext = (FileReq*)pQueue;
        pPool->pNext = (FileReq*)pPool;
        pQueue->pPrev = (FileReq*)pQueue;
        pPool->pPrev = (FileReq*)pPool;
        // each request goes in after the one before it, the first after the pool itself (EA's
        // list insert tests the pointer it inserts after, even an array element's address)
        if (pPool != NULL) {
            pPool->aReq[0].pNext = pPool->pNext;
            pPool->aReq[0].pPrev = (FileReq*)pPool;
            if (pPool->pNext != NULL) {
                pPool->pNext->pPrev = &pPool->aReq[0];
            }
            pPool->pNext = &pPool->aReq[0];
        }
        for (j = 1; j < 8; j++) {
            if (&pPool->aReq[j - 1] != NULL) {
                pPool->aReq[j].pNext = pPool->aReq[j - 1].pNext;
                pPool->aReq[j].pPrev = &pPool->aReq[j - 1];
                if (pPool->aReq[j - 1].pNext != NULL) {
                    pPool->aReq[j - 1].pNext->pPrev = &pPool->aReq[j];
                }
                pPool->aReq[j - 1].pNext = &pPool->aReq[j];
            }
        }
    }
    OSCreateThread(&lbl_8019E550, fn_80005D10, NULL, lbl_8019D54C + sizeof(lbl_8019D54C),
                   sizeof(lbl_8019D54C), 15, 1);
    OSResumeThread(&lbl_8019E550);
    fn_800B5934();
    return 0;
}

void fn_800060DC(void) {
}

// Opens a file (EA's name, from its lock: File_Open): a file already open gets one more open, a
// new one takes a free slot. Returns the slot, or -1 when none is free.
int fn_800060E0(const char* szName) {
    char szPath[0x80];          // the size is unknown: the frame leaves 0x88 bytes for it
    int hFile;
    int i;
    s32 nEntry;

    hFile = -1;
    fn_80005BE8(szName, szPath);
    fn_800B596C("File_Open");
    for (i = 0; i < 32; i++) {
        if (lbl_8019EAD0[i].nEntry == -1) {
            hFile = i;
            break;
        }
    }
    if (hFile != -1) {
        do {
            nEntry = DVDConvertPathToEntrynum(szPath);
            if (nEntry < 0) {
                fn_800B7490();
            }
        } while (nEntry < 0);
        for (i = 0; i < 32; i++) {
            if (nEntry == lbl_8019EAD0[i].nEntry && strcmp(szPath, lbl_8019EAD0[i].szPath) == 0) {
                hFile = i;
                lbl_8019EAD0[i].nOpens++;
                break;
            }
        }
        if (i >= 32) {
            for (;;) {
                if (DVDFastOpen(nEntry, &lbl_8019EAD0[hFile].info)) {
                    lbl_8019EAD0[hFile].nEntry = nEntry;
                    lbl_8019EAD0[hFile].nOpens = 1;
                    strcpy(lbl_8019EAD0[hFile].szPath, szPath);
                    strcpy(lbl_801A0350[hFile], szPath);
                    break;
                }
                fn_800B7490();
            }
            lbl_80281B80++;
        }
    }
    fn_800B5994("File_Open");
    return hFile;
}

// Closes a file (EA's name, from its lock: File_Close): one open fewer, and at none the disc file is
// closed and its slot freed.
int fn_8000633C(int hFile) {
    FileQueue* pQueue;
    FileReq* pReq;
    int bClosed;
    int i;

    fn_800B596C("File_Close");
    // what walked the two queues here was compiled out (asserts, likely)
    for (i = 0; i < 2; i++) {
        pQueue = &lbl_8019E868[i];
        for (pReq = pQueue->pNext; pReq != (FileReq*)pQueue; pReq = pReq->pNext) {
        }
    }
    lbl_8019EAD0[hFile].nOpens--;
    if (lbl_8019EAD0[hFile].nOpens <= 0) {
        do {
            bClosed = DVDClose(&lbl_8019EAD0[hFile].info);
            if (!bClosed) {
                fn_800B7490();
            }
        } while (!bClosed);
        lbl_8019EAD0[hFile].nEntry = -1;
        lbl_8019EAD0[hFile].szPath[0] = 0;
        lbl_801A0350[hFile][0] = 0;
        lbl_80281B80--;
    }
    fn_800B5994("File_Close");
    return 0;
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
        // EA's list insert again, written out on the queue element each time
        if (&lbl_8019E868[nPrio] != NULL) {
            pReq->pNext = (FileReq*)&lbl_8019E868[nPrio];
            pReq->pPrev = lbl_8019E868[nPrio].pPrev;
            if (lbl_8019E868[nPrio].pPrev != NULL) {
                lbl_8019E868[nPrio].pPrev->pNext = pReq;
            }
            lbl_8019E868[nPrio].pPrev = pReq;
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

// Reads a whole file, waiting for the drive, into a new allocation aligned to nAlign; its size goes
// to *puSize when that is not NULL. NULL when no memory is free.
void* fn_800065C8(const char* szPath, u32* puSize, int nAlign) {
    DVDFileInfo info;
    s32 nEntry;
    u32 uSize;
    s32 nLen;
    void* pData;
    s32 nStatus;
    u8 bRetry;

    do {
        nEntry = DVDConvertPathToEntrynum(szPath);
        if (nEntry < 0) {
            fn_800B7490();
        }
    } while (nEntry < 0);
    if (nEntry < 0) {           // never: the loop above waits for the file
        return NULL;
    }
    for (;;) {
        if (DVDFastOpen(nEntry, &info)) {
            break;
        }
        fn_800B7490();
    }
    uSize = info.uLength;
    nLen = uSize;
    pData = fn_80009B34(uSize, 1, nAlign, "LLFileIO_Gc.c", 750);
    if (pData == NULL) {
        return NULL;
    }
    do {
        DVDReadAsyncPrio(&info, pData, uSize, 0, NULL, 2);
        bRetry = 0;
        do {
            // a DVDFileInfo begins with its command block
            nStatus = DVDGetCommandBlockStatus((DVDCommandBlock*)&info);
            if (nStatus != 0) {
                bRetry = fn_800B7490();
            }
        } while (nStatus != 0);
    } while (bRetry);
    do {
        nStatus = DVDClose(&info);
        if (!nStatus) {
            fn_800B7490();
        }
    } while (!nStatus);
    if (puSize != NULL) {
        *puSize = uSize;
    }
    return pData;
}

// A file's size in bytes.
u32 fn_800065B0(int hFile) {
    return lbl_8019EAD0[hFile].info.uLength;
}
