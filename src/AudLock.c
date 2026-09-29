// AudLock.c (our name): the sound engine's two locks. A mutex guards the streams (movie audio takes
// it around each of its steps) and a one-count semaphore guards the stream read queue. Every lock
// call passes a string naming its caller ("Mov_Tick"); the retail build ignores it.

#include "game_types.h"
#include "platform.h"
#include "core/audtrack.h"

// Defined last-first: CodeWarrior lays .bss out in reverse order.
OSMutex lbl_801F6344;                   // the stream lock
OSSemaphore lbl_801F6338;               // the read-queue lock

// Sets both locks up: the mutex free, the read-queue semaphore at 1. LLFileIO_Gc.c calls it once
// its read thread is running (fn_80005EC0).
void AudLock_Init(void) {
    OSInitMutex(&lbl_801F6344);
    OSInitSemaphore(&lbl_801F6338, 1);
}

// Takes the mutex that the file system (its read thread, File_Open, File_Close, File_ReadAsyncEx),
// the streamed tracks and the movie sound share. szWho names the caller; it is not used.
void AudLock_Lock(const char* szWho) {
    OSLockMutex(&lbl_801F6344);
}

// Gives the mutex back (szWho is not used).
void AudLock_Unlock(const char* szWho) {
    OSUnlockMutex(&lbl_801F6344);
}

// Takes the stream read queue's semaphore (hlaudtrackstm.c), but only when it is free: when it is
// held (count 0) the caller goes on without it rather than blocking, and its
// AudLock_UnlockReadQueue then raises the count to 2. szWho is not used.
void AudLock_LockReadQueue(const char* szWho) {
    if (lbl_801F6338.nCount > 0) {
        OSWaitSemaphore(&lbl_801F6338);
    }
}

// Gives the read queue's semaphore back (szWho is not used).
void AudLock_UnlockReadQueue(const char* szWho) {
    OSSignalSemaphore(&lbl_801F6338);
}
