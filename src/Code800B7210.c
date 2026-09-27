// Code800B7210.c (our name): split from DiscError.c at 0x800B7210. Its .data (the messages),
// .sdata (lbl_802814D8) and .sbss (lbl_802821A0) each start at the next 8-aligned address after
// DiscError.c's, 4 bytes past its end, as a new object's sections do; its globals are used only
// by fn_800B7210 and fn_800B7490, DiscError.c's only by the functions before them.

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "platform.h"
#include "game.h"
#include "discerror.h"
#include "core/startup.h"

void fn_8001437C(void);
void fn_800A3F38(u8 b, u8 b2);
void VIWaitForRetrace(void);
void fn_800B6924(void);
void fn_800B6FCC(int nLines);
void fn_800B70E8(int nLine, int nLines, int nSpan, const char* szText);

// .data
const char* lbl_80190F6C[15] = {
    "The Disc Cover is open.",
    "If you want to continue the game,",
    "please close the Disc Cover.",
    "Please insert the",
    "Tiger Woods PGA TOUR (R) 2004 Game Disc 1",
    "The Game Disc could not be read.",
    "Please read the",
    "Nintendo GameCube\x80 Instruction Booklet",
    "for more information.",
    "An error has occurred.",
    "Turn the power off and refer to the",
    "for further instructions.",
    "Unrecognized Dvd Status",
    "This is not the",
    "Tiger Woods PGA TOUR (R) 2004 Game Disc 2",
};

// .sdata (discerror.h)
const char** lbl_802814D8 = lbl_80190F6C;

// .sbss (discerror.h), in reverse address order as the compiler lays it out.
u32  lbl_802821A4;
u8   lbl_802821A0;

// Draw the message for the drive status nStatus.
void fn_800B7210(s32 nStatus) {
    char szText[64];

    VIWaitForRetrace();
    switch (nStatus) {
    case 5:  // DVD_STATE_COVER_OPEN
        fn_800B6FCC(6);
        fn_800B70E8(1, 3, 6, lbl_802814D8[0]);
        fn_800B70E8(2, 3, 6, lbl_802814D8[1]);
        fn_800B70E8(3, 3, 6, lbl_802814D8[2]);
        break;
    case 4:  // DVD_STATE_NO_DISK
        fn_800B6FCC(6);
        fn_800B70E8(1, 2, 6, lbl_802814D8[3]);
        if (fn_80110468() == 0) {
            fn_800B70E8(2, 2, 6, lbl_802814D8[4]);
        } else {
            fn_800B70E8(2, 2, 6, lbl_802814D8[14]);
        }
        break;
    case 6:  // DVD_STATE_WRONG_DISK
        fn_800B6FCC(6);
        fn_800B70E8(1, 2, 4, lbl_802814D8[3]);
        if (fn_80110468() == 0) {
            fn_800B70E8(2, 2, 4, lbl_802814D8[4]);
        } else {
            fn_800B70E8(2, 2, 4, lbl_802814D8[14]);
        }
        break;
    case 11:  // DVD_STATE_RETRY
        fn_800B6FCC(6);
        fn_800B70E8(1, 4, 6, lbl_802814D8[5]);
        fn_800B70E8(2, 4, 6, lbl_802814D8[6]);
        fn_800B70E8(3, 4, 6, lbl_802814D8[7]);
        fn_800B70E8(4, 4, 6, lbl_802814D8[8]);
        break;
    case -1:  // DVD_STATE_FATAL_ERROR
        fn_800B6FCC(6);
        fn_800B70E8(1, 4, 6, lbl_802814D8[9]);
        fn_800B70E8(2, 4, 6, lbl_802814D8[10]);
        fn_800B70E8(3, 4, 6, lbl_802814D8[7]);
        fn_800B70E8(4, 4, 6, lbl_802814D8[11]);
        break;
    default:
        sprintf(szText, "%s %d", lbl_802814D8[12], nStatus);
        fn_800B6FCC(6);
        fn_800B70E8(1, 1, 6, szText);
        break;
    }
    fn_800B6924();
}

// Check the drive (and the controllers, for a reset); while it reports a problem, redraw the
// message every frame, with the audio paused unless the drive reports no disc, the wrong disc or
// a retry. Returns 1 when the screen was shown.
u8 fn_800B7490(void) {
    u8 bShown;
    s32 nStatus;
    s32 nLast;

    bShown = 0;
    nStatus = 0;
    // 8 seconds: the bus clock (at 0x800000F8) / 4 is the timer's rate
    if (lbl_802821A0 && OSGetTick() - lbl_802821A4 > *(u32*)0x800000F8 / 4 / 1000 * 8000) {
        lbl_802821A0 = 0;
    }
    for (;;) {
        fn_80013400();
        nLast = nStatus;
        nStatus = DVDGetDriveStatus();
        if ((Controller_GetButtons(0) & 0x01000000) || (Controller_GetButtons(1) & 0x01000000) ||
            (Controller_GetButtons(2) & 0x01000000) || (Controller_GetButtons(3) & 0x01000000) || nStatus
                    == 6 ||
            nStatus == 10) {
            if (nStatus != -1) {
                fn_800066E4(1, 1, 1, 0);
            }
        } else if (lbl_802821A0) {
            if (nStatus != -1) {
                fn_800066E4(1, 1, 1, 0);
            }
        } else if (nStatus != -1) {
            fn_800066E4(1, 0, 1, 0);
        }
        if ((nLast == 5 && nStatus != 5) || nStatus == 6) {
            lbl_802821A4 = OSGetTick();
            lbl_802821A0 = 1;
        }
        if ((u32)nStatus <= 1 || nStatus == 7) {
            break;
        }
        if (bShown == 0) {
            if (nStatus != 4 && nStatus != 6 && nStatus != 11) {
                fn_800A3F38(1, 1);
            }
            fn_8001437C();
            bShown = 1;
        }
        fn_800B7210(nStatus);
    }
    if (bShown && gSession.nPaused == 0) {
        fn_800A3F38(0, 1);
    }
    return bShown;
}

void fn_800B7684(u8 b) {
    lbl_802814D0 = b;
}

void fn_800B768C(u8 b) {
    lbl_802814D1 = b;
}

void fn_800B7694(u8 b) {
    lbl_802814D2 = b;
}
