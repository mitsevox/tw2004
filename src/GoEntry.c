// GoEntry.c (EA's name, from its asserts; also in EA's 2002 source tree): the game's outer loop.
// fn_800083A4 steps gSession.nGameType from start-up through the front end, the rounds and back,
// until the game is left or the console is reset.

#include "game.h"
#include "golfer.h"
#include "frontend/fe.h"
#include "game/frontend.h"

int  printf(const char* pFmt, ...);         // MSL
void VISetPostRetraceCallback(void (*pCallback)(u32 nRetrace));
void VISetBlack(int bBlack);
void VIFlush(void);
void VIWaitForRetrace(void);
int  PADRecalibrate(u32 uMask);
s32  OSResetSystem(s32, s32, s32);

// The systems the loop starts and steps (gomainloop.c and others; types from the calls here).
void fn_8006C6F0(void);
void fn_8006C720(void);
void fn_8006C770(void);
void GO_vInitFE(void);
void fn_8006CB2C(void);
void fn_8006CDC4(void);
void fn_8006CEFC(void);
void fn_8006CFC8(void);
void fn_8006D8E8(void);
void GO_vInitIG(void);
void fn_80008380(void);
void fn_801270F0(void);
void fn_800573E4(void);
void fn_80091EE4(void);
void fn_8007734C(void);
void fn_800A7644(int a);
u8*  fn_8010C718(void);                     // CharSliders.c
void fn_80091D84(void);
void fn_80091E1C(void);
void AI_TargetsClear(void);
void fn_800918A4(void);
s32  fn_800A7A34(u8 a, u8 b, u8 nListeners, int nUnused);
void fn_8001A81C(void);
void fn_8006F438(void);
void fn_800A500C(void);
void fn_80112D20(void);
void fn_8009A16C(void);
void fn_80112DA0(void);
void fn_800A5428(void);
void fn_8001C2E4(void);
void fn_8006F568(void);
void Luck_TightenOdds(void);
void fn_8009554C(void);
void fn_8001A73C(void);
u8   fn_8009A1EC(void);
void fn_80099ED8(void);
void fn_800070DC(void);                     // LLDisp_Gc.c
void fn_800072E8(void);
u8   fn_80007320(void);

u8   fn_80008820(void);
void fn_8000882C(void);

// The game's outer loop: each pass runs the current game type's step and moves nGameType on to
// the next, until type 13 (leave) or type 12 (reset the console).
void fn_800083A4(void) {
    u8 bQuit;

    bQuit = 0;
    fn_8006C720();
    fn_801270F0();
    do {
        switch (gSession.nGameType) {
        case 0:
            if (gSession.uFlags & 0x400) {
                fn_8006C6F0();
                if (gSession.uFlags & 5) {
                    gSession.nGameType = 4;
                } else {
                    gSession.nGameType = 10;
                }
            } else if (gSession.uFlags & 5) {
                fn_8006C6F0();
                gSession.nGameType = 4;
            } else {
                fn_8006CEFC();
                gSession.nGameType = 1;
            }
            if (gSession.uFlags & 0x20000) {
                fn_800573E4();
            }
            break;
        case 1:
            fn_8009005C("startup");
            fn_8006D8E8();
            fn_8006CFC8();
            if (gSession.nC == 0) {
                gSession.nGameType = 10;
            } else {
                gSession.nGameType = 12;
            }
            break;
        case 10:
            GO_vInitFE();
            gSession.nGameType = 3;
            break;
        case 3:
            fn_80091EE4();
            if (lbl_801D7148.b0F) {
                fn_8007734C();
            }
            fn_800A7644(lbl_801D7148.b0F);
            fn_8009005C("frontend");
            fn_8006D8E8();
            if (gSession.nC == 0 && gSession.nGameType != 13) {
                fn_8008B760();
                if (gSession.uFlags & 0x4000) {
                    fn_8010C718();
                }
                fn_8000BAE0();
                fn_80091D84();
            }
            fn_8006CB2C();
            if (gSession.nGameType == 13) {
                bQuit = 1;
            } else if (gSession.nC == 0) {
                gSession.nGameType = 4;
            } else {
                gSession.nGameType = 12;
            }
            break;
        case 4:
            gSession.n28++;
            GO_vInitIG();
            gSession.nGameType = 5;
            break;
        case 5:
            AI_TargetsClear();
            fn_800918A4();
            // one listener per view
            fn_800A7A34(Game_GetCourse() + 1, 1, gSession.nSplitScreen ? 2 : 1, 0);
            if (!fn_800E1734()) {
                fn_8000882C();
                fn_8001A81C();
            }
            fn_8006F438();
            fn_800A500C();
            fn_80112D20();
            gSession.nGameType = 6;
            break;
        case 6:
            if ((gSession.uFlags & 0x4000) && gSession.a8[0]) {
                fn_8009A16C();
            }
            fn_8006D8E8();
            if (gSession.nGameType == 13) {
                bQuit = 1;
            } else {
                gSession.nGameType = 7;
            }
            break;
        case 7:
            fn_80112DA0();
            fn_800A5428();
            fn_8001C2E4();
            fn_8006F568();
            Luck_TightenOdds();
            if (fn_80008820() && gSession.nC == 0) {
                gSession.nGameType = 5;
            } else {
                gSession.nGameType = 8;
            }
            if (gSession.nC == 0 || gSession.nC == 3) {
                if ((gSession.uFlags & 0x4000) && gSession.a8[0]) {
                    fn_80091E1C();
                } else {
                    fn_80091D84();
                }
            }
            fn_8009554C();
            fn_8001A73C();
            fn_800A7A98(1);
            break;
        case 8:
            fn_8006CDC4();
            if (gSession.nC == 0) {
                gSession.nGameType = 10;
            } else if (gSession.nC == 3) {
                gSession.nGameType = 4;
            } else {
                gSession.nGameType = 12;
            }
            if ((gSession.uFlags & 0x4000) && fn_8009A1EC()) {
                fn_80099ED8();
                gSession.nGameType = 4;
            }
            break;
        case 12:
            // Reset: wait for the display to finish, black the screen for two frames,
            // recalibrate all four pads and restart (nC 2: a hot reset to the menu).
            fn_800070DC();
            fn_800072E8();
            while (!fn_80007320()) {
            }
            GXDrawDone();
            VISetPostRetraceCallback(NULL);
            VISetBlack(1);
            VIFlush();
            VIWaitForRetrace();
            VISetBlack(1);
            VIFlush();
            VIWaitForRetrace();
            PADRecalibrate(0x80000000);
            PADRecalibrate(0x40000000);
            PADRecalibrate(0x20000000);
            PADRecalibrate(0x10000000);
            if (gSession.nC == 2) {
                OSResetSystem(1, 0, 1);
                return;
            }
            OSResetSystem(0, 1, 0);
            return;
        case 13:
            bQuit = 1;
            break;
        case 9:
            printf("%s (%ld), We hit an erroroneous mode!\n", "GoEntry.c", 814L);
            break;
        default:
            printf("%s (%ld), Unknown mode.\n", "GoEntry.c", 818L);
            break;
        }
        fn_80008380();
    } while (!bQuit);
    fn_8006C770();
}

// A hole load is asked for (lbl_802811E8[1], set by fn_8006F4B4): the loop loads another hole.
u8 fn_80008820(void) {
    return lbl_802811E8[1];
}

void fn_8000882C(void) {
}

int Game_GetCourse(void) {
    return gpGame->nCurCourse;
}
