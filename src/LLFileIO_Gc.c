// LLFileIO_Gc.c: the game's disc file reads. Files are opened by path into handles; reads are queued
// at two priorities, and a reader thread hands them to the drive one at a time.
#include "engine.h"
#include "platform.h"
#include "core/startup.h"
#include "core/audtrack.h"

// A copy of each open file's path, by handle: written on open, cleared on close, never read. Only
// the first FILE_NUM_FILES rows are used.
char gDiscFilePaths[64][FILE_PATH_SIZE];
DiscFile gDiscFiles[FILE_NUM_FILES];                // the open files, by handle
FileReqPool gFileReqPools[FILE_NUM_PRIORITIES];     // the free requests, per priority
FileQueue gFileQueues[FILE_NUM_PRIORITIES];         // the queued reads, per priority
OSThread gFileDaemonThread;                         // the reader thread (file_RequestDaemon)
u8 gFileDaemonStack[0x1000];                        // the reader thread's stack
OSSemaphore gFileDaemonSem;     // signalled when a read ends, or when a read is queued with none queued
s32 gFileReadResult;            // the running read's result: 0 until it ends, then the bytes read, or
                                // below 0 an error
s32 gNumOpenFiles;              // how many handles hold an open file

// Copies a file path in the disc's form: letters upper case, backslashes as slashes.
static void File_FormatFilename(const char* strFilenameSrc, char* strFilenameDest) {
    while (*strFilenameSrc != 0) {
        if (*strFilenameSrc >= 'a' && *strFilenameSrc <= 'z') {
            *strFilenameDest = *strFilenameSrc - ('a' - 'A');
        } else if (*strFilenameSrc == '\\') {
            *strFilenameDest = '/';
        } else {
            *strFilenameDest = *strFilenameSrc;
        }
        strFilenameSrc++;
        strFilenameDest++;
    }
    *strFilenameDest = 0;
}

// The drive's callback when a read ends: drops the CPU cache over what was read, keeps the result
// and wakes the reader.
static void File_AsyncCallback(s32 nResult, DVDFileInfo* pInfo) {
    if (nResult > 0) {
        DCInvalidateRange(pInfo->pAddr, nResult);
    }
    gFileReadResult = nResult;
    OSSignalSemaphore(&gFileDaemonSem);
}

// Hands a read to the drive, trying until the drive takes it; each try first waits out any disc
// error (DiscError_WaitForDrive).
static void File_StartRead(FileReq* pReq) {
    gFileReadResult = 0;
    do {
        DiscError_WaitForDrive();
    } while (!DVDReadAsyncPrio(&gDiscFiles[pReq->nFile].info, pReq->pBuf, pReq->nLen, pReq->nOffset,
                               File_AsyncCallback, 2));
}

// The reader thread. Each time gFileDaemonSem wakes it: a read that ended without an error is
// finished (its callback called, the request back on its priority's free list); otherwise it waits
// out any disc error (DiscError_WaitForDrive). Then, holding no read, it takes the first queued read
// of the most urgent priority, and starts the read it holds (again, when the last try failed).
static void* file_RequestDaemon(void* pArg) {
    int nPrio;                  // set whenever pReq is
    FileReq* pReq;
    FileReqPool* pPool;

    pReq = NULL;
    for (;;) {
        OSWaitSemaphore(&gFileDaemonSem);
        AudLock_Lock("file_RequestDaemon");
        if (pReq != NULL && gFileReadResult >= 0) {
            if (pReq->pfnDone != NULL) {
                if (pReq->bLegacy) {
                    // a CallbackPtr: the length asked for and 0, then two zeros it does not take
                    ((void (*)(int, int, int, int))pReq->pfnDone)(pReq->nLen, 0, 0, 0);
                } else {
                    // the buffer, the length asked for, and the caller's user value and id
                    ((void (*)(void*, s32, u32, u8))pReq->pfnDone)(pReq->pBuf, pReq->nLen, pReq->uUser,
                                                                  pReq->nUid);
                }
                OSCheckActiveThreads();
                pReq->pfnDone = NULL;
                pReq->uUser = 0;
                pReq->nUid = 0;
                pReq->bLegacy = 0;
            }
            pReq->nFile = -1;
            pReq->pBuf = NULL;
            pReq->nLen = 0;
            pPool = &gFileReqPools[nPrio];
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
            gFileQueues[nPrio].nCount--;
        } else {
            DiscError_WaitForDrive();
        }
        if (pReq == NULL) {
            for (nPrio = 0; nPrio < FILE_NUM_PRIORITIES; nPrio++) {
                if (gFileQueues[nPrio].nCount > 0) {
                    pReq = gFileQueues[nPrio].pNext;
                    break;
                }
            }
        }
        if (pReq != NULL) {
            File_StartRead(pReq);
        }
        AudLock_Unlock("file_RequestDaemon");
    }
}

int File_InitOnce(void) {
    FileQueue* pQueue;
    FileReqPool* pPool;
    int i;
    s32 j;

    DVDInit();
    OSInitSemaphore(&gFileDaemonSem, 0);
    Mem_set(gDiscFiles, 0, sizeof(gDiscFiles));
    for (j = 0; j < FILE_NUM_FILES; j++) {
        gDiscFiles[j].nEntry = -1;
        gDiscFiles[j].szPath[0] = 0;
        gDiscFilePaths[j][0] = 0;
    }
    gNumOpenFiles = i = 0;
    for (; i < FILE_NUM_PRIORITIES; i++) {
        pQueue = &gFileQueues[i];
        pPool = &gFileReqPools[i];
        Mem_set(pQueue, 0, sizeof(FileQueue));
        Mem_set(pPool, 0, sizeof(FileReqPool));
        pQueue->pNext = (FileReq*)pQueue;
        pPool->pNext = (FileReq*)pPool;
        pQueue->pPrev = (FileReq*)pQueue;
        pPool->pPrev = (FileReq*)pPool;
        // each request goes in after the one before it, the first after the pool itself; the
        // insert tests the node it inserts after for NULL, even an array element's address
        if (pPool != NULL) {
            pPool->aReq[0].pNext = pPool->pNext;
            pPool->aReq[0].pPrev = (FileReq*)pPool;
            if (pPool->pNext != NULL) {
                pPool->pNext->pPrev = &pPool->aReq[0];
            }
            pPool->pNext = &pPool->aReq[0];
        }
        for (j = 1; j < FILE_NUM_REQUESTS; j++) {
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
    OSCreateThread(&gFileDaemonThread, file_RequestDaemon, NULL, gFileDaemonStack + sizeof(gFileDaemonStack),
                   sizeof(gFileDaemonStack), 15, OS_THREAD_ATTR_DETACH);
    OSResumeThread(&gFileDaemonThread);
    AudLock_Init();
    return 0;
}

void File_CloseOnce(void) {
}

int File_Open(const char* filename) {
    char formattedFilename[FILE_PATH_SIZE];
    int handle;
    int i;
    s32 nEntry;

    handle = -1;
    File_FormatFilename(filename, formattedFilename);
    AudLock_Lock("File_Open");
    for (i = 0; i < FILE_NUM_FILES; i++) {
        if (gDiscFiles[i].nEntry == -1) {
            handle = i;
            break;
        }
    }
    if (handle != -1) {
        do {
            nEntry = DVDConvertPathToEntrynum(formattedFilename);
            if (nEntry < 0) {
                DiscError_WaitForDrive();
            }
        } while (nEntry < 0);
        for (i = 0; i < FILE_NUM_FILES; i++) {
            if (nEntry == gDiscFiles[i].nEntry && strcmp(formattedFilename, gDiscFiles[i].szPath) == 0) {
                handle = i;
                gDiscFiles[i].nOpens++;
                break;
            }
        }
        if (i >= FILE_NUM_FILES) {
            for (;;) {
                if (DVDFastOpen(nEntry, &gDiscFiles[handle].info)) {
                    gDiscFiles[handle].nEntry = nEntry;
                    gDiscFiles[handle].nOpens = 1;
                    strcpy(gDiscFiles[handle].szPath, formattedFilename);
                    strcpy(gDiscFilePaths[handle], formattedFilename);
                    break;
                }
                DiscError_WaitForDrive();
            }
            gNumOpenFiles++;
        }
    }
    AudLock_Unlock("File_Open");
    return handle;
}

int File_Close(int fileID) {
    FileQueue* pQueue;
    FileReq* pReq;
    int closeStatus;
    int i;

    AudLock_Lock("File_Close");
    // walks both queues without doing anything with the reads in them
    for (i = 0; i < FILE_NUM_PRIORITIES; i++) {
        pQueue = &gFileQueues[i];
        for (pReq = pQueue->pNext; pReq != (FileReq*)pQueue; pReq = pReq->pNext) {
        }
    }
    gDiscFiles[fileID].nOpens--;
    if (gDiscFiles[fileID].nOpens <= 0) {
        do {
            closeStatus = DVDClose(&gDiscFiles[fileID].info);
            if (!closeStatus) {
                DiscError_WaitForDrive();
            }
        } while (!closeStatus);
        gDiscFiles[fileID].nEntry = -1;
        gDiscFiles[fileID].szPath[0] = 0;
        gDiscFilePaths[fileID][0] = 0;
        gNumOpenFiles--;
    }
    AudLock_Unlock("File_Close");
    return 0;
}

int File_ReadAsync(int fileID, void* destAdr, u32 bytesToRead, u32 offsetFromStart,
                   CallbackPtr callbackFuncPtr) {
    return File_ReadAsyncEx(fileID, destAdr, bytesToRead, offsetFromStart, callbackFuncPtr,
                            FILE_PRIORITY_LOW, 0, 0, 1);
}

// Takes a free request of the priority, puts it at the end of that priority's queue and wakes the
// reader when it is the only read queued. Nothing is queued when none is free.
int File_ReadAsyncEx(int fileID, void* destAdr, u32 bytesToRead, u32 offsetFromStart,
                     CallbackPtrEx callbackFuncPtr, u8 priority, u32 user, u8 uid, u8 legacy) {
    FileReqPool* pPool;
    FileReq* pReq;
    FileQueue* pQueue;

    DiscError_WaitForDrive();
    AudLock_Lock("File_ReadAsyncEx");
    pPool = &gFileReqPools[priority];
    pReq = pPool->pNext;
    if (pReq != (FileReq*)pPool) {
        pReq->pPrev->pNext = pReq->pNext;
        pReq->pNext->pPrev = pReq->pPrev;
        pReq->pPrev = pReq;
        pReq->pNext = pReq;
        // in at the queue's end; the insert tests the queue's address for NULL
        if (&gFileQueues[priority] != NULL) {
            pReq->pNext = (FileReq*)&gFileQueues[priority];
            pReq->pPrev = gFileQueues[priority].pPrev;
            if (gFileQueues[priority].pPrev != NULL) {
                gFileQueues[priority].pPrev->pNext = pReq;
            }
            gFileQueues[priority].pPrev = pReq;
        }
        pQueue = gFileQueues;
        (pQueue += priority)->nCount++;   // fake match: the queue pointer stepped inside the increment
        pReq->nFile = fileID;
        pReq->pBuf = destAdr;
        pReq->nOffset = offsetFromStart;
        pReq->nLen = bytesToRead;
        pReq->pfnDone = callbackFuncPtr;
        pReq->uUser = user;
        pReq->nUid = uid;
        pReq->bLegacy = legacy;
        if (gFileQueues[FILE_PRIORITY_HIGH].nCount + gFileQueues[FILE_PRIORITY_LOW].nCount == 1) {
            OSSignalSemaphore(&gFileDaemonSem);
        }
    }
    AudLock_Unlock("File_ReadAsyncEx");
    return 0;
}

u32 File_GetLength(int fileID) {
    return gDiscFiles[fileID].info.uLength;
}

void* File_ReadFileToHeap(const char* filename, u32* fileLength, int align) {
    DVDFileInfo info;
    u32 uSize;
    s32 fileLen;
    s32 nEntry;
    void* memLoc;
    s32 nStatus;
    u8 bRetry;

    do {
        nEntry = DVDConvertPathToEntrynum(filename);
        if (nEntry < 0) {
            DiscError_WaitForDrive();
        }
    } while (nEntry < 0);
    if (nEntry < 0) {           // unreachable: the loop above waits for the file
        return NULL;
    }
    for (;;) {
        if (DVDFastOpen(nEntry, &info)) {
            break;
        }
        DiscError_WaitForDrive();
    }
    fileLen = info.uLength;
    uSize = fileLen;
    memLoc = Circular_MemAllocFunc(fileLen, MEMTYPE_HIGH, align, "LLFileIO_Gc.c", 750);
    if (memLoc == NULL) {
        return NULL;
    }
    do {
        DVDReadAsyncPrio(&info, memLoc, uSize, 0, NULL, 2);
        bRetry = 0;
        do {
            // a DVDFileInfo begins with its command block
            nStatus = DVDGetCommandBlockStatus((DVDCommandBlock*)&info);
            if (nStatus == 0) {     // the read is done: no drive wait
                break;
            }
            bRetry = DiscError_WaitForDrive();
        } while (nStatus != 0);
    } while (bRetry);
    do {
        nStatus = DVDClose(&info);
        if (!nStatus) {
            DiscError_WaitForDrive();
        }
    } while (!nStatus);
    if (fileLength != NULL) {
        *fileLength = fileLen;
    }
    return memLoc;
}
