// game.h (our name): the round and the game modes: the current hole, the golfer states, and the
// game manager and game-mode code at 0x800D0000..0x80100000 that more than one file calls.
// Players and golfers are in golfer.h, the ball in ball.h.

#ifndef GAME_H
#define GAME_H

#include "ball.h"

// ---- the round -------------------------------------------------------------------------------

int  Game_GetCourse(void);              // 0x80008830
int  Game_CurHoleIndex(void);           // 0..17 in the round (Golfer.c)
int  Game_CurrentHole(void);            // the index mapped through the hole order (Golfer.c)
void Session_SetNumPlayers(int n);      // Golfer.c

// ---- golfer states (Swing.c) -----------------------------------------------------------------

f32  fn_8005B64C(int nPlayer);          // the swing's shot power
void GOLFERSTATE_Push(int nState, int nPlayer);     // push a state and run its enter callback
void GOLFERSTATE_Set(int nState, int nPlayer);      // pop everything and start again from one state
void GOLFERSTATE_Switch(int nState, int nPlayer);   // replace the current state
int  GOLFERSTATE_GetCurrentState(int nPlayer);      // GS_*, or -1
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
void fn_80062CE0(u8 a);
void fn_80062D0C(int nPlayer);
void fn_80062D38(int a, int b, int nPlayer);
void fn_80062D6C(int a, int nPlayer);

// ---- the game manager ------------------------------------------------------------------------

f32  fn_800D0478(int nPlayer);          // the ball's distance from the pin (yards)
f32  fn_800D0550(int nPlayer);          // the shot's length
int  Hole_ScoreAfterTapIn(int nPlayer); // HoleScore.c
void fn_800D2714(u16* pDate, s32* pDay, s32* pMonth, s32* pYear);
int  fn_800D2ABC(int nCourse, int nHole);   // a hole's par on a course
int  fn_800D2AD8(int nHole);            // a hole's par
void fn_800D3548(int nPlayer, int nMoney, s32* pTotals);   // pTotals may be NULL
int  fn_800D36E0(int nWinner, int nLoser, int nMargin, int* pPrize);
int  fn_800D37BC(int nWinner, int nLoser, int nMargin, int* pPrize);
s32  fn_800D6A70(s32 nPoints, int nPlayer, int a, int b, int c, int d);
int  fn_800D7220(int nReward, int a, s32* pOut);
u8   fn_800DA174(void);
u8   fn_800DA1D4(void);
u8   fn_800DA234(void);                 // the current hole is the flagged one

// GameEffects.c
void GameEffects_ResetGameEffectSettings(void);
int  GameEffects_BallUpdatesThisFrame(int nPlayer);   // preview speed: ghost steps per frame
void fn_800DB4E8(int nPlayer);
void fn_800DB714(int nPlayer);
void fn_800DC9D4(int a);                // pause or resume a GameBreaker

// GameManager.c
void fn_800DCAD8(void);
void GM_vCloseModuleONCE(void);
u8   fn_800DCB3C(void);
int  GM_GotoNextSelectedHole(void);
void GM_EndOfGolferTurn(int nPlayer);
void GM_BallHit(int nPlayer);
void GM_PlayerAddStroke(int nPlayer);
u8   GM_CheckForBallOOB(int nPlayer);
void GM_PlayerTookShot(int nPlayer);
u8   GM_PlayerTakeMulligan(int nPlayer);
int  fn_800DDFB4(int nPlayer);
int  GM_ShowPostShotAnimation(int nPlayer);
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
int  fn_800E17AC(int nPlayer);          // the player's total strokes
u8   fn_800E1BBC(void);                 // whether the round plays every hole
u8   fn_800E23B0(int nPlayer, int nStrokes);
void fn_800E2470(void);
u8   Gimme_Allowed(int nPlayer);
void fn_800E299C(void);
void fn_800E2A88(void);
u8   fn_800E2B40(int nPlayer, Ball* pBall);   // out of bounds
void fn_800E2BA4(void);                 // a random hole from the selection
u8   fn_800E2DB4(int nPlayer);
u8   fn_800E39F0(void);
void fn_800E3B04(void);

// GameUI.c
void fn_800E3B28(void);
void fn_800E3BEC(void);
void fn_800E3C0C(u8 b);                 // show or hide the HUD on the single screen
void fn_800E3D90(void);                 // hide every HUD
u8   fn_800E3DDC(int nPlayer);
void fn_800E3EE0(void);
u8   fn_800E415C(void);
void fn_800E4164(int nMsg, int nPlayer, f32 f);
void fn_800E41C8(void);
void fn_800E41D4(int nPlayer);
void fn_800E4204(void);
u8   fn_800E4254(int nPlayer);          // whether a message or screen still holds the player
void fn_800E4364(u32 nQueue, int a, int b, int c);    // add an item to a display queue
void fn_800E45C0(void);
u8   fn_800E4BF8(void);
void fn_800E4C20(u8 bHuman);            // the end-of-round screen
void fn_800E4D88(void);
void fn_800E4D94(u8 bHuman);            // the end-of-hole screen

// GameMessages.c
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

int  fn_800E8C24(int nPlayer, int nHole);   // GameModeBestBall.c

// GameModeMatch.c: match play, which the other two-player modes build on
void fn_800E9F14(void);
s32  fn_800EA084(int nPlayer);
u8   fn_800EA278(int nPlayer, u8 bCheck);
u8   fn_800EA548(u8 bCheck);            // the game is over
u8   fn_800EA758(u8 bCheck);
void fn_800EAA40(void);
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
void fn_800EDE78(void);                 // GameMode23.c
u8   fn_800EE470(void);                 // GameMode23.c
void fn_800F06DC(void);                 // GameMode24.c
void fn_800F07C8(void);                 // GameMode24.c
u8   fn_800F0818(void);                 // GameMode24.c

// GameTargets.c: what the target games (modes 13..17) share
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

// GameMode7.c
void fn_800F9824(void);
void fn_800F9844(void);

// GameMode8.c
void fn_800F9A58(void);
void fn_800F9AB0(void);
void fn_800F9B34(void);
s32  fn_800F9BF8(int nPlayer);
u8   fn_800F9C00(int nPlayer, int a);
void fn_800F9C48(void);
u8   fn_800F9D00(u8 bCheck);
void fn_800F9E00(void);
u8   fn_800F9F04(u8 bCheck);
u8   fn_800FA118(int nPlayer, int a);
u8   fn_800FA148(int a);
s32  fn_800FA2C8(void);
void fn_800FA3AC(void);
void fn_800FA410(void);
void fn_800FA570(void);
void fn_800FA998(int nPlayer);
void fn_800FDA30(int nPlayer, int a);
void fn_800FDF38(void);
u8   fn_800FDF58(int nPlayer);
u8   fn_800FDF60(void);

// after the Stableford code (0x800FF634..)
void fn_800FF7DC(void);
u8   fn_80100294(void);                 // in a lesson (mode 11)
u8   fn_801025F4(void);

#endif
