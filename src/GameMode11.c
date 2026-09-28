// GameMode11.c (our name): game mode 11, the lessons. One player on hole 14 of course 10; eleven
// lessons (gLessonNum, 1..11; 12 when all are done), each a shot from a set spot with its own
// shot kind, club and shape (gLessons). The mode saves some of the player's options when it
// starts and puts them back when it ends. Golfer.c, Swing.c and skalib.c ask it what the lesson
// allows.

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"

// fake match: the (s8) on GOLFERSTATE_GetCurrentState (see game.h).

// One lesson: where the ball is placed and the shot kind, club and shape it sets (lessons 1..11;
// 12 is the end).
typedef struct Lesson {
    f32 vPos[4];                // 0x00  (-1, -1, -1): the ball stays on the tee
    s32 nShotKind;              // 0x10  8 = any
    s32 nClub;                  // 0x14  26 = any
    s32 nShape;                 // 0x18  7 = any
} Lesson;

// Each lesson's animation (index 0 unused), picked by Lessons_GetAnimName: gLessonTryAnims from step 6 on
// (the player's tries), gLessonDemoAnims before it (the demonstration). Only lesson 6 differs.
char* gLessonTryAnims[12] = {
    "tdlpre01", "tdlpre04", "gdlpre03", "gdlpre53", "g3lpre02", "g3lpre01",
    "g3lpre03", "gplpre51", "tdlpre04", "tdlpre05", "tdlpre04", "g3lpre04",
};
char* gLessonDemoAnims[12] = {
    "tdlpre01", "tdlpre04", "gdlpre03", "gdlpre53", "g3lpre02", "g3lpre01",
    "g3lpre02", "gplpre51", "tdlpre04", "tdlpre05", "tdlpre04", "g3lpre04",
};

// The lessons, indexed by gLessonNum - 1.
Lesson gLessons[11] = {
    {{-1.0f, -1.0f, -1.0f, 1.0f}, 8, 26, 7},
    {{-372.0f, 0.0f, 324.0f, 1.0f}, 3, 26, 7},
    {{-406.9f, 0.0f, 312.5f, 1.0f}, 5, 26, 7},
    {{-296.5f, 0.0f, 281.5f, 1.0f}, 4, 13, 7},
    {{-344.0f, 0.0f, 303.0f, 1.0f}, 1, 23, 7},
    {{-407.0f, 0.0f, 340.0f, 1.0f}, 2, 21, 7},
    {{-398.0f, 0.0f, 330.0f, 1.0f}, 8, 26, 7},
    {{-1.0f, -1.0f, -1.0f, 1.0f}, 8, 26, 6},
    {{-1.0f, -1.0f, -1.0f, 1.0f}, 8, 26, 5},
    {{-1.0f, -1.0f, -1.0f, 1.0f}, 8, 26, 7},
    {{-407.0f, 0.0f, 340.0f, 1.0f}, 8, 26, 7},
};

// The lessons' message lists: 16 message ids per row (-1 = none); gLessonLineRow is the lesson's row.
s16 gLessonLines[12 * 16] = {
    3, 4, 5, -1, -1, -1, -1, -1, 6, 7, 8, 9, 10, 11, 12, -1,
    -1, 13, 14, 15, 16, 17, -1, -1, 6, 7, 8, 18, 19, 20, 21, 12,
    -1, 22, 23, 24, 25, 26, -1, -1, 6, 7, 8, 18, -1, 20, 21, 12,
    -1, 0, 1, 33, 34, 35, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 27, 28, 29, 34, 35, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 2, -1, 33, 34, 35, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 36, -1, 33, 34, 35, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 39, 40, 33, 34, 35, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 42, 43, 44, 46, 61, 62, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 49, 50, -1, -1, -1, -1, -1, 6, 7, 8, 9, 10, 53, 54, -1,
    -1, 55, 57, 63, 59, -1, -1, -1, 58, -1, -1, -1, -1, 59, 63, 58,
    60, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

s32 gLessonSavedOptionC = 4;                    // the options' nC, saved while the mode runs
s32 gLessonStep;                    // the lesson's step
s32 gLessonNextStep;
s32 gLessonLineRow;                    // the lesson's row in gLessonLines
s32 gLessonBackswingHint;
s32 gLessonDownswingHint;
s32 gLessonHintSwapTimer;                    // frames until the two alternating hints swap
s32 gLessonHighlightTimer;                    // frames until the next highlight
s32 gLessonHighlight;                    // the highlighted one of four hints (4 = none yet)
u8  gLessonSavedCommentLevel;                    // options unk0[4], saved
u8  gLessonSavedPuttGrid;                    // options unk84, saved
u8  gLessonSavedBoost;                    // the boost option, saved
u8  gLessonSavedSpin;                    // the spin option, saved
s32 gLessonController;                    // player 0's controller, kept while the CPU demonstrates
f32 gLessonHookSlice;                    // lessons 8 and 9 test its sign
s32 gLessonNum;
u8  gLessonPauseClosed;
s32 gLessonPanel;
u8  gLessonShowBackswingHint;                    // which of the two alternating hints is showing
u8  gLessonTryHintsSet;                    // the player's try has set up its hints
u8  gLessonSkipPending;
s32 gLessonSavedWind;                    // the wind option, saved while the mode runs
u32 gLessonFailedTries;                    // picks which message of a list is shown
u8  gLessonSwingCommitted;
u8  gLessonQuitChosen;
u8  gLessonContinueChosen;
u8  gLessonWaitingForLine;
u8  gLessonBoostUsed;
u8  gLessonSpinUsed;

void  Gaud_ExitCrowdReactionSound(void);
void  fn_800E5200(int a);
u8    Gaud_GetCommentStatus(void);

void Lessons_LoadHole(void);
void Lessons_RestartHole(void);
void Lessons_Reset(void);
void Lessons_StartGamePreData(void);
void Lessons_Shutdown(void);
void Lessons_StopWaitingForLine(void);
void Lessons_PlaceBall(void);
int  Lessons_PlayLine(int nList, int nCount);
void fn_80101F18(int nPlayer);
void fn_80101F40(u8 a, int b);
void fn_80101FC0(int a, int b);
void Lessons_AfterReplan(int nPlayer);
void fn_80100C08(void);
u8   fn_80101C9C(int nPlayer, u8 bCheck);
u8   fn_80101CC4(u8 bCheck);
void fn_80101CD8(void);
void fn_8010179C(void);
void Lessons_StartTry(void);
void fn_80101F70(void);
void fn_80101F94(int a, int b);

// Mode 11 starts (pfnInit): its callbacks; the yardage, the stroke limit, gimmes, the flyovers
// (b27F, b280), setup tips, the re-plan button, the flight-camera toggles and in-flight replays
// off, with b275, b27B..b27D, b285 and b28A; n290, nC, n10 and b276 (re-plan as the swing begins)
// set to 1; the ball's random rolls off (fn_80055C1C: lies and bounces come out as in a
// simulation). The options the lessons change are saved and set: commentary level 4, the putting
// grid off, power boost and spin on. Random stream 0 is seeded with 69, so the lessons play the
// same each time.
void Lessons_Init(void) {
    gpGame->pfnInit = Lessons_Init;
    gpGame->pfnShutdown = Lessons_Shutdown;
    gpGame->pfnHoleFinished = fn_80101C9C;
    gpGame->pfnGameFinished = fn_80101CC4;
    gpGame->pfn1E4 = Lessons_LoadHole;
    gpGame->pfn220 = fn_80100C08;
    gpGame->pfn224 = Lessons_RestartHole;
    gpGame->pfn22C = Lessons_AfterReplan;
    gpGame->pfn1EC = Lessons_StartGamePreData;
    gpGame->pfnEndGame = fn_80101CD8;
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
    gLessonSavedCommentLevel = gSession.options.a0[4];
    gLessonSavedPuttGrid = gSession.options.b84;
    gLessonSavedBoost = gSession.options.bBoostEnabled;
    gLessonSavedSpin = gSession.options.bSpinEnabled;
    gSession.options.a0[4] = 4;
    gSession.options.b84 = 0;
    gSession.options.bBoostEnabled = 1;
    gSession.options.bSpinEnabled = 1;
    Misc_SetSeedFunc(0, 69);
}

// Hole start (pfn1E4): the lessons start over (Lessons_Reset).
void Lessons_LoadHole(void) {
    Lessons_Reset();
}

// The hole restarts (pfn224): the lessons start over (Lessons_Reset).
void Lessons_RestartHole(void) {
    Lessons_Reset();
}

// Back to the start: lesson 1 at step 2 (where the lessons begin), its row of lines, and the
// flyover mode reset (GM_FlyByMode_Init).
void Lessons_Reset(void) {
    gLessonStep = 2;
    gLessonNum = 1;
    gLessonLineRow = 0;
    GM_FlyByMode_Init();
}

// Before the round (pfn1EC; also Lessons_StartFromMenu): course 10 with only hole 14, tee set 0 and
// pin 0; the options' nC saved and set to 4 and the wind saved and set to calm (0); one player,
// golfer 1, played by the CPU (who demonstrates each lesson); no mulligans; the continue / stop
// answers cleared.
void Lessons_StartGamePreData(void) {
    GM_SetCurrentCourse(10);
    GM_SelectHoleSet(0);
    gpGame->bHoleSelected[13] = 1;
    GM_InitializeCurrentHoleToFirstSelected();
    gSession.nTeeSet[0] = 0;
    gSession.nTeeSet[1] = 0;
    gSession.nPinSet = 0;
    gpGame->nPinSet[Game_CurHoleIndex()] = 0;
    gLessonSavedOptionC = gSession.options.nC;
    gLessonSavedWind = gSession.options.nWind;
    gSession.options.nC = 4;
    gSession.options.nWind = 0;
    Session_SetNumPlayers(1);
    Session_SetGolfer(1, 0);
    gPlayers[0].nController = CONTROLLER_CPU;
    gpGame->nMulligans = 0;
    gLessonContinueChosen = 0;
    gLessonQuitChosen = 0;
}

// The mode ends (pfnShutdown): the options it changed go back (nC, wind, commentary level, putting
// grid, power boost, spin) and the ball's random rolls come back on.
void Lessons_Shutdown(void) {
    Session* pSession;
    gSession.options.nC = gLessonSavedOptionC;
    gSession.options.nWind = gLessonSavedWind;
    fn_80055C1C(0);
    // fake match: &gSession re-taken inside the first store after the call, as the original
    // recomputes it
    (pSession = &gSession)->options.a0[4] = gLessonSavedCommentLevel;
    (pSession)->options.b84 = gLessonSavedPuttGrid;
    (pSession)->options.bBoostEnabled = gLessonSavedBoost;
    (pSession)->options.bSpinEnabled = gLessonSavedSpin;
}

// Is a lesson running (mode 11)?
u8 Lessons_IsRunning(void) {
    return Game_GetMode() == 11;
}

// The front end ends the current wait (UI command fn_800874F0), in mode 11 only: a step waiting for
// the coach's line goes on to gLessonNextStep (the line plays on); so does a wait flagged in
// gLessonSkipPending, which nothing sets.
void Lessons_StopWaiting(void) {
    if (Lessons_IsRunning()) {
        if (gLessonSkipPending) {
            gLessonSkipPending = 0;
            gLessonStep = gLessonNextStep;
        }
        Lessons_StopWaitingForLine();
    }
}

// If the step is waiting for the coach's line to end (step 1), it waits no more: on to
// gLessonNextStep.
void Lessons_StopWaitingForLine(void) {
    if (gLessonWaitingForLine) {
        gLessonWaitingForLine = 0;
        gLessonStep = gLessonNextStep;
    }
}

// On to the next lesson (gLessonNum + 1): its row of lines in gLessonLines (the rows are not in
// lesson order), the values its backswing and downswing hints show (5 and 2; 4 and 3 in lesson 8, 6
// and 1 in lesson 9), the putting grid option on from lesson 7 and off again from lesson 10,
// gLessonSwingCommitted set for lesson 11, and gLessonPanel = the lesson - 1. After the last lesson
// (12) the hints are hidden and the closing line plays.
void Lessons_NextLesson(void) {
    switch (++gLessonNum) {
    case 1:
        gLessonLineRow = 0x00;
        gLessonBackswingHint = 5;
        gLessonDownswingHint = 2;
        break;
    case 8:
        gLessonLineRow = 0x10;
        gLessonBackswingHint = 4;
        gLessonDownswingHint = 3;
        break;
    case 9:
        gLessonLineRow = 0x20;
        gLessonBackswingHint = 6;
        gLessonDownswingHint = 1;
        break;
    case 2:
        gLessonLineRow = 0x30;
        gLessonBackswingHint = 5;
        gLessonDownswingHint = 2;
        break;
    case 5:
        gLessonLineRow = 0x40;
        gLessonBackswingHint = 5;
        gLessonDownswingHint = 2;
        break;
    case 6:
        gLessonLineRow = 0x50;
        gLessonBackswingHint = 5;
        gLessonDownswingHint = 2;
        break;
    case 3:
        gLessonLineRow = 0x60;
        gLessonBackswingHint = 5;
        gLessonDownswingHint = 2;
        break;
    case 4:
        gLessonLineRow = 0x70;
        gLessonBackswingHint = 5;
        gLessonDownswingHint = 2;
        break;
    case 7:
        gSession.options.b84 = 1;
        gLessonLineRow = 0x80;
        gLessonBackswingHint = 5;
        gLessonDownswingHint = 2;
        break;
    case 10:
        gSession.options.b84 = 0;
        gLessonLineRow = 0x90;
        gLessonBackswingHint = 5;
        gLessonDownswingHint = 2;
        break;
    case 11:
        gLessonLineRow = 0xA0;
        gLessonBackswingHint = 5;
        gLessonDownswingHint = 2;
        gLessonSwingCommitted = 1;
        break;
    case 12:
        gLessonLineRow = 0xB0;
        fn_80101F40(0, 0);
        fn_800E5200(-1);
        fn_80101F18(0);
        Lessons_PlayLine(0, 1);
        break;
    }
    gLessonPanel = gLessonNum - 1;
}

// Puts player 0's ball where the lesson starts (the tee when the lesson has no spot; a spot's
// height comes from the ground) and drops it; clears the player's attribute modifiers 0, 2, 3, 5..8
// and 11 and idles the swing; and hands the player to the CPU for the demonstration, keeping a
// human's controller in gLessonController.
void Lessons_PlaceBall(void) {
    int n = gLessonNum - 1;
    f32 fHeight;
    CourseInfo* pCourse;
    if (-1.0f == gLessons[n].vPos[0] && -1.0f == gLessons[n].vPos[1] &&
        -1.0f == gLessons[n].vPos[2]) {
        pCourse = Ter_GetTGD();
        LLMath_CopyVec(&pCourse->tee[gSession.nTeeSet[0]].x, gPlayers[0].vBall);
    } else {
        fHeight = CamScript_GuessBestPlayableHeight(gLessons[n].vPos, NULL);
        if (-65536.125f != fHeight) {
            gLessons[n].vPos[1] = fHeight;
        }
        LLMath_CopyVec(gLessons[n].vPos, gPlayers[0].vBall);
    }
    LLMath_CopyVec(gPlayers[0].vBall, gPlayers[0].vPreShot);
    LLMath_CopyVec(gPlayers[0].vBall, gPlayers[0].ball.vPos);
    Physics_InitBall(&gPlayers[0].ball, gPlayers[0].vBall, 0);
    // EA bug: always true (|| where && was meant), so the ball is always dropped.
    if (gLessonNum != 1 || gLessonNum != 8 || gLessonNum != 9 || gLessonNum != 11) {
        Physics_DropBall(&gPlayers[0].ball, gPlayers[0].vBall);
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
        gLessonController = gPlayers[0].nController;
    }
    gPlayers[0].nController = CONTROLLER_CPU;
}

// The shape the lesson sets, 7 (any) outside mode 11.
int Lessons_GetShape(int nPlayer) {
    int n = gLessonNum - 1;
    if (Game_GetMode() != 11) {
        return 7;
    }
    return gLessons[n].nShape;
}

// The club the lesson sets, 26 (any) outside mode 11.
int Lessons_GetClub(int nPlayer) {
    int n = gLessonNum - 1;
    if (Game_GetMode() != 11) {
        return 26;
    }
    return gLessons[n].nClub;
}

// The shot kind the lesson sets, 8 (any) outside mode 11.
int Lessons_GetShotKind(void) {
    int n = gLessonNum - 1;
    if (Game_GetMode() != 11) {
        return 8;
    }
    return gLessons[n].nShotKind;
}

// Plays one of the coach's lines for the lesson: list nList of its row, nCount entries long (0 the
// opening line, 1 before the demonstration, 2 after it, 3..7 a missed shot, 8..10 a fault, 11..12 a
// short shot, 13..14 several faults), the one gLessonFailedTries picks or the next one that is not
// empty. Nonzero if a line played. In lesson 7 the missed-shot line also picks the panel shown (12,
// 11 or 6).
int Lessons_PlayLine(int nList, int nCount) {
    s16* pList = &gLessonLines[gLessonLineRow] + nList;
    u32 i = gLessonFailedTries % nCount;
    int n = 0;
    while (pList[i] == -1 && n < nCount) {
        i = (i + 1) % nCount;
        n++;
    }
    if (pList[i] != -1) {
        fn_80101FC0((u16)pList[i], 0);
        if (nList == 3 && gLessonNum == 7) {
            if (i == 0) {
                gLessonPanel = 12;
            } else if (i == 2) {
                gLessonPanel = 11;
            } else {
                gLessonPanel = 6;
            }
        }
    }
    return !(pList[i] == -1);
}

// The name of the golfer's animation for the lesson (CharAnim.c uses it for clip group 1): from
// gLessonTryAnims from step 6 on (the player's tries), from gLessonDemoAnims before (the
// demonstration); NULL outside lessons 1..11.
char* Lessons_GetAnimName(void) {
    if (gLessonNum > 0 && gLessonNum < 12) {
        if (gLessonStep >= 6) {
            return gLessonTryAnims[gLessonNum];
        }
        return gLessonDemoAnims[gLessonNum];
    }
    return 0;
}

// The player's try starts: the hints hidden, the ball back at the lesson's spot
// (Lessons_PlaceBall), player 0 given back its controller and sent to pre-shot, and fn_800957D8 on
// the golfer after a failed try (steps 8..11). In lesson 7 (the putt) the default target is aimed
// at, the shot prepared, the putt line reset, the golfer lined up and the swing reset (and
// animation 1 played, unless after a failed try). The boost and spin flags clear, the step becomes
// 7, and fn_80047B6C / fn_80047BC0 are called with no ball.
void Lessons_StartTry(void) {
    fn_80101F40(0, 0);
    fn_800E5200(-1);
    fn_80101F18(0);
    Lessons_PlaceBall();
    gPlayers[0].nController = gLessonController;
    GOLFERSTATE_Switch(GS_PRE_SHOT, 0);
    if (gLessonStep == 8 || gLessonStep == 9 || gLessonStep == 10 || gLessonStep == 11) {
        fn_800957D8(gPlayers[0].pChar);
    }
    if (gLessonNum == 7) {
        AI_DefaultTarget(0);
        Shot_Prepare(0, 1);
        BreakLine_Reset(gPlayers[0].nView[0]);
        fn_8009B970(gPlayers[0].nView[0]);
        Character_AlignShotWithTarget(0, 1, 1);
        fn_800957D8(gPlayers[0].pChar);
        SW_vInitSwing(0);
        if (gLessonStep != 8 && gLessonStep != 9 && gLessonStep != 10 && gLessonStep != 11) {
            fn_80095744(gPlayers[0].pChar, 1);
        }
    }
    gLessonSpinUsed = 0;
    gLessonBoostUsed = 0;
    gLessonStep = 7;
    fn_80047B6C(NULL, 0);
    fn_80047BC0(NULL, 0);
}

// After a re-plan (pfn22C): player 0's target is picked again (the default one for a putt, else the
// AI's choice) and the shot prepared, keeping the club and shot kind the lesson set; the putt line,
// the golfer's aim and animation 5 are set up again. nPlayer is unused.
void Lessons_AfterReplan(int nPlayer) {
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
    BreakLine_Reset(gPlayers[0].nView[0]);
    fn_8009B970(gPlayers[0].nView[0]);
    Character_AlignShotWithTarget(0, 1, 1);
    fn_800957D8(gPlayers[0].pChar);
    fn_80095744(gPlayers[0].pChar, 5);
    fn_80062C38();
}

// Lesson 5 is running: its swing is a short one, so the CPU's demonstration stops its backswing at
// 65% of the way to the top instead of 98% (Swing.c).
u8 Lessons_IsShortBackswingLesson(void) {
    if (Lessons_IsRunning() && gLessonNum == 5) {
        return 1;
    }
    return 0;
}

// Starts the current lesson over from its demonstration (lesson 1 if none), from the pause menu or
// the front end's commands, unless the lessons are over, the game is paused or the continue / stop
// question is up (step 17): the coach stops, step 4 (the demonstration set up again after the
// flyover), no failed tries, the turn ended, the HUD off (fn_80062C80, GUI_ToggleUI) and the hints
// hidden.
void Lessons_RestartLesson(void) {
    if (!Lessons_IsRunning() || gLessonNum == 12) {
        return;
    }
    if (gSession.nPaused == 0 && gLessonStep != 17) {
        Gaud_StopComment();
        if (gLessonNum == 0) {
            gLessonNum = 1;
        }
        gLessonStep = 4;
        gLessonFailedTries = 0;
        gLessonWaitingForLine = 0;
        GM_EndOfGolferTurn(0);
        fn_80062C80(gPlayers[0].nC58, 0);
        GUI_ToggleUI(0, 0);
        fn_80101F40(0, 0);
        fn_80101F18(0);
        fn_800E5200(-1);
    }
}

u8 fn_80100C00(void) {
    return 0;
}

// The item (4..7) of the highlighted one of the four hints that take turns (gLessonHighlight).
static inline int Hint(void) {
    int nHint;
    if (gLessonHighlight == 0) {
        nHint = 4;
    } else if (gLessonHighlight == 1) {
        nHint = 5;
    } else {
        nHint = 7;
        if (gLessonHighlight == 2) {
            nHint = 6;
        }
    }
    return nHint;
}

// Every frame: the lesson's steps (gLessonStep). 2..5 set up a lesson and its demonstration, 6 and
// 7 the player's tries with their hints, 8..11 a failed try, 12 a passed one; 0, 1 and 18 wait (a
// button, the message, the camera), 14..16 end the turn, 13 quits, 19 follows lesson 7 (then 17:
// continue or quit) and the last lesson.
void fn_80100C08(void) {
    f32 v[4] = {0.0f, 0.0f, 0.0f, 0.5f};
    int nView;
    if (gLessonPauseClosed) {
        fn_800E58B4(39);
        gLessonPauseClosed = 0;
    }
    switch (gLessonStep) {
    case 0:
        if (Input_ReadControlPad(gPlayers[0].nController) & Controller_GetButtonMask(0, 0)) {
            gLessonStep = gLessonNextStep;
        }
        break;
    case 1:
        gLessonWaitingForLine = 1;
        if (!Gaud_GetCommentStatus()) {
            gLessonWaitingForLine = 0;
            gLessonStep = gLessonNextStep;
            if (gLessonNextStep == 13) {
                gLessonStep = 18;
                gLessonNextStep = 13;
                CameraController_FadeOut(ViewController_GetCameraControl(gPlayers[0].nView[0]), 0.25f, v);
            }
        }
        break;
    case 18:
        if (fn_80063C7C(ViewController_GetCameraControl(gPlayers[0].nView[0]))) {
            gLessonStep = gLessonNextStep;
        }
        break;
    case 2:
        Gaud_InitCrowdReactionSound(2, 1);
        gLessonNum = 0;
        Lessons_NextLesson();
        Lessons_PlaceBall();
        gLessonStep = 3;
        break;
    case 3:
        fn_80101F40(0, 0);
        fn_80101F18(0);
        fn_800E5200(-1);
        Lessons_PlayLine(0, 1);
        gLessonStep = 4;
        gLessonFailedTries = 0;
        break;
    case 4:
        if ((s8)GOLFERSTATE_GetCurrentState(0) != 20) {
            Lessons_PlaceBall();
            GOLFERSTATE_Switch(GS_PRE_SHOT, 0);
            fn_80101F40(0, 0);
            fn_800E5200(-1);
            fn_80101F18(0);
            Lessons_PlayLine(1, 1);
            gLessonStep = 5;
        }
        break;
    case 5:
        if ((s8)GOLFERSTATE_GetCurrentState(0) != GS_PRE_SHOT) {
            if (gPlayers[0].swing.nState == 1 || gPlayers[0].swing.nState == 2) {
                fn_80101F94(2, 0);
                fn_80101F40(1, gLessonBackswingHint);
                fn_800E5200(gLessonPanel);
                fn_80101F18(1);
                if (gLessonNum == 10) {
                    fn_80101F94(0, 1);
                }
            } else if (gPlayers[0].swing.nState == 3 || gPlayers[0].swing.nState == 4 ||
                       gPlayers[0].swing.nState == 5) {
                fn_80101F94(0, 0);
                if (gLessonNum == 11 && gPlayers[0].swing.nState == 5) {
                    if (gLessonSwingCommitted) {
                        fn_80101F40(1, gLessonBackswingHint);
                        fn_800E5200(gLessonPanel);
                        fn_80101F18(3);
                        fn_80101F94(2, 1);
                    } else {
                        fn_80101F40(0, 0);
                        fn_800E5200(-1);
                        fn_80101F18(0);
                    }
                } else {
                    fn_80101F40(1, gLessonDownswingHint);
                    fn_800E5200(gLessonPanel);
                    fn_80101F18(2);
                    fn_80101F94(2, 0);
                }
            } else {
                fn_80101F40(1, 0);
                fn_800E5200(gLessonPanel);
                fn_80101F94(0, 0);
            }
        }
        break;
    case 6:
        Lessons_StartTry();
        gLessonTryHintsSet = 0;
        gLessonSwingCommitted = 0;
        // falls through
    case 7:
        if (gPlayers[0].swing.nState == 0) {
            if (gLessonTryHintsSet) {
                if (--gLessonHintSwapTimer <= 0) {
                    gLessonHintSwapTimer = 59;
                    gLessonShowBackswingHint = !gLessonShowBackswingHint;
                }
                if (gLessonShowBackswingHint) {
                    fn_80101F40(1, gLessonBackswingHint);
                    fn_800E5200(gLessonPanel);
                    fn_80101F18(1);
                    if (gLessonNum == 10) {
                        fn_80101F94(0, 1);
                    }
                } else {
                    fn_80101F40(1, gLessonDownswingHint);
                    fn_800E5200(gLessonPanel);
                    fn_80101F18(2);
                    if (gLessonNum == 10) {
                        fn_80101F94(0, 0);
                    }
                }
                if ((gLessonNum == 6 || gLessonNum == 7) && --gLessonHighlightTimer <= 0) {
                    gLessonHighlightTimer = 83;
                    if (gLessonHighlight == 4) {
                        gLessonHighlight = 0;
                        fn_80101F94(4, 1);
                        fn_80101F94(5, 0);
                        fn_80101F94(6, 0);
                        fn_80101F94(7, 0);
                    } else {
                        fn_80101F94(Hint(), 0);
                        gLessonHighlight++;
                        gLessonHighlight %= 4;
                        fn_80101F94(Hint(), 1);
                    }
                }
            } else {
                gLessonHighlightTimer = 389;
                gLessonHighlight = 4;
                gLessonHintSwapTimer = 59;
                fn_80101F40(1, gLessonBackswingHint);
                fn_800E5200(gLessonPanel);
                fn_80101F18(1);
                gLessonTryHintsSet = 1;
                gLessonShowBackswingHint = 1;
                if (gLessonNum == 6 || gLessonNum == 7) {
                    fn_80101F94(4, 1);
                    fn_80101F94(5, 1);
                    fn_80101F94(6, 1);
                    fn_80101F94(7, 1);
                } else {
                    fn_80101F94(4, 0);
                    fn_80101F94(5, 0);
                    fn_80101F94(6, 0);
                    fn_80101F94(7, 0);
                }
                if (gLessonNum == 10) {
                    fn_80101F94(0, 1);
                } else if (gLessonNum == 8) {
                    fn_80101F94(5, 1);
                } else if (gLessonNum == 9) {
                    fn_80101F94(4, 1);
                } else if (gLessonNum == 2) {
                    fn_80101F94(8, 1);
                }
            }
        } else {
            fn_80101F94(4, 0);
            fn_80101F94(5, 0);
            fn_80101F94(6, 0);
            fn_80101F94(7, 0);
            fn_80101F94(8, 0);
            if (gPlayers[0].swing.nState == 2 || gPlayers[0].swing.nState == 3 ||
                (gPlayers[0].swing.nState == 1 &&
                 ((gLessonNum == 5 && gPlayers[0].pChar->fBackswing > 0.45f) ||
                  (gLessonNum != 5 && gPlayers[0].pChar->fBackswing > 0.75f)))) {
                fn_80101F40(1, gLessonDownswingHint);
                fn_800E5200(gLessonPanel);
                fn_80101F18(2);
                gLessonSwingCommitted = 1;
                if (gLessonNum == 10) {
                    fn_80101F94(0, 0);
                } else if (gLessonNum == 11) {
                    fn_80101F94(2, 0);
                }
            } else if (gPlayers[0].swing.nState == 5 || gPlayers[0].swing.nState == 4) {
                gLessonHookSlice = SW_vGetHookSlice(0);
                if (gLessonNum == 11) {
                    fn_80101F94(2, 1);
                    if (!gLessonSwingCommitted) {
                        fn_80101F40(0, 0);
                        fn_800E5200(-1);
                        fn_80101F18(0);
                    } else {
                        fn_80101F40(1, gLessonBackswingHint);
                        fn_800E5200(gLessonPanel);
                        fn_80101F18(3);
                    }
                } else {
                    fn_80101F40(0, 0);
                    fn_800E5200(-1);
                    fn_80101F18(0);
                }
            } else {
                fn_80101F40(1, gLessonBackswingHint);
                fn_800E5200(gLessonPanel);
                fn_80101F18(1);
                if (gLessonNum == 10) {
                    fn_80101F94(0, 1);
                }
            }
        }
        break;
    case 8:
        if (gLessonNum == 10) {
            fn_80101F94(0, 1);
        } else if (gLessonNum == 8) {
            fn_80101F94(5, 1);
        } else if (gLessonNum == 9) {
            fn_80101F94(4, 1);
        }
        Lessons_StartTry();
        Lessons_PlayLine(3, 5);
        break;
    case 9:
        if (gLessonNum == 10) {
            fn_80101F94(0, 1);
        } else if (gLessonNum == 8) {
            fn_80101F94(5, 1);
        } else if (gLessonNum == 9) {
            fn_80101F94(4, 1);
        }
        Lessons_StartTry();
        Lessons_PlayLine(11, 2);
        break;
    case 10:
        if (gLessonNum == 10) {
            fn_80101F94(0, 1);
        } else if (gLessonNum == 8) {
            fn_80101F94(5, 1);
        } else if (gLessonNum == 9) {
            fn_80101F94(4, 1);
        }
        Lessons_StartTry();
        Lessons_PlayLine(8, 3);
        break;
    case 11:
        if (gLessonNum == 10) {
            fn_80101F94(0, 1);
        } else if (gLessonNum == 8) {
            fn_80101F94(5, 1);
        } else if (gLessonNum == 9) {
            fn_80101F94(4, 1);
        }
        Lessons_StartTry();
        Lessons_PlayLine(13, 2);
        break;
    case 12:
        if (gLessonNum == 10) {
            fn_80101F94(2, 1);
        }
        Lessons_NextLesson();
        if (gLessonNum == 12) {
            gLessonStep = 19;
        } else if (gLessonNum == 8) {
            gLessonStep = 19;
        } else {
            gLessonNextStep = 15;
            Gaud_StopComment();
            gLessonWaitingForLine = 0;
            gLessonStep = gLessonNextStep;
            if (gLessonNextStep == 15) {
                GM_EndOfGolferTurn(0);
                gLessonStep = 3;
            }
        }
        break;
    case 15:
        GM_EndOfGolferTurn(0);
        gLessonStep = 3;
        break;
    case 16:
        GM_EndOfGolferTurn(0);
        gLessonStep = 6;
        break;
    case 14:
        gLessonNextStep = 15;
        gLessonStep = 1;
        break;
    case 13:
        gSession.b12 = 1;
        EVENT_Trigger(0, 5, 0, -1);
        break;
    case 17:
        if (gLessonContinueChosen) {
            gLessonStep = gLessonNextStep;
        } else if (gLessonQuitChosen) {
            gLessonStep = 13;
        }
        // falls through
    case 19:
        if (fn_80063C7C(ViewController_GetCameraControl(gPlayers[0].nView[0]))) {
            nView = gPlayers[0].nView[0];
            CameraController_SetCameraMode(ViewController_GetCameraControl(nView), 18, 0, nView);
            if (gLessonNum == 12) {
                gLessonNextStep = 13;
                gLessonStep = 1;
                gLessonWaitingForLine = 1;
                break;
            }
            // Profile 0 gets its first TOUR card level (lesson 12, the end, has left above).
            if (gpSaveData->nTourCardLevel < 1) {
                gpSaveData->nTourCardLevel = 1;
            }
            gLessonNextStep = 14;
            fn_80101F70();
            gLessonContinueChosen = 0;
            gLessonQuitChosen = 0;
            gLessonStep = 17;
        }
        break;
    }
}

u8 fn_80101738(void) {
    if (Game_GetMode() == 11 && gLessonStep == 5 &&
        (gLessonNum == 1 || gLessonNum == 10 || gLessonNum == 8 || gLessonNum == 9)) {
        return 0;
    }
    return 1;
}

// Judges the lesson's shot: too short (step 9), off target (step 8), a lesson-specific fault
// (step 10), several faults (step 11), or passed (step 12). A failed shot adds one to gLessonFailedTries;
// the demonstration's shot (step 5) only moves on to step 16.
void fn_8010179C(void) {
    u8 bShort = 0;
    u8 bMissed = 0;
    u8 bFault = 0;
    f32 fLength;
    int nLesson;
    int nLie;
    if (gLessonStep == 5) {
        Gaud_InitCrowdReactionSound(1, 1);
        Lessons_PlayLine(2, 1);
        gLessonStep = 16;
        return;
    }
    fLength = fn_800D0550(0);
    nLesson = gLessonNum;
    nLie = gPlayers[0].ball.nLie;
    switch (nLesson) {
    case 1:
        if (fLength < 260.0f) {
            bShort = 1;
        }
        if (nLie != 1) {
            bFault = 1;
        }
        break;
    case 8:
        if (gLessonHookSlice < 0.01f) {
            bMissed = 1;
        }
        break;
    case 9:
        if (gLessonHookSlice > -0.01f) {
            bMissed = 1;
        }
        break;
    case 2:
        if (nLie != LIE_GREEN_e && nLie != 12) {
            bMissed = 1;
        }
        break;
    case 5:
        if (nLie != LIE_GREEN_e && nLie != 12) {
            bMissed = 1;
        }
        break;
    case 80:            // EA bug: 80 for 6 (asm 8010185C cmpwi 0x50), so lesson 6 is never judged
        if (nLie != LIE_GREEN_e && nLie != 12) {
            bMissed = 1;
        }
        break;
    case 3:
        if (nLie != LIE_GREEN_e && nLie != 12) {
            bMissed = 1;
        }
        break;
    case 4:
        if (nLie != LIE_GREEN_e && nLie != 12) {
            bMissed = 1;
        }
        break;
    case 7:
        if (nLie != 12) {
            bMissed = 1;
        }
        break;
    case 10:
        if (!gLessonBoostUsed) {
            bMissed = 1;
            bFault = 1;
        }
        if (fLength < 260.0f) {
            bShort = 1;
        }
        break;
    case 11:
        if (!gLessonSpinUsed) {
            bMissed = 1;
        }
        if (nLie != LIE_GREEN_e && nLie != 12) {
            bFault = 1;
        }
        break;
    }
    if ((bShort & bMissed) || (bShort & bFault) || (bMissed & bFault)) {
        gLessonStep = 11;
        Gaud_InitCrowdReactionSound(5, 1);
        gLessonFailedTries++;
    } else if (bShort) {
        gLessonStep = 9;
        Gaud_InitCrowdReactionSound(5, 1);
        gLessonFailedTries++;
    } else if (bMissed) {
        gLessonStep = 8;
        Gaud_InitCrowdReactionSound(5, 1);
        gLessonFailedTries++;
    } else if (bFault) {
        gLessonStep = 10;
        Gaud_InitCrowdReactionSound(5, 1);
        gLessonFailedTries++;
    } else {
        gLessonStep = 12;
        if (nLesson == 7 || nLesson == 11) {
            Gaud_InitCrowdReactionSound(3, 1);
        } else {
            Gaud_InitCrowdReactionSound(1, 1);
        }
    }
}

// An event (event.c's numbers) during a lesson; nonzero blocks it. Event 10 changes the music, 32
// and 34 end the shot (it is judged), 45 and 46 are what lessons 10 and 11 wait for.
u8 fn_80101AA8(int nPlayer, int nEvent) {
    if (!Lessons_IsRunning()) {
        return 0;
    }
    if (nEvent == 10) {
        if (gLessonStep == 5) {
            Gaud_InitCrowdReactionSound(0, 0);
        } else if (gLessonNum == 1 || gLessonNum == 8 || gLessonNum == 9 || gLessonNum == 10) {
            Gaud_InitCrowdReactionSound(2, 0);
        } else {
            Gaud_InitCrowdReactionSound(0, 0);
        }
        return 0;
    }
    if (nEvent == 29) {
        gLessonSwingCommitted = 0;
        return 0;
    }
    if (nEvent == 28 && gLessonStep == 5 &&
        (gLessonNum == 1 || gLessonNum == 10 || gLessonNum == 8 || gLessonNum == 9)) {
        Lessons_PlayLine(2, 1);
        gLessonStep = 16;
        return 1;
    }
    if (nEvent == 4) {
        return 1;
    }
    if (nEvent == 32 || nEvent == 34) {
        fn_8010179C();
        return 1;
    }
    if (nEvent == 46) {
        gLessonSpinUsed = 1;
        return 1;
    }
    if (nEvent == 45) {
        gLessonBoostUsed = 1;
        return 1;
    }
    if (gLessonNum == 7 && (nEvent == 20 || nEvent == 21)) {
        return 0;
    }
    if (gLessonNum == 6 && (nEvent == 20 || nEvent == 21)) {
        return 0;
    }
    if (nEvent == 13 || nEvent == 14 || nEvent == 15 || nEvent == 16 || nEvent == 17 || nEvent == 20 ||
        nEvent == 21 || nEvent == 0 || nEvent == 1 || nEvent == 3 || nEvent == 6 || nEvent == 7) {
        return 1;
    }
    return 0;
}

// HoleFinished: once the lessons are done (lesson 12), unless the step is 19.
u8 fn_80101C9C(int nPlayer, u8 bCheck) {
    if (gLessonNum == 12 && gLessonStep != 19) {
        return 1;
    }
    return 0;
}

// GameFinished: once the lessons are done (lesson 12).
u8 fn_80101CC4(u8 bCheck) {
    return gLessonNum == 12;
}

// EndGame.
void fn_80101CD8(void) {
    EASBio_IncrementGamesWon(1);
}

void fn_80101CFC(void) {
    gLessonQuitChosen = 1;
    Gaud_ExitCrowdReactionSound();
}

void fn_80101D24(void) {
    gLessonContinueChosen = 1;
    Gaud_ExitCrowdReactionSound();
}

// A CPU player in a lesson is always lucky, except in lessons 5, 8, 9 and 11; in lesson 7 only while
// player 0 is the CPU.
u8 fn_80101D4C(int nPlayer) {
    if (gLessonNum == 7 && gPlayers[0].nController != CONTROLLER_CPU) {
        return 0;
    }
    if (Game_GetMode() == 11 && gLessonNum != 5 && gLessonNum != 11 && gLessonNum != 8 &&
        gLessonNum != 9 && gPlayers[nPlayer].nController == CONTROLLER_CPU) {
        return 1;
    }
    return 0;
}

u8 fn_80101DF4(void) {
    if (Game_GetMode() == 11 && gLessonNum != 11) {
        return 0;
    }
    return 1;
}

// Is this one of the lessons' animations?
u8 fn_80101E34(char* szName) {
    int i;
    if (szName == 0) {
        return 0;
    }
    for (i = 0; i < 12; i++) {
        if (strcmp(gLessonTryAnims[i], szName) == 0 || strcmp(gLessonDemoAnims[i], szName) == 0) {
            return 1;
        }
    }
    return 0;
}

void fn_80101EDC(void) {
    gLessonPauseClosed = 1;
}

// Round setup (Lessons_StartGamePreData), then player 0 is handed to the first controller.
void fn_80101EE8(void) {
    Lessons_StartGamePreData();
    gPlayers[0].nController = 0;
}

void fn_80101F18(int nPlayer) {
    GameMsg_SendInt(43, nPlayer);
}

void fn_80101F40(u8 a, int b) {
    GameMsg_Send2Ints(15, a, b);
}

void fn_80101F70(void) {
    fn_800E58B4(40);
}

void fn_80101F94(int a, int b) {
    GameMsg_Send2Ints(38, a, b);
}

void fn_80101FC0(int a, int b) {
    Gaud_StartComment(9, a, b);
}
