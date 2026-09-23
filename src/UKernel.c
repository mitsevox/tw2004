// UKernel.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): not yet decompiled;
// the sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern void* lbl_80281DBC;
s32 fn_80048E44(void);
extern void* lbl_80281DB8;
void fn_80048FEC(void* arg0);
void* fn_80049018();
s32 fn_800490B8(void);
void fn_8004950C(void);
void fn_80049510(void);

s32 fn_80048E44(void) {
    return (s32)lbl_80281DBC;
}

void fn_80048FEC(void* arg0) {
    if ((void* ) lbl_80281DB8 != NULL) {
        (*(void**)((u8*)(lbl_80281DB8) + 0x128)) = arg0;
        lbl_80281DB8 = arg0;
    } else {
        lbl_80281DBC = arg0;
        lbl_80281DB8 = arg0;
    }
    (*(s32*)((u8*)(arg0) + 0x128)) = 0;
}

s32 fn_800490B8(void) {
    void* temp_r3;

    temp_r3 = fn_80049018();
    if (temp_r3 != NULL) {
        return (*(s32*)((u8*)(temp_r3) + 0x134));
    }
    return -2;
}

void fn_8004950C(void) {
}

void fn_80049510(void) {
}

// ---- end of sweep code ----
