// event.c (TW06's golf/eventmanager/event.c): the game's event handlers. EVENT_Trigger calls the
// handler for an event number from the file's table (lbl_80188628): stepping through the clubs and
// shot kinds, the camera and commentary for each moment of a shot, and the lessons' checks
// (fn_80101AA8 can block an event). Most handlers pass the moment on to fn_80067710.

#include "game.h"
#include "terrain.h"
#include "sitdev.h"
#include "core/easb.h"

u32 lbl_80281E20;               // frames counted by event 26

void fn_8001C680(int nPlayer);
void fn_80033704(u16 nPatch, u16 nObject);
void fn_8003349C(f32 fPercentage, f32 fDuration, f32 fDelay);
void fn_80051C84(Ball* pBall, f32 fX, f32 fY);
void fn_8005A788(int nPlayer, int a);
void fn_8005C15C(int nPlayer, f32* pSpinY, f32* pSpinX);
void fn_8005C298(int nPlayer);
int  fn_8005CB48(int nPlayer);
void fn_800690C0(int nPlayer);
void fn_80069104(int nPlayer);
void fn_80069148(int nPlayer);
void fn_800691B0(int nPlayer);
void fn_80069A84(int nPlayer);
void fn_80069AFC(int nPlayer);
void fn_80069B74(int nPlayer);
void fn_80069BEC(int nPlayer);
int  fn_8006AA70(int nPlayer);
int  fn_8006AA84(int nPlayer);
void fn_8009A16C(void);
void fn_800A2FFC(int nPlayer, int nArg);
void fn_800A31E0(Ball* pBall, int nPlayer);
void fn_800A3348(void* pData, int nPlayer);
void fn_800A5620(void);
void fn_800A5CA4(u8 nPlayer);
void fn_800A5E94(u8 nPlayer);
void fn_800A5EC0(u8 nPlayer);
void fn_800A5F60(u8 nPlayer);
void fn_800A5FE8(u8 nPlayer);
void fn_800A6448(u8 nPlayer);
void fn_800A644C(u8 nPlayer);
void fn_800A6450(u8 nPlayer);
void fn_800A64A8(u8 nPlayer, int a);
void fn_800A6660(u8 nPlayer);
void fn_800A67E8(u8 nPlayer);
void fn_800A6854(u8 nPlayer);
void fn_800A6BA8(u8 nPlayer);
void fn_800C9FE0(void);
void fn_800DC498(int nPlayer);
void fn_800E5DE4(int n);
void fn_8010D3B8(int nPlayer);
void AI_SimAbort(void);
void AI_AimAtPin(int nPlayer);
void fn_80067220(int nPlayer);
void fn_8006752C(void);
void fn_80067550(int nPlayer);
void fn_80067554(int nPlayer);
void fn_80067558(int nPlayer);
void fn_8006755C(int nPlayer);
int  fn_80067560(void);

void fn_80065B44(void) {
    lbl_80281E20 = 0;
}

void fn_80065B50(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!fn_80101AA8(nPlayer, 41)) {
        fn_80067710(nPlayer, 2, 26);
    }
}

void fn_80065B98(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80065B9C(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!fn_80101AA8(nPlayer, 0)) {
        fn_80067710(nPlayer, 2, 1);
    }
}

void fn_80065BE4(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!fn_80101AA8(nPlayer, 1)) {
        fn_80067710(nPlayer, 2, 12);
    }
    fn_800A5620();
}

void fn_80065C30(int nPlayer, int nEvent, void* pData, int nArg) {
    int i = 0;

    do {
        Swing_ResetBoostAndSpin(i);
        i++;
    } while (i < 5);
}

void fn_80065C6C(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!fn_80101AA8(nPlayer, 3)) {
        fn_80067710(nPlayer, 2, 2);
    }
    fn_800C9FE0();
}

void fn_80065CB8(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_80101AA8(nPlayer, 4);
}

void fn_80065CDC(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!fn_80101AA8(nPlayer, 6)) {
        fn_80067710(nPlayer, 7, 3);
    }
}

void fn_80065D24(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!fn_80101AA8(nPlayer, 7)) {
        fn_80067710(nPlayer, 2, 25);
    }
}

void fn_80065D6C(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80065D70(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!fn_80101AA8(nPlayer, 21)) {
        fn_80067710(nPlayer, 2, 4);
    }
}

void fn_80065DB8(int nPlayer, int nEvent, void* pData, int nArg) {
    if (nArg == 1) {
        if (Game_GetMode() == 26 && (gSession.uFlags & 0x4000)) {
            fn_8009A16C();
        }
        gpGame->pfn260(nPlayer);
        fn_80101AA8(nPlayer, 10);
        fn_800A31E0(pData, nPlayer);
        fn_800BB1A8(&gPlayers[nPlayer].ball);
        fn_80067710(nPlayer, 2, 5);
        fn_8010D3B8(nPlayer);
    }
}

void fn_80065E70(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_80067710(nPlayer, 2, 6);
}

void fn_80065E98(int nPlayer, int nEvent, void* pData, int nArg) {
}

// Event 13: the next club down (from the longest, round to the putter) that suits the shot kind.
// The putter is kept only on the green or when the ball is close to the pin.
void fn_80065E9C(int nPlayer, int nEvent, void* pData, int nArg) {
    int nOldClub;
    int nTries;

    if (fn_80101AA8(nPlayer, 13) || Game_GetMode() == 22 || Game_GetMode() == 26) return;
    nOldClub = gPlayers[nPlayer].nClub;
    if (nOldClub == CLUB_PUTTER_e) return;
    nTries = 0;
    do {
        if (gPlayers[nPlayer].nClub > 0) {
            gPlayers[nPlayer].nClub--;
        } else {
            gPlayers[nPlayer].nClub = CLUB_PUTTER_e;
        }
        nTries++;
    } while (!Club_UsableForKind(nPlayer, gPlayers[nPlayer].nClub, gPlayers[nPlayer].nShotKind) &&
             nTries < CLUB_MAX_e);
    if (nTries == CLUB_MAX_e) {
        gPlayers[nPlayer].nClub = CLUB_PUTTER_e;
    }
    if (gPlayers[nPlayer].nClub == CLUB_PUTTER_e) {
        if (gPlayers[nPlayer].ball.nLie == LIE_GREEN_e || AI_GreenTowardPin(nPlayer, 1.5f)) {
            gPlayers[nPlayer].nShotKind = 0;
        } else {
            gPlayers[nPlayer].nClub = nOldClub;
        }
    }
    if (gPlayers[nPlayer].nShotKind != 2) {
        Shot_FitTargetToClub(nPlayer);
        Vec_Copy(gPlayers[nPlayer].vTarget, gPlayers[nPlayer].vTarget2);
    }
    gPlayers[nPlayer].fPower = AI_PowerForTarget(nPlayer);
    fn_80062C38();
    fn_8006752C();
    fn_8001C680(nPlayer);
    gPlayers[nPlayer].nClubPerKind[gPlayers[nPlayer].nShotKind] = gPlayers[nPlayer].nClub;
    if (gSession.nSplitScreen) {
        fn_80062CB0(gPlayers[nPlayer].nC58, 1);
    }
}

// Event 14: the same, a club up.
void fn_80066058(int nPlayer, int nEvent, void* pData, int nArg) {
    int nOldClub;
    int nTries;

    if (fn_80101AA8(nPlayer, 13) || Game_GetMode() == 22 || Game_GetMode() == 26) return;
    nOldClub = gPlayers[nPlayer].nClub;
    if (nOldClub == CLUB_PUTTER_e) return;
    nTries = 0;
    do {
        if (gPlayers[nPlayer].nClub < CLUB_PUTTER_e) {
            gPlayers[nPlayer].nClub++;
        } else {
            gPlayers[nPlayer].nClub = 0;
        }
        nTries++;
    } while (!Club_UsableForKind(nPlayer, gPlayers[nPlayer].nClub, gPlayers[nPlayer].nShotKind) &&
             nTries < CLUB_MAX_e);
    if (nTries == CLUB_MAX_e) {
        gPlayers[nPlayer].nClub = CLUB_PUTTER_e;
    }
    if (gPlayers[nPlayer].nClub == CLUB_PUTTER_e) {
        if (gPlayers[nPlayer].ball.nLie == LIE_GREEN_e || AI_GreenTowardPin(nPlayer, 1.5f)) {
            gPlayers[nPlayer].nShotKind = 0;
        } else {
            gPlayers[nPlayer].nClub = nOldClub;
        }
    }
    if (gPlayers[nPlayer].nShotKind != 2) {
        Shot_FitTargetToClub(nPlayer);
        Vec_Copy(gPlayers[nPlayer].vTarget, gPlayers[nPlayer].vTarget2);
    }
    gPlayers[nPlayer].fPower = AI_PowerForTarget(nPlayer);
    fn_80062C38();
    fn_8006752C();
    fn_8001C680(nPlayer);
    gPlayers[nPlayer].nClubPerKind[gPlayers[nPlayer].nShotKind] = gPlayers[nPlayer].nClub;
    if (gSession.nSplitScreen) {
        fn_80062CB0(gPlayers[nPlayer].nC58, 1);
    }
}

// Event 15: the next shot kind (fn_80067220).
void fn_80066214(int nPlayer, int nEvent, void* pData, int nArg) {
    if (fn_80101AA8(nPlayer, 15) || Game_GetMode() == 26 || Game_GetMode() == 22) return;
    fn_80067220(nPlayer);
    if (gSession.nSplitScreen) {
        fn_80062CB0(gPlayers[nPlayer].nC58, 1);
    }
}

// Events 16 and 17: a higher or lower trajectory.
void fn_800662A0(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!fn_80101AA8(nPlayer, 16)) {
        if (gPlayers[nPlayer].nTrajectory != 2) {
            gPlayers[nPlayer].nTrajectory++;
            gPlayers[nPlayer].fPower = AI_PowerForTarget(nPlayer);
        }
        fn_80062C38();
    }
}

void fn_80066324(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!fn_80101AA8(nPlayer, 17)) {
        if (gPlayers[nPlayer].nTrajectory != 0) {
            gPlayers[nPlayer].nTrajectory--;
            gPlayers[nPlayer].fPower = AI_PowerForTarget(nPlayer);
        }
        fn_80062C38();
    }
}

void fn_800663A8(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!fn_80101AA8(nPlayer, 19)) {
        fn_80069104(nPlayer);
        fn_8009B970(gPlayers[nPlayer].nView[0]);
    }
}

void fn_80066400(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!fn_80101AA8(nPlayer, 18)) {
        fn_800690C0(nPlayer);
        fn_8009B970(gPlayers[nPlayer].nView[0]);
    }
}

void fn_80066458(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!fn_80101AA8(nPlayer, 20)) {
        fn_80069148(nPlayer);
    }
}

void fn_80066498(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!fn_80101AA8(nPlayer, 21)) {
        fn_800691B0(nPlayer);
    }
}

void fn_800664D8(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_80069BEC(nPlayer);
}

void fn_800664F8(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_80069B74(nPlayer);
}

void fn_80066518(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_80069A84(nPlayer);
}

void fn_80066538(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_80069AFC(nPlayer);
}

// Event 26, each frame: every 60th frame one call, every 10th another.
void fn_80066558(int nPlayer, int nEvent, void* pData, int nArg) {
    lbl_80281E20++;
    if (lbl_80281E20 % 60 == 0) {
        fn_80067710(nPlayer, 5, 19);
    } else if (lbl_80281E20 % 10 == 0) {
        fn_80067710(nPlayer, 5, 18);
    }
}

void fn_800665D4(int nPlayer, int nEvent, void* pData, int nArg) {
    if (nArg == 1 && !fn_80101AA8(nPlayer, 28)) {
        fn_80063CF0(fn_80017028(gPlayers[nPlayer].nView[0]), 1, nPlayer);
        fn_8006ACF8(nPlayer, 3);
        fn_80095744(gPlayers[nPlayer].pChar, 13);
    }
}

void fn_80066664(int nPlayer, int nEvent, void* pData, int nArg) {
    f32 fSpinY;
    f32 fSpinX;

    if (nArg == 1) {
        fSpinY = 0.0f;
        fSpinX = 0.0f;
        if (!fn_80101AA8(nPlayer, 29)) {
            fn_80067710(nPlayer, 2, 28);
        }
        fn_80063CF0(fn_80017028(gPlayers[nPlayer].nView[0]), 3, nPlayer);
        fn_8006ACF8(nPlayer, 2);
        if (Player_IsController8(nPlayer)) {
            fn_80067554(nPlayer);
        } else {
            fn_80067550(nPlayer);
        }
        fn_8005C15C(nPlayer, &fSpinY, &fSpinX);
        if (!gSession.bReplay) {
            fn_8006C2A8(nPlayer, fSpinX, fSpinY);
        }
        fn_80051C84(&gPlayers[nPlayer].ball, fSpinY, fSpinX);
    }
}

void fn_8006676C(int nPlayer, int nEvent, void* pData, int nArg) {
    if (nArg == 1) {
        fn_80063CF0(fn_80017028(gPlayers[nPlayer].nView[0]), 30, nPlayer);
    }
}

void fn_800667C0(int nPlayer, int nEvent, void* pData, int nArg) {
    if (nArg == 1) {
        fn_80063CF0(fn_80017028(gPlayers[nPlayer].nView[0]), 31, nPlayer);
        fn_8005C298(nPlayer);
        fn_8005A788(nPlayer, 0);
    }
}

void fn_80066828(int nPlayer, int nEvent, void* pData, int nArg) {
    if (nArg == 1) {
        fn_800A5E94(nPlayer);
        if (!fn_80101AA8(nPlayer, 32)) {
            fn_80067710(nPlayer, 2, 8);
        }
        if (Player_IsController8(nPlayer)) {
            fn_8006755C(nPlayer);
        } else {
            fn_80067558(nPlayer);
        }
    }
}

void fn_800668A8(int nPlayer, int nEvent, void* pData, int nArg) {
    if (nArg == 1) {
        fn_80063CF0(fn_80017028(gPlayers[nPlayer].nView[0]), 8, nPlayer);
        fn_8006ACF8(nPlayer, 4);
        fn_800A5EC0(nPlayer);
        fn_80067710(nPlayer, 2, 9);
    }
}

void fn_80066920(int nPlayer, int nEvent, void* pData, int nArg) {
    if (nArg == 1 && !fn_80101AA8(nPlayer, 34)) {
        fn_80063CF0(fn_80017028(gPlayers[nPlayer].nView[0]), 6, nPlayer);
        fn_80067710(nPlayer, 2, 8);
    }
}

void fn_800670A8(int nPlayer, int nEvent, void* pData, int nArg);

void fn_80066994(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_800670A8(nPlayer, nEvent, pData, nArg);
}

void fn_800669B4(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_800670A8(nPlayer, nEvent, pData, nArg);
}

void fn_800669D4(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_800670A8(nPlayer, nEvent, pData, nArg);
}

void fn_800669F4(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_800670A8(nPlayer, nEvent, pData, nArg);
}

void fn_80066A14(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_80067710(nPlayer, 2, 29);
}

// Event 39: the ball hit a world object; tell the game mode which.
void fn_80066A3C(int nPlayer, int nEvent, void* pData, int nArg) {
    if (gPlayers[nPlayer].ball.pHitActor != NULL) {
        gpGame->pfn268(nPlayer, gPlayers[nPlayer].ball.pHitActor->n140);
        fn_800A5FE8(nPlayer);
    }
}

void fn_80066A9C(int nPlayer, int nEvent, void* pData, int nArg) {
    int nA;
    int nB;
    int nAnim;

    if (Game_GetMode() != 11 || nArg == 1) {
        nA = fn_8006AA70(nPlayer);
        nB = fn_8006AA84(nPlayer);
        if (nA < 5 && nA >= 0) {
            nAnim = 1;
        } else {
            nAnim = -1;
        }
        if (nAnim >= 0) {
            fn_800957B0(gPlayers[nPlayer].pChar, nAnim);
        }
        if (nA == 0) {
            switch (nB) {
            case 0:
                fn_8003349C(0.3f, 3.0f, 0.0f);
                break;
            case 1:
                fn_8003349C(0.5f, 6.0f, 0.0f);
                break;
            case 2:
                fn_8003349C(0.8f, 8.0f, 0.0f);
                break;
            default:
                fn_8003349C(1.0f, 10.0f, 0.0f);
                break;
            }
        }
    }
}

void fn_80066BB8(int nPlayer, int nEvent, void* pData, int nArg) {
    if (nArg == 1) {
        fn_80067710(nPlayer, 2, 7);
    }
}

void fn_80066BE8(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_800DC498(nPlayer);
}

void fn_80066C08(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_800335F8(0);
}

void fn_80066C2C(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_800A644C(nPlayer);
    if (fn_80101AA8(nPlayer, 46)) return;   // the result is tested (clrlwi.) with nothing after it
}

void fn_80066C6C(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_800A6448(nPlayer);
    if (fn_80101AA8(nPlayer, 45)) return;   // as in fn_80066C2C
}

void fn_80066CAC(int nPlayer, int nEvent, void* pData, int nArg) {
    if (fn_8005CB48(nPlayer) > 0 && gPlayers[nPlayer].nClub >= 0 && gPlayers[nPlayer].nClub <= 5) {
        fn_800A6450(nPlayer);
    }
}

void fn_80066D0C(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_800A67E8(nPlayer);
}

void fn_80066D30(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_800A6854(nPlayer);
}

void fn_80066D54(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066D58(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066D5C(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066D60(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066D64(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066D68(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066D6C(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066D70(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066D74(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066D78(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_800A2FFC(nPlayer, nArg);
    if (nArg == 1) {
        fn_800A6BA8(nPlayer);
    }
}

void fn_80066DC4(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_80067710(nPlayer, 2, 20);
    if (gPlayers[nPlayer].ballBefore.nLie == LIE_INCUP_e) {
        fn_80063CF0(fn_80017028(gPlayers[nPlayer].nView[0]), 11, nPlayer);
    }
}

void fn_80066E28(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_800A64A8(nPlayer, 0);
    if (fn_80067560()) {
        fn_800E5DE4((u8)(fn_80067560() + 15));
    }
}

void fn_80066E6C(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_800A6660(nPlayer);
}

void fn_80066E90(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_800A64A8(nPlayer, 1);
}

void fn_80066EB8(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_800A6660(nPlayer);
}

void fn_80066EDC(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!gSession.a8[0]) {
        EASBio_IncrementGamesPlayed(1);
    }
    fn_80067710(nPlayer, 2, 13);
}

void fn_80066F30(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_80067710(nPlayer, 4, 22);
}

void fn_80066F58(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_80067710(nPlayer, 8, 23);
}

void fn_80066F80(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_80067710(nPlayer, 8, 24);
}

void fn_80066FA8(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066FAC(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066FB0(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066FB4(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_80035574();
}

void fn_80066FD4(int nPlayer, int nEvent, void* pData, int nArg) {
    if (nArg == 5) {
        fn_80067710(nPlayer, 2, 32);
    }
}

void fn_80067004(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_80067710(nPlayer, 9, 30);
}

void fn_8006702C(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!fn_80101AA8(nPlayer, 75)) {
        fn_80067710(nPlayer, 2, 31);
    }
}

// The handlers, by event number.
EventHandler lbl_80188628[76] = {
    fn_80065B9C, fn_80065BE4, fn_80065C30, fn_80065C6C, fn_80065CB8, fn_80066EDC, fn_80065CDC,
    fn_80065D24, fn_80065D6C, fn_80065D70, fn_80065DB8, fn_80065E70, fn_80065E98, fn_80065E9C,
    fn_80066058, fn_80066214, fn_800662A0, fn_80066324, fn_80066400, fn_800663A8, fn_80066458,
    fn_80066498, fn_800664F8, fn_800664D8, fn_80066518, fn_80066538, fn_80066558, fn_80066BB8,
    fn_800665D4, fn_80066664, fn_8006676C, fn_800667C0, fn_80066828, fn_800668A8, fn_80066920,
    fn_80066994, fn_800669B4, fn_800669D4, fn_800669F4, fn_80066A3C, fn_80065B98, fn_80065B50,
    fn_80066A9C, fn_80066BE8, fn_80066C08, fn_80066C6C, fn_80066C2C, fn_80066CAC, fn_80066D0C,
    fn_80066D30, fn_80066D54, fn_80066D58, fn_80066D5C, fn_80066D60, fn_80066D64, fn_80066D68,
    fn_80066D6C, fn_80066D70, fn_80066D74, fn_80066D78, fn_80066DC4, fn_80066E28, fn_80066E6C,
    fn_80066E90, fn_80066EB8, fn_80066F30, fn_80066F58, fn_80066F80, fn_80066FA8, fn_80066FAC,
    fn_80066FB0, fn_80066FB4, fn_80066FD4, fn_80066A14, fn_80067004, fn_8006702C,
};

void EVENT_Trigger(int nPlayer, int nEvent, void* pData, int b) {
    lbl_80188628[nEvent](nPlayer, nEvent, pData, b);
}

// Events 35..38 (the ball at rest, in the hole, ...): with nArg 1, the camera and commentary.
void fn_800670A8(int nPlayer, int nEvent, void* pData, int nArg) {
    if (nArg == 1) {
        fn_80063CF0(fn_80017028(gPlayers[nPlayer].nView[0]), 7, nPlayer);
        fn_800A5CA4(nPlayer);
        fn_800A3348(pData, nPlayer);
        fn_8006ACF8(nPlayer, 1);
        if (nEvent == 36 && gPlayers[nPlayer].ball.pHitObject != NULL) {
            fn_80033704(gPlayers[nPlayer].ball.pHitObject->nPatch,
                        gPlayers[nPlayer].ball.pHitObject->nObjList);
        }
        if (nEvent == 38) {
            gPlayers[nPlayer].b30D = 1;
            fn_800A5F60(nPlayer);
        }
        if (nEvent == 36) {
            gPlayers[nPlayer].b30C = 1;
        }
        if (nEvent == 37) {
            fn_80067710(nPlayer, 2, 27);
        }
        if (nEvent == 36 || nEvent == 37) {
            fn_80067710(nPlayer, 2, 16);
        } else if (nEvent == 38) {
            fn_80067710(nPlayer, 2, 21);
        }
        gpGame->pfn23C(nPlayer);
    } else if (nEvent == 36) {
        AI_SimAbort();
    }
}

// The next shot kind, with its club and aim.
void fn_80067220(int nPlayer) {
    switch (gPlayers[nPlayer].nShotKind) {
    case 0:
        gPlayers[nPlayer].nShotKind = 3;
        gPlayers[nPlayer].nClub = gPlayers[nPlayer].nClubPerKind[gPlayers[nPlayer].nShotKind];
        break;
    case 1:
        gPlayers[nPlayer].nShotKind = 4;
        gPlayers[nPlayer].nClub = gPlayers[nPlayer].nClubPerKind[gPlayers[nPlayer].nShotKind];
        gPlayers[nPlayer].nShotKind2 = gPlayers[nPlayer].nShotKind;
        break;
    case 4:
        gPlayers[nPlayer].nShotKind = 3;
        gPlayers[nPlayer].nClub = gPlayers[nPlayer].nClubPerKind[gPlayers[nPlayer].nShotKind];
        gPlayers[nPlayer].nShotKind2 = gPlayers[nPlayer].nShotKind;
        break;
    case 3:
        if (gPlayers[nPlayer].ball.nLie == LIE_GREEN_e || AI_GreenTowardPin(nPlayer, 1.5f)) {
            AI_AimAtPin(nPlayer);
            gPlayers[nPlayer].fAim = Shot_AimAngle(nPlayer);
            gPlayers[nPlayer].nShotKind = 2;
            gPlayers[nPlayer].nClub = gPlayers[nPlayer].nClub =
                gPlayers[nPlayer].nClubPerKind[gPlayers[nPlayer].nShotKind];
            Shot_FitTargetToClub(nPlayer);
        } else {
            gPlayers[nPlayer].nShotKind = 5;
            gPlayers[nPlayer].nClub = gPlayers[nPlayer].nClubPerKind[gPlayers[nPlayer].nShotKind];
            gPlayers[nPlayer].nShotKind2 = gPlayers[nPlayer].nShotKind;
        }
        break;
    case 5:
        if (gPlayers[nPlayer].ball.nLie == LIE_GREEN_e || AI_GreenTowardPin(nPlayer, 1.5f)) {
            AI_AimAtPin(nPlayer);
            gPlayers[nPlayer].fAim = Shot_AimAngle(nPlayer);
            gPlayers[nPlayer].nShotKind = 0;
            gPlayers[nPlayer].nClub = CLUB_PUTTER_e;
            Shot_FitTargetToClub(nPlayer);
        } else if (Lie_AllowsFullSwing(nPlayer)) {
            AI_AimAtPin(nPlayer);
            gPlayers[nPlayer].fAim = Shot_AimAngle(nPlayer);
            gPlayers[nPlayer].nShotKind = 2;
            gPlayers[nPlayer].nClub = gPlayers[nPlayer].nClubPerKind[gPlayers[nPlayer].nShotKind];
            gPlayers[nPlayer].nShotKind2 = gPlayers[nPlayer].nShotKind;
            Shot_FitTargetToClub(nPlayer);
        } else {
            gPlayers[nPlayer].nShotKind = 1;
            gPlayers[nPlayer].nClub = gPlayers[nPlayer].nClubPerKind[gPlayers[nPlayer].nShotKind];
            gPlayers[nPlayer].nShotKind2 = gPlayers[nPlayer].nShotKind;
        }
        break;
    case 2:
        if (gPlayers[nPlayer].ball.nLie == LIE_GREEN_e) {
            AI_AimAtPin(nPlayer);
            gPlayers[nPlayer].fAim = Shot_AimAngle(nPlayer);
            gPlayers[nPlayer].nShotKind = 0;
            gPlayers[nPlayer].nClub = CLUB_PUTTER_e;
            Shot_FitTargetToClub(nPlayer);
        } else {
            gPlayers[nPlayer].nShotKind = 1;
            gPlayers[nPlayer].nClub = gPlayers[nPlayer].nClubPerKind[gPlayers[nPlayer].nShotKind];
            gPlayers[nPlayer].nShotKind2 = gPlayers[nPlayer].nShotKind;
        }
        break;
    default:
        gPlayers[nPlayer].nShotKind = 1;
        gPlayers[nPlayer].nClub = gPlayers[nPlayer].nClubPerKind[gPlayers[nPlayer].nShotKind];
        gPlayers[nPlayer].nShotKind2 = gPlayers[nPlayer].nShotKind;
        break;
    }
    fn_800689D4(nPlayer);
    gPlayers[nPlayer].fA60 = 0.0f;
    Shot_FitTargetToClub(nPlayer);
    Vec_Copy(gPlayers[nPlayer].vTarget, gPlayers[nPlayer].vTarget2);
    gPlayers[nPlayer].fPower = AI_PowerForTarget(nPlayer);
    fn_80062C38();
    fn_8001C680(nPlayer);
}

void fn_8006752C(void) {
    fn_800E58B4(58);
}

void fn_80067550(int nPlayer) {
}

void fn_80067554(int nPlayer) {
}

void fn_80067558(int nPlayer) {
}

void fn_8006755C(int nPlayer) {
}

// The lowest of bits 0..23 set in the effects' flags, 0 if none.
int fn_80067560(void) {
    int i;

    for (i = 0; i < 24; i++) {
        if (lbl_80202898.uFlags & (1 << i)) {
            return i;
        }
    }
    return 0;
}

void fn_80067608(void) {
    fn_80005AE8(lbl_802811B8, 0, sizeof(SitDevData));
    lbl_802811B8->pE8 = NULL;
    lbl_802811B8->n13C = 0;
    Course_RegisterLoader(5, fn_800BB6DC);
    fn_800BB0C8();
}

void fn_8006765C(void) {
    if (lbl_802811B8->pD0 != NULL) {
        fn_80009E70(lbl_802811B8->pD0);
    }
    fn_80009E70(lbl_802811B8->pCC);
    fn_80009E70(lbl_802811B8->pD4);
    lbl_80282208 = NULL;
}

void fn_800676AC(void) {
    lbl_80282210 = 0;
}

void fn_800676B8(void) {
    // port: SitDevFile.c defines the handler with the object's first word (the scripts) as its
    // parameter; UStream calls it with the object. Same address on the GameCube.
    UStream_RegisterHandler('sscr', (void (*)(UStreamObject*))SitDev_LoadScripts);
}

void fn_800676E8(void) {
    UStream_UnregisterHandler('sscr');
}
