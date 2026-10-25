// user.c: the save profiles' memory. At start-up it allocates the profiles of the five player slots
// (gpSaveData) and the one whose unlocks hold for every profile (gpPasswordUnlocks), and sets each
// slot up as a new profile; once the game's outer loop ends it frees both.

#include "game/save.h"

SaveProfile*  gpSaveData;
SaveProfile*  gpPasswordUnlocks;
SponsorSlot   gStartingSponsor;

// Once, at start-up: no sponsor signed yet for a new profile (gStartingSponsor); the save profiles,
// one per player slot (gpSaveData), and the unlocks that hold for every profile (gpPasswordUnlocks)
// allocated from the system heap; each slot set up as a new profile named "USER<n>"
// (SaveProfile_InitSlot); then those shared unlocks reset to their defaults and every cheat code
// cleared (PasswordManager_SetDefaults).
void UserManager_OpenOnce(void) {
    int i;

    gStartingSponsor.bSigned = 0;
    gStartingSponsor.nSponsor = 0;
    gpSaveData = Circular_MemAllocFunc(SAVE_NUM_SLOTS * sizeof(SaveProfile), MEMTYPE_SYSTEM, 32, "user.c",
                                       97);
    gpPasswordUnlocks = Circular_MemAllocFunc(sizeof(SaveProfile), MEMTYPE_SYSTEM, 32, "user.c", 98);
    for (i = 0; i < SAVE_NUM_SLOTS; i++) {
        SaveProfile_InitSlot(i);
    }
    PasswordManager_SetDefaults();
}

// Frees what UserManager_OpenOnce allocated, once the game's outer loop ends.
void UserManager_CloseOnce(void) {
    Circular_MemFreeFunc(gpSaveData);
    Circular_MemFreeFunc(gpPasswordUnlocks);
}
