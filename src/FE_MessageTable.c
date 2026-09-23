// FE_MessageTable.c (our name): the menus' message table. The menu UI (uiProcessInterface.c) sends
// its messages here while the front end runs (game type 3): fn_80079EA8 fills a table of 770
// handlers and fn_80079E6C calls the one for a message's number with its arguments and results.
// The handlers read and set what the menus show: golfer names, the session's setup, the save
// profile's stats and records, the Create-A-Player choices. TW06 has GetGolferName in
// apt_fe_gamemessages.c. Rounds have their own table (fn_800850E4).

#include "game.h"
#include "camera.h"
#include "game/frontend.h"
#include "frontend/fe.h"
#include "core/memcard.h"
#include "core/easb.h"

// Outside this file.
u32  fn_80013050(int nChan);            // the pad's device type (SIProbe)
void fn_800142A4(s8 n);                 // sets lbl_80281C98
void fn_80057438(SaveProfile* pProfile);
void fn_8008E354(void);                 // FEgolferanim.c
void fn_8008F80C(s32 p0, s32 p1);       // uiProcessInterface.c
void fn_8009CD80(s32 nPort, s32 nSlot); // MC_Gc.c
s32  fn_8009D390(s32 nPort, s32 nSlot); // MC_Gc.c
s32  fn_8009EB44(s32 nPort, s32 nSlot); // MC_Gc.c
s32  fn_800A1164(s32 nPort, s32 nSlot, char* pName, s32 n);     // MC.c
u8   fn_800A2604(int n);                // lbl_80281FF0: a 0x4C-byte record's first byte
char* fn_800A2614(int n);               // and the string at its 0xC
s32  fn_800A2628(void);                 // lbl_80281FF4
s32  fn_800A27F4(void);                 // lbl_80281FF8
u8   fn_800E22E4(int nSlot, int a, int b);      // GameRound.c
int  fn_800E2520(int nMode);            // GameRound.c
void fn_800E25E0(void);                 // GameRound.c
void GM_SetupCustomHoleSelection(void); // GameManager.c
int  GM_vGetAllTimeRecordsHeld(SaveProfile* pProfile);  // GameManager.c
void fn_800EAE44(int nId);              // GameMode5.c
s32  fn_800EAE6C(void);                 // GameMode5.c
char* fn_800ED280(int nId);             // GameMode5.c
char* fn_800ED2C8(int nId);             // GameMode5.c
void fn_800ED650(int i, s32* pA, s32* pB, s32* pC);     // GameMode5.c
int  fn_801020C0(void);                 // GameMode4.c
void fn_80102308(s32 n);                // GameMode4.c
void fn_8010D334(s32 v);                // CharSliders.c
void fn_8010F2FC(MsgArg* pArgs, MsgArg* pResult);
void fn_8010F3A4(MsgArg* pArgs, MsgArg* pResult);
void fn_8011DF90(MsgArg* pArgs, MsgArg* pResult);
void fn_80123FF8(void);
void fn_8012408C(s32 v);
s32  fn_8012411C(void);
void fn_80124138(s32 n);
s32  fn_80124174(void);
s32  fn_801241CC(void);
f32  fn_80012C30(char* sz);             // UFont.c

// This file.
void GetGolferName(int nGolfer, char* szName);
void fn_8007E458(int n, MsgArg* pArgs, MsgArg* pResult);

void fn_8007BBA0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800E2520((u8)pArgs[0].i);
}

void fn_8007BBD8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800E2520((u8)pArgs[0].i);
}

void fn_8007BC10(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800E2520((u8)pArgs[0].i);
}

void fn_8007BC48(MsgArg* pArgs, MsgArg* pResult) {
    fn_800E0B38((u8)pArgs[0].i);
    fn_800E25E0();
}

void fn_8007BC74(MsgArg* pArgs, MsgArg* pResult) {
    Session_SetNumPlayers((u8)pArgs[0].i);
    fn_800E25E0();
}

void fn_8007BCA0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007BCA4(MsgArg* pArgs, MsgArg* pResult) {
    fn_80079AD4();
}

void fn_8007BD18(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007BD1C(MsgArg* pArgs, MsgArg* pResult) {
    fn_800E14E0((u8)pArgs[0].i);
}

void fn_8007BD44(MsgArg* pArgs, MsgArg* pResult) {
    fn_800E1260(0);
    fn_800E1404((u8)pArgs[0].i - 1);
}

void fn_8007BD84(MsgArg* pArgs, MsgArg* pResult) {
    fn_800E1260((u8)pArgs[0].i);
}

void fn_8007BDFC(MsgArg* pArgs, MsgArg* pResult) {
    GetGolferName(pArgs[0].i, ((MsgString*)pArgs[1].p)->pStr);
}

void fn_8007BEEC(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007C12C(MsgArg* pArgs, MsgArg* pResult) {
    if (Game_GetMode() == 5) {
        lbl_80281ED4->b11703 = 1;
    }
    Session_SetGolfer(pArgs[1].i, pArgs[0].i);
}

void fn_8007C17C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gSession.nNumPlayers;
}

void fn_8007C218(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = Game_GetMode();
}

void fn_8007C248(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 1;
}

void fn_8007C254(MsgArg* pArgs, MsgArg* pResult) {
    u8 bOld;

    bOld = lbl_80281EE0->b86;
    lbl_80281EE0->b86 = pArgs[0].i;
    if (bOld != lbl_80281EE0->b86 && lbl_80281EE0->n0 == 3) {
        fn_8008E354();
    }
}

void fn_8007C330(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pResult->p)->pStr, fn_80077A80(pArgs[0].i)->szLast);
}

void fn_8007C488(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007C48C(MsgArg* pArgs, MsgArg* pResult) {
    fn_8008F80C(pArgs[0].i, (u8)pArgs[1].i);
}

void fn_8007C4B8(MsgArg* pArgs, MsgArg* pResult) {
    fn_8009CD10();
}

void fn_8007C4D8(MsgArg* pArgs, MsgArg* pResult) {
    fn_8009CD7C();
}

void fn_8007C698(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;
    s32 n;

    nError = fn_800A2100(pArgs[0].i, pArgs[1].i);
    n = 1;
    if (nError != 0) {
        n = nError;
    }
    pResult->i = n;
}

void fn_8007C784(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 30;
}

void fn_8007C790(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 1;
}

void fn_8007C79C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = pArgs[0].i + 30;
}

void fn_8007C7AC(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007C7B0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = (s8)fn_80077A80(pArgs[0].i)->bAvailable;
}

void fn_8007C94C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007C950(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8009F7E8(pArgs[0].i);
}

void fn_8007C988(MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D880C.n4 = pArgs[0].i;
    lbl_801D880C.n0 = 0;
}

// A string's width, scaled.
void fn_8007CD58(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = 512.0f * fn_80012C30(((MsgString*)pArgs[0].p)->pStr);
}

void fn_8007CDF0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007CDF4(MsgArg* pArgs, MsgArg* pResult) {
    fn_800EAE38(fn_800EAC94(pArgs[0].i));
}

void fn_8007CE1C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007CE20(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80077A80(pArgs[0].i)->nModelID;
}

void fn_8007CE58(MsgArg* pArgs, MsgArg* pResult) {
    fn_80102308(pArgs[0].i);
}

void fn_8007D25C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D260(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D264(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D268(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D26C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D270(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i != 0) {
        lbl_80281ED4->b11702 = 1;
    } else {
        lbl_80281ED4->b11702 = 0;
    }
}

void fn_8007D2D0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D380(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80077ACC()->n6C;
}

void fn_8007D3B4(MsgArg* pArgs, MsgArg* pResult) {
    fn_80057438(fn_80077ACC());
}

void fn_8007D3D8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801020C0();
}

void fn_8007D408(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D40C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D410(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D414(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D418(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D41C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D420(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D424(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D6D8(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D6DC(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D6E0(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[1].i != 0) {
        pResult->i = 0;
    } else {
        pResult->i = pArgs[0].i + 30;
    }
}

void fn_8007D76C(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile;

    pProfile = fn_80077ACC();
    pProfile->n6C = pArgs[1].i;
}

void fn_8007D7E4(MsgArg* pArgs, MsgArg* pResult) {
    FEMovie* pMovie;

    pMovie = fn_800770FC();
    pMovie->nKind = 2;
    fn_800A75B4();
}

void fn_8007D924(MsgArg* pArgs, MsgArg* pResult) {
    gSession.nSplitScreen = pArgs[0].i;
}

void fn_8007D938(MsgArg* pArgs, MsgArg* pResult) {
    fn_800E1404((u8)pArgs[0].i - 1);
}

void fn_8007D964(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D9D0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80281ED4->nSlot;
}

void fn_8007DAB0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].bActive;
}

void fn_8007DAD0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007DAD4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D87C0.n38;
}

void fn_8007DB28(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007DB2C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007DB30(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007DB34(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007DB38(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007E0BC(MsgArg* pArgs, MsgArg* pResult) {
    gSession.nPinSet = pArgs[0].i;
}

void fn_8007E0D0(MsgArg* pArgs, MsgArg* pResult) {
    fn_800142A4(pArgs[0].i);
}

// The mulligan rule: none in game mode 7, any number in mode 9, else the one picked.
void fn_8007E194(MsgArg* pArgs, MsgArg* pResult) {
    if (Game_GetMode() == 7) {
        gpGame->nMulligans = 0;
        return;
    }
    if (Game_GetMode() == 9) {
        gpGame->nMulligans = 1;
        return;
    }
    gpGame->nMulligans = pArgs[0].i;
}

void fn_8007E200(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007E288(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007E354(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007E51C(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(0, pArgs, pResult);
}

void fn_8007E548(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(1, pArgs, pResult);
}

void fn_8007E574(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(2, pArgs, pResult);
}

void fn_8007E5A0(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(3, pArgs, pResult);
}

void fn_8007E5CC(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(4, pArgs, pResult);
}

void fn_8007E5F8(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(5, pArgs, pResult);
}

void fn_8007E624(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(6, pArgs, pResult);
}

void fn_8007E650(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(7, pArgs, pResult);
}

void fn_8007E744(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007E748(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007E798(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007E85C(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;
    s32 n;

    nError = fn_800A1164(pArgs[0].i, pArgs[1].i, ((MsgString*)pArgs[2].p)->pStr, pArgs[3].i);
    n = 1;
    if (nError != 0) {
        n = nError;
    }
    pResult->i = n;
}

void fn_8007E8B4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 1;
}

void fn_8007E8C0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007E8C4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D7148.aLoaded[pArgs[0].i];
}

void fn_8007E8DC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D7148.b0F;
}

void fn_8007E8F0(MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D7148.b0F = pArgs[0].i;
}

void fn_8007E92C(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281EE0->b83 = pArgs[0].i;
}

void fn_8007E9A0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007E9A4(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007E9A8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D7148.b11;
}

void fn_8007EE7C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007EE80(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->profile.createdGolfer.nModelID = pArgs[1].i;
}

void fn_8007EF9C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007F088(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GM_vGetAllTimeRecordsHeld(&gpSaveData[pArgs[0].i]);
}

void fn_8007FCC0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D7148.nMode;
}

void fn_8007FCD4(MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D7148.nMode = pArgs[0].i;
}

void fn_8007FEAC(MsgArg* pArgs, MsgArg* pResult) {
    fn_800907AC(pArgs[0].i, ((MsgString*)pArgs[1].p)->pStr);
}

void fn_8007FED8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D7148.b18;
}

void fn_8007FF3C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80281EE0->b86;
}

// The saved replay's course, hole and golfer.
void fn_8008017C(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, lbl_80191990[gReplayData.nCourse]);
}

void fn_800801C0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gReplayData.nHole;
}

void fn_800801D4(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, gReplayData.player.golfer.szLast);
}

void fn_80080300(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80080358(MsgArg* pArgs, MsgArg* pResult) {
    fn_8009CD80(pArgs[0].i, pArgs[1].i);
    fn_8009CD7C();
}

void fn_800804D8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_800804E4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800E22E4(pArgs[0].i, pArgs[1].i, pArgs[2].i - 1);
}

void fn_800805C4(MsgArg* pArgs, MsgArg* pResult) {
    FEMovie* pMovie;

    pMovie = fn_800770FC();
    pMovie->nKind = FE_MOVIE_CREDITS;
    fn_800A75B4();
}

void fn_800805F0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800807D0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// A string's first character.
void fn_80080AD0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = ((MsgString*)pArgs[0].p)->pStr[0];
}

void fn_80080C60(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80281ED4->n4;
}

void fn_80080C74(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->n4 = pArgs[0].i;
}

void fn_80080C84(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80281ED4->n5;
}

void fn_80080C98(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->n5 = pArgs[0].i;
}

void fn_80080CA8(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[2].p = 0;
    *(s32*)pArgs[3].p = 0;
    *(s32*)pArgs[4].p = 0;
}

// Pick the saved custom round the holes come from.
void fn_800819FC(MsgArg* pArgs, MsgArg* pResult) {
    gpGame->b136 = pArgs[0].i;
    gpGame->nSaveSlot = pArgs[1].i;
    gpGame->nSaveCourse = pArgs[2].i;
    if (gpGame->b136 != 0) {
        GM_SetupCustomHoleSelection();
        fn_800E1434();
    }
}

void fn_80081BD4(MsgArg* pArgs, MsgArg* pResult) {
    fn_80077780();
}

void fn_80081BF4(MsgArg* pArgs, MsgArg* pResult) {
    fn_80077808(pArgs[0].i);
}

void fn_80081CF8(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80081F98(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->n1 = pArgs[0].i;
}

void fn_80081FA8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80281ED4->n1;
}

void fn_80082608(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80281ED4->n10620;
}

void fn_8008266C(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->n10620 = 0;
}

void fn_80082680(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800A218C(pArgs[0].i, pArgs[1].i) == 0;
}

void fn_800826C4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800A2194(pArgs[0].i, pArgs[1].i) == 0;
}

void fn_80082708(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8009EB44(pArgs[0].i, pArgs[1].i);
    *(s32*)pArgs[2].p = fn_800A2628();
}

void fn_80082758(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800A2604(pArgs[0].i);
}

void fn_80082790(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[1].p)->pStr, fn_800A2614(pArgs[0].i));
}

void fn_80082800(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[0].p = 2;
    *(s32*)pArgs[1].p = 1;
}

void fn_8008281C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 7;
}

void fn_80082928(MsgArg* pArgs, MsgArg* pResult) {
    gSession.nC = 2;
}

void fn_8008293C(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[1].p)->pStr, lbl_80191990[pArgs[0].i]);
}

void fn_80082978(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8008297C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80082980(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[1].p = pArgs[0].i;
    *(s32*)pArgs[2].p = 0;
}

void fn_800829D4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 2;
}

void fn_800829E0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 4;
}

void fn_800829EC(MsgArg* pArgs, MsgArg* pResult) {
    fn_80077968(pArgs[0].i);
}

void fn_80082A10(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
    strcpy(((MsgString*)pArgs[0].p)->pStr, "");
}

void fn_80082A44(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80082A48(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80082A4C(MsgArg* pArgs, MsgArg* pResult) {
}

// Empty a player slot: no profile in it, none loaded.
void fn_80082C74(MsgArg* pArgs, MsgArg* pResult) {
    gpSaveData[pArgs[0].i].bActive = 0;
    lbl_801D7148.aLoaded[pArgs[0].i] = 0;
}

void fn_80082CA4(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80082CA8(MsgArg* pArgs, MsgArg* pResult) {
    fn_800ED650(pArgs[0].i, pArgs[1].p, pArgs[2].p, pArgs[3].p);
}

void fn_80082CDC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = strlen(((MsgString*)pArgs[0].p)->pStr);
}

void fn_80082D98(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->n3 = pArgs[0].i;
}

void fn_80082DBC(MsgArg* pArgs, MsgArg* pResult) {
    if (fn_8009EE28(pArgs[0].i, pArgs[1].i) == MC_ERR_BADDATA) {
        pResult->i = 1;
    } else {
        pResult->i = 0;
    }
}

void fn_80082E10(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;
    s32 n;

    nError = fn_800A0A7C(pArgs[0].i, pArgs[1].i);
    n = 1;
    if (nError != 0) {
        n = nError;
    }
    pResult->i = n;
}

void fn_80083354(MsgArg* pArgs, MsgArg* pResult) {
}

// Play a golfer's bio movie, or the credits for -1.
void fn_80083358(MsgArg* pArgs, MsgArg* pResult) {
    FEMovie* pMovie;

    pMovie = fn_800770FC();
    pMovie->nKind = FE_MOVIE_BIO;
    if (pArgs[0].i == -1) {
        pMovie->nKind = FE_MOVIE_CREDITS;
    }
    fn_800A75B4();
}

void fn_800833A4(MsgArg* pArgs, MsgArg* pResult) {
    fn_800A75B4();
}

void fn_800833C4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// The pad in port pArgs[0] is a WaveBird (its SI device type).
void fn_80083430(MsgArg* pArgs, MsgArg* pResult) {
    if (fn_80013050(pArgs[0].i) == 0x8B100000) {
        pResult->i = 1;
    } else {
        pResult->i = 0;
    }
}

void fn_80083480(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D7148.b10;
}

void fn_80083494(MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D7148.b10 = pArgs[0].i;
}

void fn_800834A8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8009F728(pArgs[0].i);
}

void fn_800834DC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_800835B8(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800835BC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_800835C8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_80083890(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 150;
}

void fn_8008389C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838A0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838A4(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838A8(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838AC(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838B0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838B4(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838B8(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838BC(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838C0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838C4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = strcmp(((MsgString*)pArgs[0].p)->pStr, ((MsgString*)pArgs[1].p)->pStr);
}

void fn_80083904(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083908(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8008390C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083910(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083914(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083918(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8008391C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083920(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083924(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083928(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8008392C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083930(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083970(MsgArg* pArgs, MsgArg* pResult) {
}

// Three values out: 50, 50 and a level of 25..250 (the tenth of pArgs[3] plus one, times 25), the
// level going to the one of the three pArgs[2] picks.
void fn_80083974(MsgArg* pArgs, MsgArg* pResult) {
    s32 nWhich;
    s32 nLevel;

    nWhich = pArgs[2].i % 3;
    nLevel = (pArgs[3].i % 10 + 1) * 25;
    switch (nWhich) {
    case 0:
        *(s32*)pArgs[4].p = nLevel;
        *(s32*)pArgs[5].p = 50;
        *(s32*)pArgs[6].p = 50;
        return;
    case 1:
        *(s32*)pArgs[4].p = 50;
        *(s32*)pArgs[5].p = nLevel;
        *(s32*)pArgs[6].p = 50;
        return;
    case 2:
        *(s32*)pArgs[4].p = 50;
        *(s32*)pArgs[5].p = 50;
        *(s32*)pArgs[6].p = nLevel;
        return;
    }
}

void fn_80083A44(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083A48(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083BA4(MsgArg* pArgs, MsgArg* pResult) {
    fn_8010D334(pArgs[0].i);
}

void fn_80083E48(MsgArg* pArgs, MsgArg* pResult) {
    fn_80123FF8();
    fn_8012408C(0);
}

void fn_80083E70(MsgArg* pArgs, MsgArg* pResult) {
    fn_8012408C(6);
}

void fn_80083E94(MsgArg* pArgs, MsgArg* pResult) {
    fn_8012408C(8);
}

void fn_80083EB8(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083EBC(MsgArg* pArgs, MsgArg* pResult) {
    fn_8012408C(18);
}

void fn_80083F54(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_8008410C(MsgArg* pArgs, MsgArg* pResult) {
    s32 n;

    n = *(s32*)pArgs[0].p;
    pResult->i = n + fn_8012411C();
    fn_80124138(pResult->i);
}

void fn_80084158(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8008415C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80084160(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801241CC();
}

void fn_80084190(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80124174();
}

void fn_80084208(MsgArg* pArgs, MsgArg* pResult) {
    fn_8012408C(5);
}

// ---- the EA Sports Bio screens (EASportsBio.c does the work) ----

void fn_8008422C(MsgArg* pArgs, MsgArg* pResult) {
    fn_80125600(pArgs[0].i, pArgs[1].i);
}

void fn_80084258(MsgArg* pArgs, MsgArg* pResult) {
    fn_80125648(pArgs[0].i, pArgs[1].i, pArgs[2].i);
}

void fn_80084288(MsgArg* pArgs, MsgArg* pResult) {
    fn_8012566C(pArgs[0].i);
}

void fn_800842AC(MsgArg* pArgs, MsgArg* pResult) {
    fn_80125680(pArgs[0].i);
}

void fn_800842D0(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;

    nError = fn_80125354(pArgs[0].i, pArgs[1].i);
    lbl_80281ED4->n11704 = nError;
    if (nError != 0) {
        pResult->i = 0;
        return;
    }
    nError = fn_801253F0(pArgs[0].i, pArgs[1].i);
    lbl_80281ED4->n11704 = nError;
    pResult->i = nError == 0;
}

void fn_800844E0(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;

    nError = fn_80125434(pArgs[0].i, pArgs[1].i);
    lbl_80281ED4->n11704 = nError;
    if (nError != 0) {
        pResult->i = 0;
        return;
    }
    pResult->i = nError == 0;
}

void fn_80084544(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = EASBio_IsBioLoaded();
}

void fn_800845D4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80125528(pArgs[0].i, pArgs[1].i);
}

void fn_80084678(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;

    nError = fn_80125280(pArgs[0].i, pArgs[1].i);
    lbl_80281ED4->n11704 = nError;
    pResult->i = nError == 0;
}

void fn_800846C8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_800846D4(MsgArg* pArgs, MsgArg* pResult) {
    s32* pN;

    pN = pArgs[0].p;
    *pN = fn_80125928();
}

void fn_80084750(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80084754(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801254EC();
    pResult->i = fn_801254B8();
}

void fn_8008478C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = fn_80124BDC();
}

void fn_800847BC(MsgArg* pArgs, MsgArg* pResult) {
    fn_80084FF0(pArgs[0].i);
}

void fn_800848E4(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800848E8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800EAE6C();
}

void fn_80084918(MsgArg* pArgs, MsgArg* pResult) {
    fn_800EAE44(pArgs[0].i - 1);
}

void fn_80084940(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, fn_800ED280(pArgs[1].i - 1));
}

void fn_80084984(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, fn_800ED2C8(pArgs[1].i - 1));
}

// Passes the message on to one of three handlers, by pArgs[0].
void fn_80084B88(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 0:
        fn_8010F2FC(pArgs, pResult);
        return;
    case 1:
        fn_8010F3A4(pArgs, pResult);
        return;
    case 3:
        fn_8011DF90(pArgs, pResult);
        return;
    case 2:
        return;
    }
}

void fn_80084BE4(MsgArg* pArgs, MsgArg* pResult) {
}

// Copy a string, cut to eight characters and "..." when it is longer than 12.
void fn_80084D6C(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[1].p)->pStr, ((MsgString*)pArgs[0].p)->pStr);
    if (strlen(((MsgString*)pArgs[0].p)->pStr) > 12) {
        ((MsgString*)pArgs[1].p)->pStr[11] = '\0';
        ((MsgString*)pArgs[1].p)->pStr[10] = '.';
        ((MsgString*)pArgs[1].p)->pStr[9] = '.';
        ((MsgString*)pArgs[1].p)->pStr[8] = '.';
    }
}

// The same, to 28 characters and "..." when it is longer than 32.
void fn_80084DF4(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[1].p)->pStr, ((MsgString*)pArgs[0].p)->pStr);
    if (strlen(((MsgString*)pArgs[0].p)->pStr) > 32) {
        ((MsgString*)pArgs[1].p)->pStr[31] = '\0';
        ((MsgString*)pArgs[1].p)->pStr[30] = '.';
        ((MsgString*)pArgs[1].p)->pStr[29] = '.';
        ((MsgString*)pArgs[1].p)->pStr[28] = '.';
    }
}

void fn_80084E7C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80281ED4->n11704;
}

void fn_80084E90(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80125194(0, 0);
}

void fn_80084EC8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8009D390(pArgs[0].i, pArgs[1].i);
}

void fn_80084F04(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80084F08(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80084F0C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800A27F4();
}

void fn_80084F3C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80084F40(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801252D0(pArgs[0].i, pArgs[1].i) == 0;
}

void fn_80084F84(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = EASBio_GetCurrentRewardMessage();
}

void fn_80084FF0(int n) {
    lbl_80281FFC = n;
}
