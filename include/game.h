// game.h (our name): the round and the game modes: the current hole, the golfer states, and the
// game manager and game-mode code at 0x800D0000..0x80100000 that more than one file calls.
// Players and golfers are in golfer.h, the ball in ball.h.

#ifndef GAME_H
#define GAME_H

#include "golfer.h"
#include "ball.h"

// ---- the round -------------------------------------------------------------------------------

extern s32 lbl_80282278;                // the player whose turn it is
extern u8  lbl_8028227C;                // the split-screen choice (fn_800E25CC); the modes that
                                        // force one view put it back when they end

// A saved shot (gReplayData, 0x801D6030): the seed, player 0 as it was, and the conditions.
typedef struct Replay {
    u32    nSeed;               // 0x000
    u8     unk4[4];
    Player player;              // 0x008  player 0 before the shot
    s32    nCourse;             // 0xF00
    s16    nHole;               // 0xF04
    s8     nTeeSet;             // 0xF06
    s8     nPinSet;             // 0xF07  the session's pin set when the shot was saved
    f32    fF08;                // 0xF08
    f32    fF0C;                // 0xF0C
    u8     bF10;                // 0xF10  in-flight replays are on (GameManager.c)
    u8     unkF11;
    s16    nF12;                // 0xF12  1..3: fn_800ED6F8 is set from nF14
    s16    nF14;                // 0xF14  hundredths
    s16    nWindDir;            // 0xF16
    s16    nWindSpeed;          // 0xF18
    s16    nF1A;                // 0xF1A  -> fn_80055C40
    s16    nF1C;                // 0xF1C  -> fn_80055CAC
    s16    nF1E;                // 0xF1E  -> fn_80055CD0
    s16    nStrokes;            // 0xF20  strokes on the hole before the shot
} Replay;
LAYOUT_ASSERT(Replay, 0xF28);

extern Replay gReplayData;              // 0x801D6030

#define NUM_COURSES 30
extern char* lbl_80191990[NUM_COURSES]; // each course's name ("Pebble Beach", ...)

// The replay recorder's buffer (our name; 0x15260 bytes, made by fn_8006BED4 at the start of a
// round). Only the flag is read so far.
typedef struct ReplayBuffer {
    u8   unk0[0x1525C];
    u8   b1525C;                // 0x1525C  set by fn_8006C5E0, cleared by fn_8006C608 (not in a replay)
    u8   unk1525D[3];
} ReplayBuffer;
LAYOUT_ASSERT(ReplayBuffer, 0x15260);

extern ReplayBuffer* lbl_80281E48;      // 0x80281E48

// gomainloop.c
extern u8    lbl_80281B8E;              // the reset button was pressed (OSGetResetButtonState)
extern u8    lbl_80281E50;              // set after a create-a-player frame, cleared otherwise
extern void*       lbl_80281E54;        // the render camera made from the three below (fn_8001371C)
extern void*       lbl_80281E58;        // } made by fn_80076ACC,
extern GoFrameBuf* lbl_80281E5C;        // }   fn_8006E1C8
extern void*       lbl_80281E60;        // }   and fn_80076400 when a game type starts
extern s32   lbl_801888D0[4];           // the order the views are drawn in (0, 1, 2, 3)
extern u8*   lbl_802811E8;              // [1]: the round is over (fn_8006DC34)

// Replay.c
void fn_8006BED4(void);                 // make the replay buffer
void fn_8006BF20(void);                 // free it
void fn_8006BF4C(void);                 // in-flight replays off (gReplayData.bF10)
void fn_8006C28C(int nPlayer, int nController);
void fn_8006C2A8(int nPlayer, f32 fForwardSpin, f32 fSideSpin);
void fn_8006C2C8(int nPlayer, f32* pForwardSpin, f32* pSideSpin);
void fn_8006C5E0(void);
void fn_8006C608(void);

int  Game_GetCourse(void);              // 0x80008830
int  Game_CurHoleIndex(void);           // 0..17 in the round (Golfer.c)
int  Game_CurrentPinSet(void);          // the current hole's pin position, 0..3 (Golfer.c)
void Session_SetNumPlayers(int n);      // Golfer.c

// ---- golfer states (Swing.c) -----------------------------------------------------------------

// A row of sGolferStateEngineTable: a golfer state's callbacks.
typedef struct SwingStateDef {
    void (*pfnEnter)(int nPlayer);  // 0x00
    void (*pfnUpdate)(int nPlayer); // 0x04
    void (*pfnExit)(int nPlayer);   // 0x08
} SwingStateDef;

extern SwingStateDef sGolferStateEngineTable[GS_NUM];   // 0x801883D8  one row per GS_* state

u8   fn_80058F5C(int nPlayer);          // the per-frame swing poll: the ball was struck
void fn_80058FA4(int nPlayer);          // reset the player's swing
f32  fn_8005B64C(int nPlayer);          // the swing's shot power
void GOLFERSTATE_Push(int nState, int nPlayer);     // push a state and run its enter callback
void GOLFERSTATE_Set(int nState, int nPlayer);      // pop everything and start again from one state
void GOLFERSTATE_Switch(int nState, int nPlayer);   // replace the current state
// GS_*, or -1. The definition returns the state byte zero-extended (lbzx, Swing.c) and -1 as a full
// int, so its return type is int. Many callers sign-extend the result, as if their own prototype
// said s8; they write (s8)GOLFERSTATE_GetCurrentState(n), a fake match (without it GameMode8 falls
// to 61/66). The same holds for the (u8) some callers put on GOLFERSTATE_Set's player (GameMode8,
// GameRound). Both casts are harmless for the values these take (-1..0x30, 0..4).
int  GOLFERSTATE_GetCurrentState(int nPlayer);
u8   fn_8005D2DC(void);
void STATEFUNC_SimulateInit(int nPlayer);
void STATEFUNC_SimulateUpdate(int nPlayer);
void STATEFUNC_SimulateExit(int nPlayer);
void fn_80062B70(void);
void fn_80062B74(int nPlayer);
void fn_80062B78(int nPlayer);
void fn_80062C38(void);
void fn_80062C5C(void);
void fn_80062C80(int a, u8 b);
void fn_80062CB0(int a, u8 b);
void fn_80062CE0(u8 a);
void fn_80062D0C(int nPlayer);
void fn_80062D38(int nMsg, int nA, int nB);    // send message nMsg with two values (fn_800E5998)
void fn_80062D6C(int nMsg, int nValue);        // send message nMsg with one value (fn_800E590C)

// What lbl_802811F0 points to (its code, around 0x8006F650, is not decompiled; fn_8006F608 clears
// it). GameMode5's fn_800ED6F8 sets f18 and flags it in b1C; SitDevFile.c tests the flags.
typedef struct Unk802811F0 {
    u32 uFlags;                 // 0x00  bit 0x2: fn_80035574
    u32 u04;                    // 0x04  bit 0x2: fn_800BCC38
    s32 n08;                    // 0x08
    s32 n0C;                    // 0x0C
    s32 n10;                    // 0x10
    u8  b14;                    // 0x14  1 after fn_8006F608
    u8  unk15[3];
    f32 f18;                    // 0x18
    u8  b1C;                    // 0x1C
} Unk802811F0;
extern Unk802811F0* lbl_802811F0;       // 0x802811F0

u8   fn_80035574(void);                 // lbl_802811F0's flag 0x2

// ---- the course table (CourseData.c) ---------------------------------------------------------

#define NUM_COURSE_DATA 21      // courses in the 'CRI ' table
#define NUM_BUILT_ROUNDS 7      // rounds in the 'CMPS' table

// One hole of the course table (0x38 bytes).
typedef struct HoleData {
    s32  nPar;                  // 0x00
    s32  n04;                   // 0x04
    s32  n08;                   // 0x08  per tee set (fn_800D2B80): tee 3
    s32  n0C;                   // 0x0C  tee 2
    s32  n10;                   // 0x10  tee 1
    s32  n14;                   // 0x14  tee 0
    s32  nWindDir;              // 0x18  (Hole_WindDir)
    u8   unk1C[0x2C - 0x1C];
    f32  fWindSpeed;            // 0x2C  (Hole_WindSpeed)
    u8   unk30[4];
    u8   b34;                   // 0x34
    u8   b35;                   // 0x35
    u8   unk36;
    u8   b37;                   // 0x37  checked at the end of a PGA Tour hole
} HoleData;
LAYOUT_ASSERT(HoleData, 0x38);

// One course of the table (0x430 bytes), read from the disc's 'CRI ' chunk.
typedef struct CourseData {
    HoleData aHoles[18];        // 0x000
    struct {
        s32  n0;                // +0x0
        s32  nPar;              // +0x4  the course's par from this tee set (fn_800D2F00)
        u8   unk8[8];
    } aTeeSets[4];              // 0x3F0
} CourseData;
LAYOUT_ASSERT(CourseData, 0x430);

// A round built from other courses' holes (0x24 bytes), from the 'CMPS' chunk: for each of its 18
// holes, the course and the hole's number there (1-based).
typedef struct BuiltRound {
    struct {
        u8   nCourse;
        u8   nHole;
    } aHoles[18];
} BuiltRound;
LAYOUT_ASSERT(BuiltRound, 0x24);

extern BuiltRound lbl_801FA1F8[NUM_BUILT_ROUNDS];    // 0x801FA1F8
extern CourseData lbl_801FA2F4[NUM_COURSE_DATA];     // 0x801FA2F4

void fn_800D29E8(void);
int  Hole_WindDir(void);
f32  Hole_WindSpeed(void);
s32  fn_800D2F00(int nCourse, int nTeeSet);    // a course's par from a tee set
s32  fn_800D2FB4(s32 nTeeSet);          // the course's par (the tee set is not used)
u8   fn_800D3080(int nHole);
int  fn_800D3118(int nRound, int nHole);    // a built round's course for a hole
int  fn_800D315C(int nRound, int nHole);    // and its hole number (1-based)
int  fn_800D31A4(int nPar);             // the number of the 18 holes with that par

// ---- the game manager ------------------------------------------------------------------------

f32  fn_800D0478(int nPlayer);          // the ball's distance from the pin (yards)
f32  fn_800D0550(int nPlayer);          // the shot's length
int  Hole_ScoreAfterTapIn(int nPlayer); // HoleScore.c
u8   fn_800CF158(int nPlayer);          // gpGame->pfn1F8: holing this ball takes the lead
u8   fn_800CF450(int nPlayer);          // gpGame->pfn1FC: holing this ball wins
s32  fn_800CFE74(int nPlayer);          // gpGame->pfn200: the lead so far
s32  fn_800D0098(int nPlayer);          // gpGame->pfn204: the lead if this ball drops
s32  fn_800D030C(int nPlayer);          // gpGame->pfn208: how the hole ends if it drops
int  fn_800D0620(int nPlayer, u8 bCurrent, u8 bOnlyFlagged);   // holes under par so far
int  fn_800D06FC(int nPlayer, u8 bCurrent, u8 bOnlyFlagged);   // two under par or better so far
int  fn_800D07D8(int nPlayer, u8 bCurrent);   // the current run of holes under par
int  fn_800D089C(int nPlayer, u8 bCurrent);   // the current run of holes two under par or better
u8   fn_800D0AF4(void);                 // nobody took anything on the last hole played
s32  fn_800BCCCC(int nPlayer);          // SitDevFile.c: gpGame->pfn208
s32  fn_800BCCF8(int nPlayer);          // SitDevFile.c: strokes behind the leader (gpGame->pfn200)
u8   fn_800BCD50(void);                 // SitDevFile.c: gpGame->bD4
void fn_800D2714(u16* pDate, s32* pMonth, s32* pDay, s32* pYear);
void fn_800D2678(u16* pDate, s32 nMonth, s32 nDay, u32 nYear);    // make a date
void fn_800D27CC(u16* pDate, s32 nDays);        // move a date on by nDays
s32  fn_800D27E0(u16* pDate);                   // its day of the week, 1..7
s32  fn_800D2814(u32 nMonth, u32 nYear);        // the days in a month (compared unsigned: cmplwi)
void fn_800D2884(s32 nMonth, s32 nYear, s32* pMonth, s32* pYear);  // the month before
void fn_800D28B0(s32 nMonth, s32 nYear, s32* pMonth, s32* pYear);  // the month after
void fn_800D28DC(u16 nDate, char* szOut);       // a date as text
void fn_800D293C(u16 nDate, char* szOut);       // a date as month/day
s32  fn_800D2608(u16 nDate);            // Calendar.c
s32  fn_800D2640(u16 nDate);            // Calendar.c
u16  fn_800D2994(void);                 // today's date
int  fn_800D2ABC(int nCourse, int nHole);   // a hole's par on a course
int  fn_800D2AD8(int nHole);            // a hole's par
s32  fn_800D2C30(int nHole, int nTee);  // CourseData.c: a round hole's length from tee set nTee
s32  fn_800D3478(int nTotal, int n, int nRow);  // Earnings.c: the prize for a finishing row
void fn_800D3548(int nPlayer, int nMoney, CourseMoneyTracking* pMoney);   // pMoney may be NULL
int  fn_800D36E0(int nWinner, int nLoser, int nMargin, int* pPrize);
int  fn_800D37BC(int nWinner, int nLoser, int nMargin, int* pPrize);
int  fn_800D3C7C(int nPlayer);          // the player's earnings rating, 0..25
s32  fn_800D6A70(s32 nPoints, int nPlayer, u8 bCourse, u8 bTee, u8 bHole, CourseMoneyTracking* pMoney);
int  fn_800D7220(int nReward, int nPlayer, CourseMoneyTracking* pMoney);
s32  fn_800D7660(int nPlayer, Ball* pBall, u8 b);   // one of GameEffects' GameBreaker checks
s32  fn_800D7684(int nPlayer, Ball* pBall, u8 b);   // the same through fn_800D4F14 (pBall unused)
u8   fn_800D8DB4(int nKind);            // Earnings.c: whether goals of a kind count now
s32  fn_800D477C(int nPlayer, Ball* pBall, u8 b);   // Earnings.c: the shot's check
void fn_800D4F14(int nPlayer, u8 b);                // the putt's
void fn_800D588C(int nPlayer, u8 a, u8 bRoundOver); // the hole's
s32  fn_800D9954(void);                 // lbl_80282250: the entries in the three lists below
s32  fn_800D995C(s32 i);                // lbl_802002B8[i]
s32  fn_800D9970(s32 i);                // lbl_80200290[i]
s32  fn_800D9984(s32 i);                // lbl_80200268[i]
int  fn_800D782C(int nPlayer, Ball* pBall, int a, u8 bCountStroke, u8 bAll);
int  fn_800D7B1C(int nPlayer, Ball* pBall, int a, u8 bCountStroke, u8 bAll);   // the putt record check
s32  fn_8008AB40(void);                 // GameUICommands.c
s32  fn_8008AC00(void);                 // GameUICommands.c: the round's holes left, the current one included

// GameHoleContests.c: the longest-drive, closest-to-the-pin and hole-in-one contests
u8   fn_800DA174(void);                 // the longest drive is played on this hole
u8   fn_800DA1D4(void);                 // closest to the pin is played on this hole
u8   fn_800DA234(void);                 // the hole-in-one prize is on this hole
u8   fn_800DA264(void);                 // the player whose turn it is is on the tee
u8   fn_800DA2AC(void);                 // every player has teed off on the contest hole
void fn_800DA36C(void);
void fn_800DA48C(int nPlayer);
void fn_800DAD54(void);                 // pay the contest's winner

// GameEffects.c: slow motion, the GameBreaker (letterbox), the heartbeat rumble. The effects
// state (0x58 bytes):
typedef struct GameEffects {
    u8   bSlowMo;               // 0x00  super slow motion is on
    u8   unk1[3];
    f32  fSlowMo;               // 0x04  its rate (below 1 slows down)
    u8   unk8;
    u8   b9;                    // 0x09
    u8   unkA[2];
    f32  fC;                    // 0x0C
    u8   b10;                   // 0x10  half speed
    u8   b11;                   // 0x11  quarter-ish speed, counted in n28
    u8   bGameBreaker;          // 0x12  the letterbox is up
    u8   unk13;
    s32  nGBType;               // 0x14  0 scripted, 1 predicted
    u8   bClosing;              // 0x18  the letterbox is closing
    u8   b19;                   // 0x19
    u8   unk1A[2];
    f32  fGBTime;               // 0x1C  seconds the letterbox has been opening (or left closing)
    u8   unk20[4];
    f32  f24;                   // 0x24
    s32  n28;                   // 0x28  frames at the slow rate
    s32  n2C;                   // 0x2C  every n2C-th of them the ball moves
    s32  nPlayer;               // 0x30  whose shot
    u8   unk34[8];
    u8   bSpinWindowDone;       // 0x3C
    u8   unk3D[3];
    s32  nRumbleFrames;         // 0x40
    u8   bRumble;               // 0x44
    u8   nHeartbeats;           // 0x45
    u8   bPaused;               // 0x46
    u8   b47;                   // 0x47  u48 holds a sound to stop
    u16  u48;                   // 0x48
    u8   b4A;                   // 0x4A  u4C holds a sound to stop
    u8   unk4B;
    u16  u4C;                   // 0x4C
    u8   b4E;                   // 0x4E
    u8   n4F;                   // 0x4F  the music to go back to
    u32  uFlags;                // 0x50  bit 0x4000: an eagle on a par 5 counts
    f32  f54;                   // 0x54
} GameEffects;
LAYOUT_ASSERT(GameEffects, 0x58);

extern GameEffects lbl_80202898;        // 0x80202898

int  fn_800DB86C(int nPlayer);          // this lie is worth a GameBreaker (GameEffects.c)
u8   fn_800DC818(Ball* pBall, int nPlayer, u8 bNext);
u8   fn_800DC784(void);                 // TW06: GameEffects_SkipOtherCommentary
void fn_800DB30C(int nPlayer, int nReason);
void fn_800DBA50(int nPlayer);
GameEffects* fn_800DAF74(void);
void GameEffects_ResetGameEffectSettings(void);
int  GameEffects_BallUpdatesThisFrame(int nPlayer);   // preview speed: ghost steps per frame
void fn_800DB4E8(int nPlayer);
void fn_800DB714(int nPlayer);
void fn_800DBDA8(int nPlayer);
void GameEffects_SetSuperSlowMo(u8 bOn, int nPlayer, f32 fRate);
f32  fn_800DC3A4(void);                 // the letterbox's field-of-view change
u8   fn_800DC514(int nPlayer);         // super slow motion is on (nPlayer unused)
void fn_800DC9D4(int a);                // pause or resume a GameBreaker
u8   fn_8003DCAC(void);                 // GoDynamicCam.c: the letterbox is up (predicted, or b19)

// GameManager.c
void fn_800DCAD8(void);
void GM_vCloseModuleONCE(void);
u8   fn_800DCB00(void);
u8   fn_800DCB08(void);
u8   fn_800DCB10(int nPlayer);          // the mode's pfn1F8 answer for the player
u8   fn_800DCB3C(void);
u8   fn_800DCB74(void);
void fn_800DCB84(f32* pA, f32* pB, f32* pOut);   // out = a - b
int  GM_GotoNextSelectedHole(void);
void GM_EndOfGolferTurn(int nPlayer);
void GM_BallHit(int nPlayer);
void GM_PlayerAddStroke(int nPlayer);
u8   GM_CheckForBallOOB(int nPlayer);
void GM_BumpBallForObstructions(int nPlayer);
void GM_PlayerTookShot(int nPlayer);
u8   GM_PlayerTakeMulligan(int nPlayer);
int  fn_800DDFB4(int nPlayer);
int  GM_ShowPostShotAnimation(int nPlayer);
u8   GM_ShowPostShotCrowdFlyby(void);
void GM_FlyByMode_Init(void);
void GM_ReplaceOOBBall(int nPlayer);
void fn_800DEB5C(int nPlayer);
void GM_MovePlayerToBall(int nPlayer);
void GM_CheckForShotChanges(int nPlayer);
void GM_DoPostShotInHoleUI(int nPlayer);
int  GM_ChooseRemoveBallState(int nPlayer);
void GM_SimulateBallMovement(int nPlayer);
void fn_800E0A84(u8 b);
void fn_800E0A98(int a);
void fn_800E0AC4(int a);

// GameRound.c
void fn_800E0AF0(f32* pA, f32* pB, f32* pOut);
void fn_800E0B14(f32* pA, f32* pB, f32* pOut);
void fn_800E0B38(int nMode);            // set up game mode nMode: defaults, then the mode's own setup
void fn_800E1018(int nPlayer, int nHole);
void fn_800E1074(void);
void fn_800E1260(int nPreset);          // a hole-selection preset for the round
void fn_800E1404(int nHole);
void fn_800E1434(void);
void fn_800E1480(int nHole);            // make a hole of the round the current one
void fn_800E14E0(int nCourse);
u8   fn_800E1734(void);                 // the current hole is the round's first
int  fn_800E177C(void);
int  fn_800E1788(int nPlayer);          // the player's total for the round
int  fn_800E17AC(int nPlayer);          // the player's total strokes
int  fn_800E1904(int nPlayer, u8 bCurrent);
u8   fn_800E1BBC(void);                 // whether the round plays every hole
u8   fn_800E1CA8(void);                 // no selected hole is left after the current one
int  fn_800E1CE8(int a, int b);         // a course and hole to its marked-hole index, or -1
u8   fn_800E23B0(int nPlayer, int nStrokes);
u8   fn_800E23EC(int nPlayer);
void fn_800E2470(void);
char* fn_800E2680(void);                // the course's folder name ("01_Peb")
char* GameManager_GetHoleName(int nHole);   // "HOLE_01".."HOLE_18"
u8   fn_800E27A8(void);
int  fn_800E27C0(void);
u8   Gimme_Allowed(int nPlayer);
s32  fn_800E292C(void);                 // the first player to play (the mode's choice after nobody)
s32  fn_800E295C(void);                 // the player after that one
void fn_800E299C(void);
void fn_800E2A88(void);
u8   fn_800E2B40(int nPlayer, Ball* pBall);   // out of bounds
void fn_800E2BA4(void);                 // a random hole from the selection
u8   fn_800E2DB4(int nPlayer);
u8   fn_800E2EAC(int nPlayer);          // placing the ball (state 22), or the mode says so
u8   fn_800E39F0(void);
u8   fn_800E3A54(void);                 // modes 6, 7 and 8
void fn_800E3B04(void);

// GameUI.c
void fn_800E3B28(void);
void fn_800E3BEC(void);
void fn_800E3C0C(u8 b);                 // show or hide the HUD on the single screen
void fn_800E3C70(u8 b);
void fn_800E3CD4(u8 b);
void fn_800E3D38(int nPlayer, u8 b);    // show or hide a player's HUD
void fn_800E3D90(void);                 // hide every HUD
u8   fn_800E3DDC(int nPlayer);
void fn_800E3EE0(void);
u8   fn_800E415C(void);
void fn_800E4164(int nMsg, int nPlayer, f32 f);
void fn_800E41C8(void);
void fn_800E41D4(int nPlayer);
void fn_800E4204(void);
u8   fn_800E4254(int nPlayer);          // whether a message or screen still holds the player
u8   fn_800E430C(int nPlayer);
void fn_800E4364(u32 nQueue, int a, int b, int c);    // add an item to a display queue
void fn_800E45C0(void);
u8   fn_800E45CC(void);                 // whether any display timer or flag is still running
u8   fn_800E46B4(void);                 // the display pump; nonzero while anything is showing
u8   fn_800E4BF8(void);
void fn_800E4C20(u8 bHuman);            // the end-of-round screen
void fn_800E4D88(void);
void fn_800E4D94(u8 bHuman);            // the end-of-hole screen

// The display state (GameUI.c's data; GameMessages.c and GameAnalysis.c use some of it). Twelve
// queues of display items, each with its count; the pump shows the newest item of the first
// non-empty queue.
typedef struct UIQueueItem {
    s32  n0;                    // 0x0  three values whose meaning depends on the queue
    s32  n4;                    // 0x4
    s32  n8;                    // 0x8
} UIQueueItem;

#define UI_QUEUE_LEN 10         // items a display queue holds

extern UIQueueItem lbl_80203044[UI_QUEUE_LEN];  // queue 0 (count lbl_802822B4)
extern UIQueueItem lbl_802030BC[UI_QUEUE_LEN];  // queue 1 (lbl_802822B8)
extern UIQueueItem lbl_80202FCC[UI_QUEUE_LEN];  // queue 2 (lbl_802822B0)
extern UIQueueItem lbl_80202F54[UI_QUEUE_LEN];  // queue 3 (lbl_802822AC)
extern UIQueueItem lbl_80202EDC[UI_QUEUE_LEN];  // queue 4 (lbl_802822A8)
extern UIQueueItem lbl_80202E64[UI_QUEUE_LEN];  // queue 5 (lbl_802822A0), no duplicates
extern UIQueueItem lbl_80202DEC[UI_QUEUE_LEN];  // queue 6 (lbl_8028229C)
extern UIQueueItem lbl_80202D74[UI_QUEUE_LEN];  // queue 7 (lbl_80282298)
extern UIQueueItem lbl_80202CFC[UI_QUEUE_LEN];  // queue 8 (lbl_80282294)
extern UIQueueItem lbl_80202C84[UI_QUEUE_LEN];  // queue 9 (lbl_80282290)
extern UIQueueItem lbl_80202C0C[UI_QUEUE_LEN];  // queue 10 (lbl_8028228C)
extern UIQueueItem lbl_80202B94[UI_QUEUE_LEN];  // queue 11 (lbl_80282288)
extern u8          lbl_80202B88[9];             // the menu screens still open (GameMessages.c)
extern u8          lbl_80203138[14];            // the tips already shown (GameMessages.c, GameAnalysis.c)

extern u8  lbl_80282280;
extern u8  lbl_80282281;                    // the end-of-round screen is up
extern u8  lbl_80282282;                    // the end-of-hole screen is up
extern s32 lbl_80282284;
extern s32 lbl_80282288;
extern s32 lbl_8028228C;
extern s32 lbl_80282290;
extern s32 lbl_80282294;
extern s32 lbl_80282298;
extern s32 lbl_8028229C;
extern s32 lbl_802822A0;
extern s32 lbl_802822A4;
extern s32 lbl_802822A8;
extern s32 lbl_802822AC;
extern s32 lbl_802822B0;
extern s32 lbl_802822B4;
extern s32 lbl_802822B8;
extern u8  lbl_802822BC;
extern u8  lbl_802822BD;
extern u8  lbl_802822BE;
extern u8  lbl_802822BF;
extern u8  lbl_802822C0;
extern u8  lbl_802822C1;
extern u8  lbl_802822C2;
extern u8  lbl_802822C3;
extern u8  lbl_802822C4;
extern u32 lbl_802822C8;                    // frame counts when the HUDs last changed
extern u32 lbl_802822CC;
extern u32 lbl_802822D0;
extern u8  lbl_802822D4;
extern u8  lbl_802822D5;
extern u8  lbl_802822D6;
extern u8  lbl_802822D7;                    // the HUD on screen 3 (split screen, player 2)
extern u8  lbl_802822D8;                    // the HUD on screen 2 (split screen, player 1)
extern u8  lbl_802822D9;                    // the HUD on the single screen
extern u8  lbl_802822DA;
extern u8  lbl_802822DB;
extern u8  lbl_802822DC[3];
extern u8  lbl_802822DF;

// GameMessages.c
void fn_800E4FFC(int a);
void fn_800E502C(int a);
void fn_800E505C(int a);
u8   fn_800E5098(void);
u8   fn_800E5110(void);
void fn_800E5228(void);
u8   fn_800E5344(void);
u8   fn_800E53B8(void);
void fn_800E53F0(int nMsg, int a, int b, int c);      // send a message with three ints
void fn_800E542C(void);
void fn_800E5450(void);
void fn_800E5474(int a);
void fn_800E54A0(int a, int b, int c);
void fn_800E54D8(int a, int b, int c);
void fn_800E5510(int a, int b, int c);
void fn_800E5548(int a, int b, int c);
void fn_800E5580(int a, int b, int c);
void fn_800E55B8(int a, int b, int c);
void fn_800E55F0(int a, int b, int c);
void fn_800E5628(int a, int b, int c);
void fn_800E5660(int a, int b, int c);
void fn_800E5698(int a, int b, int c);
void fn_800E56D0(int a, int b, int c);
void fn_800E5714(int a);
void fn_800E5724(int a);
void fn_800E58B4(int nMsg);             // send a message with no values
void fn_800E590C(int nMsg, u32 uFloats, void* pA);    // one value; uFloats bit 0: a float
void fn_800E5998(int nMsg, u32 uFloats, void* pA, void* pB);
void fn_800E5A4C(int nMsg, u32 uFloats, void* pA, void* pB, void* pC);   // three values; uFloats bit n: a float
void fn_800E5B0C(int nMsg, u32 uFloats, void* pA, void* pB, void* pC, void* pD, void* pE);   // five values
void fn_800E5C08(int nMsg, char* pStr);  // send a message with a string
u8   fn_800E5C84(void);
void fn_800E5CA4(int a, int b, int c, int d, int e, int g, int h, f32 f);    // message 0x42
void fn_800E5D40(int n);                // message 89 with a value
void fn_800E5D68(char* pStr);           // message 90 with a string
void fn_800E5DA0(void);

// ---- the game modes --------------------------------------------------------------------------

// Each mode's setup, called by fn_800E0B38.
void GameModeAlternateShot_Init(void);                 // mode 21 (GameModeAlternateShot.c)
void GameModeBattle_Init(void);                 // mode 25 (GameModeBattle.c)
void fn_800E81C4(void);                 // mode 19 (GameModeBestBall.c)
void GameModeFourBall_Init(void);                 // mode 20 (GameModeFourBall.c)
void GameModeMatch_Init(void);                 // mode 1 (GameModeMatch.c)
void fn_800EACD8(void);                 // mode 5
void fn_800ED738(void);                 // mode 9
void fn_800F0448(void);                 // mode 24
void GameModeReplay_Init(void);                 // mode 10
void fn_800F2984(void);                 // mode 14
void fn_800F39F4(void);                 // mode 15
void fn_800F4B40(void);                 // mode 16
void fn_800F5AAC(void);                 // mode 17
void fn_800F6A60(void);                 // mode 13
void fn_800F80FC(void);                 // mode 2
void fn_800F944C(void);                 // mode 6
void fn_800F9610(void);                 // mode 7
void fn_800F986C(void);                 // mode 8
void GameModeStableford_Init(void);                 // mode 18 (GameModeStableford.c)
void fn_800FEAFC(void);                 // mode 12 (GameMode12.c)
void GameModeStroke_Init(void);                 // mode 0 (GameModeStroke.c)
void fn_800FFF34(void);                 // mode 11 (GameMode11.c)
void fn_80101FEC(void);                 // mode 4 (GameMode4.c)

int  fn_800E8C24(int nPlayer, int nHole);   // GameModeBestBall.c

// GameModeMatch.c: match play, which the other two-player modes build on
extern u8  lbl_80282240;                // the hole-finished test excuses the holed side's own players
void fn_800E9F14(void);
s32  GameModeMatch_GetHonors(int nPlayer);
u8   GameModeMatch_HoleFinished(int nPlayer, u8 bCheck);
u8   GameModeMatch_GameFinished(u8 bCheck);            // the game is over
u8   GameModeMatch_GoToPlayoff(u8 bCheck);
void GameModeMatch_EndHole(void);
s32  fn_800EAC7C(void);
int  fn_800EAC94(int n);

// GameMode5.c
void fn_800EADD8(void);
void fn_800EAE38(s32 a);
void fn_800EAF7C(void);
u8   fn_800EC550(void);
int  fn_800EC558(void);
void fn_800ECBE4(void);
u8   fn_800ED540(void);
void fn_800ED548(void);
void fn_800ED6E8(u8 v);
u8   fn_800ED6F0(void);
void fn_800ED6F8(f32 x);
void fn_800ED710(s32 a);

void fn_800EDAE0(int nPlayer);          // GameMode9.c
void fn_800EDE78(void);                 // GameModeDriverPGATour.c
u8   fn_800EE470(void);                 // GameModeDriverPGATour.c
void GameModeDriverRTE_StartEvent(void);                 // GameModeDriverRTE.c
void fn_800F07C8(void);                 // GameModeDriverRTE.c
u8   fn_800F0818(void);                 // GameModeDriverRTE.c

// GameModeDriver.c: the career calendar. Its functions come in tables of three, indexed by
// CareerCalendar.nDriver: no career, the PGA TOUR season (GameModeDriverPGATour.c), the real-time
// events (GameModeDriverRTE.c). The calendar shows one month as a grid of 35 day cells.
typedef struct CareerCalendar {
    u16  nToday;                // 0x00  the career's current day (fn_80117188)
    u16  nSelected;             // 0x02  the day the cursor is on
    s32  nMonth;                // 0x04  the month shown, 1..12
    u32  nYear;                 // 0x08  and its year
    u32  nFirstCell;            // 0x0C  the cell of the month's first day
    u32  nEndCell;              // 0x10  the cell after its last day
    u32  nPrevMonthDays;        // 0x14  the days in the month before
    s32  nDriver;               // 0x18  0 none, 1 PGA TOUR, 2 real-time events
    s32  n1C;                   // 0x1C  which panel the day's details show (PGA TOUR 0..3, RTE 4..6)
    u8   bSeasonOver;           // 0x20  the PGA TOUR season has no event left
    u8   unk21[0x28 - 0x21];
} CareerCalendar;
LAYOUT_ASSERT(CareerCalendar, 0x28);

extern CareerCalendar lbl_80223C48;
extern char* (*lbl_80193EDC[3])(u16 nDate);    // the name of the event on a day ("" none)
extern u16 (*lbl_80193EC4[3])(void);           // the career's current day
// The calendar screen's (CalendarScreen.c) per-driver tables.
extern u8 (*lbl_80193E70[3])(void);             // the month before; 0: the calendar may move there
extern u8 (*lbl_80193E7C[3])(void);             // the month after
extern s32 (*lbl_80193E88[3])(char* sz, u16 nDate, s32* pLook, s32* pButton);  // a day cell
extern void (*lbl_80193E94[3])(int nLine, char* sz);
extern void (*lbl_80193EA0[3])(u16 nDate, int n, char* sz);
extern s32 (*lbl_80193EAC[3])(u16 nDate);       // the day-details panel for a day
extern void (*lbl_80193EB8[3])(int nKind, char* szTitle, char* szText);
extern void (*lbl_80193ED0[3])(void);           // set the driver up
extern void* (*lbl_80193EE8[3])(u16 nDate);     // the event on a day (NULL: none)
extern void (*lbl_80193EF4[3])(void);
extern u8 (*lbl_80193F00[3])(void);

// GameModeDriver.c
void fn_80117188(void);
void fn_80117348(void);
u16  fn_801173F0(u32 nCell);            // the date in a grid cell
s32  fn_80117510(u16 nDate);

// GameTargets.c: what the target games (modes 13..17) share
extern f32 lbl_80211D38[40][4];         // the target list: lbl_80282360 points (w = 1)
extern s8  lbl_80282360;                // the number of targets (GameModeReplay.c)
void fn_800F19D4(void);                 // sort the targets by distance from the tee
void fn_800F1ABC(int nPlayer, s8 n);
void fn_800F1B60(int nPlayer, s8 n);
u8   fn_800F1BD8(int nPlayer);          // previous target
u8   fn_800F1C34(int nPlayer);          // next target
s8   fn_800F1C74(int nPlayer);          // the target nearest the ball
int  fn_800F1D34(int nPlayer);          // the target nearest the player's aim point
s32  fn_800F1E58(s32 n);
void fn_800F1EE4(void);                 // every player's target-game state cleared
void fn_800F2030(void);
s32  fn_800F20C0(int nPlayer);          // how many targets the player has hit
void fn_800F21B4(int nPlayer);          // after a shot, maybe a multiplier for the next one
s32  fn_800F2578(void);                 // the target game's prize
void fn_800F263C(s32 nMsg);
void fn_800F2664(int nPlayer);
void fn_800F2668(int nPlayer);
s32  fn_800F266C(s32 n, int i);         // scale n by table entry i
u8   fn_800F2788(int nPlayer, f32 f);   // whether f is far enough for the player's tees
s32  fn_800F2810(s32 n);
void fn_800F2958(s32 nMsg, s32 a);
// The modes' own getters behind the dispatchers fn_800F2408..fn_800F2534, which pass their
// argument on; the getters ignore it.
int  fn_800F354C(int nPlayer);          // GameMode14.c
s32  fn_800F37F8(s32 a);                // GameMode14.c
s32  fn_800F59CC(s32 a);                // GameMode16.c
s32  fn_800F59D4(s32 a);                // GameMode16.c
s32  fn_800F6A00(s32 a);                // GameMode17.c
s32  fn_800F6A34(s32 a);                // GameMode17.c
s32  fn_800F7D94(s32 a);                // GameMode13.c
s32  fn_800F7D9C(s32 a);                // GameMode13.c
s32  fn_800F80A0(s32 a);                // GameMode13.c

void fn_800F3828(void);                 // GameMode14.c
void fn_800F3980(int nMsg, s32 a, s32 b, s32 c, s32 d, s32 e);    // GameMode14.c
void fn_800F39CC(s32 a);                // GameMode14.c
void fn_800F48C4(void);                 // GameMode15.c
void fn_800F7DE8(void);                 // GameMode13.c
void fn_800F80D4(s32 a);                // GameMode13.c
s32  fn_800F9254(void);                 // GameMode2.c: the skin on this hole
s32  fn_800F9328(void);                 // GameMode2.c: the first selected hole (-1: none)
s32  fn_800F93D8(int h);                // the next selected hole after h (-1: none)
s32  fn_800F9414(int h);                // the selected hole before h (-1: none)

// GameMode7.c
void fn_800F9824(void);
void fn_800F9844(void);

// GameMode8.c (modes 6, 7 and 8 share it)
extern u8  lbl_802823C9;
extern s32 lbl_802823D0;                // the next entry of the event log lbl_802120F8 (0..99)

// One entry of the event log: the last 100 events (GameMode8.c writes it, a menu command reads it).
typedef struct SGLog {
    s32 nEvent;
    s32 nPlayer;
} SGLog;
extern SGLog lbl_802120F8[100];
extern s32 lbl_802823D4;
void fn_800F9A58(void);
void fn_800F9AB0(void);
void fn_800F9B34(void);
s32  fn_800F9BF8(int nPlayer);
u8   fn_800F9C00(int nPlayer, u8 bCheck);
void fn_800F9C48(void);
u8   fn_800F9D00(u8 bCheck);
void fn_800F9E00(void);
u8   fn_800F9F04(u8 bCheck);
u8   fn_800FA118(int nPlayer, u8 bCheck);
u8   fn_800FA148(u8 bCheck);
u8   fn_800FA2C8(u8 bCheck);
void fn_800FA3AC(void);
void fn_800FA410(void);
void fn_800FA570(void);
void fn_800FA998(int nPlayer);
void fn_800FDA30(int nPlayer, int a);
void fn_800FDF38(void);
u8   fn_800FDF58(int nPlayer);
u8   fn_800FDF60(void);

// GameModeStroke.c: stroke play (mode 0)
void GameModeStroke_SetupNextGolfer(void);
s32  GameModeStroke_GetHonors(int nPlayer);          // TW06 GetHonors: who plays next (5: nobody)
u8   GameModeStroke_HoleFinished(int nPlayer, u8 bCheck);   // TW06 HoleFinished
u8   GameModeStroke_GameFinished(u8 bCheck);                // TW06 GameFinished
u8   fn_800FFDB0(u8 bCheck);            // TW06 GoToPlayoff: stroke play has none

// GetHonors' tee-order sort (GameModeStroke.c, GameModeStableford.c): appends nPlayer to aList
// (*pnCount entries) if their score on hole nHole is nScore.
static inline void AddIfScore(s32* aList, int* pnCount, int nPlayer, int nHole, s32 nScore) {
    if (gPlayers[nPlayer].nStrokes[nHole] == nScore) {
        aList[*pnCount] = nPlayer;
        (*pnCount)++;
    }
}

// GameMode11.c: the lessons
extern s32 lbl_802823FC;                // the current lesson, 1..12
u8   fn_80100294(void);                 // in a lesson (mode 11)
int  fn_8010069C(int nPlayer); // the lesson's shape in mode 11, else 7 (none); nPlayer unused
u8   fn_80100AF8(void);                 // lesson 5 of mode 11
u8   fn_80100C00(void);
u8   fn_80101738(void);
u8   fn_80101AA8(int nPlayer, int nEvent);  // an event (event.c's numbers) in a lesson; nonzero blocks it
u8   fn_80101D4C(int nPlayer);          // a CPU in game mode 11 is always lucky
u8   fn_80101E34(char* szName);         // one of the lessons' demonstration animations
void fn_80101EDC(void);

u8   fn_801025F4(void);

// DiscCheck.c: which disc is in the drive, and changing discs
extern char lbl_80213BB0[256][0x80];    // } the file names listed for each disc (fn_8010FF9C reads
extern char lbl_8021BBB0[256][0x80];    // } them from "data/fend/d_layout.bin")
extern s32 lbl_802824C8;                // } how many names lbl_8021BBB0 holds
extern s32 lbl_802824CC;                // } and lbl_80213BB0
extern s32 lbl_802824D4;                // the disc in the drive: 0 looks names up in lbl_8021BBB0,
                                        // else in lbl_80213BB0
extern u8 lbl_802824D0;                 // the disc change has finished (set by its callback)
extern u8 lbl_802824D1;                 // fn_80110458 sets it, fn_80110460 answers it
extern u8 lbl_802824D2;                 // the lists have been read (fn_8010FF9C)
extern u8 lbl_802824D9;                 // fn_80110180 also looks for the hole's file
void fn_8010FF9C(void);                 // read the lists (at startup, gomainloop.c)
u8   fn_801100AC(char* szName);         // the file is on the disc in the drive
void fn_80110178(u8 b);
int  fn_80110180(void);                 // the current hole's course is on the disc in the drive
void fn_8006DBD4(void);                 // gomainloop.c: the frame of the current game type
s32  fn_8011027C(void);                 // the disc in the drive is not disc 1 (its disk number is not 0)
void fn_801102AC(void);                 // ask for the other disc and wait for it
int  fn_80110450(void);                 // the disc change has finished (lbl_802824D0)
void fn_80110458(u8 b);
u8   fn_80110460(void);
int  fn_80110468(void);                 // the disc in the drive (its disk number; 0: disc 1)
DVDCommandBlock* fn_801104A0(void);     // the disc change's command block
extern DVDDiskID lbl_80213B60;          // the disc asked for by fn_80110390
extern DVDCommandBlock lbl_80213B80;    // fn_80110390's disc change
extern u8 lbl_802824D8;                 // fn_801102AC left a disc change for fn_80110390 to ask for

#endif
