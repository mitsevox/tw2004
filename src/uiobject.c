// uiobject.c (our name, after TW06's golf/ui core/istudio runtime/uiobject.c; UI_Obj_InitModule is
// a TW06 pair): the 3D objects the in-game UI draws (TW06: the confidence-boost rings and the
// "tappa spinna" UI). Not yet decompiled; the code below is the sweep's matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void* fn_8000B70C(s32, s32);
u32 fn_80048808(s32);
extern u32 lbl_802820D0;
void fn_800AE380(void);
extern s32 lbl_80280DF0;
void fn_80013E38();
void fn_800AF0A8(s32 p0);

void fn_800AE380(void) {
    if ((u32) lbl_802820D0 == 0U) {
        lbl_802820D0 = fn_80048808((*(s32*)((u8*)(fn_8000B70C(0x54454F20, 0x2713)) + 4)));
    }
}

void fn_800AF0A8(s32 p0) {
    fn_80013E38(*(s32*)((u8*)lbl_80280DF0), p0, lbl_80280DF0);
}

// ---- end of sweep code ----
