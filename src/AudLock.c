// AudLock.c (our name): the sound engine's two locks. A mutex guards the streams (movie audio takes
// it around each of its steps) and a one-count semaphore guards the stream read queue. Every lock
// call passes a string naming its caller ("Mov_Tick"); the retail build ignores it.

#include "game_types.h"
#include "platform.h"
#include "core/audtrack.h"

// Defined last-first: CodeWarrior lays .bss out in reverse order.
OSMutex lbl_801F6344;                   // the stream lock
OSSemaphore lbl_801F6338;               // the read-queue lock

void fn_800B5934(void) {
    OSInitMutex(&lbl_801F6344);
    OSInitSemaphore(&lbl_801F6338, 1);
}

void fn_800B596C(const char* szWho) {
    OSLockMutex(&lbl_801F6344);
}

void fn_800B5994(const char* szWho) {
    OSUnlockMutex(&lbl_801F6344);
}

// Waits only while the semaphore is free: a count of 0 means it is already held, and the take is
// skipped rather than blocking.
void fn_800B59BC(const char* szWho) {
    if (lbl_801F6338.nCount > 0) {
        OSWaitSemaphore(&lbl_801F6338);
    }
}

void fn_800B59EC(const char* szWho) {
    OSSignalSemaphore(&lbl_801F6338);
}
