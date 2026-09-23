// ShaderRow19.c (our name): made by fold.py from 2 sweep files; not yet described.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

s32 fn_8007018C(s32, s32);
void fn_801247C0(void* arg0, void* arg1);
void fn_80070348();
void fn_80124814(u8* p);

void fn_801247C0(void* arg0, void* arg1) {
    if (arg1 != NULL) {
        (*(s32*)((u8*)(arg0) + 4)) = fn_8007018C((*(s32*)((u8*)(arg1) + 0)), (*(s32*)((u8*)(arg1) + 4)));
        return;
    }
    (*(s32*)((u8*)(arg0) + 4)) = fn_8007018C(0x32, 1);
}

void fn_80124814(u8* p) {
    fn_80070348(*(s32*)(p + 0x4));
}

// ---- end of sweep code ----
