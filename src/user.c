// user.c (EA's name, from its asserts): the save profiles' memory. At start-up it allocates the
// five player slots' profiles (gpSaveData) and the one whose unlocks hold for every profile
// (lbl_80281DF4, set by the cheat codes), and sets each profile up; at shutdown it frees both.

#include "game/save.h"

void fn_800563C4(void) {
    int i;

    lbl_80281DF0.b = 0;
    lbl_80281DF0.n = 0;
    gpSaveData = fn_80009B34(5 * sizeof(SaveProfile), 0, 32, "user.c", 97);
    lbl_80281DF4 = fn_80009B34(sizeof(SaveProfile), 0, 32, "user.c", 98);
    for (i = 0; i < 5; i++) {
        fn_80057364(i);
    }
    fn_80056B8C();
}

void fn_80056454(void) {
    fn_80009E70(gpSaveData);
    fn_80009E70(lbl_80281DF4);
}
