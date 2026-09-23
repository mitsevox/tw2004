// GameMode11.c (our name): game mode 11, the lessons (the code's "scenarios"). One player on hole 14
// of course 10; twelve lessons (lbl_802823FC, 1..12), each a shot from a set spot with a required
// shot kind, club and shape (lbl_80192DF8). The mode saves some of the player's options when it
// starts and puts them back when it ends. Golfer.c, Swing.c and skalib.c ask it what the lesson allows.

#include "golfer.h"
#include "game.h"
#include "engine.h"

// One lesson: where the ball is placed and what the shot must be (lessons 1..11; 12 is the end).
typedef struct Lesson {
    f32 vPos[4];                // 0x00  (-1, -1, -1): the ball stays on the tee
    s32 nShotKind;              // 0x10  8 = any
    s32 nClub;                  // 0x14  26 = any
    s32 nShape;                 // 0x18  7 = any
} Lesson;
extern Lesson lbl_80192DF8[11];

// Each lesson's demonstration animation: the second table from step 6 on (index 0 unused).
extern char* lbl_80192D98[12];
extern char* lbl_80192DC8[12];

// The lessons' message lists: 16 message ids per lesson (-1 = none); lbl_80282420 is the lesson's row.
extern s16 lbl_80192F2C[12 * 16];

// The tee positions follow the pins in the per-hole data (fn_8000C594).
typedef struct HoleTees {
    u8     unk0[0xB0];
    PinPos tee[4];                          // 0xB0  one per tee set
} HoleTees;

extern s32 lbl_802816D8;                    // the options' unkC, saved while the mode runs
extern u8  lbl_802823E0;
extern u8  lbl_802823E1;
extern u8  lbl_802823E2;
extern u8  lbl_802823E3;
extern u8  lbl_802823E4;
extern u8  lbl_802823E5;
extern u32 lbl_802823E8;                    // picks which message of a list is shown
extern s32 lbl_802823EC;                    // the wind option, saved while the mode runs
extern u8  lbl_802823F0;
extern s32 lbl_802823F4;
extern u8  lbl_802823F8;
extern s32 lbl_802823FC;                    // the current lesson, 1..12
extern s32 lbl_80282404;                    // player 0's controller, kept while the CPU demonstrates
extern u8  lbl_80282408;                    // the spin option, saved
extern u8  lbl_80282409;                    // the boost option, saved
extern u8  lbl_8028240A;                    // options unk84, saved
extern u8  lbl_8028240B;                    // options unk0[4], saved
extern s32 lbl_80282418;
extern s32 lbl_8028241C;
extern s32 lbl_80282420;                    // the lesson's row in lbl_80192F2C
extern s32 lbl_80282424;
extern s32 lbl_80282428;                    // the lesson's step

int   strcmp(const char* a, const char* b);
void  fn_800E14E0(int nCourse);
void  Session_SetGolfer(int nGolfer, int nPlayer);
void  fn_80055C1C(u8 b);
void  GM_FlyByMode_Init(void);
void  fn_80125854(int a);
void  fn_800A6EC8(void);
void  fn_800E5200(int a);
void  Shot_Prepare(int nPlayer, u8 bNotify);
void  AI_ChooseTarget(int nPlayer);
void  BreakLine_Start(int nView);
void  fn_8009B970(int nView);
void  fn_80058FA4(int nPlayer);
void  fn_80047B6C(u8* pBall, int nPlayer);
void  fn_80047BC0(u8* pBall, int nPlayer);
void  fn_800E3D38(int nPlayer, int a);

void fn_801000E8(void);
void fn_80100108(void);
void fn_80100128(void);
void fn_80100160(void);
void fn_80100230(void);
void fn_80100308(void);
void fn_80100508(void);
int  fn_80100798(int nList, int nCount);
void fn_80101F18(int nPlayer);
void fn_80101F40(u8 a, int b);
void fn_80101FC0(int a, int b);
void fn_80100A3C(int nPlayer);
void fn_80100C08(void);
u8   fn_80101C9C(int nPlayer, int a);
u8   fn_80101CC4(int a);
void fn_80101CD8(void);

// Mode 11 starts: one player, most of the round's rules off, a fixed random seed. The player's
// options that the lessons override are saved first.
void fn_800FFF34(void) {
    gpGame->pfn1C8 = fn_800FFF34;
    gpGame->pfn1CC = fn_80100230;
    gpGame->pfn1D8 = fn_80101C9C;
    gpGame->pfn1DC = fn_80101CC4;
    gpGame->pfn1E4 = fn_801000E8;
    gpGame->pfn220 = fn_80100C08;
    gpGame->pfn224 = fn_80100108;
    gpGame->pfn22C = fn_80100A3C;
    gpGame->pfn1EC = fn_80100160;
    gpGame->pfn1F4 = fn_80101CD8;
    gpGame->bShowYardage = 0;
    gpGame->bStrokeLimit = 0;
    gpGame->b275 = 0;
    gpGame->bGimmesAllowed = 0;
    gpGame->b27B = 0;
    gpGame->b27C = 0;
    gpGame->b27D = 0;
    gpGame->b27F = 0;
    gpGame->b280 = 0;
    gpGame->b281 = 0;
    gpGame->b284 = 0;
    gpGame->b285 = 0;
    gpGame->b286 = 0;
    gpGame->b287 = 0;
    gpGame->b28A = 0;
    gpGame->n290 = 1;
    gpGame->nC = 1;
    gpGame->n10 = 1;
    gpGame->b276 = 1;
    fn_80055C1C(1);
    lbl_8028240B = SESSION_OPTIONS->unk0[4];
    lbl_8028240A = SESSION_OPTIONS->unk84;
    lbl_80282409 = SESSION_OPTIONS->bBoostEnabled;
    lbl_80282408 = SESSION_OPTIONS->bSpinEnabled;
    SESSION_OPTIONS->unk0[4] = 4;
    SESSION_OPTIONS->unk84 = 0;
    SESSION_OPTIONS->bBoostEnabled = 1;
    SESSION_OPTIONS->bSpinEnabled = 1;
    fn_8000B1D4(0, 69);
}

// Hole start.
void fn_801000E8(void) {
    fn_80100128();
}

// The hole restarts.
void fn_80100108(void) {
    fn_80100128();
}

// Back to the first lesson.
void fn_80100128(void) {
    lbl_80282428 = 2;
    lbl_802823FC = 1;
    lbl_80282420 = 0;
    GM_FlyByMode_Init();
}

// Round setup: course 10, hole 14 only, one CPU-controlled player (golfer 1), no mulligans, and
// the options' unkC and wind saved and replaced.
void fn_80100160(void) {
    fn_800E14E0(10);
    fn_800E1260(0);
    gpGame->bHoleSelected[13] = 1;
    fn_800E1434();
    gSession.nTeeSet[0] = 0;
    gSession.nTeeSet[1] = 0;
    gSession.unk5B38 = 0;
    gpGame->holeOrder[Game_CurHoleIndex()] = 0;
    lbl_802816D8 = SESSION_OPTIONS->unkC;
    lbl_802823EC = SESSION_OPTIONS->nWind;
    SESSION_OPTIONS->unkC = 4;
    SESSION_OPTIONS->nWind = 0;
    Session_SetNumPlayers(1);
    Session_SetGolfer(1, 0);
    gPlayers[0].nController = CONTROLLER_CPU;
    gpGame->nMulligans = 0;
    lbl_802823E3 = 0;
    lbl_802823E4 = 0;
}

// The mode ends: the saved options go back.
void fn_80100230(void) {
    SESSION_OPTIONS->unkC = lbl_802816D8;
    SESSION_OPTIONS->nWind = lbl_802823EC;
    fn_80055C1C(0);
    SESSION_OPTIONS->unk0[4] = lbl_8028240B;
    SESSION_OPTIONS->unk84 = lbl_8028240A;
    SESSION_OPTIONS->bBoostEnabled = lbl_80282409;
    SESSION_OPTIONS->bSpinEnabled = lbl_80282408;
}

// Is a lesson running (mode 11)?
u8 fn_80100294(void) {
    return Game_GetMode() == 11;
}

void fn_801002C0(void) {
    if (fn_80100294()) {
        if (lbl_802823F0) {
            lbl_802823F0 = 0;
            lbl_80282428 = lbl_80282424;
        }
        fn_80100308();
    }
}

void fn_80100308(void) {
    if (lbl_802823E2) {
        lbl_802823E2 = 0;
        lbl_80282428 = lbl_80282424;
    }
}

// On to the next lesson: its row of messages and two counts.
void fn_80100328(void) {
    switch (++lbl_802823FC) {
    case 0:
        lbl_80282420 = 0x00;
        lbl_8028241C = 5;
        lbl_80282418 = 2;
        break;
    case 7:
        lbl_80282420 = 0x10;
        lbl_8028241C = 4;
        lbl_80282418 = 3;
        break;
    case 8:
        lbl_80282420 = 0x20;
        lbl_8028241C = 6;
        lbl_80282418 = 1;
        break;
    case 1:
        lbl_80282420 = 0x30;
        lbl_8028241C = 5;
        lbl_80282418 = 2;
        break;
    case 4:
        lbl_80282420 = 0x40;
        lbl_8028241C = 5;
        lbl_80282418 = 2;
        break;
    case 5:
        lbl_80282420 = 0x50;
        lbl_8028241C = 5;
        lbl_80282418 = 2;
        break;
    case 2:
        lbl_80282420 = 0x60;
        lbl_8028241C = 5;
        lbl_80282418 = 2;
        break;
    case 3:
        lbl_80282420 = 0x70;
        lbl_8028241C = 5;
        lbl_80282418 = 2;
        break;
    case 6:
        SESSION_OPTIONS->unk84 = 1;
        lbl_80282420 = 0x80;
        lbl_8028241C = 5;
        lbl_80282418 = 2;
        break;
    case 9:
        SESSION_OPTIONS->unk84 = 0;
        lbl_80282420 = 0x90;
        lbl_8028241C = 5;
        lbl_80282418 = 2;
        break;
    case 10:
        lbl_80282420 = 0xA0;
        lbl_8028241C = 5;
        lbl_80282418 = 2;
        lbl_802823E5 = 1;
        break;
    case 11:
        lbl_80282420 = 0xB0;
        fn_80101F40(0, 0);
        fn_800E5200(-1);
        fn_80101F18(0);
        fn_80100798(0, 1);
        break;
    case 12:
        break;
    }
    lbl_802823F4 = lbl_802823FC - 1;
}

// Puts player 0's ball where the lesson starts (the tee when the lesson has no spot) and hands the
// player to the CPU for the demonstration.
void fn_80100508(void) {
    int n = lbl_802823FC - 1;
    f32 fHeight;
    HoleTees* pTees;
    if (-1.0f == lbl_80192DF8[n].vPos[0] && -1.0f == lbl_80192DF8[n].vPos[1] &&
        -1.0f == lbl_80192DF8[n].vPos[2]) {
        pTees = (HoleTees*)fn_8000C594();
        Vec_Copy(&pTees->tee[gSession.nTeeSet[0]].x, &gPlayers[0].fBallX);
    } else {
        fHeight = Terrain_HeightAt(lbl_80192DF8[n].vPos, NULL);
        if (-65536.125f != fHeight) {
            lbl_80192DF8[n].vPos[1] = fHeight;
        }
        Vec_Copy(lbl_80192DF8[n].vPos, &gPlayers[0].fBallX);
    }
    Vec_Copy(&gPlayers[0].fBallX, gPlayers[0].vPreShot);
    Vec_Copy(&gPlayers[0].fBallX, (f32*)gPlayers[0].ball);
    fn_80055AA8((Ball*)gPlayers[0].ball, &gPlayers[0].fBallX, 0);
    // EA bug: always true (|| where && was meant), so the ball is always dropped.
    if (lbl_802823FC != 1 || lbl_802823FC != 8 || lbl_802823FC != 9 || lbl_802823FC != 11) {
        Physics_DropBall((Ball*)gPlayers[0].ball, &gPlayers[0].fBallX);
    }
    gPlayers[0].attrMod[0] = 0;
    gPlayers[0].attrMod[8] = 0;
    gPlayers[0].attrMod[2] = 0;
    gPlayers[0].attrMod[3] = 0;
    gPlayers[0].attrMod[5] = 0;
    gPlayers[0].attrMod[6] = 0;
    gPlayers[0].attrMod[7] = 0;
    gPlayers[0].attrMod[11] = 0;
    gPlayers[0].swing.nState = 0;
    if (gPlayers[0].nController != CONTROLLER_CPU) {
        lbl_80282404 = gPlayers[0].nController;
    }
    gPlayers[0].nController = CONTROLLER_CPU;
}

// The shape the lesson requires, 7 (any) outside mode 11.
int Scenario_RequiredShape(void) {
    int n = lbl_802823FC - 1;
    if (Game_GetMode() != 11) {
        return 7;
    }
    return lbl_80192DF8[n].nShape;
}

// The club the lesson requires, 26 (any) outside mode 11.
int fn_801006F0(int nPlayer) {
    int n = lbl_802823FC - 1;
    if (Game_GetMode() != 11) {
        return 26;
    }
    return lbl_80192DF8[n].nClub;
}

// The shot kind the lesson requires, 8 (any) outside mode 11.
int fn_80100744(void) {
    int n = lbl_802823FC - 1;
    if (Game_GetMode() != 11) {
        return 8;
    }
    return lbl_80192DF8[n].nShotKind;
}

// The lesson's demonstration animation (none outside lessons 1..11).
char* fn_801008A8(void) {
    if (lbl_802823FC > 0 && lbl_802823FC < 12) {
        if (lbl_80282428 >= 6) {
            return lbl_80192D98[lbl_802823FC];
        }
        return lbl_80192DC8[lbl_802823FC];
    }
    return 0;
}

// Shows the next message of one of the lesson's lists, skipping empty entries; nonzero if there
// was one.
int fn_80100798(int nList, int nCount) {
    s16* pList = &lbl_80192F2C[lbl_80282420] + nList;
    u32 i = lbl_802823E8 % nCount;
    int n = 0;
    while (pList[i] == -1 && n < nCount) {
        i = (i + 1) % nCount;
        n++;
    }
    if (pList[i] != -1) {
        fn_80101FC0((u16)pList[i], 0);
        if (nList == 3 && lbl_802823FC == 7) {
            if (i == 0) {
                lbl_802823F4 = 12;
            } else if (i == 2) {
                lbl_802823F4 = 11;
            } else {
                lbl_802823F4 = 6;
            }
        }
    }
    return !(pList[i] == -1);
}

// The lesson restarts: the ball back at its spot, the demonstration again from step 7.
void fn_801008F8(void) {
    fn_80101F40(0, 0);
    fn_800E5200(-1);
    fn_80101F18(0);
    fn_80100508();
    gPlayers[0].nController = lbl_80282404;
    GOLFERSTATE_Switch(GS_PRE_SHOT, 0);
    if (lbl_80282428 == 8 || lbl_80282428 == 9 || lbl_80282428 == 10 || lbl_80282428 == 11) {
        fn_800957D8(gPlayers[0].nShotHandle);
    }
    if (lbl_802823FC == 7) {
        AI_DefaultTarget(0);
        Shot_Prepare(0, 1);
        BreakLine_Start(gPlayers[0].nView0);
        fn_8009B970(gPlayers[0].nView0);
        fn_8001C804(0, 1, 1);
        fn_800957D8(gPlayers[0].nShotHandle);
        fn_80058FA4(0);
        if (lbl_80282428 != 8 && lbl_80282428 != 9 && lbl_80282428 != 10 && lbl_80282428 != 11) {
            fn_80095744(gPlayers[0].nShotHandle, 1);
        }
    }
    lbl_802823E0 = 0;
    lbl_802823E1 = 0;
    lbl_80282428 = 7;
    fn_80047B6C(NULL, 0);
    fn_80047BC0(NULL, 0);
}

// After a re-plan: the target chosen again, keeping the player's club and shot kind.
void fn_80100A3C(int nPlayer) {
    s32 nShotKind = gPlayers[0].nShotKind;
    s32 nClub = gPlayers[0].nClub;
    if (nShotKind == 0) {
        AI_DefaultTarget(0);
    } else {
        AI_ChooseTarget(0);
    }
    Shot_Prepare(0, 1);
    gPlayers[0].nShotKind = nShotKind;
    gPlayers[0].nClub = nClub;
    BreakLine_Start(gPlayers[0].nView0);
    fn_8009B970(gPlayers[0].nView0);
    fn_8001C804(0, 1, 1);
    fn_800957D8(gPlayers[0].nShotHandle);
    fn_80095744(gPlayers[0].nShotHandle, 5);
    fn_80062C38();
}

u8 fn_80100AF8(void) {
    if (fn_80100294() && lbl_802823FC == 5) {
        return 1;
    }
    return 0;
}

// Back to the lesson's start (lesson 1 if none), unless the lessons are over.
void fn_80100B38(void) {
    if (!fn_80100294() || lbl_802823FC == 12) {
        return;
    }
    if (gSession.unk14 == 0 && lbl_80282428 != 17) {
        fn_800A76E4();
        if (lbl_802823FC == 0) {
            lbl_802823FC = 1;
        }
        lbl_80282428 = 4;
        lbl_802823E8 = 0;
        lbl_802823E2 = 0;
        GM_EndOfGolferTurn(0);
        fn_80062C80(gPlayers[0].nC58, 0);
        fn_800E3D38(0, 0);
        fn_80101F40(0, 0);
        fn_80101F18(0);
        fn_800E5200(-1);
    }
}

u8 fn_80100C00(void) {
    return 0;
}

u8 fn_80101738(void) {
    if (Game_GetMode() == 11 && lbl_80282428 == 5 &&
        (lbl_802823FC == 1 || lbl_802823FC == 10 || lbl_802823FC == 8 || lbl_802823FC == 9)) {
        return 0;
    }
    return 1;
}

// HoleFinished: lesson 12 is over unless its step is 19.
u8 fn_80101C9C(int nPlayer, int a) {
    if (lbl_802823FC == 12 && lbl_80282428 != 19) {
        return 1;
    }
    return 0;
}

// GameFinished: after lesson 12.
u8 fn_80101CC4(int a) {
    return lbl_802823FC == 12;
}

// EndGame.
void fn_80101CD8(void) {
    fn_80125854(1);
}

void fn_80101CFC(void) {
    lbl_802823E4 = 1;
    fn_800A6EC8();
}

void fn_80101D24(void) {
    lbl_802823E3 = 1;
    fn_800A6EC8();
}

// A CPU player in a lesson is always lucky, except in lessons 5, 8, 9 and 11; in lesson 7 only while
// player 0 is the CPU.
u8 fn_80101D4C(int nPlayer) {
    if (lbl_802823FC == 7 && gPlayers[0].nController != CONTROLLER_CPU) {
        return 0;
    }
    if (Game_GetMode() == 11 && lbl_802823FC != 5 && lbl_802823FC != 11 && lbl_802823FC != 8 &&
        lbl_802823FC != 9 && gPlayers[nPlayer].nController == CONTROLLER_CPU) {
        return 1;
    }
    return 0;
}

u8 fn_80101DF4(void) {
    if (Game_GetMode() == 11 && lbl_802823FC != 11) {
        return 0;
    }
    return 1;
}

// Is this one of the lessons' demonstration animations?
u8 fn_80101E34(char* szName) {
    int i;
    if (szName == 0) {
        return 0;
    }
    for (i = 0; i < 12; i++) {
        if (strcmp(lbl_80192D98[i], szName) == 0 || strcmp(lbl_80192DC8[i], szName) == 0) {
            return 1;
        }
    }
    return 0;
}

void fn_80101EDC(void) {
    lbl_802823F8 = 1;
}

// Round setup again, then player 0 is handed to the first controller.
void fn_80101EE8(void) {
    fn_80100160();
    gPlayers[0].nController = 0;
}

void fn_80101F18(int nPlayer) {
    fn_80062D6C(43, nPlayer);
}

void fn_80101F40(u8 a, int b) {
    fn_80062D38(15, a, b);
}

void fn_80101F70(void) {
    fn_800E58B4(40);
}

void fn_80101F94(int a, int b) {
    fn_80062D38(38, a, b);
}

void fn_80101FC0(int a, int b) {
    fn_800A7664(9, a, b);
}
