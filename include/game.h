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

extern Replay gReplayData;              // 0x801D6030

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
void fn_80062D38(int a, int b, int nPlayer);
void fn_80062D6C(int a, int nPlayer);

// ---- the game manager ------------------------------------------------------------------------

f32  fn_800D0478(int nPlayer);          // the ball's distance from the pin (yards)
f32  fn_800D0550(int nPlayer);          // the shot's length
int  Hole_ScoreAfterTapIn(int nPlayer); // HoleScore.c
void fn_800D2714(u16* pDate, s32* pMonth, s32* pDay, s32* pYear);
u16  fn_800D2994(void);                 // today's date
int  fn_800D2ABC(int nCourse, int nHole);   // a hole's par on a course
int  fn_800D2AD8(int nHole);            // a hole's par
void fn_800D3548(int nPlayer, int nMoney, CourseMoneyTracking* pMoney);   // pMoney may be NULL
int  fn_800D36E0(int nWinner, int nLoser, int nMargin, int* pPrize);
int  fn_800D37BC(int nWinner, int nLoser, int nMargin, int* pPrize);
int  fn_800D3C7C(int nPlayer);          // the player's earnings rating, 0..25
s32  fn_800D6A70(s32 nPoints, int nPlayer, u8 bCourse, u8 bTee, u8 bHole, CourseMoneyTracking* pMoney);
int  fn_800D7220(int nReward, int nPlayer, CourseMoneyTracking* pMoney);
s32  fn_800D7660(int nPlayer, Ball* pBall, u8 b);   // one of GameEffects' GameBreaker checks
u8   fn_800DA174(void);
u8   fn_800DA1D4(void);
u8   fn_800DA234(void);                 // the current hole is the flagged one

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

extern GameEffects lbl_80202898;        // 0x80202898

GameEffects* fn_800DAF74(void);
void GameEffects_ResetGameEffectSettings(void);
int  GameEffects_BallUpdatesThisFrame(int nPlayer);   // preview speed: ghost steps per frame
void fn_800DB4E8(int nPlayer);
void fn_800DB714(int nPlayer);
void fn_800DBDA8(int nPlayer);
void GameEffects_SetSuperSlowMo(u8 bOn, int nPlayer, f32 fRate);
u8   fn_800DC514(int nPlayer);          // super slow motion is on (nPlayer unused)
void fn_800DC9D4(int a);                // pause or resume a GameBreaker

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
int  fn_800E177C(void);
int  fn_800E1788(int nPlayer);          // the player's total for the round
int  fn_800E17AC(int nPlayer);          // the player's total strokes
int  fn_800E1904(int nPlayer, u8 bCurrent);
u8   fn_800E1BBC(void);                 // whether the round plays every hole
u8   fn_800E1CA8(void);                 // no selected hole is left after the current one
u8   fn_800E23B0(int nPlayer, int nStrokes);
u8   fn_800E23EC(int nPlayer);
void fn_800E2470(void);
u8   fn_800E27A8(void);
int  fn_800E27C0(void);
u8   Gimme_Allowed(int nPlayer);
void fn_800E299C(void);
void fn_800E2A88(void);
u8   fn_800E2B40(int nPlayer, Ball* pBall);   // out of bounds
void fn_800E2BA4(void);                 // a random hole from the selection
u8   fn_800E2DB4(int nPlayer);
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
void fn_800E5DA0(void);

// ---- the game modes --------------------------------------------------------------------------

// Each mode's setup, called by fn_800E0B38.
void fn_800E68F0(void);                 // mode 21 (GameModeAlternateShot.c)
void fn_800E7980(void);                 // mode 25 (GameModeBattle.c)
void fn_800E81C4(void);                 // mode 19 (GameModeBestBall.c)
void fn_800E8D58(void);                 // mode 20 (GameModeFourBall.c)
void fn_800E9E40(void);                 // mode 1 (GameModeMatch.c)
void fn_800EACD8(void);                 // mode 5
void fn_800ED738(void);                 // mode 9
void fn_800F0448(void);                 // mode 24
void fn_800F125C(void);                 // mode 10
void fn_800F2984(void);                 // mode 14
void fn_800F39F4(void);                 // mode 15
void fn_800F4B40(void);                 // mode 16
void fn_800F5AAC(void);                 // mode 17
void fn_800F6A60(void);                 // mode 13
void fn_800F80FC(void);                 // mode 2
void fn_800F944C(void);                 // mode 6
void fn_800F9610(void);                 // mode 7
void fn_800F986C(void);                 // mode 8
void fn_800FE1B4(void);                 // mode 18 (GameModeStableford.c)
void fn_800FEAFC(void);                 // mode 12 (GameMode12.c)
void fn_800FF700(void);                 // mode 0 (GameModeStroke.c)
void fn_800FFF34(void);                 // mode 11 (GameMode11.c)
void fn_80101FEC(void);                 // mode 4 (GameMode4.c)

int  fn_800E8C24(int nPlayer, int nHole);   // GameModeBestBall.c

// GameModeMatch.c: match play, which the other two-player modes build on
extern u8  lbl_80282240;                // the hole-finished test excuses the holed side's own players
void fn_800E9F14(void);
s32  fn_800EA084(int nPlayer);
u8   fn_800EA278(int nPlayer, u8 bCheck);
u8   fn_800EA548(u8 bCheck);            // the game is over
u8   fn_800EA758(u8 bCheck);
void fn_800EAA40(void);
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
void fn_800ED6F8(f32 x);
void fn_800ED710(s32 a);

void fn_800EDAE0(int nPlayer);          // GameMode9.c
void fn_800EDE78(void);                 // GameModeDriverPGATour.c
u8   fn_800EE470(void);                 // GameModeDriverPGATour.c
void fn_800F06DC(void);                 // GameModeDriverRTE.c
void fn_800F07C8(void);                 // GameModeDriverRTE.c
u8   fn_800F0818(void);                 // GameModeDriverRTE.c

// GameTargets.c: what the target games (modes 13..17) share
extern f32 lbl_80211D38[40][4];         // the target list: lbl_80282360 points (w = 1)
extern s8  lbl_80282360;                // the number of targets (GameModeReplay.c)
void fn_800F19D4(void);                 // sort the targets by distance from the tee
void fn_800F1ABC(int nPlayer, s8 n);
void fn_800F1B60(int nPlayer, s8 n);
u8   fn_800F1BD8(int nPlayer);          // previous target
u8   fn_800F1C34(int nPlayer);          // next target
s8   fn_800F1C74(int nPlayer);          // the target nearest the ball
int  fn_800F1D34(int nPlayer);          // the player's current target
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
s32  fn_800F9328(void);                 // GameMode2.c: the first selected hole (-1: none)
s32  fn_800F93D8(int h);                // the next selected hole after h (-1: none)
s32  fn_800F9414(int h);                // the selected hole before h (-1: none)

// GameMode7.c
void fn_800F9824(void);
void fn_800F9844(void);

// GameMode8.c (modes 6, 7 and 8 share it)
extern u8  lbl_802823C9;
extern s32 lbl_802823D0;                // the next entry of the event log lbl_802120F8 (0..99)
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
void fn_800FF7DC(void);
s32  fn_800FF894(int nPlayer);          // TW06 GetHonors: who plays next (5: nobody)
u8   fn_800FFCCC(int nPlayer, u8 bCheck);   // TW06 HoleFinished
u8   fn_800FFD54(u8 bCheck);                // TW06 GameFinished
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
int  Scenario_RequiredShape(int nPlayer); // the lesson's shape in mode 11, else 7 (none); nPlayer unused
u8   fn_80100AF8(void);                 // lesson 5 of mode 11
u8   fn_80100C00(void);
u8   fn_80101738(void);
u8   fn_80101D4C(int nPlayer);          // a CPU in game mode 11 is always lucky
u8   fn_80101E34(char* szName);         // one of the lessons' demonstration animations
void fn_80101EDC(void);

u8   fn_801025F4(void);

#endif
