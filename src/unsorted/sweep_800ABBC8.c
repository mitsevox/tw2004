// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801F18B8[];
void fn_80005AE8();
void fn_800AE00C();

s32 fn_800ABBC8(void);
s32 fn_800ABBC8(void) {
    fn_80005AE8((lbl_801F18B8 + 0x1C), 0, 224);
    fn_800AE00C(lbl_801F18B8, (lbl_801F18B8 + 0x1C), 8, 28);
    *(u8*)(lbl_801F18B8 + 0x18) = 0;
    return 1;
}
