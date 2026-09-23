// Small functions found by the sweep (sweep.py). Original file: LLFont.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

s32 fn_80009E70();

void fn_80011164(void* arg0);
void fn_80011164(void* arg0) {
    if ((u32) (*(u32*)((u8*)(arg0) + 0x470)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(arg0) + 0x470)));
        (*(u32*)((u8*)(arg0) + 0x470)) = 0U;
    }
}