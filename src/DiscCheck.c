// DiscCheck.c (our name): which disc is in the drive. It reads "data/fend/d_layout.bin" (the names
// of the files on each disc), checks the disc's ID (DVDGetCurrentDiskID), asks for a disc change
// (DVDChangeDiskAsync) and looks course files ("data\<course>\<hole>\hole.hog") up in the list of
// the disc in the drive.

#include "game_types.h"
#include "engine.h"
#include "game.h"
#include "game/frontend.h"
#include "frontend/fe.h"

// The file's globals, defined last address first (CodeWarrior lays them out in reverse).
char lbl_8021BBB0[256][0x80];
char lbl_80213BB0[256][0x80];
DVDCommandBlock lbl_80213B80;
DVDDiskID lbl_80213B60;
u8 lbl_802824D9;
u8 lbl_802824D8;
s32 lbl_802824D4;
u8 lbl_802824D2;
u8 lbl_802824D1;
u8 lbl_802824D0;
s32 lbl_802824CC;
s32 lbl_802824C8;

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

// DVDChangeDiskAsync's callback: the disc change has finished (in game type 3 the menus are told
// through fn_8007BCA4). EA wrote it returning 0 (li r3, 0 before the blr) although the library's
// callback type returns nothing; the callers cast it.
int fn_80110234(s32 nResult, DVDCommandBlock* pBlock) {
    if (gSession.nGameType == 3) {
        fn_8007BCA4(NULL, NULL);
    }
    lbl_802824D0 = 1;
    return 0;
}

s32 fn_8011027C(void) {
    return DVDGetCurrentDiskID()->nDiskNumber != 0;
}

// Ask for the other disc and wait for it, keeping the screen and the game going. In game type 3
// the menus' state machine is paused instead and fn_80110390 asks later.
void fn_801102AC(void) {
    DVDDiskID id;
    DVDCommandBlock block;
    u8 nDisc;
    s32 nStatus;

    if (gSession.nGameType == 3) {
        fn_8008B760();
        fn_8008B978(1);
        lbl_802824D8 = 1;
        return;
    }
    lbl_802824D8 = 0;
    lbl_802824D0 = 0;
    if (lbl_802824D4 == 0) {
        nDisc = 1;
    } else {
        nDisc = 0;
    }
    memcpy(&id, DVDGetCurrentDiskID(), sizeof(DVDDiskID));
    id.nDiskNumber = nDisc;
    lbl_802824D4 = nDisc;
    DVDChangeDiskAsync(&block, &id, (DVDCBCallback)fn_80110234);
    fn_80006FE8();
    fn_800A7A98(1);
    do {
        nStatus = DVDGetDriveStatus();
        fn_80006EDC();
        if (gSession.nGameType == 6) {
            fn_800E3C0C(0);
        }
        fn_8006DBD4();
        fn_80006FE8();
        fn_800B7490();
    } while (nStatus != 0);
}

// The disc change fn_801102AC left for later: ask for the other disc once the drive is idle
// (not busy, waiting or with its cover open).
void fn_80110390(void) {
    u8 nDisc;
    s32 nStatus;

    if (lbl_802824D8 == 0) {
        return;
    }
    nStatus = DVDGetDriveStatus();
    if (nStatus == 6 || nStatus == 4 || nStatus == 1) {
        return;
    }
    lbl_802824D0 = 0;
    if (lbl_802824D4 == 0) {
        nDisc = 1;
    } else {
        nDisc = 0;
    }
    memcpy(&lbl_80213B60, DVDGetCurrentDiskID(), sizeof(DVDDiskID));
    lbl_802824D4 = nDisc;
    lbl_80213B60.nDiskNumber = nDisc;
    fn_800A7A98(1);
    DVDChangeDiskAsync(&lbl_80213B80, &lbl_80213B60, (DVDCBCallback)fn_80110234);
    DVDGetDriveStatus();
    lbl_802824D8 = 0;
}

// Callers compare the answer as an int (no clrlwi after the call).
int fn_80110450(void) {
    return lbl_802824D0;
}

void fn_80110458(u8 b) {
    lbl_802824D1 = b;
}

int fn_80110460(void) {
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

DVDCommandBlock* fn_801104A0(void) {
    return &lbl_80213B80;
}

// ---- end of sweep code ----
