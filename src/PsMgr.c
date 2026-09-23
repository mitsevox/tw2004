// PsMgr.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled.
#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void fn_800A2958(void);
void UStream_UnregisterHandler();
void fn_800A298C(void);
extern s32 lbl_802813F8;
void fn_800A2BA8(void);

void fn_800A2958(void) {
}

void fn_800A298C(void) {
    UStream_UnregisterHandler(1936095332);
}

void fn_800A2BA8(void) {
    *(s32*)(((u8*)lbl_802813F8) + 0x4) = (1 - *(s32*)(((u8*)lbl_802813F8) + 0x4));
}

// ---- end of sweep code ----
