// Small functions found by the sweep (sweep.py). Original file: CharSliders.c (EA's name, from its asserts; also in TW06; placed by the file map).

#include "game_types.h"

extern s32 lbl_80281764;
extern s32 lbl_8028176C;

s32 fn_8010D390(void);
s32 fn_8010D390(void) {
    if (((s32) lbl_80281764 != 5) && ((s32) lbl_8028176C > 0)) {
        return 1;
    }
    return 0;
}