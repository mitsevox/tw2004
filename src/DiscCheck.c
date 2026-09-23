// DiscCheck.c (our name): which disc is in the drive. It reads "data/fend/d_layout.bin" (the names
// of the files on each disc), checks the disc's ID (DVDGetCurrentDiskID), asks for a disc change
// (DVDChangeDiskAsync) and looks course files ("data\<course>\<hole>\hole.hog") up in the list of
// the disc in the drive.

#include "game_types.h"
#include "game.h"

// ---- sweep code (not yet cleaned up) ----

extern u8 lbl_80213B80[];

// ---- end of sweep code ----

// Note which disc is in the drive and read the list of each disc's files from
// "data/fend/d_layout.bin": the two counts (the first byte of each of two words), then disc 1's
// names and disc 2's, 0x80 bytes each. Each name's last character is cut off.
void fn_8010FF9C(void) {
    u32 uSize;
    char* pData;
    char* pName;
    int i;

    lbl_802824D4 = DVDGetCurrentDiskID()->nDiskNumber;
    pData = fn_800065C8("data/fend/d_layout.bin", &uSize, 32);
    lbl_802824C8 = pData[0];
    lbl_802824CC = pData[4];
    pName = pData + 8;
    for (i = 0; i < lbl_802824C8; i++) {
        strcpy(lbl_8021BBB0[i], pName);
        pName += 0x80;
        lbl_8021BBB0[i][strlen(lbl_8021BBB0[i]) - 1] = '\0';
    }
    for (i = 0; i < lbl_802824CC; i++) {
        strcpy(lbl_80213BB0[i], pName);
        pName += 0x80;
        lbl_80213BB0[i][strlen(lbl_80213BB0[i]) - 1] = '\0';
    }
    lbl_802824D2 = 1;
}

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

s32 fn_8011027C(void) {
    return DVDGetCurrentDiskID()->nDiskNumber != 0;
}

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

// The disc in the drive: its disk number, or the one noted when the lists were read.
int fn_80110468(void) {
    if (lbl_802824D2 == 0) {
        return DVDGetCurrentDiskID()->nDiskNumber;
    }
    return lbl_802824D4;
}

// ---- sweep code (not yet cleaned up) ----

u8* fn_801104A0(void) {
    return lbl_80213B80;
}

// ---- end of sweep code ----
