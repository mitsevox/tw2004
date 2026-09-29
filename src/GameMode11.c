// GameMode11.c (our name): game mode 11, the lessons. One player (golfer 1) on hole 14 of course 10
// works through eleven lessons (gLessonNum, 1..11; 12 when all are done), each a shot from a set
// spot with its own shot kind, club and shape (gLessons): 1 a drive to the fairway, 2..6 shots to
// the green (a pitch, shot kind 5, a 5-iron punch, a short lob-wedge swing, a sand-wedge chip), 7
// a putt to hole, 8 and 9 a shot curved one way and then the other, 10 a drive with a power boost,
// 11 a shot with spin. Each lesson has the coach's lines (gLessonLines), a demonstration by the
// CPU, then the player's tries with swing hints and highlighted HUD items until one passes
// (Lessons_JudgeShot); Lessons_Update runs it all as a sequence of steps (gLessonStep). After
// lesson 7 the player has earned the first TOUR card level and may stop. The mode saves some of
// the player's options when it starts and puts them back when it ends. Golfer.c, Swing.c,
// ai_brain.c, CharAnim.c, skalib.c, stateFunc.c and the AI's club and shot choice ask it what the
// lesson allows; event.c lets it block or react to the game's events (Lessons_OnEvent).

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

// Each lesson's golfer animation (index 0 unused), picked by Lessons_GetAnimName: gLessonTryAnims
// from step 6 on (the player's tries), gLessonDemoAnims before it (the demonstration). Only lesson 6
// differs. Lessons_IsLessonAnim checks a clip name against both.
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

// The coach's lines (commentary kind 9 ids, -1 = none), 16 per row: 0 the lesson's opening line, 1
// before the demonstration, 2 after it, 3..7 a missed shot, 8..10 a fault, 11..12 a short shot,
// 13..14 several failings (Lessons_PlayLine). gLessonLineRow is the lesson's row: lessons 1, 8, 9,
// 2, 5, 6, 3, 4, 7, 10, 11 in that order, then the closing line.
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

s32 gLessonSavedOptionC = 4;       // the options' nWeather, saved while the mode runs
s32 gLessonStep;                    // the step Lessons_Update is at
s32 gLessonNextStep;                // where a waiting step (0, 1, 18) goes next
s32 gLessonLineRow;                 // the lesson's row in gLessonLines (a multiple of 16)
s32 gLessonBackswingHint;           // the value message 15 shows with the backswing hint (5; 4, 6)
s32 gLessonDownswingHint;           // the value message 15 shows with the downswing hint (2; 3, 1)
s32 gLessonHintSwapTimer;           // frames until the two swing hints swap (59)
s32 gLessonHighlightTimer;          // frames until the next HUD item is highlighted (389, then 83)
s32 gLessonHighlight;               // lessons 6 and 7: which of HUD items 4..7 is lit (4 = none yet)
u8  gLessonSavedCommentLevel;       // options a0[4] (the commentary level), saved
u8  gLessonSavedPuttGrid;           // options b84 (the putting grid), saved
u8  gLessonSavedBoost;              // the boost option, saved
u8  gLessonSavedSpin;               // the spin option, saved
s32 gLessonController;              // player 0's controller, kept while the CPU demonstrates
f32 gLessonHookSlice;               // the try's hook-slice (SW_vGetHookSlice): lessons 8 and 9
s32 gLessonNum;                     // the current lesson, 1..11 (12: all done)
u8  gLessonPauseClosed;             // the pause menu closed: send message 39 next frame
s32 gLessonPanel;                   // the value message 60 shows with the hints (the lesson - 1)
u8  gLessonShowBackswingHint;       // which of the two swing hints is showing (1: backswing)
u8  gLessonTryHintsSet;             // the player's try has set up its hints
u8  gLessonSkipPending;             // Lessons_StopWaiting goes to gLessonNextStep; never set
s32 gLessonSavedWind;               // the wind option, saved while the mode runs
u32 gLessonFailedTries;             // this lesson's failed tries: picks the coach's line
u8  gLessonSwingCommitted;          // the backswing passed its mark; lesson 11's spin hint shows
u8  gLessonQuitChosen;              // after lesson 7: the player chose to stop
u8  gLessonContinueChosen;          // after lesson 7: the player chose to go on
u8  gLessonWaitingForLine;          // step 1 is waiting for the coach's line to end
u8  gLessonBoostUsed;               // this try used a power boost (event 45): lesson 10
u8  gLessonSpinUsed;                // this try used spin (event 46): lesson 11

void  Gaud_ExitCrowdReactionSound(void);
void  GUI_ShowLessonText(int a);
u8    Gaud_GetCommentStatus(void);

void Lessons_LoadHole(void);
void Lessons_RestartHole(void);
void Lessons_Reset(void);
void Lessons_StartGamePreData(void);
void Lessons_Shutdown(void);
void Lessons_StopWaitingForLine(void);
void Lessons_PlaceBall(void);
int  Lessons_PlayLine(int nList, int nCount);
void Lessons_SetHintPhase(int nPhase);
void Lessons_ShowSwingHint(u8 bShow, int nHint);
void Lessons_StartLine(int nLine, int a);
void Lessons_AfterReplan(int nPlayer);
void Lessons_Update(void);
u8   Lessons_HoleFinished(int nPlayer, u8 bCheck);
u8   Lessons_GameFinished(u8 bCheck);
void Lessons_EndGame(void);
void Lessons_JudgeShot(void);
void Lessons_StartTry(void);
void Lessons_AskContinue(void);
void Lessons_HighlightHudItem(int nItem, int bOn);

// Mode 11 starts (pfnInit): its callbacks; the yardage, the stroke limit, gimmes, the flyovers
// (b27F, b280), setup tips, the re-plan button, the flight-camera toggles and in-flight replays
// off, with b275, b27B..b27D, bAllowGameBreakers and b28A; n290, nC, n10 and b276 (re-plan as the
// swing begins) set to 1; the ball's random rolls off (Physics_SetNoRandomRolls: lies and bounces
// come out as in a simulation). The options the lessons change are saved and set: commentary level
// 4, the putting grid off, power boost and spin on. Random stream 0 is seeded with 69, so the
// lessons play the same each time.
void Lessons_Init(void) {
    gpGame->pfnInit = Lessons_Init;
    gpGame->pfnShutdown = Lessons_Shutdown;
    gpGame->pfnHoleFinished = Lessons_HoleFinished;
    gpGame->pfnGameFinished = Lessons_GameFinished;
    gpGame->pfnLoadHole = Lessons_LoadHole;
    gpGame->pfnUpdate = Lessons_Update;
    gpGame->pfnRestartHole = Lessons_RestartHole;
    gpGame->pfnResetShot = Lessons_AfterReplan;
    gpGame->pfnStartGamePreData = Lessons_StartGamePreData;
    gpGame->pfnEndGame = Lessons_EndGame;
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
    gpGame->bAllowGameBreakers = 0;
    gpGame->b286 = 0;
    gpGame->b287 = 0;
    gpGame->b28A = 0;
    gpGame->n290 = 1;
    gpGame->nC = 1;
    gpGame->n10 = 1;
    gpGame->b276 = 1;
    Physics_SetNoRandomRolls(1);
    gLessonSavedCommentLevel = gSession.options.a0[4];
    gLessonSavedPuttGrid = gSession.options.bPuttingGrid;
    gLessonSavedBoost = gSession.options.bBoostEnabled;
    gLessonSavedSpin = gSession.options.bSpinEnabled;
    gSession.options.a0[4] = 4;
    gSession.options.bPuttingGrid = 0;
    gSession.options.bBoostEnabled = 1;
    gSession.options.bSpinEnabled = 1;
    Misc_SetSeedFunc(0, 69);
}

// Hole start (pfnLoadHole): the lessons start over (Lessons_Reset).
void Lessons_LoadHole(void) {
    Lessons_Reset();
}

// The hole restarts (pfnRestartHole): the lessons start over (Lessons_Reset).
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

// Before the round (pfnStartGamePreData; also Lessons_StartFromMenu): course 10 with only hole 14,
// tee set 0 and pin 0; the options' nWeather saved and set to 4 and the wind saved and set to calm
// (0); one player, golfer 1, played by the CPU (who demonstrates each lesson); no mulligans; the
// continue / stop answers cleared.
void Lessons_StartGamePreData(void) {
    GM_SetCurrentCourse(10);
    GM_SelectHoleSet(0);
    gpGame->bHoleSelected[13] = 1;
    GM_InitializeCurrentHoleToFirstSelected();
    gSession.nTeeSet[0] = 0;
    gSession.nTeeSet[1] = 0;
    gSession.nPinSet = 0;
    gpGame->nPinSet[Game_CurHoleIndex()] = 0;
    gLessonSavedOptionC = gSession.options.nWeather;
    gLessonSavedWind = gSession.options.nWind;
    gSession.options.nWeather = 4;
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
    gSession.options.nWeather = gLessonSavedOptionC;
    gSession.options.nWind = gLessonSavedWind;
    Physics_SetNoRandomRolls(0);
    // fake match: &gSession re-taken inside the first store after the call, as the original
    // recomputes it
    (pSession = &gSession)->options.a0[4] = gLessonSavedCommentLevel;
    (pSession)->options.bPuttingGrid = gLessonSavedPuttGrid;
    (pSession)->options.bBoostEnabled = gLessonSavedBoost;
    (pSession)->options.bSpinEnabled = gLessonSavedSpin;
}

// Is a lesson running (mode 11)?
u8 Lessons_IsRunning(void) {
    return Game_GetMode() == 11;
}

// The front end ends the current wait (UI command GM_vLessonStopWaiting), in mode 11 only: a step waiting for
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
        gSession.options.bPuttingGrid = 1;
        gLessonLineRow = 0x80;
        gLessonBackswingHint = 5;
        gLessonDownswingHint = 2;
        break;
    case 10:
        gSession.options.bPuttingGrid = 0;
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
        Lessons_ShowSwingHint(0, 0);
        GUI_ShowLessonText(-1);
        Lessons_SetHintPhase(0);
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
        Lessons_StartLine((u16)pList[i], 0);
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
// 7, and DynObj_ShotDivotHoleHide / DynObj_DivotHide are called with no ball.
void Lessons_StartTry(void) {
    Lessons_ShowSwingHint(0, 0);
    GUI_ShowLessonText(-1);
    Lessons_SetHintPhase(0);
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
        GR_ResetGreenGrid(gPlayers[0].nView[0]);
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
    DynObj_ShotDivotHoleHide(NULL, 0);
    DynObj_DivotHide(NULL, 0);
}

// After a re-plan (pfnResetShot): player 0's target is picked again (the default one for a putt,
// else the AI's choice) and the shot prepared, keeping the club and shot kind the lesson set; the
// putt line, the golfer's aim and animation 5 are set up again. nPlayer is unused.
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
    GR_ResetGreenGrid(gPlayers[0].nView[0]);
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
        fn_80062C80(gPlayers[0].nUISlot, 0);
        GUI_ToggleUI(0, 0);
        Lessons_ShowSwingHint(0, 0);
        Lessons_SetHintPhase(0);
        GUI_ShowLessonText(-1);
    }
}

// Whether the swing reads a neutral pad instead of the player's sticks (SW_vGetStickInfo): always 0
// in this build.
u8 Lessons_UseNeutralPad(void) {
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

// Every frame (pfnUpdate): runs the lesson's steps (gLessonStep). 2 starts the lessons (lesson 1,
// Lessons_NextLesson, Lessons_PlaceBall); 3 plays the lesson's opening line (no failed tries yet);
// 4 waits out the flyover and sets up the demonstration; 5 shows the swing hints while the CPU
// demonstrates; 6 starts a try (Lessons_StartTry) and 7 shows the hints during it (the backswing
// and downswing hints take turns every 59 frames; in lessons 6 and 7 HUD items 4..7 are highlighted
// in turn every 83 frames); 8..11 play the line for a missed, short, faulty or much-faulted try and
// start another; 12 goes on to the next lesson (through 19 after lesson 7 and after the last one).
// 1 waits for the line being spoken, 18 for the fade-out and 0 for button 0 (nothing sets it), each
// then going to gLessonNextStep; 14 waits for the line and then ends the turn; 15 and 16 end the
// turn (then 3 or 6); 13 quits (event 5). 19 waits for the camera, then after lesson 7 gives
// profile 0 its first TOUR card level and asks whether to go on (17: on to lesson 8 or quit), and
// after the last lesson waits for the closing line, fades out and quits. After the pause menu
// closes, message 39 is sent first.
void Lessons_Update(void) {
    f32 v[4] = {0.0f, 0.0f, 0.0f, 0.5f};
    int nView;
    if (gLessonPauseClosed) {
        GameMsg_Send(39);
        gLessonPauseClosed = 0;
    }
    switch (gLessonStep) {
    case 0:
        if (Input_ReadControlPad(gPlayers[0].nController) & Input_uiMap(0, 0)) {
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
        if (CameraController_IsFadeOutDone(ViewController_GetCameraControl(gPlayers[0].nView[0]))) {
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
        Lessons_ShowSwingHint(0, 0);
        Lessons_SetHintPhase(0);
        GUI_ShowLessonText(-1);
        Lessons_PlayLine(0, 1);
        gLessonStep = 4;
        gLessonFailedTries = 0;
        break;
    case 4:
        if ((s8)GOLFERSTATE_GetCurrentState(0) != 20) {
            Lessons_PlaceBall();
            GOLFERSTATE_Switch(GS_PRE_SHOT, 0);
            Lessons_ShowSwingHint(0, 0);
            GUI_ShowLessonText(-1);
            Lessons_SetHintPhase(0);
            Lessons_PlayLine(1, 1);
            gLessonStep = 5;
        }
        break;
    case 5:
        if ((s8)GOLFERSTATE_GetCurrentState(0) != GS_PRE_SHOT) {
            if (gPlayers[0].swing.nState == 1 || gPlayers[0].swing.nState == 2) {
                Lessons_HighlightHudItem(2, 0);
                Lessons_ShowSwingHint(1, gLessonBackswingHint);
                GUI_ShowLessonText(gLessonPanel);
                Lessons_SetHintPhase(1);
                if (gLessonNum == 10) {
                    Lessons_HighlightHudItem(0, 1);
                }
            } else if (gPlayers[0].swing.nState == 3 || gPlayers[0].swing.nState == 4 ||
                       gPlayers[0].swing.nState == 5) {
                Lessons_HighlightHudItem(0, 0);
                if (gLessonNum == 11 && gPlayers[0].swing.nState == 5) {
                    if (gLessonSwingCommitted) {
                        Lessons_ShowSwingHint(1, gLessonBackswingHint);
                        GUI_ShowLessonText(gLessonPanel);
                        Lessons_SetHintPhase(3);
                        Lessons_HighlightHudItem(2, 1);
                    } else {
                        Lessons_ShowSwingHint(0, 0);
                        GUI_ShowLessonText(-1);
                        Lessons_SetHintPhase(0);
                    }
                } else {
                    Lessons_ShowSwingHint(1, gLessonDownswingHint);
                    GUI_ShowLessonText(gLessonPanel);
                    Lessons_SetHintPhase(2);
                    Lessons_HighlightHudItem(2, 0);
                }
            } else {
                Lessons_ShowSwingHint(1, 0);
                GUI_ShowLessonText(gLessonPanel);
                Lessons_HighlightHudItem(0, 0);
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
                    Lessons_ShowSwingHint(1, gLessonBackswingHint);
                    GUI_ShowLessonText(gLessonPanel);
                    Lessons_SetHintPhase(1);
                    if (gLessonNum == 10) {
                        Lessons_HighlightHudItem(0, 1);
                    }
                } else {
                    Lessons_ShowSwingHint(1, gLessonDownswingHint);
                    GUI_ShowLessonText(gLessonPanel);
                    Lessons_SetHintPhase(2);
                    if (gLessonNum == 10) {
                        Lessons_HighlightHudItem(0, 0);
                    }
                }
                if ((gLessonNum == 6 || gLessonNum == 7) && --gLessonHighlightTimer <= 0) {
                    gLessonHighlightTimer = 83;
                    if (gLessonHighlight == 4) {
                        gLessonHighlight = 0;
                        Lessons_HighlightHudItem(4, 1);
                        Lessons_HighlightHudItem(5, 0);
                        Lessons_HighlightHudItem(6, 0);
                        Lessons_HighlightHudItem(7, 0);
                    } else {
                        Lessons_HighlightHudItem(Hint(), 0);
                        gLessonHighlight++;
                        gLessonHighlight %= 4;
                        Lessons_HighlightHudItem(Hint(), 1);
                    }
                }
            } else {
                gLessonHighlightTimer = 389;
                gLessonHighlight = 4;
                gLessonHintSwapTimer = 59;
                Lessons_ShowSwingHint(1, gLessonBackswingHint);
                GUI_ShowLessonText(gLessonPanel);
                Lessons_SetHintPhase(1);
                gLessonTryHintsSet = 1;
                gLessonShowBackswingHint = 1;
                if (gLessonNum == 6 || gLessonNum == 7) {
                    Lessons_HighlightHudItem(4, 1);
                    Lessons_HighlightHudItem(5, 1);
                    Lessons_HighlightHudItem(6, 1);
                    Lessons_HighlightHudItem(7, 1);
                } else {
                    Lessons_HighlightHudItem(4, 0);
                    Lessons_HighlightHudItem(5, 0);
                    Lessons_HighlightHudItem(6, 0);
                    Lessons_HighlightHudItem(7, 0);
                }
                if (gLessonNum == 10) {
                    Lessons_HighlightHudItem(0, 1);
                } else if (gLessonNum == 8) {
                    Lessons_HighlightHudItem(5, 1);
                } else if (gLessonNum == 9) {
                    Lessons_HighlightHudItem(4, 1);
                } else if (gLessonNum == 2) {
                    Lessons_HighlightHudItem(8, 1);
                }
            }
        } else {
            Lessons_HighlightHudItem(4, 0);
            Lessons_HighlightHudItem(5, 0);
            Lessons_HighlightHudItem(6, 0);
            Lessons_HighlightHudItem(7, 0);
            Lessons_HighlightHudItem(8, 0);
            if (gPlayers[0].swing.nState == 2 || gPlayers[0].swing.nState == 3 ||
                (gPlayers[0].swing.nState == 1 &&
                 ((gLessonNum == 5 && gPlayers[0].pChar->fBackswing > 0.45f) ||
                  (gLessonNum != 5 && gPlayers[0].pChar->fBackswing > 0.75f)))) {
                Lessons_ShowSwingHint(1, gLessonDownswingHint);
                GUI_ShowLessonText(gLessonPanel);
                Lessons_SetHintPhase(2);
                gLessonSwingCommitted = 1;
                if (gLessonNum == 10) {
                    Lessons_HighlightHudItem(0, 0);
                } else if (gLessonNum == 11) {
                    Lessons_HighlightHudItem(2, 0);
                }
            } else if (gPlayers[0].swing.nState == 5 || gPlayers[0].swing.nState == 4) {
                gLessonHookSlice = SW_vGetHookSlice(0);
                if (gLessonNum == 11) {
                    Lessons_HighlightHudItem(2, 1);
                    if (!gLessonSwingCommitted) {
                        Lessons_ShowSwingHint(0, 0);
                        GUI_ShowLessonText(-1);
                        Lessons_SetHintPhase(0);
                    } else {
                        Lessons_ShowSwingHint(1, gLessonBackswingHint);
                        GUI_ShowLessonText(gLessonPanel);
                        Lessons_SetHintPhase(3);
                    }
                } else {
                    Lessons_ShowSwingHint(0, 0);
                    GUI_ShowLessonText(-1);
                    Lessons_SetHintPhase(0);
                }
            } else {
                Lessons_ShowSwingHint(1, gLessonBackswingHint);
                GUI_ShowLessonText(gLessonPanel);
                Lessons_SetHintPhase(1);
                if (gLessonNum == 10) {
                    Lessons_HighlightHudItem(0, 1);
                }
            }
        }
        break;
    case 8:
        if (gLessonNum == 10) {
            Lessons_HighlightHudItem(0, 1);
        } else if (gLessonNum == 8) {
            Lessons_HighlightHudItem(5, 1);
        } else if (gLessonNum == 9) {
            Lessons_HighlightHudItem(4, 1);
        }
        Lessons_StartTry();
        Lessons_PlayLine(3, 5);
        break;
    case 9:
        if (gLessonNum == 10) {
            Lessons_HighlightHudItem(0, 1);
        } else if (gLessonNum == 8) {
            Lessons_HighlightHudItem(5, 1);
        } else if (gLessonNum == 9) {
            Lessons_HighlightHudItem(4, 1);
        }
        Lessons_StartTry();
        Lessons_PlayLine(11, 2);
        break;
    case 10:
        if (gLessonNum == 10) {
            Lessons_HighlightHudItem(0, 1);
        } else if (gLessonNum == 8) {
            Lessons_HighlightHudItem(5, 1);
        } else if (gLessonNum == 9) {
            Lessons_HighlightHudItem(4, 1);
        }
        Lessons_StartTry();
        Lessons_PlayLine(8, 3);
        break;
    case 11:
        if (gLessonNum == 10) {
            Lessons_HighlightHudItem(0, 1);
        } else if (gLessonNum == 8) {
            Lessons_HighlightHudItem(5, 1);
        } else if (gLessonNum == 9) {
            Lessons_HighlightHudItem(4, 1);
        }
        Lessons_StartTry();
        Lessons_PlayLine(13, 2);
        break;
    case 12:
        if (gLessonNum == 10) {
            Lessons_HighlightHudItem(2, 1);
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
        gSession.bEndLoop = 1;
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
        if (CameraController_IsFadeOutDone(ViewController_GetCameraControl(gPlayers[0].nView[0]))) {
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
            Lessons_AskContinue();
            gLessonContinueChosen = 0;
            gLessonQuitChosen = 0;
            gLessonStep = 17;
        }
        break;
    }
}

// Whether the ball's flight gets its camera (mode 14, STATEFUNC_SimulateInit): not during the
// demonstration (step 5) of the tee-shot lessons 1, 8, 9 and 10, which is cut short at event 28
// (Lessons_OnEvent); always otherwise.
u8 Lessons_AllowFlightCamera(void) {
    if (Game_GetMode() == 11 && gLessonStep == 5 &&
        (gLessonNum == 1 || gLessonNum == 10 || gLessonNum == 8 || gLessonNum == 9)) {
        return 0;
    }
    return 1;
}

// Judges the shot when it ends. The demonstration's shot (step 5) only plays its closing line and
// goes to step 16. A try: lessons 1 and 10 want a length of 260 (else short), lesson 1 the fairway
// (else a fault), lesson 10 a power boost (else missed and a fault); lesson 8 wants a hook-slice of
// at least 0.01 and lesson 9 of at most -0.01; lessons 2..5 the green or the cup, lesson 7 the cup,
// lesson 11 spin (else missed) and the green or the cup (else a fault). Two failings or more go to
// step 11, a short shot to 9, a miss to 8, a fault to 10, each with crowd reaction sound 5 and one
// more failed try; a pass goes to step 12 (crowd reaction sound 3 after lessons 7 and 11, else 1).
// Lesson 6 is never tested (see the EA bug below), so its every try passes.
void Lessons_JudgeShot(void) {
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
    fLength = GameAnalysis_GetCurrentBallFlightDistance(0);
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

// Asked first by event.c's handlers with their event number; nonzero blocks the event's usual work
// (0 outside mode 11). Event 10 picks the crowd reaction sound (2 in the tee-shot lessons 1, 8, 9
// and 10 outside the demonstration, else 0); 29 clears gLessonSwingCommitted; 28 ends the
// demonstration of a tee-shot lesson (its closing line, step 16); 32 and 34 end the shot, which is
// judged (Lessons_JudgeShot); 46 (spin) and 45 (a power boost tap) are noted for lessons 11 and 10.
// Blocked: events 28 (in that case), 32, 34, 45, 46, 4, and 0, 1, 3, 6, 7, 13..17, 20 and 21,
// except 20 and 21 in lessons 6 and 7.
u8 Lessons_OnEvent(int nPlayer, int nEvent) {
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
        Lessons_JudgeShot();
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

// The hole is over (pfnHoleFinished) once the lessons are done (lesson 12) and the closing camera
// (step 19) has finished. nPlayer and bCheck are unused.
u8 Lessons_HoleFinished(int nPlayer, u8 bCheck) {
    if (gLessonNum == 12 && gLessonStep != 19) {
        return 1;
    }
    return 0;
}

// The game is over (pfnGameFinished) once the lessons are done (lesson 12).
u8 Lessons_GameFinished(u8 bCheck) {
    return gLessonNum == 12;
}

// Game finished (pfnEndGame): one more game won in the EA Sports Bio.
void Lessons_EndGame(void) {
    EASBio_IncrementGamesWon(1);
}

// The player chose to stop after lesson 7 (UI command GM_vLessonsQuit): step 17 goes on to quitting.
// The crowd sound ends.
void Lessons_ChooseQuit(void) {
    gLessonQuitChosen = 1;
    Gaud_ExitCrowdReactionSound();
}

// The player chose to go on after lesson 7 (UI command GM_vLessonsContinue): step 17 goes on to lesson 8.
// The crowd sound ends.
void Lessons_ChooseContinue(void) {
    gLessonContinueChosen = 1;
    Gaud_ExitCrowdReactionSound();
}

// A CPU player in a lesson is always lucky, except in lessons 5, 8, 9 and 11; in lesson 7 only while
// player 0 is the CPU.
u8 Lessons_IsLucky(int nPlayer) {
    if (gLessonNum == 7 && gPlayers[0].nController != CONTROLLER_CPU) {
        return 0;
    }
    if (Game_GetMode() == 11 && gLessonNum != 5 && gLessonNum != 11 && gLessonNum != 8 &&
        gLessonNum != 9 && gPlayers[nPlayer].nController == CONTROLLER_CPU) {
        return 1;
    }
    return 0;
}

// Whether a CPU's shot may get spin from its aim error (AI_ApplyError): in mode 11 only in lesson
// 11 (the spin lesson); always outside it.
u8 Lessons_AllowCPUSpin(void) {
    if (Game_GetMode() == 11 && gLessonNum != 11) {
        return 0;
    }
    return 1;
}

// Is this one of the lessons' animations?
u8 Lessons_IsLessonAnim(char* szName) {
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

// The pause menu closed during a lesson (GUI_PauseMenuClosed): the next update sends front-end
// message 39.
void Lessons_PauseMenuClosed(void) {
    gLessonPauseClosed = 1;
}

// The front end starts the lessons (GM_vStartEventCheckDisc): the round's setup (Lessons_StartGamePreData,
// which also puts golfer 1 in place of the golfer the front end just chose) with player 0 on the
// first controller.
void Lessons_StartFromMenu(void) {
    Lessons_StartGamePreData();
    gPlayers[0].nController = 0;
}

// Front-end message 43: which swing hint phase shows: 0 none, 1 the backswing, 2 the downswing, 3
// the spin of lesson 11.
void Lessons_SetHintPhase(int nPhase) {
    GameMsg_SendInt(43, nPhase);
}

// Front-end message 15: the swing hint shown (bShow 1) with its value (gLessonBackswingHint or
// gLessonDownswingHint; 0 in the demonstration's other swing states) or hidden (0, 0).
void Lessons_ShowSwingHint(u8 bShow, int nHint) {
    GameMsg_Send2Ints(15, bShow, nHint);
}

// Front-end message 40, sent after lesson 7: the question whether to go on, answered by
// Lessons_ChooseContinue or Lessons_ChooseQuit.
void Lessons_AskContinue(void) {
    GameMsg_Send(40);
}

// Front-end message 38: HUD item nItem (0..8) highlighted (bOn 1) or not. The lessons use item 0
// (lesson 10), 2 (lessons 10 and 11), 4..7 (the four hints of lessons 6 and 7; 5 in lesson 8, 4 in
// lesson 9) and 8 (lesson 2).
void Lessons_HighlightHudItem(int nItem, int bOn) {
    GameMsg_Send2Ints(38, nItem, bOn);
}

// Plays the coach's line nLine (commentary kind 9); a goes on to Gaud_StartComment (always 0 here).
void Lessons_StartLine(int nLine, int a) {
    Gaud_StartComment(9, nLine, a);
}
