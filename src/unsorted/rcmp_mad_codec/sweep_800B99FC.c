// Small functions found by the sweep (sweep.py). Original file: rcmp_mad_codec.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

s32 fn_8000B4B8(void*);
u8 fn_8000B508();
s32 fn_80045D80(s32);
void fn_800B9A50(void* arg0);

void fn_800B99FC(void* arg0);
void fn_800B99FC(void* arg0) {
    if (fn_8000B508() == 0) {
        (*(s32*)((u8*)(arg0) + 4)) = fn_80045D80((*(s32*)((u8*)(arg0) + 0)));
        (*(void (**)(void*))((u8*)(arg0) + 8)) = fn_800B9A50;
        fn_8000B4B8(arg0);
    }
}