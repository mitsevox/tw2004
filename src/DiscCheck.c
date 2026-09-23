// DiscCheck.c (our name): which disc is in the drive. It reads "data/fend/d_layout.bin", checks
// the disc's ID (DVDGetCurrentDiskID), asks for a disc change (DVDChangeDiskAsync) and builds
// course file paths ("\hole.hog"). Not yet decompiled beyond the sweep code.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern u8 lbl_802824D9;
void fn_80110178(u8 v);
s32 DVDGetCurrentDiskID();
s32 fn_8011027C(void);
extern u8 lbl_802824D0;
extern u8 lbl_802824D1;
u8 fn_80110450(void);
void fn_80110458(u8 v);
u8 fn_80110460(void);
extern u8 lbl_80213B80[];
u8* fn_801104A0(void);

void fn_80110178(u8 v) {
    lbl_802824D9 = v;
}

s32 fn_8011027C(void) {
    s32 t0;
    t0 = DVDGetCurrentDiskID();
    return ((u32)((-*(u8*)(((u8*)t0) + 0x6)) | *(u8*)(((u8*)t0) + 0x6)) >> 31);
}

u8 fn_80110450(void) {
    return lbl_802824D0;
}

void fn_80110458(u8 v) {
    lbl_802824D1 = v;
}

u8 fn_80110460(void) {
    return lbl_802824D1;
}

u8* fn_801104A0(void) {
    return lbl_80213B80;
}
