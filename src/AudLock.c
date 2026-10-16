// AudLock.c: the sound engine's two locks, a mutex that the file reads share with the streamed tracks
// and the movie sound, and a one-count semaphore over the stream read queue (hlaudtrackstm.c). Every
// lock call passes a label for its caller ("Mov_Tick", "File_Open"), which this build ignores.

#include "game_types.h"
#include "platform.h"
#include "core/audtrack.h"

OSMutex gAudLockMutex;                  // the file reads, the streamed tracks and the movie sound
OSSemaphore gAudLockReadQueueSem;       // the stream read queue

// Sets both locks up: the mutex free, the read-queue semaphore at 1. The file reads' set-up calls
// it once their reader thread is running.
void AudLock_Init(void) {
    OSInitMutex(&gAudLockMutex);
    OSInitSemaphore(&gAudLockReadQueueSem, 1);
}

// Takes the mutex: the file reads (opening, closing, queueing a read and the reader thread), the
// streamed tracks (Stm_Tick, Stm_Start...) and the movie sound (Mov_Tick...) each hold it while
// they work. A thread can take it again while it holds it: Stm_Tick queues its reads through
// File_ReadAsyncEx, which takes it too.
void AudLock_Lock(const char* szWho) {
    OSLockMutex(&gAudLockMutex);
}

void AudLock_Unlock(const char* szWho) {
    OSUnlockMutex(&gAudLockMutex);
}

// Takes the read queue's semaphore only when it is free; when it is held (count 0) the caller goes
// on without it rather than waiting. The DMA-done callbacks take it too (RemoveFromAudStreamQueue).
// A caller that went on without it still signals on its way out (AudLock_UnlockReadQueue), so the
// count can climb past 1.
void AudLock_LockReadQueue(const char* szWho) {
    if (gAudLockReadQueueSem.nCount > 0) {
        OSWaitSemaphore(&gAudLockReadQueueSem);
    }
}

void AudLock_UnlockReadQueue(const char* szWho) {
    OSSignalSemaphore(&gAudLockReadQueueSem);
}
