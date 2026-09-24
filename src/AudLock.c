// AudLock.c (our name): made by fold.py from 2 sweep files; not yet described.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern u8 lbl_801F6338[];
extern u8 lbl_801F6344[];
void OSInitMutex();
void OSInitSemaphore();
void OSLockMutex();
void OSUnlockMutex();
void fn_800B5934(void);
void fn_800B596C(void);
void fn_800B5994(void);
void OSSignalSemaphore();
void fn_800B59EC(void);

void fn_800B5934(void) {
    OSInitMutex(lbl_801F6344);
    OSInitSemaphore(lbl_801F6338, 1);
}

void fn_800B596C(void) {
    OSLockMutex(lbl_801F6344);
}

void fn_800B5994(void) {
    OSUnlockMutex(lbl_801F6344);
}

void fn_800B59EC(void) {
    OSSignalSemaphore(lbl_801F6338);
}

// ---- end of sweep code ----
