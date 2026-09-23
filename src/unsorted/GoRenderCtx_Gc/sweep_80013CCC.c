// Small functions found by the sweep (sweep.py). Original file: GoRenderCtx_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

s32 fn_8000A0E8();
s32 fn_8000A714();
s32 fn_800BADF8();

void fn_80013CCC(u8* arg0);
void fn_80013CCC(u8* arg0) {
    if ((u8) (*(u8*)((u8*)(arg0) + 0x1DC)) != 0) {
        fn_8000A0E8((*(s32*)((u8*)(arg0) + 0x10)) + 0x44, arg0 + 0x11C);
        fn_8000A0E8(arg0 + 0xDC, arg0 + 0x19C);
    } else {
        fn_800BADF8((*(s32*)((u8*)(arg0) + 0x10)) + 0x44, arg0 + 0x1C, arg0 + 0x11C, 4);
        fn_800BADF8(arg0 + 0xDC, arg0 + 0x1C, arg0 + 0x19C, 4);
    }
    fn_8000A714(arg0 + 0x11C, arg0 + 0x15C);
}