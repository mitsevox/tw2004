// Small functions found by the sweep (sweep.py). Original file: uiProcessInterface.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

s32 fn_80092BC4();
s32 fn_800934F8();
s32 fn_800BA038();
s32 fn_80090400();
extern void* lbl_80281F1C;

void fn_80090664(void);
void fn_80090664(void) {
    if ((void* ) lbl_80281F1C != NULL) {
        fn_80090400((void* ) lbl_80281F1C);
    }
    fn_800934F8();
    fn_80092BC4();
    fn_800BA038();
}