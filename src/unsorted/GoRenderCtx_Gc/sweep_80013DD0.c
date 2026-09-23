// Small functions found by the sweep (sweep.py). Original file: GoRenderCtx_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

s32 fn_8000A0E8();
s32 fn_8000ADC0();

void fn_80013DD0(u8* arg0, u32 arg1);
void fn_80013DD0(u8* arg0, u32 arg1) {
    if (arg1 == 0U) {
        fn_8000ADC0(arg0 + 0x1C);
        (*(s8*)((u8*)(arg0) + 0x1DC)) = 1;
        return;
    }
    fn_8000A0E8(arg1, arg0 + 0x1C);
    (*(s8*)((u8*)(arg0) + 0x1DC)) = 0;
}