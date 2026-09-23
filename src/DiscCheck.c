// DiscCheck.c (our name): which disc is in the drive. It reads "data/fend/d_layout.bin" (the names
// of the files on each disc), checks the disc's ID (DVDGetCurrentDiskID), asks for a disc change
// (DVDChangeDiskAsync) and looks course files ("data\<course>\<hole>\hole.hog") up in the list of
// the disc in the drive.

#include "game_types.h"
#include "game.h"

// ---- sweep code (not yet cleaned up) ----

s32 DVDGetCurrentDiskID();
extern u8 lbl_80213B80[];

// ---- end of sweep code ----

// Whether a file is on the disc in the drive: szName is looked up, ignoring case, in that disc's
// list of names.
u8 fn_801100AC(char* szName) {
    int i;

    if (lbl_802824D4 == 0) {
        for (i = 0; i < lbl_802824C8; i++) {
            if (stricmp(szName, lbl_8021BBB0[i]) == 0) {
                return 1;
            }
        }
    } else {
        for (i = 0; i < lbl_802824CC; i++) {
            if (stricmp(szName, lbl_80213BB0[i]) == 0) {
                return 1;
            }
        }
    }
    return 0;
}

void fn_80110178(u8 b) {
    lbl_802824D9 = b;
}

// Whether the current course is on the disc in the drive: its folder, or (when lbl_802824D9 is set)
// the current hole's file in it.
int fn_80110180(void) {
    char szPath[128];                   // the size is unknown (the frame allows up to 132)
    char* szCourse = fn_800E2680();
    char* szHole = GameManager_GetHoleName(fn_80015464());

    sprintf(szPath, "data\\%s", szCourse);
    if (fn_801100AC(szPath)) {
        return 1;
    }
    if (lbl_802824D9) {
        strcat(szPath, "\\");
        strcat(szPath, szHole);
        strcat(szPath, "\\hole.hog");
        if (fn_801100AC(szPath)) {
            return 1;
        }
    }
    return 0;
}

// ---- sweep code (not yet cleaned up) ----

s32 fn_8011027C(void) {
    s32 t0;
    t0 = DVDGetCurrentDiskID();
    return ((u32)((-*(u8*)(((u8*)t0) + 0x6)) | *(u8*)(((u8*)t0) + 0x6)) >> 31);
}

// ---- end of sweep code ----

// Callers compare the answer as an int (no clrlwi after the call).
int fn_80110450(void) {
    return lbl_802824D0;
}

void fn_80110458(u8 b) {
    lbl_802824D1 = b;
}

u8 fn_80110460(void) {
    return lbl_802824D1;
}

// ---- sweep code (not yet cleaned up) ----

u8* fn_801104A0(void) {
    return lbl_80213B80;
}

// ---- end of sweep code ----
