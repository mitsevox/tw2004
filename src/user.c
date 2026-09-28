// user.c (EA's name, from its asserts): the save profiles' memory. At start-up it allocates the
// five player slots' profiles (gpSaveData) and the one whose unlocks hold for every profile
// (lbl_80281DF4, set by the cheat codes), and sets each profile up; at shutdown it frees both.

#include "game/save.h"

// This file's .sbss (game/save.h), in reverse address order as the compiler lays it out.
SaveProfile*  gpSaveData;
SaveProfile*  lbl_80281DF4;
SponsorSlot   lbl_80281DF0;

void fn_800563C4(void) {
    int i;

    lbl_80281DF0.bSigned = 0;
    lbl_80281DF0.nSponsor = 0;
    gpSaveData = StaticMem_Alloc(5 * sizeof(SaveProfile), 0, 32, "user.c", 97);
    lbl_80281DF4 = StaticMem_Alloc(sizeof(SaveProfile), 0, 32, "user.c", 98);
    for (i = 0; i < 5; i++) {
        fn_80057364(i);
    }
    fn_80056B8C();
}

void fn_80056454(void) {
    StaticMem_Free(gpSaveData);
    StaticMem_Free(lbl_80281DF4);
}
