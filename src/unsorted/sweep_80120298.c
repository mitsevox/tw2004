// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281908;
s32 fn_8000AFA0();
void fn_8000B058();

void fn_80120298(void);
void fn_801202D0(void);
void fn_80120298(void) {
    s32 t0;
    t0 = fn_8000AFA0(32, 32, 2, 16);
    *(s32*)(((u8*)lbl_80281908) + 0x368) = t0;
}

void fn_801202D0(void) {
    fn_8000B058(*(s32*)(((u8*)lbl_80281908) + 0x368));
}
